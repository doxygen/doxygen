/*****************************************************************************
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

#ifndef CACHE_H
#define CACHE_H

#include <list>
#include <unordered_map>
#include <mutex>
#include <utility>
#include <ctype.h>

/*! Fixed size cache for value type V using keys of type K.
 *
 *  When the maximum capacity has reached, the least recently used value is removed from the cache
 *  (LRU strategy).
 */
template<typename K,typename V>
class Cache
{
  public:
    using kv_pair        = std::pair<K,V>;
    using iterator       = typename std::list<kv_pair>::iterator;
    using const_iterator = typename std::list<kv_pair>::const_iterator;

    //! creates a cache that can hold \a capacity elements
    Cache(size_t capacity) : m_capacity(capacity)
    {
    }

    //! Inserts \a value under \a key in the cache
    [[maybe_unused]] V *insert(const K &key,V &&value)
    {
      // reuse item if it already exists
      auto it = m_cacheItemMap.find(key);
      if (it != m_cacheItemMap.end())
      {
        // move the item to the front of the list
        m_cacheItemList.splice(m_cacheItemList.begin(),
                               m_cacheItemList,
                               it->second);
        std::exchange(it->second->second,value);
        return &it->second->second;
      }
      // create new item
      m_cacheItemList.push_front(kv_pair(key,std::move(value)));
      V *result = &m_cacheItemList.front().second;
      m_cacheItemMap[key] = m_cacheItemList.begin();
      // remove least recently used item if cache is full
      resize();
      return result;
    }

    //! Inserts \a value under \a key in the cache
    [[maybe_unused]] V *insert(const K &key,const V &value)
    {
      // reuse item if it already exists
      auto it = m_cacheItemMap.find(key);
      if (it != m_cacheItemMap.end())
      {
        // move the item to the front of the list
        m_cacheItemList.splice(m_cacheItemList.begin(),
                               m_cacheItemList,
                               it->second);
        it->second->second = value;
        return &it->second->second;
      }
      // store new item
      m_cacheItemList.push_front(kv_pair(key,value));
      V *result = &m_cacheItemList.front().second;
      m_cacheItemMap[key] = m_cacheItemList.begin();
      // remove least recently used item if cache is full
      resize();
      return result;
    }

    //! Removes entry \a key from the cache.
    //! \note this invalidates any iterators
    void remove(const K &key)
    {
      // remove item if it already exists
      auto it = m_cacheItemMap.find(key);
      if (it != m_cacheItemMap.end())
      {
        m_cacheItemList.erase(it->second);
        m_cacheItemMap.erase(it);
      }
    }

    //! Finds a value in the cache given the corresponding \a key.
    //! @returns a pointer to the value or nullptr if the key is not found in the cache
    //! @note The hit and miss counters are updated, see hits() and misses().
    V *find(const K &key)
    {
      auto it = m_cacheItemMap.find(key);
      if (it != m_cacheItemMap.end())
      {
        // move the item to the front of the list
        m_cacheItemList.splice(m_cacheItemList.begin(),
                               m_cacheItemList,
                               it->second);
        m_hits++;
        // return the value
        return &it->second->second;
      }
      else
      {
        m_misses++;
      }
      return nullptr;
    }

    //! Returns the number of values stored in the cache.
    size_t size() const
    {
      return m_cacheItemMap.size();
    }

    //! Returns the maximum number of values that can be stored in the cache.
    size_t capacity() const
    {
      return m_capacity;
    }

    //! Returns how many of the find() calls did find a value in the cache.
    uint64_t hits() const
    {
      return m_hits;
    }

    //! Returns how many of the find() calls did not found a value in the cache.
    uint64_t misses() const
    {
      return m_misses;
    }

    //! Clears all values in the cache.
    void clear()
    {
      m_cacheItemMap.clear();
      m_cacheItemList.clear();
    }

    iterator begin()             { return m_cacheItemList.begin();  }
    iterator end()               { return m_cacheItemList.end();    }
    const_iterator begin() const { return m_cacheItemList.cbegin(); }
    const_iterator end() const   { return m_cacheItemList.cend();   }

  private:
    // remove least recently used item if cache is full
    void resize()
    {
      if (m_cacheItemMap.size() > m_capacity)
      {
        auto last_it = m_cacheItemList.end();
        --last_it;
        m_cacheItemMap.erase(last_it->first);
        m_cacheItemList.pop_back();
      }
    }

    size_t m_capacity;
    // list of items in the cache, sorted by most to least recently used.
    std::list<kv_pair> m_cacheItemList;
    // mapping for each key to a place in the list where item is found
    std::unordered_map<K,iterator> m_cacheItemMap;
    uint64_t m_hits=0;
    uint64_t m_misses=0;
};

#endif
