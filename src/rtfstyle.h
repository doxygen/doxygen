/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#ifndef RTFSTYLE_H
#define RTFSTYLE_H

#include <qregexp.h>
#include <qdict.h>

// used for table column width calculation
const int rtf_pageWidth = 8748;

extern QCString rtf_title;
extern QCString rtf_subject;
extern QCString rtf_comments;
extern QCString rtf_company;
extern QCString rtf_logoFilename;
extern QCString rtf_author;
extern QCString rtf_manager;
extern QCString rtf_documentType;
extern QCString rtf_documentId;
extern QCString rtf_keywords;

struct RTFListItemInfo
{
  bool isEnum;
  int number;
};

const int rtf_maxIndentLevels = 10;

extern RTFListItemInfo rtf_listItemInfo[rtf_maxIndentLevels];

struct Rtf_Style_Default
{
  const char *name;
  const char *reference;
  const char *definition;
};

extern char rtf_Style_Reset[];
extern Rtf_Style_Default rtf_Style_Default[];

struct StyleData
{
  // elements of this type are stored in dictionary Rtf_Style
  //
  // to define a tag in the header reference + definition is required
  // to use a tag in the body of the document only reference is required

  unsigned index;   // index in style-sheet, i.e. number in s-clause
  char* reference;  // everything required to apply the style
  char* definition; // additional tags like \snext and style name

  StyleData(const char* reference, const char* definition);
  ~StyleData();
  bool setStyle(const char* s, const char* styleName);

  static const QRegExp s_clause;
};

extern QDict<StyleData> rtf_Style;

void loadExtensions(const char *name);
void loadStylesheet(const char *name, QDict<StyleData>& dict);

#endif
