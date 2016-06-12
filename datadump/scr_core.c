#include "proto.h"

int run_script (struct incbin * incbin, const void * data, FILE * out) {
  printf("Script file: ");
  char * script_name = read_line(stdin);
  if (!*script_name) {
    free(script_name);
    return 0;
  }
  FILE * script = fopen(script_name, "r");
  free(script_name);
  if (!script) {
    printf("err: could not open script file\n");
    return 0;
  }
  char ** script_lines = read_file_by_lines(script);
  fclose(script);
  char * error = NULL;
  char ** output_lines = execute_script(incbin, data, script_lines, &error);
  destroy_string_array(script_lines);
  if (error) {
    printf("err: %s\n", error);
    free(error);
    return 0;
  }
  write_header_comment(incbin, out);
  char ** line;
  for (line = output_lines; *line; line ++) {
    printf(">>>> %s\n", *line);
    fprintf(out, "%s\n", *line);
  }
  destroy_string_array(output_lines);
  return 1;
}

char ** execute_script (struct incbin * incbin, const void * data, char ** script_lines, char ** error) {
  unsigned data_position = 0;
  char ** lines = calloc(sizeof(char *), 1);
  unsigned exec_line_count, line_count = 0;
  char ** execution_lines;
  while ((!*error) && (data_position < incbin -> length)) {
    execution_lines = execute_script_once(incbin, data, script_lines, error, &data_position);
    if (!execution_lines) continue;
    exec_line_count = string_array_size(execution_lines);
    lines = realloc(lines, sizeof(char *) * (line_count + exec_line_count + 1));
    memcpy(lines + line_count, execution_lines, sizeof(char *) * exec_line_count);
    free(execution_lines);
    line_count += exec_line_count;
    lines[line_count] = NULL;
  }
  if (*error) {
    destroy_string_array(lines);
    return NULL;
  }
  return lines;
}
