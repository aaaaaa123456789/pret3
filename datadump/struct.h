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

struct setting_entry {
  const char * name;
  char * (* handler) (const char *);
  const char * description;
};

struct script_value {
  unsigned char type; // 0: data buffer, {1, 2, 3}: {8, 16, 32}-bit integer, 4: text, {5, 6, 7}: arrays of integers
  int value;
  void * data;
};

struct script_variables {
  unsigned count;
  char ** names;
  struct script_value * values;
};

struct script_transforms {
  char * (* transform) (struct script_value, int, struct script_value *);
  int parameter;
};

struct transform {
  const char * name;
  char * (* transform) (struct script_value, int, struct script_value *);
};

struct settings {
  unsigned insert_replacement_comment: 1;
  unsigned indent_lines: 4; // 0-9: amount of spaces, 15: one tab
  unsigned code_labels: 1;
  unsigned data_labels: 2; // 0: off, 1: exact, 2: offset
  unsigned endianness: 1; // 0: little, 1: big
};

struct ELF_section {
  unsigned type; // 2: symbol table
  unsigned offset;
  unsigned size;
  unsigned link; // for symbol table sections, related string table section
  unsigned entry_size;
};

struct ELF_symbol {
  unsigned value;
  unsigned size;
  unsigned char type; // 1: data pointer, 2: function pointer
  char name[];
};

enum execution_modes {
  MODE_INTERACTIVE = 0,
  MODE_AUTO_DATA_8 = 1,
  MODE_AUTO_DATA_16 = 2,
  MODE_AUTO_DATA_PTR = 3,
  MODE_AUTO_DATA_32 = 4,
  MODE_AUTO_SCRIPT = 5
};
