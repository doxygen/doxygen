/******************************************************************************
 *
 * Copyright (C) 2020 by Dimitri van Heesch
 * Based on a patch by David Munger
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

#ifndef CITE_H
#define CITE_H

#include <memory>

#include "qcstring.h"
#include "construct.h"

class CiteInfoOption
{
  public:
    constexpr CiteInfoOption() {}
    static constexpr CiteInfoOption makeNumber()      { return CiteInfoOption(NUMBER); }
    static constexpr CiteInfoOption makeShortAuthor() { return CiteInfoOption(SHORTAUTHOR); }
    static constexpr CiteInfoOption makeYear()        { return CiteInfoOption(YEAR); }

    constexpr void changeToNumber() noexcept        { m_bits = (m_bits & OptionMask) | NUMBER; }
    constexpr void setNoPar() noexcept              { m_bits |= NOPAR_BIT; }
    constexpr void setNoCite() noexcept             { m_bits |= NOCITE_BIT; }

    constexpr bool isUnknown() const noexcept       { return (m_bits & TypeMask)==0; }
    constexpr bool isNumber() const noexcept        { return (m_bits & NUMBER)!=0; }
    constexpr bool isShortAuthor() const noexcept   { return (m_bits & SHORTAUTHOR)!=0; }
    constexpr bool isYear() const noexcept          { return (m_bits & YEAR)!=0; }

    constexpr bool noPar() const noexcept           { return (m_bits & NOPAR_BIT)!=0; }
    constexpr bool noCite() const noexcept          { return (m_bits & NOCITE_BIT)!=0; }

    friend inline bool operator==(const CiteInfoOption &t1,const CiteInfoOption &t2) { return t1.m_bits==t2.m_bits; }
    friend inline bool operator!=(const CiteInfoOption &t1,const CiteInfoOption &t2) { return !(operator==(t1,t2)); }

  private:
    constexpr CiteInfoOption(int bits) : m_bits(bits) {}

    enum Bits
    {
      UNKNOWN     = 0,
      NUMBER      = (1<<0),
      SHORTAUTHOR = (1<<1),
      YEAR        = (1<<2),

      TypeMask    = 0x0000FFFF,
      OptionMask  = 0xFFFF0000,

      NOPAR_BIT   = (1<<16), //< Don't use square brackets
      NOCITE_BIT  = (1<<17)  //< Don't create a link
    };
    unsigned int m_bits = UNKNOWN;
};


/// Citation-related data.
struct CiteInfo
{
  ABSTRACT_BASE_CLASS(CiteInfo)

  virtual QCString label() const = 0;
  virtual QCString text() const = 0;
  virtual QCString shortAuthor() const = 0;
  virtual QCString year() const = 0;
};

/**
 * @brief Citation manager class.
 * @details This class provides access do the database of bibliographic
 * references through the bibtex backend.
 */
class CitationManager
{
  public:
    static CitationManager &instance();

    /** Insert a citation identified by \a label into the database */
    void insert(const QCString &label);

    /** Return the citation info for a given \a label.
     *  Ownership of the info stays with the manager.
     */
    const CiteInfo *find(const QCString &label) const;

    /** Generate the citations page */
    void generatePage();

    /** clears the database */
    void clear();

    /** return TRUE if there are no citations.
     */
    bool isEmpty() const;

    /** lists the bibtex cite files in a comma separated list
     */
    QCString latexBibFiles();

    QCString fileName() const;
    QCString anchorPrefix() const;

  private:
    /** Create the database, with an expected maximum of \a size entries */
    CitationManager();
   ~CitationManager() = default;
    NON_COPYABLE(CitationManager)
    void insertCrossReferencesForBibFile(const QCString &bibFile);
    QCString getFormulas(const QCString &s);
    QCString replaceFormulas(const QCString &s);
    struct Private;
    std::unique_ptr<Private> p;
};

#endif // CITE_H
