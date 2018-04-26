#include "proto.h"

void output_binary_data (const unsigned char * data, unsigned length, unsigned char width, FILE * out) {
  char * output_line;
  unsigned output_length;
  char header[8];
  char fmtstring[8];
  char value[16];
  unsigned p, n;
  if (width > 1)
    sprintf(header, ".%hhubyte", width);
  else
    strcpy(header, ".byte");
  sprintf(fmtstring, "0x%%0%dx", width << 1);
  while (length) {
    generate_initial_indented_line(&output_line, &output_length);
    if (length > 32)
      n = 32;
    else
      n = length;
    length -= n;
    n /= width;
    concatenate(&output_line, &output_length, header, " ", NULL);
    for (p = 0; p < n; p ++) {
      sprintf(value, fmtstring, convert_buffer_to_number(data, width));
      data += width;
      concatenate(&output_line, &output_length, p ? ", " : "", value, NULL);
    }
    printf(">>>> %s\n", output_line);
    fprintf(out, "%s\n", output_line);
    free(output_line);
  }
}

unsigned convert_buffer_to_number (const void * buffer, unsigned char length) {
  unsigned result = 0;
  unsigned char current;
  const unsigned char * p = buffer;
  if (global_settings.endianness)
    for (current = 0; current < length; current ++) result |= ((unsigned) p[current]) << ((length - current - 1) << 3);
  else
    for (current = 0; current < length; current ++) result |= ((unsigned) p[current]) << (current << 3);
  return result;
}
