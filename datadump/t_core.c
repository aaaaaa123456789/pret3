#include "proto.h"

char * execute_transforms (struct script_value value, struct script_transforms * transforms, unsigned count, struct script_value * result) {
  struct script_value current;
  *result = copy_script_value(value);
  unsigned p;
  char * error = NULL;
  for (p = 0; (!error) && (p < count); p ++) {
    current = *result;
    error = transforms[p].transform(current, transforms[p].parameter, result);
    destroy_script_value(current);
  }
  if (!error) return NULL;
  char * final_error = malloc(32);
  unsigned length = sprintf(final_error, "transform %u: ", p);
  concatenate(&final_error, &length, error, NULL);
  free(error);
  return final_error;
}

int enforce_value_size (int value, unsigned char size) {
  switch (size) {
    case 1: return (signed char) value;
    case 2: return (short) value;
    default: return (int) value;
  }
}

char * script_transform_basic (struct script_value value, int parameter, struct script_value * result, int (* operation) (int, int, char **)) {
  unsigned p;
  char * error = NULL;
  result -> type = value.type;
  result -> data = NULL;
  switch (value.type) {
    case 1: case 2: case 3:
      result -> value = enforce_value_size(operation(value.value, parameter, &error), value.type);
      return error;
    case 5: {
      signed char * data8 = malloc(value.value);
      memcpy(data8, value.data, value.value);
      for (p = 0; p < value.value; p ++) {
        data8[p] = operation(data8[p], parameter, &error);
        if (!error) continue;
        free(data8);
        return error;
      }
      result -> value = value.value;
      result -> data = data8;
      return NULL;
    }
    case 6: {
      short * data16 = malloc(sizeof(short) * value.value);
      memcpy(data16, value.data, sizeof(short) * value.value);
      for (p = 0; p < value.value; p ++) {
        data16[p] = operation(data16[p], parameter, &error);
        if (!error) continue;
        free(data16);
        return error;
      }
      result -> value = value.value;
      result -> data = data16;
      return NULL;
    }
    case 7: {
      int * data32 = malloc(sizeof(int) * value.value);
      memcpy(data32, value.data, sizeof(int) * value.value);
      for (p = 0; p < value.value; p ++) {
        data32[p] = operation(data32[p], parameter, &error);
        if (!error) continue;
        free(data32);
        return error;
      }
      result -> value = value.value;
      result -> data = data32;
      return NULL;
    }
    default:
      return duplicate_string("type mismatch");
  }
}
