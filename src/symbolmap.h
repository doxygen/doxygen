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

#ifndef SYMBOLMAP_H
#define SYMBOLMAP_H

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility>
#include <cassert>

//! Class implementing a symbol map that maps symbol names to objects.
//! Symbol names do not have to be unique.
//! Supports adding symbols with add(), removing symbols with remove(), and
//! finding symbols with find().
template<class T>
class SymbolMap
{
  public:
    using Ptr = T *;
    using VectorPtr = std::vector<Ptr>;
    using Map = std::unordered_map<std::string,VectorPtr>;
    using iterator = typename Map::iterator;
    using const_iterator = typename Map::const_iterator;

    //! Add a symbol \a def into the map under key \a name
    void add(const QCString &name,Ptr def)
    {
      auto it = m_map.find(name.str());
      if (it!=m_map.end())
      {
        it->second.push_back(def);
      }
      else
      {
        m_map.emplace(name.str(),VectorPtr({def}));
      }
    }

    //! Remove a symbol \a def from the map that was stored under key \a name
    void remove(const QCString &name,Ptr def)
    {
      auto it1 = m_map.find(name.str());
      if (it1!=m_map.end())
      {
        VectorPtr &v = it1->second;
        auto it2 = std::find(v.begin(),v.end(),def);
        if (it2!=v.end())
        {
          v.erase(it2);
          if (v.empty())
          {
            m_map.erase(it1);
          }
        }
      }
    }

    //! Find the list of symbols stored under key \a name
    //! Returns a pair of iterators pointing to the start and end of the range of matching symbols
    const VectorPtr &find(const QCString &name)
    {
      assert(m_noMatch.empty());
      auto it = m_map.find(name.str());
      return it==m_map.end() ? m_noMatch : it->second;
    }

    iterator begin()             { return m_map.begin();  }
    iterator end()               { return m_map.end();    }
    const_iterator begin() const { return m_map.cbegin(); }
    const_iterator end() const   { return m_map.cend();   }
    bool empty() const           { return m_map.empty();  }

  private:
    Map m_map;
    VectorPtr m_noMatch;
};

#endif
