/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
#include "dot.h"
#include "code.h"
#include "page.h"
#include "filename.h"
#include "version.h"
#include "xmldocvisitor.h"
#include "docparser.h"

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

// no debug info
#define XML_DB(x) do {} while(0)

// debug to stdout
//#define XML_DB(x) printf x

// debug inside output
//#define XML_DB(x) QCString __t;__t.sprintf x;m_t << __t

// static QCString sectionTypeToString(BaseOutputDocInterface::SectionTypes t)
// {
//   switch (t)
//   {
//     case BaseOutputDocInterface::See:         return "see";
//     case BaseOutputDocInterface::Return:      return "return";
//     case BaseOutputDocInterface::Author:      return "author";
//     case BaseOutputDocInterface::Version:     return "version";
//     case BaseOutputDocInterface::Since:       return "since";
//     case BaseOutputDocInterface::Date:        return "date";
//     case BaseOutputDocInterface::Bug:         return "bug";
//     case BaseOutputDocInterface::Note:        return "note";
//     case BaseOutputDocInterface::Warning:     return "warning";
//     case BaseOutputDocInterface::Par:         return "par";
//     case BaseOutputDocInterface::Deprecated:  return "deprecated";
//     case BaseOutputDocInterface::Pre:         return "pre";
//     case BaseOutputDocInterface::Post:        return "post";
//     case BaseOutputDocInterface::Invar:       return "invariant";
//     case BaseOutputDocInterface::Remark:      return "remark";
//     case BaseOutputDocInterface::Attention:   return "attention";
//     case BaseOutputDocInterface::Todo:        return "todo";
//     case BaseOutputDocInterface::Test:        return "test";
//     case BaseOutputDocInterface::RCS:         return "rcs";
//     case BaseOutputDocInterface::EnumValues:  return "enumvalues";
//     case BaseOutputDocInterface::Examples:    return "examples";
//   }
//   return "illegal";
// }


inline void writeXMLString(QTextStream &t,const char *s)
{
  t << convertToXML(s);
}

static void writeXMLHeader(QTextStream &t)
{
  QCString dtdName = Config_getString("XML_DTD");
  QCString schemaName = Config_getString("XML_SCHEMA");
  t << "<?xml version='1.0' encoding='ISO-8859-1' standalone='";
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
    virtual ~XMLCodeGenerator() {}
    
    void codify(const char *text) 
    {
      XML_DB(("(codify \"%s\")\n",text));
      if (m_insideCodeLine && !m_insideSpecialHL && m_normalHLNeedStartTag)
      {
        m_t << "<highlight class=\"normal\">";
        m_normalHLNeedStartTag=FALSE;
      }
      writeXMLString(m_t,text);
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

// /*! This class is used by the documentation parser.
//  *  Its methods are called when some XML text or markup
//  *  needs to be written.
//  */
// class XMLGenerator : public OutputDocInterface
// {
//   public:
// 
//     // helper functions
//     
//     void startParMode()
//     {
//       if (!m_inParStack.isEmpty() && !m_inParStack.top())
//       {
//         m_inParStack.top() = TRUE;
//         m_t << "<para>";
//         XML_DB(("start par at level=%d\n",m_inParStack.count()));
//       }
//       else if (m_inParStack.isEmpty())
//       {
//         m_inParStack.push(TRUE);
//         m_t << "<para>";
//         XML_DB(("start par at level=%d\n",m_inParStack.count()));
//       }
//     }
//     void endParMode()
//     {
//       if (!m_inParStack.isEmpty() && m_inParStack.top())
//       {
//         m_inParStack.top() = FALSE;
//         m_t << "</para>" << endl;
//         XML_DB(("end par at level=%d\n",m_inParStack.count()));
//       }
//     }
//     void startNestedPar()
//     {
//       m_inParStack.push(FALSE);
//       XML_DB(("enter par level=%d\n",m_inParStack.count()));
//     }
//     void endNestedPar()
//     {
//       XML_DB(("leave par level=%d\n",m_inParStack.count()));
//       if (m_inParStack.pop())
//       {
//         m_t << "</para>" << endl;
//       }
//       else
//       {
//         //XML_DB(("ILLEGAL par level!\n"));
//       }
//     }
//   
//     // Standard generator functions to be implemented by all generators
// 
//     void docify(const char *s) 
//     {
//       if (m_outputEnabled)
//       {
//         XML_DB(("(docify \"%s\")\n",s));
//         startParMode();
//         writeXMLString(m_t,s);
//       }
//     }
//     void writeChar(char c) 
//     {
//       char s[2];s[0]=c;s[1]=0;
//       docify(s);
//     }
//     void writeString(const char *text) 
//     {
//       //m_t << text;
//       docify(text);
//     }
//     void startItemList()       
//     { 
//       XML_DB(("(startItemList)\n"));
//       startParMode();
//       m_t << "<itemizedlist>" << endl;; 
//       m_inListStack.push(TRUE);
//     }
//     void startEnumList()       
//     { 
//       XML_DB(("(startEnumList)\n"));
//       startParMode();
//       m_t << "<orderedlist>"; 
//       m_inListStack.push(TRUE);
//     }
//     void writeListItem()       
//     { 
//       XML_DB(("(writeListItem)\n"));
//       if (!m_inListStack.isEmpty() && m_inListStack.top()) // first element
//       {
//         m_inListStack.top()=FALSE;
//       }
//       else // not first element, end previous element first
//       {
//         endParMode();
//         endNestedPar();
//         m_t << "</listitem>" << endl; 
//       }
//       m_t << "<listitem>"; 
//       startNestedPar();
//     }
//     void endItemList()         
//     {
//       XML_DB(("(endItemList)\n"));
//       if (!m_inListStack.isEmpty() && !m_inListStack.pop()) // first element
//       {
//         endParMode(); 
//         endNestedPar();
//         m_t << "</listitem>" << endl; 
//       }
//       m_t << "</itemizedlist>" << endl; 
//     }
//     void endEnumList()         
//     { 
//       XML_DB(("(endEnumList)\n"));
//       if (!m_inListStack.isEmpty() && !m_inListStack.pop()) // first element
//       {
//         endParMode(); 
//         m_t << "</listitem>" << endl; 
//         endNestedPar(); 
//       }
//       m_t << "</orderedlist>" << endl; 
//     }
//     void newParagraph()        
//     { 
//       XML_DB(("(newParagraph)\n"));
//       endParMode();
//       startParMode();
//     }
//     void startBold()           
//     { 
//       XML_DB(("(startBold)\n"));
//       startParMode();
//       m_t << "<bold>"; // non DocBook
//     }
//     void endBold()             
//     { 
//       XML_DB(("(endBold)\n"));
//       m_t << "</bold>"; // non DocBook
//     }
//     void startTypewriter()     
//     { 
//       XML_DB(("(startTypewriter)\n"));
//       startParMode();
//       m_t << "<computeroutput>";
//     }
//     void endTypewriter()       
//     { 
//       XML_DB(("(endTypewriter)\n"));
//       m_t << "</computeroutput>";
//     }
//     void startEmphasis()       
//     { 
//       XML_DB(("(startEmphasis)\n"));
//       startParMode();
//       m_t << "<emphasis>";
//     }
//     void endEmphasis()         
//     { 
//       XML_DB(("(endEmphasis)\n"));
//       m_t << "</emphasis>";
//     }
//     void startCodeFragment()   
//     { 
//       XML_DB(("(startCodeFragment)\n"));
//       startParMode();
//       m_t << "<programlisting>";
//     }
//     void endCodeFragment()     
//     { 
//       XML_DB(("(endCodeFragment)\n"));
//       m_t << "</programlisting>"; 
//     }
//     void startPreFragment()    
//     { 
//       XML_DB(("(startPreFragment)\n"));
//       startParMode();
//       m_t << "<preformatted>";
//     }
//     void endPreFragment()      
//     { 
//       XML_DB(("(endPreFragment)\n"));
//       m_t << "</preformatted>"; 
//     }
//     void startVerbatimFragment()    
//     { 
//       XML_DB(("(startVerbatimFragment)\n"));
//       startParMode();
//       m_t << "<preformatted>";
//     }
//     void endVerbatimFragment()      
//     { 
//       XML_DB(("(endVerbatimFragment)\n"));
//       m_t << "</preformatted>"; 
//     }
//     void writeRuler()          
//     { 
//       XML_DB(("(startParMode)\n"));
//       startParMode();
//       m_t << "<hruler/>"; 
//     }
//     void startDescription()    
//     { 
//       XML_DB(("(startDescription)\n"));
//       startParMode();
//       m_t << "<variablelist>"; 
//       m_inListStack.push(TRUE);
//     }
//     void endDescription()      
//     { 
//       XML_DB(("(endDescription)\n"));
//       if (!m_inListStack.isEmpty() && !m_inListStack.pop()) // first element
//       {
//         endNestedPar(); 
//         m_t << "</listitem>" << endl; 
//       }
//       m_t << "</variablelist>"; 
//       if (!m_inListStack.isEmpty()) m_inListStack.pop();
//     }
//     void startDescItem()       
//     { 
//       XML_DB(("(startDescItem)\n"));
//       if (!m_inListStack.isEmpty() && m_inListStack.top()) // first element
//       {
//         m_inListStack.top()=FALSE;
//       }
//       else // not first element, end previous element first
//       {
//         endNestedPar();
//         m_t << "</listitem>"; 
//       }
//       m_t << "<varlistentry><term>"; 
//     }
//     void endDescItem()         
//     { 
//       XML_DB(("(endDescItem)\n"));
//       m_t << "</term></varlistentry><listitem>"; 
//       startNestedPar();
//     }
//     void startDescList(SectionTypes st)       
//     { 
//       XML_DB(("(startDescList)\n"));
//       endParMode();
//       m_t << "<simplesect kind=\"" << sectionTypeToString(st);
//       m_t << "\"><title>"; 
//       startNestedPar();
//       m_inParStack.top() = TRUE;
//     }
//     void endDescList()         
//     { 
//       XML_DB(("(endDescList)\n"));
//       endNestedPar();
//       m_t << "</simplesect>";
//     }
//     void startSimpleSect(SectionTypes st,const char *,const char *,const char *)
//     {
//       XML_DB(("(startSimpleSect)\n"));
//       m_t << "<simplesect kind=\"" << sectionTypeToString(st) << "\">"; 
//       startNestedPar();
//     }
//     void endSimpleSect()         
//     {
//       XML_DB(("(endSimpleSect)\n"));
//       endNestedPar();
//       m_t << "</simplesect>";
//     }
//     void startParamList(ParamListTypes t,const char *)      
//     { 
//       XML_DB(("(startParamList)\n"));
//       startParMode();
//       QCString kind;
//       switch(t)
//       {
//         case Param:       kind="param";     break;
//         case RetVal:      kind="retval";    break;
//         case Exception:   kind="exception"; break;
//       }
//       m_t << "<parameterlist kind=\"" << kind << "\">"; // non DocBook
//       startNestedPar();
//       m_inParStack.top() = TRUE;
//       m_inParamList = TRUE;
//     }
//     void endParamList()        
//     { 
//       XML_DB(("(endParamList)\n"));
//       m_inParamList = FALSE;
//       m_t << "</parameterlist>";
//     }
//     void endDescTitle()        
//     { 
//       m_inParStack.top() = FALSE;
//       endNestedPar();
//       XML_DB(("(endDescTitle)\n"));
//       m_t << "</title>"; 
//       if (!m_inParamList) startNestedPar();
//     }
//     void writeDescItem()       
//     { 
//       XML_DB(("(writeDescItem)\n"));
//     }
//     void startDescTable()      
//     { 
//       XML_DB(("(startDescTable)\n"));
//     }
//     void endDescTable()        
//     { 
//       XML_DB(("(endDescTable)\n"));
//     }
//     void startDescTableTitle() 
//     { 
//       XML_DB(("(startDescTableTitle)\n"));
//       m_t << "<parametername>"; // non docbook
//     }
//     void endDescTableTitle()   
//     { 
//       XML_DB(("(endDescTableTitle)\n"));
//       m_t << "</parametername>"; // non docbook
//     }
//     void startDescTableData()  
//     { 
//       XML_DB(("(startDescTableData)\n"));
//       m_t << "<parameterdescription>"; // non docbook
//       startNestedPar();
//     }
//     void endDescTableData()    
//     { 
//       XML_DB(("(endDescTableData)\n"));
//       endNestedPar();
//       m_t << "</parameterdescription>"; // non docbook
//     }
//     void lineBreak()           
//     { 
//       XML_DB(("(lineBreak)\n"));
//       startParMode();
//       m_t << "<linebreak/>"; // non docbook
//     }
//     void writeNonBreakableSpace(int num) 
//     { 
//       XML_DB(("(writeNonBreakableSpace)\n"));
//       int i;for (i=0;i<num;i++) m_t << "&nbsp;"; 
//     }
//     
//     void writeObjectLink(const char *ref,const char *file,
//                          const char *anchor, const char *text) 
//     {
//       XML_DB(("(writeObjectLink)\n"));
//       startParMode();
//       writeXMLLink(m_t,ref,file,anchor,text);
//     }
//     void writeCodeLink(const char *ref,const char *file,
//                                const char *anchor,const char *text) 
//     {
//       XML_DB(("(writeCodeLink)\n"));
//       writeXMLLink(m_t,ref,file,anchor,text);
//     }
//     void startHtmlLink(const char *url)
//     {
//       XML_DB(("(startHtmlLink)\n"));
//       startParMode();
//       m_t << "<ulink url=\"" << url << "\">";
//     }
//     void endHtmlLink()
//     {
//       XML_DB(("(endHtmlLink)\n"));
//       m_t << "</ulink>";
//     }
//     void writeMailLink(const char *url) 
//     {
//       XML_DB(("(writeMailLink)\n"));
//       startParMode();
//       m_t << "<email>";
//       docify(url); 
//       m_t << "</email>";
//     }
//     void startSection(const char *id,const char *,SectionInfo::SectionType type) 
//     {
//       XML_DB(("(startSection)\n"));
//       endParMode();
//       m_t << "<sect";
//       switch(type)
//       {
//         case SectionInfo::Page:       m_t << "1"; break;
//         case SectionInfo::Section:    m_t << "2"; break;
//         case SectionInfo::Subsection: m_t << "3"; break;
//         default: ASSERT(0); break;
//       }
//       m_t << " id=\"" << id << "\">";
//       startNestedPar();
//       m_inParStack.top() = TRUE;
//     }
//     void endSection(const char *,SectionInfo::SectionType type)
//     {
//       XML_DB(("(endSection)\n"));
//       m_t << "</sect";
//       switch(type)
//       {
//         case SectionInfo::Page:       m_t << "1"; break;
//         case SectionInfo::Section:    m_t << "2"; break;
//         case SectionInfo::Subsection: m_t << "3"; break;
//         default: ASSERT(0); break;
//       }
//       m_t << ">";
//       m_inParStack.top() = FALSE;
//       endNestedPar();
//     }
//     void startSubsection() 
//     {
//       XML_DB(("(startSubsection)\n"));
//       endParMode();
//       m_t << "<sect2>";
//       startNestedPar();
//       m_inParStack.top() = TRUE;
//     }
//     void endSubsection() 
//     {
//       XML_DB(("(endSubsection)\n"));
//       m_t << "</sect2>";
//       m_inParStack.top() = FALSE;
//       endNestedPar();
//     }
//     void startSubsubsection() 
//     {
//       XML_DB(("(startSubsubsection)\n"));
//       endParMode();
//       m_t << "<sect3>";
//       startNestedPar();
//       m_inParStack.top() = TRUE;
//     }
//     void endSubsubsection() 
//     {
//       XML_DB(("(endSubsubsection)\n"));
//       m_t << "</sect3>";
//       m_inParStack.top() = FALSE;
//       endNestedPar();
//     }
//     void startCenter() 
//     {
//       XML_DB(("(startCenter)\n"));
//       startParMode();
//       m_t << "<center>"; // non docbook
//     }
//     void endCenter() 
//     {
//       XML_DB(("(endCenter)\n"));
//       m_t << "</center>"; // non docbook
//     }
//     void startSmall() 
//     {
//       XML_DB(("(startSmall)\n"));
//       startParMode();
//       m_t << "<small>"; // non docbook
//     }
//     void endSmall() 
//     {
//       XML_DB(("(endSmall)\n"));
//       m_t << "</small>"; // non docbook
//     }
//     void startSubscript() 
//     {
//       XML_DB(("(startSubscript)\n"));
//       startParMode();
//       m_t << "<subscript>";
//     }
//     void endSubscript() 
//     {
//       XML_DB(("(endSubscript)\n"));
//       m_t << "</subscript>";
//     }
//     void startSuperscript() 
//     {
//       XML_DB(("(startSuperscript)\n"));
//       startParMode();
//       m_t << "<superscript>";
//     }
//     void endSuperscript() 
//     {
//       XML_DB(("(endSuperscript)\n"));
//       m_t << "</superscript>";
//     }
//     void startTable(bool,int cols) 
//     {
//       XML_DB(("startTable\n"));
//       startParMode();
//       m_t << "<table cols=\"" << cols << "\">\n";
//     }
//     void endTable(bool hasCaption) 
//     {
//       XML_DB(("endTable\n"));
//       if (!hasCaption) m_t << "</row>\n";
//       m_t << "</table>";
//     }
//     void startCaption()
//     {
//       XML_DB(("startCaption"));
//       m_t << "</row><caption>";
//     }
//     void endCaption()
//     {
//       XML_DB(("encCaption"));
//       m_t << "</caption>";
//     }
//     void nextTableRow() 
//     {
//       XML_DB(("(nextTableRow)\n"));
//       m_t << "<row><entry>";
// 
//       // we need manually add a para here because cells are
//       // parsed before the table is generated, and thus
//       // are already parsed as if they are inside a paragraph.
//       m_t << "<para>";
//     }
//     void endTableRow() 
//     {
//       XML_DB(("(endTableRow)\n"));
//       m_t << "</row>" << endl;
//     }
//     void nextTableColumn() 
//     {
//       XML_DB(("(nextTableColumn)\n"));
//       m_t << "<entry>";
//       
//       // we need manually add a para here because cells are
//       // parsed before the table is generated, and thus
//       // are already parsed as if they are inside a paragraph.
//       m_t << "<para>";
//     }
//     void endTableColumn() 
//     {
//       XML_DB(("(endTableColumn)\n"));
//       // we need manually add a para here because cells are
//       // parsed before the table is generated, and thus
//       // are already parsed as if they are inside a paragraph.
//       m_t << "</para>";
//       m_t << "</entry>";
//     }
// 
//     void writeQuote()         { m_t << "\""; }
//     void writeCopyright()     { m_t << "&copy;"; }
//     void writeUmlaut(char c)  { m_t << "&" << c << "uml;"; }
//     void writeAcute(char c)   { m_t << "&" << c << "acute;"; }
//     void writeGrave(char c)   { m_t << "&" << c << "grave;"; }
//     void writeCirc(char c)    { m_t << "&" << c << "circ;"; }
//     void writeTilde(char c)   { m_t << "&" << c << "tilde;"; }
//     void writeRing(char c)    { m_t << "&" << c << "ring;"; }
//     void writeSharpS()        { m_t << "&szlig;"; }
//     void writeCCedil(char c)  { m_t << "&" << c << "cedil;"; }
// 
//     void startTitle() 
//     {
//       XML_DB(("(startTitle)\n"));
//       m_t << "<title>";
//       startNestedPar();
//       m_inParStack.top() = TRUE;
//     }
//     void endTitle()   
//     {
//       m_inParStack.top() = FALSE;
//       endNestedPar();
//       XML_DB(("(endTitle)\n"));
//       m_t << "</title>" << endl;
//     }
//     void writeAnchor(const char *id,const char *name) 
//     {
//       XML_DB(("(writeAnchor)\n"));
//       startParMode();
//       m_t << "<anchor id=\"" << id << "_1" << name << "\"/>";
//     }
//     void writeSectionRef(const char *,const char *id,
//                          const char *name,const char *text) 
//     {
//       XML_DB(("(writeSectionRef)\n"));
//       startParMode();
//       m_t << "<link linkend=\"" << id << "_1" << name << "\">";
//       docify(text);
//       m_t << "</link>";
//     }
//     void writeSectionRefItem(const char *,const char *,const char *) 
//     {
//       m_t << "(writeSectionRefItem)";
//     }
//     void addIndexItem(const char *primaryie,const char *secondaryie) 
//     {
//       XML_DB(("(addIndexItem)\n"));
//       startParMode();
//       m_t << "<indexentry><primaryie>";
//       docify(primaryie);
//       m_t << "</primaryie><secondaryie>";
//       docify(secondaryie);
//       m_t << "</secondaryie></indexentry>";
//     }
//     void writeFormula(const char *id,const char *text) 
//     {
//       XML_DB(("(writeFormula)\n"));
//       startParMode();
//       m_t << "<formula id=\"" << id << "\">"; // non Docbook
//       docify(text);
//       m_t << "</formula>";
//     }
//     void startImage(const char *name,const char *size,bool /*caption*/) 
//     {
//       XML_DB(("(startImage)\n"));
//       startParMode();
//       m_t << "<image name=\"" << name << "\"";
//       if (size) m_t << " size=\"" << size << "\"";
//       m_t << ">"; // non docbook 
//     }
//     void endImage(bool) 
//     {
//       XML_DB(("(endImage)\n"));
//       m_t << "</image>";
//     }
//     void startDotFile(const char *name,bool /*caption*/) 
//     {
//       XML_DB(("(startDotFile)\n"));
//       startParMode();
//       m_t << "<dotfile name=\"" << name << "\">"; // non docbook 
//     }
//     void endDotFile(bool) 
//     {
//       XML_DB(("(endDotFile)\n"));
//       m_t << "</dotfile>";
//     }
//     void startTextLink(const char *name,const char *anchor) 
//     {
//       XML_DB(("(startTextLink)\n"));
//       startParMode();
//       m_t << "<ulink url=\"" << name << "#" << anchor << "\">";
//     }
//     void endTextLink() 
//     {
//       XML_DB(("(endTextLink)\n"));
//       m_t << "</ulink>";
//     }
//     void startPageRef() 
//     {
//       XML_DB(("(startPageRef)\n"));
//       m_outputEnabled = FALSE;
//     }
//     void endPageRef(const char *,const char *) 
//     {
//       XML_DB(("(endPageRef)\n"));
//       m_outputEnabled = TRUE;
//     }
//     void writeLineNumber(const char *extRef,const char *compId,
//                          const char *anchorId,int l)
//     {
//       XML_DB(("(writeLineNumber)\n"));
//       m_t << "<linenumber";
//       m_t << " line=\"" << l << "\"";
//       if (compId)
//       {
//         m_t << " refid=\"" << compId;
//         if (anchorId) m_t << "_1" << anchorId; 
//         m_t << "\" kindref=\"";
//         if (anchorId) m_t << "member"; else m_t << "compound"; 
//         m_t << "\"";
//         if (extRef) m_t << " external=\"" << extRef << "\"";
//       }
//       m_t << "/>";
//     }
//     void startCodeLine() 
//     {
//       XML_DB(("(startCodeLine)\n"));
//       startParMode();
//       m_t << "<codeline>"; // non DocBook
//     }
//     void endCodeLine() 
//     {
//       XML_DB(("(endCodeLine)\n"));
//       m_t << "</codeline>" << endl; // non DocBook
//     }
//     void startCodeAnchor(const char *id) 
//     {
//       XML_DB(("(startCodeAnchor)\n"));
//       startParMode();
//       m_t << "<anchor id=\"" << id << "\">";
//     }
//     void endCodeAnchor() 
//     {
//       XML_DB(("(endCodeAnchor)\n"));
//       m_t << "</anchor>";
//     }
//     void startFontClass(const char *colorClass) 
//     {
//       XML_DB(("(startFontClass)\n"));
//       m_t << "<highlight class=\"" << colorClass << "\">"; // non DocBook
//     }
//     void endFontClass()
//     {
//       XML_DB(("(endFontClass)\n"));
//       m_t << "</highlight>"; // non DocBook
//     }
//     void codify(const char *text) 
//     {
//       XML_DB(("(codify \"%s\")\n",text));
//       docify(text);
//     }
//     void startHtmlOnly()  
//     {
//       XML_DB(("(startHtmlOnly)\n"));
//       m_t << "<htmlonly>" << endl;
//     }
//     void endHtmlOnly()    
//     {
//       XML_DB(("(endHtmlOnly)\n"));
//       m_t << "</htmlonly>" << endl;
//     }
//     void startLatexOnly() 
//     {
//       XML_DB(("(startLatexOnly)\n"));
//       m_t << "<latexonly>" << endl;
//     }
//     void endLatexOnly()   
//     {
//       XML_DB(("(endLatexOnly)\n"));
//       m_t << "</latexonly>" << endl;
//     }
//     void startSectionRefList()
//     {
//       XML_DB(("(startSectionRefList)\n"));
//     }
//     void endSectionRefList()
//     {
//       XML_DB(("(endSectionRefList)\n"));
//     }
//     void writeCodeAnchor(const char *)
//     {
//       XML_DB(("(writeCodeAnchor)\n"));
//     }
//     
//     // Generator specific functions
//     
//     /*! Create a clone of this generator. Uses the copy constructor */
//     OutputDocInterface *clone() 
//     {
//       return new XMLGenerator(this);
//     }
//     /*! Append the output written to generator \a g to this generator */
//     void append(const OutputDocInterface *g) 
//     {
//       const XMLGenerator *xg = (const XMLGenerator *)g;
// 
//       //printf("Appending \n>>>>\n`%s'\n<<<<\n and \n>>>>\n`%s'\n<<<<\n",getContents().data(),xg->getContents().data());
//       m_t << xg->getContents();
//       m_inParStack  = xg->m_inParStack;
//       m_inListStack = xg->m_inListStack;
//       m_inParamList = xg->m_inParamList;
//     }
//     /*! constructor. 
//      */
//     XMLGenerator() 
//     {
//       m_b.setBuffer(m_a);
//       m_b.open( IO_WriteOnly );
//       m_t.setDevice(&m_b);
//       m_t.setEncoding(QTextStream::Latin1);
//       m_inParamList = FALSE;
//       m_outputEnabled = TRUE;
//     }
//     /*! copy constructor */
//     XMLGenerator(const XMLGenerator *xg)
//     {
//       m_b.setBuffer(m_a);
//       m_b.open( IO_WriteOnly );
//       m_t.setDevice(&m_b);
//       m_t.setEncoding(QTextStream::Latin1);
// 
//       //printf("Cloning >>%s<< m_parStack.count()=%d\n",
//       //    xg->getContents().data(),xg->m_inParStack.count());
//        
//       // copy state variables
//       m_inParStack  = xg->m_inParStack;
//       m_inListStack = xg->m_inListStack;
//       m_inParamList = xg->m_inParamList;
//       m_outputEnabled = xg->m_outputEnabled;
//     } 
//     /*! destructor */
//     virtual ~XMLGenerator()
//     {
//     }
//     /*! Returns the output written to this generator as a string */
//     QCString getContents() const
//     {
//       QCString s;
//       s.resize(m_a.size()+1);
//       memcpy(s.data(),m_a.data(),m_a.size());
//       s.at(m_a.size())='\0';
//       return s;
//     }
//     
//   private:
//     // only one destination stream, so these do not have to be implemented
//     void pushGeneratorState() {}
//     void popGeneratorState() {}
//     void disableAllBut(OutputGenerator::OutputType) {}
//     void enableAll() {}
//     void disableAll() {}
//     void disable(OutputGenerator::OutputType) {}
//     void enable(OutputGenerator::OutputType) {}
//     bool isEnabled(OutputGenerator::OutputType) { return TRUE; }
// 
//     QTextStream m_t;  
//     QByteArray m_a;
//     QBuffer m_b;
//     
//     ValStack<bool> m_inParStack;  
//     ValStack<bool> m_inListStack;
//     bool m_inParamList;
//     bool m_outputEnabled;
//     
//     friend void writeXMLCodeBlock(QTextStream &t,FileDef *fd);
// };

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
  if (text.isEmpty()) return;
  // convert the documentation string into an abstract syntax tree
  DocNode *root = validatingParseDoc(fileName,lineNr,scope,md,stext);
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
  
/* 
  XMLGenerator *xmlGen = new XMLGenerator;
  //xmlGen->startParMode();
  parseDoc(*xmlGen,
           fileName,  // input definition file
           lineNr,    // input definition line
           scope,     // scope (which should not be linked to)
           md,        // member (which should not be linked to)
           stext+"\n" // actual text
          );
  xmlGen->endParMode();
  t << xmlGen->getContents();
  delete xmlGen;
*/
}

void writeXMLCodeBlock(QTextStream &t,FileDef *fd)
{
  initParseCodeContext();
  XMLCodeGenerator *xmlGen = new XMLCodeGenerator(t);
  //xmlGen->m_inParStack.push(TRUE);
  parseCode(*xmlGen,
            0,
            fileToString(fd->absFilePath(),Config_getBool("FILTER_SOURCE_FILES")),
            FALSE,
            0,
            fd);
  //t << xmlGen->getContents();
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
  Definition *d = rmd->getOuterScope();
  if (d==Doxygen::globalScope) d=rmd->getBodyDef();
  if (rmd->getStartBodyLine()!=-1 && rmd->getBodyDef()) 
    // link to definition in code
  {
    t << "        <" << tagName << " id=\"";
    t << rmd->getBodyDef()->getOutputFileBase()
      << "_1"      // encoded `:' character (see util.cpp:convertNameToFile)
      << rmd->anchor()
      << "\">" << convertToXML(name) << "</" 
      << tagName << ">" << endl;
  }
  else if (rmd->isLinkable() && d && d->isLinkable()) 
    // link to declaration in documentation (in absense of a definition)
  {
    t << "        <" << tagName << " id=\"";
    t << d->getOutputFileBase()
      << "_1"      // encoded `:' character (see util.cpp:convertNameToFile)
      << rmd->anchor()
      << "\">" << convertToXML(name) 
      << "</" << tagName << ">" << endl;
  } 
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
        t << "          <briefdescription>" << endl;
        writeXMLDocBlock(t,emd->getDefFileName(),emd->getDefLine(),scopeName,emd,emd->briefDescription());
        t << "          </briefdescription>" << endl;
        t << "          <detaileddescription>" << endl;
        writeXMLDocBlock(t,emd->getDefFileName(),emd->getDefLine(),scopeName,emd,emd->documentation());
        t << "          </detaileddescription>" << endl;
        t << "        </enumvalue>" << endl;
      }
    }
  }
  t << "        <briefdescription>" << endl;
  writeXMLDocBlock(t,md->getDefFileName(),md->getDefLine(),scopeName,md,md->briefDescription());
  t << "        </briefdescription>" << endl;
  t << "        <detaileddescription>" << endl;
  writeXMLDocBlock(t,md->getDefFileName(),md->getDefLine(),scopeName,md,md->documentation());
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
  generateXMLSection(cd,ti,t,&cd->pubMembers,"public-func");
  generateXMLSection(cd,ti,t,&cd->pubAttribs,"public-attrib");
  generateXMLSection(cd,ti,t,&cd->pubSlots,"public-slot");
  generateXMLSection(cd,ti,t,&cd->signals,"signal");
  generateXMLSection(cd,ti,t,&cd->dcopMethods,"dcop-func");
  generateXMLSection(cd,ti,t,&cd->properties,"property");
  generateXMLSection(cd,ti,t,&cd->pubStaticMembers,"public-static-func");
  generateXMLSection(cd,ti,t,&cd->pubStaticAttribs,"public-static-attrib");
  generateXMLSection(cd,ti,t,&cd->proTypes,"protected-type");
  generateXMLSection(cd,ti,t,&cd->proMembers,"protected-func");
  generateXMLSection(cd,ti,t,&cd->proAttribs,"protected-attrib");
  generateXMLSection(cd,ti,t,&cd->proSlots,"protected-slot");
  generateXMLSection(cd,ti,t,&cd->proStaticMembers,"protected-static-func");
  generateXMLSection(cd,ti,t,&cd->proStaticAttribs,"protected-static-attrib");
  generateXMLSection(cd,ti,t,&cd->priTypes,"private-type");
  generateXMLSection(cd,ti,t,&cd->priMembers,"private-func");
  generateXMLSection(cd,ti,t,&cd->priAttribs,"private-attrib");
  generateXMLSection(cd,ti,t,&cd->priSlots,"private-slot");
  generateXMLSection(cd,ti,t,&cd->priStaticMembers,"private-static-func");
  generateXMLSection(cd,ti,t,&cd->priStaticAttribs,"private-static-attrib");
  generateXMLSection(cd,ti,t,&cd->friends,"friend");
  generateXMLSection(cd,ti,t,&cd->related,"related");

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,cd->getDefFileName(),cd->getDefLine(),cd->name(),0,cd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,cd->getDefFileName(),cd->getDefLine(),cd->name(),0,cd->documentation());
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
  writeXMLDocBlock(t,nd->getDefFileName(),nd->getDefLine(),0,0,nd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,nd->getDefFileName(),nd->getDefLine(),0,0,nd->documentation());
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
  writeXMLDocBlock(t,fd->getDefFileName(),fd->getDefLine(),0,0,fd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,fd->getDefFileName(),fd->getDefLine(),0,0,fd->documentation());
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
  writeXMLDocBlock(t,gd->getDefFileName(),gd->getDefLine(),0,0,gd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,gd->getDefFileName(),gd->getDefLine(),0,0,gd->documentation());
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
  writeXMLHeader(t);
  t << "  <compounddef id=\"";
  if (Config_getBool("CASE_SENSE_NAMES")) t << pi->name; else t << pi->name.lower();
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


