#include "proto.h"

int main (int argc, char ** argv) {
  exe_name = *argv;
  int mode = parse_options(argv + 1, argc - 1);
  switch (mode) {
    case MODE_INTERACTIVE:
      return interactive_mode();
    case MODE_AUTO_DATA_8:
      // ...
    case MODE_AUTO_DATA_16:
      // ...
    case MODE_AUTO_DATA_32:
      // ...
    case MODE_AUTO_DATA_PTR:
      // ...
    case MODE_AUTO_SCRIPT:
      // ...
    default:
      return 1;
  }
}

int interactive_mode (void) {
  // ...
  FILE * in;
  FILE * out;
  if (command_line_filename_count != 2) {
    fprintf(stderr, "usage: %s <infile> <outfile> <directory>\n", exe_name);
    return 1;
  }
  in = fopen(*command_line_filenames, "r");
  if (!in) {
    fprintf(stderr, "error: could not open file %s for reading\n", *command_line_filenames);
    return 1;
  }
  out = fopen(command_line_filenames[1], "w");
  if (!out) {
    fprintf(stderr, "error: could not open file %s for writing\n", command_line_filenames[1]);
    return 1;
  }
  dump_incbins(in, out);
  fclose(in);
  fclose(out);
  return 0;
}
