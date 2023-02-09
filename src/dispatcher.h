/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <cstddef>
#include <utility>
#include <functional>
#include <variant>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////////////////////////////
// idea based on https://mpark.github.io/programming/2019/01/22/variant-visitation-v2/

namespace detail
{

//! Represents an unreachable piece of code
#ifdef __GNUC__ // GCC 4.8+, Clang, Intel and other compilers compatible with GCC (-std=c++0x or above)
[[noreturn]] inline __attribute__((always_inline)) void unreachable() { __builtin_unreachable(); }
#elif defined(_MSC_VER) // MSVC
[[noreturn]] __forceinline void unreachable() { __assume(false); }
#else // ???
#warning "no implementation of unreachable for this compiler!"
inline void unreachable() {}
#endif

//! generic template declaration for the Dispatcher without implementation
template<bool valid>
struct Dispatcher;

//! specialization for the invalid case, signaling the compiler that the path is unreachable
template<>
struct Dispatcher<false>
{
  //! case corresponding with the non-existing I'th type of variant V
  template<template<class> class W, std::size_t I, class V, class... As>
  static constexpr void case_(V &&, As &&...) { unreachable(); }
};

//! specialization for the valid case, where `case_` invokes a specific method.
template<>
struct Dispatcher<true>
{
  //! Invokes the method of a class `X` whose method pointer type is defined by `W<X>::method`
  //! where `X` matches the I'th type of variant `V` on the object held by `v`.
  template<template<class> class W, std::size_t I, class V, class... As>
  static constexpr void case_(V &&v, As &&... args) {
    using Alt = std::variant_alternative_t<I,std::decay_t<V>>;
    std::invoke( W<Alt>::method, std::get<I>( std::forward<V>(v) ), std::forward<As>(args)... );
  }
};

} // namespace detail

//! Invokes the method of a class `X` whose member pointer type is defined by `W<X>::method`
//! where `X` matches the type of the object held by `v` that is stored in a variant `V`.
//! \tparam W    a template class where `W<X>::method` points to a method of class `X` one of the types of `V`.
//! \tparam V    the type of the variant
//! \tparam As   the parameter types used to invoke the method
//! \param  v    a object of the variant type for which to invoke the method
//! \param  args the parameters to pass to the method
//! \note This implementation assumes a maximum of 10 types in the variant (easy to extend though by adding more cases).
template<template<class> class W,class V, class...As>
static constexpr void dispatch_call(V &&v,As &&... args)
{
  // size holds the number of type of variant `V`
  constexpr std::size_t size = std::variant_size_v<std::decay_t<V>>;
  // which on a worst case maximum number of types in `V`.
  // fills in an unreachable branch for indices equal or higher than size.
  switch (v.index())
  {
#define DISPATCH_AT(n)            \
    case n: detail::Dispatcher< ((n)<size) >::template case_<W,n>(v,std::forward<As>(args)...); break;

    DISPATCH_AT(0)
    DISPATCH_AT(1)
    DISPATCH_AT(2)
    DISPATCH_AT(3)
    DISPATCH_AT(4)
    DISPATCH_AT(5)
    DISPATCH_AT(6)
    DISPATCH_AT(7)
    DISPATCH_AT(8)
    DISPATCH_AT(9)
    DISPATCH_AT(10)
    DISPATCH_AT(11)
    DISPATCH_AT(12)
    DISPATCH_AT(13)
    DISPATCH_AT(14)
    DISPATCH_AT(15)
    DISPATCH_AT(16)
    DISPATCH_AT(17)
    DISPATCH_AT(18)
    DISPATCH_AT(19)

#undef DISPATCH_AT
  }
}

#endif // DISPATCHER_H
