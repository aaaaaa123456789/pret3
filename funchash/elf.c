#include "proto.h"

struct ELF * load_ELF_file (const char * file, char ** error) {
  *error = NULL;
  struct ELF * result = calloc(1, sizeof *result);
  if (!(result -> data = read_file_as_bytes(file, &(result -> size)))) {
    *error = generate_string("could not read file %s", file);
    goto fail;
  }
  if (*error = parse_ELF_header(result)) goto fail;
  if (*error = load_ELF_symbols(result)) goto fail;
  if (*error = sort_ELF_symbols(result)) goto fail;
  if (*error = load_ELF_relocations(result)) goto fail;
  return result;
  fail:
  destroy_ELF_file(result);
  return NULL;
}

void destroy_ELF_file (struct ELF * file) {
  free(file -> relocations);
  free(file -> sorted_functions);
  free(file -> sorted_symbols);
  free(file -> symbols);
  free(file -> sections);
  free(file -> data);
  free(file);
}

char * parse_ELF_header (struct ELF * file) {
  const unsigned char valid_ELF_header[16] = {127, 69, 76, 70, 1, 1, 1};
  if (file -> size < 52) return duplicate_string("invalid ELF header");
  if (memcmp(file -> data, valid_ELF_header, sizeof valid_ELF_header))
    return duplicate_string("invalid ELF header or not a 32-bit little endian file");
  if (file -> data[17]) return duplicate_string("unknown ELF type");
  file -> type = file -> data[16];
  if ((file -> type != ELF_FILE_RELOCATABLE) && (file -> type != ELF_FILE_EXECUTABLE))
    return duplicate_string("ELF must be an executable or relocatable file");
  if (convert_buffer_to_number(file -> data + 18, 2) != 40)
    return duplicate_string("ELF file must be built for an ARM32 machine");
  if (convert_buffer_to_number(file -> data + 20, 2) != 1)
    return duplicate_string("unknown ELF version");
  unsigned section_header_pointer, section_size, section_count;
  section_header_pointer = convert_buffer_to_number(file -> data + 32, 4);
  section_size = convert_buffer_to_number(file -> data + 46, 2);
  section_count = convert_buffer_to_number(file -> data + 48, 2);
  if (!(section_header_pointer && section_size && section_count))
    return duplicate_string("ELF file does not contain a section table");
  return read_ELF_section_table(file, section_header_pointer, section_count, section_size);
}

char * read_ELF_section_table (struct ELF * file, unsigned position, unsigned count, unsigned entry_size) {
  if (file -> size < (position + count * entry_size)) return duplicate_string("unexpected end of file");
  file -> sections = calloc(count, sizeof(struct ELF_section));
  unsigned index, offset = position;
  for (index = 0; index < count; index ++, offset += entry_size) {
    file -> sections[index] = (struct ELF_section) {
      .type = convert_buffer_to_number(file -> data + offset + 4, 4),
      .load_address = convert_buffer_to_number(file -> data + offset + 12, 4),
      .position = convert_buffer_to_number(file -> data + offset + 16, 4),
      .size = convert_buffer_to_number(file -> data + offset + 20, 4),
      .link = convert_buffer_to_number(file -> data + offset + 24, 4),
      .info = convert_buffer_to_number(file -> data + offset + 28, 4),
      .entry_size = convert_buffer_to_number(file -> data + offset + 36, 4)
    };
    if ((file -> sections[index].type == ELF_SECTION_NOBITS) && (file -> sections[index].position <= file -> size)) continue;
    if (file -> sections[index].position >= file -> size)
      return generate_string("section #%u is located beyond the end of the file", index);
    if ((file -> size - file -> sections[index].position) < file -> sections[index].size)
      return generate_string("section #%u ends beyond the end of the file", index);
    if (file -> sections[index].type == ELF_SECTION_STRTAB && (file -> data[file -> sections[index].position] ||
                                                               file -> data[file -> sections[index].position + file -> sections[index].size - 1]))
      return generate_string("string table section #%u does not begin and end with a null character", index);
  }
  file -> section_count = count;
  return NULL;
}

char * load_ELF_symbols (struct ELF * file) {
  unsigned section, string_table, count, pos, offset;
  for (section = 0; section < file -> section_count; section ++) if (file -> sections[section].type == ELF_SECTION_SYMTAB) {
    string_table = file -> sections[section].link;
    if ((string_table >= file -> section_count) || (file -> sections[string_table].type != ELF_SECTION_STRTAB))
      return generate_string("symbol table section #%u does not point to a valid string table", section);
    if (file -> sections[section].entry_size < 16)
      return generate_string("symbol table section #%u declares an entry size too small to contain a symbol declaration", section);
    file -> sections[section].item_offset = file -> symbol_count;
    count = file -> sections[section].size / file -> sections[section].entry_size;
    void * new_table = realloc(file -> symbols, sizeof(struct ELF_symbol) * (file -> symbol_count + count));
    if (!new_table) return duplicate_string("out of memory");
    file -> symbols = new_table;
    const unsigned char * symbol_pointer = file -> data + file -> sections[section].position;
    for (pos = 0; pos < count; pos ++, symbol_pointer += file -> sections[section].entry_size) {
      struct ELF_symbol * current_symbol = file -> symbols + file -> symbol_count + pos;
      *current_symbol = (struct ELF_symbol) {
        .value = convert_buffer_to_number(symbol_pointer + 4, 4),
        .size = convert_buffer_to_number(symbol_pointer + 8, 4),
        .section = convert_buffer_to_number(symbol_pointer + 14, 2),
        .type = symbol_pointer[12] & 15,
        .visibility = symbol_pointer[12] >> 4
      };
      offset = convert_buffer_to_number(symbol_pointer, 4);
      if (offset) {
        if (offset >= file -> sections[string_table].size)
          return generate_string("symbol #%u points to a name beyond the end of the string table", pos + file -> symbol_count);
        current_symbol -> name = file -> data + file -> sections[string_table].position + offset;
      } else
        current_symbol -> name = "";
      if ((current_symbol -> section == ELF_SYMBOL_SECTION_ABSOLUTE) || (current_symbol -> section == ELF_SYMBOL_SECTION_COMMON)) {
        if (current_symbol -> type == ELF_SYMBOL_FUNCTION)
          return generate_string("symbol #%u (function %s) is defined as absolute or common", pos + file -> symbol_count, current_symbol -> name);
        continue;
      }
      if (current_symbol -> section >= file -> section_count)
        return generate_string("symbol #%u points to non-existent section #%hu", pos + file -> symbol_count, current_symbol -> section);
      if (current_symbol -> type != ELF_SYMBOL_FUNCTION) continue;
      if (!current_symbol -> section) continue; // unknown (referenced) functions point to the undefined section
      if (file -> sections[current_symbol -> section].type != ELF_SECTION_PROGBITS)
        return generate_string("symbol #%u (function %s) points to section #%hu, which does not contain output data",
                               pos + file -> symbol_count, current_symbol -> name, current_symbol -> section);
    }
    file -> symbol_count += count;
  }
  return NULL;
}

char * sort_ELF_symbols (struct ELF * file) {
  file -> sorted_symbols = calloc(file -> symbol_count + 1, sizeof(struct ELF_symbol *));
  if (!file -> sorted_symbols) return duplicate_string("out of memory");
  unsigned pos, count;
  for (pos = 0; pos < file -> symbol_count; pos ++) file -> sorted_symbols[pos] = file -> symbols + pos;
  file -> sorted_symbols[file -> symbol_count] = NULL;
  qsort(file -> sorted_symbols, file -> symbol_count, sizeof *(file -> sorted_symbols), &compare_ELF_symbols);
  count = 0;
  file -> sorted_functions = NULL;
  void * new_table;
  for (pos = 0; pos < file -> symbol_count; pos ++) {
    const struct ELF_symbol * symbol = file -> sorted_symbols[pos];
    if (symbol -> type != ELF_SYMBOL_FUNCTION) continue;
    if (!symbol -> section) continue;
    new_table = realloc(file -> sorted_functions, sizeof(struct ELF_symbol *) * (count + 1));
    if (!new_table) return duplicate_string("out of memory");
    file -> sorted_functions = new_table;
    file -> sorted_functions[count ++] = symbol;
  }
  new_table = realloc(file -> sorted_functions, sizeof(struct ELF_symbol *) * (count + 1));
  if (!new_table) return duplicate_string("out of memory");
  file -> sorted_functions = new_table;
  file -> sorted_functions[count] = NULL;
  file -> function_count = count;
  return NULL;
}

int compare_ELF_symbols (const void * s1, const void * s2) {
  const struct ELF_symbol * const * first = s1;
  const struct ELF_symbol * const * second = s2;
  int result = strcmp((**first).name, (**second).name);
  if (!result) result = !(**second).section - !(**first).section;
  if (!result) result = !(**second).visibility - !(**first).visibility;
  if (!result) result = ((**first).visibility > (**second).visibility) - ((**first).visibility < (**second).visibility);
}

char * load_ELF_relocations (struct ELF * file) {
  unsigned pos;
  char * result;
  for (pos = 0; pos < file -> section_count; pos ++) {
    if (file -> sections[pos].type == ELF_SECTION_PROGBITS) file -> sections[pos].item_offset = -1u;
    if ((file -> sections[pos].type != ELF_SECTION_REL) && (file -> sections[pos].type != ELF_SECTION_RELA)) continue;
    if (result = load_ELF_relocations_for_section(file, pos)) return result;
  }
  if (!(file -> relocation_count)) return NULL;
  qsort(file -> relocations, file -> relocation_count, sizeof(struct ELF_relocation), &compare_ELF_relocations);
  file -> sections[file -> relocations -> section].item_offset = 0;
  for (pos = 1; pos < file -> relocation_count; pos ++)
    if (file -> relocations[pos].section != file -> relocations[pos - 1].section)
      file -> sections[file -> relocations[pos].section].item_offset = pos;
  return NULL;
}

char * load_ELF_relocations_for_section (struct ELF * file, unsigned section) {
  unsigned symtable = file -> sections[section].link;
  if ((symtable >= file -> section_count) || (file -> sections[symtable].type != ELF_SECTION_SYMTAB))
    return generate_string("relocation section #%u does not point to a valid symbol table", section);
  unsigned ref = file -> sections[section].info;
  if ((ref >= file -> section_count) || (file -> sections[ref].type != ELF_SECTION_PROGBITS))
    return generate_string("relocation section #%u does not apply to a valid data section", section);
  if (!(file -> sections[section].size)) return NULL;
  if (file -> sections[section].entry_size < ((file -> sections[section].type == ELF_SECTION_RELA) ? 12 : 8))
    return generate_string("relocation section #%u declares an entry size too small to contain a relocation%s", section,
                           (file -> sections[section].type == ELF_SECTION_RELA) ? " with explicit addend" : "");
  unsigned count = file -> sections[section].size / file -> sections[section].entry_size;
  if (!count) return NULL;
  unsigned symbols = file -> sections[symtable].size / file -> sections[symtable].entry_size;
  void * new_table = realloc(file -> relocations, sizeof(struct ELF_relocation) * (count + file -> relocation_count));
  if (!new_table) return duplicate_string("out of memory");
  file -> relocations = new_table;
  const unsigned char * relocation_pointer = file -> data + file -> sections[section].position;
  unsigned pos;
  for (pos = 0; pos < count; pos ++, relocation_pointer += file -> sections[section].entry_size) {
    struct ELF_relocation * relocation = file -> relocations + file -> relocation_count + pos;
    *relocation = (struct ELF_relocation) {
      .offset = convert_buffer_to_number(relocation_pointer, 4),
      .type = relocation_pointer[4],
      .symbol = convert_buffer_to_number(relocation_pointer + 5, 3),
      .section = ref,
      .has_addend = file -> sections[section].type == ELF_SECTION_RELA
    };
    if (relocation -> has_addend) relocation -> addend = convert_buffer_to_number(relocation_pointer + 8, 4);
    if (file -> type != ELF_FILE_RELOCATABLE) relocation -> offset -= file -> sections[ref].load_address;
    if (relocation -> offset >= file -> sections[ref].size)
      return generate_string("relocation %u (in section %u) has offset 0x%08x, past the end of section %u", pos, section, relocation -> offset, ref);
    if (relocation -> symbol >= symbols)
      return generate_string("relocation %u (in section %u) points to invalid symbol %u", pos, section, relocation -> symbol);
    relocation -> symbol += file -> sections[symtable].item_offset;
  }
  file -> relocation_count += count;
  return NULL;
}

int compare_ELF_relocations (const void * r1, const void * r2) {
  const struct ELF_relocation * first = r1;
  const struct ELF_relocation * second = r2;
  int result = (first -> section > second -> section) - (first -> section < second -> section);
  if (!result) result = (first -> offset > second -> offset) - (first -> offset < second -> offset);
  return result;
}
