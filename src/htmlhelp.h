/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 * The code is this file is largely based on a contribution from
 * Harm van der Heijden <H.v.d.Heijden@phys.tue.nl>
 * Please send thanks to him and bug reports to me :-)
 */

#ifndef HTMLHELP_H
#define HTMLHELP_H

#include "qtbc.h"
#include <qtextstream.h>

class QFile;
class HtmlHelpIndex;

/*! A class that generated the HTML Help specific files.
 *  These files can be used with the Microsoft HTML Help workshop
 *  to generate compressed HTML files (.chm).
 */
class HtmlHelp 
{
  public:
    static HtmlHelp *getInstance();
    void initialize();
    void finalize();
    int  incContentsDepth();
    int  decContentsDepth();
    /*! return the current depth of the contents tree */ 
    int  contentsDepth() { return dc; }
    void addContentsItem(const char *name, const char *ref, 
                         const char *anchor = 0);
    void addIndexItem(const char *level1, const char *level2, 
                      const char *ref, const char *anchor);
  private:
    HtmlHelp();
    QFile *cf,*kf; 
    QTextStream cts,kts;
    HtmlHelpIndex *index;
    int dc;
    static HtmlHelp *theInstance;
};

#endif /* HTMLHELP_H */
