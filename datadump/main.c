#include "proto.h"

int main (int argc, char ** argv) {
  FILE * in;
  FILE * out;
  if (argc != 4) {
    fprintf(stderr, "usage: %s <infile> <outfile> <directory>\n", *argv);
    return 1;
  }
  in = fopen(argv[1], "r");
  if (!in) {
    fprintf(stderr, "error: could not open file %s for reading\n", argv[1]);
    return 1;
  }
  out = fopen(argv[2], "w");
  if (!out) {
    fprintf(stderr, "error: could not open file %s for writing\n", argv[2]);
    return 1;
  }
  dump_incbins(in, out, argv[3]);
  fclose(in);
  fclose(out);
  return 0;
}

void dump_incbins (FILE * in, FILE * out, const char * root) {
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
        switch (parse_incbin(incbin, root, out, &script_file)) {
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
