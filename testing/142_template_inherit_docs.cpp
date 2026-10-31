// objective: inherit documentation through a templated intermediate base when the derived class has a typedef for its base (see issue #12421)
// check: struct_c.xml
// config: INHERIT_DOCS = YES
// config: WARN_IF_UNDOCUMENTED = YES
// config: EXTRACT_ALL = YES

/// Base interface with a documented virtual function.
/** @tparam T A type. */
template <typename T>
struct A
{
  /// Documented in base class.
  virtual void apply(const T &src) const;
};

/// Extends the class once.
/** @tparam T A type. */
template <typename T>
struct B : public A<T>
{
};

/// Extends the class another time, with an undocumented override.
/** @tparam U A type. */
template <typename U>
struct C : public B<U>
{
  /// Type of the base class.
  using BaseClass = B<U>;

  void apply(const U &src) const override;
};
