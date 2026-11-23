// objective: document struct and union fields by name
// input: more_157_field_command.c
// check: struct_coordinates.xml
// check: union_value.xml
// check: structspace_1_1_record.xml
// check: struct_box.xml
// check: struct_only_fields.xml
// check: struct_c_pair.xml
// check: union_c_value.xml
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
 *  \field value Field documented separately.
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
