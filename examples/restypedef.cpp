/*! \file restypedef.cpp
 * An example of resolving typedefs.
 */

/*! \struct CoordStruct
 * A coordinate pair.
 */
struct CoordStruct
{
  /*! The x coordinate */
  float x;
  /*! The y coordinate */
  float y;
};

/*! Creates a type name for CoordStruct */ 
typedef CoordStruct Coord;

/*! 
 * This function returns the addition of \a c1 and \a c2, i.e:
 * (c1.x+c2.x,c1.y+c2.y)
 */
Coord add(Coord c1,Coord c2)
{
}
