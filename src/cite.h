/******************************************************************************
 *
 * 
 *
 * Copyright (C) 2011 by Dimitri van Heesch
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

#ifndef CITEDB_H
#define CITEDB_H

#include "qtbc.h"
#include <qdict.h>
#include <qlist.h>

class FTextStream;

/// String constants for citations
struct CiteConsts
{
  static const QCString fileName;
  static const QCString anchorPrefix;
};

/// Citation-related data.
struct CiteInfo
{
  CiteInfo(const char *label_, const char *text_=0, const char *fullText_=0,
      const char *ref_=0) :
    label(label_), text(text_), fullText(fullText_), ref(ref_)
  { }

  CiteInfo(const CiteInfo &o)
  { label=o.label.copy(); text=o.text.copy(); fullText=o.fullText.copy(); ref=o.ref.copy(); }

  QCString label; 
  QCString text;
  QCString fullText;
  QCString ref;

};

/**
 * @brief Cite database access class.
 * @details This class provides access do the database of bibliographic 
 * references through the bibtex backend.
 */
class CiteDict 
{
  public:
    /** Create the database, with an expected maximum of \a size entries */
    CiteDict(int size);

//    /** Resolve references to citations */
//    void resolve();

    /** Insert a citation identified by \a label into the database */
    void insert(const char *label);

    /** Return the citation info for a given \a label */
    CiteInfo *find(const char *label) const;

    /** Generate the citations page */
    void generatePage() const;

    /** clears the database */
    void clear();

    /** return TRUE if there are no citations. 
     *  Only valid after calling resolve() 
     */
    bool isEmpty() const;

    /** writes the latex code for the standard bibliography 
     *  section to text stream \a t 
     */
    void writeLatexBibliography(FTextStream &t);

  private:
//    bool writeAux();
//    bool writeBst();
//    bool execute();
//    void parse();
//    void clean();
    QDict<CiteInfo> m_entries;
//    QList<QCString> m_ordering;
    QCString m_baseFileName;
};

#endif
