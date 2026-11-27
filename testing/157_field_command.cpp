// objective: document struct and union fields by name
// input: more_157_field_command.c
// check: struct_coordinates.xml
// check: union_value.xml
// check: structspace_1_1_record.xml
// check: struct_box.xml
// check: struct_only_fields.xml
// check: struct_c_pair.xml
// check: union_c_value.xml
// check: struct_explicit.xml
// check: structspace_1_1_external.xml
// check: structfoo3.xml
// check: structspace_1_1_later.xml
// check: struct_mixed.xml
// config: WARN_IF_UNDOCUMENTED = NO

/** Coordinates.
 *  \field x Horizontal \b coordinate, continued
 *            on the next line.
 *  @field y Vertical coordinate.
 *  @field _other Another field.
 *
 *  More about the coordinates.
 *  \note A note about the struct.
 */
struct Coordinates
{
  int x, y;
  long _other;
};

/** A value.
 *  @field integer An integer.
 *  @field real A real.
 */
union Value
{
  int integer;
  double real;
};

namespace space
{
/** \struct Record
 *  A record.
 *  \field Record::value Field documented separately.
 */
struct Record
{
  int value; ///< Existing documentation.
};
}

/** A box.
 *  @field data The stored value.
 */
template<typename T> struct Box
{
  T data;
};

/** @field field Documentation without a separate struct description. */
struct OnlyFields
{
  int field;
};

/**
 * @struct Explicit
 * @brief An explicitly documented struct.
 * @field Explicit::first First field.
 * @field Explicit::second Second field.
 */
struct Explicit
{
  int first, second;
};

namespace space
{
struct External { int first, second, third, fourth; };
}

/**
 * @struct space::External
 * @brief An out-of-place struct description.
 * @field External::first First qualified field.
 * @field space::External::second Second qualified field.
 * @field ::space::External::third Third qualified field.
 * @field fourth An unqualified field.
 */

/** @struct foo3
 * @brief the brief description
 */
struct foo3
{
  int bar3;
  int baz3;
};

/**
 * @field foo3::bar3
 * First member.
 * @field foo3::baz3
 * Second member.
 * @field ::space::Later::other A field of another struct.
 */

namespace space
{
/** A namespaced struct. */
struct Later { int value, other; };

/** @field Later::value A namespaced standalone field. */
}

/** @field Mixed::first A qualified field.
 *  @field second An unqualified field.
 */
struct Mixed { int first, second; };
