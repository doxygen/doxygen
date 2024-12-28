// objective: test type resolution with combinations of using/namespace/template
// check: namespacebb.xml
// check: namespaceitpp.xml

//! namespace itpp
namespace itpp
{
  //! Vector
  template<class Num_T>
  class Vec {
    //! Set the vector equal to the values in the \c str string
    bool set(const char *str);
  };

  //! Fixed complex data type
  class CFix {};

  //! Typedef for complex fixed-point vector type
  typedef Vec<CFix> cfixvec;

  //! setter
  template<>
  bool cfixvec::set(const char *values)
  {
    return true;
  }

} // namespace itpp

//! namespace aa
namespace aa
{
  /**
   * @brief Struct1
   */
  struct Struct1
  {
    int struct1_0; //!< field 1_0
    int struct1_1; //!< field 1_1
  };

  /**
   * @brief Struct2
   */
  struct Struct2
  {
    int struct2_0; //!< field 2_0
    int struct2_1; //!< field 2_1
  };

} // namespace aa

//! namespace bb
namespace bb {

/**
 * @brief TemplateStruct
 */
template <typename T>
struct TemplateStruct
{
    T data;             //!< data
};

/**
 * @brief alias of Structt1 in bb
 */
 using Struct1 = aa::Struct1;

 /**
 * @brief alias of Structt2 in bb
 */
 using Struct2 = aa::Struct2;

 /**
  * @brief Struct1Type
  */
 using Struct1Type = TemplateStruct<aa::Struct1>;

 /**
  * @brief Struct2Type
  */
 using Struct2Type = TemplateStruct<aa::Struct2>;

 /**
    * @brief   FunctionExample (overload 1)
    *
    * @param[out] arg1 Struct1Type argument
    * @param[in] arg2 int argument
    *
    * @return return value
  */
int FunctionExample(Struct1Type arg1, int arg2);

/**
    * @brief   FunctionExample (overload 2)
    *
    * @param[out] arg1 Struct2Type argument
    * @param[in] arg2 int argument
    *
    * @return return value
  */
int FunctionExample(Struct2Type arg1, int arg2);

} // namespace bb

