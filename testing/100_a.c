!// objective: test documentation entry association with groups
!// input: more_100_b.c
!// input: more_100_c.c
!// check: group___a.xml
!// check: group___b.xml
!// check: group___c.xml

/**
 * @file
 *
 * @ingroup A
 *
 * @brief 100_a.c
 */

/**
 * @brief E in 100_a.c
 *
 * @ingroup A
 */
enum E {
  /**
   * @brief A in 100_a.c
   */
  A,

  /**
   * @brief B in 100_a.c
   */
  B
};
