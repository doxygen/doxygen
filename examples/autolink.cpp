/*! \file autolink.cpp
  Testing automatic link generation.
  
  A link to a member of the Autolink_Test class: Autolink_Test::member, 
  
  More specific links to the each of the overloaded members:
  Autolink_Test::member(int) and Autolink_Test#member(int,int)

  A link to a protected member variable of Autolink_Test: Autolink_Test#var, 

  A link to the global enumeration type #GlobEnum.
 
  A link to the define #ABS(x).
  
  A link to the destructor of the Autolink_Test class: Autolink_Test::~Autolink_Test, 
  
  A link to the typedef ::B.
 
  A link to the enumeration type Autolink_Test::EType
  
  A link to some enumeration values Autolink_Test::Val1 and ::GVal2
*/

/*!
  This documentation block belongs to the class Autolink_Test which can also be
  linked with #Autolink_Test.

  To link to a constructor: #Autolink_Test() and Autolink_Test().

  Links to the destructor are: #~Autolink_Test and ~Autolink_Test().
  
  A link to a member in this class: member().

  More specific links to the each of the overloaded members: 
  member(int) and member(int,int). 
  
  A link to the variable #var.

  A link to the global typedef ::B.

  A link to the global enumeration type #GlobEnum.
  
  A link to the define ABS(x).
  
  A link to a variable \link #var using another text\endlink as a link.
  
  A link to the enumeration type #EType.

  A link to some enumeration values: \link Autolink_Test::Val1 Val1 \endlink and ::GVal1.

  And last but not least a link to a file: autolink.cpp.
  
  \sa Inside a see also section any word is checked, so EType, 
      Val1, GVal1, ~Autolink_Test and member will be replaced by links in HTML.
*/

class Autolink_Test
{
  public:
    Autolink_Test();               //!< constructor 
   ~Autolink_Test();               //!< destructor 
    void member(int);     /**< A member function. Details. */
    void member(int,int); /**< An overloaded member function. Details */

    /** An enum type. More details */
    enum EType { 
      Val1,               /**< enum value 1 */ 
      Val2                /**< enum value 2 */ 
    };                

  protected:
    int var;              /**< A member variable */
};

/*! details. */
Autolink_Test::Autolink_Test() { }

/*! details. */
Autolink_Test::~Autolink_Test() { }

/*! A global variable. */
int globVar;

/*! A global enum. */
enum GlobEnum { 
                GVal1,    /*!< global enum value 1 */ 
                GVal2     /*!< global enum value 2 */ 
              };

/*!
 *  A macro definition.
 */ 
#define ABS(x) (((x)>0)?(x):-(x))

typedef Autolink_Test B;

/*! \fn typedef Autolink_Test B
 *  A type definition. 
 */
