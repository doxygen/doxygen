/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
#include "searchindex.h"

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
  
static BaseCodeDocInterface * g_code;

static ClassSDict    g_codeClassSDict(17);
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
static QStack<int>   g_scopeStack;      //!< 1 if bracket starts a scope, 
                                        //   2 for internal blocks 
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
static int           g_lastVerbStringContext;
static int           g_memCallContext;
static int	     g_lastCContext;

static bool          g_insideObjC;
static bool          g_insideProtocolList;

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
static QStack<ObjCCallCtx> g_contextStack;
static QIntDict<ObjCCallCtx> g_contextDict;
static QIntDict<QCString> g_nameDict;
static QIntDict<QCString> g_objectDict;
static int g_braceCount=0;
  
static void saveObjCContext();
static void restoreObjCContext();

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
  DBG_CTX((stderr,"** addVariable trying: type=%s name=%s g_currentDefinition=%s\n",
	ltype.data(),lname.data(),g_currentDefinition?g_currentDefinition->name().data():"<none>"));
  Scope *scope = m_scopes.count()==0 ? &m_globalScope : m_scopes.getLast();
  ClassDef *varType;
  int i=0;
  if (
      (varType=g_codeClassSDict[ltype]) ||  // look for class definitions inside the code block
      (varType=getResolvedClass(g_currentDefinition,g_sourceFileDef,ltype)) // look for global class definitions
     ) 
  {
    DBG_CTX((stderr,"** addVariable type=%s name=%s\n",ltype.data(),lname.data()));
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
	   (varType=g_codeClassSDict[typeName]) ||
           // otherwise look for global class definitions
           (varType=getResolvedClass(g_currentDefinition,g_sourceFileDef,typeName))
	 ) && // and it must be a template
         varType->templateArguments())
    {
      newDef = varType->getVariableInstance( templateArgs );
    }
    if (newDef)
    {
      DBG_CTX((stderr,"** addVariable type=%s templ=%s name=%s\n",typeName.data(),templateArgs.data(),lname.data()));
      scope->append(lname, newDef);
    }
    else
    {
      // Doesn't seem to be a template. Try just the base name.
      addVariable(typeName,name);
    }
  }
  else // add a dummy entry so the name is hidden to avoid false links
  {
    DBG_CTX((stderr,"** addVariable: dummy context\n"));
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

static void setCurrentDoc(const QCString &name,const QCString &base,const QCString &anchor="")
{
  static bool searchEngineEnabled=Config_getBool("SEARCHENGINE");
  if (searchEngineEnabled)
  {
    Doxygen::searchIndex->setCurrentDoc(name,base,anchor);
  }
}

static void addToSearchIndex(const char *text)
{
  static bool searchEngineEnabled=Config_getBool("SEARCHENGINE");
  if (searchEngineEnabled)
  {
    Doxygen::searchIndex->addWord(text);
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
      g_insideBody = FALSE;
      g_searchingForBody = TRUE;
      g_realScope = d->name().copy();
      //printf("Real scope: `%s'\n",g_realScope.data());
      g_bodyCurlyCount = 0;
      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",g_yyLineNr);
      if (g_currentMemberDef)
      {
        g_code->writeLineNumber(g_currentMemberDef->getReference(),
	                        g_currentMemberDef->getOutputFileBase(),
	                        g_currentMemberDef->anchor(),g_yyLineNr);
        setCurrentDoc(
                                g_currentMemberDef->qualifiedName(),
	                        g_sourceFileDef->getSourceFileBase(),
	                        lineAnchor);
      }
      else
      {
        g_code->writeLineNumber(d->getReference(),
	                        d->getOutputFileBase(),
	                        0,g_yyLineNr);
        setCurrentDoc(
                                d->qualifiedName(),
	                        g_sourceFileDef->getSourceFileBase(),
	                        lineAnchor);
      }
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
      cd=getResolvedClass(g_currentDefinition,g_sourceFileDef,g_classScope+"::"+clName);
    }
    if (cd==0)
    {
      cd=getResolvedClass(g_currentDefinition,g_sourceFileDef,clName);
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
    //fprintf(stderr,"local variable\n");
    if (mcd!=VariableContext::dummyContext)
    {
      //fprintf(stderr,"local var `%s' mcd=%s\n",name.data(),mcd->name().data());
      g_theCallContext.setClass(mcd);
    }
  }
  else
  {
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
	  g_theCallContext.setClass(stripClassName(md->typeString()));
	}
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
  if ((Config_getBool("REFERENCES_RELATION") || Config_getBool("CALL_GRAPH")) && 
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
  if (*clName=='~') // correct for matching negated values i.s.o. destructors.
  {
    g_code->codify("~");
    clName++;
  }
  QCString className=clName;
  if (className.isEmpty()) return;
  if (g_insideProtocolList)
  {
    className+="-p";
  }
  ClassDef *cd=0;
  MemberDef *md=0;

  //printf("generateClassOrGlobalLink(className=%s)\n",className.data());
  if (!g_theVarContext.findVariable(className)) // not a local variable
  {
    Definition *d = g_currentDefinition;
    //printf("d=%p g_sourceFileDef=%p\n",d,g_currentDefinition);
    cd = getResolvedClass(d,g_sourceFileDef,className,&md);
    if (cd==0 && md==0 && (i=className.find('<'))!=-1)
    {
      cd=getResolvedClass(d,g_sourceFileDef,className.left(i),&md);
    }
    //printf("is not found as a variable %s\n",cd?cd->name().data():"<null>");
  }
  else
  {
    //printf("is a local variable!\n");
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
    writeMultiLineCodeLink(ol,cd->getReference(),cd->getOutputFileBase(),0,clName);
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
    //printf("class %s not linkable! cd=%p md=%p typeOnly=%d\n",clName,cd,md,typeOnly);
    if (md!=0 || (cd==0 && !typeOnly)) // not a class, see if it is a global enum/variable/typedef.
    {
      if (md==0) // not found as a typedef
      {
	md = setCallContextForVar(clName);
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
      if (md)
      {
        //printf("is a global md=%p g_currentDefinition=%s\n",md,g_currentDefinition?g_currentDefinition->name().data():"<none>");
        //Definition *d = md->getOuterScope()==Doxygen::globalScope ?
	//                md->getBodyDef() : md->getOuterScope();
	//printf("definition %s\n",d?d->name().data():"<none>");
        //if (md->getGroupDef()) d = md->getGroupDef();
	if (md->isLinkable())
	{
	  writeMultiLineCodeLink(ol,md->getReference(),md->getOutputFileBase(),md->anchor(),clName);
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
    codifyLines(clName);
    addToSearchIndex(clName);
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
     
      writeMultiLineCodeLink(ol,md->getReference(),
	                        md->getOutputFileBase(),
	                        md->anchor(),
				text ? text : memberName);
      addToSearchIndex(text ? text : memberName);
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

	//printf("g_currentDefiniton=%p g_currentMemberDef=%p xmd=%p g_insideBody=%d\n",g_currentDefinition,g_currentMemberDef,xmd,g_insideBody);

	if (xmd->templateMaster()) xmd = xmd->templateMaster();

	// add usage reference
	if (g_currentDefinition && g_currentMemberDef &&
	    xmd!=g_currentMemberDef && g_insideBody)
	{
	  addDocCrossReference(g_currentMemberDef,xmd);
	}

	// write the actual link
	writeMultiLineCodeLink(ol,xmd->getReference(),
	    xmd->getOutputFileBase(),xmd->anchor(),memName);
        addToSearchIndex(memName);
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
    vcd = getResolvedClass(g_currentDefinition,g_sourceFileDef,g_classScope);
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
  addToSearchIndex(memName);
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

//----------------------------------------------------------------------------

// recursively writes a linkified Objective-C method call
static void writeObjCMethodCall(ObjCCallCtx *ctx)
{
  if (ctx==0) return;
  const char *p = ctx->format.data();
  //printf("writeObjCMethodCall(%s) obj=%s method=%s\n",
  //    ctx->format.data(),ctx->objectTypeOrName.data(),ctx->methodName.data());
  char c;
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
              writeMultiLineCodeLink(*g_code,
		                     ctx->method->getReference(),
	                             ctx->method->getOutputFileBase(),
	                             ctx->method->anchor(),
				     pName->data());
	      if (g_currentMemberDef)
	      {
	        addDocCrossReference(g_currentMemberDef,ctx->method);
	      }
	    }
	    else
	    {
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
	      writeMultiLineCodeLink(*g_code,
		    ctx->objectVar->getReference(),
		    ctx->objectVar->getOutputFileBase(),
		    ctx->objectVar->anchor(),
		    pObject->data());
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
	      writeMultiLineCodeLink(*g_code,
		    cd->getReference(),
		    cd->getOutputFileBase(),
		    0,
		    pObject->data());
	    }
	    else // object still needs to be resolved
	    {
	      ClassDef *cd = getResolvedClass(g_currentDefinition, 
		  g_sourceFileDef, *pObject);
	      if (cd && cd->isLinkable())
	      {
		if (ctx->objectType==0) ctx->objectType=cd;
	   	writeMultiLineCodeLink(*g_code,
		    cd->getReference(),
		    cd->getOutputFileBase(),
		    0,
		    pObject->data());
	      }
	      else
	      {
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
		MemberName *mn=Doxygen::memberNameSDict.find(ctx->methodName);
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
ID	"$"?[a-z_A-Z][a-z_A-Z0-9]*
SCOPENAME "$"?(({ID}?{BN}*"::"{BN}*)*)((~{BN}*)?{ID})
TEMPLIST "<"[^\"\}\{\(\)\/\n\>]*">"
SCOPETNAME ((({ID}{TEMPLIST}?){BN}*"::"{BN}*)*)((~{BN}*)?{ID})
SCOPEPREFIX ({ID}{TEMPLIST}?{BN}*"::"{BN}*)+
KEYWORD_OBJC ("@public"|"@private"|"@protected"|"@class"|"@implementation"|"@interface"|"@end"|"@selector"|"@protocol")
KEYWORD ("asm"|"auto"|"class"|"const"|"const_cast"|"delete"|"dynamic_cast"|"enum"|"explicit"|"extern"|"false"|"friend"|"inline"|"mutable"|"namespace"|"new"|"operator"|"private"|"protected"|"public"|"register"|"reinterpret_cast"|"sizeof"|"static"|"static_cast"|"struct"|"template"|"this"|"self"|"true"|"typedef"|"typeid"|"typename"|"union"|"using"|"virtual"|"volatile"|"abstract"|"final"|"import"|"synchronized"|"transient"|KEYWORD_OBJC)
FLOWKW  ("break"|"case"|"catch"|"continue"|"default"|"do"|"else"|"for"|"goto"|"if"|"return"|"switch"|"throw"|"throws"|"try"|"while")
TYPEKW  ("bool"|"char"|"double"|"float"|"int"|"long"|"short"|"signed"|"unsigned"|"void"|"wchar_t"|"boolean"|"id"|"SEL")
CHARLIT   (("'"\\[0-7]{1,3}"'")|("'"\\."'")|("'"[^' \\\n]{1,4}"'"))

%option noyywrap

%x      SkipString
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
%x      Bases
%x      SkipSharp
%x      ReadInclude
%x      TemplDecl
%x	CallEnd
%x      ObjCMethod
%x	ObjCParams
%x	ObjCParamType
%x      ObjCCall
%x      ObjCMName
%x      ObjCSkipStr
%x      OldStyleArgs

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
					    g_inClass=TRUE;
  					    if (g_searchingForBody)
					    {
					      g_searchingForBody=FALSE;
					      g_insideBody=TRUE;
					    }
					    if (g_insideBody) g_bodyCurlyCount++;
					    if (!g_curClassName.isEmpty()) // valid class name
					    {
					      pushScope(g_curClassName);
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
  					  g_code->codify(yytext);
  					  g_parmType=yytext;
  					}
<ObjCParamType>")"			{
  					  g_code->codify(yytext);
  					  BEGIN(ObjCParams);
  					}
<ObjCParams>{ID}			{
  					  g_code->codify(yytext);
  					  g_parmName=yytext;
					  g_theVarContext.addVariable(g_parmType,g_parmName);
					  g_parmType.resize(0);g_parmName.resize(0);
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

					  int *scope = g_scopeStack.pop();
  					  if (scope==SCOPEBLOCK || scope==CLASSBLOCK) 
					  {
					    popScope();
					  }

  					  g_code->codify(yytext);

  					  g_inClass=FALSE; 

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
  					    if (scope==SCOPEBLOCK || scope==CLASSBLOCK) 
					    {
					      popScope();
					    }
					    g_insideBody=FALSE;
					  }

					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();

  					  g_inClass=FALSE; 

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
<ClassName,ClassVar>[*&]+       	{
					  addType();
					  g_code->codify(yytext);
					  BEGIN( Body ); // variable of type struct *
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
  					    g_scopeStack.push(CLASSBLOCK);
					    pushScope(g_curClassName);
					    //printf("***** g_curClassName=%s\n",g_curClassName.data());
					    if (getResolvedClass(g_currentDefinition,g_sourceFileDef,g_curClassName)==0)
					    {
					      ClassDef *ncd=new ClassDef("<code>",1,
				 		  g_curClassName,ClassDef::Class,0,0,FALSE);
					      g_codeClassSDict.append(g_curClassName,ncd);
					      // insert base classes.
					      char *s=g_curClassBases.first();
					      while (s)
					      {
						ClassDef *bcd;
						bcd=g_codeClassSDict[s];
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
<Bases>{ID}	                        { 
					  //printf("%s:addBase(%s)\n",g_ccd.name.data(),yytext);
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
<Body,TemplDecl,ObjCMethod>{TYPEKW}/{B}* {
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
<Body>"("{B}*("*"{B}*)+{SCOPENAME}*{B}*")"/{B}*	{  // (*p)->func() but not "if (p) ..."
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
					  //g_theVarContext.addVariable(g_type,yytext);
  					  g_bracketCount=0;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
					}
<FuncCall,Body,MemberCall,MemberCall2,SkipInits>\"	{
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
<SkipString>@?\"				{
  					  g_code->codify(yytext);
					  endFontClass();
  					  BEGIN( g_lastStringContext );
  					}
<SkipString>\\.				{
  					  g_code->codify(yytext);
					}
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
<Body>"$"?"this->"			{ g_code->codify(yytext); }
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
					      addToSearchIndex(yytext);
					    }
  					    g_name.resize(0);
					  }
					  else
					  {
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
					    if (!generateClassMemberLink(*g_code,g_theCallContext.getClass(),yytext))
					    {
					      g_code->codify(yytext);
					      addToSearchIndex(yytext);
					    }
  					    g_name.resize(0);
					  }
					  else
					  {
  					    g_code->codify(yytext);
					    addToSearchIndex(yytext);
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
					  if (g_insideObjC && *yytext=='[')
					  {
					    //printf("Found start of ObjC call!\n");
					    // start of a method call
					    g_contextDict.setAutoDelete(TRUE);
					    g_nameDict.setAutoDelete(TRUE);
					    g_objectDict.setAutoDelete(TRUE);
					    g_contextDict.clear();
					    g_nameDict.clear();
					    g_objectDict.clear();
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
					    if (!g_type.isEmpty()) 
					    {
					      if (g_scopeStack.top()!=CLASSBLOCK)
					      {
					        g_theVarContext.addVariable(g_type,g_name);
					      }
					      g_name.resize(0);
					    }
					    if (*yytext==';') 
					    {
					      g_type.resize(0);
					      g_name.resize(0);
					    }
					    else if (*yytext=='[')
					    {
					      g_theCallContext.pushScope();
					    }
					    g_args.resize(0);
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
                                      g_currentCtx->format+=yytext;
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
					  addParmType();
					  g_parmName=yytext; 
  					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
					}
<MemberCall2,FuncCall,OldStyleArgs>{TYPEKW}/([^a-z_A-Z0-9]) {
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
					  generateClassOrGlobalLink(*g_code,yytext,!g_insideBody);
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
					  g_theVarContext.addVariable(g_parmType,g_parmName);
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
<CallEnd,OldStyleArgs>({BN}"const"|"volatile")*{BN}*"{" {
                                          if (g_insideBody)
					  {
					    g_theVarContext.pushScope();
					  }
					  g_theVarContext.addVariable(g_parmType,g_parmName);
					  g_theCallContext.popScope();
					  g_parmType.resize(0);g_parmName.resize(0);
					  int index = g_name.findRev("::");
					  if (index!=-1) 
					  {
					    ClassDef *cd=getResolvedClass(Doxygen::globalScope,g_sourceFileDef,g_name.left(index));
					    if (cd)
					    {
					      setClassScope(cd->name());
					    }
					    else
					    {
					      setClassScope(g_realScope);
					    }
  					    g_scopeStack.push(SCOPEBLOCK);
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
					  if (YY_START==SkipCxxComment)
					  {
					    endFontClass();
					    BEGIN( g_lastCContext ) ;
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
                                          if (YY_START==SkipComment) REJECT;
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
  if (fd) 
  {
    setCurrentDoc(fd->name(),fd->getSourceFileBase());
    g_insideObjC = fd->name().lower().right(2)==".m" || 
                   fd->name().lower().right(3)==".mm"; 
  }
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

#if !defined(YY_FLEX_SUBMINOR_VERSION) 
extern "C" { // some bogus code to keep the compiler happy
  void codeYYdummy() { yy_flex_realloc(0,0); } 
}
#else
#error "You seem to be using a version of flex newer than 2.5.4. These are currently incompatible with 2.5.4, and do NOT work with doxygen! Please use version 2.5.4 or expect things to be parsed wrongly! A bug report has been submitted (#732132)."
#endif

