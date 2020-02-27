/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#ifndef LINKEDMAP_H
#define LINKEDMAP_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

//! @brief Container class representing a vector of objects with unique keys.
//! @details Elements can be quickly looked up given the key.
//! When adding objects the order of addition is kept, and used while iterating.
template<class T>
class LinkedMap
{
  public:
    using Ptr = std::unique_ptr<T>;
    using Vec = std::vector<Ptr>;
    using Map = std::unordered_map<std::string,T*>;
    using iterator = typename Vec::iterator;

    //! find an element given the key.
    //! Returns a pointer to the element if found or nullptr if it is not found.
    const T *find(const char *key) const
    {
      auto it = m_lookup.find(key);
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    //! non-const wrapper for find() const
    T* find(const char *key)
    {
      return const_cast<T*>(static_cast<const LinkedMap&>(*this).find(key));
    }

    //! Adds a new element to the ordered set if it was not added already.
    //! Return a non-owning pointer to the newly added item, or to the existing item if it was
    //! already inserted before.
    template<class...Args>
    T *add(const char *key, Args&&... args)
    {
      T *result = find(key);
      if (result==nullptr)
      {
        Ptr ptr = std::make_unique<T>(key,std::forward<Args>(args)...);
        result = ptr.get();
        m_lookup.insert({std::string(key),result});
        m_entries.push_back(std::move(ptr));
      }
      return result;
    }

    iterator begin()    { return m_entries.begin(); }
    iterator end()      { return m_entries.end();   }
    bool empty() const  { return m_entries.empty(); }
    int size() const    { return m_entries.size();  }

    void clear()
    {
      m_entries.clear();
      m_lookup.clear();
    }

  private:
    Map m_lookup;
    Vec m_entries;
};


#endif
