/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef XML_H
#define XML_H

class QTextStream;

void generateXML();
void writeXMLString(QTextStream &t,const char *s);
void writeXMLString(QTextStream &t,const char *s);
void writeXMLLink(QTextStream &t,const char *compoundRef,const char *memAnchor,
                                 const char *text);

#endif
