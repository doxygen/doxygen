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
#include <cctype>

#include "qcstring.h"

//! @brief Container class representing a vector of objects with keys.
//! @details Objects can efficiently be looked up given the key.
//! Objects are owned by the container.
//! When adding objects the order of addition is kept, and used while iterating.
template<class T, class Hash = std::hash<std::string>,
                  class KeyEqual = std::equal_to<std::string>,
                  class Map = std::unordered_map<std::string,T*,Hash,KeyEqual > >
class LinkedMap
{
  public:
    using Ptr = std::unique_ptr<T>;
    using Vec = std::vector<Ptr>;
    using iterator = typename Vec::iterator;
    using const_iterator = typename Vec::const_iterator;
    using reverse_iterator = typename Vec::reverse_iterator;
    using const_reverse_iterator = typename Vec::const_reverse_iterator;

    //! Find an object given the key.
    //! Returns a pointer to the element if found or nullptr if it is not found.
    const T *find(const std::string &key) const
    {
      auto it = m_lookup.find(key);
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    //! Find an object given the key.
    //! Returns a pointer to the element if found or nullptr if it is not found.
    const T *find(const QCString &key) const
    {
      auto it = m_lookup.find(key.str());
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    //! Find an object given the key.
    //! Returns a pointer to the element if found or nullptr if it is not found.
    const T *find(const char *key) const
    {
      return find(std::string(key ? key : ""));
    }

    //! A non-const wrapper for find() const
    T* find(const char *key)
    {
      return const_cast<T*>(static_cast<const LinkedMap&>(*this).find(key));
    }

    //! A non-const wrapper for find() const
    T* find(const QCString &key)
    {
      return const_cast<T*>(static_cast<const LinkedMap&>(*this).find(key));
    }

    //! A non-const wrapper for find() const
    T* find(const std::string &key)
    {
      return const_cast<T*>(static_cast<const LinkedMap&>(*this).find(key));
    }

    //! Adds a new object to the ordered vector if it was not added already.
    //! Return a non-owning pointer to the newly added object, or to the existing object if
    //! it was already inserted before under the given key.
    template<class...Args>
    [[maybe_unused]] T *add(const char *k, Args&&... args)
    {
      T *result = find(k);
      if (result==nullptr)
      {
        std::string key(k ? k : "");
        Ptr ptr = std::make_unique<T>(QCString(k),std::forward<Args>(args)...);
        result = ptr.get();
        m_lookup.emplace(key,result);
        m_entries.push_back(std::move(ptr));
      }
      return result;
    }

    template<class...Args>
    [[maybe_unused]] T *add(const QCString &k, Args&&... args)
    {
      std::string key = k.str();
      T *result = find(key);
      if (result==nullptr)
      {
        Ptr ptr = std::make_unique<T>(k,std::forward<Args>(args)...);
        result = ptr.get();
        m_lookup.emplace(key,result);
        m_entries.push_back(std::move(ptr));
      }
      return result;
    }

    //! Adds an existing object to the ordered vector (unless another object was already
    //! added under the same key). Ownership is transferred.
    //! Return a non-owning pointer to the newly added object, or to the existing object if
    //! it was already inserted before under the given key.
    [[maybe_unused]] T *add(const char *k, Ptr &&ptr)
    {
      T *result = find(k);
      if (result==nullptr)
      {
        std::string key(k ? k : "");
        result = ptr.get();
        m_lookup.emplace(key,result);
        m_entries.push_back(std::move(ptr));
      }
      return result;
    }

    [[maybe_unused]] T *add(const QCString &k, Ptr &&ptr)
    {
      std::string key = k.str();
      T *result = find(key);
      if (result==nullptr)
      {
        result = ptr.get();
        m_lookup.emplace(key,result);
        m_entries.push_back(std::move(ptr));
      }
      return result;
    }

    //! Prepends a new object to the ordered vector if it was not added already.
    //! Return a non-owning pointer to the newly added object, or to the existing object if
    //! it was already inserted before under the given key.
    template<class...Args>
    T *prepend(const char *k, Args&&... args)
    {
      T *result = find(k);
      if (result==nullptr)
      {
        QCString key(k ? k : "");
        Ptr ptr = std::make_unique<T>(key,std::forward<Args>(args)...);
        result = ptr.get();
        m_lookup.emplace(key,result);
        m_entries.push_front(std::move(ptr));
      }
      return result;
    }

    template<class...Args>
    T *prepend(const QCString &key, Args&&... args)
    {
      T *result = find(key);
      if (result==nullptr)
      {
        Ptr ptr = std::make_unique<T>(key,std::forward<Args>(args)...);
        result = ptr.get();
        m_lookup.emplace(key.str(),result);
        m_entries.push_front(std::move(ptr));
      }
      return result;
    }

    //! Removes an object from the container and deletes it.
    //! Returns true if the object was deleted or false it is was not found.
    bool del(const QCString &key)
    {
      auto it = m_lookup.find(key.str());
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

    Ptr &operator[](size_t pos)             { return m_entries[pos];      }
    const Ptr &operator[](size_t pos) const { return m_entries[pos];      }
    iterator begin()                        { return m_entries.begin();   }
    iterator end()                          { return m_entries.end();     }
    const_iterator begin() const            { return m_entries.cbegin();  }
    const_iterator end() const              { return m_entries.cend();    }
    reverse_iterator rbegin()               { return m_entries.rbegin();  }
    reverse_iterator rend()                 { return m_entries.rend();    }
    const_reverse_iterator rbegin() const   { return m_entries.crbegin(); }
    const_reverse_iterator rend() const     { return m_entries.crend();   }
    bool empty() const                      { return m_entries.empty();   }
    size_t size() const                     { return m_entries.size();    }

    void clear()
    {
      m_entries.clear();
      m_lookup.clear();
    }

  private:

    Map m_lookup;
    Vec m_entries;
};

//! @brief Container class representing a vector of objects with keys.
//! @details Objects can be efficiently be looked up given the key.
//! Objects are \e not owned by the container, the container will only hold references.
//! When adding objects the order of addition is kept, and used while iterating.
template<class T, class Hash = std::hash<std::string>,
                  class KeyEqual = std::equal_to<std::string>,
                  class Map = std::unordered_map<std::string,T*,Hash,KeyEqual > >
class LinkedRefMap
{
  public:
    using Ptr = T*;
    using Vec = std::vector<Ptr>;
    using iterator = typename Vec::iterator;
    using const_iterator = typename Vec::const_iterator;
    using reverse_iterator = typename Vec::reverse_iterator;
    using const_reverse_iterator = typename Vec::const_reverse_iterator;

    //! find an object given the key.
    //! Returns a pointer to the object if found or nullptr if it is not found.
    const T *find(const std::string &key) const
    {
      auto it = m_lookup.find(key);
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    //! find an object given the key.
    //! Returns a pointer to the object if found or nullptr if it is not found.
    const T *find(const QCString &key) const
    {
      auto it = m_lookup.find(key.str());
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    //! find an object given the key.
    //! Returns a pointer to the object if found or nullptr if it is not found.
    const T *find(const char *key) const
    {
      return find(std::string(key ? key : ""));
    }

    //! non-const wrapper for find() const
    T* find(const char *key)
    {
      return const_cast<T*>(static_cast<const LinkedRefMap&>(*this).find(key));
    }

    T* find(const QCString &key)
    {
      return const_cast<T*>(static_cast<const LinkedRefMap&>(*this).find(key));
    }

    //! non-const wrapper for find() const
    T* find(const std::string &key)
    {
      return const_cast<T*>(static_cast<const LinkedRefMap&>(*this).find(key));
    }

    //! Adds an object reference to the ordered vector if it was not added already.
    //! Return true if the reference was added, and false if an object with the same key
    //! was already added before
    bool add(const char *k, T* obj)
    {
      if (find(k)==nullptr) // new element
      {
        std::string key(k ? k : "");
        m_lookup.emplace(key,obj);
        m_entries.push_back(obj);
        return true;
      }
      else // already existing, don't add
      {
        return false;
      }
    }

    bool add(const QCString &k, T* obj)
    {
      std::string key = k.str();
      if (find(key)==nullptr) // new element
      {
        m_lookup.emplace(key,obj);
        m_entries.push_back(obj);
        return true;
      }
      else // already existing, don't add
      {
        return false;
      }
    }

    //! Prepends an object reference to the ordered vector if it was not added already.
    //! Return true if the reference was added, and false if an object with the same key
    //! was already added before
    bool prepend(const char *k, T* obj)
    {
      if (find(k)==nullptr) // new element
      {
        std::string key(k ? k : "");
        m_lookup.emplace(key,obj);
        m_entries.insert(m_entries.begin(),obj);
        return true;
      }
      else // already existing, don't add
      {
        return false;
      }
    }

    bool prepend(const QCString &key, T* obj)
    {
      if (find(key)==nullptr) // new element
      {
        m_lookup.emplace(key.str(),obj);
        m_entries.insert(m_entries.begin(),obj);
        return true;
      }
      else // already existing, don't add
      {
        return false;
      }
    }

    //! Removes an object from the container and deletes it.
    //! Returns true if the object was deleted or false it is was not found.
    bool del(const QCString &key)
    {
      auto it = m_lookup.find(key.str());
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

    Ptr &operator[](size_t pos)             { return m_entries[pos];      }
    const Ptr &operator[](size_t pos) const { return m_entries[pos];      }
    iterator begin()                        { return m_entries.begin();   }
    iterator end()                          { return m_entries.end();     }
    const_iterator begin() const            { return m_entries.cbegin();  }
    const_iterator end() const              { return m_entries.cend();    }
    reverse_iterator rbegin()               { return m_entries.rbegin();  }
    reverse_iterator rend()                 { return m_entries.rend();    }
    const_reverse_iterator rbegin() const   { return m_entries.crbegin(); }
    const_reverse_iterator rend() const     { return m_entries.crend();   }
    bool empty() const                      { return m_entries.empty();   }
    size_t size() const                     { return m_entries.size();    }

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
