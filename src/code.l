/******************************************************************************
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

%{

/*
 *	includes
 */
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <qregexp.h>
#include <qdir.h>

#include "qtbc.h"
#include "scanner.h"
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"

#define YY_NEVER_INTERACTIVE 1
  
// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)
  
#define SCOPEBLOCK (int *)4
#define INNERBLOCK (int *)8

/* -----------------------------------------------------------------
 *	statics
 */
  
static BaseCodeDocInterface * g_code;

static ClassSDict    g_codeClassSDict(17);
static ClassDef     *g_curClassDef;
static QCString      g_curClassName;
static QStrList      g_curClassBases;

// TODO: is this still needed? if so, make it work
static bool          g_inClass;

static QCString      g_parmType;
static QCString      g_parmName;

static const char *  g_inputString;     //!< the code fragment as text
static int	     g_inputPosition;   //!< read offset during parsing 
static int           g_inputLines;      //!< number of line in the code fragment
static int	     g_yyLineNr;        //!< current line number
static bool          g_needsTermination;

static bool          g_exampleBlock;
static QCString      g_exampleName;
static QCString      g_exampleFile;

static bool          g_insideTemplate = FALSE;
static QCString      g_type;
static QCString      g_name;
static QCString      g_args;
static QCString      g_classScope;
static QCString      g_realScope;
static QStack<int>   g_scopeStack;      //!< 1 if bracket starts a scope, 2 for internal blocks 
static int           g_anchorCount;
static FileDef *     g_sourceFileDef;
static Definition *  g_currentDefinition;
static MemberDef *   g_currentMemberDef;
static bool          g_includeCodeFragment;
static const char *  g_currentFontClass;
static bool          g_searchingForBody;
static bool          g_insideBody;
static int           g_bodyCurlyCount;
static QCString      g_saveName;
static QCString      g_saveType;

static int	     g_bracketCount = 0;
static int	     g_curlyCount   = 0;
static int	     g_sharpCount   = 0;

static int	     g_lastSpecialCContext;
static int           g_lastStringContext;
static int           g_memCallContext;
static int	     g_lastCContext;

//-------------------------------------------------------------------

/*! Represents a stack of variable to class mappings as found in the
 *  code. Each scope is enclosed in pushScope() and popScope() calls.
 *  Variables are added by calling addVariables() and one can search
 *  for variable using findVariable().
 */
class VariableContext
{
  public:
    static const ClassDef *dummyContext;
    
    class Scope : public SDict<ClassDef>
    {
      public:
	Scope() : SDict<ClassDef>(17) {}
    };
    
    VariableContext() 
    {
      m_scopes.setAutoDelete(TRUE);
    }
    virtual ~VariableContext()
    {
    }
    
    void pushScope()
    {
      m_scopes.append(new Scope);
      DBG_CTX((stderr,"** Push var context %d\n",m_scopes.count()));
    }

    void popScope()
    {
      if (m_scopes.count()>0)
      {
        DBG_CTX((stderr,"** Pop var context %d\n",m_scopes.count()));
	m_scopes.remove(m_scopes.count()-1);
      }
      else
      {
        DBG_CTX((stderr,"** ILLEGAL: Pop var context\n"));
      }
    }

    void clear()
    {
      m_scopes.clear();
      m_globalScope.clear();
    }

    void clearExceptGlobal()
    {
      DBG_CTX((stderr,"** Clear var context\n"));
      m_scopes.clear();
    }

    void addVariable(const QCString &type,const QCString &name);
    ClassDef *findVariable(const QCString &name);
    
  private:
    Scope        m_globalScope;
    QList<Scope> m_scopes;
};

void VariableContext::addVariable(const QCString &type,const QCString &name)
{
  //printf("VariableContext::addVariable(%s,%s)\n",type.data(),name.data());
  QCString ltype = type.simplifyWhiteSpace();
  QCString lname = name.simplifyWhiteSpace();
  if (ltype.left(7)=="struct ") 
  {
    ltype = ltype.right(ltype.length()-7);
  }
  else if (ltype.left(6)=="union ")
  {
    ltype = ltype.right(ltype.length()-6);
  }
  if (ltype.isEmpty() || lname.isEmpty()) return;
  DBG_CTX((stderr,"** AddVariable trying: type=%s name=%s\n",ltype.data(),lname.data()));
  Scope *scope = m_scopes.count()==0 ? &m_globalScope : m_scopes.getLast();
  ClassDef *varType;
  int i=0;
  if (
      (varType=g_codeClassSDict[ltype]) ||  // look for class definitions inside the code block
      (varType=getResolvedClass(g_currentDefinition,ltype)) // look for global class definitions
     ) 
  {
    DBG_CTX((stderr,"** AddVariable type=%s name=%s\n",ltype.data(),lname.data()));
    scope->append(lname,varType); // add it to a list
  }
  else if ((i=ltype.find('<'))!=-1)
  {
    // probably a template class, try without arguments as well
    addVariable(ltype.left(i),name);
  }
  else // add a dummy entry so the name is hidden
  {
    //printf("adding dummy context!\n");
    scope->append(lname,dummyContext);
  }
}

ClassDef *VariableContext::findVariable(const QCString &name)
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
      DBG_CTX((stderr,"** findVariable(%s)=%p\n",name.data(),result));
      return result;
    }
  }
  // nothing found -> also try the global scope
  result=m_globalScope.find(name);
  DBG_CTX((stderr,"** findVariable(%s)=%p\n",name.data(),result));
  return result;
}

const ClassDef *VariableContext::dummyContext = (ClassDef*)0x8;

static VariableContext g_theVarContext;

//-------------------------------------------------------------------

class CallContext
{
  public:
    CallContext() 
    {
      m_classList.append(0);
    }
    virtual ~CallContext() {}
    void setClass(ClassDef *cd)
    {
      DBG_CTX((stderr,"** Set call context %s (%p)\n",cd==0 ? "<null>" : cd->name().data(),cd));
      m_classList.removeLast();
      m_classList.append(cd);
    }
    void pushScope()
    {
      m_classList.append(0);
      DBG_CTX((stderr,"** Push call context %d\n",m_classList.count()));
    }
    void popScope()
    {
      if (m_classList.count()>1)
      {
        DBG_CTX((stderr,"** Pop call context %d\n",m_classList.count()));
	m_classList.removeLast();
      }
      else
      {
        DBG_CTX((stderr,"** ILLEGAL: Pop call context\n"));
      }
    }
    void clear()
    {
      DBG_CTX((stderr,"** Clear call context\n"));
      m_classList.clear();
      m_classList.append(0);
    }
    ClassDef *getClass() const
    {
      return m_classList.getLast();
    }

  private:
    QList<ClassDef> m_classList;    
};

static CallContext g_theCallContext;

//-------------------------------------------------------------------

/*! add class/namespace name s to the scope */
static void pushScope(const char *s)
{
  if (g_classScope.isEmpty())
  {
    g_classScope = s;
  }
  else
  {
    g_classScope += "::";
    g_classScope += s;
  }
  //printf("pushScope() result: `%s'\n",g_classScope.data());
}

/*! remove the top class/namespace name from the scope */
static void popScope()
{
  if (!g_classScope.isEmpty())
  {
    int i=g_classScope.findRev("::");
    if (i==-1) // last name, strip all
    {
      g_classScope.resize(0);
    } 
    else // strip name
    {
      g_classScope = g_classScope.left(i);
    }
  }
  else
  {
    //err("Error: Too many end of scopes found!\n");
  }
  //printf("popScope() result: `%s'\n",g_classScope.data());
}

static void setClassScope(const QCString &name)
{
  //printf("setClassScope(%s)\n",name.data());
  QCString n=name;
  n=n.simplifyWhiteSpace();
  int ts=n.find('<'); // start of template
  int te=n.findRev('>'); // end of template
  //printf("ts=%d te=%d\n",ts,te);
  if (ts!=-1 && te!=-1 && te>ts)
  {
    // remove template from scope
    n=n.left(ts)+n.right(n.length()-te-1);
  }
  g_classScope = n;
  //printf("--->New class scope `%s'\n",g_classScope.data());
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
      QCString anchor;
      g_insideBody = FALSE;
      g_searchingForBody = TRUE;
      g_realScope = d->name().copy();
      //printf("Real scope: `%s'\n",g_realScope.data());
      g_bodyCurlyCount = 0;
      if (g_currentMemberDef) anchor=g_currentMemberDef->getBodyAnchor();
      g_code->writeLineNumber(d->getReference(),d->getOutputFileBase(),
	                      anchor,g_yyLineNr);
    }
    else
    {
      //g_code->codify(lineNumber);
      g_code->writeLineNumber(0,0,0,g_yyLineNr);
    }
    //g_code->endLineNumber();
  }
  g_code->startCodeLine(); 
  if (g_currentFontClass)
  {
    g_code->startFontClass(g_currentFontClass);
  }
}


static void endFontClass();
static void endCodeLine()
{
  if (g_currentFontClass) { g_code->endFontClass(); }
  g_code->endCodeLine();
}

/*! write a code fragment `text' that may span multiple lines, inserting
 * line numbers for each line.
 */
static void codifyLines(char *text)
{
  //printf("codifyLines(%d,\"%s\")\n",g_yyLineNr,text);
  char *p=text,*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n');
    if (c=='\n')
    {
      g_yyLineNr++;
      *(p-1)='\0';
      g_code->codify(sp);
      endCodeLine();
      if (g_yyLineNr<g_inputLines) 
      {
	startCodeLine();
      }
    }
    else
    {
      g_code->codify(sp);
      done=TRUE;
    }
  }
}

/*! writes a link to a fragment \a text that may span multiple lines, inserting
 * line numbers for each line. If \a text contains newlines, the link will be 
 * split into multiple links with the same destination, one for each line.
 */
static void writeMultiLineCodeLink(BaseCodeDocInterface &ol,
                  const char *ref,const char *file,
                  const char *anchor,const char *text)
{
  bool done=FALSE;
  char *p=(char *)text;
  while (!done)
  {
    char *sp=p;
    char c;
    while ((c=*p++) && c!='\n');
    if (c=='\n')
    {
      g_yyLineNr++;
      *(p-1)='\0';
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp);
      endCodeLine();
      if (g_yyLineNr<g_inputLines) 
      {
	startCodeLine();
      }
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp);
      done=TRUE;
    }
  }
}

static void addType()
{
  if (g_name=="const") { g_name.resize(0); return; }
  if (!g_type.isEmpty()) g_type += ' ' ;
  g_type += g_name ;
  g_name.resize(0) ;
  if (!g_type.isEmpty()) g_type += ' ' ;
  g_type += g_args ;
  g_args.resize(0) ;
}

static void addParmType()
{
  if (g_parmName=="const") { g_parmName.resize(0); return; }
  if (!g_parmType.isEmpty()) g_parmType += ' ' ;
  g_parmType += g_parmName ;
  g_parmName.resize(0) ;
}

void setParameterList(MemberDef *md)
{
  g_classScope = md->getClassDef() ? md->getClassDef()->name().data() : "";
  ArgumentList *al = md->argumentList();
  if (al==0) return; 
  Argument *a = al->first();
  while (a)
  {
    g_parmName = a->name.copy();
    g_parmType = a->type.copy();
    int i = g_parmType.find('*');
    if (i!=-1) g_parmType = g_parmType.left(i);
    i = g_parmType.find('&');
    if (i!=-1) g_parmType = g_parmType.left(i);
    if (g_parmType.left(6)=="const ") g_parmType = g_parmType.right(g_parmType.length()-6);
    g_parmType=g_parmType.stripWhiteSpace();
    g_theVarContext.addVariable(g_parmType,g_parmName);
    a = al->next();
  }
}

static ClassDef *stripClassName(const char *s)
{
  int pos=0;
  QCString type = s;
  QCString className;
  QCString templSpec;
  while (extractClassNameFromType(type,pos,className,templSpec))
  {
    QCString clName=className+templSpec;
    ClassDef *cd=0;
    if (!g_classScope.isEmpty())
    {
      cd=getResolvedClass(g_currentDefinition,g_classScope+"::"+clName);
    }
    if (cd==0)
    {
      cd=getResolvedClass(g_currentDefinition,clName);
    }
    //printf("stripClass trying `%s' = %p\n",clName.data(),cd);
    if (cd)
    {
      return cd;
    }
  }

  return 0;
}

static MemberDef *setCallContextForVar(const QCString &name)
{
  if (name.isEmpty()) return 0;
  //printf("setCallContextForVar(%s)\n",name.data());

  int scopeEnd = name.findRev("::");
  if (scopeEnd!=-1) // name with explicit scope
  {
    QCString scope   = name.left(scopeEnd);
    QCString locName = name.right(name.length()-scopeEnd-2);
    //printf("explicit scope: name=%s scope=%s\n",locName.data(),scope.data());
    ClassDef *mcd = getClass(scope); // TODO: check namespace as well
    if (mcd && !locName.isEmpty())
    {
      MemberDef *md=mcd->getMemberByName(locName);
      if (md)
      {
        //printf("name=%s scope=%s\n",locName.data(),scope.data());
        g_theCallContext.setClass(stripClassName(md->typeString()));
        return md;
      }
    }
  }
  
  MemberName *mn;
  ClassDef *mcd = g_theVarContext.findVariable(name);
  if (mcd) // local variable
  {
    if (mcd!=VariableContext::dummyContext)
    {
      //printf("local var `%s' mcd=%s\n",name.data(),mcd->name().data());
      g_theCallContext.setClass(mcd);
    }
  }
  else
  {
    // look for a class member 
    mcd = getClass(g_classScope);
    if (mcd)
    {
      MemberDef *md=mcd->getMemberByName(name);
      if (md) 
      {
	g_theCallContext.setClass(stripClassName(md->typeString()));
	return md;
      }
    }
  }

  // look for a global member
  if ((mn=Doxygen::functionNameSDict[name]))
  {
    //printf("global var `%s'\n",name.data());
    if (mn->count()==1) // global defined only once
    {
      MemberDef *md=mn->getFirst();
      if (!md->isStatic() || md->getBodyDef()==g_sourceFileDef)
      {
        g_theCallContext.setClass(stripClassName(md->typeString()));
        return md;
      }
      return 0;
    }
    else if (mn->count()>1) // global defined more than once
    {
      MemberDef *md=mn->first();
      while (md)
      {
	//printf("mn=%p md=%p md->getBodyDef()=%p g_sourceFileDef=%p\n",
	//    mn,md,
	//    md->getBodyDef(),g_sourceFileDef);
	
	// in case there are multiple members we could link to, we 
	// only link to members if defined in the same file or 
	// defined as external.
        if (!md->isStatic() || md->getBodyDef()==g_sourceFileDef)
        {
          g_theCallContext.setClass(stripClassName(md->typeString()));
	  //printf("returning member %s in source file %s\n",md->name().data(),g_sourceFileDef->name().data());
          return md;
        }
	md=mn->next();
      }
      return 0;
    }
  }
  return 0;
}

static void addDocCrossReference(MemberDef *src,MemberDef *dst)
{
  //printf("addDocCrossReference src=%s,dst=%s\n",src->name().data(),dst->name().data());
  if (Config_getBool("REFERENCED_BY_RELATION") && 
      (src->isFunction() || src->isSlot()) 
     )
  {
    dst->addSourceReferencedBy(src);
  }
  if (Config_getBool("REFERENCES_RELATION") && 
      (src->isFunction() || src->isSlot())
     )
  {
    src->addSourceReferences(dst);
  }

}

static void generateClassOrGlobalLink(BaseCodeDocInterface &ol,char *clName,
                                      bool typeOnly=FALSE)
{
  int i=0;
  //printf("generateClassOrGlobalLink(clName=%s)\n",clName);
  if (*clName=='~') // correct for matching negated values i.s.o. destructors.
  {
    g_code->codify("~");
    clName++;
  }
  QCString className=clName;
  if (className.isEmpty()) return;
  ClassDef *cd=0;

  if (!g_theVarContext.findVariable(className)) // not a local variable
  {
    //printf("not a local var!\n");
    Definition *d = g_currentDefinition;
    cd = getResolvedClass(d,className);
    if (cd==0 && (i=className.find('<'))!=-1)
    {
      cd=getResolvedClass(d,className.left(i));
    }
  }
  if (cd && cd->isLinkable()) // is it a linkable class
  {
    //printf("is linkable class %s\n",clName);
    if (g_exampleBlock)
    {
      QCString anchor;
      anchor.sprintf("_a%d",g_anchorCount);
      //printf("addExampleClass(%s,%s,%s)\n",anchor.data(),g_exampleName.data(),
      //                                   g_exampleFile.data());
      if (cd->addExample(anchor,g_exampleName,g_exampleFile))
      {
	ol.writeCodeAnchor(anchor);
	g_anchorCount++;
      }
    }
    writeMultiLineCodeLink(ol,cd->getReference(),cd->getOutputFileBase(),0,className);
  }
  else 
  {
    if (cd==0 && !typeOnly) // not a class, see if it is a global enum/variable/typedef.
    {
      MemberDef *md = setCallContextForVar(clName);
      //printf("is a global md=%p\n",md);
      if (md)
      {
        Definition *d = md->getOuterScope()==Doxygen::globalScope ?
	                md->getBodyDef() : md->getOuterScope();
	//printf("definition %s\n",d?d->name().data():"<none>");
        if (md->getGroupDef()) d = md->getGroupDef();
	if (d && d->isLinkable() && md->isLinkable())
	{
	  writeMultiLineCodeLink(ol,d->getReference(),d->getOutputFileBase(),md->getBodyAnchor(),clName);
	  if (g_currentMemberDef)
	  {
	    addDocCrossReference(g_currentMemberDef,md);
	  }
	  return;
	}
      }
    }
    
    codifyLines(clName);
  }
}

static bool getLink(const char *className,
                    const char *memberName,
		    BaseCodeDocInterface &ol,
		    const char *text=0)
{
  MemberDef    *md;
  ClassDef     *cd;
  FileDef      *fd;
  NamespaceDef *nd;
  GroupDef     *gd;
  QCString m=removeRedundantWhiteSpace(memberName);
  QCString c=className;
  //printf("Trying `%s'::`%s'\n",c.data(),m.data());
  if (getDefs(c,m,"()",md,cd,fd,nd,gd,FALSE,g_sourceFileDef) && 
      md->isLinkable())
  {
    //printf("Found!\n");
    if (g_exampleBlock)
    {
      QCString anchor;
      anchor.sprintf("a%d",g_anchorCount);
      //printf("addExampleFile(%s,%s,%s)\n",anchor.data(),g_exampleName.data(),
      //                                  g_exampleFile.data());
      if (md->addExample(anchor,g_exampleName,g_exampleFile))
      {
	ol.writeCodeAnchor(anchor);
	g_anchorCount++;
      }
    }
    //Definition *d=0;
    //if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else d=gd;

    Definition *d = md->getOuterScope()==Doxygen::globalScope ?
	            md->getBodyDef() : md->getOuterScope();
    if (md->getGroupDef()) d = md->getGroupDef();
    if (d && d->isLinkable())
    {
      g_theCallContext.setClass(stripClassName(md->typeString()));
      //printf("g_currentDefinition=%p g_currentMemberDef=%p g_insideBody=%d\n",
      //        g_currentDefinition,g_currentMemberDef,g_insideBody);

      if (g_currentDefinition && g_currentMemberDef &&
	  md!=g_currentMemberDef && g_insideBody)
      {
	addDocCrossReference(g_currentMemberDef,md);
      }
      //printf("d->getReference()=`%s' d->getOutputBase()=`%s' name=`%s' member name=`%s'\n",d->getReference().data(),d->getOutputFileBase().data(),d->name().data(),md->name().data());
     
      writeMultiLineCodeLink(ol,d->getReference(),d->getOutputFileBase(),
	                       md->getBodyAnchor(),text ? text : memberName);
      return TRUE;
    } 
  }
  return FALSE;
}

static bool generateClassMemberLink(BaseCodeDocInterface &ol,ClassDef *mcd,const char *memName)
{
  if (mcd)
  {
    MemberDef *xmd = mcd->getMemberByName(memName);
    //printf("generateClassMemberLink(class=%s,member=%s)=%p\n",mcd->name().data(),memName,xmd);
    if (xmd)
    {
      // extract class definition of the return type in order to resolve
      // a->b()->c() like call chains
      
      //printf("type=`%s' args=`%s' class=%s\n",
      //  xmd->typeString(),xmd->argsString(),
      //  xmd->getClassDef()->name().data());

      if (g_exampleBlock)
      {
	QCString anchor;
	anchor.sprintf("a%d",g_anchorCount);
	//printf("addExampleFile(%s,%s,%s)\n",anchor.data(),g_exampleName.data(),
	//                                  g_exampleFile.data());
	if (xmd->addExample(anchor,g_exampleName,g_exampleFile))
	{
	  ol.writeCodeAnchor(anchor);
	  g_anchorCount++;
	}
      }

      g_theCallContext.setClass(stripClassName(xmd->typeString()));

      Definition *xd = xmd->getOuterScope()==Doxygen::globalScope ?
	               xmd->getBodyDef() : xmd->getOuterScope();
      if (xmd->getGroupDef()) xd = xmd->getGroupDef();
      if (xd)
      {

	// add usage reference
	if (g_currentDefinition && g_currentMemberDef &&
	    xmd!=g_currentMemberDef && g_insideBody)
	{
	  addDocCrossReference(g_currentMemberDef,xmd);
	}

	// write the actual link
	writeMultiLineCodeLink(ol,xd->getReference(),
	    xd->getOutputFileBase(),xmd->getBodyAnchor(),memName);
	return TRUE;
      }

    }
  }
  
  return FALSE;
}

static void generateMemberLink(BaseCodeDocInterface &ol,const QCString &varName,
            char *memName)
{
  //printf("generateMemberLink(object=%s,mem=%s) classScope=%s\n",
  //    varName.data(),memName,g_classScope.data());

  if (varName.isEmpty()) return;

  // look for the variable in the current context
  ClassDef *vcd = g_theVarContext.findVariable(varName);
  if (vcd) 
  {
    if (vcd!=VariableContext::dummyContext)
    {
      //printf("Class found!\n");
      if (getLink(vcd->name(),memName,ol)) 
      {
	//printf("Found result!\n");
	return;
      }
      BaseClassListIterator bcli(*vcd->baseClasses());
      for ( ; bcli.current() ; ++bcli)
      {
	if (getLink(bcli.current()->classDef->name(),memName,ol)) 
	{
	  //printf("Found result!\n");
	  return;
	}
      }
    }
  }
  else // variable not in current context, maybe it is in a parent context
  {
    vcd = getResolvedClass(g_currentDefinition,g_classScope);
    if (vcd && vcd->isLinkable())
    {
      //printf("Found class %s for variable `%s'\n",g_classScope.data(),varName.data());
      MemberName *vmn=Doxygen::memberNameSDict[varName];
      if (vmn==0)
      {
	int vi;
	QCString vn=varName;
	QCString scope;
	if ((vi=vn.findRev("::"))!=-1) // explicit scope A::b(), probably static member
	{
	  ClassDef *jcd = getClass(vn.left(vi));
	  vn=vn.right(vn.length()-vi-2);
	  vmn=Doxygen::memberNameSDict[vn];
	  //printf("Trying name `%s' scope=%s\n",vn.data(),scope.data());
	  if (vmn)
	  {
	    MemberNameIterator vmni(*vmn);
	    MemberDef *vmd;
	    for (;(vmd=vmni.current());++vmni)
	    {
	      if (/*(vmd->isVariable() || vmd->isFunction()) && */
		  vmd->getClassDef()==jcd)
	      {
		//printf("Found variable type=%s\n",vmd->typeString());
		ClassDef *mcd=stripClassName(vmd->typeString());
		if (mcd && mcd->isLinkable())
		{
		  if (generateClassMemberLink(ol,mcd,memName)) return;
		}
	      }
	    }
	  }
	}
      }
      if (vmn)
      {
	//printf("There is a variable with name `%s'\n",varName);
	MemberNameIterator vmni(*vmn);
	MemberDef *vmd;
	for (;(vmd=vmni.current());++vmni)
	{
	  if (/*(vmd->isVariable() || vmd->isFunction()) && */
	      vmd->getClassDef()==vcd)
	  {
	    //printf("Found variable type=%s\n",vmd->typeString());
	    ClassDef *mcd=stripClassName(vmd->typeString());
	    if (mcd && mcd->isLinkable())
	    {
	      if (generateClassMemberLink(ol,mcd,memName)) return;
	    }
	  }
	}
      }
    }
  }
  codifyLines(memName);
  return;
}

static void generateFunctionLink(BaseCodeDocInterface &ol,char *funcName)
{
  //CodeClassDef *ccd=0;
  ClassDef *ccd=0;
  QCString locScope=g_classScope.copy();
  QCString locFunc=removeRedundantWhiteSpace(funcName);
  int i=locFunc.findRev("::");
  if (i>0)
  {
    locScope=locFunc.left(i);
    locFunc=locFunc.right(locFunc.length()-i-2).stripWhiteSpace();
    int ts=locScope.find('<'); // start of template
    int te=locScope.findRev('>'); // end of template
    //printf("ts=%d te=%d\n",ts,te);
    if (ts!=-1 && te!=-1 && te>ts)
    {
      // remove template from scope
      locScope=locScope.left(ts)+locScope.right(locScope.length()-te-1);
    }
  }
  //printf("generateFunctionLink(%s) classScope=`%s'\n",locFunc.data(),locScope.data());
  if (!locScope.isEmpty() && (ccd=g_codeClassSDict[locScope]))
  {
    //printf("using classScope %s\n",g_classScope.data());
    BaseClassListIterator bcli(*ccd->baseClasses());
    for ( ; bcli.current() ; ++bcli)
    {
      if (getLink(bcli.current()->classDef->name(),locFunc,ol,funcName)) 
      {
	return;
      }
    }
  }
  if (!getLink(locScope,locFunc,ol,funcName))
  {
    generateClassOrGlobalLink(ol,funcName);
  }
  return;
}

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

static void endFontClass()
{
  if (g_currentFontClass)
  {
    g_code->endFontClass();
    g_currentFontClass=0;
  }
}

static void startFontClass(const char *s)
{
  endFontClass();
  g_code->startFontClass(s);
  g_currentFontClass=s;
}




/* -----------------------------------------------------------------
 */
#undef	YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=yyread(buf,max_size);

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

B       [ \t]
BN      [ \t\n\r]
ID	[a-z_A-Z][a-z_A-Z0-9]*
SCOPENAME (({ID}?{BN}*"::"{BN}*)*)((~{BN}*)?{ID})
TEMPLIST "<"[^\"\}\{\(\)\/\n\>]*">"
SCOPETNAME ((({ID}{TEMPLIST}?){BN}*"::"{BN}*)*)((~{BN}*)?{ID})
SCOPEPREFIX ({ID}{TEMPLIST}?{BN}*"::"{BN}*)+
KEYWORD ("asm"|"auto"|"class"|"const"|"const_cast"|"delete"|"dynamic_cast"|"enum"|"explicit"|"extern"|"false"|"friend"|"inline"|"mutable"|"namespace"|"new"|"operator"|"private"|"protected"|"public"|"register"|"reinterpret_cast"|"sizeof"|"static"|"static_cast"|"struct"|"template"|"this"|"true"|"typedef"|"typeid"|"typename"|"union"|"using"|"virtual"|"volatile"|"abstract"|"final"|"import"|"synchronized"|"transient")
FLOWKW  ("break"|"case"|"catch"|"continue"|"default"|"do"|"else"|"for"|"goto"|"if"|"return"|"switch"|"throw"|"throws"|"try"|"while")
TYPEKW  ("bool"|"char"|"double"|"float"|"int"|"long"|"short"|"signed"|"unsigned"|"void"|"wchar_t"|"boolean")

%option noyywrap

%x      SkipString
%x	SkipCPP
%x	SkipComment
%x	SkipCxxComment
%x	RemoveSpecialCComment
%x	StripSpecialCComment
%x	Body
%x      FuncCall
%x      MemberCall
%x      MemberCall2
%x      SkipInits
%x      ClassName
%x      PackageName
%x      ClassVar
%x      Bases
%x      SkipSharp
%x      ReadInclude
%x      TemplDecl
%x	CallEnd

%%

<*>\x0d
<Body>^([ \t]*"#"[ \t]*"include"[ \t]*)("<"|"\"") {
  					  startFontClass("preprocessor");
					  g_code->codify(yytext);
  					  BEGIN( ReadInclude ); 
					}
<Body>("class"|"struct"|"union"|"namespace")[ \t\n]+ { 
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					  if (!g_insideTemplate) 
					    BEGIN( ClassName ); 
					}
<Body>("package")[ \t\n]+ 		{ 
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					  BEGIN( PackageName ); 
					}
<ReadInclude>[^\n\"\>]+/(">"|"\"")  	{
					  //FileInfo *f;
					  bool ambig;
					  bool found=FALSE;
                                          FileDef *fd=0;
					  //printf("looking for include %s\n",yytext);
					  if ((fd=findFileDef(Doxygen::inputNameDict,yytext,ambig)) &&
					      fd->isLinkable())
					  {
					    if (ambig) // multiple input files match the name
					    {
					      //printf("===== yes %s is ambigious\n",yytext);
					      QCString name = convertToQCString(QDir::cleanDirPath(yytext));
					      if (!name.isEmpty() && g_sourceFileDef)
					      {
					        FileName *fn = Doxygen::inputNameDict->find(name);
						if (fn)
						{
						  FileNameIterator fni(*fn);
						  // for each include name
						  for (fni.toFirst();!found && (fd=fni.current());++fni)
						  {
						    // see if this source file actually includes the file
						    found = g_sourceFileDef->isIncluded(fd->absFilePath());
						    //printf("      include file %s found=%d\n",fd->absFilePath().data(),found);
						  }
						}
					      }
					    }
					    else // not ambiguous
					    {
					      found = TRUE;
					    }
					  }
					  if (found)
					  {
					    //printf("      include file %s found=%d\n",fd->absFilePath().data(),found);
					    g_code->writeCodeLink(fd->getReference(),fd->getOutputFileBase(),0,yytext);
					  }
					  else
					  {
					    g_code->codify(yytext);
					  }
					  char c=yyinput();
					  QCString text;
					  text+=c;
					  g_code->codify(text);
					  endFontClass();
					  BEGIN( Body );
  					}
<Body>^[ \t]*"#"			{ 
  					  startFontClass("preprocessor");
  					  g_code->codify(yytext);
  					  BEGIN( SkipCPP ) ; 
					}
<SkipCPP>.				{ 
  					  g_code->codify(yytext);
					}
<SkipCPP>\\[\r]?\n			{ 
  					  codifyLines(yytext);
					}
<SkipCPP>\n/.*\n			{ 
  					  codifyLines(yytext);
					  endFontClass();
					  BEGIN( Body ) ;
					}
<SkipCPP>"//"				{ 
  					  g_code->codify(yytext);
					}
<Body>"{"				{ 
                                          g_theVarContext.pushScope();

  					  g_scopeStack.push(INNERBLOCK);

  					  if (g_searchingForBody)
					  {
					    g_searchingForBody=FALSE;
					    g_insideBody=TRUE;
					  }
  					  g_code->codify(yytext);
  					  g_curlyCount++;
					  if (g_insideBody) 
					  {
					    g_bodyCurlyCount++;
					  }
  					  g_type.resize(0); 
					  g_name.resize(0);
					}
<Body,MemberCall,MemberCall2>"}"	{ 
                                          g_theVarContext.popScope();

  					  if (g_scopeStack.pop()==SCOPEBLOCK) 
					  {
					    popScope();
					  }

  					  g_code->codify(yytext);

  					  g_inClass=FALSE; 

					  if (--g_bodyCurlyCount<=0)
					  {
					    g_insideBody=FALSE;
					    g_currentMemberDef=0;
					    g_currentDefinition=0;
					  }
					  BEGIN(Body);
					}
<ClassName,ClassVar>";"			{ 
  					  g_code->codify(yytext);
					  g_searchingForBody=FALSE; 
  					  BEGIN( Body ); 
					}
<ClassName,ClassVar>[*&]+       	{
					  addType();
					  g_code->codify(yytext);
					}
<ClassName>{ID}("::"{ID})*	        {
                                          g_curClassName=yytext;
					  addType();
					  generateClassOrGlobalLink(*g_code,yytext);
					  BEGIN( ClassVar );
					}
<PackageName>{ID}("."{ID})*		{
                                          g_curClassName=yytext;
					  g_curClassName=substitute(g_curClassName,".","::");
					  //printf("found package: %s\n",g_curClassName.data());
					  addType();
					  codifyLines(yytext);
  					}
<ClassVar>"="				{
					  unput(*yytext);
					  BEGIN( Body );
  					}
<ClassVar>("extends"|"implements")	{ // Java
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					  g_curClassBases.clear();
  					  BEGIN( Bases ); 
					}
<ClassVar>{ID}				{
  					  g_type = g_curClassName.copy();
					  g_name = yytext;
					  g_theVarContext.addVariable(g_type,g_name);
					  generateClassOrGlobalLink(*g_code,yytext);
  					}
<ClassName,ClassVar>[ \t\n]*":"[ \t\n]*	{
  					  codifyLines(yytext);
					  g_curClassBases.clear();
  					  BEGIN( Bases ); 
					}
<PackageName>[ \t]*";"				|
<Bases,ClassName,ClassVar>[ \t]*"{"[ \t]*	{
                                          g_theVarContext.pushScope();
  					  g_code->codify(yytext);
					  g_curlyCount++;
					  g_inClass=TRUE;
  					  if (g_searchingForBody)
					  {
					    g_searchingForBody=FALSE;
					    g_insideBody=TRUE;
					  }
					  if (g_insideBody) g_bodyCurlyCount++;
					  if (!g_curClassName.isEmpty()) // valid class name
					  {
  					    g_scopeStack.push(SCOPEBLOCK);
					    pushScope(g_curClassName);
					    //printf("***** g_curClassName=%s\n",g_curClassName.data());
					    //CodeClassDef *cd=new CodeClassDef(g_ccd);
					    //g_codeClassDict.insert(cd->name,cd);
					    if (getResolvedClass(g_currentDefinition,g_curClassName)==0)
					    {
					      g_curClassDef=new ClassDef("<code>",1,
				 		  g_curClassName,ClassDef::Class);
					      g_codeClassSDict.append(g_curClassName,g_curClassDef);
					      // insert base classes.
					      char *s=g_curClassBases.first();
					      while (s)
					      {
						ClassDef *bcd;
						bcd=g_codeClassSDict[s];
						if (bcd==0) bcd=getResolvedClass(g_currentDefinition,s);
						if (bcd)
						{
						  g_curClassDef->insertBaseClass(bcd,s,Public,Normal);
						}
						s=g_curClassBases.next();
					      }
					    }
					    //printf("g_codeClassList.count()=%d\n",g_codeClassList.count());
					  }
					  else // not a class name -> assume inner block
					  {
  					    g_scopeStack.push(INNERBLOCK);
					  }
					  g_curClassName.resize(0);
					  g_curClassBases.clear();
					  BEGIN( Body );
 					}
<Bases>"virtual"|"public"|"protected"|"private" { 
  					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
					}
<Bases>{ID}	                        { 
					  //printf("%s:addBase(%s)\n",g_ccd.name.data(),yytext);
  					  g_curClassBases.inSort(yytext); 
					  generateClassOrGlobalLink(*g_code,yytext);
					}
<Bases>"<"                              { 
  					  g_code->codify(yytext);
  					  g_sharpCount=1;
					  BEGIN ( SkipSharp );
					}
<SkipSharp>"<"                          {
  					  g_code->codify(yytext);
  					  ++g_sharpCount; 
					}
<SkipSharp>">"                          { 
  					  g_code->codify(yytext);
  					  if (--g_sharpCount<=0)
					  BEGIN ( Bases );
					}
<Bases>","                              { 
  					  g_code->codify(yytext);
					}
  					

<Body>{SCOPEPREFIX}?"operator"{B}*"()"{B}*/"(" {
  					  addType();
					  generateFunctionLink(*g_code,yytext);
  					  g_bracketCount=0;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
					}
<Body>{SCOPEPREFIX}?"operator"{B}*[^\(\n]+/"(" {
  					  addType();
					  generateFunctionLink(*g_code,yytext);
  					  g_bracketCount=0;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
					}
<Body,TemplDecl>"template"/([^a-zA-Z0-9])		{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					  g_insideTemplate=TRUE;
					  g_sharpCount=0;
					}
<Body>{KEYWORD}/([^a-z_A-Z0-9]) 	{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<Body>{KEYWORD}/{B}* 			{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<Body>{KEYWORD}/{B}*"(" 		{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  				          g_name.resize(0);g_type.resize(0);
  					}
<Body>{FLOWKW}/([^a-z_A-Z0-9]) 	{
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<Body>{FLOWKW}/{B}* 			{
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<Body>{FLOWKW}/{B}*"(" 		{
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();
  				          g_name.resize(0);g_type.resize(0);
  					}
<Body>[\\|\)\+\-\/\%\~\!]		{
  					  g_code->codify(yytext);
  				          g_name.resize(0);g_type.resize(0);
					  if (*yytext==')')
					  {
					    g_theCallContext.popScope();
					  }
  					}
<Body,TemplDecl>{TYPEKW}/{B}* {
  					  startFontClass("keywordtype");
					  g_code->codify(yytext);
					  endFontClass();
					  addType();
  					  g_name+=yytext; 
  					}
<Body>"template"/{B}*"<"[^\n\/\-\.\{\"\>]*">"{B}* { // template<...>
  					  startFontClass("keyword");
					  g_code->codify(yytext);
					  endFontClass();
					  g_sharpCount=0;
					  BEGIN(TemplDecl);
                                        }
<TemplDecl>"class"|"typename"		{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<TemplDecl>"<"				{
                                          g_code->codify(yytext);
                                          g_sharpCount++;
  					}
<TemplDecl>">"				{
                                          g_code->codify(yytext);
                                          g_sharpCount--;
					  if (g_sharpCount<=0)
					  {
					    BEGIN(Body);
					  }
  					}
<Body>{SCOPENAME}{B}*"<"[^\n\/\-\.\{\"\>]*">"/{B}* { // A<T> *pt;
					  generateClassOrGlobalLink(*g_code,yytext);
					  addType();
  					  g_name+=yytext; 
					}
<Body>{SCOPENAME}/{B}* 			{ // p->func()
					  generateClassOrGlobalLink(*g_code,yytext);
					  addType();
  					  g_name+=yytext; 
					}
<Body>"("{B}*("*"{B}*)*{SCOPENAME}*{B}*")"/{B}*	{  // (*p)->func()
					  g_code->codify(yytext);
					  int s=0;while (s<yyleng && !isId(yytext[s])) s++;
                                          int e=yyleng-1;while (e>=0 && !isId(yytext[e])) e--;
					  QCString varname = ((QCString)yytext).mid(s,e-s+1); 
					  addType();
  					  g_name=varname; 
					}
<Body>{SCOPETNAME}/{B}*"("		{ // a() or c::a() or t<A,B>::a()
  					  addType();
					  generateFunctionLink(*g_code,yytext);
					  g_theVarContext.addVariable(g_type,yytext);
  					  g_bracketCount=0;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
					}
<FuncCall,Body,MemberCall,MemberCall2>\"	{
					  startFontClass("stringliteral");
  					  g_code->codify(yytext);
  					  g_lastStringContext=YY_START;
  					  BEGIN( SkipString );
  					}
<SkipString>[^\"\\\r\n]*		{ 
  					  g_code->codify(yytext);
					}
<SkipString>"//"|"/*"			{
  					  g_code->codify(yytext);
  					}
<SkipString>\"				{
  					  g_code->codify(yytext);
					  endFontClass();
  					  BEGIN( g_lastStringContext );
  					}
<SkipString>\\.				{
  					  g_code->codify(yytext);
					}
<Body>":"				{
  					  g_code->codify(yytext);
  					  g_name.resize(0);g_type.resize(0);
  					}
<Body>"<"				{
  					  if (g_insideTemplate)
					  {
					    g_sharpCount++;
					  }
  					  g_code->codify(yytext);
  					}
<Body>">"				{
  					  if (g_insideTemplate)
					  {
					    if (--g_sharpCount<=0)
					    {
					      g_insideTemplate=FALSE;
					    }
					  }
  					  g_code->codify(yytext);
  					}
<Body,MemberCall,MemberCall2,FuncCall>"'"((\\0[Xx0-9]+)|(\\.)|(.))"'"	{
  					  startFontClass("charliteral"); 
  					  g_code->codify(yytext);
					  endFontClass();
  					}
<Body>"this->"				{ g_code->codify(yytext); }
<Body>"."|"->"				{ 
  					  g_code->codify(yytext);
					  g_memCallContext = YY_START;
  					  BEGIN( MemberCall ); 
					}
<MemberCall>{SCOPETNAME}/{B}*"(" 	{
					  if (g_theCallContext.getClass())
					  {
					    if (!generateClassMemberLink(*g_code,g_theCallContext.getClass(),yytext))
					    {
					      g_code->codify(yytext);
					    }
  					    g_name.resize(0);
					  }
					  else
					  {
  					    g_code->codify(yytext);
  					    g_name.resize(0);
					  }
					  g_type.resize(0);
					  g_bracketCount=0;
					  if (g_memCallContext==Body)
					  {
					    BEGIN(FuncCall);
					  }
					  else
					  {
					    BEGIN(g_memCallContext);
					  }
  					}
<MemberCall>{SCOPENAME}/{B}*		{
					  if (g_theCallContext.getClass())
					  {
					    if (!generateClassMemberLink(*g_code,g_theCallContext.getClass(),yytext))
					    {
					      g_code->codify(yytext);
					    }
  					    g_name.resize(0);
					  }
					  else
					  {
  					    g_code->codify(yytext);
  					    g_name.resize(0);
					  }
					  g_type.resize(0);
  					  BEGIN(g_memCallContext);
  					}
<MemberCall>[^a-z_A-Z0-9(\n]		{ 
  					  g_code->codify(yytext);
    					  g_type.resize(0);
					  g_name.resize(0);
					  BEGIN(g_memCallContext); 
					}
<Body>[,=;\[]				{
  					  g_code->codify(yytext);
					  g_saveName = g_name.copy();
					  g_saveType = g_type.copy();
  					  if (!g_type.isEmpty()) 
					  {
					    g_theVarContext.addVariable(g_type,g_name);
					    g_name.resize(0);
					  }
					  if (*yytext==';') 
					  {
					    g_type.resize(0);
					    g_name.resize(0);
					  }
					  g_args.resize(0);
  					}
<Body>"]"				{
  					  g_code->codify(yytext);
					  // TODO: nested arrays like: a[b[0]->func()]->func()
					  g_name = g_saveName.copy();
					  g_type = g_saveType.copy();
					}
<Body>[0-9]+				{
					  g_code->codify(yytext);
					}
<Body>[0-9]+[xX][0-9A-Fa-f]+		{
					  g_code->codify(yytext);
					}
<MemberCall2,FuncCall>{KEYWORD}/([^a-z_A-Z0-9]) {
					  addParmType();
					  g_parmName=yytext; 
  					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
					}
<MemberCall2,FuncCall>{TYPEKW}/([^a-z_A-Z0-9]) {
					  addParmType();
					  g_parmName=yytext; 
  					  startFontClass("keywordtype");
  					  g_code->codify(yytext);
					  endFontClass();
					}
<MemberCall2,FuncCall>{FLOWKW}/([^a-z_A-Z0-9]) {
					  addParmType();
					  g_parmName=yytext; 
  					  startFontClass("keywordflow");
  					  g_code->codify(yytext);
					  endFontClass();
					}
<MemberCall2,FuncCall>[a-z_A-Z][:a-z_A-Z0-9]*({B}*"<"[^\n\<\>]*">")? {
					  addParmType();
					  g_parmName=yytext; 
					  generateClassOrGlobalLink(*g_code,yytext,TRUE);
					}
<MemberCall2,FuncCall>,			{
  					  g_code->codify(yytext);
					  g_theVarContext.addVariable(g_parmType,g_parmName);
					  g_parmType.resize(0);g_parmName.resize(0);
					}
<MemberCall2,FuncCall>"("		{
  					  g_code->codify(yytext);
  					  g_bracketCount++; 
					  g_theCallContext.pushScope();
					  if (YY_START==FuncCall && !g_insideBody)
					  {
					    g_theVarContext.pushScope();
					  }
					}
<MemberCall2,FuncCall>")"		{ 
					  g_theCallContext.popScope();
  					  g_code->codify(yytext);
  					  if (--g_bracketCount<=0) 
					  {
					    BEGIN( CallEnd ); 
					  }
					}
<CallEnd>[ \t\n]*			{ codifyLines(yytext); }
  /*
<MemberCall2,FuncCall>")"[ \t\n]*[;:]	{
  */
<CallEnd>[;:]				{
  					  codifyLines(yytext);
  					  g_bracketCount=0;
					  if (*yytext==';') g_searchingForBody=FALSE; 
					  if (!g_inClass && !g_type.isEmpty())
					  {
					    g_theVarContext.addVariable(g_type,g_name);
					  }
					  g_parmType.resize(0);g_parmName.resize(0);
					  g_theCallContext.popScope();
					  g_theCallContext.setClass(0);
  					  if (*yytext==';' || g_insideBody)
					  {
					    if (!g_insideBody)
					    {
                                              g_theVarContext.popScope();
					    }
					    g_name.resize(0);g_type.resize(0);
					    BEGIN( Body );
					  }
					  else
					  {
					    g_bracketCount=0;
					    BEGIN( SkipInits );
					  }
  					}
 /*
<MemberCall2,FuncCall>")"({BN}"const"|"volatile")*{BN}*"{" 	{
  */
<CallEnd>({BN}"const"|"volatile")*{BN}*"{" {
                                          if (g_insideBody)
					  {
					    g_theVarContext.pushScope();
					  }
					  g_theVarContext.addVariable(g_parmType,g_parmName);
					  g_theCallContext.popScope();
					  g_parmType.resize(0);g_parmName.resize(0);
					  if (g_name.find("::")!=-1) 
					  {
  					    g_scopeStack.push(SCOPEBLOCK);
					    setClassScope(g_realScope);
					  }
					  else
					  {
  					    g_scopeStack.push(INNERBLOCK);
					  }
					  yytext[yyleng-1]='\0';
					  QCString cv(yytext);
					  if (!cv.stripWhiteSpace().isEmpty())
					  {
					    startFontClass("keyword");
  					    codifyLines(yytext);
					    endFontClass();
					  }
					  else // just whitespace
					  {
  					    codifyLines(yytext);
					  }
					  g_code->codify("{");
  					  if (g_searchingForBody)
					  {
					    g_searchingForBody=FALSE;
					    g_insideBody=TRUE;
					  }
					  if (g_insideBody) g_bodyCurlyCount++;
					  g_curlyCount++;
  					  g_type.resize(0); g_name.resize(0);
					  BEGIN( Body );
  					}
<CallEnd>.				{
  					  unput(*yytext);
                                          if (!g_insideBody) 
					  {
					    g_theVarContext.popScope();
					  }
					  g_name.resize(0);g_args.resize(0);
					  g_parmType.resize(0);g_parmName.resize(0);
					  BEGIN( Body ); 
  					}
<SkipInits>";"				{
  					  g_code->codify(yytext);
  					  g_type.resize(0); g_name.resize(0);
  					  BEGIN( Body );
  					}
<SkipInits>"{"				{ 
  					  g_code->codify(yytext);
					  g_curlyCount++; 
  					  if (g_searchingForBody)
					  {
					    g_searchingForBody=FALSE;
					    g_insideBody=TRUE;
					  }
					  if (g_insideBody) g_bodyCurlyCount++;
					  if (g_name.find("::")!=-1) 
					  {
  					    g_scopeStack.push(SCOPEBLOCK);
					    setClassScope(g_realScope);
					  }
					  else
					  {
  					    g_scopeStack.push(INNERBLOCK);
					  }
  					  g_type.resize(0); g_name.resize(0);
					  BEGIN( Body ); 
					}
<SkipInits>{ID}				{
					  generateClassOrGlobalLink(*g_code,yytext);
  					}
<FuncCall>([a-z_A-Z][a-z_A-Z0-9]*)/"("	{
					  generateFunctionLink(*g_code,yytext);
					}
<FuncCall>([a-z_A-Z][a-z_A-Z0-9]*)/("."|"->") { 
  					  //g_code->codify(yytext);
					  g_name=yytext; 
					  generateClassOrGlobalLink(*g_code,yytext);
					  BEGIN( MemberCall2 ); 
					}
<FuncCall,MemberCall2>("("{B}*("*"{B}*)+[a-z_A-Z][a-z_A-Z0-9]*{B}*")"{B}*)/("."|"->") { 
  					  g_code->codify(yytext);
					  int s=0;while (!isId(yytext[s])) s++;
                                          int e=yyleng-1;while (!isId(yytext[e])) e--;
					  g_name=((QCString)yytext).mid(s,e-s+1); 
					  BEGIN( MemberCall2 ); 
					}
<MemberCall2>([a-z_A-Z][a-z_A-Z0-9]*)/([ \t\n]*"(") { 
  					  if (!g_args.isEmpty())
					    generateMemberLink(*g_code,g_args,yytext);
					  else
					    generateClassOrGlobalLink(*g_code,yytext);
					  g_args.resize(0);
					  BEGIN( FuncCall );
					}
<MemberCall2>([a-z_A-Z][a-z_A-Z0-9]*)/([ \t\n]*("."|"->")) {
  					  //g_code->codify(yytext);
					  g_name=yytext; 
					  generateClassOrGlobalLink(*g_code,yytext);
					  BEGIN( MemberCall2 ); 
    					}
<MemberCall2>"->"|"."			{
  					  g_code->codify(yytext);
					  g_memCallContext = YY_START;
  					  BEGIN( MemberCall ); 
  					}
<SkipComment>"/*"("!"?)"*/"		{ 
  					  g_code->codify(yytext);
					  endFontClass();
  					  BEGIN( g_lastCContext ) ; 
					}
<SkipComment>"//"|"/*"			{
  					  g_code->codify(yytext);
  					}
<SkipComment>[^*/\n]+			{
  					  g_code->codify(yytext);
  					}
<SkipComment>[ \t]*"*/"			{ 
  					  g_code->codify(yytext);
					  endFontClass();
  					  BEGIN( g_lastCContext ) ; 
					}
<SkipCxxComment>[^\r\n]+		{ 
  					  g_code->codify(yytext);
					}
<SkipCxxComment>\r			
<SkipCxxComment>\n			{
  					  unput('\n');
					  endFontClass();
					  BEGIN( g_lastCContext ) ;
  					}
<SkipCxxComment>.			{
  					  g_code->codify(yytext);
  					}
<RemoveSpecialCComment>"*/"{B}*\n({B}*\n)*({B}*(("//@"[{}])|("/*@"[{}]"*/")){B}*\n)?{B}*"/*"[*!]/[^/*] {
  					  g_yyLineNr+=QCString(yytext).contains('\n');
					}
<RemoveSpecialCComment>"*/"{B}*\n({B}*\n)*({B}*(("//@"[{}])|("/*@"[{}]"*/")){B}*\n)? {
  					  g_yyLineNr+=QCString(yytext).contains('\n');
                                          endCodeLine();
                                          if (g_yyLineNr<g_inputLines) 
                                          {
                                            startCodeLine();
                                          }
					  if (g_lastSpecialCContext==SkipCxxComment)
					  { // force end of C++ comment here
					    endFontClass();
					    BEGIN( g_lastCContext ) ;
					  }
					  else
					  {
  					    BEGIN(g_lastSpecialCContext);
					  }
  					}
<RemoveSpecialCComment>"*/"		{
  					  BEGIN(g_lastSpecialCContext);
  					}
<RemoveSpecialCComment>[^*\n]+
<RemoveSpecialCComment>"//"|"/*"
<RemoveSpecialCComment>\n  { g_yyLineNr++; }
<RemoveSpecialCComment>.
<*>\n({B}*"//"[!/][^\n]*\n)+		{ // remove special one-line comment
  					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_yyLineNr+=((QCString)yytext).contains('\n');
					    endCodeLine();
					    if (g_yyLineNr<g_inputLines) 
					    {
					      startCodeLine();
					    }
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
  					}
<*>\n{B}*"//@"[{}].*\n			{ // remove one-line group marker
  					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_yyLineNr+=2;
					    endCodeLine();
					    if (g_yyLineNr<g_inputLines) 
					    {
					      startCodeLine();
					    }
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
  					}
<*>\n{B}*"/*@"[{}]			{ // remove one-line group marker
					  if (Config_getBool("STRIP_CODE_COMMENTS"))
  					  {
					    g_lastSpecialCContext = YY_START;
					    g_yyLineNr++;
					    BEGIN(RemoveSpecialCComment);
					  }
					  else
					  {
					    // check is to prevent getting stuck in skipping C++ comments
					    if (YY_START != SkipCxxComment)
					    {
  					      g_lastCContext = YY_START ;
					    }
					    startFontClass("comment");
					    codifyLines(yytext);
					    BEGIN(SkipComment);
  					  }
  					}
<*>^{B}*"//@"[{}].*\n			{ // remove one-line group marker
  					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_yyLineNr++;
					    endCodeLine();
					    if (g_yyLineNr<g_inputLines) 
					    {
					      startCodeLine();
					    }
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
  					}
<*>^{B}*"/*@"[{}]			{ // remove multi-line group marker
					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_lastSpecialCContext = YY_START;
					    BEGIN(RemoveSpecialCComment);
					  }
					  else
					  {
					    // check is to prevent getting stuck in skipping C++ comments
					    if (YY_START != SkipCxxComment)
					    {
  					      g_lastCContext = YY_START ;
					    }
					    startFontClass("comment");
					    g_code->codify(yytext);
					    BEGIN(SkipComment);
 					  }
  					}
<*>^{B}*"//"[!/][^\n]*\n		{ // remove special one-line comment
  					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_yyLineNr++;
					    endCodeLine();
					    if (g_yyLineNr<g_inputLines) 
					    {
					      startCodeLine();
					    }
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
  					}
<*>"//"[!/][^\n]*\n			{ // strip special one-line comment
  					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    char c[2]; c[0]='\n'; c[1]=0;
					    codifyLines(c);
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
  					}
<*>\n{B}*"/*"[!*]/[^/*] 		{
					  if (Config_getBool("STRIP_CODE_COMMENTS"))
  					  {
					    g_lastSpecialCContext = YY_START;
					    g_yyLineNr++;
					    BEGIN(RemoveSpecialCComment);
					  }
					  else
					  {
					    // check is to prevent getting stuck in skipping C++ comments
					    if (YY_START != SkipCxxComment)
					    {
  					      g_lastCContext = YY_START ;
					    }
					    startFontClass("comment");
					    codifyLines(yytext);
					    BEGIN(SkipComment);
  					  }
					}
<*>^{B}*"/*"[!*]/[^/*]			{ // special C comment block at a new line
					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_lastSpecialCContext = YY_START;
					    BEGIN(RemoveSpecialCComment);
					  }
					  else
					  {
					    // check is to prevent getting stuck in skipping C++ comments
					    if (YY_START != SkipCxxComment)
					    {
  					      g_lastCContext = YY_START ;
					    }
					    startFontClass("comment");
					    g_code->codify(yytext);
					    BEGIN(SkipComment);
 					  }
					}
<*>"/*"[!*]/[^/*]			{ // special C comment block half way a line
					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_lastSpecialCContext = YY_START;
					    BEGIN(RemoveSpecialCComment);
					  }
					  else
					  {
					    // check is to prevent getting stuck in skipping C++ comments
					    if (YY_START != SkipCxxComment)
					    {
  					      g_lastCContext = YY_START ;
					    }
					    startFontClass("comment");
					    g_code->codify(yytext);
					    BEGIN(SkipComment);
					  }
					}
<*>"/*"("!"?)"*/"			{ if (!Config_getBool("STRIP_CODE_COMMENTS"))
  					  {
					    startFontClass("comment");
					    g_code->codify(yytext);
					    endFontClass();
					  }
					}
<*>"/*"					{ 
					  startFontClass("comment");
  					  g_code->codify(yytext);
					  // check is to prevent getting stuck in skipping C++ comments
					  if (YY_START != SkipCxxComment)
					  {
  					    g_lastCContext = YY_START ;
					  }
					  BEGIN( SkipComment ) ;
					}
<*>"//"					{ 
  					  startFontClass("comment");
  					  g_code->codify(yytext);
  					  g_lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
<*>"("					{
  					  g_code->codify(yytext);
					  g_theCallContext.pushScope();
  					}
<*>")"					{
  					  g_code->codify(yytext);
					  g_theCallContext.popScope();
  					}
<*>\n					{
  					  codifyLines(yytext); 
  					}
<*>.					{
  					  g_code->codify(yytext);
					}
  /*
<*>([ \t\n]*"\n"){2,}			{ // combine multiple blank lines
  					  //QCString sepLine=yytext;
  					  //g_code->codify("\n\n");
  					  //g_yyLineNr+=sepLine.contains('\n'); 
  					  //char sepLine[3]="\n\n";
  					  codifyLines(yytext);
					}
  */

%%

/*@ ----------------------------------------------------------------------------
 */

void initParseCodeContext()
{
  g_theVarContext.clear();
  g_codeClassSDict.setAutoDelete(TRUE);
  g_codeClassSDict.clear();
  g_curClassBases.clear();
  g_anchorCount = 0;
}

void parseCode(BaseCodeDocInterface &od,const char *className,const QCString &s, 
                  bool exBlock, const char *exName,FileDef *fd,
		  int startLine,int endLine,bool inlineFragment)
{
  if (s.isEmpty()) return;
  g_code = &od;
  g_inputString   = s;
  g_inputPosition = 0;
  g_currentFontClass = 0;
  g_needsTermination = FALSE;
  if (endLine!=-1)
    g_inputLines  = endLine+1;
  else
    g_inputLines  = countLines();

  if (startLine!=-1)
    g_yyLineNr    = startLine;
  else
    g_yyLineNr    = 1;

  g_curlyCount    = 0;
  g_bodyCurlyCount    = 0;
  g_bracketCount  = 0;
  g_sharpCount    = 0;
  g_insideTemplate = FALSE;
  g_theCallContext.clear();
  g_scopeStack.clear();
  g_classScope    = className;
  g_exampleBlock  = exBlock; 
  g_exampleName   = exName;
  g_sourceFileDef = fd;
  g_currentDefinition = 0;
  g_currentMemberDef = 0;
  g_searchingForBody = FALSE;
  g_insideBody = FALSE;
  g_bracketCount = 0;
  if (!g_exampleName.isEmpty())
  {
    g_exampleFile = convertNameToFile(g_exampleName+"-example");
  }
  g_includeCodeFragment = inlineFragment;
  startCodeLine();
  g_type.resize(0);
  g_name.resize(0);
  g_args.resize(0);
  g_parmName.resize(0);
  g_parmType.resize(0);
  codeYYrestart( codeYYin );
  BEGIN( Body );
  codeYYlex();
  if (g_needsTermination)
  {
    endFontClass();
    g_code->endCodeLine();
  }
  return;
}

extern "C" { // some bogus code to keep the compiler happy
//  int  codeYYwrap() { return 1 ; }
  void codeYYdummy() { yy_flex_realloc(0,0); } 
}
