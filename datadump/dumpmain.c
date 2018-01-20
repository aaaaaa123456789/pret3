#include "proto.h"

void dump_incbins (FILE * in, FILE * out) {
  char * line;
  struct incbin * incbin;
  int done = 0;
  char * script_file = NULL;
  while (!feof(in)) {
    line = read_line(in);
    if ((!done) && is_incbin(line)) {
      printf("<<<< %s\n", line);
      incbin = get_incbin_data(line);
      if (!incbin) {
        printf(">>>> %s\n", line);
        fprintf(out, "%s\n", line);
      } else {
        switch (parse_incbin(incbin, out, &script_file)) {
          case 3:
            done = 1;
            break;
          case 2:
            done = 1;
          case 1:
            printf(">>>> %s\n", line);
            fprintf(out, "%s\n", line);
        }
        free(incbin);
      }
    } else {
      printf("==== %s\n", line);
      fprintf(out, "%s\n", line);
    }
    free(line);
  }
  free(script_file);
}
