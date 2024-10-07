// objective: test typedef deduplication that properly considers scopes
// check: namespace_root.xml
// check: namespace_root_1_1_sub.xml
// check: struct_root_1_1_sub_1_1_struct.xml

namespace Root {

/** @brief A typedef */
typedef int Int;

/* Documentation comes later */
using Float = float;

namespace Sub {
  /* Documentation comes later */
  typedef double Double;

  /** @brief A struct */
  struct Struct {
    /** @brief A typedef that doesn't get merged with the one in Sub */
    typedef double Double;
  };

  /** @relatedalso Struct
   * @brief A typedef in Sub that doesn't get merged with the one in Root
   */
  typedef int Int;

  /** @relatedalso Struct
   * @brief A using declaration in Sub doesn't get merged with the one in Root
   */
  using Float = float;

  /** @brief Another typedef in Sub, not related to @ref Struct */
  typedef double Double;
}

/** @brief A using declaration */
using Float = float;

/* Documentation comes later */
typedef bool BoolShouldNotBecomeFunction;

/** @brief Another struct */
struct AnotherStruct {};

/** @relatedalso AnotherStruct
 * @brief A typedef that shouldn't become a function
 */
typedef bool BoolShouldNotBecomeFunction;

}
