// objective: test that overloads that only differ in their template parameters get different anchors, see issue #9468
// check: class_conv.xml
// check: namespacens.xml

/// A class template
template <typename A>
class Conv
{
  public:
    /// Implicit converting constructor
    template <typename B = A,
              typename std::enable_if_t<std::is_convertible_v<const B&, A>>* = nullptr>
    constexpr Conv(const B &b) noexcept {}

    /// Explicit converting constructor
    template <typename B,
              typename std::enable_if_t<!std::is_convertible_v<const B&, A>>* = nullptr>
    constexpr explicit Conv(const B &b) noexcept {}
};

/// A namespace
namespace ns
{
/// Overload for integral types
template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
void f(T value);

/// Overload for floating point types
template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
void f(T value);
}
