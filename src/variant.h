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

//! generic declaration of a template to handle copying, moving, and deleting
//! type that matches a given index in the list of variant parameters.
template<uint8_t n,typename... Ts>
struct HelperRecT;

//! Recursive template definition for multiple arguments.
//! Each function checks for a matching index, and if found does the action.
//! If not, the same function is called with the next index and one less type argument.
template<uint8_t n, typename F, typename... Ts>
struct HelperRecT<n, F, Ts...>
{
  //! Helper function to copy an instance of a type by performing a placement new.
  //! @param id The id if the type to search for in the template parameter list
  //! @param src_v A pointer to the value of the type to copy from.
  //! @param dst_v A pointer to the variable to copy to.
  inline static void copy(uint8_t id, const void * src_v, void * dst_v)
  {
    if (n==id) // found it
    {
      new (dst_v) F(*reinterpret_cast<const F*>(src_v));
    }
    else // continue searching
    {
      HelperRecT<n+1,Ts...>::copy(id, src_v, dst_v);
    }
  }

  //! Helper function to move an instance of a type by calling the move constructor on it.
  //! @param id The id if the type to search for in the template parameter list
  //! @param src_v A pointer to the value of the type to copy from.
  //! @param dst_v A pointer to the variable to copy to.
  inline static void move(uint8_t id, void * src_v, void * dst_v)
  {
    if (n==id) // found it
    {
      new (dst_v) F(std::move(*reinterpret_cast<F*>(src_v)));
    }
    else // continue searching
    {
      HelperRecT<n+1,Ts...>::move(id, src_v, dst_v);
    }
  }

  //! Helper function to destroy an object of a given type by calling its destructor.
  //! @param id The id if the type to search for in the template parameter list
  //! @param data A pointer to the object to destroy
  inline static void destroy(uint8_t id, void * data)
  {
    if (n==id) // found it
    {
      reinterpret_cast<F*>(data)->~F();
    }
    else // continue searching
    {
      HelperRecT<n+1,Ts...>::destroy(id, data);
    }
  }

};

//! Specialization to stop the recursion when the end of the list has reached
template<uint8_t n>
struct HelperRecT<n>
{
  inline static void copy(uint8_t id, const void * src_v, void * dst_v) { }
  inline static void move(uint8_t id, void * src_v, void * dst_v) { }
  inline static void destroy(uint8_t id, void * data) { }
};

//! Helper to kickstart the recursive search
template<typename ...Ts>
struct HelperT
{
  inline static void copy(uint8_t id, const void *src_v, void *dst_v)
  { HelperRecT<0, Ts...>::copy(id, src_v, dst_v); }
  inline static void move(uint8_t id, void *src_v, void *dst_v)
  { HelperRecT<0, Ts...>::move(id, src_v, dst_v); }
  inline static void destroy(uint8_t id,void *data)
  { HelperRecT<0, Ts...>::destroy(id, data); }
};

//! Specialization to end the recursion
template<>
struct HelperT<>
{
  inline static void copy(uint8_t id, const void * src_v, void * dst_v) { }
  inline static void move(uint8_t id, void * src_v, void * dst_v) { }
  inline static void destroy(uint8_t id, void * data) { }
};

} // namespace details

//! Generic declaration of a template type wrapper where VariantType<index,...>::type
//! represents the type of the variant at the given index.
//! The first type of the variant has index 0, the second has
//! index 1, etc.
template<uint8_t index, typename... Items>
struct VariantType;

//! specialization to stop the recursion when arrived at index 0 and type F
template<typename F,typename...Ts>
struct VariantType<0, F, Ts...>
{
  using type = F;
};

//! recursive definition of the type wrapper
template<uint8_t index, typename F, typename... Ts>
struct VariantType<index, F, Ts...>
{
  using type = typename VariantType<index-1,Ts...>::type;
};

//------------------------------------------------------------------

//! Implementation of a variant container (similar to C++17's std::variant).
//! It can hold either no instances (e.g. initially or after calling invalidate()),
//! or hold exactly one instance of an object (after calling set())
//! whose type is one of the variant's template parameters.
//! Each parameter has an index, the first parameter has index 0.
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

    template<uint8_t index>
    using Type = typename VariantType<index,Ts...>::type;

    //! The id that represents an invalid type
    static inline uint8_t invalid_id() { return 255; }

    //! the actual data
    Data m_data;
    //! a unique identifier for the type held by this variant
    uint8_t m_id;

  public:
    //! The default constructor
    Variant() : m_id(invalid_id())
    {
    }

    //! The copy constructor
    Variant(const Variant<Ts...>& src) : m_id(src.m_id)
    {
      HelperT::copy(src.m_id, &src.m_data, &m_data);
    }

    //! The move constructor
    Variant(Variant<Ts...>&& src) : m_id(src.m_id)
    {
      HelperT::move(src.m_id, &src.m_data, &m_data);
    }

    //! The copy assignment operator
    Variant<Ts...>& operator= (const Variant<Ts...> &src)
    {
      if (this!=&src) // prevent self assignment
      {
        // destroy the old value
        if (valid()) HelperT::destroy(m_id,&m_data);
        // and copy over the new one
        m_id = src.m_id;
        HelperT::copy(src.m_id, &src.m_data, &m_data);
      }
      return *this;
    }

    //! The move assignment operator
    Variant<Ts...>& operator= (Variant<Ts...> &&src)
    {
      // destroy the old value
      if (valid()) HelperT::destroy(m_id,&m_data);
      // and move in the new one
      m_id = src.m_id;
      HelperT::move(src.m_id, &src.m_data, &m_data);
      return *this;
    }

    //! The destructor
    ~Variant()
    {
      HelperT::destroy(m_id, &m_data);
    }

    //! Returns true iff the variant container holds a specific type.
    //! @tparam T the type to search for.
    template<uint8_t index>
    constexpr bool is() const { return m_id==index; }

    //! Returns true iff the Variant holds a valid type.
    constexpr bool valid() const { return m_id!=invalid_id(); }

    //! Invalidate the variant. Will destroy any object that is held.
    void invalidate()
    {
      HelperT::destroy(m_id,&m_data);
      m_id = invalid_id();
    }

    //! Returns the index of the type held by this variant, or invalid_id() if the
    //! variant does not hold any type (i.e. valid() returns false).
    constexpr uint8_t index() const { return m_id; }

    //! Replaces the contents of the variant container by constructing a type T calling
    //! the constructor with Args
    //! @tparam index the type to make the variant hold an instance of.
    //! @tparam Args The arguments types to pass to the constructor of T.
    //! @param args The argument values
    template<uint8_t index, typename... Args>
    void set(Args&&... args)
    {
      HelperT::destroy(m_id, &m_data);
      m_id = index;
      new (&m_data) Type<index>(std::forward<Args>(args)...);
    }

    //! Return a non-constant reference to the value held by the variant container.
    //! @throw std::bad_cast() if called on a variant container that does not hold
    //! an instance of the type of the variant at index.
    template<uint8_t index>
    Type<index>& get()
    {
      if (m_id != index) throw std::bad_cast();
      return *reinterpret_cast<Type<index>*>(&m_data);
    }

    //! Returns a constant reference to the value held by the variant container.
    //! @throw std::bad_cast() if called on a variant container that does not hold
    //! an instance of the type of the variant at index.
    template<uint8_t index>
    const Type<index>& get() const
    {
      if (m_id != index) throw std::bad_cast();
      return *reinterpret_cast<const Type<index>*>(&m_data);
    }

};

#endif
