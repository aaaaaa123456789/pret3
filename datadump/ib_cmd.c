#include "proto.h"

int handle_incbin_data (struct incbin * incbin, const unsigned char * data, FILE * out, char ** script_file) {
  unsigned char rv, offset = 0, limit = 16;
  unsigned p;
  if (incbin -> length < 16) limit = incbin -> length;
  printf("Binary inclusion size: %u\n", incbin -> length);
  printf("First %hhu bytes:", limit);
  for (p = 0; p < limit; p ++) printf(" %02hhx", data[p]);
  putchar('\n');
  p = 0x1f84f;
  if (!(incbin -> length & 1)) p |= 0x10;
  if (!(incbin -> length & 2)) p |= 0x20;
  do {
    rv = get_command("Action for current .incbin: ", p);
    if (rv < 11) break;
    switch (rv) {
      case 11:
        preview_incbin(data, (offset ++) << 8, incbin -> length);
        if ((offset << 8) >= incbin -> length) offset = 0;
        break;
      case 12:
        dump_incbin_as_text(incbin, data);
        break;
      case 13:
        dump_incbin_as_binary(incbin, data);
        break;
      case 14:
        if (run_script(incbin, data, out)) return 0;
        break;
      case 15:
        printf("Script file: ");
        *script_file = read_line(stdin);
        if (!**script_file) {
          free(*script_file);
          *script_file = NULL;
          break;
        }
        return run_script_auto(incbin, data, *script_file, out);
      case 16:
        settings_mode();
    }
  } while (1);
  switch (rv) {
    case 0:
      exit(0);
    case 1:
      return 2;
    case 2:
      return 1;
    case 3: case 4: case 5:
      if (global_settings.insert_replacement_comment) write_header_comment(incbin, out);
      output_binary_data(data, incbin -> length, 1 << (rv - 3), out);
      return 0;
    case 6:
      if (global_settings.insert_replacement_comment) write_header_comment(incbin, out);
      return handle_incbin_text(incbin, data, out);
  }
}

void preview_incbin (const unsigned char * data, unsigned offset, unsigned length) {
  unsigned char line;
  for (line = 0; line < 16; line ++) {
    dump_data_line_as_text(stdout, data, offset, length);
    offset += 16;
    if (offset >= length) break;
  }
}
