/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef PRE_H
#define PRE_H

#include <stdio.h>
#include <qstring.h>
#include <qfile.h>

#if 0
class Define
{
  public:
    Define(const char *n) { name=n; }
   ~Define();
   
  private:
    QString name;
    QStrList args; 
};
#endif

void initPreprocessor();
void addSearchDir(const char *dir);
void preprocessFile(const char *fileName,BufStr &output);

#endif
