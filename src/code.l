/******************************************************************************
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

%{

/*
 *	includes
 */
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <qregexp.h>

#include "qtbc.h"
#include "scanner.h"
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"

#define YY_NEVER_INTERACTIVE 1
  
#define SCOPEBLOCK (int *)4
#define INNERBLOCK (int *)8

/*! local class definition, used for classes that are defined
 *  inside code fragments.
 */
class CodeClassDef 
{
  public:
    CodeClassDef() {}
    CodeClassDef(const CodeClassDef &d)
    {
      name = d.name;
      bases = d.bases;
    }
   ~CodeClassDef() {}

    QCString name;
    QStrList bases;
};  
  
/*! local member definition, used for variables that are defined
 *  inside code fragments.
 */
class CodeVarDef
{
  public:
    CodeVarDef() {}
    CodeVarDef(const CodeVarDef &d) 
    { 
      name = d.name; 
      type = d.type; 
      classScope = d.classScope;  
    }
   ~CodeVarDef() {} 
    
    QCString name;
    QCString type; 
    QCString classScope;
};

typedef QList<CodeClassDef> CodeClassList; 
typedef QDict<CodeClassDef> CodeClassDict;
typedef QList<CodeVarDef>   CodeVarList; 

/* -----------------------------------------------------------------
 *	statics
 */
  
static OutputDocInterface * g_code;
//static CodeClassList g_codeClassList;
static CodeClassDict g_codeClassDict(1009);
static CodeVarList   g_codeVarList;
static CodeVarList   g_codeParmList;
static const char *  g_inputString;     //!< the code fragment as text
static int	     g_inputPosition;   //!< read offset during parsing 
static int           g_inputLines;      //!< number of line in the code fragment
static int	     g_yyLineNr;        //!< current line number
static int	     g_lastCContext;
static int	     g_lastSpecialCContext;
static int           g_lastStringContext;
static int	     g_bracketCount = 0;
static int	     g_curlyCount   = 0;
static int	     g_sharpCount   = 0;
static bool          g_insideTemplate = FALSE;
static QCString      g_type;
static QCString      g_name;
static QCString      g_args;
static QCString      g_parmType;
static QCString      g_parmName;
static bool          g_inClass;
static QCString      g_classScope;
static QCString      g_realScope;
static QStack<int>   g_scopeStack;      // 1 if bracket starts a scope, 2 for internal blocks 
static CodeClassDef  g_ccd;
static CodeVarDef    g_cvd;
static bool          g_exampleBlock;
static QCString      g_exampleName;
static QCString      g_exampleFile;
static int           g_anchorCount;
static FileDef *     g_sourceFileDef;
static Definition *  g_currentDefinition;
static MemberDef *   g_currentMemberDef;
static bool          g_includeCodeFragment;
static const char *  g_currentFontClass;
static bool          g_searchingForBody;
static bool          g_insideBody;
static int           g_bodyCurlyCount;
static ClassDef *    g_classVar;
static QCString      g_saveName;
static QCString      g_saveType;
static int           g_memCallContext;

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
    QCString lineNumber,lineAnchor;
    lineNumber.sprintf("%05d",g_yyLineNr);
    lineAnchor.sprintf("l%05d",g_yyLineNr);
   
    Definition *d   = g_sourceFileDef->getSourceDefinition(g_yyLineNr);
    g_code->startLineNumber();
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
      if (g_currentMemberDef) anchor=g_currentMemberDef->anchor();
      g_code->startCodeAnchor(lineAnchor);
      g_code->writeCodeLink(d->getReference(),d->getOutputFileBase(),
	               anchor,lineNumber);
      g_code->endCodeAnchor();
    }
    else
    {
      g_code->codify(lineNumber);
    }
    g_code->endLineNumber();
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
static void writeMultiLineCodeLink(OutputDocInterface &ol,
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
      ol.writeCodeLink(ref,file,anchor,sp);
      endCodeLine();
      if (g_yyLineNr<g_inputLines) 
      {
	startCodeLine();
      }
    }
    else
    {
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

static void addVariable()
{
  g_cvd.name=g_name.copy().simplifyWhiteSpace();
  g_cvd.type=g_type.copy().simplifyWhiteSpace();
  if (g_type.isEmpty()) 
  {
    return;
  }
  else 
  {
    int i;
    if ((getResolvedClass(g_currentDefinition,g_cvd.type)) || (g_codeClassDict[g_cvd.type])) 
    {
      //printf("adding variable `%s' `%s'\n",g_cvd.type.data(),g_cvd.name.data());
      g_cvd.classScope=g_classScope;
      g_codeVarList.append(new CodeVarDef(g_cvd)); // add it to a list
    }
    else if ((i=g_cvd.type.find('<'))>0)
    {
      g_cvd.type = g_cvd.type.left(i);
      if ((getResolvedClass(g_currentDefinition,g_cvd.type)) || (g_codeClassDict[g_cvd.type.left(i)]))
      {
        //printf("adding template type variable `%s' `%s'\n",g_cvd.type.data(),g_cvd.name.data());
        g_cvd.classScope=g_classScope;
        g_codeVarList.append(new CodeVarDef(g_cvd));
      }
    }
    //printf("g_codeVarList.count()=%d\n",g_codeVarList.count());
  }
}

static void addParameter()
{
  g_cvd.name=g_parmName.copy().simplifyWhiteSpace();
  g_cvd.type=g_parmType.copy().simplifyWhiteSpace();
  //printf("searching for parameter `%s' `%s'\n",g_cvd.type.data(),g_cvd.name.data());
  if (g_cvd.type.isEmpty()) 
  {
    return;
  }
  else 
  {
    if (g_cvd.type.left(7)=="struct ") g_cvd.type=g_cvd.type.right(g_cvd.type.length()-7);
    int i;
    if ((getResolvedClass(g_currentDefinition,g_cvd.type)) || (g_codeClassDict[g_cvd.type])) 
    {
      //printf("adding parameter `%s' `%s'\n",g_cvd.type.data(),g_cvd.name.data());
      g_cvd.classScope=g_classScope;
      g_codeParmList.append(new CodeVarDef(g_cvd)); // add it to a list
    }
    else if ((i=g_cvd.type.find('<'))>0)
    {
      g_cvd.type = g_cvd.type.left(i);
      if ((getResolvedClass(g_currentDefinition,g_cvd.type)) || (g_codeClassDict[g_cvd.type.left(i)]))
      {
        //printf("adding template type parameter `%s' `%s'\n",g_cvd.type.data(),g_cvd.name.data());
        g_cvd.classScope=g_classScope;
        g_codeParmList.append(new CodeVarDef(g_cvd));
      }
    }
    else
    {
      //printf("parameter `%s' `%s' not found!\n",g_cvd.type.data(),g_cvd.name.data());
    }
    //printf("g_codeParmList.count()=%d\n",g_codeParmList.count());
  }
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
    if (i!=-1) 
      g_parmType = g_parmType.left(i);
    i = g_parmType.find('&');
    if (i!=-1) 
      g_parmType = g_parmType.left(i);
    if (g_parmType.left(6)=="const ") 
      g_parmType = g_parmType.right(g_parmType.length()-6);
    g_parmType=g_parmType.stripWhiteSpace();
    addParameter();
    a = al->next();
  }
}

static void generateClassLink(OutputDocInterface &ol,char *clName,int *clNameLen=0)
{
  int i=0;
  QCString className=clName;
  if (clNameLen) *clNameLen=0;
  if (className.isEmpty()) return;
  ClassDef *cd=getResolvedClass(g_currentDefinition,className);
  if (cd==0 && (i=className.find('<'))!=-1)
  {
    cd=getResolvedClass(g_currentDefinition,className.left(i));
  }
  if (cd && cd->isLinkable())
  {
    if (g_exampleBlock)
    {
      QCString anchor;
      anchor.sprintf("_a%d",g_anchorCount);
      //printf("addExampleClass(%s,%s,%s)\n",anchor.data(),g_exampleName.data(),
      //                                   g_exampleFile.data());
      if (cd->addExample(anchor,g_exampleName,g_exampleFile))
      {
	ol.pushGeneratorState();
	//bool latexOn = ol.isEnabled(OutputGenerator::Latex);
	//if (latexOn) ol.disable(OutputGenerator::Latex);
	ol.disable(OutputGenerator::Latex);
	ol.disable(OutputGenerator::RTF);
	ol.writeAnchor(0,anchor);
	//if (latexOn) ol.enable(OutputGenerator::Latex);
	ol.popGeneratorState();
	g_anchorCount++;
      }
    }
    writeMultiLineCodeLink(ol,cd->getReference(),cd->getOutputFileBase(),0,className);
    if (clNameLen) *clNameLen=className.length()-i-1;
  }
  else
  {
    MemberName *mn;
    if (cd==0 && (mn=Doxygen::functionNameSDict[clName]))
    {
      if (mn->count()==1)
      {
	MemberDef *md=mn->getFirst();
	Definition *d=md->getNamespaceDef();
	if (d==0) d=md->getFileDef();
	if (d && md->isLinkable())
	{
          writeMultiLineCodeLink(ol,d->getReference(),d->getOutputFileBase(),md->anchor(),clName);
	  return;
	}
      }
    }
    codifyLines(clName);
    if (clNameLen) *clNameLen=className.length()-1;
  }
}

static ClassDef *stripClassName(const char *s)
{
  QCString tmp=s;
  if (tmp.isEmpty()) return 0;
  static const QRegExp re("[a-z_A-Z][a-z_A-Z0-9:]*");
  int p=0,i,l;
  while ((i=re.match(tmp,p,&l))!=-1)
  {
    ClassDef *cd=0;
    QCString clName = tmp.mid(i,l);
    //printf("g_classScope=`%s' clName=`%s'\n",g_classScope.data(),clName.data());
    if (!g_classScope.isEmpty())
    {
      cd=getResolvedClass(g_currentDefinition,g_classScope+"::"+clName);
    }
    if (cd==0)
    {
      cd=getResolvedClass(g_currentDefinition,clName);
    }
    if (cd) 
    {
      //printf("stripClassName(%s)=%s\n",s,cd->name().data());
      return cd;
    }
    p=i+l;
  }
  //printf("stripClassName(%s)=<null>\n",s);
  return 0;
}

static bool getLink(const char *className,
                    const char *memberName,OutputDocInterface &result,
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
	//bool latexEnabled = result.isEnabled(OutputGenerator::Latex);
	result.pushGeneratorState();
	//if (latexEnabled) result.disable(OutputGenerator::Latex);
	result.disable(OutputGenerator::Latex);
	result.writeAnchor(0,anchor);
	result.popGeneratorState();
	//if (latexEnabled) result.enable(OutputGenerator::Latex);
	g_anchorCount++;
      }
    }
    Definition *d=0;
    if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else d=gd;

    if (d && d->isLinkable())
    {
      g_classVar = stripClassName(md->typeString());
      if (g_currentDefinition && g_currentMemberDef &&
	  md!=g_currentMemberDef && g_insideBody)
      {
        md->addSourceReference(g_currentMemberDef);
      }
      //printf("d->getOutputBase()=`%s' name=`%s' member name=`%s'\n",d->getOutputFileBase().data(),d->name().data(),md->name().data());
      //printf("g_classVar=`%s' type=`%s'\n",g_classVar ? g_classVar->name().data() : "none",md->typeString());
     
      writeMultiLineCodeLink(result,d->getReference(),d->getOutputFileBase(),
	                       md->anchor(),text ? text : memberName);
      return TRUE;
    } 
  }
  return FALSE;
}

static bool generateClassMemberLink(OutputDocInterface &ol,ClassDef *mcd,const char *memName)
{
  //printf("generateClassMemberLink(class=%s,member=%s)\n",mcd->name().data(),memName);
  MemberName *mmn=Doxygen::memberNameSDict[memName];
  if (mmn)
  {
    MemberNameIterator mmni(*mmn);
    MemberDef *mmd,*xmd=0;
    ClassDef *xcd=0;
    const int maxInheritanceDepth = 100000;
    int mdist=maxInheritanceDepth;
    for (;(mmd=mmni.current());++mmni)
    {
      int m=minClassDistance(mcd,mmd->getClassDef());
      if (m<mdist && mmd->getClassDef()->isLinkable())
      {
	mdist=m;
	xcd=mmd->getClassDef();
	xmd=mmd;
      }
    }
    if (mdist!=maxInheritanceDepth)
    {
      // extract class definition of the return type in order to resolve
      // a->b()->c() like call chains
      g_classVar = stripClassName(xmd->typeString());
      //printf("g_classVar=%s->%p\n",xmd->typeString(),g_classVar);

      // add usage reference
      if (g_currentDefinition && g_currentMemberDef &&
	  xmd!=g_currentMemberDef && g_insideBody)
      {
	xmd->addSourceReference(g_currentMemberDef);
      }

      // write the actual link
      writeMultiLineCodeLink(ol,xcd->getReference(),
	  xcd->getOutputFileBase(),xmd->anchor(),memName);
      return TRUE;
    }
  }
  return FALSE;
}

static void generateMemberLink(OutputDocInterface &ol,const char *varName,
            char *memName)
{
  //printf("generateMemberLink(object=%s,mem=%s) classScope=%s\n",
  //                          varName,memName,g_classScope.data());
  CodeVarDef *cvd=g_codeParmList.last();
  while (cvd && cvd->name!=varName) 
  {
    cvd=g_codeParmList.prev();
  }
  if (!cvd)
  {
    cvd=g_codeVarList.last();
    while (cvd && cvd->name!=varName) cvd=g_codeVarList.prev();
  }
  if (cvd) // variable found
  {
    //printf("variable found type=%s!\n",cvd->type.data());
    CodeClassDef *ccd=g_codeClassDict[cvd->type];
    if (ccd)
    {
      //printf("Class found!\n");
      OutputDocInterface *result = ol.clone();
      if (getLink(ccd->name,memName,*result)) 
      {
        //printf("Found result!\n");
        ol.append(result);
	delete result;
        return;
      }
      char *s=ccd->bases.first();
      while (s)
      {
	OutputDocInterface *result = ol.clone();
	if (getLink(s,memName,*result)) 
	{
	  //printf("Found result!\n");
	  ol.append(result);
	  delete result;
	  return;
	}
	s=ccd->bases.next();
      }
    }
    else 
    {
      //printf("Class not found!\n");
      OutputDocInterface *result = ol.clone();
      //printf("cvd->type=`%s'\n",cvd->type.data());
      if (getLink(cvd->type,memName,*result))
      {
        ol.append(result);
      }
      else
      {
        codifyLines(memName);
      }
      delete result;
      return;
    }
  }
  else
  {
    ClassDef *vcd = getResolvedClass(g_currentDefinition,g_classScope);
    if (vcd && vcd->isLinkable())
    {
      //printf("Found class %s for variable `%s'\n",g_classScope.data(),varName);
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
	      if ((vmd->isVariable() || vmd->isFunction()) && 
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
	  if ((vmd->isVariable() || vmd->isFunction()) && 
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

static void generateFunctionLink(OutputDocInterface &ol,char *funcName)
{
  OutputDocInterface *result = ol.clone();
  CodeClassDef *ccd=0;
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
  if (!locScope.isEmpty() && (ccd=g_codeClassDict[locScope]))
  {
    //printf("using classScope %s\n",g_classScope.data());
    char *s=ccd->bases.first();
    while (s)
    {
      if (getLink(s,locFunc,*result,funcName)) 
      {
	ol.append(result);
	delete result;
	return;
      }
      s=ccd->bases.next();
    }
  }
  if (getLink(locScope,locFunc,*result,funcName))
  {
    ol.append(result);
  }
  else
  {
    codifyLines(funcName);
  }
  delete result;
  return;
}

/*! counts the number of lines in the input */
static int countLines()
{
  const char *p=g_inputString;
  char c;
  int count=1;
  while ((c=*p++)) if (c=='\n') count++; 
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
KEYWORD ("asm"|"auto"|"class"|"const"|"const_cast"|"delete"|"dynamic_cast"|"enum"|"explicit"|"extern"|"false"|"friend"|"inline"|"mutable"|"namespace"|"new"|"operator"|"private"|"protected"|"public"|"register"|"reinterpret_cast"|"sizeof"|"static"|"static_cast"|"struct"|"template"|"this"|"true"|"typedef"|"typeid"|"typename"|"union"|"using"|"virtual"|"volatile")
FLOWKW  ("break"|"case"|"catch"|"continue"|"default"|"do"|"else"|"for"|"goto"|"if"|"return"|"switch"|"throw"|"try"|"while")
TYPEKW  ("bool"|"char"|"double"|"float"|"int"|"long"|"short"|"signed"|"unsigned"|"void"|"wchar_t")

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
%x      ClassVar
%x      Bases
%x      SkipSharp
%x      ReadInclude

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
					  //g_code->codify(yytext);
  					  if (!g_insideTemplate) 
					    BEGIN( ClassName ); 
					}
<ReadInclude>[^\n\"\>]+/(">"|"\"")  	{
					  //FileInfo *f;
					  bool ambig;
                                          FileDef *fd;
					  if ((fd=findFileDef(Doxygen::inputNameDict,yytext,ambig)) &&
					      fd->isLinkable())
					  {
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
  					  g_scopeStack.push(INNERBLOCK);
  					  if (g_searchingForBody)
					  {
					    g_searchingForBody=FALSE;
					    g_insideBody=TRUE;
					  }
  					  g_code->codify(yytext);
  					  g_curlyCount++;
					  if (g_insideBody) g_bodyCurlyCount++;
  					  g_type.resize(0); 
					  g_name.resize(0);
					}
<Body>"}"				{ 
  					  if (g_scopeStack.pop()==SCOPEBLOCK) popScope();
  					  g_code->codify(yytext);
  					  g_inClass=FALSE; 
					  if (--g_curlyCount<=0) 
					  {
					    //g_classScope.resize(0);
					    g_codeParmList.clear();
					  }
					  if (--g_bodyCurlyCount<=0)
					  {
					    g_insideBody=FALSE;
					  }
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
<ClassName>{ID}			        {
					  g_ccd.name=yytext;
					  addType();
					  generateClassLink(*g_code,yytext);
					  BEGIN( ClassVar );
					}
<ClassVar>{ID}				{
  					  g_type = g_ccd.name.copy();
					  g_name = yytext;
  					  addVariable();
					  g_code->codify(yytext);
  					}
<ClassName,ClassVar>[ \t\n]*":"[ \t\n]*		{
  					  codifyLines(yytext);
  					  BEGIN( Bases ); 
					}
<Bases,ClassName,ClassVar>[ \t]*"{"[ \t]*	{
  					  g_code->codify(yytext);
					  g_curlyCount++;
					  g_inClass=TRUE;
  					  if (g_searchingForBody)
					  {
					    g_searchingForBody=FALSE;
					    g_insideBody=TRUE;
					  }
					  if (g_insideBody) g_bodyCurlyCount++;
					  if (!g_ccd.name.isEmpty())
					  {
  					    g_scopeStack.push(SCOPEBLOCK);
					    pushScope(g_ccd.name);
					    //g_classScope=g_ccd.name.copy();
					    CodeClassDef *cd=new CodeClassDef(g_ccd);
					    //g_codeClassList.append(cd);
					    g_codeClassDict.insert(cd->name,cd);
					    //printf("g_codeClassList.count()=%d\n",g_codeClassList.count());
					  }
					  else
					  {
  					    g_scopeStack.push(INNERBLOCK);
					  }
					  BEGIN( Body );
 					}
<Bases>"virtual"|"public"|"protected"|"private" { 
  					  startFontClass("keyword");
  					  g_code->codify(yytext);
					  endFontClass();
					}
<Bases>{ID}	                        { 
					  //printf("%s:addBase(%s)\n",g_ccd.name.data(),yytext);
  					  g_ccd.bases.inSort(yytext); 
					  generateClassLink(*g_code,yytext);
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
  					  g_bracketCount=1;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
					}
<Body>{SCOPEPREFIX}?"operator"{B}*[^\(\n]+/"(" {
  					  addType();
					  generateFunctionLink(*g_code,yytext);
  					  g_bracketCount=1;
					  g_args.resize(0);
  					  g_name+=yytext; 
  					  BEGIN( FuncCall );
					}
<Body>"template"/([^a-zA-Z0-9])		{
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
  					}
<Body>{TYPEKW}/{B}* {
  					  startFontClass("keywordtype");
					  g_code->codify(yytext);
					  endFontClass();
					  addType();
  					  g_name+=yytext; 
  					}
<Body>{SCOPENAME}{B}*"<"[^\n\/\-\.\{\"\>]*">"/{B}* { // A<T> *pt;
					  generateClassLink(*g_code,yytext);
					  addType();
  					  g_name+=yytext; 
					}
<Body>{SCOPENAME}/{B}* 			{ // p->func()
					  generateClassLink(*g_code,yytext);
					  addType();
  					  g_name+=yytext; 
					}
<Body>"("{B}*("*"{B}*)*{SCOPENAME}*{B}*")"/{B}*	{  // (*p)->func()
					  g_code->codify(yytext);
					  int s=0;while (!isId(yytext[s])) s++;
                                          int e=yyleng-1;while (!isId(yytext[e])) e--;
					  QCString varname = ((QCString)yytext).mid(s,e-s+1); 
					  addType();
  					  g_name=varname; 
					}
<Body>{SCOPETNAME}/{B}*"("		{ // a() or c::a() or t<A,B>::a()
  					  addType();
					  generateFunctionLink(*g_code,yytext);
  					  g_bracketCount=1;
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
                                          //printf("g_name=`%s' g_classVar=`%s'\n",g_name.data(),g_classVar?g_classVar->name().data():"<none>");
  					  if (!g_name.isEmpty())
					  {
					    generateMemberLink(*g_code,g_name,yytext);
					    g_name=yytext;
					  }
					  else if (g_classVar)
					  {
					    if (!generateClassMemberLink(*g_code,g_classVar,yytext))
					    {
					      g_code->codify(yytext);
					    }
					    g_classVar=0;
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
                                          //printf("g_name=`%s' g_classVar=`%s'\n",g_name.data(),g_classVar?g_classVar->name().data():"<none>");
  					  if (!g_name.isEmpty())
					  {
					    generateMemberLink(*g_code,g_name,yytext);
					    g_name=yytext;
					  }
					  else if (g_classVar)
					  {
					    if (!generateClassMemberLink(*g_code,g_classVar,yytext))
					    {
					      g_code->codify(yytext);
					    }
					    g_classVar=0;
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
					  g_saveType = g_name.copy();
  					  if (!g_type.isEmpty()) 
					  {
					    addVariable();
					    g_name.resize(0);
					  }
					  if (*yytext!=',') g_type.resize(0);
					  if (*yytext==';') g_name.resize(0);
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
<MemberCall2,FuncCall>[a-z_A-Z][:a-z_A-Z0-9]*({B}*"<"[^\n\>]*">")? {
					  addParmType();
					  g_parmName=yytext; 
					  generateClassLink(*g_code,yytext);
					}
<MemberCall2,FuncCall>,			{
  					  g_code->codify(yytext);
					  addParameter();
					  g_parmType.resize(0);g_parmName.resize(0);
					}
<MemberCall2,FuncCall>"("		{
  					  g_code->codify(yytext);
  					  g_bracketCount++; 
					}
<MemberCall2,FuncCall>")"		{ 
  					  g_code->codify(yytext);
  					  if (--g_bracketCount<=0) 
					  {
					    g_name.resize(0);g_args.resize(0);
					    g_parmType.resize(0);g_parmName.resize(0);
					    BEGIN( Body ); 
					  }
					}
<MemberCall2,FuncCall>")"[ \t\n]*[;:]	{
  					  codifyLines(yytext);
  					  g_bracketCount=0;
					  if (yytext[yyleng-1]==';') g_searchingForBody=FALSE; 
					  if (!g_inClass && !g_type.isEmpty())
					    addVariable();
					  g_parmType.resize(0);g_parmName.resize(0);
  					  if (yytext[yyleng-1]==';' || g_insideBody)
					  {
					    g_name.resize(0);g_type.resize(0);
					    BEGIN( Body );
					  }
					  else
					  {
					    g_bracketCount=0;
					    BEGIN( SkipInits );
					  }
  					}
<MemberCall2,FuncCall>")"({BN}"const"|"volatile")*{BN}*"{" 	{
					  addParameter();
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
					  g_code->codify(")");
					  startFontClass("keyword");
					  yytext[yyleng-1]='\0';
  					  codifyLines(yytext+1);
					  endFontClass();
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
					  generateClassLink(*g_code,yytext);
  					}
<FuncCall>([a-z_A-Z][a-z_A-Z0-9]*)/"("	{
					  generateFunctionLink(*g_code,yytext);
					}
<FuncCall>([a-z_A-Z][a-z_A-Z0-9]*)/("."|"->") { 
  					  g_code->codify(yytext);
					  g_name=yytext; 
					  BEGIN( MemberCall2 ); 
					}
<FuncCall,MemberCall2>("("{B}*("*"{B}*)*[a-z_A-Z][a-z_A-Z0-9]*{B}*")"{B}*)/("."|"->") { 
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
  					    g_code->codify(yytext);
					  g_args.resize(0);
					  BEGIN( FuncCall );
					}
<MemberCall2>([a-z_A-Z][a-z_A-Z0-9]*)/([ \t\n]*("."|"->")) {
  					  g_code->codify(yytext);
					  g_name=yytext; 
					  BEGIN( MemberCall2 ); 
    					}
<MemberCall2>"->"|"."			{
  					  g_code->codify(yytext);
					  g_memCallContext = YY_START;
  					  BEGIN( MemberCall ); 
  					}
<SkipComment>"//"			{
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
  g_codeClassDict.setAutoDelete(TRUE);
  g_codeVarList.setAutoDelete(TRUE);
  g_codeParmList.setAutoDelete(TRUE);
  g_codeClassDict.clear();
  g_codeVarList.clear();
  g_codeParmList.clear();
  g_ccd.bases.clear();
  g_anchorCount = 0;
}

void parseCode(OutputDocInterface &od,const char *className,const QCString &s, 
                  bool exBlock, const char *exName,FileDef *fd,
		  int startLine,int endLine,bool inlineFragment)
{
  if (s.isEmpty()) return;
  g_code = od.clone();
  g_inputString   = s;
  g_inputPosition = 0;
  g_currentFontClass = 0;
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
  g_classVar      = 0;
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
  endFontClass();
  if (g_inputLines==1)
  {
    g_code->endCodeLine();
  }
  od.append(g_code);
  delete g_code;
  return;
}

extern "C" { // some bogus code to keep the compiler happy
//  int  codeYYwrap() { return 1 ; }
  void codeYYdummy() { yy_flex_realloc(0,0); } 
}
