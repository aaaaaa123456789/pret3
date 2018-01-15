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
