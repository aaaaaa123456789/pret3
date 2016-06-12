#include "proto.h"

char * script_transform_length (struct script_value value, int parameter, struct script_value * result) {
  result -> data = NULL;
  result -> type = 3;
  unsigned char target_type;
  switch (parameter) {
    case 0: case 1: case 2: target_type = parameter; break;
    case 4: target_type = 3; break;
    default: return duplicate_string("length type must be 0, 1, 2 or 4");
  }
  if (value.type && (value.type < 4)) return duplicate_string("type mismatch");
  if (target_type) {
    unsigned base_length = value.value;
    if (value.type > 5) base_length <<= (value.type - 5);
    if (base_length & ((1 << (target_type - 1)) - 1)) return duplicate_string("data length is not aligned");
    result -> value = base_length >> (target_type - 1);
    return NULL;
  } else {
    if (value.type != 4) {
      result -> value = value.value;
      return NULL;
    }
    char * converted = parse_buffer(value.data, value.value);
    if (converted) {
      result -> value = strlen(converted);
      free(converted);
    } else
      result -> value = 0;
    return NULL;
  }
}
