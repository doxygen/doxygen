/*
 * Copyright (C) 2008 by Sebastian Pipping.
 * Copyright (C) 2008 Dimitri van Heesch.
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
 * Sebastian Pipping <sebastian@pipping.org>
 */

#ifndef DOXYGEN_QHP_H
#define DOXYGEN_QHP_H

#include "index.h"
#include "qhpxmlwriter.h"

class Qhp : public IndexIntf
{
  public:
    Qhp();
   ~Qhp();

    // BEGIN IndexIntf
    void initialize();
    void finalize();
    void incContentsDepth();
    void decContentsDepth();
    void addContentsItem(bool isDir, const QCString & name, const QCString & ref,
                         const QCString & file, const QCString & anchor,
                         bool separateIndex,bool addToNavIndex,
                         const Definition *def);
    void addIndexItem(const Definition *context, const MemberDef *md,
                      const QCString &sectionAnchor, const QCString &title);
    void addIndexFile(const QCString & name);
    void addImageFile(const QCString & name);
    void addStyleSheetFile(const QCString & name);
    // END IndexIntf

    static QCString getQhpFileName();

  private:
    void handlePrevSection();
    void clearPrevSection();
    void setPrevSection(const QCString & title, const QCString & basename, const QCString & anchor, int level);
    void addFile(const QCString & fileName);

    static QCString getFullProjectName();

    QhpXmlWriter m_doc;
    QhpXmlWriter m_toc;
    QhpXmlWriter m_index;
    QhpXmlWriter m_files;

    QCString m_prevSectionTitle;
    QCString m_prevSectionBaseName;
    QCString m_prevSectionAnchor;

    int m_prevSectionLevel;
    int m_sectionLevel;
    int m_openClose;

    bool m_skipMainPageSection;
};

#endif // DOXYGEN_QHP_H

