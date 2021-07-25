/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#ifndef VARIANT_H
#define VARIANT_H

#include <typeinfo>
#include <type_traits>
#include <string>
#include <utility>

namespace details
{

//----- template helper class to compute the maximum over a set of template arguments

// generic declaration
template <size_t arg1, size_t ... others>
struct TMax;

// specialization to stop the recursion for a single argument
template <size_t arg>
struct TMax<arg>
{
  static const size_t value = arg;
};

// recursive definition for multiple arguments
template <size_t arg1, size_t arg2, size_t ... others>
struct TMax<arg1, arg2, others...>
{
  static const size_t value = arg1 >= arg2 ?
        TMax<arg1, others...>::value :
        TMax<arg2, others...>::value ;
};

//------ helper class to deal with memory management of an array of template types

// generic declaration
template<typename... Ts>
struct HelperT;

// specialization to stop the recursion for an empty list
template<>
struct HelperT<>
{
  inline static void   copy(size_t src_t, const void * src_v, void * dst_v) { }
  inline static void   move(size_t src_t, void * src_v, void * dst_v) { }
  inline static void   destroy(size_t id, void * data) { }
  inline static size_t index(size_t id, size_t index) { return std::string::npos; }
};

// recursive definition
template<typename F, typename... Ts>
struct HelperT<F, Ts...>
{
  // copy the contents of a type F by calling doing a placement new
  inline static void copy(size_t src_t, const void * src_v, void * dst_v)
  {
    if (src_t == typeid(F).hash_code()) // found it
    {
      new (dst_v) F(*reinterpret_cast<const F*>(src_v));
    }
    else // continue searching
    {
      HelperT<Ts...>::copy(src_t, src_v, dst_v);
    }
  }

  // move the contents of a type F by calling std::move
  inline static void move(size_t src_t, void * src_v, void * dst_v)
  {
    if (src_t == typeid(F).hash_code()) // found it
    {
      new (dst_v) F(std::move(*reinterpret_cast<F*>(src_v)));
    }
    else // continue searching
    {
      HelperT<Ts...>::move(src_t, src_v, dst_v);
    }
  }

  // destroy a type F in the list by calling it destructor
  inline static void destroy(size_t id, void * data)
  {
    if (id == typeid(F).hash_code()) // found it
    {
      reinterpret_cast<F*>(data)->~F();
    }
    else // continue searching
    {
      HelperT<Ts...>::destroy(id, data);
    }
  }

  // return the index of type with id in the list of variant types
  inline static size_t index(size_t id,size_t index)
  {
    if (id == typeid(F).hash_code()) // found it
    {
      return index;
    }
    else // continue searching
    {
      return HelperT<Ts...>::index(id,index+1);
    }
  }

};

} // namespace details

//------------------------------------------------------------------

//! Variant class (similar to C++17's std::variant)
template<typename... Ts>
struct Variant {
  private:
    // constant respresenting the maximum size that can hold all types
    static const size_t data_size = details::TMax<sizeof(Ts)...>::value;
    // constant respresenting the maximum alignment requirement for all all types
    static const size_t data_align = details::TMax<alignof(Ts)...>::value;

    // the data type for the Variant's internal memory
    using Data   = typename std::aligned_storage<data_size, data_align>::type;
    using HelperT = details::HelperT<Ts...>;

    // id that represents an invalid type
    static inline size_t invalid_type() { return typeid(void).hash_code(); }

    size_t type_id;
    Data data;

  public:
    // default constructor
    Variant() : type_id(invalid_type()) {
    }

    // copy constructor
    Variant(const Variant<Ts...>& src) : type_id(src.type_id)
    {
      HelperT::copy(src.type_id, &src.data, &data);
    }

    // move constructor
    Variant(Variant<Ts...>&& src) : type_id(src.type_id)
    {
      HelperT::move(src.type_id, &src.data, &data);
    }

    // copy asignment operator
    Variant<Ts...>& operator= (const Variant<Ts...> &src)
    {
      if (this!=&src)
      {
        type_id = src.type_id;
        HelperT::copy(src.type_id, &src.data, &data);
      }
      return *this;
    }

    // move assignment operator
    Variant<Ts...>& operator= (Variant<Ts...> &&src)
    {
      type_id = src.type_id;
      HelperT::move(src.type_id, &src.data, &data);
      return *this;
    }

    // destructor
    ~Variant()
    {
      HelperT::destroy(type_id, &data);
    }

    // check if the Variant holds a specific type
    template<typename T>
    constexpr bool is() const { return (type_id==typeid(T).hash_code()); }

    // check if the Variant holds a valid type
    constexpr bool valid() const { return (type_id!=invalid_type()); }

    // return the index of the type held by this variant, or std::string::npos if the
    // variant is not valid.
    constexpr size_t index() const { return HelperT::index(type_id, 0); }

    // replace the contents of the Variant by constructing a type T calling
    // the constructor with Args
    template<typename T, typename... Args>
    void set(Args&&... args)
    {
      HelperT::destroy(type_id, &data);
      new (&data) T(std::forward<Args>(args)...);
      type_id = typeid(T).hash_code();
    }

    // gets the value stored in the Variant
    template<typename T>
    T& get()
    {
      if (type_id != typeid(T).hash_code()) throw std::bad_cast();
      return *reinterpret_cast<T*>(&data);
    }

    // gets the value stored in the Variant
    template<typename T>
    const T& get() const
    {
      if (type_id != typeid(T).hash_code()) throw std::bad_cast();
      return *reinterpret_cast<const T*>(&data);
    }

    void invalidate()
    {
      HelperT::destroy(type_id,&data);
      type_id = invalid_type();
    }
};

#endif
