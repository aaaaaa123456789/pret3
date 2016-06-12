#include "proto.h"

char ** execute_script_once (struct incbin * incbin, const void * data, char ** script_lines, char ** error, unsigned * data_position) {
  unsigned line_count = string_array_size(script_lines);
  unsigned count, next_line, current_line = 0;
  struct script_variables * vars = init_script_variables(incbin, data, *data_position);
  char * line_error;
  int rv;
  char * output_line;
  char ** output = NULL;
  unsigned output_count = 0;
  unsigned char line_type;
  while ((!*error) && (current_line < line_count))
    switch (get_line_type(script_lines[current_line])) {
      case 0:
        current_line ++;
        break;
      case 1:
        count = vars -> values -> value;
        line_error = script_parse_assignment_line(script_lines[current_line], vars);
        *data_position += count - vars -> values -> value;
        if (line_error) *error = generate_script_error(line_error, current_line + 1);
        current_line ++;
        break;
      case 2:
        line_error = NULL;
        rv = script_loop_test(script_lines[current_line], vars, &line_error);
        if (line_error) {
          *error = generate_script_error(line_error, current_line + 1);
          break;
        }
        if (rv) {
          current_line ++;
          break;
        }
        for (next_line = current_line + 1; (next_line < line_count) && (get_line_type(script_lines[next_line]) != 3); next_line ++);
        if (next_line >= line_count) {
          *error = generate_script_error(duplicate_string("unmatched start of loop"), current_line + 1);
          break;
        }
        current_line = next_line + 1;
        break;
      case 3:
        count = 1;
        for (next_line = current_line - 1; next_line < current_line; next_line --) {
          line_type = get_line_type(script_lines[next_line]);
          if (line_type == 3)
            count ++;
          else if (line_type == 2) {
            count --;
            if (!count) break;
          }
        }
        if (next_line >= current_line) {
          *error = generate_script_error(duplicate_string("unmatched end of loop"), current_line + 1);
          break;
        }
        current_line = next_line;
        break;
      case 4:
        line_error = NULL;
        output_line = generate_script_output_line(script_lines[current_line], vars, &line_error);
        if (line_error) {
          *error = generate_script_error(line_error, current_line + 1);
          break;
        }
        output = realloc(output, sizeof(char *) * (output_count + 1));
        output[output_count ++] = output_line;
        current_line ++;
    }
  if (*error) {
    while (output_count) free(output[-- output_count]);
    free(output);
    return NULL;
  }
  output = realloc(output, sizeof(char *) * (output_count + 1));
  output[output_count] = NULL;
  return output;
}

char * generate_script_error (char * line_error, unsigned line_number) {
  char * error = malloc(strlen(line_error) + 20);
  sprintf(error, "line %u: %s", line_number, line_error);
  free(line_error);
  return error;
}

int script_loop_test (const char * line, struct script_variables * vars, char ** error) {
  const char * variable = find_first_non_space(line) + 2;
  struct script_value * value = find_script_variable(vars, variable);
  if (!value) {
    *error = duplicate_string("unknown variable");
    return 0;
  }
  return value -> value;
}
