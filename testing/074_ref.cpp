// objective: test \ref command in combination with const
// check: struct_foo.xml
/** @brief Foo class.
 *
 *  @see @ref Foo::Foo() constructor for details.
 *  @see @ref Foo constant.
 *  @see @ref operator<<(int) "less than operator".
 *  @see @ref operator<<(int) const "const less than operator".
 *  @see @ref operator()(int) "call operator".
 *  @see @ref operator()(int) const "const call operator".
 *  @see @ref operator&=(const Foo&) "and equal operator".
 *  @see @ref operator->*(int *) "member pointer operator".
 */
struct Foo {
  /** Constructor */
  Foo();
  /**
   * @brief Fun of two
   *
   * - fun() const
   * - @ref fun() const
   * - @ref fun() const "title"
   */
  static Foo fun(Foo a, Foo b);

  /** overloaded less than operator */
  Foo& operator<< (int i);

  /** overloaded const less than operator */
  const Foo& operator<< (int i) const;

  /** overloaded call operator */
  int operator()(int i);

  /** overloaded call operator */
  int operator()(int i) const;

  /** and equal operator */
  Foo& operator&=(const Foo& rhs);

  /** and equal operator */
  Foo& operator&=(const Foo& rhs);

  /** Member pointer operator */
  int* operator->*(int *p);

  /** @brief Fun with itself */
  Foo fun() const;
};
