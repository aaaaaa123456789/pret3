#include "proto.h"

struct script_variables * init_script_variables (struct incbin * incbin, const unsigned char * data, unsigned position) {
  struct script_variables * vars = malloc(sizeof(struct script_variables));
  vars -> count = 1;
  vars -> names = malloc(sizeof(char *));
  *(vars -> names) = malloc(1);
  **(vars -> names) = 0;
  vars -> values = calloc(1, sizeof(struct script_value));
  *(vars -> values) = (struct script_value) {
    .type = 0,
    .value = (position > incbin -> length) ? 0 : (incbin -> length - position)
  };
  if (vars -> values -> value) {
    vars -> values -> data = malloc(vars -> values -> value);
    memcpy(vars -> values -> data, data + position, vars -> values -> value);
  }
  return vars;
}

struct script_value * find_script_variable (struct script_variables * vars, const char * name) {
  unsigned p;
  for (p = 0; p < vars -> count; p ++) if (!strcmp(name, p[vars -> names])) return vars -> values + p;
  return NULL;
}

void destroy_script_value (struct script_value val) {
  if (val.data) free(val.data);
}

void assign_script_value (struct script_variables * vars, const char * name, struct script_value value) {
  struct script_value * ptr = find_script_variable(vars, name);
  if (ptr) {
    destroy_script_value(*ptr);
    *ptr = value;
    return;
  }
  vars -> count ++;
  vars -> names = realloc(vars -> names, sizeof(char *) * vars -> count);
  vars -> names[vars -> count - 1] = malloc(1 + strlen(name));
  strcpy(vars -> names[vars -> count - 1], name);
  vars -> values = realloc(vars -> values, sizeof(struct script_value) * vars -> count);
  vars -> values[vars -> count - 1] = value;
}

void destroy_script_variables (struct script_variables * vars) {
  unsigned p;
  for (p = 0; p < vars -> count; p ++) {
    free(p[vars -> names]);
    destroy_script_value(p[vars -> values]);
  }
  free(vars -> names);
  free(vars -> values);
  free(vars);
}

struct script_value copy_script_value (struct script_value value) {
  struct script_value result;
  result.type = value.type;
  result.value = value.value;
  unsigned copy_amount = 1;
  switch (result.type) {
    case 7: copy_amount <<= 1;
    case 6: copy_amount <<= 1;
    case 0: case 4: case 5: copy_amount *= result.value; break;
    default: copy_amount = 0;
  }
  if (copy_amount) {
    result.data = malloc(copy_amount);
    memcpy(result.data, value.data, copy_amount);
  } else
    result.data = NULL;
  return result;
}

int validate_variable_name (const char * name) {
  return strspn(name, VALID_NAME_CHARS) == strlen(name);
}
