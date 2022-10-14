// objective: test the \defgroup, \addtogroup, and \ingroup command.
// check: group__g1.xml
// check: group__g2.xml
// check: group__g3.xml
// check: group___a.xml
// check: group___b.xml
// check: group___c.xml
// check: group___d.xml

/** \defgroup g1 First Group
 *  Text for first group.
 */

/** A function in the first group.
 *  \ingroup g1
 */
void func_g1();

//--------------------------------

/** \defgroup g2 Second Group
 *  Text for second group.
 */
/// \{

/** A function in the second group */
void func_g2();

/// \}

/** \defgroup g3 Third Group
 *  Text for third group.
 *  \ingroup g2
 *  \{
 */

//--------------------------------

/** A function in the third group */
void func_g3();

/** \} */

/** \addtogroup g3
 *  \{
 */

/** Another function added to the third group */
void func_g3_add();

/** \} */

/**
  @defgroup A A
  @{
 */

/** @defgroup B B
   @{
 */

/// \}

/** @defgroup C C
   @{
 */

/** @} */

/**
  @defgroup D D
  @{
 */

/// @}

/**
@}
*/
