/******************************************************************************
 *
 * 
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
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "arguments.h"

#define YY_NEVER_INTERACTIVE 1
  
// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)
  
#define CLASSBLOCK (int *)4
#define SCOPEBLOCK (int *)8
#define INNERBLOCK (int *)12

/* -----------------------------------------------------------------
 *	statics
 */
  
static CodeOutputInterface * g_code;

static ClassSDict    *g_codeClassSDict = 0;
static QCString      g_curClassName;
static QStrList      g_curClassBases;

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
static QStack<int>   g_scopeStack;      //!< 1 if bracket starts a scope, 
                                        //   2 for internal blocks 
static int           g_anchorCount;
static FileDef *     g_sourceFileDef;
static bool          g_lineNumbers;
static Definition *  g_currentDefinition;
static MemberDef *   g_currentMemberDef;
static bool          g_includeCodeFragment;
static const char *  g_currentFontClass;
static bool          g_searchingForBody;
static bool          g_insideBody;
static int           g_bodyCurlyCount;
static QCString      g_saveName;
static QCString      g_saveType;
static QCString      g_delimiter;

static int	     g_bracketCount = 0;
static int	     g_curlyCount   = 0;
static int	     g_sharpCount   = 0;
static bool          g_inFunctionTryBlock = FALSE;
static bool          g_inForEachExpression = FALSE;

static int           g_lastTemplCastContext;
static int	     g_lastSpecialCContext;
static int           g_lastStringContext;
static int           g_lastSkipCppContext;
static int           g_lastVerbStringContext;
static int           g_memCallContext;
static int	     g_lastCContext;

static bool          g_insideObjC;
static bool          g_insideProtocolList;

static bool          g_lexInit = FALSE;

static QStack<int>   g_classScopeLengthStack;

static Definition   *g_searchCtx;

// context for an Objective-C method call
struct ObjCCallCtx
{
  int id;
  QCString methodName;
  QCString objectTypeOrName;
  ClassDef *objectType;
  MemberDef *objectVar;
  MemberDef *method;
  QCString format;
  int lexState;
  int braceCount;
};

// globals for objective-C method calls 
static ObjCCallCtx *g_currentCtx=0;
static int g_currentCtxId=0;
static int g_currentNameId=0;
static int g_currentObjId=0;
static int g_currentWordId=0;
static QStack<ObjCCallCtx> g_contextStack;
static QIntDict<ObjCCallCtx> g_contextDict;
static QIntDict<QCString> g_nameDict;
static QIntDict<QCString> g_objectDict;
static QIntDict<QCString> g_wordDict;
static int g_braceCount=0;
  
static void saveObjCContext();
static void restoreObjCContext();

static QCString g_forceTagReference;


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

    int count() const { return m_scopes.count(); }
    
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
  DBG_CTX((stderr,"** addVariable trying: type='%s' name='%s' g_currentDefinition=%s\n",
	ltype.data(),lname.data(),g_currentDefinition?g_currentDefinition->name().data():"<none>"));
  Scope *scope = m_scopes.count()==0 ? &m_globalScope : m_scopes.getLast();
  ClassDef *varType;
  int i=0;
  if (
      (varType=g_codeClassSDict->find(ltype)) ||  // look for class definitions inside the code block
      (varType=getResolvedClass(g_currentDefinition,g_sourceFileDef,ltype)) // look for global class definitions
     ) 
  {
    DBG_CTX((stderr,"** addVariable type='%s' name='%s'\n",ltype.data(),lname.data()));
    scope->append(lname,varType); // add it to a list
  }
  else if ((i=ltype.find('<'))!=-1)
  {
    // probably a template class
    QCString typeName(ltype.left(i));
    ClassDef* newDef = 0;
    QCString templateArgs(ltype.right(ltype.length() - i));
    if (  
         ( // look for class definitions inside the code block
	   (varType=g_codeClassSDict->find(typeName)) ||
           // otherwise look for global class definitions
           (varType=getResolvedClass(g_currentDefinition,g_sourceFileDef,typeName,0,0,TRUE,TRUE))
	 ) && // and it must be a template
         varType->templateArguments())
    {
      newDef = varType->getVariableInstance( templateArgs );
    }
    if (newDef)
    {
      DBG_CTX((stderr,"** addVariable type='%s' templ='%s' name='%s'\n",typeName.data(),templateArgs.data(),lname.data()));
      scope->append(lname, newDef);
    }
    else
    {
      // Doesn't seem to be a template. Try just the base name.
      addVariable(typeName,name);
    }
  }
  else 
  {
    if (m_scopes.count()>0) // for local variables add a dummy entry so the name 
                            // is hidden to avoid false links to global variables with the same name
                            // TODO: make this work for namespaces as well!
    {
      DBG_CTX((stderr,"** addVariable: dummy context for '%s'\n",lname.data()));
      scope->append(lname,dummyContext);
    }
    else
    {
      DBG_CTX((stderr,"** addVariable: not adding variable!\n"));
    }
  }
}

ClassDef *VariableContext::findVariable(const QCString &name)
{
  if (name.isEmpty()) return 0;
  ClassDef *result = 0;
  //QListIterator<Scope> sli(m_scopes);
  Scope *scope;
  QCString key = name;
  // search from inner to outer scope
  scope = m_scopes.last();
  //for (sli.toLast();(scope=sli.current());--sli)
  while (scope)
  {
    result = scope->find(key);
    if (result) 
    {
      DBG_CTX((stderr,"** findVariable(%s)=%p\n",name.data(),result));
      return result;
    }
    scope = m_scopes.prev();
  }
  // nothing found -> also try the global scope
  result=m_globalScope.find(name);
  DBG_CTX((stderr,"** findVariable(%s)=%p\n",name.data(),result));
  return result;
}

static VariableContext g_theVarContext;
const ClassDef *VariableContext::dummyContext = (ClassDef*)0x8;

//-------------------------------------------------------------------

class CallContext
{
  public:
    struct Ctx
    {
      Ctx() : name(g_name), type(g_type), cd(0) {}
      QCString name;
      QCString type;
      ClassDef *cd;
    };

    CallContext() 
    {
      m_classList.append(new Ctx);
      m_classList.setAutoDelete(TRUE);
    }
    virtual ~CallContext() {}
    void setClass(ClassDef *cd)
    {
      Ctx *ctx = m_classList.getLast();
      if (ctx)
      {
	DBG_CTX((stderr,"** Set call context %s (%p)\n",cd==0 ? "<null>" : cd->name().data(),cd));
        ctx->cd=cd;
      }
    }
    void pushScope()
    {
      m_classList.append(new Ctx);
      DBG_CTX((stderr,"** Push call context %d\n",m_classList.count()));
    }
    void popScope()
    {
      if (m_classList.count()>1)
      {
        DBG_CTX((stderr,"** Pop call context %d\n",m_classList.count()));
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
        DBG_CTX((stderr,"** ILLEGAL: Pop call context\n"));
      }
    }
    void clear()
    {
      DBG_CTX((stderr,"** Clear call context\n"));
      m_classList.clear();
      m_classList.append(new Ctx);
    }
    ClassDef *getClass() const
    {
      Ctx *ctx = m_classList.getLast();
      if (ctx) return ctx->cd; else return 0;
    }

  private:
    QList<Ctx> m_classList;    
};

static CallContext g_theCallContext;

//-------------------------------------------------------------------

/*! add class/namespace name s to the scope */
static void pushScope(const char *s)
{
  g_classScopeLengthStack.push(new int(g_classScope.length()));
  if (g_classScope.isEmpty())
  {
    g_classScope = s;
  }
  else
  {
    g_classScope += "::";
    g_classScope += s;
  }
  //printf("pushScope(%s) result: `%s'\n",s,g_classScope.data());
}

/*! remove the top class/namespace name from the scope */
static void popScope()
{
  if (!g_classScopeLengthStack.isEmpty())
  {
    int *pLength = g_classScopeLengthStack.pop();
    g_classScope.truncate(*pLength);
    delete pLength;
  }
  else
  {
    //err("Error: Too many end of scopes found!\n");
  }
  //printf("popScope() result: `%s'\n",g_classScope.data());
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
  while (!g_classScopeLengthStack.isEmpty())
  {
    popScope();
  }
  g_classScope.resize(0);
  int i;
  while ((i=n.find("::"))!=-1)
  {
    pushScope(n.left(i));
    n = n.mid(i+2);
  }
  pushScope(n);
  //printf("--->New class scope `%s'\n",g_classScope.data());
}

/*! start a new line of code, inserting a line number if g_sourceFileDef
 * is TRUE. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine()
{
  //if (g_currentFontClass) { g_code->endFontClass(); }
  if (g_sourceFileDef && g_lineNumbers)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",g_yyLineNr);
    //lineAnchor.sprintf("l%05d",g_yyLineNr);
   
    Definition *d   = g_sourceFileDef->getSourceDefinition(g_yyLineNr);
    //printf("%s:startCodeLine(%d)=%p\n",g_sourceFileDef->name().data(),g_yyLineNr,d);
    if (!g_includeCodeFragment && d)
    {
      g_currentDefinition = d;
      g_currentMemberDef = g_sourceFileDef->getSourceMember(g_yyLineNr);
      //printf("->startCodeLine(%s)=%p\n",d->name().data(),g_currentMemberDef);
      g_insideBody = FALSE;
      g_searchingForBody = TRUE;
      g_realScope = d->name();
      //g_classScope = "";
      g_type.resize(0);
      g_name.resize(0);
      g_args.resize(0);
      g_parmType.resize(0);
      g_parmName.resize(0);
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
      else if (d->isLinkableInProject())
      {
        g_code->writeLineNumber(d->getReference(),
	                        d->getOutputFileBase(),
	                        0,g_yyLineNr);
        setCurrentDoc(lineAnchor);
      }
    }
    else
    {
      g_code->writeLineNumber(0,0,0,g_yyLineNr);
    }
  }
  g_code->startCodeLine(g_sourceFileDef && g_lineNumbers); 
  if (g_currentFontClass)
  {
    g_code->startFontClass(g_currentFontClass);
  }
}


static void endFontClass();
static void startFontClass(const char *s);

static void endCodeLine()
{
  endFontClass();
  g_code->endCodeLine();
}

static void nextCodeLine()
{
  const char * fc = g_currentFontClass;
  endCodeLine();
  if (g_yyLineNr<g_inputLines) 
  {
    g_currentFontClass = fc;
    startCodeLine();
  }
}

/*! write a code fragment `text' that may span multiple lines, inserting
 * line numbers for each line.
 */
static void codifyLines(const char *text)
{
  //printf("codifyLines(%d,\"%s\")\n",g_yyLineNr,text);
  const char *p=text,*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { }
    if (c=='\n')
    {
      g_yyLineNr++;
      //*(p-1)='\0';
      int l = p-sp-1;
      char *tmp = (char*)malloc(l+1);
      memcpy(tmp,sp,l);
      tmp[l]='\0';
      g_code->codify(tmp);
      free(tmp);
      nextCodeLine();
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

static void addUsingDirective(const char *name)
{
  if (g_sourceFileDef && name)
  {
    NamespaceDef *nd = Doxygen::namespaceSDict->find(name);
    if (nd)
    {
      g_sourceFileDef->addUsingDirective(nd);
    }
  }
}

static void setParameterList(MemberDef *md)
{
  g_classScope = md->getClassDef() ? md->getClassDef()->name().data() : "";
  LockingPtr<ArgumentList> al = md->argumentList();
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
    g_parmType.stripPrefix("const ");
    g_parmType=g_parmType.stripWhiteSpace();
    g_theVarContext.addVariable(g_parmType,g_parmName);
    a = al->next();
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
  //fprintf(stderr,"setCallContextForVar(%s) g_classScope=%s\n",name.data(),g_classScope.data());

  int scopeEnd = name.findRev("::");
  if (scopeEnd!=-1) // name with explicit scope
  {
    QCString scope   = name.left(scopeEnd);
    QCString locName = name.right(name.length()-scopeEnd-2);
    //printf("explicit scope: name=%s scope=%s\n",locName.data(),scope.data());
    ClassDef *mcd = getClass(scope); 
    if (mcd && !locName.isEmpty())
    {
      MemberDef *md=mcd->getMemberByName(locName);
      if (md)
      {
        //printf("name=%s scope=%s\n",locName.data(),scope.data());
        g_theCallContext.setClass(stripClassName(md->typeString(),md->getOuterScope()));
        return md;
      }
    }
    else // check namespace as well
    {
      NamespaceDef *mnd = getResolvedNamespace(scope);
      if (mnd && !locName.isEmpty())
      {
	MemberDef *md=mnd->getMemberByName(locName);
	if (md)
	{
	  //printf("name=%s scope=%s\n",locName.data(),scope.data());
	  g_theCallContext.setClass(stripClassName(md->typeString(),md->getOuterScope()));
	  return md;
	}
      }
    }
  }
  
  MemberName *mn;
  ClassDef *mcd = g_theVarContext.findVariable(name);
  if (mcd) // local variable
  {
    //fprintf(stderr,"local variable?\n");
    if (mcd!=VariableContext::dummyContext)
    {
      //fprintf(stderr,"local var `%s' mcd=%s\n",name.data(),mcd->name().data());
      g_theCallContext.setClass(mcd);
    }
  }
  else
  {
    //fprintf(stderr,"class member? scope=%s\n",g_classScope.data());
    // look for a class member 
    mcd = getClass(g_classScope);
    if (mcd)
    {
      //fprintf(stderr,"Inside class %s\n",mcd->name().data());
      MemberDef *md=mcd->getMemberByName(name);
      if (md) 
      {
        //fprintf(stderr,"Found member %s\n",md->name().data());
	if (g_scopeStack.top()!=CLASSBLOCK)
	{
          //fprintf(stderr,"class member `%s' mcd=%s\n",name.data(),mcd->name().data());
	  g_theCallContext.setClass(stripClassName(md->typeString(),md->getOuterScope()));
	}
	return md;
      }
    }
  }

  // look for a global member
  if ((mn=Doxygen::functionNameSDict->find(name)))
  {
    //printf("global var `%s'\n",name.data());
    if (mn->count()==1) // global defined only once
    {
      MemberDef *md=mn->getFirst();
      if (!md->isStatic() || md->getBodyDef()==g_sourceFileDef)
      {
        g_theCallContext.setClass(stripClassName(md->typeString(),md->getOuterScope()));
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
        if ((!md->isStatic() || md->getBodyDef()==g_sourceFileDef) &&
	    (g_forceTagReference.isEmpty() || g_forceTagReference==md->getReference())
	   )
        {
          g_theCallContext.setClass(stripClassName(md->typeString(),md->getOuterScope()));
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

static void updateCallContextForSmartPointer()
{
  ClassDef *cd = g_theCallContext.getClass();
  //printf("updateCallContextForSmartPointer() cd=%s\n",cd ? cd->name().data() : "<none>");
  MemberDef *md;
  if (cd && (md=cd->isSmartPointer()))
  {
    ClassDef *ncd = stripClassName(md->typeString(),md->getOuterScope());
    if (ncd)
    {
      g_theCallContext.setClass(ncd);
      //printf("Found smart pointer call %s->%s!\n",cd->name().data(),ncd->name().data());
    }
  }
}

static void addDocCrossReference(MemberDef *src,MemberDef *dst)
{
  static bool referencedByRelation = Config_getBool("REFERENCED_BY_RELATION");
  static bool referencesRelation   = Config_getBool("REFERENCES_RELATION");
  static bool callerGraph          = Config_getBool("CALLER_GRAPH");
  static bool callGraph            = Config_getBool("CALL_GRAPH");

  //printf("--> addDocCrossReference src=%s,dst=%s\n",src->name().data(),dst->name().data());
  if (dst->isTypedef() || dst->isEnumerate()) return; // don't add types
  if ((referencedByRelation || callerGraph || dst->hasCallerGraph()) && 
      (src->isFunction() || src->isSlot()) 
     )
  {
    dst->addSourceReferencedBy(src);
    MemberDef *mdDef = dst->memberDefinition();
    if (mdDef)
    {
      mdDef->addSourceReferencedBy(src);
    }
    MemberDef *mdDecl = dst->memberDeclaration();
    if (mdDecl)
    {
      mdDecl->addSourceReferencedBy(src);
    }
  }
  if ((referencesRelation || callGraph || src->hasCallGraph()) && 
      (src->isFunction() || src->isSlot())
     )
  {
    src->addSourceReferences(dst);
    MemberDef *mdDef = src->memberDefinition();
    if (mdDef)
    {
      mdDef->addSourceReferences(dst);
    }
    MemberDef *mdDecl = src->memberDeclaration();
    if (mdDecl)
    {
      mdDecl->addSourceReferences(dst);
    }
  }

}

static bool getLinkInScope(const QCString &c,  // scope
                           const QCString &m,  // member
			   const char *memberText, // exact text
			   CodeOutputInterface &ol,
			   const char *text,
			   bool varOnly=FALSE
			  )
{
  MemberDef    *md;
  ClassDef     *cd;
  FileDef      *fd;
  NamespaceDef *nd;
  GroupDef     *gd;
  //fprintf(stderr,"getLinkInScope: trying `%s'::`%s' varOnly=%d\n",c.data(),m.data(),varOnly);
  if (getDefs(c,m,"()",md,cd,fd,nd,gd,FALSE,g_sourceFileDef,FALSE,g_forceTagReference) && 
      md->isLinkable() && (!varOnly || md->isVariable()))
  {
    //printf("found it %s!\n",md->qualifiedName().data());
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

    Definition *d = md->getOuterScope()==Doxygen::globalScope ?
	            md->getBodyDef() : md->getOuterScope();
    if (md->getGroupDef()) d = md->getGroupDef();
    if (d && d->isLinkable())
    {
      g_theCallContext.setClass(stripClassName(md->typeString(),md->getOuterScope()));
      //printf("g_currentDefinition=%p g_currentMemberDef=%p g_insideBody=%d\n",
      //        g_currentDefinition,g_currentMemberDef,g_insideBody);

      if (g_currentDefinition && g_currentMemberDef &&
	  md!=g_currentMemberDef && g_insideBody)
      {
	addDocCrossReference(g_currentMemberDef,md);
      }
      //printf("d->getReference()=`%s' d->getOutputBase()=`%s' name=`%s' member name=`%s'\n",d->getReference().data(),d->getOutputFileBase().data(),d->name().data(),md->name().data());
     
      ol.linkableSymbol(g_yyLineNr,md->name(),md,
	                g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
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
		    const char *text=0,
		    bool varOnly=FALSE)
{
  //printf("getLink(%s,%s) g_curClassName=%s\n",className,memberName,g_curClassName.data());
  QCString m=removeRedundantWhiteSpace(memberName);
  QCString c=className;
  if (!getLinkInScope(c,m,memberName,ol,text,varOnly))
  {
    if (!g_curClassName.isEmpty())
    {
      if (!c.isEmpty()) c.prepend("::");
      c.prepend(g_curClassName);
      return getLinkInScope(c,m,memberName,ol,text,varOnly);
    }
    return FALSE;
  }
  return TRUE;
}

static void generateClassOrGlobalLink(CodeOutputInterface &ol,const char *clName,
                                      bool typeOnly=FALSE,bool varOnly=FALSE)
{
  int i=0;
  if (*clName=='~') // correct for matching negated values i.s.o. destructors.
  {
    g_code->codify("~");
    clName++;
  }
  QCString className=clName;
  if (className.isEmpty()) return;
  if (g_insideProtocolList) // for Obj-C
  {
    className+="-p";
  }
  className = substitute(className,"\\","::"); // for PHP namespaces
  ClassDef *cd=0,*lcd=0;
  MemberDef *md=0;
  bool isLocal=FALSE;

  //printf("generateClassOrGlobalLink(className=%s)\n",className.data());
  if ((lcd=g_theVarContext.findVariable(className))==0) // not a local variable
  {
    Definition *d = g_currentDefinition;
    //printf("d=%s g_sourceFileDef=%s\n",d?d->name().data():"<none>",g_sourceFileDef?g_sourceFileDef->name().data():"<none>");
    cd = getResolvedClass(d,g_sourceFileDef,className,&md);
    //fprintf(stderr,"non-local variable name=%s context=%d cd=%s md=%s!\n",
    //	className.data(),g_theVarContext.count(),cd?cd->name().data():"<none>",
    //    md?md->name().data():"<none>");
    if (cd==0 && md==0 && (i=className.find('<'))!=-1)
    {
      QCString bareName = className.left(i); //stripTemplateSpecifiersFromScope(className);
      //fprintf(stderr,"bareName=%s\n",bareName.data());
      if (bareName!=className)
      {
	cd=getResolvedClass(d,g_sourceFileDef,bareName,&md); // try unspecialized version
      }
    }
    //printf("md=%s\n",md?md->name().data():"<none>");
    //fprintf(stderr,"is found as a type %s\n",cd?cd->name().data():"<null>");
    if (cd==0 && md==0) // also see if it is variable or enum or enum value
    {
      if (getLink(g_classScope,clName,ol,clName,varOnly))
      {
	return;
      }
    }
  }
  else
  {
    //printf("local variable!\n");
    if (lcd!=VariableContext::dummyContext) 
    {
      //printf("non-dummy context lcd=%s!\n",lcd->name().data());
      g_theCallContext.setClass(lcd);

      // to following is needed for links to a global variable, but is
      // no good for a link to a local variable that is also a global symbol.
       
      //if (getLink(g_classScope,clName,ol,clName))
      //{
	//return;
      //}
    }
    isLocal=TRUE;
    //fprintf(stderr,"is a local variable cd=%p!\n",cd);
  }
  if (cd && cd->isLinkable()) // is it a linkable class
  {
    //fprintf(stderr,"is linkable class %s\n",clName);
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
    ol.linkableSymbol(g_yyLineNr,cd->name(),cd,
                      g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
    writeMultiLineCodeLink(ol,cd->getReference(),cd->getOutputFileBase(),cd->anchor(),clName,cd->briefDescriptionAsTooltip());
    addToSearchIndex(className);
    g_theCallContext.setClass(cd);
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
    //fprintf(stderr,"class %s not linkable! cd=%p md=%p typeOnly=%d\n",clName,cd,md,typeOnly);
    if (!isLocal && (md!=0 || (cd==0 && !typeOnly))) // not a class, see if it is a global enum/variable/typedef.
    {
      if (md==0) // not found as a typedef
      {
	md = setCallContextForVar(clName);
	//printf("setCallContextForVar(%s) md=%p g_currentDefinition=%p\n",clName,md,g_currentDefinition);
	if (md && g_currentDefinition)
	{
	  //fprintf(stderr,"%s accessible from %s? %d md->getOuterScope=%s\n",
	  //    md->name().data(),g_currentDefinition->name().data(),
	  //    isAccessibleFrom(g_currentDefinition,g_sourceFileDef,md),
	  //    md->getOuterScope()->name().data());
	}
	     
        if (md && g_currentDefinition && 
	    isAccessibleFrom(g_currentDefinition,g_sourceFileDef,md)==-1)
	{
	  md=0; // variable not accessible
	}
      }
      if (md && (!varOnly || md->isVariable()))
      {
        //fprintf(stderr,"is a global md=%p g_currentDefinition=%s linkable=%d\n",md,g_currentDefinition?g_currentDefinition->name().data():"<none>",md->isLinkable());
	if (md->isLinkable())
	{
	  QCString text;
	  if (!g_forceTagReference.isEmpty()) // explicit reference to symbol in tag file
	  {
	    text=g_forceTagReference;
	    if (text.right(4)==".tag") // strip .tag if present
	    {
	      text=text.left(text.length()-4);
	    }
	    text+=getLanguageSpecificSeparator(md->getLanguage());
	    text+=clName;
	    md->setName(text);
	    md->setLocalName(text);
	  }
	  else // normal reference
	  {
	    text=clName;
	  }
          ol.linkableSymbol(g_yyLineNr,md->name(),md,
	                    g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
	  writeMultiLineCodeLink(ol,md->getReference(),md->getOutputFileBase(),md->anchor(),text,md->briefDescriptionAsTooltip());
          addToSearchIndex(clName);
	  if (g_currentMemberDef)
	  {
	    addDocCrossReference(g_currentMemberDef,md);
	  }
	  return;
	}
      }
    }
    
    // nothing found, just write out the word
    //fprintf(stderr,"not found!\n");
    ol.linkableSymbol(g_yyLineNr,clName,0,
	              g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
    codifyLines(clName);
    addToSearchIndex(clName);
  }
}

static bool generateClassMemberLink(CodeOutputInterface &ol,MemberDef *xmd,const char *memName)
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

  ClassDef *typeClass = stripClassName(removeAnonymousScopes(xmd->typeString()),xmd->getOuterScope());
  //fprintf(stderr,"%s -> typeName=%p\n",xmd->typeString(),typeClass);
  g_theCallContext.setClass(typeClass);

  Definition *xd = xmd->getOuterScope()==Doxygen::globalScope ?
    xmd->getBodyDef() : xmd->getOuterScope();
  if (xmd->getGroupDef()) xd = xmd->getGroupDef();
  if (xd && xd->isLinkable())
  {

    //printf("g_currentDefiniton=%p g_currentMemberDef=%p xmd=%p g_insideBody=%d\n",g_currentDefinition,g_currentMemberDef,xmd,g_insideBody);

    if (xmd->templateMaster()) xmd = xmd->templateMaster();

    if (xmd->isLinkable())
    {
      // add usage reference
      if (g_currentDefinition && g_currentMemberDef &&
	  /*xmd!=g_currentMemberDef &&*/ g_insideBody)
      {
	addDocCrossReference(g_currentMemberDef,xmd);
      }

      // write the actual link
      ol.linkableSymbol(g_yyLineNr,xmd->name(),xmd,
	  g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
      writeMultiLineCodeLink(ol,xmd->getReference(),
	  xmd->getOutputFileBase(),xmd->anchor(),memName,xmd->briefDescriptionAsTooltip());
      addToSearchIndex(memName);
      return TRUE;
    }
  }

  return FALSE;
}

static bool generateClassMemberLink(CodeOutputInterface &ol,ClassDef *mcd,const char *memName)
{
  if (mcd)
  {
    MemberDef *xmd = mcd->getMemberByName(memName);
    //printf("generateClassMemberLink(class=%s,member=%s)=%p\n",mcd->name().data(),memName,xmd);
    if (xmd)
    {
      return generateClassMemberLink(ol,xmd,memName);
    }
  }
  
  return FALSE;
}

static void generateMemberLink(CodeOutputInterface &ol,const QCString &varName,
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
      if (vcd->baseClasses())
      {
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
  }
  else // variable not in current context, maybe it is in a parent context
  {
    vcd = getResolvedClass(g_currentDefinition,g_sourceFileDef,g_classScope);
    if (vcd && vcd->isLinkable())
    {
      //printf("Found class %s for variable `%s'\n",g_classScope.data(),varName.data());
      MemberName *vmn=Doxygen::memberNameSDict->find(varName);
      if (vmn==0)
      {
	int vi;
	QCString vn=varName;
	QCString scope;
	if ((vi=vn.findRev("::"))!=-1 || (vi=vn.findRev('.'))!=-1)  // explicit scope A::b(), probably static member
	{
	  ClassDef *jcd = getClass(vn.left(vi));
	  vn=vn.right(vn.length()-vi-2);
	  vmn=Doxygen::memberNameSDict->find(vn);
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
		ClassDef *mcd=stripClassName(vmd->typeString(),vmd->getOuterScope());
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
	    ClassDef *mcd=stripClassName(vmd->typeString(),vmd->getOuterScope());
	    if (mcd && mcd->isLinkable())
	    {
	      if (generateClassMemberLink(ol,mcd,memName)) return;
	    }
	  }
	}
      }
    }
  }
  // nothing found -> write result as is
  ol.linkableSymbol(g_yyLineNr,memName,0,
	            g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
  codifyLines(memName);
  addToSearchIndex(memName);
  return;
}

static void generatePHPVariableLink(CodeOutputInterface &ol,const char *varName)
{
  QCString name = varName+7; // strip $this->
  name.prepend("$");
  //printf("generatePHPVariableLink(%s) name=%s scope=%s\n",varName,name.data(),g_classScope.data());
  if (!getLink(g_classScope,name,ol,varName))
  {
    codifyLines(varName);
  }
}

static void generateFunctionLink(CodeOutputInterface &ol,const char *funcName)
{
  //CodeClassDef *ccd=0;
  ClassDef *ccd=0;
  QCString locScope=g_classScope;
  QCString locFunc=removeRedundantWhiteSpace(funcName);
  QCString funcScope;
  QCString funcWithScope=locFunc;
  QCString funcWithFullScope=locFunc;
  QCString fullScope=locScope;
  //fprintf(stdout,"*** locScope=%s locFunc=%s\n",locScope.data(),locFunc.data());
  int len=2;
  int i=locFunc.findRev("::");
  if (g_currentMemberDef && g_currentMemberDef->getClassDef() &&
      funcName==g_currentMemberDef->localName() && 
      g_currentMemberDef->getDefLine()==g_yyLineNr &&
      generateClassMemberLink(ol,g_currentMemberDef,funcName)
     )
  {
    // special case where funcName is the name of a method that is also
    // defined on this line. In this case we can directly link to 
    // g_currentMemberDef, which is not only faster, but
    // in case of overloaded methods, this will make sure that we link to
    // the correct method, and thereby get the correct reimplemented relations.
    // See also bug 549022.
    goto exit;
  }
  if (i==-1) i=locFunc.findRev("."),len=1;
  if (i==-1) i=locFunc.findRev("\\"),len=1; // for PHP
  if (i>0)
  {
    funcScope=locFunc.left(i);
    locFunc=locFunc.right(locFunc.length()-i-len).stripWhiteSpace();
    int ts=locScope.find('<'); // start of template
    int te=locScope.findRev('>'); // end of template
    //printf("ts=%d te=%d\n",ts,te);
    if (ts!=-1 && te!=-1 && te>ts)
    {
      // remove template from scope
      locScope=locScope.left(ts)+locScope.right(locScope.length()-te-1);
    }
    ts=funcScope.find('<'); // start of template
    te=funcScope.findRev('>'); // end of template
    //printf("ts=%d te=%d\n",ts,te);
    if (ts!=-1 && te!=-1 && te>ts)
    {
      // remove template from scope
      funcScope=funcScope.left(ts)+funcScope.right(funcScope.length()-te-1);
    }
    if (!funcScope.isEmpty())
    {
      funcWithScope = funcScope+"::"+locFunc;
      if (!locScope.isEmpty())
      {
        fullScope=locScope+"::"+funcScope;
      }
    }
    if (!locScope.isEmpty())
    {
      funcWithFullScope = locScope+"::"+funcWithScope;
    }
  }
  if (!fullScope.isEmpty() && (ccd=g_codeClassSDict->find(fullScope)))
  {
    //printf("using classScope %s\n",g_classScope.data());
    if (ccd->baseClasses())
    {
      BaseClassListIterator bcli(*ccd->baseClasses());
      for ( ; bcli.current() ; ++bcli)
      {
	if (getLink(bcli.current()->classDef->name(),locFunc,ol,funcName)) 
	{
	  goto exit;
	}
      }
    }
  }
  if (!locScope.isEmpty() && fullScope!=locScope && (ccd=g_codeClassSDict->find(locScope)))
  {
    //printf("using classScope %s\n",g_classScope.data());
    if (ccd->baseClasses())
    {
      BaseClassListIterator bcli(*ccd->baseClasses());
      for ( ; bcli.current() ; ++bcli)
      {
	if (getLink(bcli.current()->classDef->name(),funcWithScope,ol,funcName)) 
	{
	  goto exit;
	}
      }
    }
  }
  if (!getLink(locScope,funcWithScope,ol,funcName))
  {
    generateClassOrGlobalLink(ol,funcName);
  }
exit:  
  g_forceTagReference.resize(0);
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

//----------------------------------------------------------------------------

// recursively writes a linkified Objective-C method call
static void writeObjCMethodCall(ObjCCallCtx *ctx)
{
  if (ctx==0) return;
  char c;
  const char *p = ctx->format.data();
  if (!ctx->methodName.isEmpty())
  {
    //printf("writeObjCMethodCall(%s) obj=%s method=%s\n",
    //    ctx->format.data(),ctx->objectTypeOrName.data(),ctx->methodName.data());
    if (!ctx->objectTypeOrName.isEmpty() && ctx->objectTypeOrName.at(0)!='$')
    {
      //printf("Looking for object=%s method=%s\n",ctx->objectTypeOrName.data(),
      //	ctx->methodName.data());
      ClassDef *cd = g_theVarContext.findVariable(ctx->objectTypeOrName);
      if (cd==0) // not a local variable
      {
	if (ctx->objectTypeOrName=="self")
	{
	  if (g_currentDefinition && 
	      g_currentDefinition->definitionType()==Definition::TypeClass)
	  {
	    ctx->objectType = (ClassDef *)g_currentDefinition;
	  }
	}
	else
	{
	  ctx->objectType = getResolvedClass(
	      g_currentDefinition,
	      g_sourceFileDef,
	      ctx->objectTypeOrName,
	      &ctx->method);
	}
	//printf("  object is class? %p\n",ctx->objectType);
	if (ctx->objectType) // found class
	{
	  ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
	  //printf("    yes->method=%s\n",ctx->method?ctx->method->name().data():"<none>");
	}
	else if (ctx->method==0) // search for class variable with the same name
	{
	  //printf("    no\n");
	  //printf("g_currentDefinition=%p\n",g_currentDefinition);
	  if (g_currentDefinition && 
	      g_currentDefinition->definitionType()==Definition::TypeClass)
	  {
	    ctx->objectVar = ((ClassDef *)g_currentDefinition)->getMemberByName(ctx->objectTypeOrName);
	    //printf("      ctx->objectVar=%p\n",ctx->objectVar);
	    if (ctx->objectVar)
	    {
	      ctx->objectType = stripClassName(ctx->objectVar->typeString());
	      //printf("        ctx->objectType=%p\n",ctx->objectType);
	      if (ctx->objectType)
	      {
		ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
		//printf("          ctx->method=%p\n",ctx->method);
	      }
	    }
	  }
	}
      }
      else // local variable
      {
	//printf("  object is local variable\n");
	if (cd!=VariableContext::dummyContext)
	{
	  ctx->method = cd->getMemberByName(ctx->methodName);
	  //printf("   class=%p method=%p\n",cd,ctx->method);
	}
      }
    }
  }

  //printf("[");
  while ((c=*p++)) // for each character in ctx->format
  {
    if (c=='$')
    {
      char nc=*p++;
      if (nc=='$') // escaped $
      {
	g_code->codify("$");
      }
      else // name fragment or reference to a nested call 
      {
	if (nc=='n') // name fragment
	{
          nc=*p++;
	  QCString refIdStr;
	  while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
	  p--;
	  int refId=refIdStr.toInt();
	  QCString *pName = g_nameDict.find(refId);
	  if (pName)
	  {
	    if (ctx->method && ctx->method->isLinkable())
	    {
              g_code->linkableSymbol(g_yyLineNr,ctx->method->name(),ctx->method,
                            g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
              writeMultiLineCodeLink(*g_code,
		                     ctx->method->getReference(),
	                             ctx->method->getOutputFileBase(),
	                             ctx->method->anchor(),
				     pName->data(),
				     ctx->method->briefDescriptionAsTooltip());
	      if (g_currentMemberDef)
	      {
	        addDocCrossReference(g_currentMemberDef,ctx->method);
	      }
	    }
	    else
	    {
              g_code->linkableSymbol(g_yyLineNr,pName->data(),0,
                            g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
   	      codifyLines(pName->data());
	    }
	  }
	  else
	  {
	    //printf("Invalid name: id=%d\n",refId);
	  }
	}
	else if (nc=='o') // reference to potential object name
	{
          nc=*p++;
	  QCString refIdStr;
	  while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
	  p--;
	  int refId=refIdStr.toInt();
	  QCString *pObject = g_objectDict.find(refId);
	  if (pObject)
	  {
	    if (*pObject=="self")
	    {
	      if (g_currentDefinition && 
		  g_currentDefinition->definitionType()==Definition::TypeClass)
	      {
	        ctx->objectType = (ClassDef *)g_currentDefinition;
	        if (ctx->objectType->categoryOf()) 
	        {
	          ctx->objectType = ctx->objectType->categoryOf();
	        }
	        if (ctx->objectType)
	        {
	          ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
	        }
	      }
 	      startFontClass("keyword");
              codifyLines(pObject->data());
	      endFontClass();
	    }
	    else if (*pObject=="super")
	    {
	      if (g_currentDefinition &&
		  g_currentDefinition->definitionType()==Definition::TypeClass)
	      {
		ClassDef *cd = (ClassDef *)g_currentDefinition;
		if (cd->categoryOf()) 
		{
		  cd = cd->categoryOf();
		}
		BaseClassList *bcd = cd->baseClasses();
		if (bcd) // get direct base class (there should be only one)
		{
		  BaseClassListIterator bli(*bcd);
		  BaseClassDef *bclass;
		  for (bli.toFirst();(bclass=bli.current());++bli)
		  {
		    if (bclass->classDef->compoundType()!=ClassDef::Protocol)
		    {
		      ctx->objectType = bclass->classDef;
		      if (ctx->objectType)
		      {
			ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
		      }
		    }
		  }
		}
	      }
 	      startFontClass("keyword");
              codifyLines(pObject->data());
	      endFontClass();
	    }
	    else if (ctx->objectVar && ctx->objectVar->isLinkable()) // object is class variable
	    {
              g_code->linkableSymbol(g_yyLineNr,ctx->objectVar->name(),ctx->objectVar,
                            g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
	      writeMultiLineCodeLink(*g_code,
		    ctx->objectVar->getReference(),
		    ctx->objectVar->getOutputFileBase(),
		    ctx->objectVar->anchor(),
		    pObject->data(),
		    ctx->objectVar->briefDescriptionAsTooltip());
	      if (g_currentMemberDef)
	      {
	        addDocCrossReference(g_currentMemberDef,ctx->objectVar);
	      }
	    }
	    else if (ctx->objectType && 
		     ctx->objectType!=VariableContext::dummyContext && 
		     ctx->objectType->isLinkable()
		    ) // object is class name
	    {
	      ClassDef *cd = ctx->objectType;
              g_code->linkableSymbol(g_yyLineNr,cd->name(),cd,
                            g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
	      writeMultiLineCodeLink(*g_code,
		    cd->getReference(),
		    cd->getOutputFileBase(),
		    cd->anchor(),
		    pObject->data(),
		    cd->briefDescriptionAsTooltip());
	    }
	    else // object still needs to be resolved
	    {
	      ClassDef *cd = getResolvedClass(g_currentDefinition, 
		  g_sourceFileDef, *pObject);
	      if (cd && cd->isLinkable())
	      {
		if (ctx->objectType==0) ctx->objectType=cd;
                g_code->linkableSymbol(g_yyLineNr,cd->name(),cd,
                            g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
	   	writeMultiLineCodeLink(*g_code,
		    cd->getReference(),
		    cd->getOutputFileBase(),
		    cd->anchor(),
		    pObject->data(),
		    cd->briefDescriptionAsTooltip());
	      }
	      else
	      {
                g_code->linkableSymbol(g_yyLineNr,pObject->data(),0,
                            g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
		codifyLines(pObject->data());
	      }
	    }
	  }
	  else
	  {
	    //printf("Invalid object: id=%d\n",refId);
	  }
	}
	else if (nc=='c') // reference to nested call
	{
          nc=*p++;
	  QCString refIdStr;
	  while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
	  p--;
	  int refId=refIdStr.toInt();
	  ObjCCallCtx *ictx = g_contextDict.find(refId);
	  if (ictx) // recurse into nested call
	  {
	    writeObjCMethodCall(ictx);
	    if (ictx->method) // link to nested call successfully
	    {
	      // get the ClassDef representing the method's return type
	      if (QCString(ictx->method->typeString())=="id")
	      {
		// see if the method name is unique, if so we link to it
		MemberName *mn=Doxygen::memberNameSDict->find(ctx->methodName);
		//printf("mn->count=%d ictx->method=%s ctx->methodName=%s\n",
		//    mn==0?-1:(int)mn->count(),
		//    ictx->method->name().data(),
		//    ctx->methodName.data());
		if (mn && mn->count()==1) // member name unique
		{
		  ctx->method = mn->getFirst();
		}
	      } 
	      else
	      {
		ctx->objectType = stripClassName(ictx->method->typeString());
		if (ctx->objectType)
		{
		  ctx->method = ctx->objectType->getMemberByName(ctx->methodName);
		}
	      }
	      //printf("  ***** method=%s -> object=%p\n",ictx->method->name().data(),ctx->objectType);
	    }
	  }
	  else
	  {
	    //printf("Invalid context: id=%d\n",refId);
	  }
	}
	else if (nc=='w') // some word
	{
          nc=*p++;
	  QCString refIdStr;
	  while (nc!=0 && isdigit(nc)) { refIdStr+=nc; nc=*p++; }
	  p--;
	  int refId=refIdStr.toInt();
	  QCString *pWord = g_wordDict.find(refId);
	  if (pWord)
	  {
            g_code->linkableSymbol(g_yyLineNr,pWord->data(),0,
                         g_currentMemberDef ? g_currentMemberDef : g_currentDefinition);
            codifyLines(pWord->data());
	  }
	}
	else // illegal marker
	{
	  ASSERT(!"invalid escape sequence");
	}
      }
    }
    else // normal non-marker character
    {
      char s[2];
      s[0]=c;s[1]=0;
      codifyLines(s);
    }
  }  
  //printf("%s %s]\n",ctx->objectTypeOrName.data(),ctx->methodName.data());
  //printf("}=(type='%s',name='%s')",
  //    ctx->objectTypeOrName.data(),
  //    ctx->methodName.data());
}

// Replaces an Objective-C method name fragment s by a marker of the form
// $n12, the number (12) can later be used as a key for obtaining the name 
// fragment, from g_nameDict
static QCString escapeName(const char *s)
{
  QCString result;
  result.sprintf("$n%d",g_currentNameId);
  g_nameDict.insert(g_currentNameId,new QCString(s));
  g_currentNameId++;
  return result;
}

static QCString escapeObject(const char *s)
{
  QCString result;
  result.sprintf("$o%d",g_currentObjId);
  g_objectDict.insert(g_currentObjId,new QCString(s));
  g_currentObjId++;
  return result;
}

static QCString escapeWord(const char *s)
{
  QCString result;
  result.sprintf("$w%d",g_currentWordId);
  g_wordDict.insert(g_currentWordId,new QCString(s));
  g_currentWordId++;
  return result;
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
ID	"$"?[a-z_A-Z\x80-\xFF][a-z_A-Z0-9\x80-\xFF]*
SEP      ("::"|"\\")
SCOPENAME ({SEP}{BN}*)?({ID}{BN}*{SEP}{BN}*)*("~"{BN}*)?{ID}
TEMPLIST "<"[^\"\}\{\(\)\/\n\>]*">"
SCOPETNAME (((({ID}{TEMPLIST}?){BN}*)?{SEP}{BN}*)*)((~{BN}*)?{ID})
SCOPEPREFIX ({ID}{TEMPLIST}?{BN}*{SEP}{BN}*)+
KEYWORD_OBJC ("@public"|"@private"|"@protected"|"@class"|"@implementation"|"@interface"|"@end"|"@selector"|"@protocol"|"@optional"|"@required"|"@throw"|"@synthesize"|"@property")
KEYWORD ("asm"|"__assume"|"auto"|"class"|"const"|"delete"|"enum"|"explicit"|"extern"|"false"|"friend"|"gcnew"|"gcroot"|"get"|"inline"|"internal"|"mutable"|"namespace"|"new"|"nullptr"|"override"|"operator"|"pin_ptr"|"private"|"protected"|"public"|"raise"|"register"|"remove"|"self"|"set"|"sizeof"|"static"|"struct"|"__super"|"function"|"template"|"generic"|"this"|"true"|"typedef"|"typeid"|"typename"|"union"|"using"|"virtual"|"volatile"|"abstract"|"final"|"import"|"synchronized"|"transient"|{KEYWORD_OBJC})
FLOWKW  ("break"|"case"|"catch"|"continue"|"default"|"do"|"else"|"finally"|"for"|"foreach"|"for each"|"goto"|"if"|"return"|"switch"|"throw"|"throws"|"try"|"while"|"@try"|"@catch"|"@finally")
TYPEKW  ("bool"|"char"|"double"|"float"|"int"|"long"|"object"|"short"|"signed"|"unsigned"|"void"|"wchar_t"|"size_t"|"boolean"|"id"|"SEL"|"string"|"nullptr")
CASTKW ("const_cast"|"dynamic_cast"|"reinterpret_cast"|"static_cast")
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^' \\\n]{1,4}"'"))
ARITHOP "+"|"-"|"/"|"*"|"%"|"--"|"++"
ASSIGNOP "="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^="|"|="
LOGICOP "=="|"!="|">"|"<"|">="|"<="|"&&"|"||"|"!"
BITOP   "&"|"|"|"^"|"<<"|">>"|"~"
OPERATOR {ARITHOP}|{ASSIGNOP}|{LOGICOP}|{BITOP}
RAWBEGIN  (u|U|L|u8)?R\"[^ \t\(\)\\]{0,16}"("
RAWEND    ")"[^ \t\(\)\\]{0,16}\"

%option noyywrap

%x      SkipString
%x      SkipStringS
%x      SkipVerbString
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
%x	CppCliTypeModifierFollowup
%x      Bases
%x      SkipSharp
%x      ReadInclude
%x      TemplDecl
%x      TemplCast
%x	CallEnd
%x      ObjCMethod
%x	ObjCParams
%x	ObjCParamType
%x      ObjCCall
%x      ObjCMName
%x      ObjCSkipStr
%x      OldStyleArgs
%x	UsingName
%x      RawString

%%

<*>\x0d
<Body>^([ \t]*"#"[ \t]*("include"|"import")[ \t]*)("<"|"\"") {
  					  startFontClass("preprocessor");
					  g_code->codify(yytext);
  					  BEGIN( ReadInclude ); 
					}
<Body>("@interface"|"@implementation"|"@protocol")[ \t\n]+ { 
                                          g_insideObjC=TRUE;
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					  if (!g_insideTemplate) 
					    BEGIN( ClassName ); 
					}
<Body>(("public"|"private"){B}+)?("ref"|"value"|"interface"|"enum"){B}+("class"|"struct") {
  					  if (g_insideTemplate) REJECT;
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					  BEGIN( ClassName ); 
					}
<Body>"property"|"event"/{BN}*			{ 
  					  if (g_insideTemplate) REJECT;
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					}
<Body>(KEYWORD_CPPCLI_DATATYPE|("partial"{B}+)?"class"|"struct"|"union"|"namespace"|"interface"){B}+ { 
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
<ClassVar>\n				{
  					  if (!g_insideObjC) REJECT;
  					  codifyLines(yytext);
					  BEGIN(Body);
  					}
<Body,ClassVar,Bases>"-"|"+"		{
					  if (!g_insideObjC || g_insideBody)
					  { 
  					    g_code->codify(yytext);
					  }
					  else // Start of Objective-C method
					  {
					    //printf("Method!\n");
  					    g_code->codify(yytext);
					    BEGIN(ObjCMethod);
					  }
  					}
<ObjCMethod>":"				{
  					  g_code->codify(yytext);
					  BEGIN(ObjCParams);
  					}
<ObjCParams>"("				{
  					  g_code->codify(yytext);
  					  BEGIN(ObjCParamType);
					}
<ObjCParams,ObjCMethod>";"|"{"		{
  					  g_code->codify(yytext);
					  if (*yytext=='{')
					  {
					    g_curlyCount++;
  					    if (g_searchingForBody)
					    {
					      g_searchingForBody=FALSE;
					      g_insideBody=TRUE;
					    }
					    if (g_insideBody) g_bodyCurlyCount++;
					    if (!g_curClassName.isEmpty()) // valid class name
					    {
					      pushScope(g_curClassName);
                                              DBG_CTX((stderr,"** scope stack push SCOPEBLOCK\n"));
  					      g_scopeStack.push(SCOPEBLOCK);
					    }
					  }
                                          g_type.resize(0);
                                          g_name.resize(0);
					  BEGIN(Body);
  					}
<ObjCParams>{ID}{B}*":"			{
  					  g_code->codify(yytext);
  					}
<ObjCParamType>{TYPEKW} 		{
  					  startFontClass("keywordtype");
					  g_code->codify(yytext);
					  endFontClass();
  					  g_parmType=yytext;
  					}
<ObjCParamType>{ID}			{
					  generateClassOrGlobalLink(*g_code,yytext);
  					  g_parmType=yytext;
  					}
<ObjCParamType>")"			{
  					  g_code->codify(yytext);
  					  BEGIN(ObjCParams);
  					}
<ObjCParams>{ID}			{
                                          g_code->linkableSymbol(g_yyLineNr,yytext,0,
	                                     g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
  					  g_code->codify(yytext);
  					  g_parmName=yytext;
					  g_theVarContext.addVariable(g_parmType,g_parmName);
					  g_parmType.resize(0);g_parmName.resize(0);
  					}
<ObjCMethod,ObjCParams,ObjCParamType>{ID} {
					  generateClassOrGlobalLink(*g_code,yytext);
  					}
<ObjCMethod,ObjCParams,ObjCParamType>.	{
  					  g_code->codify(yytext);
  					}
<ObjCMethod,ObjCParams,ObjCParamType>\n	{
  					  codifyLines(yytext);
  					}
<ReadInclude>[^\n\"\>]+/(">"|"\"")  	{
					  //FileInfo *f;
					  bool ambig;
					  bool found=FALSE;
					  //QCString absPath = yytext;
					  //if (g_sourceFileDef && QDir::isRelativePath(absPath))
					  //{
					  //  absPath = QDir::cleanDirPath(g_sourceFileDef->getPath()+"/"+absPath);
					  //}

                                          FileDef *fd=findFileDef(Doxygen::inputNameDict,yytext,ambig);
					  //printf("looking for include %s -> %s fd=%p\n",yytext,absPath.data(),fd);
					  if (fd && fd->isLinkable())
					  {
					    if (ambig) // multiple input files match the name
					    {
					      //printf("===== yes %s is ambiguous\n",yytext);
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
					  //printf("      include file %s found=%d\n",fd ? fd->absFilePath().data() : "<none>",found);
					  if (found)
					  {
					    g_code->writeCodeLink(fd->getReference(),fd->getOutputFileBase(),0,yytext,fd->briefDescriptionAsTooltip());
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
<Body,Bases>^[ \t]*"#"			{ 
  					  startFontClass("preprocessor");
					  g_lastSkipCppContext = YY_START;
  					  g_code->codify(yytext);
  					  BEGIN( SkipCPP ) ; 
					}
<SkipCPP>.				{ 
  					  g_code->codify(yytext);
					}
<SkipCPP>[^\n\/\\]+			{
  					  g_code->codify(yytext);
  					}
<SkipCPP>\\[\r]?\n			{ 
  					  codifyLines(yytext);
					}
<SkipCPP>"//"				{ 
  					  g_code->codify(yytext);
					}
<Body,FuncCall>"{"			{ 
                                          g_theVarContext.pushScope();

                                          DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
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
					  BEGIN( Body );
					}
<Body,MemberCall,MemberCall2>"}"	{ 
                                          g_theVarContext.popScope();
  					  g_type.resize(0); 
					  g_name.resize(0);

					  int *scope = g_scopeStack.pop();
                                          DBG_CTX((stderr,"** scope stack pop SCOPEBLOCK=%d\n",scope==SCOPEBLOCK));
  					  if (scope==SCOPEBLOCK || scope==CLASSBLOCK) 
					  {
					    popScope();
					  }

  					  g_code->codify(yytext);

					  //fprintf(stderr,"g_bodyCurlyCount=%d\n",g_bodyCurlyCount);
					  if (--g_bodyCurlyCount<=0)
					  {
					    g_insideBody=FALSE;
					    g_currentMemberDef=0;
					    if (g_currentDefinition) 
					      g_currentDefinition=g_currentDefinition->getOuterScope();
					  }
					  BEGIN(Body);
					}
<Body,ClassVar>"@end"			{ 
  					  //printf("End of objc scope fd=%s\n",g_sourceFileDef->name().data());
                                          if (g_sourceFileDef)
					  {
					    FileDef *fd=g_sourceFileDef;
                                            g_insideObjC = fd->name().lower().right(2)==".m" || 
                                                           fd->name().lower().right(3)==".mm"; 
					    //printf("insideObjC=%d\n",g_insideObjC);
					  }
					  else
					  {
					    g_insideObjC = FALSE;
					  }
					  if (g_insideBody)
					  {
                                            g_theVarContext.popScope();

					    int *scope = g_scopeStack.pop();
                                            DBG_CTX((stderr,"** scope stack pop SCOPEBLOCK=%d\n",scope==SCOPEBLOCK));
  					    if (scope==SCOPEBLOCK || scope==CLASSBLOCK) 
					    {
					      popScope();
					    }
					    g_insideBody=FALSE;
					  }

					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();

					  g_currentMemberDef=0;
					  if (g_currentDefinition) 
					    g_currentDefinition=g_currentDefinition->getOuterScope();
					  BEGIN(Body);
					}
<ClassName,ClassVar>";"			{ 
  					  g_code->codify(yytext);
					  g_searchingForBody=FALSE; 
  					  BEGIN( Body ); 
					}
<ClassName,ClassVar>[*&^%]+       	{
  					  g_type=g_curClassName.copy();
  					  g_name.resize(0);
					  g_code->codify(yytext);
					  BEGIN( Body ); // variable of type struct *
					}
<ClassName>"__declspec"{B}*"("{B}*{ID}{B}*")"	{
					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
					}
<ClassName>{ID}("::"{ID})*	        {
                                          g_curClassName=yytext;
					  addType();
					  generateClassOrGlobalLink(*g_code,yytext);
					  BEGIN( ClassVar );
					}
<ClassName>{ID}("\\"{ID})*		{ // PHP namespace
                                          g_curClassName=substitute(yytext,"\\","::");
  					  g_scopeStack.push(CLASSBLOCK);
					  pushScope(g_curClassName);
					  addType();
					  generateClassOrGlobalLink(*g_code,yytext);
					  BEGIN( ClassVar );
  					}
<ClassName>{ID}{B}*"("{ID}")"           { // Obj-C category
                                          g_curClassName=removeRedundantWhiteSpace(yytext);
  					  g_scopeStack.push(CLASSBLOCK);
					  pushScope(g_curClassName);
					  addType();
					  generateClassOrGlobalLink(*g_code,yytext);
					  BEGIN( ClassVar );
                                        }
<PackageName>{ID}("."{ID})*		{
					  g_curClassName=substitute(yytext,".","::");
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
<ClassVar>("sealed"|"abstract")/{BN}*(":"|"{") {
					  //fprintf(stderr,"***** C++/CLI modifier %s on g_curClassName=%s\n",yytext,g_curClassName.data());
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					  BEGIN( CppCliTypeModifierFollowup ); 
					}
<ClassVar>{ID}				{
  					  g_type = g_curClassName.copy();
					  g_name = yytext;
					  if (g_insideBody)
					  {
					    g_theVarContext.addVariable(g_type,g_name);
					  }
					  generateClassOrGlobalLink(*g_code,yytext);
  					}
<ClassName,ClassVar,CppCliTypeModifierFollowup>{B}*":"{B}*	{
  					  codifyLines(yytext);
					  g_curClassBases.clear();
  					  BEGIN( Bases ); 
					}
<PackageName>[ \t]*";"			|
<Bases>^{B}*/"@"{ID}                    | // Objective-C interface
<Bases,ClassName,ClassVar,CppCliTypeModifierFollowup>{B}*"{"{B}* {
                                          g_theVarContext.pushScope();
  					  g_code->codify(yytext);
					  g_curlyCount++;
					  if (YY_START==ClassVar && g_curClassName.isEmpty())
					  {
					    g_curClassName = g_name.copy();
					  }
  					  if (g_searchingForBody)
					  {
					    g_searchingForBody=FALSE;
					    g_insideBody=TRUE;
					  }
					  if (g_insideBody) g_bodyCurlyCount++;
					  if (!g_curClassName.isEmpty()) // valid class name
					  {
                                            DBG_CTX((stderr,"** scope stack push CLASSBLOCK\n"));
  					    g_scopeStack.push(CLASSBLOCK);
					    pushScope(g_curClassName);
					    //fprintf(stderr,"***** g_curClassName=%s\n",g_curClassName.data());
					    if (getResolvedClass(g_currentDefinition,g_sourceFileDef,g_curClassName)==0)
					    {
					      //fprintf(stderr,"Adding new class %s\n",g_curClassName.data());
					      ClassDef *ncd=new ClassDef("<code>",1,
				 		  g_curClassName,ClassDef::Class,0,0,FALSE);
					      g_codeClassSDict->append(g_curClassName,ncd);
					      // insert base classes.
					      char *s=g_curClassBases.first();
					      while (s)
					      {
						ClassDef *bcd;
						bcd=g_codeClassSDict->find(s);
						if (bcd==0) bcd=getResolvedClass(g_currentDefinition,g_sourceFileDef,s);
						if (bcd && bcd!=ncd)
						{
						  ncd->insertBaseClass(bcd,s,Public,Normal);
						}
						s=g_curClassBases.next();
					      }
					    }
					    //printf("g_codeClassList.count()=%d\n",g_codeClassList.count());
					  }
					  else // not a class name -> assume inner block
					  {
                                            DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
  					    g_scopeStack.push(INNERBLOCK);
					  }
					  g_curClassName.resize(0);
					  g_curClassBases.clear();
					  BEGIN( Body );
 					}
<Bases>"virtual"|"public"|"protected"|"private"|"@public"|"@private"|"@protected" { 
  					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
					}
<Bases>{SEP}?({ID}{SEP})*{ID}           { 
					  //fprintf(stderr,"%s:addBase(%s)\n",g_curClassName.data(),yytext);
  					  g_curClassBases.inSort(yytext); 
					  generateClassOrGlobalLink(*g_code,yytext);
					}
<Bases>"<"                              { 
  					  g_code->codify(yytext);
					  if (!g_insideObjC)
					  {
  					    g_sharpCount=1;
					    BEGIN ( SkipSharp );
					  }
					  else
					  {
					    g_insideProtocolList=TRUE;
					  }
					}
<Bases>">"				{
  					  g_code->codify(yytext);
					  g_insideProtocolList=FALSE;
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
<Bases>"("                              {
                                          g_code->codify(yytext);
                                          g_sharpCount=1;
                                          BEGIN ( SkipSharp );
                                        }
<SkipSharp>"("                          {
                                          g_code->codify(yytext);
                                          ++g_sharpCount;
                                        }
<SkipSharp>")"                          {
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
<Body>{SCOPEPREFIX}?"operator"/"("      {
  					  addType();
					  generateFunctionLink(*g_code,yytext);
  					  g_bracketCount=0;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
                                        }
<Body>{SCOPEPREFIX}?"operator"[^a-z_A-Z0-9\(\n]+/"(" {
  					  addType();
					  generateFunctionLink(*g_code,yytext);
  					  g_bracketCount=0;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
					}
<Body,TemplDecl>("template"|"generic")/([^a-zA-Z0-9])		{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					  g_insideTemplate=TRUE;
					  g_sharpCount=0;
					}
<Body>"using"{BN}+"namespace"{BN}+	{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					  BEGIN(UsingName);
  					}
<UsingName>{ID}("::"{ID})*		{ addUsingDirective(yytext);
 					  generateClassOrGlobalLink(*g_code,yytext);
                                          DBG_CTX((stderr,"** scope stack push CLASSBLOCK\n"));
  					  g_scopeStack.push(CLASSBLOCK);
					  pushScope(yytext);
					  BEGIN(Body);
                                        }
<UsingName>\n				{ codifyLines(yytext); BEGIN(Body); }
<UsingName>.				{ codifyLines(yytext); BEGIN(Body); }
<Body,FuncCall>"$"?"this"("->"|".")	{ g_code->codify(yytext); // this-> for C++, this. for C#
                                        }
<Body>{KEYWORD}/([^a-z_A-Z0-9]) 	{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  if (QCString(yytext)=="typedef")
					  {
					    addType();
  					    g_name+=yytext; 
					  }
					  endFontClass();
  					}
<Body>{KEYWORD}/{B}* 			{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<Body>{KEYWORD}/{BN}*"(" 		{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  				          g_name.resize(0);g_type.resize(0);
  					}
<FuncCall>"in"/{BN}*			{
					  if (!g_inForEachExpression) REJECT;
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();
					  // insert the variable in the parent scope, see bug 546158
					  g_theVarContext.popScope();
					  g_theVarContext.addVariable(g_parmType,g_parmName);
					  g_theVarContext.pushScope();
  				          g_name.resize(0);g_type.resize(0);
					}
<Body>{FLOWKW}/{BN}*"(" 			{
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();
  				          g_name.resize(0);g_type.resize(0);
					  g_inForEachExpression = (strcmp(yytext,"for each")==0 || strcmp(yytext, "foreach")==0);
					  BEGIN(FuncCall);
  					}
<Body>{FLOWKW}/([^a-z_A-Z0-9]) 		{
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();
					  if (g_inFunctionTryBlock && (strcmp(yytext,"catch")==0 || strcmp(yytext,"finally")==0))
					  {
					    g_inFunctionTryBlock=FALSE;
					  }
  					}
<Body>{FLOWKW}/{B}* 			{
  					  startFontClass("keywordflow");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<Body>"*"{B}*")"                        { // end of cast?
  					  g_code->codify(yytext);
					  g_theCallContext.popScope();
					  g_bracketCount--;
                                          g_parmType = g_name;
					  BEGIN(FuncCall);
                                        }
<Body>[\\|\)\+\-\/\%\~\!]		{
  					  g_code->codify(yytext);
  				          g_name.resize(0);g_type.resize(0);
					  if (*yytext==')')
					  {
					    g_theCallContext.popScope();
					    g_bracketCount--;
					    BEGIN(FuncCall);
					  }
  					}
<Body,TemplDecl,ObjCMethod>{TYPEKW}/{B}* {
  					  startFontClass("keywordtype");
					  g_code->codify(yytext);
					  endFontClass();
					  addType();
  					  g_name+=yytext; 
  					}
<Body>"generic"/{B}*"<"[^\n\/\-\.\{\"\>]*">"{B}* {
  					  startFontClass("keyword");
					  g_code->codify(yytext);
					  endFontClass();
					  g_sharpCount=0;
					  BEGIN(TemplDecl);
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
<TemplCast>">"				{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					  BEGIN( g_lastTemplCastContext );
  					}
<TemplCast>{ID}("::"{ID})*		{
					  generateClassOrGlobalLink(*g_code,yytext);
					}
<TemplCast>("const"|"volatile"){B}*	{
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
					}
<TemplCast>[*^]*			{
  					  codifyLines(yytext);
					}
<Body,FuncCall>{CASTKW}"<"                { // static_cast<T>(
  					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
                                          g_lastTemplCastContext = YY_START;
					  BEGIN(TemplCast);
					}
<Body>"$this->"{SCOPENAME}/{BN}*[;,)\]] { // PHP member variable
					  addType();
					  generatePHPVariableLink(*g_code,yytext);
  					  g_name+=yytext+7; 
                                        }
<Body,TemplCast>{SCOPENAME}{B}*"<"[^\n\/\-\.\{\"\>]*">"("::"{ID})*/{B}* { // A<T> *pt;
					  int i=QCString(yytext).find('<');
					  QCString kw = QCString(yytext).left(i).stripWhiteSpace();
					  if (kw.right(5)=="_cast" && YY_START==Body)
					  {
					    REJECT;
					  }
					  addType();
					  generateClassOrGlobalLink(*g_code,yytext);
  					  g_name+=yytext; 
					}
<Body>{SCOPENAME}/{BN}*[;,)\]]		{ // "int var;" or "var, var2" or "debug(f) macro" 
					  addType();
					  // changed this to generateFunctionLink, see bug 624514
					  //generateClassOrGlobalLink(*g_code,yytext,FALSE,TRUE);
					  generateFunctionLink(*g_code,yytext);
  					  g_name+=yytext; 
					}
<Body>{SCOPENAME}/{B}* 			{ // p->func()
					  addType();
					  generateClassOrGlobalLink(*g_code,yytext);
  					  g_name+=yytext; 
					}
<Body>"("{B}*("*"{B}*)+{SCOPENAME}*{B}*")"/{B}*	{  // (*p)->func() but not "if (p) ..."
					  g_code->codify(yytext);
					  int s=0;while (s<(int)yyleng && !isId(yytext[s])) s++;
                                          int e=(int)yyleng-1;while (e>=0 && !isId(yytext[e])) e--;
					  QCString varname = ((QCString)yytext).mid(s,e-s+1); 
					  addType();
  					  g_name=varname; 
					}
<Body>{SCOPETNAME}/{BN}*"("		{ // a() or c::a() or t<A,B>::a() or A\B\foo()
  					  addType();
					  generateFunctionLink(*g_code,yytext);
  					  g_bracketCount=0;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
					}
<FuncCall,Body,MemberCall,MemberCall2,SkipInits>{RAWBEGIN}	{
                                          QCString text=yytext;
                                          int i=text.find('R');
                                          g_code->codify(text.left(i+1));
					  startFontClass("stringliteral");
  					  g_code->codify(yytext+i+1);
  					  g_lastStringContext=YY_START;
					  g_inForEachExpression = FALSE;
                                          g_delimiter = yytext+i+2;
                                          g_delimiter=g_delimiter.left(g_delimiter.length()-1);
  					  BEGIN( RawString );
                                        }
<FuncCall,Body,MemberCall,MemberCall2,SkipInits>\"	{
					  startFontClass("stringliteral");
  					  g_code->codify(yytext);
  					  g_lastStringContext=YY_START;
					  g_inForEachExpression = FALSE;
  					  BEGIN( SkipString );
  					}
<FuncCall,Body,MemberCall,MemberCall2,SkipInits>\'	{
					  startFontClass("stringliteral");
  					  g_code->codify(yytext);
  					  g_lastStringContext=YY_START;
					  g_inForEachExpression = FALSE;
  					  BEGIN( SkipStringS );
  					}
<SkipString>[^\"\\\r\n]*		{ 
  					  g_code->codify(yytext);
					}
<SkipStringS>[^\'\\\r\n]*		{
  					  g_code->codify(yytext);
  					}
<SkipString,SkipStringS>"//"|"/*"	{
  					  g_code->codify(yytext);
  					}
<SkipString>@?\"			{
  					  g_code->codify(yytext);
					  endFontClass();
  					  BEGIN( g_lastStringContext );
  					}
<SkipStringS>\'				{
  					  g_code->codify(yytext);
					  endFontClass();
  					  BEGIN( g_lastStringContext );
  					}
<SkipString,SkipStringS>\\.		{
  					  g_code->codify(yytext);
					}
<RawString>{RAWEND}                     { 
                                          g_code->codify(yytext);
                                          QCString delimiter = yytext+1;
                                          delimiter=delimiter.left(delimiter.length()-1);
                                          if (delimiter==g_delimiter)
                                          {
					    BEGIN( g_lastStringContext );
                                          }
                                        }
<RawString>[^)]+                        { g_code->codify(yytext); }
<RawString>.                            { g_code->codify(yytext); }
<RawString>\n                           { codifyLines(yytext); }
<SkipVerbString>[^"\n]+			{
  					  g_code->codify(yytext);
					}
<SkipVerbString>\"\"			{ // escaped quote
  					  g_code->codify(yytext);
					}
<SkipVerbString>\"			{ // end of string
  					  g_code->codify(yytext);
					  endFontClass();
					  BEGIN( g_lastVerbStringContext );
					}
<SkipVerbString>.			{
  					  g_code->codify(yytext);
  					}
<SkipVerbString>\n			{
  					  codifyLines(yytext);
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
<Body>"."|"->"				{ 
  				          if (yytext[0]=='-') // -> could be overloaded
					  {
					    updateCallContextForSmartPointer();
					  }
  					  g_code->codify(yytext);
					  g_memCallContext = YY_START;
  					  BEGIN( MemberCall ); 
					}
<MemberCall>{SCOPETNAME}/{BN}*"(" 	{
					  if (g_theCallContext.getClass())
					  {
					    if (!generateClassMemberLink(*g_code,g_theCallContext.getClass(),yytext))
					    {
                                              g_code->linkableSymbol(g_yyLineNr,yytext,0,
	                                                        g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
					      g_code->codify(yytext);
					      addToSearchIndex(yytext);
					    }
  					    g_name.resize(0);
					  }
					  else
					  {
                                            g_code->linkableSymbol(g_yyLineNr,yytext,0,
	                                                        g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
  					    g_code->codify(yytext);
					    addToSearchIndex(yytext);
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
					    //fprintf(stderr,"g_theCallContext.getClass()=%p\n",g_theCallContext.getClass());
					    if (!generateClassMemberLink(*g_code,g_theCallContext.getClass(),yytext))
					    {
                                              g_code->linkableSymbol(g_yyLineNr,yytext,0,
	                                                        g_currentMemberDef?g_currentMemberDef:g_currentDefinition);
					      g_code->codify(yytext);
					      addToSearchIndex(yytext);
					    }
  					    g_name.resize(0);
					  }
					  else
					  {
					    //fprintf(stderr,"no class context!\n");
  					    g_code->codify(yytext);
					    addToSearchIndex(yytext);
  					    g_name.resize(0);
					  }
					  g_type.resize(0);
  					  BEGIN(g_memCallContext);
  					}
<Body>[,=;\[]				{
					  if (g_insideObjC && *yytext=='[')
					  {
					    //printf("Found start of ObjC call!\n");
					    // start of a method call
					    g_contextDict.setAutoDelete(TRUE);
					    g_nameDict.setAutoDelete(TRUE);
					    g_objectDict.setAutoDelete(TRUE);
					    g_wordDict.setAutoDelete(TRUE);
					    g_contextDict.clear();
					    g_nameDict.clear();
					    g_objectDict.clear();
					    g_wordDict.clear();
					    g_currentCtxId  = 0;
					    g_currentNameId  = 0;
					    g_currentObjId  = 0;
					    g_currentCtx = 0;
					    g_braceCount = 0;
					    unput('[');
					    BEGIN(ObjCCall);
					  }
					  else
					  {
					    g_code->codify(yytext);
					    g_saveName = g_name.copy();
					    g_saveType = g_type.copy();
					    if (*yytext!='[' && !g_type.isEmpty()) 
					    {
					      //printf("g_scopeStack.bottom()=%p\n",g_scopeStack.bottom());
					      if (g_scopeStack.top()!=CLASSBLOCK)
					      {
						//printf("AddVariable: '%s' '%s' context=%d\n",
						//    g_type.data(),g_name.data(),g_theVarContext.count());
					        g_theVarContext.addVariable(g_type,g_name);
					      }
					      g_name.resize(0);
					    }
					    if (*yytext==';' || *yytext=='=') 
					    {
					      g_type.resize(0);
					      g_name.resize(0);
					    }
					    else if (*yytext=='[')
					    {
					      g_theCallContext.pushScope();
					    }
					    g_args.resize(0);
                                            g_parmType.resize(0);
                                            g_parmName.resize(0);
					  }
  					}
  /*
<ObjCMemberCall>{ID}			{
  					  if (strcmp(yytext,"self")==0 || strcmp(yytext,"super")==0)
					  {
					    // TODO: get proper base class for "super"
					    g_theCallContext.setClass(getClass(g_curClassName));
					    startFontClass("keyword");
					    g_code->codify(yytext); 
					    endFontClass();
					  }
					  else
					  {
					    generateClassOrGlobalLink(*g_code,yytext);
					  }
					  g_name.resize(0);
					  BEGIN(ObjCMemberCall2);
  					}
<ObjCMemberCall>"["			{
					    g_code->codify(yytext);
					    g_theCallContext.pushScope();
  					}
<ObjCMemberCall2>{ID}":"?		{
  					  g_name+=yytext;
					  if (g_theCallContext.getClass())
					  {
					    //printf("Calling method %s\n",g_name.data());
					    if (!generateClassMemberLink(*g_code,g_theCallContext.getClass(),g_name))
					    {
  					      g_code->codify(yytext);
					      addToSearchIndex(g_name);
					    }
					  }
					  else
					  {
  					    g_code->codify(yytext);
					    addToSearchIndex(g_name);
					  }
  					  g_name.resize(0);
					  BEGIN(ObjCMemberCall3);
  					}
<ObjCMemberCall2,ObjCMemberCall3>"]"	{
					  g_theCallContext.popScope();
  					  g_code->codify(yytext);
					  BEGIN(Body);
  					}
  */
<ObjCCall,ObjCMName>"["         { 
                                   saveObjCContext();
			           g_currentCtx->format+=*yytext;
			           BEGIN(ObjCCall);
		                   //printf("open\n");
                                 }
<ObjCCall,ObjCMName>"]"         { 
			            g_currentCtx->format+=*yytext;
                                    restoreObjCContext();
			            BEGIN(ObjCMName);
		             	    if (g_currentCtx==0)
				    {
				      // end of call
				      writeObjCMethodCall(g_contextDict.find(0));
				      BEGIN(Body);
				    }
			            //printf("close\n");
                                  }
<ObjCCall>{ID}	                  {
                                    g_currentCtx->format+=escapeObject(yytext);
			            if (g_braceCount==0)
			            {
			              g_currentCtx->objectTypeOrName=yytext;
                                      //printf("new type=%s\n",g_currentCtx->objectTypeOrName.data());
			              BEGIN(ObjCMName);
			            }
  		                  }
<ObjCMName>{ID}/{BN}*"]"          { 
                                    if (g_braceCount==0 && 
					g_currentCtx->methodName.isEmpty())
                                    {
			              g_currentCtx->methodName=yytext; 
                                      g_currentCtx->format+=escapeName(yytext);
			            }
				    else
				    {
                                      g_currentCtx->format+=escapeWord(yytext);
				    }
                                  }
<ObjCMName>{ID}/{BN}*":"           { 
                                     if (g_braceCount==0)
                                     {
			               g_currentCtx->methodName+=yytext;
                                       g_currentCtx->methodName+=":";
			             }
                                     g_currentCtx->format+=escapeName(yytext);
                                   }
<ObjCSkipStr>[^\n\"$\\]*           { g_currentCtx->format+=yytext; }
<ObjCSkipStr>\\.	           { g_currentCtx->format+=yytext; }
<ObjCSkipStr>"\""	           { g_currentCtx->format+=yytext; 
                                      BEGIN(g_lastStringContext); 
                                   }
<ObjCCall,ObjCMName>{CHARLIT}      { g_currentCtx->format+=yytext; }
<ObjCCall,ObjCMName>"@"?"\""       { g_currentCtx->format+=yytext; 
                                      g_lastStringContext=YY_START;
                                      BEGIN(ObjCSkipStr); 
                                   }
<ObjCCall,ObjCMName,ObjCSkipStr>"$" { g_currentCtx->format+="$$"; }
<ObjCCall,ObjCMName>"("            { g_currentCtx->format+=*yytext; g_braceCount++; }
<ObjCCall,ObjCMName>")"            { g_currentCtx->format+=*yytext; g_braceCount--; }
<ObjCSkipStr>"@"/"\""		   { // needed to prevent matching the global rule (for C#)
                                     g_currentCtx->format+=yytext;
                                   }
<ObjCCall,ObjCMName,ObjCSkipStr>{ID} { g_currentCtx->format+=escapeWord(yytext); }
<ObjCCall,ObjCMName,ObjCSkipStr>.  { g_currentCtx->format+=*yytext; }
<ObjCCall,ObjCMName,ObjCSkipStr>\n { g_currentCtx->format+=*yytext; }

<Body>"]"				{
					  g_theCallContext.popScope();
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
					  //addParmType();
					  //g_parmName=yytext; 
  					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
					}
<MemberCall2,FuncCall,OldStyleArgs,TemplCast>{TYPEKW}/([^a-z_A-Z0-9]) {
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
<MemberCall2,FuncCall>{ID}(({B}*"<"[^\n\[\](){}<>]*">")?({B}*"::"{B}*{ID})?)* {
					  addParmType();
					  g_parmName=yytext; 
					  generateClassOrGlobalLink(*g_code,yytext,!g_insideBody);
					}
<FuncCall>";"				{ // probably a cast, not a function call
  					  g_code->codify(yytext);
					  g_inForEachExpression = FALSE;
					  BEGIN( Body );
  					}
<MemberCall2,FuncCall>,			{
  					  g_code->codify(yytext);
					  g_theVarContext.addVariable(g_parmType,g_parmName);
					  g_parmType.resize(0);g_parmName.resize(0);
					}
<MemberCall2,FuncCall>"("		{
					  g_parmType.resize(0);g_parmName.resize(0);
  					  g_code->codify(yytext);
  					  g_bracketCount++; 
					  g_theCallContext.pushScope();
					  if (YY_START==FuncCall && !g_insideBody)
					  {
					    g_theVarContext.pushScope();
					  }
					}
<MemberCall2,FuncCall>{OPERATOR}        { // operator
  					  if (strcmp(yytext,"*") && 
					      strcmp(yytext,"&") &&
					      strcmp(yytext,"^") &&
					      strcmp(yytext,"%")) // typically a pointer or reference
					  {
					    // not a * or &, or C++/CLI's ^ or %
					    g_parmType.resize(0);g_parmName.resize(0);
					  }
  					  g_code->codify(yytext);
  					}
<MemberCall,MemberCall2,FuncCall>("*"{B}*)?")"	{ 
                                          if (yytext[0]==')') // no a pointer cast
                                          {
                                            //printf("addVariable(%s,%s)\n",g_parmType.data(),g_parmName.data());
					    g_theVarContext.addVariable(g_parmType,g_parmName);
                                          }
                                          else
                                          {
                                            g_parmType.resize(0);
                                            g_parmName.resize(0);
                                          }
					  g_theCallContext.popScope();
					  g_inForEachExpression = FALSE;
					  //g_theCallContext.setClass(0); // commented out, otherwise a()->b() does not work for b().
  					  g_code->codify(yytext);
  					  if (--g_bracketCount<=0) 
					  {
					    if (g_name.isEmpty())
					    {
					      BEGIN( Body );
					    }
					    else
					    {
					      BEGIN( CallEnd ); 
					    }
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
					  if (!g_type.isEmpty())
					  {
					    //fprintf(stderr,"add variable g_type=%s g_name=%s)\n",g_type.data(),g_name.data());
					    g_theVarContext.addVariable(g_type,g_name);
					  }
					  g_parmType.resize(0);g_parmName.resize(0);
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
<CallEnd>("const"|"volatile"|"sealed"|"override")({BN}+("const"|"volatile"|"sealed"|"override"))*/{BN}*(";"|"="|"throw"{BN}*"(") {
					  startFontClass("keyword");
  					  codifyLines(yytext);
					  endFontClass();
  					}
<CallEnd,OldStyleArgs>("const"|"volatile"|"sealed"|"override")*({BN}+("const"|"volatile"|"sealed"|"override"))*{BN}*"{" {
                                          if (g_insideBody)
					  {
					    g_theVarContext.pushScope();
					  }
					  g_theVarContext.addVariable(g_parmType,g_parmName);
					  //g_theCallContext.popScope();
					  g_parmType.resize(0);g_parmName.resize(0);
					  int index = g_name.findRev("::");
					  //fprintf(stderr,"g_name=%s\n",g_name.data());
					  if (index!=-1) 
					  {
					    QCString scope = g_name.left(index);
					    if (!g_classScope.isEmpty()) scope.prepend(g_classScope+"::");
					    ClassDef *cd=getResolvedClass(Doxygen::globalScope,g_sourceFileDef,scope);
					    if (cd)
					    {
					      setClassScope(cd->name());
  					      g_scopeStack.push(SCOPEBLOCK);
                                              DBG_CTX((stderr,"** scope stack push SCOPEBLOCK\n"));
					    }
					    else 
					    {
					      //setClassScope(g_realScope);
  					      g_scopeStack.push(INNERBLOCK);
                                              DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
					    }
					  }
					  else
					  {
                                            DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
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
<CallEnd>"try"				{ // function-try-block
					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
					  g_inFunctionTryBlock=TRUE;
                                        }
<CallEnd>{ID}				{
  					  if (g_insideBody || !g_parmType.isEmpty()) 
					  {
					    REJECT;
					  }
					  // could be K&R style definition
					  addParmType();
					  g_parmName=yytext; 
					  generateClassOrGlobalLink(*g_code,yytext,!g_insideBody);
					  BEGIN(OldStyleArgs);
  					}
<OldStyleArgs>{ID}			{
					  addParmType();
					  g_parmName=yytext; 
					  generateClassOrGlobalLink(*g_code,yytext,!g_insideBody);
  					}
<OldStyleArgs>[,;]			{
  					  g_code->codify(yytext);
					  g_theVarContext.addVariable(g_parmType,g_parmName);
					  if (*yytext==';') g_parmType.resize(0);
					  g_parmName.resize(0);
  					}
<CallEnd,OldStyleArgs>"#"		{
  					  startFontClass("preprocessor");
					  g_lastSkipCppContext = Body;
  					  g_code->codify(yytext);
					  BEGIN( SkipCPP );
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
                                            DBG_CTX((stderr,"** scope stack push SCOPEBLOCK\n"));
  					    g_scopeStack.push(SCOPEBLOCK);
					    setClassScope(g_realScope);
					  }
					  else
					  {
                                            DBG_CTX((stderr,"** scope stack push INNERBLOCK\n"));
  					    g_scopeStack.push(INNERBLOCK);
					  }
  					  g_type.resize(0); g_name.resize(0);
					  BEGIN( Body ); 
					}
<SkipInits>{ID}				{
					  generateClassOrGlobalLink(*g_code,yytext);
  					}
<FuncCall>{ID}/"("			{
					  generateFunctionLink(*g_code,yytext);
					}
<FuncCall>{ID}/("."|"->")               { 
					  g_name=yytext; 
					  generateClassOrGlobalLink(*g_code,yytext);
					  BEGIN( MemberCall2 ); 
					}
<FuncCall,MemberCall2>("("{B}*("*"{B}*)+{ID}*{B}*")"{B}*)/("."|"->") { 
  					  g_code->codify(yytext);
					  int s=0;while (!isId(yytext[s])) s++;
                                          int e=(int)yyleng-1;while (!isId(yytext[e])) e--;
					  g_name=((QCString)yytext).mid(s,e-s+1); 
					  BEGIN( MemberCall2 ); 
					}
<MemberCall2>{ID}/([ \t\n]*"(")         { 
  					  if (!g_args.isEmpty())
					    generateMemberLink(*g_code,g_args,yytext);
					  else
					    generateClassOrGlobalLink(*g_code,yytext);
					  g_args.resize(0);
					  BEGIN( FuncCall );
					}
<MemberCall2>{ID}/([ \t\n]*("."|"->"))  {
  					  //g_code->codify(yytext);
					  g_name=yytext; 
					  generateClassOrGlobalLink(*g_code,yytext);
					  BEGIN( MemberCall2 ); 
    					}
<MemberCall2>"->"|"."			{
  				          if (yytext[0]=='-') // -> could be overloaded
					  {
					    updateCallContextForSmartPointer();
					  }
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
  					  if (g_lastCContext==SkipCPP)
                                          {
                                            startFontClass("preprocessor");
                                          }
  					  BEGIN( g_lastCContext ) ; 
					}
<SkipCxxComment>[^\r\n]*"\\"[\r]?\n	{ // line continuation
  					  codifyLines(yytext);
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
					  nextCodeLine();
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
<MemberCall>[^a-z_A-Z0-9(\n]		{ 
  					  g_code->codify(yytext);
    					  g_type.resize(0);
					  g_name.resize(0);
					  BEGIN(g_memCallContext); 
					}
<*>\n({B}*"//"[!/][^\n]*\n)+		{ // remove special one-line comment
					  if (YY_START==SkipCPP) REJECT;
  					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_yyLineNr+=((QCString)yytext).contains('\n');
					    nextCodeLine();
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
					  if (YY_START==SkipCxxComment)
					  {
					    endFontClass();
					    BEGIN( g_lastCContext ) ;
					  }
  					}
<SkipCPP>\n/.*\n			{ 
  					  codifyLines(yytext);
					  endFontClass();
					  BEGIN( g_lastSkipCppContext ) ;
					}
<*>\n{B}*"//@"[{}].*\n			{ // remove one-line group marker
  					  if (Config_getBool("STRIP_CODE_COMMENTS"))
					  {
					    g_yyLineNr+=2;
					    nextCodeLine();
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
					  if (YY_START==SkipCxxComment)
					  {
					    endFontClass();
					    BEGIN( g_lastCContext ) ;
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
					    nextCodeLine();
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
					    nextCodeLine();
					  }
					  else
					  {
					    startFontClass("comment");
					    codifyLines(yytext);
					    endFontClass();
					  }
  					}
<*>"//"[!/][^\n]*\n			{ // strip special one-line comment
                                          if (YY_START==SkipComment || YY_START==SkipString) REJECT;
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
<*>"/*[tag:"[^\]\n]*"]*/"{B}*		{ // special pattern /*[tag:filename]*/ to force linking to a tag file
  					  g_forceTagReference=yytext;
					  int s=g_forceTagReference.find(':');
					  int e=g_forceTagReference.findRev(']');
					  g_forceTagReference = g_forceTagReference.mid(s+1,e-s-1);
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
                                          if (YY_START==SkipString) REJECT;
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
<*>"/*"("!"?)"*/"			{ 
                                          if (YY_START==SkipString) REJECT;
                                          if (!Config_getBool("STRIP_CODE_COMMENTS"))
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
<*>@\"					{ // C# verbatim string
					  startFontClass("stringliteral");
  					  g_code->codify(yytext);
					  g_lastVerbStringContext=YY_START;
					  BEGIN(SkipVerbString);
					}
<*>"//"					{ 
  					  startFontClass("comment");
  					  g_code->codify(yytext);
  					  g_lastCContext = YY_START ;
					  BEGIN( SkipCxxComment ) ;
					}
<*>"("|"["					{
  					  g_code->codify(yytext);
					  g_theCallContext.pushScope();
  					}
<*>")"|"]"					{
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

static void saveObjCContext()
{
  if (g_currentCtx)
  {
    g_currentCtx->format+=QCString().sprintf("$c%d",g_currentCtxId);
    if (g_braceCount==0 && YY_START==ObjCCall)
    {
      g_currentCtx->objectTypeOrName=g_currentCtx->format.mid(1);
      //printf("new type=%s\n",g_currentCtx->objectTypeOrName.data());
    }
    g_contextStack.push(g_currentCtx);
  }
  else
  {
    //printf("Trying to save NULL context!\n");
  }
  ObjCCallCtx *newCtx = new ObjCCallCtx;
  newCtx->id = g_currentCtxId;
  newCtx->lexState = YY_START;
  newCtx->braceCount = g_braceCount;
  newCtx->objectType = 0;
  newCtx->objectVar = 0;
  newCtx->method = 0;
  //printf("save state=%d\n",YY_START);
  g_contextDict.insert(g_currentCtxId,newCtx);
  g_currentCtx = newCtx;
  g_braceCount = 0;
  g_currentCtxId++;
}

static void restoreObjCContext()
{
  //printf("restore state=%d->%d\n",YY_START,g_currentCtx->lexState);
  BEGIN(g_currentCtx->lexState);
  g_braceCount = g_currentCtx->braceCount;
  if (!g_contextStack.isEmpty())
  {
    g_currentCtx = g_contextStack.pop();
  }
  else
  {
    g_currentCtx = 0;
    //printf("Trying to pop context while g_contextStack is empty!\n");
  }
}

void resetCCodeParserState()
{
  //printf("***initParseCodeContext()\n");
  g_forceTagReference.resize(0);
  g_theVarContext.clear();
  g_classScopeLengthStack.setAutoDelete(TRUE);
  g_classScopeLengthStack.clear();
  delete g_codeClassSDict;
  g_codeClassSDict = new ClassSDict(17);
  g_codeClassSDict->setAutoDelete(TRUE);
  g_codeClassSDict->clear();
  g_curClassBases.clear();
  g_anchorCount = 0;
}

void parseCCode(CodeOutputInterface &od,const char *className,const QCString &s, 
                  bool exBlock, const char *exName,FileDef *fd,
		  int startLine,int endLine,bool inlineFragment,
		  MemberDef *memberDef,bool showLineNumbers,Definition *searchCtx)
{
  //printf("***parseCode() exBlock=%d exName=%s fd=%p className=%s searchCtx=%s\n",
  //      exBlock,exName,fd,className,searchCtx?searchCtx->name().data():"<none>");
  if (s.isEmpty()) return;
  if (g_codeClassSDict==0)
  {
    resetCCodeParserState();
  }
  g_code = &od;
  g_inputString   = s;
  g_inputPosition = 0;
  g_currentFontClass = 0;
  g_needsTermination = FALSE;
  g_searchCtx = searchCtx;
  g_inFunctionTryBlock = FALSE;
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
  //printf("parseCCode %s\n",className);
  g_exampleBlock  = exBlock; 
  g_exampleName   = exName;
  g_sourceFileDef = fd;
  g_lineNumbers   = fd!=0 && showLineNumbers;
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
    g_insideObjC = g_sourceFileDef->name().lower().right(2)==".m" || 
                   g_sourceFileDef->name().lower().right(3)==".mm"; 
  }
  g_currentDefinition = 0;
  g_currentMemberDef = 0;
  g_searchingForBody = exBlock;
  g_insideBody = FALSE;
  g_bracketCount = 0;
  if (!g_exampleName.isEmpty())
  {
    g_exampleFile = convertNameToFile(g_exampleName+"-example",FALSE,TRUE);
    //printf("g_exampleFile=%s\n",g_exampleFile.data());
  }
  g_includeCodeFragment = inlineFragment;
  //printf("** exBlock=%d exName=%s include=%d\n",exBlock,exName,inlineFragment);
  startCodeLine();
  g_type.resize(0);
  g_name.resize(0);
  g_args.resize(0);
  g_parmName.resize(0);
  g_parmType.resize(0);
  if (memberDef) setParameterList(memberDef);
  codeYYrestart( codeYYin );
  BEGIN( Body );
  codeYYlex();
  g_lexInit=TRUE;
  if (g_needsTermination)
  {
    endFontClass();
    g_code->endCodeLine();
  }
  if (cleanupSourceDef)
  {
    // delete the temporary file definition used for this example
    delete g_sourceFileDef;
    g_sourceFileDef=0;
  }
  return;
}

void codeFreeScanner()
{
#if defined(YY_FLEX_SUBMINOR_VERSION) 
  if (g_lexInit)
  {
    codeYYlex_destroy();
  }
#endif
}



#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
  void codeYYdummy() { yy_flex_realloc(0,0); } 
}
#elif YY_FLEX_SUBMINOR_VERSION<33
#error "You seem to be using a version of flex newer than 2.5.4 but older than 2.5.33. These versions do NOT work with doxygen! Please use version <=2.5.4 or >=2.5.33 or expect things to be parsed wrongly!"
#endif

