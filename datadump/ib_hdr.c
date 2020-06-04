#include "proto.h"

struct incbin * get_incbin_data (const char * line) {
  const char * pos = strchr(strstr(line, ".incbin"), '"');
  if (!pos) {
    puts("err: no filename specified");
    return NULL;
  }
  pos ++;
  const char * end = strchr(pos, '"');
  if (!end) {
    puts("err: invalid filename specification");
    return NULL;
  }
  struct incbin * result = malloc(end - pos + 1 + sizeof(struct incbin));
  result -> offset = result -> length = 0;
  memcpy(result -> file, pos, end - pos);
  result -> file[end - pos] = 0;
  pos = strchr(end, ',');
  if (!pos) return result;
  char * trimmed_line = trim_string(pos + 1);
  char * comment = (char *) find_first_unquoted(line, '@');
  if (comment) *comment = 0;
  end = strchr(trimmed_line, ',');
  result -> offset = get_value_from_string(pos, end ? (end - trimmed_line) : strlen(trimmed_line));
  if (result -> offset == -1u) {
    puts("err: invalid offset");
    free(result);
    result = NULL;
    goto done;
  }
  if (!end) goto done;
  end ++;
  result -> length = get_value_from_string(end, strlen(end));
  if (!(result -> length) || (result -> length == -1u)) {
    puts("err: invalid length");
    free(result);
    result = NULL;
  }
  done:
  free(trimmed_line);
  return result;
}

int is_incbin (const char * line) {
  const char * pos = strstr(line, ".incbin");
  if (!pos) return 0;
  const char * comment = strchr(line, '@');
  if (!comment) return 1;
  return pos < comment;
}

void write_header_comment (struct incbin * incbin, FILE * out) {
  char * header = generate_incbin(incbin -> file, incbin -> offset, incbin -> length);
  printf(">>>> @ replacing: %s\n", header);
  fprintf(out, "@ replacing %s\n", header);
  free(header);
}

char * generate_incbin (const char * file, unsigned offset, unsigned length) {
  char * result = malloc(1);
  *result = 0;
  unsigned result_length = 0;
  char value[16];
  concatenate(&result, &result_length, ".incbin \"", file, "\", ", NULL);
  sprintf(value, "0x%08x", offset);
  concatenate(&result, &result_length, value, ", ", NULL);
  sprintf(value, "0x%x", length);
  concatenate(&result, &result_length, value, NULL);
  return result;
}

void write_incbin_for_segment (const char * file, unsigned offset, unsigned length, FILE * out) {
  if (!length) return;
  char * incbin = generate_incbin(file, offset, length);
  char * indent;
  generate_initial_indented_line(&indent, NULL);
  printf(">>>> %s%s\n", indent, incbin);
  fprintf(out, "%s%s\n", indent, incbin);
  free(incbin);
  free(indent);
}
