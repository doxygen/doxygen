/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2010 by Dimitri van Heesch.
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

#include "qtbc.h"
#include "index.h"
#include <qtextstream.h>

/* -- forward declarations */
class QFile;

/*!
 * \brief Generator of Eclipse help files
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
    virtual void addContentsItem(bool isDir, const char *name, const char *ref = 0,
                                 const char *file = 0, const char *anchor = 0);
    virtual void addIndexItem(Definition *context,MemberDef *md,
                              const char *anchor,const char *word);
    virtual void addIndexFile(const char *name);
    virtual void addImageFile(const char *name);
    virtual void addStyleSheetFile(const char *name);

  private:
    int m_depth;
    bool m_endtag;

    QFile * m_tocfile;
    QTextStream m_tocstream;
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
