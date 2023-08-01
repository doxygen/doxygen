// objective: test the \overload command
// check: class_test.xml
class Rect;
class Point;
class Test 
{
  public:
    void drawRect(int,int,int,int);
    void drawRect(const Rect &r);
    void drawRect(const Point &topLeft,const Point &bottomRight);
};

void Test::drawRect(int x,int y,int w,int h) {}
void Test::drawRect(const Rect &r) {}
/*! \overload 
 *
 *  More text.
 */
void Test::drawRect(const Point &topLeft,const Point &bottomRight) {}

/*! \class Test
 *  \brief A short description.
 *   
 *  More text.
 */

/*! \fn void Test::drawRect(int x,int y,int w,int h)
 * This command draws a rectangle with a left upper corner at ( \a x , \a y ),
 * width \a w and height \a h. 
 */

/*!
 * \overload void Test::drawRect(const Rect &r)
 */
