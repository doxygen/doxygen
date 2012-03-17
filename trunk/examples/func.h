class Test
{
  public:
    const char *member(char,int) throw(std::out_of_range);
};

const char *Test::member(char c,int n) throw(std::out_of_range) {}

/*! \class Test
 * \brief Test class.
 *
 * Details about Test.
 */

/*! \fn const char *Test::member(char c,int n) 
 *  \brief A member function.
 *  \param c a character.
 *  \param n an integer.
 *  \exception std::out_of_range parameter is out of range.
 *  \return a character pointer.
 */
