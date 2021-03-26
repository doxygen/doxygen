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

#ifndef SCOPEDTYPEVARIANT_H
#define SCOPEDTYPEVARIANT_H

#include <utility>
#include <vector>

#include "qcstring.h"
#include "definition.h"

//! Class representing a local class definition found while
//! generating syntax highlighted code.
class LocalDef
{
  public:
    void insertBaseClass(const QCString &name) { m_baseClasses.push_back(name); }
    std::vector<QCString> baseClasses() const { return m_baseClasses; }
  private:
    std::vector<QCString> m_baseClasses;
};

//-----------------------------------------------------------------------------

/*! Variant class for a scoped type.
 *
 *  Variants:
 *  - Dummy: a type used for hiding a global type.
 *  - Local: a locally defined type (e.g. found inside a function)
 *  - Global: a globally defined type (processed by doxygen in an earlier pass).
 */
class ScopedTypeVariant
{
  public:
    //! possible variant types
    enum Variant
    {
      Global,
      Local,
      Dummy
    };
    //! default constructor for creating a variant of type Dummy
    ScopedTypeVariant() : m_variant(Dummy)
    {
      m_u.globalDef = 0;
    }
    //! constructor for creating a variant of type Global
    explicit ScopedTypeVariant(const Definition *d)
    {
      if (d)
      {
        m_name = d->name();
        m_variant = Global;
        m_u.globalDef = d;
      }
      else
      {
        m_variant = Dummy;
        m_u.globalDef = 0;
      }
    }
    //! constructor for creating a variant of type Local
    explicit ScopedTypeVariant(const QCString &name)
    {
      m_name = name;
      m_variant = Local;
      m_u.localDef = new LocalDef;
    }
    //! copy constructor
    ScopedTypeVariant(const ScopedTypeVariant &stv)
    {
      m_variant = stv.m_variant;
      m_name    = stv.m_name;
      if (m_variant==Local)
      {
        m_u.localDef = new LocalDef(*stv.m_u.localDef);
      }
      else if (m_variant==Global)
      {
        m_u.globalDef = stv.m_u.globalDef;
      }
    }
    //! move constructor
    ScopedTypeVariant(ScopedTypeVariant &&stv) noexcept : ScopedTypeVariant()
    {
      swap(*this,stv);
    }
    //! assignment operator
    ScopedTypeVariant &operator=(ScopedTypeVariant stv)
    {
      swap(*this,stv);
      return *this;
    }
    //! destructor
   ~ScopedTypeVariant()
    {
      if (m_variant==Local)
      {
        delete m_u.localDef;
      }
    }
    //! swap function
    friend void swap(ScopedTypeVariant &first,ScopedTypeVariant &second)
    {
      using std::swap; // enable ADL
      swap(first.m_variant,second.m_variant);
      swap(first.m_name,second.m_name);
      swap(first.m_u.globalDef,second.m_u.globalDef);
    }
    //! Turn the variant into a Global type
    void setGlobal(const Definition *def)
    {
      if (m_variant==Local)
      {
        delete m_u.localDef;
      }
      m_variant = Global;
      m_name = def->name();
      m_u.globalDef = def;
    }
    //! Turn the variant into a Local type
    LocalDef *setLocal(const QCString &name)
    {
      if (m_variant==Local)
      {
        delete m_u.localDef;
      }
      m_variant = Local;
      m_name = name;
      m_u.localDef = new LocalDef;
      return m_u.localDef;
    }
    //! Turn the variant into a Dummy type
    void setDummy()
    {
      if (m_variant==Local)
      {
        delete m_u.localDef;
      }
      m_variant = Dummy;
      m_name = "";
      m_u.localDef=0;
    }
    Variant type() const { return m_variant; }
    QCString name() const { return m_name; }
    LocalDef *localDef() const { return m_variant==Local ? m_u.localDef : 0; }
    const Definition *globalDef() const { return m_variant==Global ? m_u.globalDef : 0; }

  private:
    Variant m_variant;
    QCString m_name;
    union
    {
      const Definition *globalDef;
      LocalDef *localDef;
    } m_u;
};

//-----------------------------------------------------------------------------

/*! Represents a stack of variable to class mappings as found in the
 *  code. Each scope is enclosed in pushScope() and popScope() calls.
 *  Variables are added by calling addVariables() and one can search
 *  for variable using findVariable().
 */
class VariableContext
{
  public:
    using Scope = std::unordered_map<std::string,ScopedTypeVariant>;

    void pushScope()
    {
      m_scopes.push_back(Scope());
    }
    void popScope()
    {
      if (!m_scopes.empty())
      {
        m_scopes.pop_back();
      }
    }
    void clear()
    {
      m_scopes.clear();
      m_globalScope.clear();
    }
    void clearExceptGlobal()
    {
      m_scopes.clear();
    }
    void addVariable(const QCString &name,ScopedTypeVariant stv)
    {
      Scope *scope = m_scopes.empty() ? &m_globalScope : &m_scopes.back();
      scope->emplace(std::make_pair(name.str(),std::move(stv))); // add it to a list
    }
    const ScopedTypeVariant *findVariable(const QCString &name)
    {
      const ScopedTypeVariant *result = 0;
      if (name.isEmpty()) return result;

      // search from inner to outer scope
      auto it = std::rbegin(m_scopes);
      while (it != std::rend(m_scopes))
      {
        auto it2 = it->find(name.str());
        if (it2 != std::end(*it))
        {
          result = &it2->second;
          return result;
        }
        ++it;
      }
      // nothing found -> also try the global scope
      auto it2 = m_globalScope.find(name.str());
      if (it2 != m_globalScope.end())
      {
        result = &it2->second;
      }
      return result;
    }
    bool atGlobalScope() const { return m_scopes.empty(); }

  private:
    Scope              m_globalScope;
    std::vector<Scope> m_scopes;
};

//-----------------------------------------------------------------------------

/** Represents the call context */
class CallContext
{
  public:
    struct Ctx
    {
      Ctx(const QCString &name_,const QCString &type_) : name(name_), type(type_) {}
      QCString name;
      QCString type;
      ScopedTypeVariant stv;
    };

    CallContext()
    {
      clear();
    }
    void setScope(const ScopedTypeVariant &stv)
    {
      Ctx &ctx = m_stvList.back();
      ctx.stv=std::move(stv);
    }
    void pushScope(const QCString &name_,const QCString &type_)
    {
      m_stvList.push_back(Ctx(name_,type_));
    }
    void popScope(QCString &name_,QCString &type_)
    {
      if (m_stvList.size()>1)
      {
        const Ctx &ctx = m_stvList.back();
        name_ = ctx.name;
        type_ = ctx.type;
        m_stvList.pop_back();
      }
    }
    void clear()
    {
      m_stvList.clear();
      m_stvList.push_back(Ctx("",""));
    }
    const ScopedTypeVariant getScope() const
    {
      return m_stvList.back().stv;
    }

  private:
    std::vector<Ctx> m_stvList;
};


#endif
