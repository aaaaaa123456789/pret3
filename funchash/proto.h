#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>

#include "struct.h"
#include "inline.h"

#define VERSION "0.1.1"

#define FUNCTION_EXCESS_BUFFER_SIZE 4
#define HASH_LENGTH 40

#if UINT_MAX < 4294967295
  #error This program will not work with ints narrower than 32 bits.
#endif

// elf.c
struct ELF * load_ELF_file(const char *, char **);
void destroy_ELF_file(struct ELF *);
char * parse_ELF_header(struct ELF *);
char * read_ELF_section_table(struct ELF *, unsigned, unsigned short, unsigned short);
char * load_ELF_section_names(struct ELF *, unsigned, unsigned short, unsigned short, unsigned short);
char * load_ELF_symbols(struct ELF *);
char * sort_ELF_symbols(struct ELF *);
int compare_ELF_symbols(const void *, const void *);
char * load_ELF_relocations(struct ELF *);
char * load_ELF_relocations_for_section(struct ELF *, unsigned);
int compare_ELF_relocations(const void *, const void *);

// function.c
char * get_function_hash(const struct ELF *, const struct ELF *, const struct ELF_symbol *, char *);
char * compare_function_data(const struct ELF *, const struct ELF *, const struct ELF_symbol *, char *);
void * extract_function_from_ELF(const struct ELF *, const struct ELF *, const struct ELF_symbol *, char **);
const struct ELF_relocation * find_first_relocation(const struct ELF *, unsigned short, unsigned, unsigned);

// main.c
int main(int, char **);
void print_version(void);
void error_exit(int, const char *, ...);
int process_multiple_files(const struct ELF *, const struct options *);

// misc.c
void * read_file_as_bytes(const char *, unsigned *);
const struct ELF_symbol * find_symbol(const struct ELF *, const char *);
const struct ELF_symbol * find_function_symbol(const struct ELF *, const char *);
const struct ELF_symbol * const * find_symbol_pointer(const struct ELF *, const char *, const struct ELF_symbol **, unsigned);
int match_symbol_name(const void *, const void *);
unsigned compute_symbol_hash(const char *);
void print_hash(const unsigned char * restrict, char * restrict);

// objfile.c
int process_file(const char *, const struct ELF *, const struct options *);
unsigned * get_all_function_symbols(const struct ELF *);
unsigned * get_matching_function_symbols(const struct ELF *, const char **);
int match_pattern(const char *, const char *);

// options.c
struct options * parse_options(int, char **);
void validate_pattern(const char *);
void destroy_options(struct options *);
void print_help(void);

// relfn.c
unsigned read_word(const unsigned char *);
unsigned read_halfword(const unsigned char *);
unsigned read_byte(const unsigned char *);
unsigned read_sbyte_shl1(const unsigned char *);
unsigned read_byte_shl2(const unsigned char *);
unsigned read_ARM_LDR(const unsigned char *);
unsigned read_ARM_jump(const unsigned char *);
unsigned read_thumb_LDR(const unsigned char *);
unsigned read_thumb_jump(const unsigned char *);
unsigned read_thumb_BL(const unsigned char *);
void write_word(unsigned char *, unsigned);
void write_halfword(unsigned char *, unsigned);
void write_byte(unsigned char *, unsigned);
void write_byte_shr1(unsigned char *, unsigned);
void write_byte_shr2(unsigned char *, unsigned);
void write_ARM_LDR(unsigned char *, unsigned);
void write_ARM_jump(unsigned char *, unsigned);
void write_thumb_LDR(unsigned char *, unsigned);
void write_thumb_jump(unsigned char *, unsigned);
void write_thumb_BL(unsigned char *, unsigned);
unsigned relocation_dummy(unsigned, unsigned, unsigned, int);
unsigned relocation_absolute(unsigned, unsigned, unsigned, int);
unsigned relocation_absolute_thumb(unsigned, unsigned, unsigned, int);
unsigned relocation_relative(unsigned, unsigned, unsigned, int);
unsigned relocation_relative_thumb(unsigned, unsigned, unsigned, int);
unsigned relocation_relative_aligned(unsigned, unsigned, unsigned, int);

// reltable.c
extern const struct relocation relocation_table[];
extern const unsigned relocation_table_size;

// relocate.c
char * relocate(unsigned char *, unsigned, const struct ELF_relocation *, const struct ELF *, const struct ELF *);
void load_effective_section_addresses(struct ELF *, const struct ELF *);

// sha1.c
unsigned char * calculate_sha1(const void *, unsigned);
void sha1_process_block(const unsigned char *, uint32_t *);

// string.c
char * duplicate_string(const char *);
char * generate_string(const char *, ...);
char * generate_string_from_varargs(const char *, va_list);
