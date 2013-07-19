// objective: test the \name and \short commands
// check: class_test.xml

/** \short A Test class.
 *
 *  More details about this class.
 */
class Test
{
  public:
    /** \name A group of functions.
     *  \{
     */

    /** foo function */
    void foo();
    /** bar function */
    void bar();

    /** \} */

    /** ungrouped function */
    void ungrouped();
};

