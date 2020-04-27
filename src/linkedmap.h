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
#include <algorithm>

//! @brief Container class representing a vector of objects with unique keys.
//! @details Objects can efficiently be looked up given the key.
//! Objects are owned by the container.
//! When adding objects the order of addition is kept, and used while iterating.
template<class T>
class LinkedMap
{
  public:
    using Ptr = std::unique_ptr<T>;
    using Vec = std::vector<Ptr>;
    using Map = std::unordered_map<std::string,T*>;
    using iterator = typename Vec::iterator;
    using const_iterator = typename Vec::const_iterator;

    //! Find an object given the key.
    //! Returns a pointer to the element if found or nullptr if it is not found.
    const T *find(const char *k) const
    {
      std::string key = k ? std::string(k) : std::string();
      auto it = m_lookup.find(key);
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    //! A non-const wrapper for find() const
    T* find(const char *key)
    {
      return const_cast<T*>(static_cast<const LinkedMap&>(*this).find(key));
    }

    //! Adds a new object to the ordered set if it was not added already.
    //! Return a non-owning pointer to the newly added object, or to the existing object if
    //! it was already inserted before under the given key.
    template<class...Args>
    T *add(const char *k, Args&&... args)
    {
      T *result = find(k);
      if (result==nullptr)
      {
        std::string key = k ? std::string(k) : std::string();
        Ptr ptr = std::make_unique<T>(k,std::forward<Args>(args)...);
        result = ptr.get();
        m_lookup.insert({key,result});
        m_entries.push_back(std::move(ptr));
      }
      return result;
    }

    //! Removes an object from the container and deletes it.
    //! Returns true if the object was delete or false it is was not found.
    bool del(const char *k)
    {
      std::string key = k ? std::string(k) : std::string();
      auto it = m_lookup.find(key);
      if (it!=m_lookup.end())
      {
        auto vecit = std::find_if(m_entries.begin(),m_entries.end(),[obj=it->second](auto &el) { return el.get()==obj; });
        if (vecit!=m_entries.end()) // should always be true
        {
          m_entries.erase(vecit);
          m_lookup.erase(it);
          return true;
        }
      }
      return false;
    }

    iterator begin()             { return m_entries.begin();  }
    iterator end()               { return m_entries.end();    }
    const_iterator begin() const { return m_entries.cbegin(); }
    const_iterator end() const   { return m_entries.cend();   }
    bool empty() const           { return m_entries.empty();  }
    int size() const             { return m_entries.size();   }

    void clear()
    {
      m_entries.clear();
      m_lookup.clear();
    }

  private:
    Map m_lookup;
    Vec m_entries;
};

#if 0 // not yet used
//! @brief Container class representing a vector of objects with unique keys.
//! @details Objects can be efficiently be looked up given the key.
//! Objects are \e not owned by the container, the container will only hold references.
//! When adding objects the order of addition is kept, and used while iterating.
template<class T>
class LinkedRefMap
{
  public:
    using Ptr = T*;
    using Vec = std::vector<Ptr>;
    using Map = std::unordered_map<std::string,T*>;
    using iterator = typename Vec::iterator;
    using const_iterator = typename Vec::const_iterator;

    //! find an object given the key.
    //! Returns a pointer to the object if found or nullptr if it is not found.
    const T *find(const char *k) const
    {
      std::string key = k ? std::string(k) : std::string();
      auto it = m_lookup.find(key);
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    //! non-const wrapper for find() const
    T* find(const char *key)
    {
      return const_cast<T*>(static_cast<const LinkedRefMap&>(*this).find(key));
    }

    //! Adds a new object to the ordered set if it was not added already.
    //! Return true if the object was added, and false if an object with the same key
    //! was already added before
    bool add(const char *k, T* obj)
    {
      if (find(k)==nullptr) // new element
      {
        std::string key = k ? std::string(k) : std::string();
        m_lookup.insert({key,obj});
        m_entries.push_back(obj);
        return true;
      }
      else // already existing, don't add
      {
        return false;
      }
    }

    iterator begin()               { return m_entries.begin();  }
    iterator end()                 { return m_entries.end();    }
    const_iterator begin() const   { return m_entries.cbegin(); }
    const_iterator end() const     { return m_entries.cend();   }
    bool empty() const             { return m_entries.empty();  }
    int size() const               { return m_entries.size();   }

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


#endif
