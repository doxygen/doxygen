/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
#include "doc.h"

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

static inline void writeXMLString(QTextStream &t,const char *s)
{
  t << convertToXML(s);
}

static void writeXMLLink(QTextStream &t,const char *compoundId,
                  const char *memId,const char *text)
{
  if (memId==0)
  {
    t << "<compoundref idref=\"" << compoundId << "\">";
    writeXMLString(t,text);
    t << "</compoundref>";
  }
  else
  {
    t << "<memberref idref=\"" << compoundId << "_1" << memId << "\">";
    writeXMLString(t,text);
    t << "</memberref>";
  }
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
      if (extRef==0) 
      { writeXMLLink(m_t,file,anchor,text); } 
      else // external references are not supported for XML
      { writeXMLString(m_t,text); }
    }
  private:
    QTextStream &m_t;
};

template<class T> class ValStack
{
  public:
    ValStack() : m_values(10), m_sp(0), m_size(10) {}
    virtual ~ValStack() {}
    ValStack(const ValStack &s)
    {
      m_values=s.m_values.copy();
      m_sp=s.m_sp;
      m_size=s.m_size;
    }
    ValStack &operator=(const ValStack &s)
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
    
  private:
    QArray<T> m_values;
    int m_sp;
    int m_size;
};

/*! This class is used by the documentation parser.
 *  Its methods are called when some XML text or markup
 *  needs to be written.
 */
class XMLGenerator : public OutputDocInterface
{
  public:

    // helper functions
    
    void startParMode()
    {
      if (!m_inParStack.isEmpty() && !m_inParStack.top())
      {
        m_inParStack.top() = TRUE;
        m_t << "<para>" << endl;
      }
      else if (m_inParStack.isEmpty())
      {
        m_inParStack.push(TRUE);
        m_t << "<para>" << endl;
      }
    }
    void endParMode()
    {
      if (!m_inParStack.isEmpty() && m_inParStack.top())
      {
        m_inParStack.top() = FALSE;
        m_t << "</para>" << endl;
      }
    }
    void startNestedPar()
    {
      m_inParStack.push(FALSE);
    }
    void endNestedPar()
    {
      if (m_inParStack.pop())
      {
        m_t << "</para>" << endl;
      }
    }
  
    // Standard generator functions to be implemented by all generators

    void docify(const char *s) 
    {
      startParMode();
      writeXMLString(m_t,s);
    }
    void writeChar(char c) 
    {
      startParMode();
      char s[2];s[0]=c;s[1]=0;
      docify(s);
    }
    void writeString(const char *text) 
    {
      startParMode();
      m_t << text;
    }
    void startItemList()       
    { 
      m_t << "<itemizedlist>" << endl;; 
      m_inListStack.push(TRUE);
    }
    void startEnumList()       
    { 
      m_t << "<orderedlist>"; 
      m_inListStack.push(TRUE);
    }
    void writeListItem()       
    { 
      if (!m_inListStack.isEmpty() && m_inListStack.top()) // first element
      {
        m_inListStack.top()=FALSE;
      }
      else // not first element, end previous element first
      {
        endParMode();
        endNestedPar();
        m_t << "</listitem>" << endl; 
      }
      m_t << "<listitem>"; 
      startNestedPar();
    }
    void endItemList()         
    {
      if (!m_inListStack.isEmpty() && !m_inListStack.pop()) // first element
      {
        endParMode(); 
        endNestedPar();
        m_t << "</listitem>" << endl; 
      }
      m_t << "</itemizedlist>" << endl; 
    }
    void endEnumList()         
    { 
      if (!m_inListStack.isEmpty() && !m_inListStack.pop()) // first element
      {
        endParMode(); 
        m_t << "</listitem>" << endl; 
        endNestedPar(); 
      }
      m_t << "</orderedlist>" << endl; 
    }
    void newParagraph()        
    { 
      endParMode();
      startParMode();
    }
    void startBold()           
    { 
      startParMode();
      m_t << "<bold>"; // non DocBook
    }
    void endBold()             
    { 
      m_t << "</bold>"; // non DocBook
    }
    void startTypewriter()     
    { 
      startParMode();
      m_t << "<computeroutput>";
    }
    void endTypewriter()       
    { 
      m_t << "</computeroutput>";
    }
    void startEmphasis()       
    { 
      startParMode();
      m_t << "<emphasis>";
    }
    void endEmphasis()         
    { 
      m_t << "</emphasis>";
    }
    void startCodeFragment()   
    { 
      endParMode();
      m_t << "<programlisting>";
    }
    void endCodeFragment()     
    { 
      m_t << "</programlisting>"; 
    }
    void startPreFragment()    
    { 
      endParMode();
      m_t << "<programlisting>";
    }
    void endPreFragment()      
    { 
      m_t << "</programlisting>"; 
    }
    void writeRuler()          
    { 
      endParMode();
      m_t << "<hruler/>"; 
    }
    void startDescription()    
    { 
      m_t << "<variablelist>"; 
      m_inListStack.push(TRUE);
    }
    void endDescription()      
    { 
      if (!m_inListStack.isEmpty() && !m_inListStack.pop()) // first element
      {
        endNestedPar(); 
        m_t << "</listitem>" << endl; 
      }
      m_t << "</variablelist>"; 
      if (!m_inListStack.isEmpty()) m_inListStack.pop();
    }
    void startDescItem()       
    { 
      if (!m_inListStack.isEmpty() && m_inListStack.top()) // first element
      {
        m_inListStack.top()=FALSE;
      }
      else // not first element, end previous element first
      {
        endNestedPar();
        m_t << "</listitem>"; 
      }
      m_t << "<varlistentry><term>"; 
    }
    void endDescItem()         
    { 
      m_t << "</term></varlistentry><listitem>"; 
      startNestedPar();
    }
    void startDescList()       
    { 
      m_t << "<simplesect><title>"; 
    }
    void endDescList()         
    { 
      endNestedPar();
      m_t << "</simplesect>";
    }
    void startParamList()      
    { 
      m_t << "<parameterlist><title>"; // non DocBook
                                // TODO: what kind of list
                                // param, retval, exception
      m_inParamList = TRUE;
    }
    void endParamList()        
    { 
      m_inParamList = FALSE;
      m_t << "</parameterlist>";
    }
    void endDescTitle()        
    { 
      m_t << "</title>"; 
      if (!m_inParamList) startNestedPar();
    }
    void writeDescItem()       { }
    void startDescTable()      { }
    void endDescTable()        { }
    void startDescTableTitle() 
    { 
      m_t << "<parametername>"; // non docbook
    }
    void endDescTableTitle()   
    { 
      m_t << "</parametername>"; // non docbook
    }
    void startDescTableData()  
    { 
      m_t << "<parameterdescription>"; // non docbook
      startNestedPar();
    }
    void endDescTableData()    
    { 
      endNestedPar();
      m_t << "</parameterdescription>"; // non docbook
    }
    void lineBreak()           
    { 
      m_t << "<linebreak/>"; // non docbook
    }
    void writeNonBreakableSpace(int num) 
    { 
      int i;for (i=0;i<num;i++) m_t << "<nonbreakablespace/>";  // non docbook
    }
    
    //// TODO: translate these as well....

    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor, const char *text) 
    {
      if (ref) // TODO: add support for external references
      {
        docify(text);
      }
      else
      {
        writeXMLLink(m_t,file,anchor,text);
      }
    }
    void writeCodeLink(const char *ref,const char *file,
                               const char *anchor,const char *text) 
    {
      if (ref) // TODO: add support for external references
      {
        docify(text);
      }
      else
      {
        writeXMLLink(m_t,file,anchor,text);
      }
    }
    void startHtmlLink(const char *url)
    {
      m_t << "<ulink url=\"" << url << "\">";
    }
    void endHtmlLink()
    {
      m_t << "</ulink>";
    }
    void writeMailLink(const char *url) 
    {
      m_t << "<email>";
      docify(url); 
      m_t << "</email>";
    }
    void startSection(const char *id,const char *,bool) 
    {
      m_t << "<sect1 id=\"" << id << "\">";
    }
    void endSection(const char *,bool)
    {
      m_t << "</sect1>";
    }
    void startSubsection() 
    {
      m_t << "<sect2>";
    }
    void endSubsection() 
    {
      m_t << "</sect2>";
    }
    void startSubsubsection() 
    {
      m_t << "<sect3>";
    }
    void endSubsubsection() 
    {
      m_t << "</sect3>";
    }
    void startCenter() 
    {
      m_t << "<center>"; // non docbook
    }
    void endCenter() 
    {
      m_t << "</center>"; // non docbook
    }
    void startSmall() 
    {
      m_t << "<small>"; // non docbook
    }
    void endSmall() 
    {
      m_t << "</small>"; // non docbook
    }
    void startSubscript() 
    {
      m_t << "<subscript>";
    }
    void endSubscript() 
    {
      m_t << "</subscript>";
    }
    void startSuperscript() 
    {
      m_t << "<superscript>";
    }
    void endSuperscript() 
    {
      m_t << "</superscript>";
    }
    void startTable(int cols) 
    {
      m_t << "<table><tgroup cols=\"" << cols << "\"><tbody>\n";
    }
    void endTable() 
    {
      m_t << "</row>\n</tbody></tgroup></table>";
    }
    void nextTableRow() 
    {
      m_t << "<row><entry>";
    }
    void endTableRow() 
    {
      m_t << "</row>" << endl;
    }
    void nextTableColumn() 
    {
      m_t << "<entry>";
    }
    void endTableColumn() 
    {
      m_t << "</entry>";
    }

    void writeQuote()         { m_t << "\""; }
    void writeCopyright()     { m_t << "&copy;"; }
    void writeUmlaut(char c)  { m_t << "&" << c << "uml;"; }
    void writeAcute(char c)   { m_t << "&" << c << "acute;"; }
    void writeGrave(char c)   { m_t << "&" << c << "grave;"; }
    void writeCirc(char c)    { m_t << "&" << c << "circ;"; }
    void writeTilde(char c)   { m_t << "&" << c << "tilde;"; }
    void writeRing(char c)    { m_t << "&" << c << "ring;"; }
    void writeSharpS()        { m_t << "&szlig;"; }
    void writeCCedil(char c)  { m_t << "&" << c << "cedil;"; }

    void startTitle() 
    {
      m_t << "<title>";
    }
    void endTitle()   
    {
      m_t << "</title>" << endl;
    }
    void writeAnchor(const char *id,const char *name) 
    {
      m_t << "<anchor id=\"" << id << "_" << name << "\"/>";
    }
    void writeSectionRef(const char *,const char *id,
                         const char *name,const char *text) 
    {
      m_t << "<link linkend=\"" << id << "_" << name << "\">";
      docify(text);
      m_t << "</link>";
    }
    void writeSectionRefItem(const char *,const char *,const char *) 
    {
      m_t << "(writeSectionRefItem)";
    }
    void addIndexItem(const char *primaryie,const char *secondaryie) 
    {
      m_t << "<indexentry><primaryie>";
      docify(primaryie);
      m_t << "</primaryie><secondaryie>";
      docify(secondaryie);
      m_t << "</secondaryie></indexentry>";
    }
    void writeFormula(const char *id,const char *text) 
    {
      m_t << "<formula id=\"" << id << "\">"; // non Docbook
      docify(text);
      m_t << "</formula>";
    }
    void startImage(const char *name,const char *size,bool caption) 
    {
      m_t << "<image name=\"" << name << "\" size=\"" << size 
          << "\" caption=\"" << (caption ? "1" : "0") << "\">"; // non docbook 
    }
    void endImage(bool) 
    {
      m_t << "</image>";
    }
    void startTextLink(const char *name,const char *anchor) 
    {
      m_t << "<ulink url=\"" << name << "#" << anchor << "\">";
    }
    void endTextLink() 
    {
      m_t << "<ulink>";
    }
    void startPageRef() 
    {
    }
    void endPageRef(const char *,const char *) 
    {
    }
    void startCodeLine() 
    {
      m_t << "<linenumber>"; // non DocBook
    }
    void endCodeLine() 
    {
      m_t << "</linenumber>"; // non DocBook
    }
    void startCodeAnchor(const char *id) 
    {
      m_t << "<anchor id=\"" << id << "\">";
    }
    void endCodeAnchor() 
    {
      m_t << "</anchor>";
    }
    void startFontClass(const char *colorClass) 
    {
      m_t << "<highlight class=\"" << colorClass << "\""; // non DocBook
    }
    void endFontClass()
    {
      m_t << "</highlight>"; // non DocBook
    }
    void codify(const char *text) 
    {
      docify(text);
    }
    
    // Generator specific functions
    
    /*! Create a clone of this generator. Uses the copy constructor */
    OutputDocInterface *clone() 
    {
      return new XMLGenerator(this);
    }
    /*! Append the output written to generator \a g to this generator */
    void append(const OutputDocInterface *g) 
    {
      const XMLGenerator *xg = (const XMLGenerator *)g;

      //if (m_inPar && !mifgen->m_inParStart)
      //{
      //  endParMode();
      //}
      //else if (!m_inPar && mifgen->m_inParStart)
      //{
      //  startParMode();
      //}
      //printf("Appending \n>>>>\n`%s'\n<<<<\n and \n>>>>\n`%s'\n<<<<\n",getContents().data(),mifgen->getContents().data());
      m_t << xg->getContents();
      m_inParStack  = xg->m_inParStack;
      m_inListStack = xg->m_inListStack;
      m_inParamList = xg->m_inParamList;
    }
    /*! constructor. 
     */
    XMLGenerator() 
    {
      m_b.setBuffer(m_a);
      m_b.open( IO_WriteOnly );
      m_t.setDevice(&m_b);
      m_t.setEncoding(QTextStream::Latin1);
    }
    /*! copy constructor */
    XMLGenerator(const XMLGenerator *xg)
    {
      m_b.setBuffer(m_a);
      m_b.open( IO_WriteOnly );
      m_t.setDevice(&m_b);
      m_t.setEncoding(QTextStream::Latin1);

      // copy state variables
      m_inParStack  = xg->m_inParStack;
      m_inListStack = xg->m_inListStack;
      m_inParamList = xg->m_inParamList;
    } 
    /*! destructor */
    virtual ~XMLGenerator()
    {
    }
    /*! Returns the output written to this generator as a string */
    QCString getContents() const
    {
      QCString s;
      s.resize(m_a.size()+1);
      memcpy(s.data(),m_a.data(),m_a.size());
      s.at(m_a.size())='\0';
      return s;
    }
    
  private:
    // only one destination stream, so these do not have to be implemented
    void pushGeneratorState() {}
    void popGeneratorState() {}
    void disableAllBut(OutputGenerator::OutputType) {}
    void enableAll() {}
    void disableAll() {}
    void disable(OutputGenerator::OutputType) {}
    void enable(OutputGenerator::OutputType) {}
    bool isEnabled(OutputGenerator::OutputType) { return TRUE; }

    QTextStream m_t;  
    QByteArray m_a;
    QBuffer m_b;
    
    ValStack<bool> m_inParStack;  
    ValStack<bool> m_inListStack;
    bool m_inParamList;
};

void writeXMLDocBlock(QTextStream &t,
                      const QCString &fileName,
                      int lineNr,
                      const QCString &scope,
                      const QCString &name,
                      const QCString &text)
{
  XMLGenerator *xmlGen = new XMLGenerator;
  parseDoc(*xmlGen,
           fileName, // input definition file
           lineNr,   // input definition line
           scope,    // scope (which should not be linked to)
           name,     // member (which should not be linked to)
           text      // actual text
          );
  xmlGen->endParMode();
  t << xmlGen->getContents();
  delete xmlGen;
}



void generateXMLForMember(MemberDef *md,QTextStream &t,Definition *def)
{
  if (md->memberType()==MemberDef::EnumValue) return;

  QCString scopeName;
  if (md->getClassDef()) 
    scopeName=md->getClassDef()->name();
  else if (md->getNamespaceDef()) 
    scopeName=md->getNamespaceDef()->name();
    
  t << "      <memberdef kind=\"";
  //enum { define_t,variable_t,typedef_t,enum_t,function_t } xmlType = function_t;
  QCString memType;
  bool isFunc=FALSE;
  switch (md->memberType())
  {
    case MemberDef::Define:      memType="define";    break;
    case MemberDef::EnumValue:   ASSERT(0);           break;
    case MemberDef::Property:    memType="property";  break;
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
  t << memType << "\" id=\"";
  t << def->getOutputFileBase()
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
  t << "\">" << endl;
  
  if (md->memberType()!=MemberDef::Define &&
      md->memberType()!=MemberDef::Enumeration
     )
  {
    QCString typeStr = replaceAnonymousScopes(md->typeString());
    t << "        <type>";
    linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),typeStr);
    t << "</type>" << endl;
  }

  t << "        <name>";
  writeXMLString(t,md->name());
  t << "</name>" << endl;
  if (isFunc) //function
  {
    ArgumentList *declAl = new ArgumentList;
    ArgumentList *defAl = md->argumentList();
    stringToArgumentList(md->argsString(),declAl);
    if (declAl->count()>0)
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
        t << "        </param>" << endl;
        if (defArg) ++defAli;
      }
    }
    delete declAl;
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
  // TODO: exceptions, const volatile
  if (md->memberType()==MemberDef::Enumeration) // enum
  {
    if (md->enumFieldList())
    {
      MemberListIterator emli(*md->enumFieldList());
      MemberDef *emd;
      for (emli.toFirst();(emd=emli.current());++emli)
      {
        t << "        <enumvalue>" << endl;
        t << "          <name>";
        writeXMLString(t,emd->name());
        t << "</name>" << endl;
        if (!emd->initializer().isEmpty())
        {
          t << "          <initializer>";
          writeXMLString(t,emd->initializer());
          t << "</initializer>" << endl;
        }
        t << "        </enumvalue>" << endl;
      }
    }
  }
  t << "        <briefdescription>" << endl;
  writeXMLDocBlock(t,md->getDefFileName(),md->getDefLine(),scopeName,md->name(),md->briefDescription());
  t << "        </briefdescription>" << endl;
  t << "        <detaileddescription>" << endl;
  writeXMLDocBlock(t,md->getDefFileName(),md->getDefLine(),scopeName,md->name(),md->documentation());
  t << "        </detaileddescription>" << endl;
  t << "      </memberdef>" << endl;
}


void generateXMLClassSection(ClassDef *cd,QTextStream &t,MemberList *ml,const char *kind)
{
  if (ml->count()>0)
  {
    t << "    <sectiondef kind=\"" << kind << "\">" << endl;
    //t << "          <memberlist>" << endl;
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      generateXMLForMember(md,t,cd);
    }
    //t << "          </memberlist>" << endl;
    t << "    </sectiondef>" << endl;
  }
}

void generateXMLForClass(ClassDef *cd,QTextStream &t)
{
  // brief description
  // detailed description
  // template arguments
  // include files
  // inheritance diagram
  // list of direct super classes
  // list of direct sub classes
  // collaboration diagram
  // list of all members
  // user defined member sections
  // standard member sections
  // detailed documentation
  // detailed member documentation
  
  if (cd->name().find('@')!=-1) return; // skip anonymous compounds
  t << "  <compounddef id=\"" 
    << cd->getOutputFileBase() << "\" kind=\"" 
    << cd->compoundTypeString() << "\">" << endl;
  t << "    <compoundname>"; 
  writeXMLString(t,cd->name()); 
  t << "</compoundname>" << endl;
  if (cd->baseClasses()->count()>0)
  {
    //t << "      <basecompoundlist>" << endl;
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "    <basecompoundref idref=\"" 
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
      t << "\"/>" << endl;
    }
    //t << "      </basecompoundlist>" << endl;
  }
  if (cd->subClasses()->count()>0)
  {
    //t << "      <derivedcompoundlist>" << endl;
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "    <derivedcompoundref idref=\"" 
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
      t << "\"/>" << endl;
    }
    //t << "      </derivedcompoundlist>" << endl;
  }
  int numMembers = 
    cd->pubTypes.count()+cd->pubMembers.count()+cd->pubAttribs.count()+
    cd->pubSlots.count()+cd->signals.count()+cd->dcopMethods.count()+
    cd->pubStaticMembers.count()+
    cd->pubStaticAttribs.count()+cd->proTypes.count()+cd->proMembers.count()+
    cd->proAttribs.count()+cd->proSlots.count()+cd->proStaticMembers.count()+
    cd->proStaticAttribs.count()+cd->priTypes.count()+cd->priMembers.count()+
    cd->priAttribs.count()+cd->priSlots.count()+cd->priStaticMembers.count()+
    cd->priStaticAttribs.count()+cd->friends.count()+cd->related.count();
  if (numMembers>0)
  {
    //t << "      <sectionlist>" << endl;
    generateXMLClassSection(cd,t,&cd->pubTypes,"public-type");
    generateXMLClassSection(cd,t,&cd->pubMembers,"public-func");
    generateXMLClassSection(cd,t,&cd->pubAttribs,"public-attrib");
    generateXMLClassSection(cd,t,&cd->pubSlots,"public-slot");
    generateXMLClassSection(cd,t,&cd->signals,"signal");
    generateXMLClassSection(cd,t,&cd->dcopMethods,"dcop-func");
    generateXMLClassSection(cd,t,&cd->properties,"property");
    generateXMLClassSection(cd,t,&cd->pubStaticMembers,"public-static-func");
    generateXMLClassSection(cd,t,&cd->pubStaticAttribs,"public-static-attrib");
    generateXMLClassSection(cd,t,&cd->proTypes,"protected-type");
    generateXMLClassSection(cd,t,&cd->proMembers,"protected-func");
    generateXMLClassSection(cd,t,&cd->proAttribs,"protected-attrib");
    generateXMLClassSection(cd,t,&cd->proSlots,"protected-slot");
    generateXMLClassSection(cd,t,&cd->proStaticMembers,"protected-static-func");
    generateXMLClassSection(cd,t,&cd->proStaticAttribs,"protected-static-attrib");
    generateXMLClassSection(cd,t,&cd->priTypes,"private-type");
    generateXMLClassSection(cd,t,&cd->priMembers,"private-func");
    generateXMLClassSection(cd,t,&cd->priAttribs,"private-attrib");
    generateXMLClassSection(cd,t,&cd->priSlots,"private-slot");
    generateXMLClassSection(cd,t,&cd->priStaticMembers,"private-static-func");
    generateXMLClassSection(cd,t,&cd->priStaticAttribs,"private-static-attrib");
    generateXMLClassSection(cd,t,&cd->friends,"signal");
    generateXMLClassSection(cd,t,&cd->related,"related");
    //t << "      </sectionlist>" << endl;
  }
  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,cd->getDefFileName(),cd->getDefLine(),cd->name(),0,cd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,cd->getDefFileName(),cd->getDefLine(),cd->name(),0,cd->documentation());
  t << "    </detaileddescription>" << endl;
  t << "  </compounddef>" << endl;
}

void generateXMLFileSection(FileDef *fd,QTextStream &t,MemberList *ml,const char *kind)
{
  if (ml->count()>0)
  {
    t << "      <sectiondef kind=\"" << kind << "\">" << endl;
    //t << "          <memberlist>" << endl;
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      generateXMLForMember(md,t,fd);
    }
    //t << "          </memberlist>" << endl;
    t << "      </sectiondef>" << endl;
  }
}

void generateXMLForFile(FileDef *fd,QTextStream &t)
{
  t << "  <compounddef id=\"" 
    << fd->getOutputFileBase() << "\" kind=\"file\">" << endl;
  t << "    <compoundname>";
  writeXMLString(t,fd->name());
  t << "</compoundname>" << endl;
  int numMembers = fd->decDefineMembers.count()+fd->decProtoMembers.count()+
                   fd->decTypedefMembers.count()+fd->decEnumMembers.count()+
                   fd->decFuncMembers.count()+fd->decVarMembers.count();
  if (numMembers>0)
  {
    //t << "      <sectionlist>" << endl;
    generateXMLFileSection(fd,t,&fd->decDefineMembers,"define");
    generateXMLFileSection(fd,t,&fd->decProtoMembers,"prototype");
    generateXMLFileSection(fd,t,&fd->decTypedefMembers,"typedef");
    generateXMLFileSection(fd,t,&fd->decEnumMembers,"enum");
    generateXMLFileSection(fd,t,&fd->decFuncMembers,"func");
    generateXMLFileSection(fd,t,&fd->decVarMembers,"var");
    //t << "      </sectionlist>" << endl;
  }
  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,fd->getDefFileName(),fd->getDefLine(),0,0,fd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,fd->getDefFileName(),fd->getDefLine(),0,0,fd->documentation());
  t << "    </detaileddescription>" << endl;
  t << "  </compounddef>" << endl;
}


void generateXML()
{
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
  
  QCString fileName=outputDirectory+"/xml/doxygen.xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t << "<?xml version='1.0' encoding='ISO-8859-1' standalone='yes'?>" << endl;
  //t << "<!DOCTYPE doxygen SYSTEM \"doxygen.dtd\">" << endl;
  t << "<doxygen>" << endl;
  if (Doxygen::classSDict.count()+Doxygen::inputNameList.count()>0)
  {
    //t << "  <compoundlist>" << endl;
    ClassSDict::Iterator cli(Doxygen::classSDict);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      generateXMLForClass(cd,t);
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
    //t << "  </compoundlist>" << endl;
  }
  t << "</doxygen>" << endl;
}


