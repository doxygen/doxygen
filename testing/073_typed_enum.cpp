// objective: test underlying type and strongness for an enum
// check: 073__typed__enum_8cpp.xml

/** \file */

/** @brief A strongly-typed enum */
enum class E: unsigned short {};

/** @brief Strongly types enum when values that has the same name as the enum */
enum class Mem : unsigned char {
  Bottom = 0,
  NotMem = 1U << 0,
  Ptr    = 1U << 1,
  Lval   = 1U << 2,
  Mem    = Ptr | Lval,
  Top    = NotMem | Mem,
};

enum {
  Unnamed1
};

enum {
  Unnamed2
};
