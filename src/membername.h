/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#ifndef MEMBERNAME_H
#define MEMBERNAME_H

#include "memberdef.h"
#include "linkedmap.h"

class MemberName
{
  public:
    using Ptr = std::unique_ptr<MemberDef>;
    using Vec = std::vector<Ptr>;
    using iterator = typename Vec::iterator;
    using const_iterator = typename Vec::const_iterator;
    using reverse_iterator = typename Vec::reverse_iterator;
    using const_reverse_iterator = typename Vec::const_reverse_iterator;

    MemberName(const QCString &name) : m_name(name) {}
    QCString memberName() const { return m_name; }

    iterator begin()                       { return m_members.begin();   }
    iterator end()                         { return m_members.end();     }
    const_iterator begin() const           { return m_members.begin();   }
    const_iterator end() const             { return m_members.end();     }
    const_iterator cbegin() const          { return m_members.cbegin();  }
    const_iterator cend() const            { return m_members.cend();    }
    reverse_iterator rbegin()              { return m_members.rbegin();  }
    reverse_iterator rend()                { return m_members.rend();    }
    const_reverse_iterator crbegin() const { return m_members.crbegin(); }
    const_reverse_iterator crend() const   { return m_members.crend();   }
    bool empty() const                     { return m_members.empty();   }
    size_t size() const                    { return m_members.size();    }
    Ptr &back()                            { return m_members.back();    }
    const Ptr &back() const                { return m_members.back();    }
    Ptr &front()                           { return m_members.front();   }
    const Ptr &front() const               { return m_members.front();   }
    iterator erase(iterator it)            { return m_members.erase(it); }
    void push_back(Ptr &&p)                { m_members.push_back(std::move(p)); }

  private:
    QCString m_name;
    Vec m_members;
};

/** Ordered dictionary of MemberName objects. */
class MemberNameLinkedMap : public LinkedMap<MemberName>
{
  public:
    MemberName::Ptr take(const QCString &key,const MemberDef *value)
    {
      MemberName::Ptr result;
      MemberName *mn = find(key);
      if (mn)
      {
        auto it = std::find_if(mn->begin(),mn->end(),[&value](const auto &el) { return el.get()==value; });
        if (it != mn->end())
        {
          it->swap(result);
          mn->erase(it);
        }
        if (mn->empty())
        {
          del(key);
        }
      }
      return result;
    }
};

/** Data associated with a MemberDef in an inheritance relation. */
class MemberInfo
{
  public:
    MemberInfo(MemberDef *md,Protection p,Specifier v,bool inh,bool vbc) :
          m_memberDef(md), m_prot(p), m_virt(v), m_inherited(inh), m_virtBaseClass(vbc) {}

    // getters
          MemberDef *memberDef()                { return m_memberDef; }
    const MemberDef *memberDef() const          { return m_memberDef; }
    Protection prot() const                     { return m_prot;      }
    Specifier  virt() const                     { return m_virt;      }
    bool       inherited() const                { return m_inherited; }
    QCString   scopePath() const                { return m_scopePath; }
    QCString   ambiguityResolutionScope() const { return m_ambiguityResolutionScope; }
    const ClassDef  *ambigClass() const         { return m_ambigClass; }
    bool       virtualBaseClass() const         { return m_virtBaseClass; }

    // setters
    void setAmbiguityResolutionScope(const QCString &s) { m_ambiguityResolutionScope = s; }
    void setScopePath(const QCString &s)                { m_scopePath = s; }
    void setAmbigClass(const ClassDef *cd)              { m_ambigClass = cd; }

  private:
    MemberDef *    m_memberDef;
    Protection     m_prot;
    Specifier      m_virt;
    bool           m_inherited;
    QCString       m_scopePath;
    QCString       m_ambiguityResolutionScope;
    const ClassDef *m_ambigClass = nullptr;
    bool           m_virtBaseClass;
};

class MemberNameInfo
{
  public:
    using Ptr = std::unique_ptr<MemberInfo>;
    using Vec = std::vector<Ptr>;
    using iterator = typename Vec::iterator;
    using const_iterator = typename Vec::const_iterator;

    MemberNameInfo(const QCString &name) : m_name(name) {}
    QCString memberName() const { return m_name; }

    iterator begin()                       { return m_members.begin();   }
    iterator end()                         { return m_members.end();     }
    const_iterator begin() const           { return m_members.begin();   }
    const_iterator end() const             { return m_members.end();     }
    bool empty() const                     { return m_members.empty();   }
    size_t size() const                    { return m_members.size();    }
    Ptr &back()                            { return m_members.back();    }
    const Ptr &back() const                { return m_members.back();    }
    Ptr &front()                           { return m_members.front();   }
    const Ptr &front() const               { return m_members.front();   }
    void push_back(Ptr &&p)                { m_members.push_back(std::move(p)); }
    iterator erase(iterator pos)           { return m_members.erase(pos); }

  private:
    QCString m_name;
    Vec m_members;
};

class MemberNameInfoLinkedMap : public LinkedMap<MemberNameInfo>
{
};

#endif
