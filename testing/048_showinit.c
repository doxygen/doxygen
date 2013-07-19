// objective: test the \showinit command
// check: 048__showinit_8c.xml
// config: MAX_INITIALIZER_LINES = 0

/** \file */

/** a variable with initializer hidden due to MAX_INITIALIZER_LINES */
int var1 = 10;

/** a variable with initializer visible 
 *  \showinitializer
 */
int var2 = 20;
