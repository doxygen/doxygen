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

#ifndef DOCPARSER_P_H
#define DOCPARSER_P_H

/** @file
 *  @brief Private header shared between docparser.cpp and docnode.cpp
 */

#include <cassert>
#include <stack>
#include <iterator>
#include <vector>
#include <deque>

#include "containers.h"
#include "docparser.h"
#include "docnode.h"
#include "doctokenizer.h"
#include "searchindex.h"

using DefinitionStack = std::vector<const Definition *>;
using DocNodeStack = std::stack<DocNodeVariant *>;

template<typename T, typename Container = std::deque<T>>
class IterableStack : public std::stack<T, Container>
{
    using std::stack<T, Container>::c;

public:

    // expose just the iterators of the underlying container
    auto begin() { return std::begin(c); }
    auto end() { return std::end(c); }

    auto begin() const { return std::begin(c); }
    auto end() const { return std::end(c); }
};
using DocStyleChangeStack = IterableStack<const DocNodeVariant *>;

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
    ~DocParser();
    void pushContext();
    void popContext();
    void handleImg(DocNodeVariant *parent,DocNodeList &children,const HtmlAttribList &tagHtmlAttribs);
    int  internalValidatingParseDoc(DocNodeVariant *parent,DocNodeList &children,
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
    bool defaultHandleToken(DocNodeVariant *parent,int tok,
                            DocNodeList &children,bool
                            handleWord=TRUE);
    void errorHandleDefaultToken(DocNodeVariant *parent,int tok,
                                 DocNodeList &children,const QCString &txt);
    void defaultHandleTitleAndSize(const int cmd, DocNodeVariant *parent,
                                   DocNodeList &children, QCString &width,QCString &height);
    int handleStyleArgument(DocNodeVariant *parent,DocNodeList &children,
                            const QCString &cmdName);
    void handleStyleEnter(DocNodeVariant *parent,DocNodeList &children, DocStyleChange::Style s,
                          const QCString &tagName,const HtmlAttribList *attribs);
    void handleStyleLeave(DocNodeVariant *parent,DocNodeList &children, DocStyleChange::Style s,
                          const QCString &tagName);
    void handlePendingStyleCommands(DocNodeVariant *parent,DocNodeList &children);
    void handleInitialStyleCommands(DocNodeVariant *parent,DocNodeList &children);
    int  handleAHref(DocNodeVariant *parent,DocNodeList &children,const HtmlAttribList &tagHtmlAttribs);
    void handleUnclosedStyleCommands();
    void handleLinkedWord(DocNodeVariant *parent,DocNodeList &children,bool ignoreAutoLinkFlag=FALSE);
    void handleParameterType(DocNodeVariant *parent,DocNodeList &children,const QCString &paramTypes);
    void handleInternalRef(DocNodeVariant *parent,DocNodeList &children);
    void handleAnchor(DocNodeVariant *parent,DocNodeList &children);
    void handleImage(DocNodeVariant *parent, DocNodeList &children);
    void readTextFileByName(const QCString &file,QCString &text);

    std::stack< DocParserContext > contextStack;
    DocParserContext               context;
    DocTokenizer                   tokenizer;
    SIDataCollection               searchData;
};

//---------------------------------------------------------------------------

class AutoNodeStack
{
  public:
    AutoNodeStack(DocParser *parser,DocNodeVariant* node)
      : m_parser(parser), m_node(node) { m_parser->context.nodeStack.push(node); }
   ~AutoNodeStack() {
#if defined(NDEBUG)
     (void)m_node;
     if (!m_parser->context.nodeStack.empty())
     {
       m_parser->context.nodeStack.pop(); // robust version that does not assert
     }
#else
     assert(m_parser->context.nodeStack.top()==m_node);
     m_parser->context.nodeStack.pop(); // error checking version
#endif
   }

  private:
   DocParser *m_parser;
   const DocNodeVariant *m_node;
};

inline bool isPreformatted(const DocNodeVariant *n)
{
  return std::visit([](auto &&x)->decltype(auto) { return x.isPreformatted(); }, *n);
}

/*! Returns TRUE iff node n is a child of a preformatted node */
inline bool insidePRE(const DocNodeVariant *n)
{
  while (n)
  {
    if (isPreformatted(n)) return TRUE;
    n=parent(n);
  }
  return FALSE;
}

/*! Returns TRUE iff node n is a child of a html list item node */
inline bool insideLI(const DocNodeVariant *n)
{
  while (n)
  {
    if (std::holds_alternative<DocHtmlListItem>(*n)) return TRUE;
    n=parent(n);
  }
  return FALSE;
}

//---------------------------------------------------------------------------

/*! Returns TRUE iff node n is a child of a unordered html list node */
inline bool insideUL(const DocNodeVariant *n)
{
  while (n)
  {
    if (std::holds_alternative<DocHtmlList>(*n) &&
        std::get<DocHtmlList>(*n).type()==DocHtmlList::Unordered)
    {
      return TRUE;
    }
    n=parent(n);
  }
  return FALSE;
}

//---------------------------------------------------------------------------

/*! Returns TRUE iff node n is a child of a ordered html list node */
inline bool insideOL(const DocNodeVariant *n)
{
  while (n)
  {
    if (std::holds_alternative<DocHtmlList>(*n) &&
        std::get<DocHtmlList>(*n).type()==DocHtmlList::Ordered)
    {
      return TRUE;
    }
    n=parent(n);
  }
  return FALSE;
}

//---------------------------------------------------------------------------

inline bool insideTable(const DocNodeVariant *n)
{
  while (n)
  {
    if (std::holds_alternative<DocHtmlTable>(*n)) return TRUE;
    n=parent(n);
  }
  return FALSE;
}

//---------------------------------------------------------------------------

inline bool insideDetails(DocStyleChangeStack styleStack)
{
  for (auto i : styleStack)
  {
     if (std::get<DocStyleChange>(*i).style() == DocStyleChange::Details) return true;
  }
  return false;
}


#endif
