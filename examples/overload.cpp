class Overload_Test 
{
  public:
    void drawRect(int,int,int,int);
    void drawRect(const Rect &r);
};

void Overload_Test::drawRect(int x,int y,int w,int h) {}
void Overload_Test::drawRect(const Rect &r) {}

/*! \class Overload_Test
 *  \brief A short description.
 *   
 *  More text.
 */

/*! \fn void Overload_Test::drawRect(int x,int y,int w,int h)
 * This command draws a rectangle with a left upper corner at ( \a x , \a y ),
 * width \a w and height \a h. 
 */

/*!
 * \overload void Overload_Test::drawRect(const Rect &r)
 */

