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

/// Citation-related data.
struct CiteInfo
{
    virtual ~CiteInfo() {}
    virtual QCString label() const = 0;
    virtual QCString text() const = 0;
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
    void insert(const char *label);

    /** Return the citation info for a given \a label.
     *  Ownership of the info stays with the manager.
     */
    const CiteInfo *find(const char *label) const;

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

    const char *fileName() const;
    const char *anchorPrefix() const;

  private:
    /** Create the database, with an expected maximum of \a size entries */
    CitationManager();
    void insertCrossReferencesForBibFile(const QCString &bibFile);
    struct Private;
    std::unique_ptr<Private> p;
};

#endif // CITE_H
