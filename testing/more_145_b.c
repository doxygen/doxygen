/** @file */

/** @brief Helper of the other file. */
static int helper(void) { return 2; }

/** @copydoc helper() */
int other_api(void) { return helper(); }

/** @brief Cross-file helper. */
static int cross_helper(void) { return 3; }
