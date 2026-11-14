// objective: test that the brief description of an \example is not repeated in its detailed description (see issue #10256)
// check: example_test_8cpp-example.xml

/** \brief A Test class. */
class Test
{
  public:
    /** \brief An example member function. */
    void example();
};

/** \example example_test.cpp
 *  \brief Brief description of the example.
 *
 *  Detailed description of the example.
 */
