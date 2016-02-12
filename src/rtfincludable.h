/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Parker Waechter & Dimitri van Heesch.
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

bool isIncludableRtf();
bool isTableOfContentEntriesEnabled();
bool writeFileLink(OutputList& ol, const FileDef &fd);
bool writeFileLink(OutputList& ol, const QCString& path);

class FileLink
{
public:
	QCString url;
	QCString name;
};

bool resolveFileLink(const QCString& currentFileName, const QCString& link, FileLink& outLink);
