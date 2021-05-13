#include "proto.h"

struct options * parse_options (int argc, char ** argv) {
  struct options * result = calloc(1, sizeof *result);
  unsigned pattern_count = 0, filename_count = 0;
  int raw_mode = 0;
  for (argv ++; *argv; argv ++)
    if (!raw_mode && (**argv == '-')) {
      if (!1[*argv] || 2[*argv]) error_exit(3, "unknown option %s", *argv);
      switch (1[*argv]) {
        case '-':
          raw_mode = 1;
          break;
        case 'f':
          if (!argv[1]) error_exit(3, "error: -f option requires an argument");
          validate_pattern(*(++ argv));
          result -> patterns = realloc(result -> patterns, sizeof(const char *) * (pattern_count + 1));
          result -> patterns[pattern_count ++] = *argv;
          break;
        case 'c':
          result -> compare_mode = 1;
        case 'r':
          if (result -> reference) error_exit(3, "error: -c and -r options can only be given once and not simultaneously");
          if (!argv[1]) error_exit(3, "error: -%c option requires an argument", 1[*argv]);
          result -> reference = *(++ argv);
          break;
        case 'q':
          if (result -> suppress_errors) error_exit(3, "error: -q option can only be given once");
          result -> suppress_errors = 1;
          break;
        case 'v':
          print_version();
          exit(3);
        case '?': case 'h':
          print_help();
          exit(3);
        default:
          error_exit(3, "unknown option -%c", 1[*argv]);
      }
    } else {
      result -> filenames = realloc(result -> filenames, sizeof(const char *) * (filename_count + 1));
      result -> filenames[filename_count ++] = *argv;
    }
  if (!filename_count) error_exit(3, "error: no input files");
  result -> filenames = realloc(result -> filenames, sizeof(const char *) * (filename_count + 1));
  result -> filenames[filename_count] = NULL;
  if (pattern_count) {
    result -> patterns = realloc(result -> patterns, sizeof(const char *) * (pattern_count + 1));
    result -> patterns[pattern_count] = NULL;
  }
  return result;
}

void validate_pattern (const char * pattern) {
  const char * wildcard = strchr(pattern, '*');
  if (!wildcard) return;
  wildcard = strchr(wildcard + 1, '*');
  if (wildcard) error_exit(3, "error: pattern contains more than one wildcard: %s", pattern);
}

void destroy_options (struct options * options) {
  free(options -> filenames);
  free(options -> patterns);
  free(options);
}

void print_help (void) {
  print_version();
  fputs("gbafhash is released to the public domain. No copyright is claimed, and no\n"
        "warranty, implied or otherwise, is given with the program.\n"
        "\n"
        "Usage: gbafhash [-q] [-c <base> | -r <base>] [-f <pattern> [-f <pattern> ...]]\n"
        "                [--] <file> [<file> ...]\n"
        " -f <pattern>: only hash functions matching the name or pattern given in this\n"
        "               option; the pattern can be a function name to match exactly, or\n"
        "               it may contain a single * character that will match anything.\n"
        "               This option may be given more than once.\n"
        " -r <base>:    calculate relocations against the symbols defined by this file.\n"
        " -c <base>:    compare functions against the ones defined in this function,\n"
        "               printing an OK/FAIL status instead of a hash for each function.\n"
        "               This option implies -r, and they cannot be combined.\n"
        " -q:           suppress all error output unless a fatal error occurs.\n"
        "Note that -- may be used to terminate the option list and parse all remaining\n"
        "command-line arguments as filenames. Also, -v will print version information.\n"
        "If neither -r nor -c are given, relocations will be handled depending on the\n"
        "type of file given, as defined by the ELF header. For executable files, the\n"
        "relocations contained in the file itself will be used; for relocatable files,\n"
        "fake relocations will be calculated for all symbols based on a hash of the\n"
        "symbol's name. In all cases, missing symbols will cause relocation to fail.\n", stderr);
}
