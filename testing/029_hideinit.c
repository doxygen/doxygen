// objective: test the \hideinitializer command
// check: 029__hideinit_8c.xml

/** \file */

/** a variable with initializer visible */
int var1 = 10;

/** a variable without initializer visible 
 *  \hideinitializer
 */
int var2 = 20;
