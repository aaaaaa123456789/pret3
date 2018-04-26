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
  unsigned pos;
  for (pos = 0; pos < length; pos += 4) output_pointer(convert_buffer_to_number(data + pos, 4), out);
}

void output_pointer (unsigned pointer, FILE * out) {
  char * output_line;
  unsigned output_length;
  generate_initial_indented_line(&output_line, &output_length);
  char * pointer_text = generate_pointer_text(pointer);
  concatenate(&output_line, &output_length, ".4byte ", pointer_text, NULL);
  free(pointer_text);
  printf(">>>> %s\n", output_line);
  fprintf(out, "%s\n", output_line);
  free(output_line);
}

char * generate_pointer_text (unsigned pointer) {
  if (!pointer) return duplicate_string("0");
  char * result = NULL;
  unsigned result_length = 0;
  char value[11];
  struct ELF_symbol * symbol = NULL;
  if (global_symbol_table) symbol = find_symbol_for_address(pointer);
  if (symbol) {
    pointer -= symbol -> value;
    concatenate(&result, &result_length, symbol -> name, pointer ? " + " : "", NULL);
  }
  if (pointer) {
    sprintf(value, "0x%x", pointer);
    concatenate(&result, &result_length, value, NULL);
  }
  return result;
}
