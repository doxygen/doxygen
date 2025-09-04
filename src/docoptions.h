/******************************************************************************
 *
 * Copyright (C) 1997-2025 by Dimitri van Heesch.
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

#ifndef DOCOPTIONS_H
#define DOCOPTIONS_H

#include "config.h"
#include "qcstring.h"

struct DocOptions
{
  public:
    // === getters for optional params
    bool indexWords()        const { return m_indexWords;      }
    bool isExample()         const { return m_isExample;       }
    QCString exampleName()   const { return m_exampleName;     }
    bool singleLine()        const { return m_singleLine;      }
    bool linkFromIndex()     const { return m_linkFromIndex;   }
    bool markdownSupport()   const { return m_markdownSupport; }
    bool autolinkSupport()   const { return m_autolinkSupport; }
    int sectionLevel()       const { return m_sectionLevel;    }

    // === setters for optional params

    /// Indicates whether or not words should be put in the search index.
    DocOptions &setIndexWords(bool b)
    { m_indexWords=b; return *this; }

    /// Associate this comment block with a given example
    DocOptions &setExample(const QCString &name)
    { m_isExample=!name.isEmpty(); m_exampleName = name; return *this; }

    /// Select if this is for a single line, so without starting a new paragraph at the end.
    DocOptions &setSingleLine(bool b)
    { m_singleLine=b; return *this; }

    /// Select if the documentation is generated from an
    /// index page. In this case context is not used to determine
    /// the relative path when making a link.
    DocOptions &setLinkFromIndex(bool b)
    { m_linkFromIndex=b; return *this; }

    /// Select if the input needs to take markdown markup into account.
    DocOptions &setMarkdownSupport(bool b)
    { m_markdownSupport=b; return *this; }

    /// Select if the input need to perform auto linking of words
    DocOptions &setAutolinkSupport(bool b)
    { m_autolinkSupport=b; return *this; }

    /// Restrict output up to the given section level
    DocOptions &setSectionLevel(int l)
    { m_sectionLevel=l; return *this; }

  private:
    // optional params with defaults
    bool m_indexWords = false;
    bool m_isExample = false;
    QCString m_exampleName;
    bool m_singleLine = false;
    bool m_linkFromIndex = false;
    bool m_markdownSupport = Config_getBool(MARKDOWN_SUPPORT);
    bool m_autolinkSupport = Config_getBool(AUTOLINK_SUPPORT);
    int  m_sectionLevel = -1;
};

#endif

