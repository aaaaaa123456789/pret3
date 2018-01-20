#include "proto.h"

int parse_incbin (struct incbin * incbin, FILE * out, char ** script_file) {
  int rv;
  void * buffer = get_incbin_contents(incbin);
  if (!buffer) return 1;
  if (*script_file)
    rv = run_script_auto(incbin, buffer, *script_file, out);
  else
    rv = handle_incbin_data(incbin, buffer, out, script_file);
  free(buffer);
  return rv;
}

void * get_incbin_contents (struct incbin * incbin) {
  char * location = malloc(1);
  *location = 0;
  unsigned p = 0;
  int rv;
  concatenate(&location, &p, repository_path, "/", incbin -> file, NULL);
  FILE * data_file = fopen(location, "rb");
  free(location);
  if (!data_file) {
    puts("err: could not open file for reading");
    return NULL;
  }
  if (incbin -> offset) {
    rv = fseek(data_file, incbin -> offset, 0);
    if (rv) {
      fclose(data_file);
      puts("err: could not seek on file");
      return NULL;
    }
  }
  if (!(incbin -> length)) {
    p = get_file_length(data_file);
    if (!p) {
      fclose(data_file);
      puts("err: could not get file length for file");
      return NULL;
    }
    incbin -> length = p - incbin -> offset;
  }
  void * buffer = malloc(incbin -> length);
  if (!buffer) {
    fclose(data_file);
    puts("err: out of memory");
    return NULL;
  }
  if (fread(buffer, 1, incbin -> length, data_file) != (incbin -> length)) {
    fclose(data_file);
    free(buffer);
    printf("err: could not read %u bytes from file\n", incbin -> length);
    return NULL;
  }
  fclose(data_file);
  return buffer;
}
