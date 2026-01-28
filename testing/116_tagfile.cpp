// objective: test the tagfile output
// config: GENERATE_TAGFILE = "$TESTOUTDIR/out/tagfile"
// config: STRIP_FROM_INC_PATH = "$INPUTDIR/more_116/include"
// check: 116__tagfile_8cpp.xml
// check: class_namespace_1_1_class.xml
// check: f_8cpp.xml
// check: group___group.xml
// check: more__116_2a_2116__tagfile_8cpp.xml
// check: more__116_2b_2116__tagfile_8cpp.xml
// check: namespace_namespace.xml
// check: s_2h_8h.xml
// check: struct_struct.xml
// check: t_2s_2h_8h.xml
// check: tagfile
// check: union_union.xml
// input: more_116/f.cpp
// input: more_116/a/116_tagfile.cpp
// input: more_116/b/116_tagfile.cpp
// input: more_116/include/s/h.h
// input: more_116/include/t/s/h.h

/**
 * @defgroup Group Group
 *
 * @brief Group brief (in 116_tagfile.cpp)
 *
 * Group description (in 116_tagfile.cpp).
 *
 * @{
 */

/**
 * @file
 *
 * @brief 116_tagfile.cpp brief (in 116_tagfile.cpp)
 *
 * 116_tagfile.cpp description (in 116_tagfile.cpp).
 */

/**
 * @brief Define brief (in 116_tagfile.cpp).
 *
 * Define description (in 116_tagfile.cpp).
 */
#define Define 123

/**
 * @brief Macro brief (in 116_tagfile.cpp).
 *
 * Macro description (in 116_tagfile.cpp).
 *
 * @param x is the parameter.
 */
#define Macro(x) ((x) * 456)

/**
 * @brief Object brief (in 116_tagfile.cpp).
 *
 * Object description (in 116_tagfile.cpp).
 */
extern int Object;

/**
 * @brief Enum brief (in 116_tagfile.cpp).
 *
 * Enum description (in 116_tagfile.cpp).
 */
enum Enum {
  /**
   * @brief EnumVal brief (in 116_tagfile.cpp).
   *
   * EnumVal description (in 116_tagfile.cpp).
   */
  EnumVal
};

/**
 * @brief EnumTypedef brief (in 116_tagfile.cpp).
 *
 * EnumTypedef description (in 116_tagfile.cpp).
 */
typedef Enum EnumTypedef;

/**
 * @brief Struct brief (in 116_tagfile.cpp).
 *
 * Struct description (in 116_tagfile.cpp).
 */
struct Struct {
  /**
   * @brief StructMember brief (in 116_tagfile.cpp).
   *
   * StructMember description (in 116_tagfile.cpp).
   */
  int StructMember;
};

/**
 * @brief Union brief (in 116_tagfile.cpp).
 *
 * Union description (in 116_tagfile.cpp).
 */
union Union {
  /**
   * @brief UnionMember brief (in 116_tagfile.cpp).
   *
   * UnionMember description (in 116_tagfile.cpp).
   */
  int UnionMember;
};

/**
 * @brief Function brief (in 116_tagfile.cpp).
 *
 * Function description (in 116_tagfile.cpp).
 *
 * @param a is the parameter.
 *
 * @param[in] b is the in parameter.
 *
 * @param[out] c is the out parameter.
 *
 * @param[in, out] d is the in/out parameter.
 */
void Function(int a, const int *b, int *c, int *d);

/**
 * @brief InlineFunction brief (in 116_tagfile.cpp).
 *
 * InlineFunction description (in 116_tagfile.cpp).
 *
 * @param a is the parameter.
 *
 * @param[in] b is the in parameter.
 *
 * @param[out] c is the out parameter.
 *
 * @param[in, out] d is the in/out parameter.
 */
static inline void InlineFunction(int a, const int *b, int *c, int *d)
{
  *c = a;
  *d = *b;
}

/**
 * @brief Namespace brief (in 116_tagfile.cpp).
 *
 * Namespace description (in 116_tagfile.cpp).
 */
namespace Namespace {
  /**
   * @brief Class brief (in 116_tagfile.cpp).
   *
   * Class description (in 116_tagfile.cpp).
   */
  class Class {
    public:
      /**
       * @brief Class constructor brief (in 116_tagfile.cpp).
       *
       * Class constructor description (in 116_tagfile.cpp).
       */
      Class() : ClassMember(0) {}

      /**
       * @brief ClassMember brief (in 116_tagfile.cpp).
       *
       * ClassMember description (in 116_tagfile.cpp).
       */
      int ClassMember;
  };
}

/**
 * @file more_116/f.cpp
 *
 * more_116/f.cpp description (in 116_tagfile.cpp).
 */

/**
 * @file more_116/a/116_tagfile.cpp
 *
 * more_116/a/116_tagfile.cpp description (in 116_tagfile.cpp).
 */

/**
 * @file more_116/b/116_tagfile.cpp
 *
 * more_116/b/116_tagfile.cpp description (in 116_tagfile.cpp).
 */

/** @} */
