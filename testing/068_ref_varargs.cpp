// objective: test \ref command with function variadic arguments '...'
// check: 068__ref__varargs_8cpp.xml

/** \file
 *  See \ref func(int,...) "the function" for more info.
 *  See the \ref Test "test" class.
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
