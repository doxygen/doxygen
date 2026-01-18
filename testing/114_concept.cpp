// objective: test C++20 concept with embedded doxygen style comments
// check: concept_hashable.xml

/// Declaration of the concept "Hashable", which is satisfied by any type `T`
template<typename T>
concept Hashable = requires(T a)
{
    /// For values `a` of type `T`, the expression `std::hash<T>{}(a)`
    /// should compile and its result is convertible to `std::size_t`
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};
