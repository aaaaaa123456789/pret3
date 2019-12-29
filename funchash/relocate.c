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
  const char * name;
  if (file -> symbols[relocation -> symbol].type == ELF_SYMBOL_SECTION) {
    if (relocation -> section >= file -> section_count)
      return generate_string("relocation (at 0x%08x) failed: symbol points to unknown section #%hu", address, relocation -> section);
    else if (!(relocation -> section))
      return generate_string("relocation (at 0x%08x) failed: symbol points to the undefined section", address);
    name = file -> sections[file -> symbols[relocation -> symbol].section].name;
  } else
    name = file -> symbols[relocation -> symbol].name;
  unsigned ref;
  int thumb = 0;
  if (base) {
    const struct ELF_symbol * symbol = NULL;
    struct ELF_symbol symbol_data; // for section-based relocations
    if (file -> symbols[relocation -> symbol].type == ELF_SYMBOL_SECTION) {
      symbol_data = (struct ELF_symbol) {
        .type = ELF_SYMBOL_SECTION,
        .section = file -> symbols[relocation -> symbol].section,
        .size = file -> sections[file -> symbols[relocation -> symbol].section].size,
        .name = name,
        .value = file -> sections[file -> symbols[relocation -> symbol].section].load_address
      };
      if (symbol_data.value == -1u)
        return generate_string("relocation (at 0x%08x) failed: could not determine load address for section %s", address, name);
      symbol = &symbol_data;
    } else
      symbol = find_symbol(base, name);
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

void load_effective_section_addresses (struct ELF * file, const struct ELF * base) {
  unsigned short section;
  unsigned symbol;
  const struct ELF_symbol * const * reference;
  for (section = 0; section < file -> section_count; section ++) {
    file -> sections[section].load_address = -1u;
    if ((file -> sections[section].type != ELF_SECTION_PROGBITS) && (file -> sections[section].type != ELF_SECTION_NOBITS)) continue;
    // attempt to find a function or object symbol in the section, and look it up in the reference file
    // if there is one and only one match, the difference in addresses is the section's load address
    // if no matches can be found, the load address is unknown and left as -1
    for (symbol = 0; symbol < file -> symbol_count; symbol ++) {
      if (file -> symbols[symbol].section != section) continue;
      if ((file -> symbols[symbol].type != ELF_SYMBOL_FUNCTION) && (file -> symbols[symbol].type != ELF_SYMBOL_OBJECT)) continue;
      reference = find_symbol_pointer(base, file -> symbols[symbol].name, base -> sorted_symbols, base -> symbol_count);
      if (!reference) continue;
      if (reference[1] && !strcmp(reference[1] -> name, file -> symbols[symbol].name)) continue;
      if ((**reference).type != file -> symbols[symbol].type) continue;
      break;
    }
    if (symbol >= file -> symbol_count) continue;
    file -> sections[section].load_address = (**reference).value - file -> symbols[symbol].value;
  }
}
