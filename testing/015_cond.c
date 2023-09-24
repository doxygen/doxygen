// objective: test the `cond` command
// check: 015__cond_8c.xml
// config: ENABLED_SECTIONS = COND_ENABLED

/** \file
 *  Text \a argument more text.
 */

/// \cond

/** A function */
void func();

/** A macro */
#define MACRO 42

/// \endcond

/// \cond COND_ENABLED
/// Function to be shown.
void cond_enabled()
{
}
/// \endcond

/** \cond COND_DISABLED 
    Function not to be shown. */
void cond_disabled()
{
}
/** \endcond */

