#include "proto.h"

int validate_pointers (const unsigned char * data, unsigned length) {
  unsigned count, current, nonzero = 0, pos = 0;
  while (pos < length) {
    current = 0;
    for (count = 0; count < 4; count ++) current |= ((unsigned) data[pos ++]) << (count << 3);
    if (current) nonzero = current;
    switch (current >> 24) {
      case 0:
        if (current) return 0;
        break;
      case 2:
        if (current >= 0x2040000) return 0;
        break;
      case 3:
        if (current >= 0x3080000) return 0;
      case 8: case 9:
        break;
      default:
        return 0;
    }
  }
  return nonzero;
}

void output_pointers (const unsigned char * data, unsigned length, FILE * out) {
  unsigned count, current, pos = 0;
  while (pos < length) {
    current = 0;
    for (count = 0; count < 4; count ++) current |= ((unsigned) data[pos ++]) << (count << 3);
    output_pointer(current, out);
  }
}

void output_pointer (unsigned pointer, FILE * out) {
  char * output_line;
  unsigned output_length;
  char value[11];
  generate_initial_indented_line(&output_line, &output_length);
  concatenate(&output_line, &output_length, ".4byte ", NULL);
  if (!pointer) {
    printf(">>>> %s0\n", output_line);
    fprintf(out, "%s0\n", output_line);
    free(output_line);
    return;
  }
  struct ELF_symbol * symbol = NULL;
  if (global_symbol_table) symbol = find_symbol_for_address(pointer);
  if (symbol) {
    pointer -= symbol -> value;
    concatenate(&output_line, &output_length, symbol -> name, pointer ? " + " : "", NULL);
  }
  if (pointer) {
    sprintf(value, "0x%x", pointer);
    concatenate(&output_line, &output_length, value, NULL);
  }
  printf(">>>> %s\n", output_line);
  fprintf(out, "%s\n", output_line);
  free(output_line);
}
