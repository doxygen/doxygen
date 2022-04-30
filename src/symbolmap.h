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

#ifndef SYMBOLMAP_H
#define SYMBOLMAP_H

#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <utility>

//! Class implementing a symbol map that maps symbol names to objects.
//! Symbol names do not have to be unique.
//! Supports adding symbols with add(), removing symbols with remove(), and
//! finding symbols with find().
template<class T>
class SymbolMap
{
  public:
    using Ptr = T *;
    using Map = std::multimap<std::string,Ptr>;
    using iterator = typename Map::iterator;
    using const_iterator = typename Map::const_iterator;

    //! Add a symbol \a def into the map under key \a name
    void add(const QCString &name,Ptr def)
    {
      m_map.insert({name.str(),def});
    }

    //! Remove a symbol \a def from the map that was stored under key \a name
    void remove(const QCString &name,Ptr def)
    {
      auto range = find(name);
      for (auto it=range.first; it!=range.second; )
      {
        if (it->second==def) it = m_map.erase(it); else ++it;
      }
    }

    //! Find the list of symbols stored under key \a name
    //! Returns a pair of iterators pointing to the start and end of the range of matching symbols
    std::pair<const_iterator,const_iterator> find(const QCString &name) const
    {
      return m_map.equal_range(name.str());
    }

    //! Find the list of symbols stored under key \a name
    //! Returns a pair of iterators pointing to the start and end of the range of matching symbols
    std::pair<iterator,iterator> find(const QCString &name)
    {
      return m_map.equal_range(name.str());
    }

    iterator begin()             { return m_map.begin();  }
    iterator end()               { return m_map.end();    }
    const_iterator begin() const { return m_map.cbegin(); }
    const_iterator end() const   { return m_map.cend();   }
    bool empty() const           { return m_map.empty();  }
    size_t size() const          { return m_map.size();   }

  private:
    Map m_map;
};

#endif
