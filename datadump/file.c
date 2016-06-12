#include "proto.h"

char * read_line (FILE * file) {
  int character;
  char * result = NULL;
  unsigned length = 0;
  while (1) {
    character = getc(file);
    if ((character == EOF) || (character == '\n')) break;
    result = realloc(result, length + 1);
    result[length ++] = character;
  }
  result = realloc(result, length + 1);
  result[length] = 0;
  return result;
}

unsigned get_file_length (FILE * file) {
  long pos, length;
  pos = ftell(file);
  if (pos < 0) return 0;
  if (fseek(file, 0, 2)) return 0;;
  length = ftell(file);
  if (fseek(file, pos, 0)) return 0;
  if (length == -1) return 0;
  return length;
}

char ** read_file_by_lines (FILE * file) {
  char ** lines = NULL;
  unsigned line_count = 0;
  while (!feof(file)) {
    lines = realloc(lines, sizeof(char *) * (line_count + 1));
    lines[line_count ++] = read_line(file);
  }
  lines = realloc(lines, sizeof(char *) * (line_count + 1));
  lines[line_count] = NULL;
  return lines;
}
