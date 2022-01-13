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
/*
 * eclipsehelp.h
 *
 *  Created on: 7.11.2009
 *      Author: ondrej
 */

#ifndef ECLIPSEHELP_H
#define ECLIPSEHELP_H

#include <fstream>

#include "index.h"

/* -- forward declarations */
class Definition;

/** Generator for Eclipse help files.
 *
 * This class generates the Eclipse specific help files.
 * These files can be used to generate a help plugin readable
 * by the Eclipse IDE.
 */
class EclipseHelp : public IndexIntf
{
  public:
    EclipseHelp();
    virtual ~EclipseHelp();

    /* -- index interface */
    virtual void initialize();
    virtual void finalize();
    virtual void incContentsDepth();
    virtual void decContentsDepth();
    virtual void addContentsItem(bool isDir, const QCString &name, const QCString &ref,
                                 const QCString &file, const QCString &anchor,bool separateIndex,bool addToNavIndex,
                                 const Definition *def);
    virtual void closeContentsItem() {}
    virtual void addIndexItem(const Definition *context,const MemberDef *md,
                              const QCString &sectionAnchor,const QCString &title);
    virtual void addIndexFile(const QCString &name);
    virtual void addImageFile(const QCString &name);
    virtual void addStyleSheetFile(const QCString &name);

  private:
    int m_depth;
    bool m_endtag;
    int m_openTags;

    std::ofstream m_tocstream;
    QCString m_pathprefix;

    /* -- avoid copying */
    EclipseHelp(const EclipseHelp &);
    EclipseHelp & operator = (const EclipseHelp &);

    /* -- formatting helpers */
    void indent();
    void closedTag();
    void openedTag();
};

#endif /* ECLIPSEHELP_H */
