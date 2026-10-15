// objective: test that a member function template defined outside its namespace is not duplicated
// check: class_namespace_1_1_class.xml

/** @brief The reproducer for issue #12379. */
namespace Namespace {
/** @brief A class. */
class Class
{
 public:
  /** @brief A brief for the function.
   *  @param par A parameter.
   *  @return A returned value.
   */
  template <typename T>
  static Class function(T par);
};
}  // namespace Namespace

template <typename T>
Namespace::Class Namespace::Class::function(T par)
{
  return Namespace::Class{};
}
