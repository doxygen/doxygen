// objective: test noexcept with expression and nodiscard and constexpr
// check: class_a.xml
//
#ifndef INCLUDE_GUARD
#define INCLUDE_GUARD

#include <type_traits>

/// @brief Some class
class A
{
  public:
    /// @brief Does stuff
    /// @details Details
    [[nodiscard]] constexpr int do_stuff() noexcept(std::is_nothrow_move_assignable_v<A>)
    {
    }

    /// @brief Does stuff
    /// @details Details
    [[nodiscard]] constexpr auto do_stuff2() noexcept(std::is_nothrow_move_assignable_v<A>) -> int
    {
    }
};

#endif
