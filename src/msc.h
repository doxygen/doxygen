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

#ifndef _MSC_H
#define _MSC_H

class QCString;
class FTextStream;

enum MscOutputFormat { MSC_BITMAP , MSC_EPS };

void writeMscGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,MscOutputFormat format);

QCString getMscImageMapFromFile(const QCString& inFile, const QCString& outDir,
                                const QCString& relPath,const QCString& context);

void writeMscImageMapFromFile(FTextStream &t,const QCString &inFile,
                              const QCString &outDir, const QCString &relPath,
                              const QCString &baseName, const QCString &context);

#endif

