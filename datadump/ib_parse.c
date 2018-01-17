#include "proto.h"

int parse_incbin (struct incbin * incbin, FILE * out, char ** script_file) {
  char * location = malloc(1);
  *location = 0;
  unsigned p = 0;
  int rv;
  concatenate(&location, &p, repository_path, "/", incbin -> file, NULL);
  FILE * data_file = fopen(location, "rb");
  free(location);
  if (!data_file) {
    printf("err: could not open file for reading\n");
    return 1;
  }
  if (incbin -> offset) {
    rv = fseek(data_file, incbin -> offset, 0);
    if (rv) {
      fclose(data_file);
      printf("err: could not seek on file\n");
      return 1;
    }
  }
  if (!(incbin -> length)) {
    p = get_file_length(data_file);
    if (!p) {
      fclose(data_file);
      printf("err: could not get file length for file\n");
      return 1;
    }
    incbin -> length = p - incbin -> offset;
  }
  void * buffer = malloc(incbin -> length);
  if (!buffer) {
    fclose(data_file);
    printf("err: out of memory\n");
    return 1;
  }
  if (fread(buffer, 1, incbin -> length, data_file) != (incbin -> length)) {
    fclose(data_file);
    printf("err: could not read %u bytes from file\n", incbin -> length);
    return 1;
  }
  fclose(data_file);
  if (*script_file)
    rv = run_script_auto(incbin, buffer, *script_file, out);
  else
    rv = handle_incbin_data(incbin, buffer, out, script_file);
  free(buffer);
  return rv;
}
