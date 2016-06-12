#include "proto.h"

void dump_incbin_as_text (struct incbin * incbin, const unsigned char * data) {
  char filename[20];
  sprintf(filename, "incbin_%08x.txt", incbin -> offset);
  FILE * fp = fopen(filename, "w");
  if (!fp) {
    printf("err: could not open file %s for writing\n", filename);
    return;
  }
  unsigned offset;
  for (offset = 0; offset < incbin -> length; offset += 16) dump_data_line_as_text(fp, data, offset, incbin -> length);
  fclose(fp);
}

void dump_incbin_as_binary (struct incbin * incbin, const unsigned char * data) {
  char filename[20];
  sprintf(filename, "incbin_%08x.bin", incbin -> offset);
  FILE * fp = fopen(filename, "wb");
  if (!fp) {
    printf("err: could not open file %s for writing\n", filename);
    return;
  }
  if (fwrite(data, 1, incbin -> length, fp) != incbin -> length) printf("err: failed writing data to file %s\n", filename);
  fclose(fp);
}

void dump_data_line_as_text (FILE * out, const unsigned char * data, unsigned offset, unsigned length) {
  unsigned char amount, p, c;
  fprintf(out, "%08x: ", offset);
  amount = ((length - offset) > 16) ? 16 : (length - offset);
  for (p = 0; p < amount; p ++) fprintf(out, "%02hhx ", data[offset + p]);
  for (; p < 16; p ++) fprintf(out, "   ");
  for (p = 0; p < amount; p ++) {
    c = data[offset + p];
    if ((previewable[c >> 3] >> (c & 7)) & 1)
      fprintf(out, "%s", text_table[c]);
    else
      putc('@', out);
  }
  putc('\n', out);
}
