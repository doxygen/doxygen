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
#include <variant>

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
 *  - DummyDef: a type used for hiding a global type.
 *  - LocalDef: a locally defined type (e.g. found inside a function)
 *  - GlobalDef: a globally defined type (processed by doxygen in an earlier pass).
 */
class ScopedTypeVariant
{
  public:
    struct DummyDef {};
    using GlobalDef = const Definition *;
    using Variant = std::variant<DummyDef,LocalDef,GlobalDef>;
    explicit ScopedTypeVariant(GlobalDef d = nullptr)
      : m_name(d ? d->name() : QCString()), m_variant(d ? Variant(d) : Variant(DummyDef())) {}
    explicit ScopedTypeVariant(const QCString &name)
      : m_name(name), m_variant(LocalDef()) {}
    QCString name() const               { return m_name; }
    LocalDef *localDef()                { return std::get_if<LocalDef>(&m_variant); }
    const LocalDef *localDef() const    { return std::get_if<LocalDef>(&m_variant); }
    const Definition *globalDef() const { auto pp = std::get_if<GlobalDef>(&m_variant); return pp ? *pp : nullptr; }
    bool isDummy() const                { return std::holds_alternative<DummyDef>(m_variant); }

  private:
    QCString m_name;
    Variant m_variant;
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
      m_scopes.emplace_back();
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
      scope->emplace(name.str(),std::move(stv)); // add it to a list
    }
    const ScopedTypeVariant *findVariable(const QCString &name)
    {
      const ScopedTypeVariant *result = nullptr;
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
      Ctx(const QCString &name_,const QCString &type_, int bracketCount_) : name(name_), type(type_), bracketCount(bracketCount_) {}
      QCString name;
      QCString type;
      int bracketCount;
      ScopedTypeVariant stv;
    };

    CallContext()
    {
      clear();
    }
    void setScope(const ScopedTypeVariant &stv)
    {
      Ctx &ctx = m_stvList.back();
      ctx.stv=stv;
    }
    void pushScope(const QCString &name_,const QCString &type_, int bracketCount_)
    {
      m_stvList.emplace_back(name_,type_,bracketCount_);
    }
    void popScope(QCString &name_,QCString &type_, int &bracketCount_)
    {
      if (m_stvList.size()>1)
      {
        const Ctx &ctx = m_stvList.back();
        name_ = ctx.name;
        type_ = ctx.type;
        bracketCount_ = ctx.bracketCount;
        m_stvList.pop_back();
      }
    }
    void clear()
    {
      m_stvList.clear();
      m_stvList.emplace_back(QCString(),QCString(),0);
    }
    const ScopedTypeVariant getScope() const
    {
      return m_stvList.back().stv;
    }

  private:
    std::vector<Ctx> m_stvList;
};


#endif
