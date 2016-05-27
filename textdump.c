#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void concatenate(char **, unsigned *, ...);
char * parse_buffer(const unsigned char *, unsigned);
char * read_line(FILE *);
int main(int, char **);
void dump_incbins(FILE *, FILE *, const char *);
int is_incbin(const char *);
struct incbin * get_incbin_data(const char *);
unsigned get_value_from_string(const char *, unsigned);
unsigned char get_command(const char *, unsigned);
void print_command_help(unsigned);
int parse_incbin(struct incbin *, const char *, FILE *);
unsigned get_file_length(FILE *);
int handle_incbin_data(struct incbin *, const unsigned char *, FILE *);
void write_header_comment(struct incbin *, FILE *);
void output_binary_data(const unsigned char *, unsigned, unsigned char, FILE *);
char * generate_incbin(const char *, unsigned, unsigned);
int handle_incbin_text(struct incbin *, const unsigned char *, FILE *);
void write_incbin_for_segment(const char *, unsigned, unsigned, FILE *);

struct incbin {
  unsigned offset;
  unsigned length;
  char file[];
};

struct command {
  const char * name;
  const char * alias;
  const char * help_text;
};

#define COMMAND_LENGTH "10"
#define HELP_TEXT_NEWLINE "\n             "

const char * text_table[] = {
  //        0 / 8           1 / 9           2 / A           3 / B           4 / C           5 / D           6 / E           7 / F
  /* 00 */  " ",            "\xc3\x80",     "\xc3\x81",     "\xc3\x82",     "\xc3\x87",     "\xc3\x88",     "\xc3\x89",     "\xc3\x8a",
  /* 08 */  "\xc3\x8b",     "\xc3\x8c",     "\xe3\x81\x93", "\xc3\x8e",     "\xc3\x8f",     "\xc3\x92",     "\xc3\x93",     "\xc3\x94",
  /* 10 */  "\xc5\x92",     "\xc3\x99",     "\xc3\x9a",     "\xc3\x9b",     "\xc3\x91",     "\xc3\x9f",     "\xc3\xa0",     "\xc3\xa1",
  /* 18 */  "\xe3\x81\xad", "\xc3\xa7",     "\xc3\xa8",     "\xc3\xa9",     "\xc3\xaa",     "\xc3\xab",     "\xc3\xac",     "\xe3\x81\xbe",
  /* 20 */  "\xc3\xae",     "\xc3\xaf",     "\xc3\xb2",     "\xc3\xb3",     "\xc3\xb4",     "\xc5\x93",     "\xc3\xb9",     "\xc3\xba",
  /* 28 */  "\xc3\xbb",     "\xc3\xb1",     "\xc2\xba",     "\xc2\xaa",     "<er>",         "&",            "+",            "\xe3\x81\x82",
  /* 30 */  "\xe3\x81\x83", "\xe3\x81\x85", "\xe3\x81\x87", "\xe3\x81\x89", "<Lv>",         "=",            "\xe3\x82\x87", "\xe3\x81\x8c",
  /* 38 */  "\xe3\x81\x8e", "\xe3\x81\x90", "\xe3\x81\x92", "\xe3\x81\x94", "\xe3\x81\x96", "\xe3\x81\x98", "\xe3\x81\x9a", "\xe3\x81\x9c",
  /* 40 */  "\xe3\x81\x9e", "\xe3\x81\xa0", "\xe3\x81\xa2", "\xe3\x81\xa5", "\xe3\x81\xa7", "\xe3\x81\xa9", "\xe3\x81\xb0", "\xe3\x81\xb3",
  /* 48 */  "\xe3\x81\xb6", "\xe3\x81\xb9", "\xe3\x81\xbc", "\xe3\x81\xb1", "\xe3\x81\xb4", "\xe3\x81\xb7", "\xe3\x81\xba", "\xe3\x81\xbd",
  /* 50 */  "\xe3\x81\xa3", "\xc2\xbf",     "\xc2\xa1",     "<PK>",         "<MN>",         "<PO>",         "<Ke>",         "<BL>",
  /* 58 */  "<OC>",         "<K>",          "\xc3\x8d",     "%",            "(",            ")",            "\xe3\x82\xbb", "\xe3\x82\xbd",
  /* 60 */  "\xe3\x82\xbf", "\xe3\x83\x81", "\xe3\x83\x84", "\xe3\x83\x86", "\xe3\x83\x88", "\xe3\x83\x8a", "\xe3\x83\x8b", "\xe3\x83\x8c",
  /* 68 */  "\xc3\xa2",     "\xe3\x83\x8e", "\xe3\x83\x8f", "\xe3\x83\x92", "\xe3\x83\x95", "\xe3\x83\x98", "\xe3\x83\x9b", "\xc3\xad",
  /* 70 */  "\xe3\x83\x9f", "\xe3\x83\xa0", "\xe3\x83\xa1", "\xe3\x83\xa2", "\xe3\x83\xa4", "\xe3\x83\xa6", "\xe3\x83\xa8", "\xe3\x83\xa9",
  /* 78 */  "\xe3\x83\xaa", "\xe2\xac\x86", "\xe2\xac\x87", "\xe2\xac\x85", "\xe2\x9e\xa1", "\xe3\x83\xb2", "\xe3\x83\xb3", "\xe3\x82\xa1",
  /* 80 */  "\xe3\x82\xa3", "\xe3\x82\xa5", "\xe3\x82\xa7", "\xe3\x82\xa9", "\xe3\x83\xa3", "\xe3\x83\xa5", "\xe3\x83\xa7", "\xe3\x82\xac",
  /* 88 */  "\xe3\x82\xae", "\xe3\x82\xb0", "\xe3\x82\xb2", "\xe3\x82\xb4", "\xe3\x82\xb6", "\xe3\x82\xb8", "\xe3\x82\xba", "\xe3\x82\xbc",
  /* 90 */  "\xe3\x82\xbe", "\xe3\x83\x80", "\xe3\x83\x82", "\xe3\x83\x85", "\xe3\x83\x87", "\xe3\x83\x89", "\xe3\x83\x90", "\xe3\x83\x93",
  /* 98 */  "\xe3\x83\x96", "\xe3\x83\x99", "\xe3\x83\x9c", "\xe3\x83\x91", "\xe3\x83\x94", "\xe3\x83\x97", "\xe3\x83\x9a", "\xe3\x83\x9d",
  /* A0 */  "\xe3\x83\x83", "0",            "1",            "2",            "3",            "4",            "5",            "6",
  /* A8 */  "7",            "8",            "9",            "!",            "?",            ".",            "-",            "\xe3\x83\xbb",
  /* B0 */  "\xe2\x80\xa6", "\xe2\x80\x9c", "\xe2\x80\x9d", "\xe2\x80\x98", "\xe2\x80\x99", "\xe2\x99\x82", "\xe2\x99\x80", "<yen>",
  /* B8 */  ",",            "\xc3\x97",     "/",            "A",            "B",            "C",            "D",            "E",
  /* C0 */  "F",            "G",            "H",            "I",            "J",            "K",            "L",            "M",
  /* C8 */  "N",            "O",            "P",            "Q",            "R",            "S",            "T",            "U",
  /* D0 */  "V",            "W",            "X",            "Y",            "Z",            "a",            "b",            "c",
  /* D8 */  "d",            "e",            "f",            "g",            "h",            "i",            "j",            "k",
  /* E0 */  "l",            "m",            "n",            "o",            "p",            "q",            "r",            "s",
  /* E8 */  "t",            "u",            "v",            "w",            "x",            "y",            "z",            "\xe2\x96\xb6",
  /* F0 */  ":",            "\xc3\x84",     "\xc3\x96",     "\xc3\x9c",     "\xc3\xa4",     "\xc3\xb6",     "\xc3\xbc",     "\xe2\xac\x86",
  /* F8 */  "\xe2\xac\x87", "\xe2\xac\x85", "\\l",          "\\p",          "",             "",             "\\n",          "$"
};

const char * colors[] = {"TRANSPARENT", "DARK_GREY", "RED", "GREEN", "BLUE", "YELLOW", "CYAN", "MAGENTA",
                         "LIGHT_GRAY", "BLACK", "BLACK2", "SILVER", "WHITE", "SKY_BLUE", "LIGHT_BLUE", "WHITE2"};

const char * buffers[] = {NULL, "PLAYER", "STR_VAR_1", "STR_VAR_2", "STR_VAR_3", NULL, "RIVAL", "VERSION",
                          "EVIL_TEAM", "GOOD_TEAM", "EVIL_LEADER", "GOOD_LEADER", "EVIL_LEGENDARY", "GOOD_LEGENDARY", NULL, NULL};

struct command commands[] = {
  {"abort",        NULL,        "aborts all processing and exits"},
  {"done",         "!",         "finishes normally without parsing any more files"},
  {"skip",         "s",         "skips the current file entirely and outputs it" HELP_TEXT_NEWLINE
                                "as an .incbin"},
  {"data8",        NULL,        "outputs the current file as a series of 8-bit values"},
  {"data16",       NULL,        "outputs the current file as a series of 16-bit values"},
  {"data32",       NULL,        "outputs the current file as a series of 32-bit values"},
  {"textscan",     "t",         "scans the current file for text strings to output"},
  {"yes",          "y",         "preserves the current string as a .string"},
  {"yestoall",     "y!",        "preserves the current string and all subsequent" HELP_TEXT_NEWLINE
                                "ones in the file as .string values"},
  {"no",           "n",         "keeps the current string as part of an .incbin"},
  {"notoall",      "n!",        "keeps the current string and all subsequent ones" HELP_TEXT_NEWLINE
                                "in the file as part of an .incbin"},
  {NULL,           NULL,        NULL}
};

void concatenate (char ** string, unsigned * length, ...) {
  va_list ap;
  va_start(ap, length);
  const char * next;
  unsigned nextlen;
  while (next = va_arg(ap, const char *)) {
    nextlen = strlen(next);
    *string = realloc(*string, *length + nextlen + 1);
    memcpy(*string + *length, next, nextlen);
    *length += nextlen;
  }
  va_end(ap);
  (*string)[*length] = 0;
}

char * parse_buffer (const unsigned char * buffer, unsigned buffer_length) {
  unsigned char shift_state = 0;
  unsigned char prev_value;
  char * result = malloc(1);
  *result = 0;
  unsigned length = 0;
  unsigned pos;
  char temp[8];
  for (pos = 0; pos < buffer_length; pos ++)
    switch (shift_state) {
      case 0:
        if ((buffer[pos] & 0xfe) == 0xfc)
          shift_state = buffer[pos];
        else
          concatenate(&result, &length, text_table[buffer[pos]], NULL);
        break;
      case 1:
      case 2:
      case 3:
        shift_state --;
        sprintf(temp, "%hhu", buffer[pos]);
        concatenate(&result, &length, "{", shift_state[(const char * []) {"COLOR", "HIGHLIGHT", "SHADOW"}], " ",
                    (buffer[pos] > 15) ? temp : colors[buffer[pos]], "}", NULL);
        shift_state = 0;
        break;
      case 4:
        prev_value = buffer[pos];
        shift_state = 5;
        break;
      case 5:
        concatenate(&result, &length, "{COLOR_HIGHLIGHT_SHADOW ", (prev_value > 15) ? temp : colors[prev_value], " ",
                    (buffer[pos] > 15) ? temp : colors[buffer[pos]], "}", NULL);
        shift_state = 0;
        break;
      case 6:
        concatenate(&result, &length, buffer[pos] ? "{BIG}" : "{SMALL}", NULL);
        shift_state = 0;
        break;
      case 8:
        sprintf(temp, "%hhu", buffer[pos]);
        concatenate(&result, &length, "{PAUSE ", temp, "}", NULL);
        shift_state = 0;
        break;
      case 12:
        if (buffer[pos] < 250)
          concatenate(&result, &length, text_table[buffer[pos]], NULL);
        else if (buffer[pos] == 250)
          concatenate(&result, &length, "\xe2\x9e\xa1", NULL);
        else if (buffer[pos] == 251)
          concatenate(&result, &length, "+", NULL);
        shift_state = 0;
        break;
      case 13:
        sprintf(temp, "%hhu", buffer[pos]);
        concatenate(&result, &length, "{SHIFT_TEXT ", temp, "}", NULL);
        shift_state = 0;
        break;
      case 16:
        prev_value = buffer[pos];
        shift_state = 17;
        break;
      case 17:
        sprintf(temp, "%02hhX%02hhX", buffer[pos], prev_value);
        concatenate(&result, &length, "{PLAY_MUSIC 0x", temp, "}", NULL);
        shift_state = 0;
        break;
      case 0xfc:
        switch (buffer[pos]) {
          case 1: case 2: case 3: case 4: case 6: case 8: case 12: case 13: case 16:
            shift_state = buffer[pos];
            break;
          case 0:
            concatenate(&result, &length, "{NAME_END}", NULL);
          case 9:
            concatenate(&result, &length, "{PAUSE_UNTIL_PRESS}", NULL);
            break;
          case 21: case 22: case 23: case 24:
            concatenate(&result, &length, ((const char * []) {"{ENLARGE}", "{SET_TO_DEFAULT_SIZE}", "{PAUSE_MUSIC}", "{RESUME_MUSIC}"})[buffer[pos] - 21], NULL);
            break;
          default:
            sprintf(temp, "%hhu", buffer[pos]);
            concatenate(&result, &length, "{UNKNOWN ", temp, "}", NULL);
            break;
        }
        if (shift_state == 0xfc) shift_state = 0;
        break;
      case 0xfd:
        if ((buffer[pos] < 16) && buffers[buffer[pos]])
          concatenate(&result, &length, "{", buffers[buffer[pos]], "}", NULL);
        else {
          sprintf(temp, "%hhu", buffer[pos]);
          concatenate(&result, &length, "{STRING ", temp, "}", NULL);
        }
        shift_state = 0;
        break;
    }
  if (shift_state) {
    free(result);
    return NULL;
  }
  result = realloc(result, length + 1);
  result[length] = 0;
  return result;
}

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

int main (int argc, char ** argv) {
  FILE * in;
  FILE * out;
  if (argc != 4) {
    fprintf(stderr, "usage: %s <infile> <outfile> <directory>\n", *argv);
    return 1;
  }
  in = fopen(argv[1], "r");
  if (!in) {
    fprintf(stderr, "error: could not open file %s for reading\n", argv[1]);
    return 1;
  }
  out = fopen(argv[2], "w");
  if (!out) {
    fprintf(stderr, "error: could not open file %s for writing\n", argv[2]);
    return 1;
  }
  dump_incbins(in, out, argv[3]);
  fclose(in);
  fclose(out);
  return 0;
}

void dump_incbins (FILE * in, FILE * out, const char * root) {
  char * line;
  struct incbin * incbin;
  int done = 0;
  while (!feof(in)) {
    line = read_line(in);
    if ((!done) && is_incbin(line)) {
      printf("<<<< %s\n", line);
      incbin = get_incbin_data(line);
      if (!incbin) {
        printf(">>>> %s\n", line);
        fprintf(out, "%s\n", line);
      } else {
        switch (parse_incbin(incbin, root, out)) {
          case 3:
            done = 1;
            break;
          case 2:
            done = 1;
          case 1:
            printf(">>>> %s\n", line);
            fprintf(out, "%s\n", line);
        }
        free(incbin);
      }
    } else {
      printf("==== %s\n", line);
      fprintf(out, "%s\n", line);
    }
    free(line);
  }
}

int is_incbin (const char * line) {
  const char * pos = strstr(line, ".incbin");
  if (!pos) return 0;
  const char * comment = strchr(line, '@');
  if (!comment) return 1;
  return pos < comment;
}

struct incbin * get_incbin_data (const char * line) {
  const char * pos = strchr(strstr(line, ".incbin"), '"');
  if (!pos) {
    printf("err: no filename specified\n");
    return NULL;
  }
  char * filename;
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

unsigned get_value_from_string (const char * string, unsigned length) {
  if (!(string && length && *string)) return -1;
  char * copy = malloc(length + 1);
  memcpy(copy, string, length);
  copy[length] = 0;
  char * errp;
  unsigned long long result = strtoull(copy, &errp, 0);
  char err = *errp;
  free(copy);
  if (err) return -1;
  if (result > 0xfffffffeULL) return -1;
  return result;
}

unsigned char get_command (const char * prompt, unsigned mask) {
  char * line;
  unsigned char command;
  while (1) {
    printf("%s", prompt);
    line = read_line(stdin);
    if (!(*line && strcmp(line, "?") && strcmp(line, "help"))) {
      print_command_help(mask);
      free(line);
      continue;
    }
    for (command = 0; commands[command].name; command ++) {
      if (!(mask & (1 << command))) continue;
      if (!strcmp(line, commands[command].name) || (commands[command].alias && !strcmp(line, commands[command].alias))) {
        free(line);
        return command;
      }
    }
    free(line);
    printf("Invalid command, enter ? for help.\n");
  }
}

void print_command_help (unsigned mask) {
  unsigned char command;
  for (command = 0; commands[command].name; command ++) {
    if (!(mask & (1 << command))) continue;
    printf("%-" COMMAND_LENGTH "s - ", commands[command].name);
    if (commands[command].alias) printf("(alias: %s) ", commands[command].alias);
    printf("%s\n", commands[command].help_text);
  }
  printf("%-" COMMAND_LENGTH "s - %s\n", "help", "(alias: ?) displays this help");
}

int parse_incbin (struct incbin * incbin, const char * directory, FILE * out) {
  char * location = malloc(1);
  *location = 0;
  unsigned p = 0;
  int rv;
  concatenate(&location, &p, directory, "/", incbin -> file, NULL);
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
  rv = handle_incbin_data(incbin, buffer, out);
  free(buffer);
  return rv;
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

int handle_incbin_data (struct incbin * incbin, const unsigned char * data, FILE * out) {
  unsigned char p, limit = 16;
  if (incbin -> length < 16) limit = incbin -> length;
  printf("Binary inclusion size: %u\n", incbin -> length);
  printf("First %hhu bytes:", limit);
  for (p = 0; p < limit; p ++) printf(" %02hhx", data[p]);
  putchar('\n');
  p = 0x4f;
  if (!(incbin -> length & 1)) p |= 0x10;
  if (!(incbin -> length & 2)) p |= 0x20;
  p = get_command("Action for current .incbin: ", p);
  switch (p) {
    case 0:
      exit(0);
    case 1:
      return 2;
    case 2:
      return 1;
    case 3: case 4: case 5:
      write_header_comment(incbin, out);
      output_binary_data(data, incbin -> length, 1 << (p - 3), out);
      return 0;
    case 6:
      write_header_comment(incbin, out);
      return handle_incbin_text(incbin, data, out);
  }
}

void write_header_comment (struct incbin * incbin, FILE * out) {
  char * header = generate_incbin(incbin -> file, incbin -> offset, incbin -> length);
  printf(">>>> @ replacing: %s\n", header);
  fprintf(out, "@ replacing %s\n", header);
  free(header);
}

void output_binary_data (const unsigned char * data, unsigned length, unsigned char width, FILE * out) {
  char * output_line;
  unsigned output_length;
  char header[8];
  char fmtstring[8];
  char value[16];
  unsigned p, n, val;
  unsigned char pos;
  if (width > 1)
    sprintf(header, ".%hhubyte", width);
  else
    strcpy(header, ".byte");
  sprintf(fmtstring, "0x%%0%hhux", width << 1);
  while (length) {
    output_line = malloc(1);
    *output_line = 0;
    output_length = 0;
    if (length > 32)
      n = 32;
    else
      n = length;
    length -= n;
    n /= width;
    concatenate(&output_line, &output_length, header, " ", NULL);
    for (p = 0; p < n; p ++) {
      val = 0;
      for (pos = 0; pos < width; pos ++) val |= *(data ++) << (pos << 3);
      sprintf(value, fmtstring, val);
      concatenate(&output_line, &output_length, p ? ", " : "", value, NULL);
    }
    printf(">>>> %s\n", output_line);
    fprintf(out, "%s\n", output_line);
    free(output_line);
  }
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

int handle_incbin_text (struct incbin * incbin, const unsigned char * data, FILE * out) {
  int result = 0, yestoall = 0;
  unsigned prev_pos = 0;
  unsigned pos = 0;
  const unsigned char * next_terminator;
  unsigned block;
  char * string;
  unsigned char command;
  while (pos < incbin -> length) {
    next_terminator = memchr(data + pos, 0xff, incbin -> length - pos);
    if (!next_terminator)
      block = incbin -> length - pos;
    else
      block = (next_terminator - data) - pos + 1;
    if (block > 999) {
      pos += block;
      continue;
    }
    if (!(string = parse_buffer(data + pos, block))) continue;
    if (yestoall) {
      write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, pos - prev_pos, out);
      printf(">>>> .string \"%s\"\n", string);
      fprintf(out, ".string \"%s\"\n", string);
      free(string);
      pos += block;
      prev_pos = pos;
      continue;
    }
    printf("String found:\n%s\n", string);
    command = get_command("Keep string? ", 0x783);
    switch (command) {
      case 0:
        exit(0);
      case 1:
        result = 3;
      case 10:
        write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, incbin -> length - prev_pos, out);
        return result;
      case 9:
        free(string);
        pos += block;
        continue;
      case 8:
        yestoall = 1;
      case 7:
        write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, pos - prev_pos, out);
        printf(">>>> .string \"%s\"\n", string);
        fprintf(out, ".string \"%s\"\n", string);
        free(string);
        pos += block;
        prev_pos = pos;
    }
  }
  write_incbin_for_segment(incbin -> file, incbin -> offset + prev_pos, incbin -> length - prev_pos, out);
  return 0;
}

void write_incbin_for_segment (const char * file, unsigned offset, unsigned length, FILE * out) {
  if (!length) return;
  char * incbin = generate_incbin(file, offset, length);
  printf(">>>> %s\n", incbin);
  fprintf(out, "%s\n", incbin);
  free(incbin);
}