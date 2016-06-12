#include "proto.h"

char * script_transform_byte_count (struct script_value value, int parameter, struct script_value * result, int count_equals) {
  result -> type = 3;
  result -> data = NULL;
  if ((parameter < -128) || (parameter > 255))
    return duplicate_string("parameter is not a byte");
  else if (value.type)
    return duplicate_string("type mismatch");
  unsigned char compare = parameter;
  const unsigned char * data = value.data;
  unsigned pos;
  for (pos = 0; pos < value.value; pos ++) if ((data[pos] == compare) == !count_equals) break;
  result -> value = pos;
  return NULL;
}

char * script_transform_byteswhile (struct script_value value, int parameter, struct script_value * result) {
  return script_transform_byte_count(value, parameter, result, 1);
}

char * script_transform_bytesuntil (struct script_value value, int parameter, struct script_value * result) {
  return script_transform_byte_count(value, parameter, result, 0);
}
