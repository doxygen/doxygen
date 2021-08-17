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

static const std::set<std::string> g_plantumlEngine {
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
  QCString relPath;

  bool         hasParamCommand;
  bool         hasReturnCommand;
  StringMultiSet retvalsFound;
  StringMultiSet paramsFound;
  const MemberDef *  memberDef;
  bool         isExample;
  QCString     exampleName;
  QCString     searchUrl;

  QCString includeFileName;
  QCString includeFileText;
  uint     includeFileOffset;
  uint     includeFileLength;
  int      includeFileLine;
  bool     includeFileShowLineNo;

  TokenInfo *token;
  int      lineNo;
  bool     markdownSupport;
};

class DocParser : public IDocParser
{
  public:
    void pushContext();
    void popContext();
    void handleImg(DocNode *parent,DocNodeList &children,const HtmlAttribList &tagHtmlAttribs);
    int  internalValidatingParseDoc(DocNode *parent,DocNodeList &children,
                                    const QCString &doc);
    QCString processCopyDoc(const char *data,uint &len);
    QCString findAndCopyImage(const QCString &fileName,DocImage::Type type, bool doWarn = true);
    void checkArgumentName();
    void checkRetvalName();
    void checkUnOrMultipleDocumentedParams();
    bool findDocsForMemberOrCompound(const QCString &commandName,
                                     QCString *pDoc,
                                     QCString *pBrief,
                                     const Definition **pDef);
    bool defaultHandleToken(DocNode *parent,int tok,
                            DocNodeList &children,bool
                            handleWord=TRUE);
    void errorHandleDefaultToken(DocNode *parent,int tok,
                                 DocNodeList &children,const QCString &txt);
    void defaultHandleTitleAndSize(const int cmd, DocNode *parent,
                                   DocNodeList &children, QCString &width,QCString &height);
    int handleStyleArgument(DocNode *parent,DocNodeList &children,
                            const QCString &cmdName);
    void handleStyleEnter(DocNode *parent,DocNodeList &children, DocStyleChange::Style s,
                          const QCString &tagName,const HtmlAttribList *attribs);
    void handleStyleLeave(DocNode *parent,DocNodeList &children, DocStyleChange::Style s,
                          const QCString &tagName);
    void handlePendingStyleCommands(DocNode *parent,DocNodeList &children);
    void handleInitialStyleCommands(DocPara *parent,DocNodeList &children);
    int  handleAHref(DocNode *parent,DocNodeList &children,const HtmlAttribList &tagHtmlAttribs);
    void handleUnclosedStyleCommands();
    void handleLinkedWord(DocNode *parent,DocNodeList &children,bool ignoreAutoLinkFlag=FALSE);
    void handleParameterType(DocNode *parent,DocNodeList &children,const QCString &paramTypes);
    DocInternalRef *handleInternalRef(DocNode *parent);
    DocAnchor *handleAnchor(DocNode *parent);
    void readTextFileByName(const QCString &file,QCString &text);

    std::stack< DocParserContext > contextStack;
    DocParserContext               context;
    DocTokenizer                   tokenizer;
};

std::unique_ptr<IDocParser> createDocParser()
{
  return std::make_unique<DocParser>();
}

//---------------------------------------------------------------------------

class AutoNodeStack
{
  public:
    AutoNodeStack(DocParser &parser,const DocNode* node)
      : m_parser(parser), m_node(node) { m_parser.context.nodeStack.push(node); }
   ~AutoNodeStack() {
#if defined(NDEBUG)
     (void)m_node;
     if (!m_parser.context.nodeStack.empty()) m_parser.context.nodeStack.pop(); // robust version that does not assert
#else
     assert(m_parser.context.nodeStack.top()==m_node); m_parser.context.nodeStack.pop(); // error checking version
#endif
   }

  private:
   DocParser &m_parser;
   const DocNode *m_node;
};

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void DocParser::pushContext()
{
  //QCString indent;
  //indent.fill(' ',contextStack.size()*2+2);
  //printf("%sdocParserPushContext() count=%zu\n",qPrint(indent),context.nodeStack.size());

  tokenizer.pushContext();
  contextStack.push(DocParserContext());
  auto &ctx = contextStack.top();
  ctx = context;
  ctx.lineNo = tokenizer.getLineNr();
  context.token = tokenizer.newToken();
}

void DocParser::popContext()
{
  auto &ctx = contextStack.top();
  context = ctx;
  tokenizer.setLineNr(ctx.lineNo);
  context.token = ctx.token;
  tokenizer.replaceToken(context.token);
  contextStack.pop();
  tokenizer.popContext();

  //QCString indent;
  //indent.fill(' ',contextStack.size()*2+2);
  //printf("%sdocParserPopContext() count=%zu\n",qPrint(indent),context.nodeStack.size());
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
QCString DocParser::findAndCopyImage(const QCString &fileName, DocImage::Type type, bool doWarn)
{
  QCString result;
  bool ambig;
  FileDef *fd = findFileDef(Doxygen::imageNameLinkedMap,fileName,ambig);
  //printf("Search for %s\n",fileName);
  if (fd)
  {
    if (ambig & doWarn)
    {
      QCString text;
      text.sprintf("image file name %s is ambiguous.\n",qPrint(fileName));
      text+="Possible candidates:\n";
      text+=showFileDefMatches(Doxygen::imageNameLinkedMap,fileName);
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"%s", qPrint(text));
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
        warn_doc_error(context.fileName,tokenizer.getLineNr(),
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
      warn_doc_error(context.fileName,tokenizer.getLineNr(),
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
    if (result.left(5)!="http:" && result.left(6)!="https:" && doWarn)
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),
           "image file %s is not found in IMAGE_PATH: "
	   "assuming external image.",qPrint(fileName)
          );
    }
  }
  return result;
}

/*! Collects the parameters found with \@param command
 *  in a list context.paramsFound. If
 *  the parameter is not an actual parameter of the current
 *  member context.memberDef, then a warning is raised (unless warnings
 *  are disabled altogether).
 */
void DocParser::checkArgumentName()
{
  if (!Config_getBool(WARN_IF_DOC_ERROR)) return;
  if (context.memberDef==0) return; // not a member
  std::string name = context.token->name.str();
  const ArgumentList &al=context.memberDef->isDocsForDefinition() ?
	                 context.memberDef->argumentList() :
                         context.memberDef->declArgumentList();
  SrcLangExt lang      = context.memberDef->getLanguage();
  //printf("isDocsForDefinition()=%d\n",context.memberDef->isDocsForDefinition());
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
      QCString argName = context.memberDef->isDefine() ? a.type : a.name;
      if (lang==SrcLangExt_Fortran) argName=argName.lower();
      argName=argName.stripWhiteSpace();
      //printf("argName='%s' aName=%s\n",qPrint(argName),qPrint(aName));
      if (argName.right(3)=="...") argName=argName.left(argName.length()-3);
      if (aName==argName)
      {
	context.paramsFound.insert(aName.str());
	found=TRUE;
	break;
      }
    }
    if (!found)
    {
      //printf("member type=%d\n",context.memberDef->memberType());
      QCString scope=context.memberDef->getScopeString();
      if (!scope.isEmpty()) scope+="::"; else scope="";
      QCString inheritedFrom = "";
      QCString docFile = context.memberDef->docFile();
      int docLine = context.memberDef->docLine();
      const MemberDef *inheritedMd = context.memberDef->inheritsDocsFrom();
      if (inheritedMd) // documentation was inherited
      {
        inheritedFrom.sprintf(" inherited from member %s at line "
            "%d in file %s",qPrint(inheritedMd->name()),
            inheritedMd->docLine(),qPrint(inheritedMd->docFile()));
        docFile = context.memberDef->getDefFileName();
        docLine = context.memberDef->getDefLine();
      }
      QCString alStr = argListToString(al);
      warn_doc_error(docFile,docLine,
	  "argument '%s' of command @param "
	  "is not found in the argument list of %s%s%s%s",
	  qPrint(aName), qPrint(scope), qPrint(context.memberDef->name()),
	  qPrint(alStr), qPrint(inheritedFrom));
    }
  }
}
/*! Collects the return values found with \@retval command
 *  in a global list g_parserContext.retvalsFound.
 */
void DocParser::checkRetvalName()
{
  QCString name = context.token->name;
  if (!Config_getBool(WARN_IF_DOC_ERROR)) return;
  if (context.memberDef==0 || name.isEmpty()) return; // not a member or no valid name
  if (context.retvalsFound.count(name.str())==1) // only report the first double entry
  {
     warn_doc_error(context.memberDef->getDefFileName(),
                    context.memberDef->getDefLine(),
                    "%s",
                    qPrint("return value '" + name + "' of " +
                    QCString(context.memberDef->qualifiedName()) +
                    " has multiple documentation sections"));
  }
  context.retvalsFound.insert(name.str());
}

/*! Checks if the parameters that have been specified using \@param are
 *  indeed all parameters and that a parameter does not have multiple
 *  \@param blocks.
 *  Must be called after checkArgumentName() has been called for each
 *  argument.
 */
void DocParser::checkUnOrMultipleDocumentedParams()
{
  if (context.memberDef && context.hasParamCommand)
  {
    const ArgumentList &al=context.memberDef->isDocsForDefinition() ?
      context.memberDef->argumentList() :
      context.memberDef->declArgumentList();
    SrcLangExt lang = context.memberDef->getLanguage();
    if (!al.empty())
    {
      ArgumentList undocParams;
      for (const Argument &a: al)
      {
        QCString argName = context.memberDef->isDefine() ? a.type : a.name;
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
          size_t count = context.paramsFound.count(argName.str());
          if (count==0 && a.docs.isEmpty())
          {
            undocParams.push_back(a);
          }
          else if (count>1 && Config_getBool(WARN_IF_DOC_ERROR))
          {
            warn_doc_error(context.memberDef->getDefFileName(),
                           context.memberDef->getDefLine(),
                           "%s",
                           qPrint("argument '" + aName +
                           "' from the argument list of " +
                           QCString(context.memberDef->qualifiedName()) +
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
            QCString(context.memberDef->qualifiedName()) +
            QCString(argListToString(al)) +
            (undocParams.size()>1 ? " are" : " is") + " not documented:\n";
        for (const Argument &a : undocParams)
        {
          QCString argName = context.memberDef->isDefine() ? a.type : a.name;
          if (lang==SrcLangExt_Fortran) argName = argName.lower();
          argName=argName.stripWhiteSpace();
          if (!first) errMsg+="\n";
          first=FALSE;
          errMsg+="  parameter '"+argName+"'";
        }
        warn_incomplete_doc(context.memberDef->getDefFileName(),
                            context.memberDef->getDefLine(),
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
 *  context (g_parserContext.context). The resulting documentation string is
 *  put in pDoc, the definition in which the documentation was found is
 *  put in pDef.
 *  @retval TRUE if name was found.
 *  @retval FALSE if name was not found.
 */
bool DocParser::findDocsForMemberOrCompound(const QCString &commandName,
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
    size_t rightBracePos = cmdArg.find("}", static_cast<int>(qstrlen("anonymous_namespace{")));
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
      context.context.find('.')==-1?context.context:QCString(), // find('.') is a hack to detect files
      name,
      args.isEmpty() ? QCString() : args,
      md,cd,fd,nd,gd,FALSE,0,TRUE);
  //printf("found=%d context=%s name=%s\n",found,qPrint(context.context),qPrint(name));
  if (found && md)
  {
    *pDoc=md->documentation();
    *pBrief=md->briefDescription();
    *pDef=md;
    return TRUE;
  }


  int scopeOffset=(int)context.context.length();
  do // for each scope
  {
    QCString fullName=cmdArg;
    if (scopeOffset>0)
    {
      fullName.prepend(context.context.left(scopeOffset)+"::");
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
      scopeOffset = context.context.findRev("::",scopeOffset-1);
      if (scopeOffset==-1) scopeOffset=0;
    }
  } while (scopeOffset>=0);


  return FALSE;
}

//---------------------------------------------------------------------------
void DocParser::errorHandleDefaultToken(DocNode *parent,int tok,
                                        DocNodeList &children,const QCString &txt)
{
  const char *cmd_start = "\\";
  switch (tok)
  {
    case TK_COMMAND_AT:
      cmd_start = "@";
      // fall through
    case TK_COMMAND_BS:
      children.push_back(std::make_unique<DocWord>(*this,parent,TK_COMMAND_CHAR(tok) + context.token->name));
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"Illegal command %s as part of a %s",
       qPrint(cmd_start + context.token->name),qPrint(txt));
      break;
    case TK_SYMBOL:
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"Unsupported symbol %s found found as part of a %s",
           qPrint(context.token->name), qPrint(txt));
      break;
    default:
      children.push_back(std::make_unique<DocWord>(*this,parent,context.token->name));
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"Unexpected token %s found as part of a %s",
        DocTokenizer::tokToString(tok), qPrint(txt));
      break;
  }
}

//---------------------------------------------------------------------------

int DocParser::handleStyleArgument(DocNode *parent,DocNodeList &children,const QCString &cmdName)
{
  DBG(("handleStyleArgument(%s)\n",qPrint(cmdName)));
  QCString saveCmdName = cmdName;
  int tok=tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"expected whitespace after \\%s command",
	qPrint(saveCmdName));
    return tok;
  }
  while ((tok=tokenizer.lex()) &&
          tok!=TK_WHITESPACE &&
          tok!=TK_NEWPARA &&
          tok!=TK_LISTITEM &&
          tok!=TK_ENDLIST
        )
  {
    static const reg::Ex specialChar(R"([.,|()\[\]:;?])");
    if (tok==TK_WORD && context.token->name.length()==1 &&
        reg::match(context.token->name.str(),specialChar))
    {
      // special character that ends the markup command
      return tok;
    }
    if (!defaultHandleToken(parent,tok,children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          if (insideLI(parent) && Mappers::htmlTagMapper->map(context.token->name) && context.token->endTag)
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
void DocParser::handleStyleEnter(DocNode *parent,DocNodeList &children,
          DocStyleChange::Style s,const QCString &tagName,const HtmlAttribList *attribs)
{
  DBG(("HandleStyleEnter\n"));
  DocStyleChange *sc= new DocStyleChange(*this,parent,(uint)context.nodeStack.size(),s,tagName,TRUE,attribs);
  children.push_back(std::unique_ptr<DocStyleChange>(sc));
  context.styleStack.push(sc);
}

/*! Called when a style change ends. For instance a \</b\> command is
 *  encountered.
 */
void DocParser::handleStyleLeave(DocNode *parent,DocNodeList &children,
         DocStyleChange::Style s,const QCString &tagName)
{
  DBG(("HandleStyleLeave\n"));
  QCString tagNameLower = QCString(tagName).lower();
  if (context.styleStack.empty() ||                            // no style change
      context.styleStack.top()->style()!=s ||                  // wrong style change
      context.styleStack.top()->tagName()!=tagNameLower ||     // wrong style change
      context.styleStack.top()->position()!=context.nodeStack.size() // wrong position
     )
  {
    if (context.styleStack.empty())
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"found </%s> tag without matching <%s>",
          qPrint(tagName),qPrint(tagName));
    }
    else if (context.styleStack.top()->tagName()!=tagNameLower)
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"found </%s> tag while expecting </%s>",
          qPrint(tagName),qPrint(context.styleStack.top()->tagName()));
    }
    else if (context.styleStack.top()->style()!=s)
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"found </%s> tag while expecting </%s>",
          qPrint(tagName),qPrint(context.styleStack.top()->tagName()));
    }
    else
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"found </%s> at different nesting level (%zu) than expected (%d)",
          qPrint(tagName),context.nodeStack.size(),context.styleStack.top()->position());
    }
  }
  else // end the section
  {
    children.push_back(
        std::make_unique<DocStyleChange>(
          *this,parent,(uint)context.nodeStack.size(),s,context.styleStack.top()->tagName(),FALSE));
    context.styleStack.pop();
  }
}

/*! Called at the end of a paragraph to close all open style changes
 *  (e.g. a <b> without a </b>). The closed styles are pushed onto a stack
 *  and entered again at the start of a new paragraph.
 */
void DocParser::handlePendingStyleCommands(DocNode *parent,DocNodeList &children)
{
  if (!context.styleStack.empty())
  {
    const DocStyleChange *sc = context.styleStack.top();
    while (sc && sc->position()>=context.nodeStack.size())
    { // there are unclosed style modifiers in the paragraph
      children.push_back(std::make_unique<DocStyleChange>(*this,parent,(uint)context.nodeStack.size(),sc->style(),sc->tagName(),FALSE));
      context.initialStyleStack.push(sc);
      context.styleStack.pop();
      sc = !context.styleStack.empty() ? context.styleStack.top() : 0;
    }
  }
}

void DocParser::handleInitialStyleCommands(DocPara *parent,DocNodeList &children)
{
  while (!context.initialStyleStack.empty())
  {
    const DocStyleChange *sc = context.initialStyleStack.top();
    handleStyleEnter(parent,children,sc->style(),sc->tagName(),&sc->attribs());
    context.initialStyleStack.pop();
  }
}

int DocParser::handleAHref(DocNode *parent,DocNodeList &children,
                           const HtmlAttribList &tagHtmlAttribs)
{
  uint index=0;
  int retval = RetVal_OK;
  for (const auto &opt : tagHtmlAttribs)
  {
    if (opt.name=="name" || opt.name=="id") // <a name=label> or <a id=label> tag
    {
      if (!opt.value.isEmpty())
      {
        children.push_back(std::make_unique<DocAnchor>(*this,parent,opt.value,TRUE));
        break; // stop looking for other tag attribs
      }
      else
      {
        warn_doc_error(context.fileName,tokenizer.getLineNr(),"found <a> tag with name option but without value!");
      }
    }
    else if (opt.name=="href") // <a href=url>..</a> tag
    {
      // copy attributes
      HtmlAttribList attrList = tagHtmlAttribs;
      // and remove the href attribute
      attrList.erase(attrList.begin()+index);
      QCString relPath;
      if (opt.value.at(0) != '#') relPath = context.relPath;
      DocHRef *href = new DocHRef(*this,parent,attrList,opt.value,relPath,convertNameToFile(context.fileName,FALSE,TRUE));
      children.push_back(std::unique_ptr<DocHRef>(href));
      context.insideHtmlLink=TRUE;
      retval = href->parse();
      context.insideHtmlLink=FALSE;
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

void DocParser::handleUnclosedStyleCommands()
{
  if (!context.initialStyleStack.empty())
  {
    const DocStyleChange *sc = context.initialStyleStack.top();
    context.initialStyleStack.pop();
    handleUnclosedStyleCommands();
    warn_doc_error(context.fileName,tokenizer.getLineNr(),
             "end of comment block while expecting "
             "command </%s>",qPrint(sc->tagName()));
  }
}

void DocParser::handleLinkedWord(DocNode *parent,DocNodeList &children,bool ignoreAutoLinkFlag)
{
  QCString name = linkToText(SrcLangExt_Unknown,context.token->name,TRUE);
  static bool autolinkSupport = Config_getBool(AUTOLINK_SUPPORT);
  if (!autolinkSupport && !ignoreAutoLinkFlag) // no autolinking -> add as normal word
  {
    children.push_back(std::make_unique<DocWord>(*this,parent,name));
    return;
  }

  // ------- try to turn the word 'name' into a link

  const Definition *compound=0;
  const MemberDef  *member=0;
  uint len = context.token->name.length();
  ClassDef *cd=0;
  bool ambig;
  FileDef *fd = findFileDef(Doxygen::inputNameLinkedMap,context.fileName,ambig);
  //printf("handleLinkedWord(%s) context.context=%s\n",qPrint(context.token->name),qPrint(context.context));
  if (!context.insideHtmlLink &&
      (resolveRef(context.context,context.token->name,context.inSeeBlock,&compound,&member,TRUE,fd,TRUE)
       || (!context.context.isEmpty() &&  // also try with global scope
           resolveRef("",context.token->name,context.inSeeBlock,&compound,&member,FALSE,0,TRUE))
      )
     )
  {
    //printf("resolveRef %s = %p (linkable?=%d)\n",qPrint(context.token->name),member,member ? member->isLinkable() : FALSE);
    if (member && member->isLinkable()) // member link
    {
      if (member->isObjCMethod())
      {
        bool localLink = context.memberDef ? member->getClassDef()==context.memberDef->getClassDef() : FALSE;
        name = member->objCMethodName(localLink,context.inSeeBlock);
      }
      children.push_back(
         std::make_unique<DocLinkedWord>(
            *this,parent,name,
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
        name=context.token->name;
      }
      else if (compound->definitionType()==Definition::TypeGroup)
      {
        name=toGroupDef(compound)->groupTitle();
      }
      children.push_back(
          std::make_unique<DocLinkedWord>(
            *this,parent,name,
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
             *this,parent,context.token->name,
             compound->getReference(),
             compound->getSourceFileBase(),
             "",
             compound->briefDescriptionAsTooltip()));
    }
    else // not linkable
    {
      children.push_back(std::make_unique<DocWord>(*this,parent,name));
    }
  }
  else if (!context.insideHtmlLink && len>1 && context.token->name.at(len-1)==':')
  {
    // special case, where matching Foo: fails to be an Obj-C reference,
    // but Foo itself might be linkable.
    context.token->name=context.token->name.left(len-1);
    handleLinkedWord(parent,children,ignoreAutoLinkFlag);
    children.push_back(std::make_unique<DocWord>(*this,parent,":"));
  }
  else if (!context.insideHtmlLink && (cd=getClass(context.token->name+"-p")))
  {
    // special case 2, where the token name is not a class, but could
    // be a Obj-C protocol
    children.push_back(
        std::make_unique<DocLinkedWord>(
          *this,parent,name,
          cd->getReference(),
          cd->getOutputFileBase(),
          cd->anchor(),
          cd->briefDescriptionAsTooltip()));
  }
  else // normal non-linkable word
  {
    if (context.token->name.left(1)=="#" || context.token->name.left(2)=="::")
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"explicit link request to '%s' could not be resolved",qPrint(name));
      children.push_back(std::make_unique<DocWord>(*this,parent,context.token->name));
    }
    else
    {
      children.push_back(std::make_unique<DocWord>(*this,parent,name));
    }
  }
}

void DocParser::handleParameterType(DocNode *parent,DocNodeList &children,const QCString &paramTypes)
{
  QCString name = context.token->name; // save token name
  QCString name1;
  int p=0,i,ii;
  while ((i=paramTypes.find('|',p))!=-1)
  {
    name1 = paramTypes.mid(p,i-p);
    ii=name1.find('[');
    context.token->name=ii!=-1 ? name1.mid(0,ii) : name1; // take part without []
    handleLinkedWord(parent,children);
    if (ii!=-1) children.push_back(std::make_unique<DocWord>(*this,parent,name1.mid(ii))); // add [] part
    p=i+1;
    children.push_back(std::make_unique<DocSeparator>(*this,parent,"|"));
  }
  name1 = paramTypes.mid(p);
  ii=name1.find('[');
  context.token->name=ii!=-1 ? name1.mid(0,ii) : name1;
  handleLinkedWord(parent,children);
  if (ii!=-1) children.push_back(std::make_unique<DocWord>(*this,parent,name1.mid(ii)));
  context.token->name = name; // restore original token name
}

DocInternalRef *DocParser::handleInternalRef(DocNode *parent)
{
  //printf("CMD_INTERNALREF\n");
  int tok=tokenizer.lex();
  QCString tokenName = context.token->name;
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(tokenName));
    return 0;
  }
  tokenizer.setStateInternalRef();
  tok=tokenizer.lex(); // get the reference id
  if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(tokenName));
    return 0;
  }
  return new DocInternalRef(*this,parent,context.token->name);
}

DocAnchor *DocParser::handleAnchor(DocNode *parent)
{
  int tok=tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(context.token->name));
    return 0;
  }
  tokenizer.setStateAnchor();
  tok=tokenizer.lex();
  if (tok==0)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(context.token->name));
    return 0;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(context.token->name));
    return 0;
  }
  tokenizer.setStatePara();
  return new DocAnchor(*this,parent,context.token->name,FALSE);
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
void DocParser::defaultHandleTitleAndSize(const int cmd, DocNode *parent, DocNodeList &children, QCString &width,QCString &height)
{
  auto ns = AutoNodeStack(*this,parent);

  // parse title
  tokenizer.setStateTitle();
  int tok;
  while ((tok=tokenizer.lex()))
  {
    if (tok==TK_WORD && (context.token->name=="width=" || context.token->name=="height="))
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
    tok=tokenizer.lex();
  }
  while (tok==TK_WHITESPACE || tok==TK_WORD) // there are values following the title
  {
    if(tok == TK_WORD)
    {
      if (context.token->name=="width=" || context.token->name=="height=")
      {
        tokenizer.setStateTitleAttrValue();
        context.token->name = context.token->name.left(context.token->name.length()-1);
      }

      if (context.token->name=="width")
      {
        width = context.token->chars;
      }
      else if (context.token->name=="height")
      {
        height = context.token->chars;
      }
      else
      {
        warn_doc_error(context.fileName,tokenizer.getLineNr(),"Unknown option '%s' after \\%s command, expected 'width' or 'height'",
                       qPrint(context.token->name), qPrint(Mappers::cmdMapper->find(cmd)));
        break;
      }
    }

    tok=tokenizer.lex();
  }
  tokenizer.setStatePara();

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
bool DocParser::defaultHandleToken(DocNode *parent,int tok, DocNodeList &children,bool handleWord)
{
  DBG(("token %s at %d",DocTokenizer::tokToString(tok),tokenizer.getLineNr()));
  if (tok==TK_WORD || tok==TK_LNKWORD || tok==TK_SYMBOL || tok==TK_URL ||
      tok==TK_COMMAND_AT || tok==TK_COMMAND_BS || tok==TK_HTMLTAG
     )
  {
    DBG((" name=%s",qPrint(context.token->name)));
  }
  DBG(("\n"));
reparsetoken:
  QCString tokenName = context.token->name;
  switch (tok)
  {
    case TK_COMMAND_AT:
        // fall through
    case TK_COMMAND_BS:
      switch (Mappers::cmdMapper->map(tokenName))
      {
        case CMD_BSLASH:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_BSlash));
          break;
        case CMD_AT:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_At));
          break;
        case CMD_LESS:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Less));
          break;
        case CMD_GREATER:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Greater));
          break;
        case CMD_AMP:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Amp));
          break;
        case CMD_DOLLAR:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Dollar));
          break;
        case CMD_HASH:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Hash));
          break;
        case CMD_DCOLON:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_DoubleColon));
          break;
        case CMD_PERCENT:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Percent));
          break;
        case CMD_NDASH:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Minus));
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Minus));
          break;
        case CMD_MDASH:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Minus));
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Minus));
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Minus));
          break;
        case CMD_QUOTE:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Quot));
          break;
        case CMD_PUNT:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Dot));
          break;
        case CMD_PLUS:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Plus));
          break;
        case CMD_MINUS:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Minus));
          break;
        case CMD_EQUAL:
          children.push_back(std::make_unique<DocSymbol>(*this,parent,DocSymbol::Sym_Equal));
          break;
        case CMD_EMPHASIS:
          {
            children.push_back(std::make_unique<DocStyleChange>(*this,parent,(uint)context.nodeStack.size(),DocStyleChange::Italic,tokenName,TRUE));
            tok=handleStyleArgument(parent,children,tokenName);
            children.push_back(std::make_unique<DocStyleChange>(*this,parent,(uint)context.nodeStack.size(),DocStyleChange::Italic,tokenName,FALSE));
            if (tok!=TK_WORD) children.push_back(std::make_unique<DocWhiteSpace>(*this,parent," "));
            if (tok==TK_NEWPARA) goto handlepara;
            else if (tok==TK_WORD || tok==TK_HTMLTAG)
            {
	      DBG(("CMD_EMPHASIS: reparsing command %s\n",qPrint(context.token->name)));
              goto reparsetoken;
            }
          }
          break;
        case CMD_BOLD:
          {
            children.push_back(std::make_unique<DocStyleChange>(*this,parent,(uint)context.nodeStack.size(),DocStyleChange::Bold,tokenName,TRUE));
            tok=handleStyleArgument(parent,children,tokenName);
            children.push_back(std::make_unique<DocStyleChange>(*this,parent,(uint)context.nodeStack.size(),DocStyleChange::Bold,tokenName,FALSE));
            if (tok!=TK_WORD) children.push_back(std::make_unique<DocWhiteSpace>(*this,parent," "));
            if (tok==TK_NEWPARA) goto handlepara;
            else if (tok==TK_WORD || tok==TK_HTMLTAG)
            {
	      DBG(("CMD_BOLD: reparsing command %s\n",qPrint(context.token->name)));
              goto reparsetoken;
            }
          }
          break;
        case CMD_CODE:
          {
            children.push_back(std::make_unique<DocStyleChange>(*this,parent,(uint)context.nodeStack.size(),DocStyleChange::Code,tokenName,TRUE));
            tok=handleStyleArgument(parent,children,tokenName);
            children.push_back(std::make_unique<DocStyleChange>(*this,parent,(uint)context.nodeStack.size(),DocStyleChange::Code,tokenName,FALSE));
            if (tok!=TK_WORD) children.push_back(std::make_unique<DocWhiteSpace>(*this,parent," "));
            if (tok==TK_NEWPARA) goto handlepara;
            else if (tok==TK_WORD || tok==TK_HTMLTAG)
            {
	      DBG(("CMD_CODE: reparsing command %s\n",qPrint(context.token->name)));
              goto reparsetoken;
            }
          }
          break;
        case CMD_HTMLONLY:
          {
            tokenizer.setStateHtmlOnly();
            tok = tokenizer.lex();
            children.push_back(std::make_unique<DocVerbatim>(*this,parent,context.context,context.token->verb,DocVerbatim::HtmlOnly,context.isExample,context.exampleName,context.token->name=="block"));
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"htmlonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_MANONLY:
          {
            tokenizer.setStateManOnly();
            tok = tokenizer.lex();
            children.push_back(std::make_unique<DocVerbatim>(*this,parent,context.context,context.token->verb,DocVerbatim::ManOnly,context.isExample,context.exampleName));
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"manonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_RTFONLY:
          {
            tokenizer.setStateRtfOnly();
            tok = tokenizer.lex();
            children.push_back(std::make_unique<DocVerbatim>(*this,parent,context.context,context.token->verb,DocVerbatim::RtfOnly,context.isExample,context.exampleName));
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"rtfonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_LATEXONLY:
          {
            tokenizer.setStateLatexOnly();
            tok = tokenizer.lex();
            children.push_back(std::make_unique<DocVerbatim>(*this,parent,context.context,context.token->verb,DocVerbatim::LatexOnly,context.isExample,context.exampleName));
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"latexonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_XMLONLY:
          {
            tokenizer.setStateXmlOnly();
            tok = tokenizer.lex();
            children.push_back(std::make_unique<DocVerbatim>(*this,parent,context.context,context.token->verb,DocVerbatim::XmlOnly,context.isExample,context.exampleName));
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"xmlonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_DBONLY:
          {
            tokenizer.setStateDbOnly();
            tok = tokenizer.lex();
            children.push_back(std::make_unique<DocVerbatim>(*this,parent,context.context,context.token->verb,DocVerbatim::DocbookOnly,context.isExample,context.exampleName));
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"docbookonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_FORMULA:
          {
            children.push_back(std::make_unique<DocFormula>(*this,parent,context.token->id));
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
            tokenizer.setStatePara();
          }
          break;
        case CMD_SETSCOPE:
          {
            QCString scope;
            tokenizer.setStateSetScope();
            (void)tokenizer.lex();
            scope = context.token->name;
            context.context = scope;
            //printf("Found scope='%s'\n",qPrint(scope));
            tokenizer.setStatePara();
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
            warn_doc_error(context.fileName,tokenizer.getLineNr(),"found <div> tag in heading\n");
            break;
          case HTML_PRE:
            warn_doc_error(context.fileName,tokenizer.getLineNr(),"found <pre> tag in heading\n");
            break;
          case HTML_BOLD:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Bold,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Bold,tokenName);
            }
            break;
          case HTML_S:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::S,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::S,tokenName);
            }
            break;
          case HTML_STRIKE:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Strike,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Strike,tokenName);
            }
            break;
          case HTML_DEL:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Del,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Del,tokenName);
            }
            break;
          case HTML_UNDERLINE:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Underline,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Underline,tokenName);
            }
            break;
          case HTML_INS:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Ins,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Ins,tokenName);
            }
            break;
          case HTML_CODE:
          case XML_C:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Code,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Code,tokenName);
            }
            break;
          case HTML_EMPHASIS:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Italic,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Italic,tokenName);
            }
            break;
          case HTML_SUB:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Subscript,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Subscript,tokenName);
            }
            break;
          case HTML_SUP:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Superscript,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Superscript,tokenName);
            }
            break;
          case HTML_CENTER:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Center,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Center,tokenName);
            }
            break;
          case HTML_SMALL:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Small,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Small,tokenName);
            }
            break;
          case HTML_IMG:
            if (!context.token->endTag)
              handleImg(parent,children,context.token->attribs);
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
          children.push_back(std::make_unique<DocSymbol>(*this,parent,s));
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
        children.push_back(std::make_unique<DocWhiteSpace>(*this,parent,context.token->chars));
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
        children.push_back(std::make_unique<DocWord>(*this,parent,context.token->name));
      }
      else
        return FALSE;
      break;
    case TK_URL:
      if (context.insideHtmlLink)
      {
        children.push_back(std::make_unique<DocWord>(*this,parent,context.token->name));
      }
      else
      {
        children.push_back(std::make_unique<DocURL>(*this,parent,context.token->name,context.token->isEMailAddr));
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

//---------------------------------------------------------------------------

void DocParser::handleImg(DocNode *parent, DocNodeList &children,const HtmlAttribList &tagHtmlAttribs)
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
            *this,parent,attrList,
            findAndCopyImage(opt.value,t,false),
            t,opt.value));
      found = TRUE;
    }
    ++index;
  }
  if (!found)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"IMG tag does not have a SRC attribute!\n");
  }
}

//---------------------------------------------------------------------------

DocSymbol::SymType DocSymbol::decodeSymbol(const QCString &symName)
{
  DBG(("decodeSymbol(%s)\n",qPrint(symName)));
  return HtmlEntityMapper::instance()->name2sym(symName);
}

//---------------------------------------------------------------------------

DocEmoji::DocEmoji(DocParser &parser,DocNode *parent,const QCString &symName) :
      DocNode(parser), m_symName(symName), m_index(-1)
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
    warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"Found unsupported emoji symbol '%s'\n",qPrint(m_symName));
  }
}

//---------------------------------------------------------------------------

int DocParser::internalValidatingParseDoc(DocNode *parent,DocNodeList &children,
                                    const QCString &doc)
{
  int retval = RetVal_OK;

  if (doc.isEmpty()) return retval;

  tokenizer.init(doc.data(),context.fileName,context.markdownSupport);

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
    DocPara *par = new DocPara(*this,parent);
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

void DocParser::readTextFileByName(const QCString &file,QCString &text)
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
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"included file name %s is ambiguous"
           "Possible candidates:\n%s",qPrint(file),
           qPrint(showFileDefMatches(Doxygen::exampleNameLinkedMap,file))
          );
    }
  }
  else
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"included file %s is not found. "
           "Check your EXAMPLE_PATH",qPrint(file));
  }
}

//---------------------------------------------------------------------------

DocWord::DocWord(DocParser &parser,DocNode *parent,const QCString &word) :
      DocNode(parser), m_word(word)
{
  m_parent = parent;
  //printf("new word %s url=%s\n",qPrint(word),qPrint(parser.context.searchUrl));
  if (Doxygen::searchIndex && !parser.context.searchUrl.isEmpty())
  {
    Doxygen::searchIndex->addWord(word,FALSE);
  }
}

//---------------------------------------------------------------------------

DocLinkedWord::DocLinkedWord(DocParser &parser,DocNode *parent,const QCString &word,
                  const QCString &ref,const QCString &file,
                  const QCString &anchor,const QCString &tooltip) :
      DocNode(parser), m_word(word), m_ref(ref),
      m_file(file), m_relPath(parser.context.relPath), m_anchor(anchor),
      m_tooltip(tooltip)
{
  m_parent = parent;
  //printf("DocLinkedWord: new word %s url=%s tooltip='%s'\n",
  //    qPrint(word),qPrint(parser.context.searchUrl),qPrint(tooltip));
  if (Doxygen::searchIndex && !parser.context.searchUrl.isEmpty())
  {
    Doxygen::searchIndex->addWord(word,FALSE);
  }
}

//---------------------------------------------------------------------------

DocAnchor::DocAnchor(DocParser &parser,DocNode *parent,const QCString &id,bool newAnchor) : DocNode(parser)
{
  m_parent = parent;
  if (id.isEmpty())
  {
    warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"Empty anchor label");
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
      warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"Invalid cite anchor id '%s'",qPrint(id));
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
      warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"Invalid anchor id '%s'",qPrint(id));
      m_anchor = "invalid";
      m_file = "invalid";
    }
  }
}

//---------------------------------------------------------------------------

DocVerbatim::DocVerbatim(DocParser &parser,DocNode *parent,const QCString &context,
    const QCString &text, Type t,bool isExample,
    const QCString &exampleFile,bool isBlock,const QCString &lang)
  : DocNode(parser), m_context(context), m_text(text), m_type(t),
    m_isExample(isExample), m_exampleFile(exampleFile),
    m_relPath(parser.context.relPath), m_lang(lang), m_isBlock(isBlock)
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
      m_parser.readTextFileByName(m_file,m_text);
      m_parser.context.includeFileName   = m_file;
      m_parser.context.includeFileText   = m_text;
      m_parser.context.includeFileOffset = 0;
      m_parser.context.includeFileLength = m_text.length();
      m_parser.context.includeFileLine   = 0;
      m_parser.context.includeFileShowLineNo = (m_type == DontIncWithLines || m_type == IncWithLines);
      //printf("parser.context.includeFile=<<%s>>\n",qPrint(parser.context.includeFileText));
      break;
    case VerbInclude:
      // fall through
    case HtmlInclude:
    case LatexInclude:
    case DocInclude::RtfInclude:
    case DocInclude::ManInclude:
    case DocInclude::XmlInclude:
    case DocInclude::DocbookInclude:
      m_parser.readTextFileByName(m_file,m_text);
      break;
    case Snippet:
    case SnipWithLines:
      m_parser.readTextFileByName(m_file,m_text);
      // check here for the existence of the blockId inside the file, so we
      // only generate the warning once.
      int count;
      if (!m_blockId.isEmpty() && (count=m_text.contains(m_blockId.data()))!=2)
      {
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"block marked with %s for \\snippet should appear twice in file %s, found it %d times\n",
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
  if (m_parser.context.includeFileName.isEmpty())
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),
                   "No previous '\\include' or '\\dontinclude' command for '\\%s' present",
                   typeAsString());
  }

  m_includeFileName = m_parser.context.includeFileName;
  const char *p = m_parser.context.includeFileText.data();
  uint l = m_parser.context.includeFileLength;
  uint o = m_parser.context.includeFileOffset;
  int il = m_parser.context.includeFileLine;
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
          m_parser.context.includeFileLine++;
          if (nonEmpty) break; // we have a pattern to match
          so=o+1; // no pattern, skip empty line
        }
        else if (!isspace((uchar)c)) // no white space char
        {
          nonEmpty=TRUE;
        }
        o++;
      }
      if (m_parser.context.includeFileText.mid(so,o-so).find(m_pattern)!=-1)
      {
        m_line  = il;
        m_text = m_parser.context.includeFileText.mid(so,o-so);
        DBG(("DocIncOperator::parse() Line: %s\n",qPrint(m_text)));
      }
      m_parser.context.includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = m_parser.context.includeFileShowLineNo;
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
            m_parser.context.includeFileLine++;
            if (nonEmpty) break; // we have a pattern to match
            so=o+1; // no pattern, skip empty line
          }
          else if (!isspace((uchar)c)) // no white space char
          {
            nonEmpty=TRUE;
          }
          o++;
        }
        if (m_parser.context.includeFileText.mid(so,o-so).find(m_pattern)!=-1)
        {
          m_line  = il;
          m_text = m_parser.context.includeFileText.mid(so,o-so);
          DBG(("DocIncOperator::parse() SkipLine: %s\n",qPrint(m_text)));
          break;
        }
        o++; // skip new line
      }
      m_parser.context.includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = m_parser.context.includeFileShowLineNo;
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
            m_parser.context.includeFileLine++;
            if (nonEmpty) break; // we have a pattern to match
            so=o+1; // no pattern, skip empty line
          }
          else if (!isspace((uchar)c)) // no white space char
          {
            nonEmpty=TRUE;
          }
          o++;
        }
        if (m_parser.context.includeFileText.mid(so,o-so).find(m_pattern)!=-1)
        {
          break;
        }
        o++; // skip new line
      }
      m_parser.context.includeFileOffset = so; // set pointer to start of new line
      m_showLineNo = m_parser.context.includeFileShowLineNo;
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
            m_parser.context.includeFileLine++;
            if (nonEmpty) break; // we have a pattern to match
            so=o+1; // no pattern, skip empty line
          }
          else if (!isspace((uchar)c)) // no white space char
          {
            nonEmpty=TRUE;
          }
          o++;
        }
        if (m_parser.context.includeFileText.mid(so,o-so).find(m_pattern)!=-1)
        {
          m_line  = il;
          m_text = m_parser.context.includeFileText.mid(bo,o-bo);
          DBG(("DocIncOperator::parse() Until: %s\n",qPrint(m_text)));
          break;
        }
        o++; // skip new line
      }
      m_parser.context.includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = m_parser.context.includeFileShowLineNo;
      break;
  }
}

//---------------------------------------------------------------------------

DocXRefItem::DocXRefItem(DocParser &parser,DocNode *parent,int id,const QCString &key) :
   CompAccept<DocXRefItem>(parser), m_id(id), m_key(key), m_relPath(parser.context.relPath)
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
      if (m_parser.context.memberDef && m_parser.context.memberDef->name().at(0)=='@')
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
        m_parser.pushContext();
        m_parser.internalValidatingParseDoc(this,m_children,item->text());
        m_parser.popContext();
      }
    }
    return TRUE;
  }
  return FALSE;
}

//---------------------------------------------------------------------------

DocFormula::DocFormula(DocParser &parser,DocNode *parent,int id) : DocNode(parser),
      m_relPath(parser.context.relPath)
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
    warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"Wrong formula id %d",id);
    m_id = -1;
  }
}

//---------------------------------------------------------------------------

//int DocLanguage::parse()
//{
//  int retval;
//  DBG(("DocLanguage::parse() start\n"));
//  auto ns = AutoNodeStack(m_parser,this);
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

DocSecRefItem::DocSecRefItem(DocParser &parser,DocNode *parent,const QCString &target) :
      CompAccept<DocSecRefItem>(parser), m_target(target), m_relPath(parser.context.relPath)
{
  m_parent = parent;
}

void DocSecRefItem::parse()
{
  DBG(("DocSecRefItem::parse() start\n"));
  auto ns = AutoNodeStack(m_parser,this);

  m_parser.tokenizer.setStateTitle();
  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children))
    {
      m_parser.errorHandleDefaultToken(this,tok,m_children,"\\refitem");
    }
  }
  m_parser.tokenizer.setStatePara();
  m_parser.handlePendingStyleCommands(this,m_children);

  if (!m_target.isEmpty())
  {
    SrcLangExt lang = getLanguageFromFileName(m_target);
    const SectionInfo *sec = SectionManager::instance().find(m_target);
    if (sec==0 && lang==SrcLangExt_Markdown) // lookup as markdown file
    {
      sec = SectionManager::instance().find(markdownFileNameToId(m_target));
    }
    if (sec) // ref to section or anchor
    {
      // set defaults
      m_ref       = sec->ref();
      m_file      = stripKnownExtensions(sec->fileName());
      m_refType   = Section;
      m_anchor    = sec->label();
      m_isSubPage = false;
      // adjust if needed
      switch (sec->type())
      {
        case SectionType::Page:
          {
            PageDef *pd = Doxygen::pageLinkedMap->find(m_target);
            m_isSubPage = pd && pd->hasParentPage();
            if (!m_isSubPage)
            {
              m_anchor="";
            }
          }
          break;
        case SectionType::Anchor:
          m_refType = Anchor;
          break;
        case SectionType::Table:
          m_refType = Table;
          break;
        default:
          break;
      }
      //printf("m_ref=%s,m_file=%s,type=%d\n",
      //    qPrint(m_ref),qPrint(m_file),m_refType);
    }
    else
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"reference to unknown section %s",
          qPrint(m_target));
    }
  }
  else
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"reference to empty target");
  }

  DBG(("DocSecRefItem::parse() end\n"));
}

//---------------------------------------------------------------------------

void DocSecRefList::parse()
{
  DBG(("DocSecRefList::parse() start\n"));
  auto ns = AutoNodeStack(m_parser,this);

  int tok=m_parser.tokenizer.lex();
  // skip white space
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=m_parser.tokenizer.lex();
  // handle items
  while (tok)
  {
    if (tok==TK_COMMAND_AT || tok == TK_COMMAND_BS)
    {
      const char *cmd_start = (tok==TK_COMMAND_AT ? "@" : "\\");
      switch (Mappers::cmdMapper->map(m_parser.context.token->name))
      {
        case CMD_SECREFITEM:
          {
            tok=m_parser.tokenizer.lex();
            if (tok!=TK_WHITESPACE)
            {
              warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\refitem command");
              break;
            }
            tok=m_parser.tokenizer.lex();
            if (tok!=TK_WORD && tok!=TK_LNKWORD)
            {
              warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of \\refitem",
                  DocTokenizer::tokToString(tok));
              break;
            }

            DocSecRefItem *item = new DocSecRefItem(m_parser,this,m_parser.context.token->name);
            m_children.push_back(std::unique_ptr<DocSecRefItem>(item));
            item->parse();
          }
          break;
        case CMD_ENDSECREFLIST:
          goto endsecreflist;
        default:
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Illegal command %s as part of a \\secreflist",
              qPrint(cmd_start + m_parser.context.token->name));
          goto endsecreflist;
      }
    }
    else if (tok==TK_WHITESPACE)
    {
      // ignore whitespace
    }
    else
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected token %s inside section reference list",
          DocTokenizer::tokToString(tok));
      goto endsecreflist;
    }
    tok=m_parser.tokenizer.lex();
  }

endsecreflist:
  DBG(("DocSecRefList::parse() end\n"));
}

//---------------------------------------------------------------------------

DocInternalRef::DocInternalRef(DocParser &parser,DocNode *parent,const QCString &ref)
  : CompAccept<DocInternalRef>(parser), m_relPath(parser.context.relPath)
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocInternalRef::parse() start\n"));

  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children))
    {
      m_parser.errorHandleDefaultToken(this,tok,m_children,"\\ref");
    }
  }

  m_parser.handlePendingStyleCommands(this,m_children);
  DBG(("DocInternalRef::parse() end\n"));
}

//---------------------------------------------------------------------------

DocRef::DocRef(DocParser &parser,DocNode *parent,const QCString &target,const QCString &context) :
   CompAccept<DocRef>(parser), m_refType(Unknown), m_isSubPage(FALSE)
{
  m_parent = parent;
  const Definition  *compound = 0;
  QCString     anchor;
  //printf("DocRef::DocRef(target=%s,context=%s)\n",qPrint(target),qPrint(context));
  ASSERT(!target.isEmpty());
  SrcLangExt lang = getLanguageFromFileName(target);
  m_relPath = parser.context.relPath;
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
        bool localLink = parser.context.memberDef ? member->getClassDef()==parser.context.memberDef->getClassDef() : FALSE;
        m_text = member->objCMethodName(localLink,parser.context.inSeeBlock);
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
  warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"unable to resolve reference to '%s' for \\ref command",
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocRef::parse() start\n"));

  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          break;
        default:
          m_parser.errorHandleDefaultToken(this,tok,m_children,"\\ref");
          break;
      }
    }
  }

  if (m_children.empty() && !m_text.isEmpty())
  {
    m_parser.context.insideHtmlLink=TRUE;
    m_parser.pushContext();
    m_parser.internalValidatingParseDoc(this,m_children,m_text);
    m_parser.popContext();
    m_parser.context.insideHtmlLink=FALSE;
    flattenParagraphs(this,m_children);
  }

  m_parser.handlePendingStyleCommands(this,m_children);
}

//---------------------------------------------------------------------------

DocCite::DocCite(DocParser &parser,DocNode *parent,const QCString &target,const QCString &) : DocNode(parser)
{
  size_t numBibFiles = Config_getList(CITE_BIB_FILES).size();
  m_parent = parent;
  //printf("DocCite::DocCite(target=%s)\n",qPrint(target));
  ASSERT(!target.isEmpty());
  m_relPath = parser.context.relPath;
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
    warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"\\cite command found but no bib files specified via CITE_BIB_FILES!");
  }
  else if (cite==0)
  {
    warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"unable to resolve reference to '%s' for \\cite command",
             qPrint(target));
  }
  else
  {
    warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"\\cite command to '%s' does not have an associated number",
             qPrint(target));
  }
}

//---------------------------------------------------------------------------

DocLink::DocLink(DocParser &parser,DocNode *parent,const QCString &target) : CompAccept<DocLink>(parser)
{
  m_parent = parent;
  const Definition *compound = 0;
  QCString anchor;
  m_refText = target;
  m_relPath = parser.context.relPath;
  if (!m_refText.isEmpty() && m_refText.at(0)=='#')
  {
    m_refText = m_refText.right(m_refText.length()-1);
  }
  if (resolveLink(parser.context.context,stripKnownExtensions(target),parser.context.inSeeBlock,&compound,anchor))
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
  warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"unable to resolve link to '%s' for \\link command",
         qPrint(target));
}


QCString DocLink::parse(bool isJavaLink,bool isXmlLink)
{
  QCString result;
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocLink::parse() start\n"));

  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children,FALSE))
    {
      const char *cmd_start = "\\";
      switch (tok)
      {
        case TK_COMMAND_AT:
          cmd_start = "@";
        // fall through
        case TK_COMMAND_BS:
          switch (Mappers::cmdMapper->map(m_parser.context.token->name))
          {
            case CMD_ENDLINK:
              if (isJavaLink)
              {
                warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"{@link.. ended with @endlink command");
              }
              goto endlink;
            default:
              warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Illegal command %s as part of a \\link",
                  qPrint(cmd_start + m_parser.context.token->name));
              break;
          }
          break;
        case TK_SYMBOL:
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unsupported symbol %s found as part of a \\link",
              qPrint(m_parser.context.token->name));
          break;
        case TK_HTMLTAG:
          if (m_parser.context.token->name!="see" || !isXmlLink)
          {
            warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected xml/html command %s found as part of a \\link",
                qPrint(m_parser.context.token->name));
          }
          goto endlink;
        case TK_LNKWORD:
        case TK_WORD:
          if (isJavaLink) // special case to detect closing }
          {
            QCString w = m_parser.context.token->name;
            int p;
            if (w=="}")
            {
              goto endlink;
            }
            else if ((p=w.find('}'))!=-1)
            {
              uint l=w.length();
              m_children.push_back(std::make_unique<DocWord>(m_parser,this,w.left(p)));
              if ((uint)p<l-1) // something left after the } (for instance a .)
              {
                result=w.right((int)l-p-1);
              }
              goto endlink;
            }
          }
          m_children.push_back(std::make_unique<DocWord>(m_parser,this,m_parser.context.token->name));
          break;
        default:
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected token %s",
             DocTokenizer::tokToString(tok));
        break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " link command\n");
  }
endlink:

  if (m_children.empty()) // no link text
  {
    m_children.push_back(std::make_unique<DocWord>(m_parser,this,m_refText));
  }

  m_parser.handlePendingStyleCommands(this,m_children);
  DBG(("DocLink::parse() end\n"));
  return result;
}


//---------------------------------------------------------------------------

DocDotFile::DocDotFile(DocParser &parser,DocNode *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile,int srcLine) :
  DocDiagramFileBase(parser,name,context,srcFile,srcLine)
{
  m_relPath = parser.context.relPath;
  m_parent = parent;
}

bool DocDotFile::parse()
{
  bool ok = false;
  m_parser.defaultHandleTitleAndSize(CMD_DOTFILE,this,m_children,m_width,m_height);

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
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"included dot file name %s is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(m_name),
           qPrint(showFileDefMatches(Doxygen::dotFileNameLinkedMap,m_name))
          );
    }
  }
  else
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"included dot file %s is not found "
           "in any of the paths specified via DOTFILE_DIRS!",qPrint(m_name));
  }
  return ok;
}

DocMscFile::DocMscFile(DocParser &parser,DocNode *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile, int srcLine) :
  DocDiagramFileBase(parser,name,context,srcFile,srcLine)
{
  m_relPath = parser.context.relPath;
  m_parent = parent;
}

bool DocMscFile::parse()
{
  bool ok = false;
  m_parser.defaultHandleTitleAndSize(CMD_MSCFILE,this,m_children,m_width,m_height);

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
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"included msc file name %s is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(m_name),
           qPrint(showFileDefMatches(Doxygen::mscFileNameLinkedMap,m_name))
          );
    }
  }
  else
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"included msc file %s is not found "
           "in any of the paths specified via MSCFILE_DIRS!",qPrint(m_name));
  }
  return ok;
}

//---------------------------------------------------------------------------

DocDiaFile::DocDiaFile(DocParser &parser,DocNode *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile,int srcLine) :
  DocDiagramFileBase(parser,name,context,srcFile,srcLine)
{
  m_relPath = parser.context.relPath;
  m_parent = parent;
}

bool DocDiaFile::parse()
{
  bool ok = false;
  m_parser.defaultHandleTitleAndSize(CMD_DIAFILE,this,m_children,m_width,m_height);

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
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"included dia file name %s is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(m_name),
           qPrint(showFileDefMatches(Doxygen::diaFileNameLinkedMap,m_name))
          );
    }
  }
  else
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"included dia file %s is not found "
           "in any of the paths specified via DIAFILE_DIRS!",qPrint(m_name));
  }
  return ok;
}

//---------------------------------------------------------------------------

DocVhdlFlow::DocVhdlFlow(DocParser &parser,DocNode *parent) : CompAccept<DocVhdlFlow>(parser)
{
  m_parent = parent;
}

void DocVhdlFlow::parse()
{
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocVhdlFlow::parse() start\n"));

  m_parser.tokenizer.setStateTitle();
  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children))
    {
      m_parser.errorHandleDefaultToken(this,tok,m_children,"\\vhdlflow");
    }
  }
  tok=m_parser.tokenizer.lex();

  m_parser.tokenizer.setStatePara();
  m_parser.handlePendingStyleCommands(this,m_children);

  DBG(("DocVhdlFlow::parse() end\n"));
  VhdlDocGen::createFlowChart(m_parser.context.memberDef);
}


//---------------------------------------------------------------------------

DocImage::DocImage(DocParser &parser,DocNode *parent,const HtmlAttribList &attribs,const QCString &name,
                   Type t,const QCString &url, bool inlineImage) :
      CompAccept<DocImage>(parser), m_attribs(attribs), m_name(name),
      m_type(t), m_relPath(parser.context.relPath),
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
  m_parser.defaultHandleTitleAndSize(CMD_IMAGE,this,m_children,m_width,m_height);
}


//---------------------------------------------------------------------------

int DocHtmlHeader::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlHeader::parse() start\n"));

  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
            if (tagId==HTML_H1 && m_parser.context.token->endTag) // found </h1> tag
            {
              if (m_level!=1)
              {
                warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"<h%d> ended with </h1>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H2 && m_parser.context.token->endTag) // found </h2> tag
            {
              if (m_level!=2)
              {
                warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"<h%d> ended with </h2>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H3 && m_parser.context.token->endTag) // found </h3> tag
            {
              if (m_level!=3)
              {
                warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"<h%d> ended with </h3>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H4 && m_parser.context.token->endTag) // found </h4> tag
            {
              if (m_level!=4)
              {
                warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"<h%d> ended with </h4>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H5 && m_parser.context.token->endTag) // found </h5> tag
            {
              if (m_level!=5)
              {
                warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"<h%d> ended with </h5>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H6 && m_parser.context.token->endTag) // found </h6> tag
            {
              if (m_level!=6)
              {
                warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"<h%d> ended with </h6>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_A)
            {
              if (!m_parser.context.token->endTag)
              {
                m_parser.handleAHref(this,m_children,m_parser.context.token->attribs);
              }
            }
            else if (tagId==HTML_BR)
            {
              m_children.push_back(std::make_unique<DocLineBreak>(m_parser,this,m_parser.context.token->attribs));
            }
            else
            {
              warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected html tag <%s%s> found within <h%d> context",
                  m_parser.context.token->endTag?"/":"",qPrint(m_parser.context.token->name),m_level);
            }
          }
          break;
        default:
	  char tmp[20];
	  sprintf(tmp,"<h%d>tag",m_level);
          m_parser.errorHandleDefaultToken(this,tok,m_children,tmp);
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <h%d> tag\n",m_level);
  }
endheader:
  m_parser.handlePendingStyleCommands(this,m_children);
  DBG(("DocHtmlHeader::parse() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocHRef::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHRef::parse() start\n"));

  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
            if (tagId==HTML_A && m_parser.context.token->endTag) // found </a> tag
            {
              goto endhref;
            }
            else
            {
              warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected html tag <%s%s> found within <a href=...> context",
                  m_parser.context.token->endTag?"/":"",qPrint(m_parser.context.token->name));
            }
          }
          break;
        default:
          m_parser.errorHandleDefaultToken(this,tok,m_children,"<a>..</a> block");
          break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <a href=...> tag");
  }
endhref:
  m_parser.handlePendingStyleCommands(this,m_children);
  DBG(("DocHRef::parse() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocInternal::parse(int level)
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocInternal::parse() start\n"));

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  do
  {
    DocPara *par = new DocPara(m_parser,this);
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
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Invalid list item found");
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
    DocSection *s=new DocSection(m_parser,this,
        std::min(level+Doxygen::subpageNestingLevel,5),m_parser.context.token->sectionId);
    m_children.push_back(std::unique_ptr<DocSection>(s));
    retval = s->parse();
  }

  if (retval==RetVal_Internal)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"\\internal command found inside internal section");
  }

  DBG(("DocInternal::parse() end: retval=%x\n",retval));
  return retval;
}

//---------------------------------------------------------------------------

int DocIndexEntry::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocIndexEntry::parse() start\n"));
  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\addindex command");
    goto endindexentry;
  }
  m_parser.tokenizer.setStateTitle();
  m_entry="";
  while ((tok=m_parser.tokenizer.lex()))
  {
    switch (tok)
    {
      case TK_WHITESPACE:
        m_entry+=" ";
        break;
      case TK_WORD:
      case TK_LNKWORD:
        m_entry+=m_parser.context.token->name;
        break;
      case TK_SYMBOL:
        {
          DocSymbol::SymType s = DocSymbol::decodeSymbol(m_parser.context.token->name);
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
              warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected symbol found as argument of \\addindex");
              break;
          }
        }
        break;
      case TK_COMMAND_AT:
        // fall through
      case TK_COMMAND_BS:
        switch (Mappers::cmdMapper->map(m_parser.context.token->name))
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
               warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected command %s found as argument of \\addindex",
                              qPrint(m_parser.context.token->name));
               break;
        }
      break;
      default:
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected token %s",
            DocTokenizer::tokToString(tok));
        break;
    }
  }
  m_parser.tokenizer.setStatePara();
  m_entry = m_entry.stripWhiteSpace();
endindexentry:
  DBG(("DocIndexEntry::parse() end retval=%x\n",retval));
  return retval;
}

//---------------------------------------------------------------------------

DocHtmlCaption::DocHtmlCaption(DocParser &parser,DocNode *parent,const HtmlAttribList &attribs)
  : CompAccept<DocHtmlCaption>(parser)
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
        warn_doc_error(parser.context.fileName,parser.tokenizer.getLineNr(),"Invalid caption id '%s'",qPrint(opt.value));
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlCaption::parse() start\n"));
  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
            if (tagId==HTML_CAPTION && m_parser.context.token->endTag) // found </caption> tag
            {
              retval = RetVal_OK;
              goto endcaption;
            }
            else
            {
              warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected html tag <%s%s> found within <caption> context",
                  m_parser.context.token->endTag?"/":"",qPrint(m_parser.context.token->name));
            }
          }
          break;
        default:
          m_parser.errorHandleDefaultToken(this,tok,m_children,"<caption> tag");
          break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <caption> tag");
  }
endcaption:
  m_parser.handlePendingStyleCommands(this,m_children);
  DBG(("DocHtmlCaption::parse() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlCell::parse()
{
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlCell::parse() start\n"));

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(m_parser,this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
    if (retval==TK_HTMLTAG)
    {
      int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
      if (tagId==HTML_TD && m_parser.context.token->endTag) // found </dt> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
      else if (tagId==HTML_TH && m_parser.context.token->endTag) // found </th> tag
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlCell::parseXml() start\n"));

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(m_parser,this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
    if (retval==TK_HTMLTAG)
    {
      int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
      if (tagId==XML_ITEM && m_parser.context.token->endTag) // found </item> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
      else if (tagId==XML_DESCRIPTION && m_parser.context.token->endTag) // found </description> tag
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlRow::parse() start\n"));

  bool isHeading=FALSE;
  bool isFirst=TRUE;
  DocHtmlCell *cell=0;

  // get next token
  int tok=m_parser.tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=m_parser.tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
    if (tagId==HTML_TD && !m_parser.context.token->endTag) // found <td> tag
    {
    }
    else if (tagId==HTML_TH && !m_parser.context.token->endTag) // found <th> tag
    {
      isHeading=TRUE;
    }
    else // found some other tag
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <td> or <th> tag but "
          "found <%s> instead!",qPrint(m_parser.context.token->name));
      m_parser.tokenizer.pushBackHtmlTag(m_parser.context.token->name);
      goto endrow;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto endrow;
  }
  else // token other than html token
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <td> or <th> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto endrow;
  }

  // parse one or more cells
  do
  {
    cell=new DocHtmlCell(m_parser,this,m_parser.context.token->attribs,isHeading);
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlRow::parseXml() start\n"));

  bool isFirst=TRUE;
  DocHtmlCell *cell=0;

  // get next token
  int tok=m_parser.tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=m_parser.tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
    if (tagId==XML_TERM && !m_parser.context.token->endTag) // found <term> tag
    {
    }
    else if (tagId==XML_DESCRIPTION && !m_parser.context.token->endTag) // found <description> tag
    {
    }
    else // found some other tag
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <term> or <description> tag but "
          "found <%s> instead!",qPrint(m_parser.context.token->name));
      m_parser.tokenizer.pushBackHtmlTag(m_parser.context.token->name);
      goto endrow;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto endrow;
  }
  else // token other than html token
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <td> or <th> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto endrow;
  }

  do
  {
    cell=new DocHtmlCell(m_parser,this,m_parser.context.token->attribs,isHeading);
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlTable::parse() start\n"));

getrow:
  // get next token
  int tok=m_parser.tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=m_parser.tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
    if (tagId==HTML_TR && !m_parser.context.token->endTag) // found <tr> tag
    {
      // no caption, just rows
      retval=RetVal_TableRow;
    }
    else if (tagId==HTML_CAPTION && !m_parser.context.token->endTag) // found <caption> tag
    {
      if (m_caption)
      {
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"table already has a caption, found another one");
      }
      else
      {
        m_caption = new DocHtmlCaption(m_parser,this,m_parser.context.token->attribs);
        retval=m_caption->parse();

        if (retval==RetVal_OK) // caption was parsed ok
        {
          goto getrow;
        }
      }
    }
    else // found wrong token
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <tr> or <caption> tag but "
          "found <%s%s> instead!", m_parser.context.token->endTag ? "/" : "", qPrint(m_parser.context.token->name));
    }
  }
  else if (tok==0) // premature end of comment
  {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment while looking"
          " for a <tr> or <caption> tag");
  }
  else // token other than html token
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <tr> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
  }

  // parse one or more rows
  while (retval==RetVal_TableRow)
  {
    DocHtmlRow *tr=new DocHtmlRow(m_parser,this,m_parser.context.token->attribs);
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlTable::parseXml() start\n"));

  // get next token
  int tok=m_parser.tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=m_parser.tokenizer.lex();
  // should find a html tag now
  int tagId=0;
  bool isHeader=FALSE;
  if (tok==TK_HTMLTAG)
  {
    tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
    if (tagId==XML_ITEM && !m_parser.context.token->endTag) // found <item> tag
    {
      retval=RetVal_TableRow;
    }
    if (tagId==XML_LISTHEADER && !m_parser.context.token->endTag) // found <listheader> tag
    {
      retval=RetVal_TableRow;
      isHeader=TRUE;
    }
  }

  // parse one or more rows
  while (retval==RetVal_TableRow)
  {
    DocHtmlRow *tr=new DocHtmlRow(m_parser,this,m_parser.context.token->attribs);
    m_children.push_back(std::unique_ptr<DocHtmlRow>(tr));
    retval=tr->parseXml(isHeader);
    isHeader=FALSE;
  }

  computeTableGrid();

  DBG(("DocHtmlTable::parseXml() end\n"));
  tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
  return tagId==XML_LIST && m_parser.context.token->endTag ? RetVal_OK : retval;
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlDescTitle::parse() start\n"));

  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children))
    {
      const char *cmd_start = "\\";
      switch (tok)
      {
        case TK_COMMAND_AT:
          cmd_start = "@";
        // fall through
        case TK_COMMAND_BS:
          {
            QCString cmdName=m_parser.context.token->name;
            bool isJavaLink=FALSE;
            switch (Mappers::cmdMapper->map(cmdName))
            {
              case CMD_REF:
                {
                  tok=m_parser.tokenizer.lex();
                  if (tok!=TK_WHITESPACE)
                  {
                    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
                        qPrint(m_parser.context.token->name));
                  }
                  else
                  {
                    m_parser.tokenizer.setStateRef();
                    tok=m_parser.tokenizer.lex(); // get the reference id
                    if (tok!=TK_WORD)
                    {
                      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of \\%s command",
                          DocTokenizer::tokToString(tok),qPrint(cmdName));
                    }
                    else
                    {
                      DocRef *ref = new DocRef(m_parser,this,m_parser.context.token->name,m_parser.context.context);
                      m_children.push_back(std::unique_ptr<DocRef>(ref));
                      ref->parse();
                    }
                    m_parser.tokenizer.setStatePara();
                  }
                }
                break;
              case CMD_JAVALINK:
                isJavaLink=TRUE;
                // fall through
              case CMD_LINK:
                {
                  tok=m_parser.tokenizer.lex();
                  if (tok!=TK_WHITESPACE)
                  {
                    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
                        qPrint(cmdName));
                  }
                  else
                  {
                    m_parser.tokenizer.setStateLink();
                    tok=m_parser.tokenizer.lex();
                    if (tok!=TK_WORD)
                    {
                      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of \\%s command",
                          DocTokenizer::tokToString(tok),qPrint(cmdName));
                    }
                    else
                    {
                      m_parser.tokenizer.setStatePara();
                      DocLink *lnk = new DocLink(m_parser,this,m_parser.context.token->name);
                      m_children.push_back(std::unique_ptr<DocLink>(lnk));
                      QCString leftOver = lnk->parse(isJavaLink);
                      if (!leftOver.isEmpty())
                      {
                        m_children.push_back(std::make_unique<DocWord>(m_parser,this,leftOver));
                      }
                    }
                  }
                }

                break;
              default:
                warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Illegal command %s found as part of a <dt> tag",
                  qPrint(cmd_start + m_parser.context.token->name));
            }
          }
          break;
        case TK_SYMBOL:
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unsupported symbol \\%s found as part of a <dt> tag",
              qPrint(m_parser.context.token->name));
          break;
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
            if (tagId==HTML_DD && !m_parser.context.token->endTag) // found <dd> tag
            {
              retval = RetVal_DescData;
              goto endtitle;
            }
            else if (tagId==HTML_DT && m_parser.context.token->endTag)
            {
              // ignore </dt> tag.
            }
            else if (tagId==HTML_DT)
            {
              // missing <dt> tag.
              retval = RetVal_DescTitle;
              goto endtitle;
            }
            else if (tagId==HTML_DL && m_parser.context.token->endTag)
            {
              retval=RetVal_EndDesc;
              goto endtitle;
            }
            else if (tagId==HTML_A)
            {
              if (!m_parser.context.token->endTag)
              {
                m_parser.handleAHref(this,m_children,m_parser.context.token->attribs);
              }
            }
            else
            {
              warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected html tag <%s%s> found within <dt> context",
                  m_parser.context.token->endTag?"/":"",qPrint(m_parser.context.token->name));
            }
          }
          break;
        default:
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected token %s found as part of a <dt> tag",
              DocTokenizer::tokToString(tok));
          break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected end of comment while inside"
        " <dt> tag");
  }
endtitle:
  m_parser.handlePendingStyleCommands(this,m_children);
  DBG(("DocHtmlDescTitle::parse() end\n"));
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlDescData::parse()
{
  m_attribs = m_parser.context.token->attribs;
  int retval=0;
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlDescData::parse() start\n"));

  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(m_parser,this);
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
  auto ns = AutoNodeStack(m_parser,this);
  DBG(("DocHtmlDescList::parse() start\n"));

  // get next token
  int tok=m_parser.tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=m_parser.tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
    if (tagId==HTML_DT && !m_parser.context.token->endTag) // found <dt> tag
    {
      // continue
    }
    else // found some other tag
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <dt> tag but "
          "found <%s> instead!",qPrint(m_parser.context.token->name));
      m_parser.tokenizer.pushBackHtmlTag(m_parser.context.token->name);
      goto enddesclist;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto enddesclist;
  }
  else // token other than html token
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <dt> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto enddesclist;
  }

  do
  {
    DocHtmlDescTitle *dt=new DocHtmlDescTitle(m_parser,this,m_parser.context.token->attribs);
    m_children.push_back(std::unique_ptr<DocHtmlDescTitle>(dt));
    DocHtmlDescData *dd=new DocHtmlDescData(m_parser,this);
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
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment while inside <dl> block");
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
  auto ns = AutoNodeStack(m_parser,this);

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(m_parser,this);
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
  auto ns = AutoNodeStack(m_parser,this);

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(m_parser,this);
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    m_children.push_back(std::unique_ptr<DocPara>(par));
    retval=par->parse();
    if (retval==0) break;

    //printf("new item: retval=%x m_parser.context.token->name=%s m_parser.context.token->endTag=%d\n",
    //    retval,qPrint(m_parser.context.token->name),m_parser.context.token->endTag);
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
  auto ns = AutoNodeStack(m_parser,this);

  // get next token
  int tok=m_parser.tokenizer.lex();
  // skip whitespace and paragraph breaks
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=m_parser.tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
    if (tagId==HTML_LI && !m_parser.context.token->endTag) // found <li> tag
    {
      // ok, we can go on.
    }
    else if (((m_type==Unordered && tagId==HTML_UL) ||
              (m_type==Ordered   && tagId==HTML_OL)
             ) && m_parser.context.token->endTag
            ) // found empty list
    {
      // add dummy item to obtain valid HTML
      m_children.push_back(std::make_unique<DocHtmlListItem>(m_parser,this,HtmlAttribList(),1));
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"empty list!");
      retval = RetVal_EndList;
      goto endlist;
    }
    else // found some other tag
    {
      // add dummy item to obtain valid HTML
      m_children.push_back(std::make_unique<DocHtmlListItem>(m_parser,this,HtmlAttribList(),1));
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <li> tag but "
          "found <%s%s> instead!",m_parser.context.token->endTag?"/":"",qPrint(m_parser.context.token->name));
      m_parser.tokenizer.pushBackHtmlTag(m_parser.context.token->name);
      goto endlist;
    }
  }
  else if (tok==0) // premature end of comment
  {
    // add dummy item to obtain valid HTML
    m_children.push_back(std::make_unique<DocHtmlListItem>(m_parser,this,HtmlAttribList(),1));
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html list item");
    goto endlist;
  }
  else // token other than html token
  {
    // add dummy item to obtain valid HTML
    m_children.push_back(std::make_unique<DocHtmlListItem>(m_parser,this,HtmlAttribList(),1));
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <li> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto endlist;
  }

  do
  {
    DocHtmlListItem *li=new DocHtmlListItem(m_parser,this,m_parser.context.token->attribs,num++);
    m_children.push_back(std::unique_ptr<DocHtmlListItem>(li));
    retval=li->parse();
  } while (retval==RetVal_ListItem);

  if (retval==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment while inside <%cl> block",
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
  auto ns = AutoNodeStack(m_parser,this);

  // get next token
  int tok=m_parser.tokenizer.lex();
  // skip whitespace and paragraph breaks
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=m_parser.tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(m_parser.context.token->name);
    //printf("m_parser.context.token->name=%s m_parser.context.token->endTag=%d\n",qPrint(m_parser.context.token->name),m_parser.context.token->endTag);
    if (tagId==XML_ITEM && !m_parser.context.token->endTag) // found <item> tag
    {
      // ok, we can go on.
    }
    else // found some other tag
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <item> tag but "
          "found <%s> instead!",qPrint(m_parser.context.token->name));
      m_parser.tokenizer.pushBackHtmlTag(m_parser.context.token->name);
      goto endlist;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html list item");
    goto endlist;
  }
  else // token other than html token
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected <item> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto endlist;
  }

  do
  {
    DocHtmlListItem *li=new DocHtmlListItem(m_parser,this,m_parser.context.token->attribs,num++);
    m_children.push_back(std::unique_ptr<DocHtmlListItem>(li));
    retval=li->parseXml();
    if (retval==0) break;
    //printf("retval=%x m_parser.context.token->name=%s\n",retval,qPrint(m_parser.context.token->name));
  } while (retval==RetVal_ListItem);

  if (retval==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment while inside <list type=\"%s\"> block",
        m_type==Unordered ? "bullet" : "number");
  }

endlist:
  DBG(("DocHtmlList::parseXml() end retval=%x\n",retval));
  return retval==RetVal_EndList ||
         (retval==RetVal_CloseXml || m_parser.context.token->name=="list") ?
         RetVal_OK : retval;
}

//--------------------------------------------------------------------------

int DocHtmlBlockQuote::parse()
{
  DBG(("DocHtmlBlockQuote::parse() start\n"));
  int retval=0;
  auto ns = AutoNodeStack(m_parser,this);

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(m_parser,this);
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
  auto ns = AutoNodeStack(m_parser,this);

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    par = new DocPara(m_parser,this);
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
  auto ns = AutoNodeStack(m_parser,this);
  int rv=m_paragraph->parse();
  m_paragraph->markFirst();
  m_paragraph->markLast();
  return rv;
}

//--------------------------------------------------------------------------

int DocSimpleList::parse()
{
  auto ns = AutoNodeStack(m_parser,this);
  int rv;
  do
  {
    DocSimpleListItem *li=new DocSimpleListItem(m_parser,this);
    m_children.push_back(std::unique_ptr<DocSimpleListItem>(li));
    rv=li->parse();
  } while (rv==RetVal_ListItem);
  return (rv!=TK_NEWPARA) ? rv : RetVal_OK;
}

//--------------------------------------------------------------------------

DocAutoListItem::DocAutoListItem(DocParser &parser,DocNode *parent,int indent,int num)
      : CompAccept<DocAutoListItem>(parser), m_indent(indent), m_itemNum(num)
{
  m_parent = parent;
}

int DocAutoListItem::parse()
{
  int retval = RetVal_OK;
  auto ns = AutoNodeStack(m_parser,this);

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  do
  {
    DocPara *par = new DocPara(m_parser,this);
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
  } while (retval==TK_NEWPARA && m_parser.context.token->indent>m_indent);
  if (lastPar) lastPar->markLast();

  //printf("DocAutoListItem: retval=%d indent=%d\n",retval,m_parser.context.token->indent);
  return retval;
}

//--------------------------------------------------------------------------

DocAutoList::DocAutoList(DocParser &parser,DocNode *parent,int indent,bool isEnumList,
                         int depth) :
      CompAccept<DocAutoList>(parser), m_indent(indent), m_isEnumList(isEnumList),
      m_depth(depth)
{
  m_parent = parent;
}

int DocAutoList::parse()
{
  int retval = RetVal_OK;
  int num=1;
  auto ns = AutoNodeStack(m_parser,this);
  m_parser.tokenizer.startAutoList();
	  // first item or sub list => create new list
  do
  {
    if (m_parser.context.token->id!=-1) // explicitly numbered list
    {
      num=m_parser.context.token->id;  // override num with real number given
    }
    DocAutoListItem *li = new DocAutoListItem(m_parser,this,m_indent,num++);
    m_children.push_back(std::unique_ptr<DocAutoListItem>(li));
    retval=li->parse();
    //printf("DocAutoList::parse(): retval=0x%x m_parser.context.token->indent=%d m_indent=%d "
    //       "m_isEnumList=%d m_parser.context.token->isEnumList=%d m_parser.context.token->name=%s\n",
    //       retval,m_parser.context.token->indent,m_indent,m_isEnumList,m_parser.context.token->isEnumList,
    //       qPrint(m_parser.context.token->name));
    //printf("num=%d m_parser.context.token->id=%d\n",num,m_parser.context.token->id);
  }
  while (retval==TK_LISTITEM &&                // new list item
         m_indent==m_parser.context.token->indent &&          // at same indent level
	 m_isEnumList==m_parser.context.token->isEnumList &&  // of the same kind
         (m_parser.context.token->id==-1 || m_parser.context.token->id>=num)  // increasing number (or no number)
        );

  m_parser.tokenizer.endAutoList();
  return retval;
}

//--------------------------------------------------------------------------

void DocTitle::parse()
{
  DBG(("DocTitle::parse() start\n"));
  auto ns = AutoNodeStack(m_parser,this);
  m_parser.tokenizer.setStateTitle();
  int tok;
  while ((tok=m_parser.tokenizer.lex()))
  {
    if (!m_parser.defaultHandleToken(this,tok,m_children))
    {
      m_parser.errorHandleDefaultToken(this,tok,m_children,"title section");
    }
  }
  m_parser.tokenizer.setStatePara();
  m_parser.handlePendingStyleCommands(this,m_children);
  DBG(("DocTitle::parse() end\n"));
}

void DocTitle::parseFromString(const QCString &text)
{
  m_children.push_back(std::make_unique<DocWord>(m_parser,this,text));
}

//--------------------------------------------------------------------------

DocSimpleSect::DocSimpleSect(DocParser &parser,DocNode *parent,Type t) :
     CompAccept<DocSimpleSect>(parser), m_type(t)
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
  auto ns = AutoNodeStack(m_parser,this);

  // handle case for user defined title
  if (userTitle)
  {
    m_title = new DocTitle(m_parser,this);
    m_title->parse();
  }

  // add new paragraph as child
  DocPara *par = new DocPara(m_parser,this);
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
  if (needsSeparator) m_children.push_back(std::make_unique<DocSimpleSectSep>(m_parser,this));
  m_children.push_back(std::unique_ptr<DocPara>(par));

  // parse the contents of the paragraph
  int retval = par->parse();

  DBG(("DocSimpleSect::parse() end retval=%d\n",retval));
  return retval; // 0==EOF, TK_NEWPARA, TK_LISTITEM, TK_ENDLIST, RetVal_SimpleSec
}

int DocSimpleSect::parseRcs()
{
  DBG(("DocSimpleSect::parseRcs() start\n"));
  auto ns = AutoNodeStack(m_parser,this);

  m_title = new DocTitle(m_parser,this);
  m_title->parseFromString(m_parser.context.token->name);

  QCString text = m_parser.context.token->text;
  m_parser.pushContext(); // this will create a new parser.context.token
  m_parser.internalValidatingParseDoc(this,m_children,text);
  m_parser.popContext(); // this will restore the old parser.context.token

  DBG(("DocSimpleSect::parseRcs()\n"));
  return RetVal_OK;
}

int DocSimpleSect::parseXml()
{
  DBG(("DocSimpleSect::parse() start\n"));
  auto ns = AutoNodeStack(m_parser,this);

  int retval = RetVal_OK;
  for (;;)
  {
    // add new paragraph as child
    DocPara *par = new DocPara(m_parser,this);
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
    p = new DocPara(m_parser,this);
    m_children.push_back(std::unique_ptr<DocPara>(p));
  }
  else
  {
    p = (DocPara *)m_children.back().get();

    // Comma-separate <seealso> links.
    p->injectToken(TK_WORD,",");
    p->injectToken(TK_WHITESPACE," ");
  }

  m_parser.context.inSeeBlock=TRUE;
  p->injectToken(TK_LNKWORD,word);
  m_parser.context.inSeeBlock=FALSE;
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
  auto ns = AutoNodeStack(m_parser,this);
  DocPara *par=0;
  QCString saveCmdName = cmdName;

  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    retval=RetVal_EndParBlock;
    goto endparamlist;
  }
  m_parser.tokenizer.setStateParam();
  tok=m_parser.tokenizer.lex();
  while (tok==TK_WORD) /* there is a parameter name */
  {
    if (m_type==DocParamSect::Param)
    {
      int typeSeparator = m_parser.context.token->name.find('#'); // explicit type position
      if (typeSeparator!=-1)
      {
        m_parser.handleParameterType(this,m_paramTypes,m_parser.context.token->name.left(typeSeparator));
        m_parser.context.token->name = m_parser.context.token->name.mid(typeSeparator+1);
        m_parser.context.hasParamCommand=TRUE;
        m_parser.checkArgumentName();
        ((DocParamSect*)parent())->m_hasTypeSpecifier=TRUE;
      }
      else
      {
        m_parser.context.hasParamCommand=TRUE;
        m_parser.checkArgumentName();
      }
    }
    else if (m_type==DocParamSect::RetVal)
    {
      m_parser.context.hasReturnCommand=TRUE;
      m_parser.checkRetvalName();
    }
    //m_params.append(m_parser.context.token->name);
    m_parser.handleLinkedWord(this,m_params);
    tok=m_parser.tokenizer.lex();
  }
  m_parser.tokenizer.setStatePara();
  if (tok==0) /* premature end of comment block */
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(saveCmdName));
    retval=RetVal_EndParBlock;
    goto endparamlist;
  }
  if (tok!=TK_WHITESPACE) /* premature end of comment block */
  {
    if (tok!=TK_NEWPARA) /* empty param description */
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token in comment block while parsing the "
          "argument of command %s",qPrint(saveCmdName));
    }
    retval=RetVal_EndParBlock;
    goto endparamlist;
  }

  par = new DocPara(m_parser,this);
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
  auto ns = AutoNodeStack(m_parser,this);

  m_parser.context.token->name = paramName;
  if (m_type==DocParamSect::Param)
  {
    m_parser.context.hasParamCommand=TRUE;
    m_parser.checkArgumentName();
  }
  else if (m_type==DocParamSect::RetVal)
  {
    m_parser.context.hasReturnCommand=TRUE;
    m_parser.checkRetvalName();
  }

  m_parser.handleLinkedWord(this,m_params);

  do
  {
    DocPara *par = new DocPara(m_parser,this);
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
           Mappers::htmlTagMapper->map(m_parser.context.token->name)!=XML_PARAM &&
           Mappers::htmlTagMapper->map(m_parser.context.token->name)!=XML_TYPEPARAM &&
           Mappers::htmlTagMapper->map(m_parser.context.token->name)!=XML_EXCEPTION);


  if (retval==0) /* premature end of comment block */
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unterminated param or exception tag");
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
  auto ns = AutoNodeStack(m_parser,this);

  if (d!=Unspecified)
  {
    m_hasInOutSpecifier=TRUE;
  }

  DocParamList *pl = new DocParamList(m_parser,this,m_type,d);
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
    ss=new DocSimpleSect(m_parser,this,t);
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
    ps=new DocParamSect(m_parser,this,t);
    m_children.push_back(std::unique_ptr<DocParamSect>(ps));
  }
  int rv=ps->parse(cmdName,xmlContext,(DocParamSect::Direction)direction);
  return (rv!=TK_NEWPARA) ? rv : RetVal_OK;
}

void DocPara::handleCite()
{
  // get the argument of the cite command.
  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint("cite"));
    return;
  }
  m_parser.tokenizer.setStateCite();
  tok=m_parser.tokenizer.lex();
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s\n", qPrint("cite"));
    return;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint("cite"));
    return;
  }
  m_parser.context.token->sectionId = m_parser.context.token->name;
  m_children.push_back(
      std::make_unique<DocCite>(
        m_parser,this,m_parser.context.token->name,m_parser.context.context));

  m_parser.tokenizer.setStatePara();
}

void DocPara::handleEmoji()
{
  // get the argument of the emoji command.
  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint("emoji"));
    return;
  }
  m_parser.tokenizer.setStateEmoji();
  tok=m_parser.tokenizer.lex();
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s\n", qPrint("emoji"));
    return;
  }
  else if (tok!=TK_WORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint("emoji"));
    return;
  }
  m_children.push_back(
      std::make_unique<DocEmoji>(
         m_parser,this,m_parser.context.token->name));
  m_parser.tokenizer.setStatePara();
}

int DocPara::handleXRefItem()
{
  int retval=m_parser.tokenizer.lex();
  ASSERT(retval==TK_WHITESPACE);
  m_parser.tokenizer.setStateXRefItem();
  retval=m_parser.tokenizer.lex();
  if (retval==RetVal_OK)
  {
    DocXRefItem *ref = new DocXRefItem(m_parser,this,m_parser.context.token->id,m_parser.context.token->name);
    if (ref->parse())
    {
      m_children.push_back(std::unique_ptr<DocXRefItem>(ref));
    }
    else
    {
      delete ref;
    }
  }
  m_parser.tokenizer.setStatePara();
  return retval;
}

void DocPara::handleIline()
{
  m_parser.tokenizer.setStateIline();
  int tok = m_parser.tokenizer.lex();
  if (tok!=TK_WORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"invalid argument for command '\\iline'\n");
    return;
  }
  m_parser.tokenizer.setStatePara();
}

void DocPara::handleIncludeOperator(const QCString &cmdName,DocIncOperator::Type t)
{
  QCString saveCmdName = cmdName;
  DBG(("handleIncludeOperator(%s)\n",qPrint(saveCmdName)));
  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  m_parser.tokenizer.setStatePattern();
  tok=m_parser.tokenizer.lex();
  m_parser.tokenizer.setStatePara();
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s", qPrint(saveCmdName));
    return;
  }
  else if (tok!=TK_WORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  DocIncOperator *op = new DocIncOperator(m_parser,this,t,m_parser.context.token->name,m_parser.context.context,m_parser.context.isExample,m_parser.context.exampleName);
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
  bool inlineImage = false;
  QCString anchorStr;

  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    if (tok==TK_WORD)
    {
      if (m_parser.context.token->name == "{")
      {
        m_parser.tokenizer.setStateOptions();
        tok=m_parser.tokenizer.lex();
        m_parser.tokenizer.setStatePara();
        StringVector optList=split(m_parser.context.token->name.str(),",");
        for (const auto &opt : optList)
        {
          if (opt.empty()) continue;
          QCString locOpt(opt);
          QCString locOptLow;
          locOpt = locOpt.stripWhiteSpace();
          locOptLow = locOpt.lower();
          if (locOptLow == "inline")
          {
            inlineImage = true;
          }
          else if (locOptLow.startsWith("anchor:"))
          {
            if (!anchorStr.isEmpty())
            {
               warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),
                  "multiple use of option 'anchor' for '%s' command, ignoring: '%s'",
                  qPrint(saveCmdName),qPrint(locOpt.mid(7)));
            }
            else
            {
               anchorStr = locOpt.mid(7);
            }
          }
          else
          {
            warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),
                  "unknown option '%s' for '%s' command specified",
                  qPrint(locOpt), qPrint(saveCmdName));
          }
        }
        tok=m_parser.tokenizer.lex();
        if (tok!=TK_WHITESPACE)
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
              qPrint(saveCmdName));
          return;
        }
      }
    }
    else
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
      return;
    }
  }
  tok=m_parser.tokenizer.lex();
  if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  DocImage::Type t;
  QCString imgType = m_parser.context.token->name.lower();
  if      (imgType=="html")    t=DocImage::Html;
  else if (imgType=="latex")   t=DocImage::Latex;
  else if (imgType=="docbook") t=DocImage::DocBook;
  else if (imgType=="rtf")     t=DocImage::Rtf;
  else
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"output format %s specified as the first argument of "
        "%s command is not valid",
        qPrint(imgType),qPrint(saveCmdName));
    return;
  }
  m_parser.tokenizer.setStateFile();
  tok=m_parser.tokenizer.lex();
  m_parser.tokenizer.setStatePara();
  if (tok!=TK_WORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  if (!anchorStr.isEmpty())
  {
    DocAnchor *anchor = new DocAnchor(m_parser,this,anchorStr,true);
    m_children.push_back(std::unique_ptr<DocAnchor>(anchor));
  }
  HtmlAttribList attrList;
  DocImage *img = new DocImage(m_parser,this,attrList,
                 m_parser.findAndCopyImage(m_parser.context.token->name,t),t,"",inlineImage);
  m_children.push_back(std::unique_ptr<DocImage>(img));
  img->parse();
}

template<class T>
void DocPara::handleFile(const QCString &cmdName)
{
  QCString saveCmdName = cmdName;
  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  m_parser.tokenizer.setStateFile();
  tok=m_parser.tokenizer.lex();
  m_parser.tokenizer.setStatePara();
  if (tok!=TK_WORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  QCString name = m_parser.context.token->name;
  T *df = new T(m_parser,this,name,m_parser.context.context,m_parser.context.fileName,m_parser.tokenizer.getLineNr());
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
  DocVhdlFlow *vf = new DocVhdlFlow(m_parser,this);
  m_children.push_back(std::unique_ptr<DocVhdlFlow>(vf));
  vf->parse();
}

void DocPara::handleLink(const QCString &cmdName,bool isJavaLink)
{
  QCString saveCmdName = cmdName;
  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  m_parser.tokenizer.setStateLink();
  tok=m_parser.tokenizer.lex();
  if (tok!=TK_WORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"%s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  m_parser.tokenizer.setStatePara();
  DocLink *lnk = new DocLink(m_parser,this,m_parser.context.token->name);
  m_children.push_back(std::unique_ptr<DocLink>(lnk));
  QCString leftOver = lnk->parse(isJavaLink);
  if (!leftOver.isEmpty())
  {
    m_children.push_back(std::make_unique<DocWord>(m_parser,this,leftOver));
  }
}

void DocPara::handleRef(const QCString &cmdName)
{
  QCString saveCmdName = cmdName;
  DBG(("handleRef(%s)\n",qPrint(saveCmdName)));
  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  m_parser.tokenizer.setStateRef();
  tok=m_parser.tokenizer.lex(); // get the reference id
  DocRef *ref=0;
  if (tok!=TK_WORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    goto endref;
  }
  ref = new DocRef(m_parser,this,m_parser.context.token->name,m_parser.context.context);
  m_children.push_back(std::unique_ptr<DocRef>(ref));
  ref->parse();
endref:
  m_parser.tokenizer.setStatePara();
}

void DocPara::handleInclude(const QCString &cmdName,DocInclude::Type t)
{
  DBG(("handleInclude(%s)\n",qPrint(cmdName)));
  QCString saveCmdName = cmdName;
  int tok=m_parser.tokenizer.lex();
  bool isBlock = false;
  if (tok==TK_WORD && m_parser.context.token->name=="{")
  {
    m_parser.tokenizer.setStateOptions();
    tok=m_parser.tokenizer.lex();
    m_parser.tokenizer.setStatePara();
    StringVector optList=split(m_parser.context.token->name.str(),",");
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
    tok=m_parser.tokenizer.lex();
    if (tok!=TK_WHITESPACE)
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
          qPrint(saveCmdName));
      return;
    }
  }
  else if (tok==TK_WORD && m_parser.context.token->name=="[")
  {
    m_parser.tokenizer.setStateBlock();
    tok=m_parser.tokenizer.lex();
    isBlock = (m_parser.context.token->name.stripWhiteSpace() == "block");
    m_parser.tokenizer.setStatePara();
    tok=m_parser.tokenizer.lex();
  }
  else if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  m_parser.tokenizer.setStateFile();
  tok=m_parser.tokenizer.lex();
  m_parser.tokenizer.setStatePara();
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(saveCmdName));
    return;
  }
  else if (tok!=TK_WORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  QCString fileName = m_parser.context.token->name;
  QCString blockId;
  if (t==DocInclude::Snippet || t==DocInclude::SnipWithLines || t==DocInclude::SnippetDoc)
  {
    if (fileName == "this") fileName=m_parser.context.fileName;
    m_parser.tokenizer.setStateSnippet();
    tok=m_parser.tokenizer.lex();
    m_parser.tokenizer.setStatePara();
    if (tok!=TK_WORD)
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected block identifier, but found token %s instead while parsing the %s command",
          DocTokenizer::tokToString(tok),qPrint(saveCmdName));
      return;
    }
    blockId = "["+m_parser.context.token->name+"]";
  }

  // This is the only place to handle the \includedoc and \snippetdoc commands,
  // as the content is included here as if it is really here.
  if (t==DocInclude::IncludeDoc || t==DocInclude::SnippetDoc)
  {
     QCString inc_text;
     int inc_line  = 1;
     m_parser.readTextFileByName(fileName,inc_text);
     if (t==DocInclude::SnippetDoc)
     {
       int count;
       if (!blockId.isEmpty() && (count=inc_text.contains(blockId.data()))!=2)
       {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"block marked with %s for \\snippet should appear twice in file %s, found it %d times\n",
            qPrint(blockId),qPrint(fileName),count);
       }
       inc_line = lineBlock(inc_text, blockId);
       inc_text = extractBlock(inc_text, blockId);
     }

     Markdown markdown(fileName,inc_line);
     QCString strippedDoc = stripIndentation(inc_text);
     QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(strippedDoc,inc_line) : strippedDoc;

     m_parser.pushContext();
     m_parser.context.fileName = fileName;
     m_parser.tokenizer.setLineNr(inc_line);
     m_parser.internalValidatingParseDoc(this,m_children,processedDoc);
     m_parser.popContext();
  }
  else
  {
    DocInclude *inc = new DocInclude(m_parser,this,fileName,m_parser.context.context,t,m_parser.context.isExample,m_parser.context.exampleName,blockId,isBlock);
    m_children.push_back(std::unique_ptr<DocInclude>(inc));
    inc->parse();
  }
}

void DocPara::handleSection(const QCString &cmdName)
{
  QCString saveCmdName = cmdName;
  // get the argument of the section command.
  int tok=m_parser.tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  tok=m_parser.tokenizer.lex();
  if (tok==0)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s\n", qPrint(saveCmdName));
    return;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  m_parser.context.token->sectionId = m_parser.context.token->name;
  m_parser.tokenizer.setStateSkipTitle();
  m_parser.tokenizer.lex();
  m_parser.tokenizer.setStatePara();
}

int DocPara::handleHtmlHeader(const HtmlAttribList &tagHtmlAttribs,int level)
{
  DocHtmlHeader *header = new DocHtmlHeader(m_parser,this,tagHtmlAttribs,level);
  m_children.push_back(std::unique_ptr<DocHtmlHeader>(header));
  int retval = header->parse();
  return (retval==RetVal_OK) ? TK_NEWPARA : retval;
}

// For XML tags whose content is stored in attributes rather than
// contained within the element, we need a way to inject the attribute
// text into the current paragraph.
bool DocPara::injectToken(int tok,const QCString &tokText)
{
  m_parser.context.token->name = tokText;
  return m_parser.defaultHandleToken(this,tok,m_children);
}

int DocPara::handleStartCode()
{
  int retval = m_parser.tokenizer.lex();
  QCString lang = m_parser.context.token->name;
  if (!lang.isEmpty() && lang.at(0)!='.')
  {
    lang="."+lang;
  }
  if (m_parser.context.xmlComment)
  {
    m_parser.context.token->verb = substitute(substitute(m_parser.context.token->verb,"&lt;","<"),"&gt;",">");
  }
  // search for the first non-whitespace line, index is stored in li
  int i=0,li=0,l=m_parser.context.token->verb.length();
  while (i<l && (m_parser.context.token->verb.at(i)==' ' || m_parser.context.token->verb.at(i)=='\n'))
  {
    if (m_parser.context.token->verb.at(i)=='\n') li=i+1;
    i++;
  }
  m_children.push_back(std::make_unique<DocVerbatim>(m_parser,this,m_parser.context.context,stripIndentation(m_parser.context.token->verb.mid(li)),DocVerbatim::Code,m_parser.context.isExample,m_parser.context.exampleName,FALSE,lang));
  if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"code section ended without end marker");
  m_parser.tokenizer.setStatePara();
  return retval;
}

void DocPara::handleInheritDoc()
{
  if (m_parser.context.memberDef) // inheriting docs from a member
  {
    const MemberDef *reMd = m_parser.context.memberDef->reimplements();
    if (reMd) // member from which was inherited.
    {
      const MemberDef *thisMd = m_parser.context.memberDef;
      //printf("{InheritDocs:%s=>%s}\n",qPrint(m_parser.context.memberDef->qualifiedName()),qPrint(reMd->qualifiedName()));
      m_parser.pushContext();
      m_parser.context.scope=reMd->getOuterScope();
      if (m_parser.context.scope!=Doxygen::globalScope)
      {
        m_parser.context.context=m_parser.context.scope->name();
      }
      m_parser.context.memberDef=reMd;
      while (!m_parser.context.styleStack.empty()) m_parser.context.styleStack.pop();
      while (!m_parser.context.nodeStack.empty()) m_parser.context.nodeStack.pop();
      m_parser.context.copyStack.push_back(reMd);
      m_parser.internalValidatingParseDoc(this,m_children,reMd->briefDescription());
      m_parser.internalValidatingParseDoc(this,m_children,reMd->documentation());
      m_parser.context.copyStack.pop_back();
      auto hasParamCommand   = m_parser.context.hasParamCommand;
      auto hasReturnCommand  = m_parser.context.hasReturnCommand;
      auto retvalsFound      = m_parser.context.retvalsFound;
      auto paramsFound       = m_parser.context.paramsFound;
      m_parser.popContext();
      m_parser.context.hasParamCommand      = hasParamCommand;
      m_parser.context.hasReturnCommand     = hasReturnCommand;
      m_parser.context.retvalsFound         = retvalsFound;
      m_parser.context.paramsFound          = paramsFound;
      m_parser.context.memberDef = thisMd;
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
      m_children.push_back(std::make_unique<DocWord>(m_parser,this,TK_COMMAND_CHAR(tok) + cmdName));
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Found unknown command '%s%s'",TK_COMMAND_CHAR(tok),qPrint(cmdName));
      break;
    case CMD_EMPHASIS:
      m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Italic,cmdName,TRUE));
      retval=m_parser.handleStyleArgument(this,m_children,cmdName);
      m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Italic,cmdName,FALSE));
      if (retval!=TK_WORD) m_children.push_back(std::make_unique<DocWhiteSpace>(m_parser,this," "));
      break;
    case CMD_BOLD:
      m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Bold,cmdName,TRUE));
      retval=m_parser.handleStyleArgument(this,m_children,cmdName);
      m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Bold,cmdName,FALSE));
      if (retval!=TK_WORD) m_children.push_back(std::make_unique<DocWhiteSpace>(m_parser,this," "));
      break;
    case CMD_CODE:
      m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Code,cmdName,TRUE));
      retval=m_parser.handleStyleArgument(this,m_children,cmdName);
      m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Code,cmdName,FALSE));
      if (retval!=TK_WORD) m_children.push_back(std::make_unique<DocWhiteSpace>(m_parser,this," "));
      break;
    case CMD_BSLASH:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_BSlash));
      break;
    case CMD_AT:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_At));
      break;
    case CMD_LESS:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Less));
      break;
    case CMD_GREATER:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Greater));
      break;
    case CMD_AMP:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Amp));
      break;
    case CMD_DOLLAR:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Dollar));
      break;
    case CMD_HASH:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Hash));
      break;
    case CMD_PIPE:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Pipe));
      break;
    case CMD_DCOLON:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_DoubleColon));
      break;
    case CMD_PERCENT:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Percent));
      break;
    case CMD_NDASH:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
      break;
    case CMD_MDASH:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
      break;
    case CMD_QUOTE:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Quot));
      break;
    case CMD_PUNT:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Dot));
      break;
    case CMD_PLUS:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Plus));
      break;
    case CMD_MINUS:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
      break;
    case CMD_EQUAL:
      m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Equal));
      break;
    case CMD_SA:
      m_parser.context.inSeeBlock=TRUE;
      retval = handleSimpleSection(DocSimpleSect::See);
      m_parser.context.inSeeBlock=FALSE;
      break;
    case CMD_RETURN:
      retval = handleSimpleSection(DocSimpleSect::Return);
      m_parser.context.hasReturnCommand=TRUE;
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
        DocSimpleList *sl=new DocSimpleList(m_parser,this);
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
        m_parser.tokenizer.setStateCode();
        retval = handleStartCode();
      }
      break;
    case CMD_HTMLONLY:
      {
        m_parser.tokenizer.setStateHtmlOnly();
        retval = m_parser.tokenizer.lex();
        m_children.push_back(std::make_unique<DocVerbatim>(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::HtmlOnly,m_parser.context.isExample,m_parser.context.exampleName,m_parser.context.token->name=="block"));
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"htmlonly section ended without end marker");
        m_parser.tokenizer.setStatePara();
      }
      break;
    case CMD_MANONLY:
      {
        m_parser.tokenizer.setStateManOnly();
        retval = m_parser.tokenizer.lex();
        m_children.push_back(std::make_unique<DocVerbatim>(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::ManOnly,m_parser.context.isExample,m_parser.context.exampleName));
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"manonly section ended without end marker");
        m_parser.tokenizer.setStatePara();
      }
      break;
    case CMD_RTFONLY:
      {
        m_parser.tokenizer.setStateRtfOnly();
        retval = m_parser.tokenizer.lex();
        m_children.push_back(std::make_unique<DocVerbatim>(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::RtfOnly,m_parser.context.isExample,m_parser.context.exampleName));
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"rtfonly section ended without end marker");
        m_parser.tokenizer.setStatePara();
      }
      break;
    case CMD_LATEXONLY:
      {
        m_parser.tokenizer.setStateLatexOnly();
        retval = m_parser.tokenizer.lex();
        m_children.push_back(std::make_unique<DocVerbatim>(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::LatexOnly,m_parser.context.isExample,m_parser.context.exampleName));
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"latexonly section ended without end marker");
        m_parser.tokenizer.setStatePara();
      }
      break;
    case CMD_XMLONLY:
      {
        m_parser.tokenizer.setStateXmlOnly();
        retval = m_parser.tokenizer.lex();
        m_children.push_back(std::make_unique<DocVerbatim>(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::XmlOnly,m_parser.context.isExample,m_parser.context.exampleName));
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"xmlonly section ended without end marker");
        m_parser.tokenizer.setStatePara();
      }
      break;
    case CMD_DBONLY:
      {
        m_parser.tokenizer.setStateDbOnly();
        retval = m_parser.tokenizer.lex();
        m_children.push_back(std::make_unique<DocVerbatim>(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::DocbookOnly,m_parser.context.isExample,m_parser.context.exampleName));
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"docbookonly section ended without end marker");
        m_parser.tokenizer.setStatePara();
      }
      break;
    case CMD_VERBATIM:
      {
        m_parser.tokenizer.setStateVerbatim();
        retval = m_parser.tokenizer.lex();
        m_children.push_back(std::make_unique<DocVerbatim>(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::Verbatim,m_parser.context.isExample,m_parser.context.exampleName));
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"verbatim section ended without end marker");
        m_parser.tokenizer.setStatePara();
      }
      break;
    case CMD_DOT:
      {
        DocVerbatim *dv = new DocVerbatim(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::Dot,m_parser.context.isExample,m_parser.context.exampleName);
        m_parser.tokenizer.setStatePara();
        QCString width,height;
        m_parser.defaultHandleTitleAndSize(CMD_DOT,dv,dv->children(),width,height);
        m_parser.tokenizer.setStateDot();
        retval = m_parser.tokenizer.lex();
        dv->setText(m_parser.context.token->verb);
        dv->setWidth(width);
        dv->setHeight(height);
        dv->setLocation(m_parser.context.fileName,m_parser.tokenizer.getLineNr());
        m_children.push_back(std::unique_ptr<DocVerbatim>(dv));
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"dot section ended without end marker");
        m_parser.tokenizer.setStatePara();
      }
      break;
    case CMD_MSC:
      {
        DocVerbatim *dv = new DocVerbatim(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::Msc,m_parser.context.isExample,m_parser.context.exampleName);
        m_parser.tokenizer.setStatePara();
        QCString width,height;
        m_parser.defaultHandleTitleAndSize(CMD_MSC,dv,dv->children(),width,height);
        m_parser.tokenizer.setStateMsc();
        retval = m_parser.tokenizer.lex();
        dv->setText(m_parser.context.token->verb);
        dv->setWidth(width);
        dv->setHeight(height);
        dv->setLocation(m_parser.context.fileName,m_parser.tokenizer.getLineNr());
        m_children.push_back(std::unique_ptr<DocVerbatim>(dv));
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"msc section ended without end marker");
        m_parser.tokenizer.setStatePara();
      }
      break;
    case CMD_STARTUML:
      {
        static QCString jarPath = Config_getString(PLANTUML_JAR_PATH);
        m_parser.tokenizer.setStatePlantUMLOpt();
        retval = m_parser.tokenizer.lex();

        QCString fullMatch = m_parser.context.token->sectionId;
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
             if (g_plantumlEngine.find(locOpt.str())!=g_plantumlEngine.end())
             {
               if (!engine.isEmpty())
               {
                 warn(m_parser.context.fileName,m_parser.tokenizer.getLineNr(), "Multiple definition of engine for '\\startuml'");
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
                 warn(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Multiple use of of filename for '\\startuml'");
               }
             }
           }
        }
        else
        {
          sectionId = m_parser.context.token->sectionId;
        }
        if (engine.isEmpty()) engine = "uml";

        if (sectionId.isEmpty())
        {
          m_parser.tokenizer.setStatePlantUMLOpt();
          retval = m_parser.tokenizer.lex();
          assert(retval==RetVal_OK);

          sectionId = m_parser.context.token->sectionId;
          sectionId = sectionId.stripWhiteSpace();
        }

        QCString plantFile(sectionId);
        DocVerbatim *dv = new DocVerbatim(m_parser,this,m_parser.context.context,m_parser.context.token->verb,DocVerbatim::PlantUML,FALSE,plantFile);
        dv->setEngine(engine);
        m_parser.tokenizer.setStatePara();
        QCString width,height;
        m_parser.defaultHandleTitleAndSize(CMD_STARTUML,dv,dv->children(),width,height);
        m_parser.tokenizer.setStatePlantUML();
        retval = m_parser.tokenizer.lex();
        int line = 0;
        QCString trimmedVerb = stripLeadingAndTrailingEmptyLines(m_parser.context.token->verb,line);
        if (engine == "ditaa")
        {
          dv->setUseBitmap(true);
        }
        else if (engine == "uml")
        {
          int i = trimmedVerb.find('\n');
          QCString firstLine = i==-1 ? trimmedVerb : trimmedVerb.left(i);
          if (firstLine.stripWhiteSpace() == "ditaa") dv->setUseBitmap(true);
        }
        dv->setText(trimmedVerb);
        dv->setWidth(width);
        dv->setHeight(height);
        dv->setLocation(m_parser.context.fileName,m_parser.tokenizer.getLineNr());
        if (jarPath.isEmpty())
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"ignoring \\startuml command because PLANTUML_JAR_PATH is not set");
          delete dv;
        }
        else
        {
          m_children.push_back(std::unique_ptr<DocVerbatim>(dv));
        }
        if (retval==0) warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"startuml section ended without end marker");
        m_parser.tokenizer.setStatePara();
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
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected command %s",qPrint(m_parser.context.token->name));
      break;
    case CMD_PARAM:
      retval = handleParamSection(cmdName,DocParamSect::Param,FALSE,m_parser.context.token->paramDir);
      break;
    case CMD_TPARAM:
      retval = handleParamSection(cmdName,DocParamSect::TemplateParam,FALSE,m_parser.context.token->paramDir);
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
        m_children.push_back(std::make_unique<DocLineBreak>(m_parser,this));
      }
      break;
    case CMD_ANCHOR:
      {
        DocAnchor *anchor = m_parser.handleAnchor(this);
        if (anchor)
        {
          m_children.push_back(std::unique_ptr<DocAnchor>(anchor));
        }
      }
      break;
    case CMD_ADDINDEX:
      {
        DocIndexEntry *ie = new DocIndexEntry(m_parser,this,
                     m_parser.context.scope!=Doxygen::globalScope?m_parser.context.scope:0,
                     m_parser.context.memberDef);
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
        DocParBlock *block = new DocParBlock(m_parser,this);
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
        DocSecRefList *list = new DocSecRefList(m_parser,this);
        m_children.push_back(std::unique_ptr<DocSecRefList>(list));
        list->parse();
      }
      break;
    case CMD_SECREFITEM:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected command %s",qPrint(m_parser.context.token->name));
      break;
    case CMD_ENDSECREFLIST:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected command %s",qPrint(m_parser.context.token->name));
      break;
    case CMD_FORMULA:
      {
        m_children.push_back(
            std::make_unique<DocFormula>(
              m_parser,this,m_parser.context.token->id));
      }
      break;
    //case CMD_LANGSWITCH:
    //  retval = handleLanguageSwitch();
    //  break;
    case CMD_INTERNALREF:
      //warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"unexpected command %s",qPrint(m_parser.context.token->name));
      {
        DocInternalRef *ref = m_parser.handleInternalRef(this);
        if (ref)
        {
          m_children.push_back(std::unique_ptr<DocInternalRef>(ref));
          ref->parse();
        }
        m_parser.tokenizer.setStatePara();
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
  if (m_parser.context.token->emptyTag && !(tagId&XML_CmdMask) &&
      tagId!=HTML_UNKNOWN && tagId!=HTML_IMG && tagId!=HTML_BR && tagId!=HTML_HR && tagId!=HTML_P)
  {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"HTML tag ('<%s/>') may not use the 'empty tag' XHTML syntax.",
                     qPrint(tagName));
  }
  switch (tagId)
  {
    case HTML_UL:
      if (!m_parser.context.token->emptyTag)
      {
        DocHtmlList *list = new DocHtmlList(m_parser,this,tagHtmlAttribs,DocHtmlList::Unordered);
        m_children.push_back(std::unique_ptr<DocHtmlList>(list));
        retval=list->parse();
      }
      break;
    case HTML_OL:
      if (!m_parser.context.token->emptyTag)
      {
        DocHtmlList *list = new DocHtmlList(m_parser,this,tagHtmlAttribs,DocHtmlList::Ordered);
        m_children.push_back(std::unique_ptr<DocHtmlList>(list));
        retval=list->parse();
      }
      break;
    case HTML_LI:
      if (m_parser.context.token->emptyTag) break;
      if (!insideUL(this) && !insideOL(this))
      {
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"lonely <li> tag found");
      }
      else
      {
        retval=RetVal_ListItem;
      }
      break;
    case HTML_BOLD:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Bold,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_S:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::S,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_STRIKE:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Strike,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_DEL:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Del,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_UNDERLINE:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Underline,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_INS:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Ins,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_CODE:
      if (m_parser.context.token->emptyTag) break;
      if (/*getLanguageFromFileName(m_parser.context.fileName)==SrcLangExt_CSharp ||*/ m_parser.context.xmlComment)
        // for C# source or inside a <summary> or <remark> section we
        // treat <code> as an XML tag (so similar to @code)
      {
        m_parser.tokenizer.setStateXmlCode();
        retval = handleStartCode();
      }
      else // normal HTML markup
      {
        m_parser.handleStyleEnter(this,m_children,DocStyleChange::Code,tagName,&m_parser.context.token->attribs);
      }
      break;
    case HTML_EMPHASIS:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Italic,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_DIV:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Div,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_SPAN:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Span,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_SUB:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Subscript,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_SUP:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Superscript,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_CENTER:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Center,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_SMALL:
      if (!m_parser.context.token->emptyTag) m_parser.handleStyleEnter(this,m_children,DocStyleChange::Small,tagName,&m_parser.context.token->attribs);
      break;
    case HTML_PRE:
      if (m_parser.context.token->emptyTag) break;
      m_parser.handleStyleEnter(this,m_children,DocStyleChange::Preformatted,tagName,&m_parser.context.token->attribs);
      setInsidePreformatted(TRUE);
      m_parser.tokenizer.setInsidePre(TRUE);
      break;
    case HTML_P:
      retval=TK_NEWPARA;
      break;
    case HTML_DL:
      if (!m_parser.context.token->emptyTag)
      {
        DocHtmlDescList *list = new DocHtmlDescList(m_parser,this,tagHtmlAttribs);
        m_children.push_back(std::unique_ptr<DocHtmlDescList>(list));
        retval=list->parse();
      }
      break;
    case HTML_DT:
      retval = RetVal_DescTitle;
      break;
    case HTML_DD:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag <dd> found");
      break;
    case HTML_TABLE:
      if (!m_parser.context.token->emptyTag)
      {
        DocHtmlTable *table = new DocHtmlTable(m_parser,this,tagHtmlAttribs);
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
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag <caption> found");
      break;
    case HTML_BR:
      {
        m_children.push_back(std::make_unique<DocLineBreak>(m_parser,this,tagHtmlAttribs));
      }
      break;
    case HTML_HR:
      {
        m_children.push_back(std::make_unique<DocHorRuler>(m_parser,this,tagHtmlAttribs));
      }
      break;
    case HTML_A:
      retval = m_parser.handleAHref(this,m_children,tagHtmlAttribs);
      break;
    case HTML_H1:
      if (!m_parser.context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,1);
      break;
    case HTML_H2:
      if (!m_parser.context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,2);
      break;
    case HTML_H3:
      if (!m_parser.context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,3);
      break;
    case HTML_H4:
      if (!m_parser.context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,4);
      break;
    case HTML_H5:
      if (!m_parser.context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,5);
      break;
    case HTML_H6:
      if (!m_parser.context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,6);
      break;
    case HTML_IMG:
      {
        m_parser.handleImg(this,m_children,tagHtmlAttribs);
      }
      break;
    case HTML_BLOCKQUOTE:
      if (!m_parser.context.token->emptyTag)
      {
        DocHtmlBlockQuote *block = new DocHtmlBlockQuote(m_parser,this,tagHtmlAttribs);
        m_children.push_back(std::unique_ptr<DocHtmlBlockQuote>(block));
        retval = block->parse();
      }
      break;

    case XML_SUMMARY:
    case XML_REMARKS:
    case XML_EXAMPLE:
      m_parser.context.xmlComment=TRUE;
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
      m_parser.handleStyleEnter(this,m_children,DocStyleChange::Code,tagName,&m_parser.context.token->attribs);
      break;
    case XML_PARAM:
    case XML_TYPEPARAM:
      {
        m_parser.context.xmlComment=TRUE;
        QCString paramName;
        if (findAttribute(tagHtmlAttribs,"name",&paramName))
        {
          if (paramName.isEmpty())
          {
            if (Config_getBool(WARN_NO_PARAMDOC))
            {
              warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"empty 'name' attribute for <param%s> tag.",tagId==XML_PARAM?"":"type");
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
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Missing 'name' attribute from <param%s> tag.",tagId==XML_PARAM?"":"type");
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
          m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Italic,tagName,TRUE));
          m_children.push_back(std::make_unique<DocWord>(m_parser,this,paramName));
          m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Italic,tagName,FALSE));
          if (retval!=TK_WORD) m_children.push_back(std::make_unique<DocWhiteSpace>(m_parser,this," "));
        }
        else
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Missing 'name' attribute from <param%sref> tag.",tagId==XML_PARAMREF?"":"type");
        }
      }
      break;
    case XML_EXCEPTION:
      {
        m_parser.context.xmlComment=TRUE;
        QCString exceptName;
        if (findAttribute(tagHtmlAttribs,"cref",&exceptName))
        {
          unescapeCRef(exceptName);
          retval = handleParamSection(exceptName,DocParamSect::Exception,TRUE);
        }
        else
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Missing 'cref' attribute from <exception> tag.");
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
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"lonely <item> tag found");
      }
      break;
    case XML_RETURNS:
      m_parser.context.xmlComment=TRUE;
      retval = handleSimpleSection(DocSimpleSect::Return,TRUE);
      m_parser.context.hasReturnCommand=TRUE;
      break;
    case XML_TERM:
      //m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Bold,TRUE));
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
        //printf("XML_SEE: empty tag=%d\n",m_parser.context.token->emptyTag);
        if (findAttribute(tagHtmlAttribs,"cref",&cref))
        {
          unescapeCRef(cref);
          if (m_parser.context.token->emptyTag) // <see cref="..."/> style
          {
            bool inSeeBlock = m_parser.context.inSeeBlock;
            m_parser.context.token->name = cref;
            m_parser.context.inSeeBlock = TRUE;
            m_parser.handleLinkedWord(this,m_children,TRUE);
            m_parser.context.inSeeBlock = inSeeBlock;
          }
          else // <see cref="...">...</see> style
          {
            //DocRef *ref = new DocRef(this,cref);
            //m_children.append(ref);
            //ref->parse();
            m_parser.tokenizer.setStatePara();
            DocLink *lnk = new DocLink(m_parser,this,cref);
            m_children.push_back(std::unique_ptr<DocLink>(lnk));
            QCString leftOver = lnk->parse(FALSE,TRUE);
            if (!leftOver.isEmpty())
            {
              m_children.push_back(std::make_unique<DocWord>(m_parser,this,leftOver));
            }
          }
        }
        else if (findAttribute(tagHtmlAttribs,"langword",&cref)) // <see langword="..."/> or <see langword="..."></see>
        {
          bool inSeeBlock = m_parser.context.inSeeBlock;
          m_parser.context.token->name = cref;
          m_parser.context.inSeeBlock = TRUE;
          m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Code,tagName,TRUE));
          m_parser.handleLinkedWord(this,m_children,TRUE);
          m_children.push_back(std::make_unique<DocStyleChange>(m_parser,this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Code,tagName,FALSE));
          m_parser.context.inSeeBlock = inSeeBlock;
        }
        else
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Missing 'cref' or 'langword' attribute from <see> tag.");
        }
      }
      break;
    case XML_SEEALSO:
      {
        m_parser.context.xmlComment=TRUE;
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
            ss=new DocSimpleSect(m_parser,this,DocSimpleSect::See);
            m_children.push_back(std::unique_ptr<DocSimpleSect>(ss));
          }

          ss->appendLinkWord(cref);
          retval = RetVal_OK;
        }
        else
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Missing 'cref' attribute from <seealso> tag.");
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
          DocHtmlTable *table = new DocHtmlTable(m_parser,this,emptyList);
          m_children.push_back(std::unique_ptr<DocHtmlTable>(table));
          retval=table->parseXml();
        }
        else
        {
          DocHtmlList *list = new DocHtmlList(m_parser,this,emptyList,listType);
          m_children.push_back(std::unique_ptr<DocHtmlList>(list));
          retval=list->parseXml();
        }
      }
      break;
    case XML_INCLUDE:
    case XML_PERMISSION:
      // These tags are defined in .Net but are currently unsupported
      m_parser.context.xmlComment=TRUE;
      break;
    case HTML_UNKNOWN:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unsupported xml/html tag <%s> found", qPrint(tagName));
      m_children.push_back(std::make_unique<DocWord>(m_parser,this, "<"+tagName+m_parser.context.token->attribsStr+">"));
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
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"found </ul> tag without matching <ul>");
      }
      else
      {
        retval=RetVal_EndList;
      }
      break;
    case HTML_OL:
      if (!insideOL(this))
      {
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"found </ol> tag without matching <ol>");
      }
      else
      {
        retval=RetVal_EndList;
      }
      break;
    case HTML_LI:
      if (!insideLI(this))
      {
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"found </li> tag without matching <li>");
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
    //    warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"found </pre> tag without matching <pre>");
    //  }
    //  else
    //  {
    //    retval=RetVal_EndPre;
    //  }
    //  break;
    case HTML_BOLD:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Bold,tagName);
      break;
    case HTML_S:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::S,"s");
      break;
    case HTML_STRIKE:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Strike,tagName);
      break;
    case HTML_DEL:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Del,tagName);
      break;
    case HTML_UNDERLINE:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Underline,tagName);
      break;
    case HTML_INS:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Ins,tagName);
      break;
    case HTML_CODE:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Code,tagName);
      break;
    case HTML_EMPHASIS:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Italic,tagName);
      break;
    case HTML_DIV:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Div,tagName);
      break;
    case HTML_SPAN:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Span,tagName);
      break;
    case HTML_SUB:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Subscript,tagName);
      break;
    case HTML_SUP:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Superscript,tagName);
      break;
    case HTML_CENTER:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Center,tagName);
      break;
    case HTML_SMALL:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Small,tagName);
      break;
    case HTML_PRE:
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Preformatted,tagName);
      setInsidePreformatted(FALSE);
      m_parser.tokenizer.setInsidePre(FALSE);
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
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag </caption> found");
      break;
    case HTML_BR:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Illegal </br> tag found\n");
      break;
    case HTML_H1:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag </h1> found");
      break;
    case HTML_H2:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag </h2> found");
      break;
    case HTML_H3:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag </h3> found");
      break;
    case HTML_H4:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag </h4> found");
      break;
    case HTML_H5:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag </h5> found");
      break;
    case HTML_H6:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag </h6> found");
      break;
    case HTML_IMG:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag </img> found");
      break;
    case HTML_HR:
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Illegal </hr> tag found\n");
      break;
    case HTML_A:
      //warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected tag </a> found");
      // ignore </a> tag (can be part of <a name=...></a>
      break;

    case XML_TERM:
      //m_children.push_back(std::make_unique<DocStyleChange>(this,(uint)m_parser.context.nodeStack.size(),DocStyleChange::Bold,FALSE));
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
      m_parser.handleStyleLeave(this,m_children,DocStyleChange::Code,tagName);
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
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unsupported xml/html tag </%s> found", qPrint(tagName));
      m_children.push_back(std::make_unique<DocWord>(m_parser,this,"</"+tagName+">"));
      break;
    default:
      // we should not get here!
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected end tag %s\n",qPrint(tagName));
      ASSERT(0);
      break;
  }
  return retval;
}

int DocPara::parse()
{
  DBG(("DocPara::parse() start\n"));
  auto ns = AutoNodeStack(m_parser,this);
  // handle style commands "inherited" from the previous paragraph
  m_parser.handleInitialStyleCommands(this,m_children);
  int tok;
  int retval=0;
  while ((tok=m_parser.tokenizer.lex())) // get the next token
  {
reparsetoken:
    DBG(("token %s at %d",DocTokenizer::tokToString(tok),m_parser.tokenizer.getLineNr()));
    if (tok==TK_WORD || tok==TK_LNKWORD || tok==TK_SYMBOL || tok==TK_URL ||
        tok==TK_COMMAND_AT || tok == TK_COMMAND_BS || tok==TK_HTMLTAG
       )
    {
      DBG((" name=%s",qPrint(m_parser.context.token->name)));
    }
    DBG(("\n"));
    switch(tok)
    {
      case TK_WORD:
        m_children.push_back(std::make_unique<DocWord>(m_parser,this,m_parser.context.token->name));
        break;
      case TK_LNKWORD:
        m_parser.handleLinkedWord(this,m_children);
        break;
      case TK_URL:
        m_children.push_back(std::make_unique<DocURL>(m_parser,this,m_parser.context.token->name,m_parser.context.token->isEMailAddr));
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
            m_children.push_back(std::make_unique<DocWhiteSpace>(m_parser,this,m_parser.context.token->chars));
          }
        }
        break;
      case TK_LISTITEM:
        {
          DBG(("found list item at %d parent=%d\n",m_parser.context.token->indent,parent()->kind()));
          DocNode *n=parent();
          while (n && n->kind()!=DocNode::Kind_AutoList) n=n->parent();
          if (n) // we found an auto list up in the hierarchy
          {
            DocAutoList *al = (DocAutoList *)n;
            DBG(("previous list item at %d\n",al->indent()));
            if (al->indent()>=m_parser.context.token->indent)
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
            al = new DocAutoList(m_parser,this,m_parser.context.token->indent,
                                 m_parser.context.token->isEnumList,depth);
            m_children.push_back(std::unique_ptr<DocAutoList>(al));
            retval = al->parse();
          } while (retval==TK_LISTITEM &&         // new list
              al->indent()==m_parser.context.token->indent  // at same indent level
              );

          // check the return value
          if (retval==RetVal_SimpleSec) // auto list ended due to simple section command
          {
            // Reparse the token that ended the section at this level,
            // so a new simple section will be started at this level.
            // This is the same as unputting the last read token and continuing.
            m_parser.context.token->name = m_parser.context.token->simpleSectName;
            if (m_parser.context.token->name.left(4)=="rcs:") // RCS section
            {
              m_parser.context.token->name = m_parser.context.token->name.mid(4);
              m_parser.context.token->text = m_parser.context.token->simpleSectText;
              tok = TK_RCSTAG;
            }
            else // other section
            {
              tok = TK_COMMAND_BS;
            }
            DBG(("reparsing command %s\n",qPrint(m_parser.context.token->name)));
            goto reparsetoken;
          }
          else if (retval==TK_ENDLIST)
          {
            if (al->indent()>m_parser.context.token->indent) // end list
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
        DBG(("Found end of list inside of paragraph at line %d\n",m_parser.tokenizer.getLineNr()));
        if (parent()->kind()==DocNode::Kind_AutoListItem)
        {
          ASSERT(parent()->parent()->kind()==DocNode::Kind_AutoList);
          DocAutoList *al = (DocAutoList *)parent()->parent();
          if (al->indent()>=m_parser.context.token->indent)
          {
            // end of list marker ends this paragraph
            retval=TK_ENDLIST;
            goto endparagraph;
          }
          else
          {
            warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"End of list marker found "
                "has invalid indent level");
          }
        }
        else
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"End of list marker found without any preceding "
              "list items");
        }
        break;
      case TK_COMMAND_AT:
        // fall through
      case TK_COMMAND_BS:
        {
          // see if we have to start a simple section
          int cmd = Mappers::cmdMapper->map(m_parser.context.token->name);
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
              m_parser.context.token->simpleSectName = m_parser.context.token->name;
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
          retval=handleCommand(m_parser.context.token->name,tok);
          DBG(("handleCommand returns %x\n",retval));

          // check the return value
          if (retval==RetVal_SimpleSec)
          {
            // Reparse the token that ended the section at this level,
            // so a new simple section will be started at this level.
            // This is the same as unputting the last read token and continuing.
            m_parser.context.token->name = m_parser.context.token->simpleSectName;
            if (m_parser.context.token->name.left(4)=="rcs:") // RCS section
            {
              m_parser.context.token->name = m_parser.context.token->name.mid(4);
              m_parser.context.token->text = m_parser.context.token->simpleSectText;
              tok = TK_RCSTAG;
            }
            else // other section
            {
              tok = TK_COMMAND_BS;
            }
            DBG(("reparsing command %s\n",qPrint(m_parser.context.token->name)));
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
          if (!m_parser.context.token->endTag) // found a start tag
          {
            retval = handleHtmlStartTag(m_parser.context.token->name,m_parser.context.token->attribs);
          }
          else // found an end tag
          {
            retval = handleHtmlEndTag(m_parser.context.token->name);
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
          DocSymbol::SymType s = DocSymbol::decodeSymbol(m_parser.context.token->name);
          if (s!=DocSymbol::Sym_Unknown)
          {
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,s));
          }
          else
          {
            m_children.push_back(std::make_unique<DocWord>(m_parser,this,m_parser.context.token->name));
            warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unsupported symbol %s found",
                qPrint(m_parser.context.token->name));
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
            m_parser.context.token->simpleSectName = "rcs:"+m_parser.context.token->name;
            m_parser.context.token->simpleSectText = m_parser.context.token->text;
            retval=RetVal_SimpleSec;
            goto endparagraph;
          }

          // see if we are in a simple list
          DocSimpleSect *ss=new DocSimpleSect(m_parser,this,DocSimpleSect::Rcs);
          m_children.push_back(std::unique_ptr<DocSimpleSect>(ss));
          ss->parseRcs();
        }
        break;
      default:
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),
            "Found unexpected token (id=%x)\n",tok);
        break;
    }
  }
  retval=0;
endparagraph:
  m_parser.handlePendingStyleCommands(this,m_children);
  DBG(("DocPara::parse() end retval=%x\n",retval));
  const DocNode *n = m_parser.context.nodeStack.top();
  if (!m_parser.context.token->endTag && n->kind()==DocNode::Kind_Para &&
      retval==TK_NEWPARA && m_parser.context.token->name.lower() == "p")
  {
    ((DocPara *)n)->setAttribs(m_parser.context.token->attribs);
  }
  INTERNAL_ASSERT(retval==0 || retval==TK_NEWPARA || retval==TK_LISTITEM ||
         retval==TK_ENDLIST || retval>RetVal_OK
	);

  return retval;
}

//--------------------------------------------------------------------------

int DocSection::parse()
{
  DBG(("DocSection::parse() start %s level=%d\n",qPrint(m_parser.context.token->sectionId),m_level));
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(m_parser,this);

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
    DocPara *par = new DocPara(m_parser,this);
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
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Invalid list item found");
    }
    if (retval==RetVal_Internal)
    {
      DocInternal *in = new DocInternal(m_parser,this);
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
        DocSection *s=new DocSection(m_parser,this,
            std::min(2+Doxygen::subpageNestingLevel,5),m_parser.context.token->sectionId);
        m_children.push_back(std::unique_ptr<DocSection>(s));
        retval = s->parse();
      }
      break;
    }
    else if (retval==RetVal_Subsubsection && m_level<=Doxygen::subpageNestingLevel+2)
    {
      if ((m_level<=1+Doxygen::subpageNestingLevel) && !m_parser.context.token->sectionId.startsWith("autotoc_md"))
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected subsubsection command found inside %s!",g_sectionLevelToName[m_level]);
      // then parse any number of nested sections
      while (retval==RetVal_Subsubsection) // more sections follow
      {
        DocSection *s=new DocSection(m_parser,this,
            std::min(3+Doxygen::subpageNestingLevel,5),m_parser.context.token->sectionId);
        m_children.push_back(std::unique_ptr<DocSection>(s));
        retval = s->parse();
      }
      if (!(m_level<Doxygen::subpageNestingLevel+2 && retval == RetVal_Subsection)) break;
    }
    else if (retval==RetVal_Paragraph && m_level<=std::min(5,Doxygen::subpageNestingLevel+3))
    {
      if ((m_level<=2+Doxygen::subpageNestingLevel) && !m_parser.context.token->sectionId.startsWith("autotoc_md"))
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected paragraph command found inside %s!",g_sectionLevelToName[m_level]);
      // then parse any number of nested sections
      while (retval==RetVal_Paragraph) // more sections follow
      {
        DocSection *s=new DocSection(m_parser,this,
            std::min(4+Doxygen::subpageNestingLevel,5),m_parser.context.token->sectionId);
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
  auto ns = AutoNodeStack(m_parser,this);
  m_parser.tokenizer.setStateText();

  int tok;
  while ((tok=m_parser.tokenizer.lex())) // get the next token
  {
    switch(tok)
    {
      case TK_WORD:
	m_children.push_back(std::make_unique<DocWord>(m_parser,this,m_parser.context.token->name));
	break;
      case TK_WHITESPACE:
        m_children.push_back(std::make_unique<DocWhiteSpace>(m_parser,this,m_parser.context.token->chars));
	break;
      case TK_SYMBOL:
        {
          DocSymbol::SymType s = DocSymbol::decodeSymbol(m_parser.context.token->name);
          if (s!=DocSymbol::Sym_Unknown)
          {
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,s));
          }
          else
          {
            warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unsupported symbol %s found",
                qPrint(m_parser.context.token->name));
          }
        }
        break;
      case TK_COMMAND_AT:
        // fall through
      case TK_COMMAND_BS:
        switch (Mappers::cmdMapper->map(m_parser.context.token->name))
        {
          case CMD_BSLASH:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_BSlash));
            break;
          case CMD_AT:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_At));
            break;
          case CMD_LESS:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Less));
            break;
          case CMD_GREATER:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Greater));
            break;
          case CMD_AMP:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Amp));
            break;
          case CMD_DOLLAR:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Dollar));
            break;
          case CMD_HASH:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Hash));
            break;
          case CMD_DCOLON:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_DoubleColon));
            break;
          case CMD_PERCENT:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Percent));
            break;
          case CMD_NDASH:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
            break;
          case CMD_MDASH:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
            break;
          case CMD_QUOTE:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Quot));
            break;
          case CMD_PUNT:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Dot));
            break;
          case CMD_PLUS:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Plus));
            break;
          case CMD_MINUS:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Minus));
            break;
          case CMD_EQUAL:
            m_children.push_back(std::make_unique<DocSymbol>(m_parser,this,DocSymbol::Sym_Equal));
            break;
          default:
            warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected command '%s' found",
                      qPrint(m_parser.context.token->name));
            break;
        }
        break;
      default:
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Unexpected token %s",
            DocTokenizer::tokToString(tok));
        break;
    }
  }

  m_parser.handleUnclosedStyleCommands();

  DBG(("DocText::parse() end\n"));
}


//--------------------------------------------------------------------------

void DocRoot::parse()
{
  DBG(("DocRoot::parse() start\n"));
  auto ns = AutoNodeStack(m_parser,this);
  m_parser.tokenizer.setStatePara();
  int retval=0;

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  do
  {
    DocPara *par = new DocPara(m_parser,this);
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
      if (!m_parser.context.token->sectionId.startsWith("autotoc_md"))
      {
         warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"found paragraph command (id: '%s') outside of subsubsection context!",qPrint(m_parser.context.token->sectionId));
      }
      while (retval==RetVal_Paragraph)
      {
        if (!m_parser.context.token->sectionId.isEmpty())
        {
          const SectionInfo *sec=SectionManager::instance().find(m_parser.context.token->sectionId);
          if (sec)
          {
            DocSection *s=new DocSection(m_parser,this,
                std::min(4+Doxygen::subpageNestingLevel,5),m_parser.context.token->sectionId);
            m_children.push_back(std::unique_ptr<DocSection>(s));
            retval = s->parse();
          }
          else
          {
            warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Invalid paragraph id '%s'; ignoring paragraph",qPrint(m_parser.context.token->sectionId));
            retval = 0;
          }
        }
        else
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Missing id for paragraph; ignoring paragraph");
          retval = 0;
        }
      }
    }
    if (retval==RetVal_Subsubsection)
    {
      if (!(m_parser.context.token->sectionId.startsWith("autotoc_md")))
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"found subsubsection command (id: '%s') outside of subsection context!",qPrint(m_parser.context.token->sectionId));
      while (retval==RetVal_Subsubsection)
      {
        if (!m_parser.context.token->sectionId.isEmpty())
        {
          const SectionInfo *sec=SectionManager::instance().find(m_parser.context.token->sectionId);
          if (sec)
          {
            DocSection *s=new DocSection(m_parser,this,
                std::min(3+Doxygen::subpageNestingLevel,5),m_parser.context.token->sectionId);
            m_children.push_back(std::unique_ptr<DocSection>(s));
            retval = s->parse();
          }
          else
          {
            warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Invalid subsubsection id '%s'; ignoring subsubsection",qPrint(m_parser.context.token->sectionId));
            retval = 0;
          }
        }
        else
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Missing id for subsubsection; ignoring subsubsection");
          retval = 0;
        }
      }
    }
    if (retval==RetVal_Subsection)
    {
      if (!m_parser.context.token->sectionId.startsWith("autotoc_md"))
      {
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"found subsection command (id: '%s') outside of section context!",qPrint(m_parser.context.token->sectionId));
      }
      while (retval==RetVal_Subsection)
      {
        if (!m_parser.context.token->sectionId.isEmpty())
        {
          const SectionInfo *sec=SectionManager::instance().find(m_parser.context.token->sectionId);
          if (sec)
          {
            DocSection *s=new DocSection(m_parser,this,
                std::min(2+Doxygen::subpageNestingLevel,5),m_parser.context.token->sectionId);
            m_children.push_back(std::unique_ptr<DocSection>(s));
            retval = s->parse();
          }
          else
          {
            warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Invalid subsection id '%s'; ignoring subsection",qPrint(m_parser.context.token->sectionId));
            retval = 0;
          }
        }
        else
        {
          warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Missing id for subsection; ignoring subsection");
          retval = 0;
        }
      }
    }
    if (retval==TK_LISTITEM)
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Invalid list item found");
    }
    if (retval==RetVal_Internal)
    {
      DocInternal *in = new DocInternal(m_parser,this);
      m_children.push_back(std::unique_ptr<DocInternal>(in));
      retval = in->parse(1);
    }
  } while (retval!=0 && retval!=RetVal_Section);
  if (lastPar) lastPar->markLast();

  //printf("DocRoot::parse() retval=%d %d\n",retval,RetVal_Section);
  // then parse any number of level1 sections
  while (retval==RetVal_Section)
  {
    if (!m_parser.context.token->sectionId.isEmpty())
    {
      const SectionInfo *sec=SectionManager::instance().find(m_parser.context.token->sectionId);
      if (sec)
      {
        DocSection *s=new DocSection(m_parser,this,
            std::min(1+Doxygen::subpageNestingLevel,5),m_parser.context.token->sectionId);
        m_children.push_back(std::unique_ptr<DocSection>(s));
        retval = s->parse();
      }
      else
      {
        warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Invalid section id '%s'; ignoring section",qPrint(m_parser.context.token->sectionId));
        retval = 0;
      }
    }
    else
    {
      warn_doc_error(m_parser.context.fileName,m_parser.tokenizer.getLineNr(),"Missing id for section; ignoring section");
      retval = 0;
    }
  }

  m_parser.handleUnclosedStyleCommands();

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

QCString DocParser::processCopyDoc(const char *data,uint &len)
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
          auto it = std::find(context.copyStack.begin(),context.copyStack.end(),def);
          if (it==context.copyStack.end()) // definition not parsed earlier
          {
            context.copyStack.push_back(def);
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
            context.copyStack.pop_back();
          }
          else
          {
            warn_doc_error(context.fileName,tokenizer.getLineNr(),
	         "Found recursive @copy%s or @copydoc relation for argument '%s'.\n",
                 isBrief?"brief":"details",qPrint(id));
          }
        }
        else
        {
          warn_doc_error(context.fileName,tokenizer.getLineNr(),
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

DocRoot *validatingParseDoc(IDocParser &parserIntf,
                            const QCString &fileName,int startLine,
                            const Definition *ctx,const MemberDef *md,
                            const QCString &input,bool indexWords,
                            bool isExample, const QCString &exampleName,
                            bool singleLine, bool linkFromIndex,
                            bool markdownSupport)
{
  DocParser &parser = dynamic_cast<DocParser&>(parserIntf);
  //printf("validatingParseDoc(%s,%s)=[%s]\n",ctx?qPrint(ctx->name()):"<none>",
  //                                     md?qPrint(md->name()):"<none>",
  //                                     input);
  //printf("========== validating %s at line %d\n",qPrint(fileName),startLine);
  //printf("---------------- input --------------------\n%s\n----------- end input -------------------\n",qPrint(input));
  //parser.context.token = new TokenInfo;

  // store parser state so we can re-enter this function if needed
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  parser.pushContext();

  if (ctx && ctx!=Doxygen::globalScope &&
      (ctx->definitionType()==Definition::TypeClass ||
       ctx->definitionType()==Definition::TypeNamespace
      )
     )
  {
    parser.context.context = ctx->name();
  }
  else if (ctx && ctx->definitionType()==Definition::TypePage)
  {
    const Definition *scope = (toPageDef(ctx))->getPageScope();
    if (scope && scope!=Doxygen::globalScope) parser.context.context = scope->name();
  }
  else if (ctx && ctx->definitionType()==Definition::TypeGroup)
  {
    const Definition *scope = (toGroupDef(ctx))->getGroupScope();
    if (scope && scope!=Doxygen::globalScope) parser.context.context = scope->name();
  }
  else
  {
    parser.context.context = "";
  }
  parser.context.scope = ctx;

  if (indexWords && Doxygen::searchIndex)
  {
    if (md)
    {
      parser.context.searchUrl=md->getOutputFileBase();
      Doxygen::searchIndex->setCurrentDoc(md,md->anchor(),FALSE);
    }
    else if (ctx)
    {
      parser.context.searchUrl=ctx->getOutputFileBase();
      Doxygen::searchIndex->setCurrentDoc(ctx,ctx->anchor(),FALSE);
    }
  }
  else
  {
    parser.context.searchUrl="";
  }

  parser.context.fileName = fileName;
  parser.context.relPath = (!linkFromIndex && ctx) ?
               QCString(relativePathToRoot(ctx->getOutputFileBase())) :
               QCString("");
  //printf("ctx->name=%s relPath=%s\n",qPrint(ctx->name()),qPrint(parser.context.relPath));
  parser.context.memberDef = md;
  while (!parser.context.nodeStack.empty()) parser.context.nodeStack.pop();
  while (!parser.context.styleStack.empty()) parser.context.styleStack.pop();
  while (!parser.context.initialStyleStack.empty()) parser.context.initialStyleStack.pop();
  parser.context.inSeeBlock = FALSE;
  parser.context.xmlComment = FALSE;
  parser.context.insideHtmlLink = FALSE;
  parser.context.includeFileText = "";
  parser.context.includeFileOffset = 0;
  parser.context.includeFileLength = 0;
  parser.context.isExample = isExample;
  parser.context.exampleName = exampleName;
  parser.context.hasParamCommand = FALSE;
  parser.context.hasReturnCommand = FALSE;
  parser.context.retvalsFound.clear();
  parser.context.paramsFound.clear();
  parser.context.markdownSupport = markdownSupport;

  //printf("Starting comment block at %s:%d\n",qPrint(parser.context.fileName),startLine);
  parser.tokenizer.setLineNr(startLine);
  uint ioLen = input.length();
  QCString inpStr = parser.processCopyDoc(input.data(),ioLen);
  if (inpStr.isEmpty() || inpStr.at(inpStr.length()-1)!='\n')
  {
    inpStr+='\n';
  }
  //printf("processCopyDoc(in='%s' out='%s')\n",input,qPrint(inpStr));
  parser.tokenizer.init(inpStr.data(),parser.context.fileName,markdownSupport);

  // build abstract syntax tree
  DocRoot *root = new DocRoot(parser,md!=0,singleLine);
  root->parse();


  if (Debug::isFlagSet(Debug::PrintTree))
  {
    // pretty print the result
    PrintDocVisitor *v = new PrintDocVisitor;
    root->accept(v);
    delete v;
  }

  parser.checkUnOrMultipleDocumentedParams();
  if (parser.context.memberDef) parser.context.memberDef->detectUndocumentedParams(parser.context.hasParamCommand,parser.context.hasReturnCommand);

  // TODO: These should be called at the end of the program.
  //parser.tokenizer.cleanup();
  //Mappers::cmdMapper->freeInstance();
  //Mappers::htmlTagMapper->freeInstance();

  // restore original parser state
  parser.popContext();

  //printf(">>>>>> end validatingParseDoc(%s,%s)\n",ctx?qPrint(ctx->name()):"<none>",
  //                                     md?qPrint(md->name()):"<none>");

  return root;
}

DocText *validatingParseText(IDocParser &parserIntf,const QCString &input)
{
  DocParser &parser = dynamic_cast<DocParser&>(parserIntf);
  // store parser state so we can re-enter this function if needed
  parser.pushContext();

  //printf("------------ input ---------\n%s\n"
  //       "------------ end input -----\n",input);
  //parser.context.token = new TokenInfo;
  parser.context.context = "";
  parser.context.fileName = "<parseText>";
  parser.context.relPath = "";
  parser.context.memberDef = 0;
  while (!parser.context.nodeStack.empty()) parser.context.nodeStack.pop();
  while (!parser.context.styleStack.empty()) parser.context.styleStack.pop();
  while (!parser.context.initialStyleStack.empty()) parser.context.initialStyleStack.pop();
  parser.context.inSeeBlock = FALSE;
  parser.context.xmlComment = FALSE;
  parser.context.insideHtmlLink = FALSE;
  parser.context.includeFileText = "";
  parser.context.includeFileOffset = 0;
  parser.context.includeFileLength = 0;
  parser.context.isExample = FALSE;
  parser.context.exampleName = "";
  parser.context.hasParamCommand = FALSE;
  parser.context.hasReturnCommand = FALSE;
  parser.context.retvalsFound.clear();
  parser.context.paramsFound.clear();
  parser.context.searchUrl="";

  DocText *txt = new DocText(parser);

  if (!input.isEmpty())
  {
    parser.tokenizer.setLineNr(1);
    parser.tokenizer.init(input.data(),parser.context.fileName,Config_getBool(MARKDOWN_SUPPORT));

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
  parser.popContext();
  return txt;
}

DocRef *createRef(IDocParser &parserIntf,const QCString &target,const QCString &context)
{
  DocParser &parser = dynamic_cast<DocParser&>(parserIntf);
  return new DocRef(parser,0,target,context);
}

void docFindSections(const QCString &input,
                     const Definition *d,
                     const QCString &fileName)
{
  DocParser parser;
  parser.tokenizer.findSections(input,d,fileName);
}

