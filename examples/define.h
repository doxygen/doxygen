/*! \file define.h
    \brief testing defines
    
    This is to test the documentation of defines.
*/

/*!
  \def MAX(x,y)
  Computes the maximum of \a x and \a y.
*/

/*! 
   Computes the absolute value of its argument \a x.
*/
#define ABS(x) (((x)>0)?(x):-(x))
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)>(y)?(y):(x)) 
        /*!< Computes the minimum of \a x and \a y. */
