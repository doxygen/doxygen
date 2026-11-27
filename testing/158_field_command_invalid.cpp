// objective: field commands document class variables but reject invalid targets
// check: struct_derived.xml
// check: struct_base.xml
// check: class_not_a_struct.xml
// config: EXTRACT_ALL = YES
// config: WARN_IF_DOC_ERROR = NO
// config: WARN_IF_UNDOCUMENTED = NO

struct Base
{
  int inherited;
};

/** Derived.
 *  @field inherited Not a direct field.
 *  @field method Not a field.
 *  @field shared Not an instance field.
 *  @field missing Not declared. @field Base::unrelated Wrong class.
 *  @field own A direct field.
 */
struct Derived : Base
{
  int own, unrelated;
  void method();
  static int shared;
};

/** A class.
 *  @field field A class field.
 *  @field shared A static class variable.
 *  @field method Not a class variable.
 */
class NotAStruct
{
public:
  int field;
  static int shared;
  void method();
};

/** @field Missing::value Unknown class.
 *  @field Derived::method A function is not a field.
 *  @field Derived::shared A static struct member.
 *  @field Derived::inherited An inherited field.
 */
