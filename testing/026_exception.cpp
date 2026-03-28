// objective: test the \exception, \param, and \tparam commands
// check: class_test.xml
#include <stdexcept>
/** A Test class.
 *  More details about this class.
 *  @tparam T A template parameter.
 */
template<class T> class Test
{
  public:
    /** An example member function.
     *  \param p1 First parameter.
     *  \param p2 Second parameter.
     *  \exception std::out_of_range parameter is out of range. 
     *  \retval  0 if p1 and p2 are equal
     *  \retval -1 if p1 is smaller than p2
     *  \retval  1 if p1 is bigger than p2
     */
    int example(int p1,int p2) throw(std::out_of_range);
};

