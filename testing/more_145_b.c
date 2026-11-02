/** @file */

/** @brief Helper of the other file. */
static int helper(void) { return 2; }

/** @copydoc helper() */
int other_api(void) { return helper(); }
