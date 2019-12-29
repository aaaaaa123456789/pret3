#include "proto.h"

char * relocate (unsigned char * buffer, unsigned address, const struct ELF_relocation * relocation, const struct ELF * file, const struct ELF * base) {
  const struct relocation * type = NULL;
  unsigned pos;
  for (pos = 0; pos < relocation_table_size; pos ++) if (relocation_table[pos].type == relocation -> type) {
    type = relocation_table + pos;
    break;
  }
  if (!type) return generate_string("relocation (at 0x%08x) failed: unknown relocation type %hhu", address, relocation -> type);
  unsigned value = relocation -> has_addend ? relocation -> addend : type -> read(buffer);
  const char * name = file -> symbols[relocation -> symbol].name;
  unsigned ref;
  int thumb = 0;
  if (base) {
    const struct ELF_symbol * symbol = find_symbol(base, name);
    if (!symbol) return generate_string("relocation (at 0x%08x) failed: unknown symbol %s", address, name);
    if (!(symbol -> section)) return generate_string("relocation (at 0x%08x) failed: symbol %s has no value", address, name);
    ref = symbol -> value;
    if ((symbol -> type == ELF_SYMBOL_FUNCTION) && (symbol -> value & 1)) {
      ref --;
      thumb = 1;
    }
  } else
    ref = compute_symbol_hash(name);
  value = type -> relocate(address, ref, value, thumb);
  unsigned bits = (type -> maskbits < 0) ? -(type -> maskbits) : type -> maskbits;
  unsigned mask = ((bits >= 32) ? 0 : (1 << bits)) - (1 << (type -> alignment));
  // flip the sign-extension bits for overflow checks if the value is signed and negative
  if ((type -> maskbits < 0) && (type -> maskbits > -32) && ((value >> (bits - 1)) & 1)) value ^= -1u << bits;
  if (value & ~mask)
    if (base)
      return generate_string("relocation (at 0x%08x) failed: overflow", address);
    else
      value &= mask;
  if ((type -> maskbits < 0) && (type -> maskbits > -32) && ((value >> (bits - 1)) & 1)) value ^= -1u << bits; // undo the flip
  type -> write(buffer, value);
  return NULL;
}
