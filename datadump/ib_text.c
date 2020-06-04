#include "proto.h"

int handle_incbin_text (struct incbin * incbin, const unsigned char * data, FILE * out) {
  int result = 0, yestoall = 0;
  unsigned prev_pos = 0;
  unsigned pos = 0;
  const unsigned char * next_terminator;
  unsigned block;
  char * string;
  unsigned char command;
  char * indent;
  generate_initial_indented_line(&indent, NULL);
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
      printf(">>>> %s.string \"%s\"\n", indent, string);
      fprintf(out, "%s.string \"%s\"\n", indent, string);
      free(string);
      pos += block;
      prev_pos = pos;
      continue;
    }
    printf("String found:\n%s\n", string);
    do {
      command = get_command("Keep string? ", (1 << COMMAND_ABORT) | (1 << COMMAND_DONE) | (1 << COMMAND_YES) | (1 << COMMAND_YESTOALL) |
                                             (1 << COMMAND_NO) | (1 << COMMAND_NOTOALL) | (1 << COMMAND_SETTINGS));
      switch (command) {
        case COMMAND_ABORT:
          exit(0);
        case COMMAND_DONE:
          result = 3;
        case COMMAND_NOTOALL:
          write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, incbin -> length - prev_pos, out);
          free(indent);
          return result;
        case COMMAND_NO:
          free(string);
          pos += block;
          continue;
        case COMMAND_YESTOALL:
          yestoall = 1;
        case COMMAND_YES:
          write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, pos - prev_pos, out);
          printf(">>>> %s.string \"%s\"\n", indent, string);
          fprintf(out, "%s.string \"%s\"\n", indent, string);
          free(string);
          pos += block;
          prev_pos = pos;
          break;
        case COMMAND_SETTINGS:
          settings_mode();
      }
    } while (command == COMMAND_SETTINGS);
  }
  write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, incbin -> length - prev_pos, out);
  free(indent);
  return 0;
}
