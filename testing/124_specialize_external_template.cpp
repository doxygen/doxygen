// objective: test that specializing a template declared outside the input does not warn about a missing scope
// check: structstd_1_1hash_3_01_foo_01_4.xml

/** @brief A class of our own. */
struct Foo {};

/** @brief `std::hash` is not part of the input, so this specialization can only be spelled qualified. */
template<>
struct std::hash<Foo>
{
  /** @brief Hashes a Foo. */
  unsigned long operator()(const Foo &) const noexcept { return 0; }
};
