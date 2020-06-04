#include "proto.h"

int handle_incbin_data (struct incbin * incbin, const unsigned char * data, FILE * out, char ** script_file) {
  unsigned char rv, offset = 0, limit = 16;
  unsigned p;
  if (incbin -> length < 16) limit = incbin -> length;
  printf("Binary inclusion size: %u\n", incbin -> length);
  printf("First %hhu bytes:", limit);
  for (p = 0; p < limit; p ++) printf(" %02hhx", data[p]);
  putchar('\n');
  p = (1 << COMMAND_ABORT) | (1 << COMMAND_DONE) | (1 << COMMAND_SKIP) | (1 << COMMAND_DATA8) | (1 << COMMAND_TEXTSCAN) | (1 << COMMAND_PREVIEW) |
      (1 << COMMAND_DUMP) | (1 << COMMAND_DUMPRAW) | (1 << COMMAND_SCRIPT) | (1 << COMMAND_SCRIPTALL) | (1 << COMMAND_SETTINGS) | (1 << COMMAND_LOADSYM);
  if (!(incbin -> length & 1)) p |= 1 << COMMAND_DATA16;
  if (!(incbin -> length & 2)) p |= (1 << COMMAND_DATA32) | (validate_pointers(data, incbin -> length) ? 1 << COMMAND_DATAPTR : 0);
  if (global_symbol_table) p |= 1 << COMMAND_UNLOADSYM;
  do {
    rv = get_command("Action for current .incbin: ", p);
    if (rv < 8) break; // commands 0-7 will handle the .incbin
    switch (rv) {
      case COMMAND_PREVIEW:
        preview_incbin(data, (offset ++) << 8, incbin -> length);
        if ((offset << 8) >= incbin -> length) offset = 0;
        break;
      case COMMAND_DUMP:
        dump_incbin_as_text(incbin, data);
        break;
      case COMMAND_DUMPRAW:
        dump_incbin_as_binary(incbin, data);
        break;
      case COMMAND_SCRIPT:
        if (run_script(incbin, data, out)) return 0;
        break;
      case COMMAND_SCRIPTALL:
        fputs("Script file: ", stdout);
        *script_file = read_line(stdin);
        if (!**script_file) {
          free(*script_file);
          *script_file = NULL;
          break;
        }
        return run_script_auto(incbin, data, *script_file, out);
      case COMMAND_SETTINGS:
        settings_mode();
        if (validate_pointers(data, incbin -> length))
          p |= 1 << COMMAND_DATAPTR;
        else
          p &= ~(1 << COMMAND_DATAPTR);
        break;
      case COMMAND_LOADSYM:
        load_symbols();
        break;
      case COMMAND_UNLOADSYM:
        unload_symbols();
    }
  } while (1);
  if (!rv) exit(0);
  if (rv < 3) return 3 - rv; // 0: abort, 1: done, 2: skip - which respectively return 3, 2 and 1 as the result status
  if (global_settings.insert_replacement_comment) write_header_comment(incbin, out);
  switch (rv) {
    case COMMAND_DATA8: case COMMAND_DATA16: case COMMAND_DATA32:
      // data8, data16 and data32 have sequential command IDs
      output_binary_data(data, incbin -> length, 1 << (rv - COMMAND_DATA8), out);
      return 0;
    case COMMAND_DATAPTR:
      output_pointers(data, incbin -> length, out);
      return 0;
    case COMMAND_TEXTSCAN:
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
