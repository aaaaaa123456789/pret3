#include "proto.h"

char * script_transform_append (struct script_value value, int parameter, struct script_value * result) {
  result -> type = value.type;
  result -> value = value.value + 1;
  result -> data = NULL;
  switch (value.type) {
    case 5: {
      signed char * data8 = malloc(result -> value);
      memcpy(data8, value.data, value.value);
      data8[value.value] = parameter;
      result -> data = data8;
    } break;
    case 6: {
      short * data16 = malloc(result -> value * sizeof(short));
      memcpy(data16, value.data, value.value * sizeof(short));
      data16[value.value] = parameter;
      result -> data = data16;
    } break;
    case 7: {
      int * data32 = malloc(result -> value * sizeof(int));
      memcpy(data32, value.data, value.value * sizeof(int));
      data32[value.value] = parameter;
      result -> data = data32;
    } break;
    default:
      return duplicate_string("type mismatch");
  }
  return NULL;
}

char * script_transform_prepend (struct script_value value, int parameter, struct script_value * result) {
  result -> type = value.type;
  result -> value = value.value + 1;
  result -> data = NULL;
  switch (value.type) {
    case 5: {
      signed char * data8 = malloc(result -> value);
      memcpy(data8 + 1, value.data, value.value);
      *data8 = parameter;
      result -> data = data8;
    } break;
    case 6: {
      short * data16 = malloc(result -> value * sizeof(short));
      memcpy(data16 + 1, value.data, value.value * sizeof(short));
      *data16 = parameter;
      result -> data = data16;
    } break;
    case 7: {
      int * data32 = malloc(result -> value * sizeof(int));
      memcpy(data32 + 1, value.data, value.value * sizeof(int));
      *data32 = parameter;
      result -> data = data32;
    } break;
    default:
      return duplicate_string("type mismatch");
  }
  return NULL;
}

char * script_transform_rotate (struct script_value value, int parameter, struct script_value * result) {
  result -> type = value.type;
  result -> value = value.value;
  result -> data = NULL;
  if (value.type && (value.type < 5)) return duplicate_string("type mismatch");
  parameter %= value.value;
  if (parameter < 0) parameter += value.value;
  unsigned size_factor;
  switch (value.type) {
    case 0: case 5: size_factor = 1; break;
    case 6: size_factor = sizeof(short); break;
    case 7: size_factor = sizeof(int);
  }
  char * data = malloc(size_factor * value.value);
  memcpy(data, ((char *) (value.data)) + size_factor * parameter, size_factor * (value.value - parameter));
  memcpy(data + (value.value - parameter) * size_factor, value.data, parameter * size_factor);
  result -> data = data;
  return NULL;
}

char * script_transform_rotateback (struct script_value value, int parameter, struct script_value * result) {
  return script_transform_rotate(value, -parameter, result);
}
