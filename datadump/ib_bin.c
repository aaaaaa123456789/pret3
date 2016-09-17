#include "proto.h"

void output_binary_data (const unsigned char * data, unsigned length, unsigned char width, FILE * out) {
  char * output_line;
  unsigned output_length;
  char header[8];
  char fmtstring[8];
  char value[16];
  unsigned p, n, val;
  unsigned char pos;
  if (width > 1)
    sprintf(header, ".%hhubyte", width);
  else
    strcpy(header, ".byte");
  sprintf(fmtstring, "0x%%0%hhux", width << 1);
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
      val = 0;
      for (pos = 0; pos < width; pos ++) val |= *(data ++) << (pos << 3);
      sprintf(value, fmtstring, val);
      concatenate(&output_line, &output_length, p ? ", " : "", value, NULL);
    }
    printf(">>>> %s\n", output_line);
    fprintf(out, "%s\n", output_line);
    free(output_line);
  }
}
