#include "proto.h"

int process_file (const char * filename, const struct ELF * reference, const struct options * options) {
  char * error = NULL;
  struct ELF * file = load_ELF_file(filename, &error);
  if (!file) {
    printf("error: %s\n", error);
    free(error);
    return 2;
  }
  unsigned * function_symbols;
  int status = 2;
  if (options -> patterns)
    function_symbols = get_matching_function_symbols(file, options -> patterns);
  else
    function_symbols = get_all_function_symbols(file);
  if (!function_symbols || (*function_symbols == -1u)) {
    puts(options -> patterns ? "error: no matching functions found" : "error: file contains no functions");
    goto done;
  }
  char hash[HASH_LENGTH + 1] = {0};
  status = 0;
  char * (* calculation) (const struct ELF *, const struct ELF *, const struct ELF_symbol *, char *);
  calculation = (options -> compare_mode) ? &compare_function_data : &get_function_hash;
  unsigned * current_symbol;
  for (current_symbol = function_symbols; *current_symbol != -1u; current_symbol ++) {
    error = calculation(file, reference, file -> symbols + *current_symbol, hash);
    if (error) {
      printf("error: %s\n", error);
      free(error);
      status = 1;
    }
    printf("%s %s\n", hash, file -> symbols[*current_symbol].name);
  }
  done:
  free(function_symbols);
  destroy_ELF_file(file);
  return status;
}

unsigned * get_all_function_symbols (const struct ELF * file) {
  if (!(file -> function_count)) return NULL;
  unsigned * result = malloc(sizeof(unsigned) * (file -> function_count + 1));
  unsigned pos;
  for (pos = 0; pos < file -> function_count; pos ++)
    result[pos] = file -> sorted_functions[pos] - file -> symbols;
  result[file -> function_count] = -1u;
  return result;
}

unsigned * get_matching_function_symbols (const struct ELF * file, const char ** patterns) {
  if (!(file -> function_count)) return NULL;
  unsigned * result = NULL;
  unsigned pos, count = 0;
  const char ** pattern;
  for (pos = 0; pos < file -> function_count; pos ++) for (pattern = patterns; *pattern; pattern ++)
    if (match_pattern(file -> sorted_functions[pos] -> name, *pattern)) {
      result = realloc(result, sizeof(unsigned) * (count + 1));
      result[count ++] = file -> sorted_functions[pos] - file -> symbols;
      break; // out of the innermost pattern loop
    }
  if (!result) return NULL;
  result = realloc(result, sizeof(unsigned) * (count + 1));
  result[count] = -1u;
  return result;
}

int match_pattern (const char * name, const char * pattern) {
  const char * wildcard = strchr(pattern, '*');
  if (!wildcard) return !strcmp(name, pattern);
  unsigned name_length = strlen(name), tail_length = strlen(wildcard + 1);
  if (name_length < (wildcard - pattern + tail_length)) return 0; // ensure there's no overlap between head and tail matches
  if ((wildcard != pattern) && strncmp(name, pattern, wildcard - pattern)) return 0;
  if (tail_length && strcmp(name + (name_length - tail_length), wildcard + 1)) return 0;
  return 1;
}
