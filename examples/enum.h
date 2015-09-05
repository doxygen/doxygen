class Enum_Test
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

/*! \class Enum_Test
 * The class description.
 */

/*! \enum Enum_Test::TEnum
 * A description of the enum type.
 */

/*! \var Enum_Test::TEnum Enum_Test::Val1
 * The description of the first enum value.
 */
