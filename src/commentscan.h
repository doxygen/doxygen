/******************************************************************************
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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

#ifndef COMMENTSCAN_H
#define COMMENTSCAN_H

#include "qtbc.h"
#include "entry.h"

class ParserInterface;

int parseCommentBlock(/* in */     ParserInterface *parser,
                      /* in */     Entry *curEntry,
                      /* in */     const QCString &comment,
	              /* in */     const QCString &fileName,
		      /* in */     int lineNr,
		      /* in */     bool isBrief,
		      /* in */     bool isJavaDocStyle,
		      /* in,out */ Protection &prot,
                      /* in,out */ int &position,
                      /* out */    bool &newEntryNeeded
		     );

void groupEnterFile(const char *file,int line);
void groupLeaveFile(const char *file,int line);
void groupLeaveCompound(const char *file,int line,const char *name);
void groupEnterCompound(const char *file,int line,const char *name);
void openGroup(Entry *e,const char *file,int line);
void closeGroup(Entry *,const char *file,int line);
void initGroupInfo(Entry *e);


#endif
