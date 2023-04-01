// objective: test the \def, \var, \fn, and \typedef commands
// check: 018__def_8c.xml

/** \file
 *  Text \a argument more text.
 */

#define MACRO 42
int var = 10;
void func(int) {}
void func(int,const char *) {}
typedef int Type;
enum E { E1, E2 };

/** \def MACRO
 *  A macro definition 
 */

/** Another macro definition.
 */
#define MACRO4 2

/** \var var
 *  A variable
 */

/** \fn func(int)
 *  A function with one parameter.
 */

/** \fn func(int,const char *)
 *  A function with two parameters
 */

/** \typedef Type
 *  A type definition.
 */

/** \enum E
 *  An enum
 */
