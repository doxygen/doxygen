// objective: test that a (partial) specialization does not get the members and base classes of the primary template, see issue #11541
// check: structt_3_01int_01_4.xml
// check: structt_3_01_t_01_5_01_4.xml
// check: structw.xml
/** @file */

/** @brief First base. */
struct Base1 {};

/** @brief Second base. */
struct Base2 {};

/** @brief Primary template. */
template<class T>
struct t : public Base1
{
  /** @brief Type of the primary template. */
  using A = T;
  /** @brief Function of the primary template. */
  T get() const;
};

/** @brief Explicit specialization. */
template<>
struct t<int> : public Base2
{
  /** @brief Type of the explicit specialization. */
  using B = int;
};

/** @brief Partial specialization. */
template<class T>
struct t<T*>
{
  /** @brief Variable of the partial specialization. */
  T *ptr;
};

/** @brief Derived from an instance of the primary template. */
struct w : public t<char>
{
};

/** @brief Function made a member of the primary template by its documentation.
 *  @memberof t
 */
void h();
