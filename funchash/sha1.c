#include "proto.h"

unsigned char * calculate_sha1 (const void * data, unsigned length) {
  uint32_t state[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
  const char * current;
  unsigned remaining;
  for (current = data, remaining = length; remaining >= 64; current += 64, remaining -= 64) sha1_process_block(current, state);
  unsigned char last_block[64] = {0};
  memcpy(last_block, current, remaining);
  last_block[remaining] = 0x80;
  if (remaining >= 56) {
    sha1_process_block(last_block, state);
    memset(last_block, 0, 64);
  }
  unsigned long long bit_length = ((unsigned long long) length) << 3;
  for (remaining = 5; remaining; remaining --) {
    last_block[58 + remaining] = bit_length;
    bit_length >>= 8;
  }
  sha1_process_block(last_block, state);
  unsigned char * result = malloc(20);
  for (remaining = 0; remaining < 20; remaining ++) result[remaining] = state[remaining >> 2] >> ((~remaining & 3) << 3);
  return result;
}

void sha1_process_block (const unsigned char * block, uint32_t * state) {
  uint32_t words[80];
  unsigned pos, temp, count, a, b, c, d, e;
  // constants used by SHA-1; they are actually simply the square roots of 2, 3, 5 and 10 as a fixed-point number (2.30 format)
  const uint32_t hash_constants[4] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};
  memset(words, 0, 16 * sizeof(uint32_t));
  for (pos = 0; pos < 64; pos ++) words[pos >> 2] = (words[pos >> 2] << 8) | block[pos];
  for (pos = 16; pos < 80; pos ++) words[pos] = rotate(words[pos - 3] ^ words[pos - 8] ^ words[pos - 14] ^ words[pos - 16], 1);
  a = *state;
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];
  for (pos = 0; pos < 4; pos ++) for (count = 0; count < 20; count ++) {
    temp = rotate(a, 5) + e + words[pos * 20 + count] + hash_constants[pos];
    switch (pos) {
      case 0:
        temp += (b & c) | (~b & d);
        break;
      case 2:
        temp += (b & c) | (b & d) | (c & d);
        break;
      default:
        temp += b ^ c ^ d;
    }
    e = d;
    d = c;
    c = rotate(b, 30);
    b = a;
    a = temp;
  }
  *state += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
}
