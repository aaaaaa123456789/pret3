#include "proto.h"

struct incbin * get_incbin_data (const char * line) {
  const char * pos = strchr(strstr(line, ".incbin"), '"');
  if (!pos) {
    printf("err: no filename specified\n");
    return NULL;
  }
  pos ++;
  const char * end = strchr(pos, '"');
  if (!end) {
    printf("err: invalid filename specification\n");
    return NULL;
  }
  struct incbin * result = malloc(end - pos + 1 + sizeof(struct incbin));
  result -> offset = result -> length = 0;
  memcpy(result -> file, pos, end - pos);
  result -> file[end - pos] = 0;
  pos = strchr(end, ',');
  if (!pos) return result;
  pos ++;
  end = strchr(pos, ',');
  result -> offset = get_value_from_string(pos, end ? (end - pos) : strlen(pos));
  if (result -> offset == ((unsigned) -1)) {
    printf("err: invalid offset\n");
    free(result);
    return NULL;
  }
  if (!end) return result;
  end ++;
  result -> length = get_value_from_string(end, strlen(end));
  if (!(result -> length) || (result -> length == ((unsigned) -1))) {
    printf("err: invalid length\n");
    free(result);
    return NULL;
  }
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
  printf(">>>> %s\n", incbin);
  fprintf(out, "%s\n", incbin);
  free(incbin);
}
