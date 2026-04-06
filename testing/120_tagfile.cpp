// objective: test the tagfile output
// config: GENERATE_TAGFILE = "$TESTOUTDIR/tagfile"
// config: STRIP_FROM_INC_PATH = "$INPUTDIR/more_120/include"
// check: 120__tagfile_8cpp.xml
// check: class_namespace_1_1_class.xml
// check: f_8cpp.xml
// check: group___group.xml
// check: more__120_2a_2120__tagfile_8cpp.xml
// check: more__120_2b_2120__tagfile_8cpp.xml
// check: namespace_namespace.xml
// check: s_2h_8h.xml
// check: struct_struct.xml
// check: t_2s_2h_8h.xml
// check: tagfile
// check: union_union.xml
// input: more_120/f.cpp
// input: more_120/a/120_tagfile.cpp
// input: more_120/b/120_tagfile.cpp
// input: more_120/include/s/h.h
// input: more_120/include/t/s/h.h

/**
 * @defgroup Group Group
 *
 * @brief Group brief (in 120_tagfile.cpp)
 *
 * Group description (in 120_tagfile.cpp).
 *
 * @{
 */

/**
 * @file
 *
 * @brief 120_tagfile.cpp brief (in 120_tagfile.cpp)
 *
 * 120_tagfile.cpp description (in 120_tagfile.cpp).
 */

/**
 * @brief Define brief (in 120_tagfile.cpp).
 *
 * Define description (in 120_tagfile.cpp).
 */
#define Define 123

/**
 * @brief Macro brief (in 120_tagfile.cpp).
 *
 * Macro description (in 120_tagfile.cpp).
 *
 * @param x is the parameter.
 */
#define Macro(x) ((x) * 456)

/**
 * @brief Object brief (in 120_tagfile.cpp).
 *
 * Object description (in 120_tagfile.cpp).
 */
extern int Object;

/**
 * @brief Enum brief (in 120_tagfile.cpp).
 *
 * Enum description (in 120_tagfile.cpp).
 */
enum Enum {
  /**
   * @brief EnumVal brief (in 120_tagfile.cpp).
   *
   * EnumVal description (in 120_tagfile.cpp).
   */
  EnumVal
};

/**
 * @brief EnumTypedef brief (in 120_tagfile.cpp).
 *
 * EnumTypedef description (in 120_tagfile.cpp).
 */
typedef Enum EnumTypedef;

/**
 * @brief Struct brief (in 120_tagfile.cpp).
 *
 * Struct description (in 120_tagfile.cpp).
 */
struct Struct {
  /**
   * @brief StructMember brief (in 120_tagfile.cpp).
   *
   * StructMember description (in 120_tagfile.cpp).
   */
  int StructMember;
};

/**
 * @brief Union brief (in 120_tagfile.cpp).
 *
 * Union description (in 120_tagfile.cpp).
 */
union Union {
  /**
   * @brief UnionMember brief (in 120_tagfile.cpp).
   *
   * UnionMember description (in 120_tagfile.cpp).
   */
  int UnionMember;
};

/**
 * @brief Function brief (in 120_tagfile.cpp).
 *
 * Function description (in 120_tagfile.cpp).
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
 * @brief InlineFunction brief (in 120_tagfile.cpp).
 *
 * InlineFunction description (in 120_tagfile.cpp).
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
 * @brief Namespace brief (in 120_tagfile.cpp).
 *
 * Namespace description (in 120_tagfile.cpp).
 */
namespace Namespace {
  /**
   * @brief Class brief (in 120_tagfile.cpp).
   *
   * Class description (in 120_tagfile.cpp).
   */
  class Class {
    public:
      /**
       * @brief Class constructor brief (in 120_tagfile.cpp).
       *
       * Class constructor description (in 120_tagfile.cpp).
       */
      Class() : ClassMember(0) {}

      /**
       * @brief ClassMember brief (in 120_tagfile.cpp).
       *
       * ClassMember description (in 120_tagfile.cpp).
       */
      int ClassMember;
  };
}

/**
 * @file more_120/f.cpp
 *
 * more_120/f.cpp description (in 120_tagfile.cpp).
 */

/**
 * @file more_120/a/120_tagfile.cpp
 *
 * more_120/a/120_tagfile.cpp description (in 120_tagfile.cpp).
 */

/**
 * @file more_120/b/120_tagfile.cpp
 *
 * more_120/b/120_tagfile.cpp description (in 120_tagfile.cpp).
 */

/** @} */
