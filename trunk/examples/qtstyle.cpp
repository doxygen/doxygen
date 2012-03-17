//!  A test class. 
/*!
  A more elaborate class description.
*/

class Test
{
  public:

    //! An enum.
    /*! More detailed enum description. */
    enum TEnum { 
                 TVal1, /*!< Enum value TVal1. */  
                 TVal2, /*!< Enum value TVal2. */  
                 TVal3  /*!< Enum value TVal3. */  
               } 
         //! Enum pointer.
         /*! Details. */
         *enumPtr, 
         //! Enum variable.
         /*! Details. */
         enumVar;  
    
    //! A constructor.
    /*!
      A more elaborate description of the constructor.
    */
    Test();

    //! A destructor.
    /*!
      A more elaborate description of the destructor.
    */
   ~Test();
    
    //! A normal member taking two arguments and returning an integer value.
    /*!
      \param a an integer argument.
      \param s a constant character pointer.
      \return The test results
      \sa Test(), ~Test(), testMeToo() and publicVar()
    */
    int testMe(int a,const char *s);
       
    //! A pure virtual member.
    /*!
      \sa testMe()
      \param c1 the first argument.
      \param c2 the second argument.
    */
    virtual void testMeToo(char c1,char c2) = 0;
   
    //! A public variable.
    /*!
      Details.
    */
    int publicVar;
       
    //! A function variable.
    /*!
      Details.
    */
    int (*handler)(int a,int b);
};

