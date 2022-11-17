/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#include <map>
#include <string>

#include "qcstring.h"

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

  public:
    StyleData() = default;
    StyleData(const std::string &reference, const std::string &definition);
    bool setStyle(const std::string &command, const std::string &styleName);
    const char *reference() const { return m_reference.c_str(); }
    const char *definition() const { return m_definition.c_str(); }
    uint index() const { return m_index; }

  private:
    uint m_index = 0; // index in style-sheet, i.e. number in s-clause
    std::string m_reference;    // everything required to apply the style
    std::string m_definition;   // additional tags like \snext and style name
};

using StyleDataMap = std::map<std::string,StyleData>;

extern StyleDataMap rtf_Style;

void loadExtensions(const QCString &name);
void loadStylesheet(const QCString &name, StyleDataMap& map);

#endif
