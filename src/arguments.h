/******************************************************************************
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

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <vector>
#include "qcstring.h"

/*! \brief This class contains the information about the argument of a
 *         function or template
 *
 */
struct Argument
{
  /*! return TRUE if this argument is documentation and the argument has a
   *  non empty name.
   */
  bool hasDocumentation() const
  {
    return !name.isEmpty() && !docs.isEmpty();
  }

  bool hasTemplateDocumentation() const
  {
    return (!name.isEmpty() || !type.isEmpty()) && !docs.isEmpty();
  }

  QCString attrib;   /*!< Argument's attribute (IDL only) */
  QCString type;     /*!< Argument's type */
  QCString canType;  /*!< Cached value of canonical type (after type resolution). Empty initially. */
  QCString name;     /*!< Argument's name (may be empty) */
  QCString array;    /*!< Argument's array specifier (may be empty) */
  QCString defval;   /*!< Argument's default value (may be empty) */
  QCString docs;     /*!< Argument's documentation (may be empty) */
  QCString typeConstraint;  /*!< Used for Java generics: \<T extends C\> */
};

enum class RefQualifierType
{
  None,
  LValue,
  RValue
};

/*! \brief This class represents an function or template argument list.
 *
 *  This class also stores some information about member that is typically
 *  put after the argument list, such as whether the member is const,
 *  volatile or pure virtual.
 */
class ArgumentList
{
  public:
    using Vec = std::vector<Argument>;
    using iterator = typename Vec::iterator;
    using const_iterator = typename Vec::const_iterator;

    /*! Does any argument of this list have documentation? */
    bool hasDocumentation() const;
    /*! Does any template argument of this list have documentation? */
    bool hasTemplateDocumentation() const;
    /*! Does this list have zero or more parameters */
    bool hasParameters() const
    {
      return !empty() || m_noParameters;
    }
    void reset()
    {
      clear();
      m_constSpecifier = FALSE;
      m_volatileSpecifier = FALSE;
      m_pureSpecifier = FALSE;
      m_trailingReturnType.clear();
      m_isDeleted = FALSE;
      m_refQualifier = RefQualifierType::None;
      m_noParameters = FALSE;
    }

    // make vector accessible
    iterator begin()                      { return m_args.begin(); }
    iterator end()                        { return m_args.end();   }
    const_iterator begin() const          { return m_args.cbegin(); }
    const_iterator end() const            { return m_args.cend();   }
    const_iterator cbegin() const         { return m_args.cbegin(); }
    const_iterator cend() const           { return m_args.cend();   }
    bool empty() const                    { return m_args.empty(); }
    size_t size() const                   { return m_args.size();  }
    void clear()                          { m_args.clear(); }
    void push_back(const Argument &a)     { m_args.push_back(a); }
    Argument &back()                      { return m_args.back(); }
    const Argument &back() const          { return m_args.back(); }
    Argument &front()                     { return m_args.front(); }
    const Argument &front() const         { return m_args.front(); }
    Argument &at(size_t i)                { return m_args.at(i); }
    const Argument &at(size_t i) const    { return m_args.at(i); }

    // getters for list wide attributes
    bool constSpecifier() const           { return m_constSpecifier; }
    bool volatileSpecifier() const        { return m_volatileSpecifier; }
    bool pureSpecifier() const            { return m_pureSpecifier; }
    QCString trailingReturnType() const   { return m_trailingReturnType; }
    bool isDeleted() const                { return m_isDeleted; }
    RefQualifierType refQualifier() const { return m_refQualifier; }
    bool noParameters() const             { return m_noParameters; }

    void setConstSpecifier(bool b)        { m_constSpecifier = b; }
    void setVolatileSpecifier(bool b)     { m_volatileSpecifier = b; }
    void setPureSpecifier(bool b)         { m_pureSpecifier = b; }
    void setTrailingReturnType(const QCString &s);
    void appendTrailingReturnType(const QCString &s);
    void finishTrailingReturnType();
    void setIsDeleted(bool b)             { m_isDeleted = b; }
    void setRefQualifier(RefQualifierType t) { m_refQualifier = t; }
    void setNoParameters(bool b)          { m_noParameters = b; }

  private:
    std::vector<Argument> m_args;
    /*! Does the member modify the state of the class? */
    bool m_constSpecifier = FALSE;
    /*! Is the member volatile? */
    bool m_volatileSpecifier = FALSE;
    /*! Is this a pure virtual member? */
    bool m_pureSpecifier = FALSE;
    /*! C++11 style Trailing return type? */
    QCString m_trailingReturnType;
    /*! method with =delete */
    bool m_isDeleted = FALSE;
    /*! C++11 ref qualifier */
    RefQualifierType m_refQualifier = RefQualifierType::None;
    /*! is it an explicit empty list */
    bool m_noParameters = FALSE;
};

using ArgumentLists = std::vector<ArgumentList>;

#endif
