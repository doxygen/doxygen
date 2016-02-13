/******************************************************************************
 *
 *
 *
 * Copyright(C) 2016 by Andreas Grimme
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

#pragma once

#include <qstring.h>

class OutputList;
class FileDef;
class QCString;

class FileLink
{
public:
	QString url;
	QString name;
};

//! Determines URL and name of for relativeFileName which is specified relative to basePath
bool resolveFileLink(const QString& relativeFileName, const QString& basePath, FileLink& outLink)

bool writeFileLink(OutputList& ol, const FileDef &fd);

//! Writes a paragraph that states from which file this content was generated.
bool writeGeneratedFromFile(OutputList& ol, const QString& path);
