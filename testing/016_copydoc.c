// objective: test the \copydoc, \copybrief, \copydetails, and \details commands
// check: 016__copydoc_8c.xml

/** \file
 *  Text \a argument more text.
 */

/** \brief Brief description.
 *  \details Detailed description.
 */
void func(int i);

/** \copybrief func(int) */
void func_brief();

/** \copydetails func(int) */
void func_details();

/** \copydoc func(int)
 *  More text.
 */
void func_doc();

