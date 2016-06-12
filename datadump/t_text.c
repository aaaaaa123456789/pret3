#include "proto.h"

char * script_transform_text (struct script_value value, int parameter, struct script_value * result) {
  result -> type = 4;
  result -> value = parameter;
  result -> data = NULL;
  if (parameter < 0)
    return duplicate_string("negative length");
  else if (value.type)
    return duplicate_string("type mismatch");
  else if (parameter > value.value)
    return duplicate_string("length is greater than data size");
  void * buffer = malloc(parameter);
  memcpy(buffer, value.data, parameter);
  char * converted = parse_buffer(buffer, parameter);
  free(converted);
  if (!converted) {
    free(buffer);
    return duplicate_string("data does not contain text");
  }
  result -> data = buffer;
  return NULL;
}
