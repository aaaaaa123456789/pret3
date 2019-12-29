#include "proto.h"

const struct relocation relocation_table[] = {
  {.type = REL_ARM_NONE,       .maskbits =  32, .alignment = 0, .read = &read_word,       .write = &write_word,       .relocate = &relocation_dummy},
  {.type = REL_ARM_ABS32,      .maskbits =  32, .alignment = 0, .read = &read_word,       .write = &write_word,       .relocate = &relocation_absolute_thumb},
  {.type = REL_ARM_REL32,      .maskbits =  32, .alignment = 0, .read = &read_word,       .write = &write_word,       .relocate = &relocation_relative_thumb},
  {.type = REL_ARM_ABS16,      .maskbits = -17, .alignment = 0, .read = &read_halfword,   .write = &write_halfword,   .relocate = &relocation_absolute},
  {.type = REL_ARM_ABS12,      .maskbits = -13, .alignment = 0, .read = &read_ARM_LDR,    .write = &write_ARM_LDR,    .relocate = &relocation_absolute},
  {.type = REL_ARM_THM_ABS5,   .maskbits =   7, .alignment = 2, .read = &read_thumb_LDR,  .write = &write_thumb_LDR,  .relocate = &relocation_absolute},
  {.type = REL_ARM_ABS8,       .maskbits =  -9, .alignment = 0, .read = &read_byte,       .write = &write_byte,       .relocate = &relocation_absolute},
  {.type = REL_ARM_THM_CALL,   .maskbits = -23, .alignment = 1, .read = &read_thumb_BL,   .write = &write_thumb_BL,   .relocate = &relocation_relative},
  {.type = REL_ARM_THM_PC8,    .maskbits =  10, .alignment = 2, .read = &read_byte_shl2,  .write = &write_byte_shr2,  .relocate = &relocation_relative_aligned},
  {.type = REL_ARM_CALL,       .maskbits = -26, .alignment = 2, .read = &read_ARM_jump,   .write = &write_ARM_jump,   .relocate = &relocation_relative},
  {.type = REL_ARM_JUMP24,     .maskbits = -26, .alignment = 2, .read = &read_ARM_jump,   .write = &write_ARM_jump,   .relocate = &relocation_relative},
  {.type = REL_ARM_ABS32_NOI,  .maskbits =  32, .alignment = 0, .read = &read_word,       .write = &write_word,       .relocate = &relocation_absolute},
  {.type = REL_ARM_REL32_NOI,  .maskbits =  32, .alignment = 0, .read = &read_word,       .write = &write_word,       .relocate = &relocation_relative},
  {.type = REL_ARM_THM_JUMP11, .maskbits = -12, .alignment = 1, .read = &read_thumb_jump, .write = &write_thumb_jump, .relocate = &relocation_relative},
  {.type = REL_ARM_THM_JUMP8,  .maskbits =  -9, .alignment = 1, .read = &read_sbyte_shl1, .write = &write_byte_shr1,  .relocate = &relocation_relative}
};

const unsigned relocation_table_size = sizeof relocation_table / sizeof *relocation_table;
