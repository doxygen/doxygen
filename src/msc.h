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

class DString;
class TextStream;

enum class MscOutputFormat { BITMAP, EPS, SVG };

void writeMscGraphFromFile(const DString &inFile,const DString &outDir,
                           const DString &outFile,MscOutputFormat format,
                           const DString &srcFile,int srcLine,bool toIndex);

DString getMscImageMapFromFile(const DString &inFile, const DString &outDir,
                                const DString &relPath,const DString &context,
                                const DString &srcFile,int srcLine);

void writeMscImageMapFromFile(TextStream &t,const DString &inFile,
                              const DString &outDir, const DString &relPath,
                              const DString &baseName, const DString &context,
                              MscOutputFormat format,
                              const DString &srcFile,int srcLine
                             );

#endif

