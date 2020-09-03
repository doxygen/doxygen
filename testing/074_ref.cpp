// objective: test \ref command in combination with const
// check: struct_foo.xml
// check: namespacens.xml

#include <initializer_list>

/** @brief Foo class.
 *
 *  @see @ref Foo::Foo() constructor for details.
 *  @see @ref Foo constant.
 *  @see @ref operator<<(int) "less than operator".
 *  @see @ref operator<<(int) const "const less than operator".
 *  @see @ref operator()(int) "call operator".
 *  @see @ref operator()(int) const "const call operator".
 *  @see @ref operator&=(const Foo&) "and equal operator".
 *  @see @ref operator&=(const Foo&) const "const and equal operator".
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
  const Foo& operator&=(const Foo& rhs) const;

  /** Member pointer operator */
  int* operator->*(int *p);

  /** @brief Fun with itself */
  Foo fun() const;
};

/**
@brief A namespace

- Link to an UDL w/o spaces: @link operator""_op @endlink
- Link to an UDL with spaces: @link operator""_oq @endlink
- Link to a function with spaces: @ref foo(std::initializer_list< int* >)
- Link to a function w/o spaces: @ref foo(std::initializer_list<int*>)
*/
namespace ns {

/** @brief An operator */
int operator""_op(unsigned long long);

/** @brief Another operator */
int operator "" _oq(unsigned long long);

/** @brief Function */
void foo(std::initializer_list<int*>);

}
