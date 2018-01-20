#include "proto.h"

int main (int argc, char ** argv) {
  exe_name = *argv;
  int mode = parse_options(argv + 1, argc - 1);
  if (!repository_path) error_exit(1, "repository path not given");
  if (!command_line_filename_count) error_exit(1, "no input files");
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

void error_exit (int status, const char * fmt, ...) {
  va_list ap;
  fputs("error: ", stderr);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  putc('\n', stderr);
  va_end(ap);
  exit(status);
}

int interactive_mode (void) {
  FILE * in;
  FILE * out;
  if (command_line_filename_count > 2) error_exit(1, "only a single input and output file can be given in interactive mode");
  in = fopen(*command_line_filenames, "r");
  if (!in) error_exit(2, "could not open file %s for reading", *command_line_filenames);
  if (command_line_filename_count == 2) {
    out = fopen(command_line_filenames[1], "w");
    if (!out) error_exit(2, "could not open file %s for writing", command_line_filenames[1]);
    dump_incbins(in, out);
    fclose(in);
    fclose(out);
  } else {
    global_temporary_file = tmpfile();
    if (!global_temporary_file) error_exit(2, "could not create temporary file");
    dump_incbins(in, global_temporary_file);
    fclose(in);
    in = fopen(*command_line_filenames, "w");
    if (!in) {
      fclose(global_temporary_file);
      error_exit(2, "could not open file %s for writing", *command_line_filenames);
    }
    transfer_temporary_to_file(in);
  }
  return 0;
}
