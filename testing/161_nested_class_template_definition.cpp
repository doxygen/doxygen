// objective: test that the members of nested classes defined outside a class template are kept when the definition uses another template parameter name, see issue #11216
// check: struct_a_1_1_b.xml
// check: struct_a_1_1_b_1_1_c.xml

/// @brief Outer class template.
template <typename T>
struct A
{
    struct B;
};

/// @brief Nested class.
template <typename U>
struct A<U>::B
{
    using Outer = A<U>; //!< The enclosing class.
    int value;          //!< The value.
    struct C;
};

/// @brief Class nested two levels deep.
template <typename U>
struct A<U>::B::C
{
    C() = default; //!< Constructor.
    void f();      //!< Does something.
};
