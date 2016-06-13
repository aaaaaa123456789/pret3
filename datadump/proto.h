#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "struct.h"

#define COMMAND_LENGTH "10"
#define HELP_TEXT_NEWLINE "\n             "
#define VALID_NAME_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"

// command.c
unsigned char get_command(const char *, unsigned);
void print_command_help(unsigned);

// file.c
char * read_line(FILE *);
unsigned get_file_length(FILE *);
char ** read_file_by_lines(FILE *);

// global.c
#ifndef ___NO_DEFINE_VARS
extern const char * text_table[];
extern const unsigned char previewable[];
extern const char * colors[];
extern const char * buffers[];
extern struct command commands[];
extern struct transform transforms[];
#endif

// ib_bin.c
void output_binary_data(const unsigned char *, unsigned, unsigned char, FILE *);

// ib_cmd.c
int handle_incbin_data(struct incbin *, const unsigned char *, FILE *);
void preview_incbin(const unsigned char *, unsigned, unsigned);

// ib_dump.c
void dump_incbin_as_text(struct incbin *, const unsigned char *);
void dump_incbin_as_binary(struct incbin *, const unsigned char *);
void dump_data_line_as_text(FILE *, const unsigned char *, unsigned, unsigned);

// ib_hdr.c
struct incbin * get_incbin_data(const char *);
int is_incbin(const char *);
void write_header_comment(struct incbin *, FILE *);
char * generate_incbin(const char *, unsigned, unsigned);
void write_incbin_for_segment(const char *, unsigned, unsigned, FILE *);

// ib_parse.c
int parse_incbin(struct incbin *, const char *, FILE *);

// ib_text.c
int handle_incbin_text(struct incbin *, const unsigned char *, FILE *);

// main.c
int main(int, char **);
void dump_incbins(FILE *, FILE *, const char *);

// scr_base.c
char * script_get_expression_value(const char *, struct script_variables *, int *);
unsigned char get_line_type(const char *);

// scr_core.c
int run_script(struct incbin *, const void *, FILE *);
char ** execute_script(struct incbin *, const void *, char **, char **);

// scr_exec.c
char ** execute_script_once(struct incbin *, const void *, char **, char **, unsigned *);
char * generate_script_error(char *, unsigned);
int script_loop_test(const char *, struct script_variables *, char **);

// scr_prnt.c
char * generate_script_output_line(const char *, struct script_variables *, char **);
char * print_script_variable_contents(struct script_value, unsigned char);

// scr_tf.c
char * script_parse_assignment_line(const char *, struct script_variables *);
char * script_get_initializer_value(const char *, struct script_value *, struct script_variables *);
char * script_get_transforms_for_line(char **, struct script_variables *, struct script_transforms *);

// scr_vars.c
struct script_variables * init_script_variables(struct incbin *, const unsigned char *, unsigned);
struct script_value * find_script_variable(struct script_variables *, const char *);
void destroy_script_value(struct script_value);
void assign_script_value(struct script_variables *, const char *, struct script_value);
void destroy_script_variables(struct script_variables *);
struct script_value copy_script_value(struct script_value);
int validate_variable_name(const char *);

// string.c
unsigned get_value_from_string(const char *, unsigned);
int convert_string_to_number(const char *, int);
void concatenate(char **, unsigned *, ...);
char * duplicate_string(const char *);
char ** split_by_spaces(const char *);
unsigned string_array_size(char **);
void destroy_string_array(char **);
const char * find_first_non_space(const char *);

// txtparse.c
char * parse_buffer(const unsigned char *, unsigned);

// t_basic.c
#include "tbproto.h"

// t_bytes.c
char * script_transform_byte_count(struct script_value, int, struct script_value *, int);
char * script_transform_byteswhile(struct script_value, int, struct script_value *);
char * script_transform_bytesuntil(struct script_value, int, struct script_value *);

// t_core.c
char * execute_transforms(struct script_value, struct script_transforms *, unsigned, struct script_value *);
int enforce_value_size(int, unsigned char);
char * script_transform_basic(struct script_value, int, struct script_value *, int (*) (int, int, char **));
short read_16(const unsigned char *);
int read_32(const unsigned char *);

// t_data.c
char * script_transform_skip(struct script_value, int, struct script_value *);
char * script_transform_item(struct script_value, int, struct script_value *);
char * script_transform_copy(struct script_value, int, struct script_value *);

// t_int.c
char * script_transform_int(struct script_value, int, struct script_value *);

// t_length.c
char * script_transform_length(struct script_value, int, struct script_value *);

// t_multi.c
char * script_transform_multi(struct script_value, int, struct script_value *, unsigned char);
char * script_transform_multi8(struct script_value, int, struct script_value *);
char * script_transform_multi16(struct script_value, int, struct script_value *);
char * script_transform_multi32(struct script_value, int, struct script_value *);

// t_text.c
char * script_transform_text(struct script_value, int, struct script_value *);
