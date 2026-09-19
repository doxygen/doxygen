// objective: test that a qualified specialization of a variable template is documented in its namespace
// check: namespacestd_1_1ranges.xml
// check: namespacens.xml

/** @brief A view type of our own. */
template<class T>
struct MyView {};

/** @brief `std::ranges::enable_view` is not part of the input, so this specialization can only be spelled qualified. */
template<class T>
constexpr bool std::ranges::enable_view<MyView<T>> = true;

namespace ns
{
/** @brief A variable template that is part of the input. */
template<class T> constexpr bool trait = false;
}

/** @brief A specialization must not be merged into the primary template above. */
template<class T> constexpr bool ns::trait<MyView<T>> = true;
