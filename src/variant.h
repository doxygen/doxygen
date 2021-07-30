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

//! generic declaration of a template to compute the maximum size of a set of template parameters.
template <size_t arg1, size_t ... others>
struct TMax;

//! specialization to stop the recursion when arrived at a single argument.
template <size_t arg>
struct TMax<arg>
{
  //! the compile time computed maximum value
  static const size_t value = arg;
};

//! recursive definition for multiple arguments
template <size_t arg1, size_t arg2, size_t ... others>
struct TMax<arg1, arg2, others...>
{
  //! the compile time computed maximum value
  static const size_t value = arg1 >= arg2 ?
        TMax<arg1, others...>::value :
        TMax<arg2, others...>::value ;
};

//------ helper class to deal with memory management of an array of template types

//! generic declaration of a template to handle copyin, moving, deleting, and computing the
//! index for a type that matches one in the set of template parameters.
template<typename... Ts>
struct HelperT;

//! specialization to stop the recursion for an empty list.
template<>
struct HelperT<>
{
  inline static void   copy(size_t src_t, const void * src_v, void * dst_v) { }
  inline static void   move(size_t src_t, void * src_v, void * dst_v) { }
  inline static void   destroy(size_t id, void * data) { }
  inline static size_t index(size_t id, size_t index) { return std::string::npos; }
};

//! Recursive template definition for multiple arguments.
//! Each function check the first parameter, and if the type matches does the action.
//! If not, the same function is called without this template parameters.
template<typename F, typename... Ts>
struct HelperT<F, Ts...>
{
  //! Helper function to copy an instance of a type by performing a placement new.
  //! @param src_t The id if the type to search for in the template parameter list
  //! @param src_v A pointer to the value of the type to copy from.
  //! @param dst_v A pointer to the variable to copy to.
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

  //! Helper function to move an instance of a type by calling the move constructor on it.
  //! @param src_t The id if the type to search for in the template parameter list
  //! @param src_v A pointer to the value of the type to copy from.
  //! @param dst_v A pointer to the variable to copy to.
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

  //! Helper function to destroy an object of a given type by calling its destructor.
  //! @param id The id if the type to search for in the template parameter list
  //! @param data A pointer to the object to destroy
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

  //! Helper function to returns the index of a given type within the template parameter list
  //! @param id The id to search for
  //! @param index The index into the orginal template list
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

//! Implementation of a variant container (similar to C++17's std::variant).
//! It can hold either no instances (e.g. initially or after calling invalidate()),
//! or hold exactly one instance of an object (after calling set())
//! whose type is one of the variant's template parameters.
//! It behaves similar to a C union, in that the memory of all
//! possible object types is shared, but unlike a C union
//! it does allow C++ objects with constructors and destructors to be stored and
//! knows what type is stored.
template<typename... Ts>
struct Variant {
  private:
    //! constant respresenting the maximum size that can hold all types in the template list
    static const size_t data_size = details::TMax<sizeof(Ts)...>::value;
    //! constant respresenting the maximum alignment requirement for all types in the template list
    static const size_t data_align = details::TMax<alignof(Ts)...>::value;

    //! the data type for the Variant's internal memory
    using Data   = typename std::aligned_storage<data_size, data_align>::type;
    //! a short hand name for the helper class
    using HelperT = details::HelperT<Ts...>;

    //! The id that represents an invalid type
    static inline size_t invalid_type() { return typeid(void).hash_code(); }

    //! a unique indentifier for the type held by this variant
    size_t type_id;
    //! the actual data
    Data data;

  public:
    //! The default constructor
    Variant() : type_id(invalid_type())
    {
    }

    //! The copy constructor
    Variant(const Variant<Ts...>& src) : type_id(src.type_id)
    {
      HelperT::copy(src.type_id, &src.data, &data);
    }

    //! The move constructor
    Variant(Variant<Ts...>&& src) : type_id(src.type_id)
    {
      HelperT::move(src.type_id, &src.data, &data);
    }

    //! The copy assignment operator
    Variant<Ts...>& operator= (const Variant<Ts...> &src)
    {
      if (this!=&src)
      {
        type_id = src.type_id;
        HelperT::copy(src.type_id, &src.data, &data);
      }
      return *this;
    }

    //! The move assignment operator
    Variant<Ts...>& operator= (Variant<Ts...> &&src)
    {
      type_id = src.type_id;
      HelperT::move(src.type_id, &src.data, &data);
      return *this;
    }

    //! The destructor
    ~Variant()
    {
      HelperT::destroy(type_id, &data);
    }

    //! Returns true iff the variant container holds a specific type.
    //! @tparam T the type to search for.
    template<typename T>
    constexpr bool is() const { return (type_id==typeid(T).hash_code()); }

    //! Returns true iff the Variant holds a valid type.
    constexpr bool valid() const { return (type_id!=invalid_type()); }

    //! Invalidate the variant. Will destroy any object that is held.
    void invalidate()
    {
      HelperT::destroy(type_id,&data);
      type_id = invalid_type();
    }

    //! Returns the index of the type held by this variant, or std::string::npos if the
    //! variant does not hold any type (i.e. valid() returns false).
    constexpr size_t index() const { return HelperT::index(type_id, 0); }

    //! Replaces the contents of the variant container by constructing a type T calling
    //! the constructor with Args
    //! @param T the type to make the variant hold an instance of.
    //! @param Args The arguments to pass to the constructor of T.
    template<typename T, typename... Args>
    void set(Args&&... args)
    {
      HelperT::destroy(type_id, &data);
      new (&data) T(std::forward<Args>(args)...);
      type_id = typeid(T).hash_code();
    }

    //! Return a non-constant reference to the value held by the variant container.
    //! @throw std::bad_cast() if called on a variant container that does not hold
    //! an instance of the specified type.
    template<typename T>
    T& get()
    {
      if (type_id != typeid(T).hash_code()) throw std::bad_cast();
      return *reinterpret_cast<T*>(&data);
    }

    //! Returns a constant reference to the value held by the variant container.
    //! @throw std::bad_cast() if called on a variant container that does not hold
    //! an instance of the specified type.
    template<typename T>
    const T& get() const
    {
      if (type_id != typeid(T).hash_code()) throw std::bad_cast();
      return *reinterpret_cast<const T*>(&data);
    }

};

#endif
