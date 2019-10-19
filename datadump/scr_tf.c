#include "proto.h"

char * script_parse_assignment_line (const char * line, struct script_variables * vars, unsigned * new_incbin_size) {
  *new_incbin_size = 0;
  char ** parts = split_by_spaces(line);
  if (!parts || (string_array_size(parts) < 2)) {
    if (parts) destroy_string_array(parts);
    return duplicate_string("syntax error in assignment statement");
  }
  if ((**parts != '$') || !validate_variable_name(1 + *parts)) {
    destroy_string_array(parts);
    return duplicate_string("invalid variable for assignment");
  }
  const char * variable = 1 + *parts;
  unsigned transform_count = string_array_size(parts + 2);
  struct script_transforms * line_transforms = calloc(transform_count, sizeof(struct script_transforms));
  char * error = script_get_transforms_for_line(parts + 2, vars, line_transforms);
  if (error) {
    destroy_string_array(parts);
    free(line_transforms);
    return error;
  }
  struct script_value initial_value;
  error = script_get_initializer_value(parts[1], &initial_value, vars, new_incbin_size);
  if (error) {
    destroy_string_array(parts);
    free(line_transforms);
    return error;
  }
  struct script_value result;
  error = execute_transforms(initial_value, line_transforms, transform_count, &result);
  free(line_transforms);
  destroy_script_value(initial_value);
  if (error || !*variable)
    destroy_script_value(result);
  else
    assign_script_value(vars, variable, result);
  destroy_string_array(parts);
  return error;
}

char * script_get_initializer_value (const char * initializer, struct script_value * value, struct script_variables * vars, unsigned * new_incbin_size) {
  int init_read = strspn(initializer, ":");
  initializer += init_read;
  struct script_value initial_value;
  if (init_read > 2) return duplicate_string("initializer value can have at most two leading colons");
  if (*initializer == '$') {
    struct script_value * pinit = find_script_variable(vars, initializer + 1);
    if (!pinit) return duplicate_string("variable does not exist");
    initial_value = copy_script_value(*pinit);
  } else if (init_read && !*initializer) {
    initial_value.data = NULL;
    initial_value.type = 3;
    initial_value.value = vars -> values -> value;
  } else {
    initial_value.data = NULL;
    initial_value.type = 3;
    if (!convert_string_to_number(initializer, 1)) return duplicate_string("syntax error");
    initial_value.value = convert_string_to_number(initializer, 0);
  }
  if (!init_read) {
    *value = initial_value;
    return NULL;
  }
  if (!initial_value.type || (initial_value.type > 3)) {
    destroy_script_value(initial_value);
    return duplicate_string("type mismatch");
  } else if (initial_value.value < 0)
    return duplicate_string("negative read amount");
  if (!(vars -> count) || **(vars -> names))
    return duplicate_string("internal state error: $ not found");
  else if (vars -> values -> value < initial_value.value)
    return duplicate_string("not enough data to read");
  unsigned read_length = initial_value.value;
  char * error = script_transform_copy(*(vars -> values), read_length, value);
  if (error) return error;
  error = script_transform_skip(*(vars -> values), read_length, &initial_value);
  if (error) {
    destroy_script_value(*value);
    value -> data = NULL;
    return error;
  }
  assign_script_value(vars, "", initial_value);
  if (init_read > 1) *new_incbin_size = read_length;
  return NULL;
}

char * script_get_transforms_for_line (char ** parts, struct script_variables * vars, struct script_transforms * result) {
  unsigned transform, tlen, tnum;
  int parameter;
  char * error = NULL;
  char * p;
  for (transform = 0; *parts; transform ++, parts ++, result ++) {
    p = strchr(*parts, ':');
    if (!p) {
      error = duplicate_string("syntax error");
      break;
    }
    tlen = (p ++) - *parts;
    for (tnum = 0; transforms[tnum].name; tnum ++) {
      if (strlen(transforms[tnum].name) != tlen) continue;
      if (!strncmp(transforms[tnum].name, *parts, tlen)) break;
    }
    if (!transforms[tnum].name) {
      error = duplicate_string("unknown transform");
      break;
    }
    error = script_get_expression_value(p, vars, &parameter);
    if (error) break;
    result -> transform = transforms[tnum].transform;
    result -> parameter = parameter;
  }
  if (!error) return NULL;
  char * full_error = malloc(24 + strlen(error));
  sprintf(full_error, "transform %u: %s", transform + 1, error);
  free(error);
  return full_error;
}
