/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#include "classlist.h"
#include "cmdmapper.h"
#include "config.h"
#include "debug.h"
#include "dir.h"
#include "docparser.h"
#include "docparser_p.h"
#include "doxygen.h"
#include "filedef.h"
#include "fileinfo.h"
#include "groupdef.h"
#include "namespacedef.h"
#include "message.h"
#include "pagedef.h"
#include "portable.h"
#include "printdocvisitor.h"
#include "util.h"
#include "indexlist.h"

// debug off
#define DBG(x) do {} while(0)

// debug to stdout
//#define DBG(x) printf x

// debug to stderr
//#define myprintf(...) fprintf(stderr,__VA_ARGS__)
//#define DBG(x) myprintf x


//---------------------------------------------------------------------------

IDocParserPtr createDocParser()
{
  return std::make_unique<DocParser>();
}

//---------------------------------------------------------------------------
DocParser::~DocParser()
{
  try
  {
    searchData.transfer();
  }
  catch(...)
  {
    err("Unexpected exception caught in DocParser\n");
  }
}

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
	result = result.right(static_cast<int>(result.length())-i-1);
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
        case DocImage::Xml:
	  if (!Config_getBool(GENERATE_XML)) return result;
	  outputDir = Config_getString(XML_OUTPUT);
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
	fd->name().endsWith(".eps")
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
    if (!result.startsWith("http:") && !result.startsWith("https:") && doWarn)
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
  if (!(Config_getBool(WARN_IF_DOC_ERROR) || Config_getBool(WARN_IF_INCOMPLETE_DOC))) return;
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
      if (argName.endsWith("...")) argName=argName.left(argName.length()-3);
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
        if (argName.endsWith("...")) argName=argName.left(argName.length()-3);
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
            warn_doc_error(context.memberDef->docFile(),
                           context.memberDef->docLine(),
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
        warn_incomplete_doc(context.memberDef->docFile(),
                            context.memberDef->docLine(),
                            "%s",
                            qPrint(substitute(errMsg,"%","%%")));
      }
    }
  }
}


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

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
  bool extractAnonNs = Config_getBool(EXTRACT_ANON_NSPACES);
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

  int l=static_cast<int>(cmdArg.length());

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


  int scopeOffset=static_cast<int>(context.context.length());
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
void DocParser::errorHandleDefaultToken(DocNodeVariant *parent,int tok,
                                        DocNodeList &children,const QCString &txt)
{
  const char *cmd_start = "\\";
  switch (tok)
  {
    case TK_COMMAND_AT:
      cmd_start = "@";
      // fall through
    case TK_COMMAND_BS:
      children.append<DocWord>(this,parent,TK_COMMAND_CHAR(tok) + context.token->name);
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"Illegal command %s found as part of a %s",
       qPrint(cmd_start + context.token->name),qPrint(txt));
      break;
    case TK_SYMBOL:
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"Unsupported symbol %s found as part of a %s",
           qPrint(context.token->name), qPrint(txt));
      break;
    default:
      children.append<DocWord>(this,parent,context.token->name);
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"Unexpected token %s found as part of a %s",
        DocTokenizer::tokToString(tok), qPrint(txt));
      break;
  }
}

//---------------------------------------------------------------------------

int DocParser::handleStyleArgument(DocNodeVariant *parent,DocNodeList &children,const QCString &cmdName)
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
          DBG(("handleStyleArgument(%s) end tok=%s\n",qPrint(saveCmdName), DocTokenizer::tokToString(tok)));
          return tok;
          break;
        default:
	  errorHandleDefaultToken(parent,tok,children,"\\" + saveCmdName + " command");
          break;
      }
      break;
    }
  }
  DBG(("handleStyleArgument(%s) end tok=%s\n",qPrint(saveCmdName), DocTokenizer::tokToString(tok)));
  return (tok==TK_NEWPARA || tok==TK_LISTITEM || tok==TK_ENDLIST
         ) ? tok : RetVal_OK;
}

/*! Called when a style change starts. For instance a \<b\> command is
 *  encountered.
 */
void DocParser::handleStyleEnter(DocNodeVariant *parent,DocNodeList &children,
          DocStyleChange::Style s,const QCString &tagName,const HtmlAttribList *attribs)
{
  DBG(("HandleStyleEnter\n"));
  children.append<DocStyleChange>(this,parent,context.nodeStack.size(),s,tagName,TRUE,attribs);
  context.styleStack.push(&children.back());
}

/*! Called when a style change ends. For instance a \</b\> command is
 *  encountered.
 */
void DocParser::handleStyleLeave(DocNodeVariant *parent,DocNodeList &children,
         DocStyleChange::Style s,const QCString &tagName)
{
  DBG(("HandleStyleLeave\n"));
  QCString tagNameLower = QCString(tagName).lower();

  auto topStyleChange = [](const DocStyleChangeStack &stack) -> const DocStyleChange &
  {
    return std::get<DocStyleChange>(*stack.top());
  };

  if (context.styleStack.empty() ||                            // no style change
      topStyleChange(context.styleStack).style()!=s ||         // wrong style change
      topStyleChange(context.styleStack).tagName()!=tagNameLower ||  // wrong style change
      topStyleChange(context.styleStack).position()!=context.nodeStack.size() // wrong position
     )
  {
    if (context.styleStack.empty())
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"found </%s> tag without matching <%s>",
          qPrint(tagName),qPrint(tagName));
    }
    else if (topStyleChange(context.styleStack).tagName()!=tagNameLower)
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"found </%s> tag while expecting </%s>",
          qPrint(tagName),qPrint(topStyleChange(context.styleStack).tagName()));
    }
    else if (topStyleChange(context.styleStack).style()!=s)
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"found </%s> tag while expecting </%s>",
          qPrint(tagName),qPrint(topStyleChange(context.styleStack).tagName()));
    }
    else
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"found </%s> at different nesting level (%zu) than expected (%zu)",
          qPrint(tagName),context.nodeStack.size(),topStyleChange(context.styleStack).position());
    }
  }
  else // end the section
  {
    children.append<DocStyleChange>(
          this,parent,context.nodeStack.size(),s,
          topStyleChange(context.styleStack).tagName(),FALSE);
    context.styleStack.pop();
  }
}

/*! Called at the end of a paragraph to close all open style changes
 *  (e.g. a <b> without a </b>). The closed styles are pushed onto a stack
 *  and entered again at the start of a new paragraph.
 */
void DocParser::handlePendingStyleCommands(DocNodeVariant *parent,DocNodeList &children)
{
  if (!context.styleStack.empty())
  {
    const DocStyleChange *sc = &std::get<DocStyleChange>(*context.styleStack.top());
    while (sc && sc->position()>=context.nodeStack.size())
    { // there are unclosed style modifiers in the paragraph
      children.append<DocStyleChange>(this,parent,context.nodeStack.size(),
                                           sc->style(),sc->tagName(),FALSE);
      context.initialStyleStack.push(context.styleStack.top());
      context.styleStack.pop();
      sc = !context.styleStack.empty() ? &std::get<DocStyleChange>(*context.styleStack.top()) : 0;
    }
  }
}

void DocParser::handleInitialStyleCommands(DocNodeVariant *parent,DocNodeList &children)
{
  while (!context.initialStyleStack.empty())
  {
    const DocStyleChange &sc = std::get<DocStyleChange>(*context.initialStyleStack.top());
    handleStyleEnter(parent,children,sc.style(),sc.tagName(),&sc.attribs());
    context.initialStyleStack.pop();
  }
}

int DocParser::handleAHref(DocNodeVariant *parent,DocNodeList &children,
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
        children.append<DocAnchor>(this,parent,opt.value,TRUE);
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
      auto vDocHRef = children.append<DocHRef>(this, parent, attrList,
                                               opt.value, relPath,
                                               convertNameToFile(context.fileName, FALSE, TRUE));
      DocHRef *href = children.get_last<DocHRef>();
      context.insideHtmlLink=TRUE;
      retval = href->parse(vDocHRef);
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

void DocParser::handleUnclosedStyleCommands()
{
  if (!context.initialStyleStack.empty())
  {
    QCString tagName = std::get<DocStyleChange>(*context.initialStyleStack.top()).tagName();
    context.initialStyleStack.pop();
    handleUnclosedStyleCommands();
    warn_doc_error(context.fileName,tokenizer.getLineNr(),
             "end of comment block while expecting "
             "command </%s>",qPrint(tagName));
  }
}

void DocParser::handleLinkedWord(DocNodeVariant *parent,DocNodeList &children,bool ignoreAutoLinkFlag)
{
  QCString name = linkToText(SrcLangExt_Unknown,context.token->name,TRUE);
  bool autolinkSupport = Config_getBool(AUTOLINK_SUPPORT);
  if (!autolinkSupport && !ignoreAutoLinkFlag) // no autolinking -> add as normal word
  {
    children.append<DocWord>(this,parent,name);
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
      children.append<DocLinkedWord>(
            this,parent,name,
            member->getReference(),
            member->getOutputFileBase(),
            member->anchor(),
            member->briefDescriptionAsTooltip());
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
      children.append<DocLinkedWord>(
            this,parent,name,
            compound->getReference(),
            compound->getOutputFileBase(),
            anchor,
            compound->briefDescriptionAsTooltip());
    }
    else if (compound->definitionType()==Definition::TypeFile &&
             (toFileDef(compound))->generateSourceFile()
            ) // undocumented file that has source code we can link to
    {
      children.append<DocLinkedWord>(
             this,parent,context.token->name,
             compound->getReference(),
             compound->getSourceFileBase(),
             "",
             compound->briefDescriptionAsTooltip());
    }
    else // not linkable
    {
      children.append<DocWord>(this,parent,name);
    }
  }
  else if (!context.insideHtmlLink && len>1 && context.token->name.at(len-1)==':')
  {
    // special case, where matching Foo: fails to be an Obj-C reference,
    // but Foo itself might be linkable.
    context.token->name=context.token->name.left(len-1);
    handleLinkedWord(parent,children,ignoreAutoLinkFlag);
    children.append<DocWord>(this,parent,":");
  }
  else if (!context.insideHtmlLink && (cd=getClass(context.token->name+"-p")))
  {
    // special case 2, where the token name is not a class, but could
    // be a Obj-C protocol
    children.append<DocLinkedWord>(
          this,parent,name,
          cd->getReference(),
          cd->getOutputFileBase(),
          cd->anchor(),
          cd->briefDescriptionAsTooltip());
  }
  else // normal non-linkable word
  {
    if (context.token->name.startsWith("#") || context.token->name.startsWith("::"))
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"explicit link request to '%s' could not be resolved",qPrint(name));
      children.append<DocWord>(this,parent,context.token->name);
    }
    else
    {
      children.append<DocWord>(this,parent,name);
    }
  }
}

void DocParser::handleParameterType(DocNodeVariant *parent,DocNodeList &children,const QCString &paramTypes)
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
    if (ii!=-1) children.append<DocWord>(this,parent,name1.mid(ii)); // add [] part
    p=i+1;
    children.append<DocSeparator>(this,parent,"|");
  }
  name1 = paramTypes.mid(p);
  ii=name1.find('[');
  context.token->name=ii!=-1 ? name1.mid(0,ii) : name1;
  handleLinkedWord(parent,children);
  if (ii!=-1) children.append<DocWord>(this,parent,name1.mid(ii));
  context.token->name = name; // restore original token name
}

void DocParser::handleInternalRef(DocNodeVariant *parent,DocNodeList &children)
{
  //printf("CMD_INTERNALREF\n");
  int tok=tokenizer.lex();
  QCString tokenName = context.token->name;
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(tokenName));
    return;
  }
  tokenizer.setStateInternalRef();
  tok=tokenizer.lex(); // get the reference id
  if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(tokenName));
    return;
  }
  auto vDocInternalRef = children.append<DocInternalRef>(this,parent,context.token->name);
  children.get_last<DocInternalRef>()->parse(vDocInternalRef);
}

void DocParser::handleAnchor(DocNodeVariant *parent,DocNodeList &children)
{
  int tok=tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(context.token->name));
    return;
  }
  tokenizer.setStateAnchor();
  tok=tokenizer.lex();
  if (tok==0)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(context.token->name));
    return;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(context.token->name));
    return;
  }
  tokenizer.setStatePara();
  children.append<DocAnchor>(this,parent,context.token->name,FALSE);
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
void DocParser::defaultHandleTitleAndSize(const int cmd, DocNodeVariant *parent, DocNodeList &children, QCString &width,QCString &height)
{
  auto ns = AutoNodeStack(this,parent);

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
    if (tok==TK_WORD)
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
        tokenizer.unputString(context.token->name);
        warn_doc_error(context.fileName,tokenizer.getLineNr(),"Unknown option '%s' after \\%s command, expected 'width' or 'height'",
                       qPrint(context.token->name), qPrint(Mappers::cmdMapper->find(cmd)));
        break;
      }
    }

    tok=tokenizer.lex();
    // if we found something we did not expect, push it back to the stream
    // so it can still be processed
    if (tok==TK_COMMAND_AT || tok==TK_COMMAND_BS)
    {
      tokenizer.unputString(context.token->name);
      tokenizer.unputString(tok==TK_COMMAND_AT ? "@" : "\\");
    }
    else if (tok==TK_SYMBOL || tok==TK_HTMLTAG)
    {
      tokenizer.unputString(context.token->name);
    }
  }
  tokenizer.setStatePara();

  handlePendingStyleCommands(parent,children);
}

void DocParser::handleImage(DocNodeVariant *parent, DocNodeList &children)
{
  bool inlineImage = false;
  QCString anchorStr;

  int tok=tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    if (tok==TK_WORD)
    {
      if (context.token->name == "{")
      {
        tokenizer.setStateOptions();
        tok=tokenizer.lex();
        tokenizer.setStatePara();
        StringVector optList=split(context.token->name.str(),",");
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
               warn_doc_error(context.fileName,tokenizer.getLineNr(),
                  "multiple use of option 'anchor' for 'image' command, ignoring: '%s'",
                  qPrint(locOpt.mid(7)));
            }
            else
            {
               anchorStr = locOpt.mid(7);
            }
          }
          else
          {
            warn_doc_error(context.fileName,tokenizer.getLineNr(),
                  "unknown option '%s' for 'image' command specified",
                  qPrint(locOpt));
          }
        }
        tok=tokenizer.lex();
        if (tok!=TK_WHITESPACE)
        {
          warn_doc_error(context.fileName,tokenizer.getLineNr(),"expected whitespace after \\image command");
          return;
        }
      }
    }
    else
    {
      warn_doc_error(context.fileName,tokenizer.getLineNr(),"expected whitespace after \\image command");
      return;
    }
  }
  tok=tokenizer.lex();
  if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"unexpected token %s as the argument of \\image",
        DocTokenizer::tokToString(tok));
    return;
  }
  tok=tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"expected whitespace after \\image command");
    return;
  }
  DocImage::Type t;
  QCString imgType = context.token->name.lower();
  if      (imgType=="html")    t=DocImage::Html;
  else if (imgType=="latex")   t=DocImage::Latex;
  else if (imgType=="docbook") t=DocImage::DocBook;
  else if (imgType=="rtf")     t=DocImage::Rtf;
  else if (imgType=="xml")     t=DocImage::Xml;
  else
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"output format `%s` specified as the first argument of "
        "\\image command is not valid",
        qPrint(imgType));
    return;
  }
  tokenizer.setStateFile();
  tok=tokenizer.lex();
  tokenizer.setStatePara();
  if (tok!=TK_WORD)
  {
    warn_doc_error(context.fileName,tokenizer.getLineNr(),"unexpected token %s as the argument of \\image",
        DocTokenizer::tokToString(tok));
    return;
  }
  if (!anchorStr.isEmpty())
  {
    children.append<DocAnchor>(this,parent,anchorStr,true);
  }
  HtmlAttribList attrList;
  auto vDocImage = children.append<DocImage>(this,parent,attrList,
                 findAndCopyImage(context.token->name,t),t,"",inlineImage);
  children.get_last<DocImage>()->parse(vDocImage);
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
bool DocParser::defaultHandleToken(DocNodeVariant *parent,int tok, DocNodeList &children,bool handleWord)
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
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_BSlash);
          break;
        case CMD_AT:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_At);
          break;
        case CMD_LESS:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Less);
          break;
        case CMD_GREATER:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Greater);
          break;
        case CMD_AMP:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Amp);
          break;
        case CMD_DOLLAR:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Dollar);
          break;
        case CMD_HASH:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Hash);
          break;
        case CMD_DCOLON:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_DoubleColon);
          break;
        case CMD_PERCENT:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Percent);
          break;
        case CMD_NDASH:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Minus);
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Minus);
          break;
        case CMD_MDASH:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Minus);
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Minus);
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Minus);
          break;
        case CMD_QUOTE:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Quot);
          break;
        case CMD_PUNT:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Dot);
          break;
        case CMD_PLUS:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Plus);
          break;
        case CMD_MINUS:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Minus);
          break;
        case CMD_EQUAL:
          children.append<DocSymbol>(this,parent,HtmlEntityMapper::Sym_Equal);
          break;
        case CMD_EMPHASIS:
          {
            children.append<DocStyleChange>(this,parent,context.nodeStack.size(),DocStyleChange::Italic,tokenName,TRUE);
            tok=handleStyleArgument(parent,children,tokenName);
            children.append<DocStyleChange>(this,parent,context.nodeStack.size(),DocStyleChange::Italic,tokenName,FALSE);
            if (tok!=TK_WORD) children.append<DocWhiteSpace>(this,parent," ");
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
            children.append<DocStyleChange>(this,parent,context.nodeStack.size(),DocStyleChange::Bold,tokenName,TRUE);
            tok=handleStyleArgument(parent,children,tokenName);
            children.append<DocStyleChange>(this,parent,context.nodeStack.size(),DocStyleChange::Bold,tokenName,FALSE);
            if (tok!=TK_WORD) children.append<DocWhiteSpace>(this,parent," ");
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
            children.append<DocStyleChange>(this,parent,context.nodeStack.size(),DocStyleChange::Code,tokenName,TRUE);
            tok=handleStyleArgument(parent,children,tokenName);
            children.append<DocStyleChange>(this,parent,context.nodeStack.size(),DocStyleChange::Code,tokenName,FALSE);
            if (tok!=TK_WORD) children.append<DocWhiteSpace>(this,parent," ");
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
            children.append<DocVerbatim>(this,parent,context.context,context.token->verb,DocVerbatim::HtmlOnly,context.isExample,context.exampleName,context.token->name=="block");
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"htmlonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_MANONLY:
          {
            tokenizer.setStateManOnly();
            tok = tokenizer.lex();
            children.append<DocVerbatim>(this,parent,context.context,context.token->verb,DocVerbatim::ManOnly,context.isExample,context.exampleName);
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"manonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_RTFONLY:
          {
            tokenizer.setStateRtfOnly();
            tok = tokenizer.lex();
            children.append<DocVerbatim>(this,parent,context.context,context.token->verb,DocVerbatim::RtfOnly,context.isExample,context.exampleName);
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"rtfonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_LATEXONLY:
          {
            tokenizer.setStateLatexOnly();
            tok = tokenizer.lex();
            children.append<DocVerbatim>(this,parent,context.context,context.token->verb,DocVerbatim::LatexOnly,context.isExample,context.exampleName);
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"latexonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_XMLONLY:
          {
            tokenizer.setStateXmlOnly();
            tok = tokenizer.lex();
            children.append<DocVerbatim>(this,parent,context.context,context.token->verb,DocVerbatim::XmlOnly,context.isExample,context.exampleName);
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"xmlonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_DBONLY:
          {
            tokenizer.setStateDbOnly();
            tok = tokenizer.lex();
            children.append<DocVerbatim>(this,parent,context.context,context.token->verb,DocVerbatim::DocbookOnly,context.isExample,context.exampleName);
            if (tok==0) warn_doc_error(context.fileName,tokenizer.getLineNr(),"docbookonly section ended without end marker");
            tokenizer.setStatePara();
          }
          break;
        case CMD_FORMULA:
          {
            children.append<DocFormula>(this,parent,context.token->id);
          }
          break;
        case CMD_ANCHOR:
          {
            handleAnchor(parent,children);
          }
          break;
        case CMD_INTERNALREF:
          {
            handleInternalRef(parent,children);
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
          handleImage(parent,children);
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
          case HTML_CITE:
            if (!context.token->endTag)
            {
              handleStyleEnter(parent,children,DocStyleChange::Cite,tokenName,&context.token->attribs);
            }
            else
            {
              handleStyleLeave(parent,children,DocStyleChange::Cite,tokenName);
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
        HtmlEntityMapper::SymType s = DocSymbol::decodeSymbol(tokenName);
        if (s!=HtmlEntityMapper::Sym_Unknown)
        {
          children.append<DocSymbol>(this,parent,s);
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
        children.append<DocWhiteSpace>(this,parent,context.token->chars);
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
        children.append<DocWord>(this,parent,context.token->name);
      }
      else
        return FALSE;
      break;
    case TK_URL:
      if (context.insideHtmlLink)
      {
        children.append<DocWord>(this,parent,context.token->name);
      }
      else
      {
        children.append<DocURL>(this,parent,context.token->name,context.token->isEMailAddr);
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

//---------------------------------------------------------------------------

void DocParser::handleImg(DocNodeVariant *parent, DocNodeList &children,const HtmlAttribList &tagHtmlAttribs)
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
      children.append<DocImage>(
            this,parent,attrList,
            findAndCopyImage(opt.value,t,false),
            t,opt.value);
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

int DocParser::internalValidatingParseDoc(DocNodeVariant *parent,DocNodeList &children,
                                    const QCString &doc)
{
  int retval = RetVal_OK;

  if (doc.isEmpty()) return retval;

  tokenizer.init(doc.data(),context.fileName,context.markdownSupport);

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=!children.empty() ? std::get_if<DocPara>(&children.back()): 0;
  if (lastPar)
  { // last child item was a paragraph
    isFirst=FALSE;
  }
  do
  {
    auto vDocPara = children.append<DocPara>(this,parent);
    DocPara *par  = children.get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse(vDocPara);
    if (!par->isEmpty())
    {
      if (lastPar) lastPar->markLast(FALSE);
      lastPar=par;
    }
    else
    {
      children.pop_back();
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

static QCString extractCopyDocId(const char *data, uint &j, uint len)
{
  uint s=j;
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
  uint e=j;
  if (j>0 && data[j-1]=='.') { e--; } // do not include punctuation added by Definition::_setBriefDescription()
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
  uint j=0;
  if (i==0 || (data[i-1]!='@' && data[i-1]!='\\')) // not an escaped command
  {
    CHECK_FOR_COMMAND("copybrief",brief=TRUE);    // @copybrief or \copybrief
    CHECK_FOR_COMMAND("copydetails",brief=FALSE); // @copydetails or \copydetails
  }
  return j;
}

static uint isVerbatimSection(const char *data,uint i,uint len,QCString &endMarker)
{
  uint j=0;
  if (i==0 || (data[i-1]!='@' && data[i-1]!='\\')) // not an escaped command
  {
    CHECK_FOR_COMMAND("dot",endMarker="enddot");
    CHECK_FOR_COMMAND("code",endMarker="endcode");
    CHECK_FOR_COMMAND("msc",endMarker="endmsc");
    CHECK_FOR_COMMAND("verbatim",endMarker="endverbatim");
    CHECK_FOR_COMMAND("iliteral",endMarker="endiliteral");
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
  int lineNr = tokenizer.getLineNr();
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
            QCString orgFileName = context.fileName;
            context.copyStack.push_back(def);
            if (isBrief)
            {
              buf.addStr("\\ifile \""+QCString(def->briefFile())+"\" ");
              buf.addStr("\\iline "+QCString().setNum(def->briefLine())+" ");
              uint l=static_cast<uint>(brief.length());
              buf.addStr(processCopyDoc(brief.data(),l));
            }
            else
            {
              buf.addStr("\\ifile \""+QCString(def->docFile())+"\" ");
              buf.addStr("\\iline "+QCString().setNum(def->docLine())+" ");
              uint l=static_cast<uint>(doc.length());
              buf.addStr(processCopyDoc(doc.data(),l));
            }
            context.copyStack.pop_back();
            buf.addStr("\\ifile \""+context.fileName+"\" ");
            buf.addStr("\\iline "+QCString().setNum(lineNr)+" ");
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
          uint orgPos = i;
          i=skipToEndMarker(data,k,len,endMarker);
          buf.addStr(data+orgPos,i-orgPos);
          // TODO: adjust lineNr
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
      lineNr += (c=='\n') ? 1 : 0;
    }
  }
  len = static_cast<uint>(buf.getPos());
  buf.addChar(0);
  return buf.get();
}


//---------------------------------------------------------------------------

IDocNodeASTPtr validatingParseDoc(IDocParser &parserIntf,
                            const QCString &fileName,int startLine,
                            const Definition *ctx,const MemberDef *md,
                            const QCString &input,bool indexWords,
                            bool isExample, const QCString &exampleName,
                            bool singleLine, bool linkFromIndex,
                            bool markdownSupport)
{
  DocParser *parser = dynamic_cast<DocParser*>(&parserIntf);
  assert(parser!=0);
  if (parser==0) return 0;
  //printf("validatingParseDoc(%s,%s)=[%s]\n",ctx?qPrint(ctx->name()):"<none>",
  //                                     md?qPrint(md->name()):"<none>",
  //                                     input);
  //printf("========== validating %s at line %d\n",qPrint(fileName),startLine);
  //printf("---------------- input --------------------\n%s\n----------- end input -------------------\n",qPrint(input));
  //parser->context.token = new TokenInfo;

  // store parser state so we can re-enter this function if needed
  //bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  parser->pushContext();

  if (ctx && ctx!=Doxygen::globalScope &&
      (ctx->definitionType()==Definition::TypeClass ||
       ctx->definitionType()==Definition::TypeNamespace
      )
     )
  {
    parser->context.context = ctx->name();
  }
  else if (ctx && ctx->definitionType()==Definition::TypePage)
  {
    const Definition *scope = (toPageDef(ctx))->getPageScope();
    if (scope && scope!=Doxygen::globalScope) parser->context.context = scope->name();
  }
  else if (ctx && ctx->definitionType()==Definition::TypeGroup)
  {
    const Definition *scope = (toGroupDef(ctx))->getGroupScope();
    if (scope && scope!=Doxygen::globalScope) parser->context.context = scope->name();
  }
  else
  {
    parser->context.context = "";
  }
  parser->context.scope = ctx;

  if (indexWords && Doxygen::searchIndex)
  {
    if (md)
    {
      parser->context.searchUrl=md->getOutputFileBase();
      parser->searchData.setCurrentDoc(md,md->anchor(),false);
    }
    else if (ctx)
    {
      parser->context.searchUrl=ctx->getOutputFileBase();
      parser->searchData.setCurrentDoc(ctx,ctx->anchor(),false);
    }
  }
  else
  {
    parser->context.searchUrl="";
  }

  parser->context.fileName = fileName;
  parser->context.relPath = (!linkFromIndex && ctx) ?
               QCString(relativePathToRoot(ctx->getOutputFileBase())) :
               QCString("");
  //printf("ctx->name=%s relPath=%s\n",qPrint(ctx->name()),qPrint(parser->context.relPath));
  parser->context.memberDef = md;
  while (!parser->context.nodeStack.empty()) parser->context.nodeStack.pop();
  while (!parser->context.styleStack.empty()) parser->context.styleStack.pop();
  while (!parser->context.initialStyleStack.empty()) parser->context.initialStyleStack.pop();
  parser->context.inSeeBlock = FALSE;
  parser->context.xmlComment = FALSE;
  parser->context.insideHtmlLink = FALSE;
  parser->context.includeFileText = "";
  parser->context.includeFileOffset = 0;
  parser->context.includeFileLength = 0;
  parser->context.isExample = isExample;
  parser->context.exampleName = exampleName;
  parser->context.hasParamCommand = FALSE;
  parser->context.hasReturnCommand = FALSE;
  parser->context.retvalsFound.clear();
  parser->context.paramsFound.clear();
  parser->context.markdownSupport = markdownSupport;

  //printf("Starting comment block at %s:%d\n",qPrint(parser->context.fileName),startLine);
  parser->tokenizer.setLineNr(startLine);
  uint ioLen = static_cast<uint>(input.length());
  QCString inpStr = parser->processCopyDoc(input.data(),ioLen);
  if (inpStr.isEmpty() || inpStr.at(inpStr.length()-1)!='\n')
  {
    inpStr+='\n';
  }
  //printf("processCopyDoc(in='%s' out='%s')\n",input,qPrint(inpStr));
  parser->tokenizer.init(inpStr.data(),parser->context.fileName,markdownSupport);

  // build abstract syntax tree
  auto ast = std::make_unique<DocNodeAST>(DocRoot(parser,md!=0,singleLine));
  std::get<DocRoot>(ast->root).parse(&ast->root);

  if (Debug::isFlagSet(Debug::PrintTree))
  {
    // pretty print the result
    std::visit(PrintDocVisitor{},ast->root);
  }

  parser->checkUnOrMultipleDocumentedParams();
  if (parser->context.memberDef) parser->context.memberDef->detectUndocumentedParams(parser->context.hasParamCommand,parser->context.hasReturnCommand);

  // TODO: These should be called at the end of the program.
  //parser->tokenizer.cleanup();
  //Mappers::cmdMapper->freeInstance();
  //Mappers::htmlTagMapper->freeInstance();

  // restore original parser state
  parser->popContext();

  //printf(">>>>>> end validatingParseDoc(%s,%s)\n",ctx?qPrint(ctx->name()):"<none>",
  //                                     md?qPrint(md->name()):"<none>");

  return ast;
}

IDocNodeASTPtr validatingParseText(IDocParser &parserIntf,const QCString &input)
{
  DocParser *parser = dynamic_cast<DocParser*>(&parserIntf);
  assert(parser!=0);
  if (parser==0) return 0;

  // store parser state so we can re-enter this function if needed
  parser->pushContext();

  //printf("------------ input ---------\n%s\n"
  //       "------------ end input -----\n",input);
  //parser->context.token = new TokenInfo;
  parser->context.context = "";
  parser->context.fileName = "<parseText>";
  parser->context.relPath = "";
  parser->context.memberDef = 0;
  while (!parser->context.nodeStack.empty()) parser->context.nodeStack.pop();
  while (!parser->context.styleStack.empty()) parser->context.styleStack.pop();
  while (!parser->context.initialStyleStack.empty()) parser->context.initialStyleStack.pop();
  parser->context.inSeeBlock = FALSE;
  parser->context.xmlComment = FALSE;
  parser->context.insideHtmlLink = FALSE;
  parser->context.includeFileText = "";
  parser->context.includeFileOffset = 0;
  parser->context.includeFileLength = 0;
  parser->context.isExample = FALSE;
  parser->context.exampleName = "";
  parser->context.hasParamCommand = FALSE;
  parser->context.hasReturnCommand = FALSE;
  parser->context.retvalsFound.clear();
  parser->context.paramsFound.clear();
  parser->context.searchUrl="";


  auto ast = std::make_unique<DocNodeAST>(DocText(parser));

  if (!input.isEmpty())
  {
    parser->tokenizer.setLineNr(1);
    parser->tokenizer.init(input.data(),parser->context.fileName,Config_getBool(MARKDOWN_SUPPORT));

    // build abstract syntax tree
    std::get<DocText>(ast->root).parse(&ast->root);

    if (Debug::isFlagSet(Debug::PrintTree))
    {
      // pretty print the result
      std::visit(PrintDocVisitor{},ast->root);
    }
  }

  // restore original parser state
  parser->popContext();
  return ast;
}

IDocNodeASTPtr createRef(IDocParser &parserIntf,const QCString &target,const QCString &context)
{
  DocParser *parser = dynamic_cast<DocParser*>(&parserIntf);
  assert(parser!=0);
  if (parser==0) return 0;
  return std::make_unique<DocNodeAST>(DocRef(parser,0,target,context));
}

void docFindSections(const QCString &input,
                     const Definition *d,
                     const QCString &fileName)
{
  DocParser parser;
  parser.tokenizer.findSections(input,d,fileName);
}

