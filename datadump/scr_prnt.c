#include "proto.h"

char * generate_script_output_line (const char * line, struct script_variables * vars, char ** error) {
  char * result = malloc(1);
  *result = 0;
  unsigned length = 0;
  const char * next_var;
  unsigned char print_style;
  unsigned var_name_length;
  char * text;
  const char * conversion;
  struct script_value * value;
  while (*line) {
    next_var = strchr(line, '$');
    if (!next_var) {
      concatenate(&result, &length, line, NULL);
      break;
    }
    if (next_var != line) {
      result = realloc(result, length + (next_var - line) + 1);
      memcpy(result + length, line, next_var - line);
      length += next_var - line;
      result[length] = 0;
      line = next_var;
    }
    line ++;
    text = "+-*%";
    conversion = *line ? strchr(text, *line) : NULL;
    print_style = conversion ? conversion - text : -1u;
    if (conversion) line ++;
    var_name_length = strspn(line, VALID_NAME_CHARS);
    text = malloc(var_name_length + 1);
    memcpy(text, line, var_name_length);
    text[var_name_length] = 0;
    line += var_name_length;
    if (*line == '$') line ++;
    value = find_script_variable(vars, text);
    free(text);
    if (!value) {
      free(result);
      *error = duplicate_string("unknown variable");
      return NULL;
    }
    text = print_script_variable_contents(*value, print_style);
    concatenate(&result, &length, text, NULL);
    free(text);
  }
  return result;
}

char * print_script_variable_contents (struct script_value value, unsigned char style) {
  char * result;
  unsigned pos, length;
  char buf[12];
  unsigned short last_surrogate = 0;
  switch (value.type) {
    case 0:
      if (style == 3) {
        result = malloc(value.value + 1);
        unsigned pos;
        char * current = result;
        for (pos = 0; pos < value.value; pos ++) if (pos[(char *) value.data]) *(current ++) = pos[(char *) value.data];
        *current = 0;
        return result;
      }
      result = malloc(1 + (value.value << 1));
      for (pos = 0; pos < value.value; pos ++) sprintf(result + (pos << 1), "%02hhx", pos[(unsigned char *) (value.data)]);
      result[value.value << 1] = 0;
      return result;
    case 1:
      result = malloc(5);
      switch (style) {
        case 0: sprintf(result, "%hhu", (unsigned char) value.value); break;
        case 1: sprintf(result, "%hhd", (signed char) value.value); break;
        case 3: generate_UTF8_character(result, (unsigned char) value.value); break;
        default: sprintf(result, "0x%02hhx", (unsigned char) value.value);
      }
      return result;
    case 2:
      result = malloc(7);
      switch (style) {
        case 0: sprintf(result, "%hu", (unsigned short) value.value); break;
        case 1: sprintf(result, "%hd", (short) value.value); break;
        case 3: generate_UTF8_character(result, (unsigned short) value.value); break;
        default: sprintf(result, "0x%04hx", (unsigned short) value.value);
      }
      return result;
    case 3:
      switch (style) {
        case 0: sprintf(buf, "%u", (unsigned) value.value); break;
        case 1: sprintf(buf, "%d", value.value); break;
        case 2: return generate_pointer_text(value.value);
        case 3: generate_UTF8_character(buf, (unsigned) value.value); break;
        default: sprintf(buf, "0x%08x", (unsigned) value.value);
      }
      result = malloc(12);
      memcpy(result, buf, 12);
      return result;
    case 4:
      result = parse_buffer(value.data, value.value);
      if (!result) result = duplicate_string("");
      return result;
    case 5: {
      unsigned char * data8 = value.data;
      result = malloc(1);
      *result = 0;
      length = 0;
      for (pos = 0; pos < value.value; pos ++) {
        switch (style) {
          case 0: sprintf(buf, "%hhu", data8[pos]); break;
          case 1: sprintf(buf, "%hhd", (signed char) data8[pos]); break;
          case 3: generate_UTF8_character(buf, data8[pos]); break;
          default: sprintf(buf, "0x%02hhx", data8[pos]);
        }
        concatenate(&result, &length, (pos && (style != 3)) ? ", " : "", buf, NULL);
      }
      return result;
    }
    case 6: {
      short * data16 = value.data;
      result = malloc(1);
      *result = 0;
      length = 0;
      for (pos = 0; pos < value.value; pos ++) {
        switch (style) {
          case 0: sprintf(buf, "%hu", (unsigned short) data16[pos]); break;
          case 1: sprintf(buf, "%hd", data16[pos]); break;
          case 3: process_next_codepoint(buf, &last_surrogate, (unsigned short) data16[pos]); break;
          default: sprintf(buf, "0x%04hx", data16[pos]);
        }
        if (*buf) concatenate(&result, &length, (pos && (style != 3)) ? ", " : "", buf, NULL);
      }
      return result;
    }
    case 7: {
      int * data32 = value.data;
      result = malloc(1);
      *result = 0;
      length = 0;
      for (pos = 0; pos < value.value; pos ++) {
        switch (style) {
          case 0: sprintf(buf, "%u", (unsigned) data32[pos]); break;
          case 1: sprintf(buf, "%d", data32[pos]); break;
          case 2: {
            *buf = 0;
            char * pointer_value = generate_pointer_text(data32[pos]);
            concatenate(&result, &length, pos ? ", " : "", pointer_value, NULL);
            free(pointer_value);
          } break;
          case 3: process_next_codepoint(buf, &last_surrogate, data32[pos]); break;
          default: sprintf(buf, "0x%08x", data32[pos]);
        }
        if (*buf) concatenate(&result, &length, (pos && (style != 3)) ? ", " : "", buf, NULL);
      }
      return result;
    }
  }
}

void process_next_codepoint (char * result, unsigned short * last_surrogate, unsigned codepoint) {
  *result = 0;
  if ((codepoint & -0x400u) == 0xd800) {
    *last_surrogate = codepoint;
    return;
  }
  if (*last_surrogate && ((codepoint & -0x400u) == 0xdc00))
    codepoint = 0x10000u + ((*last_surrogate & 0x3ffu) << 10) + (codepoint & 0x3ffu);
  *last_surrogate = 0;
  generate_UTF8_character(result, codepoint);
}
