#include "proto.h"

void dump_incbin_as_text (struct incbin * incbin, const unsigned char * data) {
  FILE * fp = get_dump_file("w");
  if (!fp) return;
  unsigned offset;
  for (offset = 0; offset < incbin -> length; offset += 16) dump_data_line_as_text(fp, data, offset, incbin -> length);
  fclose(fp);
}

void dump_incbin_as_binary (struct incbin * incbin, const unsigned char * data) {
  FILE * fp = get_dump_file("wb");
  if (!fp) return;
  if (fwrite(data, 1, incbin -> length, fp) != incbin -> length) printf("err: failed writing data to file\n");
  fclose(fp);
}

void dump_data_line_as_text (FILE * out, const unsigned char * data, unsigned offset, unsigned length) {
  unsigned char amount, p, c;
  fprintf(out, "%08x:   ", offset);
  amount = ((length - offset) > 16) ? 16 : (length - offset);
  for (p = 0; p < amount; p ++) fprintf(out, "%02hhx ", data[offset + p]);
  for (; p < 16; p ++) fprintf(out, "   ");
  fprintf(out, "  ");
  for (p = 0; p < amount; p ++) {
    c = data[offset + p];
    if ((previewable[c >> 3] >> (c & 7)) & 1)
      fprintf(out, "%s", text_table[c]);
    else
      putc('@', out);
  }
  putc('\n', out);
}

FILE * get_dump_file (const char * fopen_mode) {
  printf("File to write: ");
  char * filename = read_line(stdin);
  if (!*filename) {
    free(filename);
    return NULL;
  }
  FILE * fp = fopen(filename, fopen_mode);
  if (!fp) {
    printf("err: could not open file %s for writing\n", filename);
    free(filename);
    return NULL;
  }
  free(filename);
  return fp;
}
