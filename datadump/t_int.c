#include "proto.h"

char * script_transform_int (struct script_value value, int parameter, struct script_value * result) {
  result -> data = NULL;
  unsigned char target_type;
  unsigned n;
  int * extracted_values;
  switch (parameter) {
    case 1: case 2: target_type = parameter; break;
    case 4: target_type = 3; break;
    default: return duplicate_string("integer size must be 1, 2 or 4");
  }
  switch (value.type) {
    case 0:
      if (value.value < (1 << (target_type - 1))) return duplicate_string("not enough data to extract integer");
      result -> type = target_type;
      switch (target_type) {
        case 1: result -> value = *((const signed char *) (value.data)); break;
        case 2: result -> value = read_16(value.data); break;
        case 3: result -> value = read_32(value.data); break;
        default: return duplicate_string("internal error");
      }
      return NULL;
    case 1: case 2: case 3:
      result -> type = target_type;
      result -> value = enforce_value_size(value.value, target_type);
      return NULL;
    case 4:
      return duplicate_string("type mismatch");
    case 5: {
      signed char * data8 = value.data;
      extracted_values = malloc(sizeof(int) * value.value);
      for (n = 0; n < value.value; n ++) extracted_values[n] = data8[n];
    } break;
    case 6: {
      short * data16 = value.data;
      extracted_values = malloc(sizeof(int) * value.value);
      for (n = 0; n < value.value; n ++) extracted_values[n] = data16[n];
    } break;
    case 7:
      extracted_values = malloc(sizeof(int) * value.value);
      memcpy(extracted_values, value.data, sizeof(int) * value.value);
      break;
    default:
      return duplicate_string("internal error");
  }
  result -> type = 4 + target_type;
  result -> value = value.value;
  switch (target_type) {
    case 1: {
      signed char * data8 = malloc(value.value);
      for (n = 0; n < value.value; n ++) data8[n] = extracted_values[n];
      free(extracted_values);
      result -> data = data8;
    } break;
    case 2: {
      short * data16 = malloc(value.value * sizeof(short));
      for (n = 0; n < value.value; n ++) data16[n] = extracted_values[n];
      free(extracted_values);
      result -> data = data16;
    } break;
    case 3:
      result -> data = extracted_values;
      break;
    default:
      free(extracted_values);
      return duplicate_string("internal error");
  }
  return NULL;
}
