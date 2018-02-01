// objective: test \link command with function variadic arguments '...'
// check: 067__link__varargs_8cpp.xml

/** \file
 *  See \link func(int,...) the function\endlink for more info.
 *  See the \link Test test\endlink class.
 */

/** A function
 */
void func(int p);

/** Overloaded function taking variadic arguments
 */
void func(int p, ...);

/** A test */
class Test
{
};
