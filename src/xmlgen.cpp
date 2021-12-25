/******************************************************************************
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

#include <stdlib.h>

#include "textstream.h"
#include "xmlgen.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "classlist.h"
#include "util.h"
#include "defargs.h"
#include "outputgen.h"
#include "dot.h"
#include "dotclassgraph.h"
#include "dotincldepgraph.h"
#include "pagedef.h"
#include "filename.h"
#include "version.h"
#include "xmldocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "parserintf.h"
#include "arguments.h"
#include "memberlist.h"
#include "groupdef.h"
#include "memberdef.h"
#include "namespacedef.h"
#include "membername.h"
#include "membergroup.h"
#include "dirdef.h"
#include "section.h"
#include "htmlentity.h"
#include "resourcemgr.h"
#include "dir.h"
#include "utf8.h"

// no debug info
#define XML_DB(x) do {} while(0)
// debug to stdout
//#define XML_DB(x) printf x
// debug inside output
//#define XML_DB(x) QCString __t;__t.sprintf x;m_t << __t

//------------------

static std::map<MemberListType,std::string> g_xmlSectionMap =
{
  { MemberListType_pubTypes,"public-type" },
  { MemberListType_pubMethods,"public-func" },
  { MemberListType_pubAttribs,"public-attrib" },
  { MemberListType_pubSlots,"public-slot" },
  { MemberListType_signals,"signal" },
  { MemberListType_dcopMethods,"dcop-func" },
  { MemberListType_properties,"property" },
  { MemberListType_events,"event" },
  { MemberListType_interfaces,"interfaces" },
  { MemberListType_services,"services" },
  { MemberListType_pubStaticMethods,"public-static-func" },
  { MemberListType_pubStaticAttribs,"public-static-attrib" },
  { MemberListType_proTypes,"protected-type" },
  { MemberListType_proMethods,"protected-func" },
  { MemberListType_proAttribs,"protected-attrib" },
  { MemberListType_proSlots,"protected-slot" },
  { MemberListType_proStaticMethods,"protected-static-func" },
  { MemberListType_proStaticAttribs,"protected-static-attrib" },
  { MemberListType_pacTypes,"package-type" },
  { MemberListType_pacMethods,"package-func" },
  { MemberListType_pacAttribs,"package-attrib" },
  { MemberListType_pacStaticMethods,"package-static-func" },
  { MemberListType_pacStaticAttribs,"package-static-attrib" },
  { MemberListType_priTypes,"private-type" },
  { MemberListType_priMethods,"private-func" },
  { MemberListType_priAttribs,"private-attrib" },
  { MemberListType_priSlots,"private-slot" },
  { MemberListType_priStaticMethods,"private-static-func" },
  { MemberListType_priStaticAttribs,"private-static-attrib" },
  { MemberListType_friends,"friend" },
  { MemberListType_related,"related" },
  { MemberListType_decDefineMembers,"define" },
  { MemberListType_decProtoMembers,"prototype" },
  { MemberListType_decTypedefMembers,"typedef" },
  { MemberListType_decSequenceMembers,"sequence" },
  { MemberListType_decDictionaryMembers,"dictionary" },
  { MemberListType_decEnumMembers,"enum" },
  { MemberListType_decFuncMembers,"func" },
  { MemberListType_decVarMembers,"var" },
};

static const char *xmlSectionMapper(MemberListType ml)
{
  auto it = g_xmlSectionMap.find(ml);
  return it!=g_xmlSectionMap.end() ? it->second.c_str() : "";
}


inline void writeXMLString(TextStream &t,const QCString &s)
{
  t << convertToXML(s);
}

inline void writeXMLCodeString(TextStream &t,const QCString &str, int &col)
{
  if (str.isEmpty()) return;
  const char *s = str.data();
  char c;
  while ((c=*s++))
  {
    switch(c)
    {
      case '\t':
      {
        static int tabSize = Config_getInt(TAB_SIZE);
	int spacesToNextTabStop = tabSize - (col%tabSize);
	col+=spacesToNextTabStop;
	while (spacesToNextTabStop--) t << "<sp/>";
	break;
	}
      case ' ':  t << "<sp/>"; col++;  break;
      case '<':  t << "&lt;"; col++;   break;
      case '>':  t << "&gt;"; col++;   break;
      case '&':  t << "&amp;"; col++;  break;
      case '\'': t << "&apos;"; col++; break;
      case '"':  t << "&quot;"; col++; break;
      case  1: case  2: case  3: case  4: case  5: case  6: case  7: case  8:
      case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
      case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
      case 27: case 28: case 29: case 30: case 31:
        // encode invalid XML characters (see http://www.w3.org/TR/2000/REC-xml-20001006#NT-Char)
        t << "<sp value=\"" << int(c) << "\"/>";
        break;
      default:   s=writeUTF8Char(t,s-1); col++; break;
    }
  }
}


static void writeXMLHeader(TextStream &t)
{
  t << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n";;
  t << "<doxygen xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
  t << "xsi:noNamespaceSchemaLocation=\"compound.xsd\" ";
  t << "version=\"" << getDoxygenVersion() << "\" ";
  t << "xml:lang=\"" << theTranslator->trISOLang() << "\"";
  t << ">\n";
}

static void writeCombineScript()
{
  QCString outputDirectory = Config_getString(XML_OUTPUT);
  QCString fileName=outputDirectory+"/combine.xslt";
  std::ofstream t(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!t.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }

  t <<
  "<!-- XSLT script to combine the generated output into a single file. \n"
  "     If you have xsltproc you could use:\n"
  "     xsltproc combine.xslt index.xml >all.xml\n"
  "-->\n"
  "<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\">\n"
  "  <xsl:output method=\"xml\" version=\"1.0\" indent=\"no\" standalone=\"yes\" />\n"
  "  <xsl:template match=\"/\">\n"
  "    <doxygen version=\"{doxygenindex/@version}\" xml:lang=\"{doxygenindex/@xml:lang}\">\n"
  "      <!-- Load all doxygen generated xml files -->\n"
  "      <xsl:for-each select=\"doxygenindex/compound\">\n"
  "        <xsl:copy-of select=\"document( concat( @refid, '.xml' ) )/doxygen/*\" />\n"
  "      </xsl:for-each>\n"
  "    </doxygen>\n"
  "  </xsl:template>\n"
  "</xsl:stylesheet>\n";

}

void writeXMLLink(TextStream &t,const QCString &extRef,const QCString &compoundId,
                  const QCString &anchorId,const QCString &text,const QCString &tooltip)
{
  t << "<ref refid=\"" << compoundId;
  if (!anchorId.isEmpty()) t << "_1" << anchorId;
  t << "\" kindref=\"";
  if (!anchorId.isEmpty()) t << "member"; else t << "compound";
  t << "\"";
  if (!extRef.isEmpty()) t << " external=\"" << extRef << "\"";
  if (!tooltip.isEmpty()) t << " tooltip=\"" << convertToXML(tooltip) << "\"";
  t << ">";
  writeXMLString(t,text);
  t << "</ref>";
}

/** Implements TextGeneratorIntf for an XML stream. */
class TextGeneratorXMLImpl : public TextGeneratorIntf
{
  public:
    TextGeneratorXMLImpl(TextStream &t): m_t(t) {}
    void writeString(const QCString &s,bool /*keepSpaces*/) const
    {
      writeXMLString(m_t,s);
    }
    void writeBreak(int) const {}
    void writeLink(const QCString &extRef,const QCString &file,
                   const QCString &anchor,const QCString &text
                  ) const
    {
      writeXMLLink(m_t,extRef,file,anchor,text,QCString());
    }
  private:
    TextStream &m_t;
};

//-------------------------------------------------------------------------------------------

XMLCodeGenerator::XMLCodeGenerator(TextStream &t) : m_t(t), m_lineNumber(-1), m_isMemberRef(FALSE), m_col(0),
      m_insideCodeLine(FALSE), m_normalHLNeedStartTag(TRUE), m_insideSpecialHL(FALSE)
{
}

/** Generator for producing XML formatted source code. */
void XMLCodeGenerator::codify(const QCString &text)
{
  XML_DB(("(codify \"%s\")\n",text));
  if (m_insideCodeLine && !m_insideSpecialHL && m_normalHLNeedStartTag)
  {
    m_t << "<highlight class=\"normal\">";
    m_normalHLNeedStartTag=FALSE;
  }
  writeXMLCodeString(m_t,text,m_col);
}
void XMLCodeGenerator::writeCodeLink(CodeSymbolType,
                   const QCString &ref,const QCString &file,
                   const QCString &anchor,const QCString &name,
                   const QCString &tooltip)
{
  XML_DB(("(writeCodeLink)\n"));
  if (m_insideCodeLine && !m_insideSpecialHL && m_normalHLNeedStartTag)
  {
    m_t << "<highlight class=\"normal\">";
    m_normalHLNeedStartTag=FALSE;
  }
  writeXMLLink(m_t,ref,file,anchor,name,tooltip);
  m_col+=name.length();
}
void XMLCodeGenerator::writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                  const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                 )
{
  XML_DB(("(writeToolTip)\n"));
}
void XMLCodeGenerator::startCodeLine(bool)
{
  XML_DB(("(startCodeLine)\n"));
  m_t << "<codeline";
  if (m_lineNumber!=-1)
  {
    m_t << " lineno=\"" << m_lineNumber << "\"";
    if (!m_refId.isEmpty())
    {
      m_t << " refid=\"" << m_refId << "\"";
      if (m_isMemberRef)
      {
        m_t << " refkind=\"member\"";
      }
      else
      {
        m_t << " refkind=\"compound\"";
      }
    }
    if (!m_external.isEmpty())
    {
      m_t << " external=\"" << m_external << "\"";
    }
  }
  m_t << ">";
  m_insideCodeLine=TRUE;
  m_col=0;
}
void XMLCodeGenerator::endCodeLine()
{
  XML_DB(("(endCodeLine)\n"));
  if (!m_insideSpecialHL && !m_normalHLNeedStartTag)
  {
    m_t << "</highlight>";
    m_normalHLNeedStartTag=TRUE;
  }
  if (m_insideCodeLine) m_t << "</codeline>\n"; // non DocBook
  m_lineNumber = -1;
  m_refId.resize(0);
  m_external.resize(0);
  m_insideCodeLine=FALSE;
}
void XMLCodeGenerator::startFontClass(const QCString &colorClass)
{
  XML_DB(("(startFontClass)\n"));
  if (m_insideCodeLine && !m_insideSpecialHL && !m_normalHLNeedStartTag)
  {
    m_t << "</highlight>";
    m_normalHLNeedStartTag=TRUE;
  }
  m_t << "<highlight class=\"" << colorClass << "\">"; // non DocBook
  m_insideSpecialHL=TRUE;
}
void XMLCodeGenerator::endFontClass()
{
  XML_DB(("(endFontClass)\n"));
  m_t << "</highlight>"; // non DocBook
  m_insideSpecialHL=FALSE;
}
void XMLCodeGenerator::writeCodeAnchor(const QCString &)
{
  XML_DB(("(writeCodeAnchor)\n"));
}
void XMLCodeGenerator::writeLineNumber(const QCString &extRef,const QCString &compId,
                     const QCString &anchorId,int l,bool)
{
  XML_DB(("(writeLineNumber)\n"));
  // we remember the information provided here to use it
  // at the <codeline> start tag.
  m_lineNumber = l;
  if (!compId.isEmpty())
  {
    m_refId=compId;
    if (!anchorId.isEmpty()) m_refId+=(QCString)"_1"+anchorId;
    m_isMemberRef = anchorId!=0;
    if (!extRef.isEmpty()) m_external=extRef;
  }
}
void XMLCodeGenerator::finish()
{
  if (m_insideCodeLine) endCodeLine();
}

void XMLCodeGenerator::startCodeFragment(const QCString &)
{
  m_t << "    <programlisting>\n";
}

void XMLCodeGenerator::endCodeFragment(const QCString &)
{
  m_t << "    </programlisting>\n";
}

//-------------------------------------------------------------------------------------------

static void writeTemplateArgumentList(TextStream &t,
                                      const ArgumentList &al,
                                      const Definition *scope,
                                      const FileDef *fileScope,
                                      int indent)
{
  QCString indentStr;
  indentStr.fill(' ',indent);
  if (al.hasParameters())
  {
    t << indentStr << "<templateparamlist>\n";
    for (const Argument &a : al)
    {
      t << indentStr << "  <param>\n";
      if (!a.type.isEmpty())
      {
        t << indentStr <<  "    <type>";
        linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a.type);
        t << "</type>\n";
      }
      if (!a.name.isEmpty())
      {
        t << indentStr <<  "    <declname>" << convertToXML(a.name) << "</declname>\n";
        t << indentStr <<  "    <defname>" << convertToXML(a.name) << "</defname>\n";
      }
      if (!a.defval.isEmpty())
      {
        t << indentStr << "    <defval>";
        linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a.defval);
        t << "</defval>\n";
      }
      if (!a.typeConstraint.isEmpty())
      {
        t << indentStr << "    <typeconstraint>";
        linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a.typeConstraint);
        t << "</typeconstraint>\n";
      }
      t << indentStr << "  </param>\n";
    }
    t << indentStr << "</templateparamlist>\n";
  }
}

static void writeMemberTemplateLists(const MemberDef *md,TextStream &t)
{
  writeTemplateArgumentList(t,md->templateArguments(),md->getClassDef(),md->getFileDef(),8);
}

static void writeTemplateList(const ClassDef *cd,TextStream &t)
{
  writeTemplateArgumentList(t,cd->templateArguments(),cd,cd->getFileDef(),4);
}

static void writeTemplateList(const ConceptDef *cd,TextStream &t)
{
  writeTemplateArgumentList(t,cd->getTemplateParameterList(),cd,cd->getFileDef(),4);
}

static void writeXMLDocBlock(TextStream &t,
                      const QCString &fileName,
                      int lineNr,
                      const Definition *scope,
                      const MemberDef * md,
                      const QCString &text)
{
  QCString stext = text.stripWhiteSpace();
  if (stext.isEmpty()) return;
  // convert the documentation string into an abstract syntax tree
  std::unique_ptr<IDocParser> parser { createDocParser() };
  std::unique_ptr<DocNode>    root   { validatingParseDoc(*parser.get(),
                                       fileName,lineNr,scope,md,text,FALSE,FALSE,
                                       QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
  // create a code generator
  auto xmlCodeGen = std::make_unique<XMLCodeGenerator>(t);
  // create a parse tree visitor for XML
  auto visitor = std::make_unique<XmlDocVisitor>(t,*xmlCodeGen,scope?scope->getDefFileExtension():QCString(""));
  // visit all nodes
  root->accept(visitor.get());
  // clean up
}

void writeXMLCodeBlock(TextStream &t,FileDef *fd)
{
  auto intf=Doxygen::parserManager->getCodeParser(fd->getDefFileExtension());
  SrcLangExt langExt = getLanguageFromFileName(fd->getDefFileExtension());
  intf->resetCodeParserState();
  XMLCodeGenerator *xmlGen = new XMLCodeGenerator(t);
  xmlGen->startCodeFragment("DoxyCode");
  intf->parseCode(*xmlGen,    // codeOutIntf
                QCString(),           // scopeName
                fileToString(fd->absFilePath(),Config_getBool(FILTER_SOURCE_FILES)),
                langExt,     // lang
                FALSE,       // isExampleBlock
                QCString(),           // exampleName
                fd,          // fileDef
                -1,          // startLine
                -1,          // endLine
                FALSE,       // inlineFragment
                0,           // memberDef
                TRUE         // showLineNumbers
                );
  xmlGen->endCodeFragment("DoxyCode");
  xmlGen->finish();
  delete xmlGen;
}

static void writeMemberReference(TextStream &t,const Definition *def,const MemberDef *rmd,const QCString &tagName)
{
  QCString scope = rmd->getScopeString();
  QCString name = rmd->name();
  if (!scope.isEmpty() && scope!=def->name())
  {
    name.prepend(scope+getLanguageSpecificSeparator(rmd->getLanguage()));
  }
  t << "        <" << tagName << " refid=\"";
  t << rmd->getOutputFileBase() << "_1" << rmd->anchor() << "\"";
  if (rmd->getStartBodyLine()!=-1 && rmd->getBodyDef())
  {
    t << " compoundref=\"" << rmd->getBodyDef()->getOutputFileBase() << "\"";
    t << " startline=\"" << rmd->getStartBodyLine() << "\"";
    if (rmd->getEndBodyLine()!=-1)
    {
      t << " endline=\"" << rmd->getEndBodyLine() << "\"";
    }
  }
  t << ">" << convertToXML(name) << "</" << tagName << ">\n";

}

static void stripQualifiers(QCString &typeStr)
{
  bool done=FALSE;
  typeStr.stripPrefix("friend ");
  while (!done)
  {
    if (typeStr.stripPrefix("static "));
    else if (typeStr.stripPrefix("virtual "));
    else if (typeStr=="virtual") typeStr="";
    else done=TRUE;
  }
}

static QCString classOutputFileBase(const ClassDef *cd)
{
  //static bool inlineGroupedClasses = Config_getBool(INLINE_GROUPED_CLASSES);
  //if (inlineGroupedClasses && cd->partOfGroups()!=0)
  return cd->getOutputFileBase();
  //else
  //  return cd->getOutputFileBase();
}

static QCString memberOutputFileBase(const MemberDef *md)
{
  //static bool inlineGroupedClasses = Config_getBool(INLINE_GROUPED_CLASSES);
  //if (inlineGroupedClasses && md->getClassDef() && md->getClassDef()->partOfGroups()!=0)
  //  return md->getClassDef()->getXmlOutputFileBase();
  //else
  //  return md->getOutputFileBase();
  return md->getOutputFileBase();
}


static void generateXMLForMember(const MemberDef *md,TextStream &ti,TextStream &t,const Definition *def)
{

  // + declaration/definition arg lists
  // + reimplements
  // + reimplementedBy
  // + exceptions
  // + const/volatile specifiers
  // - examples
  // + source definition
  // + source references
  // + source referenced by
  // - body code
  // + template arguments
  //     (templateArguments(), definitionTemplateParameterLists())
  // - call graph

  // enum values are written as part of the enum
  if (md->memberType()==MemberType_EnumValue) return;
  if (md->isHidden()) return;

  // group members are only visible in their group
  //if (def->definitionType()!=Definition::TypeGroup && md->getGroupDef()) return;

  QCString memType;
  bool isFunc=FALSE;
  switch (md->memberType())
  {
    case MemberType_Define:      memType="define";      break;
    case MemberType_Function:    memType="function";    isFunc=TRUE; break;
    case MemberType_Variable:    memType="variable";    break;
    case MemberType_Typedef:     memType="typedef";     break;
    case MemberType_Enumeration: memType="enum";        break;
    case MemberType_EnumValue:   ASSERT(0);             break;
    case MemberType_Signal:      memType="signal";      isFunc=TRUE; break;
    case MemberType_Slot:        memType="slot";        isFunc=TRUE; break;
    case MemberType_Friend:      memType="friend";      isFunc=TRUE; break;
    case MemberType_DCOP:        memType="dcop";        isFunc=TRUE; break;
    case MemberType_Property:    memType="property";    break;
    case MemberType_Event:       memType="event";       break;
    case MemberType_Interface:   memType="interface";   break;
    case MemberType_Service:     memType="service";     break;
    case MemberType_Sequence:    memType="sequence";    break;
    case MemberType_Dictionary:  memType="dictionary";  break;
  }

  ti << "    <member refid=\"" << memberOutputFileBase(md)
     << "_1" << md->anchor() << "\" kind=\"" << memType << "\"><name>"
     << convertToXML(md->name()) << "</name></member>\n";

  QCString scopeName;
  if (md->getClassDef())
    scopeName=md->getClassDef()->name();
  else if (md->getNamespaceDef())
    scopeName=md->getNamespaceDef()->name();

  t << "      <memberdef kind=\"";
  //enum { define_t,variable_t,typedef_t,enum_t,function_t } xmlType = function_t;
  t << memType << "\" id=\"";
  if (md->getGroupDef() && def->definitionType()==Definition::TypeGroup)
  {
    t << md->getGroupDef()->getOutputFileBase();
  }
  else
  {
    t << memberOutputFileBase(md);
  }
  t << "_1"      // encoded ':' character (see util.cpp:convertNameToFile)
    << md->anchor();
  t << "\" prot=\"";
  switch(md->protection())
  {
    case Public:    t << "public";     break;
    case Protected: t << "protected";  break;
    case Private:   t << "private";    break;
    case Package:   t << "package";    break;
  }
  t << "\"";

  t << " static=\"";
  if (md->isStatic()) t << "yes"; else t << "no";
  t << "\"";

  if (md->isConstExpr())
  {
    t << " constexpr=\"yes\"";
  }

  if (isFunc)
  {
    const ArgumentList &al = md->argumentList();
    t << " const=\"";
    if (al.constSpecifier())    t << "yes"; else t << "no";
    t << "\"";

    t << " explicit=\"";
    if (md->isExplicit()) t << "yes"; else t << "no";
    t << "\"";

    t << " inline=\"";
    if (md->isInline()) t << "yes"; else t << "no";
    t << "\"";

    if (al.refQualifier()!=RefQualifierNone)
    {
      t << " refqual=\"";
      if (al.refQualifier()==RefQualifierLValue) t << "lvalue"; else t << "rvalue";
      t << "\"";
    }

    if (md->isFinal())
    {
      t << " final=\"yes\"";
    }

    if (md->isSealed())
    {
      t << " sealed=\"yes\"";
    }

    if (md->isNew())
    {
      t << " new=\"yes\"";
    }

    if (md->isOptional())
    {
      t << " optional=\"yes\"";
    }

    if (md->isRequired())
    {
      t << " required=\"yes\"";
    }

    if (md->isNoExcept())
    {
      t << " noexcept=\"yes\"";
    }

    if (al.volatileSpecifier())
    {
      t << " volatile=\"yes\"";
    }

    t << " virt=\"";
    switch (md->virtualness())
    {
      case Normal:  t << "non-virtual";  break;
      case Virtual: t << "virtual";      break;
      case Pure:    t << "pure-virtual"; break;
      default: ASSERT(0);
    }
    t << "\"";
  }

  if (md->memberType() == MemberType_Enumeration)
  {
    t << " strong=\"";
    if (md->isStrong()) t << "yes"; else t << "no";
    t << "\"";
  }

  if (md->memberType() == MemberType_Variable)
  {
    //ArgumentList *al = md->argumentList();
    //t << " volatile=\"";
    //if (al && al->volatileSpecifier) t << "yes"; else t << "no";

    t << " mutable=\"";
    if (md->isMutable()) t << "yes"; else t << "no";
    t << "\"";

    if (md->isInitonly())
    {
      t << " initonly=\"yes\"";
    }
    if (md->isAttribute())
    {
      t << " attribute=\"yes\"";
    }
    if (md->isUNOProperty())
    {
      t << " property=\"yes\"";
    }
    if (md->isReadonly())
    {
      t << " readonly=\"yes\"";
    }
    if (md->isBound())
    {
      t << " bound=\"yes\"";
    }
    if (md->isRemovable())
    {
      t << " removable=\"yes\"";
    }
    if (md->isConstrained())
    {
      t << " constrained=\"yes\"";
    }
    if (md->isTransient())
    {
      t << " transient=\"yes\"";
    }
    if (md->isMaybeVoid())
    {
      t << " maybevoid=\"yes\"";
    }
    if (md->isMaybeDefault())
    {
      t << " maybedefault=\"yes\"";
    }
    if (md->isMaybeAmbiguous())
    {
      t << " maybeambiguous=\"yes\"";
    }
  }
  else if (md->memberType() == MemberType_Property)
  {
    t << " readable=\"";
    if (md->isReadable()) t << "yes"; else t << "no";
    t << "\"";

    t << " writable=\"";
    if (md->isWritable()) t << "yes"; else t << "no";
    t << "\"";

    t << " gettable=\"";
    if (md->isGettable()) t << "yes"; else t << "no";
    t << "\"";

    t << " privategettable=\"";
    if (md->isPrivateGettable()) t << "yes"; else t << "no";
    t << "\"";

    t << " protectedgettable=\"";
    if (md->isProtectedGettable()) t << "yes"; else t << "no";
    t << "\"";

    t << " settable=\"";
    if (md->isSettable()) t << "yes"; else t << "no";
    t << "\"";

    t << " privatesettable=\"";
    if (md->isPrivateSettable()) t << "yes"; else t << "no";
    t << "\"";

    t << " protectedsettable=\"";
    if (md->isProtectedSettable()) t << "yes"; else t << "no";
    t << "\"";

    if (md->isAssign() || md->isCopy() || md->isRetain() || md->isStrong() || md->isWeak())
    {
      t << " accessor=\"";
      if (md->isAssign())      t << "assign";
      else if (md->isCopy())   t << "copy";
      else if (md->isRetain()) t << "retain";
      else if (md->isStrong()) t << "strong";
      else if (md->isWeak())   t << "weak";
      t << "\"";
    }
  }
  else if (md->memberType() == MemberType_Event)
  {
    t << " add=\"";
    if (md->isAddable()) t << "yes"; else t << "no";
    t << "\"";

    t << " remove=\"";
    if (md->isRemovable()) t << "yes"; else t << "no";
    t << "\"";

    t << " raise=\"";
    if (md->isRaisable()) t << "yes"; else t << "no";
    t << "\"";
  }

  t << ">\n";

  if (md->memberType()!=MemberType_Define &&
      md->memberType()!=MemberType_Enumeration
     )
  {
    writeMemberTemplateLists(md,t);
    QCString typeStr = md->typeString();
    stripQualifiers(typeStr);
    t << "        <type>";
    linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,typeStr);
    t << "</type>\n";
    t << "        <definition>" << convertToXML(md->definition()) << "</definition>\n";
    t << "        <argsstring>" << convertToXML(md->argsString()) << "</argsstring>\n";
  }

  if (md->memberType() == MemberType_Enumeration)
  {
    t << "        <type>";
    linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,md->enumBaseType());
    t << "</type>\n";
  }

  t << "        <name>" << convertToXML(md->name()) << "</name>\n";

  if (md->memberType() == MemberType_Property)
  {
    if (md->isReadable())
      t << "        <read>" << convertToXML(md->getReadAccessor()) << "</read>\n";
    if (md->isWritable())
      t << "        <write>" << convertToXML(md->getWriteAccessor()) << "</write>\n";
  }

  if (md->memberType()==MemberType_Variable && !md->bitfieldString().isEmpty())
  {
    QCString bitfield = md->bitfieldString();
    if (bitfield.at(0)==':') bitfield=bitfield.mid(1);
    t << "        <bitfield>" << convertToXML(bitfield) << "</bitfield>\n";
  }

  const MemberDef *rmd = md->reimplements();
  if (rmd)
  {
    t << "        <reimplements refid=\""
      << memberOutputFileBase(rmd) << "_1" << rmd->anchor() << "\">"
      << convertToXML(rmd->name()) << "</reimplements>\n";
  }
  for (const auto &rbmd : md->reimplementedBy())
  {
    t << "        <reimplementedby refid=\""
      << memberOutputFileBase(rbmd) << "_1" << rbmd->anchor() << "\">"
      << convertToXML(rbmd->name()) << "</reimplementedby>\n";
  }

  if (md->isFriendClass()) // for friend classes we show a link to the class as a "parameter"
  {
    t << "        <param>\n";
    t << "          <type>";
    linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,md->name());
    t << "</type>\n";
    t << "        </param>\n";
  }
  else if (isFunc) //function
  {
    const ArgumentList &declAl = md->declArgumentList();
    const ArgumentList &defAl = md->argumentList();
    if (declAl.hasParameters())
    {
      auto defIt = defAl.begin();
      for (const Argument &a : declAl)
      {
        //const Argument *defArg = defAli.current();
        const Argument *defArg = 0;
        if (defIt!=defAl.end())
        {
          defArg = &(*defIt);
          ++defIt;
        }
        t << "        <param>\n";
        if (!a.attrib.isEmpty())
        {
          t << "          <attributes>";
          writeXMLString(t,a.attrib);
          t << "</attributes>\n";
        }
        if (!a.type.isEmpty())
        {
          t << "          <type>";
          linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,a.type);
          t << "</type>\n";
        }
        if (!a.name.isEmpty())
        {
          t << "          <declname>";
          writeXMLString(t,a.name);
          t << "</declname>\n";
        }
        if (defArg && !defArg->name.isEmpty() && defArg->name!=a.name)
        {
          t << "          <defname>";
          writeXMLString(t,defArg->name);
          t << "</defname>\n";
        }
        if (!a.array.isEmpty())
        {
          t << "          <array>";
          writeXMLString(t,a.array);
          t << "</array>\n";
        }
        if (!a.defval.isEmpty())
        {
          t << "          <defval>";
          linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,a.defval);
          t << "</defval>\n";
        }
        if (defArg && defArg->hasDocumentation())
        {
          t << "          <briefdescription>";
          writeXMLDocBlock(t,md->getDefFileName(),md->getDefLine(),
                           md->getOuterScope(),md,defArg->docs);
          t << "</briefdescription>\n";
        }
        t << "        </param>\n";
      }
    }
  }
  else if (md->memberType()==MemberType_Define &&
          !md->argsString().isEmpty()) // define
  {
    if (md->argumentList().empty())     // special case for "foo()" to
                                        // distinguish it from "foo".
    {
      t << "        <param></param>\n";
    }
    else
    {
      for (const Argument &a : md->argumentList())
      {
        t << "        <param><defname>" << a.type << "</defname></param>\n";
      }
    }
  }
  if (!md->requiresClause().isEmpty())
  {
    t << "    <requiresclause>";
    linkifyText(TextGeneratorXMLImpl(t),md,md->getFileDef(),md,md->requiresClause());
    t << "    </requiresclause>\n";
  }

  if (md->hasOneLineInitializer() || md->hasMultiLineInitializer())
  {
    t << "        <initializer>";
    linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,md->initializer());
    t << "</initializer>\n";
  }

  if (!md->excpString().isEmpty())
  {
    t << "        <exceptions>";
    linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,md->excpString());
    t << "</exceptions>\n";
  }

  if (md->memberType()==MemberType_Enumeration) // enum
  {
    for (const auto &emd : md->enumFieldList())
    {
      ti << "    <member refid=\"" << memberOutputFileBase(md)
         << "_1" << emd->anchor() << "\" kind=\"enumvalue\"><name>"
         << convertToXML(emd->name()) << "</name></member>\n";

      t << "        <enumvalue id=\"" << memberOutputFileBase(md) << "_1"
        << emd->anchor() << "\" prot=\"";
      switch (emd->protection())
      {
        case Public:    t << "public";    break;
        case Protected: t << "protected"; break;
        case Private:   t << "private";   break;
        case Package:   t << "package";   break;
      }
      t << "\">\n";
      t << "          <name>";
      writeXMLString(t,emd->name());
      t << "</name>\n";
      if (!emd->initializer().isEmpty())
      {
        t << "          <initializer>";
        writeXMLString(t,emd->initializer());
        t << "</initializer>\n";
      }
      t << "          <briefdescription>\n";
      writeXMLDocBlock(t,emd->briefFile(),emd->briefLine(),emd->getOuterScope(),emd,emd->briefDescription());
      t << "          </briefdescription>\n";
      t << "          <detaileddescription>\n";
      writeXMLDocBlock(t,emd->docFile(),emd->docLine(),emd->getOuterScope(),emd,emd->documentation());
      t << "          </detaileddescription>\n";
      t << "        </enumvalue>\n";
    }
  }
  t << "        <briefdescription>\n";
  writeXMLDocBlock(t,md->briefFile(),md->briefLine(),md->getOuterScope(),md,md->briefDescription());
  t << "        </briefdescription>\n";
  t << "        <detaileddescription>\n";
  writeXMLDocBlock(t,md->docFile(),md->docLine(),md->getOuterScope(),md,md->documentation());
  t << "        </detaileddescription>\n";
  t << "        <inbodydescription>\n";
  writeXMLDocBlock(t,md->docFile(),md->inbodyLine(),md->getOuterScope(),md,md->inbodyDocumentation());
  t << "        </inbodydescription>\n";
  if (md->getDefLine()!=-1)
  {
    t << "        <location file=\""
      << convertToXML(stripFromPath(md->getDefFileName())) << "\" line=\""
      << md->getDefLine() << "\" column=\""
      << md->getDefColumn() << "\"" ;
    if (md->getStartBodyLine()!=-1)
    {
      const FileDef *bodyDef = md->getBodyDef();
      if (bodyDef)
      {
        t << " bodyfile=\"" << convertToXML(stripFromPath(bodyDef->absFilePath())) << "\"";
      }
      t << " bodystart=\"" << md->getStartBodyLine() << "\" bodyend=\""
        << md->getEndBodyLine() << "\"";
    }
    if (md->getDeclLine()!=-1)
    {
      t << " declfile=\"" << convertToXML(stripFromPath(md->getDeclFileName())) << "\" declline=\""
        << md->getDeclLine() << "\" declcolumn=\""
        << md->getDeclColumn() << "\"";
    }
    t << "/>\n";
  }

  //printf("md->getReferencesMembers()=%p\n",md->getReferencesMembers());
  auto refList = md->getReferencesMembers();
  for (const auto &refmd : refList)
  {
    writeMemberReference(t,def,refmd,"references");
  }
  auto refByList = md->getReferencedByMembers();
  for (const auto &refmd : refByList)
  {
    writeMemberReference(t,def,refmd,"referencedby");
  }

  t << "      </memberdef>\n";
}

// namespace members are also inserted in the file scope, but
// to prevent this duplication in the XML output, we optionally filter those here.
static bool memberVisible(const Definition *d,const MemberDef *md)
{
    return Config_getBool(XML_NS_MEMB_FILE_SCOPE) ||
           d->definitionType()!=Definition::TypeFile ||
           md->getNamespaceDef()==0;
}

static void generateXMLSection(const Definition *d,TextStream &ti,TextStream &t,
                      const MemberList *ml,const QCString &kind,const QCString &header=QCString(),
                      const QCString &documentation=QCString())
{
  if (ml==0) return;
  int count=0;
  for (const auto &md : *ml)
  {
    if (memberVisible(d,md) && (md->memberType()!=MemberType_EnumValue) &&
        !md->isHidden())
    {
      count++;
    }
  }
  if (count==0) return; // empty list

  t << "      <sectiondef kind=\"" << kind << "\">\n";
  if (!header.isEmpty())
  {
    t << "      <header>" << convertToXML(header) << "</header>\n";
  }
  if (!documentation.isEmpty())
  {
    t << "      <description>";
    writeXMLDocBlock(t,d->docFile(),d->docLine(),d,0,documentation);
    t << "</description>\n";
  }
  for (const auto &md : *ml)
  {
    if (memberVisible(d,md))
    {
      generateXMLForMember(md,ti,t,d);
    }
  }
  t << "      </sectiondef>\n";
}

static void writeListOfAllMembers(const ClassDef *cd,TextStream &t)
{
  t << "    <listofallmembers>\n";
  for (auto &mni : cd->memberNameInfoLinkedMap())
  {
    for (auto &mi : *mni)
    {
      const MemberDef *md=mi->memberDef();
      if (!md->isAnonymous())
      {
        Protection prot = mi->prot();
        Specifier virt=md->virtualness();
        t << "      <member refid=\"" << memberOutputFileBase(md) << "_1" <<
          md->anchor() << "\" prot=\"";
        switch (prot)
        {
          case Public:    t << "public";    break;
          case Protected: t << "protected"; break;
          case Private:   t << "private";   break;
          case Package:   t << "package";   break;
        }
        t << "\" virt=\"";
        switch(virt)
        {
          case Normal:  t << "non-virtual";  break;
          case Virtual: t << "virtual";      break;
          case Pure:    t << "pure-virtual"; break;
        }
        t << "\"";
        if (!mi->ambiguityResolutionScope().isEmpty())
        {
          t << " ambiguityscope=\"" << convertToXML(mi->ambiguityResolutionScope()) << "\"";
        }
        t << "><scope>" << convertToXML(cd->name()) << "</scope><name>" <<
          convertToXML(md->name()) << "</name></member>\n";
      }
    }
  }
  t << "    </listofallmembers>\n";
}

static void writeInnerClasses(const ClassLinkedRefMap &cl,TextStream &t)
{
  for (const auto &cd : cl)
  {
    if (!cd->isHidden() && !cd->isAnonymous())
    {
      t << "    <innerclass refid=\"" << classOutputFileBase(cd)
        << "\" prot=\"";
      switch(cd->protection())
      {
         case Public:    t << "public";     break;
         case Protected: t << "protected";  break;
         case Private:   t << "private";    break;
         case Package:   t << "package";    break;
      }
      t << "\">" << convertToXML(cd->name()) << "</innerclass>\n";
    }
  }
}

static void writeInnerNamespaces(const NamespaceLinkedRefMap &nl,TextStream &t)
{
  for (const auto &nd : nl)
  {
    if (!nd->isHidden() && !nd->isAnonymous())
    {
      t << "    <innernamespace refid=\"" << nd->getOutputFileBase()
        << "\"" << (nd->isInline() ? " inline=\"yes\"" : "")
        << ">" << convertToXML(nd->name()) << "</innernamespace>\n";
    }
  }
}

static void writeInnerFiles(const FileList &fl,TextStream &t)
{
  for (const auto &fd : fl)
  {
    t << "    <innerfile refid=\"" << fd->getOutputFileBase()
      << "\">" << convertToXML(fd->name()) << "</innerfile>\n";
  }
}

static void writeInnerPages(const PageLinkedRefMap &pl,TextStream &t)
{
  for (const auto &pd : pl)
  {
    t << "    <innerpage refid=\"" << pd->getOutputFileBase();
    if (pd->getGroupDef())
    {
      t << "_" << pd->name();
    }
    t << "\">" << convertToXML(pd->title()) << "</innerpage>\n";
  }
}

static void writeInnerGroups(const GroupList &gl,TextStream &t)
{
  for (const auto &sgd : gl)
  {
    t << "    <innergroup refid=\"" << sgd->getOutputFileBase()
      << "\">" << convertToXML(sgd->groupTitle())
      << "</innergroup>\n";
  }
}

static void writeInnerDirs(const DirList *dl,TextStream &t)
{
  if (dl)
  {
    for(const auto subdir : *dl)
    {
      t << "    <innerdir refid=\"" << subdir->getOutputFileBase()
        << "\">" << convertToXML(subdir->displayName()) << "</innerdir>\n";
    }
  }
}

static void writeIncludeInfo(const IncludeInfo *ii,TextStream &t)
{
  if (ii)
  {
    QCString nm = ii->includeName;
    if (nm.isEmpty() && ii->fileDef) nm = ii->fileDef->docName();
    if (!nm.isEmpty())
    {
      t << "    <includes";
      if (ii->fileDef && !ii->fileDef->isReference()) // TODO: support external references
      {
        t << " refid=\"" << ii->fileDef->getOutputFileBase() << "\"";
      }
      t << " local=\"" << (ii->local ? "yes" : "no") << "\">";
      t << nm;
      t << "</includes>\n";
    }
  }
}

static void generateXMLForClass(const ClassDef *cd,TextStream &ti)
{
  // + brief description
  // + detailed description
  // + template argument list(s)
  // - include file
  // + member groups
  // + inheritance diagram
  // + list of direct super classes
  // + list of direct sub classes
  // + list of inner classes
  // + collaboration diagram
  // + list of all members
  // + user defined member sections
  // + standard member sections
  // + detailed member documentation
  // - examples using the class

  if (cd->isReference())        return; // skip external references.
  if (cd->isHidden())           return; // skip hidden classes.
  if (cd->isAnonymous())        return; // skip anonymous compounds.
  if (cd->templateMaster()!=0)  return; // skip generated template instances.
  if (cd->isArtificial())       return; // skip artificially created classes

  msg("Generating XML output for class %s\n",qPrint(cd->name()));

  ti << "  <compound refid=\"" << classOutputFileBase(cd)
     << "\" kind=\"" << cd->compoundTypeString()
     << "\"><name>" << convertToXML(cd->name()) << "</name>\n";

  QCString outputDirectory = Config_getString(XML_OUTPUT);
  QCString fileName=outputDirectory+"/"+ classOutputFileBase(cd)+".xml";
  std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }
  TextStream t(&f);

  writeXMLHeader(t);
  t << "  <compounddef id=\""
    << classOutputFileBase(cd) << "\" kind=\""
    << cd->compoundTypeString() << "\" language=\""
    << langToString(cd->getLanguage()) << "\" prot=\"";
  switch (cd->protection())
  {
    case Public:    t << "public";    break;
    case Protected: t << "protected"; break;
    case Private:   t << "private";   break;
    case Package:   t << "package";   break;
  }
  if (cd->isFinal()) t << "\" final=\"yes";
  if (cd->isSealed()) t << "\" sealed=\"yes";
  if (cd->isAbstract()) t << "\" abstract=\"yes";
  t << "\">\n";
  t << "    <compoundname>";
  writeXMLString(t,cd->name());
  t << "</compoundname>\n";
  for (const auto &bcd : cd->baseClasses())
  {
    t << "    <basecompoundref ";
    if (bcd.classDef->isLinkable())
    {
      t << "refid=\"" << classOutputFileBase(bcd.classDef) << "\" ";
    }
    t << "prot=\"";
    switch (bcd.prot)
    {
      case Public:    t << "public";    break;
      case Protected: t << "protected"; break;
      case Private:   t << "private";   break;
      case Package: ASSERT(0); break;
    }
    t << "\" virt=\"";
    switch(bcd.virt)
    {
      case Normal:  t << "non-virtual";  break;
      case Virtual: t << "virtual";      break;
      case Pure:    t <<"pure-virtual"; break;
    }
    t << "\">";
    if (!bcd.templSpecifiers.isEmpty())
    {
      t << convertToXML(
          insertTemplateSpecifierInScope(
            bcd.classDef->name(),bcd.templSpecifiers)
          );
    }
    else
    {
      t << convertToXML(bcd.classDef->displayName());
    }
    t  << "</basecompoundref>\n";
  }
  for (const auto &bcd : cd->subClasses())
  {
    t << "    <derivedcompoundref refid=\""
      << classOutputFileBase(bcd.classDef)
      << "\" prot=\"";
    switch (bcd.prot)
    {
      case Public:    t << "public";    break;
      case Protected: t << "protected"; break;
      case Private:   t << "private";   break;
      case Package: ASSERT(0); break;
    }
    t << "\" virt=\"";
    switch (bcd.virt)
    {
      case Normal:  t << "non-virtual";  break;
      case Virtual: t << "virtual";      break;
      case Pure:    t << "pure-virtual"; break;
    }
    t << "\">" << convertToXML(bcd.classDef->displayName())
      << "</derivedcompoundref>\n";
  }

  writeIncludeInfo(cd->includeInfo(),t);

  writeInnerClasses(cd->getClasses(),t);

  writeTemplateList(cd,t);
  for (const auto &mg : cd->getMemberGroups())
  {
    generateXMLSection(cd,ti,t,&mg->members(),"user-defined",mg->header(),
        mg->documentation());
  }

  for (const auto &ml : cd->getMemberLists())
  {
    if ((ml->listType()&MemberListType_detailedLists)==0)
    {
      generateXMLSection(cd,ti,t,ml.get(),xmlSectionMapper(ml->listType()));
    }
  }

  if (!cd->requiresClause().isEmpty())
  {
    t << "    <requiresclause>";
    linkifyText(TextGeneratorXMLImpl(t),cd,cd->getFileDef(),0,cd->requiresClause());
    t << "    </requiresclause>\n";
  }

  t << "    <briefdescription>\n";
  writeXMLDocBlock(t,cd->briefFile(),cd->briefLine(),cd,0,cd->briefDescription());
  t << "    </briefdescription>\n";
  t << "    <detaileddescription>\n";
  writeXMLDocBlock(t,cd->docFile(),cd->docLine(),cd,0,cd->documentation());
  t << "    </detaileddescription>\n";
  DotClassGraph inheritanceGraph(cd,Inheritance);
  if (!inheritanceGraph.isTrivial())
  {
    t << "    <inheritancegraph>\n";
    inheritanceGraph.writeXML(t);
    t << "    </inheritancegraph>\n";
  }
  DotClassGraph collaborationGraph(cd,Collaboration);
  if (!collaborationGraph.isTrivial())
  {
    t << "    <collaborationgraph>\n";
    collaborationGraph.writeXML(t);
    t << "    </collaborationgraph>\n";
  }
  t << "    <location file=\""
    << convertToXML(stripFromPath(cd->getDefFileName())) << "\" line=\""
    << cd->getDefLine() << "\"" << " column=\""
    << cd->getDefColumn() << "\"" ;
    if (cd->getStartBodyLine()!=-1)
    {
      const FileDef *bodyDef = cd->getBodyDef();
      if (bodyDef)
      {
        t << " bodyfile=\"" << convertToXML(stripFromPath(bodyDef->absFilePath())) << "\"";
      }
      t << " bodystart=\"" << cd->getStartBodyLine() << "\" bodyend=\""
        << cd->getEndBodyLine() << "\"";
    }
  t << "/>\n";
  writeListOfAllMembers(cd,t);
  t << "  </compounddef>\n";
  t << "</doxygen>\n";

  ti << "  </compound>\n";
}

static void generateXMLForConcept(const ConceptDef *cd,TextStream &ti)
{
  if (cd->isReference() || cd->isHidden()) return; // skip external references.

  ti << "  <compound refid=\"" << cd->getOutputFileBase()
     << "\" kind=\"concept\"" << "><name>"
     << convertToXML(cd->name()) << "</name>\n";

  QCString outputDirectory = Config_getString(XML_OUTPUT);
  QCString fileName=outputDirectory+"/"+cd->getOutputFileBase()+".xml";
  std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }
  TextStream t(&f);
  writeXMLHeader(t);
  t << "  <compounddef id=\"" << cd->getOutputFileBase()
    << "\" kind=\"concept\">\n";
  t << "    <compoundname>";
  writeXMLString(t,cd->name());
  t << "</compoundname>\n";
  writeIncludeInfo(cd->includeInfo(),t);
  writeTemplateList(cd,t);
  t << "    <initializer>";
  linkifyText(TextGeneratorXMLImpl(t),cd,cd->getFileDef(),0,cd->initializer());
  t << "    </initializer>\n";
  t << "    <briefdescription>\n";
  writeXMLDocBlock(t,cd->briefFile(),cd->briefLine(),cd,0,cd->briefDescription());
  t << "    </briefdescription>\n";
  t << "    <detaileddescription>\n";
  writeXMLDocBlock(t,cd->docFile(),cd->docLine(),cd,0,cd->documentation());
  t << "    </detaileddescription>\n";
  t << "    <location file=\""
    << convertToXML(stripFromPath(cd->getDefFileName())) << "\" line=\""
    << cd->getDefLine() << "\"" << " column=\""
    << cd->getDefColumn() << "\"/>\n" ;
  t << "  </compounddef>\n";
  t << "</doxygen>\n";

  ti << "  </compound>\n";
}

static void generateXMLForNamespace(const NamespaceDef *nd,TextStream &ti)
{
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + location
  // - files containing (parts of) the namespace definition

  if (nd->isReference() || nd->isHidden()) return; // skip external references

  ti << "  <compound refid=\"" << nd->getOutputFileBase()
     << "\" kind=\"namespace\"" << "><name>"
     << convertToXML(nd->name()) << "</name>\n";

  QCString outputDirectory = Config_getString(XML_OUTPUT);
  QCString fileName=outputDirectory+"/"+nd->getOutputFileBase()+".xml";
  std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }
  TextStream t(&f);

  writeXMLHeader(t);
  t << "  <compounddef id=\"" << nd->getOutputFileBase()
    << "\" kind=\"namespace\" "
    << (nd->isInline()?"inline=\"yes\" ":"")
    << "language=\""
    << langToString(nd->getLanguage()) << "\">\n";
  t << "    <compoundname>";
  writeXMLString(t,nd->name());
  t << "</compoundname>\n";

  writeInnerClasses(nd->getClasses(),t);
  writeInnerNamespaces(nd->getNamespaces(),t);

  for (const auto &mg : nd->getMemberGroups())
  {
    generateXMLSection(nd,ti,t,&mg->members(),"user-defined",mg->header(),
          mg->documentation());
  }

  for (const auto &ml : nd->getMemberLists())
  {
    if ((ml->listType()&MemberListType_declarationLists)!=0)
    {
      generateXMLSection(nd,ti,t,ml.get(),xmlSectionMapper(ml->listType()));
    }
  }

  t << "    <briefdescription>\n";
  writeXMLDocBlock(t,nd->briefFile(),nd->briefLine(),nd,0,nd->briefDescription());
  t << "    </briefdescription>\n";
  t << "    <detaileddescription>\n";
  writeXMLDocBlock(t,nd->docFile(),nd->docLine(),nd,0,nd->documentation());
  t << "    </detaileddescription>\n";
  t << "    <location file=\""
    << convertToXML(stripFromPath(nd->getDefFileName())) << "\" line=\""
    << nd->getDefLine() << "\"" << " column=\""
    << nd->getDefColumn() << "\"/>\n" ;
  t << "  </compounddef>\n";
  t << "</doxygen>\n";

  ti << "  </compound>\n";
}

static void generateXMLForFile(FileDef *fd,TextStream &ti)
{
  // + includes files
  // + includedby files
  // + include graph
  // + included by graph
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + source code
  // + location
  // - number of lines

  if (fd->isReference()) return; // skip external references

  ti << "  <compound refid=\"" << fd->getOutputFileBase()
     << "\" kind=\"file\"><name>" << convertToXML(fd->name())
     << "</name>\n";

  QCString outputDirectory = Config_getString(XML_OUTPUT);
  QCString fileName=outputDirectory+"/"+fd->getOutputFileBase()+".xml";
  std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }
  TextStream t(&f);

  writeXMLHeader(t);
  t << "  <compounddef id=\"" << fd->getOutputFileBase()
    << "\" kind=\"file\" language=\""
    << langToString(fd->getLanguage()) << "\">\n";
  t << "    <compoundname>";
  writeXMLString(t,fd->name());
  t << "</compoundname>\n";

  for (const auto &inc : fd->includeFileList())
  {
    t << "    <includes";
    if (inc.fileDef && !inc.fileDef->isReference()) // TODO: support external references
    {
      t << " refid=\"" << inc.fileDef->getOutputFileBase() << "\"";
    }
    t << " local=\"" << (inc.local ? "yes" : "no") << "\">";
    t << inc.includeName;
    t << "</includes>\n";
  }

  for (const auto &inc : fd->includedByFileList())
  {
    t << "    <includedby";
    if (inc.fileDef && !inc.fileDef->isReference()) // TODO: support external references
    {
      t << " refid=\"" << inc.fileDef->getOutputFileBase() << "\"";
    }
    t << " local=\"" << (inc.local ? "yes" : "no") << "\">";
    t << inc.includeName;
    t << "</includedby>\n";
  }

  DotInclDepGraph incDepGraph(fd,FALSE);
  if (!incDepGraph.isTrivial())
  {
    t << "    <incdepgraph>\n";
    incDepGraph.writeXML(t);
    t << "    </incdepgraph>\n";
  }

  DotInclDepGraph invIncDepGraph(fd,TRUE);
  if (!invIncDepGraph.isTrivial())
  {
    t << "    <invincdepgraph>\n";
    invIncDepGraph.writeXML(t);
    t << "    </invincdepgraph>\n";
  }

  writeInnerClasses(fd->getClasses(),t);
  writeInnerNamespaces(fd->getNamespaces(),t);

  for (const auto &mg : fd->getMemberGroups())
  {
    generateXMLSection(fd,ti,t,&mg->members(),"user-defined",mg->header(),
        mg->documentation());
  }

  for (const auto &ml : fd->getMemberLists())
  {
    if ((ml->listType()&MemberListType_declarationLists)!=0)
    {
      generateXMLSection(fd,ti,t,ml.get(),xmlSectionMapper(ml->listType()));
    }
  }

  t << "    <briefdescription>\n";
  writeXMLDocBlock(t,fd->briefFile(),fd->briefLine(),fd,0,fd->briefDescription());
  t << "    </briefdescription>\n";
  t << "    <detaileddescription>\n";
  writeXMLDocBlock(t,fd->docFile(),fd->docLine(),fd,0,fd->documentation());
  t << "    </detaileddescription>\n";
  if (Config_getBool(XML_PROGRAMLISTING))
  {
    writeXMLCodeBlock(t,fd);
  }
  t << "    <location file=\"" << convertToXML(stripFromPath(fd->getDefFileName())) << "\"/>\n";
  t << "  </compounddef>\n";
  t << "</doxygen>\n";

  ti << "  </compound>\n";
}

static void generateXMLForGroup(const GroupDef *gd,TextStream &ti)
{
  // + members
  // + member groups
  // + files
  // + classes
  // + namespaces
  // - packages
  // + pages
  // + child groups
  // - examples
  // + brief description
  // + detailed description

  if (gd->isReference()) return; // skip external references

  ti << "  <compound refid=\"" << gd->getOutputFileBase()
     << "\" kind=\"group\"><name>" << convertToXML(gd->name()) << "</name>\n";

  QCString outputDirectory = Config_getString(XML_OUTPUT);
  QCString fileName=outputDirectory+"/"+gd->getOutputFileBase()+".xml";
  std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }
  TextStream t(&f);

  writeXMLHeader(t);
  t << "  <compounddef id=\""
    << gd->getOutputFileBase() << "\" kind=\"group\">\n";
  t << "    <compoundname>" << convertToXML(gd->name()) << "</compoundname>\n";
  t << "    <title>" << convertToXML(gd->groupTitle()) << "</title>\n";

  writeInnerFiles(gd->getFiles(),t);
  writeInnerClasses(gd->getClasses(),t);
  writeInnerNamespaces(gd->getNamespaces(),t);
  writeInnerPages(gd->getPages(),t);
  writeInnerGroups(gd->getSubGroups(),t);

  for (const auto &mg : gd->getMemberGroups())
  {
    generateXMLSection(gd,ti,t,&mg->members(),"user-defined",mg->header(),
        mg->documentation());
  }

  for (const auto &ml : gd->getMemberLists())
  {
    if ((ml->listType()&MemberListType_declarationLists)!=0)
    {
      generateXMLSection(gd,ti,t,ml.get(),xmlSectionMapper(ml->listType()));
    }
  }

  t << "    <briefdescription>\n";
  writeXMLDocBlock(t,gd->briefFile(),gd->briefLine(),gd,0,gd->briefDescription());
  t << "    </briefdescription>\n";
  t << "    <detaileddescription>\n";
  writeXMLDocBlock(t,gd->docFile(),gd->docLine(),gd,0,gd->documentation());
  t << "    </detaileddescription>\n";
  t << "  </compounddef>\n";
  t << "</doxygen>\n";

  ti << "  </compound>\n";
}

static void generateXMLForDir(DirDef *dd,TextStream &ti)
{
  if (dd->isReference()) return; // skip external references
  ti << "  <compound refid=\"" << dd->getOutputFileBase()
     << "\" kind=\"dir\"><name>" << convertToXML(dd->displayName())
     << "</name>\n";

  QCString outputDirectory = Config_getString(XML_OUTPUT);
  QCString fileName=outputDirectory+"/"+dd->getOutputFileBase()+".xml";
  std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }
  TextStream t(&f);

  writeXMLHeader(t);
  t << "  <compounddef id=\""
    << dd->getOutputFileBase() << "\" kind=\"dir\">\n";
  t << "    <compoundname>" << convertToXML(dd->displayName()) << "</compoundname>\n";

  writeInnerDirs(&dd->subDirs(),t);
  writeInnerFiles(dd->getFiles(),t);

  t << "    <briefdescription>\n";
  writeXMLDocBlock(t,dd->briefFile(),dd->briefLine(),dd,0,dd->briefDescription());
  t << "    </briefdescription>\n";
  t << "    <detaileddescription>\n";
  writeXMLDocBlock(t,dd->docFile(),dd->docLine(),dd,0,dd->documentation());
  t << "    </detaileddescription>\n";
  t << "    <location file=\"" << convertToXML(stripFromPath(dd->name())) << "\"/>\n";
  t << "  </compounddef>\n";
  t << "</doxygen>\n";

  ti << "  </compound>\n";
}

static void generateXMLForPage(PageDef *pd,TextStream &ti,bool isExample)
{
  // + name
  // + title
  // + documentation
  // + location

  const char *kindName = isExample ? "example" : "page";

  if (pd->isReference()) return;

  QCString pageName = pd->getOutputFileBase();
  if (pd->getGroupDef())
  {
    pageName+=(QCString)"_"+pd->name();
  }
  if (pageName=="index") pageName="indexpage"; // to prevent overwriting the generated index page.

  ti << "  <compound refid=\"" << pageName
     << "\" kind=\"" << kindName << "\"><name>" << convertToXML(pd->name())
     << "</name>\n";

  QCString outputDirectory = Config_getString(XML_OUTPUT);
  QCString fileName=outputDirectory+"/"+pageName+".xml";
  std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }
  TextStream t(&f);

  writeXMLHeader(t);
  t << "  <compounddef id=\"" << pageName;
  t << "\" kind=\"" << kindName << "\">\n";
  t << "    <compoundname>" << convertToXML(pd->name())
    << "</compoundname>\n";

  if (pd==Doxygen::mainPage.get()) // main page is special
  {
    QCString title;
    if (mainPageHasTitle())
    {
      title = filterTitle(convertCharEntitiesToUTF8(Doxygen::mainPage->title()).str());
    }
    else
    {
      title = Config_getString(PROJECT_NAME);
    }
    t << "    <title>" << convertToXML(convertCharEntitiesToUTF8(title))
      << "</title>\n";
  }
  else
  {
    const SectionInfo *si = SectionManager::instance().find(pd->name());
    if (si)
    {
      t << "    <title>" << convertToXML(filterTitle(convertCharEntitiesToUTF8(si->title()).str()))
        << "</title>\n";
    }
  }
  writeInnerPages(pd->getSubPages(),t);
  const SectionRefs &sectionRefs = pd->getSectionRefs();
  if (pd->localToc().isXmlEnabled() && !sectionRefs.empty())
  {
    t << "    <tableofcontents>\n";
    int level=1,l;
    bool inLi[5]={ FALSE, FALSE, FALSE, FALSE, FALSE };
    int maxLevel = pd->localToc().xmlLevel();
    for (const SectionInfo *si : sectionRefs)
    {
      if (isSection(si->type()))
      {
        //printf("  level=%d title=%s\n",level,qPrint(si->title));
        int nextLevel = (int)si->type();
        if (nextLevel>level)
        {
          for (l=level;l<nextLevel;l++)
          {
            if (l < maxLevel) t << "    <tableofcontents>\n";
          }
        }
        else if (nextLevel<level)
        {
          for (l=level;l>nextLevel;l--)
          {
            if (l <= maxLevel && inLi[l]) t << "    </tocsect>\n";
            inLi[l]=FALSE;
            if (l <= maxLevel) t << "    </tableofcontents>\n";
          }
        }
        if (nextLevel <= maxLevel)
        {
          if (inLi[nextLevel]) t << "    </tocsect>\n";
          QCString titleDoc = convertToXML(si->title());
          t << "      <tocsect>\n";
          t << "        <name>" << (si->title().isEmpty()?si->label():titleDoc) << "</name>\n";
          t << "        <reference>"  <<  convertToXML(pageName) << "_1" << convertToXML(si->label()) << "</reference>\n";
          inLi[nextLevel]=TRUE;
          level = nextLevel;
        }
      }
    }
    while (level>1 && level <= maxLevel)
    {
      if (inLi[level]) t << "    </tocsect>\n";
      inLi[level]=FALSE;
      t << "    </tableofcontents>\n";
      level--;
    }
    if (level <= maxLevel && inLi[level]) t << "    </tocsect>\n";
    inLi[level]=FALSE;
    t << "    </tableofcontents>\n";
  }
  t << "    <briefdescription>\n";
  writeXMLDocBlock(t,pd->briefFile(),pd->briefLine(),pd,0,pd->briefDescription());
  t << "    </briefdescription>\n";
  t << "    <detaileddescription>\n";
  if (isExample)
  {
    writeXMLDocBlock(t,pd->docFile(),pd->docLine(),pd,0,
        pd->documentation()+"\n\\include "+pd->name());
  }
  else
  {
    writeXMLDocBlock(t,pd->docFile(),pd->docLine(),pd,0,
        pd->documentation());
  }
  t << "    </detaileddescription>\n";

  t << "    <location file=\"" << convertToXML(stripFromPath(pd->getDefFileName())) << "\"/>\n";

  t << "  </compounddef>\n";
  t << "</doxygen>\n";

  ti << "  </compound>\n";
}

void generateXML()
{
  // + classes
  // + concepts
  // + namespaces
  // + files
  // + groups
  // + related pages
  // - examples

  QCString outputDirectory = Config_getString(XML_OUTPUT);
  Dir xmlDir(outputDirectory.str());
  createSubDirs(xmlDir);

  ResourceMgr::instance().copyResource("xml.xsd",outputDirectory);
  ResourceMgr::instance().copyResource("index.xsd",outputDirectory);
  ResourceMgr::instance().copyResource("doxyfile.xsd",outputDirectory);

  QCString fileName=outputDirectory+"/compound.xsd";
  std::ofstream f(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }
  {
    TextStream t(&f);

    // write compound.xsd, but replace special marker with the entities
    QCString compound_xsd = ResourceMgr::instance().getAsString("compound.xsd");
    const char *startLine = compound_xsd.data();
    while (*startLine)
    {
      // find end of the line
      const char *endLine = startLine+1;
      while (*endLine && *(endLine-1)!='\n') endLine++; // skip to end of the line including \n
      int len=static_cast<int>(endLine-startLine);
      if (len>0)
      {
        QCString s(startLine,len);
        if (s.find("<!-- Automatically insert here the HTML entities -->")!=-1)
        {
          HtmlEntityMapper::instance()->writeXMLSchema(t);
        }
        else
        {
          t.write(startLine,len);
        }
      }
      startLine=endLine;
    }
  }
  f.close();

  fileName=outputDirectory+"/Doxyfile.xml";
  f.open(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing\n",fileName.data());
    return;
  }
  else
  {
    TextStream t(&f);
    Config::writeXMLDoxyfile(t);
  }
  f.close();

  fileName=outputDirectory+"/index.xml";
  f.open(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!f.is_open())
  {
    err("Cannot open file %s for writing!\n",qPrint(fileName));
    return;
  }
  else
  {
    TextStream t(&f);

    // write index header
    t << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n";;
    t << "<doxygenindex xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    t << "xsi:noNamespaceSchemaLocation=\"index.xsd\" ";
    t << "version=\"" << getDoxygenVersion() << "\" ";
    t << "xml:lang=\"" << theTranslator->trISOLang() << "\"";
    t << ">\n";

    for (const auto &cd : *Doxygen::classLinkedMap)
    {
      generateXMLForClass(cd.get(),t);
    }
    for (const auto &cd : *Doxygen::conceptLinkedMap)
    {
      msg("Generating XML output for concept %s\n",qPrint(cd->name()));
      generateXMLForConcept(cd.get(),t);
    }
    for (const auto &nd : *Doxygen::namespaceLinkedMap)
    {
      msg("Generating XML output for namespace %s\n",qPrint(nd->name()));
      generateXMLForNamespace(nd.get(),t);
    }
    for (const auto &fn : *Doxygen::inputNameLinkedMap)
    {
      for (const auto &fd : *fn)
      {
        msg("Generating XML output for file %s\n",qPrint(fd->name()));
        generateXMLForFile(fd.get(),t);
      }
    }
    for (const auto &gd : *Doxygen::groupLinkedMap)
    {
      msg("Generating XML output for group %s\n",qPrint(gd->name()));
      generateXMLForGroup(gd.get(),t);
    }
    for (const auto &pd : *Doxygen::pageLinkedMap)
    {
      msg("Generating XML output for page %s\n",qPrint(pd->name()));
      generateXMLForPage(pd.get(),t,FALSE);
    }
    for (const auto &dd : *Doxygen::dirLinkedMap)
    {
      msg("Generate XML output for dir %s\n",qPrint(dd->name()));
      generateXMLForDir(dd.get(),t);
    }
    for (const auto &pd : *Doxygen::exampleLinkedMap)
    {
      msg("Generating XML output for example %s\n",qPrint(pd->name()));
      generateXMLForPage(pd.get(),t,TRUE);
    }
    if (Doxygen::mainPage)
    {
      msg("Generating XML output for the main page\n");
      generateXMLForPage(Doxygen::mainPage.get(),t,FALSE);
    }

    //t << "  </compoundlist>\n";
    t << "</doxygenindex>\n";
  }

  writeCombineScript();
  clearSubDirs(xmlDir);
}


