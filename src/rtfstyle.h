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

#include <cstdint>
#include <map>
#include <string>

#include "dstring.h"

// used for table column width calculation
const int rtf_pageWidth = 8748;

extern DString rtf_title;
extern DString rtf_subject;
extern DString rtf_comments;
extern DString rtf_company;
extern DString rtf_logoFilename;
extern DString rtf_author;
extern DString rtf_manager;
extern DString rtf_documentType;
extern DString rtf_documentId;
extern DString rtf_keywords;

struct Rtf_Style_Default
{
  const char *name;
  const char *reference;
  const char *definition;
};

struct Rtf_Table_Default
{
  int id;
  int lvl;
  const char *definition;
  const char *place;
};

extern char rtf_Style_Reset[];
extern Rtf_Style_Default rtf_Style_Default[];
extern Rtf_Table_Default rtf_Table_Default[];

struct StyleData
{
  // elements of this type are stored in dictionary Rtf_Style
  //
  // to define a tag in the header reference + definition is required
  // to use a tag in the body of the document only reference is required

  public:
    StyleData() = default;
    StyleData(const DString &reference, const DString &definition);
    bool setStyle(const DString &command, const DString &styleName);
    DString reference() const { return m_reference; }
    DString definition() const { return m_definition; }
    uint32_t index() const { return m_index; }

  private:
    uint32_t m_index = 0; // index in style-sheet, i.e. number in s-clause
    DString m_reference;    // everything required to apply the style
    DString m_definition;   // additional tags like \snext and style name
};

using StyleDataMap = std::map<std::string,StyleData>;

extern StyleDataMap rtf_Style;

void loadExtensions(const DString &name);
void loadStylesheet(const DString &name, StyleDataMap& map);

#endif
