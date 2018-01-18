#include "proto.h"

int parse_options (char ** options, unsigned count) {
  int done_options = 0;
  int mode = MODE_INTERACTIVE;
  unsigned current;
  for (current = 0; current < count; current ++) {
    if (!options[current]) break;
    if (done_options || (*(options[current]) != '-')) {
      // ...
    } else if (!strcmp(options[current], "--"))
      done_options = 1;
    else if (!strcmp(options[current], "-a")) {
      if (mode) multiple_execution_mode_error(mode);
      mode = auto_execution_mode(get_option_argument(options, count, &current));
    } else if (!strcmp(options[current], "-c"))
      parse_configuration_line(get_option_argument(options, count, &current));
    // ...
    else {
      fprintf(stderr, "error: unknown option: %s\n", options[current]);
      exit(1);
    }
  }
  return mode;
}

const char * get_option_argument (char ** options, unsigned count, unsigned * current) {
  if ((*current + 1) >= count) {
    fprintf(stderr, "error: option %s requires an argument\n", options[*current]);
    exit(1);
  }
  return options[++ (*current)];
}

void multiple_execution_mode_error (int previous_mode) {
  fprintf(stderr, "error: conflicting execution modes given (previous mode was %s)\n", ((previous_mode >= 0) && (previous_mode <= 5)) ?
          previous_mode[(const char * []) {"interactive", "-a 8", "-a 16", "-a ptr", "-a 32", "-s"}] : "unknown");
  exit(1);
}

int auto_execution_mode (const char * mode_string) {
  // we could parse the number, but to keep it simple, just do a string compare -- nobody should be doing -a 0x20 anyway
  if (!strcmp(mode_string, "8"))
    return MODE_AUTO_DATA_8;
  else if (!strcmp(mode_string, "16"))
    return MODE_AUTO_DATA_16;
  else if (!strcmp(mode_string, "32"))
    return MODE_AUTO_DATA_32;
  else if (!strcmp(mode_string, "ptr"))
    return MODE_AUTO_DATA_PTR;
  fprintf(stderr, "error: unknown execution mode: -a %s\n", mode_string);
  exit(1);
}
