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

void dump_incbins_to_data (FILE * file, unsigned char bytes_per_value) {
  // always dumps to the global temporary file
  // bytes_per_value = 0 means dump pointers
  char * line;
  struct incbin * incbin;
  void * data;
  while (!feof(file)) {
    line = read_line(file);
    if (is_incbin(line)) {
      printf("<<<< %s\n", line);
      incbin = get_incbin_data(line);
      if (incbin) {
        if (incbin -> length % (bytes_per_value ? bytes_per_value : 4)) goto nodump;
        if (!(data = get_incbin_contents(incbin))) goto nodump;
        if (!(bytes_per_value || validate_pointers(data, incbin -> length))) {
          free(data);
          goto nodump;
        }
        if (global_settings.insert_replacement_comment) write_header_comment(incbin, global_temporary_file);
        if (bytes_per_value)
          output_binary_data(data, incbin -> length, bytes_per_value, global_temporary_file);
        else
          output_pointers(data, incbin -> length, global_temporary_file);
        free(data);
      } else {
        nodump:
        printf(">>>> %s\n", line);
        fprintf(global_temporary_file, "%s\n", line);
      }
      free(incbin);
    } else {
      printf("==== %s\n", line);
      fprintf(global_temporary_file, "%s\n", line);
    }
    free(line);
  }
}
