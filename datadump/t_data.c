#include "proto.h"

char * script_transform_skip (struct script_value value, int parameter, struct script_value * result) {
  result -> type = value.type;
  result -> data = NULL;
  result -> value = value.value - parameter;
  if (parameter < 0)
    return duplicate_string("negative skip count");
  else if (value.type && (value.type < 4))
    return duplicate_string("type mismatch");
  else if (parameter > value.value)
    return duplicate_string("skip count is greater than length");
  unsigned length_factor;
  switch (value.type) {
    case 0: case 4: case 5:
      length_factor = 1;
      break;
    case 6:
      length_factor = sizeof(short);
      break;
    case 7:
      length_factor = sizeof(int);
      break;
    default:
      return duplicate_string("internal error");
  }
  result -> data = malloc(length_factor * result -> value);
  memcpy(result -> data, ((char *) (value.data)) + length_factor * parameter, length_factor * result -> value);
  return NULL;
}

char * script_transform_item (struct script_value value, int parameter, struct script_value * result) {
  result -> type = value.type - 4;
  result -> data = NULL;
  if (parameter < 0)
    return duplicate_string("negative index");
  else if (value.type < 5)
    return duplicate_string("type mismatch");
  else if (parameter >= value.value)
    return duplicate_string("index out of bounds");
  switch (value.type) {
    case 5:
      result -> value = parameter[(signed char *) (value.data)];
      break;
    case 6:
      result -> value = parameter[(short *) (value.data)];
      break;
    case 7:
      result -> value = parameter[(int *) (value.data)];
      break;
    default:
      return duplicate_string("internal error");
  }
  return NULL;
}

char * script_transform_copy (struct script_value value, int parameter, struct script_value * result) {
  result -> type = value.type;
  result -> data = NULL;
  result -> value = parameter;
  if (parameter < 0)
    return duplicate_string("negative length");
  else if (value.type && (value.type < 4))
    return duplicate_string("type mismatch");
  else if (parameter > value.value)
    return duplicate_string("length is greater than data size");
  unsigned length_factor;
  switch (value.type) {
    case 0: case 4: case 5:
      length_factor = 1;
      break;
    case 6:
      length_factor = sizeof(short);
      break;
    case 7:
      length_factor = sizeof(int);
      break;
    default:
      return duplicate_string("internal error");
  }
  result -> data = malloc(length_factor * parameter);
  memcpy(result -> data, value.data, length_factor * parameter);
  return NULL;
}

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
