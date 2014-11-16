// objective: test linking to the cast operator with and without const
// check: struct_foo.xml
/**
 * @brief Foo
 *
 * - No autolink for operator int()
 * - @ref operator int()
 * - @ref operator int() "title"
 * - Foo::operator int()
 * - @ref Foo::operator int()
 * - @ref Foo::operator int() "title"
 *
 * - No autolink for operator int() const
 * - @ref operator int() const
 * - @ref operator int() const "title"
 * - Foo::operator int() const
 * - @ref Foo::operator int() const
 * - @ref Foo::operator int() const "title"
 */
struct Foo {
  /** @brief Conversion to int */
  operator int();
  /** @brief Conversion to int const */
  operator int() const;
};
