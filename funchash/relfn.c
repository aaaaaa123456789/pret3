#include "proto.h"

unsigned read_word (const unsigned char * buffer) {
  return convert_buffer_to_number(buffer, 4);
}

unsigned read_halfword (const unsigned char * buffer) {
  unsigned result = convert_buffer_to_number(buffer, 2);
  if (result & 0x8000) result |= 0xFFFF0000u;
  return result;
}

unsigned read_byte (const unsigned char * buffer) {
  unsigned result = *buffer;
  if (result & 0x80) result |= 0xFFFFFF00u;
  return result;
}

unsigned read_sbyte_shl1 (const unsigned char * buffer) {
  unsigned result = ((unsigned) *buffer) << 1;
  if (result & 0x100) result -= 0x200;
  return result;
}

unsigned read_byte_shl2 (const unsigned char * buffer) {
  return ((unsigned) *buffer) << 2;
}

unsigned read_ARM_LDR (const unsigned char * buffer) {
  unsigned result = convert_buffer_to_number(buffer, 2) & 0xFFF;
  if (buffer[2] & 0x80) result = -result;
  return result;
}

unsigned read_ARM_jump (const unsigned char * buffer) {
  unsigned result = convert_buffer_to_number(buffer, 3) << 2;
  if (result & 0x2000000) result -= 0x4000000;
  return result;
}

unsigned read_thumb_LDR (const unsigned char * buffer) {
  return (convert_buffer_to_number(buffer, 2) >> 4) & 0x7C;
}

unsigned read_thumb_jump (const unsigned char * buffer) {
  unsigned result = convert_buffer_to_number(buffer, 2) & 0x7FF;
  result <<= 1;
  if (result & 0x800) result -= 0x1000;
  return result;
}

unsigned read_thumb_BL (const unsigned char * buffer) {
  unsigned high = convert_buffer_to_number(buffer, 2), low = convert_buffer_to_number(buffer + 2, 2);
  unsigned result = ((low & 0x7FF) << 1) | ((high & 0x7FF) << 12);
  if (result & 0x400000) result -= 0x800000;
  return result;
}

void write_word (unsigned char * buffer, unsigned word) {
  write_number_to_buffer(buffer, word, 4);
}

void write_halfword (unsigned char * buffer, unsigned word) {
  write_number_to_buffer(buffer, word, 2);
}

void write_byte (unsigned char * buffer, unsigned word) {
  *buffer = word;
}

void write_byte_shr1 (unsigned char * buffer, unsigned word) {
  *buffer = word >> 1;
}

void write_byte_shr2 (unsigned char * buffer, unsigned word) {
  *buffer = word >> 2;
}

void write_ARM_LDR (unsigned char * buffer, unsigned word) {
  if (word & 0x80000000u) {
    word = -word;
    buffer[2] |= 0x80;
  } else
    buffer[2] = 0x7F;
  word = (word & 0xFFF) | ((buffer[1] & 0xF0) << 8);
  write_number_to_buffer(buffer, word, 2);
}

void write_ARM_jump (unsigned char * buffer, unsigned word) {
  write_number_to_buffer(buffer, word >> 2, 3);
}

void write_thumb_LDR (unsigned char * buffer, unsigned word) {
  word = (word & 0x7C) << 4;
  word |= convert_buffer_to_number(buffer, 2) & 0xF83F;
  write_number_to_buffer(buffer, word, 2);
}

void write_thumb_jump (unsigned char * buffer, unsigned word) {
  word = (word >> 1) & 0x7FF;
  word |= (buffer[1] & 0xF8) << 8;
  write_number_to_buffer(buffer, word, 2);
}

void write_thumb_BL (unsigned char * buffer, unsigned word) {
  unsigned high = (word >> 12) & 0x7FF, low = (word >> 1) & 0x7FF;
  high |= (buffer[1] & 0xF8) << 8;
  low |= (buffer[3] & 0xF8) << 8;
  write_number_to_buffer(buffer, high, 2);
  write_number_to_buffer(buffer + 2, low, 2);
}

unsigned relocation_dummy (unsigned address, unsigned symbol, unsigned addend, int thumb) {
  return addend;
}

unsigned relocation_absolute (unsigned address, unsigned symbol, unsigned addend, int thumb) {
  return symbol + addend;
}

unsigned relocation_absolute_thumb (unsigned address, unsigned symbol, unsigned addend, int thumb) {
  return (symbol + addend) | !!thumb;
}

unsigned relocation_relative (unsigned address, unsigned symbol, unsigned addend, int thumb) {
  return symbol + addend - address;
}

unsigned relocation_relative_thumb (unsigned address, unsigned symbol, unsigned addend, int thumb) {
  return ((symbol + addend) | !!thumb) - address;
}

unsigned relocation_relative_aligned (unsigned address, unsigned symbol, unsigned addend, int thumb) {
  return symbol + addend - (address & ~3);
}
