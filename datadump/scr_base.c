#include "proto.h"

char * script_get_expression_value (const char * expression, struct script_variables * vars, int * result) {
  if (*expression == '$') {
    struct script_value * val = find_script_variable(vars, expression + 1);
    if (!val)
      return duplicate_string("unknown variable");
    else if (!(val -> type) || (val -> type > 3))
      return duplicate_string("variable does not contain an integer");
    *result = val -> value;
    return NULL;
  }
  if (!convert_string_to_number(expression, 1)) return duplicate_string("syntax error");
  *result = convert_string_to_number(expression, 0);
  return NULL;
}

unsigned char get_line_type (const char * line) {
  // 0: blank/comment, 1: assignment, 2: loop start, 3: loop end, 4: print
  line = find_first_non_space(line);
  switch (*line) {
    case 0: return 0;
    case '$': return 1;
    case ':':
      switch (line[1]) {
        case ':': return 3;
        case '$': return 2;
        default: return 0;
      }
    default: return 4;
  }
}
