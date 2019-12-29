enum ELF_constants {
  // only the interesting constants from the standard
  // section types
  ELF_SECTION_PROGBITS        =      1,
  ELF_SECTION_SYMTAB          =      2,
  ELF_SECTION_STRTAB          =      3,
  ELF_SECTION_RELA            =      4,
  ELF_SECTION_NOBITS          =      8,
  ELF_SECTION_REL             =      9,
  // symbol types
  ELF_SYMBOL_OBJECT           =      1,
  ELF_SYMBOL_FUNCTION         =      2,
  ELF_SYMBOL_SECTION          =      3,
  // ELF file types
  ELF_FILE_RELOCATABLE        =      1,
  ELF_FILE_EXECUTABLE         =      2,
  // special section IDs for symbols
  ELF_SYMBOL_SECTION_ABSOLUTE = 0xFFF1,
  ELF_SYMBOL_SECTION_COMMON   = 0xFFF2
};

enum ARM_relocations {
  // only the types we care to implement: no Thumb32, dynamic linking, or other things not used by the platform
  // no group-based relocations either since the toolchain doesn't use them and they are very hard to implement
  REL_ARM_NONE       =   0,
  REL_ARM_ABS32      =   2,
  REL_ARM_REL32      =   3,
  REL_ARM_ABS16      =   5,
  REL_ARM_ABS12      =   6,
  REL_ARM_THM_ABS5   =   7,
  REL_ARM_ABS8       =   8,
  REL_ARM_THM_CALL   =  10, // labelled as Thumb32, but used by Thumb16's split BL instruction
  REL_ARM_THM_PC8    =  11,
  REL_ARM_CALL       =  28,
  REL_ARM_JUMP24     =  29,
  REL_ARM_ABS32_NOI  =  55,
  REL_ARM_REL32_NOI  =  56,
  REL_ARM_THM_JUMP11 = 102,
  REL_ARM_THM_JUMP8  = 103
};

struct ELF_section {
  unsigned type;
  unsigned load_address;
  unsigned position;
  unsigned size;
  unsigned entry_size;
  unsigned link;
  unsigned info;
  unsigned item_offset; // SYMTAB: first symbol; PROGBITS: first relocation (or -1 for none)
};

struct ELF_symbol {
  unsigned char type;
  unsigned char visibility;
  unsigned short section;
  unsigned value;
  unsigned size;
  const char * name;
};

struct ELF_relocation {
  unsigned offset;
  unsigned addend;
  unsigned symbol;
  unsigned short section;
  unsigned char type;
  unsigned char has_addend;
};

struct ELF {
  unsigned char type;
  unsigned size;
  unsigned section_count;
  unsigned symbol_count;
  unsigned function_count;
  unsigned relocation_count;
  unsigned char * data;
  struct ELF_section * sections;
  struct ELF_symbol * symbols;
  struct ELF_relocation * relocations;
  const struct ELF_symbol ** sorted_symbols;
  const struct ELF_symbol ** sorted_functions;
};

struct relocation {
  unsigned char type;
  signed char maskbits; // negative for a signed value
  unsigned char alignment;
  unsigned (* read) (const unsigned char * buffer);
  void (* write) (unsigned char * buffer, unsigned value);
  unsigned (* relocate) (unsigned address, unsigned symbol, unsigned addend, int thumb);
};

struct options {
  const char ** filenames;
  const char * reference;
  const char ** patterns;
  int compare_mode;
};
