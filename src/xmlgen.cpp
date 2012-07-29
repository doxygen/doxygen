/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
#include "xmlgen.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "classlist.h"
#include "util.h"
#include "defargs.h"
#include "outputgen.h"
#include "dot.h"
#include "pagedef.h"
#include "filename.h"
#include "version.h"
#include "xmldocvisitor.h"
#include "docparser.h"
#include "language.h"
#include "parserintf.h"
#include "arguments.h"

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qintdict.h>

// no debug info
#define XML_DB(x) do {} while(0)
// debug to stdout
//#define XML_DB(x) printf x
// debug inside output
//#define XML_DB(x) QCString __t;__t.sprintf x;m_t << __t

//------------------

static const char index_xsd[] =
#include "index_xsd.h"
;

//------------------
//
static const char compound_xsd[] =
#include "compound_xsd.h"
;

//------------------

/** Helper class mapping MemberList::ListType to a string representing */
class XmlSectionMapper : public QIntDict<char>
{
  public:
    XmlSectionMapper() : QIntDict<char>(47)
    {
      insert(MemberList::pubTypes,"public-type");
      insert(MemberList::pubMethods,"public-func");
      insert(MemberList::pubAttribs,"public-attrib");
      insert(MemberList::pubSlots,"public-slot");
      insert(MemberList::signals,"signal");
      insert(MemberList::dcopMethods,"dcop-func");
      insert(MemberList::properties,"property");
      insert(MemberList::events,"event");
      insert(MemberList::pubStaticMethods,"public-static-func");
      insert(MemberList::pubStaticAttribs,"public-static-attrib");
      insert(MemberList::proTypes,"protected-type");
      insert(MemberList::proMethods,"protected-func");
      insert(MemberList::proAttribs,"protected-attrib");
      insert(MemberList::proSlots,"protected-slot");
      insert(MemberList::proStaticMethods,"protected-static-func");
      insert(MemberList::proStaticAttribs,"protected-static-attrib");
      insert(MemberList::pacTypes,"package-type");
      insert(MemberList::pacMethods,"package-func");
      insert(MemberList::pacAttribs,"package-attrib");
      insert(MemberList::pacStaticMethods,"package-static-func");
      insert(MemberList::pacStaticAttribs,"package-static-attrib");
      insert(MemberList::priTypes,"private-type");
      insert(MemberList::priMethods,"private-func");
      insert(MemberList::priAttribs,"private-attrib");
      insert(MemberList::priSlots,"private-slot");
      insert(MemberList::priStaticMethods,"private-static-func");
      insert(MemberList::priStaticAttribs,"private-static-attrib");
      insert(MemberList::friends,"friend");
      insert(MemberList::related,"related");
      insert(MemberList::decDefineMembers,"define");
      insert(MemberList::decProtoMembers,"prototype");
      insert(MemberList::decTypedefMembers,"typedef");
      insert(MemberList::decEnumMembers,"enum");
      insert(MemberList::decFuncMembers,"func");
      insert(MemberList::decVarMembers,"var");
    }
};

static XmlSectionMapper g_xmlSectionMapper;


inline void writeXMLString(FTextStream &t,const char *s)
{
  t << convertToXML(s);
}

inline void writeXMLCodeString(FTextStream &t,const char *s, int &col)
{
  char c;
  while ((c=*s++))
  {
    switch(c)
    {
      case '\t': 
      { 
        static int tabSize = Config_getInt("TAB_SIZE");
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
      default:   t << c; col++;        break;         
    }
  } 
}


static void writeXMLHeader(FTextStream &t)
{
  t << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>" << endl;;
  t << "<doxygen xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
  t << "xsi:noNamespaceSchemaLocation=\"compound.xsd\" ";
  t << "version=\"" << versionString << "\">" << endl;
}

static void writeCombineScript()
{
  QCString outputDirectory = Config_getString("XML_OUTPUT");
  QCString fileName=outputDirectory+"/combine.xslt";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);

  t <<
  "<!-- XSLT script to combine the generated output into a single file. \n"
  "     If you have xsltproc you could use:\n"
  "     xsltproc combine.xslt index.xml >all.xml\n"
  "-->\n"
  "<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\">\n"
  "  <xsl:output method=\"xml\" version=\"1.0\" indent=\"no\" standalone=\"yes\" />\n"
  "  <xsl:template match=\"/\">\n"
  "    <doxygen version=\"{doxygenindex/@version}\">\n"
  "      <!-- Load all doxgen generated xml files -->\n"
  "      <xsl:for-each select=\"doxygenindex/compound\">\n"
  "        <xsl:copy-of select=\"document( concat( @refid, '.xml' ) )/doxygen/*\" />\n"
  "      </xsl:for-each>\n"
  "    </doxygen>\n"
  "  </xsl:template>\n"
  "</xsl:stylesheet>\n";

}

void writeXMLLink(FTextStream &t,const char *extRef,const char *compoundId,
                  const char *anchorId,const char *text,const char *tooltip)
{
  t << "<ref refid=\"" << compoundId;
  if (anchorId) t << "_1" << anchorId;
  t << "\" kindref=\"";
  if (anchorId) t << "member"; else t << "compound"; 
  t << "\"";
  if (extRef) t << " external=\"" << extRef << "\"";
  if (tooltip) t << " tooltip=\"" << convertToXML(tooltip) << "\"";
  t << ">";
  writeXMLString(t,text);
  t << "</ref>";
}

/** Implements TextGeneratorIntf for an XML stream. */
class TextGeneratorXMLImpl : public TextGeneratorIntf
{
  public:
    TextGeneratorXMLImpl(FTextStream &t): m_t(t) {}
    void writeString(const char *s,bool /*keepSpaces*/) const
    {
      writeXMLString(m_t,s); 
    }
    void writeBreak(int) const {}
    void writeLink(const char *extRef,const char *file,
                   const char *anchor,const char *text
                  ) const
    {
      writeXMLLink(m_t,extRef,file,anchor,text,0);
    }
  private:
    FTextStream &m_t;
};

/** Helper class representing a stack of objects stored by value */
template<class T> class ValStack
{
  public:
    ValStack() : m_values(10), m_sp(0), m_size(10) {}
    virtual ~ValStack() {}
    ValStack(const ValStack<T> &s)
    {
      m_values=s.m_values.copy();
      m_sp=s.m_sp;
      m_size=s.m_size;
    }
    ValStack &operator=(const ValStack<T> &s)
    {
      m_values=s.m_values.copy();
      m_sp=s.m_sp;
      m_size=s.m_size;
      return *this;
    }
    void push(T v)
    {
      m_sp++;
      if (m_sp>=m_size)
      {
        m_size+=10;
        m_values.resize(m_size);
      }
      m_values[m_sp]=v;
    }
    T pop()
    {
      ASSERT(m_sp!=0);
      return m_values[m_sp--];
    }
    T& top()
    {
      ASSERT(m_sp!=0);
      return m_values[m_sp];
    }
    bool isEmpty()
    {
      return m_sp==0;
    }
    uint count() const
    {
      return m_sp;
    }
    
  private:
    QArray<T> m_values;
    int m_sp;
    int m_size;
};

/** Generator for producing XML formatted source code. */
class XMLCodeGenerator : public CodeOutputInterface
{
  public:

    XMLCodeGenerator(FTextStream &t) : m_t(t), m_lineNumber(-1),
      m_insideCodeLine(FALSE), m_normalHLNeedStartTag(TRUE), 
      m_insideSpecialHL(FALSE) {}
    virtual ~XMLCodeGenerator() { }
    
    void codify(const char *text) 
    {
      XML_DB(("(codify \"%s\")\n",text));
      if (m_insideCodeLine && !m_insideSpecialHL && m_normalHLNeedStartTag)
      {
        m_t << "<highlight class=\"normal\">";
        m_normalHLNeedStartTag=FALSE;
      }
      writeXMLCodeString(m_t,text,col);
    }
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip) 
    {
      XML_DB(("(writeCodeLink)\n"));
      if (m_insideCodeLine && !m_insideSpecialHL && m_normalHLNeedStartTag)
      {
        m_t << "<highlight class=\"normal\">";
        m_normalHLNeedStartTag=FALSE;
      }
      writeXMLLink(m_t,ref,file,anchor,name,tooltip);
      col+=strlen(name);
    }
    void startCodeLine(bool) 
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
      col=0;
    }
    void endCodeLine() 
    {
      XML_DB(("(endCodeLine)\n"));
      if (!m_insideSpecialHL && !m_normalHLNeedStartTag)
      {
        m_t << "</highlight>";
        m_normalHLNeedStartTag=TRUE;
      }
      m_t << "</codeline>" << endl; // non DocBook
      m_lineNumber = -1;
      m_refId.resize(0);
      m_external.resize(0);
      m_insideCodeLine=FALSE;
    }
    void startCodeAnchor(const char *id) 
    {
      XML_DB(("(startCodeAnchor)\n"));
      if (m_insideCodeLine && !m_insideSpecialHL && m_normalHLNeedStartTag)
      {
        m_t << "<highlight class=\"normal\">";
        m_normalHLNeedStartTag=FALSE;
      }
      m_t << "<anchor id=\"" << id << "\">";
    }
    void endCodeAnchor() 
    {
      XML_DB(("(endCodeAnchor)\n"));
      m_t << "</anchor>";
    }
    void startFontClass(const char *colorClass) 
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
    void endFontClass()
    {
      XML_DB(("(endFontClass)\n"));
      m_t << "</highlight>"; // non DocBook
      m_insideSpecialHL=FALSE;
    }
    void writeCodeAnchor(const char *)
    {
      XML_DB(("(writeCodeAnchor)\n"));
    }
    void writeLineNumber(const char *extRef,const char *compId,
                         const char *anchorId,int l)
    {
      XML_DB(("(writeLineNumber)\n"));
      // we remember the information provided here to use it 
      // at the <codeline> start tag.
      m_lineNumber = l;
      if (compId)
      {
        m_refId=compId;
        if (anchorId) m_refId+=(QCString)"_1"+anchorId;
        m_isMemberRef = anchorId!=0;
        if (extRef) m_external=extRef;
      }
    }
    void linkableSymbol(int, const char *,Definition *,Definition *) 
    {
    }

    void finish()
    {
      if (m_insideCodeLine) endCodeLine();
    }

  private:
    FTextStream &m_t;  
    QCString m_refId;
    QCString m_external;
    int m_lineNumber;
    bool m_isMemberRef;
    int col;

    bool m_insideCodeLine;
    bool m_normalHLNeedStartTag;
    bool m_insideSpecialHL;
};


static void writeTemplateArgumentList(ArgumentList *al,
                                      FTextStream &t,
                                      Definition *scope,
                                      FileDef *fileScope,
                                      int indent)
{
  QCString indentStr;
  indentStr.fill(' ',indent);
  if (al)
  {
    t << indentStr << "<templateparamlist>" << endl;
    ArgumentListIterator ali(*al);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      t << indentStr << "  <param>" << endl;
      if (!a->type.isEmpty())
      {
        t << indentStr <<  "    <type>";
        linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a->type);
        t << "</type>" << endl;
      }
      if (!a->name.isEmpty())
      {
        t << indentStr <<  "    <declname>" << a->name << "</declname>" << endl;
        t << indentStr <<  "    <defname>" << a->name << "</defname>" << endl;
      }
      if (!a->defval.isEmpty())
      {
        t << indentStr << "    <defval>";
        linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a->defval);
        t << "</defval>" << endl;
      }
      t << indentStr << "  </param>" << endl;
    }
    t << indentStr << "</templateparamlist>" << endl;
  }
}

static void writeMemberTemplateLists(MemberDef *md,FTextStream &t)
{
  LockingPtr<ArgumentList> templMd = md->templateArguments();
  if (templMd!=0) // function template prefix
  {
    writeTemplateArgumentList(templMd.pointer(),t,md->getClassDef(),md->getFileDef(),8);
  }
}

static void writeTemplateList(ClassDef *cd,FTextStream &t)
{
  writeTemplateArgumentList(cd->templateArguments(),t,cd,0,4);
}

static void writeXMLDocBlock(FTextStream &t,
                      const QCString &fileName,
                      int lineNr,
                      Definition *scope,
                      MemberDef * md,
                      const QCString &text)
{
  QCString stext = text.stripWhiteSpace();
  if (stext.isEmpty()) return;
  // convert the documentation string into an abstract syntax tree
  DocNode *root = validatingParseDoc(fileName,lineNr,scope,md,text+"\n",FALSE,FALSE);
  // create a code generator
  XMLCodeGenerator *xmlCodeGen = new XMLCodeGenerator(t);
  // create a parse tree visitor for XML
  XmlDocVisitor *visitor = new XmlDocVisitor(t,*xmlCodeGen);
  // visit all nodes
  root->accept(visitor);
  // clean up
  delete visitor;
  delete xmlCodeGen;
  delete root;
  
}

void writeXMLCodeBlock(FTextStream &t,FileDef *fd)
{
  ParserInterface *pIntf=Doxygen::parserManager->getParser(fd->getDefFileExtension());
  pIntf->resetCodeParserState();
  XMLCodeGenerator *xmlGen = new XMLCodeGenerator(t);
  pIntf->parseCode(*xmlGen,  // codeOutIntf
                0,           // scopeName
                fileToString(fd->absFilePath(),Config_getBool("FILTER_SOURCE_FILES")),
                FALSE,       // isExampleBlock
                0,           // exampleName
                fd,          // fileDef
                -1,          // startLine
                -1,          // endLine
                FALSE,       // inlineFragement
                0,           // memberDef
                TRUE         // showLineNumbers
                );
  xmlGen->finish();
  delete xmlGen;
}

static void writeMemberReference(FTextStream &t,Definition *def,MemberDef *rmd,const char *tagName)
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
  t << ">" << convertToXML(name) << "</" << tagName << ">" << endl;
  
}

static void stripQualifiers(QCString &typeStr)
{
  bool done=FALSE;
  while (!done)
  {
    if (typeStr.stripPrefix("static "));
    else if (typeStr.stripPrefix("virtual "));
    else if (typeStr.stripPrefix("volatile "));
    else if (typeStr=="virtual") typeStr="";
    else done=TRUE;
  }
}

static QCString classOutputFileBase(ClassDef *cd)
{
  //static bool inlineGroupedClasses = Config_getBool("INLINE_GROUPED_CLASSES");
  //if (inlineGroupedClasses && cd->partOfGroups()!=0) 
  return cd->getOutputFileBase();
  //else 
  //  return cd->getOutputFileBase();
}

static QCString memberOutputFileBase(MemberDef *md)
{
  //static bool inlineGroupedClasses = Config_getBool("INLINE_GROUPED_CLASSES");
  //if (inlineGroupedClasses && md->getClassDef() && md->getClassDef()->partOfGroups()!=0) 
  //  return md->getClassDef()->getXmlOutputFileBase();
  //else 
  //  return md->getOutputFileBase();
  return md->getOutputFileBase();
}


static void generateXMLForMember(MemberDef *md,FTextStream &ti,FTextStream &t,Definition *def)
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
  if (md->memberType()==MemberDef::EnumValue) return;
  if (md->isHidden()) return;
  //if (md->name().at(0)=='@') return; // anonymous member

  // group members are only visible in their group
  //if (def->definitionType()!=Definition::TypeGroup && md->getGroupDef()) return;

  QCString memType;
  bool isFunc=FALSE;
  switch (md->memberType())
  {
    case MemberDef::Define:      memType="define";    break;
    case MemberDef::EnumValue:   ASSERT(0);           break;
    case MemberDef::Property:    memType="property";  break;
    case MemberDef::Event:       memType="event";     break;
    case MemberDef::Variable:    memType="variable";  break;
    case MemberDef::Typedef:     memType="typedef";   break;
    case MemberDef::Enumeration: memType="enum";      break;
    case MemberDef::Function:    memType="function";  isFunc=TRUE; break;
    case MemberDef::Signal:      memType="signal";    isFunc=TRUE; break;
    case MemberDef::Friend:      memType="friend";    isFunc=TRUE; break;
    case MemberDef::DCOP:        memType="dcop";      isFunc=TRUE; break;
    case MemberDef::Slot:        memType="slot";      isFunc=TRUE; break;
  }

  ti << "    <member refid=\"" << memberOutputFileBase(md) 
     << "_1" << md->anchor() << "\" kind=\"" << memType << "\"><name>" 
     << convertToXML(md->name()) << "</name></member>" << endl;
  
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
  t << "_1"      // encoded `:' character (see util.cpp:convertNameToFile)
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

  if (isFunc)
  {
    LockingPtr<ArgumentList> al = md->argumentList();
    t << " const=\"";
    if (al!=0 && al->constSpecifier)    t << "yes"; else t << "no"; 
    t << "\"";

    t << " explicit=\"";
    if (md->isExplicit()) t << "yes"; else t << "no";
    t << "\"";

    t << " inline=\"";
    if (md->isInline()) t << "yes"; else t << "no";
    t << "\"";

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

  if (md->memberType() == MemberDef::Variable)
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
    
  }
  else if (md->memberType() == MemberDef::Property)
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

    t << " settable=\"";
    if (md->isSettable()) t << "yes"; else t << "no";
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
  else if (md->memberType() == MemberDef::Event)
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

  t << ">" << endl;

  if (md->memberType()!=MemberDef::Define &&
      md->memberType()!=MemberDef::Enumeration
     )
  {
    if (md->memberType()!=MemberDef::Typedef)
    {
      writeMemberTemplateLists(md,t);
    }
    QCString typeStr = md->typeString(); //replaceAnonymousScopes(md->typeString());
    stripQualifiers(typeStr);
    t << "        <type>";
    linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,typeStr);
    t << "</type>" << endl;
    t << "        <definition>" << convertToXML(md->definition()) << "</definition>" << endl;
    t << "        <argsstring>" << convertToXML(md->argsString()) << "</argsstring>" << endl;
  }

  t << "        <name>" << convertToXML(md->name()) << "</name>" << endl;
  
  if (md->memberType() == MemberDef::Property)
  {
    if (md->isReadable())
      t << "        <read>" << convertToXML(md->getReadAccessor()) << "</read>" << endl;
    if (md->isWritable())
      t << "        <write>" << convertToXML(md->getWriteAccessor()) << "</write>" << endl;
  }
  if (md->memberType()==MemberDef::Variable && md->bitfieldString())
  {
    QCString bitfield = md->bitfieldString();
    if (bitfield.at(0)==':') bitfield=bitfield.mid(1);
    t << "        <bitfield>" << bitfield << "</bitfield>" << endl;
  }
  
  MemberDef *rmd = md->reimplements();
  if (rmd)
  {
    t << "        <reimplements refid=\"" 
      << memberOutputFileBase(rmd) << "_1" << rmd->anchor() << "\">"
      << convertToXML(rmd->name()) << "</reimplements>" << endl;
  }
  LockingPtr<MemberList> rbml = md->reimplementedBy();
  if (rbml!=0)
  {
    MemberListIterator mli(*rbml);
    for (mli.toFirst();(rmd=mli.current());++mli)
    {
      t << "        <reimplementedby refid=\"" 
        << memberOutputFileBase(rmd) << "_1" << rmd->anchor() << "\">"
        << convertToXML(rmd->name()) << "</reimplementedby>" << endl;
    }
  }

  if (isFunc) //function
  {
    LockingPtr<ArgumentList> declAl = md->declArgumentList();
    LockingPtr<ArgumentList> defAl = md->argumentList();
    if (declAl!=0 && declAl->count()>0)
    {
      ArgumentListIterator declAli(*declAl);
      ArgumentListIterator defAli(*defAl);
      Argument *a;
      for (declAli.toFirst();(a=declAli.current());++declAli)
      {
        Argument *defArg = defAli.current();
        t << "        <param>" << endl;
        if (!a->attrib.isEmpty())
        {
          t << "          <attributes>";
          writeXMLString(t,a->attrib);
          t << "</attributes>" << endl;
        }
        if (!a->type.isEmpty())
        {
          t << "          <type>";
          linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,a->type);
          t << "</type>" << endl;
        }
        if (!a->name.isEmpty())
        {
          t << "          <declname>";
          writeXMLString(t,a->name); 
          t << "</declname>" << endl;
        }
        if (defArg && !defArg->name.isEmpty() && defArg->name!=a->name)
        {
          t << "          <defname>";
          writeXMLString(t,defArg->name);
          t << "</defname>" << endl;
        }
        if (!a->array.isEmpty())
        {
          t << "          <array>"; 
          writeXMLString(t,a->array); 
          t << "</array>" << endl;
        }
        if (!a->defval.isEmpty())
        {
          t << "          <defval>";
          linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,a->defval);
          t << "</defval>" << endl;
        }
        if (defArg && defArg->hasDocumentation())
        {
          t << "          <briefdescription>";
          writeXMLDocBlock(t,md->getDefFileName(),md->getDefLine(),
                           md->getOuterScope(),md,defArg->docs);
          t << "</briefdescription>" << endl;
        }
        t << "        </param>" << endl;
        if (defArg) ++defAli;
      }
    }
  }
  else if (md->memberType()==MemberDef::Define && 
          md->argsString()) // define
  {
    if (md->argumentList()->count()==0) // special case for "foo()" to
                                        // disguish it from "foo".
    {
      t << "        <param></param>" << endl;
    }
    else
    {
      ArgumentListIterator ali(*md->argumentList());
      Argument *a;
      for (ali.toFirst();(a=ali.current());++ali)
      {
        t << "        <param><defname>" << a->type << "</defname></param>" << endl;
      }
    }
  }
  // avoid that extremely large tables are written to the output. 
  // todo: it's better to adhere to MAX_INITIALIZER_LINES.
  if (!md->initializer().isEmpty() && md->initializer().length()<2000)
  {
    t << "        <initializer>";
    linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,md->initializer());
    t << "</initializer>" << endl;
  }

  if (md->excpString())
  {
    t << "        <exceptions>";
    linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,md->excpString());
    t << "</exceptions>" << endl;
  }
  
  if (md->memberType()==MemberDef::Enumeration) // enum
  {
    LockingPtr<MemberList> enumFields = md->enumFieldList();
    if (enumFields!=0)
    {
      MemberListIterator emli(*enumFields);
      MemberDef *emd;
      for (emli.toFirst();(emd=emli.current());++emli)
      {
        ti << "    <member refid=\"" << memberOutputFileBase(emd) 
           << "_1" << emd->anchor() << "\" kind=\"enumvalue\"><name>" 
           << convertToXML(emd->name()) << "</name></member>" << endl;

        t << "        <enumvalue id=\"" << memberOutputFileBase(emd) << "_1" 
          << emd->anchor() << "\" prot=\"";
        switch (emd->protection())
        {
          case Public:    t << "public";    break;
          case Protected: t << "protected"; break;
          case Private:   t << "private";   break;
          case Package:   t << "package";   break;
        }
        t << "\">" << endl;
        t << "          <name>";
        writeXMLString(t,emd->name());
        t << "</name>" << endl;
        if (!emd->initializer().isEmpty())
        {
          t << "          <initializer>";
          writeXMLString(t,emd->initializer());
          t << "</initializer>" << endl;
        }
        t << "          <briefdescription>" << endl;
        writeXMLDocBlock(t,emd->briefFile(),emd->briefLine(),emd->getOuterScope(),emd,emd->briefDescription());
        t << "          </briefdescription>" << endl;
        t << "          <detaileddescription>" << endl;
        writeXMLDocBlock(t,emd->docFile(),emd->docLine(),emd->getOuterScope(),emd,emd->documentation());
        t << "          </detaileddescription>" << endl;
        t << "        </enumvalue>" << endl;
      }
    }
  }
  t << "        <briefdescription>" << endl;
  writeXMLDocBlock(t,md->briefFile(),md->briefLine(),md->getOuterScope(),md,md->briefDescription());
  t << "        </briefdescription>" << endl;
  t << "        <detaileddescription>" << endl;
  writeXMLDocBlock(t,md->docFile(),md->docLine(),md->getOuterScope(),md,md->documentation());
  t << "        </detaileddescription>" << endl;
  t << "        <inbodydescription>" << endl;
  writeXMLDocBlock(t,md->docFile(),md->inbodyLine(),md->getOuterScope(),md,md->inbodyDocumentation());
  t << "        </inbodydescription>" << endl;
  if (md->getDefLine()!=-1)
  {
    t << "        <location file=\"" 
      << md->getDefFileName() << "\" line=\"" 
      << md->getDefLine() << "\"";
    if (md->getStartBodyLine()!=-1)
    {
      FileDef *bodyDef = md->getBodyDef();
      if (bodyDef)
      {
        t << " bodyfile=\"" << bodyDef->absFilePath() << "\"";
      }
      t << " bodystart=\"" << md->getStartBodyLine() << "\" bodyend=\"" 
        << md->getEndBodyLine() << "\"";
    }
    t << "/>" << endl;
  }

  //printf("md->getReferencesMembers()=%p\n",md->getReferencesMembers());
  LockingPtr<MemberSDict> mdict = md->getReferencesMembers();
  if (mdict!=0)
  {
    MemberSDict::Iterator mdi(*mdict);
    MemberDef *rmd;
    for (mdi.toFirst();(rmd=mdi.current());++mdi)
    {
      writeMemberReference(t,def,rmd,"references");
    }
  }
  mdict = md->getReferencedByMembers();
  if (mdict!=0)
  {
    MemberSDict::Iterator mdi(*mdict);
    MemberDef *rmd;
    for (mdi.toFirst();(rmd=mdi.current());++mdi)
    {
      writeMemberReference(t,def,rmd,"referencedby");
    }
  }
  
  t << "      </memberdef>" << endl;
}

static void generateXMLSection(Definition *d,FTextStream &ti,FTextStream &t,
                      MemberList *ml,const char *kind,const char *header=0,
                      const char *documentation=0)
{
  if (ml==0) return;
  MemberListIterator mli(*ml);
  MemberDef *md;
  int count=0;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    // namespace members are also inserted in the file scope, but 
    // to prevent this duplication in the XML output, we filter those here.
    if (d->definitionType()!=Definition::TypeFile || md->getNamespaceDef()==0)
    {
      count++;
    }
  }
  if (count==0) return; // empty list

  t << "      <sectiondef kind=\"" << kind << "\">" << endl;
  if (header)
  {
    t << "      <header>" << convertToXML(header) << "</header>" << endl;
  }
  if (documentation)
  {
    t << "      <description>";
    writeXMLDocBlock(t,d->docFile(),d->docLine(),d,0,documentation);
    t << "</description>" << endl;
  }
  for (mli.toFirst();(md=mli.current());++mli)
  {
    // namespace members are also inserted in the file scope, but 
    // to prevent this duplication in the XML output, we filter those here.
    if (d->definitionType()!=Definition::TypeFile || md->getNamespaceDef()==0)
    {
      generateXMLForMember(md,ti,t,d);
    }
  }
  t << "      </sectiondef>" << endl;
}

static void writeListOfAllMembers(ClassDef *cd,FTextStream &t)
{
  t << "    <listofallmembers>" << endl;
  if (cd->memberNameInfoSDict())
  {
    MemberNameInfoSDict::Iterator mnii(*cd->memberNameInfoSDict());
    MemberNameInfo *mni;
    for (mnii.toFirst();(mni=mnii.current());++mnii)
    {
      MemberNameInfoIterator mii(*mni);
      MemberInfo *mi;
      for (mii.toFirst();(mi=mii.current());++mii)
      {
        MemberDef *md=mi->memberDef;
        if (md->name().at(0)!='@') // skip anonymous members
        {
          Protection prot = mi->prot;
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
          if (!mi->ambiguityResolutionScope.isEmpty())
          {
            t << " ambiguityscope=\"" << convertToXML(mi->ambiguityResolutionScope) << "\"";
          }
          t << "><scope>" << convertToXML(cd->name()) << "</scope><name>" << 
            convertToXML(md->name()) << "</name></member>" << endl;
        }
      }
    }
  }
  t << "    </listofallmembers>" << endl;
}

static void writeInnerClasses(const ClassSDict *cl,FTextStream &t)
{
  if (cl)
  {
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      if (!cd->isHidden() && cd->name().find('@')==-1) // skip anonymous scopes
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
        t << "\">" << convertToXML(cd->name()) << "</innerclass>" << endl;
      }
    }
  }
}

static void writeInnerNamespaces(const NamespaceSDict *nl,FTextStream &t)
{
  if (nl)
  {
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
    {
      if (!nd->isHidden() && nd->name().find('@')==-1) // skip anonymouse scopes
      {
        t << "    <innernamespace refid=\"" << nd->getOutputFileBase()
          << "\">" << convertToXML(nd->name()) << "</innernamespace>" << endl;
      }
    }
  }
}

static void writeInnerFiles(const FileList *fl,FTextStream &t)
{
  if (fl)
  {
    QListIterator<FileDef> fli(*fl);
    FileDef *fd;
    for (fli.toFirst();(fd=fli.current());++fli)
    {
      t << "    <innerfile refid=\"" << fd->getOutputFileBase() 
        << "\">" << convertToXML(fd->name()) << "</innerfile>" << endl;
    }
  }
}

static void writeInnerPages(const PageSDict *pl,FTextStream &t)
{
  if (pl)
  {
    PageSDict::Iterator pli(*pl);
    PageDef *pd;
    for (pli.toFirst();(pd=pli.current());++pli)
    {
      t << "    <innerpage refid=\"" << pd->getOutputFileBase();
      if (pd->getGroupDef())
      {
        t << "_" << pd->name();
      }
      t << "\">" << convertToXML(pd->title()) << "</innerpage>" << endl;
    }
  }
}

static void writeInnerGroups(const GroupList *gl,FTextStream &t)
{
  if (gl)
  {
    GroupListIterator gli(*gl);
    GroupDef *sgd;
    for (gli.toFirst();(sgd=gli.current());++gli)
    {
      t << "    <innergroup refid=\"" << sgd->getOutputFileBase()
        << "\">" << convertToXML(sgd->groupTitle()) 
        << "</innergroup>" << endl;
    }
  }
}

static void writeInnerDirs(const DirList *dl,FTextStream &t)
{
  if (dl)
  {
    QListIterator<DirDef> subdirs(*dl);
    DirDef *subdir;
    for (subdirs.toFirst();(subdir=subdirs.current());++subdirs)
    {
      t << "    <innerdir refid=\"" << subdir->getOutputFileBase() 
        << "\">" << convertToXML(subdir->displayName()) << "</innerdir>" << endl;
    }
  }
}
  
static void generateXMLForClass(ClassDef *cd,FTextStream &ti)
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
  if (cd->name().find('@')!=-1) return; // skip anonymous compounds.
  if (cd->templateMaster()!=0)  return; // skip generated template instances.
  if (cd->isArtificial())       return; // skip artificially created classes

  msg("Generating XML output for class %s\n",cd->name().data());

  ti << "  <compound refid=\"" << classOutputFileBase(cd) 
     << "\" kind=\"" << cd->compoundTypeString()
     << "\"><name>" << convertToXML(cd->name()) << "</name>" << endl;
  
  QCString outputDirectory = Config_getString("XML_OUTPUT");
  QCString fileName=outputDirectory+"/"+ classOutputFileBase(cd)+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);

  writeXMLHeader(t);
  t << "  <compounddef id=\"" 
    << classOutputFileBase(cd) << "\" kind=\"" 
    << cd->compoundTypeString() << "\" prot=\"";
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
  t << "\">" << endl;
  t << "    <compoundname>"; 
  writeXMLString(t,cd->name()); 
  t << "</compoundname>" << endl;
  if (cd->baseClasses())
  {
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "    <basecompoundref ";
      if (bcd->classDef->isLinkable())
      {
        t << "refid=\"" << classOutputFileBase(bcd->classDef) << "\" ";
      }
      t << "prot=\"";
      switch (bcd->prot)
      {
        case Public:    t << "public";    break;
        case Protected: t << "protected"; break;
        case Private:   t << "private";   break;
        case Package: ASSERT(0); break;
      }
      t << "\" virt=\"";
      switch(bcd->virt)
      {
        case Normal:  t << "non-virtual";  break;
        case Virtual: t << "virtual";      break;
        case Pure:    t <<"pure-virtual"; break;
      }
      t << "\">";
      if (!bcd->templSpecifiers.isEmpty())
      {
        t << convertToXML(
              insertTemplateSpecifierInScope(
              bcd->classDef->name(),bcd->templSpecifiers)
           );
      }
      else
      {
        t << convertToXML(bcd->classDef->displayName());
      }
      t  << "</basecompoundref>" << endl;
    }
  }
  if (cd->subClasses())
  {
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "    <derivedcompoundref refid=\"" 
        << classOutputFileBase(bcd->classDef)
        << "\" prot=\"";
      switch (bcd->prot)
      {
        case Public:    t << "public";    break;
        case Protected: t << "protected"; break;
        case Private:   t << "private";   break;
        case Package: ASSERT(0); break;
      }
      t << "\" virt=\"";
      switch(bcd->virt)
      {
        case Normal:  t << "non-virtual";  break;
        case Virtual: t << "virtual";      break;
        case Pure:    t << "pure-virtual"; break;
      }
      t << "\">" << convertToXML(bcd->classDef->displayName()) 
        << "</derivedcompoundref>" << endl;
    }
  }

  IncludeInfo *ii=cd->includeInfo();
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
      t << "</includes>" << endl;
    }
  }

  writeInnerClasses(cd->getClassSDict(),t);

  writeTemplateList(cd,t);
  if (cd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*cd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      generateXMLSection(cd,ti,t,mg->members(),"user-defined",mg->header(),
          mg->documentation());
    }
  }

  QListIterator<MemberList> mli(cd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberList::detailedLists)==0)
    {
      generateXMLSection(cd,ti,t,ml,g_xmlSectionMapper.find(ml->listType()));
    }
  }
#if 0
  generateXMLSection(cd,ti,t,cd->pubTypes,"public-type");
  generateXMLSection(cd,ti,t,cd->pubMethods,"public-func");
  generateXMLSection(cd,ti,t,cd->pubAttribs,"public-attrib");
  generateXMLSection(cd,ti,t,cd->pubSlots,"public-slot");
  generateXMLSection(cd,ti,t,cd->signals,"signal");
  generateXMLSection(cd,ti,t,cd->dcopMethods,"dcop-func");
  generateXMLSection(cd,ti,t,cd->properties,"property");
  generateXMLSection(cd,ti,t,cd->events,"event");
  generateXMLSection(cd,ti,t,cd->pubStaticMethods,"public-static-func");
  generateXMLSection(cd,ti,t,cd->pubStaticAttribs,"public-static-attrib");
  generateXMLSection(cd,ti,t,cd->proTypes,"protected-type");
  generateXMLSection(cd,ti,t,cd->proMethods,"protected-func");
  generateXMLSection(cd,ti,t,cd->proAttribs,"protected-attrib");
  generateXMLSection(cd,ti,t,cd->proSlots,"protected-slot");
  generateXMLSection(cd,ti,t,cd->proStaticMethods,"protected-static-func");
  generateXMLSection(cd,ti,t,cd->proStaticAttribs,"protected-static-attrib");
  generateXMLSection(cd,ti,t,cd->pacTypes,"package-type");
  generateXMLSection(cd,ti,t,cd->pacMethods,"package-func");
  generateXMLSection(cd,ti,t,cd->pacAttribs,"package-attrib");
  generateXMLSection(cd,ti,t,cd->pacStaticMethods,"package-static-func");
  generateXMLSection(cd,ti,t,cd->pacStaticAttribs,"package-static-attrib");
  generateXMLSection(cd,ti,t,cd->priTypes,"private-type");
  generateXMLSection(cd,ti,t,cd->priMethods,"private-func");
  generateXMLSection(cd,ti,t,cd->priAttribs,"private-attrib");
  generateXMLSection(cd,ti,t,cd->priSlots,"private-slot");
  generateXMLSection(cd,ti,t,cd->priStaticMethods,"private-static-func");
  generateXMLSection(cd,ti,t,cd->priStaticAttribs,"private-static-attrib");
  generateXMLSection(cd,ti,t,cd->friends,"friend");
  generateXMLSection(cd,ti,t,cd->related,"related");
#endif

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,cd->briefFile(),cd->briefLine(),cd,0,cd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,cd->docFile(),cd->docLine(),cd,0,cd->documentation());
  t << "    </detaileddescription>" << endl;
  DotClassGraph inheritanceGraph(cd,DotNode::Inheritance);
  if (!inheritanceGraph.isTrivial())
  {
    t << "    <inheritancegraph>" << endl;
    inheritanceGraph.writeXML(t);
    t << "    </inheritancegraph>" << endl;
  }
  DotClassGraph collaborationGraph(cd,DotNode::Collaboration);
  if (!collaborationGraph.isTrivial())
  {
    t << "    <collaborationgraph>" << endl;
    collaborationGraph.writeXML(t);
    t << "    </collaborationgraph>" << endl;
  }
  t << "    <location file=\"" 
    << cd->getDefFileName() << "\" line=\"" 
    << cd->getDefLine() << "\"";
    if (cd->getStartBodyLine()!=-1)
    {
      FileDef *bodyDef = cd->getBodyDef();
      if (bodyDef)
      {
        t << " bodyfile=\"" << bodyDef->absFilePath() << "\"";
      }
      t << " bodystart=\"" << cd->getStartBodyLine() << "\" bodyend=\"" 
        << cd->getEndBodyLine() << "\"";
    }
  t << "/>" << endl;
  writeListOfAllMembers(cd,t);
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

static void generateXMLForNamespace(NamespaceDef *nd,FTextStream &ti)
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
     << convertToXML(nd->name()) << "</name>" << endl;
  
  QCString outputDirectory = Config_getString("XML_OUTPUT");
  QCString fileName=outputDirectory+"/"+nd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);
  
  writeXMLHeader(t);
  t << "  <compounddef id=\"" 
    << nd->getOutputFileBase() << "\" kind=\"namespace\">" << endl;
  t << "    <compoundname>";
  writeXMLString(t,nd->name());
  t << "</compoundname>" << endl;

  writeInnerClasses(nd->getClassSDict(),t);
  writeInnerNamespaces(nd->getNamespaceSDict(),t);

  if (nd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*nd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      generateXMLSection(nd,ti,t,mg->members(),"user-defined",mg->header(),
          mg->documentation());
    }
  }

  QListIterator<MemberList> mli(nd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberList::declarationLists)!=0)
    {
      generateXMLSection(nd,ti,t,ml,g_xmlSectionMapper.find(ml->listType()));
    }
  }
#if 0
  generateXMLSection(nd,ti,t,&nd->decDefineMembers,"define");
  generateXMLSection(nd,ti,t,&nd->decProtoMembers,"prototype");
  generateXMLSection(nd,ti,t,&nd->decTypedefMembers,"typedef");
  generateXMLSection(nd,ti,t,&nd->decEnumMembers,"enum");
  generateXMLSection(nd,ti,t,&nd->decFuncMembers,"func");
  generateXMLSection(nd,ti,t,&nd->decVarMembers,"var");
#endif

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,nd->briefFile(),nd->briefLine(),nd,0,nd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,nd->docFile(),nd->docLine(),nd,0,nd->documentation());
  t << "    </detaileddescription>" << endl;
  t << "    <location file=\"" 
    << nd->getDefFileName() << "\" line=\"" 
    << nd->getDefLine() << "\"/>" << endl;
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

static void generateXMLForFile(FileDef *fd,FTextStream &ti)
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
     << "</name>" << endl;
  
  QCString outputDirectory = Config_getString("XML_OUTPUT");
  QCString fileName=outputDirectory+"/"+fd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);

  writeXMLHeader(t);
  t << "  <compounddef id=\"" 
    << fd->getOutputFileBase() << "\" kind=\"file\">" << endl;
  t << "    <compoundname>";
  writeXMLString(t,fd->name());
  t << "</compoundname>" << endl;

  IncludeInfo *inc;

  if (fd->includeFileList())
  {
    QListIterator<IncludeInfo> ili1(*fd->includeFileList());
    for (ili1.toFirst();(inc=ili1.current());++ili1)
    {
      t << "    <includes";
      if (inc->fileDef && !inc->fileDef->isReference()) // TODO: support external references
      {
        t << " refid=\"" << inc->fileDef->getOutputFileBase() << "\"";
      }
      t << " local=\"" << (inc->local ? "yes" : "no") << "\">";
      t << inc->includeName;
      t << "</includes>" << endl;
    }
  }

  if (fd->includedByFileList())
  {
    QListIterator<IncludeInfo> ili2(*fd->includedByFileList());
    for (ili2.toFirst();(inc=ili2.current());++ili2)
    {
      t << "    <includedby";
      if (inc->fileDef && !inc->fileDef->isReference()) // TODO: support external references
      {
        t << " refid=\"" << inc->fileDef->getOutputFileBase() << "\"";
      }
      t << " local=\"" << (inc->local ? "yes" : "no") << "\">";
      t << inc->includeName;
      t << "</includedby>" << endl;
    }
  }

  DotInclDepGraph incDepGraph(fd,FALSE);
  if (!incDepGraph.isTrivial())
  {
    t << "    <incdepgraph>" << endl;
    incDepGraph.writeXML(t);
    t << "    </incdepgraph>" << endl;
  }

  DotInclDepGraph invIncDepGraph(fd,TRUE);
  if (!invIncDepGraph.isTrivial())
  {
    t << "    <invincdepgraph>" << endl;
    invIncDepGraph.writeXML(t);
    t << "    </invincdepgraph>" << endl;
  }

  if (fd->getClassSDict())
  {
    writeInnerClasses(fd->getClassSDict(),t);
  }
  if (fd->getNamespaceSDict())
  {
    writeInnerNamespaces(fd->getNamespaceSDict(),t);
  }

  if (fd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*fd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      generateXMLSection(fd,ti,t,mg->members(),"user-defined",mg->header(),
          mg->documentation());
    }
  }

  QListIterator<MemberList> mli(fd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberList::declarationLists)!=0)
    {
      generateXMLSection(fd,ti,t,ml,g_xmlSectionMapper.find(ml->listType()));
    }
  }
#if 0
  generateXMLSection(fd,ti,t,fd->decDefineMembers,"define");
  generateXMLSection(fd,ti,t,fd->decProtoMembers,"prototype");
  generateXMLSection(fd,ti,t,fd->decTypedefMembers,"typedef");
  generateXMLSection(fd,ti,t,fd->decEnumMembers,"enum");
  generateXMLSection(fd,ti,t,fd->decFuncMembers,"func");
  generateXMLSection(fd,ti,t,fd->decVarMembers,"var");
#endif

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,fd->briefFile(),fd->briefLine(),fd,0,fd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,fd->docFile(),fd->docLine(),fd,0,fd->documentation());
  t << "    </detaileddescription>" << endl;
  if (Config_getBool("XML_PROGRAMLISTING"))
  {
    t << "    <programlisting>" << endl;
    writeXMLCodeBlock(t,fd);
    t << "    </programlisting>" << endl;
  }
  t << "    <location file=\"" << fd->getDefFileName() << "\"/>" << endl;
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

static void generateXMLForGroup(GroupDef *gd,FTextStream &ti)
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
     << "\" kind=\"group\"><name>" << convertToXML(gd->name()) << "</name>" << endl;
  
  QCString outputDirectory = Config_getString("XML_OUTPUT");
  QCString fileName=outputDirectory+"/"+gd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }

  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);
  writeXMLHeader(t);
  t << "  <compounddef id=\"" 
    << gd->getOutputFileBase() << "\" kind=\"group\">" << endl;
  t << "    <compoundname>" << convertToXML(gd->name()) << "</compoundname>" << endl;
  t << "    <title>" << convertToXML(gd->groupTitle()) << "</title>" << endl;

  writeInnerFiles(gd->getFiles(),t);
  writeInnerClasses(gd->getClasses(),t);
  writeInnerNamespaces(gd->getNamespaces(),t);
  writeInnerPages(gd->getPages(),t);
  writeInnerGroups(gd->getSubGroups(),t);

  if (gd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*gd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      generateXMLSection(gd,ti,t,mg->members(),"user-defined",mg->header(),
          mg->documentation());
    }
  }

  QListIterator<MemberList> mli(gd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberList::declarationLists)!=0)
    {
      generateXMLSection(gd,ti,t,ml,g_xmlSectionMapper.find(ml->listType()));
    }
  }
#if 0
  generateXMLSection(gd,ti,t,&gd->decDefineMembers,"define");
  generateXMLSection(gd,ti,t,&gd->decProtoMembers,"prototype");
  generateXMLSection(gd,ti,t,&gd->decTypedefMembers,"typedef");
  generateXMLSection(gd,ti,t,&gd->decEnumMembers,"enum");
  generateXMLSection(gd,ti,t,&gd->decFuncMembers,"func");
  generateXMLSection(gd,ti,t,&gd->decVarMembers,"var");
#endif

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,gd->briefFile(),gd->briefLine(),gd,0,gd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,gd->docFile(),gd->docLine(),gd,0,gd->documentation());
  t << "    </detaileddescription>" << endl;
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

static void generateXMLForDir(DirDef *dd,FTextStream &ti)
{
  if (dd->isReference()) return; // skip external references
  ti << "  <compound refid=\"" << dd->getOutputFileBase() 
     << "\" kind=\"dir\"><name>" << convertToXML(dd->displayName()) 
     << "</name>" << endl;

  QCString outputDirectory = Config_getString("XML_OUTPUT");
  QCString fileName=outputDirectory+"/"+dd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }

  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);
  writeXMLHeader(t);
  t << "  <compounddef id=\"" 
    << dd->getOutputFileBase() << "\" kind=\"dir\">" << endl;
  t << "    <compoundname>" << convertToXML(dd->displayName()) << "</compoundname>" << endl;

  writeInnerDirs(&dd->subDirs(),t);
  writeInnerFiles(dd->getFiles(),t);

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,dd->briefFile(),dd->briefLine(),dd,0,dd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,dd->docFile(),dd->docLine(),dd,0,dd->documentation());
  t << "    </detaileddescription>" << endl;
  t << "    <location file=\"" << dd->name() << "\"/>" << endl; 
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

static void generateXMLForPage(PageDef *pd,FTextStream &ti,bool isExample)
{
  // + name
  // + title
  // + documentation

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
     << "</name>" << endl;
  
  QCString outputDirectory = Config_getString("XML_OUTPUT");
  QCString fileName=outputDirectory+"/"+pageName+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }

  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);
  writeXMLHeader(t);
  t << "  <compounddef id=\"" << pageName;
  t << "\" kind=\"" << kindName << "\">" << endl;
  t << "    <compoundname>" << convertToXML(pd->name()) 
    << "</compoundname>" << endl;

  SectionInfo *si = Doxygen::sectionDict.find(pd->name());
  if (si)
  {
    t << "    <title>" << convertToXML(si->title) << "</title>" << endl;
  }
  writeInnerPages(pd->getSubPages(),t);
  t << "    <detaileddescription>" << endl;
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
  t << "    </detaileddescription>" << endl;

  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

void generateXML()
{

  // + classes
  // + namespaces
  // + files
  // + groups
  // + related pages
  // - examples
  
  QCString outputDirectory = Config_getString("XML_OUTPUT");
  if (outputDirectory.isEmpty())
  {
    outputDirectory=QDir::currentDirPath().utf8();
  }
  else
  {
    QDir dir(outputDirectory);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(outputDirectory))
      {
        err("error: tag XML_OUTPUT: Output directory `%s' does not "
            "exist and cannot be created\n",outputDirectory.data());
        exit(1);
      }
      else if (!Config_getBool("QUIET"))
      {
        err("notice: Output directory `%s' does not exist. "
            "I have created it for you.\n", outputDirectory.data());
      }
      dir.cd(outputDirectory);
    }
    outputDirectory=dir.absPath().utf8();
  }

  QDir dir(outputDirectory);
  if (!dir.exists())
  {
    dir.setPath(QDir::currentDirPath());
    if (!dir.mkdir(outputDirectory))
    {
      err("Cannot create directory %s\n",outputDirectory.data());
      return;
    }
  }
  QDir xmlDir(outputDirectory);
  createSubDirs(xmlDir);
  QCString fileName=outputDirectory+"/index.xsd";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  f.writeBlock(index_xsd,strlen(index_xsd));
  f.close();

  fileName=outputDirectory+"/compound.xsd";
  f.setName(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  f.writeBlock(compound_xsd,strlen(compound_xsd));
  f.close();

  fileName=outputDirectory+"/index.xml";
  f.setName(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);

  // write index header
  t << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>" << endl;;
  t << "<doxygenindex xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
  t << "xsi:noNamespaceSchemaLocation=\"index.xsd\" ";
  t << "version=\"" << versionString << "\">" << endl;

  {
    ClassSDict::Iterator cli(*Doxygen::classSDict);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      generateXMLForClass(cd,t);
    }
  }
  //{
  //  ClassSDict::Iterator cli(Doxygen::hiddenClasses);
  //  ClassDef *cd;
  //  for (cli.toFirst();(cd=cli.current());++cli)
  //  {
  //    msg("Generating XML output for class %s\n",cd->name().data());
  //    generateXMLForClass(cd,t);
  //  }
  //}
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    msg("Generating XML output for namespace %s\n",nd->name().data());
    generateXMLForNamespace(nd,t);
  }
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      msg("Generating XML output for file %s\n",fd->name().data());
      generateXMLForFile(fd,t);
    }
  }
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    msg("Generating XML output for group %s\n",gd->name().data());
    generateXMLForGroup(gd,t);
  }
  {
    PageSDict::Iterator pdi(*Doxygen::pageSDict);
    PageDef *pd=0;
    for (pdi.toFirst();(pd=pdi.current());++pdi)
    {
      msg("Generating XML output for page %s\n",pd->name().data());
      generateXMLForPage(pd,t,FALSE);
    }
  }
  {
    DirDef *dir;
    DirSDict::Iterator sdi(*Doxygen::directories);
    for (sdi.toFirst();(dir=sdi.current());++sdi)
    {
      msg("Generate XML output for dir %s\n",dir->name().data());
      generateXMLForDir(dir,t);
    }
  }
  {
    PageSDict::Iterator pdi(*Doxygen::exampleSDict);
    PageDef *pd=0;
    for (pdi.toFirst();(pd=pdi.current());++pdi)
    {
      msg("Generating XML output for example %s\n",pd->name().data());
      generateXMLForPage(pd,t,TRUE);
    }
  }
  if (Doxygen::mainPage)
  {
    msg("Generating XML output for the main page\n");
    generateXMLForPage(Doxygen::mainPage,t,FALSE);
  }

  //t << "  </compoundlist>" << endl;
  t << "</doxygenindex>" << endl;

  writeCombineScript();
}


