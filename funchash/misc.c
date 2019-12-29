#include "proto.h"

void * read_file_as_bytes (const char * filename, unsigned * size) {
  if (size) *size = 0;
  FILE * fp = fopen(filename, "rb");
  if (!fp) return NULL;
  char * result = NULL;
  char buffer[16384];
  unsigned read_count, result_size = 0;
  char * new_result;
  do {
    read_count = fread(buffer, 1, sizeof buffer, fp);
    if ((result_size + read_count) < result_size) goto error;
    new_result = realloc(result, result_size + read_count);
    if (!new_result) goto error;
    result = new_result;
    memcpy(result + result_size, buffer, read_count);
    result_size += read_count;
  } while (read_count == sizeof buffer);
  if (result_size) {
    if (size) *size = result_size;
    return result;
  }
  error:
  free(result);
  return NULL;
}

const struct ELF_symbol * find_symbol (const struct ELF * file, const char * name) {
  const struct ELF_symbol * const * result = find_symbol_pointer(file, name, file -> sorted_symbols, file -> symbol_count);
  if (!result) return NULL;
  return *result;
}

const struct ELF_symbol * find_function_symbol (const struct ELF * file, const char * name) {
  const struct ELF_symbol * const * result = find_symbol_pointer(file, name, file -> sorted_functions, file -> function_count);
  if (!result) return NULL;
  return *result;
}

const struct ELF_symbol * const * find_symbol_pointer (const struct ELF * file, const char * name, const struct ELF_symbol ** list, unsigned length) {
  if (!(file && name && *name)) return NULL;
  const struct ELF_symbol * const * result = bsearch(name, list, length, sizeof(struct ELF_symbol *), &match_symbol_name);
  if (!result) return NULL;
  while ((result > file -> sorted_symbols) && !strcmp(name, result[-1] -> name)) result --;
  return result;
}

int match_symbol_name (const void * name, const void * symbol) {
  return strcmp(name, (**(const struct ELF_symbol * const *) symbol).name);
}

unsigned compute_symbol_hash (const char * name) {
  unsigned char * hash = calculate_sha1(name, strlen(name));
  unsigned pos, result = 0;
  for (pos = 0; pos < 5; pos ++) result ^= convert_buffer_to_number(hash + (pos << 2), 4);
  free(hash);
  return result;
}

void print_hash (const unsigned char * restrict hash, char * restrict result) {
  // outputs 40 characters; assumes that result has room for 40 + terminator
  unsigned char current, pos;
  for (pos = 0; pos < ((HASH_LENGTH + 1) >> 1); pos ++) {
    current = hash[pos];
    *(result ++) = hex_digit(current >> 4);
    *(result ++) = hex_digit(current & 15);
  }
  *result = 0;
}
