#include "proto.h"

#define basic_transform(name, operation)                                                        \
  int function_ ## name (int a, int b, char ** error) {                                         \
    *error = NULL;                                                                              \
    return operation;                                                                           \
  }                                                                                             \
  char * script_transform_ ## name (struct script_value v, int p, struct script_value * r) {    \
    return script_transform_basic(v, p, r, &function_ ## name);                                 \
  }

basic_transform(add, a + b);
basic_transform(subtract, a - b);
basic_transform(multiply, a * b);
basic_transform(divide, a / b);
basic_transform(modulo, a % b);
basic_transform(rsub, b - a);
basic_transform(rdiv, b / a);
basic_transform(rmod, b % a);
basic_transform(and, a & b);
basic_transform(or, a | b);
basic_transform(xor, a ^ b);
basic_transform(shl, a << b);
basic_transform(shr, ((unsigned) a) >> b);
basic_transform(asr, a >> b);
basic_transform(forceminu, (((unsigned) a) < ((unsigned) b)) ? b : a);
basic_transform(forcemaxu, (((unsigned) a) > ((unsigned) b)) ? b : a);
basic_transform(forcemin, (a < b) ? b : a);
basic_transform(forcemax, (a > b) ? b : a);
basic_transform(compare, (a == b) ? 0 : ((a < b) ? -1 : 1));
basic_transform(compareu, (a == b) ? 0 : ((((unsigned) a) < ((unsigned) b)) ? -1 : 1));

#define checking_transform(name, check)                                                         \
  int function_ ## name (int a, int b, char ** error) {                                         \
    *error = (check) ? NULL : duplicate_string("value not in required range");                  \
    return a;                                                                                   \
  }                                                                                             \
  char * script_transform_ ## name (struct script_value v, int p, struct script_value * r) {    \
    return script_transform_basic(v, p, r, &function_ ## name);                                 \
  }

checking_transform(checkminu, ((unsigned) a) >= ((unsigned) b));
checking_transform(checkmaxu, ((unsigned) a) <= ((unsigned) b));
checking_transform(checkmin, a >= b);
checking_transform(checkmax, a <= b);
checking_transform(checkptr, (validate_pointer(a) || !(a || b)));
checking_transform(require, a == b);
checking_transform(requirenot, a != b);
