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

#include "docnode.h"
#include "docparser_p.h"
#include "htmlentity.h"
#include "configimpl.h"
#include "configoptions.h"
#include "emoji.h"
#include "message.h"
#include "doxygen.h"
#include "cite.h"
#include "util.h"
#include "formula.h"
#include "markdown.h"
#include "pagedef.h"
#include "namespacedef.h"
#include "groupdef.h"
#include "cmdmapper.h"
#include "config.h"
#include "vhdldocgen.h"
#include "doctokenizer.h"
#include "plantuml.h"
#include "language.h"
#include "datetime.h"
#include "trace.h"
#include "anchor.h"

#define INTERNAL_ASSERT(x) do {} while(0)
//#define INTERNAL_ASSERT(x) if (!(x)) TRACE("INTERNAL_ASSERT({}) failed retval={:#x}: file={} line={}",#x,retval,__FILE__,__LINE__)

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


//---------------------------------------------------------------------------

static const std::set<std::string> g_plantumlEngine {
  "uml", "bpm", "wire", "dot", "ditaa",
  "salt", "math", "latex", "gantt", "mindmap",
  "wbs", "yaml", "creole", "json", "flow",
  "board", "git", "hcl", "regex", "ebnf"
};

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

/*! Strips known html and tex extensions from \a text. */
static QCString stripKnownExtensions(const QCString &text)
{
  QCString result=text;
  if (result.endsWith(".tex"))
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

static void setParent(DocNodeVariant *n,DocNodeVariant *newParent)
{
  std::visit([&](auto &&x)->decltype(auto) { return x.setParent(newParent); }, *n);
}

//----------- DocStyleChange

const char *DocStyleChange::styleString() const
{
  switch (m_style)
  {
    case DocStyleChange::Bold:         return "b";
    case DocStyleChange::Italic:       return "em";
    case DocStyleChange::Code:         return "code";
    case DocStyleChange::Center:       return "center";
    case DocStyleChange::Small:        return "small";
    case DocStyleChange::Cite:         return "cite";
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

//----------- DocSymbol

HtmlEntityMapper::SymType DocSymbol::decodeSymbol(const QCString &symName)
{
  return HtmlEntityMapper::instance().name2sym(symName);
}

//----------- DocEmoji

DocEmoji::DocEmoji(DocParser *parser,DocNodeVariant *parent,const QCString &symName) :
      DocNode(parser,parent), m_symName(symName), m_index(-1)
{
  QCString locSymName = symName;
  uint32_t len=locSymName.length();
  if (len>0)
  {
    if (locSymName.at(len-1)!=':') locSymName.append(":");
    if (locSymName.at(0)!=':')     locSymName.prepend(":");
  }
  m_symName = locSymName;
  m_index = EmojiEntityMapper::instance().symbol2index(m_symName.str());
  if (m_index==-1)
  {
    warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"Found unsupported emoji symbol '%s'\n",qPrint(m_symName));
  }
}

//---------------------------------------------------------------------------

DocWord::DocWord(DocParser *parser,DocNodeVariant *parent,const QCString &word) :
      DocNode(parser,parent), m_word(word)
{
  //printf("new word %s url=%s\n",qPrint(word),qPrint(parser->context.searchUrl));
  if (Doxygen::searchIndex && !parser->context.searchUrl.isEmpty())
  {
    Doxygen::searchIndex->addWord(word,false);
  }
}

//---------------------------------------------------------------------------

DocLinkedWord::DocLinkedWord(DocParser *parser,DocNodeVariant *parent,const QCString &word,
                  const QCString &ref,const QCString &file,
                  const QCString &anchor,const QCString &tooltip) :
      DocNode(parser,parent), m_word(word), m_ref(ref),
      m_file(file), m_relPath(parser->context.relPath), m_anchor(anchor),
      m_tooltip(tooltip)
{
  //printf("DocLinkedWord: new word %s url=%s tooltip='%s'\n",
  //    qPrint(word),qPrint(parser->context.searchUrl),qPrint(tooltip));
  if (Doxygen::searchIndex && !parser->context.searchUrl.isEmpty())
  {
    Doxygen::searchIndex->addWord(word,false);
  }
}

//---------------------------------------------------------------------------

DocAnchor::DocAnchor(DocParser *parser,DocNodeVariant *parent,const QCString &id,bool newAnchor) : DocNode(parser,parent)
{
  if (id.isEmpty())
  {
    warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"Empty anchor label");
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
      warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"Invalid cite anchor id '%s'",qPrint(id));
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
      warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"Invalid anchor id '%s'",qPrint(id));
      m_anchor = "invalid";
      m_file = "invalid";
    }
  }
}

//---------------------------------------------------------------------------

DocVerbatim::DocVerbatim(DocParser *parser,DocNodeVariant *parent,const QCString &context,
    const QCString &text, Type t,bool isExample,
    const QCString &exampleFile,bool isBlock,const QCString &lang)
  : DocNode(parser,parent), p(std::make_unique<Private>(context, text, t, isExample, exampleFile, parser->context.relPath, lang, isBlock))
{
}


//---------------------------------------------------------------------------

void DocInclude::parse()
{
  AUTO_TRACE("file={} text={}",m_file,Trace::trunc(m_text));
  switch(m_type)
  {
    case DontIncWithLines:
      // fall through
    case IncWithLines:
      // fall through
    case Include:
      // fall through
    case DontInclude:
      parser()->readTextFileByName(m_file,m_text);
      parser()->context.includeFileName   = m_file;
      parser()->context.includeFileText   = m_text;
      parser()->context.includeFileOffset = 0;
      parser()->context.includeFileLength = m_text.length();
      parser()->context.includeFileLine   = 0;
      parser()->context.includeFileShowLineNo = (m_type == DontIncWithLines || m_type == IncWithLines);
      //printf("parser->context.includeFile=<<%s>>\n",qPrint(parser->context.includeFileText));
      break;
    case VerbInclude:
      // fall through
    case HtmlInclude:
    case LatexInclude:
    case DocInclude::RtfInclude:
    case DocInclude::ManInclude:
    case DocInclude::XmlInclude:
    case DocInclude::DocbookInclude:
      parser()->readTextFileByName(m_file,m_text);
      break;
    case Snippet:
    case SnippetTrimLeft:
    case SnipWithLines:
      parser()->readTextFileByName(m_file,m_text);
      // check here for the existence of the blockId inside the file, so we
      // only generate the warning once.
      int count;
      if (!m_blockId.isEmpty() && (count=m_text.contains(m_blockId.data()))!=2)
      {
        warn_doc_error(parser()->context.fileName,
                       parser()->tokenizer.getLineNr(),
                       "block marked with %s for \\snippet should appear twice in file %s, found it %d times\n",
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
  if (parser()->context.includeFileName.isEmpty())
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
                   "No previous '\\include' or '\\dontinclude' command for '\\%s' present",
                   typeAsString());
  }

  m_includeFileName = parser()->context.includeFileName;
  const char *p = parser()->context.includeFileText.data();
  uint32_t l = parser()->context.includeFileLength;
  uint32_t o = parser()->context.includeFileOffset;
  int il = parser()->context.includeFileLine;
  AUTO_TRACE("text={} off={} len={}",Trace::trunc(p),o,l);
  uint32_t so = o,bo;
  bool nonEmpty = FALSE;
  switch(type())
  {
    case Line:
      while (o<l)
      {
        char c = p[o];
        if (c=='\n')
        {
          parser()->context.includeFileLine++;
          if (nonEmpty) break; // we have a pattern to match
          so=o+1; // no pattern, skip empty line
        }
        else if (!isspace(static_cast<uint8_t>(c))) // no white space char
        {
          nonEmpty=TRUE;
        }
        o++;
      }
      if (parser()->context.includeFileText.mid(so,o-so).find(m_pattern)!=-1)
      {
        m_line  = il;
        m_text = parser()->context.includeFileText.mid(so,o-so);
        AUTO_TRACE_ADD("\\line {}",Trace::trunc(m_text));
      }
      parser()->context.includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = parser()->context.includeFileShowLineNo;
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
            parser()->context.includeFileLine++;
            if (nonEmpty) break; // we have a pattern to match
            so=o+1; // no pattern, skip empty line
          }
          else if (!isspace(static_cast<uint8_t>(c))) // no white space char
          {
            nonEmpty=TRUE;
          }
          o++;
        }
        if (parser()->context.includeFileText.mid(so,o-so).find(m_pattern)!=-1)
        {
          m_line  = il;
          m_text = parser()->context.includeFileText.mid(so,o-so);
          AUTO_TRACE_ADD("\\skipline {}",Trace::trunc(m_text));
          break;
        }
        o++; // skip new line
      }
      parser()->context.includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = parser()->context.includeFileShowLineNo;
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
            parser()->context.includeFileLine++;
            if (nonEmpty) break; // we have a pattern to match
            so=o+1; // no pattern, skip empty line
          }
          else if (!isspace(static_cast<uint8_t>(c))) // no white space char
          {
            nonEmpty=TRUE;
          }
          o++;
        }
        if (parser()->context.includeFileText.mid(so,o-so).find(m_pattern)!=-1)
        {
          break;
        }
        o++; // skip new line
      }
      parser()->context.includeFileOffset = so; // set pointer to start of new line
      m_showLineNo = parser()->context.includeFileShowLineNo;
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
            parser()->context.includeFileLine++;
            if (nonEmpty) break; // we have a pattern to match
            so=o+1; // no pattern, skip empty line
          }
          else if (!isspace(static_cast<uint8_t>(c))) // no white space char
          {
            nonEmpty=TRUE;
          }
          o++;
        }
        if (parser()->context.includeFileText.mid(so,o-so).find(m_pattern)!=-1)
        {
          m_line  = il;
          m_text = parser()->context.includeFileText.mid(bo,o-bo);
          AUTO_TRACE_ADD("\\until {}",Trace::trunc(m_text));
          break;
        }
        o++; // skip new line
      }
      parser()->context.includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = parser()->context.includeFileShowLineNo;
      break;
  }
}

//---------------------------------------------------------------------------

DocXRefItem::DocXRefItem(DocParser *parser,DocNodeVariant *parent,int id,const QCString &key) :
   DocCompoundNode(parser,parent), m_id(id), m_key(key), m_relPath(parser->context.relPath)
{
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
      if (parser()->context.memberDef && parser()->context.memberDef->name().at(0)=='@')
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
        parser()->pushContext();
        parser()->internalValidatingParseDoc(thisVariant(),children(),item->text());
        parser()->popContext();
      }
    }
    return TRUE;
  }
  return FALSE;
}

//---------------------------------------------------------------------------

DocFormula::DocFormula(DocParser *parser,DocNodeVariant *parent,int id) : DocNode(parser,parent),
      m_relPath(parser->context.relPath)
{
  const Formula *formula = FormulaManager::instance().findFormula(id);
  if (formula && !formula->text().isEmpty())
  {
    m_id = id;
    m_name.sprintf("form_%d",m_id);
    m_text = formula->text();
  }
  else // wrong \_form#<n> command
  {
    warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"Wrong formula id %d",id);
    m_id = -1;
  }
}

//---------------------------------------------------------------------------

DocSecRefItem::DocSecRefItem(DocParser *parser,DocNodeVariant *parent,const QCString &target) :
      DocCompoundNode(parser,parent), m_target(target), m_relPath(parser->context.relPath)
{
}

void DocSecRefItem::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  parser()->tokenizer.setStateTitle();
  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"\\refitem");
    }
  }
  parser()->tokenizer.setStatePara();
  parser()->handlePendingStyleCommands(thisVariant(),children());

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
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"reference to unknown section %s",
          qPrint(m_target));
    }
  }
  else
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"reference to empty target");
  }
}

//---------------------------------------------------------------------------

void DocSecRefList::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  int tok=parser()->tokenizer.lex();
  // skip white space
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=parser()->tokenizer.lex();
  // handle items
  while (tok)
  {
    if (tok==TK_COMMAND_AT || tok == TK_COMMAND_BS)
    {
      const char *cmd_start = (tok==TK_COMMAND_AT ? "@" : "\\");
      switch (Mappers::cmdMapper->map(parser()->context.token->name))
      {
        case CMD_SECREFITEM:
          {
            tok=parser()->tokenizer.lex();
            if (tok!=TK_WHITESPACE)
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\refitem command");
              break;
            }
            tok=parser()->tokenizer.lex();
            if (tok!=TK_WORD && tok!=TK_LNKWORD)
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of \\refitem",
                  DocTokenizer::tokToString(tok));
              break;
            }

            children().append<DocSecRefItem>(parser(),thisVariant(),parser()->context.token->name);
            children().get_last<DocSecRefItem>()->parse();
          }
          break;
        case CMD_ENDSECREFLIST:
          return;
        default:
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal command %s as part of a \\secreflist",
              qPrint(cmd_start + parser()->context.token->name));
          return;
      }
    }
    else if (tok==TK_WHITESPACE)
    {
      // ignore whitespace
    }
    else
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected token %s inside section reference list",
          DocTokenizer::tokToString(tok));
      return;
    }
    tok=parser()->tokenizer.lex();
  }

}

//---------------------------------------------------------------------------

DocInternalRef::DocInternalRef(DocParser *parser,DocNodeVariant *parent,const QCString &ref)
  : DocCompoundNode(parser,parent), m_relPath(parser->context.relPath)
{
  int i=ref.find('#');
  if (i!=-1)
  {
    m_anchor = ref.right(static_cast<int>(ref.length())-i-1);
    m_file   = ref.left(i);
  }
  else
  {
    m_file = ref;
  }
}

void DocInternalRef::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"\\ref");
    }
  }

  parser()->handlePendingStyleCommands(thisVariant(),children());
}

//---------------------------------------------------------------------------

DocRef::DocRef(DocParser *parser,DocNodeVariant *parent,const QCString &target,const QCString &context) :
   DocCompoundNode(parser,parent), m_refType(Unknown), m_isSubPage(FALSE)
{
  const Definition  *compound = 0;
  QCString     anchor;
  //printf("DocRef::DocRef(target=%s,context=%s)\n",qPrint(target),qPrint(context));
  ASSERT(!target.isEmpty());
  SrcLangExt lang = getLanguageFromFileName(target);
  m_relPath = parser->context.relPath;
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
    m_sectionType = sec->type();
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
        bool localLink = parser->context.memberDef ? member->getClassDef()==parser->context.memberDef->getClassDef() : FALSE;
        m_text = member->objCMethodName(localLink,parser->context.inSeeBlock);
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
  warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"unable to resolve reference to '%s' for \\ref command",
           qPrint(target));
}

static void flattenParagraphs(DocNodeVariant *root,DocNodeList &children)
{
  DocNodeList newChildren;
  for (auto &dn : children)
  {
    DocPara *para = std::get_if<DocPara>(&dn);
    if (para)
    {
      //// move the children of the paragraph to the end of the newChildren list
      newChildren.move_append(para->children());
    }
  }

  // replace the children list by the newChildren list
  children.clear();
  children.move_append(newChildren);
  // reparent the children
  for (auto &cn : children)
  {
    setParent(&cn,root);
  }
}

void DocRef::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          break;
        default:
          parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"\\ref");
          break;
      }
    }
  }

  if (children().empty() && !m_text.isEmpty())
  {
    parser()->context.insideHtmlLink=TRUE;
    parser()->pushContext();
    parser()->internalValidatingParseDoc(thisVariant(),children(),m_text);
    parser()->popContext();
    parser()->context.insideHtmlLink=FALSE;
    flattenParagraphs(thisVariant(),children());
  }

  parser()->handlePendingStyleCommands(thisVariant(),children());
}

//---------------------------------------------------------------------------

DocCite::DocCite(DocParser *parser,DocNodeVariant *parent,const QCString &target,const QCString &) : DocNode(parser,parent)
{
  size_t numBibFiles = Config_getList(CITE_BIB_FILES).size();
  //printf("DocCite::DocCite(target=%s)\n",qPrint(target));
  ASSERT(!target.isEmpty());
  m_relPath = parser->context.relPath;
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
    warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"\\cite command found but no bib files specified via CITE_BIB_FILES!");
  }
  else if (cite==0)
  {
    warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"unable to resolve reference to '%s' for \\cite command",
             qPrint(target));
  }
  else
  {
    warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"\\cite command to '%s' does not have an associated number",
             qPrint(target));
  }
}

//---------------------------------------------------------------------------

DocLink::DocLink(DocParser *parser,DocNodeVariant *parent,const QCString &target) : DocCompoundNode(parser,parent)
{
  const Definition *compound = 0;
  QCString anchor;
  m_refText = target;
  m_relPath = parser->context.relPath;
  if (!m_refText.isEmpty() && m_refText.at(0)=='#')
  {
    m_refText = m_refText.right(m_refText.length()-1);
  }
  if (resolveLink(parser->context.context,stripKnownExtensions(target),parser->context.inSeeBlock,&compound,anchor))
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
  warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"unable to resolve link to '%s' for \\link command",
         qPrint(target));
}


QCString DocLink::parse(bool isJavaLink,bool isXmlLink)
{
  AUTO_TRACE();
  QCString result;
  auto ns = AutoNodeStack(parser(),thisVariant());

  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children(),FALSE))
    {
      const char *cmd_start = "\\";
      switch (tok)
      {
        case TK_COMMAND_AT:
          cmd_start = "@";
        // fall through
        case TK_COMMAND_BS:
          switch (Mappers::cmdMapper->map(parser()->context.token->name))
          {
            case CMD_ENDLINK:
              if (isJavaLink)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"{@link.. ended with @endlink command");
              }
              goto endlink;
            default:
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal command %s as part of a \\link",
                  qPrint(cmd_start + parser()->context.token->name));
              break;
          }
          break;
        case TK_SYMBOL:
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported symbol %s found as part of a \\link",
              qPrint(parser()->context.token->name));
          break;
        case TK_HTMLTAG:
          if (parser()->context.token->name!="see" || !isXmlLink)
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected xml/html command %s found as part of a \\link",
                qPrint(parser()->context.token->name));
          }
          goto endlink;
        case TK_LNKWORD:
        case TK_WORD:
          if (isJavaLink) // special case to detect closing }
          {
            QCString w = parser()->context.token->name;
            int p;
            if (w=="}")
            {
              goto endlink;
            }
            else if ((p=w.find('}'))!=-1)
            {
              uint32_t l=w.length();
              children().append<DocWord>(parser(),thisVariant(),w.left(p));
              if (static_cast<uint32_t>(p)<l-1) // something left after the } (for instance a .)
              {
                result=w.right(static_cast<int>(l)-p-1);
              }
              goto endlink;
            }
          }
          children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
          break;
        default:
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected token %s",
             DocTokenizer::tokToString(tok));
        break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,
                   parser()->tokenizer.getLineNr(),
                   "Unexpected end of comment while inside link command\n");
  }
endlink:

  if (children().empty()) // no link text
  {
    children().append<DocWord>(parser(),thisVariant(),m_refText);
  }

  parser()->handlePendingStyleCommands(thisVariant(),children());
  return result;
}


//---------------------------------------------------------------------------

DocDotFile::DocDotFile(DocParser *parser,DocNodeVariant *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile,int srcLine) :
  DocDiagramFileBase(parser,parent,name,context,srcFile,srcLine)
{
  p->relPath = parser->context.relPath;
}

bool DocDotFile::parse()
{
  bool ok = false;
  parser()->defaultHandleTitleAndSize(CMD_DOTFILE,thisVariant(),children(),p->width,p->height);

  bool ambig;
  FileDef *fd = findFileDef(Doxygen::dotFileNameLinkedMap,p->name,ambig);
  if (fd==0 && !p->name.endsWith(".dot")) // try with .dot extension as well
  {
    fd = findFileDef(Doxygen::dotFileNameLinkedMap,p->name+".dot",ambig);
  }
  if (fd)
  {
    p->file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included dot file name %s is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(p->name),
           qPrint(showFileDefMatches(Doxygen::dotFileNameLinkedMap,p->name))
          );
    }
  }
  else
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included dot file %s is not found "
           "in any of the paths specified via DOTFILE_DIRS!",qPrint(p->name));
  }
  return ok;
}

DocMscFile::DocMscFile(DocParser *parser,DocNodeVariant *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile, int srcLine) :
  DocDiagramFileBase(parser,parent,name,context,srcFile,srcLine)
{
  p->relPath = parser->context.relPath;
}

bool DocMscFile::parse()
{
  bool ok = false;
  parser()->defaultHandleTitleAndSize(CMD_MSCFILE,thisVariant(),children(),p->width,p->height);

  bool ambig;
  FileDef *fd = findFileDef(Doxygen::mscFileNameLinkedMap,p->name,ambig);
  if (fd==0 && !p->name.endsWith(".msc")) // try with .msc extension as well
  {
    fd = findFileDef(Doxygen::mscFileNameLinkedMap,p->name+".msc",ambig);
  }
  if (fd)
  {
    p->file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included msc file name %s is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(p->name),
           qPrint(showFileDefMatches(Doxygen::mscFileNameLinkedMap,p->name))
          );
    }
  }
  else
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included msc file %s is not found "
           "in any of the paths specified via MSCFILE_DIRS!",qPrint(p->name));
  }
  return ok;
}

//---------------------------------------------------------------------------

DocDiaFile::DocDiaFile(DocParser *parser,DocNodeVariant *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile,int srcLine) :
  DocDiagramFileBase(parser,parent,name,context,srcFile,srcLine)
{
  p->relPath = parser->context.relPath;
}

bool DocDiaFile::parse()
{
  bool ok = false;
  parser()->defaultHandleTitleAndSize(CMD_DIAFILE,thisVariant(),children(),p->width,p->height);

  bool ambig;
  FileDef *fd = findFileDef(Doxygen::diaFileNameLinkedMap,p->name,ambig);
  if (fd==0 && !p->name.endsWith(".dia")) // try with .dia extension as well
  {
    fd = findFileDef(Doxygen::diaFileNameLinkedMap,p->name+".dia",ambig);
  }
  if (fd)
  {
    p->file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included dia file name %s is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(p->name),
           qPrint(showFileDefMatches(Doxygen::diaFileNameLinkedMap,p->name))
          );
    }
  }
  else
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included dia file %s is not found "
           "in any of the paths specified via DIAFILE_DIRS!",qPrint(p->name));
  }
  return ok;
}

//---------------------------------------------------------------------------

DocVhdlFlow::DocVhdlFlow(DocParser *parser,DocNodeVariant *parent) : DocCompoundNode(parser,parent)
{
}

void DocVhdlFlow::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  parser()->tokenizer.setStateTitle();
  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"\\vhdlflow");
    }
  }
  tok=parser()->tokenizer.lex();

  parser()->tokenizer.setStatePara();
  parser()->handlePendingStyleCommands(thisVariant(),children());

  VhdlDocGen::createFlowChart(parser()->context.memberDef);
}


//---------------------------------------------------------------------------

DocImage::DocImage(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs,const QCString &name,
                   Type t,const QCString &url, bool inlineImage) :
      DocCompoundNode(parser,parent), p(std::make_unique<Private>(attribs, name, t, parser->context.relPath, url, inlineImage))
{
}

bool DocImage::isSVG() const
{
  QCString  locName = p->url.isEmpty() ? p->name : p->url;
  int len = static_cast<int>(locName.length());
  int fnd = locName.find('?'); // ignore part from ? until end
  if (fnd==-1) fnd=len;
  return fnd>=4 && locName.mid(fnd-4,4)==".svg";
}

void DocImage::parse()
{
  parser()->defaultHandleTitleAndSize(CMD_IMAGE,thisVariant(),children(),p->width,p->height);
}


//---------------------------------------------------------------------------

int DocHtmlHeader::parse()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
            if (tagId==HTML_H1 && parser()->context.token->endTag) // found </h1> tag
            {
              if (m_level!=1)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h1>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H2 && parser()->context.token->endTag) // found </h2> tag
            {
              if (m_level!=2)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h2>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H3 && parser()->context.token->endTag) // found </h3> tag
            {
              if (m_level!=3)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h3>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H4 && parser()->context.token->endTag) // found </h4> tag
            {
              if (m_level!=4)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h4>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H5 && parser()->context.token->endTag) // found </h5> tag
            {
              if (m_level!=5)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h5>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_H6 && parser()->context.token->endTag) // found </h6> tag
            {
              if (m_level!=6)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h6>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HTML_A)
            {
              if (!parser()->context.token->endTag)
              {
                parser()->handleAHref(thisVariant(),children(),parser()->context.token->attribs);
              }
            }
            else if (tagId==HTML_BR)
            {
              children().append<DocLineBreak>(parser(),thisVariant(),parser()->context.token->attribs);
            }
            else
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected html tag <%s%s> found within <h%d> context",
                  parser()->context.token->endTag?"/":"",qPrint(parser()->context.token->name),m_level);
            }
          }
          break;
        default:
	        char tmp[20];
	        qsnprintf(tmp,20,"<h%d>tag",m_level);
          parser()->errorHandleDefaultToken(thisVariant(),tok,children(),tmp);
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <h%d> tag\n",m_level);
  }
endheader:
  parser()->handlePendingStyleCommands(thisVariant(),children());
  return retval;
}
//---------------------------------------------------------------------------

void DocHtmlSummary::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  parser()->tokenizer.setStateTitle();
  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    int tagId;
    // check of </summary>
    if (tok==TK_HTMLTAG &&
        (tagId=Mappers::htmlTagMapper->map(parser()->context.token->name)) && tagId==XML_SUMMARY &&
        parser()->context.token->endTag
       )
    {
      break;
    }
    else if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"summary section");
    }
  }
  parser()->tokenizer.setStatePara();
}

//---------------------------------------------------------------------------

int DocHtmlDetails::parse()
{
  AUTO_TRACE();
  int retval=0;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  if (!summary())
  {
    HtmlAttribList summaryAttribs;
    m_summary = createDocNode<DocHtmlSummary>(parser(),thisVariant(),summaryAttribs);
    DocHtmlSummary *summary = &std::get<DocHtmlSummary>(*m_summary);
    summary->children().append<DocWord>(parser(),thisVariant(),theTranslator->trDetails());
  }
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return (retval==RetVal_EndHtmlDetails) ? RetVal_OK : retval;
}

void DocHtmlDetails::parseSummary(DocNodeVariant *parent,HtmlAttribList &attribs)
{
  AUTO_TRACE();
  m_summary = createDocNode<DocHtmlSummary>(parser(),parent,attribs);
  DocHtmlSummary *summary = &std::get<DocHtmlSummary>(*m_summary);
  summary->parse();
}

//---------------------------------------------------------------------------

int DocHRef::parse()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
            if (tagId==HTML_A && parser()->context.token->endTag) // found </a> tag
            {
              goto endhref;
            }
            else if (tagId==HTML_BR)
            {
              children().append<DocLineBreak>(parser(),thisVariant(),parser()->context.token->attribs);
            }
            else
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected html tag <%s%s> found within <a href=...> context",
                  parser()->context.token->endTag?"/":"",qPrint(parser()->context.token->name));
            }
          }
          break;
        default:
          parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"<a>..</a> block");
          break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <a href=...> tag");
  }
endhref:
  parser()->handlePendingStyleCommands(thisVariant(),children());
  return retval;
}

//---------------------------------------------------------------------------

int DocInternal::parse(int level)
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    DocPara *par  = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (!par->isEmpty())
    {
      if (lastPar) lastPar->markLast(FALSE);
      lastPar=par;
    }
    else
    {
      children().pop_back();
    }
    if (retval==TK_LISTITEM)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid list item found");
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
    children().append<DocSection>(parser(),thisVariant(),
                                  std::min(level+Doxygen::subpageNestingLevel,5),
                                  parser()->context.token->sectionId);
    retval = children().get_last<DocSection>()->parse();
  }

  if (retval==RetVal_Internal)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"\\internal command found inside internal section");
  }

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

//---------------------------------------------------------------------------

int DocIndexEntry::parse()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\addindex command");
    goto endindexentry;
  }
  parser()->tokenizer.setStateTitle();
  m_entry="";
  while ((tok=parser()->tokenizer.lex()))
  {
    switch (tok)
    {
      case TK_WHITESPACE:
        m_entry+=" ";
        break;
      case TK_WORD:
      case TK_LNKWORD:
        m_entry+=parser()->context.token->name;
        break;
      case TK_SYMBOL:
        {
          HtmlEntityMapper::SymType s = DocSymbol::decodeSymbol(parser()->context.token->name);
          switch (s)
          {
            case HtmlEntityMapper::Sym_BSlash:  m_entry+='\\'; break;
            case HtmlEntityMapper::Sym_At:      m_entry+='@';  break;
            case HtmlEntityMapper::Sym_Less:    m_entry+='<';  break;
            case HtmlEntityMapper::Sym_Greater: m_entry+='>';  break;
            case HtmlEntityMapper::Sym_Amp:     m_entry+='&';  break;
            case HtmlEntityMapper::Sym_Dollar:  m_entry+='$';  break;
            case HtmlEntityMapper::Sym_Hash:    m_entry+='#';  break;
            case HtmlEntityMapper::Sym_Percent: m_entry+='%';  break;
            case HtmlEntityMapper::Sym_apos:    m_entry+='\''; break;
            case HtmlEntityMapper::Sym_Quot:    m_entry+='"';  break;
            case HtmlEntityMapper::Sym_lsquo:   m_entry+='`';  break;
            case HtmlEntityMapper::Sym_rsquo:   m_entry+='\'';  break;
            case HtmlEntityMapper::Sym_ldquo:   m_entry+="``";  break;
            case HtmlEntityMapper::Sym_rdquo:   m_entry+="''";  break;
            case HtmlEntityMapper::Sym_ndash:   m_entry+="--";  break;
            case HtmlEntityMapper::Sym_mdash:   m_entry+="---";  break;
            default:
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected symbol found as argument of \\addindex");
              break;
          }
        }
        break;
      case TK_COMMAND_AT:
        // fall through
      case TK_COMMAND_BS:
        switch (Mappers::cmdMapper->map(parser()->context.token->name))
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
               warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected command %s found as argument of \\addindex",
                              qPrint(parser()->context.token->name));
               break;
        }
      break;
      default:
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected token %s",
            DocTokenizer::tokToString(tok));
        break;
    }
  }
  parser()->tokenizer.setStatePara();
  m_entry = m_entry.stripWhiteSpace();
endindexentry:
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

//---------------------------------------------------------------------------

DocHtmlCaption::DocHtmlCaption(DocParser *parser,DocNodeVariant *parent,const HtmlAttribList &attribs)
  : DocCompoundNode(parser,parent)
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
        warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"Invalid caption id '%s'",qPrint(opt.value));
      }
    }
    else // copy attribute
    {
      m_attribs.push_back(opt);
    }
  }
}

int DocHtmlCaption::parse()
{
  AUTO_TRACE();
  int retval=0;
  auto ns = AutoNodeStack(parser(),thisVariant());
  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      switch (tok)
      {
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
            if (tagId==HTML_CAPTION && parser()->context.token->endTag) // found </caption> tag
            {
              retval = RetVal_OK;
              goto endcaption;
            }
            else
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected html tag <%s%s> found within <caption> context",
                  parser()->context.token->endTag?"/":"",qPrint(parser()->context.token->name));
            }
          }
          break;
        default:
          parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"<caption> tag");
          break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <caption> tag");
  }
endcaption:
  parser()->handlePendingStyleCommands(thisVariant(),children());
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlCell::parse()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (retval==TK_HTMLTAG)
    {
      int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
      if (tagId==HTML_TD && parser()->context.token->endTag) // found </td> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
      else if (tagId==HTML_TH && parser()->context.token->endTag) // found </th> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
    }
  }
  while ((retval==TK_NEWPARA) || (retval==RetVal_EndParBlock));
  if (par) par->markLast();

  return retval;
}

int DocHtmlCell::parseXml()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (retval==TK_HTMLTAG)
    {
      int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
      if (tagId==XML_ITEM && parser()->context.token->endTag) // found </item> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
      else if (tagId==XML_DESCRIPTION && parser()->context.token->endTag) // found </description> tag
      {
        retval=TK_NEWPARA; // ignore the tag
      }
    }
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  return retval;
}

uint32_t DocHtmlCell::rowSpan() const
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

uint32_t DocHtmlCell::colSpan() const
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

bool DocHtmlRow::isHeading() const
{ // a row is a table heading if all cells are marked as such
  bool heading=TRUE;
  for (const auto &n : children())
  {
    const DocHtmlCell *cell = std::get_if<DocHtmlCell>(&n);
    if (cell && !cell->isHeading())
    {
      heading = FALSE;
      break;
    }
  }
  return !children().empty() && heading;
}

int DocHtmlRow::parse()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  bool isHeading=FALSE;
  bool isFirst=TRUE;
  DocHtmlCell *cell=0;

  // get next token
  int tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HTML_TD && !parser()->context.token->endTag) // found <td> tag
    {
    }
    else if (tagId==HTML_TH && !parser()->context.token->endTag) // found <th> tag
    {
      isHeading=TRUE;
    }
    else // found some other tag
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <td> or <th> tag but "
          "found <%s> instead!",qPrint(parser()->context.token->name));
      parser()->tokenizer.pushBackHtmlTag(parser()->context.token->name);
      goto endrow;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto endrow;
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <td> or <th> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto endrow;
  }

  // parse one or more cells
  do
  {
    children().append<DocHtmlCell>(parser(),thisVariant(),
                                            parser()->context.token->attribs,
                                            isHeading);
    cell = children().get_last<DocHtmlCell>();
    cell->markFirst(isFirst);
    isFirst=FALSE;
    retval=cell->parse();
    isHeading = retval==RetVal_TableHCell;
  }
  while (retval==RetVal_TableCell || retval==RetVal_TableHCell);
  cell->markLast(TRUE);

endrow:
  return retval;
}

int DocHtmlRow::parseXml(bool isHeading)
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  bool isFirst=TRUE;
  DocHtmlCell *cell=0;

  // get next token
  int tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==XML_TERM && !parser()->context.token->endTag) // found <term> tag
    {
    }
    else if (tagId==XML_DESCRIPTION && !parser()->context.token->endTag) // found <description> tag
    {
    }
    else // found some other tag
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <term> or <description> tag but "
          "found <%s> instead!",qPrint(parser()->context.token->name));
      parser()->tokenizer.pushBackHtmlTag(parser()->context.token->name);
      goto endrow;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto endrow;
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <td> or <th> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto endrow;
  }

  do
  {
    children().append<DocHtmlCell>(parser(),thisVariant(),parser()->context.token->attribs,isHeading);
    cell = children().get_last<DocHtmlCell>();
    cell->markFirst(isFirst);
    isFirst=FALSE;
    retval=cell->parseXml();
  }
  while (retval==RetVal_TableCell || retval==RetVal_TableHCell);
  cell->markLast(TRUE);

endrow:
  return retval;
}

//---------------------------------------------------------------------------

bool DocHtmlTable::hasCaption() const
{
  return m_caption!=nullptr;
}

const DocNodeVariant *DocHtmlTable::caption() const
{
  return m_caption.get();
}

const DocNodeVariant *DocHtmlTable::firstRow() const
{
  if (!children().empty() && std::holds_alternative<DocHtmlRow>(children().front()))
  {
    return &children().front();
  }
  return 0;
}

int DocHtmlTable::parse()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

getrow:
  // get next token
  int tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HTML_THEAD && !parser()->context.token->endTag) // found <thead> tag
    {
      goto getrow;
    }
    else if (tagId==HTML_TBODY && !parser()->context.token->endTag) // found <tbody> tag
    {
      goto getrow;
    }
    else if (tagId==HTML_TFOOT && !parser()->context.token->endTag) // found <tfoot> tag
    {
      goto getrow;
    }
    else if (tagId==HTML_TR && !parser()->context.token->endTag) // found <tr> tag
    {
      // no caption, just rows
      retval=RetVal_TableRow;
    }
    else if (tagId==HTML_CAPTION && !parser()->context.token->endTag) // found <caption> tag
    {
      if (m_caption)
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"table already has a caption, found another one");
      }
      else
      {
        m_caption = createDocNode<DocHtmlCaption>(parser(),thisVariant(),parser()->context.token->attribs);
        retval=std::get<DocHtmlCaption>(*m_caption).parse();

        if (retval==RetVal_OK) // caption was parsed ok
        {
          goto getrow;
        }
      }
    }
    else // found wrong token
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <tr> or <caption> tag but "
          "found <%s%s> instead!", parser()->context.token->endTag ? "/" : "", qPrint(parser()->context.token->name));
    }
  }
  else if (tok==0) // premature end of comment
  {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
          " for a <tr> or <caption> tag");
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <tr> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
  }

  // parse one or more rows
  while (retval==RetVal_TableRow)
  {
    children().append<DocHtmlRow>(parser(),thisVariant(),parser()->context.token->attribs);
    retval = children().get_last<DocHtmlRow>()->parse();
  }

  computeTableGrid();

  return retval==RetVal_EndTable ? RetVal_OK : retval;
}

int DocHtmlTable::parseXml()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // get next token
  int tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=parser()->tokenizer.lex();
  // should find a html tag now
  int tagId=0;
  bool isHeader=FALSE;
  if (tok==TK_HTMLTAG)
  {
    tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==XML_ITEM && !parser()->context.token->endTag) // found <item> tag
    {
      retval=RetVal_TableRow;
    }
    if (tagId==XML_LISTHEADER && !parser()->context.token->endTag) // found <listheader> tag
    {
      retval=RetVal_TableRow;
      isHeader=TRUE;
    }
  }

  // parse one or more rows
  while (retval==RetVal_TableRow)
  {
    children().append<DocHtmlRow>(parser(),thisVariant(),parser()->context.token->attribs);
    DocHtmlRow *tr = children().get_last<DocHtmlRow>();
    retval=tr->parseXml(isHeader);
    isHeader=FALSE;
  }

  computeTableGrid();

  tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
  return tagId==XML_LIST && parser()->context.token->endTag ? RetVal_OK : retval;
}

/** Helper class to compute the grid for an HTML style table */
struct ActiveRowSpan
{
  ActiveRowSpan(uint32_t rows,uint32_t col) : rowsLeft(rows), column(col) {}
  uint32_t rowsLeft;
  uint32_t column;
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
  uint32_t maxCols=0;
  uint32_t rowIdx=1;
  for (auto &rowNode : children())
  {
    uint32_t colIdx=1;
    uint32_t cells=0;
    DocHtmlRow *row = std::get_if<DocHtmlRow>(&rowNode);
    if (row)
    {
      size_t i;
      for (auto &cellNode : row->children())
      {
        DocHtmlCell *cell = std::get_if<DocHtmlCell>(&cellNode);
        if (cell)
        {
          uint32_t rs = cell->rowSpan();
          uint32_t cs = cell->colSpan();

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

//---------------------------------------------------------------------------

int DocHtmlDescTitle::parse()
{
  AUTO_TRACE();
  int retval=0;
  auto ns = AutoNodeStack(parser(),thisVariant());

  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      const char *cmd_start = "\\";
      switch (tok)
      {
        case TK_COMMAND_AT:
          cmd_start = "@";
        // fall through
        case TK_COMMAND_BS:
          {
            QCString cmdName=parser()->context.token->name;
            bool isJavaLink=FALSE;
            switch (Mappers::cmdMapper->map(cmdName))
            {
              case CMD_REF:
                {
                  tok=parser()->tokenizer.lex();
                  if (tok!=TK_WHITESPACE)
                  {
                    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
                        qPrint(parser()->context.token->name));
                  }
                  else
                  {
                    parser()->tokenizer.setStateRef();
                    tok=parser()->tokenizer.lex(); // get the reference id
                    if (tok!=TK_WORD)
                    {
                      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of \\%s command",
                          DocTokenizer::tokToString(tok),qPrint(cmdName));
                    }
                    else
                    {
                      children().append<DocRef>(parser(),thisVariant(),parser()->context.token->name,parser()->context.context);
                      children().get_last<DocRef>()->parse();
                    }
                    parser()->tokenizer.setStatePara();
                  }
                }
                break;
              case CMD_JAVALINK:
                isJavaLink=TRUE;
                // fall through
              case CMD_LINK:
                {
                  tok=parser()->tokenizer.lex();
                  if (tok!=TK_WHITESPACE)
                  {
                    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
                        qPrint(cmdName));
                  }
                  else
                  {
                    parser()->tokenizer.setStateLink();
                    tok=parser()->tokenizer.lex();
                    if (tok!=TK_WORD)
                    {
                      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of \\%s command",
                          DocTokenizer::tokToString(tok),qPrint(cmdName));
                    }
                    else
                    {
                      parser()->tokenizer.setStatePara();
                      children().append<DocLink>(parser(),thisVariant(),parser()->context.token->name);
                      DocLink *lnk  = children().get_last<DocLink>();
                      QCString leftOver = lnk->parse(isJavaLink);
                      if (!leftOver.isEmpty())
                      {
                        children().append<DocWord>(parser(),thisVariant(),leftOver);
                      }
                    }
                  }
                }

                break;
              default:
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal command %s found as part of a <dt> tag",
                  qPrint(cmd_start + parser()->context.token->name));
            }
          }
          break;
        case TK_SYMBOL:
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported symbol \\%s found as part of a <dt> tag",
              qPrint(parser()->context.token->name));
          break;
        case TK_HTMLTAG:
          {
            int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
            if (tagId==HTML_DD && !parser()->context.token->endTag) // found <dd> tag
            {
              retval = RetVal_DescData;
              goto endtitle;
            }
            else if (tagId==HTML_DT && parser()->context.token->endTag)
            {
              // ignore </dt> tag.
            }
            else if (tagId==HTML_DT)
            {
              // missing <dt> tag.
              retval = RetVal_DescTitle;
              goto endtitle;
            }
            else if (tagId==HTML_DL && parser()->context.token->endTag)
            {
              retval=RetVal_EndDesc;
              goto endtitle;
            }
            else if (tagId==HTML_A)
            {
              if (!parser()->context.token->endTag)
              {
                parser()->handleAHref(thisVariant(),children(),parser()->context.token->attribs);
              }
            }
            else
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected html tag <%s%s> found within <dt> context",
                  parser()->context.token->endTag?"/":"",qPrint(parser()->context.token->name));
            }
          }
          break;
        default:
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected token %s found as part of a <dt> tag",
              DocTokenizer::tokToString(tok));
          break;
      }
    }
  }
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end of comment while inside"
        " <dt> tag");
  }
endtitle:
  parser()->handlePendingStyleCommands(thisVariant(),children());
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlDescData::parse()
{
  AUTO_TRACE();
  m_attribs = parser()->context.token->attribs;
  int retval=0;
  auto ns = AutoNodeStack(parser(),thisVariant());

  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlDescList::parse()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // get next token
  int tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HTML_DT && !parser()->context.token->endTag) // found <dt> tag
    {
      // continue
    }
    else // found some other tag
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <dt> tag but "
          "found <%s> instead!",qPrint(parser()->context.token->name));
      parser()->tokenizer.pushBackHtmlTag(parser()->context.token->name);
      goto enddesclist;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto enddesclist;
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <dt> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto enddesclist;
  }

  do
  {
    children().append<DocHtmlDescTitle>(parser(),thisVariant(),parser()->context.token->attribs);
    DocHtmlDescTitle *dt   = children().get_last<DocHtmlDescTitle>();
    children().append<DocHtmlDescData>(parser(),thisVariant());
    DocHtmlDescData *dd    = children().get_last<DocHtmlDescData>();
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
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while inside <dl> block");
  }

enddesclist:

  return retval==RetVal_EndDesc ? RetVal_OK : retval;
}

//---------------------------------------------------------------------------

int DocHtmlListItem::parse()
{
  AUTO_TRACE();
  int retval=0;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

int DocHtmlListItem::parseXml()
{
  AUTO_TRACE();
  int retval=0;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (retval==0) break;

    //printf("new item: retval=%x parser()->context.token->name=%s parser()->context.token->endTag=%d\n",
    //    retval,qPrint(parser()->context.token->name),parser()->context.token->endTag);
    if (retval==RetVal_ListItem)
    {
      break;
    }
  }
  while (retval!=RetVal_CloseXml);

  if (par) par->markLast();

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

//---------------------------------------------------------------------------

int DocHtmlList::parse()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  int num=1;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // get next token
  int tok=parser()->tokenizer.lex();
  // skip whitespace and paragraph breaks
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HTML_LI && !parser()->context.token->endTag) // found <li> tag
    {
      // ok, we can go on.
    }
    else if (((m_type==Unordered && tagId==HTML_UL) ||
              (m_type==Ordered   && tagId==HTML_OL)
             ) && parser()->context.token->endTag
            ) // found empty list
    {
      // add dummy item to obtain valid HTML
      children().append<DocHtmlListItem>(parser(),thisVariant(),HtmlAttribList(),1);
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"empty list!");
      retval = RetVal_EndList;
      goto endlist;
    }
    else // found some other tag
    {
      // add dummy item to obtain valid HTML
      children().append<DocHtmlListItem>(parser(),thisVariant(),HtmlAttribList(),1);
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <li> tag but "
          "found <%s%s> instead!",parser()->context.token->endTag?"/":"",qPrint(parser()->context.token->name));
      parser()->tokenizer.pushBackHtmlTag(parser()->context.token->name);
      goto endlist;
    }
  }
  else if (tok==0) // premature end of comment
  {
    // add dummy item to obtain valid HTML
    children().append<DocHtmlListItem>(parser(),thisVariant(),HtmlAttribList(),1);
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html list item");
    goto endlist;
  }
  else // token other than html token
  {
    // add dummy item to obtain valid HTML
    children().append<DocHtmlListItem>(parser(),thisVariant(),HtmlAttribList(),1);
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <li> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto endlist;
  }

  do
  {
    children().append<DocHtmlListItem>(parser(),thisVariant(),parser()->context.token->attribs,num++);
    DocHtmlListItem *li = children().get_last<DocHtmlListItem>();
    retval=li->parse();
  } while (retval==RetVal_ListItem);

  if (retval==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while inside <%cl> block",
        m_type==Unordered ? 'u' : 'o');
  }

endlist:
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval==RetVal_EndList ? RetVal_OK : retval;
}

int DocHtmlList::parseXml()
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  int num=1;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // get next token
  int tok=parser()->tokenizer.lex();
  // skip whitespace and paragraph breaks
  while (tok==TK_WHITESPACE || tok==TK_NEWPARA) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok==TK_HTMLTAG)
  {
    int tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    //printf("parser()->context.token->name=%s parser()->context.token->endTag=%d\n",qPrint(parser()->context.token->name),parser()->context.token->endTag);
    if (tagId==XML_ITEM && !parser()->context.token->endTag) // found <item> tag
    {
      // ok, we can go on.
    }
    else // found some other tag
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <item> tag but "
          "found <%s> instead!",qPrint(parser()->context.token->name));
      parser()->tokenizer.pushBackHtmlTag(parser()->context.token->name);
      goto endlist;
    }
  }
  else if (tok==0) // premature end of comment
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html list item");
    goto endlist;
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <item> tag but found %s token instead!",
        DocTokenizer::tokToString(tok));
    goto endlist;
  }

  do
  {
    children().append<DocHtmlListItem>(parser(),thisVariant(),parser()->context.token->attribs,num++);
    DocHtmlListItem *li   = children().get_last<DocHtmlListItem>();
    retval=li->parseXml();
    if (retval==0) break;
    //printf("retval=%x parser()->context.token->name=%s\n",retval,qPrint(parser()->context.token->name));
  } while (retval==RetVal_ListItem);

  if (retval==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while inside <list type=\"%s\"> block",
        m_type==Unordered ? "bullet" : "number");
  }

endlist:
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval==RetVal_EndList ||
         (retval==RetVal_CloseXml || parser()->context.token->name=="list") ?
         RetVal_OK : retval;
}

//--------------------------------------------------------------------------

int DocHtmlBlockQuote::parse()
{
  AUTO_TRACE();
  int retval=0;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return (retval==RetVal_EndBlockQuote) ? RetVal_OK : retval;
}

//---------------------------------------------------------------------------

int DocParBlock::parse()
{
  AUTO_TRACE();
  int retval=0;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval==TK_NEWPARA);
  if (par) par->markLast();

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return (retval==RetVal_EndBlockQuote) ? RetVal_OK : retval;
}

//---------------------------------------------------------------------------

DocSimpleListItem::DocSimpleListItem(DocParser *parser,DocNodeVariant *parent)
      : DocNode(parser,parent)
{
}


int DocSimpleListItem::parse()
{
  auto ns = AutoNodeStack(parser(),thisVariant());
  m_paragraph = createDocNode<DocPara>(parser(),thisVariant());
  DocPara *par = &std::get<DocPara>(*m_paragraph);
  int rv=par->parse();
  par->markFirst();
  par->markLast();
  return rv;
}

//--------------------------------------------------------------------------

int DocSimpleList::parse()
{
  auto ns = AutoNodeStack(parser(),thisVariant());
  int rv;
  do
  {
    children().append<DocSimpleListItem>(parser(),thisVariant());
    DocSimpleListItem *li   = children().get_last<DocSimpleListItem>();
    rv=li->parse();
  } while (rv==RetVal_ListItem);
  return (rv!=TK_NEWPARA) ? rv : RetVal_OK;
}

//--------------------------------------------------------------------------

DocAutoListItem::DocAutoListItem(DocParser *parser,DocNodeVariant *parent,int indent,int num)
      : DocCompoundNode(parser,parent), m_indent(indent), m_itemNum(num)
{
}

int DocAutoListItem::parse()
{
  AUTO_TRACE();
  int retval = RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=0;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    DocPara *par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (!par->isEmpty())
    {
      if (lastPar) lastPar->markLast(FALSE);
      lastPar=par;
    }
    else
    {
      children().pop_back();
    }
    // next paragraph should be more indented than the - marker to belong
    // to this item
  } while (retval==TK_NEWPARA && parser()->context.token->indent>m_indent);
  if (lastPar) lastPar->markLast();

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

//--------------------------------------------------------------------------

DocAutoList::DocAutoList(DocParser *parser,DocNodeVariant *parent,int indent,bool isEnumList,
                         int depth) :
      DocCompoundNode(parser,parent), m_indent(indent), m_isEnumList(isEnumList),
      m_depth(depth)
{
}

int DocAutoList::parse()
{
  AUTO_TRACE();
  int retval = RetVal_OK;
  int num=1;
  auto ns = AutoNodeStack(parser(),thisVariant());
  parser()->tokenizer.startAutoList();
	  // first item or sub list => create new list
  do
  {
    if (parser()->context.token->id!=-1) // explicitly numbered list
    {
      num=parser()->context.token->id;  // override num with real number given
    }
    children().append<DocAutoListItem>(parser(),thisVariant(),m_indent,num++);
    retval = children().get_last<DocAutoListItem>()->parse();
    //printf("DocAutoList::parse(): retval=0x%x parser()->context.token->indent=%d m_indent=%d "
    //       "m_isEnumList=%d parser()->context.token->isEnumList=%d parser()->context.token->name=%s\n",
    //       retval,parser()->context.token->indent,m_indent,m_isEnumList,parser()->context.token->isEnumList,
    //       qPrint(parser()->context.token->name));
    //printf("num=%d parser()->context.token->id=%d\n",num,parser()->context.token->id);
  }
  while (retval==TK_LISTITEM &&                // new list item
         m_indent==parser()->context.token->indent &&          // at same indent level
	 m_isEnumList==parser()->context.token->isEnumList &&  // of the same kind
         (parser()->context.token->id==-1 || parser()->context.token->id>=num)  // increasing number (or no number)
        );

  parser()->tokenizer.endAutoList();
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

//--------------------------------------------------------------------------

void DocTitle::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  parser()->tokenizer.setStateTitle();
  int tok;
  while ((tok=parser()->tokenizer.lex()))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"title section");
    }
  }
  parser()->tokenizer.setStatePara();
  parser()->handlePendingStyleCommands(thisVariant(),children());
}

void DocTitle::parseFromString(DocNodeVariant *parent,const QCString &text)
{
  children().append<DocWord>(parser(),parent,text);
}

//--------------------------------------------------------------------------

DocSimpleSect::DocSimpleSect(DocParser *parser,DocNodeVariant *parent,Type t) :
     DocCompoundNode(parser,parent), m_type(t)
{
}

bool DocSimpleSect::hasTitle() const
{
  return m_title && std::get<DocTitle>(*m_title).hasTitle();
}

int DocSimpleSect::parse(bool userTitle,bool needsSeparator)
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // handle case for user defined title
  if (userTitle)
  {
    m_title = createDocNode<DocTitle>(parser(),thisVariant());
    std::get_if<DocTitle>(m_title.get())->parse();
  }

  // add new paragraph as child
  if (!children().empty() && std::holds_alternative<DocPara>(children().back()))
  {
    std::get<DocPara>(children().back()).markLast(FALSE);
  }
  bool markFirst = children().empty();
  if (needsSeparator)
  {
    children().append<DocSimpleSectSep>(parser(),thisVariant());
  }
  children().append<DocPara>(parser(),thisVariant());
  DocPara *par  = children().get_last<DocPara>();
  if (markFirst)
  {
    par->markFirst();
  }
  par->markLast();

  // parse the contents of the paragraph
  int retval = par->parse();

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval; // 0==EOF, TK_NEWPARA, TK_LISTITEM, TK_ENDLIST, RetVal_SimpleSec
}

int DocSimpleSect::parseRcs()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  m_title = createDocNode<DocTitle>(parser(),thisVariant());
  DocTitle *title = &std::get<DocTitle>(*m_title);
  title->parseFromString(thisVariant(),parser()->context.token->name);

  QCString text = parser()->context.token->text;
  parser()->pushContext(); // this will create a new parser->context.token
  parser()->internalValidatingParseDoc(thisVariant(),children(),text);
  parser()->popContext(); // this will restore the old parser->context.token

  return RetVal_OK;
}

int DocSimpleSect::parseXml()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  int retval = RetVal_OK;
  for (;;)
  {
    // add new paragraph as child
    if (!children().empty() && std::holds_alternative<DocPara>(children().back()))
    {
      std::get<DocPara>(children().back()).markLast(false);
    }
    bool markFirst = children().empty();
    children().append<DocPara>(parser(),thisVariant());
    DocPara *par  = children().get_last<DocPara>();
    if (markFirst)
    {
      par->markFirst();
    }
    par->markLast();

    // parse the contents of the paragraph
    retval = par->parse();
    if (retval == 0) break;
    if (retval == RetVal_CloseXml)
    {
      retval = RetVal_OK;
      break;
    }
  }

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

void DocSimpleSect::appendLinkWord(const QCString &word)
{
  DocPara *p=0;
  if (children().empty() || (p=std::get_if<DocPara>(&children().back()))==0)
  {
    children().append<DocPara>(parser(),thisVariant());
    p = children().get_last<DocPara>();
  }
  else
  {
    // Comma-separate <seealso> links.
    p->injectToken(TK_WORD,",");
    p->injectToken(TK_WHITESPACE," ");
  }

  parser()->context.inSeeBlock=TRUE;
  p->injectToken(TK_LNKWORD,word);
  parser()->context.inSeeBlock=FALSE;
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
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());
  DocPara *par=0;
  QCString saveCmdName = cmdName;

  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    retval=RetVal_EndParBlock;
    goto endparamlist;
  }
  parser()->tokenizer.setStateParam();
  tok=parser()->tokenizer.lex();
  while (tok==TK_WORD) /* there is a parameter name */
  {
    if (m_type==DocParamSect::Param)
    {
      int typeSeparator = parser()->context.token->name.find('#'); // explicit type position
      if (typeSeparator!=-1)
      {
        parser()->handleParameterType(thisVariant(),m_paramTypes,parser()->context.token->name.left(typeSeparator));
        parser()->context.token->name = parser()->context.token->name.mid(typeSeparator+1);
        parser()->context.hasParamCommand=TRUE;
        parser()->checkArgumentName();
        if (parent() && std::holds_alternative<DocParamSect>(*parent()))
        {
          std::get<DocParamSect>(*parent()).m_hasTypeSpecifier=true;
        }
      }
      else
      {
        parser()->context.hasParamCommand=TRUE;
        parser()->checkArgumentName();
      }
    }
    else if (m_type==DocParamSect::RetVal)
    {
      parser()->context.hasReturnCommand=TRUE;
      parser()->checkRetvalName();
    }
    //m_params.append(parser()->context.token->name);
    parser()->handleLinkedWord(thisVariant(),m_params);
    tok=parser()->tokenizer.lex();
  }
  parser()->tokenizer.setStatePara();
  if (tok==0) /* premature end of comment block */
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(saveCmdName));
    retval=RetVal_EndParBlock;
    goto endparamlist;
  }
  if (tok!=TK_WHITESPACE) /* premature end of comment block */
  {
    if (tok!=TK_NEWPARA) /* empty param description */
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s in comment block while parsing the "
          "argument of command %s",DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    }
    retval=RetVal_EndParBlock;
    goto endparamlist;
  }

  m_paragraphs.append<DocPara>(parser(),thisVariant());
  par = m_paragraphs.get_last<DocPara>();
  retval = par->parse();
  par->markFirst();
  par->markLast();

endparamlist:
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

int DocParamList::parseXml(const QCString &paramName)
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  parser()->context.token->name = paramName;
  if (m_type==DocParamSect::Param)
  {
    parser()->context.hasParamCommand=TRUE;
    parser()->checkArgumentName();
  }
  else if (m_type==DocParamSect::RetVal)
  {
    parser()->context.hasReturnCommand=TRUE;
    parser()->checkRetvalName();
  }

  parser()->handleLinkedWord(thisVariant(),m_params);

  do
  {
    m_paragraphs.append<DocPara>(parser(),thisVariant());
    DocPara *par =  m_paragraphs.get_last<DocPara>();
    retval = par->parse();
    if (par->isEmpty()) // avoid adding an empty paragraph for the whitespace
                        // after </para> and before </param>
    {
      m_paragraphs.pop_back();
      break;
    }
    else // append the paragraph to the list
    {
      if (!m_paragraphs.empty())
      {
        m_paragraphs.get_last<DocPara>()->markLast(FALSE);
      }
      bool markFirst = m_paragraphs.empty();
      par = &std::get<DocPara>(m_paragraphs.back());
      if (markFirst)
      {
        par->markFirst();
      }
      par->markLast();
    }

    if (retval == 0) break;

  } while (retval==RetVal_CloseXml &&
           Mappers::htmlTagMapper->map(parser()->context.token->name)!=XML_PARAM &&
           Mappers::htmlTagMapper->map(parser()->context.token->name)!=XML_TYPEPARAM &&
           Mappers::htmlTagMapper->map(parser()->context.token->name)!=XML_EXCEPTION);


  if (retval==0) /* premature end of comment block */
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unterminated param or exception tag");
  }
  else
  {
    retval=RetVal_OK;
  }

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

//--------------------------------------------------------------------------

int DocParamSect::parse(const QCString &cmdName,bool xmlContext, Direction d)
{
  AUTO_TRACE();
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

  if (d!=Unspecified)
  {
    m_hasInOutSpecifier=TRUE;
  }

  if (!children().empty() && std::holds_alternative<DocParamList>(children().back()))
  {
    DocParamList &lastPl = std::get<DocParamList>(children().back());
    lastPl.markLast(false);
  }
  bool markFirst = children().empty();
  children().append<DocParamList>(parser(),thisVariant(),m_type,d);
  DocParamList *pl = children().get_last<DocParamList>();
  if (markFirst)
  {
    pl->markFirst();
  }
  pl->markLast();
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

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

//--------------------------------------------------------------------------

DocPara::DocPara(DocParser *parser,DocNodeVariant *parent) :
        DocCompoundNode(parser,parent),
        m_isFirst(FALSE), m_isLast(FALSE)
{
}

int DocPara::handleSimpleSection(DocSimpleSect::Type t, bool xmlContext)
{
  AUTO_TRACE();
  DocSimpleSect *ss=0;
  bool needsSeparator = FALSE;
  if (!children().empty() &&                                         // has previous element
      (ss=children().get_last<DocSimpleSect>()) &&                   // was a simple sect
      ss->type()==t &&                                               // of same type
      t!=DocSimpleSect::User)                                        // but not user defined
  {
    // append to previous section
    needsSeparator = TRUE;
  }
  else // start new section
  {
    children().append<DocSimpleSect>(parser(),thisVariant(),t);
    ss = children().get_last<DocSimpleSect>();
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
  AUTO_TRACE();
  DocParamSect   *ps = 0;
  if (!children().empty() &&                                       // previous element
      (ps=children().get_last<DocParamSect>()) &&                  // was a param sect
      ps->type()==t)                                               // of same type
  { // append to previous section ps
  }
  else // start new section
  {
    children().append<DocParamSect>(parser(),thisVariant(),t);
    ps = children().get_last<DocParamSect>();
  }
  int rv=ps->parse(cmdName,xmlContext,
                   static_cast<DocParamSect::Direction>(direction));
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(rv));
  return (rv!=TK_NEWPARA) ? rv : RetVal_OK;
}

void DocPara::handleCite()
{
  AUTO_TRACE();
  // get the argument of the cite command.
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint("cite"));
    return;
  }
  parser()->tokenizer.setStateCite();
  tok=parser()->tokenizer.lex();
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s\n", qPrint("cite"));
    return;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint("cite"));
    return;
  }
  parser()->context.token->sectionId = parser()->context.token->name;
  children().append<DocCite>(
        parser(),thisVariant(),parser()->context.token->name,parser()->context.context);

  parser()->tokenizer.setStatePara();
}

void DocPara::handleEmoji()
{
  AUTO_TRACE();
  // get the argument of the emoji command.
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint("emoji"));
    return;
  }
  parser()->tokenizer.setStateEmoji();
  tok=parser()->tokenizer.lex();
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"no emoji name given or unexpected end of comment block while parsing the "
        "argument of command %s", qPrint("emoji"));
    parser()->tokenizer.setStatePara();
    return;
  }
  else if (tok!=TK_WORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint("emoji"));
    parser()->tokenizer.setStatePara();
    return;
  }
  children().append<DocEmoji>(parser(),thisVariant(),parser()->context.token->name);
  parser()->tokenizer.setStatePara();
}

void DocPara::handleDoxyConfig()
{
  // get the argument of the cite command.
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\doxyconfig command");
    return;
  }
  parser()->tokenizer.setStateDoxyConfig();
  tok=parser()->tokenizer.lex();
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command \\doxyconfig\n");
    return;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of \\doxyconfig",
        DocTokenizer::tokToString(tok));
    return;
  }
  ConfigOption * opt = ConfigImpl::instance()->get(parser()->context.token->name);
  if (opt)
  {
    QCString optionValue;
    switch (opt->kind())
    {
      case ConfigOption::O_Bool:
        optionValue = *(static_cast<ConfigBool*>(opt)->valueStringRef());
        break;
      case ConfigOption::O_String:
        optionValue = *(static_cast<ConfigString*>(opt)->valueRef());
        break;
      case ConfigOption::O_Enum:
        optionValue = *(static_cast<ConfigEnum*>(opt)->valueRef());
        break;
      case ConfigOption::O_Int:
        optionValue = *(static_cast<ConfigInt*>(opt)->valueStringRef());
        break;
      case ConfigOption::O_List:
        {
          StringVector *lst = static_cast<ConfigList*>(opt)->valueRef();
          optionValue="";
          if (!lst->empty())
          {
            std::string lstFormat = theTranslator->trWriteList(static_cast<int>(lst->size())).str();
            static const reg::Ex marker(R"(@(\d+))");
            reg::Iterator it(lstFormat,marker);
            reg::Iterator end;
            size_t index=0;
            // now replace all markers with the real text
            for ( ; it!=end ; ++it)
            {
              const auto &match = *it;
              size_t newIndex = match.position();
              size_t matchLen = match.length();
              optionValue += lstFormat.substr(index,newIndex-index);
              unsigned long entryIndex = std::stoul(match[1].str());
              if (entryIndex<(unsigned long)lst->size())
              {
                optionValue += lst->at(entryIndex);
              }
              index=newIndex+matchLen;
            }
            optionValue+=lstFormat.substr(index);
          }
        }
        break;
      case ConfigOption::O_Obsolete:
        warn(parser()->context.fileName,parser()->tokenizer.getLineNr(), "Obsolete setting for '\\doxyconfig': '%s'",
              qPrint(parser()->context.token->name));
        break;
      case ConfigOption::O_Disabled:
        warn(parser()->context.fileName,parser()->tokenizer.getLineNr(),
              "Disabled setting (i.e. not supported in this doxygen executable) for '\\doxyconfig': '%s'",
              qPrint(parser()->context.token->name));
        break;
      case ConfigOption::O_Info:
        // nothing to show here
        break;
    }
    if (!optionValue.isEmpty())
    {
      children().append<DocWord>(parser(),thisVariant(),optionValue);
    }
  }
  else
  {
    warn(parser()->context.fileName,parser()->tokenizer.getLineNr(), "Unknown option for '\\doxyconfig': '%s'",
         qPrint(parser()->context.token->name));
    children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
  }
  parser()->tokenizer.setStatePara();
}

int DocPara::handleXRefItem()
{
  AUTO_TRACE();
  int retval=parser()->tokenizer.lex();
  ASSERT(retval==TK_WHITESPACE);
  parser()->tokenizer.setStateXRefItem();
  retval=parser()->tokenizer.lex();
  if (retval==RetVal_OK)
  {
    children().append<DocXRefItem>(parser(),thisVariant(),
                                   parser()->context.token->id,parser()->context.token->name);
    DocXRefItem *ref =  children().get_last<DocXRefItem>();
    if (!ref->parse())
    {
      children().pop_back();
    }
  }
  parser()->tokenizer.setStatePara();
  return retval;
}

void DocPara::handleShowDate()
{
  AUTO_TRACE();
  QCString fmt;
  QCString date;
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after '\\%s' command",
        qPrint("showdate"));
    return;
  }
  parser()->tokenizer.setStateQuotedString();
  tok = parser()->tokenizer.lex();
  if (tok!=TK_WORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"invalid <format> argument for command '\\showdate'");
    parser()->tokenizer.setStatePara();
    return;
  }
  fmt = parser()->context.token->name;

  parser()->tokenizer.setStateShowDate();
  tok = parser()->tokenizer.lex();

  QCString specDateRaw = tok==TK_WORD ? parser()->context.token->name : QCString();
  QCString specDate    = specDateRaw.stripWhiteSpace();
  bool specDateOnlyWS  = !specDateRaw.isEmpty() && specDate.isEmpty();
  if (!specDate.isEmpty() && tok!=TK_WORD && tok!=0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"invalid <date_time> argument for command '\\showdate'");
    parser()->tokenizer.setStatePara();
    return;
  }

  std::tm dat{};
  int specFormat=0;
  QCString err = dateTimeFromString(specDate,dat,specFormat);
  if (!err.isEmpty())
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"invalid <date_time> argument for command '\\showdate': %s",qPrint(err));
    parser()->tokenizer.setStatePara();
    return;
  }

  int usedFormat=0;
  QCString dateTimeStr = formatDateTime(fmt,dat,usedFormat);

  // warn the user if the format contains markers that are not explicitly filled in
  for (int i=0;i<SF_NumBits;i++)
  {
    int bitMask = 1<<i;
    if ((usedFormat&bitMask) && !(specFormat&bitMask)) // a part was used in the format string but its value was not specified.
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"'\\showdate' <format> parameter '%s' has %s related markers which are not specified in the <date_time> parameter '%s'. Filling in the current value for %s instead.",
          qPrint(fmt),SF_bit2str(i),qPrint(specDate),SF_bit2str(i));
    }
  }

  children().append<DocWord>(parser(),thisVariant(),dateTimeStr);
  if (specDateOnlyWS) // specDate is only whitespace
  {
    children().append<DocWhiteSpace>(parser(),thisVariant()," ");
  }
  parser()->tokenizer.setStatePara();
}

void DocPara::handleILine()
{
  AUTO_TRACE();
  parser()->tokenizer.setStateILine();
  int tok = parser()->tokenizer.lex();
  if (tok!=TK_WORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"invalid argument for command '\\iline'\n");
    return;
  }
  parser()->tokenizer.setStatePara();
}

void DocPara::handleIFile()
{
  AUTO_TRACE();
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\ifile command");
    return;
  }
  parser()->tokenizer.setStateFile();
  tok=parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
  if (tok!=TK_WORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of \\ifile",
        DocTokenizer::tokToString(tok));
    return;
  }
  parser()->context.fileName = parser()->context.token->name;
  parser()->tokenizer.setStatePara();
}


void DocPara::handleIncludeOperator(const QCString &cmdName,DocIncOperator::Type t)
{
  AUTO_TRACE("cmdName={}",cmdName);
  QCString saveCmdName = cmdName;
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStatePattern();
  tok=parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s", qPrint(saveCmdName));
    return;
  }
  else if (tok!=TK_WORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  auto it1 = children().size()>=1 ? std::prev(children().end()) : children().end();
  auto it2 = children().size()>=2 ? std::prev(it1)              : children().end();
  DocNodeVariant *n1 = it1!=children().end() ? &(*it1) : 0;
  DocNodeVariant *n2 = it2!=children().end() ? &(*it2) : 0;
  children().append<DocIncOperator>(parser(),thisVariant(),t,
                                    parser()->context.token->name,
                                    parser()->context.context,
                                    parser()->context.isExample,
                                    parser()->context.exampleName);
  DocIncOperator *op = children().get_last<DocIncOperator>();
  DocIncOperator *n1_docIncOp = std::get_if<DocIncOperator>(n1);
  DocWhiteSpace  *n1_docWs    = std::get_if<DocWhiteSpace >(n1);
  DocIncOperator *n2_docIncOp = std::get_if<DocIncOperator>(n2);
  bool isFirst = !n1 ||                            // no last node
                 (!n1_docIncOp && !n1_docWs) ||    // last node is not operator or whitespace
                 (n1_docWs && n2 && !n2_docIncOp); // last node is not operator
  op->markFirst(isFirst);
  op->markLast(true);
  if (n1_docIncOp)
  {
    n1_docIncOp->markLast(false);
  }
  else if (n1_docWs && n2_docIncOp)
  {
    n2_docIncOp->markLast(false);
  }
  op->parse();
}

template<class T>
void DocPara::handleFile(const QCString &cmdName)
{
  AUTO_TRACE("cmdName={}",cmdName);
  QCString saveCmdName = cmdName;
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStateFile();
  tok=parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
  if (tok!=TK_WORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  QCString name = parser()->context.token->name;
  children().append<T>(parser(),thisVariant(),name,
                       parser()->context.context,
                       parser()->context.fileName,
                       parser()->tokenizer.getLineNr());
  auto df = children().get_last<T>();
  if (!df->parse())
  {
    children().pop_back();
  }
}

void DocPara::handleVhdlFlow()
{
  AUTO_TRACE();
  children().append<DocVhdlFlow>(parser(),thisVariant());
  children().get_last<DocVhdlFlow>()->parse();
}

void DocPara::handleLink(const QCString &cmdName,bool isJavaLink)
{
  AUTO_TRACE("cmdName={} isJavaLink={}",cmdName,isJavaLink);
  QCString saveCmdName = cmdName;
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStateLink();
  tok=parser()->tokenizer.lex();
  if (tok!=TK_WORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"%s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  if (saveCmdName == "javalink")
  {
    children().append<DocStyleChange>(parser(),thisVariant(),
                                           parser()->context.nodeStack.size(),
                                           DocStyleChange::Code,cmdName,TRUE);
  }
  parser()->tokenizer.setStatePara();
  children().append<DocLink>(parser(),thisVariant(),parser()->context.token->name);
  DocLink *lnk =  children().get_last<DocLink>();
  if (saveCmdName == "javalink")
  {
    children().append<DocStyleChange>(parser(),thisVariant(),
                                           parser()->context.nodeStack.size(),
                                           DocStyleChange::Code,cmdName,FALSE);
  }
  QCString leftOver = lnk->parse(isJavaLink);
  if (!leftOver.isEmpty())
  {
    children().append<DocWord>(parser(),thisVariant(),leftOver);
  }
}

void DocPara::handleRef(const QCString &cmdName)
{
  AUTO_TRACE("cmdName={}",cmdName);
  QCString saveCmdName = cmdName;
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStateRef();
  tok=parser()->tokenizer.lex(); // get the reference id
  if (tok!=TK_WORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    goto endref;
  }
  children().append<DocRef>(parser(),thisVariant(),
                            parser()->context.token->name,
                            parser()->context.context);
  children().get_last<DocRef>()->parse();
endref:
  parser()->tokenizer.setStatePara();
}

void DocPara::handleInclude(const QCString &cmdName,DocInclude::Type t)
{
  AUTO_TRACE("cmdName={}",cmdName);
  QCString saveCmdName = cmdName;
  int tok=parser()->tokenizer.lex();
  bool isBlock = false;
  if (tok==TK_WORD && parser()->context.token->name=="{")
  {
    parser()->tokenizer.setStateOptions();
    tok=parser()->tokenizer.lex();
    parser()->tokenizer.setStatePara();
    StringVector optList=split(parser()->context.token->name.str(),",");
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
    else if (t==DocInclude::Snippet && contains("trimleft"))
    {
      t = DocInclude::SnippetTrimLeft;
    }
    tok=parser()->tokenizer.lex();
    if (tok!=TK_WHITESPACE)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
          qPrint(saveCmdName));
      return;
    }
  }
  else if (tok==TK_WORD && parser()->context.token->name=="[")
  {
    parser()->tokenizer.setStateBlock();
    tok=parser()->tokenizer.lex();
    isBlock = (parser()->context.token->name.stripWhiteSpace() == "block");
    parser()->tokenizer.setStatePara();
    tok=parser()->tokenizer.lex();
  }
  else if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStateFile();
  tok=parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(saveCmdName));
    return;
  }
  else if (tok!=TK_WORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  QCString fileName = parser()->context.token->name;
  QCString blockId;
  if (t==DocInclude::Snippet || t==DocInclude::SnipWithLines || t==DocInclude::SnippetDoc || t == DocInclude::SnippetTrimLeft)
  {
    if (fileName == "this") fileName=parser()->context.fileName;
    parser()->tokenizer.setStateSnippet();
    tok=parser()->tokenizer.lex();
    parser()->tokenizer.setStatePara();
    if (tok!=TK_WORD)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected block identifier, but found token %s instead while parsing the %s command",
          DocTokenizer::tokToString(tok),qPrint(saveCmdName));
      return;
    }
    blockId = "["+parser()->context.token->name+"]";
  }

  // This is the only place to handle the \includedoc and \snippetdoc commands,
  // as the content is included here as if it is really here.
  if (t==DocInclude::IncludeDoc || t==DocInclude::SnippetDoc)
  {
     QCString inc_text;
     int inc_line  = 1;
     parser()->readTextFileByName(fileName,inc_text);
     if (t==DocInclude::SnippetDoc)
     {
       int count;
       if (!blockId.isEmpty() && (count=inc_text.contains(blockId.data()))!=2)
       {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"block marked with %s for \\snippet should appear twice in file %s, found it %d times\n",
            qPrint(blockId),qPrint(fileName),count);
       }
       inc_line = lineBlock(inc_text, blockId);
       inc_text = extractBlock(inc_text, blockId);
     }

     Markdown markdown(fileName,inc_line);
     QCString strippedDoc = stripIndentation(inc_text);
     QCString processedDoc = Config_getBool(MARKDOWN_SUPPORT) ? markdown.process(strippedDoc,inc_line) : strippedDoc;

     parser()->pushContext();
     parser()->context.fileName = fileName;
     parser()->tokenizer.setLineNr(inc_line);
     parser()->internalValidatingParseDoc(thisVariant(),children(),processedDoc);
     parser()->popContext();
  }
  else
  {
    children().append<DocInclude>(parser(),thisVariant(),fileName,
                                  parser()->context.context,t,
                                  parser()->context.isExample,
                                  parser()->context.exampleName,
                                  blockId,isBlock);
    children().get_last<DocInclude>()->parse();
  }
}

void DocPara::handleSection(const QCString &cmdName)
{
  AUTO_TRACE("cmdName={}",cmdName);
  QCString saveCmdName = cmdName;
  // get the argument of the section command.
  int tok=parser()->tokenizer.lex();
  if (tok!=TK_WHITESPACE)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  tok=parser()->tokenizer.lex();
  if (tok==0)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s\n", qPrint(saveCmdName));
    return;
  }
  else if (tok!=TK_WORD && tok!=TK_LNKWORD)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        DocTokenizer::tokToString(tok),qPrint(saveCmdName));
    return;
  }
  parser()->context.token->sectionId = parser()->context.token->name;
  parser()->tokenizer.setStateSkipTitle();
  parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
}

int DocPara::handleHtmlHeader(const HtmlAttribList &tagHtmlAttribs,int level)
{
  AUTO_TRACE();
  children().append<DocHtmlHeader>(parser(),thisVariant(),tagHtmlAttribs,level);
  int retval = children().get_last<DocHtmlHeader>()->parse();
  return (retval==RetVal_OK) ? TK_NEWPARA : retval;
}

// For XML tags whose content is stored in attributes rather than
// contained within the element, we need a way to inject the attribute
// text into the current paragraph.
bool DocPara::injectToken(int tok,const QCString &tokText)
{
  AUTO_TRACE();
  parser()->context.token->name = tokText;
  return parser()->defaultHandleToken(thisVariant(),tok,children());
}

int DocPara::handleStartCode()
{
  AUTO_TRACE();
  int retval = parser()->tokenizer.lex();
  QCString lang = parser()->context.token->name;
  if (!lang.isEmpty() && lang.at(0)!='.')
  {
    lang="."+lang;
  }
  if (parser()->context.xmlComment)
  {
    parser()->context.token->verb = substitute(substitute(parser()->context.token->verb,"&lt;","<"),"&gt;",">");
  }
  // search for the first non-whitespace line, index is stored in li
  int i=0,li=0,l=parser()->context.token->verb.length();
  while (i<l && (parser()->context.token->verb.at(i)==' ' || parser()->context.token->verb.at(i)=='\n'))
  {
    if (parser()->context.token->verb.at(i)=='\n') li=i+1;
    i++;
  }
  children().append<DocVerbatim>(parser(),thisVariant(),
                                 parser()->context.context,
                                 stripIndentation(parser()->context.token->verb.mid(li)),
                                 DocVerbatim::Code,
                                 parser()->context.isExample,
                                 parser()->context.exampleName,
                                 FALSE,lang);
  if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"code section ended without end marker");
  parser()->tokenizer.setStatePara();
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

void DocPara::handleInheritDoc()
{
  if (parser()->context.memberDef) // inheriting docs from a member
  {
    const MemberDef *reMd = parser()->context.memberDef->reimplements();
    if (reMd) // member from which was inherited.
    {
      const MemberDef *thisMd = parser()->context.memberDef;
      //printf("{InheritDocs:%s=>%s}\n",qPrint(parser()->context.memberDef->qualifiedName()),qPrint(reMd->qualifiedName()));
      parser()->pushContext();
      parser()->context.scope=reMd->getOuterScope();
      if (parser()->context.scope!=Doxygen::globalScope)
      {
        parser()->context.context=parser()->context.scope->name();
      }
      parser()->context.memberDef=reMd;
      while (!parser()->context.styleStack.empty()) parser()->context.styleStack.pop();
      while (!parser()->context.nodeStack.empty()) parser()->context.nodeStack.pop();
      parser()->context.copyStack.push_back(reMd);
      parser()->internalValidatingParseDoc(thisVariant(),children(),reMd->briefDescription());
      parser()->internalValidatingParseDoc(thisVariant(),children(),reMd->documentation());
      parser()->context.copyStack.pop_back();
      auto hasParamCommand   = parser()->context.hasParamCommand;
      auto hasReturnCommand  = parser()->context.hasReturnCommand;
      auto retvalsFound      = parser()->context.retvalsFound;
      auto paramsFound       = parser()->context.paramsFound;
      parser()->popContext();
      parser()->context.hasParamCommand      = hasParamCommand;
      parser()->context.hasReturnCommand     = hasReturnCommand;
      parser()->context.retvalsFound         = retvalsFound;
      parser()->context.paramsFound          = paramsFound;
      parser()->context.memberDef = thisMd;
    }
  }
}


int DocPara::handleCommand(const QCString &cmdName, const int tok)
{
  AUTO_TRACE("cmdName={}",cmdName);
  int retval = RetVal_OK;
  int cmdId = Mappers::cmdMapper->map(cmdName);
  switch (cmdId)
  {
    case CMD_UNKNOWN:
      children().append<DocWord>(parser(),thisVariant(),TK_COMMAND_CHAR(tok) + cmdName);
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Found unknown command '%s%s'",TK_COMMAND_CHAR(tok),qPrint(cmdName));
      break;
    case CMD_EMPHASIS:
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Italic,cmdName,TRUE);
      retval=parser()->handleStyleArgument(thisVariant(),children(),cmdName);
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Italic,cmdName,FALSE);
      if (retval!=TK_WORD) children().append<DocWhiteSpace>(parser(),thisVariant()," ");
      break;
    case CMD_BOLD:
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Bold,cmdName,TRUE);
      retval=parser()->handleStyleArgument(thisVariant(),children(),cmdName);
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Bold,cmdName,FALSE);
      if (retval!=TK_WORD) children().append<DocWhiteSpace>(parser(),thisVariant()," ");
      break;
    case CMD_CODE:
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Code,cmdName,TRUE);
      retval=parser()->handleStyleArgument(thisVariant(),children(),cmdName);
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Code,cmdName,FALSE);
      if (retval!=TK_WORD) children().append<DocWhiteSpace>(parser(),thisVariant()," ");
      break;
    case CMD_BSLASH:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_BSlash);
      break;
    case CMD_AT:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_At);
      break;
    case CMD_LESS:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Less);
      break;
    case CMD_GREATER:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Greater);
      break;
    case CMD_AMP:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Amp);
      break;
    case CMD_DOLLAR:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Dollar);
      break;
    case CMD_HASH:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Hash);
      break;
    case CMD_PIPE:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Pipe);
      break;
    case CMD_DCOLON:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_DoubleColon);
      break;
    case CMD_PERCENT:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Percent);
      break;
    case CMD_NDASH:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      break;
    case CMD_MDASH:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      break;
    case CMD_QUOTE:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Quot);
      break;
    case CMD_PUNT:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Dot);
      break;
    case CMD_PLUS:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Plus);
      break;
    case CMD_MINUS:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      break;
    case CMD_EQUAL:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Equal);
      break;
    case CMD_SA:
      parser()->context.inSeeBlock=TRUE;
      retval = handleSimpleSection(DocSimpleSect::See);
      parser()->context.inSeeBlock=FALSE;
      break;
    case CMD_RETURN:
      retval = handleSimpleSection(DocSimpleSect::Return);
      parser()->context.hasReturnCommand=TRUE;
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
        children().append<DocSimpleList>(parser(),thisVariant());
        retval = children().get_last<DocSimpleList>()->parse();
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
    case CMD_ISTARTCODE:
      {
        parser()->tokenizer.setStateICode();
        retval = handleStartCode();
      }
      break;
    case CMD_STARTCODE:
      {
        parser()->tokenizer.setStateCode();
        retval = handleStartCode();
      }
      break;
    case CMD_HTMLONLY:
      {
        parser()->tokenizer.setStateHtmlOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::HtmlOnly,parser()->context.isExample,parser()->context.exampleName,parser()->context.token->name=="block");
        if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"htmlonly section ended without end marker");
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_MANONLY:
      {
        parser()->tokenizer.setStateManOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::ManOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"manonly section ended without end marker");
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_RTFONLY:
      {
        parser()->tokenizer.setStateRtfOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::RtfOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"rtfonly section ended without end marker");
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_LATEXONLY:
      {
        parser()->tokenizer.setStateLatexOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::LatexOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"latexonly section ended without end marker");
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_XMLONLY:
      {
        parser()->tokenizer.setStateXmlOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::XmlOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"xmlonly section ended without end marker");
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_DBONLY:
      {
        parser()->tokenizer.setStateDbOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::DocbookOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"docbookonly section ended without end marker");
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_ILITERAL:
      {
        DocVerbatim::Type t = DocVerbatim::JavaDocLiteral;
        parser()->tokenizer.setStateILiteralOpt();
        retval = parser()->tokenizer.lex();

        QCString fullMatch = parser()->context.token->verb;
        int idx = fullMatch.find('{');
        int idxEnd = fullMatch.find("}",idx+1);
        StringVector optList;
        if (idx != -1) // options present
        {
           QCString optStr = fullMatch.mid(idx+1,idxEnd-idx-1).stripWhiteSpace();
           optList = split(optStr.str(),",");
           for (const auto &opt : optList)
           {
             if (opt.empty()) continue;
             QCString locOpt(opt);
             locOpt = locOpt.stripWhiteSpace().lower();
             if (locOpt == "code")
             {
               t = DocVerbatim::JavaDocCode;
             }
             else if (!locOpt.isEmpty())
             {
               warn(parser()->context.fileName,parser()->tokenizer.getLineNr(), "Unknown option '%s' for '\\iliteral'",qPrint(opt));
             }
           }
        }

        parser()->tokenizer.setStateILiteral();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,t,parser()->context.isExample,parser()->context.exampleName);
        if (retval==0)
        {
          if (t == DocVerbatim::JavaDocCode)
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"javadoc code section ended without end marker");
          }
          else
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"javadoc literal section ended without end marker");
          }
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_IVERBATIM:
    case CMD_VERBATIM:
      {
        if (cmdId == CMD_VERBATIM)
        {
          parser()->tokenizer.setStateVerbatim();
        }
        else
        {
          parser()->tokenizer.setStateIVerbatim();
        }
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::Verbatim,parser()->context.isExample,parser()->context.exampleName);
        if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"verbatim section ended without end marker");
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_DOT:
      {
        children().append<DocVerbatim>(parser(),thisVariant(),
                                       parser()->context.context,
                                       parser()->context.token->verb,
                                       DocVerbatim::Dot,
                                       parser()->context.isExample,
                                       parser()->context.exampleName);
        DocVerbatim *dv = children().get_last<DocVerbatim>();
        parser()->tokenizer.setStatePara();
        QCString width,height;
        parser()->defaultHandleTitleAndSize(CMD_DOT,&children().back(),dv->children(),width,height);
        parser()->tokenizer.setStateDot();
        retval = parser()->tokenizer.lex();
        dv->setText(parser()->context.token->verb);
        dv->setWidth(width);
        dv->setHeight(height);
        dv->setLocation(parser()->context.fileName,parser()->tokenizer.getLineNr());
        if (!Config_getBool(HAVE_DOT))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"ignoring \\dot command because HAVE_DOT is not set");
          children().pop_back();
        }
        if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"dot section ended without end marker");
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_MSC:
      {
        children().append<DocVerbatim>(parser(),thisVariant(),
                                       parser()->context.context,
                                       parser()->context.token->verb,
                                       DocVerbatim::Msc,
                                       parser()->context.isExample,
                                       parser()->context.exampleName);
        DocVerbatim *dv = children().get_last<DocVerbatim>();
        parser()->tokenizer.setStatePara();
        QCString width,height;
        parser()->defaultHandleTitleAndSize(CMD_MSC,&children().back(),dv->children(),width,height);
        parser()->tokenizer.setStateMsc();
        retval = parser()->tokenizer.lex();
        dv->setText(parser()->context.token->verb);
        dv->setWidth(width);
        dv->setHeight(height);
        dv->setLocation(parser()->context.fileName,parser()->tokenizer.getLineNr());
        if (retval==0)
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"msc section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_STARTUML:
      {
        QCString jarPath = Config_getString(PLANTUML_JAR_PATH);
        parser()->tokenizer.setStatePlantUMLOpt();
        retval = parser()->tokenizer.lex();
        QCString fullMatch = parser()->context.token->sectionId;
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
                 warn(parser()->context.fileName,parser()->tokenizer.getLineNr(), "Multiple definition of engine for '\\startuml'");
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
                 warn(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Multiple use of filename for '\\startuml'");
               }
             }
           }
        }
        else
        {
          sectionId = parser()->context.token->sectionId;
        }
        if (engine.isEmpty()) engine = "uml";

        if (sectionId.isEmpty())
        {
          parser()->tokenizer.setStatePlantUMLOpt();
          retval = parser()->tokenizer.lex();
          assert(retval==RetVal_OK);

          sectionId = parser()->context.token->sectionId;
          sectionId = sectionId.stripWhiteSpace();
        }

        QCString plantFile(sectionId);
        children().append<DocVerbatim>(parser(),thisVariant(),
                                       parser()->context.context,
                                       parser()->context.token->verb,
                                       DocVerbatim::PlantUML,
                                       FALSE,plantFile);
        DocVerbatim *dv = children().get_last<DocVerbatim>();
        dv->setEngine(engine);
        parser()->tokenizer.setStatePara();
        QCString width,height;
        parser()->defaultHandleTitleAndSize(CMD_STARTUML,&children().back(),dv->children(),width,height);
        parser()->tokenizer.setStatePlantUML();
        retval = parser()->tokenizer.lex();
        int line = 0;
        QCString trimmedVerb = stripLeadingAndTrailingEmptyLines(parser()->context.token->verb,line);
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
        dv->setLocation(parser()->context.fileName,parser()->tokenizer.getLineNr());
        if (jarPath.isEmpty())
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"ignoring \\startuml command because PLANTUML_JAR_PATH is not set");
          children().pop_back();
        }
        if (retval==0) warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"startuml section ended without end marker");
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_ENDPARBLOCK:
      retval=RetVal_EndParBlock;
      break;
    case CMD_ENDICODE:
    case CMD_ENDCODE:
    case CMD_ENDHTMLONLY:
    case CMD_ENDMANONLY:
    case CMD_ENDRTFONLY:
    case CMD_ENDLATEXONLY:
    case CMD_ENDXMLONLY:
    case CMD_ENDDBONLY:
    case CMD_ENDLINK:
    case CMD_ENDVERBATIM:
    case CMD_ENDIVERBATIM:
    case CMD_ENDILITERAL:
    case CMD_ENDDOT:
    case CMD_ENDMSC:
    case CMD_ENDUML:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected command %s",qPrint(parser()->context.token->name));
      break;
    case CMD_PARAM:
      retval = handleParamSection(cmdName,DocParamSect::Param,FALSE,parser()->context.token->paramDir);
      break;
    case CMD_TPARAM:
      retval = handleParamSection(cmdName,DocParamSect::TemplateParam,FALSE,parser()->context.token->paramDir);
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
        children().append<DocLineBreak>(parser(),thisVariant());
      }
      break;
    case CMD_IANCHOR:
    case CMD_ANCHOR:
      {
        parser()->handleAnchor(thisVariant(),children());
      }
      break;
    case CMD_ADDINDEX:
      {
        children().append<DocIndexEntry>(parser(),thisVariant(),
                     parser()->context.scope!=Doxygen::globalScope?parser()->context.scope:0,
                     parser()->context.memberDef);
        retval = children().get_last<DocIndexEntry>()->parse();
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
        children().append<DocParBlock>(parser(),thisVariant());
        retval = children().get_last<DocParBlock>()->parse();
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
      parser()->handleImage(thisVariant(),children());
      break;
    case CMD_DOTFILE:
      if (!Config_getBool(HAVE_DOT))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
                       "ignoring \\dotfile command because HAVE_DOT is not set");
      }
      else
      {
        handleFile<DocDotFile>(cmdName);
      }
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
    case CMD_DOXYCONFIG:
      handleDoxyConfig();
      break;
    case CMD_REF: // fall through
    case CMD_SUBPAGE:
      handleRef(cmdName);
      break;
    case CMD_SECREFLIST:
      {
        children().append<DocSecRefList>(parser(),thisVariant());
        children().get_last<DocSecRefList>()->parse();
      }
      break;
    case CMD_SECREFITEM:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected command %s",qPrint(parser()->context.token->name));
      break;
    case CMD_ENDSECREFLIST:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected command %s",qPrint(parser()->context.token->name));
      break;
    case CMD_FORMULA:
      {
        children().append<DocFormula>(parser(),thisVariant(),parser()->context.token->id);
      }
      break;
    //case CMD_LANGSWITCH:
    //  retval = handleLanguageSwitch();
    //  break;
    case CMD_INTERNALREF:
      //warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected command %s",qPrint(parser()->context.token->name));
      {
        parser()->handleInternalRef(thisVariant(),children());
        parser()->tokenizer.setStatePara();
      }
      break;
    case CMD_INHERITDOC:
      handleInheritDoc();
      break;
    case CMD_SHOWDATE:
      handleShowDate();
      break;
    case CMD_ILINE:
      handleILine();
      break;
    case CMD_IFILE:
      handleIFile();
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
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
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
  AUTO_TRACE("tagName={} #tagHtmlAttrs={}",tagName,tagHtmlAttribs.size());
  int retval=RetVal_OK;
  int tagId = Mappers::htmlTagMapper->map(tagName);
  if (parser()->context.token->emptyTag && !(tagId&XML_CmdMask) &&
      tagId!=HTML_UNKNOWN && tagId!=HTML_IMG && tagId!=HTML_BR && tagId!=HTML_HR && tagId!=HTML_P
      && tagId!=HTML_DIV && tagId!=HTML_SPAN)
  {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"HTML tag ('<%s/>') may not use the 'empty tag' XHTML syntax.",
                     qPrint(tagName));
  }
  switch (tagId)
  {
    case HTML_UL:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlList>(parser(),thisVariant(),
                                       tagHtmlAttribs,DocHtmlList::Unordered);
        retval=children().get_last<DocHtmlList>()->parse();
      }
      break;
    case HTML_OL:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlList>(parser(),thisVariant(),
                                       tagHtmlAttribs,DocHtmlList::Ordered);
        retval=children().get_last<DocHtmlList>()->parse();
      }
      break;
    case HTML_LI:
      if (parser()->context.token->emptyTag) break;
      if (!insideUL(thisVariant()) && !insideOL(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"lonely <li> tag found");
      }
      else
      {
        retval=RetVal_ListItem;
      }
      break;
    case HTML_BOLD:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Bold,tagName,&parser()->context.token->attribs);
      break;
    case HTML_S:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::S,tagName,&parser()->context.token->attribs);
      break;
    case HTML_STRIKE:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Strike,tagName,&parser()->context.token->attribs);
      break;
    case HTML_DEL:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Del,tagName,&parser()->context.token->attribs);
      break;
    case HTML_UNDERLINE:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Underline,tagName,&parser()->context.token->attribs);
      break;
    case HTML_INS:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Ins,tagName,&parser()->context.token->attribs);
      break;
    case HTML_CODE:
      if (parser()->context.token->emptyTag) break;
      if (/*getLanguageFromFileName(parser()->context.fileName)==SrcLangExt_CSharp ||*/ parser()->context.xmlComment)
        // for C# source or inside a <summary> or <remark> section we
        // treat <code> as an XML tag (so similar to @code)
      {
        parser()->tokenizer.setStateXmlCode();
        retval = handleStartCode();
      }
      else // normal HTML markup
      {
        parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Code,tagName,&parser()->context.token->attribs);
      }
      break;
    case HTML_EMPHASIS:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Italic,tagName,&parser()->context.token->attribs);
      break;
    case HTML_DIV:
      parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Div,tagName,&parser()->context.token->attribs);
      if (parser()->context.token->emptyTag) parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Div,tagName);
      break;
    case HTML_SPAN:
      parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Span,tagName,&parser()->context.token->attribs);
      if (parser()->context.token->emptyTag) parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Span,tagName);
      break;
    case HTML_SUB:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Subscript,tagName,&parser()->context.token->attribs);
      break;
    case HTML_SUP:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Superscript,tagName,&parser()->context.token->attribs);
      break;
    case HTML_CENTER:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Center,tagName,&parser()->context.token->attribs);
      break;
    case HTML_SMALL:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Small,tagName,&parser()->context.token->attribs);
      break;
    case HTML_CITE:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Cite,tagName,&parser()->context.token->attribs);
      break;
    case HTML_PRE:
      if (parser()->context.token->emptyTag) break;
      parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Preformatted,tagName,&parser()->context.token->attribs);
      setInsidePreformatted(TRUE);
      parser()->tokenizer.setInsidePre(TRUE);
      break;
    case HTML_P:
      retval=TK_NEWPARA;
      break;
    case HTML_DL:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlDescList>(parser(),thisVariant(),tagHtmlAttribs);
        retval=children().get_last<DocHtmlDescList>()->parse();
      }
      break;
    case HTML_DT:
      retval = RetVal_DescTitle;
      break;
    case HTML_DD:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag <dd> found");
      break;
    case HTML_TABLE:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlTable>(parser(),thisVariant(),tagHtmlAttribs);
        retval=children().get_last<DocHtmlTable>()->parse();
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
    case HTML_THEAD:
    case HTML_TBODY:
    case HTML_TFOOT:
      // for time being ignore </t....> tag
      break;
    case HTML_CAPTION:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag <caption> found");
      break;
    case HTML_BR:
      {
        children().append<DocLineBreak>(parser(),thisVariant(),tagHtmlAttribs);
      }
      break;
    case HTML_HR:
      {
        children().append<DocHorRuler>(parser(),thisVariant(),tagHtmlAttribs);
      }
      break;
    case HTML_A:
      retval = parser()->handleAHref(thisVariant(),children(),tagHtmlAttribs);
      break;
    case HTML_H1:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,1);
      break;
    case HTML_H2:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,2);
      break;
    case HTML_H3:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,3);
      break;
    case HTML_H4:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,4);
      break;
    case HTML_H5:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,5);
      break;
    case HTML_H6:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,6);
      break;
    case HTML_IMG:
      {
        parser()->handleImg(thisVariant(),children(),tagHtmlAttribs);
      }
      break;
    case HTML_DETAILS:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlDetails>(parser(),thisVariant(),tagHtmlAttribs);
        retval=children().get_last<DocHtmlDetails>()->parse();
      }
      break;
    case HTML_BLOCKQUOTE:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlBlockQuote>(parser(),thisVariant(),tagHtmlAttribs);
        retval = children().get_last<DocHtmlBlockQuote>()->parse();
      }
      break;

    case XML_SUMMARY:
      if (insideDetails(thisVariant()))
      {
        if (!parser()->context.token->emptyTag)
        {
          DocNodeVariant *n=parent();
          while (n && !std::holds_alternative<DocHtmlDetails>(*n)) n=::parent(n);
          DocHtmlDetails *d = std::get_if<DocHtmlDetails>(n);
          if (d)
          {
            if (!d->summary()) // details section does not have a summary yet
            {
              d->parseSummary(n,parser()->context.token->attribs);
            }
            else
            {
              retval = TK_NEWPARA;
            }
          }
        }
      }
      break;
    case XML_REMARKS:
    case XML_EXAMPLE:
      parser()->context.xmlComment=TRUE;
      // fall through
    case XML_VALUE:
    case XML_PARA:
      if (!children().empty())
      {
        retval = TK_NEWPARA;
      }
      break;
    case XML_DESCRIPTION:
      if (insideTable(thisVariant()))
      {
        retval=RetVal_TableCell;
      }
      break;
    case XML_C:
      parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Code,tagName,&parser()->context.token->attribs);
      break;
    case XML_PARAM:
    case XML_TYPEPARAM:
      {
        parser()->context.xmlComment=TRUE;
        QCString paramName;
        if (findAttribute(tagHtmlAttribs,"name",&paramName))
        {
          if (paramName.isEmpty())
          {
            if (Config_getBool(WARN_NO_PARAMDOC))
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"empty 'name' attribute for <param%s> tag.",tagId==XML_PARAM?"":"type");
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
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing 'name' attribute from <param%s> tag.",tagId==XML_PARAM?"":"type");
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
          children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Italic,tagName,TRUE);
          children().append<DocWord>(parser(),thisVariant(),paramName);
          children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Italic,tagName,FALSE);
          if (retval!=TK_WORD) children().append<DocWhiteSpace>(parser(),thisVariant()," ");
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing 'name' attribute from <param%sref> tag.",tagId==XML_PARAMREF?"":"type");
        }
      }
      break;
    case XML_EXCEPTION:
      {
        parser()->context.xmlComment=TRUE;
        QCString exceptName;
        if (findAttribute(tagHtmlAttribs,"cref",&exceptName))
        {
          unescapeCRef(exceptName);
          retval = handleParamSection(exceptName,DocParamSect::Exception,TRUE);
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing 'cref' attribute from <exception> tag.");
        }
      }
      break;
    case XML_ITEM:
    case XML_LISTHEADER:
      if (insideTable(thisVariant()))
      {
        retval=RetVal_TableRow;
      }
      else if (insideUL(thisVariant()) || insideOL(thisVariant()))
      {
        retval=RetVal_ListItem;
      }
      else
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"lonely <item> tag found");
      }
      break;
    case XML_RETURNS:
      parser()->context.xmlComment=TRUE;
      retval = handleSimpleSection(DocSimpleSect::Return,TRUE);
      parser()->context.hasReturnCommand=TRUE;
      break;
    case XML_TERM:
      if (insideTable(thisVariant()))
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
        //printf("XML_SEE: empty tag=%d\n",parser()->context.token->emptyTag);
        if (findAttribute(tagHtmlAttribs,"cref",&cref))
        {
          unescapeCRef(cref);
          if (parser()->context.token->emptyTag) // <see cref="..."/> style
          {
            bool inSeeBlock = parser()->context.inSeeBlock;
            parser()->context.token->name = cref;
            parser()->context.inSeeBlock = TRUE;
            parser()->handleLinkedWord(thisVariant(),children(),TRUE);
            parser()->context.inSeeBlock = inSeeBlock;
          }
          else // <see cref="...">...</see> style
          {
            //DocRef *ref = new DocRef(this,cref);
            //children().append(ref);
            //ref->parse();
            parser()->tokenizer.setStatePara();
            children().append<DocLink>(parser(),thisVariant(),cref);
            DocLink *lnk  = children().get_last<DocLink>();
            QCString leftOver = lnk->parse(FALSE,TRUE);
            if (!leftOver.isEmpty())
            {
              children().append<DocWord>(parser(),thisVariant(),leftOver);
            }
          }
        }
        else if (findAttribute(tagHtmlAttribs,"langword",&cref)) // <see langword="..."/> or <see langword="..."></see>
        {
          bool inSeeBlock = parser()->context.inSeeBlock;
          parser()->context.token->name = cref;
          parser()->context.inSeeBlock = TRUE;
          children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Code,tagName,TRUE);
          parser()->handleLinkedWord(thisVariant(),children(),TRUE);
          children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Code,tagName,FALSE);
          parser()->context.inSeeBlock = inSeeBlock;
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing 'cref' or 'langword' attribute from <see> tag.");
        }
      }
      break;
    case XML_SEEALSO:
      {
        parser()->context.xmlComment=TRUE;
        QCString cref;
        if (findAttribute(tagHtmlAttribs,"cref",&cref))
        {
          unescapeCRef(cref);
          // Look for an existing "see" section
          DocNodeVariant *vss=0;
          for (auto &n : children())
          {
            DocSimpleSect *candidate = std::get_if<DocSimpleSect>(&n);
            if (candidate && candidate->type()==DocSimpleSect::See)
            {
              vss = &n;
            }
          }

          if (!vss)  // start new section
          {
            children().append<DocSimpleSect>(parser(),thisVariant(),DocSimpleSect::See);
            vss = &children().back();
          }

          std::get<DocSimpleSect>(*vss).appendLinkWord(cref);
          retval = RetVal_OK;
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing 'cref' attribute from <seealso> tag.");
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
          children().append<DocHtmlTable>(parser(),thisVariant(),emptyList);
          retval=children().get_last<DocHtmlTable>()->parseXml();
        }
        else
        {
          children().append<DocHtmlList>(parser(),thisVariant(),emptyList,listType);
          retval=children().get_last<DocHtmlList>()->parseXml();
        }
      }
      break;
    case XML_INCLUDE:
    case XML_PERMISSION:
      // These tags are defined in .Net but are currently unsupported
      parser()->context.xmlComment=TRUE;
      break;
    case HTML_UNKNOWN:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported xml/html tag <%s> found", qPrint(tagName));
      children().append<DocWord>(parser(),thisVariant(), "<"+tagName+parser()->context.token->attribsStr+">");
      break;
  case XML_INHERITDOC:
      handleInheritDoc();
      break;
  default:
      // we should not get here!
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected start tag %s\n",qPrint(tagName));
      ASSERT(0);
      break;
  }
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

int DocPara::handleHtmlEndTag(const QCString &tagName)
{
  AUTO_TRACE("tagName={}",tagName);
  int tagId = Mappers::htmlTagMapper->map(tagName);
  int retval=RetVal_OK;
  switch (tagId)
  {
    case HTML_UL:
      if (!insideUL(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"found </ul> tag without matching <ul>");
      }
      else
      {
        retval=RetVal_EndList;
      }
      break;
    case HTML_OL:
      if (!insideOL(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"found </ol> tag without matching <ol>");
      }
      else
      {
        retval=RetVal_EndList;
      }
      break;
    case HTML_LI:
      if (!insideLI(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"found </li> tag without matching <li>");
      }
      else
      {
        // ignore </li> tags
      }
      break;
    case HTML_DETAILS:
      retval=RetVal_EndHtmlDetails;
      break;
    case HTML_BLOCKQUOTE:
      retval=RetVal_EndBlockQuote;
      break;
    case HTML_BOLD:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Bold,tagName);
      break;
    case HTML_S:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::S,"s");
      break;
    case HTML_STRIKE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Strike,tagName);
      break;
    case HTML_DEL:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Del,tagName);
      break;
    case HTML_UNDERLINE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Underline,tagName);
      break;
    case HTML_INS:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Ins,tagName);
      break;
    case HTML_CODE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Code,tagName);
      break;
    case HTML_EMPHASIS:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Italic,tagName);
      break;
    case HTML_DIV:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Div,tagName);
      break;
    case HTML_SPAN:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Span,tagName);
      break;
    case HTML_SUB:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Subscript,tagName);
      break;
    case HTML_SUP:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Superscript,tagName);
      break;
    case HTML_CENTER:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Center,tagName);
      break;
    case HTML_SMALL:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Small,tagName);
      break;
    case HTML_CITE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Cite,tagName);
      break;
    case HTML_PRE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Preformatted,tagName);
      setInsidePreformatted(FALSE);
      parser()->tokenizer.setInsidePre(FALSE);
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
    case HTML_THEAD:
    case HTML_TBODY:
    case HTML_TFOOT:
      // for time being ignore </t....> tag
      break;
    case HTML_CAPTION:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </caption> found");
      break;
    case HTML_BR:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal </br> tag found\n");
      break;
    case HTML_H1:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h1> found");
      break;
    case HTML_H2:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h2> found");
      break;
    case HTML_H3:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h3> found");
      break;
    case HTML_H4:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h4> found");
      break;
    case HTML_H5:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h5> found");
      break;
    case HTML_H6:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h6> found");
      break;
    case HTML_IMG:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </img> found");
      break;
    case HTML_HR:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal </hr> tag found\n");
      break;
    case HTML_A:
      //warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </a> found");
      // ignore </a> tag (can be part of <a name=...></a>
      break;

    case XML_TERM:
      break;
    case XML_SUMMARY:
      retval=TK_NEWPARA;
      break;
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
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Code,tagName);
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
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported xml/html tag </%s> found", qPrint(tagName));
      children().append<DocWord>(parser(),thisVariant(),"</"+tagName+">");
      break;
    default:
      // we should not get here!
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end tag %s\n",qPrint(tagName));
      ASSERT(0);
      break;
  }
  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

int DocPara::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  // handle style commands "inherited" from the previous paragraph
  parser()->handleInitialStyleCommands(thisVariant(),children());
  int tok;
  int retval=0;
  while ((tok=parser()->tokenizer.lex())) // get the next token
  {
reparsetoken:
    AUTO_TRACE_ADD("token '{}' at {}",DocTokenizer::tokToString(tok),parser()->tokenizer.getLineNr());
    if (tok==TK_WORD || tok==TK_LNKWORD || tok==TK_SYMBOL || tok==TK_URL ||
        tok==TK_COMMAND_AT || tok == TK_COMMAND_BS || tok==TK_HTMLTAG
       )
    {
      AUTO_TRACE_ADD("name={}",parser()->context.token->name);
    }
    switch(tok)
    {
      case TK_WORD:
        children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
        break;
      case TK_LNKWORD:
        parser()->handleLinkedWord(thisVariant(),children());
        break;
      case TK_URL:
        children().append<DocURL>(parser(),thisVariant(),parser()->context.token->name,parser()->context.token->isEMailAddr);
        break;
      case TK_WHITESPACE:
        {
          // prevent leading whitespace and collapse multiple whitespace areas
          if (insidePRE(thisVariant()) || // all whitespace is relevant
              (
               // remove leading whitespace
               !children().empty()  &&
               // and whitespace after certain constructs
               !holds_one_of_alternatives<DocHtmlDescList, DocHtmlTable,     DocHtmlList,   DocSimpleSect,
                                          DocAutoList,     DocSimpleList,    DocHtmlHeader, DocHtmlBlockQuote,
                                          DocParamSect,    DocHtmlDetails,   DocXRefItem>(children().back())
              )
             )
          {
            children().append<DocWhiteSpace>(parser(),thisVariant(),parser()->context.token->chars);
          }
        }
        break;
      case TK_LISTITEM:
        {
          AUTO_TRACE_ADD("found list item at {}",parser()->context.token->indent);
          const DocNodeVariant *n=parent();
          while (n && !std::holds_alternative<DocAutoList>(*n)) n=::parent(n);
          const DocAutoList *al = std::get_if<DocAutoList>(n);
          if (al) // we found an auto list up in the hierarchy
          {
            AUTO_TRACE_ADD("previous list item at {}",al->indent());
            if (al->indent()>=parser()->context.token->indent)
              // new item at the same or lower indent level
            {
              retval=TK_LISTITEM;
              goto endparagraph;
            }
          }

          // determine list depth
          int depth = 0;
          n=parent();
          while (n)
          {
            al = std::get_if<DocAutoList>(n);
            if (al && al->isEnumList()) depth++;
            n=::parent(n);
          }

          // first item or sub list => create new list
          do
          {
            children().append<DocAutoList>(parser(),thisVariant(),
                                           parser()->context.token->indent,
                                           parser()->context.token->isEnumList,depth);
            al = children().get_last<DocAutoList>();
            retval = children().get_last<DocAutoList>()->parse();
          } while (retval==TK_LISTITEM &&                   // new list
              al->indent()==parser()->context.token->indent  // at same indent level
              );

          // check the return value
          if (retval==RetVal_SimpleSec) // auto list ended due to simple section command
          {
            // Reparse the token that ended the section at this level,
            // so a new simple section will be started at this level.
            // This is the same as unputting the last read token and continuing.
            parser()->context.token->name = parser()->context.token->simpleSectName;
            if (parser()->context.token->name.startsWith("rcs:")) // RCS section
            {
              parser()->context.token->name = parser()->context.token->name.mid(4);
              parser()->context.token->text = parser()->context.token->simpleSectText;
              tok = TK_RCSTAG;
            }
            else // other section
            {
              tok = TK_COMMAND_BS;
            }
            AUTO_TRACE_ADD("reparsing command {}",parser()->context.token->name);
            goto reparsetoken;
          }
          else if (retval==TK_ENDLIST)
          {
            if (al->indent()>parser()->context.token->indent) // end list
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
        AUTO_TRACE_ADD("Found end of list inside of paragraph at line {}",parser()->tokenizer.getLineNr());
        if (std::get_if<DocAutoListItem>(parent()))
        {
          const DocAutoList *al = std::get_if<DocAutoList>(::parent(parent()));
          if (al && al->indent()>=parser()->context.token->indent)
          {
            // end of list marker ends this paragraph
            retval=TK_ENDLIST;
            goto endparagraph;
          }
          else
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"End of list marker found "
                "has invalid indent level");
          }
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"End of list marker found without any preceding "
              "list items");
        }
        break;
      case TK_COMMAND_AT:
        // fall through
      case TK_COMMAND_BS:
        {
          // see if we have to start a simple section
          int cmd = Mappers::cmdMapper->map(parser()->context.token->name);
          const DocNodeVariant *n=parent();
          while (n && !std::holds_alternative<DocSimpleSect>(*n) &&
                      !std::holds_alternative<DocParamSect>(*n))
          {
            n=::parent(n);
          }
          if (cmd&SIMPLESECT_BIT)
          {
            if (n)  // already in a simple section
            {
              // simple section cannot start in this paragraph, need
              // to unwind the stack and remember the command.
              parser()->context.token->simpleSectName = parser()->context.token->name;
              retval=RetVal_SimpleSec;
              goto endparagraph;
            }
          }
          // see if we are in a simple list
          n=parent();
          while (n && !std::holds_alternative<DocSimpleListItem>(*n)) n=::parent(n);
          if (n)
          {
            if (cmd==CMD_LI)
            {
              retval=RetVal_ListItem;
              goto endparagraph;
            }
          }

          // handle the command
          retval=handleCommand(parser()->context.token->name,tok);
          AUTO_TRACE_ADD("handleCommand returns {}",DocTokenizer::retvalToString(retval));

          // check the return value
          if (retval==RetVal_SimpleSec)
          {
            // Reparse the token that ended the section at this level,
            // so a new simple section will be started at this level.
            // This is the same as unputting the last read token and continuing.
            parser()->context.token->name = parser()->context.token->simpleSectName;
            if (parser()->context.token->name.startsWith("rcs:")) // RCS section
            {
              parser()->context.token->name = parser()->context.token->name.mid(4);
              parser()->context.token->text = parser()->context.token->simpleSectText;
              tok = TK_RCSTAG;
            }
            else // other section
            {
              tok = TK_COMMAND_BS;
            }
            AUTO_TRACE_ADD("reparsing command {}",parser()->context.token->name);
            goto reparsetoken;
          }
          else if (retval>0 && retval<RetVal_OK)
          {
            // the command ended with a new command, reparse this token
            tok = retval;
            goto reparsetoken;
          }
          else if (retval != RetVal_OK) // end of file, end of paragraph, start or end of section
                                        // or some auto list marker
          {
            goto endparagraph;
          }
        }
        break;
      case TK_HTMLTAG:
        {
          if (!parser()->context.token->endTag) // found a start tag
          {
            retval = handleHtmlStartTag(parser()->context.token->name,parser()->context.token->attribs);
          }
          else // found an end tag
          {
            retval = handleHtmlEndTag(parser()->context.token->name);
          }
          if (retval!=RetVal_OK)
          {
            goto endparagraph;
          }
        }
        break;
      case TK_SYMBOL:
        {
          HtmlEntityMapper::SymType s = DocSymbol::decodeSymbol(parser()->context.token->name);
          if (s!=HtmlEntityMapper::Sym_Unknown)
          {
            children().append<DocSymbol>(parser(),thisVariant(),s);
          }
          else
          {
            children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported symbol %s found",
                qPrint(parser()->context.token->name));
          }
          break;
        }
      case TK_NEWPARA:
        retval=TK_NEWPARA;
        goto endparagraph;
      case TK_RCSTAG:
        {
          const DocNodeVariant *n=parent();
          while (n && !std::holds_alternative<DocSimpleSect>(*n) &&
                      !std::holds_alternative<DocParamSect>(*n))
          {
            n=::parent(n);
          }
          if (n)  // already in a simple section
          {
            // simple section cannot start in this paragraph, need
            // to unwind the stack and remember the command.
            parser()->context.token->simpleSectName = "rcs:"+parser()->context.token->name;
            parser()->context.token->simpleSectText = parser()->context.token->text;
            retval=RetVal_SimpleSec;
            goto endparagraph;
          }

          // see if we are in a simple list
          children().append<DocSimpleSect>(parser(),thisVariant(),DocSimpleSect::Rcs);
          children().get_last<DocSimpleSect>()->parseRcs();
        }
        break;
      default:
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
            "Found unexpected token (id=%s)\n",DocTokenizer::tokToString(tok));
        break;
    }
  }
  retval=0;
endparagraph:
  parser()->handlePendingStyleCommands(thisVariant(),children());
  DocPara *par = std::get_if<DocPara>(parser()->context.nodeStack.top());
  if (!parser()->context.token->endTag && par &&
      retval==TK_NEWPARA && parser()->context.token->name.lower() == "p")
  {
    par->setAttribs(parser()->context.token->attribs);
  }
  INTERNAL_ASSERT(retval==0 || retval==TK_NEWPARA || retval==TK_LISTITEM ||
         retval==TK_ENDLIST || retval>RetVal_OK
	);

  AUTO_TRACE_EXIT("retval={}",DocTokenizer::retvalToString(retval));
  return retval;
}

//--------------------------------------------------------------------------

int DocSection::parse()
{
  AUTO_TRACE("start {} level={}", parser()->context.token->sectionId, m_level);
  int retval=RetVal_OK;
  auto ns = AutoNodeStack(parser(),thisVariant());

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
    children().append<DocPara>(parser(),thisVariant());
    DocPara *par  = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (!par->isEmpty())
    {
      if (lastPar) lastPar->markLast(FALSE);
      lastPar = par;
    }
    else
    {
      children().pop_back();
    }
    if (retval==TK_LISTITEM)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid list item found");
    }
    if (retval==RetVal_Internal)
    {
      children().append<DocInternal>(parser(),thisVariant());
      retval = children().get_last<DocInternal>()->parse(m_level+1);
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
        children().append<DocSection>(parser(),thisVariant(),
                                std::min(2+Doxygen::subpageNestingLevel,5),
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
      }
      break;
    }
    else if (retval==RetVal_Subsubsection && m_level<=Doxygen::subpageNestingLevel+2)
    {
      if ((m_level <= 1 + Doxygen::subpageNestingLevel) &&
          AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
      {
        warn_doc_error(parser()->context.fileName,
                       parser()->tokenizer.getLineNr(),
                       "Unexpected subsubsection command found inside %s!",
                       g_sectionLevelToName[m_level]);
      }
      // then parse any number of nested sections
      while (retval==RetVal_Subsubsection) // more sections follow
      {
        children().append<DocSection>(parser(),thisVariant(),
                                std::min(3+Doxygen::subpageNestingLevel,5),
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
      }
      if (!(m_level < Doxygen::subpageNestingLevel + 2 && retval == RetVal_Subsection)) break;
    }
    else if (retval==RetVal_Paragraph && m_level<=std::min(5,Doxygen::subpageNestingLevel+3))
    {
      if ((m_level <= 2 + Doxygen::subpageNestingLevel) &&
          AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
                       "Unexpected paragraph command found inside %s!",
                       g_sectionLevelToName[m_level]);
      }
      // then parse any number of nested sections
      while (retval==RetVal_Paragraph) // more sections follow
      {
        children().append<DocSection>(parser(),thisVariant(),
                                std::min(4+Doxygen::subpageNestingLevel,5),
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
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

  AUTO_TRACE_EXIT("retval={}", DocTokenizer::retvalToString(retval));
  return retval;
}

//--------------------------------------------------------------------------

void DocText::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  parser()->tokenizer.setStateText();

  int tok;
  while ((tok=parser()->tokenizer.lex())) // get the next token
  {
    switch(tok)
    {
      case TK_WORD:
	children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
	break;
      case TK_WHITESPACE:
        children().append<DocWhiteSpace>(parser(),thisVariant(),parser()->context.token->chars);
	break;
      case TK_SYMBOL:
        {
          HtmlEntityMapper::SymType s = DocSymbol::decodeSymbol(parser()->context.token->name);
          if (s!=HtmlEntityMapper::Sym_Unknown)
          {
            children().append<DocSymbol>(parser(),thisVariant(),s);
          }
          else
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported symbol %s found",
                qPrint(parser()->context.token->name));
          }
        }
        break;
      case TK_COMMAND_AT:
        // fall through
      case TK_COMMAND_BS:
        switch (Mappers::cmdMapper->map(parser()->context.token->name))
        {
          case CMD_BSLASH:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_BSlash);
            break;
          case CMD_AT:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_At);
            break;
          case CMD_LESS:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Less);
            break;
          case CMD_GREATER:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Greater);
            break;
          case CMD_AMP:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Amp);
            break;
          case CMD_DOLLAR:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Dollar);
            break;
          case CMD_HASH:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Hash);
            break;
          case CMD_DCOLON:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_DoubleColon);
            break;
          case CMD_PERCENT:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Percent);
            break;
          case CMD_NDASH:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            break;
          case CMD_MDASH:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            break;
          case CMD_QUOTE:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Quot);
            break;
          case CMD_PUNT:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Dot);
            break;
          case CMD_PLUS:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Plus);
            break;
          case CMD_MINUS:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            break;
          case CMD_EQUAL:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Equal);
            break;
          default:
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected command '%s' found",
                      qPrint(parser()->context.token->name));
            break;
        }
        break;
      default:
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected token %s",
            DocTokenizer::tokToString(tok));
        break;
    }
  }

  parser()->handleUnclosedStyleCommands();

}


//--------------------------------------------------------------------------

void DocRoot::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  parser()->tokenizer.setStatePara();
  int retval=0;

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar = nullptr;
  do
  {
    {
      children().append<DocPara>(parser(),thisVariant());
      DocPara *par  = children().get_last<DocPara>();
      if (isFirst) { par->markFirst(); isFirst=FALSE; }
      retval=par->parse();
      if (par->isEmpty() && par->attribs().empty())
      {
        children().pop_back();
      }
      else
      {
        lastPar = par;
      }
    }
    if (retval == RetVal_Paragraph)
    {
      if (!AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
      {
        warn_doc_error(parser()->context.fileName,
                       parser()->tokenizer.getLineNr(),
                       "found paragraph command (id: '%s') outside of subsubsection context!",
                       qPrint(parser()->context.token->sectionId));
      }
      while (retval==RetVal_Paragraph)
      {
        if (!parser()->context.token->sectionId.isEmpty())
        {
          const SectionInfo *sec=SectionManager::instance().find(parser()->context.token->sectionId);
          if (sec)
          {
            children().append<DocSection>(parser(),thisVariant(),
                                    std::min(4+Doxygen::subpageNestingLevel,5),
                                    parser()->context.token->sectionId);
            retval = children().get_last<DocSection>()->parse();
          }
          else
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid paragraph id '%s'; ignoring paragraph",qPrint(parser()->context.token->sectionId));
            retval = 0;
          }
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing id for paragraph; ignoring paragraph");
          retval = 0;
        }
      }
    }
    if (retval==RetVal_Subsubsection)
    {
      if (!AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
      {
        warn_doc_error(parser()->context.fileName,
                       parser()->tokenizer.getLineNr(),
                       "found subsubsection command (id: '%s') outside of subsection context!",
                       qPrint(parser()->context.token->sectionId));
      }
      while (retval==RetVal_Subsubsection)
      {
        if (!parser()->context.token->sectionId.isEmpty())
        {
          const SectionInfo *sec=SectionManager::instance().find(parser()->context.token->sectionId);
          if (sec)
          {
            children().append<DocSection>(parser(),thisVariant(),
                                    std::min(3+Doxygen::subpageNestingLevel,5),
                                    parser()->context.token->sectionId);
            retval = children().get_last<DocSection>()->parse();
          }
          else
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid subsubsection id '%s'; ignoring subsubsection",qPrint(parser()->context.token->sectionId));
            retval = 0;
          }
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing id for subsubsection; ignoring subsubsection");
          retval = 0;
        }
      }
    }
    if (retval==RetVal_Subsection)
    {
      if (!AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
      {
        warn_doc_error(parser()->context.fileName,
                       parser()->tokenizer.getLineNr(),
                       "found subsection command (id: '%s') outside of section context!",
                       qPrint(parser()->context.token->sectionId));
      }
      while (retval==RetVal_Subsection)
      {
        if (!parser()->context.token->sectionId.isEmpty())
        {
          const SectionInfo *sec=SectionManager::instance().find(parser()->context.token->sectionId);
          if (sec)
          {
            children().append<DocSection>(parser(),thisVariant(),
                                    std::min(2+Doxygen::subpageNestingLevel,5),
                                    parser()->context.token->sectionId);
            retval = children().get_last<DocSection>()->parse();
          }
          else
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid subsection id '%s'; ignoring subsection",qPrint(parser()->context.token->sectionId));
            retval = 0;
          }
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing id for subsection; ignoring subsection");
          retval = 0;
        }
      }
    }
    if (retval==TK_LISTITEM)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid list item found");
    }
    if (retval==RetVal_Internal)
    {
      children().append<DocInternal>(parser(),thisVariant());
      retval = children().get_last<DocInternal>()->parse(1);
    }
  } while (retval!=0 && retval!=RetVal_Section);
  if (lastPar) lastPar->markLast();

  //printf("DocRoot::parse() retval=%d %d\n",retval,RetVal_Section);
  // then parse any number of level1 sections
  while (retval==RetVal_Section)
  {
    if (!parser()->context.token->sectionId.isEmpty())
    {
      const SectionInfo *sec=SectionManager::instance().find(parser()->context.token->sectionId);
      if (sec)
      {
        children().append<DocSection>(parser(),thisVariant(),
                                std::min(1+Doxygen::subpageNestingLevel,5),
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
      }
      else
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid section id '%s'; ignoring section",qPrint(parser()->context.token->sectionId));
        retval = 0;
      }
    }
    else
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing id for section; ignoring section");
      retval = 0;
    }
  }

  parser()->handleUnclosedStyleCommands();
}
