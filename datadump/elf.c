#include "proto.h"

struct ELF_symbol ** read_symbols_from_ELF (const char * file, unsigned * count, const char ** error) {
  *count = 0;
  FILE * fp = fopen(file, "rb");
  if (!fp) {
    *error = "could not open file for reading";
    return NULL;
  }
  *error = validate_ELF_file(fp);
  if (*error) {
    fclose(fp);
    return NULL;
  }
  unsigned section_count;
  struct ELF_section * sections = read_ELF_section_table(fp, &section_count, error);
  if (*error) {
    fclose(fp);
    return NULL;
  }
  struct ELF_symbol ** symbols = NULL;
  struct ELF_symbol ** section_symbols;
  unsigned current, section_total, cumulative_total = 0;
  for (current = 0; current < section_count; current ++) {
    if (sections[current].type != 2) continue;
    section_symbols = read_ELF_symbols_from_section(fp, sections[current], sections[sections[current].link], &section_total, error);
    if (*error) {
      fclose(fp);
      free(sections);
      destroy_ELF_symbols(symbols, cumulative_total);
      return NULL;
    }
    symbols = realloc(symbols, sizeof(struct ELF_symbol *) * (cumulative_total + section_total));
    memcpy(symbols + cumulative_total, section_symbols, sizeof(struct ELF_symbol *) * section_total);
    free(section_symbols);
    cumulative_total += section_total;
  }
  fclose(fp);
  free(sections);
  if (!cumulative_total) {
    free(symbols);
    *error = "ELF file does not contain symbols";
    return NULL;
  }
  qsort(symbols, cumulative_total, sizeof(struct ELF_symbol *), &compare_ELF_symbols);
  *error = NULL;
  *count = cumulative_total;
  return symbols;
}

void destroy_ELF_symbols (struct ELF_symbol ** symbols, unsigned count) {
  unsigned current;
  for (current = 0; current < count; current ++) free(symbols[current]);
  free(symbols);
}

const char * validate_ELF_file (FILE * file) {
  const char * error;
  if (error = validate_file_value(file, 0, 4, 0x464c457f, "invalid ELF header")) return error;
  if (error = validate_file_value(file, 16, 4, 0x280002, "invalid ELf type")) return error;
  if (error = validate_file_value(file, 20, 4, 1, "invalid ELF version")) return error;
  return NULL;
}

const char * validate_file_value (FILE * file, unsigned offset, unsigned length, unsigned expected, const char * error_message) {
  const char * error;
  unsigned rv = read_file_value(file, offset, length, &error);
  if (error) return error;
  if (rv != expected) return error_message;
  return NULL;
}

struct ELF_section * read_ELF_section_table (FILE * file, unsigned * count, const char ** error) {
  unsigned base_offset, entry_size, section_count;
  base_offset = read_file_value(file, 32, 4, error);
  if (*error) return NULL;
  entry_size = read_file_value(file, 46, 2, error);
  if (*error) return NULL;
  section_count = read_file_value(file, 48, 2, error);
  if (*error) return NULL;
  if (!(base_offset && entry_size && section_count)) {
    *error = "ELF file does not contain sections";
    return NULL;
  }
  if (entry_size < 40) {
    *error = "invalid section entry size";
    return NULL;
  }
  struct ELF_section * result = malloc(section_count * sizeof(struct ELF_section));
  unsigned current, offset;
  for (current = 0, offset = base_offset; current < section_count; current ++, offset += entry_size) {
    result[current].type = read_file_value(file, offset + 4, 4, error);
    if (!*error) result[current].offset = read_file_value(file, offset + 16, 4, error);
    if (!*error) result[current].size = read_file_value(file, offset + 20, 4, error);
    if (!*error) result[current].link = read_file_value(file, offset + 24, 4, error);
    if (!*error) result[current].entry_size = read_file_value(file, offset + 36, 4, error);
    if (*error) {
      free(result);
      return NULL;
    }
  }
  // *error must be NULL here
  *count = section_count;
  return result;
}

struct ELF_symbol ** read_ELF_symbols_from_section (FILE * file, struct ELF_section section, struct ELF_section strings, unsigned * count, const char ** error) {
  struct ELF_symbol ** symbols = NULL;
  struct ELF_symbol * symbol;
  unsigned pos, offset, name_offset, symbol_count = 0;
  unsigned char type;
  char * name;
  *count = 0;
  for (pos = 0; pos < section.size; pos += section.entry_size) {
    offset = pos + section.offset;
    type = read_file_value(file, offset + 12, 1, error) & 15;
    if (*error) {
      destroy_ELF_symbols(symbols, symbol_count);
      return NULL;
    }
    if ((type != 1) && (type != 2)) continue;
    name_offset = read_file_value(file, offset, 4, error);
    if (!*error) name = read_file_string(file, strings.offset + name_offset, error);
    if (*error) {
      destroy_ELF_symbols(symbols, symbol_count);
      return NULL;
    }
    symbol = malloc(sizeof(struct ELF_symbol) + strlen(name) + 1);
    strcpy(symbol -> name, name);
    free(name);
    symbol -> type = type;
    symbol -> value = read_file_value(file, offset + 4, 4, error);
    if (!*error) symbol -> size = read_file_value(file, offset + 8, 4, error);
    if (*error) {
      free(symbol);
      destroy_ELF_symbols(symbols, symbol_count);
      return NULL;
    }
    symbols = realloc(symbols, sizeof(struct ELF_symbol *) * (symbol_count + 1));
    symbols[symbol_count ++] = symbol;
  }
  *count = symbol_count;
  *error = NULL;
  return symbols;
}

int compare_ELF_symbols (const void * first, const void * second) {
  const struct ELF_symbol * s1 = *(struct ELF_symbol **) first;
  const struct ELF_symbol * s2 = *(struct ELF_symbol **) second;
  if (s1 -> value != s2 -> value) return (s1 -> value > s2 -> value) - (s1 -> value < s2 -> value);
  return (s1 -> size < s2 -> size) - (s1 -> size > s2 -> size);
}
