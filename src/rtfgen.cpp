/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Parker Waechter & Dimitri van Heesch.
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

#include <mutex>
#include <stdlib.h>
#include <algorithm>
#include <unordered_map>

#include "rtfgen.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "util.h"
#include "diagram.h"
#include "language.h"
#include "dot.h"
#include "dotcallgraph.h"
#include "dotclassgraph.h"
#include "dotdirdeps.h"
#include "dotincldepgraph.h"
#include "version.h"
#include "pagedef.h"
#include "rtfstyle.h"
#include "rtfdocvisitor.h"
#include "docparser.h"
#include "dirdef.h"
#include "vhdldocgen.h"
#include "portable.h"
#include "groupdef.h"
#include "classlist.h"
#include "filename.h"
#include "namespacedef.h"
#include "dir.h"
#include "utf8.h"
#include "debug.h"
#include "datetime.h"
#include "outputlist.h"
#include "moduledef.h"

//#define DBG_RTF(x) x;
#define DBG_RTF(x)

static StringSet removeSet;

static QCString dateToRTFDateString()
{
  auto tm = getCurrentDateTime();
  QCString result;
  switch (Config_getEnum(TIMESTAMP))
  {
    case TIMESTAMP_t::YES:
    case TIMESTAMP_t::DATETIME:
      result.sprintf("\\yr%d\\mo%d\\dy%d\\hr%d\\min%d\\sec%d",
          tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
          tm.tm_hour, tm.tm_min, tm.tm_sec);
      break;
    case TIMESTAMP_t::DATE:
      result.sprintf("\\yr%d\\mo%d\\dy%d",
          tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
      break;
    case TIMESTAMP_t::NO:
      return "";
  }
  return "{\\creatim " + result + "}\n";
}

static QCString docifyToString(const QCString &str)
{
  QCString result;
  result.reserve(str.length());
  if (!str.isEmpty())
  {
    const char *p=str.data();
    while (*p)
    {
      char c=*p++;

      switch (c)
      {
        case '{':  result += "\\{";            break;
        case '}':  result += "\\}";            break;
        case '\\': result += "\\\\";           break;
        default:   result += c;                break;
      }
    }
  }
  return result;
}

static QCString makeIndexName(const QCString &s,int i)
{
  QCString result=s;
  result+=static_cast<char>(i+'0');
  return result;
}


//------------------------------------------------------------------------------------------------

RTFCodeGenerator::RTFCodeGenerator(TextStream *t) : m_t(t)
{
}

void RTFCodeGenerator::writeCodeLink(CodeSymbolType,
                                 const QCString &ref,const QCString &f,
                                 const QCString &anchor,const QCString &name,
                                 const QCString &)
{
  if (m_hide) return;
  if (ref.isEmpty() && Config_getBool(RTF_HYPERLINKS))
  {
    QCString refName;
    if (!f.isEmpty())
    {
      refName+=stripPath(f);
    }
    if (!anchor.isEmpty())
    {
      refName+='_';
      refName+=anchor;
    }

    *m_t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    *m_t << rtfFormatBmkStr(refName);
    *m_t << "\" }{}";
    *m_t << "}{\\fldrslt {\\cs37\\ul\\cf2 ";

    codify(name);

    *m_t << "}}}\n";
  }
  else
  {
    codify(name);
  }
}

void RTFCodeGenerator::codify(const QCString &str)
{
  if (m_hide) return;
  // note that RTF does not have a "verbatim", so "\n" means
  // nothing... add a "newParagraph()";
  //static char spaces[]="        ";
  if (!str.isEmpty())
  {
    const char *p=str.data();

    while (*p)
    {
      //static bool MultiByte = FALSE;

      char c=*p++;

      switch(c)
      {
        case '\t':  {
                      int spacesToNextTabStop = Config_getInt(TAB_SIZE) - (m_col%Config_getInt(TAB_SIZE));
                      *m_t << Doxygen::spaces.left(spacesToNextTabStop);
                      m_col+=spacesToNextTabStop;
                      break;
                    }
        case '\n':  *m_t << "\\par\n";
                    m_col=0;
                    break;
        case '{':   *m_t << "\\{"; m_col++;          break;
        case '}':   *m_t << "\\}"; m_col++;          break;
        case '\\':  *m_t << "\\\\"; m_col++;         break;
        default:    p=writeUTF8Char(*m_t,p-1); m_col++; break;
      }
    }
  }
}

void RTFCodeGenerator::stripCodeComments(bool b)
{
  m_stripCodeComments = b;
}

void RTFCodeGenerator::startSpecialComment()
{
  m_hide = m_stripCodeComments;
}

void RTFCodeGenerator::endSpecialComment()
{
  m_hide = false;
}


void RTFCodeGenerator::startCodeFragment(const QCString &)
{
  DBG_RTF(*m_t << "{\\comment (startCodeFragment) }\n")
  *m_t << "{\n";
  *m_t << "\\par\n";
  *m_t << rtf_Style_Reset << rtf_Code_DepthStyle();
}

void RTFCodeGenerator::endCodeFragment(const QCString &)
{
  endCodeLine();

  DBG_RTF(*m_t << "{\\comment (endCodeFragment) }\n")
  *m_t << "}\n";
  //m_omitParagraph = TRUE;
}

void RTFCodeGenerator::writeLineNumber(const QCString &ref,const QCString &fileName,const QCString &anchor,int l,bool writeLineAnchor)
{
  if (m_hide) return;
  bool rtfHyperlinks = Config_getBool(RTF_HYPERLINKS);

  m_doxyCodeLineOpen = true;
  if (Config_getBool(SOURCE_BROWSER))
  {
    QCString lineNumber;
    lineNumber.sprintf("%05d",l);

    QCString lineAnchor;
    if (!m_sourceFileName.isEmpty())
    {
      lineAnchor.sprintf("_l%05d",l);
      lineAnchor.prepend(stripExtensionGeneral(stripPath(m_sourceFileName), ".rtf"));
    }
    bool showTarget = rtfHyperlinks && !lineAnchor.isEmpty() && writeLineAnchor;
    if (showTarget)
    {
        *m_t << "{\\bkmkstart ";
        *m_t << rtfFormatBmkStr(lineAnchor);
        *m_t << "}";
        *m_t << "{\\bkmkend ";
        *m_t << rtfFormatBmkStr(lineAnchor);
        *m_t << "}\n";
    }
    if (!fileName.isEmpty())
    {
      writeCodeLink(CodeSymbolType::Default,ref,fileName,anchor,lineNumber,QCString());
    }
    else
    {
      *m_t << lineNumber;
    }
    *m_t << " ";
  }
  else
  {
    *m_t << l << " ";
  }
  m_col=0;
}

void RTFCodeGenerator::startCodeLine(int)
{
  if (m_hide) return;
  m_doxyCodeLineOpen = true;
  m_col=0;
}

void RTFCodeGenerator::endCodeLine()
{
  if (m_hide) return;
  if (m_doxyCodeLineOpen) *m_t << "\\par\n";
  m_doxyCodeLineOpen = false;
}

void RTFCodeGenerator::startFontClass(const QCString &name)
{
  if (m_hide) return;
  int cod = 2;
  static const std::unordered_map<std::string,int> map {
   { "keyword",       17 },
   { "keywordtype",   18 },
   { "keywordflow",   19 },
   { "comment",       20 },
   { "preprocessor",  21 },
   { "stringliteral", 22 },
   { "charliteral",   23 },
   { "vhdldigit",     24 },
   { "vhdlchar",      25 },
   { "vhdlkeyword",   26 },
   { "vhdllogic",     27 }
  };
  auto it = map.find(name.str());
  if (it != map.end())
  {
    cod = it->second;
  }
  *m_t << "{\\cf" << cod << " ";
}

void RTFCodeGenerator::endFontClass()
{
  if (m_hide) return;
  *m_t << "}";
}

QCString RTFCodeGenerator::rtf_Code_DepthStyle()
{
  QCString n=makeIndexName("CodeExample",m_indentLevel);
  return rtf_Style[n.str()].reference();
}

void RTFCodeGenerator::setSourceFileName(const QCString &name)
{
  m_sourceFileName = name;
}

//------------------------------------------------------------------------------------------------

RTFGenerator::RTFGenerator()
  : OutputGenerator(Config_getString(RTF_OUTPUT))
  , m_codeList(std::make_unique<OutputCodeList>())
{
  m_codeGen = m_codeList->add<RTFCodeGenerator>(&m_t);
}

RTFGenerator::RTFGenerator(const RTFGenerator &og) : OutputGenerator(og.m_dir), OutputGenIntf()
{
  m_codeList       = std::make_unique<OutputCodeList>(*og.m_codeList);
  m_codeGen        = m_codeList->get<RTFCodeGenerator>(OutputType::RTF);
  m_codeGen->setTextStream(&m_t);
  m_bstartedBody   = og.m_bstartedBody;
  m_omitParagraph  = og.m_omitParagraph;
  m_numCols        = og.m_numCols;
  m_relPath        = og.m_relPath;
  m_indentLevel    = og.m_indentLevel;
  m_listItemInfo   = og.m_listItemInfo;
}

RTFGenerator &RTFGenerator::operator=(const RTFGenerator &og)
{
  if (this!=&og)
  {
    m_dir            = og.m_dir;
    m_codeList       = std::make_unique<OutputCodeList>(*og.m_codeList);
    m_codeGen        = m_codeList->get<RTFCodeGenerator>(OutputType::RTF);
    m_codeGen->setTextStream(&m_t);
    m_bstartedBody   = og.m_bstartedBody;
    m_omitParagraph  = og.m_omitParagraph;
    m_numCols        = og.m_numCols;
    m_relPath        = og.m_relPath;
    m_indentLevel    = og.m_indentLevel;
    m_listItemInfo   = og.m_listItemInfo;
  }
  return *this;
}

RTFGenerator::~RTFGenerator() = default;

void RTFGenerator::addCodeGen(OutputCodeList &list)
{
  list.add<RTFCodeGeneratorDefer>(m_codeGen);
}

void RTFGenerator::setRelativePath(const QCString &path)
{
  m_relPath = path;
}

void RTFGenerator::setSourceFileName(const QCString &name)
{
  m_codeGen->setSourceFileName(name);
}

void RTFGenerator::writeStyleSheetFile(TextStream &t)
{
  t << "# Generated by doxygen " << getDoxygenVersion() << "\n\n";
  t << "# This file describes styles used for generating RTF output.\n";
  t << "# All text after a hash (#) is considered a comment and will be ignored.\n";
  t << "# Remove a hash to activate a line.\n\n";

  for (int i=0 ; rtf_Style_Default[i].reference!=nullptr ; i++ )
  {
    t << "# " << rtf_Style_Default[i].name << " = "
              << rtf_Style_Default[i].reference
              << rtf_Style_Default[i].definition << "\n";
  }
}

void RTFGenerator::writeExtensionsFile(TextStream &t)
{
  t << "# Generated by doxygen " << getDoxygenVersion() << "\n\n";
  t << "# This file describes extensions used for generating RTF output.\n";
  t << "# All text after a hash (#) is considered a comment and will be ignored.\n";
  t << "# Remove a hash to activate a line.\n\n";

  t << "# Overrides the project title.\n";

  t << "#Title           = \n\n";

  t << "# Name of the company that produced this document.\n";
  t << "#Company         = \n\n";

  t << "# Filename of a company or project logo.\n";
  t << "#LogoFilename    = \n\n";

  t << "# Author of the document.\n";
  t << "#Author          = \n\n";

  t << "# Type of document (e.g. Design Specification, User Manual, etc.).\n";
  t << "#DocumentType    = \n\n";

  t << "# Document tracking number.\n";
  t << "#DocumentId      = \n\n";

  t << "# Name of the author's manager.\n";
  t << "# This field is not displayed in the document itself, but it is \n";
  t << "# available in the information block of the rtf file.  In Microsoft \n";
  t << "# Word, it is available under File:Properties.\n";
  t << "#Manager         = \n\n";

  t << "# Subject of the document.\n";
  t << "# This field is not displayed in the document itself, but it is \n";
  t << "# available in the information block of the rtf file.  In Microsoft \n";
  t << "# Word, it is available under File:Properties.\n";
  t << "#Subject         = \n\n";

  t << "# Comments regarding the document.\n";
  t << "# This field is not displayed in the document itself, but it is \n";
  t << "# available in the information block of the rtf file.  In Microsoft \n";
  t << "# Word, it is available under File:Properties.\n";
  t << "#Comments        = \n\n";

  t << "# Keywords associated with the document.\n";
  t << "# This field is not displayed in the document itself, but it is \n";
  t << "# available in the information block of the rtf file.  In Microsoft \n";
  t << "# Word, it is available under File:Properties.\n";
  t << "#Keywords        = \n\n";
}


void RTFGenerator::init()
{
  QCString dir=Config_getString(RTF_OUTPUT);
  Dir d(dir.str());
  if (!d.exists() && !d.mkdir(dir.str()))
  {
    term("Could not create output directory %s\n",qPrint(dir));
  }

  // first duplicate strings of rtf_Style_Default
  const struct Rtf_Style_Default* def = rtf_Style_Default;
  while (def->reference)
  {
    if (def->definition == nullptr)
    {
      err("Internal: rtf_Style_Default[%s] has no definition.\n", def->name);
    }
    else
    {
      rtf_Style.emplace(def->name, StyleData(def->reference, def->definition));
    }
    def++;
  }

  // overwrite some (or all) definitions from file
  QCString rtfStyleSheetFile = Config_getString(RTF_STYLESHEET_FILE);
  if (!rtfStyleSheetFile.isEmpty())
  {
    loadStylesheet(rtfStyleSheetFile, rtf_Style);
  }

  // If user has defined an extension file, load its contents.
  QCString rtfExtensionsFile = Config_getString(RTF_EXTENSIONS_FILE);
  if (!rtfExtensionsFile.isEmpty())
  {
    loadExtensions(rtfExtensionsFile);

    if (!rtf_logoFilename.isEmpty())
    {
      FileInfo fi(rtf_logoFilename.str());
      if (!fi.exists())
      {
        err("Logo '%s' specified by 'LogoFilename' in the rtf extension file '%s' does not exist!\n",
            qPrint(rtf_logoFilename), qPrint(rtfExtensionsFile));
        rtf_logoFilename = "";
      }
      else
      {
        QCString destFileName = Config_getString(RTF_OUTPUT)+"/"+fi.fileName();
        copyFile(rtf_logoFilename,destFileName);
        rtf_logoFilename = fi.fileName();
      }
    }
  }

  createSubDirs(d);
}

void RTFGenerator::cleanup()
{
  QCString dname = Config_getString(RTF_OUTPUT);
  Dir d(dname.str());
  clearSubDirs(d);
}

void RTFGenerator::beginRTFDocument()
{
  /* all the included RTF files should begin with the
   * same header
   */
  m_t << "{\\rtf1\\ansi\\ansicpg" << theTranslator->trRTFansicp();
  m_t << "\\uc1 \\deff0\\deflang1033\\deflangfe1033\n";

  DBG_RTF(m_t << "{\\comment Beginning font list}\n")
  m_t << "{\\fonttbl ";
  m_t << "{\\f0\\froman\\fcharset" << theTranslator->trRTFCharSet();
  m_t << "\\fprq2{\\*\\panose 02020603050405020304}Times New Roman;}\n";
  m_t << "{\\f1\\fswiss\\fcharset" << theTranslator->trRTFCharSet();
  m_t << "\\fprq2{\\*\\panose 020b0604020202020204}Arial;}\n";
  m_t << "{\\f2\\fmodern\\fcharset" << theTranslator->trRTFCharSet();
  m_t << "\\fprq1{\\*\\panose 02070309020205020404}Courier New;}\n";
  m_t << "{\\f3\\froman\\fcharset2\\fprq2{\\*\\panose 05050102010706020507}Symbol;}\n";
  m_t << "}\n";
  DBG_RTF(m_t << "{\\comment begin colors}\n")
  m_t << "{\\colortbl;";
  m_t << "\\red0\\green0\\blue0;";
  m_t << "\\red0\\green0\\blue255;";
  m_t << "\\red0\\green255\\blue255;";
  m_t << "\\red0\\green255\\blue0;";
  m_t << "\\red255\\green0\\blue255;";
  m_t << "\\red255\\green0\\blue0;";
  m_t << "\\red255\\green255\\blue0;";
  m_t << "\\red255\\green255\\blue255;";
  m_t << "\\red0\\green0\\blue128;";
  m_t << "\\red0\\green128\\blue128;";
  m_t << "\\red0\\green128\\blue0;";
  m_t << "\\red128\\green0\\blue128;";
  m_t << "\\red128\\green0\\blue0;";
  m_t << "\\red128\\green128\\blue0;";
  m_t << "\\red128\\green128\\blue128;";
  m_t << "\\red192\\green192\\blue192;";

  // code highlighting colors. Note order is important see also RTFGenerator::startFontClass
  m_t << "\\red0\\green128\\blue0;";   // keyword = index 17
  m_t << "\\red96\\green64\\blue32;";  // keywordtype
  m_t << "\\rede0\\green128\\blue0;";  // keywordflow
  m_t << "\\red128\\green0\\blue0;";   // comment
  m_t << "\\red128\\green96\\blue32;"; // preprocessor
  m_t << "\\red0\\green32\\blue128;";  // stringliteral
  m_t << "\\red0\\green128\\blue128;"; // charliteral
  m_t << "\\red255\\green0\\blue255;"; // vhdldigit
  m_t << "\\red0\\green0\\blue0;";     // vhdlchar
  m_t << "\\red112\\green0\\blue112;"; // vhdlkeyword
  m_t << "\\red255\\green0\\blue0;";   // vhdllogic

  m_t << "}\n";

  DBG_RTF(m_t << "{\\comment Beginning style list}\n")
  m_t << "{\\stylesheet\n";
  m_t << "{\\widctlpar\\adjustright \\fs20\\cgrid \\snext0 Normal;}\n";

  // set the paper dimensions according to PAPER_TYPE
  auto paperType = Config_getEnum(PAPER_TYPE);
  m_t << "{";
  switch (paperType)
  {
    // width & height values are inches * 1440
    case PAPER_TYPE_t::a4:        m_t << "\\paperw11900\\paperh16840"; break;
    case PAPER_TYPE_t::letter:    m_t << "\\paperw12240\\paperh15840"; break;
    case PAPER_TYPE_t::legal:     m_t << "\\paperw12240\\paperh20160"; break;
    case PAPER_TYPE_t::executive: m_t << "\\paperw10440\\paperh15120"; break;
  }
  m_t << "\\margl1800\\margr1800\\margt1440\\margb1440\\gutter0\\ltrsect}\n";

  // sort styles ascending by \s-number via an intermediate QArray
  unsigned maxIndex = 0;
  for (const auto &[name,data] : rtf_Style)
  {
    uint32_t index = data.index();
    if (index > maxIndex) maxIndex = index;
  }
  std::vector<const StyleData*> array(maxIndex + 1, nullptr);
  ASSERT(maxIndex < array.size());

  for (const auto &[name,data] : rtf_Style)
  {
    uint32_t index = data.index();
    if (array[index] != nullptr)
    {
      err("Style '%s' redefines \\s%d.\n", name.c_str(), index);
    }
    array[index] = &data;
  }

  // write array elements
  size_t size = array.size();
  for(size_t i = 0; i < size; i++)
  {
    const StyleData *pStyle = array[i];
    if (pStyle)
    {
      m_t << "{" << pStyle->reference() << pStyle->definition() << ";}\n";
    }
  }

  m_t << "}\n";

  // place to write rtf_Table_Default
  int id = -1;
  m_t << "{\\*\\listtable" << "\n";
  for (int i=0 ; rtf_Table_Default[i].definition ; i++ )
  {
    if (id != rtf_Table_Default[i].id)
    {
      if (id != -1)
      {
        m_t << "\\listid" << id << "}" << "\n";
      }
      id = rtf_Table_Default[i].id;
      m_t << "{\\list\\listtemplateid" << rtf_Table_Default[i].id << "\n";
    }
    m_t << "{ " << rtf_Table_Default[i].definition << " }" << "\n";
  }
  m_t << "\\listid" << id << "}" << "\n";
  m_t << "}" <<"\n";
  m_t << "{\\listoverridetable" <<"\n";
  id = -1;
  for (int i=0 ; rtf_Table_Default[i].definition ; i++ )
  {
    if (id != rtf_Table_Default[i].id)
    {
      id = rtf_Table_Default[i].id;
      m_t << "{\\listoverride\\listid" << id << "\\listoverridecount0\\ls" << id << "}" << "\n";
    }
  }
  m_t << "}" << "\n";

  // this comment is needed for postprocessing!
  m_t << "{\\comment begin body}\n";

}

void RTFGenerator::beginRTFChapter()
{
  m_t << "\n";
  DBG_RTF(m_t << "{\\comment BeginRTFChapter}\n")
  m_t << rtf_Style_Reset;

  // if we are compact, no extra page breaks...
  if (Config_getBool(COMPACT_RTF))
  {
    //      m_t << "\\sect\\sectd\\sbknone\n";
    m_t << "\\sect\\sbknone\n";
    rtfwriteRuler_thick();
  }
  else
    m_t << "\\sect\\sbkpage\n";
  //m_t << "\\sect\\sectd\\sbkpage\n";

  m_t << rtf_Style["Heading1"].reference() << "\n";
}

void RTFGenerator::beginRTFSection()
{
  m_t << "\n";
  DBG_RTF(m_t << "{\\comment BeginRTFSection}\n")
  m_t << rtf_Style_Reset;

  // if we are compact, no extra page breaks...
  if (Config_getBool(COMPACT_RTF))
  {
    m_t << "\\sect\\sbknone\n";
    rtfwriteRuler_emboss();
  }
  else
  {
    m_t << "\\sect\\sbkpage\n";
  }
  int level = 2 + m_hierarchyLevel;

  m_t << rtf_Style[QCString().sprintf("Heading%d", level).str()].reference() << "\n";
}

void RTFGenerator::startFile(const QCString &name,const QCString &,const QCString &,int,int hierarchyLevel)
{
  //setEncoding(QCString().sprintf("CP%s",theTranslator->trRTFansicp()));
  QCString fileName=name;
  m_relPath = relativePathToRoot(fileName);
  m_hierarchyLevel = hierarchyLevel;

  if (!fileName.endsWith(".rtf")) fileName+=".rtf";
  startPlainFile(fileName);
  setRelativePath(m_relPath);
  setSourceFileName(stripPath(fileName));
  beginRTFDocument();
}

void RTFGenerator::endFile()
{
  DBG_RTF(m_t << "{\\comment endFile}\n")
  m_t << "}";

  endPlainFile();
  setSourceFileName("");
}

void RTFGenerator::startProjectNumber()
{
  DBG_RTF(m_t << "{\\comment startProjectNumber }\n")
  m_t << " ";
}

void RTFGenerator::endProjectNumber()
{
  DBG_RTF(m_t << "{\\comment endProjectNumber }\n")
}

void RTFGenerator::startIndexSection(IndexSection is)
{
  //QCString paperName;

  //m_indentLevel = 0;

  switch (is)
  {
    case IndexSection::isTitlePageStart:
      // basic RTFstart
      // get readyfor author etc

      m_t << "{\\info \n";
      m_t << "{\\title {\\comment ";
      break;
    case IndexSection::isTitlePageAuthor:
      m_t << "}\n";
      if (!rtf_subject.isEmpty())      m_t << "{\\subject "  << rtf_subject      << "}\n";
      if (!rtf_comments.isEmpty())     m_t << "{\\comment "  << rtf_comments     << "}\n";
      if (!rtf_company.isEmpty())      m_t << "{\\company "  << rtf_company      << "}\n";
      if (!rtf_author.isEmpty())       m_t << "{\\author "   << rtf_author       << "}\n";
      if (!rtf_manager.isEmpty())      m_t << "{\\manager "  << rtf_manager      << "}\n";
      if (!rtf_documentType.isEmpty()) m_t << "{\\category " << rtf_documentType << "}\n";
      if (!rtf_keywords.isEmpty())     m_t << "{\\keywords " << rtf_keywords     << "}\n";
      m_t << "{\\comment ";
      break;
    case IndexSection::isMainPage:
      //Introduction
      beginRTFChapter();
      break;
    case IndexSection::isTopicIndex:
      //Topic Index
      beginRTFChapter();
      break;
    case IndexSection::isModuleIndex:
      //Module Index
      beginRTFChapter();
      break;
    case IndexSection::isDirIndex:
      //Directory Index
      beginRTFChapter();
      break;
    case IndexSection::isNamespaceIndex:
      //Namespace Index
      beginRTFChapter();
      break;
    case IndexSection::isConceptIndex:
      //Concept Index
      beginRTFChapter();
      break;
    case IndexSection::isClassHierarchyIndex:
      //Hierarchical Index
      DBG_RTF(m_t << "{\\comment start classhierarchy}\n")
      beginRTFChapter();
      break;
    case IndexSection::isCompoundIndex:
      //Annotated Compound Index
      beginRTFChapter();
      break;
    case IndexSection::isFileIndex:
      //Annotated File Index
      beginRTFChapter();
      break;
    case IndexSection::isPageIndex:
      //Related Page Index
      beginRTFChapter();
      break;
    case IndexSection::isTopicDocumentation:
      {
        //Topic Documentation
        for (const auto &gd : *Doxygen::groupLinkedMap)
        {
          if (!gd->isReference())
          {
            beginRTFChapter();
            break;
          }
        }
      }
      break;
    case IndexSection::isModuleDocumentation:
      {
        //Module Documentation
        for (const auto &mod : ModuleManager::instance().modules())
        {
          if (!mod->isReference() && mod->isPrimaryInterface())
          {
            beginRTFChapter();
            break;
          }
        }
      }
      break;
    case IndexSection::isDirDocumentation:
      {
        //Directory Documentation
        for (const auto &dd : *Doxygen::dirLinkedMap)
        {
          if (dd->isLinkableInProject())
          {
            beginRTFChapter();
            break;
          }
        }
      }
      break;
    case IndexSection::isNamespaceDocumentation:
      {
        // Namespace Documentation
        for (const auto &nd : *Doxygen::namespaceLinkedMap)
        {
          if (nd->isLinkableInProject())
          {
            beginRTFChapter();
            break;
          }
        }
      }
      break;
    case IndexSection::isConceptDocumentation:
      {
        // Concept Documentation
        for (const auto &cd : *Doxygen::conceptLinkedMap)
        {
          if (cd->isLinkableInProject())
          {
            beginRTFChapter();
            break;
          }
        }
      }
      break;
    case IndexSection::isClassDocumentation:
      {
        //Compound Documentation
        for (const auto &cd : *Doxygen::classLinkedMap)
        {
          if (cd->isLinkableInProject() &&
              cd->templateMaster()==nullptr &&
             !cd->isEmbeddedInOuterScope() &&
             !cd->isAlias()
             )
          {
            beginRTFChapter();
            break;
          }
        }
      }
      break;
    case IndexSection::isFileDocumentation:
      {
        //File Documentation
        bool isFirst=TRUE;
        for (const auto &fn : *Doxygen::inputNameLinkedMap)
        {
          for (const auto &fd : *fn)
          {
            if (fd->isLinkableInProject() || fd->generateSourceFile())
            {
              if (isFirst)
              {
                beginRTFChapter();
                isFirst=FALSE;
                break;
              }
            }
          }
          if (!isFirst)
          {
            break;
          }
        }
      }
      break;
    case IndexSection::isExampleDocumentation:
      {
        //Example Documentation
        beginRTFChapter();
      }
      break;
    case IndexSection::isPageDocumentation:
      break;
    case IndexSection::isPageDocumentation2:
      break;
    case IndexSection::isEndIndex:
      break;
  }
}

void RTFGenerator::endIndexSection(IndexSection is)
{
  bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  bool vhdlOpt    = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  QCString projectName = Config_getString(PROJECT_NAME);

  switch (is)
  {
    case IndexSection::isTitlePageStart:
      if (!rtf_title.isEmpty())
        // User has overridden document title in extensions file
        m_t << "}" << rtf_title;
      else
        m_t << "}" << projectName;
      break;
    case IndexSection::isTitlePageAuthor:
      {
        m_t << " doxygen " << getDoxygenVersion() << ".}\n";
        m_t << dateToRTFDateString();
        DBG_RTF(m_t << "{\\comment end of infoblock}\n");
        // setup for this section
        m_t << "}";
        m_t << rtf_Style_Reset <<"\n";
        m_t << "\\sectd\\pgnlcrm\n";
        m_t << "{\\footer "<<rtf_Style["Footer"].reference() << "{\\chpgn}}\n";
        // the title entry
        DBG_RTF(m_t << "{\\comment begin title page}\n")


        m_t << rtf_Style_Reset << rtf_Style["SubTitle"].reference() << "\n"; // set to title style

        m_t << "\\vertalc\\qc\\par\\par\\par\\par\\par\\par\\par\n";
        if (!rtf_logoFilename.isEmpty())
        {
          m_t << "{\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE \"" << rtf_logoFilename;
          m_t << "\" \\\\d \\\\*MERGEFORMAT} {\\fldrslt IMAGE }}\\par\\par\n";
        }
        if (!rtf_company.isEmpty())
        {
          m_t << rtf_company << "\\par\\par\n";
        }

        m_t << rtf_Style_Reset << rtf_Style["Title"].reference() << "\n"; // set to title style
        if (!rtf_title.isEmpty())
        {
          // User has overridden document title in extensions file
          m_t << "{\\field\\fldedit {\\*\\fldinst TITLE \\\\*MERGEFORMAT}{\\fldrslt " << rtf_title << "}}\\par\n";
        }
        else
        {
          auto parser { createDocParser() };
          auto ast    { validatingParseText(*parser.get(), projectName) };
          if (ast)
          {
            m_t << "{\\field\\fldedit {\\*\\fldinst TITLE \\\\*MERGEFORMAT}{\\fldrslt ";
            writeDoc(ast.get(),nullptr,nullptr,0);
            m_t << "}}\\par\n";
          }
        }

        m_t << rtf_Style_Reset << rtf_Style["SubTitle"].reference() << "\n"; // set to title style
        m_t << "\\par\n";
        if (!rtf_documentType.isEmpty())
        {
          m_t << rtf_documentType << "\\par\n";
        }
        if (!rtf_documentId.isEmpty())
        {
          m_t << rtf_documentId << "\\par\n";
        }
        m_t << "\\par\\par\\par\\par\\par\\par\\par\\par\\par\\par\\par\\par\n";

        m_t << rtf_Style_Reset << rtf_Style["SubTitle"].reference() << "\n"; // set to subtitle style
        if (!rtf_author.isEmpty())
        {
          m_t << "{\\field\\fldedit {\\*\\fldinst AUTHOR \\\\*MERGEFORMAT}{\\fldrslt "<< rtf_author << " }}\\par\n";
        }
        else
        {
          m_t << "{\\field\\fldedit {\\*\\fldinst AUTHOR \\\\*MERGEFORMAT}{\\fldrslt AUTHOR}}\\par\n";
        }

        m_t << theTranslator->trVersion() << " " << Config_getString(PROJECT_NUMBER) << "\\par";
        switch (Config_getEnum(TIMESTAMP))
        {
          case TIMESTAMP_t::YES:
          case TIMESTAMP_t::DATETIME:
            m_t << "{\\field\\fldedit {\\*\\fldinst CREATEDATE \\\\*MERGEFORMAT}"
              "{\\fldrslt "<< dateToString(DateTimeType::DateTime) << " }}\\par\n";
            break;
          case TIMESTAMP_t::DATE:
            m_t << "{\\field\\fldedit {\\*\\fldinst CREATEDATE \\\\*MERGEFORMAT}"
              "{\\fldrslt "<< dateToString(DateTimeType::Date) << " }}\\par\n";
            break;
          case TIMESTAMP_t::NO:
            break;
        }
        m_t << "\\page\\page";
        DBG_RTF(m_t << "{\\comment End title page}\n")

        // table of contents section
        DBG_RTF(m_t << "{\\comment Table of contents}\n")
        m_t << "\\vertalt\n";
        m_t << rtf_Style_Reset << "\n";
        m_t << rtf_Style["Heading1"].reference();
        m_t << theTranslator->trRTFTableOfContents() << "\\par\n";
        m_t << rtf_Style_Reset << "\\par\n";
        m_t << "{\\field\\fldedit {\\*\\fldinst TOC \\\\f \\\\*MERGEFORMAT}{\\fldrslt Table of contents}}\\par\n";
        m_t << rtf_Style_Reset << "\n";
      }
      break;
    case IndexSection::isMainPage:
      if (Doxygen::mainPage)
      {
        writePageLink(Doxygen::mainPage->getOutputFileBase(), TRUE);
      }
      break;
    case IndexSection::isTopicIndex:
      m_t << "\\par " << rtf_Style_Reset << "\n";
      m_t << "{\\tc \\v " << theTranslator->trTopicIndex() << "}\n";
      m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"topics.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case IndexSection::isModuleIndex:
      m_t << "\\par " << rtf_Style_Reset << "\n";
      m_t << "{\\tc \\v " << theTranslator->trModuleIndex() << "}\n";
      m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"modules.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case IndexSection::isDirIndex:
      m_t << "\\par " << rtf_Style_Reset << "\n";
      m_t << "{\\tc \\v " << theTranslator->trDirIndex() << "}\n";
      m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"dirs.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case IndexSection::isNamespaceIndex:
      m_t << "\\par " << rtf_Style_Reset << "\n";
      if (fortranOpt)
      {
          m_t << "{\\tc \\v " << theTranslator->trModulesIndex() << "}\n";
      }
      else
      {
          m_t << "{\\tc \\v " << theTranslator->trNamespaceIndex() << "}\n";
      }

      m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"namespaces.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case IndexSection::isConceptIndex:
      m_t << "\\par " << rtf_Style_Reset << "\n";
      m_t << "{\\tc \\v " << theTranslator->trConceptIndex() << "}\n";
      m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"concepts.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case IndexSection::isClassHierarchyIndex:
      m_t << "\\par " << rtf_Style_Reset << "\n";
      m_t << "{\\tc \\v " << theTranslator->trHierarchicalIndex() << "}\n";
      m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"hierarchy.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case IndexSection::isCompoundIndex:
      m_t << "\\par " << rtf_Style_Reset << "\n";
      if (fortranOpt)
      {
        m_t << "{\\tc \\v " << theTranslator->trCompoundIndexFortran() << "}\n";
      }
      else if (vhdlOpt)
      {
        m_t << "{\\tc \\v " << theTranslator->trDesignUnitIndex() << "}\n";
      }
      else
      {
        m_t << "{\\tc \\v " << theTranslator->trCompoundIndex() << "}\n";
      }
      m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"annotated.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case IndexSection::isFileIndex:
      m_t << "\\par " << rtf_Style_Reset << "\n";
      m_t << "{\\tc \\v " << theTranslator->trFileIndex() << "}\n";
      m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"files.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case IndexSection::isPageIndex:
      m_t << "\\par " << rtf_Style_Reset << "\n";
      m_t << "{\\tc \\v " << theTranslator->trPageIndex() << "}\n";
      m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"pages.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case IndexSection::isTopicDocumentation:
      {
        m_t << "{\\tc \\v " << theTranslator->trTopicDocumentation() << "}\n";
        for (const auto &gd : *Doxygen::groupLinkedMap)
        {
          if (!gd->isReference() && !gd->isASubGroup())
          {
            writePageLink(gd->getOutputFileBase(), FALSE);
          }
        }
      }
      break;
    case IndexSection::isModuleDocumentation:
      {
        m_t << "{\\tc \\v " << theTranslator->trModuleDocumentation() << "}\n";
        for (const auto &mod : ModuleManager::instance().modules())
        {
          if (!mod->isReference() && mod->isPrimaryInterface())
          {
            writePageLink(mod->getOutputFileBase(), FALSE);
          }
        }
      }
      break;
    case IndexSection::isDirDocumentation:
      {
        bool first=true;
        m_t << "{\\tc \\v " << theTranslator->trDirDocumentation() << "}\n";
        for (const auto &dd : *Doxygen::dirLinkedMap)
        {
          if (dd->isLinkableInProject())
          {
            m_t << "\\par " << rtf_Style_Reset << "\n";
            if (!first)
            {
              beginRTFSection();
            }
            first=false;
            m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            m_t << dd->getOutputFileBase();
            m_t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          }
        }
      }
      break;
    case IndexSection::isNamespaceDocumentation:
      {
        bool first=true;
        for (const auto &nd : *Doxygen::namespaceLinkedMap)
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            m_t << "\\par " << rtf_Style_Reset << "\n";
            if (!first)
            {
              beginRTFSection();
            }
            first=false;
            m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            m_t << nd->getOutputFileBase();
            m_t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          }
        }
      }
      break;
    case IndexSection::isConceptDocumentation:
      {
        bool first=true;
        for (const auto &cd : *Doxygen::conceptLinkedMap)
        {
          if (cd->isLinkableInProject() && !cd->isAlias())
          {
            m_t << "\\par " << rtf_Style_Reset << "\n";
            if (!first)
            {
              beginRTFSection();
            }
            first=false;
            m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            m_t << cd->getOutputFileBase();
            m_t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          }
        }
      }
      break;
    case IndexSection::isClassDocumentation:
      {
        bool first=true;
        if (fortranOpt)
        {
          m_t << "{\\tc \\v " << theTranslator->trTypeDocumentation() << "}\n";
        }
        else
        {
          m_t << "{\\tc \\v " << theTranslator->trClassDocumentation() << "}\n";
        }
        for (const auto &cd : *Doxygen::classLinkedMap)
        {
          if (cd->isLinkableInProject() &&
              cd->templateMaster()==nullptr &&
             !cd->isEmbeddedInOuterScope() &&
             !cd->isAlias()
             )
          {
            m_t << "\\par " << rtf_Style_Reset << "\n";
            if (!first)
            {
              beginRTFSection();
            }
            first=false;
            m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            m_t << cd->getOutputFileBase();
            m_t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          }
        }
      }
      break;
    case IndexSection::isFileDocumentation:
      {
        bool isFirst=TRUE;

        m_t << "{\\tc \\v " << theTranslator->trFileDocumentation() << "}\n";
        for (const auto &fn : *Doxygen::inputNameLinkedMap)
        {
          for (const auto &fd : *fn)
          {
            if (fd->isLinkableInProject())
            {
              m_t << "\\par " << rtf_Style_Reset << "\n";
              if (!isFirst)
              {
                beginRTFSection();
              }
              isFirst=FALSE;
              m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
              m_t << fd->getOutputFileBase();
              m_t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
            }
            if (fd->generateSourceFile())
            {
              m_t << "\\par " << rtf_Style_Reset << "\n";
              if (!isFirst)
              {
                beginRTFSection();
              }
              isFirst=FALSE;
              m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
              m_t << fd->getSourceFileBase();
              m_t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
            }
          }
        }
      }
      break;
    case IndexSection::isExampleDocumentation:
      {
        //m_t << "}\n";
        bool isFirst=true;
        m_t << "{\\tc \\v " << theTranslator->trExamples() << "}\n";
        for (const auto &pd : *Doxygen::exampleLinkedMap)
        {
          m_t << "\\par " << rtf_Style_Reset << "\n";
          if (!isFirst)
          {
            beginRTFSection();
          }
          isFirst=false;
          m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
          m_t << pd->getOutputFileBase();
          m_t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
        }
      }
      break;
    case IndexSection::isPageDocumentation:
      {
        m_t << "\\par " << rtf_Style_Reset << "\n";
        for (const auto &pd : *Doxygen::pageLinkedMap)
        {
          if (!pd->getGroupDef() && !pd->isReference() && !pd->hasParentPage()
            && Doxygen::mainPage.get() != pd.get())
          {
            writePageLink(pd->getOutputFileBase(), FALSE);
          }
        }
      }
      break;
    case IndexSection::isPageDocumentation2:
      break;
    case IndexSection::isEndIndex:
      beginRTFChapter();
      m_t << rtf_Style["Heading1"].reference();
      m_t << theTranslator->trRTFGeneralIndex() << "\\par \n";
      m_t << rtf_Style_Reset << "\n";
      m_t << "{\\tc \\v " << theTranslator->trRTFGeneralIndex() << "}\n";
      m_t << "{\\field\\fldedit {\\*\\fldinst INDEX \\\\c2 \\\\*MERGEFORMAT}{\\fldrslt INDEX}}\n";

      break;
   }
}

void RTFGenerator::writePageLink(const QCString &name,bool)
{
   m_t << "\\par " << rtf_Style_Reset << "\n";
   m_t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
   m_t << name;
   m_t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
}

void RTFGenerator::lastIndexPage()
{
  DBG_RTF(m_t << "{\\comment Beginning Body of RTF Document}\n")
  // end page and setup for rest of document
  m_t << "\\sect \\sbkpage \\pgndec \\pgnrestart\n";
  m_t << "\\sect \\sectd \\sbknone\n";

  // set new footer with arabic numbers
  m_t << "{\\footer "<< rtf_Style["Footer"].reference() << "{\\chpgn}}\n";

}

void RTFGenerator::writeStyleInfo(int)
{
}

void RTFGenerator::lineBreak(const QCString &)
{
  DBG_RTF(m_t << "{\\comment (lineBreak)}"    << "\n")
  m_t << "\\par\n";
  m_omitParagraph = TRUE;
}

void RTFGenerator::writeString(const QCString &text)
{
  m_t << text;
}

void RTFGenerator::startIndexList()
{
  DBG_RTF(m_t << "{\\comment (startIndexList)}\n")
  m_t << "{\n";
  m_t << "\\par\n";
  incIndentLevel();
  m_t << rtf_Style_Reset << rtf_LCList_DepthStyle() << "\n";
  m_omitParagraph = TRUE;
}

void RTFGenerator::endIndexList()
{
  DBG_RTF(m_t << "{\\comment (endIndexList)}\n")
  if (!m_omitParagraph)
  {
    m_t << "\\par";
    m_omitParagraph = TRUE;
  }
  m_t << "}";
  decIndentLevel();
}

/*! start bullet list */
void RTFGenerator::startItemList()
{
  newParagraph();
  incIndentLevel();
  int level = indentLevel();
  DBG_RTF(m_t << "{\\comment (startItemList level=" << level << ") }\n")
  m_t << "{";
  m_listItemInfo[level].number = 1;
  m_listItemInfo[level].isEnum = false;
  m_listItemInfo[level].type   = '1';
}

/*! end bullet list */
void RTFGenerator::endItemList()
{
  newParagraph();
  DBG_RTF(m_t << "{\\comment (endItemList level=" << indentLevel() << ")}\n")
  m_t << "}";
  decIndentLevel();
  m_omitParagraph = TRUE;
}

/*! write bullet or enum item */
void RTFGenerator::startItemListItem()
{
  DBG_RTF(m_t << "{\\comment (startItemListItem)}\n")
  newParagraph();
  m_t << rtf_Style_Reset;
  int level = indentLevel();
  if (m_listItemInfo[level].isEnum)
  {
    m_t << rtf_EList_DepthStyle() << "\n";
    m_t << m_listItemInfo[level].number << ".\\tab ";
    m_listItemInfo[level].number++;
  }
  else
  {
    m_t << rtf_BList_DepthStyle() << "\n";
  }
  m_omitParagraph = TRUE;
}

void RTFGenerator::endItemListItem()
{
  DBG_RTF(m_t << "{\\comment (endItemListItem)}\n")
}

void RTFGenerator::startIndexItem(const QCString &,const QCString &)
{
  DBG_RTF(m_t << "{\\comment (startIndexItem)}\n")

  if (!m_omitParagraph)
  {
    m_t << "\\par\n";
    m_omitParagraph = TRUE;
  }
}

void RTFGenerator::endIndexItem(const QCString &ref,const QCString &fn)
{
  DBG_RTF(m_t << "{\\comment (endIndexItem)}\n")
  if (ref.isEmpty() && !fn.isEmpty())
  {
    m_t << "\\tab ";
    writeRTFReference(fn);
    m_t << "\n";
  }
  else
  {
    m_t << "\n";
  }
  m_omitParagraph = TRUE;
}

void RTFGenerator::writeStartAnnoItem(const QCString &,const QCString &f,
    const QCString &path,const QCString &name)
{
  DBG_RTF(m_t << "{\\comment (writeStartAnnoItem)}\n")
  m_t << "{\\b ";
  if (!path.isEmpty()) docify(path);
  if (!f.isEmpty() && Config_getBool(RTF_HYPERLINKS))
  {
    m_t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    m_t << rtfFormatBmkStr(stripPath(f));
    m_t << "\" }{}";
    m_t << "}{\\fldrslt {\\cs37\\ul\\cf2 ";

    docify(name);

    m_t << "}}}\n";
  }
  else
  {
    docify(name);
  }
  m_t << "} ";
}

void RTFGenerator::startIndexKey()
{
  DBG_RTF(m_t << "{\\comment (startIndexKey)}\n")
  m_t << "{\\b ";
}

void RTFGenerator::endIndexKey()
{
  DBG_RTF(m_t << "{\\comment (endIndexKey)}\n")
}

void RTFGenerator::startIndexValue(bool hasBrief)
{
  DBG_RTF(m_t << "{\\comment (startIndexValue)}\n")
  m_t << " ";
  if (hasBrief) m_t << "(";
}

void RTFGenerator::endIndexValue(const QCString &name,bool hasBrief)
{
  DBG_RTF(m_t << "{\\comment (endIndexValue)}\n")
  if (hasBrief) m_t << ")";
  m_t << "} ";
  if (!name.isEmpty())
  {
    m_t << "\\tab ";
    writeRTFReference(name);
    m_t << "\n";
  }
  else
  {
    m_t << "\n";
  }
  m_omitParagraph=FALSE;
  newParagraph();
}

void RTFGenerator::startCompoundTemplateParams()
{
  //beginRTFSubSubSection();
  m_t << "\n";
  DBG_RTF(m_t << "{\\comment Begin SubSubSection}\n")
  m_t << "{\n";
  int level = 4 + m_hierarchyLevel;
  m_t << rtf_Style_Reset << rtf_Style[QCString().sprintf("Heading%d", level).str()].reference() << "\n";
}

void RTFGenerator::endCompoundTemplateParams()
{
  newParagraph();
  m_t << "}\n";
}

void RTFGenerator::startTextLink(const QCString &f,const QCString &anchor)
{
  if (Config_getBool(RTF_HYPERLINKS))
  {
    QCString ref;
    if (!f.isEmpty())
    {
      ref+=stripPath(f);
    }
    if (!anchor.isEmpty())
    {
      ref+='_';
      ref+=anchor;
    }

    m_t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    m_t << rtfFormatBmkStr(ref);
    m_t << "\" }{}";
    m_t << "}{\\fldrslt {\\cs37\\ul\\cf2 ";
  }
}

void RTFGenerator::endTextLink()
{
  if (Config_getBool(RTF_HYPERLINKS))
  {
    m_t << "}}}\n";
  }
}

static QCString objectLinkToString(const QCString &ref,    const QCString &f,
                                   const QCString &anchor, const QCString &text)
{
  QCString result;
  if (ref.isEmpty() && Config_getBool(RTF_HYPERLINKS))
  {
    QCString refName;
    if (!f.isEmpty())
    {
      refName+=stripPath(f);
    }
    if (!anchor.isEmpty())
    {
      refName+='_';
      refName+=anchor;
    }

    result += "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    result += rtfFormatBmkStr(refName);
    result += "\" }{}";
    result += "}{\\fldrslt {\\cs37\\ul\\cf2 ";

    result += docifyToString(text);

    result += "}}}\n";
  }
  else
  {
    result += "{\\b ";
    result += docifyToString(text);
    result += "}";
  }
  return result;
}

void RTFGenerator::writeObjectLink(const QCString &ref,    const QCString &f,
                                   const QCString &anchor, const QCString &text)
{
  m_t << objectLinkToString(ref,f,anchor,text);
}

void RTFGenerator::startPageRef()
{
  m_t << " (";
  startEmphasis();
}

void RTFGenerator::endPageRef(const QCString &clname, const QCString &anchor)
{
  QCString ref;
  if (!clname.isEmpty())
  {
    ref+=clname;
  }
  if (!anchor.isEmpty())
  {
    ref+='_';
    ref+=anchor;
  }
  writeRTFReference(ref);
  endEmphasis();
  m_t << ")";
}

void RTFGenerator::startTitleHead(const QCString &)
{
  DBG_RTF(m_t << "{\\comment startTitleHead}\n")
  int level = 2 + m_hierarchyLevel;
  QCString heading;
  heading.sprintf("Heading%d", level);
  //    beginRTFSection();
  m_t << rtf_Style_Reset << rtf_Style[heading.str()].reference() << "\n";
}

void RTFGenerator::endTitleHead(const QCString &fileName,const QCString &name)
{
  DBG_RTF(m_t << "{\\comment endTitleHead}\n")
  m_t << "\\par " << rtf_Style_Reset << "\n";
  if (!name.isEmpty())
  {
    // make table of contents entry
    int level = 2 + m_hierarchyLevel;
    m_t << "{\\tc\\tcl" << level << " \\v ";
    docify(name);
    m_t << "}\n";

    // make an index entry
    addIndexItem(name,QCString());
  }
  if (!fileName.isEmpty())
  {
    writeAnchor(fileName,QCString());
  }
}

void RTFGenerator::startGroupHeader(int extraIndent)
{
  DBG_RTF(m_t << "{\\comment startGroupHeader}\n")
  m_t << rtf_Style_Reset;
  extraIndent += m_hierarchyLevel;
  if (extraIndent>=2)
  {
    m_t << rtf_Style["Heading5"].reference();
  }
  else if (extraIndent==1)
  {
    m_t << rtf_Style["Heading4"].reference();
  }
  else // extraIndent==0
  {
    m_t << rtf_Style["Heading3"].reference();
  }
  m_t << "\n";
}

void RTFGenerator::endGroupHeader(int)
{
  DBG_RTF(m_t << "{\\comment endGroupHeader}\n")
  m_t << "\\par\n";
  m_t << rtf_Style_Reset << "\n";
}

void RTFGenerator::startMemberDoc(const QCString &clname,
    const QCString &memname,
    const QCString &,
    const QCString &,
    int,
    int,
    bool showInline)
{
  DBG_RTF(m_t << "{\\comment startMemberDoc}\n")
  if (!memname.isEmpty() && memname[0]!='@')
  {
    addIndexItem(memname,clname);
    addIndexItem(clname,memname);
  }

  int level = 4 + m_hierarchyLevel;
  if (showInline)
    ++level;
  if (level > 5)
    level = 5;
  if (level < 1)
    level = 1;
  m_t << rtf_Style_Reset << rtf_Style[QCString().sprintf("Heading%d", level).str()].reference();
  //styleStack.push(rtf_Style_Heading4);
  m_t << "{\n";
  //printf("RTFGenerator::startMemberDoc() '%s'\n",rtf_Style["Heading4"].reference());
  startBold();
  m_t << "\n";
}

void RTFGenerator::endMemberDoc(bool)
{
  DBG_RTF(m_t << "{\\comment endMemberDoc}\n")
  //const QCString &style = styleStack.pop();
  //printf("RTFGenerator::endMemberDoc() '%s'\n",style);
  //ASSERT(style==rtf_Style["Heading4"].reference());
  endBold();
  m_t << "}\n";
  newParagraph();
}

void RTFGenerator::startDoxyAnchor(const QCString &,const QCString &,
                                   const QCString &,const QCString &,
                                   const QCString &
                                  )
{
  DBG_RTF(m_t << "{\\comment startDoxyAnchor}\n")
}

void RTFGenerator::endDoxyAnchor(const QCString &fName,const QCString &anchor)
{
  QCString ref;
  if (!fName.isEmpty())
  {
    ref+=stripPath(fName);
  }
  if (!anchor.isEmpty())
  {
    ref+='_';
    ref+=anchor;
  }

  DBG_RTF(m_t << "{\\comment endDoxyAnchor}\n")
  m_t << "{\\bkmkstart ";
  m_t << rtfFormatBmkStr(ref);
  m_t << "}\n";
  m_t << "{\\bkmkend ";
  m_t << rtfFormatBmkStr(ref);
  m_t << "}\n";
}

void RTFGenerator::addLabel(const QCString &,const QCString &)
{
  DBG_RTF(m_t << "{\\comment addLabel}\n")
}


void RTFGenerator::addIndexItem(const QCString &s1,const QCString &s2)
{
  if (!s1.isEmpty())
  {
    m_t << "{\\xe \\v ";
    docify(s1);
    if (!s2.isEmpty())
    {
      m_t << "\\:";
      docify(s2);
    }
    m_t << "}\n";
  }
}

void RTFGenerator::startIndent()
{
  incIndentLevel();
  DBG_RTF(m_t << "{\\comment (startIndent) }\n")
  m_t << "{\n";
  m_t << rtf_Style_Reset << rtf_CList_DepthStyle() << "\n";
}

void RTFGenerator::endIndent()
{
  m_t << "}\n";
  decIndentLevel();
}


void RTFGenerator::startMemberDescription(const QCString &,const QCString &,bool)
{
  DBG_RTF(m_t << "{\\comment (startMemberDescription)}\n")
  m_t << "{\n";
  incIndentLevel();
  m_t << rtf_Style_Reset << rtf_CList_DepthStyle();
  startEmphasis();
}

void RTFGenerator::endMemberDescription()
{
  DBG_RTF(m_t << "{\\comment (endMemberDescription)}\n")
  endEmphasis();
  decIndentLevel();
  m_t << "\\par";
  m_t << "}\n";
  m_omitParagraph = TRUE;
}

void RTFGenerator::startDescForItem()
{
  DBG_RTF(m_t << "{\\comment (startDescForItem) }\n")
}

void RTFGenerator::endDescForItem()
{
  DBG_RTF(m_t << "{\\comment (endDescForItem) }\n")
}

void RTFGenerator::startSection(const QCString &,const QCString &title,SectionType type)
{
  DBG_RTF(m_t << "{\\comment (startSection)}\n")
  m_t << "{";
  m_t << rtf_Style_Reset;
  int num=SectionType::MaxLevel;
  switch(type.level())
  {
    case SectionType::Page:             num=2+m_hierarchyLevel; break;
    case SectionType::Section:          num=3+m_hierarchyLevel; break;
    case SectionType::Subsection:       // fall through
    case SectionType::Subsubsection:    // fall through
    case SectionType::Paragraph:        // fall through
    case SectionType::Subparagraph:     // fall through
    case SectionType::Subsubparagraph:  num=4+m_hierarchyLevel; break;
    default: ASSERT(0); break;
  }
  num = std::clamp(num, SectionType::MinLevel, SectionType::MaxLevel);
  QCString heading;
  heading.sprintf("Heading%d",num);
  // set style
  m_t << rtf_Style[heading.str()].reference();
  // make table of contents entry
  m_t << "{\\tc\\tcl" << num << " \\v ";
  docify(title);
  m_t << "}\n";
}

void RTFGenerator::endSection(const QCString &lab,SectionType)
{
  DBG_RTF(m_t << "{\\comment (endSection)}\n")
  // make bookmark
  m_omitParagraph=FALSE;
  newParagraph();
  writeAnchor(QCString(),lab);
  m_t << "}";
}

void RTFGenerator::docify(const QCString &str)
{
  if (str.isEmpty()) return;
  m_t << docifyToString(str);
  m_omitParagraph = FALSE;
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
  DBG_RTF(m_t << "{\\comment startClassDiagram }\n")
}

void RTFGenerator::endClassDiagram(const ClassDiagram &d,
    const QCString &fileName,const QCString &)
{
  newParagraph();

  // create a png file
  d.writeImage(m_t,dir(),m_relPath,fileName,FALSE);

  // display the file
  m_t << "{\n";
  m_t << rtf_Style_Reset << "\n";
  m_t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE \"";
  m_t << fileName << ".png\"";
  m_t << " \\\\d \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par\n";
  m_t << "}\n";
}

void RTFGenerator::startMemberItem(const QCString &,MemberItemType,const QCString &)
{
  DBG_RTF(m_t << "{\\comment startMemberItem }\n")
  m_t << rtf_Style_Reset << rtf_BList_DepthStyle() << "\n"; // set style to appropriate depth
}

void RTFGenerator::endMemberItem(MemberItemType)
{
  DBG_RTF(m_t << "{\\comment endMemberItem }\n")
  newParagraph();
}

void RTFGenerator::writeAnchor(const QCString &fileName,const QCString &name)
{
  QCString anchor;
  if (!fileName.isEmpty())
  {
    anchor+=stripPath(fileName);
  }
  if (!fileName.isEmpty() && !name.isEmpty())
  {
    anchor+='_';
  }
  if (!name.isEmpty())
  {
    anchor+=name;
  }
  //printf("writeAnchor(%s->%s)\n",qPrint(anchor),qPrint(rtfFormatBmkStr(anchor)));

  DBG_RTF(m_t << "{\\comment writeAnchor (" << anchor << ")}\n")
  m_t << "{\\bkmkstart " << rtfFormatBmkStr(anchor) << "}\n";
  m_t << "{\\bkmkend " << rtfFormatBmkStr(anchor) << "}\n";
}

void RTFGenerator::writeRTFReference(const QCString &label)
{
  m_t << "{\\field\\fldedit {\\*\\fldinst PAGEREF ";
  m_t << rtfFormatBmkStr(stripPath(label));
  m_t << " \\\\*MERGEFORMAT}{\\fldrslt pagenum}}";
}

void RTFGenerator::writeNonBreakableSpace(int)
{
  m_t << "\\~ ";
}


void RTFGenerator::startMemberList()
{
  m_t << "\n";
  DBG_RTF(m_t << "{\\comment (startMemberList) }\n")
  m_t << "{\n";
#ifdef DELETEDCODE
  if (!insideTabbing)
    m_t << "\\begin{CompactItemize}\n";
#endif
}

void RTFGenerator::endMemberList()
{
  DBG_RTF(m_t << "{\\comment (endMemberList) }\n")
  m_t << "}\n";
#ifdef DELETEDCODE
  if (!insideTabbing)
    m_t << "\\end{CompactItemize}\n";
#endif
}

void RTFGenerator::startDescTable(const QCString &title,const bool hasInits)
{
  DBG_RTF(m_t << "{\\comment (startDescTable) }\n")
  m_t << "{\\par\n";
  m_t << "{" << rtf_Style["Heading5"].reference() << "\n";
  docify(title);
  m_t << ":\\par}\n";
  m_t << rtf_Style_Reset << rtf_DList_DepthStyle();
  m_t << "\\trowd \\trgaph108\\trleft426\\tblind426"
       "\\trbrdrt\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrl\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrb\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrr\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrh\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrv\\brdrs\\brdrw10\\brdrcf15 \n";
  int columnPos2[2] = { 25, 100 };
  int columnPos3[3] = { 25, 45, 100 };
  for (int i=0;i<(hasInits?3:2);i++)
  {
    m_t << "\\clvertalt\\clbrdrt\\brdrs\\brdrw10\\brdrcf15 "
         "\\clbrdrl\\brdrs\\brdrw10\\brdrcf15 "
         "\\clbrdrb\\brdrs\\brdrw10\\brdrcf15 "
         "\\clbrdrr \\brdrs\\brdrw10\\brdrcf15 "
         "\\cltxlrtb "
         "\\cellx" << (rtf_pageWidth*(hasInits?columnPos3[i]:columnPos2[i])/100) << "\n";
  }
  m_t << "\\pard \\widctlpar\\intbl\\adjustright\n";
}

void RTFGenerator::endDescTable()
{
  DBG_RTF(m_t << "{\\comment (endDescTable)}\n")
  m_t << "}\n";
}

void RTFGenerator::startDescTableRow()
{
}

void RTFGenerator::endDescTableRow()
{
}

void RTFGenerator::startDescTableTitle()
{
  DBG_RTF(m_t << "{\\comment (startDescTableTitle) }\n")
  m_t << "{";
  m_t << rtf_Style["BodyText"].reference();
}

void RTFGenerator::endDescTableTitle()
{
  DBG_RTF(m_t << "{\\comment (endDescTableTitle) }\n")
  m_t << "\\cell }";
}

void RTFGenerator::startDescTableInit()
{
  DBG_RTF(m_t << "{\\comment (startDescTableInit) }"    << endl)
  m_t << "{";
  m_t << rtf_Style["BodyText"].reference();
  m_t << "\\qr ";
}

void RTFGenerator::endDescTableInit()
{
  DBG_RTF(m_t << "{\\comment (endDescTableInit) }"    << endl)
  m_t << "\\cell }";
}

void RTFGenerator::startDescTableData()
{
  DBG_RTF(m_t << "{\\comment (startDescTableData) }\n")
  m_t << "{";
}

void RTFGenerator::endDescTableData()
{
  DBG_RTF(m_t << "{\\comment (endDescTableData) }\n")
  m_t << "\\cell }{\\row }\n";
}

// a style for list formatted as a "bulleted list"

int RTFGenerator::indentLevel() const
{
  return std::min(m_indentLevel,maxIndentLevels-1);
}

void RTFGenerator::incIndentLevel()
{
  m_indentLevel++;
  if (m_indentLevel>=maxIndentLevels)
  {
    m_indentLevel = maxIndentLevels-1;
    err("Maximum indent level (%d) exceeded while generating RTF output!\n",maxIndentLevels);
  }
  m_codeGen->setIndentLevel(m_indentLevel);
}

void RTFGenerator::decIndentLevel()
{
  m_indentLevel--;
  if (m_indentLevel<0)
  {
    err("Negative indent level while generating RTF output!\n");
    m_indentLevel=0;
  }
  m_codeGen->setIndentLevel(m_indentLevel);
}

// a style for list formatted with "list continue" style
QCString RTFGenerator::rtf_CList_DepthStyle()
{
  QCString n=makeIndexName("ListContinue",indentLevel());
  return rtf_Style[n.str()].reference();
}

// a style for list formatted as a "latext style" table of contents
QCString RTFGenerator::rtf_LCList_DepthStyle()
{
  QCString n=makeIndexName("LatexTOC",indentLevel());
  return rtf_Style[n.str()].reference();
}

// a style for list formatted as a "bullet" style
QCString RTFGenerator::rtf_BList_DepthStyle()
{
  QCString n=makeIndexName("ListBullet",indentLevel());
  return rtf_Style[n.str()].reference();
}

// a style for list formatted as a "enumeration" style
QCString RTFGenerator::rtf_EList_DepthStyle()
{
  QCString n=makeIndexName("ListEnum",indentLevel());
  return rtf_Style[n.str()].reference();
}

QCString RTFGenerator::rtf_DList_DepthStyle()
{
  QCString n=makeIndexName("DescContinue",indentLevel());
  return rtf_Style[n.str()].reference();
}

void RTFGenerator::startTextBlock(bool dense)
{
  DBG_RTF(m_t << "{\\comment startTextBlock}\n")
  m_t << "{\n";
  m_t << rtf_Style_Reset;
  if (dense) // no spacing between "paragraphs"
  {
    m_t << rtf_Style["DenseText"].reference();
  }
  else // some spacing
  {
    m_t << rtf_Style["BodyText"].reference();
  }
}

void RTFGenerator::endTextBlock(bool /*paraBreak*/)
{
  newParagraph();
  DBG_RTF(m_t << "{\\comment endTextBlock}\n")
  m_t << "}\n";
  //m_omitParagraph = TRUE;
}

void RTFGenerator::newParagraph()
{
  if (!m_omitParagraph)
  {
    DBG_RTF(m_t << "{\\comment (newParagraph)}\n")
    m_t << "\\par\n";
  }
  m_omitParagraph = FALSE;
}

void RTFGenerator::startParagraph(const QCString &txt)
{
  DBG_RTF(m_t << "{\\comment startParagraph}\n")
  newParagraph();
  m_t << "{\n";
  if (QCString(txt) == "reference") m_t << "\\ql\n";
}

void RTFGenerator::endParagraph()
{
  DBG_RTF(m_t << "{\\comment endParagraph}\n")
  m_t << "}\\par\n";
  m_omitParagraph = TRUE;
}

void RTFGenerator::startMemberSubtitle()
{
  DBG_RTF(m_t << "{\\comment startMemberSubtitle}\n")
  m_t << "{\n";
  m_t << rtf_Style_Reset << rtf_CList_DepthStyle() << "\n";
}

void RTFGenerator::endMemberSubtitle()
{
  DBG_RTF(m_t << "{\\comment endMemberSubtitle}\n")
  newParagraph();
  m_t << "}\n";
}

bool isLeadBytes(int c)
{
  bool result=false;           // for SBCS Codepages (cp1252,1251 etc...);

  QCString codePage = theTranslator->trRTFansicp();

  if (codePage == "932")       // cp932 (Japanese Shift-JIS)
  {
    result = (0x81<=c && c<=0x9f) || (0xe0<=c && c<=0xfc);
  }
  else if (codePage == "936")  // cp936 (Simplified Chinese GBK)
  {
    result = 0x81<=c && c<=0xFE;
  }
  else if (codePage == "949")  // cp949 (Korean)
  {
    result = 0x81<=c && c<=0xFE;
  }
  else if (codePage == "950")  // cp950 (Traditional Chinese Big5)
  {
    result = 0x81<=c && c<=0xFE;
  }

  return result;
}


// note: function is not reentrant!
static void encodeForOutput(TextStream &t,const QCString &s)
{
  if (s==nullptr) return;
  QCString encoding;
  bool converted=FALSE;
  size_t l = s.length();
  static std::vector<char> enc;
  if (l*4>enc.size()) enc.resize(l*4); // worst case
  encoding.sprintf("CP%s",qPrint(theTranslator->trRTFansicp()));
  if (!encoding.isEmpty())
  {
    // convert from UTF-8 back to the output encoding
    void *cd = portable_iconv_open(encoding.data(),"UTF-8");
    if (cd!=reinterpret_cast<void *>(-1))
    {
      size_t iLeft=l;
      size_t oLeft=enc.size();
      const char *inputPtr = s.data();
      char *outputPtr = &enc[0];
      if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
      {
        enc.resize(enc.size()-oLeft);
        converted=TRUE;
      }
      portable_iconv_close(cd);
    }
  }
  if (!converted) // if we did not convert anything, copy as is.
  {
    memcpy(enc.data(),s.data(),l);
    enc.resize(l);
  }
  bool multiByte = FALSE;

  for (size_t i=0;i<enc.size();i++)
  {
    uint8_t c = static_cast<uint8_t>(enc.at(i));

    if (c>=0x80 || multiByte)
    {
      char esc[10];
      qsnprintf(esc,10,"\\'%X",c);        // escape sequence for SBCS and DBCS(1st&2nd bytes).
      t << esc;

      if (!multiByte)
      {
        multiByte = isLeadBytes(c);  // It may be DBCS Codepages.
      }
      else
      {
        multiByte = FALSE;           // end of Double Bytes Character.
      }
    }
    else
    {
      t << c;
    }
  }
}

/**
 * VERY brittle routine inline RTF's included by other RTF's.
 * it is recursive and ugly.
 */
static bool preProcessFile(Dir &d,const QCString &infName, TextStream &t, bool bIncludeHeader=true, bool removeFile = true)
{
  static bool rtfDebug = Debug::isFlagSet(Debug::Rtf);
  std::ifstream f = Portable::openInputStream(infName);
  if (!f.is_open())
  {
    err("problems opening rtf file '%s' for reading\n",infName.data());
    return false;
  }

  const int maxLineLength = 10240;
  static QCString lineBuf(maxLineLength, QCString::ExplicitSize);

  // scan until find end of header
  // this is EXTREEEEEEEMLY brittle.  It works on OUR rtf
  // files because the first line before the body
  // ALWAYS contains "{\comment begin body}"
  std::string line;
  while (getline(f,line))
  {
    line+='\n';
    if (line.find("\\comment begin body")!=std::string::npos) break;
    if (bIncludeHeader) encodeForOutput(t,line.c_str());
  }

  std::string prevLine;
  bool first=true;
  while (getline(f,line))
  {
    line+='\n';
    size_t pos=prevLine.find("INCLUDETEXT \"");
    if (pos!=std::string::npos)
    {
      size_t startNamePos  = prevLine.find('"',pos)+1;
      size_t endNamePos    = prevLine.find('"',startNamePos);
      std::string fileName = prevLine.substr(startNamePos,endNamePos-startNamePos);
      DBG_RTF(t << "{\\comment begin include " << fileName << "}\n")
      if (!preProcessFile(d,fileName.c_str(),t,FALSE)) return FALSE;
      DBG_RTF(t << "{\\comment end include " << fileName << "}\n")
    }
    else if (!first) // no INCLUDETEXT on this line
    {
      encodeForOutput(t,prevLine.c_str());
    }
    prevLine = line;
    first=false;
  }
  if (!bIncludeHeader) // skip final '}' in case we don't include headers
  {
    size_t pos = line.rfind('}');
    if (pos==std::string::npos)
    {
      err("Strange, the last char was not a '}'\n");
      pos = line.length();
    }
    encodeForOutput(t,line.substr(0,pos).c_str());
  }
  else
  {
    encodeForOutput(t,line.c_str());
  }
  f.close();
  // remove temporary file
  if (!rtfDebug && removeFile) removeSet.insert(FileInfo(d.filePath(infName.str())).absFilePath());
  return TRUE;
}

void RTFGenerator::startDotGraph()
{
  DBG_RTF(m_t << "{\\comment (startDotGraph)}\n")
}

void RTFGenerator::endDotGraph(DotClassGraph &g)
{
  newParagraph();

  QCString fn = g.writeGraph(m_t,GraphOutputFormat::BITMAP,EmbeddedOutputFormat::Rtf,dir(),fileName(),m_relPath,TRUE,FALSE);

  // display the file
  m_t << "{\n";
  m_t << rtf_Style_Reset << "\n";
  m_t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE \"";
  QCString imgExt = getDotImageExtension();
  m_t << fn << "." << imgExt;
  m_t << "\" \\\\d \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par\n";
  m_t << "}\n";
  newParagraph();
  DBG_RTF(m_t << "{\\comment (endDotGraph)}\n")
}

void RTFGenerator::startInclDepGraph()
{
  DBG_RTF(m_t << "{\\comment (startInclDepGraph)}\n")
}

void RTFGenerator::endInclDepGraph(DotInclDepGraph &g)
{
  newParagraph();

  QCString fn = g.writeGraph(m_t,GraphOutputFormat::BITMAP,EmbeddedOutputFormat::Rtf,dir(),fileName(),m_relPath,FALSE);

  // display the file
  m_t << "{\n";
  m_t << rtf_Style_Reset << "\n";
  m_t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE \"";
  QCString imgExt = getDotImageExtension();
  m_t << fn << "." << imgExt;
  m_t << "\" \\\\d \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par\n";
  m_t << "}\n";
  DBG_RTF(m_t << "{\\comment (endInclDepGraph)}\n")
}

void RTFGenerator::startGroupCollaboration()
{
}

void RTFGenerator::endGroupCollaboration(DotGroupCollaboration &)
{
}

void RTFGenerator::startCallGraph()
{
  DBG_RTF(m_t << "{\\comment (startCallGraph)}\n")
}

void RTFGenerator::endCallGraph(DotCallGraph &g)
{
  newParagraph();

  QCString fn = g.writeGraph(m_t,GraphOutputFormat::BITMAP,EmbeddedOutputFormat::Rtf,dir(),fileName(),m_relPath,FALSE);

  // display the file
  m_t << "{\n";
  m_t << rtf_Style_Reset << "\n";
  m_t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE \"";
  QCString imgExt = getDotImageExtension();
  m_t << fn << "." << imgExt;
  m_t << "\" \\\\d \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par\n";
  m_t << "}\n";
  DBG_RTF(m_t << "{\\comment (endCallGraph)}\n")
}

void RTFGenerator::startDirDepGraph()
{
  DBG_RTF(m_t << "{\\comment (startDirDepGraph)}\n")
}

void RTFGenerator::endDirDepGraph(DotDirDeps &g)
{
  newParagraph();

  QCString fn = g.writeGraph(m_t,GraphOutputFormat::BITMAP,EmbeddedOutputFormat::Rtf,dir(),fileName(),m_relPath,FALSE);

  // display the file
  m_t << "{\n";
  m_t << rtf_Style_Reset << "\n";
  m_t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE \"";
  QCString imgExt = getDotImageExtension();
  m_t << fn << "." << imgExt;
  m_t << "\" \\\\d \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par\n";
  m_t << "}\n";
  DBG_RTF(m_t << "{\\comment (endDirDepGraph)}\n")
}

/** Tests the integrity of the result by counting brackets.
 *
 */
void testRTFOutput(const QCString &name)
{
  int bcount=0;
  int line=1;
  int c=0;
  std::ifstream f = Portable::openInputStream(name);
  if (f.is_open())
  {
    while ((c=f.get())!=-1)
    {
      if (c=='\\') // escape char
      {
        c=f.get();
        if (c==-1) break;
      }
      else if (c=='{') // open bracket
      {
        bcount++;
      }
      else if (c=='}') // close bracket
      {
        bcount--;
        if (bcount<0)
        {
          goto err;
          break;
        }
      }
      else if (c=='\n') // newline
      {
        line++;
      }
    }
  }
  if (bcount==0) return; // file is OK.
err:
  err("RTF integrity test failed at line %d of %s due to a bracket mismatch.\n"
      "       Please try to create a small code example that produces this error \n"
      "       and send that to doxygen@gmail.com.\n",line,qPrint(name));
}

/**
 * This is an API to a VERY brittle RTF preprocessor that combines nested
 * RTF files.  This version replaces the infile with the new file
 */
bool RTFGenerator::preProcessFileInplace(const QCString &path,const QCString &name)
{
  static bool rtfDebug = Debug::isFlagSet(Debug::Rtf);

  Dir d(path.str());
  // store the original directory
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",qPrint(path));
    return FALSE;
  }
  std::string oldDir = Dir::currentDirPath();

  // go to the html output directory (i.e. path)
  Dir::setCurrent(d.absPath());
  Dir thisDir;

  QCString combinedName = path+"/combined.rtf";
  QCString mainRTFName  = path+"/"+name;

  std::ofstream f = Portable::openOutputStream(combinedName);
  if (!f.is_open())
  {
    err("Failed to open %s for writing!\n",combinedName.data());
    Dir::setCurrent(oldDir);
    return FALSE;
  }
  TextStream outt(&f);

  if (!preProcessFile(thisDir,mainRTFName,outt,true,false))
  {
    // it failed, remove the temp file
    outt.flush();
    f.close();
    if (!rtfDebug) removeSet.insert(FileInfo(thisDir.filePath(combinedName.str())).absFilePath());
    Dir::setCurrent(oldDir);
    return FALSE;
  }

  // everything worked, move the files
  outt.flush();
  f.close();
  if (!rtfDebug)
  {
    thisDir.remove(mainRTFName.str());
  }
  else
  {
    thisDir.rename(mainRTFName.str(),mainRTFName.str() + ".org");
  }
  thisDir.rename(combinedName.str(),mainRTFName.str());

  testRTFOutput(mainRTFName);

  QCString rtfOutputDir = Dir::currentDirPath();
  for (auto &s : removeSet)
  {
    QCString s1(s.c_str());
    if (s1.startsWith(rtfOutputDir)) Portable::unlink(s1);
  }

  Dir::setCurrent(oldDir);
  return TRUE;
}

void RTFGenerator::startMemberGroupHeader(bool hasHeader)
{
  DBG_RTF(m_t << "{\\comment startMemberGroupHeader}\n")
  m_t << "{\n";
  if (hasHeader) incIndentLevel();
  m_t << rtf_Style_Reset << rtf_Style["GroupHeader"].reference();
}

void RTFGenerator::endMemberGroupHeader()
{
  DBG_RTF(m_t << "{\\comment endMemberGroupHeader}\n")
  newParagraph();
  m_t << rtf_Style_Reset << rtf_CList_DepthStyle();
}

void RTFGenerator::startMemberGroupDocs()
{
  DBG_RTF(m_t << "{\\comment startMemberGroupDocs}\n")
  startEmphasis();
}

void RTFGenerator::endMemberGroupDocs()
{
  DBG_RTF(m_t << "{\\comment endMemberGroupDocs}\n")
  endEmphasis();
  newParagraph();
}

void RTFGenerator::startMemberGroup()
{
  DBG_RTF(m_t << "{\\comment startMemberGroup}\n")
  m_t << rtf_Style_Reset << rtf_BList_DepthStyle() << "\n";
}

void RTFGenerator::endMemberGroup(bool hasHeader)
{
  DBG_RTF(m_t << "{\\comment endMemberGroup}\n")
  if (hasHeader) decIndentLevel();
  m_t << "}";
}

void RTFGenerator::startExamples()
{
  DBG_RTF(m_t << "{\\comment (startExamples)}\n")
  m_t << "{"; // ends at endDescList
  m_t << "{"; // ends at endDescTitle
  startBold();
  newParagraph();
  docify(theTranslator->trExamples());
  endBold();
  m_t << "}";
  newParagraph();
  incIndentLevel();
  m_t << rtf_Style_Reset << rtf_DList_DepthStyle();
}

void RTFGenerator::endExamples()
{
  DBG_RTF(m_t << "{\\comment (endExamples)}\n")
  m_omitParagraph = FALSE;
  newParagraph();
  decIndentLevel();
  m_omitParagraph = TRUE;
  m_t << "}";
}

void RTFGenerator::startParameterType(bool first,const QCString &key)
{
  DBG_RTF(m_t << "{\\comment (startParameterType)}\n")
  if (!first && !key.isEmpty())
  {
    m_t << " " << key << " ";
  }
}

void RTFGenerator::endParameterType()
{
  DBG_RTF(m_t << "{\\comment (endParameterType)}\n")
  m_t << " ";
}

void RTFGenerator::exceptionEntry(const QCString &prefix,bool closeBracket)
{
  DBG_RTF(m_t << "{\\comment (exceptionEntry)}\n")
  if (!prefix.isEmpty())
  {
    m_t << " " << prefix << "(";
  }
  else if (closeBracket)
  {
    m_t << ")";
  }
  m_t << " ";
}

void RTFGenerator::writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *,int)
{
  auto astImpl = dynamic_cast<const DocNodeAST*>(ast);
  if (astImpl)
  {
    RTFDocVisitor visitor(m_t,*m_codeList,ctx?ctx->getDefFileExtension():QCString(""),m_hierarchyLevel);
    std::visit(visitor,astImpl->root);
  }
  m_omitParagraph = TRUE;
}

void RTFGenerator::rtfwriteRuler_doubleline()
{
  DBG_RTF(m_t << "{\\comment (rtfwriteRuler_doubleline)}\n")
  m_t << "{\\pard\\widctlpar\\brdrb\\brdrdb\\brdrw15\\brsp20 \\adjustright \\par}\n";
}

void RTFGenerator::rtfwriteRuler_emboss()
{
  DBG_RTF(m_t << "{\\comment (rtfwriteRuler_emboss)}\n")
  m_t << "{\\pard\\widctlpar\\brdrb\\brdremboss\\brdrw15\\brsp20 \\adjustright \\par}\n";
}

void RTFGenerator::rtfwriteRuler_thick()
{
  DBG_RTF(m_t << "{\\comment (rtfwriteRuler_thick)}\n")
  m_t << "{\\pard\\widctlpar\\brdrb\\brdrs\\brdrw75\\brsp20 \\adjustright \\par}\n";
}

void RTFGenerator::rtfwriteRuler_thin()
{
  DBG_RTF(m_t << "{\\comment (rtfwriteRuler_thin)}\n")
  m_t << "{\\pard\\widctlpar\\brdrb\\brdrs\\brdrw5\\brsp20 \\adjustright \\par}\n";
}

void RTFGenerator::startConstraintList(const QCString &header)
{
  DBG_RTF(m_t << "{\\comment (startConstraintList)}\n")
  m_t << "{"; // ends at endConstraintList
  m_t << "{";
  startBold();
  newParagraph();
  docify(header);
  endBold();
  m_t << "}";
  newParagraph();
  incIndentLevel();
  m_t << rtf_Style_Reset << rtf_DList_DepthStyle();
}

void RTFGenerator::startConstraintParam()
{
  DBG_RTF(m_t << "{\\comment (startConstraintParam)}\n")
  startEmphasis();
}

void RTFGenerator::endConstraintParam()
{
  DBG_RTF(m_t << "{\\comment (endConstraintParam)}\n")
  endEmphasis();
  m_t << " : ";
}

void RTFGenerator::startConstraintType()
{
  DBG_RTF(m_t << "{\\comment (startConstraintType)}\n")
  startEmphasis();
}

void RTFGenerator::endConstraintType()
{
  DBG_RTF(m_t << "{\\comment (endConstraintType)}\n")
  endEmphasis();
  m_t << " ";
}

void RTFGenerator::startConstraintDocs()
{
  DBG_RTF(m_t << "{\\comment (startConstraintDocs)}\n")
}

void RTFGenerator::endConstraintDocs()
{
  DBG_RTF(m_t << "{\\comment (endConstraintDocs)}\n")
  newParagraph();
}

void RTFGenerator::endConstraintList()
{
  DBG_RTF(m_t << "{\\comment (endConstraintList)}\n")
  newParagraph();
  decIndentLevel();
  m_omitParagraph = TRUE;
  m_t << "}";
}

void RTFGenerator::startIndexListItem()
{
  DBG_RTF(m_t << "{\\comment (startIndexListItem)}\n")
}

void RTFGenerator::endIndexListItem()
{
  DBG_RTF(m_t << "{\\comment (endIndexListItem)}\n")
  m_t << "\\par\n";
}

void RTFGenerator::startInlineHeader()
{
  DBG_RTF(m_t << "{\\comment (startInlineHeader)}\n")
  m_t << "{\n";
  m_t << rtf_Style_Reset << rtf_Style["Heading5"].reference();
  startBold();
}

void RTFGenerator::endInlineHeader()
{
  DBG_RTF(m_t << "{\\comment (endInlineHeader)}\n")
  endBold();
  m_t << "\\par";
  m_t << "}\n";
}

void RTFGenerator::startMemberDocSimple(bool isEnum)
{
  DBG_RTF(m_t << "{\\comment (startMemberDocSimple)}\n")
  m_t << "{\\par\n";
  m_t << "{" << rtf_Style["Heading5"].reference() << "\n";
  if (isEnum)
  {
    m_t << theTranslator->trEnumerationValues();
  }
  else
  {
    m_t << theTranslator->trCompoundMembers();
  }
  m_t << ":\\par}\n";
  m_t << rtf_Style_Reset << rtf_DList_DepthStyle();
  m_t << "\\trowd \\trgaph108\\trleft426\\tblind426"
       "\\trbrdrt\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrl\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrb\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrr\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrh\\brdrs\\brdrw10\\brdrcf15 "
       "\\trbrdrv\\brdrs\\brdrw10\\brdrcf15 \n";
  int n=3,columnPos[3] = { 25, 50, 100 };
  if (isEnum)
  {
    columnPos[0]=30;
    columnPos[1]=100;
    n=2;
  }
  for (int i=0;i<n;i++)
  {
    m_t << "\\clvertalt\\clbrdrt\\brdrs\\brdrw10\\brdrcf15 "
         "\\clbrdrl\\brdrs\\brdrw10\\brdrcf15 "
         "\\clbrdrb\\brdrs\\brdrw10\\brdrcf15 "
         "\\clbrdrr \\brdrs\\brdrw10\\brdrcf15 "
         "\\cltxlrtb "
         "\\cellx" << (rtf_pageWidth*columnPos[i]/100) << "\n";
  }
  m_t << "\\pard \\widctlpar\\intbl\\adjustright\n";
}

void RTFGenerator::endMemberDocSimple(bool)
{
  DBG_RTF(m_t << "{\\comment (endMemberDocSimple)}\n")
  m_t << "}\n";
}

void RTFGenerator::startInlineMemberType()
{
  DBG_RTF(m_t << "{\\comment (startInlineMemberType)}\n")
  m_t << "{\\qr ";
}

void RTFGenerator::endInlineMemberType()
{
  DBG_RTF(m_t << "{\\comment (endInlineMemberType)}\n")
  m_t << "\\cell }";
}

void RTFGenerator::startInlineMemberName()
{
  DBG_RTF(m_t << "{\\comment (startInlineMemberName)}\n")
  m_t << "{";
}

void RTFGenerator::endInlineMemberName()
{
  DBG_RTF(m_t << "{\\comment (endInlineMemberName)}\n")
  m_t << "\\cell }";
}

void RTFGenerator::startInlineMemberDoc()
{
  DBG_RTF(m_t << "{\\comment (startInlineMemberDoc)}\n")
  m_t << "{";
}

void RTFGenerator::endInlineMemberDoc()
{
  DBG_RTF(m_t << "{\\comment (endInlineMemberDoc)}\n")
  m_t << "\\cell }{\\row }\n";
}

void RTFGenerator::startLabels()
{
}

void RTFGenerator::writeLabel(const QCString &l,bool isLast)
{
  m_t << "{\\f2 [" << l << "]}";
  if (!isLast) m_t << ", ";
}

void RTFGenerator::endLabels()
{
}

void RTFGenerator::writeInheritedSectionTitle(
                  const QCString &/*id*/,const QCString &ref,
                  const QCString &file,  const QCString &anchor,
                  const QCString &title, const QCString &name)
{
  m_t << rtf_Style_Reset;
  m_t << rtf_Style["Heading4"].reference();
  m_t << "\n";
  m_t << theTranslator->trInheritedFrom(docifyToString(title), objectLinkToString(ref, file, anchor, name));
  m_t << "\\par\n";
  m_t << rtf_Style_Reset << "\n";
}

void RTFGenerator::startParameterList(bool openBracket)
{
  if (openBracket) m_t << "(";
}

void RTFGenerator::endParameterExtra(bool last,bool /* emptyList */, bool closeBracket)
{
  if (last && closeBracket)
  {
    m_t << ")";
  }
}

//----------------------------------------------------------------------

static std::mutex g_rtfFormatMutex;
static std::unordered_map<std::string,std::string> g_tagMap;
static QCString g_nextTag( "AAAAAAAAAA" );

QCString rtfFormatBmkStr(const QCString &name)
{
  std::lock_guard<std::mutex> lock(g_rtfFormatMutex);

  // To overcome the 40-character tag limitation, we
  // substitute a short arbitrary string for the name
  // supplied, and keep track of the correspondence
  // between names and strings.
  auto it = g_tagMap.find(name.str());
  if (it!=g_tagMap.end()) // already known
  {
    return QCString(it->second);
  }

  QCString tag = g_nextTag;
  auto result = g_tagMap.emplace(name.str(), g_nextTag.str());

  if (result.second) // new item was added
  {
    // increment the next tag.

    char* nxtTag = g_nextTag.rawData() + g_nextTag.length() - 1;
    for ( unsigned int i = 0; i < g_nextTag.length(); ++i, --nxtTag )
    {
      if ( ( ++(*nxtTag) ) > 'Z' )
      {
        *nxtTag = 'A';
      }
      else
      {
        // Since there was no carry, we can stop now
        break;
      }
    }
  }

  Debug::print(Debug::Rtf,0,"Name = %s RTF_tag = %s\n",qPrint(name),qPrint(tag));
  return tag;
}

