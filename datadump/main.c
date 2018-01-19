#include "proto.h"

int main (int argc, char ** argv) {
  exe_name = *argv;
  int mode = parse_options(argv + 1, argc - 1);
  if (!repository_path) {
    fputs("error: repository path not given\n", stderr);
    return 1;
  }
  if (!command_line_filename_count) {
    fputs("error: no input files\n", stderr);
    return 1;
  }
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
  FILE * in;
  FILE * out;
  if (command_line_filename_count > 2) {
    fputs("error: only a single input and output file can be given in interactive mode\n", stderr);
    return 1;
  }
  in = fopen(*command_line_filenames, "r");
  if (!in) {
    fprintf(stderr, "error: could not open file %s for reading\n", *command_line_filenames);
    return 2;
  }
  if (command_line_filename_count == 2) {
    out = fopen(command_line_filenames[1], "w");
    if (!out) {
      fprintf(stderr, "error: could not open file %s for writing\n", command_line_filenames[1]);
      return 2;
    }
    dump_incbins(in, out);
    fclose(in);
    fclose(out);
  } else {
    global_temporary_file = tmpfile();
    if (!global_temporary_file) {
      fclose(in);
      fputs("error: could not create temporary file\n", stderr);
      return 2;
    }
    dump_incbins(in, global_temporary_file);
    fclose(in);
    in = fopen(*command_line_filenames, "w");
    if (!in) {
      fprintf(stderr, "error: could not open file %s for writing\n", *command_line_filenames);
      fclose(global_temporary_file);
      return 2;
    }
    transfer_temporary_to_file(in);
  }
  return 0;
}
