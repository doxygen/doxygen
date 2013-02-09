/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
/*  This code is based on the work done by the MoxyPyDoxy team
 *  (Linda Leong, Mike Rivera, Kim Truong, and Gabriel Estrada)
 *  in Spring 2005 as part of CS 179E: Compiler Design Project
 *  at the University of California, Riverside; the course was
 *  taught by Peter H. Froehlich <phf@acm.org>.
 */


%{

#include <stdio.h>
#include <qvaluestack.h>

#include "pycode.h"
#include "message.h"

#include "scanner.h"
#include "entry.h"
#include "doxygen.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "config.h"
#include "groupdef.h"
#include "classlist.h"
#include "filedef.h"
#include "namespacedef.h"

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_INPUT 1

static ClassSDict    g_codeClassSDict(17);
static QCString      g_curClassName;
static QStrList      g_curClassBases;


static CodeOutputInterface * g_code;
static const char *  g_inputString;     //!< the code fragment as text
static int	     g_inputPosition;   //!< read offset during parsing 
static const char *  g_currentFontClass;
static bool          g_needsTermination;
static Definition   *g_searchCtx;
static int           g_inputLines;      //!< number of line in the code fragment
static int	     g_yyLineNr;        //!< current line number
static FileDef *     g_sourceFileDef;
static Definition *  g_currentDefinition;
static MemberDef *   g_currentMemberDef;
static bool          g_includeCodeFragment;
static QCString      g_realScope;
//static bool          g_insideBody;
static int           g_bodyCurlyCount;
static bool          g_searchingForBody;
static QCString      g_classScope;
static int           g_paramParens;
//static int           g_anchorCount;

static bool          g_exampleBlock;
static QCString      g_exampleName;
static QCString      g_exampleFile;

static QCString      g_type;
static QCString      g_name;

static bool          g_doubleStringIsDoc;
static bool          g_doubleQuote;
static bool          g_noSuiteFound;
static int           g_stringContext;

static QValueStack<uint> g_indents;  //!< Tracks indentation levels for scoping in python

static void endFontClass();
static void adjustScopesAndSuites(unsigned indentLength);


/*! Represents a stack of variable to class mappings as found in the
 *  code. Each scope is enclosed in pushScope() and popScope() calls.
 *  Variables are added by calling addVariables() and one can search
 *  for variable using findVariable().
 */
class PyVariableContext 
{
  public:
    static const ClassDef *dummyContext;    
    class Scope : public SDict<ClassDef> 
    {
      public:
	Scope() : SDict<ClassDef>(17) {}
    };
    
    PyVariableContext() 
    {
      m_scopes.setAutoDelete(TRUE);
    }

    virtual ~PyVariableContext() 
    {
    }
    
    void pushScope() 
    {
      m_scopes.append(new Scope);
    }

    void popScope() 
    {
      if (m_scopes.count()>0) 
      {
	m_scopes.remove(m_scopes.count()-1);
      }
    }

    void clear() 
    {
      m_scopes.clear();
      m_globalScope.clear();
    }

    void clearExceptGlobal() 
    {
      m_scopes.clear();
    }

    void addVariable(const QCString &type,const QCString &name);
    ClassDef *findVariable(const QCString &name);
    
  private:
    Scope        m_globalScope;
    QList<Scope> m_scopes;
};

void PyVariableContext::addVariable(const QCString &type,const QCString &name)
{
  //printf("PyVariableContext::addVariable(%s,%s)\n",type.data(),name.data());
  QCString ltype = type.simplifyWhiteSpace();
  QCString lname = name.simplifyWhiteSpace();

  Scope *scope = m_scopes.count()==0 ? &m_globalScope : m_scopes.getLast();
  ClassDef *varType;
  if (
      (varType=g_codeClassSDict[ltype]) ||  // look for class definitions inside the code block
      (varType=getResolvedClass(g_currentDefinition,g_sourceFileDef,ltype)) // look for global class definitions
     ) 
  {
    scope->append(lname,varType); // add it to a list
  }
  else 
  {
    if (m_scopes.count()>0) // for local variables add a dummy entry so the name 
                            // is hidden to avoid FALSE links to global variables with the same name
                            // TODO: make this work for namespaces as well!
    {
      scope->append(lname,dummyContext);
    }
  }
}

ClassDef *PyVariableContext::findVariable(const QCString &name)
{
  if (name.isEmpty()) return 0;
  ClassDef *result = 0;
  QListIterator<Scope> sli(m_scopes);
  Scope *scope;
  // search from inner to outer scope
  for (sli.toLast();(scope=sli.current());--sli)
  {
    result = scope->find(name);
    if (result) 
    {
      return result;
    }
  }
  // nothing found -> also try the global scope
  result=m_globalScope.find(name);
  return result;
}

static PyVariableContext g_theVarContext;
const ClassDef *PyVariableContext::dummyContext = (ClassDef*)0x8;

class PyCallContext
{
  public:
    struct Ctx
    {
      Ctx() : name(g_name), type(g_type), cd(0) {}
      QCString name;
      QCString type;
      ClassDef *cd;
    };

    PyCallContext() 
    {
      m_classList.append(new Ctx);
      m_classList.setAutoDelete(TRUE);
    }

    virtual ~PyCallContext() {}

    void setClass(ClassDef *cd)
    {
      Ctx *ctx = m_classList.getLast();
      if (ctx) 
      {
        ctx->cd=cd;
      }
    }
    void pushScope()
    {
      m_classList.append(new Ctx);
    }

    void popScope()
    {
      if (m_classList.count()>1)
      {
	Ctx *ctx = m_classList.getLast();
	if (ctx)
	{
	  g_name = ctx->name;
	  g_type = ctx->type;
	}
	m_classList.removeLast();
      }
      else
      {
      }
    }

    void clear()
    {
      m_classList.clear();
      m_classList.append(new Ctx);
    }

    ClassDef *getClass() const
    {
      Ctx *ctx = m_classList.getLast();

      if (ctx)
        return ctx->cd;
      else
        return 0;
    }

  private:
    QList<Ctx> m_classList;    
};

static PyCallContext g_theCallContext;


/*! counts the number of lines in the input */
static int countLines()
{
  const char *p=g_inputString;
  char c;
  int count=1;
  while ((c=*p)) 
  { 
    p++ ; 
    if (c=='\n') count++;  
  }
  if (p>g_inputString && *(p-1)!='\n') 
  { // last line does not end with a \n, so we add an extra
    // line and explicitly terminate the line after parsing.
    count++, 
    g_needsTermination=TRUE; 
  } 
  return count;
}

static void setCurrentDoc(const QCString &anchor)
{
  if (Doxygen::searchIndex)
  {
    if (g_searchCtx)
    {
      Doxygen::searchIndex->setCurrentDoc(g_searchCtx,g_searchCtx->anchor(),FALSE);
    }
    else
    {
      Doxygen::searchIndex->setCurrentDoc(g_sourceFileDef,anchor,TRUE);
    }
  }
}

static void addToSearchIndex(const char *text)
{
  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->addWord(text,FALSE);
  }
}


static ClassDef *stripClassName(const char *s,Definition *d=g_currentDefinition)
{
  int pos=0;
  QCString type = s;
  QCString className;
  QCString templSpec;
  while (extractClassNameFromType(type,pos,className,templSpec)!=-1)
  {
    QCString clName=className+templSpec;

    ClassDef *cd=0;
    if (!g_classScope.isEmpty())
    {
      cd=getResolvedClass(d,g_sourceFileDef,g_classScope+"::"+clName);
    }
    if (cd==0)
    {
      cd=getResolvedClass(d,g_sourceFileDef,clName);
    }
    if (cd)
    {
      return cd;
    }
  }

  return 0;
}



/*! start a new line of code, inserting a line number if g_sourceFileDef
 * is TRUE. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine()
{
  //if (g_currentFontClass) { g_code->endFontClass(); }
  if (g_sourceFileDef)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",g_yyLineNr);
    //lineAnchor.sprintf("l%05d",g_yyLineNr);
   
    Definition *d   = g_sourceFileDef->getSourceDefinition(g_yyLineNr);
    //printf("startCodeLine %d d=%p\n",g_yyLineNr,d);
    //g_code->startLineNumber();
    if (!g_includeCodeFragment && d && d->isLinkableInProject())
    {
      g_currentDefinition = d;
      g_currentMemberDef = g_sourceFileDef->getSourceMember(g_yyLineNr);
      //g_insideBody = FALSE;
      g_searchingForBody = TRUE;
      g_realScope = d->name().copy();
      g_classScope = d->name().copy();
      //printf("Real scope: `%s'\n",g_realScope.data());
      g_bodyCurlyCount = 0;
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",g_yyLineNr);
      if (g_currentMemberDef)
      {
        g_code->writeLineNumber(g_currentMemberDef->getReference(),
	                        g_currentMemberDef->getOutputFileBase(),
	                        g_currentMemberDef->anchor(),g_yyLineNr);
        setCurrentDoc(lineAnchor);
      }
      else
      {
        g_code->writeLineNumber(d->getReference(),
	                        d->getOutputFileBase(),
	                        0,g_yyLineNr);
        setCurrentDoc(lineAnchor);
      }
    }
    else
    {
      //g_code->codify(lineNumber);
      g_code->writeLineNumber(0,0,0,g_yyLineNr);
    }
    //g_code->endLineNumber();
  }
  g_code->startCodeLine(g_sourceFileDef); 
  if (g_currentFontClass)
  {
    g_code->startFontClass(g_currentFontClass);
  }
}

static void codify(const char* text) 
{ 
  g_code->codify(text);
}

static void endCodeLine()
{
  endFontClass();
  g_code->endCodeLine();
}

static void nextCodeLine()
{
  const char *fc = g_currentFontClass;
  endCodeLine();
  if (g_yyLineNr<g_inputLines) 
  {
    g_currentFontClass = fc;
    startCodeLine();
  }
}


/*! writes a link to a fragment \a text that may span multiple lines, inserting
 * line numbers for each line. If \a text contains newlines, the link will be 
 * split into multiple links with the same destination, one for each line.
 */
static void writeMultiLineCodeLink(CodeOutputInterface &ol,
                  const char *ref,const char *file,
                  const char *anchor,const char *text,
                  const char *tooltip)
{
  bool done=FALSE;
  char *p=(char *)text;
  while (!done)
  {
    char *sp=p;
    char c;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      g_yyLineNr++;
      *(p-1)='\0';
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      nextCodeLine();
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}


static void codifyLines(char *text)
{
  //printf("codifyLines(%d,\"%s\")\n",g_yyLineNr,text);
  char *p=text,*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      g_yyLineNr++;
      *(p-1)='\0';
      g_code->codify(sp);
      nextCodeLine();
    }
    else
    {
      g_code->codify(sp);
      done=TRUE;
    }
  }
}

static const char* fileLocation() 
{
  const int maxLen=4096;
  static char floc[maxLen+1];
  floc[maxLen]='\0';
  snprintf(floc,maxLen,"%s:%d:%d",g_sourceFileDef?g_sourceFileDef->absFilePath().data():"[unknown]",g_yyLineNr,1);
  return floc;
}

static void addDocCrossReference(MemberDef *src,MemberDef *dst)
{
  static bool referencedByRelation = Config_getBool("REFERENCED_BY_RELATION");
  static bool callerGraph =  Config_getBool("CALLER_GRAPH");
  static bool referencesRelation = Config_getBool("REFERENCES_RELATION");
  static bool callGraph = Config_getBool("CALL_GRAPH");
  if (dst->isTypedef() || dst->isEnumerate()) return; // don't add types
  //printf("addDocCrossReference src=%s,dst=%s\n",src->name().data(),dst->name().data());
  if ((referencedByRelation || callerGraph) && (src->isFunction() || src->isSlot()))
  {
    dst->addSourceReferencedBy(src,fileLocation());
  }
  if ((referencesRelation || callGraph) && (src->isFunction() || src->isSlot()))
  {
    src->addSourceReferences(dst,fileLocation());
  }
}



static bool getLinkInScope(const QCString &c,  // scope
                           const QCString &m,  // member
			   const char *memberText, // exact text
			   CodeOutputInterface &ol,
			   const char *text
			  )
{
  MemberDef    *md;
  ClassDef     *cd;
  FileDef      *fd;
  NamespaceDef *nd;
  GroupDef     *gd;
  //printf("Trying `%s'::`%s'\n",c.data(),m.data());
  if (getDefs(c,m,"()",md,cd,fd,nd,gd,FALSE,g_sourceFileDef) && 
      md->isLinkable())
  {
    //Definition *d=0;
    //if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else d=gd;

    Definition *d = md->getOuterScope()==Doxygen::globalScope ?
	            md->getBodyDef() : md->getOuterScope();
    //printf("Found! d=%s\n",d?d->name().data():"<none>");
    if (md->getGroupDef()) d = md->getGroupDef();
    if (d && d->isLinkable())
    {
      g_theCallContext.setClass(stripClassName(md->typeString(),md->getOuterScope()));
      //printf("g_currentDefinition=%p g_currentMemberDef=%p\n",
      //        g_currentDefinition,g_currentMemberDef);

      if (g_currentDefinition && g_currentMemberDef &&
	  md!=g_currentMemberDef)
      {
	addDocCrossReference(g_currentMemberDef,md);
      }
      //printf("d->getReference()=`%s' d->getOutputBase()=`%s' name=`%s' member name=`%s'\n",d->getReference().data(),d->getOutputFileBase().data(),d->name().data(),md->name().data());
     
      writeMultiLineCodeLink(ol,md->getReference(),
	                        md->getOutputFileBase(),
	                        md->anchor(),
				text ? text : memberText,
                                md->briefDescriptionAsTooltip());
      addToSearchIndex(text ? text : memberText);
      return TRUE;
    } 
  }
  return FALSE;
}

static bool getLink(const char *className,
                    const char *memberName,
		    CodeOutputInterface &ol,
		    const char *text=0)
{
  QCString m=removeRedundantWhiteSpace(memberName);
  QCString c=className;
  if (!getLinkInScope(c,m,memberName,ol,text))
  {
    if (!g_curClassName.isEmpty())
    {
      if (!c.isEmpty()) c.prepend("::");
      c.prepend(g_curClassName);
      return getLinkInScope(c,m,memberName,ol,text);
    }
    return FALSE;
  }
  return TRUE;
}


/*
  For a given string in the source code,
  finds its class or global id and links to it.
*/
static void generateClassOrGlobalLink(CodeOutputInterface &ol,char *clName,
                                      bool typeOnly=FALSE)
{
  QCString className=clName;

  // Don't do anything for empty text
  if (className.isEmpty()) return;

  DBG_CTX((stderr,"generateClassOrGlobalLink(className=%s)\n",className.data()));

  ClassDef *cd=0,*lcd=0;  /** Class def that we may find */
  MemberDef *md=0;        /** Member def that we may find */
  //bool isLocal=FALSE;

  if ((lcd=g_theVarContext.findVariable(className))==0) // not a local variable
  {
    Definition *d = g_currentDefinition;
    QCString scope = substitute(className,".","::");

    cd = getResolvedClass(d,g_sourceFileDef,substitute(className,".","::"),&md);

    DBG_CTX((stderr,"d=%s g_sourceFileDef=%s\n",
        d?d->displayName().data():"<null>",
        g_currentDefinition?g_currentDefinition->displayName().data():"<null>"));
    DBG_CTX((stderr,"is found as a type %s\n",cd?cd->name().data():"<null>"));

    if (cd==0 && md==0) // also see if it is variable or enum or enum value
    {
      NamespaceDef *nd = getResolvedNamespace(scope);
      if (nd)
      {
        writeMultiLineCodeLink(ol,nd->getReference(),nd->getOutputFileBase(),nd->anchor(),clName,nd->briefDescriptionAsTooltip());
        addToSearchIndex(className);
        return;
      }
      else if (getLink(g_classScope,clName,ol,clName))
      {
	return;
      }
    }
  }
  else
  {
    if (lcd!=PyVariableContext::dummyContext) 
    {
      g_theCallContext.setClass(lcd);
    }
    //isLocal=TRUE;
    DBG_CTX((stderr,"is a local variable cd=%p!\n",cd));
  }

  if (cd && cd->isLinkable()) // is it a linkable class
  {
    writeMultiLineCodeLink(ol,cd->getReference(),cd->getOutputFileBase(),cd->anchor(),clName,cd->briefDescriptionAsTooltip());
    addToSearchIndex(className);
    if (md)
    {
      Definition *d = md->getOuterScope()==Doxygen::globalScope ?
                      md->getBodyDef() : md->getOuterScope();
      if (md->getGroupDef()) d = md->getGroupDef();
      if (d && d->isLinkable() && md->isLinkable() && g_currentMemberDef)
      {
        addDocCrossReference(g_currentMemberDef,md);
      }
    }
  }
  else // not a class, maybe a global member
  {
    int scopeEnd = className.findRev(".");
    if (scopeEnd!=-1 && !typeOnly) // name with explicit scope
    {
      QCString scope = substitute(className.left(scopeEnd),".","::");
      QCString locName = className.right(className.length()-scopeEnd-1);
      ClassDef *mcd = getClass(scope);
      DBG_CTX((stderr,"scope=%s locName=%s mcd=%p\n",scope.data(),locName.data(),mcd));
      if (mcd)
      {
	MemberDef *md = mcd->getMemberByName(locName);
	if (md)
	{
          g_theCallContext.setClass(stripClassName(md->typeString(),md->getOuterScope()));
          writeMultiLineCodeLink(ol,md->getReference(),md->getOutputFileBase(),md->anchor(),clName,md->briefDescriptionAsTooltip());
          addToSearchIndex(className);
	  Definition *d = md->getOuterScope()==Doxygen::globalScope ?
	                  md->getBodyDef() : md->getOuterScope();
	  if (md->getGroupDef()) d = md->getGroupDef();
	  if (d && d->isLinkable() && md->isLinkable() && g_currentMemberDef)
	  {
	    addDocCrossReference(g_currentMemberDef,md);
	  }
	  return;
	}
      }
      else // check namespace as well
      {
        NamespaceDef *mnd = getResolvedNamespace(scope);
        if (mnd)
        {
  	  MemberDef *md=mnd->getMemberByName(locName);
	  if (md)
          {
	    //printf("name=%s scope=%s\n",locName.data(),scope.data());
	    g_theCallContext.setClass(stripClassName(md->typeString(),md->getOuterScope()));
            writeMultiLineCodeLink(ol,md->getReference(),md->getOutputFileBase(),md->anchor(),clName,md->briefDescriptionAsTooltip());
            addToSearchIndex(className);
	    Definition *d = md->getOuterScope()==Doxygen::globalScope ?
	                    md->getBodyDef() : md->getOuterScope();
	    if (md->getGroupDef()) d = md->getGroupDef();
	    if (d && d->isLinkable() && md->isLinkable() && g_currentMemberDef)
	    {
	      addDocCrossReference(g_currentMemberDef,md);
	    }
	    return;
          }
        }
      }
    }
    
    // nothing found, just write out the word
    codifyLines(clName);
    addToSearchIndex(clName);
  }
}

/*
   As of June 1, this function seems to work
   for file members, but scopes are not
   being correctly tracked for classes
   so it doesn't work for classes yet.

*/
static void generateFunctionLink(CodeOutputInterface &ol,char *funcName)
{
  //CodeClassDef *ccd=0;
  ClassDef *ccd=0;
  QCString locScope=g_classScope.copy();
  QCString locFunc=removeRedundantWhiteSpace(funcName);
  DBG_CTX((stdout,"*** locScope=%s locFunc=%s\n",locScope.data(),locFunc.data()));
  int i=locFunc.findRev("::");
  if (i>0)
  {
    locScope=locFunc.left(i);
    locFunc=locFunc.right(locFunc.length()-i-2).stripWhiteSpace();
  }
  //printf("generateFunctionLink(%s) classScope=`%s'\n",locFunc.data(),locScope.data());
  if (!locScope.isEmpty() && (ccd=g_codeClassSDict[locScope]))
  {
    //printf("using classScope %s\n",g_classScope.data());
    if (ccd->baseClasses())
    {
      BaseClassListIterator bcli(*ccd->baseClasses());
      for ( ; bcli.current() ; ++bcli)
      {
	if (getLink(bcli.current()->classDef->name(),locFunc,ol,funcName)) 
	{
	  return;
	}
      }
    }
  }
  if (!getLink(locScope,locFunc,ol,funcName))
  {
    generateClassOrGlobalLink(ol,funcName);
  }
  return;
}

static bool findMemberLink(CodeOutputInterface &ol,Definition *sym,const char *symName)
{
  //printf("sym %s outerScope=%s equal=%d\n",
  //    sym->name().data(),sym->getOuterScope()->name().data(),
  //    sym->getOuterScope()==g_currentDefinition);

  if (sym->getOuterScope() &&
      sym->getOuterScope()->definitionType()==Definition::TypeClass &&
      g_currentDefinition->definitionType()==Definition::TypeClass)
  {
    ClassDef *cd = (ClassDef*)sym->getOuterScope();
    ClassDef *thisCd = (ClassDef *)g_currentDefinition;
    QCString anchor=sym->anchor();
    if (sym->definitionType()==Definition::TypeMember)
    {
      if (g_currentMemberDef)
      {
	addDocCrossReference(g_currentMemberDef,(MemberDef*)sym);
      }
    }
    DBG_CTX((stderr,"cd=%s thisCd=%s\n",cd?cd->name().data():"<none>",thisCd?thisCd->name().data():"<none>"));

    // TODO: find the nearest base class in case cd is a base class of
    // thisCd 
    if (cd==thisCd || (thisCd && thisCd->isBaseClass(cd,TRUE)))
    {
      writeMultiLineCodeLink(ol,sym->getReference(),
          sym->getOutputFileBase(),
          anchor,
          symName,
          sym->briefDescriptionAsTooltip());
      return TRUE;
    }
  }
  return FALSE;
}

static void findMemberLink(CodeOutputInterface &ol,char *symName)
{
  //printf("Member reference: %s scope=%s member=%s\n",
  //    yytext,
  //    g_currentDefinition?g_currentDefinition->name().data():"<none>",
  //    g_currentMemberDef?g_currentMemberDef->name().data():"<none>"
  //    );
  if (g_currentDefinition)
  {
    DefinitionIntf *di = Doxygen::symbolMap->find(symName);
    if (di)
    {
      if (di->definitionType()==DefinitionIntf::TypeSymbolList) // multiple symbols
      {
	DefinitionListIterator dli(*(DefinitionList*)di);
	Definition *sym;
	for (dli.toFirst();(sym=dli.current());++dli)
	{
	  if (findMemberLink(ol,sym,symName)) return;
	}
      }
      else // single symbol
      {
	if (findMemberLink(ol,(Definition*)di,symName)) return;
      }
    }
  }
  //printf("sym %s not found\n",&yytext[5]);
  codify(symName);
}

static void startFontClass(const char *s)
{
  endFontClass();
  g_code->startFontClass(s);
  g_currentFontClass=s;
}

static void endFontClass()
{
  if (g_currentFontClass)
  {
    g_code->endFontClass();
    g_currentFontClass=0;
  }
}

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

static int yyread(char *buf,int max_size)
{
  int c=0;
  while( c < max_size && g_inputString[g_inputPosition] )
  {
    *buf = g_inputString[g_inputPosition++] ;
    c++; buf++;
  }
  return c;
}

%}


BB                [ \t]+
B                 [ \t]*
NEWLINE           \n

DIGIT             [0-9]
LETTER            [A-Za-z]
NONEMPTY          [A-Za-z0-9_]
EXPCHAR           [#(){}\[\],:.%/\\=`*~|&<>!;+-]
NONEMPTYEXP       [^ \t\n:]
PARAMNONEMPTY     [^ \t\n():]
IDENTIFIER        ({LETTER}|"_")({LETTER}|{DIGIT}|"_")*  
BORDER            ([^A-Za-z0-9])

POUNDCOMMENT      "#".*

TRISINGLEQUOTE    "'''"
TRIDOUBLEQUOTE    "\"\"\""
LONGSTRINGCHAR    [^\\"']
ESCAPESEQ         ("\\")(.)
LONGSTRINGITEM    ({LONGSTRINGCHAR}|{ESCAPESEQ})
SMALLQUOTE        ("\"\""|"\""|"'"|"''")
LONGSTRINGBLOCK   ({LONGSTRINGITEM}+|{SMALLQUOTE})

SHORTSTRING       ("'"{SHORTSTRINGITEM}*"'"|'"'{SHORTSTRINGITEM}*'"')
SHORTSTRINGITEM   ({SHORTSTRINGCHAR}|{ESCAPESEQ})
SHORTSTRINGCHAR   [^\\\n"]
STRINGLITERAL     {STRINGPREFIX}?( {SHORTSTRING} | {LONGSTRING})  
STRINGPREFIX      ("r"|"u"|"ur"|"R"|"U"|"UR"|"Ur"|"uR")
KEYWORD ("lambda"|"import"|"class"|"assert"|"as"|"from"|"global"|"def"|"True"|"False")
FLOWKW  ("or"|"and"|"is"|"not"|"print"|"for"|"in"|"if"|"try"|"except"|"yield"|"raise"|"break"|"continue"|"pass"|"if"|"return"|"while"|"elif"|"else"|"finally")
QUOTES            ("\""[^"]*"\"")
SINGLEQUOTES      ("'"[^']*"'")

LONGINTEGER       {INTEGER}("l"|"L")
INTEGER           ({DECIMALINTEGER}|{OCTINTEGER}|{HEXINTEGER})
DECIMALINTEGER    ({NONZERODIGIT}{DIGIT}*|"0")
OCTINTEGER        "0"{OCTDIGIT}+
HEXINTEGER        "0"("x"|"X"){HEXDIGIT}+  
NONZERODIGIT      [1-9]  
OCTDIGIT          [0-7]  
HEXDIGIT          ({DIGIT}|[a-f]|[A-F])
FLOATNUMBER       ({POINTFLOAT}|{EXPONENTFLOAT})
POINTFLOAT        ({INTPART}?{FRACTION}|{INTPART}".")  
EXPONENTFLOAT     ({INTPART}|{POINTFLOAT}){EXPONENT}
INTPART             {DIGIT}+  
FRACTION             "."{DIGIT}+  
EXPONENT             ("e"|"E")("+"|"-")?{DIGIT}+
IMAGNUMBER ({FLOATNUMBER}|{INTPART})("j"|"J")
ATOM              ({IDENTIFIER}|{LITERAL}|{ENCLOSURE})
ENCLOSURE             ({PARENTH_FORM}|{LIST_DISPLAY}|{DICT_DISPLAY}|{STRING_CONVERSION})
LITERAL             ({STRINGLITERAL}|{INTEGER}|{LONGINTEGER}|{FLOATNUMBER}|{IMAGNUMBER})
PARENTH_FORM       "("{EXPRESSION_LIST}?")"
TEST             ({AND_TEST}("or"{AND_TEST})*|{LAMBDA_FORM})
TESTLIST             {TEST}( ","{TEST})*","?
LIST_DISPLAY        "["{LISTMAKER}?"]"  
LISTMAKER             {EXPRESSION}({LIST_FOR}|(","{EXPRESSION})*","?)  
LIST_ITER             ({LIST_FOR}|{LIST_IF})  
LIST_FOR             "for"{EXPRESSION_LIST}"in"{TESTLIST}{LIST_ITER}?
LIST_IF             "if"{TEST}{LIST_ITER}?
DICT_DISPLAY             "\{"{KEY_DATUM_LIST}?"\}"
KEY_DATUM_LIST       {KEY_DATUM}(","{KEY_DATUM})*","? 
KEY_DATUM              {EXPRESSION}":"{EXPRESSION}
STRING_CONVERSION        "`"{EXPRESSION_LIST}"`"
PRIMARY             ({ATOM}|{ATTRIBUTEREF}|{SUBSCRIPTION}|{SLICING}|{CALL})
ATTRIBUTEREF             {PRIMARY}"."{IDENTIFIER}
SUBSCRIPTION             {PRIMARY}"["{EXPRESSION_LIST}"]"
SLICING            ({SIMPLE_SLICING}|{EXTENDED_SLICING})
SIMPLE_SLICING             {PRIMARY}"["{SHORT_SLICE}"]"  
EXTENDED_SLICING           {PRIMARY}"["{SLICE_LIST}"]" 
SLICE_LIST          {SLICE_ITEM}(","{SLICE_ITEM})*","?
SLICE_ITEM           ({EXPRESSION}|{PROPER_SLICE}|{ELLIPSIS})
PROPER_SLICE           ({SHORT_SLICE}|{LONG_SLICE})
SHORT_SLICE              {LOWER_BOUND}?":"{UPPER_BOUND}?  
LONG_SLICE             {SHORT_SLICE}":"{STRIDE}?
LOWER_BOUND             {EXPRESSION}  
UPPER_BOUND             {EXPRESSION}
STRIDE             {EXPRESSION}
ELLIPSIS             "..."
CALL             {PRIMARY}"("({ARGUMENT_LIST}","?)?")"
ARGUMENT_LIST       ({POSITIONAL_ARGUMENTS}(","{KEYWORD_ARGUMENTS})?(",""*"{EXPRESSION})?(",""**"{EXPRESSION})?|{KEYWORD_ARGUMENTS}(",""*"{EXPRESSION})?(",""**"{EXPRESSION})?|"*"{EXPRESSION}(",""**"{EXPRESSION})?|"**"{EXPRESSION})
POSITIONAL_ARGUMENTS             {EXPRESSION}(","{EXPRESSION})*
KEYWORD_ARGUMENTS              {KEYWORD_ITEM}(","{KEYWORD_ITEM})*
KEYWORD_ITEM           {IDENTIFIER}"="{EXPRESSION}
POWER             {PRIMARY}("**"{U_EXPR})?
U_EXPR            ({POWER}|"-"{U_EXPR}|"+"{U_EXPR}|"\~"{U_EXPR})
M_EXPR            ({U_EXPR}|{M_EXPR}"*"{U_EXPR}|{M_EXPR}"//"{U_EXPR}|{M_EXPR}"/"{U_EXPR}|{M_EXPR}"\%"{U_EXPR})
A_EXPR         ({M_EXPR}|{A_EXPR}"+"{M_EXPR}|{A_EXPR}"-"{M_EXPR}
SHIFT_EXPR            ({A_EXPR}|{SHIFT_EXPR}("<<"|">>"){A_EXPR})
AND_EXPR            ({SHIFT_EXPR}|{AND_EXPR}"\;SPMamp;"{SHIFT_EXPR}
XOR_EXPR            ({AND_EXPR}|{XOR_EXPR}"\textasciicircum"{AND_EXPR})
OR_EXPR            ({XOR_EXPR}|{OR_EXPR}"|"{ XOR_EXPR})

COMPARISON             {OR_EXPR}({COMP_OPERATOR}{OR_EXPR})*
COMP_OPERATOR         ("<"|">"|"=="|">="|"<="|"<>"|"!="|"is""not"?|"not"?"in")
EXPRESSION            ({OR_TEST}|{LAMBDA_FORM})
OR_TEST             ({AND_TEST}|{OR_TEST}"or"{AND_TEST})
AND_TEST          ({NOT_TEST}|{AND_TEST}"and"{NOT_TEST})
NOT_TEST           ({COMPARISON}|"not"{NOT_TEST})
LAMBDA_FORM       "lambda"{PARAMETER_LIST}?":"{EXPRESSION}
EXPRESSION_LIST      {EXPRESSION}(","{EXPRESSION})*","?
SIMPLE_STMT       ({EXPRESSION_STMT}|{ASSERT_STMT}|{ASSIGNMENT_STMT}|{AUGMENTED_ASSIGNMENT_STMT}|{PASS_STMT}|{DEL_STMT}|{PRINT_STMT}|{RETURN_STMT}|{YIELD_STMT}|{RAISE_STMT}|{BREAK_STMT}|{CONTINUE_STMT}|{IMPORT_STMT}|{GLOBAL_STMT}|{EXEC_STMT})
EXPRESSION_STMT     {EXPRESSION_LIST}
ASSERT_STMT             "assert"{EXPRESSION}(","{EXPRESSION})?
ASSIGNMENT_STMT     ({TARGET_LIST}"=")+{EXPRESSION_LIST}
TARGET_LIST             {TARGET}(","{TARGET})*","?
TARGET           ({IDENTIFIER}|"("{TARGET_LIST}")"|"["{TARGET_LIST}"]"|{ATTRIBUTEREF}|{SUBSCRIPTION}|{SLICING})


%option noyywrap
%option nounput

%x Body

%x FunctionDec
%x FunctionParams

%x ClassDec
%x ClassInheritance

%x Suite
%x SuiteCaptureIndent
%x SuiteStart
%x SuiteMaintain
%x SuiteContinuing

%x LongString

%x SingleQuoteString
%x DoubleQuoteString
%x TripleString

%%

<Body,Suite>{
      "def"{BB}                     {
				        startFontClass("keyword");
					codify(yytext);
					endFontClass();
					BEGIN( FunctionDec );
		                    }

      "class"{BB}                   {
				        startFontClass("keyword");
					codify(yytext);
					endFontClass();
					BEGIN( ClassDec );
		                    }
      "None"                        {
				        startFontClass("keywordtype");
					codify(yytext);
					endFontClass();
				    }
      "self."{IDENTIFIER}/"("       {
				        codify("self.");
				        findMemberLink(*g_code,&yytext[5]);
	                            }
      "self."{IDENTIFIER}           {
				        codify("self.");
				        findMemberLink(*g_code,&yytext[5]);
	                            }
}

<ClassDec>{IDENTIFIER}              {

					generateClassOrGlobalLink(*g_code,yytext);
					// codify(yytext);
					g_curClassName = yytext;
					g_curClassBases.clear();
					BEGIN( ClassInheritance );
		                    }

<ClassInheritance>{
   ({BB}|[(,)])                     {
					codify(yytext);
                                    }

   ({IDENTIFIER}".")*{IDENTIFIER}   {
                                        // The parser
					// is assuming
					// that ALL identifiers
					// in this state
					// are base classes;
					// it doesn't check to see
					// that the first parenthesis
					// has been seen.

					// This is bad - it should
					// probably be more strict
					// about what to accept.

                                        g_curClassBases.inSort(yytext);
					generateClassOrGlobalLink(*g_code,yytext);
					// codify(yytext);
                                    }

    ":"                             {
				      codify(yytext);

				      // Assume this will
				      // be a one-line suite;
				      // found counter-example
				      // in SuiteStart.

                                      // Push a class scope

                                      ClassDef *classDefToAdd = new ClassDef("<code>",1,1,g_curClassName,ClassDef::Class,0,0,FALSE);
                                      g_codeClassSDict.append(g_curClassName,classDefToAdd);
                                      char *s=g_curClassBases.first();
                                      while (s) 
				      {
                                        ClassDef *baseDefToAdd;
                                        baseDefToAdd=g_codeClassSDict[s];

					// Try to find class in global
					// scope
					if (baseDefToAdd==0) 
					{
					  baseDefToAdd=getResolvedClass(g_currentDefinition,g_sourceFileDef,s);
					}

					if (baseDefToAdd && baseDefToAdd!=classDefToAdd) 
					{
				          classDefToAdd->insertBaseClass(baseDefToAdd,s,Public,Normal);
					}

                                        s=g_curClassBases.next();
			              }

				      // Reset class-parsing variables.
				      g_curClassName.resize(0);
				      g_curClassBases.clear();
				      
				      g_noSuiteFound = TRUE;
				      BEGIN( SuiteStart );
                                    }
}


<FunctionDec>{
    {IDENTIFIER}                    {
                                        generateFunctionLink(*g_code,yytext);
                                    }

    {B}"("                          {
				        codify(yytext);
					BEGIN( FunctionParams );
		                    }
}

<FunctionParams>{
    ({BB}|",")                      {
                                         // Parses delimiters
				         codify(yytext);
                                    }

    ({IDENTIFIER}|{PARAMNONEMPTY}+) {
				         codify(yytext);
                                    }

    ")"                             {
                                         codify(yytext);
                                    }

    ":"                             {
				      codify(yytext);

				      // Assume this will
				      // be a one-line suite;
				      // found counter-example
				      // in SuiteStart.
				      g_noSuiteFound = TRUE;
				      BEGIN( SuiteStart );
                                    }
}

<Body,Suite>{

    {KEYWORD}                  {
                                 // Position-sensitive rules!
                                 // Must come AFTER keyword-triggered rules
                                 // Must come BEFORE identifier NONEMPTY-like rules
                                 //   to syntax highlight.

  		                 startFontClass("keyword");
  		                 codify(yytext);
		                 endFontClass();
                               }

    {FLOWKW}                   {
  		                 startFontClass("keywordflow");
  		                 codify(yytext);
		                 endFontClass();
                               }
    ({IDENTIFIER}".")*{IDENTIFIER}/"("  {
				 generateClassOrGlobalLink(*g_code,yytext);
      			       }
    ({IDENTIFIER}".")+{IDENTIFIER} {
				 generateClassOrGlobalLink(*g_code,yytext,TRUE);
			       }
    {IDENTIFIER}               { codify(yytext); }
		   	
}



<SuiteStart>{

    {BB}                               {
                                         codify(yytext);
                                       }
    "pass"			       {
                                          startFontClass("keyword");
					  codifyLines(yytext);
		                          endFontClass();
					  BEGIN(Body);
      				       }
    {KEYWORD}                          {
                                          startFontClass("keyword");
					  codifyLines(yytext);
		                          endFontClass();

					  // No indentation necesary
					  g_noSuiteFound = FALSE;
                                       }

    {FLOWKW}                           {
                                          startFontClass("keywordflow");
					  codifyLines(yytext);
		                          endFontClass();

					  // No indentation necesary
					  g_noSuiteFound = FALSE;
                                       }
    {IDENTIFIER}                       {
                                         codify(yytext);
				       } 


    {POUNDCOMMENT}                     {
                                          // This eats EVERYTHING
					  // except the newline
                                          startFontClass("comment");
					  codifyLines(yytext);
		                          endFontClass();
                                       }

    {NEWLINE}                          {
					  codifyLines(yytext);
					  if ( g_noSuiteFound ) 
					  {
                                            // printf("New suite to capture! [%d]\n", g_yyLineNr);
					    BEGIN ( SuiteCaptureIndent );
					  }
                                       }
}

<SuiteCaptureIndent>{
    "\n"|({BB}"\n")            {
                                 // Blankline - ignore, keep looking for indentation.
				 codifyLines(yytext);
                               }

    {BB}                       {
                                 // This state lasts momentarily,
                                 // to check the indentation
                                 // level that is about to be
                                 // used.
				 codifyLines(yytext);
				 g_indents.push(yyleng);
                                 // printf("Captured indent of %d [line %d]\n", yyleng, g_yyLineNr);
				 BEGIN( Suite );
                               }
}

<SuiteMaintain>{

    {BB}/({NONEMPTY}|{EXPCHAR}) {
                                 // This implements poor
				 // indendation-tracking;
                                 // should be improved.
				 // (translate tabs to space, etc)
  		                 codifyLines(yytext);
                                 adjustScopesAndSuites((int)yyleng);
                               }

    "\n"|({BB}"\n")            {
                                 // If this ever succeeds,
				 // it means that this is
				 // a blank line, and
				 // can be ignored.
  		                 codifyLines(yytext);
                               }

    ""/({NONEMPTY}|{EXPCHAR})  {
                                 // Default rule; matches
				 // the empty string, assuming
				 // real text starts here.
				 // Just go straight to Body.
                                 adjustScopesAndSuites(0);
                               }
}


<Suite>{NEWLINE}               {
                                 codifyLines(yytext);
			         BEGIN( SuiteMaintain );
		               }
<Body>{IDENTIFIER}	       {
  			         codify(yytext);
                               }
<Body>{NEWLINE}                {
  		                 codifyLines(yytext);
                               }

<SingleQuoteString>{ // Single quoted string like 'That\'s a """nice""" string!'
    \\{B}\n                    { // line continuation
  		                 codifyLines(yytext);
                               }
    \\.			       { // espaced char
  		                 codify(yytext);
                               }
    {STRINGPREFIX}?{TRIDOUBLEQUOTE} { // tripple double quotes
  		                 codify(yytext);
      			       }
    "'"			       { // end of the string
  		                 codify(yytext);
				 endFontClass();
      		                 BEGIN(g_stringContext);
                               }
    [^"'\n\\]+	               { // normal chars
  		                 codify(yytext);
                               }
    .			       { // normal char
  		                 codify(yytext);
                               }
}

<DoubleQuoteString>{ // Double quoted string like "That's \"a '''nice'''\" string!"
    \\{B}\n                    { // line continuation
  		                 codifyLines(yytext);
                               }
    \\.			       { // espaced char
  		                 codify(yytext);
                               }
    {STRINGPREFIX}?{TRISINGLEQUOTE} { // tripple single quotes
  		                 codify(yytext);
      			       }
    "\""		       { // end of the string
  		                 codify(yytext);
				 endFontClass();
      		                 BEGIN(g_stringContext);
                               }
    [^"'\n\\]+	               { // normal chars
  		                 codify(yytext);
                               }
    .			       { // normal char
  		                 codify(yytext);
                               }
}

<TripleString>{
    {TRIDOUBLEQUOTE}   | 
    {TRISINGLEQUOTE}   {
  		          codify(yytext);
			  if (g_doubleQuote==(yytext[0]=='"')) 
			  {
			    endFontClass();
			    BEGIN(g_stringContext);
			  }
		       }
    {LONGSTRINGBLOCK}  {
			 codifyLines(yytext);
		       }
    \n		       {
			 codifyLines(yytext);
                       }
    .		       {
                         codify(yytext);
                       }
}

  /*
<*>({NONEMPTY}|{EXPCHAR}|{BB})           { // This should go one character at a time.
  		                 codify(yytext);
				 // printf("[pycode] '%s' [ state %d ]  [line %d] no match\n",
				 //       yytext, YY_START, g_yyLineNr);

				 //endFontClass();
				 BEGIN(Body);					
                               }
   */

<*>{STRINGPREFIX}?{TRISINGLEQUOTE} |
<*>{STRINGPREFIX}?{TRIDOUBLEQUOTE} {
  				 startFontClass("stringliteral");
				 g_stringContext=YY_START;
				 g_doubleQuote=yytext[yyleng-1]=='"';
  		                 codify(yytext);
				 BEGIN(TripleString);
                               }
<*>{STRINGPREFIX}?"'"	       { // single quoted string
  				 startFontClass("stringliteral");
				 g_stringContext=YY_START;
  		                 codify(yytext);
				 BEGIN(SingleQuoteString);
  			       }
<*>{STRINGPREFIX}?"\""         { // double quoted string
  				 startFontClass("stringliteral");
				 g_stringContext=YY_START;
  		                 codify(yytext);
				 BEGIN(DoubleQuoteString);
                               }
<*>{POUNDCOMMENT}              {
  				 if (YY_START==SingleQuoteString || 
				     YY_START==DoubleQuoteString || 
				     YY_START==TripleString
				    )
				 {
				   REJECT;
				 }
                                 // This eats EVERYTHING
				 // except the newline
                                 startFontClass("comment");
				 codifyLines(yytext);
		                 endFontClass();
                               }
<*>{NEWLINE}                   {
  		                 codifyLines(yytext);
				 //printf("[pycode] %d NEWLINE [line %d] no match\n",
				 //       YY_START, g_yyLineNr);

				 //endFontClass();
				 BEGIN(Body);
                               }

<*>[ \t]+                      {
  		                 codify(yytext);
				 BEGIN(Body);					
  			       }
<*>.                           {
  		                 codify(yytext);
				 // printf("[pycode] '%s' [ state %d ]  [line %d] no match\n",
				 //        yytext, YY_START, g_yyLineNr);

				 //endFontClass();
				 BEGIN(Body);					
                               }

%%

/*@ ----------------------------------------------------------------------------
 */

void resetPythonCodeParserState() 
{
  g_currentDefinition = 0;
  g_currentMemberDef = 0;
  g_doubleStringIsDoc = FALSE;
  g_paramParens = 0;
  g_indents.clear();
  BEGIN( Body );
}

/*!
  Examines current stack of white-space indentations;
  re-syncs the parser with the correct scope.
*/
static void adjustScopesAndSuites(unsigned indentLength) 
{
  // States to pop
  if (!g_indents.isEmpty() && indentLength < g_indents.top()) 
  {
    while (!g_indents.isEmpty() && indentLength < g_indents.top()) 
    {
      // printf("Exited scope indent of [%d]\n", g_indents.top());
      g_indents.pop(); // Pop the old suite's indentation

      g_currentMemberDef=0;
      if (g_currentDefinition) 
	g_currentDefinition=g_currentDefinition->getOuterScope();
    }
  }

  // Are there any remaining indentation levels for suites?
  if (!g_indents.isEmpty()) 
  {
    BEGIN( Suite );
  }
  else 
  {
    BEGIN( Body );
  }
}

void parsePythonCode(CodeOutputInterface &od,const char * /*className*/,
                 const QCString &s,bool exBlock, const char *exName,
		 FileDef *fd,int startLine,int endLine,bool /*inlineFragment*/,
		 MemberDef *,bool,Definition *searchCtx) 
{

  //printf("***parseCode()\n");
  
  //--------------------------------------
  if (s.isEmpty()) return;
  g_code = &od;
  g_inputString   = s;
  g_inputPosition = 0;
  g_currentFontClass = 0;
  g_needsTermination = FALSE;
  g_searchCtx=searchCtx;
  if (endLine!=-1)
    g_inputLines  = endLine+1;
  else
    g_inputLines  = countLines();
  
  if (startLine!=-1)
    g_yyLineNr    = startLine;
  else
    g_yyLineNr    = 1;
  
  g_exampleBlock  = exBlock; 
  g_exampleName   = exName;
  g_sourceFileDef = fd;

  bool cleanupSourceDef = FALSE;
  if (fd==0)
  {
    // create a dummy filedef for the example
    g_sourceFileDef = new FileDef("",(exName?exName:"generated"));
    cleanupSourceDef = TRUE;
  }
  if (g_sourceFileDef) 
  {
    setCurrentDoc("l00001");
  }

  // Starts line 1 on the output  
  startCodeLine();

  pycodeYYrestart( pycodeYYin );

  pycodeYYlex();

  if (!g_indents.isEmpty()) 
  {
    // printf("Exited pysourceparser in inconsistent state!\n");
  }

  if (g_needsTermination)
  {
    endCodeLine();
  }
  if (cleanupSourceDef)
  {
    // delete the temporary file definition used for this example
    delete g_sourceFileDef;
    g_sourceFileDef=0;
  }
  return;
}


#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
  void pycodeYYdummy() { yy_flex_realloc(0,0); } 
}
#elif YY_FLEX_SUBMINOR_VERSION<33
#error "You seem to be using a version of flex newer than 2.5.4. These are currently incompatible with 2.5.4, and do NOT work with doxygen! Please use version 2.5.4 or expect things to be parsed wrongly! A bug report has been submitted (#732132)."
#endif

