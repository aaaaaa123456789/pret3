#include "proto.h"

char * read_line (FILE * file) {
  int character;
  char * result = NULL;
  unsigned length = 0;
  while (1) {
    character = getc(file);
    if (character == '\r') continue;
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

void * read_file_buffer (FILE * file, unsigned offset, unsigned length, const char ** error) {
  *error = NULL;
  if (!length) return NULL;
  if (fseek(file, offset, 0)) {
    *error = "could not seek to offset";
    return NULL;
  }
  void * result = malloc(length);
  int rv = fread(result, 1, length, file);
  if (rv != length) {
    *error = rv ? "premature end of file" : "could not read file";
    free(result);
    return NULL;
  }
  return result;
}

unsigned long long read_file_value (FILE * file, unsigned offset, unsigned char size, const char ** error) {
  *error = NULL;
  if (!size) return 0;
  if (size > sizeof(unsigned long long)) {
    *error = "invalid value size";
    return 0;
  }
  unsigned char * buffer = read_file_buffer(file, offset, size, error);
  if (*error) return 0;
  unsigned result = 0;
  unsigned char pos;
  for (pos = 0; pos < size; pos ++) result |= ((unsigned) buffer[pos]) << (pos << 3);
  free(buffer);
  return result;
}

char * read_file_string (FILE * file, unsigned offset, const char ** error) {
  if (fseek(file, offset, 0)) {
    *error = "could not seek to offset";
    return NULL;
  }
  char * result = NULL;
  unsigned length = 0;
  int c;
  do {
    c = getc(file);
    if (c == EOF) {
      *error = length ? "premature end of file" : "could not read file";
      free(result);
      return NULL;
    }
    result = realloc(result, length + 1);
    result[length ++] = c;
  } while (c);
  *error = NULL;
  return result;
}

void transfer_temporary_to_file (FILE * file) {
  // note that the temporary file is always opened in w+b mode by tmpfile(), so seeking and positioning will not be affected by \n to \r\n conversions
  if (!global_temporary_file) return;
  int rv = fseek(global_temporary_file, 0, 2);
  long file_size = -1;
  if (rv >= 0) file_size = ftell(global_temporary_file);
  if (file_size != -1) rv = fseek(global_temporary_file, 0, 0);
  if ((file_size == -1) || (rv < 0)) {
    fputs("error: could not seek in temporary file\n", stderr);
    fclose(global_temporary_file); // try to delete it
    exit(2);
  }
  char * buffer = malloc(65536);
  clearerr(global_temporary_file);
  clearerr(file);
  while (!(feof(global_temporary_file) || ferror(global_temporary_file))) {
    rv = fread(buffer, 1, 65536, global_temporary_file);
    if (rv < 0) break;
    if (fwrite(buffer, 1, rv, file) != rv) break;
  }
  if (ferror(global_temporary_file) || ferror(file)) {
    fclose(global_temporary_file);
    fputs("error: could not write to file\n", stderr);
    exit(2);
  }
  free(buffer);
  fclose(global_temporary_file);
  global_temporary_file = NULL;
}
