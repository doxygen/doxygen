/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
#include "code.h"
#include "page.h"
#include "filename.h"
#include "version.h"
#include "xmldocvisitor.h"
#include "docparser.h"
#include "language.h"

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

// no debug info
#define XML_DB(x) do {} while(0)
// debug to stdout
//#define XML_DB(x) printf x
// debug inside output
//#define XML_DB(x) QCString __t;__t.sprintf x;m_t << __t


inline void writeXMLString(QTextStream &t,const char *s)
{
  t << convertToXML(s);
}

inline void writeXMLCodeString(QTextStream &t,const char *s)
{
  char c;
  while ((c=*s++))
  {
    switch(c)
    {
      case ' ':  t << "<sp/>";  break;
      case '<':  t << "&lt;";   break;
      case '>':  t << "&gt;";   break;
      case '&':  t << "&amp;";  break;
      case '\'': t << "&apos;"; break; 
      case '"':  t << "&quot;"; break;
      default:   t << c;        break;         
    }
  } 
}


static void writeXMLHeader(QTextStream &t)
{
  QCString dtdName = Config_getString("XML_DTD");
  QCString schemaName = Config_getString("XML_SCHEMA");
  t << "<?xml version='1.0' encoding='" << theTranslator->idLanguageCharset()
    << "' standalone='";
  if (dtdName.isEmpty() && schemaName.isEmpty()) t << "yes"; else t << "no";
  t << "'?>" << endl;
  if (!dtdName.isEmpty())
  {
    t << "<!DOCTYPE doxygen SYSTEM \"doxygen.dtd\">" << endl;
  }
  t << "<doxygen ";
  if (!schemaName.isEmpty())
  {
    t << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    t << "xsi:noNamespaceSchemaLocation=\"doxygen.xsd\" ";
  }
  t << "version=\"" << versionString << "\">" << endl;
}

void writeXMLLink(QTextStream &t,const char *extRef,const char *compoundId,
                  const char *anchorId,const char *text)
{
  t << "<ref refid=\"" << compoundId;
  if (anchorId) t << "_1" << anchorId;
  t << "\" kindref=\"";
  if (anchorId) t << "member"; else t << "compound"; 
  t << "\"";
  if (extRef) t << " external=\"" << extRef << "\"";
  t << ">";
  writeXMLString(t,text);
  t << "</ref>";
}

class TextGeneratorXMLImpl : public TextGeneratorIntf
{
  public:
    TextGeneratorXMLImpl(QTextStream &t): m_t(t) {}
    void writeString(const char *s) const
    {
      writeXMLString(m_t,s); 
    }
    void writeBreak() const {}
    void writeLink(const char *extRef,const char *file,
                   const char *anchor,const char *text
                  ) const
    {
      writeXMLLink(m_t,extRef,file,anchor,text);
    }
  private:
    QTextStream &m_t;
};

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


class XMLCodeGenerator : public BaseCodeDocInterface
{
  public:

    XMLCodeGenerator(QTextStream &t) : m_t(t), m_lineNumber(-1),
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
      writeXMLCodeString(m_t,text);
    }
    void writeCodeLink(const char *ref,const char *file,
                               const char *anchor,const char *text) 
    {
      XML_DB(("(writeCodeLink)\n"));
      if (m_insideCodeLine && !m_insideSpecialHL && m_normalHLNeedStartTag)
      {
        m_t << "<highlight class=\"normal\">";
        m_normalHLNeedStartTag=FALSE;
      }
      writeXMLLink(m_t,ref,file,anchor,text);
    }
    void startCodeLine() 
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
    void finish()
    {
      if (m_insideCodeLine) endCodeLine();
    }

  private:
    QTextStream &m_t;  
    QCString m_refId;
    QCString m_external;
    int m_lineNumber;
    bool m_isMemberRef;

    bool m_insideCodeLine;
    bool m_normalHLNeedStartTag;
    bool m_insideSpecialHL;
};


static void writeTemplateArgumentList(ArgumentList *al,QTextStream &t,const char *name,int indent)
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
        linkifyText(TextGeneratorXMLImpl(t),name,0,a->type);
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
        linkifyText(TextGeneratorXMLImpl(t),name,0,a->defval);
        t << "</defval>" << endl;
      }
      t << indentStr << "  </param>" << endl;
    }
    t << indentStr << "</templateparamlist>" << endl;
  }
}

static void writeMemberTemplateLists(MemberDef *md,QTextStream &t)
{
  ClassDef *cd = md->getClassDef();
  const char *cname = cd ? cd->name().data() : 0;
  if (md->templateArguments()) // function template prefix
  {
    writeTemplateArgumentList(md->templateArguments(),t,cname,8);
  }
}

static void writeTemplateList(ClassDef *cd,QTextStream &t)
{
  writeTemplateArgumentList(cd->templateArguments(),t,cd->name(),4);
}

static void writeXMLDocBlock(QTextStream &t,
                      const QCString &fileName,
                      int lineNr,
                      const QCString &scope,
                      MemberDef * md,
                      const QCString &text)
{
  QCString stext = text.stripWhiteSpace();
  if (stext.isEmpty()) return;
  // convert the documentation string into an abstract syntax tree
  DocNode *root = validatingParseDoc(fileName,lineNr,scope,md,text+"\n",FALSE);
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

void writeXMLCodeBlock(QTextStream &t,FileDef *fd)
{
  initParseCodeContext();
  XMLCodeGenerator *xmlGen = new XMLCodeGenerator(t);
  parseCode(*xmlGen,
            0,
            fileToString(fd->absFilePath(),Config_getBool("FILTER_SOURCE_FILES")),
            FALSE,
            0,
            fd);
  xmlGen->finish();
  delete xmlGen;
}

static void writeMemberReference(QTextStream &t,Definition *def,MemberDef *rmd,const char *tagName)
{
  QCString scope = rmd->getScopeString();
  QCString name = rmd->name();
  if (!scope.isEmpty() && scope!=def->name())
  {
    name.prepend(scope+"::");
  }
  t << "        <" << tagName << " id=\"";
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

static void generateXMLForMember(MemberDef *md,QTextStream &ti,QTextStream &t,Definition *def)
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
  
  // enum values are written as part of the enum
  if (md->memberType()==MemberDef::EnumValue) return;

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
    case MemberDef::Prototype:   memType="prototype"; isFunc=TRUE; break;
    case MemberDef::Friend:      memType="friend";    isFunc=TRUE; break;
    case MemberDef::DCOP:        memType="dcop";      isFunc=TRUE; break;
    case MemberDef::Slot:        memType="slot";      isFunc=TRUE; break;
  }

  ti << "    <member refid=\"" << md->getOutputFileBase() 
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
  t << md->getOutputFileBase()
    << "_1"      // encoded `:' character (see util.cpp:convertNameToFile)
    << md->anchor();
  t << "\"";
  t << " virt=\"";
  switch (md->virtualness())
  {
    case Normal:  t << "normal";       break;
    case Virtual: t << "virtual";      break;
    case Pure:    t << "pure-virtual"; break;
    default: ASSERT(0);
  }
  t << "\" prot=\"";
  switch(md->protection())
  {
    case Public:    t << "public";     break;
    case Protected: t << "protected";  break;
    case Private:   t << "private";    break;
  }
  t << "\" static=\"";
  if (md->isStatic()) t << "yes"; else t << "no";
  t << "\"";

  if (isFunc)
  {
    ArgumentList *al = md->argumentList();
    t << " const=\"";
    if (al && al->constSpecifier)    t << "yes"; else t << "no"; 
    t << "\" volatile=\"";
    if (al && al->volatileSpecifier) t << "yes"; else t << "no"; 
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
    t << "        <type>";
    linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),typeStr);
    t << "</type>" << endl;
    t << "        <definition>" << convertToXML(md->definition()) << "</definition>" << endl;
    t << "        <argsstring>" << convertToXML(md->argsString()) << "</argsstring>" << endl;
  }

  t << "        <name>" << convertToXML(md->name()) << "</name>" << endl;
  MemberDef *rmd = md->reimplements();
  if (rmd)
  {
    t << "        <reimplements id=\"" 
      << rmd->getOutputFileBase() << "_1" << rmd->anchor() << "\">"
      << convertToXML(rmd->name()) << "</reimplements>";
  }
  MemberList *rbml = md->reimplementedBy();
  if (rbml)
  {
    MemberListIterator mli(*rbml);
    for (mli.toFirst();(rmd=mli.current());++mli)
    {
      t << "        <reimplementedby id=\"" 
        << rmd->getOutputFileBase() << "_1" << rmd->anchor() << "\">"
        << convertToXML(rmd->name()) << "</reimplementedby>";
    }
  }
  
  if (isFunc) //function
  {
    ArgumentList *declAl = md->declArgumentList();
    ArgumentList *defAl = md->argumentList();
    if (declAl && declAl->count()>0)
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
          linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),a->type);
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
          linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),a->defval);
          t << "</defval>" << endl;
        }
        if (defArg && defArg->hasDocumentation())
        {
          t << "          <briefdescription>";
          writeXMLDocBlock(t,md->getDefFileName(),md->getDefLine(),
                           scopeName,md,defArg->docs);
          t << "</briefdescription>" << endl;
        }
        t << "        </param>" << endl;
        if (defArg) ++defAli;
      }
    }
  }
  else if (md->memberType()==MemberDef::Define && 
           md->argsString()!=0) // define
  {
    ArgumentListIterator ali(*md->argumentList());
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      t << "        <param><defname>" << a->type << "</defname></param>" << endl;
    }
  }
  if (!md->initializer().isEmpty())
  {
    t << "        <initializer>";
    linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),md->initializer());
    t << "</initializer>" << endl;
  }

  if (md->excpString())
  {
    t << "        <exceptions>";
    linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),md->excpString());
    t << "</exceptions>" << endl;
  }
  
  if (md->memberType()==MemberDef::Enumeration) // enum
  {
    if (md->enumFieldList())
    {
      MemberListIterator emli(*md->enumFieldList());
      MemberDef *emd;
      for (emli.toFirst();(emd=emli.current());++emli)
      {
        ti << "    <member refid=\"" << emd->getOutputFileBase() 
           << "_1" << emd->anchor() << "\" kind=\"enumvalue\"><name>" 
           << convertToXML(emd->name()) << "</name></member>" << endl;

        t << "        <enumvalue id=\"" << emd->getOutputFileBase() << "_1" 
          << emd->anchor() << "\" prot=\"";
        switch (emd->protection())
        {
          case Public:    t << "public";    break;
          case Protected: t << "protected"; break;
          case Private:   t << "private";   break;
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
        writeXMLDocBlock(t,emd->briefFile(),emd->briefLine(),scopeName,emd,emd->briefDescription());
        t << "          </briefdescription>" << endl;
        t << "          <detaileddescription>" << endl;
        writeXMLDocBlock(t,emd->docFile(),emd->docLine(),scopeName,emd,emd->documentation());
        t << "          </detaileddescription>" << endl;
        t << "        </enumvalue>" << endl;
      }
    }
  }
  t << "        <briefdescription>" << endl;
  writeXMLDocBlock(t,md->briefFile(),md->briefLine(),scopeName,md,md->briefDescription());
  t << "        </briefdescription>" << endl;
  t << "        <detaileddescription>" << endl;
  writeXMLDocBlock(t,md->docFile(),md->docLine(),scopeName,md,md->documentation());
  t << "        </detaileddescription>" << endl;
  if (md->getDefLine()!=-1)
  {
    t << "        <location file=\"" 
      << md->getDefFileName() << "\" line=\"" 
      << md->getDefLine() << "\"";
    if (md->getStartBodyLine()!=-1)
    {
      t << " bodystart=\"" << md->getStartBodyLine() << "\" bodyend=\"" 
        << md->getEndBodyLine() << "\"";
    }
    t << "/>" << endl;
  }

  //printf("md->getReferencesMembers()=%p\n",md->getReferencesMembers());
  if (md->getReferencesMembers())
  {
    MemberSDict::Iterator mdi(*md->getReferencesMembers());
    MemberDef *rmd;
    for (mdi.toFirst();(rmd=mdi.current());++mdi)
    {
      writeMemberReference(t,def,rmd,"references");
    }
  }
  if (md->getReferencedByMembers())
  {
    MemberSDict::Iterator mdi(*md->getReferencedByMembers());
    MemberDef *rmd;
    for (mdi.toFirst();(rmd=mdi.current());++mdi)
    {
      writeMemberReference(t,def,rmd,"referencedby");
    }
  }
  
  t << "      </memberdef>" << endl;
}

static void generateXMLSection(Definition *d,QTextStream &ti,QTextStream &t,
                      MemberList *ml,const char *kind,const char *header=0)
{
  if (ml->count()==0) return; // empty list

  t << "      <sectiondef kind=\"" << kind << "\">" << endl;
  if (header)
  {
    t << "      <header>" << convertToXML(header) << "</header>" << endl;
  }
  MemberListIterator mli(*ml);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    generateXMLForMember(md,ti,t,d);
  }
  t << "      </sectiondef>" << endl;
}

static void writeListOfAllMembers(ClassDef *cd,QTextStream &t)
{
  t << "    <listofallmembers>" << endl;
  MemberNameInfoSDict::Iterator mnii(*cd->memberNameInfoSDict());
  MemberNameInfo *mni;
  for (mnii.toFirst();(mni=mnii.current());++mnii)
  {
    MemberNameInfoIterator mii(*mni);
    MemberInfo *mi;
    for (mii.toFirst();(mi=mii.current());++mii)
    {
      MemberDef *md=mi->memberDef;
      ClassDef  *cd=md->getClassDef();
      Definition *d=md->getGroupDef();
      if (d==0) d = cd;
      Protection prot = mi->prot;
      Specifier virt=md->virtualness();
      t << "      <member refid=\"" << d->getOutputFileBase() << "_1" <<
        md->anchor() << "\" prot=\"";
      switch (prot)
      {
        case Public:    t << "public";    break;
        case Protected: t << "protected"; break;
        case Private:   t << "private";   break;
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
  t << "    </listofallmembers>" << endl;
}

static void generateXMLForClass(ClassDef *cd,QTextStream &ti)
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
  if (cd->name().find('@')!=-1) return; // skip anonymous compounds.
  if (cd->templateMaster()!=0)  return; // skip generated template instances.

  ti << "  <compound refid=\"" << cd->getOutputFileBase() 
     << "\" kind=\"" << cd->compoundTypeString()
     << "\"><name>" << convertToXML(cd->name()) << "</name>" << endl;
  
  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  QCString fileName=outputDirectory+"/xml/"+cd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t.setEncoding(QTextStream::Latin1);

  writeXMLHeader(t);
  t << "  <compounddef id=\"" 
    << cd->getOutputFileBase() << "\" kind=\"" 
    << cd->compoundTypeString() << "\">" << endl;
  t << "    <compoundname>"; 
  writeXMLString(t,cd->name()); 
  t << "</compoundname>" << endl;
  if (cd->baseClasses()->count()>0)
  {
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "    <basecompoundref refid=\"" 
        << bcd->classDef->getOutputFileBase()
        << "\" prot=\"";
      switch (bcd->prot)
      {
        case Public:    t << "public";    break;
        case Protected: t << "protected"; break;
        case Private:   t << "private";   break;
      }
      t << "\" virt=\"";
      switch(bcd->virt)
      {
        case Normal:  t << "non-virtual";  break;
        case Virtual: t << "virtual";      break;
        case Pure:    t <<"pure-virtual"; break;
      }
      t << "\">" << convertToXML(bcd->classDef->displayName()) 
        << "</basecompoundref>" << endl;
    }
  }
  if (cd->subClasses()->count()>0)
  {
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "    <derivedcompoundref refid=\"" 
        << bcd->classDef->getOutputFileBase()
        << "\" prot=\"";
      switch (bcd->prot)
      {
        case Public:    t << "public";    break;
        case Protected: t << "protected"; break;
        case Private:   t << "private";   break;
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

  ClassSDict *cl = cd->getInnerClasses();
  if (cl)
  {
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      t << "    <innerclass refid=\"" << cd->getOutputFileBase()
        << "\">" << convertToXML(cd->name()) << "</innerclass>" << endl;
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
        t << " id=\"" << ii->fileDef->getOutputFileBase() << "\"";
      }
      t << " local=\"" << (ii->local ? "yes" : "no") << "\">";
      t << nm;
      t << "</includes>" << endl;
    }
  }

  writeTemplateList(cd,t);
  writeListOfAllMembers(cd,t);
  MemberGroupSDict::Iterator mgli(*cd->memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    generateXMLSection(cd,ti,t,mg->members(),"user-defined",mg->header());
  }

  generateXMLSection(cd,ti,t,&cd->pubTypes,"public-type");
  generateXMLSection(cd,ti,t,&cd->pubMethods,"public-func");
  generateXMLSection(cd,ti,t,&cd->pubAttribs,"public-attrib");
  generateXMLSection(cd,ti,t,&cd->pubSlots,"public-slot");
  generateXMLSection(cd,ti,t,&cd->signals,"signal");
  generateXMLSection(cd,ti,t,&cd->dcopMethods,"dcop-func");
  generateXMLSection(cd,ti,t,&cd->properties,"property");
  generateXMLSection(cd,ti,t,&cd->events,"event");
  generateXMLSection(cd,ti,t,&cd->pubStaticMethods,"public-static-func");
  generateXMLSection(cd,ti,t,&cd->pubStaticAttribs,"public-static-attrib");
  generateXMLSection(cd,ti,t,&cd->proTypes,"protected-type");
  generateXMLSection(cd,ti,t,&cd->proMethods,"protected-func");
  generateXMLSection(cd,ti,t,&cd->proAttribs,"protected-attrib");
  generateXMLSection(cd,ti,t,&cd->proSlots,"protected-slot");
  generateXMLSection(cd,ti,t,&cd->proStaticMethods,"protected-static-func");
  generateXMLSection(cd,ti,t,&cd->proStaticAttribs,"protected-static-attrib");
  generateXMLSection(cd,ti,t,&cd->priTypes,"private-type");
  generateXMLSection(cd,ti,t,&cd->priMethods,"private-func");
  generateXMLSection(cd,ti,t,&cd->priAttribs,"private-attrib");
  generateXMLSection(cd,ti,t,&cd->priSlots,"private-slot");
  generateXMLSection(cd,ti,t,&cd->priStaticMethods,"private-static-func");
  generateXMLSection(cd,ti,t,&cd->priStaticAttribs,"private-static-attrib");
  generateXMLSection(cd,ti,t,&cd->friends,"friend");
  generateXMLSection(cd,ti,t,&cd->related,"related");

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,cd->briefFile(),cd->briefLine(),cd->name(),0,cd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,cd->docFile(),cd->docLine(),cd->name(),0,cd->documentation());
  t << "    </detaileddescription>" << endl;
  DotClassGraph inheritanceGraph(cd,DotClassGraph::Inheritance);
  if (!inheritanceGraph.isTrivial())
  {
    t << "    <inheritancegraph>" << endl;
    inheritanceGraph.writeXML(t);
    t << "    </inheritancegraph>" << endl;
  }
  DotClassGraph collaborationGraph(cd,DotClassGraph::Implementation);
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
      t << " bodystart=\"" << cd->getStartBodyLine() << "\" bodyend=\"" 
        << cd->getEndBodyLine() << "\"";
    }
  t << "/>" << endl;
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

static void generateXMLForNamespace(NamespaceDef *nd,QTextStream &ti)
{
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + location
  // - files containing (parts of) the namespace definition

  if (nd->isReference()) return; // skip external references

  ti << "  <compound refid=\"" << nd->getOutputFileBase() 
     << "\" kind=\"namespace\"" << "><name>" 
     << convertToXML(nd->name()) << "</name>" << endl;
  
  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  QCString fileName=outputDirectory+"/xml/"+nd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t.setEncoding(QTextStream::Latin1);
  
  writeXMLHeader(t);
  t << "  <compounddef id=\"" 
    << nd->getOutputFileBase() << "\" kind=\"namespace\">" << endl;
  t << "    <compoundname>";
  writeXMLString(t,nd->name());
  t << "</compoundname>" << endl;

  ClassSDict *cl = nd->classSDict;
  if (cl)
  {
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      t << "    <innerclass refid=\"" << cd->getOutputFileBase()
        << "\">" << convertToXML(cd->name()) << "</innerclass>" << endl;
    }
  }
  NamespaceSDict *nl = nd->namespaceSDict;
  if (nl)
  {
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
    {
      t << "    <innernamespace refid=\"" << nd->getOutputFileBase()
        << "\">" << convertToXML(nd->name()) << "</innernamespace>" << endl;
    }
  }
  MemberGroupSDict::Iterator mgli(*nd->memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    generateXMLSection(nd,ti,t,mg->members(),"user-defined",mg->header());
  }

  generateXMLSection(nd,ti,t,&nd->decDefineMembers,"define");
  generateXMLSection(nd,ti,t,&nd->decProtoMembers,"prototype");
  generateXMLSection(nd,ti,t,&nd->decTypedefMembers,"typedef");
  generateXMLSection(nd,ti,t,&nd->decEnumMembers,"enum");
  generateXMLSection(nd,ti,t,&nd->decFuncMembers,"func");
  generateXMLSection(nd,ti,t,&nd->decVarMembers,"var");

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,nd->briefFile(),nd->briefLine(),0,0,nd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,nd->docFile(),nd->docLine(),0,0,nd->documentation());
  t << "    </detaileddescription>" << endl;
  t << "    <location file=\"" 
    << nd->getDefFileName() << "\" line=\"" 
    << nd->getDefLine() << "\"/>" << endl;
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

static void generateXMLForFile(FileDef *fd,QTextStream &ti)
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
  
  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  QCString fileName=outputDirectory+"/xml/"+fd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t.setEncoding(QTextStream::Latin1);

  writeXMLHeader(t);
  t << "  <compounddef id=\"" 
    << fd->getOutputFileBase() << "\" kind=\"file\">" << endl;
  t << "    <compoundname>";
  writeXMLString(t,fd->name());
  t << "</compoundname>" << endl;

  QListIterator<IncludeInfo> ili1(*fd->includeFileList());
  IncludeInfo *inc;
  for (ili1.toFirst();(inc=ili1.current());++ili1)
  {
    t << "    <includes";
    if (inc->fileDef && !inc->fileDef->isReference()) // TODO: support external references
    {
      t << " id=\"" << inc->fileDef->getOutputFileBase() << "\"";
    }
    t << " local=\"" << (inc->local ? "yes" : "no") << "\">";
    t << inc->includeName;
    t << "</includes>" << endl;
  }

  QListIterator<IncludeInfo> ili2(*fd->includedByFileList());
  for (ili2.toFirst();(inc=ili2.current());++ili2)
  {
    t << "    <includedby";
    if (inc->fileDef && !inc->fileDef->isReference()) // TODO: support external references
    {
      t << " id=\"" << inc->fileDef->getOutputFileBase() << "\"";
    }
    t << " local=\"" << (inc->local ? "yes" : "no") << "\">";
    t << inc->includeName;
    t << "</includedby>" << endl;
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

  ClassSDict *cl = fd->classSDict;
  if (cl)
  {
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      t << "    <innerclass refid=\"" << cd->getOutputFileBase()
        << "\">" << convertToXML(cd->name()) << "</innerclass>" << endl;
    }
  }
  NamespaceSDict *nl = fd->namespaceSDict;
  if (nl)
  {
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
    {
      t << "    <innernamespace refid=\"" << nd->getOutputFileBase()
        << "\">" << convertToXML(nd->name()) << "</innernamespace>" << endl;
    }
  }

  MemberGroupSDict::Iterator mgli(*fd->memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    generateXMLSection(fd,ti,t,mg->members(),"user-defined",mg->header());
  }

  generateXMLSection(fd,ti,t,&fd->decDefineMembers,"define");
  generateXMLSection(fd,ti,t,&fd->decProtoMembers,"prototype");
  generateXMLSection(fd,ti,t,&fd->decTypedefMembers,"typedef");
  generateXMLSection(fd,ti,t,&fd->decEnumMembers,"enum");
  generateXMLSection(fd,ti,t,&fd->decFuncMembers,"func");
  generateXMLSection(fd,ti,t,&fd->decVarMembers,"var");

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,fd->briefFile(),fd->briefLine(),0,0,fd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,fd->docFile(),fd->docLine(),0,0,fd->documentation());
  t << "    </detaileddescription>" << endl;
  t << "    <programlisting>" << endl;
  writeXMLCodeBlock(t,fd);
  t << "    </programlisting>" << endl;
  t << "    <location file=\"" << fd->getDefFileName() << "\"/>" << endl;
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

static void generateXMLForGroup(GroupDef *gd,QTextStream &ti)
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
  
  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  QCString fileName=outputDirectory+"/xml/"+gd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }

  QTextStream t(&f);
  t.setEncoding(QTextStream::Latin1);
  writeXMLHeader(t);
  t << "  <compounddef id=\"" 
    << gd->getOutputFileBase() << "\" kind=\"group\">" << endl;
  t << "    <compoundname>" << convertToXML(gd->name()) << "</compoundname>" << endl;
  t << "    <title>" << convertToXML(gd->groupTitle()) << "</title>" << endl;

  FileList *fl = gd->getFiles();
  if (fl)
  {
    QListIterator<FileDef> fli(*fl);
    FileDef *fd = fl->first();
    for (fli.toFirst();(fd=fli.current());++fli)
    {
      t << "    <innerfile refid=\"" << fd->getOutputFileBase() 
        << "\">" << convertToXML(fd->name()) << "</innerfile>" << endl;
    }
  }
  ClassSDict *cl = gd->getClasses();
  if (cl)
  {
    ClassSDict::Iterator cli(*cl);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      t << "    <innerclass refid=\"" << cd->getOutputFileBase()
        << "\">" << convertToXML(cd->name()) << "</innerclass>" << endl;
    }
  }
  NamespaceList *nl = gd->getNamespaces();
  if (nl)
  {
    NamespaceListIterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
    {
      t << "    <innernamespace refid=\"" << nd->getOutputFileBase()
        << "\">" << convertToXML(nd->name()) << "</innernamespace>" << endl;
    }
  }
  PageSDict *pl = gd->getPages();
  if (pl)
  {
    PageSDict::Iterator pli(*pl);
    PageInfo *pi;
    for (pli.toFirst();(pi=pli.current());++pli)
    {
      t << "    <innerpage refid=\"" << pi->getOutputFileBase()
        << "\">" << convertToXML(pi->title) << "</innerpage>" << endl;
    }
  }

  GroupList *gl = gd->getSubGroups();
  if (gl)
  {
    GroupListIterator gli(*gl);
    GroupDef *sgd;
    for (gli.toFirst();(sgd=gli.current());++gli)
    {
      t << "    <innergroup refid=\"" << gd->getOutputFileBase()
        << "\">" << convertToXML(sgd->groupTitle()) 
        << "</innergroup>" << endl;
    }
  }

  MemberGroupSDict::Iterator mgli(*gd->memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    generateXMLSection(gd,ti,t,mg->members(),"user-defined",mg->header());
  }

  generateXMLSection(gd,ti,t,&gd->decDefineMembers,"define");
  generateXMLSection(gd,ti,t,&gd->decProtoMembers,"prototype");
  generateXMLSection(gd,ti,t,&gd->decTypedefMembers,"typedef");
  generateXMLSection(gd,ti,t,&gd->decEnumMembers,"enum");
  generateXMLSection(gd,ti,t,&gd->decFuncMembers,"func");
  generateXMLSection(gd,ti,t,&gd->decVarMembers,"var");

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,gd->briefFile(),gd->briefLine(),0,0,gd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,gd->docFile(),gd->docLine(),0,0,gd->documentation());
  t << "    </detaileddescription>" << endl;
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
}

static void generateXMLForPage(PageInfo *pi,QTextStream &ti)
{
  // + name
  // + title
  // + documentation

  if (pi->isReference()) return;
  
  QCString pageName = pi->getOutputFileBase();
  if (pageName=="index") pageName="indexpage"; // to prevent overwriting the generated index page.
  
  ti << "  <compound refid=\"" << pageName
     << "\" kind=\"page\"><name>" << convertToXML(pi->name) << "</name>" << endl;
  
  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  QCString fileName=outputDirectory+"/xml/"+pageName+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }

  QTextStream t(&f);
  t.setEncoding(QTextStream::Latin1);
  writeXMLHeader(t);
  t << "  <compounddef id=\"" << pageName;
  t << "\" kind=\"page\">" << endl;
  t << "    <compoundname>" << convertToXML(pi->name) << "</compoundname>" << endl;
  SectionInfo *si = Doxygen::sectionDict.find(pi->name);
  if (si)
  {
    t << "    <title>" << si->title << "</title>" << endl;
  }
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,pi->defFileName,pi->defLine,0,0,pi->doc);
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
  // - packages
  // + groups
  // + related pages
  // - examples
  
  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  if (outputDirectory.isEmpty())
  {
    outputDirectory=QDir::currentDirPath();
  }
  else
  {
    QDir dir(outputDirectory);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(outputDirectory))
      {
        err("Error: tag OUTPUT_DIRECTORY: Output directory `%s' does not "
            "exist and cannot be created\n",outputDirectory.data());
        exit(1);
      }
      else if (!Config_getBool("QUIET"))
      {
        err("Notice: Output directory `%s' does not exist. "
            "I have created it for you.\n", outputDirectory.data());
      }
      dir.cd(outputDirectory);
    }
    outputDirectory=dir.absPath();
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
  QDir xmlDir(outputDirectory+"/xml");
  if (!xmlDir.exists() && !xmlDir.mkdir(outputDirectory+"/xml"))
  {
    err("Could not create xml directory in %s\n",outputDirectory.data());
    return;
  }
  
  QCString fileName=outputDirectory+"/xml/index.xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t.setEncoding(QTextStream::Latin1);
  writeXMLHeader(t);
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    generateXMLForClass(cd,t);
  }
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    generateXMLForNamespace(nd,t);
  }
  FileNameListIterator fnli(Doxygen::inputNameList);
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      generateXMLForFile(fd,t);
    }
  }
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    generateXMLForGroup(gd,t);
  }
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageInfo *pi=0;
  for (pdi.toFirst();(pi=pdi.current());++pdi)
  {
    generateXMLForPage(pi,t);
  }
  if (Doxygen::mainPage)
  {
    generateXMLForPage(Doxygen::mainPage,t);
  }

  //t << "  </compoundlist>" << endl;
  t << "</doxygen>" << endl;
}


