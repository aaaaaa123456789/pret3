#define basic_transform_headers(name)                                                  \
  int function_ ## name(int, int, char **);                                            \
  char * script_transform_ ## name(struct script_value, int, struct script_value *);

basic_transform_headers(add);
basic_transform_headers(subtract);
basic_transform_headers(multiply);
basic_transform_headers(divide);
basic_transform_headers(modulo);
basic_transform_headers(rsub);
basic_transform_headers(rdiv);
basic_transform_headers(rmod);
basic_transform_headers(and);
basic_transform_headers(or);
basic_transform_headers(xor);
basic_transform_headers(shl);
basic_transform_headers(shr);
basic_transform_headers(asr);
basic_transform_headers(forcemin);
basic_transform_headers(forcemax);
basic_transform_headers(forceminu);
basic_transform_headers(forcemaxu);
basic_transform_headers(compare);
basic_transform_headers(compareu);
basic_transform_headers(checkmin);
basic_transform_headers(checkmax);
basic_transform_headers(checkminu);
basic_transform_headers(checkmaxu);
basic_transform_headers(checkptr);
basic_transform_headers(require);
basic_transform_headers(requirenot);
