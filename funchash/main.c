#include "proto.h"

int main (int argc, char ** argv) {
  if (argc < 2) {
    print_version();
    fprintf(stderr, "\nNo arguments given. Type %s -? for help.\n", *argv);
    return 3;
  }
  struct options * options = parse_options(argc, argv);
  struct ELF * reference = NULL;
  char * error = NULL;
  if (options -> reference) {
    reference = load_ELF_file(options -> reference, &error);
    if (error) error_exit(2, "error loading %s: %s", options -> reference, error);
    if (reference -> type != ELF_FILE_EXECUTABLE) error_exit(2, "error: %s is not an executable ELF file", options -> reference);
  }
  int status;
  if (options -> filenames[1])
    status = process_multiple_files(reference, options);
  else
    status = process_file(*(options -> filenames), reference, options);
  if (reference) destroy_ELF_file(reference);
  destroy_options(options);
  return status;
}

void print_version (void) {
  fputs("gbafhash (GBA function hasher) - version " VERSION "\n", stderr);
}

void error_exit (int code, const char * fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  putc('\n', stderr);
  exit(code);
}

int process_multiple_files (const struct ELF * reference, const struct options * options) {
  const char ** file;
  int status = 0;
  for (file = options -> filenames; *file; file ++) {
    if (file != options -> filenames) puts("");
    puts(*file);
    if (process_file(*file, reference, options)) status = 1;
  }
  return status;
}
