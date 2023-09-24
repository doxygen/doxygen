// objective: test the \class and \headerfile commands
// check: class_t1.xml
// check: class_t2.xml
// check: class_t3.xml
// check: class_t4.xml

/** A class
 *  \headerfile 013_class.h "inc/013_class.h"
 */
class T1
{
};

class T2
{
};

class T3
{
};

class T4
{
};

/** \class T2
 *  \headerfile <>
 *  class T2
 */

/** \class T3 013_class.h
 *  class T3
 */

/** \class T4 013_class.h "inc/013_class.h"
 *  class T4
 */
