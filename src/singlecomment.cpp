/******************************************************************************
 *
 * Copyright (C) 1997-2024 by Dimitri van Heesch.
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

#include <cstdio>

#include "singlecomment.h"
#include "docnode.h"
#include "htmldocvisitor.h"
#include "commentscan.h"
#include "commentcnv.h"
#include "markdown.h"
#include "entry.h"
#include "outputlist.h"

static void generateHtmlOutput(const QCString &fileName,const QCString &doc)
{
  //printf("------\n%s\n------\n",qPrint(doc));
  auto parser { createDocParser() };
  auto ast { validatingParseDoc(*parser.get(),
                                fileName,
                                1,
                                nullptr,
                                nullptr,
                                doc,
                                DocOptions()
                                .setMarkdownSupport(true)
                                .setAutolinkSupport(true))
           };
  const DocNodeAST *astImpl = dynamic_cast<const DocNodeAST*>(ast.get());
  if (astImpl)
  {
    TextStream t;
    OutputCodeList codeList;
    codeList.add<HtmlCodeGenerator>(&t);
    HtmlDocVisitor visitor(t,codeList,nullptr,fileName);
    std::visit(visitor,astImpl->root);
    std::string content = t.str()+"\n";
    fwrite(content.c_str(),content.length(),1,stdout);
  }
}

void generateHtmlForComment(const std::string &fn,const std::string &text)
{
  QCString fileName = "index.html";
  std::shared_ptr<Entry> root = std::make_shared<Entry>();

  // 1. Pass input through commentcnv
  std::string convBuf;
  convBuf.reserve(text.size()+1024);
  convertCppComments(text,convBuf,"input.md");

  // 2. Pass result through commentscan
  MarkdownOutlineParser markdownParser;
  CommentScanner scanner;
  int lineNr=1;
  scanner.enterFile(fileName,lineNr);
  int pos=0;
  bool needsEntry=false;
  GuardedSectionStack guards;
  Markdown markdown(fileName,1,0);
  QCString processedDocs = markdown.process(convBuf.data(),lineNr,true);
  std::shared_ptr<Entry> current = std::make_shared<Entry>();
  current->lang = SrcLangExt::Markdown;
  current->fileName = fn;
  current->docFile  = fn;
  current->docLine  = 1;
  auto prot = Protection::Public;
  while (scanner.parseCommentBlock(&markdownParser,
        current.get(),
        processedDocs,
        fileName,lineNr,
        false, // isBrief
        false, // isAutoBriefOn
        false, // isInbody
        prot,
        pos,
        needsEntry,
        true,  // markdownSupport
        &guards))
  {
    if (needsEntry)
    {
      QCString docFile = current->docFile;
      root->moveToSubEntryAndRefresh(current);
      current->lang = SrcLangExt::Markdown;
      current->docFile = docFile;
      current->docLine = lineNr;
    }
  }
  root->moveToSubEntryAndKeep(current);
  scanner.leaveFile(fileName,lineNr);

  // 3. Pass result through docparser
  for (const auto &child : root->children())
  {
    if (!child->brief.isEmpty())
    {
      generateHtmlOutput(fn,child->brief);
    }
    if (!child->doc.isEmpty())
    {
      generateHtmlOutput(fn,child->doc);
    }
  }
}
