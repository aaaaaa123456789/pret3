#define ___NO_DEFINE_VARS
#include "proto.h"

struct settings global_settings = {
  .insert_replacement_comment = 1,
  .indent_lines               = 0,
  .code_labels                = 1,
  .data_labels                = 2,
  .endianness                 = 0,
  .elf_endian                 = 1,
  .pointer_model              = 1
};

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
  /* B0 */  "\xe2\x80\xa6", "\xe2\x80\x9c", "\xe2\x80\x9d", "\xe2\x80\x98", "'",            "\xe2\x99\x82", "\xe2\x99\x80", "\xc2\xa5",
  /* B8 */  ",",            "\xc3\x97",     "/",            "A",            "B",            "C",            "D",            "E",
  /* C0 */  "F",            "G",            "H",            "I",            "J",            "K",            "L",            "M",
  /* C8 */  "N",            "O",            "P",            "Q",            "R",            "S",            "T",            "U",
  /* D0 */  "V",            "W",            "X",            "Y",            "Z",            "a",            "b",            "c",
  /* D8 */  "d",            "e",            "f",            "g",            "h",            "i",            "j",            "k",
  /* E0 */  "l",            "m",            "n",            "o",            "p",            "q",            "r",            "s",
  /* E8 */  "t",            "u",            "v",            "w",            "x",            "y",            "z",            "\xe2\x96\xb6",
  /* F0 */  ":",            "\xc3\x84",     "\xc3\x96",     "\xc3\x9c",     "\xc3\xa4",     "\xc3\xb6",     "\xc3\xbc",     NULL,
  /* F8 */  NULL,           NULL,           "\\l",          "\\p",          NULL,           NULL,           "\\n",          "$"
};

const unsigned char previewable[] = {0xff, 0xfb, 0xff, 0x7e, 0xff, 0x6f, 0x20,    0,    0,    0,    6, 0x3c,    0, 0x81,    0,    0,
                                        0,    0,    0,    0, 0xfe, 0x7f,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x80};

const char * colors[] = {"TRANSPARENT", "DARK_GREY", "RED", "GREEN", "BLUE", "YELLOW", "CYAN", "MAGENTA",
                         "LIGHT_GRAY", "BLACK", "BLACK2", "SILVER", "WHITE", "SKY_BLUE", "LIGHT_BLUE", "WHITE2"};

const char * buffers[] = {NULL, "PLAYER", "STR_VAR_1", "STR_VAR_2", "STR_VAR_3", NULL, "RIVAL", "VERSION",
                          "EVIL_TEAM", "GOOD_TEAM", "EVIL_LEADER", "GOOD_LEADER", "EVIL_LEGENDARY", "GOOD_LEGENDARY", NULL, NULL};

const char * joypad_buttons[] = {"A_BUTTON", "B_BUTTON", "L_BUTTON", "R_BUTTON", "START_BUTTON", "SELECT_BUTTON",
                                 "DPAD_UP", "DPAD_DOWN", "DPAD_LEFT", "DPAD_RIGHT", "DPAD_UPDOWN", "DPAD_LEFTRIGHT", "DPAD_ANY"};

const char * extra_symbols[] = {"UP_ARROW_2", "DOWN_ARROW_2", "LEFT_ARROW_2", "RIGHT_ARROW_2", "PLUS", "LV_2", "PP",
                                "ID", "NO", "UNDERSCORE", "CIRCLE_1", "CIRCLE_2", "CIRCLE_3", "CIRCLE_4", "CIRCLE_5",
                                "CIRCLE_6", "CIRCLE_7", "CIRCLE_8", "CIRCLE_9", "LEFT_PAREN", "RIGHT_PAREN", "CIRCLE_DOT", "TRIANGLE", "BIG_MULT_X"};

const char * emoji[] = {"EMOJI_UNDERSCORE", "EMOJI_PIPE", "EMOJI_HIGHBAR", "EMOJI_TILDE", "EMOJI_LEFT_PAREN",
                        "EMOJI_RIGHT_PAREN", "EMOJI_UNION", "EMOJI_GREATER_THAN", "EMOJI_LEFT_EYE", "EMOJI_RIGHT_EYE",
                        "EMOJI_AT", "EMOJI_SEMICOLON", "EMOJI_PLUS", "EMOJI_MINUS", "EMOJI_EQUALS", "EMOJI_SPIRAL",
                        "EMOJI_TONGUE", "EMOJI_TRIANGLE_OUTLINE", "EMOJI_ACUTE", "EMOJI_GRAVE", "EMOJI_CIRCLE",
                        "EMOJI_TRIANGLE", "EMOJI_SQUARE", "EMOJI_HEART", "EMOJI_MOON", "EMOJI_NOTE", "EMOJI_BALL",
                        "EMOJI_BOLT", "EMOJI_LEAF", "EMOJI_FIRE", "EMOJI_WATER", "EMOJI_LEFT_FIST", "EMOJI_RIGHT_FIST",
                        "EMOJI_BIGWHEEL", "EMOJI_SMALLWHEEL", "EMOJI_SPHERE", "EMOJI_IRRITATED", "EMOJI_MISCHIEVOUS",
                        "EMOJI_HAPPY", "EMOJI_ANGRY", "EMOJI_SURPRISED", "EMOJI_BIGSMILE", "EMOJI_EVIL", "EMOJI_TIRED",
                        "EMOJI_NEUTRAL", "EMOJI_SHOCKED", "EMOJI_BIGANGER"};

struct command commands[] = {
  [COMMAND_ABORT]     = {"abort",        NULL,        "aborts all processing and exits"},
  [COMMAND_DONE]      = {"done",         "!",         "finishes normally without parsing any more files"},
  [COMMAND_SKIP]      = {"skip",         "s",         "skips the current file entirely and outputs it" HELP_TEXT_NEWLINE
                                                      "as an .incbin"},
  [COMMAND_DATA8]     = {"data8",        NULL,        "outputs the current file as a series of 8-bit values"},
  [COMMAND_DATA16]    = {"data16",       NULL,        "outputs the current file as a series of 16-bit values"},
  [COMMAND_DATA32]    = {"data32",       NULL,        "outputs the current file as a series of 32-bit values"},
  [COMMAND_DATAPTR]   = {"dataptr",      NULL,        "outputs the current file as a series of pointers"},
  [COMMAND_TEXTSCAN]  = {"textscan",     "t",         "scans the current file for text strings to output"},
  [COMMAND_YES]       = {"yes",          "y",         "preserves the current string as a .string"},
  [COMMAND_YESTOALL]  = {"yestoall",     "y!",        "preserves the current string and all subsequent" HELP_TEXT_NEWLINE
                                                      "ones in the file as .string values"},
  [COMMAND_NO]        = {"no",           "n",         "keeps the current string as part of an .incbin"},
  [COMMAND_NOTOALL]   = {"notoall",      "n!",        "keeps the current string and all subsequent ones" HELP_TEXT_NEWLINE
                                                      "in the file as part of an .incbin"},
  [COMMAND_PREVIEW]   = {"preview",      "p",         "previews the current .incbin, 256 bytes at a time"},
  [COMMAND_DUMP]      = {"dump",         NULL,        "dumps the current .incbin as text data (in the same format" HELP_TEXT_NEWLINE
                                                      "as a preview) into a file"},
  [COMMAND_DUMPRAW]   = {"dumpraw",      NULL,        "dumps the data of the current .incbin into a binary file"},
  [COMMAND_SCRIPT]    = {"script",       NULL,        "executes a user-defined script on the current .incbin"},
  [COMMAND_SCRIPTALL] = {"scriptall",    NULL,        "executes a user-defined script on every remaining .incbin" HELP_TEXT_NEWLINE
                                                      "in the input, outputting the results or preserving the" HELP_TEXT_NEWLINE
                                                      "original .incbin on script error"},
  [COMMAND_SETTINGS]  = {"settings",     "set",       "enters settings and configuration mode"},
  [COMMAND_LOADSYM]   = {"loadsym",      "ls",        "loads symbol data from an ELF file"},
  [COMMAND_UNLOADSYM] = {"unloadsym",    "us",        "unloads the currently loaded symbol data"},
  [COMMANDS]          = {NULL,           NULL,        NULL}
};

struct setting_entry setting_entries[] = {
  {"headers",      &headers_setting_handler,        "enables or disables the initial @replacing .incbin header." HELP_TEXT_NEWLINE
                                                    "Valid values are on and off."},
  {"indent",       &indent_setting_handler,         "changes the indentation with which new content is written to" HELP_TEXT_NEWLINE
                                                    "the output. Valid values are 0-9, tab and none."},
  {"codelabels",   &code_labels_setting_handler,    "determines whether dataptr will replace code pointers with" HELP_TEXT_NEWLINE
                                                    "the corresponding function labels after symbols have been" HELP_TEXT_NEWLINE
                                                    "loaded. Valid values are on and off."},
  {"datalabels",   &data_labels_setting_handler,    "determines whether dataptr will replace data pointers with" HELP_TEXT_NEWLINE
                                                    "labels after symbols have been loaded. Valid values are off," HELP_TEXT_NEWLINE
                                                    "exact (only exact matches) and on (using label + offset)."},
  {"endianness",   &endianness_setting_handler,     "sets the endianness of the data in the binary files. Valid" HELP_TEXT_NEWLINE
                                                    "values are little and big."},
  {"elfendian",    &elf_endian_setting_handler,     "determines whether reading an ELF file for symbol also sets" HELP_TEXT_NEWLINE
                                                    "the current endianness. Valid values are off and on."},
  {"ptrmodel",     &pointer_model_setting_handler,  "selects the address ranges used to validate pointers. Valid" HELP_TEXT_NEWLINE
                                                    "values are any, gba and n64-flat."},
  {NULL,           NULL,                            NULL}
};

#define transform_entry(name) {#name, &script_transform_ ## name}

struct transform transforms[] = {
  transform_entry(add),
  transform_entry(all),
  transform_entry(and),
  transform_entry(any),
  transform_entry(append),
  transform_entry(asr),
  transform_entry(bytesuntil),
  transform_entry(byteswhile),
  transform_entry(checkmax),
  transform_entry(checkmaxu),
  transform_entry(checkmin),
  transform_entry(checkminu),
  transform_entry(checkptr),
  transform_entry(compare),
  transform_entry(compareu),
  transform_entry(copy),
  transform_entry(count),
  transform_entry(divide),
  transform_entry(forcemax),
  transform_entry(forcemaxu),
  transform_entry(forcemin),
  transform_entry(forceminu),
  transform_entry(int),
  transform_entry(item),
  transform_entry(length),
  transform_entry(modulo),
  transform_entry(multi),
  transform_entry(multi8),
  transform_entry(multi16),
  transform_entry(multi32),
  transform_entry(multiply),
  transform_entry(or),
  transform_entry(prepend),
  transform_entry(rdiv),
  transform_entry(require),
  transform_entry(requirenot),
  transform_entry(rmod),
  transform_entry(rotate),
  transform_entry(rotateback),
  transform_entry(rsub),
  transform_entry(shl),
  transform_entry(shr),
  transform_entry(skip),
  transform_entry(subtract),
  transform_entry(text),
  transform_entry(xor),
  {NULL, NULL}
};

struct ELF_symbol ** global_symbol_table = NULL;
unsigned global_symbol_count = 0;

const char * repository_path = NULL;
const char * global_script_path = NULL;

char ** command_line_filenames = NULL; // do not free individual entries!
unsigned command_line_filename_count = 0;

FILE * global_temporary_file = NULL;

int last_command_entered = -1;
