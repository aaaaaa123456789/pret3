#include "proto.h"

int handle_incbin_data (struct incbin * incbin, const unsigned char * data, FILE * out, char ** script_file) {
  unsigned char rv, offset = 0, limit = 16;
  unsigned p;
  if (incbin -> length < 16) limit = incbin -> length;
  printf("Binary inclusion size: %u\n", incbin -> length);
  printf("First %hhu bytes:", limit);
  for (p = 0; p < limit; p ++) printf(" %02hhx", data[p]);
  putchar('\n');
  p = 0x7f08f;
  if (!(incbin -> length & 1)) p |= 0x10;
  if (!(incbin -> length & 2)) p |= validate_pointers(data, incbin -> length) ? 0x60 : 0x20;
  if (global_symbol_table) p |= 0x80000;
  do {
    rv = get_command("Action for current .incbin: ", p);
    if (rv < 12) break;
    switch (rv) {
      case 12:
        preview_incbin(data, (offset ++) << 8, incbin -> length);
        if ((offset << 8) >= incbin -> length) offset = 0;
        break;
      case 13:
        dump_incbin_as_text(incbin, data);
        break;
      case 14:
        dump_incbin_as_binary(incbin, data);
        break;
      case 15:
        if (run_script(incbin, data, out)) return 0;
        break;
      case 16:
        fputs("Script file: ", stdout);
        *script_file = read_line(stdin);
        if (!**script_file) {
          free(*script_file);
          *script_file = NULL;
          break;
        }
        return run_script_auto(incbin, data, *script_file, out);
      case 17:
        settings_mode();
        if (validate_pointers(data, incbin -> length))
          p |= 0x40;
        else
          p &= ~0x40;
        break;
      case 18:
        load_symbols();
        break;
      case 19:
        unload_symbols();
    }
  } while (1);
  if (!rv) exit(0);
  if (rv < 3) return 3 - rv;
  if (global_settings.insert_replacement_comment) write_header_comment(incbin, out);
  switch (rv) {
    case 3: case 4: case 5:
      output_binary_data(data, incbin -> length, 1 << (rv - 3), out);
      return 0;
    case 6:
      output_pointers(data, incbin -> length, out);
      return 0;
    case 7:
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
