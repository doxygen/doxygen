/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#include <stdio.h>
#include <stdlib.h>
#include <cassert>

#include <ctype.h>

#include "qcstring.h"
#include "regex.h"
#include "doxygen.h"
#include "debug.h"
#include "util.h"
#include "pagedef.h"
#include "docparser.h"
#include "doctokenizer.h"
#include "cmdmapper.h"
#include "printdocvisitor.h"
#include "message.h"
#include "section.h"
#include "searchindex.h"
#include "language.h"
#include "portable.h"
#include "cite.h"
#include "arguments.h"
#include "vhdldocgen.h"
#include "groupdef.h"
#include "classlist.h"
#include "filedef.h"
#include "memberdef.h"
#include "namespacedef.h"
#include "reflist.h"
#include "formula.h"
#include "config.h"
#include "growbuf.h"
#include "markdown.h"
#include "htmlentity.h"
#include "emoji.h"
#include "fileinfo.h"
#include "dir.h"

#define TK_COMMAND_CHAR(token) ((token)==TK_COMMAND_AT ? "@" : "\\")

// debug off
#define DBG(x) do {} while(0)

// debug to stdout
//#define DBG(x) printf x

// debug to stderr
//#define myprintf(x...) fprintf(stderr,x)
//#define DBG(x) myprintf x

#define INTERNAL_ASSERT(x) do {} while(0)
//#define INTERNAL_ASSERT(x) if (!(x)) DBG(("INTERNAL_ASSERT(%s) failed retval=0x%x: file=%s line=%d\n",#x,retval,__FILE__,__LINE__));

//---------------------------------------------------------------------------

static const char *g_sectionLevelToName[] =
{
  "page",
  "section",
  "subsection",
  "subsubsection",
  "paragraph",
  "subparagraph"
};

static std::set<QCString> g_plantumlEngine {
  "uml", "bpm", "wire", "dot", "ditaa",
  "salt", "math", "latex", "gantt", "mindmap",
  "wbs", "yaml", "creole", "json", "flow",
  "board", "git"
};

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

using DefinitionStack = std::vector<const Definition *>;
using DocNodeStack = std::stack<const DocNode *>;
using DocStyleChangeStack = std::stack<const DocStyleChange *>;

// Parser state: global variables during a call to validatingParseDoc
static const Definition *     g_scope;
static QCString               g_context;
static bool                   g_inSeeBlock;
static bool                   g_xmlComment;
static bool                   g_insideHtmlLink;
static DocNodeStack           g_nodeStack;
static DocStyleChangeStack    g_styleStack;
static DocStyleChangeStack    g_initialStyleStack;
static DefinitionStack        g_copyStack;
static QCString               g_fileName;
static QCString               g_relPath;

static bool                   g_hasParamCommand;
static bool                   g_hasReturnCommand;
static StringMultiSet         g_retvalsFound;
static StringMultiSet         g_paramsFound;
static const MemberDef *      g_memberDef;
static bool                   g_isExample;
static QCString               g_exampleName;
static QCString               g_searchUrl;

static QCString               g_includeFileName;
static QCString               g_includeFileText;
static uint                   g_includeFileOffset;
static uint                   g_includeFileLength;
static int                    g_includeFileLine;
static bool                   g_includeFileShowLineNo;
static bool                   g_markdownSupport;


/** Parser's context to store all global variables.
 */
struct DocParserContext
{
  const Definition *scope;
  QCString context;
  bool inSeeBlock;
  bool xmlComment;
  bool insideHtmlLink;
  DocNodeStack nodeStack;
  DocStyleChangeStack styleStack;
  DocStyleChangeStack initialStyleStack;
  DefinitionStack copyStack;
  QCString fileName;
  int lineNo;
  QCString relPath;

  bool         hasParamCommand;
  bool         hasReturnCommand;
  const MemberDef *  memberDef;
  StringMultiSet retvalsFound;
  StringMultiSet paramsFound;
  bool         isExample;
  QCString     exampleName;
  QCString     searchUrl;

  QCString  includeFileText;
  uint     includeFileOffset;
  uint     includeFileLength;
  int      includeFileLine;
  bool     includeFileLineNo;

  TokenInfo *token;
};

static std::stack< std::unique_ptr<DocParserContext> > g_parserStack;

//---------------------------------------------------------------------------

class AutoNodeStack
{
  public:
    AutoNodeStack(const DocNode* node) : m_node(node) { g_nodeStack.push(node); }
   ~AutoNodeStack() {
#if defined(NDEBUG)
     (void)m_node;
     if (!g_nodeStack.empty()) g_nodeStack.pop(); // robust version that does not assert
#else
     assert(g_nodeStack.top()==m_node); g_nodeStack.pop(); // error checking version
#endif
   }

  private:
   const DocNode *m_node;
};

//---------------------------------------------------------------------------
static void handleImg(DocNode *parent,DocNodeList &children,const HtmlAttribList &tagHtmlAttribs);

//---------------------------------------------------------------------------
static void docParserPushContext(bool saveParamInfo=TRUE)
{
  //QCString indent;
  //indent.fill(' ',g_parserStack.count()*2+2);
  //printf("%sdocParserPushContext() count=%d\n",qPrint(indent),g_nodeStack.count());

  doctokenizerYYpushContext();

  g_parserStack.push(std::make_unique<DocParserContext>());
  auto &ctx = g_parserStack.top();
  ctx->scope              = g_scope;
  ctx->context            = g_context;
  ctx->inSeeBlock         = g_inSeeBlock;
  ctx->xmlComment         = g_xmlComment;
  ctx->insideHtmlLink     = g_insideHtmlLink;
  ctx->nodeStack          = g_nodeStack;
  ctx->styleStack         = g_styleStack;
  ctx->initialStyleStack  = g_initialStyleStack;
  ctx->copyStack          = g_copyStack;
  ctx->fileName           = g_fileName;
  ctx->lineNo             = getDoctokinizerLineNr();
  ctx->relPath            = g_relPath;

  if (saveParamInfo)
  {
    ctx->hasParamCommand    = g_hasParamCommand;
    ctx->hasReturnCommand   = g_hasReturnCommand;
    ctx->paramsFound        = g_paramsFound;
    ctx->retvalsFound       = g_retvalsFound;
  }

  ctx->memberDef          = g_memberDef;
  ctx->isExample          = g_isExample;
  ctx->exampleName        = g_exampleName;
  ctx->searchUrl          = g_searchUrl;

  ctx->includeFileText    = g_includeFileText;
  ctx->includeFileOffset  = g_includeFileOffset;
  ctx->includeFileLength  = g_includeFileLength;
  ctx->includeFileLine    = g_includeFileLine;
  ctx->includeFileLineNo  = g_includeFileShowLineNo;

  ctx->token              = g_token;
  g_token = new TokenInfo;
}

static void docParserPopContext(bool keepParamInfo=FALSE)
{
  auto &ctx = g_parserStack.top();
  g_scope               = ctx->scope;
  g_context             = ctx->context;
  g_inSeeBlock          = ctx->inSeeBlock;
  g_xmlComment          = ctx->xmlComment;
  g_insideHtmlLink      = ctx->insideHtmlLink;
  g_nodeStack           = ctx->nodeStack;
  g_styleStack          = ctx->styleStack;
  g_initialStyleStack   = ctx->initialStyleStack;
  g_copyStack           = ctx->copyStack;
  g_fileName            = ctx->fileName;
  setDoctokinizerLineNr(ctx->lineNo);
  g_relPath             = ctx->relPath;

  if (!keepParamInfo)
  {
    g_hasParamCommand     = ctx->hasParamCommand;
    g_hasReturnCommand    = ctx->hasReturnCommand;
    g_retvalsFound        = ctx->retvalsFound;
    g_paramsFound         = ctx->paramsFound;
  }
  g_memberDef           = ctx->memberDef;
  g_isExample           = ctx->isExample;
  g_exampleName         = ctx->exampleName;
  g_searchUrl           = ctx->searchUrl;

  g_includeFileText     = ctx->includeFileText;
  g_includeFileOffset   = ctx->includeFileOffset;
  g_includeFileLength   = ctx->includeFileLength;
  g_includeFileLine     = ctx->includeFileLine;
  g_includeFileShowLineNo   = ctx->includeFileLineNo;

  delete g_token;
  g_token               = ctx->token;

  g_parserStack.pop();
  doctokenizerYYpopContext();

  //QCString indent;
  //indent.fill(' ',g_parserStack.count()*2+2);
  //printf("%sdocParserPopContext() count=%d\n",qPrint(indent),g_nodeStack.count());
}

//---------------------------------------------------------------------------

// replaces { with < and } with > and also
// replaces &gt; with < and &gt; with > within string s
static void unescapeCRef(QCString &s)
{
  QCString result;
  const char *p = s.data();
  if (p)
  {
    char c;
    while ((c=*p++))
    {
      if (c=='{') c='<'; else if (c=='}') c='>';
      result+=c;
    }
  }

  result=substitute(result,"&lt;","<");
  result=substitute(result,"&gt;",">");
  s = result;
}

//---------------------------------------------------------------------------

/*! search for an image in the imageNameDict and if found
 * copies the image to the output directory (which depends on the \a type
 * parameter).
 */
static QCString findAndCopyImage(const QCString &fileName,DocImage::Type type, bool dowarn = true)
{
  QCString result;
  bool ambig;
  FileDef *fd = findFileDef(Doxygen::imageNameLinkedMap,fileName,ambig);
  //printf("Search for %s\n",fileName);
  if (fd)
  {
    if (ambig & dowarn)
    {
      QCString text;
      text.sprintf("image file name %s is ambiguous.\n",qPrint(fileName));
      text+="Possible candidates:\n";
      text+=showFileDefMatches(Doxygen::imageNameLinkedMap,fileName);
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"%s", qPrint(text));
    }

    QCString inputFile = fd->absFilePath();
    FileInfo infi(inputFile.str());
    if (infi.exists())
    {
      result = fileName;
      int i;
      if ((i=result.findRev('/'))!=-1 || (i=result.findRev('\\'))!=-1)
      {
	result = result.right((int)result.length()-i-1);
      }
      //printf("fileName=%s result=%s\n",fileName,qPrint(result));
      QCString outputDir;
      switch(type)
      {
        case DocImage::Html:
	  if (!Config_getBool(GENERATE_HTML)) return result;
	  outputDir = Config_getString(HTML_OUTPUT);
	  break;
        case DocImage::Latex:
	  if (!Config_getBool(GENERATE_LATEX)) return result;
	  outputDir = Config_getString(LATEX_OUTPUT);
	  break;
        case DocImage::DocBook:
	  if (!Config_getBool(GENERATE_DOCBOOK)) return result;
	  outputDir = Config_getString(DOCBOOK_OUTPUT);
	  break;
        case DocImage::Rtf:
	  if (!Config_getBool(GENERATE_RTF)) return result;
	  outputDir = Config_getString(RTF_OUTPUT);
	  break;
      }
      QCString outputFile = outputDir+"/"+result;
      FileInfo outfi(outputFile.str());
      if (outfi.isSymLink())
      {
        Dir().remove(outputFile.str());
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),
            "destination of image %s is a symlink, replacing with image",
            qPrint(outputFile));
      }
      if (outputFile!=inputFile) // prevent copying to ourself
      {
        if (copyFile(inputFile,outputFile) && type==DocImage::Html)
        {
          Doxygen::indexList->addImageFile(result);
        }
      }
    }
    else
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),
	  "could not open image %s",qPrint(fileName));
    }

    if (type==DocImage::Latex && Config_getBool(USE_PDFLATEX) &&
	fd->name().right(4)==".eps"
       )
    { // we have an .eps image in pdflatex mode => convert it to a pdf.
      QCString outputDir = Config_getString(LATEX_OUTPUT);
      QCString baseName  = fd->name().left(fd->name().length()-4);
      QCString epstopdfArgs(4096);
      epstopdfArgs.sprintf("\"%s/%s.eps\" --outfile=\"%s/%s.pdf\"",
                           qPrint(outputDir), qPrint(baseName),
			   qPrint(outputDir), qPrint(baseName));
      Portable::sysTimerStart();
      if (Portable::system("epstopdf",epstopdfArgs)!=0)
      {
	err("Problems running epstopdf. Check your TeX installation!\n");
      }
      Portable::sysTimerStop();
      return baseName;
    }
  }
  else
  {
    result=fileName;
    if (result.left(5)!="http:" && result.left(6)!="https:" && dowarn)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),
           "image file %s is not found in IMAGE_PATH: "
	   "assuming external image.",qPrint(fileName)
          );
    }
  }
  return result;
}

/*! Collects the parameters found with \@param command
 *  in a global list g_paramsFound. If
 *  the parameter is not an actual parameter of the current
 *  member g_memberDef, then a warning is raised (unless warnings
 *  are disabled altogether).
 */
static void checkArgumentName(const std::string &name)
{
  if (!Config_getBool(WARN_IF_DOC_ERROR)) return;
  if (g_memberDef==0) return; // not a member
  const ArgumentList &al=g_memberDef->isDocsForDefinition() ?
	                 g_memberDef->argumentList() :
                         g_memberDef->declArgumentList();
  SrcLangExt lang = g_memberDef->getLanguage();
  //printf("isDocsForDefinition()=%d\n",g_memberDef->isDocsForDefinition());
  if (al.empty()) return; // no argument list

  static const reg::Ex re(R"(\$?\w+\.*)");
  reg::Iterator it(name,re);
  reg::Iterator end;
  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    QCString aName=match.str();
    if (lang==SrcLangExt_Fortran) aName=aName.lower();
    //printf("aName='%s'\n",qPrint(aName));
    bool found=FALSE;
    for (const Argument &a : al)
    {
      QCString argName = g_memberDef->isDefine() ? a.type : a.name;
      if (lang==SrcLangExt_Fortran) argName=argName.lower();
      argName=argName.stripWhiteSpace();
      //printf("argName='%s' aName=%s\n",qPrint(argName),qPrint(aName));
      if (argName.right(3)=="...") argName=argName.left(argName.length()-3);
      if (aName==argName)
      {
	g_paramsFound.insert(aName.str());
	found=TRUE;
	break;
      }
    }
    if (!found)
    {
      //printf("member type=%d\n",g_memberDef->memberType());
      QCString scope=g_memberDef->getScopeString();
      if (!scope.isEmpty()) scope+="::"; else scope="";
      QCString inheritedFrom = "";
      QCString docFile = g_memberDef->docFile();
      int docLine = g_memberDef->docLine();
      const MemberDef *inheritedMd = g_memberDef->inheritsDocsFrom();
      if (inheritedMd) // documentation was inherited
      {
        inheritedFrom.sprintf(" inherited from member %s at line "
            "%d in file %s",qPrint(inheritedMd->name()),
            inheritedMd->docLine(),qPrint(inheritedMd->docFile()));
        docFile = g_memberDef->getDefFileName();
        docLine = g_memberDef->getDefLine();
      }
      QCString alStr = argListToString(al);
      warn_doc_error(docFile,docLine,
	  "argument '%s' of command @param "
	  "is not found in the argument list of %s%s%s%s",
	  qPrint(aName), qPrint(scope), qPrint(g_memberDef->name()),
	  qPrint(alStr), qPrint(inheritedFrom));
    }
  }
}
/*! Collects the return values found with \@retval command
 *  in a global list g_retvalsFound.
 */
static void checkRetvalName(const QCString &name)
{
  if (!Config_getBool(WARN_IF_DOC_ERROR)) return;
  if (g_memberDef==0 || name.isEmpty()) return; // not a member or no valid name
  if (g_retvalsFound.count(name.str())==1) // only report the first double entry
  {
     warn_doc_error(g_memberDef->getDefFileName(),
                    g_memberDef->getDefLine(),
                    "%s",
                    qPrint("return value '" + name + "' of " +
                    QCString(g_memberDef->qualifiedName()) +
                    " has multiple documentation sections"));
  }
  g_retvalsFound.insert(name.str());
}

/*! Checks if the parameters that have been specified using \@param are
 *  indeed all parameters and that a parameter does not have multiple
 *  \@param blocks.
 *  Must be called after checkArgumentName() has been called for each
 *  argument.
 */
static void checkUnOrMultipleDocumentedParams()
{
  if (g_memberDef && g_hasParamCommand)
  {
    const ArgumentList &al=g_memberDef->isDocsForDefinition() ?
      g_memberDef->argumentList() :
      g_memberDef->declArgumentList();
    SrcLangExt lang = g_memberDef->getLanguage();
    if (!al.empty())
    {
      ArgumentList undocParams;
      for (const Argument &a: al)
      {
        QCString argName = g_memberDef->isDefine() ? a.type : a.name;
        if (lang==SrcLangExt_Fortran) argName = argName.lower();
        argName=argName.stripWhiteSpace();
        QCString aName = argName;
        if (argName.right(3)=="...") argName=argName.left(argName.length()-3);
        if (lang==SrcLangExt_Python && (argName=="self" || argName=="cls"))
        {
          // allow undocumented self / cls parameter for Python
        }
        else if (!argName.isEmpty())
        {
          size_t count = g_paramsFound.count(argName.str());
          if (count==0 && a.docs.isEmpty())
          {
            undocParams.push_back(a);
          }
          else if (count>1 && Config_getBool(WARN_IF_DOC_ERROR))
          {
            warn_doc_error(g_memberDef->getDefFileName(),
                           g_memberDef->getDefLine(),
                           "%s",
                           qPrint("argument '" + aName +
                           "' from the argument list of " +
                           QCString(g_memberDef->qualifiedName()) +
                           " has multiple @param documentation sections"));
          }
        }
      }
      if (!undocParams.empty() && Config_getBool(WARN_IF_INCOMPLETE_DOC))
      {
        bool first=TRUE;
        QCString errMsg = "The following parameter";
        if (undocParams.size()>1) errMsg+="s";
        errMsg+=" of "+
            QCString(g_memberDef->qualifiedName()) +
            QCString(argListToString(al)) +
            (undocParams.size()>1 ? " are" : " is") + " not documented:\n";
        for (const Argument &a : undocParams)
        {
          QCString argName = g_memberDef->isDefine() ? a.type : a.name;
          if (lang==SrcLangExt_Fortran) argName = argName.lower();
          argName=argName.stripWhiteSpace();
          if (!first) errMsg+="\n";
          first=FALSE;
          errMsg+="  parameter '"+argName+"'";
        }
        warn_incomplete_doc(g_memberDef->getDefFileName(),
                            g_memberDef->getDefLine(),
                            "%s",
                            qPrint(substitute(errMsg,"%","%%")));
      }
    }
  }
}

//---------------------------------------------------------------------------

/*! Strips known html and tex extensions from \a text. */
static QCString stripKnownExtensions(const QCString &text)
{
  QCString result=text;
  if (result.right(4)==".tex")
  {
    result=result.left(result.length()-4);
  }
  else if (result.right(Doxygen::htmlFileExtension.length())==
         QCString(Doxygen::htmlFileExtension))
  {
    result=result.left(result.length()-Doxygen::htmlFileExtension.length());
  }
  return result;
}


//---------------------------------------------------------------------------

/*! Returns TRUE iff node n is a child of a preformatted node */
static bool insidePRE(DocNode *n)
{
  while (n)
  {
    if (n->isPreformatted()) return TRUE;
    n=n->parent();
  }
  return FALSE;
}

//---------------------------------------------------------------------------

/*! Returns TRUE iff node n is a child of a html list item node */
static bool insideLI(DocNode *n)
{
  while (n)
  {
    if (n->kind()==DocNode::Kind_HtmlListItem) return TRUE;
    n=n->parent();
  }
  return FALSE;
}

//---------------------------------------------------------------------------

/*! Returns TRUE iff node n is a child of a unordered html list node */
static bool insideUL(DocNode *n)
{
  while (n)
  {
    if (n->kind()==DocNode::Kind_HtmlList &&
        ((DocHtmlList *)n)->type()==DocHtmlList::Unordered) return TRUE;
    n=n->parent();
  }
  return FALSE;
}

//---------------------------------------------------------------------------

/*! Returns TRUE iff node n is a child of a ordered html list node */
static bool insideOL(DocNode *n)
{
  while (n)
  {
    if (n->kind()==DocNode::Kind_HtmlList &&
        ((DocHtmlList *)n)->type()==DocHtmlList::Ordered) return TRUE;
    n=n->parent();
  }
  return FALSE;
}

//---------------------------------------------------------------------------

static bool insideTable(DocNode *n)
{
  while (n)
  {
    if (n->kind()==DocNode::Kind_HtmlTable) return TRUE;
    n=n->parent();
  }
  return FALSE;
}

//---------------------------------------------------------------------------
/*! Looks for a documentation block with name commandName in the current
 *  context (g_context). The resulting documentation string is
 *  put in pDoc, the definition in which the documentation was found is
 *  put in pDef.
 *  @retval TRUE if name was found.
 *  @retval FALSE if name was not found.
 */
static bool findDocsForMemberOrCompound(const QCString &commandName,
                                        QCString *pDoc,
                                        QCString *pBrief,
                                        const Definition **pDef)
{
  //printf("findDocsForMemberOrCompound(%s)\n",commandName);
  *pDoc="";
  *pBrief="";
  *pDef=0;
  QCString cmdArg=commandName;
  if (cmdArg.isEmpty()) return FALSE;

  const FileDef      *fd=0;
  const GroupDef     *gd=0;
  const PageDef      *pd=0;
  gd = Doxygen::groupLinkedMap->find(cmdArg);
  if (gd) // group
  {
    *pDoc=gd->documentation();
    *pBrief=gd->briefDescription();
    *pDef=gd;
    return TRUE;
  }
  pd = Doxygen::pageLinkedMap->find(cmdArg);
  if (pd) // page
  {
    *pDoc=pd->documentation();
    *pBrief=pd->briefDescription();
    *pDef=pd;
    return TRUE;
  }
  bool ambig;
  fd = findFileDef(Doxygen::inputNameLinkedMap,cmdArg,ambig);
  if (fd && !ambig) // file
  {
    *pDoc=fd->documentation();
    *pBrief=fd->briefDescription();
    *pDef=fd;
    return TRUE;
  }

  // for symbols we need to normalize the separator, so A#B, or A\B, or A.B becomes A::B
  cmdArg = substitute(cmdArg,"#","::");
  cmdArg = substitute(cmdArg,"\\","::");
  static bool extractAnonNs = Config_getBool(EXTRACT_ANON_NSPACES);
  if (extractAnonNs &&
      cmdArg.startsWith("anonymous_namespace{")
      )
  {
    int rightBracePos = cmdArg.find("}", std::strlen("anonymous_namespace{"));
    QCString leftPart = cmdArg.left(rightBracePos + 1);
    QCString rightPart = cmdArg.right(cmdArg.size() - rightBracePos - 1);
    rightPart = substitute(rightPart, ".", "::");
    cmdArg = leftPart + rightPart;
  }
  else
  {
    cmdArg = substitute(cmdArg,".","::");
  }

  int l=(int)cmdArg.length();

  int funcStart=cmdArg.find('(');
  if (funcStart==-1)
  {
    funcStart=l;
  }
  else
  {
    // Check for the case of operator() and the like.
    // beware of scenarios like operator()((foo)bar)
    int secondParen = cmdArg.find('(', funcStart+1);
    int leftParen   = cmdArg.find(')', funcStart+1);
    if (leftParen!=-1 && secondParen!=-1)
    {
      if (leftParen<secondParen)
      {
        funcStart=secondParen;
      }
    }
  }

  QCString name=removeRedundantWhiteSpace(cmdArg.left(funcStart));
  QCString args=cmdArg.right(l-funcStart);
  // try if the link is to a member
  const MemberDef    *md=0;
  const ClassDef     *cd=0;
  const NamespaceDef *nd=0;
  bool found = getDefs(
      g_context.find('.')==-1?g_context:QCString(), // find('.') is a hack to detect files
      name,
      args.isEmpty() ? QCString() : args,
      md,cd,fd,nd,gd,FALSE,0,TRUE);
  //printf("found=%d context=%s name=%s\n",found,qPrint(g_context),qPrint(name));
  if (found && md)
  {
    *pDoc=md->documentation();
    *pBrief=md->briefDescription();
    *pDef=md;
    return TRUE;
  }


  int scopeOffset=(int)g_context.length();
  do // for each scope
  {
    QCString fullName=cmdArg;
    if (scopeOffset>0)
    {
      fullName.prepend(g_context.left(scopeOffset)+"::");
    }
    //printf("Trying fullName='%s'\n",qPrint(fullName));

    // try class, namespace, group, page, file reference
    cd = Doxygen::classLinkedMap->find(fullName);
    if (cd) // class
    {
      *pDoc=cd->documentation();
      *pBrief=cd->briefDescription();
      *pDef=cd;
      return TRUE;
    }
    nd = Doxygen::namespaceLinkedMap->find(fullName);
    if (nd) // namespace
    {
      *pDoc=nd->documentation();
      *pBrief=nd->briefDescription();
      *pDef=nd;
      return TRUE;
    }
    if (scopeOffset==0)
    {
      scopeOffset=-1;
    }
    else
    {
      scopeOffset = g_context.findRev("::",scopeOffset-1);
      if (scopeOffset==-1) scopeOffset=0;
    }
  } while (scopeOffset>=0);


  return FALSE;
}

//---------------------------------------------------------------------------
inline void errorHandleDefaultToken(DocNode *parent,int tok,
                               DocNodeList &children,const QCString &txt)
{
  const char *cmd_start = "\\";
  switch (tok)
  {
    case TK_COMMAND_AT:
      cmd_start = "@";
      // fall through
    case TK_COMMAND_BS:
      children.push_back(std::make_unique<DocWord>(parent,TK_COMMAND_CHAR(tok) + g_token->name));
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Illegal command %s as part of a %s",
       qPrint(cmd_start + g_token->name),qPrint(txt));
      break;
    case TK_SYMBOL:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unsupported symbol %s found found as part of a %s",
           qPrint(g_token->name), qPrint(txt));
      break;
    default:
      children.push_back(std::make_unique<DocWord>(parent,g_token->name));
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected token %s found as part of a %s",
        tokToString(tok), qPrint(txt));
      break;
  }
}

//---------------------------------------------------------------------------
// forward declaration
static bool defaultHandleToken(DocNode *parent,int tok,
                               DocNodeList &children,bool
                               handleWord=TRUE);

static int handleStyleArgument(DocNode *parent,DocNodeList &children,
                               const QCString &cmdName)
{
  DBG(("handleStyleArgument(%s)\n",qPrint(cmdName)));
  QCString saveCmdName = cmdName;
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
	qPrint(saveCmdName));
    return tok;
  }
  while ((tok=doctokenizerYYlex()) &&
          tok!=TK_WHITESPACE &&
          tok!=TK_NEWPARA &&
          tok!=TK_LISTITEM &&
          tok!=TK_ENDLIST
        )
  {
    static const reg::Ex specialChar(R"([.,|()\[\]:;?])");
    if (tok==TK_WORD && g_token->name.length()==1 &&
        reg::match(g_token->name.str(),specialChar))
    {
      // special character that ends the markup command
      return tok;
    }
    if (!defaultHandleToken(parent,tok,children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          if (insideLI(parent) && Mappers::htmlTagMapper->map(g_token->name) && g_token->endTag)
          { // ignore </li> as the end of a style command
            continue;
          }
          return tok;
          break;
        default:
	  errorHandleDefaultToken(parent,tok,children,"\\" + saveCmdName + " command");
          break;
      }
      break;
    }
  }
  DBG(("handleStyleArgument(%s) end tok=%x\n",qPrint(saveCmdName),tok));
  return (tok==TK_NEWPARA || tok==TK_LISTITEM || tok==TK_ENDLIST
         ) ? tok : RetVal_OK;
}

/*! Called when a style change starts. For instance a \<b\> command is
 *  encountered.
 */
static void handleStyleEnter(DocNode *parent,DocNodeList &children,
          DocStyleChange::Style s,const QCString &tagName,const HtmlAttribList *attribs)
{
  DBG(("HandleStyleEnter\n"));
  DocStyleChange *sc= new DocStyleChange(parent,(uint)g_nodeStack.size(),s,tagName,TRUE,attribs);
  children.push_back(std::unique_ptr<DocStyleChange>(sc));
  g_styleStack.push(sc);
}

/*! Called when a style change ends. For instance a \</b\> command is
 *  encountered.
 */
static void handleStyleLeave(DocNode *parent,DocNodeList &children,
         DocStyleChange::Style s,const QCString &tagName)
{
  DBG(("HandleStyleLeave\n"));
  QCString tagNameLower = QCString(tagName).lower();
  if (g_styleStack.empty() ||                            // no style change
      g_styleStack.top()->style()!=s ||                  // wrong style change
      g_styleStack.top()->tagName()!=tagNameLower ||     // wrong style change
      g_styleStack.top()->position()!=g_nodeStack.size() // wrong position
     )
  {
    if (g_styleStack.empty())
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found </%s> tag without matching <%s>",
          qPrint(tagName),qPrint(tagName));
    }
    else if (g_styleStack.top()->tagName()!=tagNameLower)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found </%s> tag while expecting </%s>",
          qPrint(tagName),qPrint(g_styleStack.top()->tagName()));
    }
    else if (g_styleStack.top()->style()!=s)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found </%s> tag while expecting </%s>",
          qPrint(tagName),qPrint(g_styleStack.top()->tagName()));
    }
    else
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found </%s> at different nesting level (%zu) than expected (%d)",
          qPrint(tagName),g_nodeStack.size(),g_styleStack.top()->position());
    }
  }
  else // end the section
  {
    children.push_back(
        std::make_unique<DocStyleChange>(
          parent,(uint)g_nodeStack.size(),s,g_styleStack.top()->tagName(),FALSE));
    g_styleStack.pop();
  }
}

/*! Called at the end of a paragraph to close all open style changes
 *  (e.g. a <b> without a </b>). The closed styles are pushed onto a stack
 *  and entered again at the start of a new paragraph.
 */
static void handlePendingStyleCommands(DocNode *parent,DocNodeList &children)
{
  if (!g_styleStack.empty())
  {
    const DocStyleChange *sc = g_styleStack.top();
    while (sc && sc->position()>=g_nodeStack.size())
    { // there are unclosed style modifiers in the paragraph
      children.push_back(std::make_unique<DocStyleChange>(parent,(uint)g_nodeStack.size(),sc->style(),sc->tagName(),FALSE));
      g_initialStyleStack.push(sc);
      g_styleStack.pop();
      sc = !g_styleStack.empty() ? g_styleStack.top() : 0;
    }
  }
}

static void handleInitialStyleCommands(DocPara *parent,DocNodeList &children)
{
  while (!g_initialStyleStack.empty())
  {
    const DocStyleChange *sc = g_initialStyleStack.top();
    handleStyleEnter(parent,children,sc->style(),sc->tagName(),&sc->attribs());
    g_initialStyleStack.pop();
  }
}

static int handleAHref(DocNode *parent,DocNodeList &children,const HtmlAttribList &tagHtmlAttribs)
{
  uint index=0;
  int retval = RetVal_OK;
  for (const auto &opt : tagHtmlAttribs)
  {
    if (opt.name=="name" || opt.name=="id") // <a name=label> or <a id=label> tag
    {
      if (!opt.value.isEmpty())
      {
        children.push_back(std::make_unique<DocAnchor>(parent,opt.value,TRUE));
        break; // stop looking for other tag attribs
      }
      else
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found <a> tag with name option but without value!");
      }
    }
    else if (opt.name=="href") // <a href=url>..</a> tag
    {
      // copy attributes
      HtmlAttribList attrList = tagHtmlAttribs;
      // and remove the href attribute
      attrList.erase(attrList.begin()+index);
      QCString relPath;
      if (opt.value.at(0) != '#') relPath = g_relPath;
      DocHRef *href = new DocHRef(parent,attrList,opt.value,relPath,convertNameToFile(g_fileName,FALSE,TRUE));
      children.push_back(std::unique_ptr<DocHRef>(href));
      g_insideHtmlLink=TRUE;
      retval = href->parse();
      g_insideHtmlLink=FALSE;
      break;
    }
    else // unsupported option for tag a
    {
    }
    ++index;
  }
  return retval;
}

const char *DocStyleChange::styleString() const
{
  switch (m_style)
  {
    case DocStyleChange::Bold:         return "b";
    case DocStyleChange::Italic:       return "em";
    case DocStyleChange::Code:         return "code";
    case DocStyleChange::Center:       return "center";
    case DocStyleChange::Small:        return "small";
    case DocStyleChange::Subscript:    return "subscript";
    case DocStyleChange::Superscript:  return "superscript";
    case DocStyleChange::Preformatted: return "pre";
    case DocStyleChange::Div:          return "div";
    case DocStyleChange::Span:         return "span";
    case DocStyleChange::Strike:       return "strike";
    case DocStyleChange::S:            return "s";
    case DocStyleChange::Del:          return "del";
    case DocStyleChange::Underline:    return "u";
    case DocStyleChange::Ins:          return "ins";
  }
  return "<invalid>";
}

static void handleUnclosedStyleCommands()
{
  if (!g_initialStyleStack.empty())
  {
    const DocStyleChange *sc = g_initialStyleStack.top();
    g_initialStyleStack.pop();
    handleUnclosedStyleCommands();
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),
             "end of comment block while expecting "
             "command </%s>",qPrint(sc->tagName()));
  }
}

static void handleLinkedWord(DocNode *parent,DocNodeList &children,bool ignoreAutoLinkFlag=FALSE)
{
  QCString name = linkToText(SrcLangExt_Unknown,g_token->name,TRUE);
  static bool autolinkSupport = Config_getBool(AUTOLINK_SUPPORT);
  if (!autolinkSupport && !ignoreAutoLinkFlag) // no autolinking -> add as normal word
  {
    children.push_back(std::make_unique<DocWord>(parent,name));
    return;
  }

  // ------- try to turn the word 'name' into a link

  const Definition *compound=0;
  const MemberDef  *member=0;
  uint len = g_token->name.length();
  ClassDef *cd=0;
  bool ambig;
  FileDef *fd = findFileDef(Doxygen::inputNameLinkedMap,g_fileName,ambig);
  //printf("handleLinkedWord(%s) g_context=%s\n",qPrint(g_token->name),qPrint(g_context));
  if (!g_insideHtmlLink &&
      (resolveRef(g_context,g_token->name,g_inSeeBlock,&compound,&member,TRUE,fd,TRUE)
       || (!g_context.isEmpty() &&  // also try with global scope
           resolveRef("",g_token->name,g_inSeeBlock,&compound,&member,FALSE,0,TRUE))
      )
     )
  {
    //printf("resolveRef %s = %p (linkable?=%d)\n",qPrint(g_token->name),member,member ? member->isLinkable() : FALSE);
    if (member && member->isLinkable()) // member link
    {
      if (member->isObjCMethod())
      {
        bool localLink = g_memberDef ? member->getClassDef()==g_memberDef->getClassDef() : FALSE;
        name = member->objCMethodName(localLink,g_inSeeBlock);
      }
      children.push_back(
         std::make_unique<DocLinkedWord>(
            parent,name,
            member->getReference(),
            member->getOutputFileBase(),
            member->anchor(),
            member->briefDescriptionAsTooltip()));
    }
    else if (compound->isLinkable()) // compound link
    {
      QCString anchor = compound->anchor();
      if (compound->definitionType()==Definition::TypeFile)
      {
        name=g_token->name;
      }
      else if (compound->definitionType()==Definition::TypeGroup)
      {
        name=toGroupDef(compound)->groupTitle();
      }
      children.push_back(
          std::make_unique<DocLinkedWord>(
            parent,name,
            compound->getReference(),
            compound->getOutputFileBase(),
            anchor,
            compound->briefDescriptionAsTooltip()));
    }
    else if (compound->definitionType()==Definition::TypeFile &&
             (toFileDef(compound))->generateSourceFile()
            ) // undocumented file that has source code we can link to
    {
      children.push_back(
          std::make_unique<DocLinkedWord>(
             parent,g_token->name,
             compound->getReference(),
             compound->getSourceFileBase(),
             "",
             compound->briefDescriptionAsTooltip()));
    }
    else // not linkable
    {
      children.push_back(std::make_unique<DocWord>(parent,name));
    }
  }
  else if (!g_insideHtmlLink && len>1 && g_token->name.at(len-1)==':')
  {
    // special case, where matching Foo: fails to be an Obj-C reference,
    // but Foo itself might be linkable.
    g_token->name=g_token->name.left(len-1);
    handleLinkedWord(parent,children,ignoreAutoLinkFlag);
    children.push_back(std::make_unique<DocWord>(parent,":"));
  }
  else if (!g_insideHtmlLink && (cd=getClass(g_token->name+"-p")))
  {
    // special case 2, where the token name is not a class, but could
    // be a Obj-C protocol
    children.push_back(
        std::make_unique<DocLinkedWord>(
          parent,name,
          cd->getReference(),
          cd->getOutputFileBase(),
          cd->anchor(),
          cd->briefDescriptionAsTooltip()));
  }
  else // normal non-linkable word
  {
    if (g_token->name.left(1)=="#" || g_token->name.left(2)=="::")
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"explicit link request to '%s' could not be resolved",qPrint(name));
      children.push_back(std::make_unique<DocWord>(parent,g_token->name));
    }
    else
    {
      children.push_back(std::make_unique<DocWord>(parent,name));
    }
  }
}

static void handleParameterType(DocNode *parent,DocNodeList &children,const QCString &paramTypes)
{
  QCString name = g_token->name; // save token name
  QCString name1;
  int p=0,i,ii;
  while ((i=paramTypes.find('|',p))!=-1)
  {
    name1 = paramTypes.mid(p,i-p);
    ii=name1.find('[');
    g_token->name=ii!=-1 ? name1.mid(0,ii) : name1; // take part without []
    handleLinkedWord(parent,children);
    if (ii!=-1) children.push_back(std::make_unique<DocWord>(parent,name1.mid(ii))); // add [] part
    p=i+1;
    children.push_back(std::make_unique<DocSeparator>(parent,"|"));
  }
  name1 = paramTypes.mid(p);
  ii=name1.find('[');
  g_token->name=ii!=-1 ? name1.mid(0,ii) : name1;
  handleLinkedWord(parent,children);
  if (ii!=-1) children.push_back(std::make_unique<DocWord>(parent,name1.mid(ii)));
  g_token->name = name; // restore original token name
}

static DocInternalRef *handleInternalRef(DocNode *parent)
{
  //printf("CMD_INTERNALREF\n");
  int tok=doctokenizerYYlex();
  QCString tokenName = g_token->name;
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(tokenName));
    return 0;
  }
  doctokenizerYYsetStateInternalRef();
  tok=doctokenizerYYlex(); // get the reference id
  if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint(tokenName));
    return 0;
  }
  return new DocInternalRef(parent,g_token->name);
}

static DocAnchor *handleAnchor(DocNode *parent)
{
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(g_token->name));
    return 0;
  }
  doctokenizerYYsetStateAnchor();
  tok=doctokenizerYYlex();
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(g_token->name));
    return 0;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint(g_token->name));
    return 0;
  }
  doctokenizerYYsetStatePara();
  return new DocAnchor(parent,g_token->name,FALSE);
}


/* Helper function that deals with the title, width, and height arguments of various commands.
 * @param[in] cmd        Command id for which to extract caption and size info.
 * @param[in] parent     Parent node, owner of the children list passed as
 *                       the third argument.
 * @param[in] children   The list of child nodes to which the node representing
 *                       the token can be added.
 * @param[out] width     the extracted width specifier
 * @param[out] height    the extracted height specifier
 */
static void defaultHandleTitleAndSize(const int cmd, DocNode *parent, DocNodeList &children, QCString &width,QCString &height)
{
  auto ns = AutoNodeStack(parent);

  // parse title
  doctokenizerYYsetStateTitle();
  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (tok==TK_WORD && (g_token->name=="width=" || g_token->name=="height="))
    {
      // special case: no title, but we do have a size indicator
      break;
    }
    if (!defaultHandleToken(parent,tok,children))
    {
      errorHandleDefaultToken(parent,tok,children,Mappers::cmdMapper->find(cmd));
    }
  }
  // parse size attributes
  if (tok == 0)
  {
    tok=doctokenizerYYlex();
  }
  while (tok==TK_WHITESPACE || tok==TK_WORD) // there are values following the title
  {
    if(tok == TK_WORD)
    {
      if (g_token->name=="width=" || g_token->name=="height=")
      {
        doctokenizerYYsetStateTitleAttrValue();
        g_token->name = g_token->name.left(g_token->name.length()-1);
      }

      if (g_token->name=="width")
      {
        width = g_token->chars;
      }
      else if (g_token->name=="height")
      {
        height = g_token->chars;
      }
      else
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unknown option '%s' after \\%s command, expected 'width' or 'height'",
                       qPrint(g_token->name), qPrint(Mappers::cmdMapper->find(cmd)));
        break;
      }
    }

    tok=doctokenizerYYlex();
  }
  doctokenizerYYsetStatePara();

  handlePendingStyleCommands(parent,children);
}

/* Helper function that deals with the most common tokens allowed in
 * title like sections.
 * @param parent     Parent node, owner of the children list passed as
 *                   the third argument.
 * @param tok        The token to process.
 * @param children   The list of child nodes to which the node representing
 *                   the token can be added.
 * @param handleWord Indicates if word token should be processed
 * @retval TRUE      The token was handled.
 * @retval FALSE     The token was not handled.
 */
static bool defaultHandleToken(DocNode *parent,int tok, DocNodeList &children,bool
    handleWord)
{
  DBG(("token %s at %d",tokToString(tok),getDoctokinizerLineNr()));
  if (tok==TK_WORD || tok==TK_LNKWORD || tok==TK_SYMBOL || tok==TK_URL ||
      tok==TK_COMMAND_AT || tok==TK_COMMAND_BS || tok==TK_HTMLTAG
     )
  {
    DBG((" name=%s",qPrint(g_token->name)));
  }
  DBG(("\n"));
reparsetoken:
  QCString tokenName = g_token->name;
  switch (tok)
  {
    case TK_COMMAND_AT:
        // fall through
    case TK_COMMAND_BS:
      switch (Mappers::cmdMapper->map(tokenName))
      {
        case CMD_BSLASH:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_BSlash));
          break;
        case CMD_AT:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_At));
          break;
        case CMD_LESS:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Less));
          break;
        case CMD_GREATER:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Greater));
          break;
        case CMD_AMP:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Amp));
          break;
        case CMD_DOLLAR:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Dollar));
          break;
        case CMD_HASH:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Hash));
          break;
        case CMD_DCOLON:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_DoubleColon));
          break;
        case CMD_PERCENT:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Percent));
          break;
        case CMD_NDASH:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Minus));
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Minus));
          break;
        case CMD_MDASH:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Minus));
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Minus));
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Minus));
          break;
        case CMD_QUOTE:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Quot));
          break;
        case CMD_PUNT:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Dot));
          break;
        case CMD_PLUS:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Plus));
          break;
        case CMD_MINUS:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Minus));
          break;
        case CMD_EQUAL:
          children.push_back(std::make_unique<DocSymbol>(parent,DocSymbol::Sym_Equal));
          break;
        case CMD_EMPHASIS:
          {
            children.push_back(std::make_unique<DocStyleChange>(parent,(uint)g_nodeStack.size(),DocStyleChange::Italic,tokenName,TRUE));
            tok=handleStyleArgument(parent,children,tokenName);
            children.push_back(std::make_unique<DocStyleChange>(parent,(uint)g_nodeStack.size(),DocStyleChange::Italic,tokenName,FALSE));
            if (tok!=TK_WORD) children.push_back(std::make_unique<DocWhiteSpace>(parent," "));
            if (tok==TK_NEWPARA) goto handlepara;
            else if (tok==TK_WORD || tok==TK_HTMLTAG)
            {
	      DBG(("CMD_EMPHASIS: reparsing command %s\n",qPrint(g_token->name)));
              goto reparsetoken;
            }
          }
          break;
        case CMD_BOLD:
          {
            children.push_back(std::make_unique<DocStyleChange>(parent,(uint)g_nodeStack.size(),DocStyleChange::Bold,tokenName,TRUE));
            tok=handleStyleArgument(parent,children,tokenName);
            children.push_back(std::make_unique<DocStyleChange>(parent,(uint)g_nodeStack.size(),DocStyleChange::Bold,tokenName,FALSE));
            if (tok!=TK_WORD) children.push_back(std::make_unique<DocWhiteSpace>(parent," "));
            if (tok==TK_NEWPARA) goto handlepara;
            else if (tok==TK_WORD || tok==TK_HTMLTAG)
            {
	      DBG(("CMD_BOLD: reparsing command %s\n",qPrint(g_token->name)));
              goto reparsetoken;
            }
          }
          break;
        case CMD_CODE:
          {
            children.push_back(std::make_unique<DocStyleChange>(parent,(uint)g_nodeStack.size(),DocStyleChange::Code,tokenName,TRUE));
            tok=handleStyleArgument(parent,children,tokenName);
            children.push_back(std::make_unique<DocStyleChange>(parent,(uint)g_nodeStack.size(),DocStyleChange::Code,tokenName,FALSE));
            if (tok!=TK_WORD) children.push_back(std::make_unique<DocWhiteSpace>(parent," "));
            if (tok==TK_NEWPARA) goto handlepara;
            else if (tok==TK_WORD || tok==TK_HTMLTAG)
            {
	      DBG(("CMD_CODE: reparsing command %s\n",qPrint(g_token->name)));
              goto reparsetoken;
            }
          }
          break;
        case CMD_HTMLONLY:
          {
            doctokenizerYYsetStateHtmlOnly();
            tok = doctokenizerYYlex();
            children.push_back(std::make_unique<DocVerbatim>(parent,g_context,g_token->verb,DocVerbatim::HtmlOnly,g_isExample,g_exampleName,g_token->name=="block"));
            if (tok==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"htmlonly section ended without end marker");
            doctokenizerYYsetStatePara();
          }
          break;
        case CMD_MANONLY:
          {
            doctokenizerYYsetStateManOnly();
            tok = doctokenizerYYlex();
            children.push_back(std::make_unique<DocVerbatim>(parent,g_context,g_token->verb,DocVerbatim::ManOnly,g_isExample,g_exampleName));
            if (tok==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"manonly section ended without end marker");
            doctokenizerYYsetStatePara();
          }
          break;
        case CMD_RTFONLY:
          {
            doctokenizerYYsetStateRtfOnly();
            tok = doctokenizerYYlex();
            children.push_back(std::make_unique<DocVerbatim>(parent,g_context,g_token->verb,DocVerbatim::RtfOnly,g_isExample,g_exampleName));
            if (tok==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"rtfonly section ended without end marker");
            doctokenizerYYsetStatePara();
          }
          break;
        case CMD_LATEXONLY:
          {
            doctokenizerYYsetStateLatexOnly();
            tok = doctokenizerYYlex();
            children.push_back(std::make_unique<DocVerbatim>(parent,g_context,g_token->verb,DocVerbatim::LatexOnly,g_isExample,g_exampleName));
            if (tok==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"latexonly section ended without end marker");
            doctokenizerYYsetStatePara();
          }
          break;
        case CMD_XMLONLY:
          {
            doctokenizerYYsetStateXmlOnly();
            tok = doctokenizerYYlex();
            children.push_back(std::make_unique<DocVerbatim>(parent,g_context,g_token->verb,DocVerbatim::XmlOnly,g_isExample,g_exampleName));
            if (tok==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"xmlonly section ended without end marker");
            doctokenizerYYsetStatePara();
          }
          break;
        case CMD_DBONLY:
          {
            doctokenizerYYsetStateDbOnly();
            tok = doctokenizerYYlex();
            children.push_back(std::make_unique<DocVerbatim>(parent,g_context,g_token->verb,DocVerbatim::DocbookOnly,g_isExample,g_exampleName));
            if (tok==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"docbookonly section ended without end marker");
            doctokenizerYYsetStatePara();
          }
          break;
        case CMD_FORMULA:
          {
            children.push_back(std::make_unique<DocFormula>(parent,g_token->id));
          }
          break;
        case CMD_ANCHOR:
          {
            DocAnchor *anchor = handleAnchor(parent);
            if (anchor)
            {
              children.push_back(std::unique_ptr<DocAnchor>(anchor));
            }
          }
          break;
        case CMD_INTERNALREF:
          {
            DocInternalRef *ref = handleInternalRef(parent);
            if (ref)
            {
              children.push_back(std::unique_ptr<DocInternalRef>(ref));
              ref->parse();
            }
            doctokenizerYYsetStatePara();
          }
          break;
        case CMD_SETSCOPE:
          {
            QCString scope;
            doctokenizerYYsetStateSetScope();
            (void)doctokenizerYYlex();
            scope = g_token->name;
            g_context = scope;
            //printf("Found scope='%s'\n",qPrint(scope));
            doctokenizerYYsetStatePara();
          }
          break;
        case CMD_IMAGE:
          ((DocPara *)parent) -> handleImage("image");
          break;
        default:
          return FALSE;
      }
      break;
    case TK_HTMLTAG:
      {
        switch (Mappers::htmlTagMapper->map(tokenName))
        {
          case HTML_DIV:
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found <div> tag in heading\n");
            break;
          case HTML_PRE:
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found <pre> tag in heading\n");
            break;
          case HTML_BOLD:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Bold,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Bold,tokenName);
            }
            break;
          case HTML_S:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::S,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::S,tokenName);
            }
            break;
          case HTML_STRIKE:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Strike,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Strike,tokenName);
            }
            break;
          case HTML_DEL:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Del,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Del,tokenName);
            }
            break;
          case HTML_UNDERLINE:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Underline,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Underline,tokenName);
            }
            break;
          case HTML_INS:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Ins,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Ins,tokenName);
            }
            break;
          case HTML_CODE:
          case XML_C:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Code,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Code,tokenName);
            }
            break;
          case HTML_EMPHASIS:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Italic,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Italic,tokenName);
            }
            break;
          case HTML_SUB:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Subscript,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Subscript,tokenName);
            }
            break;
          case HTML_SUP:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Superscript,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Superscript,tokenName);
            }
            break;
          case HTML_CENTER:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Center,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Center,tokenName);
            }
            break;
          case HTML_SMALL:
            if (!g_token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Small,tokenName,&g_token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Small,tokenName);
            }
            break;
          case HTML_IMG:
            if (!g_token->endTag)
              handleImg(parent,children,g_token->attribs);
	    break;
          default:
            return FALSE;
            break;
        }
      }
      break;
    case TK_SYMBOL:
      {
        DocSymbol::SymType s = DocSymbol::decodeSymbol(tokenName);
        if (s!=DocSymbol::Sym_Unknown)
        {
          children.push_back(std::make_unique<DocSymbol>(parent,s));
        }
        else
        {
          return FALSE;
        }
      }
      break;
    case TK_WHITESPACE:
    case TK_NEWPARA:
handlepara:
      if (insidePRE(parent) || !children.empty())
      {
        children.push_back(std::make_unique<DocWhiteSpace>(parent,g_token->chars));
      }
      break;
    case TK_LNKWORD:
      if (handleWord)
      {
        handleLinkedWord(parent,children);
      }
      else
        return FALSE;
      break;
    case TK_WORD:
      if (handleWord)
      {
        children.push_back(std::make_unique<DocWord>(parent,g_token->name));
      }
      else
        return FALSE;
      break;
    case TK_URL:
      if (g_insideHtmlLink)
      {
        children.push_back(std::make_unique<DocWord>(parent,g_token->name));
      }
      else
      {
        children.push_back(std::make_unique<DocURL>(parent,g_token->name,g_token->isEMailAddr));
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

//---------------------------------------------------------------------------

static void handleImg(DocNode *parent, DocNodeList &children,const HtmlAttribList &tagHtmlAttribs)
{
  bool found=FALSE;
  uint index=0;
  for (const auto &opt : tagHtmlAttribs)
  {
    //printf("option name=%s value=%s\n",qPrint(opt.name),qPrint(opt.value));
    if (opt.name=="src" && !opt.value.isEmpty())
    {
      // copy attributes
      HtmlAttribList attrList = tagHtmlAttribs;
      // and remove the src attribute
      attrList.erase(attrList.begin()+index);
      DocImage::Type t = DocImage::Html;
      children.push_back(
          std::make_unique<DocImage>(
            parent,attrList,findAndCopyImage(opt.value,t,false),t,opt.value));
      found = TRUE;
    }
    ++index;
  }
  if (!found)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"IMG tag does not have a SRC attribute!\n");
  }
}

//---------------------------------------------------------------------------

DocSymbol::SymType DocSymbol::decodeSymbol(const QCString &symName)
{
  DBG(("decodeSymbol(%s)\n",qPrint(symName)));
  return HtmlEntityMapper::instance()->name2sym(symName);
}

//---------------------------------------------------------------------------

DocEmoji::DocEmoji(DocNode *parent,const QCString &symName) :
      m_symName(symName), m_index(-1)
{
  m_parent = parent;
  QCString locSymName = symName;
  uint len=locSymName.length();
  if (len>0)
  {
    if (locSymName.at(len-1)!=':') locSymName.append(":");
    if (locSymName.at(0)!=':')     locSymName.prepend(":");
  }
  m_symName = locSymName;
  m_index = EmojiEntityMapper::instance()->symbol2index(m_symName.str());
  if (m_index==-1)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Found unsupported emoji symbol '%s'\n",qPrint(m_symName));
  }
}

//---------------------------------------------------------------------------

static int internalValidatingParseDoc(DocNode *parent,DocNodeList &children,
                                    const QCString &doc)
{
  int retval = RetVal_OK;

  if (doc.isEmpty()) return retval;

  doctokenizerYYinit(doc.data(),g_fileName,g_markdownSupport);

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  if (!children.empty() && children.back()->kind()==DocNode::Kind_Para)
  { // last child item was a paragraph
    lastPar = (DocPara*)children.back().get();
    isFirst=FALSE;
  }
  do
  {
    DocPara *par = new DocPara(parent);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (!par->isEmpty())
    {
      children.push_back(std::unique_ptr<DocNode>(par));
      if (lastPar) lastPar->markLast(FALSE);
      lastPar=par;
    }
    else
    {
      delete par;
    }
  } while (retval==TK_NEWPARA);
  if (lastPar) lastPar->markLast();

  //printf("internalValidateParsingDoc: %p: isFirst=%d isLast=%d\n",
  //   lastPar,lastPar?lastPar->isFirst():-1,lastPar?lastPar->isLast():-1);

  return retval;
}

//---------------------------------------------------------------------------

static void readTextFileByName(const QCString &file,QCString &text)
{
  if (Portable::isAbsolutePath(file))
  {
    FileInfo fi(file.str());
    if (fi.exists())
    {
      text = fileToString(file,Config_getBool(FILTER_SOURCE_FILES));
      return;
    }
  }
  const StringVector &examplePathList = Config_getList(EXAMPLE_PATH);
  for (const auto &s : examplePathList)
  {
    std::string absFileName = s+(Portable::pathSeparator()+file).str();
    FileInfo fi(absFileName);
    if (fi.exists())
    {
      text = fileToString(QCString(absFileName),Config_getBool(FILTER_SOURCE_FILES));
      return;
    }
  }

  // as a fallback we also look in the exampleNameDict
  bool ambig;
  FileDef *fd = findFileDef(Doxygen::exampleNameLinkedMap,file,ambig);
  if (fd)
  {
    text = fileToString(fd->absFilePath(),Config_getBool(FILTER_SOURCE_FILES));
    if (ambig)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"included file name %s is ambiguous"
           "Possible candidates:\n%s",qPrint(file),
           qPrint(showFileDefMatches(Doxygen::exampleNameLinkedMap,file))
          );
    }
  }
  else
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"included file %s is not found. "
           "Check your EXAMPLE_PATH",qPrint(file));
  }
}

//---------------------------------------------------------------------------

DocWord::DocWord(DocNode *parent,const QCString &word) :
      m_word(word)
{
  m_parent = parent;
  //printf("new word %s url=%s\n",qPrint(word),qPrint(g_searchUrl));
  if (Doxygen::searchIndex && !g_searchUrl.isEmpty())
  {
    Doxygen::searchIndex->addWord(word,FALSE);
  }
}

//---------------------------------------------------------------------------

DocLinkedWord::DocLinkedWord(DocNode *parent,const QCString &word,
                  const QCString &ref,const QCString &file,
                  const QCString &anchor,const QCString &tooltip) :
      m_word(word), m_ref(ref),
      m_file(file), m_relPath(g_relPath), m_anchor(anchor),
      m_tooltip(tooltip)
{
  m_parent = parent;
  //printf("DocLinkedWord: new word %s url=%s tooltip='%s'\n",
  //    qPrint(word),qPrint(g_searchUrl),qPrint(tooltip));
  if (Doxygen::searchIndex && !g_searchUrl.isEmpty())
  {
    Doxygen::searchIndex->addWord(word,FALSE);
  }
}

//---------------------------------------------------------------------------

DocAnchor::DocAnchor(DocNode *parent,const QCString &id,bool newAnchor)
{
  m_parent = parent;
  if (id.isEmpty())
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Empty anchor label");
    return;
  }

  const CitationManager &ct = CitationManager::instance();
  QCString anchorPrefix = ct.anchorPrefix();
  if (id.left(anchorPrefix.length()) == anchorPrefix)
  {
    const CiteInfo *cite = ct.find(id.mid(anchorPrefix.length()));
    if (cite)
    {
      m_file = convertNameToFile(ct.fileName(),FALSE,TRUE);
      m_anchor = id;
    }
    else
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid cite anchor id '%s'",qPrint(id));
      m_anchor = "invalid";
      m_file = "invalid";
    }
  }
  else if (newAnchor) // found <a name="label">
  {
    m_anchor = id;
  }
  else // found \anchor label
  {
    const SectionInfo *sec = SectionManager::instance().find(id);
    if (sec)
    {
      //printf("Found anchor %s\n",qPrint(id));
      m_file   = sec->fileName();
      m_anchor = sec->label();
    }
    else
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid anchor id '%s'",qPrint(id));
      m_anchor = "invalid";
      m_file = "invalid";
    }
  }
}

//---------------------------------------------------------------------------

DocVerbatim::DocVerbatim(DocNode *parent,const QCString &context,
    const QCString &text, Type t,bool isExample,
    const QCString &exampleFile,bool isBlock,const QCString &lang)
  : m_context(context), m_text(text), m_type(t),
    m_isExample(isExample), m_exampleFile(exampleFile),
    m_relPath(g_relPath), m_lang(lang), m_isBlock(isBlock)
{
  m_parent = parent;
}


//---------------------------------------------------------------------------

void DocInclude::parse()
{
  DBG(("DocInclude::parse(file=%s,text=%s)\n",qPrint(m_file),qPrint(m_text)));
  switch(m_type)
  {
    case DontIncWithLines:
      // fall through
    case IncWithLines:
      // fall through
    case Include:
      // fall through
    case DontInclude:
      readTextFileByName(m_file,m_text);
      g_includeFileName   = m_file;
      g_includeFileText   = m_text;
      g_includeFileOffset = 0;
      g_includeFileLength = m_text.length();
      g_includeFileLine   = 0;
      g_includeFileShowLineNo = (m_type == DontIncWithLines || m_type == IncWithLines);
      //printf("g_includeFile=<<%s>>\n",qPrint(g_includeFileText));
      break;
    case VerbInclude:
      // fall through
    case HtmlInclude:
    case LatexInclude:
    case DocInclude::RtfInclude:
    case DocInclude::ManInclude:
    case DocInclude::XmlInclude:
    case DocInclude::DocbookInclude:
      readTextFileByName(m_file,m_text);
      break;
    case Snippet:
    case SnipWithLines:
      readTextFileByName(m_file,m_text);
      // check here for the existence of the blockId inside the file, so we
      // only generate the warning once.
      int count;
      if (!m_blockId.isEmpty() && (count=m_text.contains(m_blockId.data()))!=2)
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"block marked with %s for \\snippet should appear twice in file %s, found it %d times\n",
            qPrint(m_blockId),qPrint(m_file),count);
      }
      break;
    case DocInclude::SnippetDoc:
    case DocInclude::IncludeDoc:
      err("Internal inconsistency: found switch SnippetDoc / IncludeDoc in file: %s"
          "Please create a bug report\n",__FILE__);
      break;
  }
}

//---------------------------------------------------------------------------

void DocIncOperator::parse()
{
  if (g_includeFileName.isEmpty())
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),
                   "No previous '\\include' or '\\dontinclude' command for '\\%s' present",
                   typeAsString());
  }

  m_includeFileName = g_includeFileName;
  const char *p = g_includeFileText.data();
  uint l = g_includeFileLength;
  uint o = g_includeFileOffset;
  int il = g_includeFileLine;
  DBG(("DocIncOperator::parse() text=%s off=%d len=%d\n",p,o,l));
  uint so = o,bo;
  bool nonEmpty = FALSE;
  switch(type())
  {
    case Line:
      while (o<l)
      {
        char c = p[o];
        if (c=='\n')
        {
          g_includeFileLine++;
          if (nonEmpty) break; // we have a pattern to match
          so=o+1; // no pattern, skip empty line
        }
        else if (!isspace((uchar)c)) // no white space char
        {
          nonEmpty=TRUE;
        }
        o++;
      }
      if (g_includeFileText.mid(so,o-so).find(m_pattern)!=-1)
      {
        m_line  = il;
        m_text = g_includeFileText.mid(so,o-so);
        DBG(("DocIncOperator::parse() Line: %s\n",qPrint(m_text)));
      }
      g_includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = g_includeFileShowLineNo;
      break;
    case SkipLine:
      while (o<l)
      {
        so=o;
        while (o<l)
        {
          char c = p[o];
          if (c=='\n')
          {
            g_includeFileLine++;
            if (nonEmpty) break; // we have a pattern to match
            so=o+1; // no pattern, skip empty line
          }
          else if (!isspace((uchar)c)) // no white space char
          {
            nonEmpty=TRUE;
          }
          o++;
        }
        if (g_includeFileText.mid(so,o-so).find(m_pattern)!=-1)
        {
          m_line  = il;
          m_text = g_includeFileText.mid(so,o-so);
          DBG(("DocIncOperator::parse() SkipLine: %s\n",qPrint(m_text)));
          break;
        }
        o++; // skip new line
      }
      g_includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = g_includeFileShowLineNo;
      break;
    case Skip:
      while (o<l)
      {
        so=o;
        while (o<l)
        {
          char c = p[o];
          if (c=='\n')
          {
            g_includeFileLine++;
            if (nonEmpty) break; // we have a pattern to match
            so=o+1; // no pattern, skip empty line
          }
          else if (!isspace((uchar)c)) // no white space char
          {
            nonEmpty=TRUE;
          }
          o++;
        }
        if (g_includeFileText.mid(so,o-so).find(m_pattern)!=-1)
        {
          break;
        }
        o++; // skip new line
      }
      g_includeFileOffset = so; // set pointer to start of new line
      m_showLineNo = g_includeFileShowLineNo;
      break;
    case Until:
      bo=o;
      while (o<l)
      {
        so=o;
        while (o<l)
        {
          char c = p[o];
          if (c=='\n')
          {
            g_includeFileLine++;
            if (nonEmpty) break; // we have a pattern to match
            so=o+1; // no pattern, skip empty line
          }
          else if (!isspace((uchar)c)) // no white space char
          {
            nonEmpty=TRUE;
          }
          o++;
        }
        if (g_includeFileText.mid(so,o-so).find(m_pattern)!=-1)
        {
          m_line  = il;
          m_text = g_includeFileText.mid(bo,o-bo);
          DBG(("DocIncOperator::parse() Until: %s\n",qPrint(m_text)));
          break;
        }
        o++; // skip new line
      }
      g_includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = g_includeFileShowLineNo;
      break;
  }
}

//---------------------------------------------------------------------------

DocXRefItem::DocXRefItem(DocNode *parent,int id,const QCString &key) :
   m_id(id), m_key(key), m_relPath(g_relPath)
{
   m_parent = parent;
}

bool DocXRefItem::parse()
{
  RefList *refList = RefListManager::instance().find(m_key);
  if (refList && refList->isEnabled())
  {
    RefItem *item = refList->find(m_id);
    ASSERT(item!=0);
    if (item)
    {
      if (g_memberDef && g_memberDef->name().at(0)=='@')
      {
        m_file   = "@";  // can't cross reference anonymous enum
        m_anchor = "@";
      }
      else
      {
        m_file   = refList->fileName();
        m_anchor = item->anchor();
      }
      m_title  = refList->sectionTitle();
      //printf("DocXRefItem: file=%s anchor=%s title=%s\n",
      //    qPrint(m_file),qPrint(m_anchor),qPrint(m_title));

      if (!item->text().isEmpty())
      {
        docParserPushContext();
        internalValidatingParseDoc(this,m_children,item->text());
        docParserPopContext();
      }
    }
    return TRUE;
  }
  return FALSE;
}

//---------------------------------------------------------------------------

DocFormula::DocFormula(DocNode *parent,int id) :
      m_relPath(g_relPath)
{
  m_parent = parent;
  QCString text = FormulaManager::instance().findFormula(id);
  if (!text.isEmpty())
  {
    m_id = id;
    m_name.sprintf("form_%d",m_id);
    m_text = text;
  }
  else // wrong \_form#<n> command
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Wrong formula id %d",id);
    m_id = -1;
  }
}

//---------------------------------------------------------------------------

//int DocLanguage::parse()
//{
//  int retval;
//  DBG(("DocLanguage::parse() start\n"));
//  auto ns = AutoNodeStack(this);
//
//  // parse one or more paragraphs
//  bool isFirst=TRUE;
//  DocPara *par=0;
//  do
//  {
//    par = new DocPara(this);
//    if (isFirst) { par->markFirst(); isFirst=FALSE; }
//    m_children.push_back(std::unique_ptr<DocPara>(par));
//    retval=par->parse();
//  }
//  while (retval==TK_NEWPARA);
//  if (par) par->markLast();
//
//  DBG(("DocLanguage::parse() end\n"));
//  return retval;
//}

//---------------------------------------------------------------------------

void DocSecRefItem::parse()
{
  DBG(("DocSecRefItem::parse() start\n"));
  auto ns = AutoNodeStack(this);

  doctokenizerYYsetStateTitle();
  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children))
    {
      errorHandleDefaultToken(this,tok,m_children,"\\refitem");
    }
  }
  doctokenizerYYsetStatePara();
  handlePendingStyleCommands(this,m_children);

  const SectionInfo *sec=0;
  if (!m_target.isEmpty())
  {
    sec = SectionManager::instance().find(m_target);
    if (sec)
    {
      m_file   = sec->fileName();
      m_anchor = sec->label();
    }
    else
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"reference to unknown section %s",
          qPrint(m_target));
    }
  }
  else
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"reference to empty target");
  }

  DBG(("DocSecRefItem::parse() end\n"));
}

//---------------------------------------------------------------------------

void DocSecRefList::parse()
{
  DBG(("DocSecRefList::parse() start\n"));
  auto ns = AutoNodeStack(this);

  int tok=doctokenizerYYlex();
  // skip white space
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=doctokenizerYYlex();
  // handle items
  while (tok)
  {
    if (tok==TK_COMMAND_AT || tok == TK_COMMAND_BS)
    {
      const char *cmd_start = (tok==TK_COMMAND_AT ? "@" : "\\");
      switch (Mappers::cmdMapper->map(g_token->name))
      {
        case CMD_SECREFITEM:
          {
            tok=doctokenizerYYlex();
            if (tok!=TK_WHITESPACE)
            {
              warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\refitem command");
              break;
            }
            tok=doctokenizerYYlex();
            if (tok!=TK_WORD && tok!=TK_LNKWORD)
            {
              warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of \\refitem",
                  tokToString(tok));
              break;
            }

            DocSecRefItem *item = new DocSecRefItem(this,g_token->name);
            m_children.push_back(std::unique_ptr<DocSecRefItem>(item));
            item->parse();
          }
          break;
        case CMD_ENDSECREFLIST:
          goto endsecreflist;
        default:
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Illegal command %s as part of a \\secreflist",
              qPrint(cmd_start + g_token->name));
          goto endsecreflist;
      }
    }
    else if (tok==TK_WHITESPACE)
    {
      // ignore whitespace
    }
    else
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected token %s inside section reference list",
          tokToString(tok));
      goto endsecreflist;
    }
    tok=doctokenizerYYlex();
  }

endsecreflist:
  DBG(("DocSecRefList::parse() end\n"));
}

//---------------------------------------------------------------------------

DocInternalRef::DocInternalRef(DocNode *parent,const QCString &ref)
  : m_relPath(g_relPath)
{
  m_parent = parent;
  int i=ref.find('#');
  if (i!=-1)
  {
    m_anchor = ref.right((int)ref.length()-i-1);
    m_file   = ref.left(i);
  }
  else
  {
    m_file = ref;
  }
}

void DocInternalRef::parse()
{
  auto ns = AutoNodeStack(this);
  DBG(("DocInternalRef::parse() start\n"));

  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children))
    {
      errorHandleDefaultToken(this,tok,m_children,"\\ref");
    }
  }

  handlePendingStyleCommands(this,m_children);
  DBG(("DocInternalRef::parse() end\n"));
}

//---------------------------------------------------------------------------

DocRef::DocRef(DocNode *parent,const QCString &target,const QCString &context) :
   m_refType(Unknown), m_isSubPage(FALSE)
{
  m_parent = parent;
  const Definition  *compound = 0;
  QCString     anchor;
  //printf("DocRef::DocRef(target=%s,context=%s)\n",qPrint(target),qPrint(context));
  ASSERT(!target.isEmpty());
  SrcLangExt lang = getLanguageFromFileName(target);
  m_relPath = g_relPath;
  const SectionInfo *sec = SectionManager::instance().find(target);
  if (sec==0 && lang==SrcLangExt_Markdown) // lookup as markdown file
  {
    sec = SectionManager::instance().find(markdownFileNameToId(target));
  }
  if (sec) // ref to section or anchor
  {
    PageDef *pd = 0;
    if (sec->type()==SectionType::Page)
    {
      pd = Doxygen::pageLinkedMap->find(target);
    }
    m_text         = sec->title();
    if (m_text.isEmpty()) m_text = sec->label();

    m_ref          = sec->ref();
    m_file         = stripKnownExtensions(sec->fileName());
    if (sec->type()==SectionType::Anchor)
    {
      m_refType = Anchor;
    }
    else if (sec->type()==SectionType::Table)
    {
      m_refType = Table;
    }
    else
    {
      m_refType = Section;
    }
    m_isSubPage    = pd && pd->hasParentPage();
    if (sec->type()!=SectionType::Page || m_isSubPage) m_anchor = sec->label();
    //printf("m_text=%s,m_ref=%s,m_file=%s,type=%d\n",
    //    qPrint(m_text),qPrint(m_ref),qPrint(m_file),m_refType);
    return;
  }
  else if (resolveLink(context,target,TRUE,&compound,anchor))
  {
    bool isFile = compound ?
                 (compound->definitionType()==Definition::TypeFile ||
                  compound->definitionType()==Definition::TypePage ? TRUE : FALSE) :
                 FALSE;
    m_text = linkToText(compound?compound->getLanguage():SrcLangExt_Unknown,target,isFile);
    m_anchor = anchor;
    if (compound && compound->isLinkable()) // ref to compound
    {
      if (anchor.isEmpty() &&                                  /* compound link */
          compound->definitionType()==Definition::TypeGroup && /* is group */
          !toGroupDef(compound)->groupTitle().isEmpty()        /* with title */
         )
      {
        m_text=(toGroupDef(compound))->groupTitle(); // use group's title as link
      }
      else if (compound->definitionType()==Definition::TypeMember &&
          toMemberDef(compound)->isObjCMethod())
      {
        // Objective C Method
        const MemberDef *member = toMemberDef(compound);
        bool localLink = g_memberDef ? member->getClassDef()==g_memberDef->getClassDef() : FALSE;
        m_text = member->objCMethodName(localLink,g_inSeeBlock);
      }

      m_file = compound->getOutputFileBase();
      m_ref  = compound->getReference();
      //printf("isFile=%d compound=%s (%d)\n",isFile,qPrint(compound->name()),
      //    compound->definitionType());
      return;
    }
    else if (compound && compound->definitionType()==Definition::TypeFile &&
             toFileDef(compound)->generateSourceFile()
            ) // undocumented file that has source code we can link to
    {
      m_file = compound->getSourceFileBase();
      m_ref  = compound->getReference();
      return;
    }
  }
  m_text = target;
  warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unable to resolve reference to '%s' for \\ref command",
           qPrint(target));
}

static void flattenParagraphs(DocNode *root,DocNodeList &children)
{
  DocNodeList newChildren;
  for (const auto &dn : children)
  {
    if (dn->kind()==DocNode::Kind_Para)
    {
      DocPara *para = (DocPara*)dn.get();
      // move the children of the paragraph to the end of the newChildren list
      newChildren.insert(newChildren.end(),
                         std::make_move_iterator(para->children().begin()),
                         std::make_move_iterator(para->children().end()));
    }
  }

  // replace the children list by the newChildren list
  children.clear();
  children.insert(children.end(),
                  std::make_move_iterator(newChildren.begin()),
                  std::make_move_iterator(newChildren.end()));
  // reparent the children
  for (const auto &cn : children)
  {
    cn->setParent(root);
  }
}

void DocRef::parse()
{
  auto ns = AutoNodeStack(this);
  DBG(("DocRef::parse() start\n"));

  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          break;
        default:
          errorHandleDefaultToken(this,tok,m_children,"\\ref");
          break;
      }
    }
  }

  if (m_children.empty() && !m_text.isEmpty())
  {
    g_insideHtmlLink=TRUE;
    docParserPushContext();
    internalValidatingParseDoc(this,m_children,m_text);
    docParserPopContext();
    g_insideHtmlLink=FALSE;
    flattenParagraphs(this,m_children);
  }

  handlePendingStyleCommands(this,m_children);
}

//---------------------------------------------------------------------------

DocCite::DocCite(DocNode *parent,const QCString &target,const QCString &) //context)
{
  size_t numBibFiles = Config_getList(CITE_BIB_FILES).size();
  m_parent = parent;
  //printf("DocCite::DocCite(target=%s)\n",qPrint(target));
  ASSERT(!target.isEmpty());
  m_relPath = g_relPath;
  const CitationManager &ct = CitationManager::instance();
  const CiteInfo *cite = ct.find(target);
  //printf("cite=%p text='%s' numBibFiles=%d\n",cite,cite?qPrint(cite->text):"<null>",numBibFiles);
  if (numBibFiles>0 && cite && !cite->text().isEmpty()) // ref to citation
  {
    m_text         = cite->text();
    m_ref          = "";
    m_anchor       = ct.anchorPrefix()+cite->label();
    m_file         = convertNameToFile(ct.fileName(),FALSE,TRUE);
    //printf("CITE ==> m_text=%s,m_ref=%s,m_file=%s,m_anchor=%s\n",
    //    qPrint(m_text),qPrint(m_ref),qPrint(m_file),qPrint(m_anchor));
    return;
  }
  m_text = target;
  if (numBibFiles==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"\\cite command found but no bib files specified via CITE_BIB_FILES!");
  }
  else if (cite==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unable to resolve reference to '%s' for \\cite command",
             qPrint(target));
  }
  else
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"\\cite command to '%s' does not have an associated number",
             qPrint(target));
  }
}

//---------------------------------------------------------------------------

DocLink::DocLink(DocNode *parent,const QCString &target)
{
  m_parent = parent;
  const Definition *compound = 0;
  QCString anchor;
  m_refText = target;
  m_relPath = g_relPath;
  if (!m_refText.isEmpty() && m_refText.at(0)=='#')
  {
    m_refText = m_refText.right(m_refText.length()-1);
  }
  if (resolveLink(g_context,stripKnownExtensions(target),g_inSeeBlock,&compound,anchor))
  {
    m_anchor = anchor;
    if (compound && compound->isLinkable())
    {
      m_file = compound->getOutputFileBase();
      m_ref  = compound->getReference();
    }
    else if (compound && compound->definitionType()==Definition::TypeFile &&
             (toFileDef(compound))->generateSourceFile()
            ) // undocumented file that has source code we can link to
    {
      m_file = compound->getSourceFileBase();
      m_ref  = compound->getReference();
    }
    return;
  }

  // bogus link target
  warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unable to resolve link to '%s' for \\link command",
         qPrint(target));
}


QCString DocLink::parse(bool isJavaLink,bool isXmlLink)
{
  QCString result;
  auto ns = AutoNodeStack(this);
  DBG(("DocLink::parse() start\n"));

  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children,FALSE))
    {
      const char *cmd_start = "\\";
      switch (tok)
      {
        case TK_COMMAND_AT:
          cmd_start = "@";
        // fall through
        case TK_COMMAND_BS:
          switch (Mappers::cmdMapper->map(g_token->name))
          {
            case CMD_ENDLINK:
              if (isJavaLink)
              {
                warn_doc_error(g_fileName,getDoctokinizerLineNr(),"{@link.. ended with @endlink command");
              }
              goto endlink;
            default:
              warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Illegal command %s as part of a \\link",
                  qPrint(cmd_start + g_token->name));
              break;
          }
          break;
        case TK_SYMBOL:
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unsupported symbol %s found as part of a \\link",
              qPrint(g_token->name));
          break;
        case TK_HTMLTAG:
          if (g_token->name!="see" || !isXmlLink)
          {
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected xml/html command %s found as part of a \\link",
                qPrint(g_token->name));
          }
          goto endlink;
        case TK_LNKWORD:
        case TK_WORD:
          if (isJavaLink) // special case to detect closing }
          {
            QCString w = g_token->name;
            int p;
            if (w=="}")
            {
              goto endlink;
            }
            else if ((p=w.find('}'))!=-1)
            {
              uint l=w.length();
              m_children.push_back(std::make_unique<DocWord>(this,w.left(p)));
              if ((uint)p<l-1) // something left after the } (for instance a .)
              {
                result=w.right((int)l-p-1);
              }
              goto endlink;
            }
          }
          m_children.push_back(std::make_unique<DocWord>(this,g_token->name));
          break;
        default:
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected token %s",
             tokToString(tok));
        break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected end of comment while inside"
           " link command\n");
  }
endlink:

  if (m_children.empty()) // no link text
  {
    m_children.push_back(std::make_unique<DocWord>(this,m_refText));
  }

  handlePendingStyleCommands(this,m_children);
  DBG(("DocLink::parse() end\n"));
  return result;
}


//---------------------------------------------------------------------------

DocDotFile::DocDotFile(DocNode *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile,int srcLine) :
  DocDiagramFileBase(name,context,srcFile,srcLine)
{
  m_relPath = g_relPath;
  m_parent = parent;
}

bool DocDotFile::parse()
{
  bool ok = false;
  defaultHandleTitleAndSize(CMD_DOTFILE,this,m_children,m_width,m_height);

  bool ambig;
  FileDef *fd = findFileDef(Doxygen::dotFileNameLinkedMap,m_name,ambig);
  if (fd==0 && m_name.right(4)!=".dot") // try with .dot extension as well
  {
    fd = findFileDef(Doxygen::dotFileNameLinkedMap,m_name+".dot",ambig);
  }
  if (fd)
  {
    m_file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"included dot file name %s is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(m_name),
           qPrint(showFileDefMatches(Doxygen::dotFileNameLinkedMap,m_name))
          );
    }
  }
  else
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"included dot file %s is not found "
           "in any of the paths specified via DOTFILE_DIRS!",qPrint(m_name));
  }
  return ok;
}

DocMscFile::DocMscFile(DocNode *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile, int srcLine) :
  DocDiagramFileBase(name,context,srcFile,srcLine)
{
  m_relPath = g_relPath;
  m_parent = parent;
}

bool DocMscFile::parse()
{
  bool ok = false;
  defaultHandleTitleAndSize(CMD_MSCFILE,this,m_children,m_width,m_height);

  bool ambig;
  FileDef *fd = findFileDef(Doxygen::mscFileNameLinkedMap,m_name,ambig);
  if (fd==0 && m_name.right(4)!=".msc") // try with .msc extension as well
  {
    fd = findFileDef(Doxygen::mscFileNameLinkedMap,m_name+".msc",ambig);
  }
  if (fd)
  {
    m_file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"included msc file name %s is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(m_name),
           qPrint(showFileDefMatches(Doxygen::mscFileNameLinkedMap,m_name))
          );
    }
  }
  else
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"included msc file %s is not found "
           "in any of the paths specified via MSCFILE_DIRS!",qPrint(m_name));
  }
  return ok;
}

//---------------------------------------------------------------------------

DocDiaFile::DocDiaFile(DocNode *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile,int srcLine) :
  DocDiagramFileBase(name,context,srcFile,srcLine)
{
  m_relPath = g_relPath;
  m_parent = parent;
}

bool DocDiaFile::parse()
{
  bool ok = false;
  defaultHandleTitleAndSize(CMD_DIAFILE,this,m_children,m_width,m_height);

  bool ambig;
  FileDef *fd = findFileDef(Doxygen::diaFileNameLinkedMap,m_name,ambig);
  if (fd==0 && m_name.right(4)!=".dia") // try with .dia extension as well
  {
    fd = findFileDef(Doxygen::diaFileNameLinkedMap,m_name+".dia",ambig);
  }
  if (fd)
  {
    m_file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"included dia file name %s is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(m_name),
           qPrint(showFileDefMatches(Doxygen::diaFileNameLinkedMap,m_name))
          );
    }
  }
  else
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"included dia file %s is not found "
           "in any of the paths specified via DIAFILE_DIRS!",qPrint(m_name));
  }
  return ok;
}

//---------------------------------------------------------------------------

DocVhdlFlow::DocVhdlFlow(DocNode *parent)
{
  m_parent = parent;
}

void DocVhdlFlow::parse()
{
  auto ns = AutoNodeStack(this);
  DBG(("DocVhdlFlow::parse() start\n"));

  doctokenizerYYsetStateTitle();
  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children))
    {
      errorHandleDefaultToken(this,tok,m_children,"\\vhdlflow");
    }
  }
  tok=doctokenizerYYlex();

  doctokenizerYYsetStatePara();
  handlePendingStyleCommands(this,m_children);

  DBG(("DocVhdlFlow::parse() end\n"));
  VhdlDocGen::createFlowChart(g_memberDef);
}


//---------------------------------------------------------------------------

DocImage::DocImage(DocNode *parent,const HtmlAttribList &attribs,const QCString &name,
                   Type t,const QCString &url, bool inlineImage) :
      m_attribs(attribs), m_name(name),
      m_type(t), m_relPath(g_relPath),
      m_url(url), m_inlineImage(inlineImage)
{
  m_parent = parent;
}

bool DocImage::isSVG() const
{
  QCString  locName = m_url.isEmpty() ? m_name : m_url;
  int len = (int)locName.length();
  int fnd = locName.find('?'); // ignore part from ? until end
  if (fnd==-1) fnd=len;
  return fnd>=4 && locName.mid(fnd-4,4)==".svg";
}

void DocImage::parse()
{
  defaultHandleTitleAndSize(CMD_IMAGE,this,m_children,m_width,m_height);
}


//---------------------------------------------------------------------------

int DocHtmlHeader::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlHeader::parse() start\n"));

  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(g_token->name);
            if (tagId==HTML_H1 && g_token->endTag) // found </h1> tag
            {
              if (m_level!=1)
              {
                warn_doc_error(g_fileName,getDoctokinizerLineNr(),"<h%d> ended with </h1>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H2 && g_token->endTag) // found </h2> tag
            {
              if (m_level!=2)
              {
                warn_doc_error(g_fileName,getDoctokinizerLineNr(),"<h%d> ended with </h2>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H3 && g_token->endTag) // found </h3> tag
            {
              if (m_level!=3)
              {
                warn_doc_error(g_fileName,getDoctokinizerLineNr(),"<h%d> ended with </h3>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H4 && g_token->endTag) // found </h4> tag
            {
              if (m_level!=4)
              {
                warn_doc_error(g_fileName,getDoctokinizerLineNr(),"<h%d> ended with </h4>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H5 && g_token->endTag) // found </h5> tag
            {
              if (m_level!=5)
              {
                warn_doc_error(g_fileName,getDoctokinizerLineNr(),"<h%d> ended with </h5>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H6 && g_token->endTag) // found </h6> tag
            {
              if (m_level!=6)
              {
                warn_doc_error(g_fileName,getDoctokinizerLineNr(),"<h%d> ended with </h6>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_A)
            {
              if (!g_token->endTag)
              {
                handleAHref(this,m_children,g_token->attribs);
              }
            }
            else if (tagId==HTML_BR)
            {
              m_children.push_back(std::make_unique<DocLineBreak>(this,g_token->attribs));
            }
            else
            {
              warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected html tag <%s%s> found within <h%d> context",
                  g_token->endTag?"/":"",qPrint(g_token->name),m_level);
            }
          }
          break;
        default:
	  char tmp[20];
	  sprintf(tmp,"<h%d>tag",m_level);
          errorHandleDefaultToken(this,tok,m_children,tmp);
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected end of comment while inside"
           " <h%d> tag\n",m_level);
  }
endheader:
  handlePendingStyleCommands(this,m_children);
  DBG(("DocHtmlHeader::parse() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocHRef::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocHRef::parse() start\n"));

  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(g_token->name);
            if (tagId==HTML_A && g_token->endTag) // found </a> tag
            {
              goto endhref;
            }
            else
            {
              warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected html tag <%s%s> found within <a href=...> context",
                  g_token->endTag?"/":"",qPrint(g_token->name));
            }
          }
          break;
        default:
          errorHandleDefaultToken(this,tok,m_children,"<a>..</a> block");
          break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected end of comment while inside"
           " <a href=...> tag");
  }
endhref:
  handlePendingStyleCommands(this,m_children);
  DBG(("DocHRef::parse() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocInternal::parse(int level)
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocInternal::parse() start\n"));

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  do
  {
    DocPara *par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (!par->isEmpty())
    {
      m_children.push_back(std::unique_ptr<DocPara>(par));
      lastPar=par;
    }
    else
    {
      delete par;
    }
    if (retval==TK_LISTITEM)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid list item found");
    }
  } while (retval!=0 &&
           retval!=RetVal_Section &&
           retval!=RetVal_Subsection &&
           retval!=RetVal_Subsubsection &&
           retval!=RetVal_Paragraph &&
           retval!=RetVal_EndInternal
          );
  if (lastPar) lastPar->markLast();

  // then parse any number of level-n sections
  while ((level==1 && retval==RetVal_Section) ||
         (level==2 && retval==RetVal_Subsection) ||
         (level==3 && retval==RetVal_Subsubsection) ||
         (level==4 && retval==RetVal_Paragraph)
        )
  {
    DocSection *s=new DocSection(this,
        std::min(level+Doxygen::subpageNestingLevel,5),g_token->sectionId);
    m_children.push_back(std::unique_ptr<DocSection>(s));
    retval = s->parse();
  }

  if (retval==RetVal_Internal)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"\\internal command found inside internal section");
  }

  DBG(("DocInternal::parse() end: retval=%x\n",retval));
  return retval;
}

//---------------------------------------------------------------------------

int DocIndexEntry::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocIndexEntry::parse() start\n"));
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\addindex command");
    goto endindexentry;
  }
  doctokenizerYYsetStateTitle();
  m_entry="";
  while ((tok=doctokenizerYYlex()))
  {
    switch (tok)
    {
      case TK_WHITESPACE:
        m_entry+=" ";
        break;
      case TK_WORD:
      case TK_LNKWORD:
        m_entry+=g_token->name;
        break;
      case TK_SYMBOL:
        {
          DocSymbol::SymType s = DocSymbol::decodeSymbol(g_token->name);
          switch (s)
          {
            case DocSymbol::Sym_BSlash:  m_entry+='\\'; break;
            case DocSymbol::Sym_At:      m_entry+='@';  break;
            case DocSymbol::Sym_Less:    m_entry+='<';  break;
            case DocSymbol::Sym_Greater: m_entry+='>';  break;
            case DocSymbol::Sym_Amp:     m_entry+='&';  break;
            case DocSymbol::Sym_Dollar:  m_entry+='$';  break;
            case DocSymbol::Sym_Hash:    m_entry+='#';  break;
            case DocSymbol::Sym_Percent: m_entry+='%';  break;
            case DocSymbol::Sym_apos:    m_entry+='\''; break;
            case DocSymbol::Sym_Quot:    m_entry+='"';  break;
            case DocSymbol::Sym_lsquo:   m_entry+='`';  break;
            case DocSymbol::Sym_rsquo:   m_entry+='\'';  break;
            case DocSymbol::Sym_ldquo:   m_entry+="``";  break;
            case DocSymbol::Sym_rdquo:   m_entry+="''";  break;
            case DocSymbol::Sym_ndash:   m_entry+="--";  break;
            case DocSymbol::Sym_mdash:   m_entry+="---";  break;
            default:
              warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected symbol found as argument of \\addindex");
              break;
          }
        }
        break;
      case TK_COMMAND_AT:
        // fall through
      case TK_COMMAND_BS:
        switch (Mappers::cmdMapper->map(g_token->name))
        {
          case CMD_BSLASH:  m_entry+='\\';  break;
          case CMD_AT:      m_entry+='@';   break;
          case CMD_LESS:    m_entry+='<';   break;
          case CMD_GREATER: m_entry+='>';   break;
          case CMD_AMP:     m_entry+='&';   break;
          case CMD_DOLLAR:  m_entry+='$';   break;
          case CMD_HASH:    m_entry+='#';   break;
          case CMD_DCOLON:  m_entry+="::";  break;
          case CMD_PERCENT: m_entry+='%';   break;
          case CMD_NDASH:   m_entry+="--";  break;
          case CMD_MDASH:   m_entry+="---"; break;
          case CMD_QUOTE:   m_entry+='"';   break;
          case CMD_PUNT:    m_entry+='.';   break;
          case CMD_PLUS:    m_entry+='+';   break;
          case CMD_MINUS:   m_entry+='-';   break;
          case CMD_EQUAL:   m_entry+='=';   break;
          default:
               warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected command %s found as argument of \\addindex",
                              qPrint(g_token->name));
               break;
        }
      break;
      default:
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected token %s",
            tokToString(tok));
        break;
    }
  }
  doctokenizerYYsetStatePara();
  m_entry = m_entry.stripWhiteSpace();
endindexentry:
  DBG(("DocIndexEntry::parse() end retval=%x\n",retval));
  return retval;
}

//---------------------------------------------------------------------------

DocHtmlCaption::DocHtmlCaption(DocNode *parent,const HtmlAttribList &attribs)
{
  m_hasCaptionId = FALSE;
  for (const auto &opt : attribs)
  {
    if (opt.name=="id" && !opt.value.isEmpty()) // interpret id attribute as an anchor
    {
      const SectionInfo *sec = SectionManager::instance().find(opt.value);
      if (sec)
      {
        //printf("Found anchor %s\n",qPrint(id));
        m_file   = sec->fileName();
        m_anchor = sec->label();
        m_hasCaptionId = TRUE;
      }
      else
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid caption id '%s'",qPrint(opt.value));
      }
    }
    else // copy attribute
    {
      m_attribs.push_back(opt);
    }
  }
  m_parent = parent;
}

int DocHtmlCaption::parse()
{
  int retval=0;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlCaption::parse() start\n"));
  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(g_token->name);
            if (tagId==HTML_CAPTION && g_token->endTag) // found </caption> tag
            {
              retval = RetVal_OK;
              goto endcaption;
            }
            else
            {
              warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected html tag <%s%s> found within <caption> context",
                  g_token->endTag?"/":"",qPrint(g_token->name));
            }
          }
          break;
        default:
          errorHandleDefaultToken(this,tok,m_children,"<caption> tag");
          break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected end of comment while inside"
           " <caption> tag");
  }
endcaption:
  handlePendingStyleCommands(this,m_children);
  DBG(("DocHtmlCaption::parse() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlCell::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlCell::parse() start\n"));

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
    if (retval==TK_HTMLTAG)
    {
      int tagId=Mappers::htmlTagMapper->map(g_token->name);
      if (tagId==HTML_TD && g_token->endTag) // found </dt> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
      else if (tagId==HTML_TH && g_token->endTag) // found </th> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
    }
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  DBG(("DocHtmlCell::parse() end\n"));
  return retval;
}

int DocHtmlCell::parseXml()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlCell::parseXml() start\n"));

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
    if (retval==TK_HTMLTAG)
    {
      int tagId=Mappers::htmlTagMapper->map(g_token->name);
      if (tagId==XML_ITEM && g_token->endTag) // found </item> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
      else if (tagId==XML_DESCRIPTION && g_token->endTag) // found </description> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
    }
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  DBG(("DocHtmlCell::parseXml() end\n"));
  return retval;
}

uint DocHtmlCell::rowSpan() const
{
  for (const auto &attr : attribs())
  {
    if (attr.name.lower()=="rowspan")
    {
      return attr.value.toUInt();
    }
  }
  return 0;
}

uint DocHtmlCell::colSpan() const
{
  for (const auto &attr : attribs())
  {
    if (attr.name.lower()=="colspan")
    {
      return std::max(1u,attr.value.toUInt());
    }
  }
  return 1;
}

DocHtmlCell::Alignment DocHtmlCell::alignment() const
{
  for (const auto &attr : attribs())
  {
    QCString attrName  = attr.name.lower();
    QCString attrValue = attr.value.lower();
    if (attrName=="align")
    {
      if (attrValue=="center")
        return Center;
      else if (attrValue=="right")
        return Right;
      else return Left;
    }
    else if (attrName=="class" && attrValue.startsWith("markdowntable"))
    {
      if (attrValue=="markdowntableheadcenter")
        return Center;
      else if (attrValue=="markdowntableheadright")
        return Right;
      else if (attrValue=="markdowntableheadleft")
        return Left;
      else if (attrValue=="markdowntableheadnone")
        return Center;
      else if (attrValue=="markdowntablebodycenter")
        return Center;
      else if (attrValue=="markdowntablebodyright")
        return Right;
      else if (attrValue=="markdowntablebodyleft")
        return Left;
      else if (attrValue=="markdowntablebodynone")
        return Left;
      else return Left;
    }
  }
  return Left;
}

DocHtmlCell::Valignment DocHtmlCell::valignment() const
{
  for (const auto &attr : attribs())
  {
    QCString attrName  = attr.name.lower();
    QCString attrValue = attr.value.lower();
    if (attrName=="valign")
    {
      if (attrValue=="top")
        return Top;
      else if (attrValue=="bottom")
        return Bottom;
      else if (attrValue=="middle")
        return Middle;
      else return Middle;
    }
  }
  return Middle;
}

//---------------------------------------------------------------------------

int DocHtmlRow::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlRow::parse() start\n"));

  bool isHeading=FALSE;
  bool isFirst=TRUE;
  DocHtmlCell *cell=0;

  // get next token
  int tok=doctokenizerYYlex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=doctokenizerYYlex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(g_token->name);
    if (tagId==HTML_TD && !g_token->endTag) // found <td> tag
    {
    }
    else if (tagId==HTML_TH && !g_token->endTag) // found <th> tag
    {
      isHeading=TRUE;
    }
    else // found some other tag
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <td> or <th> tag but "
          "found <%s> instead!",qPrint(g_token->name));
      doctokenizerYYpushBackHtmlTag(g_token->name);
      goto endrow;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto endrow;
  }
  else // token other than html token
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <td> or <th> tag but found %s token instead!",
        tokToString(tok));
    goto endrow;
  }

  // parse one or more cells
  do
  {
    cell=new DocHtmlCell(this,g_token->attribs,isHeading);
    cell->markFirst(isFirst);
    isFirst=FALSE;
    m_children.push_back(std::unique_ptr<DocHtmlCell>(cell));
    retval=cell->parse();
    isHeading = retval==RetVal_TableHCell;
  }
  while (retval==RetVal_TableCell || retval==RetVal_TableHCell);
  if (cell) cell->markLast(TRUE);

endrow:
  DBG(("DocHtmlRow::parse() end\n"));
  return retval;
}

int DocHtmlRow::parseXml(bool isHeading)
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlRow::parseXml() start\n"));

  bool isFirst=TRUE;
  DocHtmlCell *cell=0;

  // get next token
  int tok=doctokenizerYYlex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=doctokenizerYYlex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(g_token->name);
    if (tagId==XML_TERM && !g_token->endTag) // found <term> tag
    {
    }
    else if (tagId==XML_DESCRIPTION && !g_token->endTag) // found <description> tag
    {
    }
    else // found some other tag
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <term> or <description> tag but "
          "found <%s> instead!",qPrint(g_token->name));
      doctokenizerYYpushBackHtmlTag(g_token->name);
      goto endrow;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto endrow;
  }
  else // token other than html token
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <td> or <th> tag but found %s token instead!",
        tokToString(tok));
    goto endrow;
  }

  do
  {
    cell=new DocHtmlCell(this,g_token->attribs,isHeading);
    cell->markFirst(isFirst);
    isFirst=FALSE;
    m_children.push_back(std::unique_ptr<DocHtmlCell>(cell));
    retval=cell->parseXml();
  }
  while (retval==RetVal_TableCell || retval==RetVal_TableHCell);
  if (cell) cell->markLast(TRUE);

endrow:
  DBG(("DocHtmlRow::parseXml() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlTable::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlTable::parse() start\n"));

getrow:
  // get next token
  int tok=doctokenizerYYlex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=doctokenizerYYlex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(g_token->name);
    if (tagId==HTML_TR && !g_token->endTag) // found <tr> tag
    {
      // no caption, just rows
      retval=RetVal_TableRow;
    }
    else if (tagId==HTML_CAPTION && !g_token->endTag) // found <caption> tag
    {
      if (m_caption)
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"table already has a caption, found another one");
      }
      else
      {
        m_caption = new DocHtmlCaption(this,g_token->attribs);
        retval=m_caption->parse();

        if (retval==RetVal_OK) // caption was parsed ok
        {
          goto getrow;
        }
      }
    }
    else // found wrong token
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <tr> or <caption> tag but "
          "found <%s%s> instead!", g_token->endTag ? "/" : "", qPrint(g_token->name));
    }
  }
  else if (tok==0) // premature end of comment
  {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment while looking"
          " for a <tr> or <caption> tag");
  }
  else // token other than html token
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <tr> tag but found %s token instead!",
        tokToString(tok));
  }

  // parse one or more rows
  while (retval==RetVal_TableRow)
  {
    DocHtmlRow *tr=new DocHtmlRow(this,g_token->attribs);
    m_children.push_back(std::unique_ptr<DocHtmlRow>(tr));
    retval=tr->parse();
  }

  computeTableGrid();

  DBG(("DocHtmlTable::parse() end\n"));
  return retval==RetVal_EndTable ? RetVal_OK : retval;
}

int DocHtmlTable::parseXml()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlTable::parseXml() start\n"));

  // get next token
  int tok=doctokenizerYYlex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=doctokenizerYYlex();
  // should find a html tag now
  int tagId=0;
  bool isHeader=FALSE;
  if (tok==TK_HTMLTAG)
  {
    tagId=Mappers::htmlTagMapper->map(g_token->name);
    if (tagId==XML_ITEM && !g_token->endTag) // found <item> tag
    {
      retval=RetVal_TableRow;
    }
    if (tagId==XML_LISTHEADER && !g_token->endTag) // found <listheader> tag
    {
      retval=RetVal_TableRow;
      isHeader=TRUE;
    }
  }

  // parse one or more rows
  while (retval==RetVal_TableRow)
  {
    DocHtmlRow *tr=new DocHtmlRow(this,g_token->attribs);
    m_children.push_back(std::unique_ptr<DocHtmlRow>(tr));
    retval=tr->parseXml(isHeader);
    isHeader=FALSE;
  }

  computeTableGrid();

  DBG(("DocHtmlTable::parseXml() end\n"));
  tagId=Mappers::htmlTagMapper->map(g_token->name);
  return tagId==XML_LIST && g_token->endTag ? RetVal_OK : retval;
}

/** Helper class to compute the grid for an HTML style table */
struct ActiveRowSpan
{
  ActiveRowSpan(uint rows,uint col) : rowsLeft(rows), column(col) {}
  uint rowsLeft;
  uint column;
};

/** List of ActiveRowSpan classes. */
typedef std::vector<ActiveRowSpan> RowSpanList;

/** determines the location of all cells in a grid, resolving row and
    column spans. For each the total number of visible cells is computed,
    and the total number of visible columns over all rows is stored.
 */
void DocHtmlTable::computeTableGrid()
{
  //printf("computeTableGrid()\n");
  RowSpanList rowSpans;
  uint maxCols=0;
  uint rowIdx=1;
  for (const auto &rowNode : children())
  {
    uint colIdx=1;
    uint cells=0;
    if (rowNode->kind()==DocNode::Kind_HtmlRow)
    {
      size_t i;
      DocHtmlRow *row = (DocHtmlRow*)rowNode.get();
      for (const auto &cellNode : row->children())
      {
        if (cellNode->kind()==DocNode::Kind_HtmlCell)
        {
          DocHtmlCell *cell = (DocHtmlCell*)cellNode.get();
          uint rs = cell->rowSpan();
          uint cs = cell->colSpan();

          for (i=0;i<rowSpans.size();i++)
          {
            if (rowSpans[i].rowsLeft>0 &&
                rowSpans[i].column==colIdx)
            {
              colIdx=rowSpans[i].column+1;
              cells++;
            }
          }
          if (rs>0) rowSpans.emplace_back(rs,colIdx);
          //printf("found cell at (%d,%d)\n",rowIdx,colIdx);
          cell->setRowIndex(rowIdx);
          cell->setColumnIndex(colIdx);
          colIdx+=cs;
          cells++;
        }
      }
      for (i=0;i<rowSpans.size();i++)
      {
        if (rowSpans[i].rowsLeft>0) rowSpans[i].rowsLeft--;
      }
      row->setVisibleCells(cells);
      row->setRowIndex(rowIdx);
      rowIdx++;
    }
    if (colIdx-1>maxCols) maxCols=colIdx-1;
  }
  m_numCols = maxCols;
}

void DocHtmlTable::accept(DocVisitor *v)
{
  v->visitPre(this);
  // for HTML output we put the caption first
  //if (m_caption && v->id()==DocVisitor_Html) m_caption->accept(v);
  // doxygen 1.8.11: always put the caption first
  if (m_caption) m_caption->accept(v);
  for (const auto &n : m_children) n->accept(v);
  // for other output formats we put the caption last
  //if (m_caption && v->id()!=DocVisitor_Html) m_caption->accept(v);
  v->visitPost(this);
}

//---------------------------------------------------------------------------

int DocHtmlDescTitle::parse()
{
  int retval=0;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlDescTitle::parse() start\n"));

  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children))
    {
      const char *cmd_start = "\\";
      switch (tok)
      {
        case TK_COMMAND_AT:
          cmd_start = "@";
        // fall through
        case TK_COMMAND_BS:
          {
            QCString cmdName=g_token->name;
            bool isJavaLink=FALSE;
            switch (Mappers::cmdMapper->map(cmdName))
            {
              case CMD_REF:
                {
                  tok=doctokenizerYYlex();
                  if (tok!=TK_WHITESPACE)
                  {
                    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
                        qPrint(g_token->name));
                  }
                  else
                  {
                    doctokenizerYYsetStateRef();
                    tok=doctokenizerYYlex(); // get the reference id
                    if (tok!=TK_WORD)
                    {
                      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of \\%s command",
                          tokToString(tok),qPrint(cmdName));
                    }
                    else
                    {
                      DocRef *ref = new DocRef(this,g_token->name,g_context);
                      m_children.push_back(std::unique_ptr<DocRef>(ref));
                      ref->parse();
                    }
                    doctokenizerYYsetStatePara();
                  }
                }
                break;
              case CMD_JAVALINK:
                isJavaLink=TRUE;
                // fall through
              case CMD_LINK:
                {
                  tok=doctokenizerYYlex();
                  if (tok!=TK_WHITESPACE)
                  {
                    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
                        qPrint(cmdName));
                  }
                  else
                  {
                    doctokenizerYYsetStateLink();
                    tok=doctokenizerYYlex();
                    if (tok!=TK_WORD)
                    {
                      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of \\%s command",
                          tokToString(tok),qPrint(cmdName));
                    }
                    else
                    {
                      doctokenizerYYsetStatePara();
                      DocLink *lnk = new DocLink(this,g_token->name);
                      m_children.push_back(std::unique_ptr<DocLink>(lnk));
                      QCString leftOver = lnk->parse(isJavaLink);
                      if (!leftOver.isEmpty())
                      {
                        m_children.push_back(std::make_unique<DocWord>(this,leftOver));
                      }
                    }
                  }
                }

                break;
              default:
                warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Illegal command %s found as part of a <dt> tag",
                  qPrint(cmd_start + g_token->name));
            }
          }
          break;
        case TK_SYMBOL:
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unsupported symbol \\%s found as part of a <dt> tag",
              qPrint(g_token->name));
          break;
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(g_token->name);
            if (tagId==HTML_DD && !g_token->endTag) // found <dd> tag
            {
              retval = RetVal_DescData;
              goto endtitle;
            }
            else if (tagId==HTML_DT && g_token->endTag)
            {
              // ignore </dt> tag.
            }
            else if (tagId==HTML_DT)
            {
              // missing <dt> tag.
              retval = RetVal_DescTitle;
              goto endtitle;
            }
            else if (tagId==HTML_DL && g_token->endTag)
            {
              retval=RetVal_EndDesc;
              goto endtitle;
            }
            else if (tagId==HTML_A)
            {
              if (!g_token->endTag)
              {
                handleAHref(this,m_children,g_token->attribs);
              }
            }
            else
            {
              warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected html tag <%s%s> found within <dt> context",
                  g_token->endTag?"/":"",qPrint(g_token->name));
            }
          }
          break;
        default:
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected token %s found as part of a <dt> tag",
              tokToString(tok));
          break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected end of comment while inside"
        " <dt> tag");
  }
endtitle:
  handlePendingStyleCommands(this,m_children);
  DBG(("DocHtmlDescTitle::parse() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlDescData::parse()
{
  m_attribs = g_token->attribs;
  int retval=0;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlDescData::parse() start\n"));

  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  DBG(("DocHtmlDescData::parse() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlDescList::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);
  DBG(("DocHtmlDescList::parse() start\n"));

  // get next token
  int tok=doctokenizerYYlex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=doctokenizerYYlex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(g_token->name);
    if (tagId==HTML_DT && !g_token->endTag) // found <dt> tag
    {
      // continue
    }
    else // found some other tag
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <dt> tag but "
          "found <%s> instead!",qPrint(g_token->name));
      doctokenizerYYpushBackHtmlTag(g_token->name);
      goto enddesclist;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto enddesclist;
  }
  else // token other than html token
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <dt> tag but found %s token instead!",
        tokToString(tok));
    goto enddesclist;
  }

  do
  {
    DocHtmlDescTitle *dt=new DocHtmlDescTitle(this,g_token->attribs);
    m_children.push_back(std::unique_ptr<DocHtmlDescTitle>(dt));
    DocHtmlDescData *dd=new DocHtmlDescData(this);
    m_children.push_back(std::unique_ptr<DocHtmlDescData>(dd));
    retval=dt->parse();
    if (retval==RetVal_DescData)
    {
      retval=dd->parse();
    }
    else if (retval!=RetVal_DescTitle)
    {
      // error
      break;
    }
  } while (retval==RetVal_DescTitle);

  if (retval==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment while inside <dl> block");
  }

enddesclist:

  DBG(("DocHtmlDescList::parse() end\n"));
  return retval==RetVal_EndDesc ? RetVal_OK : retval;
}

//---------------------------------------------------------------------------

int DocHtmlListItem::parse()
{
  DBG(("DocHtmlListItem::parse() start\n"));
  int retval=0;
  auto ns = AutoNodeStack(this);

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  DBG(("DocHtmlListItem::parse() end retval=%x\n",retval));
  return retval;
}

int DocHtmlListItem::parseXml()
{
  DBG(("DocHtmlListItem::parseXml() start\n"));
  int retval=0;
  auto ns = AutoNodeStack(this);

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
    if (retval==0) break;

    //printf("new item: retval=%x g_token->name=%s g_token->endTag=%d\n",
    //    retval,qPrint(g_token->name),g_token->endTag);
    if (retval==RetVal_ListItem)
    {
      break;
    }
  }
  while (retval!=RetVal_CloseXml);

  if (par) par->markLast();

  DBG(("DocHtmlListItem::parseXml() end retval=%x\n",retval));
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlList::parse()
{
  DBG(("DocHtmlList::parse() start\n"));
  int retval=RetVal_OK;
  int num=1;
  auto ns = AutoNodeStack(this);

  // get next token
  int tok=doctokenizerYYlex();
  // skip whitespace and paragraph breaks
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=doctokenizerYYlex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(g_token->name);
    if (tagId==HTML_LI && !g_token->endTag) // found <li> tag
    {
      // ok, we can go on.
    }
    else if (((m_type==Unordered && tagId==HTML_UL) ||
              (m_type==Ordered   && tagId==HTML_OL)
             ) && g_token->endTag
            ) // found empty list
    {
      // add dummy item to obtain valid HTML
      m_children.push_back(std::make_unique<DocHtmlListItem>(this,HtmlAttribList(),1));
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"empty list!");
      retval = RetVal_EndList;
      goto endlist;
    }
    else // found some other tag
    {
      // add dummy item to obtain valid HTML
      m_children.push_back(std::make_unique<DocHtmlListItem>(this,HtmlAttribList(),1));
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <li> tag but "
          "found <%s%s> instead!",g_token->endTag?"/":"",qPrint(g_token->name));
      doctokenizerYYpushBackHtmlTag(g_token->name);
      goto endlist;
    }
  }
  else if (tok==0) // premature end of comment
  {
    // add dummy item to obtain valid HTML
    m_children.push_back(std::make_unique<DocHtmlListItem>(this,HtmlAttribList(),1));
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment while looking"
        " for a html list item");
    goto endlist;
  }
  else // token other than html token
  {
    // add dummy item to obtain valid HTML
    m_children.push_back(std::make_unique<DocHtmlListItem>(this,HtmlAttribList(),1));
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <li> tag but found %s token instead!",
        tokToString(tok));
    goto endlist;
  }

  do
  {
    DocHtmlListItem *li=new DocHtmlListItem(this,g_token->attribs,num++);
    m_children.push_back(std::unique_ptr<DocHtmlListItem>(li));
    retval=li->parse();
  } while (retval==RetVal_ListItem);

  if (retval==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment while inside <%cl> block",
        m_type==Unordered ? 'u' : 'o');
  }

endlist:
  DBG(("DocHtmlList::parse() end retval=%x\n",retval));
  return retval==RetVal_EndList ? RetVal_OK : retval;
}

int DocHtmlList::parseXml()
{
  DBG(("DocHtmlList::parseXml() start\n"));
  int retval=RetVal_OK;
  int num=1;
  auto ns = AutoNodeStack(this);

  // get next token
  int tok=doctokenizerYYlex();
  // skip whitespace and paragraph breaks
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=doctokenizerYYlex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(g_token->name);
    //printf("g_token->name=%s g_token->endTag=%d\n",qPrint(g_token->name),g_token->endTag);
    if (tagId==XML_ITEM && !g_token->endTag) // found <item> tag
    {
      // ok, we can go on.
    }
    else // found some other tag
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <item> tag but "
          "found <%s> instead!",qPrint(g_token->name));
      doctokenizerYYpushBackHtmlTag(g_token->name);
      goto endlist;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment while looking"
        " for a html list item");
    goto endlist;
  }
  else // token other than html token
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected <item> tag but found %s token instead!",
        tokToString(tok));
    goto endlist;
  }

  do
  {
    DocHtmlListItem *li=new DocHtmlListItem(this,g_token->attribs,num++);
    m_children.push_back(std::unique_ptr<DocHtmlListItem>(li));
    retval=li->parseXml();
    if (retval==0) break;
    //printf("retval=%x g_token->name=%s\n",retval,qPrint(g_token->name));
  } while (retval==RetVal_ListItem);

  if (retval==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment while inside <list type=\"%s\"> block",
        m_type==Unordered ? "bullet" : "number");
  }

endlist:
  DBG(("DocHtmlList::parseXml() end retval=%x\n",retval));
  return retval==RetVal_EndList ||
         (retval==RetVal_CloseXml || g_token->name=="list") ?
         RetVal_OK : retval;
}

//--------------------------------------------------------------------------

int DocHtmlBlockQuote::parse()
{
  DBG(("DocHtmlBlockQuote::parse() start\n"));
  int retval=0;
  auto ns = AutoNodeStack(this);

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  DBG(("DocHtmlBlockQuote::parse() end retval=%x\n",retval));
  return (retval==RetVal_EndBlockQuote) ? RetVal_OK : retval;
}

//---------------------------------------------------------------------------

int DocParBlock::parse()
{
  DBG(("DocParBlock::parse() start\n"));
  int retval=0;
  auto ns = AutoNodeStack(this);

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  DBG(("DocParBlock::parse() end retval=%x\n",retval));
  return (retval==RetVal_EndBlockQuote) ? RetVal_OK : retval;
}

//---------------------------------------------------------------------------

int DocSimpleListItem::parse()
{
  auto ns = AutoNodeStack(this);
  int rv=m_paragraph->parse();
  m_paragraph->markFirst();
  m_paragraph->markLast();
  return rv;
}

//--------------------------------------------------------------------------

int DocSimpleList::parse()
{
  auto ns = AutoNodeStack(this);
  int rv;
  do
  {
    DocSimpleListItem *li=new DocSimpleListItem(this);
    m_children.push_back(std::unique_ptr<DocSimpleListItem>(li));
    rv=li->parse();
  } while (rv==RetVal_ListItem);
  return (rv!=TK_NEWPARA) ? rv : RetVal_OK;
}

//--------------------------------------------------------------------------

DocAutoListItem::DocAutoListItem(DocNode *parent,int indent,int num)
      : m_indent(indent), m_itemNum(num)
{
  m_parent = parent;
}

int DocAutoListItem::parse()
{
  int retval = RetVal_OK;
  auto ns = AutoNodeStack(this);

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  do
  {
    DocPara *par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (!par->isEmpty())
    {
      m_children.push_back(std::unique_ptr<DocPara>(par));
      if (lastPar) lastPar->markLast(FALSE);
      lastPar=par;
    }
    else
    {
      delete par;
    }
    // next paragraph should be more indented than the - marker to belong
    // to this item
  } while (retval==TK_NEWPARA && g_token->indent>m_indent);
  if (lastPar) lastPar->markLast();

  //printf("DocAutoListItem: retval=%d indent=%d\n",retval,g_token->indent);
  return retval;
}

//--------------------------------------------------------------------------

DocAutoList::DocAutoList(DocNode *parent,int indent,bool isEnumList,
                         int depth) :
      m_indent(indent), m_isEnumList(isEnumList),
      m_depth(depth)
{
  m_parent = parent;
}

int DocAutoList::parse()
{
  int retval = RetVal_OK;
  int num=1;
  auto ns = AutoNodeStack(this);
  doctokenizerYYstartAutoList();
	  // first item or sub list => create new list
  do
  {
    if (g_token->id!=-1) // explicitly numbered list
    {
      num=g_token->id;  // override num with real number given
    }
    DocAutoListItem *li = new DocAutoListItem(this,m_indent,num++);
    m_children.push_back(std::unique_ptr<DocAutoListItem>(li));
    retval=li->parse();
    //printf("DocAutoList::parse(): retval=0x%x g_token->indent=%d m_indent=%d "
    //       "m_isEnumList=%d g_token->isEnumList=%d g_token->name=%s\n",
    //       retval,g_token->indent,m_indent,m_isEnumList,g_token->isEnumList,
    //       qPrint(g_token->name));
    //printf("num=%d g_token->id=%d\n",num,g_token->id);
  }
  while (retval==TK_LISTITEM &&                // new list item
         m_indent==g_token->indent &&          // at same indent level
	 m_isEnumList==g_token->isEnumList &&  // of the same kind
         (g_token->id==-1 || g_token->id>=num)  // increasing number (or no number)
        );

  doctokenizerYYendAutoList();
  return retval;
}

//--------------------------------------------------------------------------

void DocTitle::parse()
{
  DBG(("DocTitle::parse() start\n"));
  auto ns = AutoNodeStack(this);
  doctokenizerYYsetStateTitle();
  int tok;
  while ((tok=doctokenizerYYlex()))
  {
    if (!defaultHandleToken(this,tok,m_children))
    {
      errorHandleDefaultToken(this,tok,m_children,"title section");
    }
  }
  doctokenizerYYsetStatePara();
  handlePendingStyleCommands(this,m_children);
  DBG(("DocTitle::parse() end\n"));
}

void DocTitle::parseFromString(const QCString &text)
{
  m_children.push_back(std::make_unique<DocWord>(this,text));
}

//--------------------------------------------------------------------------

DocSimpleSect::DocSimpleSect(DocNode *parent,Type t) :
     m_type(t)
{
  m_parent = parent;
  m_title=0;
}

DocSimpleSect::~DocSimpleSect()
{
  delete m_title;
}

void DocSimpleSect::accept(DocVisitor *v)
{
  v->visitPre(this);
  if (m_title) m_title->accept(v);
  for (const auto &n : m_children) n->accept(v);
  v->visitPost(this);
}

int DocSimpleSect::parse(bool userTitle,bool needsSeparator)
{
  DBG(("DocSimpleSect::parse() start\n"));
  auto ns = AutoNodeStack(this);

  // handle case for user defined title
  if (userTitle)
  {
    m_title = new DocTitle(this);
    m_title->parse();
  }

  // add new paragraph as child
  DocPara *par = new DocPara(this);
  if (m_children.empty())
  {
    par->markFirst();
  }
  else
  {
    ASSERT(m_children.back()->kind()==DocNode::Kind_Para);
    ((DocPara *)m_children.back().get())->markLast(FALSE);
  }
  par->markLast();
  if (needsSeparator) m_children.push_back(std::make_unique<DocSimpleSectSep>(this));
  m_children.push_back(std::unique_ptr<DocPara>(par));

  // parse the contents of the paragraph
  int retval = par->parse();

  DBG(("DocSimpleSect::parse() end retval=%d\n",retval));
  return retval; // 0==EOF, TK_NEWPARA, TK_LISTITEM, TK_ENDLIST, RetVal_SimpleSec
}

int DocSimpleSect::parseRcs()
{
  DBG(("DocSimpleSect::parseRcs() start\n"));
  auto ns = AutoNodeStack(this);

  m_title = new DocTitle(this);
  m_title->parseFromString(g_token->name);

  QCString text = g_token->text;
  docParserPushContext(); // this will create a new g_token
  internalValidatingParseDoc(this,m_children,text);
  docParserPopContext(); // this will restore the old g_token

  DBG(("DocSimpleSect::parseRcs()\n"));
  return RetVal_OK;
}

int DocSimpleSect::parseXml()
{
  DBG(("DocSimpleSect::parse() start\n"));
  auto ns = AutoNodeStack(this);

  int retval = RetVal_OK;
  for (;;)
  {
    // add new paragraph as child
    DocPara *par = new DocPara(this);
    if (m_children.empty())
    {
      par->markFirst();
    }
    else
    {
      ASSERT(m_children.back()->kind()==DocNode::Kind_Para);
      ((DocPara *)m_children.back().get())->markLast(FALSE);
    }
    par->markLast();
    m_children.push_back(std::unique_ptr<DocPara>(par));

    // parse the contents of the paragraph
    retval = par->parse();
    if (retval == 0) break;
    if (retval == RetVal_CloseXml)
    {
      retval = RetVal_OK;
      break;
    }
  }

  DBG(("DocSimpleSect::parseXml() end retval=%d\n",retval));
  return retval;
}

void DocSimpleSect::appendLinkWord(const QCString &word)
{
  DocPara *p;
  if (m_children.empty() || m_children.back()->kind()!=DocNode::Kind_Para)
  {
    p = new DocPara(this);
    m_children.push_back(std::unique_ptr<DocPara>(p));
  }
  else
  {
    p = (DocPara *)m_children.back().get();

    // Comma-separate <seealso> links.
    p->injectToken(TK_WORD,",");
    p->injectToken(TK_WHITESPACE," ");
  }

  g_inSeeBlock=TRUE;
  p->injectToken(TK_LNKWORD,word);
  g_inSeeBlock=FALSE;
}

QCString DocSimpleSect::typeString() const
{
  switch (m_type)
  {
    case Unknown:    break;
    case See:        return "see";
    case Return:     return "return";
    case Author:     // fall through
    case Authors:    return "author";
    case Version:    return "version";
    case Since:      return "since";
    case Date:       return "date";
    case Note:       return "note";
    case Warning:    return "warning";
    case Pre:        return "pre";
    case Post:       return "post";
    case Copyright:  return "copyright";
    case Invar:      return "invariant";
    case Remark:     return "remark";
    case Attention:  return "attention";
    case User:       return "user";
    case Rcs:        return "rcs";
  }
  return "unknown";
}

//--------------------------------------------------------------------------

int DocParamList::parse(const QCString &cmdName)
{
  int retval=RetVal_OK;
  DBG(("DocParamList::parse() start\n"));
  auto ns = AutoNodeStack(this);
  DocPara *par=0;
  QCString saveCmdName = cmdName;

  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    retval=RetVal_EndParBlock;
    goto endparamlist;
  }
  doctokenizerYYsetStateParam();
  tok=doctokenizerYYlex();
  while (tok==TK_WORD) /* there is a parameter name */
  {
    if (m_type==DocParamSect::Param)
    {
      int typeSeparator = g_token->name.find('#'); // explicit type position
      if (typeSeparator!=-1)
      {
        handleParameterType(this,m_paramTypes,g_token->name.left(typeSeparator));
        g_token->name = g_token->name.mid(typeSeparator+1);
        g_hasParamCommand=TRUE;
        checkArgumentName(g_token->name.str());
        ((DocParamSect*)parent())->m_hasTypeSpecifier=TRUE;
      }
      else
      {
        g_hasParamCommand=TRUE;
        checkArgumentName(g_token->name.str());
      }
    }
    else if (m_type==DocParamSect::RetVal)
    {
      g_hasReturnCommand=TRUE;
      checkRetvalName(g_token->name);
    }
    //m_params.append(g_token->name);
    handleLinkedWord(this,m_params);
    tok=doctokenizerYYlex();
  }
  doctokenizerYYsetStatePara();
  if (tok==0) /* premature end of comment block */
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(saveCmdName));
    retval=RetVal_EndParBlock;
    goto endparamlist;
  }
  if (tok!=TK_WHITESPACE) /* premature end of comment block */
  {
    if (tok!=TK_NEWPARA) /* empty param description */
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token in comment block while parsing the "
          "argument of command %s",qPrint(saveCmdName));
    }
    retval=RetVal_EndParBlock;
    goto endparamlist;
  }

  par = new DocPara(this);
  m_paragraphs.push_back(std::unique_ptr<DocPara>(par));
  retval = par->parse();
  par->markFirst();
  par->markLast();

endparamlist:
  DBG(("DocParamList::parse() end retval=%d\n",retval));
  return retval;
}

int DocParamList::parseXml(const QCString &paramName)
{
  int retval=RetVal_OK;
  DBG(("DocParamList::parseXml() start\n"));
  auto ns = AutoNodeStack(this);

  g_token->name = paramName;
  if (m_type==DocParamSect::Param)
  {
    g_hasParamCommand=TRUE;
    checkArgumentName(g_token->name.str());
  }
  else if (m_type==DocParamSect::RetVal)
  {
    g_hasReturnCommand=TRUE;
    checkRetvalName(g_token->name);
  }

  handleLinkedWord(this,m_params);

  do
  {
    DocPara *par = new DocPara(this);
    retval = par->parse();
    if (par->isEmpty()) // avoid adding an empty paragraph for the whitespace
                        // after </para> and before </param>
    {
      delete par;
      break;
    }
    else // append the paragraph to the list
    {
      if (m_paragraphs.empty())
      {
        par->markFirst();
      }
      else
      {
        m_paragraphs.back()->markLast(FALSE);
      }
      par->markLast();
      m_paragraphs.push_back(std::unique_ptr<DocPara>(par));
    }

    if (retval == 0) break;

  } while (retval==RetVal_CloseXml &&
           Mappers::htmlTagMapper->map(g_token->name)!=XML_PARAM &&
           Mappers::htmlTagMapper->map(g_token->name)!=XML_TYPEPARAM &&
           Mappers::htmlTagMapper->map(g_token->name)!=XML_EXCEPTION);


  if (retval==0) /* premature end of comment block */
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unterminated param or exception tag");
  }
  else
  {
    retval=RetVal_OK;
  }


  DBG(("DocParamList::parse() end retval=%d\n",retval));
  return retval;
}

//--------------------------------------------------------------------------

int DocParamSect::parse(const QCString &cmdName,bool xmlContext, Direction d)
{
  int retval=RetVal_OK;
  DBG(("DocParamSect::parse() start\n"));
  auto ns = AutoNodeStack(this);

  if (d!=Unspecified)
  {
    m_hasInOutSpecifier=TRUE;
  }

  DocParamList *pl = new DocParamList(this,m_type,d);
  if (m_children.empty())
  {
    pl->markFirst();
    pl->markLast();
  }
  else
  {
    ASSERT(m_children.back()->kind()==DocNode::Kind_ParamList);
    ((DocParamList *)m_children.back().get())->markLast(FALSE);
    pl->markLast();
  }
  m_children.push_back(std::unique_ptr<DocParamList>(pl));
  if (xmlContext)
  {
    retval = pl->parseXml(cmdName);
  }
  else
  {
    retval = pl->parse(cmdName);
  }
  if (retval==RetVal_EndParBlock)
  {
    retval = RetVal_OK;
  }

  DBG(("DocParamSect::parse() end retval=%d\n",retval));
  return retval;
}

//--------------------------------------------------------------------------

int DocPara::handleSimpleSection(DocSimpleSect::Type t, bool xmlContext)
{
  DocSimpleSect *ss=0;
  bool needsSeparator = FALSE;
  if (!m_children.empty() &&                           // previous element
      m_children.back()->kind()==Kind_SimpleSect &&      // was a simple sect
      ((DocSimpleSect *)m_children.back().get())->type()==t && // of same type
      t!=DocSimpleSect::User)                            // but not user defined
  {
    // append to previous section
    ss=(DocSimpleSect *)m_children.back().get();
    needsSeparator = TRUE;
  }
  else // start new section
  {
    ss=new DocSimpleSect(this,t);
    m_children.push_back(std::unique_ptr<DocSimpleSect>(ss));
  }
  int rv = RetVal_OK;
  if (xmlContext)
  {
    return ss->parseXml();
  }
  else
  {
    rv = ss->parse(t==DocSimpleSect::User,needsSeparator);
  }
  return (rv!=TK_NEWPARA) ? rv : RetVal_OK;
}

int DocPara::handleParamSection(const QCString &cmdName,
                                DocParamSect::Type t,
                                bool xmlContext=FALSE,
                                int direction=DocParamSect::Unspecified)
{
  DocParamSect *ps=0;
  if (!m_children.empty() &&                        // previous element
      m_children.back()->kind()==Kind_ParamSect &&    // was a param sect
      ((DocParamSect *)m_children.back().get())->type()==t) // of same type
  {
    // append to previous section
    ps=(DocParamSect *)m_children.back().get();
  }
  else // start new section
  {
    ps=new DocParamSect(this,t);
    m_children.push_back(std::unique_ptr<DocParamSect>(ps));
  }
  int rv=ps->parse(cmdName,xmlContext,(DocParamSect::Direction)direction);
  return (rv!=TK_NEWPARA) ? rv : RetVal_OK;
}

void DocPara::handleCite()
{
  // get the argument of the cite command.
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint("cite"));
    return;
  }
  doctokenizerYYsetStateCite();
  tok=doctokenizerYYlex();
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s\n", qPrint("cite"));
    return;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint("cite"));
    return;
  }
  g_token->sectionId = g_token->name;
  m_children.push_back(
      std::make_unique<DocCite>(
        this,g_token->name,g_context));

  doctokenizerYYsetStatePara();
}

void DocPara::handleEmoji()
{
  // get the argument of the emoji command.
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint("emoji"));
    return;
  }
  doctokenizerYYsetStateEmoji();
  tok=doctokenizerYYlex();
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s\n", qPrint("emoji"));
    return;
  }
  else if (tok!=TK_WORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint("emoji"));
    return;
  }
  m_children.push_back(
      std::make_unique<DocEmoji>(
         this,g_token->name));
  doctokenizerYYsetStatePara();
}

int DocPara::handleXRefItem()
{
  int retval=doctokenizerYYlex();
  ASSERT(retval==TK_WHITESPACE);
  doctokenizerYYsetStateXRefItem();
  retval=doctokenizerYYlex();
  if (retval==RetVal_OK)
  {
    DocXRefItem *ref = new DocXRefItem(this,g_token->id,g_token->name);
    if (ref->parse())
    {
      m_children.push_back(std::unique_ptr<DocXRefItem>(ref));
    }
    else
    {
      delete ref;
    }
  }
  doctokenizerYYsetStatePara();
  return retval;
}

void DocPara::handleIline()
{
  doctokenizerYYsetStateIline();
  int retval=doctokenizerYYlex();
  doctokenizerYYsetStatePara();
}

void DocPara::handleIncludeOperator(const QCString &cmdName,DocIncOperator::Type t)
{
  QCString saveCmdName = cmdName;
  DBG(("handleIncludeOperator(%s)\n",qPrint(saveCmdName)));
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  doctokenizerYYsetStatePattern();
  tok=doctokenizerYYlex();
  doctokenizerYYsetStatePara();
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s", qPrint(saveCmdName));
    return;
  }
  else if (tok!=TK_WORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint(saveCmdName));
    return;
  }
  DocIncOperator *op = new DocIncOperator(this,t,g_token->name,g_context,g_isExample,g_exampleName);
  DocNode *n1 = m_children.size()>=1 ? m_children.at(m_children.size()-1).get() : 0;
  DocNode *n2 = m_children.size()>=2 ? m_children.at(m_children.size()-2).get() : 0;
  bool isFirst = n1==0 || // no last node
                 (n1->kind()!=DocNode::Kind_IncOperator &&
                  n1->kind()!=DocNode::Kind_WhiteSpace
                 ) || // last node is not operator or whitespace
                 (n1->kind()==DocNode::Kind_WhiteSpace &&
                  n2!=0 && n2->kind()!=DocNode::Kind_IncOperator
                 ); // previous not is not operator
  op->markFirst(isFirst);
  op->markLast(TRUE);
  if (n1!=0 && n1->kind()==DocNode::Kind_IncOperator)
  {
    ((DocIncOperator *)n1)->markLast(FALSE);
  }
  else if (n1!=0 && n1->kind()==DocNode::Kind_WhiteSpace &&
           n2!=0 && n2->kind()==DocNode::Kind_IncOperator
          )
  {
    ((DocIncOperator *)n2)->markLast(FALSE);
  }
  m_children.push_back(std::unique_ptr<DocIncOperator>(op));
  op->parse();
}

void DocPara::handleImage(const QCString &cmdName)
{
  QCString saveCmdName = cmdName;
  bool inlineImage = FALSE;

  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    if (tok==TK_WORD)
    {
      if (g_token->name == "{")
      {
        while ((tok=doctokenizerYYlex())==TK_WHITESPACE);
        if (g_token->name != "}") // non-empty option string
	{
          if (g_token->name.lower() != "inline")
          {
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"currently only 'inline' supported as option of %s command",
              qPrint(saveCmdName));
          }
          else
          {
            inlineImage = TRUE;
          }
          while ((tok=doctokenizerYYlex())==TK_WHITESPACE);
	}
        if (!((tok==TK_WORD) && (g_token->name == "}")))
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected closing '}' at option of %s command",
            qPrint(saveCmdName));
          return;
        }
        tok=doctokenizerYYlex();
        if (tok!=TK_WHITESPACE)
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command with option",
            qPrint(saveCmdName));
          return;
        }
      }
    }
    else
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
      return;
    }
  }
  tok=doctokenizerYYlex();
  if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint(saveCmdName));
    return;
  }
  tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  DocImage::Type t;
  QCString imgType = g_token->name.lower();
  if      (imgType=="html")    t=DocImage::Html;
  else if (imgType=="latex")   t=DocImage::Latex;
  else if (imgType=="docbook") t=DocImage::DocBook;
  else if (imgType=="rtf")     t=DocImage::Rtf;
  else
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"output format %s specified as the first argument of "
        "%s command is not valid",
        qPrint(imgType),qPrint(saveCmdName));
    return;
  }
  doctokenizerYYsetStateFile();
  tok=doctokenizerYYlex();
  doctokenizerYYsetStatePara();
  if (tok!=TK_WORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint(saveCmdName));
    return;
  }
  HtmlAttribList attrList;
  DocImage *img = new DocImage(this,attrList,findAndCopyImage(g_token->name,t),t,"",inlineImage);
  m_children.push_back(std::unique_ptr<DocImage>(img));
  img->parse();
}

template<class T>
void DocPara::handleFile(const QCString &cmdName)
{
  QCString saveCmdName = cmdName;
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  doctokenizerYYsetStateFile();
  tok=doctokenizerYYlex();
  doctokenizerYYsetStatePara();
  if (tok!=TK_WORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint(saveCmdName));
    return;
  }
  QCString name = g_token->name;
  T *df = new T(this,name,g_context,g_fileName,getDoctokinizerLineNr());
  if (df->parse())
  {
    m_children.push_back(std::unique_ptr<T>(df));
  }
  else
  {
    delete df;
  }
}

void DocPara::handleVhdlFlow()
{
  DocVhdlFlow *vf = new DocVhdlFlow(this);
  m_children.push_back(std::unique_ptr<DocVhdlFlow>(vf));
  vf->parse();
}

void DocPara::handleLink(const QCString &cmdName,bool isJavaLink)
{
  QCString saveCmdName = cmdName;
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  doctokenizerYYsetStateLink();
  tok=doctokenizerYYlex();
  if (tok!=TK_WORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"%s as the argument of %s",
        tokToString(tok),qPrint(saveCmdName));
    return;
  }
  doctokenizerYYsetStatePara();
  DocLink *lnk = new DocLink(this,g_token->name);
  m_children.push_back(std::unique_ptr<DocLink>(lnk));
  QCString leftOver = lnk->parse(isJavaLink);
  if (!leftOver.isEmpty())
  {
    m_children.push_back(std::make_unique<DocWord>(this,leftOver));
  }
}

void DocPara::handleRef(const QCString &cmdName)
{
  QCString saveCmdName = cmdName;
  DBG(("handleRef(%s)\n",qPrint(saveCmdName)));
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  doctokenizerYYsetStateRef();
  tok=doctokenizerYYlex(); // get the reference id
  DocRef *ref=0;
  if (tok!=TK_WORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint(saveCmdName));
    goto endref;
  }
  ref = new DocRef(this,g_token->name,g_context);
  m_children.push_back(std::unique_ptr<DocRef>(ref));
  ref->parse();
endref:
  doctokenizerYYsetStatePara();
}

void DocPara::handleInclude(const QCString &cmdName,DocInclude::Type t)
{
  DBG(("handleInclude(%s)\n",qPrint(cmdName)));
  QCString saveCmdName = cmdName;
  int tok=doctokenizerYYlex();
  bool isBlock = false;
  if (tok==TK_WORD && g_token->name=="{")
  {
    doctokenizerYYsetStateOptions();
    tok=doctokenizerYYlex();
    doctokenizerYYsetStatePara();
    StringVector optList=split(g_token->name.str(),",");
    auto contains = [&optList](const char *kw)
    {
      return std::find(optList.begin(),optList.end(),kw)!=optList.end();
    };
    if (t==DocInclude::Include && contains("lineno"))
    {
      t = DocInclude::IncWithLines;
    }
    else if (t==DocInclude::Snippet && contains("lineno"))
    {
      t = DocInclude::SnipWithLines;
    }
    else if (t==DocInclude::DontInclude && contains("lineno"))
    {
      t = DocInclude::DontIncWithLines;
    }
    else if (t==DocInclude::Include && contains("doc"))
    {
      t = DocInclude::IncludeDoc;
    }
    else if (t==DocInclude::Snippet && contains("doc"))
    {
      t = DocInclude::SnippetDoc;
    }
    tok=doctokenizerYYlex();
  }
  else if (tok==TK_WORD && g_token->name=="[")
  {
    doctokenizerYYsetStateBlock();
    tok=doctokenizerYYlex();
    isBlock = (g_token->name.stripWhiteSpace() == "block");
    doctokenizerYYsetStatePara();
    tok=doctokenizerYYlex();
  }
  else if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  doctokenizerYYsetStateFile();
  tok=doctokenizerYYlex();
  doctokenizerYYsetStatePara();
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(saveCmdName));
    return;
  }
  else if (tok!=TK_WORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint(saveCmdName));
    return;
  }
  QCString fileName = g_token->name;
  QCString blockId;
  if (t==DocInclude::Snippet || t==DocInclude::SnipWithLines || t==DocInclude::SnippetDoc)
  {
    if (fileName == "this") fileName=g_fileName;
    doctokenizerYYsetStateSnippet();
    tok=doctokenizerYYlex();
    doctokenizerYYsetStatePara();
    if (tok!=TK_WORD)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected block identifier, but found token %s instead while parsing the %s command",
          tokToString(tok),qPrint(saveCmdName));
      return;
    }
    blockId = "["+g_token->name+"]";
  }

  // This is the only place to handle the \includedoc and \snippetdoc commands,
  // as the content is included here as if it is really here.
  if (t==DocInclude::IncludeDoc || t==DocInclude::SnippetDoc)
  {
     QCString inc_text;
     int inc_line  = 1;
     readTextFileByName(fileName,inc_text);
     if (t==DocInclude::SnippetDoc)
     {
       inc_line = lineBlock(inc_text, blockId);
       inc_text = extractBlock(inc_text, blockId);
     }

     Markdown markdown(fileName,inc_line);
     QCString strippedDoc = stripIndentation(inc_text);
     QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(strippedDoc,inc_line) : strippedDoc;

     docParserPushContext();
     g_fileName = fileName;
     setDoctokinizerLineNr(inc_line);
     internalValidatingParseDoc(this,m_children,processedDoc);
     docParserPopContext();
  }
  else
  {
    DocInclude *inc = new DocInclude(this,fileName,g_context,t,g_isExample,g_exampleName,blockId,isBlock);
    m_children.push_back(std::unique_ptr<DocInclude>(inc));
    inc->parse();
  }
}

void DocPara::handleSection(const QCString &cmdName)
{
  QCString saveCmdName = cmdName;
  // get the argument of the section command.
  int tok=doctokenizerYYlex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  tok=doctokenizerYYlex();
  if (tok==0)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s\n", qPrint(saveCmdName));
    return;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected token %s as the argument of %s",
        tokToString(tok),qPrint(saveCmdName));
    return;
  }
  g_token->sectionId = g_token->name;
  doctokenizerYYsetStateSkipTitle();
  doctokenizerYYlex();
  doctokenizerYYsetStatePara();
}

int DocPara::handleHtmlHeader(const HtmlAttribList &tagHtmlAttribs,int level)
{
  DocHtmlHeader *header = new DocHtmlHeader(this,tagHtmlAttribs,level);
  m_children.push_back(std::unique_ptr<DocHtmlHeader>(header));
  int retval = header->parse();
  return (retval==RetVal_OK) ? TK_NEWPARA : retval;
}

// For XML tags whose content is stored in attributes rather than
// contained within the element, we need a way to inject the attribute
// text into the current paragraph.
bool DocPara::injectToken(int tok,const QCString &tokText)
{
  g_token->name = tokText;
  return defaultHandleToken(this,tok,m_children);
}

int DocPara::handleStartCode()
{
  int retval = doctokenizerYYlex();
  QCString lang = g_token->name;
  if (!lang.isEmpty() && lang.at(0)!='.')
  {
    lang="."+lang;
  }
  if (g_xmlComment)
  {
    g_token->verb = substitute(substitute(g_token->verb,"&lt;","<"),"&gt;",">");
  }
  // search for the first non-whitespace line, index is stored in li
  int i=0,li=0,l=g_token->verb.length();
  while (i<l && (g_token->verb.at(i)==' ' || g_token->verb.at(i)=='\n'))
  {
    if (g_token->verb.at(i)=='\n') li=i+1;
    i++;
  }
  m_children.push_back(std::make_unique<DocVerbatim>(this,g_context,stripIndentation(g_token->verb.mid(li)),DocVerbatim::Code,g_isExample,g_exampleName,FALSE,lang));
  if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"code section ended without end marker");
  doctokenizerYYsetStatePara();
  return retval;
}

void DocPara::handleInheritDoc()
{
  if (g_memberDef) // inheriting docs from a member
  {
    const MemberDef *reMd = g_memberDef->reimplements();
    if (reMd) // member from which was inherited.
    {
      const MemberDef *thisMd = g_memberDef;
      //printf("{InheritDocs:%s=>%s}\n",qPrint(g_memberDef->qualifiedName()),qPrint(reMd->qualifiedName()));
      docParserPushContext();
      g_scope=reMd->getOuterScope();
      if (g_scope!=Doxygen::globalScope)
      {
        g_context=g_scope->name();
      }
      g_memberDef=reMd;
      while (!g_styleStack.empty()) g_styleStack.pop();
      while (!g_nodeStack.empty()) g_nodeStack.pop();
      g_copyStack.push_back(reMd);
      internalValidatingParseDoc(this,m_children,reMd->briefDescription());
      internalValidatingParseDoc(this,m_children,reMd->documentation());
      g_copyStack.pop_back();
      docParserPopContext(TRUE);
      g_memberDef = thisMd;
    }
  }
}


int DocPara::handleCommand(const QCString &cmdName, const int tok)
{
  DBG(("handleCommand(%s)\n",qPrint(cmdName)));
  int retval = RetVal_OK;
  int cmdId = Mappers::cmdMapper->map(cmdName);
  switch (cmdId)
  {
    case CMD_UNKNOWN:
      m_children.push_back(std::make_unique<DocWord>(this,TK_COMMAND_CHAR(tok) + cmdName));
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Found unknown command '%s%s'",TK_COMMAND_CHAR(tok),qPrint(cmdName));
      break;
    case CMD_EMPHASIS:
      m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Italic,cmdName,TRUE));
      retval=handleStyleArgument(this,m_children,cmdName);
      m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Italic,cmdName,FALSE));
      if (retval!=TK_WORD) m_children.push_back(std::make_unique<DocWhiteSpace>(this," "));
      break;
    case CMD_BOLD:
      m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Bold,cmdName,TRUE));
      retval=handleStyleArgument(this,m_children,cmdName);
      m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Bold,cmdName,FALSE));
      if (retval!=TK_WORD) m_children.push_back(std::make_unique<DocWhiteSpace>(this," "));
      break;
    case CMD_CODE:
      m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Code,cmdName,TRUE));
      retval=handleStyleArgument(this,m_children,cmdName);
      m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Code,cmdName,FALSE));
      if (retval!=TK_WORD) m_children.push_back(std::make_unique<DocWhiteSpace>(this," "));
      break;
    case CMD_BSLASH:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_BSlash));
      break;
    case CMD_AT:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_At));
      break;
    case CMD_LESS:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Less));
      break;
    case CMD_GREATER:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Greater));
      break;
    case CMD_AMP:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Amp));
      break;
    case CMD_DOLLAR:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Dollar));
      break;
    case CMD_HASH:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Hash));
      break;
    case CMD_PIPE:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Pipe));
      break;
    case CMD_DCOLON:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_DoubleColon));
      break;
    case CMD_PERCENT:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Percent));
      break;
    case CMD_NDASH:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
      break;
    case CMD_MDASH:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
      break;
    case CMD_QUOTE:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Quot));
      break;
    case CMD_PUNT:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Dot));
      break;
    case CMD_PLUS:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Plus));
      break;
    case CMD_MINUS:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
      break;
    case CMD_EQUAL:
      m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Equal));
      break;
    case CMD_SA:
      g_inSeeBlock=TRUE;
      retval = handleSimpleSection(DocSimpleSect::See);
      g_inSeeBlock=FALSE;
      break;
    case CMD_RETURN:
      retval = handleSimpleSection(DocSimpleSect::Return);
      g_hasReturnCommand=TRUE;
      break;
    case CMD_AUTHOR:
      retval = handleSimpleSection(DocSimpleSect::Author);
      break;
    case CMD_AUTHORS:
      retval = handleSimpleSection(DocSimpleSect::Authors);
      break;
    case CMD_VERSION:
      retval = handleSimpleSection(DocSimpleSect::Version);
      break;
    case CMD_SINCE:
      retval = handleSimpleSection(DocSimpleSect::Since);
      break;
    case CMD_DATE:
      retval = handleSimpleSection(DocSimpleSect::Date);
      break;
    case CMD_NOTE:
      retval = handleSimpleSection(DocSimpleSect::Note);
      break;
    case CMD_WARNING:
      retval = handleSimpleSection(DocSimpleSect::Warning);
      break;
    case CMD_PRE:
      retval = handleSimpleSection(DocSimpleSect::Pre);
      break;
    case CMD_POST:
      retval = handleSimpleSection(DocSimpleSect::Post);
      break;
    case CMD_COPYRIGHT:
      retval = handleSimpleSection(DocSimpleSect::Copyright);
      break;
    case CMD_INVARIANT:
      retval = handleSimpleSection(DocSimpleSect::Invar);
      break;
    case CMD_REMARK:
      retval = handleSimpleSection(DocSimpleSect::Remark);
      break;
    case CMD_ATTENTION:
      retval = handleSimpleSection(DocSimpleSect::Attention);
      break;
    case CMD_PAR:
      retval = handleSimpleSection(DocSimpleSect::User);
      break;
    case CMD_LI:
      {
        DocSimpleList *sl=new DocSimpleList(this);
        m_children.push_back(std::unique_ptr<DocSimpleList>(sl));
        retval = sl->parse();
      }
      break;
    case CMD_SECTION:
      {
        handleSection(cmdName);
        retval = RetVal_Section;
      }
      break;
    case CMD_SUBSECTION:
      {
        handleSection(cmdName);
        retval = RetVal_Subsection;
      }
      break;
    case CMD_SUBSUBSECTION:
      {
        handleSection(cmdName);
        retval = RetVal_Subsubsection;
      }
      break;
    case CMD_PARAGRAPH:
      {
        handleSection(cmdName);
        retval = RetVal_Paragraph;
      }
      break;
    case CMD_STARTCODE:
      {
        doctokenizerYYsetStateCode();
        retval = handleStartCode();
      }
      break;
    case CMD_HTMLONLY:
      {
        doctokenizerYYsetStateHtmlOnly();
        retval = doctokenizerYYlex();
        m_children.push_back(std::make_unique<DocVerbatim>(this,g_context,g_token->verb,DocVerbatim::HtmlOnly,g_isExample,g_exampleName,g_token->name=="block"));
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"htmlonly section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_MANONLY:
      {
        doctokenizerYYsetStateManOnly();
        retval = doctokenizerYYlex();
        m_children.push_back(std::make_unique<DocVerbatim>(this,g_context,g_token->verb,DocVerbatim::ManOnly,g_isExample,g_exampleName));
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"manonly section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_RTFONLY:
      {
        doctokenizerYYsetStateRtfOnly();
        retval = doctokenizerYYlex();
        m_children.push_back(std::make_unique<DocVerbatim>(this,g_context,g_token->verb,DocVerbatim::RtfOnly,g_isExample,g_exampleName));
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"rtfonly section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_LATEXONLY:
      {
        doctokenizerYYsetStateLatexOnly();
        retval = doctokenizerYYlex();
        m_children.push_back(std::make_unique<DocVerbatim>(this,g_context,g_token->verb,DocVerbatim::LatexOnly,g_isExample,g_exampleName));
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"latexonly section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_XMLONLY:
      {
        doctokenizerYYsetStateXmlOnly();
        retval = doctokenizerYYlex();
        m_children.push_back(std::make_unique<DocVerbatim>(this,g_context,g_token->verb,DocVerbatim::XmlOnly,g_isExample,g_exampleName));
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"xmlonly section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_DBONLY:
      {
        doctokenizerYYsetStateDbOnly();
        retval = doctokenizerYYlex();
        m_children.push_back(std::make_unique<DocVerbatim>(this,g_context,g_token->verb,DocVerbatim::DocbookOnly,g_isExample,g_exampleName));
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"docbookonly section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_VERBATIM:
      {
        doctokenizerYYsetStateVerbatim();
        retval = doctokenizerYYlex();
        m_children.push_back(std::make_unique<DocVerbatim>(this,g_context,g_token->verb,DocVerbatim::Verbatim,g_isExample,g_exampleName));
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"verbatim section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_DOT:
      {
        DocVerbatim *dv = new DocVerbatim(this,g_context,g_token->verb,DocVerbatim::Dot,g_isExample,g_exampleName);
        doctokenizerYYsetStatePara();
        QCString width,height;
        defaultHandleTitleAndSize(CMD_DOT,dv,dv->children(),width,height);
        doctokenizerYYsetStateDot();
        retval = doctokenizerYYlex();
        dv->setText(g_token->verb);
        dv->setWidth(width);
        dv->setHeight(height);
        dv->setLocation(g_fileName,getDoctokinizerLineNr());
        m_children.push_back(std::unique_ptr<DocVerbatim>(dv));
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"dot section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_MSC:
      {
        DocVerbatim *dv = new DocVerbatim(this,g_context,g_token->verb,DocVerbatim::Msc,g_isExample,g_exampleName);
        doctokenizerYYsetStatePara();
        QCString width,height;
        defaultHandleTitleAndSize(CMD_MSC,dv,dv->children(),width,height);
        doctokenizerYYsetStateMsc();
        retval = doctokenizerYYlex();
        dv->setText(g_token->verb);
        dv->setWidth(width);
        dv->setHeight(height);
        dv->setLocation(g_fileName,getDoctokinizerLineNr());
        m_children.push_back(std::unique_ptr<DocVerbatim>(dv));
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"msc section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_STARTUML:
      {
        static QCString jarPath = Config_getString(PLANTUML_JAR_PATH);
        doctokenizerYYsetStatePlantUMLOpt();
        retval = doctokenizerYYlex();

        QCString fullMatch = g_token->sectionId;
        QCString sectionId = "";
        int idx = fullMatch.find('{');
        int idxEnd = fullMatch.find("}",idx+1);
        StringVector optList;
        QCString engine;
        if (idx != -1) // options present
        {
           QCString optStr = fullMatch.mid(idx+1,idxEnd-idx-1).stripWhiteSpace();
           optList = split(optStr.str(),",");
           for (const auto &opt : optList)
           {
             if (opt.empty()) continue;
             bool found = false;
             QCString locOpt(opt);
             locOpt = locOpt.stripWhiteSpace().lower();
             if (g_plantumlEngine.find(locOpt)!=g_plantumlEngine.end())
             {
               if (!engine.isEmpty())
               {
                 warn(g_fileName,getDoctokinizerLineNr(), "Multiple definition of engine for '\\startuml'");
               }
               engine = locOpt;
               found = true;
             }
             if (!found)
             {
               if (sectionId.isEmpty())
               {
                 sectionId = opt;
               }
               else
               {
                 warn(g_fileName,getDoctokinizerLineNr(),"Multiple use of of filename for '\\startuml'");
               }
             }
           }
        }
        else
        {
          sectionId = g_token->sectionId;
        }
        if (engine.isEmpty()) engine = "uml";

        if (sectionId.isEmpty())
        {
          doctokenizerYYsetStatePlantUMLOpt();
          retval = doctokenizerYYlex();

          sectionId = g_token->sectionId;
          sectionId = sectionId.stripWhiteSpace();
        }

        QCString plantFile(sectionId);
        DocVerbatim *dv = new DocVerbatim(this,g_context,g_token->verb,DocVerbatim::PlantUML,FALSE,plantFile);
        dv->setEngine(engine);
        doctokenizerYYsetStatePara();
        QCString width,height;
        defaultHandleTitleAndSize(CMD_STARTUML,dv,dv->children(),width,height);
        doctokenizerYYsetStatePlantUML();
        retval = doctokenizerYYlex();
        int line=0;
        dv->setText(stripLeadingAndTrailingEmptyLines(g_token->verb,line));
        dv->setWidth(width);
        dv->setHeight(height);
        dv->setLocation(g_fileName,getDoctokinizerLineNr());
        if (jarPath.isEmpty())
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"ignoring \\startuml command because PLANTUML_JAR_PATH is not set");
          delete dv;
        }
        else
        {
          m_children.push_back(std::unique_ptr<DocVerbatim>(dv));
        }
        if (retval==0) warn_doc_error(g_fileName,getDoctokinizerLineNr(),"startuml section ended without end marker");
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_ENDPARBLOCK:
      retval=RetVal_EndParBlock;
      break;
    case CMD_ENDCODE:
    case CMD_ENDHTMLONLY:
    case CMD_ENDMANONLY:
    case CMD_ENDRTFONLY:
    case CMD_ENDLATEXONLY:
    case CMD_ENDXMLONLY:
    case CMD_ENDDBONLY:
    case CMD_ENDLINK:
    case CMD_ENDVERBATIM:
    case CMD_ENDDOT:
    case CMD_ENDMSC:
    case CMD_ENDUML:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected command %s",qPrint(g_token->name));
      break;
    case CMD_PARAM:
      retval = handleParamSection(cmdName,DocParamSect::Param,FALSE,g_token->paramDir);
      break;
    case CMD_TPARAM:
      retval = handleParamSection(cmdName,DocParamSect::TemplateParam,FALSE,g_token->paramDir);
      break;
    case CMD_RETVAL:
      retval = handleParamSection(cmdName,DocParamSect::RetVal);
      break;
    case CMD_EXCEPTION:
      retval = handleParamSection(cmdName,DocParamSect::Exception);
      break;
    case CMD_XREFITEM:
      retval = handleXRefItem();
      break;
    case CMD_LINEBREAK:
      {
        m_children.push_back(std::make_unique<DocLineBreak>(this));
      }
      break;
    case CMD_ANCHOR:
      {
        DocAnchor *anchor = handleAnchor(this);
        if (anchor)
        {
          m_children.push_back(std::unique_ptr<DocAnchor>(anchor));
        }
      }
      break;
    case CMD_ADDINDEX:
      {
        DocIndexEntry *ie = new DocIndexEntry(this,
                     g_scope!=Doxygen::globalScope?g_scope:0,
                     g_memberDef);
        m_children.push_back(std::unique_ptr<DocIndexEntry>(ie));
        retval = ie->parse();
      }
      break;
    case CMD_INTERNAL:
      retval = RetVal_Internal;
      break;
    case CMD_ENDINTERNAL:
      retval = RetVal_EndInternal;
      break;
    case CMD_PARBLOCK:
      {
        DocParBlock *block = new DocParBlock(this);
        m_children.push_back(std::unique_ptr<DocParBlock>(block));
        retval = block->parse();
      }
      break;
    case CMD_COPYDOC:   // fall through
    case CMD_COPYBRIEF: // fall through
    case CMD_COPYDETAILS:
      //retval = RetVal_CopyDoc;
      // these commands should already be resolved by processCopyDoc()
      break;
    case CMD_INCLUDE:
      handleInclude(cmdName,DocInclude::Include);
      break;
    case CMD_INCWITHLINES:
      handleInclude(cmdName,DocInclude::IncWithLines);
      break;
    case CMD_DONTINCLUDE:
      handleInclude(cmdName,DocInclude::DontInclude);
      break;
    case CMD_HTMLINCLUDE:
      handleInclude(cmdName,DocInclude::HtmlInclude);
      break;
    case CMD_LATEXINCLUDE:
      handleInclude(cmdName,DocInclude::LatexInclude);
      break;
    case CMD_RTFINCLUDE:
      handleInclude(cmdName,DocInclude::RtfInclude);
      break;
    case CMD_MANINCLUDE:
      handleInclude(cmdName,DocInclude::ManInclude);
      break;
    case CMD_XMLINCLUDE:
      handleInclude(cmdName,DocInclude::XmlInclude);
      break;
    case CMD_DOCBOOKINCLUDE:
      handleInclude(cmdName,DocInclude::DocbookInclude);
      break;
    case CMD_VERBINCLUDE:
      handleInclude(cmdName,DocInclude::VerbInclude);
      break;
    case CMD_SNIPPET:
      handleInclude(cmdName,DocInclude::Snippet);
      break;
    case CMD_SNIPWITHLINES:
      handleInclude(cmdName,DocInclude::SnipWithLines);
      break;
    case CMD_INCLUDEDOC:
      handleInclude(cmdName,DocInclude::IncludeDoc);
      break;
    case CMD_SNIPPETDOC:
      handleInclude(cmdName,DocInclude::SnippetDoc);
      break;
    case CMD_SKIP:
      handleIncludeOperator(cmdName,DocIncOperator::Skip);
      break;
    case CMD_UNTIL:
      handleIncludeOperator(cmdName,DocIncOperator::Until);
      break;
    case CMD_SKIPLINE:
      handleIncludeOperator(cmdName,DocIncOperator::SkipLine);
      break;
    case CMD_LINE:
      handleIncludeOperator(cmdName,DocIncOperator::Line);
      break;
    case CMD_IMAGE:
      handleImage(cmdName);
      break;
    case CMD_DOTFILE:
      handleFile<DocDotFile>(cmdName);
      break;
    case CMD_VHDLFLOW:
      handleVhdlFlow();
      break;
    case CMD_MSCFILE:
      handleFile<DocMscFile>(cmdName);
      break;
    case CMD_DIAFILE:
      handleFile<DocDiaFile>(cmdName);
      break;
    case CMD_LINK:
      handleLink(cmdName,FALSE);
      break;
    case CMD_JAVALINK:
      handleLink(cmdName,TRUE);
      break;
    case CMD_CITE:
      handleCite();
      break;
    case CMD_EMOJI:
      handleEmoji();
      break;
    case CMD_REF: // fall through
    case CMD_SUBPAGE:
      handleRef(cmdName);
      break;
    case CMD_SECREFLIST:
      {
        DocSecRefList *list = new DocSecRefList(this);
        m_children.push_back(std::unique_ptr<DocSecRefList>(list));
        list->parse();
      }
      break;
    case CMD_SECREFITEM:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected command %s",qPrint(g_token->name));
      break;
    case CMD_ENDSECREFLIST:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected command %s",qPrint(g_token->name));
      break;
    case CMD_FORMULA:
      {
        m_children.push_back(
            std::make_unique<DocFormula>(
              this,g_token->id));
      }
      break;
    //case CMD_LANGSWITCH:
    //  retval = handleLanguageSwitch();
    //  break;
    case CMD_INTERNALREF:
      //warn_doc_error(g_fileName,getDoctokinizerLineNr(),"unexpected command %s",qPrint(g_token->name));
      {
        DocInternalRef *ref = handleInternalRef(this);
        if (ref)
        {
          m_children.push_back(std::unique_ptr<DocInternalRef>(ref));
          ref->parse();
        }
        doctokenizerYYsetStatePara();
      }
      break;
    case CMD_INHERITDOC:
      handleInheritDoc();
      break;
    case CMD_ILINE:
      handleIline();
      break;
    default:
      // we should not get here!
      ASSERT(0);
      break;
  }
  INTERNAL_ASSERT(retval==0 || retval==RetVal_OK || retval==RetVal_SimpleSec ||
         retval==TK_LISTITEM || retval==TK_ENDLIST || retval==TK_NEWPARA ||
         retval==RetVal_Section || retval==RetVal_EndList ||
         retval==RetVal_Internal || retval==RetVal_SwitchLang ||
         retval==RetVal_EndInternal
        );
  DBG(("handleCommand(%s) end retval=%x\n",qPrint(cmdName),retval));
  return retval;
}

static bool findAttribute(const HtmlAttribList &tagHtmlAttribs,
                          const char *attrName,
                          QCString *result)
{

  for (const auto &opt : tagHtmlAttribs)
  {
    if (opt.name==attrName)
    {
      *result = opt.value;
      return TRUE;
    }
  }
  return FALSE;
}

int DocPara::handleHtmlStartTag(const QCString &tagName,const HtmlAttribList &tagHtmlAttribs)
{
  DBG(("handleHtmlStartTag(%s,%d)\n",qPrint(tagName),tagHtmlAttribs.count()));
  int retval=RetVal_OK;
  int tagId = Mappers::htmlTagMapper->map(tagName);
  if (g_token->emptyTag && !(tagId&XML_CmdMask) &&
      tagId!=HTML_UNKNOWN && tagId!=HTML_IMG && tagId!=HTML_BR && tagId!=HTML_HR && tagId!=HTML_P)
  {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"HTML tag ('<%s/>') may not use the 'empty tag' XHTML syntax.",
                     qPrint(tagName));
  }
  switch (tagId)
  {
    case HTML_UL:
      if (!g_token->emptyTag)
      {
        DocHtmlList *list = new DocHtmlList(this,tagHtmlAttribs,DocHtmlList::Unordered);
        m_children.push_back(std::unique_ptr<DocHtmlList>(list));
        retval=list->parse();
      }
      break;
    case HTML_OL:
      if (!g_token->emptyTag)
      {
        DocHtmlList *list = new DocHtmlList(this,tagHtmlAttribs,DocHtmlList::Ordered);
        m_children.push_back(std::unique_ptr<DocHtmlList>(list));
        retval=list->parse();
      }
      break;
    case HTML_LI:
      if (g_token->emptyTag) break;
      if (!insideUL(this) && !insideOL(this))
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"lonely <li> tag found");
      }
      else
      {
        retval=RetVal_ListItem;
      }
      break;
    case HTML_BOLD:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Bold,tagName,&g_token->attribs);
      break;
    case HTML_S:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::S,tagName,&g_token->attribs);
      break;
    case HTML_STRIKE:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Strike,tagName,&g_token->attribs);
      break;
    case HTML_DEL:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Del,tagName,&g_token->attribs);
      break;
    case HTML_UNDERLINE:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Underline,tagName,&g_token->attribs);
      break;
    case HTML_INS:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Ins,tagName,&g_token->attribs);
      break;
    case HTML_CODE:
      if (g_token->emptyTag) break;
      if (/*getLanguageFromFileName(g_fileName)==SrcLangExt_CSharp ||*/ g_xmlComment)
        // for C# source or inside a <summary> or <remark> section we
        // treat <code> as an XML tag (so similar to @code)
      {
        doctokenizerYYsetStateXmlCode();
        retval = handleStartCode();
      }
      else // normal HTML markup
      {
        handleStyleEnter(this,m_children,DocStyleChange::Code,tagName,&g_token->attribs);
      }
      break;
    case HTML_EMPHASIS:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Italic,tagName,&g_token->attribs);
      break;
    case HTML_DIV:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Div,tagName,&g_token->attribs);
      break;
    case HTML_SPAN:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Span,tagName,&g_token->attribs);
      break;
    case HTML_SUB:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Subscript,tagName,&g_token->attribs);
      break;
    case HTML_SUP:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Superscript,tagName,&g_token->attribs);
      break;
    case HTML_CENTER:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Center,tagName,&g_token->attribs);
      break;
    case HTML_SMALL:
      if (!g_token->emptyTag) handleStyleEnter(this,m_children,DocStyleChange::Small,tagName,&g_token->attribs);
      break;
    case HTML_PRE:
      if (g_token->emptyTag) break;
      handleStyleEnter(this,m_children,DocStyleChange::Preformatted,tagName,&g_token->attribs);
      setInsidePreformatted(TRUE);
      doctokenizerYYsetInsidePre(TRUE);
      break;
    case HTML_P:
      retval=TK_NEWPARA;
      break;
    case HTML_DL:
      if (!g_token->emptyTag)
      {
        DocHtmlDescList *list = new DocHtmlDescList(this,tagHtmlAttribs);
        m_children.push_back(std::unique_ptr<DocHtmlDescList>(list));
        retval=list->parse();
      }
      break;
    case HTML_DT:
      retval = RetVal_DescTitle;
      break;
    case HTML_DD:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag <dd> found");
      break;
    case HTML_TABLE:
      if (!g_token->emptyTag)
      {
        DocHtmlTable *table = new DocHtmlTable(this,tagHtmlAttribs);
        m_children.push_back(std::unique_ptr<DocHtmlTable>(table));
        retval=table->parse();
      }
      break;
    case HTML_TR:
      retval = RetVal_TableRow;
      break;
    case HTML_TD:
      retval = RetVal_TableCell;
      break;
    case HTML_TH:
      retval = RetVal_TableHCell;
      break;
    case HTML_CAPTION:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag <caption> found");
      break;
    case HTML_BR:
      {
        m_children.push_back(std::make_unique<DocLineBreak>(this,tagHtmlAttribs));
      }
      break;
    case HTML_HR:
      {
        m_children.push_back(std::make_unique<DocHorRuler>(this,tagHtmlAttribs));
      }
      break;
    case HTML_A:
      retval=handleAHref(this,m_children,tagHtmlAttribs);
      break;
    case HTML_H1:
      if (!g_token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,1);
      break;
    case HTML_H2:
      if (!g_token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,2);
      break;
    case HTML_H3:
      if (!g_token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,3);
      break;
    case HTML_H4:
      if (!g_token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,4);
      break;
    case HTML_H5:
      if (!g_token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,5);
      break;
    case HTML_H6:
      if (!g_token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,6);
      break;
    case HTML_IMG:
      {
        handleImg(this,m_children,tagHtmlAttribs);
      }
      break;
    case HTML_BLOCKQUOTE:
      if (!g_token->emptyTag)
      {
        DocHtmlBlockQuote *block = new DocHtmlBlockQuote(this,tagHtmlAttribs);
        m_children.push_back(std::unique_ptr<DocHtmlBlockQuote>(block));
        retval = block->parse();
      }
      break;

    case XML_SUMMARY:
    case XML_REMARKS:
    case XML_EXAMPLE:
      g_xmlComment=TRUE;
      // fall through
    case XML_VALUE:
    case XML_PARA:
      if (!m_children.empty())
      {
        retval = TK_NEWPARA;
      }
      break;
    case XML_DESCRIPTION:
      if (insideTable(this))
      {
        retval=RetVal_TableCell;
      }
      break;
    case XML_C:
      handleStyleEnter(this,m_children,DocStyleChange::Code,tagName,&g_token->attribs);
      break;
    case XML_PARAM:
    case XML_TYPEPARAM:
      {
        g_xmlComment=TRUE;
        QCString paramName;
        if (findAttribute(tagHtmlAttribs,"name",&paramName))
        {
          if (paramName.isEmpty())
          {
            if (Config_getBool(WARN_NO_PARAMDOC))
            {
              warn_doc_error(g_fileName,getDoctokinizerLineNr(),"empty 'name' attribute for <param%s> tag.",tagId==XML_PARAM?"":"type");
            }
          }
          else
          {
            retval = handleParamSection(paramName,
                tagId==XML_PARAM ? DocParamSect::Param : DocParamSect::TemplateParam,
                TRUE);
          }
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Missing 'name' attribute from <param%s> tag.",tagId==XML_PARAM?"":"type");
        }
      }
      break;
    case XML_PARAMREF:
    case XML_TYPEPARAMREF:
      {
        QCString paramName;
        if (findAttribute(tagHtmlAttribs,"name",&paramName))
        {
          //printf("paramName=%s\n",qPrint(paramName));
          m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Italic,tagName,TRUE));
          m_children.push_back(std::make_unique<DocWord>(this,paramName));
          m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Italic,tagName,FALSE));
          if (retval!=TK_WORD) m_children.push_back(std::make_unique<DocWhiteSpace>(this," "));
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Missing 'name' attribute from <param%sref> tag.",tagId==XML_PARAMREF?"":"type");
        }
      }
      break;
    case XML_EXCEPTION:
      {
        g_xmlComment=TRUE;
        QCString exceptName;
        if (findAttribute(tagHtmlAttribs,"cref",&exceptName))
        {
          unescapeCRef(exceptName);
          retval = handleParamSection(exceptName,DocParamSect::Exception,TRUE);
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Missing 'cref' attribute from <exception> tag.");
        }
      }
      break;
    case XML_ITEM:
    case XML_LISTHEADER:
      if (insideTable(this))
      {
        retval=RetVal_TableRow;
      }
      else if (insideUL(this) || insideOL(this))
      {
        retval=RetVal_ListItem;
      }
      else
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"lonely <item> tag found");
      }
      break;
    case XML_RETURNS:
      g_xmlComment=TRUE;
      retval = handleSimpleSection(DocSimpleSect::Return,TRUE);
      g_hasReturnCommand=TRUE;
      break;
    case XML_TERM:
      //m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Bold,TRUE));
      if (insideTable(this))
      {
        retval=RetVal_TableCell;
      }
      break;
    case XML_SEE:
      // I'm not sure if <see> is the same as <seealso> or if it
      // should you link a member without producing a section. The
      // C# specification is extremely vague about this (but what else
      // can we expect from Microsoft...)
      {
        QCString cref;
        //printf("XML_SEE: empty tag=%d\n",g_token->emptyTag);
        if (findAttribute(tagHtmlAttribs,"cref",&cref))
        {
          unescapeCRef(cref);
          if (g_token->emptyTag) // <see cref="..."/> style
          {
            bool inSeeBlock = g_inSeeBlock;
            g_token->name = cref;
            g_inSeeBlock = TRUE;
            handleLinkedWord(this,m_children,TRUE);
            g_inSeeBlock = inSeeBlock;
          }
          else // <see cref="...">...</see> style
          {
            //DocRef *ref = new DocRef(this,cref);
            //m_children.append(ref);
            //ref->parse();
            doctokenizerYYsetStatePara();
            DocLink *lnk = new DocLink(this,cref);
            m_children.push_back(std::unique_ptr<DocLink>(lnk));
            QCString leftOver = lnk->parse(FALSE,TRUE);
            if (!leftOver.isEmpty())
            {
              m_children.push_back(std::make_unique<DocWord>(this,leftOver));
            }
          }
        }
        else if (findAttribute(tagHtmlAttribs,"langword",&cref)) // <see langword="..."/> or <see langword="..."></see>
        {
          bool inSeeBlock = g_inSeeBlock;
          g_token->name = cref;
          g_inSeeBlock = TRUE;
          m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Code,tagName,TRUE));
          handleLinkedWord(this,m_children,TRUE);
          m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Code,tagName,FALSE));
          g_inSeeBlock = inSeeBlock;
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Missing 'cref' or 'langword' attribute from <see> tag.");
        }
      }
      break;
    case XML_SEEALSO:
      {
        g_xmlComment=TRUE;
        QCString cref;
        if (findAttribute(tagHtmlAttribs,"cref",&cref))
        {
          unescapeCRef(cref);
          // Look for an existing "see" section
          DocSimpleSect *ss=0;
          for (const auto &n : m_children)
          {
            if (n->kind()==Kind_SimpleSect && ((DocSimpleSect *)n.get())->type()==DocSimpleSect::See)
            {
              ss = (DocSimpleSect *)n.get();
            }
          }

          if (!ss)  // start new section
          {
            ss=new DocSimpleSect(this,DocSimpleSect::See);
            m_children.push_back(std::unique_ptr<DocSimpleSect>(ss));
          }

          ss->appendLinkWord(cref);
          retval = RetVal_OK;
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Missing 'cref' attribute from <seealso> tag.");
        }
      }
      break;
    case XML_LIST:
      {
        QCString type;
        findAttribute(tagHtmlAttribs,"type",&type);
        DocHtmlList::Type listType = DocHtmlList::Unordered;
        HtmlAttribList emptyList;
        if (type=="number")
        {
          listType=DocHtmlList::Ordered;
        }
        if (type=="table")
        {
          DocHtmlTable *table = new DocHtmlTable(this,emptyList);
          m_children.push_back(std::unique_ptr<DocHtmlTable>(table));
          retval=table->parseXml();
        }
        else
        {
          DocHtmlList *list = new DocHtmlList(this,emptyList,listType);
          m_children.push_back(std::unique_ptr<DocHtmlList>(list));
          retval=list->parseXml();
        }
      }
      break;
    case XML_INCLUDE:
    case XML_PERMISSION:
      // These tags are defined in .Net but are currently unsupported
      g_xmlComment=TRUE;
      break;
    case HTML_UNKNOWN:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unsupported xml/html tag <%s> found", qPrint(tagName));
      m_children.push_back(std::make_unique<DocWord>(this, "<"+tagName+g_token->attribsStr+">"));
      break;
  case XML_INHERITDOC:
      handleInheritDoc();
      break;
  default:
      // we should not get here!
      ASSERT(0);
      break;
  }
  return retval;
}

int DocPara::handleHtmlEndTag(const QCString &tagName)
{
  DBG(("handleHtmlEndTag(%s)\n",qPrint(tagName)));
  int tagId = Mappers::htmlTagMapper->map(tagName);
  int retval=RetVal_OK;
  switch (tagId)
  {
    case HTML_UL:
      if (!insideUL(this))
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found </ul> tag without matching <ul>");
      }
      else
      {
        retval=RetVal_EndList;
      }
      break;
    case HTML_OL:
      if (!insideOL(this))
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found </ol> tag without matching <ol>");
      }
      else
      {
        retval=RetVal_EndList;
      }
      break;
    case HTML_LI:
      if (!insideLI(this))
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found </li> tag without matching <li>");
      }
      else
      {
        // ignore </li> tags
      }
      break;
    case HTML_BLOCKQUOTE:
      retval=RetVal_EndBlockQuote;
      break;
    //case HTML_PRE:
    //  if (!insidePRE(this))
    //  {
    //    warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found </pre> tag without matching <pre>");
    //  }
    //  else
    //  {
    //    retval=RetVal_EndPre;
    //  }
    //  break;
    case HTML_BOLD:
      handleStyleLeave(this,m_children,DocStyleChange::Bold,tagName);
      break;
    case HTML_S:
      handleStyleLeave(this,m_children,DocStyleChange::S,"s");
      break;
    case HTML_STRIKE:
      handleStyleLeave(this,m_children,DocStyleChange::Strike,tagName);
      break;
    case HTML_DEL:
      handleStyleLeave(this,m_children,DocStyleChange::Del,tagName);
      break;
    case HTML_UNDERLINE:
      handleStyleLeave(this,m_children,DocStyleChange::Underline,tagName);
      break;
    case HTML_INS:
      handleStyleLeave(this,m_children,DocStyleChange::Ins,tagName);
      break;
    case HTML_CODE:
      handleStyleLeave(this,m_children,DocStyleChange::Code,tagName);
      break;
    case HTML_EMPHASIS:
      handleStyleLeave(this,m_children,DocStyleChange::Italic,tagName);
      break;
    case HTML_DIV:
      handleStyleLeave(this,m_children,DocStyleChange::Div,tagName);
      break;
    case HTML_SPAN:
      handleStyleLeave(this,m_children,DocStyleChange::Span,tagName);
      break;
    case HTML_SUB:
      handleStyleLeave(this,m_children,DocStyleChange::Subscript,tagName);
      break;
    case HTML_SUP:
      handleStyleLeave(this,m_children,DocStyleChange::Superscript,tagName);
      break;
    case HTML_CENTER:
      handleStyleLeave(this,m_children,DocStyleChange::Center,tagName);
      break;
    case HTML_SMALL:
      handleStyleLeave(this,m_children,DocStyleChange::Small,tagName);
      break;
    case HTML_PRE:
      handleStyleLeave(this,m_children,DocStyleChange::Preformatted,tagName);
      setInsidePreformatted(FALSE);
      doctokenizerYYsetInsidePre(FALSE);
      break;
    case HTML_P:
      retval=TK_NEWPARA;
      break;
    case HTML_DL:
      retval=RetVal_EndDesc;
      break;
    case HTML_DT:
      // ignore </dt> tag
      break;
    case HTML_DD:
      // ignore </dd> tag
      break;
    case HTML_TABLE:
      retval=RetVal_EndTable;
      break;
    case HTML_TR:
      // ignore </tr> tag
      break;
    case HTML_TD:
      // ignore </td> tag
      break;
    case HTML_TH:
      // ignore </th> tag
      break;
    case HTML_CAPTION:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag </caption> found");
      break;
    case HTML_BR:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Illegal </br> tag found\n");
      break;
    case HTML_H1:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag </h1> found");
      break;
    case HTML_H2:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag </h2> found");
      break;
    case HTML_H3:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag </h3> found");
      break;
    case HTML_H4:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag </h4> found");
      break;
    case HTML_H5:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag </h5> found");
      break;
    case HTML_H6:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag </h6> found");
      break;
    case HTML_IMG:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag </img> found");
      break;
    case HTML_HR:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Illegal </hr> tag found\n");
      break;
    case HTML_A:
      //warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected tag </a> found");
      // ignore </a> tag (can be part of <a name=...></a>
      break;

    case XML_TERM:
      //m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)g_nodeStack.size(),DocStyleChange::Bold,FALSE));
      break;
    case XML_SUMMARY:
    case XML_REMARKS:
    case XML_PARA:
    case XML_VALUE:
    case XML_EXAMPLE:
    case XML_PARAM:
    case XML_LIST:
    case XML_TYPEPARAM:
    case XML_RETURNS:
    case XML_SEE:
    case XML_SEEALSO:
    case XML_EXCEPTION:
    case XML_INHERITDOC:
      retval = RetVal_CloseXml;
      break;
    case XML_C:
      handleStyleLeave(this,m_children,DocStyleChange::Code,tagName);
      break;
    case XML_ITEM:
    case XML_LISTHEADER:
    case XML_INCLUDE:
    case XML_PERMISSION:
    case XML_DESCRIPTION:
    case XML_PARAMREF:
    case XML_TYPEPARAMREF:
      // These tags are defined in .Net but are currently unsupported
      break;
    case HTML_UNKNOWN:
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unsupported xml/html tag </%s> found", qPrint(tagName));
      m_children.push_back(std::make_unique<DocWord>(this,"</"+tagName+">"));
      break;
    default:
      // we should not get here!
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected end tag %s\n",qPrint(tagName));
      ASSERT(0);
      break;
  }
  return retval;
}

int DocPara::parse()
{
  DBG(("DocPara::parse() start\n"));
  auto ns = AutoNodeStack(this);
  // handle style commands "inherited" from the previous paragraph
  handleInitialStyleCommands(this,m_children);
  int tok;
  int retval=0;
  while ((tok=doctokenizerYYlex())) // get the next token
  {
reparsetoken:
    DBG(("token %s at %d",tokToString(tok),getDoctokinizerLineNr()));
    if (tok==TK_WORD || tok==TK_LNKWORD || tok==TK_SYMBOL || tok==TK_URL ||
        tok==TK_COMMAND_AT || tok == TK_COMMAND_BS || tok==TK_HTMLTAG
       )
    {
      DBG((" name=%s",qPrint(g_token->name)));
    }
    DBG(("\n"));
    switch(tok)
    {
      case TK_WORD:
        m_children.push_back(std::make_unique<DocWord>(this,g_token->name));
        break;
      case TK_LNKWORD:
        handleLinkedWord(this,m_children);
        break;
      case TK_URL:
        m_children.push_back(std::make_unique<DocURL>(this,g_token->name,g_token->isEMailAddr));
        break;
      case TK_WHITESPACE:
        {
          // prevent leading whitespace and collapse multiple whitespace areas
          DocNode::Kind k;
          if (insidePRE(this) || // all whitespace is relevant
              (
               // remove leading whitespace
               !m_children.empty()  &&
               // and whitespace after certain constructs
               (k=m_children.back()->kind())!=DocNode::Kind_HtmlDescList &&
               k!=DocNode::Kind_HtmlTable &&
               k!=DocNode::Kind_HtmlList &&
               k!=DocNode::Kind_SimpleSect &&
               k!=DocNode::Kind_AutoList &&
               k!=DocNode::Kind_SimpleList &&
               /*k!=DocNode::Kind_Verbatim &&*/
               k!=DocNode::Kind_HtmlHeader &&
               k!=DocNode::Kind_HtmlBlockQuote &&
               k!=DocNode::Kind_ParamSect &&
               k!=DocNode::Kind_XRefItem
              )
             )
          {
            m_children.push_back(std::make_unique<DocWhiteSpace>(this,g_token->chars));
          }
        }
        break;
      case TK_LISTITEM:
        {
          DBG(("found list item at %d parent=%d\n",g_token->indent,parent()->kind()));
          DocNode *n=parent();
          while (n && n->kind()!=DocNode::Kind_AutoList) n=n->parent();
          if (n) // we found an auto list up in the hierarchy
          {
            DocAutoList *al = (DocAutoList *)n;
            DBG(("previous list item at %d\n",al->indent()));
            if (al->indent()>=g_token->indent)
              // new item at the same or lower indent level
            {
              retval=TK_LISTITEM;
              goto endparagraph;
            }
          }

          // determine list depth
          int depth = 0;
          n=parent();
          while(n)
          {
            if (n->kind() == DocNode::Kind_AutoList &&
                ((DocAutoList*)n)->isEnumList()) depth++;
            n=n->parent();
          }

          // first item or sub list => create new list
          DocAutoList *al=0;
          do
          {
            al = new DocAutoList(this,g_token->indent,
                                 g_token->isEnumList,depth);
            m_children.push_back(std::unique_ptr<DocAutoList>(al));
            retval = al->parse();
          } while (retval==TK_LISTITEM &&         // new list
              al->indent()==g_token->indent  // at same indent level
              );

          // check the return value
          if (retval==RetVal_SimpleSec) // auto list ended due to simple section command
          {
            // Reparse the token that ended the section at this level,
            // so a new simple section will be started at this level.
            // This is the same as unputting the last read token and continuing.
            g_token->name = g_token->simpleSectName;
            if (g_token->name.left(4)=="rcs:") // RCS section
            {
              g_token->name = g_token->name.mid(4);
              g_token->text = g_token->simpleSectText;
              tok = TK_RCSTAG;
            }
            else // other section
            {
              tok = TK_COMMAND_BS;
            }
            DBG(("reparsing command %s\n",qPrint(g_token->name)));
            goto reparsetoken;
          }
          else if (retval==TK_ENDLIST)
          {
            if (al->indent()>g_token->indent) // end list
            {
              goto endparagraph;
            }
            else // continue with current paragraph
            {
            }
          }
          else // paragraph ended due to TK_NEWPARA, TK_LISTITEM, or EOF
          {
            goto endparagraph;
          }
        }
        break;
      case TK_ENDLIST:
        DBG(("Found end of list inside of paragraph at line %d\n",getDoctokinizerLineNr()));
        if (parent()->kind()==DocNode::Kind_AutoListItem)
        {
          ASSERT(parent()->parent()->kind()==DocNode::Kind_AutoList);
          DocAutoList *al = (DocAutoList *)parent()->parent();
          if (al->indent()>=g_token->indent)
          {
            // end of list marker ends this paragraph
            retval=TK_ENDLIST;
            goto endparagraph;
          }
          else
          {
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"End of list marker found "
                "has invalid indent level");
          }
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"End of list marker found without any preceding "
              "list items");
        }
        break;
      case TK_COMMAND_AT:
        // fall through
      case TK_COMMAND_BS:
        {
          // see if we have to start a simple section
          int cmd = Mappers::cmdMapper->map(g_token->name);
          DocNode *n=parent();
          while (n &&
              n->kind()!=DocNode::Kind_SimpleSect &&
              n->kind()!=DocNode::Kind_ParamSect
              )
          {
            n=n->parent();
          }
          if (cmd&SIMPLESECT_BIT)
          {
            if (n)  // already in a simple section
            {
              // simple section cannot start in this paragraph, need
              // to unwind the stack and remember the command.
              g_token->simpleSectName = g_token->name;
              retval=RetVal_SimpleSec;
              goto endparagraph;
            }
          }
          // see if we are in a simple list
          n=parent();
          while (n && n->kind()!=DocNode::Kind_SimpleListItem) n=n->parent();
          if (n)
          {
            if (cmd==CMD_LI)
            {
              retval=RetVal_ListItem;
              goto endparagraph;
            }
          }

          // handle the command
          retval=handleCommand(g_token->name,tok);
          DBG(("handleCommand returns %x\n",retval));

          // check the return value
          if (retval==RetVal_SimpleSec)
          {
            // Reparse the token that ended the section at this level,
            // so a new simple section will be started at this level.
            // This is the same as unputting the last read token and continuing.
            g_token->name = g_token->simpleSectName;
            if (g_token->name.left(4)=="rcs:") // RCS section
            {
              g_token->name = g_token->name.mid(4);
              g_token->text = g_token->simpleSectText;
              tok = TK_RCSTAG;
            }
            else // other section
            {
              tok = TK_COMMAND_BS;
            }
            DBG(("reparsing command %s\n",qPrint(g_token->name)));
            goto reparsetoken;
          }
          else if (retval==RetVal_OK)
          {
            // the command ended normally, keep scanning for new tokens.
            retval = 0;
          }
          else if (retval>0 && retval<RetVal_OK)
          {
            // the command ended with a new command, reparse this token
            tok = retval;
            goto reparsetoken;
          }
          else // end of file, end of paragraph, start or end of section
            // or some auto list marker
          {
            goto endparagraph;
          }
        }
        break;
      case TK_HTMLTAG:
        {
          if (!g_token->endTag) // found a start tag
          {
            retval = handleHtmlStartTag(g_token->name,g_token->attribs);
          }
          else // found an end tag
          {
            retval = handleHtmlEndTag(g_token->name);
          }
          if (retval==RetVal_OK)
          {
            // the command ended normally, keep scanner for new tokens.
            retval = 0;
          }
          else
          {
            goto endparagraph;
          }
        }
        break;
      case TK_SYMBOL:
        {
          DocSymbol::SymType s = DocSymbol::decodeSymbol(g_token->name);
          if (s!=DocSymbol::Sym_Unknown)
          {
            m_children.push_back(std::make_unique<DocSymbol>(this,s));
          }
          else
          {
            m_children.push_back(std::make_unique<DocWord>(this,g_token->name));
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unsupported symbol %s found",
                qPrint(g_token->name));
          }
          break;
        }
      case TK_NEWPARA:
        retval=TK_NEWPARA;
        goto endparagraph;
      case TK_RCSTAG:
        {
          DocNode *n=parent();
          while (n &&
              n->kind()!=DocNode::Kind_SimpleSect &&
              n->kind()!=DocNode::Kind_ParamSect
              )
          {
            n=n->parent();
          }
          if (n)  // already in a simple section
          {
            // simple section cannot start in this paragraph, need
            // to unwind the stack and remember the command.
            g_token->simpleSectName = "rcs:"+g_token->name;
            g_token->simpleSectText = g_token->text;
            retval=RetVal_SimpleSec;
            goto endparagraph;
          }

          // see if we are in a simple list
          DocSimpleSect *ss=new DocSimpleSect(this,DocSimpleSect::Rcs);
          m_children.push_back(std::unique_ptr<DocSimpleSect>(ss));
          ss->parseRcs();
        }
        break;
      default:
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),
            "Found unexpected token (id=%x)\n",tok);
        break;
    }
  }
  retval=0;
endparagraph:
  handlePendingStyleCommands(this,m_children);
  DBG(("DocPara::parse() end retval=%x\n",retval));
  const DocNode *n = g_nodeStack.top();
  if (!g_token->endTag && n->kind()==DocNode::Kind_Para &&
      retval==TK_NEWPARA && g_token->name.lower() == "p")
  {
    ((DocPara *)n)->setAttribs(g_token->attribs);
  }
  INTERNAL_ASSERT(retval==0 || retval==TK_NEWPARA || retval==TK_LISTITEM ||
         retval==TK_ENDLIST || retval>RetVal_OK
	);

  return retval;
}

//--------------------------------------------------------------------------

int DocSection::parse()
{
  DBG(("DocSection::parse() start %s level=%d\n",qPrint(g_token->sectionId),m_level));
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(this);

  if (!m_id.isEmpty())
  {
    const SectionInfo *sec = SectionManager::instance().find(m_id);
    if (sec)
    {
      m_file   = sec->fileName();
      m_anchor = sec->label();
      m_title  = sec->title();
      if (m_title.isEmpty()) m_title = sec->label();
    }
  }

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  do
  {
    DocPara *par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (!par->isEmpty())
    {
      m_children.push_back(std::unique_ptr<DocPara>(par));
      lastPar=par;
    }
    else
    {
      delete par;
    }
    if (retval==TK_LISTITEM)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid list item found");
    }
    if (retval==RetVal_Internal)
    {
      DocInternal *in = new DocInternal(this);
      m_children.push_back(std::unique_ptr<DocInternal>(in));
      retval = in->parse(m_level+1);
      if (retval==RetVal_EndInternal)
      {
        retval=RetVal_OK;
      }
    }
  } while (retval!=0 &&
           retval!=RetVal_Section       &&
           retval!=RetVal_Subsection    &&
           retval!=RetVal_Subsubsection &&
           retval!=RetVal_Paragraph     &&
           retval!=RetVal_EndInternal
          );

  if (lastPar) lastPar->markLast();

  //printf("m_level=%d <-> %d\n",m_level,Doxygen::subpageNestingLevel);

  while (true)
  {
    if (retval==RetVal_Subsection && m_level<=Doxygen::subpageNestingLevel+1)
    {
      // then parse any number of nested sections
      while (retval==RetVal_Subsection) // more sections follow
      {
        DocSection *s=new DocSection(this,
            std::min(2+Doxygen::subpageNestingLevel,5),g_token->sectionId);
        m_children.push_back(std::unique_ptr<DocSection>(s));
        retval = s->parse();
      }
      break;
    }
    else if (retval==RetVal_Subsubsection && m_level<=Doxygen::subpageNestingLevel+2)
    {
      if ((m_level<=1+Doxygen::subpageNestingLevel) && !g_token->sectionId.startsWith("autotoc_md"))
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected subsubsection command found inside %s!",g_sectionLevelToName[m_level]);
      // then parse any number of nested sections
      while (retval==RetVal_Subsubsection) // more sections follow
      {
        DocSection *s=new DocSection(this,
            std::min(3+Doxygen::subpageNestingLevel,5),g_token->sectionId);
        m_children.push_back(std::unique_ptr<DocSection>(s));
        retval = s->parse();
      }
      if (!(m_level<Doxygen::subpageNestingLevel+2 && retval == RetVal_Subsection)) break;
    }
    else if (retval==RetVal_Paragraph && m_level<=std::min(5,Doxygen::subpageNestingLevel+3))
    {
      if ((m_level<=2+Doxygen::subpageNestingLevel) && !g_token->sectionId.startsWith("autotoc_md"))
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected paragraph command found inside %s!",g_sectionLevelToName[m_level]);
      // then parse any number of nested sections
      while (retval==RetVal_Paragraph) // more sections follow
      {
        DocSection *s=new DocSection(this,
            std::min(4+Doxygen::subpageNestingLevel,5),g_token->sectionId);
        m_children.push_back(std::unique_ptr<DocSection>(s));
        retval = s->parse();
      }
      if (!(m_level<Doxygen::subpageNestingLevel+3 && (retval == RetVal_Subsection || retval == RetVal_Subsubsection))) break;
    }
    else
    {
      break;
    }
  }

  INTERNAL_ASSERT(retval==0 ||
                  retval==RetVal_Section ||
                  retval==RetVal_Subsection ||
                  retval==RetVal_Subsubsection ||
                  retval==RetVal_Paragraph ||
                  retval==RetVal_Internal ||
                  retval==RetVal_EndInternal
                 );

  DBG(("DocSection::parse() end: retval=%x\n",retval));
  return retval;
}

//--------------------------------------------------------------------------

void DocText::parse()
{
  DBG(("DocText::parse() start\n"));
  auto ns = AutoNodeStack(this);
  doctokenizerYYsetStateText();

  int tok;
  while ((tok=doctokenizerYYlex())) // get the next token
  {
    switch(tok)
    {
      case TK_WORD:
	m_children.push_back(std::make_unique<DocWord>(this,g_token->name));
	break;
      case TK_WHITESPACE:
        m_children.push_back(std::make_unique<DocWhiteSpace>(this,g_token->chars));
	break;
      case TK_SYMBOL:
        {
          DocSymbol::SymType s = DocSymbol::decodeSymbol(g_token->name);
          if (s!=DocSymbol::Sym_Unknown)
          {
            m_children.push_back(std::make_unique<DocSymbol>(this,s));
          }
          else
          {
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unsupported symbol %s found",
                qPrint(g_token->name));
          }
        }
        break;
      case TK_COMMAND_AT:
        // fall through
      case TK_COMMAND_BS:
        switch (Mappers::cmdMapper->map(g_token->name))
        {
          case CMD_BSLASH:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_BSlash));
            break;
          case CMD_AT:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_At));
            break;
          case CMD_LESS:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Less));
            break;
          case CMD_GREATER:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Greater));
            break;
          case CMD_AMP:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Amp));
            break;
          case CMD_DOLLAR:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Dollar));
            break;
          case CMD_HASH:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Hash));
            break;
          case CMD_DCOLON:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_DoubleColon));
            break;
          case CMD_PERCENT:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Percent));
            break;
          case CMD_NDASH:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
            break;
          case CMD_MDASH:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
            break;
          case CMD_QUOTE:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Quot));
            break;
          case CMD_PUNT:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Dot));
            break;
          case CMD_PLUS:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Plus));
            break;
          case CMD_MINUS:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Minus));
            break;
          case CMD_EQUAL:
            m_children.push_back(std::make_unique<DocSymbol>(this,DocSymbol::Sym_Equal));
            break;
          default:
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected command '%s' found",
                      qPrint(g_token->name));
            break;
        }
        break;
      default:
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Unexpected token %s",
            tokToString(tok));
        break;
    }
  }

  handleUnclosedStyleCommands();

  DBG(("DocText::parse() end\n"));
}


//--------------------------------------------------------------------------

void DocRoot::parse()
{
  DBG(("DocRoot::parse() start\n"));
  auto ns = AutoNodeStack(this);
  doctokenizerYYsetStatePara();
  int retval=0;

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  do
  {
    DocPara *par = new DocPara(this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (!par->isEmpty() || !par->attribs().empty())
    {
      m_children.push_back(std::unique_ptr<DocPara>(par));
      lastPar=par;
    }
    else
    {
      delete par;
    }
    if (retval==RetVal_Paragraph)
    {
      if (!g_token->sectionId.startsWith("autotoc_md"))
      {
         warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found paragraph command (id: '%s') outside of subsubsection context!",qPrint(g_token->sectionId));
      }
      while (retval==RetVal_Paragraph)
      {
        if (!g_token->sectionId.isEmpty())
        {
          const SectionInfo *sec=SectionManager::instance().find(g_token->sectionId);
          if (sec)
          {
            DocSection *s=new DocSection(this,
                std::min(4+Doxygen::subpageNestingLevel,5),g_token->sectionId);
            m_children.push_back(std::unique_ptr<DocSection>(s));
            retval = s->parse();
          }
          else
          {
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid paragraph id '%s'; ignoring paragraph",qPrint(g_token->sectionId));
            retval = 0;
          }
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Missing id for paragraph; ignoring paragraph");
          retval = 0;
        }
      }
    }
    if (retval==RetVal_Subsubsection)
    {
      if (!(g_token->sectionId.startsWith("autotoc_md")))
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found subsubsection command (id: '%s') outside of subsection context!",qPrint(g_token->sectionId));
      while (retval==RetVal_Subsubsection)
      {
        if (!g_token->sectionId.isEmpty())
        {
          const SectionInfo *sec=SectionManager::instance().find(g_token->sectionId);
          if (sec)
          {
            DocSection *s=new DocSection(this,
                std::min(3+Doxygen::subpageNestingLevel,5),g_token->sectionId);
            m_children.push_back(std::unique_ptr<DocSection>(s));
            retval = s->parse();
          }
          else
          {
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid subsubsection id '%s'; ignoring subsubsection",qPrint(g_token->sectionId));
            retval = 0;
          }
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Missing id for subsubsection; ignoring subsubsection");
          retval = 0;
        }
      }
    }
    if (retval==RetVal_Subsection)
    {
      if (!g_token->sectionId.startsWith("autotoc_md"))
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"found subsection command (id: '%s') outside of section context!",qPrint(g_token->sectionId));
      }
      while (retval==RetVal_Subsection)
      {
        if (!g_token->sectionId.isEmpty())
        {
          const SectionInfo *sec=SectionManager::instance().find(g_token->sectionId);
          if (sec)
          {
            DocSection *s=new DocSection(this,
                std::min(2+Doxygen::subpageNestingLevel,5),g_token->sectionId);
            m_children.push_back(std::unique_ptr<DocSection>(s));
            retval = s->parse();
          }
          else
          {
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid subsection id '%s'; ignoring subsection",qPrint(g_token->sectionId));
            retval = 0;
          }
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Missing id for subsection; ignoring subsection");
          retval = 0;
        }
      }
    }
    if (retval==TK_LISTITEM)
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid list item found");
    }
    if (retval==RetVal_Internal)
    {
      DocInternal *in = new DocInternal(this);
      m_children.push_back(std::unique_ptr<DocInternal>(in));
      retval = in->parse(1);
    }
  } while (retval!=0 && retval!=RetVal_Section);
  if (lastPar) lastPar->markLast();

  //printf("DocRoot::parse() retval=%d %d\n",retval,RetVal_Section);
  // then parse any number of level1 sections
  while (retval==RetVal_Section)
  {
    if (!g_token->sectionId.isEmpty())
    {
      const SectionInfo *sec=SectionManager::instance().find(g_token->sectionId);
      if (sec)
      {
        DocSection *s=new DocSection(this,
            std::min(1+Doxygen::subpageNestingLevel,5),g_token->sectionId);
        m_children.push_back(std::unique_ptr<DocSection>(s));
        retval = s->parse();
      }
      else
      {
        warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Invalid section id '%s'; ignoring section",qPrint(g_token->sectionId));
        retval = 0;
      }
    }
    else
    {
      warn_doc_error(g_fileName,getDoctokinizerLineNr(),"Missing id for section; ignoring section");
      retval = 0;
    }
  }

  handleUnclosedStyleCommands();

  DBG(("DocRoot::parse() end\n"));
}

static QCString extractCopyDocId(const char *data, uint &j, uint len)
{
  uint s=j;
  uint e=j;
  int round=0;
  bool insideDQuote=FALSE;
  bool insideSQuote=FALSE;
  bool found=FALSE;
  while (j<len && !found)
  {
    if (!insideSQuote && !insideDQuote)
    {
      switch (data[j])
      {
        case '(': round++; break;
        case ')': round--; break;
        case '"': insideDQuote=TRUE; break;
        case '\'': insideSQuote=TRUE; break;
        case ' ':  // fall through
        case '\t': // fall through
        case '\n':
          found=(round==0);
          break;
      }
    }
    else if (insideSQuote) // look for single quote end
    {
      if (data[j]=='\'' && (j==0 || data[j]!='\\'))
      {
        insideSQuote=FALSE;
      }
    }
    else if (insideDQuote) // look for double quote end
    {
      if (data[j]=='"' && (j==0 || data[j]!='\\'))
      {
        insideDQuote=FALSE;
      }
    }
    if (!found) j++;
  }
  if (qstrncmp(data+j," const",6)==0)
  {
    j+=6;
  }
  else if (qstrncmp(data+j," volatile",9)==0)
  {
    j+=9;
  }
  e=j;
  QCString id(data+s,e-s);
  //printf("extractCopyDocId='%s' input='%s'\n",qPrint(id),&data[s]);
  return id;
}

// macro to check if the input starts with a specific command.
// note that data[i] should point to the start of the command (\ or @ character)
// and the sizeof(str) returns the size of str including the '\0' terminator;
// a fact we abuse to skip over the start of the command character.
#define CHECK_FOR_COMMAND(str,action) \
   do if ((i+sizeof(str)<len) && qstrncmp(data+i+1,str,sizeof(str)-1)==0) \
   { j=i+sizeof(str); action; } while(0)

static uint isCopyBriefOrDetailsCmd(const char *data, uint i,uint len,bool &brief)
{
  int j=0;
  if (i==0 || (data[i-1]!='@' && data[i-1]!='\\')) // not an escaped command
  {
    CHECK_FOR_COMMAND("copybrief",brief=TRUE);    // @copybrief or \copybrief
    CHECK_FOR_COMMAND("copydetails",brief=FALSE); // @copydetails or \copydetails
  }
  return j;
}

static uint isVerbatimSection(const char *data,uint i,uint len,QCString &endMarker)
{
  int j=0;
  if (i==0 || (data[i-1]!='@' && data[i-1]!='\\')) // not an escaped command
  {
    CHECK_FOR_COMMAND("dot",endMarker="enddot");
    CHECK_FOR_COMMAND("code",endMarker="endcode");
    CHECK_FOR_COMMAND("msc",endMarker="endmsc");
    CHECK_FOR_COMMAND("verbatim",endMarker="endverbatim");
    CHECK_FOR_COMMAND("latexonly",endMarker="endlatexonly");
    CHECK_FOR_COMMAND("htmlonly",endMarker="endhtmlonly");
    CHECK_FOR_COMMAND("xmlonly",endMarker="endxmlonly");
    CHECK_FOR_COMMAND("rtfonly",endMarker="endrtfonly");
    CHECK_FOR_COMMAND("manonly",endMarker="endmanonly");
    CHECK_FOR_COMMAND("docbookonly",endMarker="enddocbookonly");
    CHECK_FOR_COMMAND("startuml",endMarker="enduml");
  }
  //printf("isVerbatimSection(%s)=%d)\n",qPrint(QCString(&data[i]).left(10)),j);
  return j;
}

static uint skipToEndMarker(const char *data,uint i,uint len,const QCString &endMarker)
{
  while (i<len)
  {
    if ((data[i]=='@' || data[i]=='\\') &&  // start of command character
        (i==0 || (data[i-1]!='@' && data[i-1]!='\\'))) // that is not escaped
    {
      if (i+endMarker.length()+1<=len && qstrncmp(data+i+1,endMarker.data(),endMarker.length())==0)
      {
        return i+endMarker.length()+1;
      }
    }
    i++;
  }
  // oops no endmarker found...
  return i<len ? i+1 : len;
}

static QCString processCopyDoc(const char *data,uint &len)
{
  //printf("processCopyDoc start '%s'\n",data);
  GrowBuf buf;
  uint i=0;
  while (i<len)
  {
    char c = data[i];
    if (c=='@' || c=='\\') // look for a command
    {
      bool isBrief=TRUE;
      uint j=isCopyBriefOrDetailsCmd(data,i,len,isBrief);
      if (j>0)
      {
        // skip whitespace
        while (j<len && (data[j]==' ' || data[j]=='\t')) j++;
        // extract the argument
        QCString id = extractCopyDocId(data,j,len);
        const Definition *def = 0;
        QCString doc,brief;
        //printf("resolving docs='%s'\n",qPrint(id));
        if (findDocsForMemberOrCompound(id,&doc,&brief,&def))
        {
          //printf("found it def=%p brief='%s' doc='%s' isBrief=%d\n",def,qPrint(brief),qPrint(doc),isBrief);
          auto it = std::find(g_copyStack.begin(),g_copyStack.end(),def);
          if (it==g_copyStack.end()) // definition not parsed earlier
          {
            g_copyStack.push_back(def);
            if (isBrief)
            {
              uint l=brief.length();
              buf.addStr(processCopyDoc(brief.data(),l));
            }
            else
            {
              uint l=doc.length();
              buf.addStr(processCopyDoc(doc.data(),l));
            }
            g_copyStack.pop_back();
          }
          else
          {
            warn_doc_error(g_fileName,getDoctokinizerLineNr(),
	         "Found recursive @copy%s or @copydoc relation for argument '%s'.\n",
                 isBrief?"brief":"details",qPrint(id));
          }
        }
        else
        {
          warn_doc_error(g_fileName,getDoctokinizerLineNr(),
               "@copy%s or @copydoc target '%s' not found", isBrief?"brief":"details",
               qPrint(id));
        }
        // skip over command
        i=j;
      }
      else
      {
        QCString endMarker;
        uint k = isVerbatimSection(data,i,len,endMarker);
        if (k>0)
        {
          int orgPos = i;
          i=skipToEndMarker(data,k,len,endMarker);
          buf.addStr(data+orgPos,i-orgPos);
        }
        else
        {
          buf.addChar(c);
          i++;
        }
      }
    }
    else // not a command, just copy
    {
      buf.addChar(c);
      i++;
    }
  }
  len = buf.getPos();
  buf.addChar(0);
  return buf.get();
}

//---------------------------------------------------------------------------

DocRoot *validatingParseDoc(const QCString &fileName,int startLine,
                            const Definition *ctx,const MemberDef *md,
                            const QCString &input,bool indexWords,
                            bool isExample, const QCString &exampleName,
                            bool singleLine, bool linkFromIndex,
                            bool markdownSupport)
{
  //printf("validatingParseDoc(%s,%s)=[%s]\n",ctx?qPrint(ctx->name()):"<none>",
  //                                     md?qPrint(md->name()):"<none>",
  //                                     input);
  //printf("========== validating %s at line %d\n",fileName,startLine);
  //printf("---------------- input --------------------\n%s\n----------- end input -------------------\n",input);
  //g_token = new TokenInfo;

  // store parser state so we can re-enter this function if needed
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  docParserPushContext();

  if (ctx && ctx!=Doxygen::globalScope &&
      (ctx->definitionType()==Definition::TypeClass ||
       ctx->definitionType()==Definition::TypeNamespace
      )
     )
  {
    g_context = ctx->name();
  }
  else if (ctx && ctx->definitionType()==Definition::TypePage)
  {
    const Definition *scope = (toPageDef(ctx))->getPageScope();
    if (scope && scope!=Doxygen::globalScope) g_context = scope->name();
  }
  else if (ctx && ctx->definitionType()==Definition::TypeGroup)
  {
    const Definition *scope = (toGroupDef(ctx))->getGroupScope();
    if (scope && scope!=Doxygen::globalScope) g_context = scope->name();
  }
  else
  {
    g_context = "";
  }
  g_scope = ctx;

  if (indexWords && Doxygen::searchIndex)
  {
    if (md)
    {
      g_searchUrl=md->getOutputFileBase();
      Doxygen::searchIndex->setCurrentDoc(md,md->anchor(),FALSE);
    }
    else if (ctx)
    {
      g_searchUrl=ctx->getOutputFileBase();
      Doxygen::searchIndex->setCurrentDoc(ctx,ctx->anchor(),FALSE);
    }
  }
#if 0
  if (indexWords && md && Doxygen::searchIndex)
  {
    g_searchUrl=md->getOutputFileBase();
    Doxygen::searchIndex->setCurrentDoc(
        (md->getLanguage()==SrcLangExt_Fortran ?
         theTranslator->trSubprogram(TRUE,TRUE):
         theTranslator->trMember(TRUE,TRUE))+" "+md->qualifiedName(),
        g_searchUrl,
        md->anchor());
  }
  else if (indexWords && ctx && Doxygen::searchIndex)
  {
    g_searchUrl=ctx->getOutputFileBase();
    QCString name = ctx->qualifiedName();

    SrcLangExt lang = ctx->getLanguage();
    QCString sep = getLanguageSpecificSeparator(lang);
    if (sep!="::")
    {
      name = substitute(name,"::",sep);
    }

    switch (ctx->definitionType())
    {
      case Definition::TypePage:
        {
          PageDef *pd = (PageDef *)ctx;
          if (pd->hasTitle())
          {
            name = theTranslator->trPage(TRUE,TRUE)+" "+pd->title();
          }
          else
          {
            name = theTranslator->trPage(TRUE,TRUE)+" "+pd->name();
          }
        }
        break;
      case Definition::TypeClass:
        {
          ClassDef *cd = (ClassDef *)ctx;
          name.prepend(cd->compoundTypeString()+" ");
        }
        break;
      case Definition::TypeNamespace:
        {
          if (lang==SrcLangExt_Java || lang==SrcLangExt_CSharp)
          {
            name = theTranslator->trPackage(name);
          }
          else if (lang==SrcLangExt_Fortran)
          {
            name.prepend(theTranslator->trModule(TRUE,TRUE)+" ");
          }
          else
          {
            name.prepend(theTranslator->trNamespace(TRUE,TRUE)+" ");
          }
        }
        break;
      case Definition::TypeGroup:
        {
          GroupDef *gd = (GroupDef *)ctx;
          if (gd->groupTitle())
          {
            name = theTranslator->trGroup(TRUE,TRUE)+" "+gd->groupTitle();
          }
          else
          {
            name.prepend(theTranslator->trGroup(TRUE,TRUE)+" ");
          }
        }
        break;
      default:
        break;
    }
    Doxygen::searchIndex->setCurrentDoc(name,g_searchUrl);
  }
#endif
  else
  {
    g_searchUrl="";
  }

  g_fileName = fileName;
  g_relPath = (!linkFromIndex && ctx) ?
               QCString(relativePathToRoot(ctx->getOutputFileBase())) :
               QCString("");
  //printf("ctx->name=%s relPath=%s\n",qPrint(ctx->name()),qPrint(g_relPath));
  g_memberDef = md;
  while (!g_nodeStack.empty()) g_nodeStack.pop();
  while (!g_styleStack.empty()) g_styleStack.pop();
  while (!g_initialStyleStack.empty()) g_initialStyleStack.pop();
  g_inSeeBlock = FALSE;
  g_xmlComment = FALSE;
  g_insideHtmlLink = FALSE;
  g_includeFileText = "";
  g_includeFileOffset = 0;
  g_includeFileLength = 0;
  g_isExample = isExample;
  g_exampleName = exampleName;
  g_hasParamCommand = FALSE;
  g_hasReturnCommand = FALSE;
  g_retvalsFound.clear();
  g_paramsFound.clear();
  g_markdownSupport = markdownSupport;

  //printf("Starting comment block at %s:%d\n",qPrint(g_fileName),startLine);
  setDoctokinizerLineNr(startLine);
  uint ioLen = input.length();
  QCString inpStr = processCopyDoc(input.data(),ioLen);
  if (inpStr.isEmpty() || inpStr.at(inpStr.length()-1)!='\n')
  {
    inpStr+='\n';
  }
  //printf("processCopyDoc(in='%s' out='%s')\n",input,qPrint(inpStr));
  doctokenizerYYinit(inpStr.data(),g_fileName,markdownSupport);

  // build abstract syntax tree
  DocRoot *root = new DocRoot(md!=0,singleLine);
  root->parse();


  if (Debug::isFlagSet(Debug::PrintTree))
  {
    // pretty print the result
    PrintDocVisitor *v = new PrintDocVisitor;
    root->accept(v);
    delete v;
  }

  checkUnOrMultipleDocumentedParams();
  if (g_memberDef) g_memberDef->detectUndocumentedParams(g_hasParamCommand,g_hasReturnCommand);

  // TODO: These should be called at the end of the program.
  //doctokenizerYYcleanup();
  //Mappers::cmdMapper->freeInstance();
  //Mappers::htmlTagMapper->freeInstance();

  // restore original parser state
  docParserPopContext();

  //printf(">>>>>> end validatingParseDoc(%s,%s)\n",ctx?qPrint(ctx->name()):"<none>",
  //                                     md?qPrint(md->name()):"<none>");

  return root;
}

DocText *validatingParseText(const QCString &input)
{
  // store parser state so we can re-enter this function if needed
  docParserPushContext();

  //printf("------------ input ---------\n%s\n"
  //       "------------ end input -----\n",input);
  //g_token = new TokenInfo;
  g_context = "";
  g_fileName = "<parseText>";
  g_relPath = "";
  g_memberDef = 0;
  while (!g_nodeStack.empty()) g_nodeStack.pop();
  while (!g_styleStack.empty()) g_styleStack.pop();
  while (!g_initialStyleStack.empty()) g_initialStyleStack.pop();
  g_inSeeBlock = FALSE;
  g_xmlComment = FALSE;
  g_insideHtmlLink = FALSE;
  g_includeFileText = "";
  g_includeFileOffset = 0;
  g_includeFileLength = 0;
  g_isExample = FALSE;
  g_exampleName = "";
  g_hasParamCommand = FALSE;
  g_hasReturnCommand = FALSE;
  g_retvalsFound.clear();
  g_paramsFound.clear();
  g_searchUrl="";

  DocText *txt = new DocText;

  if (!input.isEmpty())
  {
    setDoctokinizerLineNr(1);
    doctokenizerYYinit(input.data(),g_fileName,Config_getBool(MARKDOWN_SUPPORT));

    // build abstract syntax tree
    txt->parse();

    if (Debug::isFlagSet(Debug::PrintTree))
    {
      // pretty print the result
      PrintDocVisitor *v = new PrintDocVisitor;
      txt->accept(v);
      delete v;
    }
  }

  // restore original parser state
  docParserPopContext();
  return txt;
}

void docFindSections(const QCString &input,
                     const Definition *d,
                     const QCString &fileName)
{
  doctokenizerYYFindSections(input,d,fileName);
}
