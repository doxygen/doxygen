class Test
{
  public:
    enum TEnum { Val1, Val2 };

    /*! Another enum, with inline docs */
    enum AnotherEnum 
    { 
      V1, /*!< value 1 */
      V2  /*!< value 2 */
    };
};

/*! \class Test
 * The class description.
 */

/*! \enum Test::TEnum
 * A description of the enum type.
 */

/*! \var Test::TEnum Test::Val1
 * The description of the first enum value.
 */
