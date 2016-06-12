#include "proto.h"

int handle_incbin_text (struct incbin * incbin, const unsigned char * data, FILE * out) {
  int result = 0, yestoall = 0;
  unsigned prev_pos = 0;
  unsigned pos = 0;
  const unsigned char * next_terminator;
  unsigned block;
  char * string;
  unsigned char command;
  while (pos < incbin -> length) {
    next_terminator = memchr(data + pos, 0xff, incbin -> length - pos);
    if (!next_terminator)
      block = incbin -> length - pos;
    else
      block = (next_terminator - data) - pos + 1;
    if (block > 999) {
      pos += block;
      continue;
    }
    if (!(string = parse_buffer(data + pos, block))) continue;
    if (yestoall) {
      write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, pos - prev_pos, out);
      printf(">>>> .string \"%s\"\n", string);
      fprintf(out, ".string \"%s\"\n", string);
      free(string);
      pos += block;
      prev_pos = pos;
      continue;
    }
    printf("String found:\n%s\n", string);
    command = get_command("Keep string? ", 0x783);
    switch (command) {
      case 0:
        exit(0);
      case 1:
        result = 3;
      case 10:
        write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, incbin -> length - prev_pos, out);
        return result;
      case 9:
        free(string);
        pos += block;
        continue;
      case 8:
        yestoall = 1;
      case 7:
        write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, pos - prev_pos, out);
        printf(">>>> .string \"%s\"\n", string);
        fprintf(out, ".string \"%s\"\n", string);
        free(string);
        pos += block;
        prev_pos = pos;
    }
  }
  write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, incbin -> length - prev_pos, out);
  return 0;
}
