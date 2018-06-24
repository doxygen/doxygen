
/*! A test class. */

class Include_Test
{
  public:
    /// a member function
    void example();
};

/*! \page pag_example
 *  \dontinclude include_test.cpp
 *  Our main function starts like this:
 *  \skip main
 *  \until {
 *  First we create an object \c t of the Include_Test class.
 *  \skipline Include_Test
 *  Then we call the example member function 
 *  \line example
 *  After that our little test routine ends.
 *  \line }
 */
