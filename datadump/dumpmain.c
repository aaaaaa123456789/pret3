#include "proto.h"

void dump_incbins_interactively (FILE * in, FILE * out) {
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

void dump_incbins_via_callback (FILE * file, int (* callback) (struct incbin *, void *, void *), void * argument) {
  // always dumps to the global temporary file
  char * line;
  struct incbin * incbin;
  void * data;
  int rv;
  while (!feof(file)) {
    line = read_line(file);
    if (is_incbin(line)) {
      printf("<<<< %s\n", line);
      incbin = get_incbin_data(line);
      if (incbin) {
        data = get_incbin_contents(incbin);
        if (!data) goto nodump;
        rv = callback(incbin, data, argument);
        free(data);
        if (!rv) goto nodump;
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

int dump_data_from_incbin (struct incbin * incbin, void * data, void * argument) {
  // bytes_per_value = 0 means dump pointers
  unsigned bytes_per_value = *((unsigned *) argument);
  if (incbin -> length % (bytes_per_value ? bytes_per_value : 4)) return 0;
  if (!(bytes_per_value || validate_pointers(data, incbin -> length))) return 0;
  if (global_settings.insert_replacement_comment) write_header_comment(incbin, global_temporary_file);
  if (bytes_per_value)
    output_binary_data(data, incbin -> length, bytes_per_value, global_temporary_file);
  else
    output_pointers(data, incbin -> length, global_temporary_file);
  return 1;
}

int dump_data_with_script (struct incbin * incbin, void * data, void * script) {
  char ** script_lines = script;
  char * error = NULL;
  char ** output_lines = execute_script(incbin, data, script_lines, &error);
  return handle_script_output(output_lines, error, incbin, global_temporary_file);
}
