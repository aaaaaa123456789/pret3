#include "proto.h"

char * get_function_hash (const struct ELF * file, const struct ELF * base, const struct ELF_symbol * reference, char * result) {
  char * error = NULL;
  strcpy(result, "****************************************");
  void * buffer = extract_function_from_ELF(file, base, reference, &error);
  if (!buffer) return error;
  unsigned char * hash = calculate_sha1(buffer, reference -> size);
  free(buffer);
  print_hash(hash, result);
  free(hash);
  return NULL;
}

char * compare_function_data (const struct ELF * file, const struct ELF * base, const struct ELF_symbol * reference, char * result) {
  char * error = NULL;
  strcpy(result, "****");
  void * first = NULL;
  void * second = NULL;
  const struct ELF_symbol * symbol = find_function_symbol(base, reference -> name);
  if (!symbol)
    return generate_string("function %s not found in reference file", reference -> name);
  else if (reference -> size != symbol -> size) {
    strcpy(result, "FAIL");
    return NULL;
  }
  first = extract_function_from_ELF(file, base, reference, &error);
  if (!first) return error;
  second = extract_function_from_ELF(base, NULL, symbol, &error);
  if (!second) {
    char * old_error = error;
    error = generate_string("(in reference file) %s", old_error);
    free(old_error);
    free(first);
    return error;
  }
  strcpy(result, memcmp(first, second, reference -> size) ? "FAIL" : "OK  ");
  free(second);
  free(first);
  return error;
}

void * extract_function_from_ELF (const struct ELF * file, const struct ELF * base, const struct ELF_symbol * reference, char ** error) {
  // assume that reference is a symbol from file
  // base = NULL: file is a stand-alone file; relocate against itself if it is executable, or make up the relocations if it is relocatable
  // base != NULL: base must be an executable file; relocate against base and fail if some relocations are unresolved
  if (!error) return NULL;
  *error = NULL;
  if (!(file || reference))
    *error = duplicate_string("internal error: invalid null pointer argument");
  else if (reference -> type != ELF_SYMBOL_FUNCTION)
    *error = duplicate_string("not a function");
  else if (!(reference -> size))
    *error = duplicate_string("function is empty");
  else if (base && (base -> type != ELF_FILE_EXECUTABLE))
    *error = duplicate_string("relocation base is not an executable ELF file");
  if (*error) return NULL;
  unsigned char * result = malloc(reference -> size + FUNCTION_EXCESS_BUFFER_SIZE);
  unsigned true_offset = reference -> value & ~1; // Thumb functions have the lowest address bit set
  if (file -> type == ELF_FILE_EXECUTABLE) true_offset -= file -> sections[reference -> section].load_address;
  memcpy(result, file -> data + file -> sections[reference -> section].position + true_offset, reference -> size);
  memset(result + reference -> size, 0, FUNCTION_EXCESS_BUFFER_SIZE);
  const struct ELF_relocation * relocation = find_first_relocation(file, reference -> section, true_offset, reference -> size);
  if (!relocation) return result;
  unsigned base_address;
  if (base) {
    const struct ELF_symbol * symbol = find_symbol(base, reference -> name);
    if (!symbol)
      *error = duplicate_string("could not find function in reference file");
    else if (symbol -> type != ELF_SYMBOL_FUNCTION)
      *error = duplicate_string("not a function in reference file");
    else if ((symbol -> value ^ reference -> value) & 1)
      *error = duplicate_string("function has different type in reference file");
    if (*error) goto done;
    base_address = symbol -> value & ~1;
  } else if (file -> type != ELF_FILE_RELOCATABLE) {
    base = file;
    base_address = true_offset + file -> sections[reference -> section].load_address;
  } else
    base_address = compute_symbol_hash(reference -> name) & ~1;
  if (base_address & 1) {
    *error = duplicate_string("misaligned base address");
    goto done;
  }
  while ((relocation < (file -> relocations + file -> relocation_count)) && (relocation -> section == reference -> section)) {
    unsigned pos = relocation -> offset - true_offset;
    if (pos >= reference -> size) break;
    *error = relocate(result + pos, base_address + pos, relocation, file, base);
    if (*error) goto done;
    relocation ++;
  }
  done:
  if (!*error) return result;
  free(result);
  return NULL;
}

const struct ELF_relocation * find_first_relocation (const struct ELF * file, unsigned short section, unsigned offset, unsigned size) {
  unsigned pos = file -> sections[section].item_offset;
  // sections without relocations have an item offset of -1u, aka 0x...FFFFFF, which will never be less than any unsigned integer
  while ((pos < file -> relocation_count) && (file -> relocations[pos].section == section) && (file -> relocations[pos].offset < offset)) pos ++;
  if (pos >= file -> relocation_count) return NULL;
  if ((file -> relocations[pos].section != section) || ((file -> relocations[pos].offset - offset) >= size)) return NULL;
  return file -> relocations + pos;
}
