/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Parker Waechter & Dimitri van Heesch.
 *
 * Style sheet additions by Alexander Bartolich
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

#include <stdlib.h>

#include "qtbc.h"
#include <qdir.h>
#include <qregexp.h>

#include "rtfgen.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "util.h"
#include "diagram.h"
#include "language.h"
#include "dot.h"
#include "version.h"
#include "page.h"

//#define DBG_RTF(x) x;
#define DBG_RTF(x)

const int indentLevels = 10;

struct ListItemInfo
{
  bool isEnum;
  int number;
};

static ListItemInfo listItemInfo[indentLevels];

// it is undocumented as far as I know, but 
// "."'s in a bookmarkname are converted to "_"'s
// when an RTF file is read in...
static QCString formatBmkStr(const char *name)
{
  QCString result=name;
  if (!result.isEmpty())
  {
    char c;
    char *p=result.data();
    while ((c=*p))
    {
      switch(c)
      {
        case '.':
          // fall through 
        case ':':  
          *p='_'; 
          break;
        default: 
          break;
      }
      p++;
    }
  }
  return result;
}

static QCString dateToRTFDateString()
{
  const QDateTime &d = QDateTime::currentDateTime();
  QCString result;
  result.sprintf("\\yr%d\\mo%d\\dy%d\\hr%d\\min%d\\sec%d",
      d.date().year(), d.date().month(), d.date().day(), 
      d.time().hour(),d.time().minute(),d.time().second());
  return result;
}

RTFGenerator::RTFGenerator() : OutputGenerator()
{
  dir=Config::rtfOutputDir;
  col=0;
  //insideTabbing=FALSE;
  m_listLevel = 0;
  m_bstartedBody = FALSE;
  m_omitParagraph = FALSE;
}

RTFGenerator::~RTFGenerator()
{
}

void RTFGenerator::append(const OutputGenerator *g)
{
  t << g->getContents();
  col+=((RTFGenerator *)g)->col;
  //insideTabbing=insideTabbing || ((RTFGenerator *)g)->insideTabbing;
  m_listLevel=((RTFGenerator *)g)->m_listLevel;
  m_omitParagraph=((RTFGenerator *)g)->m_omitParagraph;
  //printf("RTFGenerator::append(%s) insideTabbing=%s\n", g->getContents().data(),
  //    insideTabbing ? "TRUE" : "FALSE" );
}

OutputGenerator *RTFGenerator::copy()
{
  RTFGenerator *result = new RTFGenerator;
  //result->insideTabbing=insideTabbing;
  result->m_listLevel=m_listLevel;
  result->m_omitParagraph=m_omitParagraph;
  return result;
}

static const char Rtf_Style_Reset[] = "\\pard\\plain ";

struct Rtf_Style_Default
{
  const char *name;
  const char *reference;
  const char *definition;
};

static const struct Rtf_Style_Default Rtf_Style_Default[] =
{
  { "Heading1",
    "\\s1\\sb240\\sa60\\keepn\\widctlpar\\adjustright \\b\\f1\\fs36\\kerning36\\cgrid ",
    "\\sbasedon0 \\snext0 heading 1"    
  },
  { "Heading2",
    "\\s2\\sb240\\sa60\\keepn\\widctlpar\\adjustright \\b\\f1\\fs28\\kerning28\\cgrid ",
    "\\sbasedon0 \\snext0 heading 2"    
  },
  { "Heading3",
    "\\s3\\sb240\\sa60\\keepn\\widctlpar\\adjustright \\b\\f1\\cgrid ",
    "\\sbasedon0 \\snext0 heading 3"    
  },
  { "Heading4",
    "\\s4\\sb240\\sa60\\keepn\\widctlpar\\adjustright \\b\\f1\\fs20\\cgrid ",
    "\\sbasedon0 \\snext0 heading 4;}{\\*\\cs10 \\additive Default Paragraph Font"    
  },
  { "Title",
    "\\s15\\qc\\sb240\\sa60\\widctlpar\\outlinelevel0\\adjustright \\b\\f1\\fs32\\kerning28\\cgrid ",
    "\\sbasedon0 \\snext15 Title"    
  },
  { "SubTitle",
    "\\s16\\qc\\sa60\\widctlpar\\outlinelevel1\\adjustright \\f1\\cgrid ",
    "\\sbasedon0 \\snext16 Subtitle"    
  },
  { "BodyText",
    "\\s17\\sa60\\sb30\\widctlpar\\qj \\fs22\\cgrid ",
    "\\sbasedon0 \\snext17 BodyText"    
  },
  { "DenseText",
    "\\s18\\widctlpar\\fs22\\cgrid ",
    "\\sbasedon0 \\snext18 DenseText"    
  },
  { "Header",
    "\\s28\\widctlpar\\tqc\\tx4320\\tqr\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext28 header"    
  },
  { "Footer",
    "\\s29\\widctlpar\\tqc\\tx4320\\tqr\\tx8640\\qr\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext29 footer"    
  },
  { "GroupHeader",
    "\\s30\\li360\\sa60\\sb120\\keepn\\widctlpar\\adjustright \\b\\f1\\fs20\\cgrid ",
    "\\sbasedon0 \\snext30 GroupHeader"    
  },
  { "CodeExample0",  
    "\\s40\\li0\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext41 Code Example 0"    
  },
  { "CodeExample1",  
    "\\s41\\li360\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext42 Code Example 1"    
  },
  { "CodeExample2",    
    "\\s42\\li720\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext43 Code Example 2"    
  },
  { "CodeExample3",  
    "\\s43\\li1080\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext44 Code Example 3"    
  },
  { "CodeExample4",  
    "\\s44\\li1440\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext45 Code Example 4"    
  },
  { "CodeExample5",    
    "\\s45\\li1800\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext46 Code Example 5"    
  },
  { "CodeExample6",     
    "\\s46\\li2160\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext47 Code Example 6"    
  },
  { "CodeExample7",    
    "\\s47\\li2520\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext48 Code Example 7"    
  },
  { "CodeExample8",    
    "\\s48\\li2880\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext49 Code Example 8"    
  },
  { "CodeExample9",
    "\\s49\\li3240\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
    "\\sbasedon0 \\snext49 Code Example 9"    
  },
  { "ListContinue0",
    "\\s50\\li0\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext51 List Continue 0"    
  },
  { "ListContinue1",  
    "\\s51\\li360\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext52 List Continue 1"    
  },
  { "ListContinue2",  
    "\\s52\\li720\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext53 List Continue 2"    
  },
  { "ListContinue3",  
    "\\s53\\li1080\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext54 List Continue 3"    
  },
  { "ListContinue4",  
    "\\s54\\li1440\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext55 List Continue 4"    
  },
  { "ListContinue5",  
    "\\s55\\li1800\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext56 List Continue 5"    
  },
  { "ListContinue6",  
    "\\s56\\li2160\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext57 List Continue 6"    
  },
  { "ListContinue7",  
    "\\s57\\li2520\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext58 List Continue 7"    
  },
  { "ListContinue8",  
    "\\s58\\li2880\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext59 List Continue 8"    
  },
  { "ListContinue9",  
    "\\s59\\li3240\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext59 List Continue 9"    
  },
  { "DescContinue0",
    "\\s60\\li0\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext61 DescContinue 0"    
  },
  { "DescContinue1",  
    "\\s61\\li360\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext62 DescContinue 1"    
  },
  { "DescContinue2",
    "\\s62\\li720\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext63 DescContinue 2"    
  },
  { "DescContinue3",
    "\\s63\\li1080\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext64 DescContinue 3"    
  },
  { "DescContinue4",
    "\\s64\\li1440\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext65 DescContinue 4"    
  },
  { "DescContinue5",
    "\\s65\\li1800\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext66 DescContinue 5"    
  },
  { "DescContinue6",
    "\\s66\\li2160\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext67 DescContinue 6"    
  },
  { "DescContinue7",
    "\\s67\\li2520\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext68 DescContinue 7"    
  },
  { "DescContinue8",
    "\\s68\\li2880\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext69 DescContinue 8"
  },
  { "DescContinue9",
    "\\s69\\li3240\\widctlpar\\ql\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext69 DescContinue 9"    
  },
  { "LatexTOC0",
    "\\s70\\li0\\sa30\\sb30\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext81 LatexTOC 0"    
  },
  { "LatexTOC1",
    "\\s71\\li360\\sa27\\sb27\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext82 LatexTOC 1"    
  },
  { "LatexTOC2",
    "\\s72\\li720\\sa24\\sb24\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext83 LatexTOC 2"    
  },
  { "LatexTOC3",
    "\\s73\\li1080\\sa21\\sb21\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext84 LatexTOC 3"    
  },
  { "LatexTOC4",
    "\\s74\\li1440\\sa18\\sb18\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext85 LatexTOC 4"    
  },
  { "LatexTOC5",
    "\\s75\\li1800\\sa15\\sb15\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext86 LatexTOC 5"    
  },
  { "LatexTOC6",
    "\\s76\\li2160\\sa12\\sb12\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext87 LatexTOC 6"    
  },
  { "LatexTOC7",
    "\\s77\\li2520\\sa9\\sb9\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext88 LatexTOC 7"    
  },
  { "LatexTOC8",
    "\\s78\\li2880\\sa6\\sb6\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext89 LatexTOC 8"    
  },
  { "LatexTOC9",
    "\\s79\\li3240\\sa3\\sb3\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext89 LatexTOC 9"    
  },
  { "ListBullet0",
    "\\s80\\fi-360\\li360\\widctlpar\\jclisttab\\tx360{\\*\\pn \\pnlvlbody\\ilvl0\\ls1\\pnrnot0\\pndec }\\ls1\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext81 \\sautoupd List Bullet 0"    
  },
  { "ListBullet1",    
    "\\s81\\fi-360\\li720\\widctlpar\\jclisttab\\tx720{\\*\\pn \\pnlvlbody\\ilvl0\\ls2\\pnrnot0\\pndec }\\ls2\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext82 \\sautoupd List Bullet 1"    
  },
  { "ListBullet2",
    "\\s82\\fi-360\\li1080\\widctlpar\\jclisttab\\tx1080{\\*\\pn \\pnlvlbody\\ilvl0\\ls3\\pnrnot0\\pndec }\\ls3\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext83 \\sautoupd List Bullet 2"    
  },
  { "ListBullet3",
    "\\s83\\fi-360\\li1440\\widctlpar\\jclisttab\\tx1440{\\*\\pn \\pnlvlbody\\ilvl0\\ls4\\pnrnot0\\pndec }\\ls4\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext84 \\sautoupd List Bullet 3"    
  },
  { "ListBullet4",
    "\\s84\\fi-360\\li1800\\widctlpar\\jclisttab\\tx1800{\\*\\pn \\pnlvlbody\\ilvl0\\ls5\\pnrnot0\\pndec }\\ls5\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext85 \\sautoupd List Bullet 4"    
  },
  { "ListBullet5",
    "\\s85\\fi-360\\li2160\\widctlpar\\jclisttab\\tx2160{\\*\\pn \\pnlvlbody\\ilvl0\\ls6\\pnrnot0\\pndec }\\ls6\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext86 \\sautoupd List Bullet 5"    
  },
  { "ListBullet6",
    "\\s86\\fi-360\\li2520\\widctlpar\\jclisttab\\tx2520{\\*\\pn \\pnlvlbody\\ilvl0\\ls7\\pnrnot0\\pndec }\\ls7\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext87 \\sautoupd List Bullet 6"    
  },
  { "ListBullet7",
    "\\s87\\fi-360\\li2880\\widctlpar\\jclisttab\\tx2880{\\*\\pn \\pnlvlbody\\ilvl0\\ls8\\pnrnot0\\pndec }\\ls8\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext88 \\sautoupd List Bullet 7"    
  },
  { "ListBullet8",
    "\\s88\\fi-360\\li3240\\widctlpar\\jclisttab\\tx3240{\\*\\pn \\pnlvlbody\\ilvl0\\ls9\\pnrnot0\\pndec }\\ls9\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext89 \\sautoupd List Bullet 8"    
  },
  { "ListBullet9",
    "\\s89\\fi-360\\li3600\\widctlpar\\jclisttab\\tx3600{\\*\\pn \\pnlvlbody\\ilvl0\\ls10\\pnrnot0\\pndec }\\ls10\\adjustright \\fs20\\cgrid ",
    "\\sbasedon0 \\snext89 \\sautoupd List Bullet 9"    
  },
  { "ListEnum0",
    "\\s90\\fi-360\\li360\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext91 \\sautoupd List Enum 0"    
  },
  { "ListEnum1",
    "\\s91\\fi-360\\li720\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext92 \\sautoupd List Enum 1"    
  },
  { "ListEnum2",
    "\\s92\\fi-360\\li1080\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext93 \\sautoupd List Enum 2"    
  },
  { "ListEnum3",
    "\\s93\\fi-360\\li1440\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext94 \\sautoupd List Enum 3"    
  },
  { "ListEnum4",
    "\\s94\\fi-360\\li1800\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext95 \\sautoupd List Enum 4"    
  },
  { "ListEnum5",
    "\\s95\\fi-360\\li2160\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext96 \\sautoupd List Enum 5"    
  },
  { "ListEnum6",
    "\\s96\\fi-360\\li2520\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext96 \\sautoupd List Enum 5"    
  },
  { "ListEnum7",
    "\\s97\\fi-360\\li2880\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext98 \\sautoupd List Enum 7"    
  },
  { "ListEnum8",
    "\\s98\\fi-360\\li3240\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext99 \\sautoupd List Enum 8"    
  },
  { "ListEnum9",
    "\\s99\\fi-360\\li3600\\widctlpar\\fs20\\cgrid ",
    "\\sbasedon0 \\snext99 \\sautoupd List Enum 9"    
  },
  { 0,
    0,
    0
  }
};

void RTFGenerator::writeStyleSheetFile(QFile &file)
{
  QTextStream t(&file);
  t << "# Generated by doxygen " << versionString << "\n\n";
  t << "# This file describes styles used for generating RTF output.\n";
  t << "# All text after a hash (#) is considered a comment and will be ignored.\n";
  t << "# Remove a hash to activate a line.\n\n"; 

  int i;
  for (i=0;Rtf_Style_Default[i].reference!=0;i++)
  {
    t << "# " << Rtf_Style_Default[i].name << " = "
              << Rtf_Style_Default[i].reference
              << Rtf_Style_Default[i].definition << endl;
  }
}

struct StyleData
{
  // elements of this type are stored in dictionary Rtf_Style
  //
  // to define a tag in the header reference + definition is required
  // to use a tag in the body of the document only reference is required

  unsigned index;   // index in style-sheet, i.e. number in s-clause
  char* reference;  // everything required to apply the style
  char* definition; // aditional tags like \snext and style name

  StyleData(const char* reference, const char* definition);
  ~StyleData();
  bool setStyle(const char* s, const char* styleName);

  static const QRegExp s_clause;
};

const QRegExp StyleData::s_clause("\\\\s[0-9]+\\s*");

StyleData::StyleData(const char* reference, const char* definition)
{
  int start = s_clause.match(reference); ASSERT(start >= 0);
  reference += start;
  index = (int)atol(reference + 2); ASSERT(index > 0);

  ASSERT(reference != 0);
  size_t size = 1 + strlen(reference);
  memcpy(this->reference = new char[size], reference, size);

  ASSERT(definition != 0);
  size = 1 + strlen(definition);
  memcpy(this->definition = new char[size], definition, size);
}

StyleData::~StyleData()
{
  delete[] reference;
  delete[] definition;
}

bool StyleData::setStyle(const char* s, const char* styleName)
{
  static const QRegExp subgroup("^{[^}]*}\\s*");
  static const QRegExp any_clause("^\\\\[a-z][a-z0-9-]*\\s*");
	
  int len;
  int start = s_clause.match(s, 0, &len);
  if (start < 0)
  {
    err("Style sheet '%s' contains no '\\s' clause.\n{%s}\n", styleName, s);
    return FALSE;
  }
  s += start;
  index = (int)atol(s + 2); ASSERT(index > 0);

  // search for the end of pure formatting codes
  const char* end = s + len;
  bool haveNewDefinition = TRUE;
  for(;;)
  {
    if (*end == '{')
    {
      // subgroups are used for \\additive
      if (0 != subgroup.match(end, 0, &len))
        break;
      else
        end += len;
    }
    else if (*end == '\\')
    {
      if (0 == strncmp(end, "\\snext", 6))
        break;
      if (0 == strncmp(end, "\\sbasedon", 9))
        break;
      if (0 != any_clause.match(end, 0, &len))
        break;
      end += len;
    }
    else if (*end == 0)
    { // no style-definition part, keep default value
      haveNewDefinition = FALSE;
      break;
    }
    else // plain name without leading \\snext
      break;
  }
  delete[] reference;
  reference = new char[len + 1];
  memcpy(reference, s, len); reference[len] = 0;
  if (haveNewDefinition)
  {
    delete[] definition;
    size_t size = 1 + strlen(end);
    definition = new char[size];
    memcpy(definition, end, size);
  }
  return TRUE;
}

static void loadStylesheet(const char *name, QDict<StyleData>& dict)
{
  QFile file(name);
  if (!file.open(IO_ReadOnly))
  {
    err("Can't open RTF style sheet file %s. Using defaults.\n",name);
    return;
  }
  msg("Loading RTF style sheet %s...\n",name);

  static const QRegExp separator("[ \t]*=[ \t]*");
  uint lineNr=1;
  QTextStream t(&file);
  while (!t.eof())
  {
    QCString s(4096); // string buffer of max line length
    s = t.readLine().stripWhiteSpace();
    if (s.length()==0 || s.at(0)=='#') continue; // skip blanks & comments
    int sepLength;
    int sepStart = separator.match(s,0,&sepLength);
    if (sepStart<=0) // no valid assignment statement
    {
      warn(name,lineNr,"Assignment of style sheet name expected!\n"); 
      continue;
    }
    QCString key=s.left(sepStart);
    if (dict[key]==0) // not a valid style sheet name
    {
      warn(name,lineNr,"Invalid style sheet name %s ignored.\n",key.data());
      continue;
    }
    StyleData* styleData = dict.find(key);
    if (styleData == 0)
    {
      warn(name,lineNr,"Unknown style sheet name %s ignored.\n",key.data());
      continue;
    }
    s+=" "; // add command separator
    styleData->setStyle(s.data() + sepStart + sepLength, key.data());
    lineNr++;
  }
}

static QDict<StyleData> Rtf_Style(257);

void RTFGenerator::init()
{
  QCString dir=Config::rtfOutputDir;
  QDir d(dir);
  if (!d.exists() && !d.mkdir(dir))
  {
    err("Could not create output directory %s\n",dir.data());
    exit(1);
  }
  Rtf_Style.setAutoDelete(TRUE);
  
  // first duplicate strings of Rtf_Style_Default
  const struct Rtf_Style_Default* def = Rtf_Style_Default;
  while(def->reference != 0)
  {
    if (def->definition == 0)
      err("Internal error: Rtf_Style_Default[%s] has no definition.\n", def->name);
    StyleData* styleData = new StyleData(def->reference, def->definition);
    Rtf_Style.insert(def->name, styleData);
    def++;
  }

  // overwrite some (or all) definitions from file
  if (!Config::rtfStylesheetFile.isEmpty())
    loadStylesheet(Config::rtfStylesheetFile, Rtf_Style);
}

static QCString makeIndexName(const char *s,int i)
{
  QCString result=s;
  result+=(char)(i+'0');
  return result;
}

void RTFGenerator::beginRTFDocument()
{
  /* all the included RTF files should begin with the
   * same header
   */
  t <<"{\\rtf1\\ansi\\ansicpg1252\\uc1 \\deff0\\deflang1033\\deflangfe1033\n";

  DBG_RTF(t <<"{\\comment Begining font list}\n")
  t <<"{\\fonttbl ";
  t <<"{\\f0\\froman\\fcharset0\\fprq2{\\*\\panose 02020603050405020304}Times New Roman;}\n";
  t <<"{\\f1\\fswiss\\fcharset0\\fprq2{\\*\\panose 020b0604020202020204}Arial;}\n";
  t <<"{\\f2\\fmodern\\fcharset0\\fprq1{\\*\\panose 02070309020205020404}Courier New;}\n";
  t <<"{\\f3\\froman\\fcharset2\\fprq2{\\*\\panose 05050102010706020507}Symbol;}\n";
  t <<"}\n";
  DBG_RTF(t <<"{\\comment begin colors}\n")
  t <<"{\\colortbl;";
  t <<"\\red0\\green0\\blue0;";
  t <<"\\red0\\green0\\blue255;";
  t <<"\\red0\\green255\\blue255;";
  t <<"\\red0\\green255\\blue0;";
  t <<"\\red255\\green0\\blue255;";
  t <<"\\red255\\green0\\blue0;";
  t <<"\\red255\\green255\\blue0;";
  t <<"\\red255\\green255\\blue255;";
  t <<"\\red0\\green0\\blue128;";
  t <<"\\red0\\green128\\blue128;";
  t <<"\\red0\\green128\\blue0;";
  t <<"\\red128\\green0\\blue128;";
  t <<"\\red128\\green0\\blue0;";
  t <<"\\red128\\green128\\blue0;";
  t <<"\\red128\\green128\\blue128;";
  t <<"\\red192\\green192\\blue192;}" << endl;

  DBG_RTF(t <<"{\\comment Beginning style list}\n")
  t <<"{\\stylesheet\n";
  t <<"{\\widctlpar\\adjustright \\fs20\\cgrid \\snext0 Normal;}\n";

  // sort styles ascending by \s-number via an intermediate QArray
  QArray<const StyleData*> array(128);
  array.fill(0);
  QDictIterator<StyleData> iter(Rtf_Style);
  const StyleData* style;
  for(; (style = iter.current()); ++iter)
  {
    unsigned index = style->index;
    unsigned size = array.size();    
    if (index >= size)
    {      
      // +1 to add at least one element, then align up to multiple of 8
      array.resize((index + 1 + 7) & ~7);
      array.fill(0, size);
      ASSERT(index < array.size());
    }    
    if (array.at(index) != 0)
    {
      QCString key(convertToQCString(iter.currentKey()));
      msg("Style '%s' redefines \\s%d.\n", key.data(), index);
    }
    array.at(index) = style;      
  }

  // write array elements
  unsigned size = array.size();
  for(unsigned i = 0; i < size; i++)
  {
    const StyleData* style = array.at(i);
    if (style != 0)
      t <<"{" << style->reference << style->definition << ";}\n";
  }

  t <<"}" << endl;
  // this comment is needed for postprocessing!
  t <<"{\\comment begin body}" << endl;

}

void RTFGenerator::beginRTFChapter()
{
  t <<"\n";
  DBG_RTF(t << "{\\comment Begin Chapter}\n")
  t << Rtf_Style_Reset;

  // if we are compact, no extra page breaks...
  if (Config::compactRTFFlag)
  {
    //		t <<"\\sect\\sectd\\sbknone\n";
    t <<"\\sect\\sbknone\n";
    RtfwriteRuler_thick();
  }
  else
    t <<"\\sect\\sbkpage\n";
  //t <<"\\sect\\sectd\\sbkpage\n";

  t << Rtf_Style["Heading1"]->reference << "\n";
}

void RTFGenerator::beginRTFSection()
{
  t <<"\n";
  DBG_RTF(t << "{\\comment Begin Section}\n")
  t << Rtf_Style_Reset;

  // if we are compact, no extra page breaks...
  if (Config::compactRTFFlag)
  {
    //		t <<"\\sect\\sectd\\sbknone\n";
    t <<"\\sect\\sbknone\n";
    RtfwriteRuler_emboss();
  }
  else
    t <<"\\sect\\sbkpage\n";
  //t <<"\\sect\\sectd\\sbkpage\n";

  t << Rtf_Style["Heading2"]->reference << "\n";
}

void RTFGenerator::startFile(const char *name,const char *, bool )
{
  QCString fileName=name;

  if (fileName.right(4)!=".rtf" ) fileName+=".rtf";
  startPlainFile(fileName);
  beginRTFDocument();
}

void RTFGenerator::endFile()
{
  t << "}";

  endPlainFile();
}

void RTFGenerator::startProjectNumber()
{
  t << " ";
}

void RTFGenerator::endProjectNumber() 
{
}

void RTFGenerator::startIndexSection(IndexSections is)
{
  //QCString paperName;

  m_listLevel = 0;

  switch (is)
  {
    case isTitlePageStart:
      // basic RTFstart
      // get readyfor author etc
      t << "{\\info \n";
      t << "{\\title ";
      break;
    case isTitlePageAuthor:
      t << "}\n{\\author ";
      break;
    case isMainPage:
      //Introduction
      beginRTFChapter();
      break;
    case isPackageIndex:
      //Package Index
      beginRTFChapter();
      break;
    case isModuleIndex:
      //Module Index
      beginRTFChapter();
      break;
    case isNamespaceIndex:
      //Namespace Index
      beginRTFChapter();  
      break;
    case isClassHierarchyIndex:
      //Hierarchical Index
      DBG_RTF(t << "{\\comment start classhierarchy}\n")
      beginRTFChapter();
      break;
    case isCompoundIndex:
      //Annotated Compound Index
      beginRTFChapter();
      break;
    case isFileIndex:
      //Annotated File Index
      beginRTFChapter();
      break;
    case isPageIndex:
      //Related Page Index
      beginRTFChapter();
      break;
    case isPackageDocumentation:
      {
        //Package Documentation
        PackageSDict::Iterator pdi(packageDict);
        PackageDef *pd=pdi.toFirst();
        bool found=FALSE;
        while (pd && !found)
        {
          beginRTFChapter();
          found=TRUE;
          ++pdi;
          pd=pdi.current();
        }
      }
      break;
    case isModuleDocumentation:
      {
        //Module Documentation
        GroupDef *gd=groupList.first();
        bool found=FALSE;
        while (gd && !found)
        {
          if (!gd->isReference())
          {
            beginRTFChapter();
            found=TRUE;
          }
          gd=groupList.next();
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        // Namespace Documentation
        NamespaceDef *nd=namespaceList.first();
        bool found=FALSE;
        while (nd && !found)
        {
          if (nd->isLinkableInProject())
          {
            beginRTFChapter();
            found=TRUE;
          }
          nd=namespaceList.next();
        } 
      }
      break;
    case isClassDocumentation:
      {
        //Compound Documentation
        ClassDef *cd=classList.first();
        bool found=FALSE;
        while (cd && !found)
        {
          if (cd->isLinkableInProject())
          {
            beginRTFChapter();
            found=TRUE;
          }
          cd=classList.next();
        }
      }
      break;
    case isFileDocumentation:
      {
        //File Documentation
        bool isFirst=TRUE;
        FileName *fn=inputNameList.first();
        while (fn)
        {
          FileDef *fd=fn->first();
          while (fd)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                beginRTFChapter();				  
                isFirst=FALSE;
                break;
              }
            }
            fd=fn->next();
          }
          fn=inputNameList.next();
        }
      }
      break;
    case isExampleDocumentation:
      {
        //Example Documentation
        beginRTFChapter();
      }
      break;
    case isPageDocumentation:
      {
        //Page Documentation
        beginRTFChapter();
      }
      break;
    case isEndIndex:
      break;
  }
}

void RTFGenerator::endIndexSection(IndexSections is)
{
  switch (is)
  {
    case isTitlePageStart:
      break;
    case isTitlePageAuthor:
      {
        t << " Doxygen}\n"
          "{\\creatim " << dateToRTFDateString() << "}\n"
          "}";
        DBG_RTF(t << "{\\comment end of infoblock}\n")
        // setup for this section
        t << Rtf_Style_Reset <<"\n";
        t <<"\\sectd\\pgnlcrm\n";
        t <<"{\\footer "<<Rtf_Style["Footer"]->reference << "{\\chpgn}}\n";
        // the title entry
        DBG_RTF(t << "{\\comment begin title page}\n")
        t << Rtf_Style_Reset << Rtf_Style["Title"]->reference << endl; // set to title style
        t << "{\\field\\fldedit {\\*\\fldinst TITLE \\\\*MERGEFORMAT}{\\fldrslt TITLE}}\\par" << endl;
        t << Rtf_Style_Reset << Rtf_Style["SubTitle"]->reference << endl; // set to subtitle style
        t << "{\\field\\fldedit {\\*\\fldinst AUTHOR \\\\*MERGEFORMAT}{\\fldrslt AUTHOR}}\\par" << endl;
        t << "{\\field\\fldedit {\\*\\fldinst CREATEDATE \\\\*MERGEFORMAT}"
          "{\\fldrslt CREATEDATE}}\\par"<<endl;
        DBG_RTF(t << "{\\comment End title page}" << endl)

        // table of contents section
        DBG_RTF(t << "{\\comment Table of contents}\n")
        t << Rtf_Style_Reset << endl;
        t << "{\\field\\fldedit {\\*\\fldinst TOC \\\\f \\\\*MERGEFORMAT}{\\fldrslt Table of contents}}\\par\n";
        t << Rtf_Style_Reset << endl;

      }
      break;
    case isMainPage:
      t << "\\par " << Rtf_Style_Reset << endl;
      t << "{\\tc \\v " << theTranslator->trMainPage() << "}"<< endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"index.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isPackageIndex:
      t << "\\par " << Rtf_Style_Reset << endl;
      t << "{\\tc \\v " << theTranslator->trPackageList() << "}"<< endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"packages.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isModuleIndex:
      t << "\\par " << Rtf_Style_Reset << endl;
      t << "{\\tc \\v " << theTranslator->trModuleIndex() << "}"<< endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"modules.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isNamespaceIndex:
      t << "\\par " << Rtf_Style_Reset << endl;
      t << "{\\tc \\v " << theTranslator->trNamespaceIndex() << "}"<< endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"namespaces.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isClassHierarchyIndex:
      t << "\\par " << Rtf_Style_Reset << endl;
      t << "{\\tc \\v " << theTranslator->trHierarchicalIndex() << "}"<< endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"hierarchy.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isCompoundIndex:
      t << "\\par " << Rtf_Style_Reset << endl;
      t << "{\\tc \\v " << theTranslator->trCompoundIndex() << "}"<< endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"annotated.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isFileIndex:
      t << "\\par " << Rtf_Style_Reset << endl;
      t << "{\\tc \\v " << theTranslator->trFileIndex() << "}"<< endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"files.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isPageIndex:
      t << "\\par " << Rtf_Style_Reset << endl;
      t << "{\\tc \\v " << theTranslator->trPageIndex() << "}"<< endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"pages.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isPackageDocumentation:
      {
        PackageSDict::Iterator pdi(packageDict);
        PackageDef *pd=pdi.toFirst();
        t << "{\\tc \\v " << theTranslator->trPackageDocumentation() << "}"<< endl;
        while (pd)
        {
          t << "\\par " << Rtf_Style_Reset << endl;
          t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
          t << pd->getOutputFileBase();
          t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          ++pdi;
          pd=pdi.current();
        }
      }
      break;
    case isModuleDocumentation:
      {
        GroupDef *gd=groupList.first();
        t << "{\\tc \\v " << theTranslator->trModuleDocumentation() << "}"<< endl;
        while (gd)
        {
          if (!gd->isReference())
          {
            t << "\\par " << Rtf_Style_Reset << endl;
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << gd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          }
          gd=groupList.next();
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        NamespaceDef *nd=namespaceList.first();
        bool found=FALSE;
        while (nd && !found)
        {
          if (nd->isLinkableInProject())
          {
            t << "\\par " << Rtf_Style_Reset << endl;
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << nd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
            found=TRUE;
          }
          nd=namespaceList.next();
        }
        while (nd)
        {
          if (nd->isLinkableInProject())
          {
            t << "\\par " << Rtf_Style_Reset << endl;
            beginRTFSection();
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << nd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          }
          nd=namespaceList.next();
        }
      }
      break;
    case isClassDocumentation:
      {
        ClassDef *cd=classList.first();
        bool found=FALSE;

        t << "{\\tc \\v " << theTranslator->trClassDocumentation() << "}"<< endl;
        while (cd && !found)
        {
          if (cd->isLinkableInProject())
          {
            t << "\\par " << Rtf_Style_Reset << endl;
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << cd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
            found=TRUE;
          }
          cd=classList.next();
        }
        while (cd)
        {
          if (cd->isLinkableInProject())
          {
            t << "\\par " << Rtf_Style_Reset << endl;
            beginRTFSection();
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << cd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          } 
          cd=classList.next();
        }
      }
      break;
    case isFileDocumentation:
      {
        bool isFirst=TRUE;
        FileName *fn=inputNameList.first();

        t << "{\\tc \\v " << theTranslator->trFileDocumentation() << "}"<< endl;
        while (fn)
        {
          FileDef *fd=fn->first();
          while (fd)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                t << "\\par " << Rtf_Style_Reset << endl;
                t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
                t << fd->getOutputFileBase();
                t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
                isFirst=FALSE;
              }
              else
              {
                t << "\\par " << Rtf_Style_Reset << endl;
                beginRTFSection();
                t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
                t << fd->getOutputFileBase();
                t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
              }
            }
            fd=fn->next();
          }
          fn=inputNameList.next();
        }
      }
      break;
    case isExampleDocumentation:
      {
        //t << "}\n";
        t << "{\\tc \\v " << theTranslator->trExampleDocumentation() << "}"<< endl;
        PageSDictIterator pdi(*exampleSDict);
        PageInfo *pi=pdi.toFirst();
        if (pi)
        {
          t << "\\par " << Rtf_Style_Reset << endl;
          t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
          t << convertFileName(pi->name);
          t << "-example.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
        }
        for (++pdi;(pi=pdi.current());++pdi)
        {
          t << "\\par " << Rtf_Style_Reset << endl;
          beginRTFSection();
          t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
          t << convertFileName(pi->name);
          t << "-example.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
        }
      }
      break;
    case isPageDocumentation:
      {
        t << "{\\tc \\v " << theTranslator->trPageDocumentation() << "}"<< endl;
        PageSDictIterator pdi(*pageSDict);
        PageInfo *pi=pdi.toFirst();
        bool first=TRUE;
        for (pdi.toFirst();(pi=pdi.current());++pdi)
        {
          if (!pi->inGroup)
          {
            QCString pageName;
            if (Config::caseSensitiveNames)
              pageName=pi->name.copy();
            else
              pageName=pi->name.lower();
            if (first) t << "\\par " << Rtf_Style_Reset << endl;
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << pageName;
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
            first=FALSE;
          }
        }
      }
      break;
    case isEndIndex:
      beginRTFChapter();				  
      t << Rtf_Style["Heading1"]->reference;
      t << "Index\\par "<< endl;
      t << Rtf_Style_Reset << endl;
      t << "{\\tc \\v Index}" << endl;
      t << "{\\field\\fldedit {\\*\\fldinst INDEX \\\\c2 \\\\*MERGEFORMAT}{\\fldrslt INDEX}}\n";

      break;
   }
}

void RTFGenerator::lastIndexPage()
{
  DBG_RTF(t <<"{\\comment Begining Body of RTF Document}\n")
  // end page and setup for rest of document
  t <<"\\sect \\sbkpage \\pgnrestart\n";
  t <<"\\sect \\sectd \\sbknone \\pgndec\n";

  // set footer
  t <<"{\\footer "<< Rtf_Style["Footer"]->reference << "{\\chpgn}}\n";
  //t << Rtf_Style["Heading1"]->reference << "\n";

}

void RTFGenerator::writeStyleInfo(int)
{
}

void RTFGenerator::lineBreak()
{
  //t << "\\par" << endl;
  //newParagraph();
  t << "\\line" << endl;
}

void RTFGenerator::writeString(const char *text)
{
  t << text;
}

void RTFGenerator::startIndexList() 
{ 
  DBG_RTF(t << "{\\comment (startIndexList)}" << endl)
  t << "{" << endl;
  incrementIndentLevel();
  t << Rtf_Style_Reset << Rtf_LCList_DepthStyle() << endl;
  newParagraph();
  m_omitParagraph=TRUE;
}

void RTFGenerator::endIndexList()
{ 
  DBG_RTF(t << "{\\comment (endIndexList)}" << endl)
  newParagraph();
  t << "}";
  decrementIndentLevel();
  m_omitParagraph=TRUE;
}

/*! start bullet list */
void RTFGenerator::startItemList()  
{ 
  DBG_RTF(t << "{\\comment (startItemList level=" << m_listLevel << ") }" << endl)
  t << "{";
  incrementIndentLevel();
  listItemInfo[m_listLevel].isEnum = FALSE;
  //t << Rtf_Style_Reset << Rtf_BList_DepthStyle();
}

/*! end bullet list */
void RTFGenerator::endItemList()    
{ 
  newParagraph();
  DBG_RTF(t << "{\\comment (endItemList level=" << m_listLevel << ")}" << endl)
  t << "}";
  decrementIndentLevel();
  m_omitParagraph=TRUE;
  //t << Rtf_Style_Reset << styleStack.top() << endl;
  //printf("RTFGenerator::endItemList() `%s'\n",styleStack.top());
  //newParagraph();
}

/*! start enumeration list */
void RTFGenerator::startEnumList()  // starts an enumeration list
{ 
  DBG_RTF(t << "{\\comment (startEnumList)}" << endl)
  t << "{" << endl;
  incrementIndentLevel();
  listItemInfo[m_listLevel].isEnum = TRUE;
  listItemInfo[m_listLevel].number = 1;
  //t << Rtf_Style_Reset << Rtf_EList_DepthStyle() << endl;
  //newParagraph();
  //m_omitParagraph=TRUE;

}

/*! end enumeration list */
void RTFGenerator::endEnumList()
{ 
  newParagraph();
  DBG_RTF(t << "{\\comment (endEnumList)}" << endl)
  t << "}";
  decrementIndentLevel();
  m_omitParagraph=TRUE;
}

/*! write bullet or enum item */
void RTFGenerator::writeListItem()
{
  DBG_RTF(t << "{\\comment (writeListItem)}" << endl)
  newParagraph();
  t << Rtf_Style_Reset;
  if (listItemInfo[m_listLevel].isEnum)
  {
    t << Rtf_EList_DepthStyle() << endl;
    t << listItemInfo[m_listLevel].number << ".\\tab ";
    listItemInfo[m_listLevel].number++;
  }
  else
  {
    t << Rtf_BList_DepthStyle() << endl;
  }
  m_omitParagraph=TRUE;
}

void RTFGenerator::writeIndexItem(const char *ref,const char *fn,
    const char *name)
{
  DBG_RTF(t << "{\\comment (writeIndexItem)}" << endl)
  //t << Rtf_LCList_DepthStyle() << endl;
  docify(name);
  if (!ref && fn)
  {
    t << "\\tab ";
    WriteRTFReference(fn);
    t << endl;
  }
  else
  {
    t << endl;	
  }
  newParagraph();
  m_omitParagraph=TRUE;
}

//void RTFGenerator::writeIndexFileItem(const char *,const char *text)
//{
//  t << "\\item\\contentsline{section}{";
//  docify(text);
//  t << "}{\\pageref{" << text << "}}" << endl;
//}

void RTFGenerator::writeHtmlLink(const char *url,const char *text)
{

  if (url && Config::rtfHyperFlag)
  {
    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << url;
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 

    if (text)
      docify(text);
    else
      docify(url);

    t << "}}}" << endl;
  }
  else
  {
    startTypewriter();
    docify(text);
    endTypewriter();
  }
}

void RTFGenerator::writeMailLink(const char *url)
{
  startTypewriter();
  docify(url);
  endTypewriter();
}

void RTFGenerator::writeStartAnnoItem(const char *,const char *f,
    const char *path,const char *name)
{
  DBG_RTF(t << "{\\comment (writeStartAnnoItem)}" << endl)
  t << "{\\b ";
  if (path) docify(path);
  if (f && Config::rtfHyperFlag)
  {
    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << formatBmkStr(f);
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 

    docify(name);

    t << "}}}" << endl;
  }
  else
  {
    docify(name); 
  }
  t << "} ";
}

void RTFGenerator::writeEndAnnoItem(const char *name)
{
  DBG_RTF(t << "{\\comment (writeEndAnnoItem)}" << endl)
  if (name)
  {
    t << "\\tab ";
    WriteRTFReference(name);
    t << endl;
  }
  else
  {
    t << endl;	
  }
  newParagraph();
}

void RTFGenerator::startSubsection() 
{ 
  //beginRTFSubSection();
  t <<"\n";
  DBG_RTF(t << "{\\comment Begin SubSection}\n")
  t << Rtf_Style_Reset;
  t << Rtf_Style["Heading3"]->reference << "\n";
}

void RTFGenerator::endSubsection() 
{
  newParagraph();
  t << Rtf_Style_Reset << endl;
}

void RTFGenerator::startSubsubsection() 
{
  //beginRTFSubSubSection();
  t << "\n";
  DBG_RTF(t << "{\\comment Begin SubSubSection}\n")
  t << "{" << endl;
  t << Rtf_Style_Reset << Rtf_Style["Heading4"]->reference << "\n";
}

void RTFGenerator::endSubsubsection() 
{ 
  newParagraph();
  t << "}" << endl;
} 


//void RTFGenerator::writeClassLink(const char *,const char *,
//                                    const char *,const char *name)
//{
//  t << "{\\bf ";
//  docify(name);
//  t << "}"; 
//}

void RTFGenerator::startTextLink(const char *f,const char *anchor)
{
  if (Config::rtfHyperFlag)
  {
    QCString ref;
    if (f)
    {
      ref+=f;
    }
    if (anchor)
    {
      ref+='_';
      ref+=anchor;
    }

    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << formatBmkStr(ref);
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 
  }
}

void RTFGenerator::endTextLink()
{
  if (Config::rtfHyperFlag)
  {
    t << "}}}" << endl;
  }
}

void RTFGenerator::writeObjectLink(const char *ref, const char *f,
    const char *anchor, const char *text)
{
  if (!ref && Config::rtfHyperFlag)
  {
    QCString refName;
    if (f)
    {
      refName+=f;
    }
    if (anchor)
    {
      refName+='_';
      refName+=anchor;
    }

    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << formatBmkStr(refName);
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 

    docify(text);

    t << "}}}" << endl;
  }
  else
  {
    startBold();
    docify(text);
    endBold();
  } 
}

void RTFGenerator::startPageRef()
{
  t << " (";
  startEmphasis();
}

void RTFGenerator::endPageRef(const char *clname, const char *anchor)
{
  QCString ref;
  if (clname)
  {
    ref+=clname;
  }
  if (anchor)
  {
    ref+='_';
    ref+=anchor;
  }
  WriteRTFReference(ref);
  endEmphasis();
  t << ")";
}

void RTFGenerator::writeCodeLink(const char *ref,const char *f,
    const char *anchor,const char *name)
{
  if (!ref && Config::rtfHyperFlag)
  {
    QCString refName;
    if (f)
    {
      refName+=f;
    }
    if (anchor)
    {
      refName+='_';
      refName+=anchor;
    }

    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << formatBmkStr(refName);
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 

    codify(name);

    t << "}}}" << endl;
  }
  else
  {
    codify(name);
  }
}

void RTFGenerator::startTitleHead(const char *)
{
  DBG_RTF(t <<"{\\comment startTitleHead}" << endl)

  //	beginRTFSection();
  t << Rtf_Style_Reset << Rtf_Style["Heading2"]->reference << endl;
}

void RTFGenerator::endTitleHead(const char *fileName,const char *name)
{
  DBG_RTF(t <<"{\\comment endTitleHead}" << endl)
  t << "\\par " << Rtf_Style_Reset << endl;
  if (name)
  {
    // make table of contents entry
    t << "{\\tc\\tcl2 \\v ";
    docify(name);
    t << "}" << endl;

    // make an index entry
    addIndexItem(name,0);

    //if (name)
    //{
    //  writeAnchor(0,name);
    //}
    //
    //if (Config::rtfHyperFlag && fileName)
    //{
      writeAnchor(fileName,0);
    //}
  }
}

void RTFGenerator::startTitle()
{
  DBG_RTF(t <<"{\\comment startTitle}" << endl)
  if (Config::compactRTFFlag)
    beginRTFSection();
  else
    beginRTFChapter();
}

void RTFGenerator::startGroupHeader()
{
  DBG_RTF(t <<"{\\comment startGroupHeader}" << endl)
  newParagraph();
  t << Rtf_Style_Reset;
  t << Rtf_Style["Heading3"]->reference;
  t << endl;
}

void RTFGenerator::endGroupHeader()
{
  DBG_RTF(t <<"{\\comment endGroupHeader}" << endl)
  newParagraph();
  t << Rtf_Style_Reset << endl;
}

void RTFGenerator::startMemberDoc(const char *clname,
    const char *memname,
    const char *,
    const char *)
{
  DBG_RTF(t << "{\\comment startMemberDoc}" << endl)
  if (memname && memname[0]!='@')
  {
    addIndexItem(memname,clname);
    addIndexItem(clname,memname);
  }
  t << Rtf_Style_Reset << Rtf_Style["Heading4"]->reference;
  //styleStack.push(Rtf_Style_Heading4);
  t << "{" << endl;
  //printf("RTFGenerator::startMemberDoc() `%s'\n",Rtf_Style["Heading4"]->reference);
  startBold();
  t << endl;
}

void RTFGenerator::endMemberDoc() 
{
  DBG_RTF(t << "{\\comment endMemberDoc}" << endl)
  t << "}" << endl;
  //const char *style = styleStack.pop();
  //printf("RTFGenerator::endMemberDoc() `%s'\n",style);
  //ASSERT(style==Rtf_Style["Heading4"]->reference);
  endBold();
  newParagraph();
}

void RTFGenerator::startDoxyAnchor(const char *,const char *,const char *)
{
}

void RTFGenerator::endDoxyAnchor(const char *fName,const char *anchor)
{
  QCString ref;
  if (fName)
  {
    ref+=fName;
  }
  if (anchor)
  {
    ref+='_';
    ref+=anchor;
  }
  
  t << "{\\bkmkstart ";
  t << formatBmkStr(ref);
  t << "}" << endl;
  t << "{\\bkmkend ";
  t << formatBmkStr(ref);
  t << "}" << endl;
}


//void RTFGenerator::writeLatexLabel(const char *clName,const char *anchor)
//{
//  writeDoxyAnchor(0,clName,anchor,0);
//}

void RTFGenerator::addIndexItem(const char *s1,const char *s2)
{
  if (s1)
  {
    t << "{\\xe \\v ";
    docify(s1);
    if (s2)
    {
      t << "\\:";
      docify(s2);
    }
    t << "}" << endl;
  }
}

void RTFGenerator::startIndent()
{
  incrementIndentLevel();
  DBG_RTF(t << "{\\comment (startIndent) }" << endl)
  t << "{" << endl;
  t << Rtf_Style_Reset << Rtf_CList_DepthStyle() << endl;
  //styleStack.push(style);
}

void RTFGenerator::endIndent()
{
  //const char *style = /* Rtf_CList_DepthStyle(); */
  t << "}" << endl;
  decrementIndentLevel();
}


void RTFGenerator::startDescription() 
{ 
  DBG_RTF(t << "{\\comment (startDescription)}"    << endl)
  t << "{" << endl;
  t << Rtf_Style_Reset << Rtf_CList_DepthStyle();
}

void RTFGenerator::endDescription()   
{ 
  DBG_RTF(t << "{\\comment (endDescription)}"    << endl)
  t << "}";
  newParagraph();
}

void RTFGenerator::startDescItem()
{ 
  newParagraph();
  DBG_RTF(t << "{\\comment (startDescItem)}"    << endl)
  t << "{\\b ";
}

void RTFGenerator::endDescItem()
{ 
  DBG_RTF(t << "{\\comment (endDescItem)}"    << endl)
  t << "}" << endl;
  newParagraph();
}

void RTFGenerator::startMemberDescription() 
{ 
  DBG_RTF(t << "{\\comment (startMemberDescription)}"    << endl)
  t << "{" << endl;
  incrementIndentLevel();
  t << Rtf_Style_Reset << Rtf_CList_DepthStyle();
  startEmphasis();
}

void RTFGenerator::endMemberDescription()
{ 
  DBG_RTF(t << "{\\comment (endMemberDescription)}"    << endl)
  endEmphasis();
  newParagraph();
  decrementIndentLevel();
  t << "}" << endl;
}

void RTFGenerator::startDescList()     
{ 
  DBG_RTF(t << "{\\comment (startDescList)}"    << endl)
  t << "{";
  newParagraph();
}

void RTFGenerator::endDescTitle()      
{ 
  DBG_RTF(t << "{\\comment (endDescTitle) }"    << endl)
  newParagraph();
  //t << Rtf_Style_Reset << styleStack.top();
  incrementIndentLevel();
  t << Rtf_Style_Reset << Rtf_DList_DepthStyle();
}

void RTFGenerator::writeDescItem()
{
  DBG_RTF(t << "{\\comment (writeDescItem) }"    << endl)
  //	incrementIndentLevel();
  //t << Rtf_Style_Reset << Rtf_CList_DepthStyle();
}

void RTFGenerator::endDescList()       
{
  DBG_RTF(t << "{\\comment (endDescList)}"    << endl)
  newParagraph();
  t << "}";
  decrementIndentLevel();
  m_omitParagraph = TRUE;
  //t << Rtf_Style_Reset << styleStack.top() << endl;
}

void RTFGenerator::startSection(const char *,const char *title,bool sub)
{
  DBG_RTF(t << "{\\comment (startSection)}"    << endl)
  t << "{";
  t<< Rtf_Style_Reset;
  if (sub)
  {
    // set style
    t << Rtf_Style["Heading3"]->reference;
    // make table of contents entry
    t << "{\\tc\\tcl3 \\v ";
    docify(title);
    t << "}" << endl;
  }
  else
  {
    // set style
    t << Rtf_Style["Heading2"]->reference;
    // make table of contents entry
    t << "{\\tc\\tcl2 \\v ";
    docify(title);
    t << "}" << endl;
  }
}

void RTFGenerator::endSection(const char *lab,bool)
{
  newParagraph();
  // make bookmark
  writeAnchor(0,lab);
  t << "}";
}

void RTFGenerator::writeSectionRef(const char *,const char *lab,
    const char *title)
{
  startBold();
  docify(title);
  endBold();
  t << " (";
  docify(theTranslator->trPageAbbreviation());
  WriteRTFReference(lab);
  t << ")" << endl;
}

void RTFGenerator::writeSectionRefItem(const char *,const char *lab,
    const char *title)
{
  docify(title);
  t << "\\tab";
  WriteRTFReference(lab);
  t << endl;
}

void RTFGenerator::writeSectionRefAnchor(const char *name,const char *lab,
    const char *title)
{
  writeSectionRef(name,lab,title);
}

void RTFGenerator::docify(const char *str)
{
  if (str)
  {
    const unsigned char *p=(const unsigned char *)str;
    unsigned char c;
    unsigned char pc='\0';
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '{':  t << "\\{";            break;
        case '}':  t << "\\}";            break;
        case '\\': t << "\\\\";	          break;
        default:   
          { 
            // see if we can insert an hyphenation hint
            //if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
            t << (char)c;    
          }
      }
      pc = c;
      m_omitParagraph = FALSE;
    }
  }
}

void RTFGenerator::codify(const char *str)
{
  // note that RTF does not have a "verbatim", so "\n" means
  // nothing... add a "newParagraph()";
  //static char spaces[]="        ";
  if (str)
  { 
    const char *p=str;
    char c;
    int spacesToNextTabStop;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '\t':  spacesToNextTabStop = Config::tabSize - (col%Config::tabSize);
                    t << spaces.left(spacesToNextTabStop); 
                    col+=spacesToNextTabStop;
                    break; 
        case '\n':  newParagraph();
                    t << '\n'; col=0;
                    break;
        case '{':   t << "\\{"; col++;          break;
        case '}':   t << "\\}"; col++;          break;
        case '\\':  t << "\\\\"; col++;         break;
        default:    t << c;    col++;           break;
      }
    }
  }
}

void RTFGenerator::writeChar(char c)
{
  char cs[2];
  cs[0]=c;
  cs[1]=0;
  docify(cs);
}

void RTFGenerator::startClassDiagram()
{
}

void RTFGenerator::endClassDiagram(ClassDiagram &d,
    const char *fileName,const char *)
{
  newParagraph();

  // create a gif file
  d.writeImage(t,dir,fileName,FALSE);

  // display the file
  t << "{" << endl;
  t << Rtf_Style_Reset << endl;
  t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE ";
  t << fileName << ".gif";
  t << " \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par" << endl;
  t << "}" << endl;
}

void RTFGenerator::writeFormula(const char *,const char *text)
{
  t << text;
}

void RTFGenerator::startMemberItem(int) 
{ 
  DBG_RTF(t <<"{\\comment startMemberItem }" << endl)
  t << Rtf_Style_Reset << Rtf_BList_DepthStyle() << endl; // set style to apropriate depth
}

void RTFGenerator::endMemberItem(bool) 
{
  DBG_RTF(t <<"{\\comment endMemberItem }" << endl)
  newParagraph();
}

void RTFGenerator::writeAnchor(const char *fileName,const char *name) 
{ 
  t << "{\\bkmkstart ";
  if (fileName) t << formatBmkStr(fileName);
  if (fileName && name) t << "_";
  if (name) t << formatBmkStr(name);
  t << "}" << endl;
  t << "{\\bkmkend " << formatBmkStr(name) << "}" << endl;
}

void RTFGenerator::WriteRTFReference(const char *label)
{
  t << "{\\field\\fldedit {\\*\\fldinst PAGEREF ";
  t << formatBmkStr(label);
  t << " \\\\*MERGEFORMAT}{\\fldrslt pagenum}}";
}

void RTFGenerator::startCodeFragment() 
{ 
  t << "{" << endl;
  newParagraph();
  t << Rtf_Style_Reset << Rtf_Code_DepthStyle();
  //styleStack.push(Rtf_Style_CodeExample);
}

void RTFGenerator::endCodeFragment()
{ 
  newParagraph();
  //styleStack.pop();
  //printf("RTFGenerator::endCodeFrament() top=%s\n",styleStack.top());
  //t << Rtf_Style_Reset << styleStack.top() << endl;
  t << "}" << endl;
  m_omitParagraph = TRUE;
}

void RTFGenerator::writeNonBreakableSpace() 
{
  t << "\\~ ";
}


void RTFGenerator::startMemberList()  
{
  t << endl;
  DBG_RTF(t << "{\\comment (startMemberList) }"    << endl)
  t << "{" << endl;
#ifdef DELETEDCODE
  if (!insideTabbing)
    t << "\\begin{CompactItemize}" << endl; 
#endif
}

void RTFGenerator::endMemberList()    
{
  DBG_RTF(t << "{\\comment (endMemberList) }"    << endl)
  t << "}" << endl;
#ifdef DELETEDCODE
  if (!insideTabbing)
    t << "\\end{CompactItemize}"   << endl; 
#endif
}

void RTFGenerator::startImage(const char *,const char *,bool)
{
  // not yet implemented
}

void RTFGenerator::endImage(bool)
{
  // not yet implemented
}

void RTFGenerator::startDescTable()  
{ 
  DBG_RTF(t << "{\\comment (startDescTable) }"    << endl)
  t << "{" << endl;
  //incrementIndentLevel();
  //t << Rtf_Style_Reset << Rtf_CList_DepthStyle();
}

void RTFGenerator::endDescTable()    
{ 
  //decrementIndentLevel();
  DBG_RTF(t << "{\\comment (endDescTable)}"      << endl)
  t << "}" << endl;
  //t << Rtf_Style_Reset << styleStack.top();
}

void RTFGenerator::startDescTableTitle()
{
  //t << Rtf_BList_DepthStyle() << endl;
  DBG_RTF(t << "{\\comment (startDescTableTitle) }"    << endl)
  startBold();
}

void RTFGenerator::endDescTableTitle()
{
  DBG_RTF(t << "{\\comment (endDescTableTitle) }"    << endl)
  endBold();
  t << "  ";
}

void RTFGenerator::startDescTableData() 
{
  DBG_RTF(t << "{\\comment (startDescTableData) }"    << endl)
}

void RTFGenerator::endDescTableData() 
{
  DBG_RTF(t << "{\\comment (endDescTableData) }"    << endl)
  newParagraph();
}

// a style for list formatted as a "bulleted list"

void RTFGenerator::incrementIndentLevel()
{
  m_listLevel++;
  if (m_listLevel>indentLevels-1) 
  {
    warn_cont("Warning: Maximum indent level (%d) exceeded while generating RTF output!\n",indentLevels); 
    m_listLevel=indentLevels-1;
  }
}

void RTFGenerator::decrementIndentLevel()
{
  m_listLevel--;
  if (m_listLevel<0) 
  {
    warn_cont("Warning: Negative indent level while generating RTF output!\n");
    m_listLevel=0;
  }
}

// a style for list formatted with "list continue" style
const char * RTFGenerator::Rtf_CList_DepthStyle()
{
  QCString n=makeIndexName("ListContinue",m_listLevel);
  return Rtf_Style[n]->reference;
}

// a style for list formatted as a "latext style" table of contents
const char * RTFGenerator::Rtf_LCList_DepthStyle()
{
  QCString n=makeIndexName("LatexTOC",m_listLevel);
  return Rtf_Style[n]->reference;
}

// a style for list formatted as a "bullet" style 
const char * RTFGenerator::Rtf_BList_DepthStyle()
{
  QCString n=makeIndexName("ListBullet",m_listLevel);
  return Rtf_Style[n]->reference;
}

// a style for list formatted as a "enumeration" style 
const char * RTFGenerator::Rtf_EList_DepthStyle()
{
  QCString n=makeIndexName("ListEnum",m_listLevel);
  return Rtf_Style[n]->reference;
}

const char * RTFGenerator::Rtf_DList_DepthStyle()
{
  QCString n=makeIndexName("DescContinue",m_listLevel);
  return Rtf_Style[n]->reference;
}

const char * RTFGenerator::Rtf_Code_DepthStyle()
{
  QCString n=makeIndexName("CodeExample",m_listLevel);
  return Rtf_Style[n]->reference;
}

void RTFGenerator::startTextBlock(bool dense)
{
  DBG_RTF(t << "{\\comment Start TextBlock}" << endl)
  t << "{" << endl;
  t << Rtf_Style_Reset;
  if (dense) // no spacing between "paragraphs"
  {
    t << Rtf_Style["DenseText"]->reference;
  }
  else // some spacing
  {
    t << Rtf_Style["BodyText"]->reference;
  }
}

void RTFGenerator::endTextBlock()
{
  newParagraph();
  t << "}" << endl;
  DBG_RTF(t << "{\\comment End TextBlock}" << endl)
  m_omitParagraph = TRUE;
}

void RTFGenerator::newParagraph()
{
  if (!m_omitParagraph) t << "\\par" << endl;
  m_omitParagraph = FALSE;
}

void RTFGenerator::startMemberSubtitle()
{
  t << "{" << endl;
  t << Rtf_Style_Reset << Rtf_CList_DepthStyle() << endl;
}

void RTFGenerator::endMemberSubtitle()
{
  newParagraph();
  t << "}" << endl;
}

void RTFGenerator::writeUmlaut(char c)
{
  switch(c)
  {
    case 'A' : t << '\304'; break;
    case 'E' : t << '\313'; break;
    case 'I' : t << '\317'; break;
    case 'O' : t << '\326'; break;
    case 'U' : t << '\334'; break;
    case 'Y' : t << 'Y';    break;
    case 'a' : t << '\344'; break;
    case 'e' : t << '\353'; break;
    case 'i' : t << '\357'; break;
    case 'o' : t << '\366'; break;
    case 'u' : t << '\374'; break;
    case 'y' : t << '\377'; break;
    default: t << '?'; break;
  }
}

void RTFGenerator::writeAcute(char c)
{
  switch(c)
  {
    case 'A' : t << '\301'; break;
    case 'E' : t << '\311'; break;
    case 'I' : t << '\315'; break;
    case 'O' : t << '\323'; break;
    case 'U' : t << '\332'; break;
    case 'Y' : t << '\335'; break;
    case 'a' : t << '\341'; break;
    case 'e' : t << '\351'; break;
    case 'i' : t << '\355'; break;
    case 'o' : t << '\363'; break;
    case 'u' : t << '\372'; break;
    case 'y' : t << '\375'; break;
    default: t << '?'; break;
  }
}

void RTFGenerator::writeGrave(char c)
{
  switch(c)
  {
    case 'A' : t << '\300'; break;
    case 'E' : t << '\310'; break;
    case 'I' : t << '\314'; break;
    case 'O' : t << '\322'; break;
    case 'U' : t << '\331'; break;
    case 'a' : t << '\340'; break;
    case 'e' : t << '\350'; break;
    case 'i' : t << '\354'; break;
    case 'o' : t << '\362'; break;
    case 'u' : t << '\371'; break;
    default: t << '?'; break;
  }
}

void RTFGenerator::writeCirc(char c)
{
  switch(c)
  {
    case 'A' : t << '\302'; break;
    case 'E' : t << '\312'; break;
    case 'I' : t << '\316'; break;
    case 'O' : t << '\324'; break;
    case 'U' : t << '\333'; break;
    case 'a' : t << '\342'; break;
    case 'e' : t << '\352'; break;
    case 'i' : t << '\356'; break;
    case 'o' : t << '\364'; break;
    case 'u' : t << '\373'; break;
    default: t << '?'; break;
  }
}

void RTFGenerator::writeTilde(char c)
{
  switch(c)
  {
    case 'A' : t << '\303'; break;
    case 'N' : t << '\321'; break;
    case 'O' : t << '\325'; break;
    case 'a' : t << '\343'; break;
    case 'n' : t << '\361'; break;
    case 'o' : t << '\365'; break;
    default: t << '?'; break;
  }
}

void RTFGenerator::writeRing(char c)
{
  switch(c)
  {
    case 'A' : t << '\305'; break;
    case 'a' : t << '\345'; break;
    default: t << '?'; break;
  }
}

/**
 * VERY brittle routine inline RTF's included by other RTF's.
 * it is recursive and ugly.
 */
static bool PreProcessFile(QDir &d,QCString &infName, QTextStream &t, bool bIncludeHeader=TRUE)
{
  QFile f(infName);
  if (!f.open(IO_ReadOnly))
  {
    err("Error opening rtf file %s for reading\n",infName.data());
    return FALSE;
  }

  const int maxLineLength = 10240; 
  static QCString lineBuf(maxLineLength);

  // scan until find end of header
  // this is EXTREEEEEEEMLY brittle.  It works on OUR rtf
  // files because the first line before the body
  // ALWAYS contains "{\comment begin body}"
  do
  {
    if (f.readLine(lineBuf.data(),maxLineLength)==-1)
    {
      err("ERROR - read error in %s before end of RTF header!\n",infName.data());
      return FALSE;
    }
    if (bIncludeHeader) t << lineBuf;
  } while (lineBuf.find("\\comment begin body")==-1);


  //while (fgets(buffer,sizeof(buffer),infp) != NULL)
  while (f.readLine(lineBuf.data(),maxLineLength)!=-1)
  {
    int pos;
    if ((pos=lineBuf.find("INCLUDETEXT"))!=-1)
    {
      int startNamePos  = lineBuf.find('"',pos)+1;
      int endNamePos    = lineBuf.find('"',startNamePos);
      QCString fileName = lineBuf.mid(startNamePos,endNamePos-startNamePos);
      DBG_RTF(t << "{\\comment begin include " << fileName << "}" << endl)
      if (!PreProcessFile(d,fileName,t,FALSE)) return FALSE;
      DBG_RTF(t << "{\\comment end include " << fileName << "}" << endl)
    }
    else
    {
      // elaborate hoopla to skip  the final "}" if we didn't include the
      // headers
      if (!f.atEnd() || bIncludeHeader)
      {
        t << lineBuf;
      }
      else
      {
        // null terminate at the last '}'
        //char *str = strrchr(buffer,'}');
        int pos = lineBuf.findRev('}');

        if (pos != -1)
          lineBuf.at(pos) = '\0';
        else
          err("Strange, the last char was not a '}'\n");
        t << lineBuf;
      }
    }
  }
  f.close();
  // remove temporary file
  d.remove(infName);
  return TRUE;
}

void RTFGenerator::startDotGraph() 
{
}

void RTFGenerator::endDotGraph(DotClassGraph &g) 
{
  newParagraph();

  g.writeGraph(t,GIF,Config::rtfOutputDir,TRUE,FALSE);

  // display the file
  t << "{" << endl;
  t << Rtf_Style_Reset << endl;
  t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE ";
  t << g.diskName() << ".gif";
  t << " \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par" << endl;
  t << "}" << endl;
}

void RTFGenerator::startInclDepGraph() 
{
}

void RTFGenerator::endInclDepGraph(DotInclDepGraph &g) 
{
  newParagraph();

  g.writeGraph(t,GIF,Config::rtfOutputDir,FALSE);

  //QCString diskName = g.diskName();

  // display the file
  t << "{" << endl;
  t << Rtf_Style_Reset << endl;
  t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE ";
  t << g.diskName() << ".gif";
  t << " \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par" << endl;
  t << "}" << endl;
}

/**
 * This is an API to a VERY brittle RTF preprocessor that combines nested
 * RTF files.  This version replaces the infile with the new file 
 */
bool RTFGenerator::preProcessFileInplace(const char *path,const char *name)
{
  QDir d(path);
  // store the original directory
  if (!d.exists()) 
  { 
    err("Error: Output dir %s does not exist!\n",path); 
    return FALSE;
  }
  QCString oldDir = convertToQCString(QDir::currentDirPath());

  // goto the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;

  QCString combinedName = (QCString)path+"/combined.rtf";
  QCString mainRTFName  = (QCString)path+"/"+name;

  QFile outf(combinedName);
  if (!outf.open(IO_WriteOnly))
  {
    err("Failed to open %s for writing!\n",combinedName.data());
    return FALSE;
  }
  QTextStream outt(&outf);

  if (!PreProcessFile(thisDir,mainRTFName,outt))
  {
    // it failed, remove the temp file
    outf.close();
    thisDir.remove(combinedName);
    QDir::setCurrent(oldDir);
    return FALSE;
  }

  // everything worked, move the files
  outf.close();
  thisDir.remove(mainRTFName);
  thisDir.rename(combinedName,mainRTFName);

  QDir::setCurrent(oldDir);
  return TRUE;
}

void RTFGenerator::startMemberGroupHeader(bool hasHeader)
{
  DBG_RTF(t << "{\\comment startMemberGroupHeader}" << endl)
  t << "{" << endl;
  if (hasHeader) incrementIndentLevel();
  t << Rtf_Style_Reset << Rtf_Style["GroupHeader"]->reference;
}

void RTFGenerator::endMemberGroupHeader()
{
  DBG_RTF(t << "{\\comment endMemberGroupHeader}" << endl)
  newParagraph();
  t << Rtf_Style_Reset << Rtf_CList_DepthStyle();
}

void RTFGenerator::startMemberGroupDocs()
{
  DBG_RTF(t << "{\\comment startMemberGroupDocs}" << endl)
  startEmphasis();
}

void RTFGenerator::endMemberGroupDocs()
{
  DBG_RTF(t << "{\\comment endMemberGroupDocs}" << endl)
  endEmphasis();
  newParagraph();
}

void RTFGenerator::startMemberGroup()
{
  DBG_RTF(t << "{\\comment startMemberGroup}" << endl)
  t << Rtf_Style_Reset << Rtf_BList_DepthStyle() << endl;
}

void RTFGenerator::endMemberGroup(bool hasHeader)
{
  DBG_RTF(t << "{\\comment endMemberGroup}" << endl)
  if (hasHeader) decrementIndentLevel();
  t << "}";
}

