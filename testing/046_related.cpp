// objective: test the \related, \relatedalso, \see, and \sa commands
// check: class_test.xml
// check: 046__related_8cpp.xml

/** @file */

/** A test class 
 *  @see Test::method()
 */
class Test 
{
  public:
    /** A method */
    void method();
};

/*!
 * A function.
 * \related Test
 * \sa another()
 */
void function();

/*!
 * Another function
 * \relatedalso Test
 */
void another();
