#include "proto.h"

void load_symbols (void) {
  printf("ELF file: ");
  char * filename = read_line(stdin);
  if (!*filename) {
    free(filename);
    return;
  }
  const char * error;
  unsigned count;
  struct ELF_symbol ** symbols = read_symbols_from_ELF(filename, &count, &error);
  free(filename);
  if (error) {
    printf("err: %s\n", error);
    return;
  }
  if (global_symbol_table) destroy_ELF_symbols(global_symbol_table, global_symbol_count);
  global_symbol_table = symbols;
  global_symbol_count = count;
  printf("sym: %u symbols loaded\n", count);
}

void unload_symbols (void) {
  if (!global_symbol_table) return;
  destroy_ELF_symbols(global_symbol_table, global_symbol_count);
  global_symbol_table = NULL;
  global_symbol_count = 0;
}

void preload_symbols (const char * file) {
  // similar in spirit to load_symbols(), but we just fail on exit
  const char * error;
  unsigned count;
  struct ELF_symbol ** symbols = read_symbols_from_ELF(file, &count, &error);
  if (error) error_exit(1, "could not load symbols from %s: %s", file, error);
  global_symbol_table = symbols;
  global_symbol_count = count;
  printf("sym: %u symbols loaded\n", count);
}

struct ELF_symbol * find_symbol_for_address (unsigned address) {
  if (!global_symbol_count) return NULL;
  unsigned low = 0, high = global_symbol_count - 1, current;
  while (low < high) {
    current = (low + high + 1) >> 1;
    if (address == global_symbol_table[current] -> value) return check_exact_symbol_match(current);
    if (address < global_symbol_table[current] -> value) {
      if (current <= low) return NULL;
      high = current - 1;
    } else
      low = current;
  }
  if (global_symbol_table[low] -> value == address) return check_exact_symbol_match(low);
  if (global_symbol_table[low] -> type != 1) return NULL;
  if (global_settings.data_labels < 2) return NULL;
  if ((address - global_symbol_table[low] -> value) < global_symbol_table[low] -> size) return global_symbol_table[low];
  return NULL;
}

struct ELF_symbol * check_exact_symbol_match (unsigned index) {
  while ((index < global_symbol_count) && (global_symbol_table[index] -> value == global_symbol_table[index + 1] -> value)) index ++;
  if (global_symbol_table[index] -> type == 2) {
    if (global_settings.code_labels) return global_symbol_table[index];
    return NULL;
  }
  if (global_settings.data_labels) return global_symbol_table[index];
  return NULL;
}
