#include "proto.h"

char * duplicate_string (const char * string) {
  return strcpy(malloc(strlen(string) + 1), string);
}

char * generate_string (const char * fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char * result = generate_string_from_varargs(fmt, ap);
  va_end(ap);
  return result;
}

char * generate_string_from_varargs (const char * fmt, va_list varargs) {
  char testbuf[2];
  va_list temp;
  va_copy(temp, varargs);
  int rv = vsnprintf(testbuf, 1, fmt, temp);
  va_end(temp);
  if (rv < 0) return NULL;
  char * result = malloc(rv + 1);
  vsnprintf(result, rv + 1, fmt, varargs);
  return result;
}
