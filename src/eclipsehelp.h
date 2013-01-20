/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#include "index.h"
#include "ftextstream.h"

/* -- forward declarations */
class QFile;
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
    virtual void addContentsItem(bool isDir, const char *name, const char *ref,
                                 const char *file, const char *anchor,bool separateIndex,bool addToNavIndex,
                                 Definition *def);
    virtual void addIndexItem(Definition *context,MemberDef *md,
                              const char *sectionAnchor,const char *title);
    virtual void addIndexFile(const char *name);
    virtual void addImageFile(const char *name);
    virtual void addStyleSheetFile(const char *name);

  private:
    int m_depth;
    bool m_endtag;
    int m_openTags;

    QFile * m_tocfile;
    FTextStream m_tocstream;
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
