// objective: test static assertions
// check: 101__static__assert_8c.xml

/**
 * @file
 *
 * @code{.java}
 * public class static_assert {
 *   public static void static_assert() {}
 *   public static void _Static_assert() {}
 * }
 * @endcode
 */

static_assert(0 == 0);
static_assert(1 == 1, "m1");
_Static_assert(2 == 2);
_Static_assert(3 == 3, "m3");
