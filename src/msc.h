/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#ifndef MSC_H
#define MSC_H

class QCString;
class TextStream;

enum class MscOutputFormat { BITMAP, EPS, SVG };

void writeMscGraphFromFile(const QCString &inFile,const QCString &outDir,
                           const QCString &outFile,MscOutputFormat format,
                           const QCString &srcFile,int srcLine);

QCString getMscImageMapFromFile(const QCString &inFile, const QCString &outDir,
                                const QCString &relPath,const QCString &context,
                                const QCString &srcFile,int srcLine);

void writeMscImageMapFromFile(TextStream &t,const QCString &inFile,
                              const QCString &outDir, const QCString &relPath,
                              const QCString &baseName, const QCString &context,
			      MscOutputFormat format,
                              const QCString &srcFile,int srcLine
 			    );

#endif

