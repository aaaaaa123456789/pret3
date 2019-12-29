#include <stdint.h>

static inline unsigned convert_buffer_to_number (const void * buffer, unsigned char size) {
  const unsigned char * p = buffer;
  unsigned result = 0;
  if (size > sizeof result) return -1;
  unsigned char pos;
  for (pos = 0; pos < size; pos ++) result |= *(p ++) << (pos << 3);
  return result;
}

static inline void write_number_to_buffer (void * buffer, unsigned value, unsigned char size) {
  if (size > sizeof value) return;
  unsigned char * p = buffer;
  while (size --) {
    *(p ++) = value;
    value >>= 8;
  }
}

static inline uint32_t rotate (uint32_t value, unsigned count) {
  return (value << count) | (value >> (32 - count));
}

static inline char hex_digit (unsigned char value) {
  if (value < 10) return '0' + value;
  if (value < 16) return 'a' + (value - 10);
  return 'x';
}
