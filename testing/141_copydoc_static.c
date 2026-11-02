// objective: test that \copydoc of a file-scope static uses the static of its own file, see issue #12369
// input: more_141_b.c
// check: 141__copydoc__static_8c.xml
// check: more__141__b_8c.xml
/** @file */

/** @brief Helper of this file. */
static int helper(void) { return 1; }

/** @copydoc helper() */
int api(void) { return helper(); }
