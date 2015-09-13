class Fn_Test
{
  public:
    const char *member(char,int) throw(std::out_of_range);
};

const char *Fn_Test::member(char c,int n) throw(std::out_of_range) {}

/*! \class Fn_Test
 * \brief Fn_Test class.
 *
 * Details about Fn_Test.
 */

/*! \fn const char *Fn_Test::member(char c,int n) 
 *  \brief A member function.
 *  \param c a character.
 *  \param n an integer.
 *  \exception std::out_of_range parameter is out of range.
 *  \return a character pointer.
 */
