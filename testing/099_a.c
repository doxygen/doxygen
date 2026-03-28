// objective: test enum field association
// input: more_099_b.c
// check: 099__a_8c.xml
// check: more__099__b_8c.xml

/**
 * @file
 *
 * @brief a.c
 */

/**
 * @brief E in a.c
 */
enum E {
  /**
   * @brief A in a.c
   */
  A,

  /**
   * @brief B in a.c
   */
  B
};
