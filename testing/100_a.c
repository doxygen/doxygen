// objective: test documentation entry association with groups
// input: more_100_b.c
// input: more_100_c.c
// input: more_100_d.cpp
// check: group___a.xml
// check: group___b.xml
// check: group___c.xml
// check: group___d.xml
// check: structs.xml

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

/**
 * @brief D in 100_a.c.
 *
 * @ingroup A
 */
#define D 1

/**
 * @brief f() in 100_a.c.
 *
 * @ingroup A
 */
static void f(void) {};

/**
 * @brief g() in 100_a.c.
 *
 * @ingroup A
 */
void g(void) {};

/**
 * @brief Struct s in 100_a.c.
 *
 * @ingroup A
 */
struct s {
  /**
   * @brief Member s.
   */
  int s;
};

/**
 * @brief T in 100_a.c.
 *
 * @ingroup A
 */
typedef struct s T;

/**
 * @brief i in 100_a.c.
 *
 * @ingroup A
 */
int i;

/**
 * @brief j in 100_a.c.
 *
 * @ingroup A
 */
static int j;
