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
#include "aliases.h"

#if !ENABLE_DOCPARSER_TRACING
#undef  AUTO_TRACE
#undef  AUTO_TRACE_ADD
#undef  AUTO_TRACE_EXIT
#define AUTO_TRACE(...)      (void)0
#define AUTO_TRACE_ADD(...)  (void)0
#define AUTO_TRACE_EXIT(...) (void)0
#endif

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

static const StringUnorderedSet g_plantumlEngine {
  "uml", "bpm", "wire", "dot", "ditaa",
  "salt", "math", "latex", "gantt", "mindmap",
  "wbs", "yaml", "creole", "json", "flow",
  "board", "git", "hcl", "regex", "ebnf",
  "files", "chen", "chronology"
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
    char c = 0;
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
  size_t len=locSymName.length();
  if (len>0)
  {
    if (locSymName.at(len-1)!=':') locSymName.append(":");
    if (locSymName.at(0)!=':')     locSymName.prepend(":");
  }
  m_symName = locSymName;
  m_index = EmojiEntityMapper::instance().symbol2index(m_symName.str());
  if (m_index==-1)
  {
    warn_doc_error(parser->context.fileName,parser->tokenizer.getLineNr(),"Found unsupported emoji symbol '%s'",qPrint(m_symName));
  }
}

//---------------------------------------------------------------------------

DocWord::DocWord(DocParser *parser,DocNodeVariant *parent,const QCString &word) :
      DocNode(parser,parent), m_word(word)
{
  //printf("new word %s url=%s\n",qPrint(word),qPrint(parser->context.searchUrl));
  if (Doxygen::searchIndex.enabled() && !parser->context.searchUrl.isEmpty())
  {
    Doxygen::searchIndex.addWord(word,false);
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
  if (Doxygen::searchIndex.enabled() && !parser->context.searchUrl.isEmpty())
  {
    Doxygen::searchIndex.addWord(word,false);
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
      parser()->context.stripCodeComments = m_stripCodeComments;
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
    case SnippetWithLines:
      parser()->readTextFileByName(m_file,m_text);
      // check here for the existence of the blockId inside the file, so we
      // only generate the warning once.
      int count = 0;
      if (!m_blockId.isEmpty() && (count=m_text.contains(m_blockId.data()))!=2)
      {
        warn_doc_error(parser()->context.fileName,
                       parser()->tokenizer.getLineNr(),
                       "block marked with %s for \\snippet should appear twice in file %s, found it %d times",
                       qPrint(m_blockId),qPrint(m_file),count);
      }
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
  bool found = false;

  m_includeFileName = parser()->context.includeFileName;
  const char *p = parser()->context.includeFileText.data();
  size_t l = parser()->context.includeFileLength;
  size_t o = parser()->context.includeFileOffset;
  int il = parser()->context.includeFileLine;
  AUTO_TRACE("text={} off={} len={}",Trace::trunc(p),o,l);
  size_t so = o, bo = 0;
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
        found = true;
        AUTO_TRACE_ADD("\\line {}",Trace::trunc(m_text));
      }
      parser()->context.includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = parser()->context.includeFileShowLineNo;
      m_stripCodeComments = parser()->context.stripCodeComments;
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
          found = true;
          AUTO_TRACE_ADD("\\skipline {}",Trace::trunc(m_text));
          break;
        }
        o++; // skip new line
      }
      parser()->context.includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = parser()->context.includeFileShowLineNo;
      m_stripCodeComments = parser()->context.stripCodeComments;
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
        found = true;
          break;
        }
        o++; // skip new line
      }
      parser()->context.includeFileOffset = so; // set pointer to start of new line
      m_showLineNo = parser()->context.includeFileShowLineNo;
      m_stripCodeComments = parser()->context.stripCodeComments;
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
          found = true;
          AUTO_TRACE_ADD("\\until {}",Trace::trunc(m_text));
          break;
        }
        o++; // skip new line
      }
      parser()->context.includeFileOffset = std::min(l,o+1); // set pointer to start of new line
      m_showLineNo = parser()->context.includeFileShowLineNo;
      m_stripCodeComments = parser()->context.stripCodeComments;
      break;
  }
  if (!found)
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
      "referenced pattern '%s' for command '\\%s' not found",qPrint(m_pattern),typeAsString());
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
    ASSERT(item!=nullptr);
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
  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"\\refitem");
    }
    tok = parser()->tokenizer.lex();
  }
  parser()->tokenizer.setStatePara();
  parser()->handlePendingStyleCommands(thisVariant(),children());

  if (!m_target.isEmpty())
  {
    const SectionInfo *sec = SectionManager::instance().find(m_target);
    if (sec==nullptr && parser()->context.lang==SrcLangExt::Markdown) // lookup as markdown file
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
      switch (sec->type().level())
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

  Token tok=parser()->tokenizer.lex();
  // skip white space
  while (tok.is_any_of(TokenRetval::TK_WHITESPACE, TokenRetval::TK_NEWPARA)) tok=parser()->tokenizer.lex();
  // handle items
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (tok.is_any_of(TokenRetval::TK_COMMAND_AT, TokenRetval::TK_COMMAND_BS))
    {
      switch (Mappers::cmdMapper->map(parser()->context.token->name))
      {
        case CommandType::CMD_SECREFITEM:
          {
            tok=parser()->tokenizer.lex();
            if (!tok.is(TokenRetval::TK_WHITESPACE))
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\refitem command");
              break;
            }
            tok=parser()->tokenizer.lex();
            if (!tok.is_any_of(TokenRetval::TK_WORD,TokenRetval::TK_LNKWORD))
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of \\refitem",
                  tok.to_string());
              break;
            }

            children().append<DocSecRefItem>(parser(),thisVariant(),parser()->context.token->name);
            children().get_last<DocSecRefItem>()->parse();
          }
          break;
        case CommandType::CMD_ENDSECREFLIST:
          return;
        default:
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal command '%c%s' as part of a \\secreflist",
              tok.command_to_char(),qPrint(parser()->context.token->name));
          return;
      }
    }
    else if (tok.is(TokenRetval::TK_WHITESPACE))
    {
      // ignore whitespace
    }
    else
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected token %s inside section reference list",
          tok.to_string());
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

  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"\\ref");
    }
    tok=parser()->tokenizer.lex();
  }

  parser()->handlePendingStyleCommands(thisVariant(),children());
}

//---------------------------------------------------------------------------

DocRef::DocRef(DocParser *parser,DocNodeVariant *parent,const QCString &target,const QCString &context) :
   DocCompoundNode(parser,parent), m_refType(Unknown), m_isSubPage(FALSE)
{
  const Definition  *compound = nullptr;
  QCString     anchor;
  //printf("DocRef::DocRef(target=%s,context=%s)\n",qPrint(target),qPrint(context));
  ASSERT(!target.isEmpty());
  m_relPath = parser->context.relPath;
  const SectionInfo *sec = SectionManager::instance().find(parser->context.prefix+target);
  if (sec==nullptr && getLanguageFromFileName(target)==SrcLangExt::Markdown) // lookup as markdown file
  {
    sec = SectionManager::instance().find(markdownFileNameToId(target));
  }
  if (sec) // ref to section or anchor
  {
    PageDef *pd = nullptr;
    int secLevel = sec->type().level();
    if (secLevel==SectionType::Page)
    {
      pd = Doxygen::pageLinkedMap->find(target);
    }
    m_text         = sec->title();
    if (m_text.isEmpty()) m_text = sec->label();

    m_ref          = sec->ref();
    m_file         = stripKnownExtensions(sec->fileName());
    if (secLevel==SectionType::Anchor)
    {
      m_refType = Anchor;
    }
    else if (secLevel==SectionType::Table)
    {
      m_refType = Table;
    }
    else
    {
      m_refType = Section;
    }
    m_isSubPage    = pd && pd->hasParentPage();
    if (secLevel!=SectionType::Page || m_isSubPage) m_anchor = sec->label();
    m_sectionType = sec->type();
    //printf("m_text=%s,m_ref=%s,m_file=%s,type=%d\n",
    //    qPrint(m_text),qPrint(m_ref),qPrint(m_file),m_refType);
    return;
  }
  else if (resolveLink(context,target,true,&compound,anchor,parser->context.prefix))
  {
    bool isFile = compound ?
                 (compound->definitionType()==Definition::TypeFile ||
                  compound->definitionType()==Definition::TypePage ? TRUE : FALSE) :
                 FALSE;
    m_text = linkToText(parser->context.lang,target,isFile);
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
      else if (Config_getBool(HIDE_SCOPE_NAMES))
      {
        m_text=stripScope(m_text);
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

void DocNodeList::move_append(DocNodeList &elements)
{
  for (auto &&elem : elements)
  {
    emplace_back(std::move(elem));
  }
  elements.clear();
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
    // we also need to set the parent for each child of cn, as cn's address may have changed.
    auto opt_children = call_method_children(&cn);
    if (opt_children)
    {
      for (auto &ccn : *opt_children)
      {
        setParent(&ccn,&cn);
      }
    }
  }
}

void DocRef::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      switch (tok.value())
      {
        case TokenRetval::TK_HTMLTAG:
          break;
        default:
          parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"\\ref");
          break;
      }
    }
    tok=parser()->tokenizer.lex();
  }

  if (children().empty() && !m_text.isEmpty())
  {
    QCString text = m_text;
    if (parser()->context.insideHtmlLink)
    {
      // we already in a link/title only output anchor
      text = m_anchor;
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
          "Potential recursion while resolving \\ref command!");
    }
    parser()->context.insideHtmlLink=TRUE;
    parser()->pushContext();
    parser()->internalValidatingParseDoc(thisVariant(),children(),text);
    parser()->popContext();
    parser()->context.insideHtmlLink=FALSE;
    parser()->tokenizer.setStatePara();
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
  else if (cite==nullptr)
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
  const Definition *compound = nullptr;
  QCString anchor;
  m_refText = target;
  m_relPath = parser->context.relPath;
  if (!m_refText.isEmpty() && m_refText.at(0)=='#')
  {
    m_refText = m_refText.right(m_refText.length()-1);
  }
  if (resolveLink(parser->context.context,stripKnownExtensions(target),parser->context.inSeeBlock,&compound,anchor,parser->context.prefix))
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

  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children(),FALSE))
    {
      switch (tok.value())
      {
        case TokenRetval::TK_COMMAND_AT:
        // fall through
        case TokenRetval::TK_COMMAND_BS:
          switch (Mappers::cmdMapper->map(parser()->context.token->name))
          {
            case CommandType::CMD_ENDLINK:
              if (isJavaLink)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"'{@link...' ended with '%c%s' command",
                  tok.command_to_char(),qPrint(parser()->context.token->name));
              }
              goto endlink;
            default:
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal command '%c%s' as part of a \\link",
                  tok.command_to_char(),qPrint(parser()->context.token->name));
              break;
          }
          break;
        case TokenRetval::TK_SYMBOL:
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported symbol '%s' found as part of a \\link",
              qPrint(parser()->context.token->name));
          break;
        case TokenRetval::TK_HTMLTAG:
          if (parser()->context.token->name!="see" || !isXmlLink)
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected xml/html command %s found as part of a \\link",
                qPrint(parser()->context.token->name));
          }
          goto endlink;
        case TokenRetval::TK_LNKWORD:
        case TokenRetval::TK_WORD:
          if (isJavaLink) // special case to detect closing }
          {
            QCString w = parser()->context.token->name;
            int p = 0;
            if (w=="}")
            {
              goto endlink;
            }
            else if ((p=w.find('}'))!=-1)
            {
              int l = static_cast<int>(w.length());
              children().append<DocWord>(parser(),thisVariant(),w.left(p));
              if (p<l-1) // something left after the } (for instance a .)
              {
                result=w.right(l-p-1);
              }
              goto endlink;
            }
          }
          children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
          break;
        default:
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected token %s",tok.to_string());
        break;
      }
    }
    tok = parser()->tokenizer.lex();
  }
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,
                   parser()->tokenizer.getLineNr(),
                   "Unexpected end of comment while inside link command");
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
  parser()->defaultHandleTitleAndSize(CommandType::CMD_DOTFILE,thisVariant(),children(),p->width,p->height);

  bool ambig = false;
  FileDef *fd = findFileDef(Doxygen::dotFileNameLinkedMap,p->name,ambig);
  if (fd==nullptr && !p->name.endsWith(".dot")) // try with .dot extension as well
  {
    fd = findFileDef(Doxygen::dotFileNameLinkedMap,p->name+".dot",ambig);
  }
  if (fd)
  {
    p->file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included dot file name '%s' is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(p->name),
           qPrint(showFileDefMatches(Doxygen::dotFileNameLinkedMap,p->name))
          );
    }
  }
  else
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included dot file '%s' is not found "
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
  parser()->defaultHandleTitleAndSize(CommandType::CMD_MSCFILE,thisVariant(),children(),p->width,p->height);

  bool ambig = false;
  FileDef *fd = findFileDef(Doxygen::mscFileNameLinkedMap,p->name,ambig);
  if (fd==nullptr && !p->name.endsWith(".msc")) // try with .msc extension as well
  {
    fd = findFileDef(Doxygen::mscFileNameLinkedMap,p->name+".msc",ambig);
  }
  if (fd)
  {
    p->file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included msc file name '%s' is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(p->name),
           qPrint(showFileDefMatches(Doxygen::mscFileNameLinkedMap,p->name))
          );
    }
  }
  else
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included msc file '%s' is not found "
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
  parser()->defaultHandleTitleAndSize(CommandType::CMD_DIAFILE,thisVariant(),children(),p->width,p->height);

  bool ambig = false;
  FileDef *fd = findFileDef(Doxygen::diaFileNameLinkedMap,p->name,ambig);
  if (fd==nullptr && !p->name.endsWith(".dia")) // try with .dia extension as well
  {
    fd = findFileDef(Doxygen::diaFileNameLinkedMap,p->name+".dia",ambig);
  }
  if (fd)
  {
    p->file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included dia file name '%s' is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(p->name),
           qPrint(showFileDefMatches(Doxygen::diaFileNameLinkedMap,p->name))
          );
    }
  }
  else
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included dia file '%s' is not found "
           "in any of the paths specified via DIAFILE_DIRS!",qPrint(p->name));
  }
  return ok;
}
//---------------------------------------------------------------------------

DocPlantUmlFile::DocPlantUmlFile(DocParser *parser,DocNodeVariant *parent,const QCString &name,const QCString &context,
                       const QCString &srcFile,int srcLine) :
  DocDiagramFileBase(parser,parent,name,context,srcFile,srcLine)
{
  p->relPath = parser->context.relPath;
}

bool DocPlantUmlFile::parse()
{
  bool ok = false;
  parser()->defaultHandleTitleAndSize(CommandType::CMD_PLANTUMLFILE,thisVariant(),children(),p->width,p->height);

  bool ambig = false;
  FileDef *fd = findFileDef(Doxygen::plantUmlFileNameLinkedMap,p->name,ambig);
  if (fd==nullptr && !p->name.endsWith(".puml")) // try with .puml extension as well
  {
    fd = findFileDef(Doxygen::plantUmlFileNameLinkedMap,p->name+".puml",ambig);
    if (fd==nullptr && !p->name.endsWith(".pu")) // try with .pu extension as well
    {
      fd = findFileDef(Doxygen::plantUmlFileNameLinkedMap,p->name+".pu",ambig);
    }
  }
  if (fd)
  {
    p->file = fd->absFilePath();
    ok = true;
    if (ambig)
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included uml file name '%s' is ambiguous.\n"
           "Possible candidates:\n%s",qPrint(p->name),
           qPrint(showFileDefMatches(Doxygen::plantUmlFileNameLinkedMap,p->name))
          );
    }
  }
  else
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"included uml file '%s' is not found "
           "in any of the paths specified via PLANTUMLFILE_DIRS!",qPrint(p->name));
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
  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"\\vhdlflow");
    }
    tok = parser()->tokenizer.lex();
  }
  parser()->tokenizer.lex();

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
  parser()->defaultHandleTitleAndSize(CommandType::CMD_IMAGE,thisVariant(),children(),p->width,p->height);
}


//---------------------------------------------------------------------------

Token DocHtmlHeader::parse()
{
  AUTO_TRACE();
  Token retval(TokenRetval::RetVal_OK);
  auto ns = AutoNodeStack(parser(),thisVariant());

  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      switch (tok.value())
      {
        case TokenRetval::TK_HTMLTAG:
          {
            HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
            if (tagId==HtmlTagType::HTML_H1 && parser()->context.token->endTag) // found </h1> tag
            {
              if (m_level!=1)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h1>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HtmlTagType::HTML_H2 && parser()->context.token->endTag) // found </h2> tag
            {
              if (m_level!=2)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h2>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HtmlTagType::HTML_H3 && parser()->context.token->endTag) // found </h3> tag
            {
              if (m_level!=3)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h3>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HtmlTagType::HTML_H4 && parser()->context.token->endTag) // found </h4> tag
            {
              if (m_level!=4)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h4>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HtmlTagType::HTML_H5 && parser()->context.token->endTag) // found </h5> tag
            {
              if (m_level!=5)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h5>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HtmlTagType::HTML_H6 && parser()->context.token->endTag) // found </h6> tag
            {
              if (m_level!=6)
              {
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"<h%d> ended with </h6>",
                    m_level);
              }
              goto endheader;
            }
            else if (tagId==HtmlTagType::HTML_A)
            {
              if (!parser()->context.token->endTag)
              {
                parser()->handleAHref(thisVariant(),children(),parser()->context.token->attribs);
              }
            }
            else if (tagId==HtmlTagType::HTML_BR)
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
    tok = parser()->tokenizer.lex();
  }
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <h%d> tag",m_level);
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
  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    HtmlTagType tagId = HtmlTagType::UNKNOWN;
    // check of </summary>
    if (tok.value()==TokenRetval::TK_HTMLTAG &&
        (tagId=Mappers::htmlTagMapper->map(parser()->context.token->name))==HtmlTagType::XML_SUMMARY &&
        parser()->context.token->endTag
       )
    {
      break;
    }
    else if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"summary section");
    }
    tok = parser()->tokenizer.lex();
  }
  parser()->tokenizer.setStatePara();
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <summary> tag");
  }
}

//---------------------------------------------------------------------------

Token DocHtmlDetails::parse()
{
  AUTO_TRACE();
  Token retval(TokenRetval::TK_NONE);
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=nullptr;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval.is(TokenRetval::TK_NEWPARA));
  if (par) par->markLast();

  if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while inside <details> block");
  }

  if (!summary())
  {
    HtmlAttribList summaryAttribs;
    m_summary = createDocNode<DocHtmlSummary>(parser(),thisVariant(),summaryAttribs);
    DocHtmlSummary *summary = &std::get<DocHtmlSummary>(*m_summary);
    summary->children().append<DocWord>(parser(),thisVariant(),theTranslator->trDetails());
  }
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval.is(TokenRetval::RetVal_EndHtmlDetails) ? Token::make_RetVal_OK() : retval;
}

void DocHtmlDetails::parseSummary(DocNodeVariant *parent,HtmlAttribList &attribs)
{
  AUTO_TRACE();
  m_summary = createDocNode<DocHtmlSummary>(parser(),parent,attribs);
  DocHtmlSummary *summary = &std::get<DocHtmlSummary>(*m_summary);
  summary->parse();
}

//---------------------------------------------------------------------------

Token DocHRef::parse()
{
  AUTO_TRACE();
  Token retval(TokenRetval::RetVal_OK);
  auto ns = AutoNodeStack(parser(),thisVariant());

  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      switch (tok.value())
      {
        case TokenRetval::TK_HTMLTAG:
          {
            HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
            if (tagId==HtmlTagType::HTML_A && parser()->context.token->endTag) // found </a> tag
            {
              goto endhref;
            }
            else if (tagId==HtmlTagType::HTML_BR)
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
    tok = parser()->tokenizer.lex();
  }
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <a href=...> tag");
  }
endhref:
  parser()->handlePendingStyleCommands(thisVariant(),children());
  return retval;
}

//---------------------------------------------------------------------------

Token DocInternal::parse(int level)
{
  AUTO_TRACE();
  Token retval(TokenRetval::RetVal_OK);
  auto ns = AutoNodeStack(parser(),thisVariant());

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=nullptr;
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
    if (retval.is(TokenRetval::TK_LISTITEM))
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid list item found");
    }
  } while (!retval.is_any_of(TokenRetval::TK_NONE,          TokenRetval::TK_EOF,
                             TokenRetval::RetVal_Section,   TokenRetval::RetVal_Subsection,   TokenRetval::RetVal_Subsubsection,
                             TokenRetval::RetVal_Paragraph, TokenRetval::RetVal_SubParagraph, TokenRetval::RetVal_SubSubParagraph,
                             TokenRetval::RetVal_EndInternal));
  if (lastPar) lastPar->markLast();

  // then parse any number of level-n sections
  while ((level==1 && retval.is(TokenRetval::RetVal_Section)) ||
         (level==2 && retval.is(TokenRetval::RetVal_Subsection)) ||
         (level==3 && retval.is(TokenRetval::RetVal_Subsubsection)) ||
         (level==4 && retval.is(TokenRetval::RetVal_Paragraph)) ||
         (level==5 && retval.is(TokenRetval::RetVal_SubParagraph)) ||
         (level==6 && retval.is(TokenRetval::RetVal_SubSubParagraph))
        )
  {
    children().append<DocSection>(parser(),thisVariant(),
                                  level,
                                  parser()->context.token->sectionId);
    retval = children().get_last<DocSection>()->parse();
  }

  if (retval.is(TokenRetval::RetVal_Internal))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"\\internal command found inside internal section");
  }

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

//---------------------------------------------------------------------------

Token DocIndexEntry::parse()
{
  AUTO_TRACE();
  Token retval(TokenRetval::RetVal_OK);
  auto ns = AutoNodeStack(parser(),thisVariant());
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\addindex command");
    goto endindexentry;
  }
  parser()->tokenizer.setStateTitle();
  m_entry="";
  tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    switch (tok.value())
    {
      case TokenRetval::TK_WHITESPACE:
        m_entry+=" ";
        break;
      case TokenRetval::TK_WORD:
      case TokenRetval::TK_LNKWORD:
        m_entry+=parser()->context.token->name;
        break;
      case TokenRetval::TK_SYMBOL:
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
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected symbol '%s' found as argument of \\addindex",qPrint(parser()->context.token->name));
              break;
          }
        }
        break;
      case TokenRetval::TK_COMMAND_AT:
        // fall through
      case TokenRetval::TK_COMMAND_BS:
        switch (Mappers::cmdMapper->map(parser()->context.token->name))
        {
          case CommandType::CMD_BSLASH:  m_entry+='\\';  break;
          case CommandType::CMD_AT:      m_entry+='@';   break;
          case CommandType::CMD_LESS:    m_entry+='<';   break;
          case CommandType::CMD_GREATER: m_entry+='>';   break;
          case CommandType::CMD_AMP:     m_entry+='&';   break;
          case CommandType::CMD_DOLLAR:  m_entry+='$';   break;
          case CommandType::CMD_HASH:    m_entry+='#';   break;
          case CommandType::CMD_DCOLON:  m_entry+="::";  break;
          case CommandType::CMD_PERCENT: m_entry+='%';   break;
          case CommandType::CMD_NDASH:   m_entry+="--";  break;
          case CommandType::CMD_MDASH:   m_entry+="---"; break;
          case CommandType::CMD_QUOTE:   m_entry+='"';   break;
          case CommandType::CMD_PUNT:    m_entry+='.';   break;
          case CommandType::CMD_PLUS:    m_entry+='+';   break;
          case CommandType::CMD_MINUS:   m_entry+='-';   break;
          case CommandType::CMD_EQUAL:   m_entry+='=';   break;
          default:
               warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected command %s found as argument of \\addindex",
                              qPrint(parser()->context.token->name));
               break;
        }
      break;
      default:
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected token %s",
            tok.to_string());
        break;
    }
    tok = parser()->tokenizer.lex();
  }
  parser()->tokenizer.setStatePara();
  m_entry = m_entry.stripWhiteSpace();
endindexentry:
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
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

Token DocHtmlCaption::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_TK_NONE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      switch (tok.value())
      {
        case TokenRetval::TK_HTMLTAG:
          {
            HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
            if (tagId==HtmlTagType::HTML_CAPTION && parser()->context.token->endTag) // found </caption> tag
            {
              retval = Token::make_RetVal_OK();
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
    tok = parser()->tokenizer.lex();
  }
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end of comment while inside"
           " <caption> tag");
  }
endcaption:
  parser()->handlePendingStyleCommands(thisVariant(),children());
  return retval;
}

//---------------------------------------------------------------------------

Token DocHtmlCell::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=nullptr;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (retval.is(TokenRetval::TK_HTMLTAG))
    {
      HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
      if (tagId==HtmlTagType::HTML_TD && parser()->context.token->endTag) // found </td> tag
      {
        retval = Token::make_TK_NEWPARA(); // ignore the tag
      }
      else if (tagId==HtmlTagType::HTML_TH && parser()->context.token->endTag) // found </th> tag
      {
        retval = Token::make_TK_NEWPARA(); // ignore the tag
      }
    }
  }
  while (retval.is_any_of(TokenRetval::TK_NEWPARA,TokenRetval::RetVal_EndParBlock));
  if (par) par->markLast();

  return retval;
}

Token DocHtmlCell::parseXml()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=nullptr;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (retval.is(TokenRetval::TK_HTMLTAG))
    {
      HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
      if (tagId==HtmlTagType::XML_ITEM && parser()->context.token->endTag) // found </item> tag
      {
        retval = Token::make_TK_NEWPARA(); // ignore the tag
      }
      else if (tagId==HtmlTagType::XML_DESCRIPTION && parser()->context.token->endTag) // found </description> tag
      {
        retval = Token::make_TK_NEWPARA(); // ignore the tag
      }
    }
  }
  while (retval.is(TokenRetval::TK_NEWPARA));
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

Token DocHtmlRow::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());

  bool isHeading=FALSE;
  bool isFirst=TRUE;
  DocHtmlCell *cell=nullptr;

  // get next token
  Token tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok.is_any_of(TokenRetval::TK_WHITESPACE,TokenRetval::TK_NEWPARA)) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok.is(TokenRetval::TK_HTMLTAG))
  {
    HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HtmlTagType::HTML_TD && !parser()->context.token->endTag) // found <td> tag
    {
    }
    else if (tagId==HtmlTagType::HTML_TH && !parser()->context.token->endTag) // found <th> tag
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
  else if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) // premature end of comment
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto endrow;
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <td> or <th> tag but found %s token instead!",
        tok.to_string());
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
    isHeading = retval.is(TokenRetval::RetVal_TableHCell);
  }
  while (retval.is_any_of(TokenRetval::RetVal_TableCell,TokenRetval::RetVal_TableHCell));
  cell->markLast(TRUE);

endrow:
  return retval;
}

Token DocHtmlRow::parseXml(bool isHeading)
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());

  bool isFirst=TRUE;
  DocHtmlCell *cell=nullptr;

  // get next token
  Token tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok.is_any_of(TokenRetval::TK_WHITESPACE,TokenRetval::TK_NEWPARA)) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok.is(TokenRetval::TK_HTMLTAG))
  {
    HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HtmlTagType::XML_TERM && !parser()->context.token->endTag) // found <term> tag
    {
    }
    else if (tagId==HtmlTagType::XML_DESCRIPTION && !parser()->context.token->endTag) // found <description> tag
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
  else if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) // premature end of comment
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto endrow;
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <td> or <th> tag but found %s token instead!",
        tok.to_string());
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
  while (retval.is_any_of(TokenRetval::RetVal_TableCell,TokenRetval::RetVal_TableHCell));
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
  return nullptr;
}

Token DocHtmlTable::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());

getrow:
  // get next token
  Token tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok.is_any_of(TokenRetval::TK_WHITESPACE,TokenRetval::TK_NEWPARA)) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok.is(TokenRetval::TK_HTMLTAG))
  {
    HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HtmlTagType::HTML_THEAD && !parser()->context.token->endTag) // found <thead> tag
    {
      goto getrow;
    }
    else if (tagId==HtmlTagType::HTML_TBODY && !parser()->context.token->endTag) // found <tbody> tag
    {
      goto getrow;
    }
    else if (tagId==HtmlTagType::HTML_TFOOT && !parser()->context.token->endTag) // found <tfoot> tag
    {
      goto getrow;
    }
    else if (tagId==HtmlTagType::HTML_TR && !parser()->context.token->endTag) // found <tr> tag
    {
      // no caption, just rows
      retval = Token::make_RetVal_TableRow();
    }
    else if (tagId==HtmlTagType::HTML_CAPTION && !parser()->context.token->endTag) // found <caption> tag
    {
      if (m_caption)
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"table already has a caption, found another one");
      }
      else
      {
        m_caption = createDocNode<DocHtmlCaption>(parser(),thisVariant(),parser()->context.token->attribs);
        retval=std::get<DocHtmlCaption>(*m_caption).parse();

        if (retval.is(TokenRetval::RetVal_OK)) // caption was parsed ok
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
  else if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) // premature end of comment
  {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
          " for a <tr> or <caption> tag");
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <tr> tag but found %s token instead!",
        tok.to_string());
  }

  // parse one or more rows
  while (retval.is(TokenRetval::RetVal_TableRow))
  {
    children().append<DocHtmlRow>(parser(),thisVariant(),parser()->context.token->attribs);
    retval = children().get_last<DocHtmlRow>()->parse();
  }

  computeTableGrid();

  return retval.is(TokenRetval::RetVal_EndTable) ? Token::make_RetVal_OK() : retval;
}

Token DocHtmlTable::parseXml()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // get next token
  Token tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok.is_any_of(TokenRetval::TK_WHITESPACE,TokenRetval::TK_NEWPARA)) tok=parser()->tokenizer.lex();
  // should find a html tag now
  HtmlTagType tagId=HtmlTagType::UNKNOWN;
  bool isHeader=FALSE;
  if (tok.is(TokenRetval::TK_HTMLTAG))
  {
    tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HtmlTagType::XML_ITEM && !parser()->context.token->endTag) // found <item> tag
    {
      retval = Token::make_RetVal_TableRow();
    }
    if (tagId==HtmlTagType::XML_LISTHEADER && !parser()->context.token->endTag) // found <listheader> tag
    {
      retval = Token::make_RetVal_TableRow();
      isHeader=TRUE;
    }
  }

  // parse one or more rows
  while (retval.is(TokenRetval::RetVal_TableRow))
  {
    children().append<DocHtmlRow>(parser(),thisVariant(),parser()->context.token->attribs);
    DocHtmlRow *tr = children().get_last<DocHtmlRow>();
    retval=tr->parseXml(isHeader);
    isHeader=FALSE;
  }

  computeTableGrid();

  tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
  return tagId==HtmlTagType::XML_LIST && parser()->context.token->endTag ? Token::make_RetVal_OK() : retval;
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
      for (auto &cellNode : row->children())
      {
        DocHtmlCell *cell = std::get_if<DocHtmlCell>(&cellNode);
        if (cell)
        {
          uint32_t rs = cell->rowSpan();
          uint32_t cs = cell->colSpan();

          for (size_t i=0;i<rowSpans.size();i++)
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
      for (size_t i=0;i<rowSpans.size();i++)
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

Token DocHtmlDescTitle::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_TK_NONE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      switch (tok.value())
      {
        case TokenRetval::TK_COMMAND_AT:
        // fall through
        case TokenRetval::TK_COMMAND_BS:
          {
            QCString cmdName=parser()->context.token->name;
            bool isJavaLink=FALSE;
            switch (Mappers::cmdMapper->map(cmdName))
            {
              case CommandType::CMD_REF:
                {
                  tok=parser()->tokenizer.lex();
                  if (!tok.is(TokenRetval::TK_WHITESPACE))
                  {
                    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after '%c%s' command",
                        tok.command_to_char(),qPrint(cmdName));
                  }
                  else
                  {
                    parser()->tokenizer.setStateRef();
                    tok=parser()->tokenizer.lex(); // get the reference id
                    if (!tok.is(TokenRetval::TK_WORD))
                    {
                      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of '%c%s' command",
                          tok.to_string(),tok.command_to_char(),qPrint(cmdName));
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
              case CommandType::CMD_JAVALINK:
                isJavaLink=TRUE;
                // fall through
              case CommandType::CMD_LINK:
                {
                  tok=parser()->tokenizer.lex();
                  if (!tok.is(TokenRetval::TK_WHITESPACE))
                  {
                    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
                        qPrint(cmdName));
                  }
                  else
                  {
                    parser()->tokenizer.setStateLink();
                    tok=parser()->tokenizer.lex();
                    if (!tok.is(TokenRetval::TK_WORD))
                    {
                      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of \\%s command",
                          tok.to_string(),qPrint(cmdName));
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
                warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal command '%c%s' found as part of a <dt> tag",
                  tok.command_to_char(),qPrint(cmdName));
            }
          }
          break;
        case TokenRetval::TK_SYMBOL:
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported symbol '%s' found as part of a <dt> tag",
              qPrint(parser()->context.token->name));
          break;
        case TokenRetval::TK_HTMLTAG:
          {
            HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
            if (tagId==HtmlTagType::HTML_DD && !parser()->context.token->endTag) // found <dd> tag
            {
              retval = Token::make_RetVal_DescData();
              goto endtitle;
            }
            else if (tagId==HtmlTagType::HTML_DT && parser()->context.token->endTag)
            {
              // ignore </dt> tag.
            }
            else if (tagId==HtmlTagType::HTML_DT)
            {
              // missing <dt> tag.
              retval = Token::make_RetVal_DescTitle();
              goto endtitle;
            }
            else if (tagId==HtmlTagType::HTML_DL && parser()->context.token->endTag)
            {
              retval = Token::make_RetVal_EndDesc();
              goto endtitle;
            }
            else if (tagId==HtmlTagType::HTML_A)
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
              tok.to_string());
          break;
      }
    }
    tok = parser()->tokenizer.lex();
  }
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end of comment while inside"
        " <dt> tag");
  }
endtitle:
  parser()->handlePendingStyleCommands(thisVariant(),children());
  return retval;
}

//---------------------------------------------------------------------------

Token DocHtmlDescData::parse()
{
  AUTO_TRACE();
  m_attribs = parser()->context.token->attribs;
  Token retval = Token::make_TK_NONE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  bool isFirst=TRUE;
  DocPara *par=nullptr;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval.is(TokenRetval::TK_NEWPARA));
  if (par) par->markLast();

  return retval;
}

//---------------------------------------------------------------------------

Token DocHtmlDescList::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // get next token
  Token tok=parser()->tokenizer.lex();
  // skip whitespace
  while (tok.is_any_of(TokenRetval::TK_WHITESPACE,TokenRetval::TK_NEWPARA)) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok.is(TokenRetval::TK_HTMLTAG))
  {
    HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HtmlTagType::HTML_DT && !parser()->context.token->endTag) // found <dt> tag
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
  else if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) // premature end of comment
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html description title");
    goto enddesclist;
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <dt> tag but found %s token instead!",
        tok.to_string());
    goto enddesclist;
  }

  do
  {
    children().append<DocHtmlDescTitle>(parser(),thisVariant(),parser()->context.token->attribs);
    DocHtmlDescTitle *dt   = children().get_last<DocHtmlDescTitle>();
    children().append<DocHtmlDescData>(parser(),thisVariant());
    DocHtmlDescData *dd    = children().get_last<DocHtmlDescData>();
    retval=dt->parse();
    if (retval.is(TokenRetval::RetVal_DescData))
    {
      retval=dd->parse();
      while (retval.is(TokenRetval::RetVal_DescData))
      {
        children().append<DocHtmlDescData>(parser(),thisVariant());
        dd    = children().get_last<DocHtmlDescData>();
        retval=dd->parse();
      }
    }
    else if (!retval.is(TokenRetval::RetVal_DescTitle))
    {
      // error
      break;
    }
  } while (retval.is(TokenRetval::RetVal_DescTitle));

  if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while inside <dl> block");
  }

enddesclist:

  return retval.is(TokenRetval::RetVal_EndDesc) ? Token::make_RetVal_OK() : retval;
}

//---------------------------------------------------------------------------

Token DocHtmlListItem::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_TK_NONE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=nullptr;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval.is(TokenRetval::TK_NEWPARA));
  if (par) par->markLast();

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

Token DocHtmlListItem::parseXml()
{
  AUTO_TRACE();
  Token retval = Token::make_TK_NONE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=nullptr;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
    if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) break;

    //printf("new item: retval=%x parser()->context.token->name=%s parser()->context.token->endTag=%d\n",
    //    retval,qPrint(parser()->context.token->name),parser()->context.token->endTag);
    if (retval.is(TokenRetval::RetVal_ListItem))
    {
      break;
    }
  }
  while (!retval.is(TokenRetval::RetVal_CloseXml));

  if (par) par->markLast();

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

//---------------------------------------------------------------------------

Token DocHtmlList::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  int num=1;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // get next token
  Token tok=parser()->tokenizer.lex();
  // skip whitespace and paragraph breaks
  while (tok.is_any_of(TokenRetval::TK_WHITESPACE,TokenRetval::TK_NEWPARA)) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok.is(TokenRetval::TK_HTMLTAG))
  {
    HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    if (tagId==HtmlTagType::HTML_LI && !parser()->context.token->endTag) // found <li> tag
    {
      // ok, we can go on.
    }
    else if (((m_type==Unordered && tagId==HtmlTagType::HTML_UL) ||
              (m_type==Ordered   && tagId==HtmlTagType::HTML_OL)
             ) && parser()->context.token->endTag
            ) // found empty list
    {
      // add dummy item to obtain valid HTML
      children().append<DocHtmlListItem>(parser(),thisVariant(),HtmlAttribList(),1);
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"empty list!");
      retval = Token::make_RetVal_EndList();
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
  else if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) // premature end of comment
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
        tok.to_string());
    goto endlist;
  }

  do
  {
    children().append<DocHtmlListItem>(parser(),thisVariant(),parser()->context.token->attribs,num++);
    DocHtmlListItem *li = children().get_last<DocHtmlListItem>();
    retval=li->parse();
  } while (retval.is(TokenRetval::RetVal_ListItem));

  if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while inside <%cl> block",
        m_type==Unordered ? 'u' : 'o');
  }

endlist:
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval.is(TokenRetval::RetVal_EndList) ? Token::make_RetVal_OK() : retval;
}

Token DocHtmlList::parseXml()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  int num=1;
  auto ns = AutoNodeStack(parser(),thisVariant());

  // get next token
  Token tok=parser()->tokenizer.lex();
  // skip whitespace and paragraph breaks
  while (tok.is_any_of(TokenRetval::TK_WHITESPACE,TokenRetval::TK_NEWPARA)) tok=parser()->tokenizer.lex();
  // should find a html tag now
  if (tok.is(TokenRetval::TK_HTMLTAG))
  {
    HtmlTagType tagId=Mappers::htmlTagMapper->map(parser()->context.token->name);
    //printf("parser()->context.token->name=%s parser()->context.token->endTag=%d\n",qPrint(parser()->context.token->name),parser()->context.token->endTag);
    if (tagId==HtmlTagType::XML_ITEM && !parser()->context.token->endTag) // found <item> tag
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
  else if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) // premature end of comment
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while looking"
        " for a html list item");
    goto endlist;
  }
  else // token other than html token
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected <item> tag but found %s token instead!",
        tok.to_string());
    goto endlist;
  }

  do
  {
    children().append<DocHtmlListItem>(parser(),thisVariant(),parser()->context.token->attribs,num++);
    DocHtmlListItem *li   = children().get_last<DocHtmlListItem>();
    retval=li->parseXml();
    if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) break;
    //printf("retval=%x parser()->context.token->name=%s\n",retval,qPrint(parser()->context.token->name));
  } while (retval.is(TokenRetval::RetVal_ListItem));

  if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while inside <list type=\"%s\"> block",
        m_type==Unordered ? "bullet" : "number");
  }

endlist:
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return (retval.is_any_of(TokenRetval::RetVal_EndList,TokenRetval::RetVal_CloseXml) || parser()->context.token->name=="list") ?
         Token::make_RetVal_OK() : retval;
}

//--------------------------------------------------------------------------

Token DocHtmlBlockQuote::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_TK_NONE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=nullptr;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval.is(TokenRetval::TK_NEWPARA));
  if (par) par->markLast();

  if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment while inside <blockquote> block");
  }

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval.is(TokenRetval::RetVal_EndBlockQuote) ? Token::make_RetVal_OK() : retval;
}

//---------------------------------------------------------------------------

Token DocParBlock::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_TK_NONE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // parse one or more paragraphs
  bool isFirst=TRUE;
  DocPara *par=nullptr;
  do
  {
    children().append<DocPara>(parser(),thisVariant());
    par = children().get_last<DocPara>();
    if (isFirst) { par->markFirst(); isFirst=FALSE; }
    retval=par->parse();
  }
  while (retval.is(TokenRetval::TK_NEWPARA));
  if (par) par->markLast();

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval.is(TokenRetval::RetVal_EndBlockQuote) ? Token::make_RetVal_OK() : retval;
}

//---------------------------------------------------------------------------

DocSimpleListItem::DocSimpleListItem(DocParser *parser,DocNodeVariant *parent)
      : DocNode(parser,parent)
{
}


Token DocSimpleListItem::parse()
{
  auto ns = AutoNodeStack(parser(),thisVariant());
  m_paragraph = createDocNode<DocPara>(parser(),thisVariant());
  DocPara *par = &std::get<DocPara>(*m_paragraph);
  Token rv=par->parse();
  par->markFirst();
  par->markLast();
  return rv;
}

//--------------------------------------------------------------------------

Token DocSimpleList::parse()
{
  auto ns = AutoNodeStack(parser(),thisVariant());
  Token rv = Token::make_TK_NONE();
  do
  {
    children().append<DocSimpleListItem>(parser(),thisVariant());
    DocSimpleListItem *li   = children().get_last<DocSimpleListItem>();
    rv=li->parse();
  } while (rv.is(TokenRetval::RetVal_ListItem));
  return (!rv.is(TokenRetval::TK_NEWPARA)) ? rv : Token::make_RetVal_OK();
}

//--------------------------------------------------------------------------

DocAutoListItem::DocAutoListItem(DocParser *parser,DocNodeVariant *parent,int indent,int num)
      : DocCompoundNode(parser,parent), m_indent(indent), m_itemNum(num)
{
}

Token DocAutoListItem::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=nullptr;
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
  } while (retval.is(TokenRetval::TK_NEWPARA) && parser()->context.token->indent>m_indent);
  if (lastPar) lastPar->markLast();

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

//--------------------------------------------------------------------------

DocAutoList::DocAutoList(DocParser *parser,DocNodeVariant *parent,int indent,bool isEnumList,
                         int depth, bool isCheckedList):
      DocCompoundNode(parser,parent), m_indent(indent), m_isEnumList(isEnumList),
      m_isCheckedList(isCheckedList), m_depth(depth)
{
}

Token DocAutoList::parse()
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  int num=1;
  auto ns = AutoNodeStack(parser(),thisVariant());
  parser()->tokenizer.startAutoList();
	  // first item or sub list => create new list
  do
  {
    switch (parser()->context.token->id)
    {
      case -1:
        break;
      case DocAutoList::Unchecked: // unchecked
      case DocAutoList::Checked_x: // checked with x
      case DocAutoList::Checked_X: // checked with X
        num = parser()->context.token->id;
        break;
      default: // explicitly numbered list
        num=parser()->context.token->id;  // override num with real number given
        break;
    }

    children().append<DocAutoListItem>(parser(),thisVariant(),m_indent,num++);
    retval = children().get_last<DocAutoListItem>()->parse();
    //printf("DocAutoList::parse(): retval=0x%x parser()->context.token->indent=%d m_indent=%d "
    //       "m_isEnumList=%d parser()->context.token->isEnumList=%d parser()->context.token->name=%s\n",
    //       retval,parser()->context.token->indent,m_indent,m_isEnumList,parser()->context.token->isEnumList,
    //       qPrint(parser()->context.token->name));
    //printf("num=%d parser()->context.token->id=%d\n",num,parser()->context.token->id);
  }
  while (retval.is(TokenRetval::TK_LISTITEM) &&                // new list item
         m_indent==parser()->context.token->indent &&          // at same indent level
	 m_isEnumList==parser()->context.token->isEnumList &&  // of the same kind
         m_isCheckedList==parser()->context.token->isCheckedList &&  // of the same kind
         (parser()->context.token->id==-1 || parser()->context.token->id>=num)  // increasing number (or no number or checked list)
        );

  parser()->tokenizer.endAutoList();
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

//--------------------------------------------------------------------------

void DocTitle::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  parser()->tokenizer.setStateTitle();
  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF))
  {
    if (!parser()->defaultHandleToken(thisVariant(),tok,children()))
    {
      parser()->errorHandleDefaultToken(thisVariant(),tok,children(),"title section");
    }
    tok = parser()->tokenizer.lex();
  }
  parser()->tokenizer.setStatePara();
  parser()->handlePendingStyleCommands(thisVariant(),children());
}

void DocTitle::parseFromString(DocNodeVariant *parent,const QCString &text)
{
  parser()->context.insideHtmlLink=TRUE;
  parser()->pushContext(); // this will create a new parser->context.token
  parser()->internalValidatingParseDoc(thisVariant(),children(),text);
  parser()->popContext(); // this will restore the old parser->context.token
  parser()->context.insideHtmlLink=FALSE;
  parser()->tokenizer.setStatePara();
  flattenParagraphs(thisVariant(),children());
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

Token DocSimpleSect::parse(bool userTitle,bool needsSeparator)
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
  Token retval = par->parse();

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval; // 0==EOF, TokenRetval::TK_NEWPARA, TokenRetval::TK_LISTITEM, TokenRetval::TK_ENDLIST, TokenRetval::RetVal_SimpleSec
}

Token DocSimpleSect::parseRcs()
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

  return Token::make_RetVal_OK();
}

Token DocSimpleSect::parseXml()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());

  Token retval = Token::make_RetVal_OK();
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
    if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) break;
    if (retval.is(TokenRetval::RetVal_CloseXml))
    {
      retval = Token::make_RetVal_OK();
      break;
    }
  }

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

void DocSimpleSect::appendLinkWord(const QCString &word)
{
  DocPara *p=nullptr;
  if (children().empty() || (p=std::get_if<DocPara>(&children().back()))==nullptr)
  {
    children().append<DocPara>(parser(),thisVariant());
    p = children().get_last<DocPara>();
  }
  else
  {
    // Comma-separate <seealso> links.
    p->injectToken(Token::make_TK_WORD(),",");
    p->injectToken(Token::make_TK_WHITESPACE()," ");
  }

  parser()->context.inSeeBlock=TRUE;
  p->injectToken(Token::make_TK_LNKWORD(),word);
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
    case Important:  return "important";
    case User:       return "user";
    case Rcs:        return "rcs";
  }
  return "unknown";
}

//--------------------------------------------------------------------------

Token DocParamList::parse(const QCString &cmdName)
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());
  DocPara *par=nullptr;
  QCString saveCmdName = cmdName;

  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    retval = Token::make_RetVal_EndParBlock();
    goto endparamlist;
  }
  parser()->tokenizer.setStateParam();
  tok=parser()->tokenizer.lex();
  while (tok.is(TokenRetval::TK_WORD)) /* there is a parameter name */
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
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) // premature end of comment
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(saveCmdName));
    retval = Token::make_RetVal_EndParBlock();
    goto endparamlist;
  }
  if (!tok.is(TokenRetval::TK_WHITESPACE)) /* premature end of comment block */
  {
    if (!tok.is(TokenRetval::TK_NEWPARA)) /* empty param description */
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s in comment block while parsing the "
          "argument of command %s",tok.to_string(),qPrint(saveCmdName));
    }
    retval = Token::make_RetVal_EndParBlock();
    goto endparamlist;
  }

  m_paragraphs.append<DocPara>(parser(),thisVariant());
  par = m_paragraphs.get_last<DocPara>();
  retval = par->parse();
  par->markFirst();
  par->markLast();

endparamlist:
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

Token DocParamList::parseXml(const QCString &paramName)
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
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

    if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) break;

  } while (retval.is(TokenRetval::RetVal_CloseXml) &&
           Mappers::htmlTagMapper->map(parser()->context.token->name)!=HtmlTagType::XML_PARAM &&
           Mappers::htmlTagMapper->map(parser()->context.token->name)!=HtmlTagType::XML_TYPEPARAM &&
           Mappers::htmlTagMapper->map(parser()->context.token->name)!=HtmlTagType::XML_EXCEPTION);

  if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF)) /* premature end of comment block */
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unterminated param or exception tag");
  }
  else
  {
    retval = Token::make_RetVal_OK();
  }

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

//--------------------------------------------------------------------------

Token DocParamSect::parse(const QCString &cmdName,bool xmlContext, Direction d)
{
  AUTO_TRACE();
  Token retval = Token::make_RetVal_OK();
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
  if (retval.is(TokenRetval::RetVal_EndParBlock))
  {
    retval = Token::make_RetVal_OK();
  }

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

//--------------------------------------------------------------------------

DocPara::DocPara(DocParser *parser,DocNodeVariant *parent) :
        DocCompoundNode(parser,parent),
        m_isFirst(FALSE), m_isLast(FALSE)
{
}

Token DocPara::handleSimpleSection(DocSimpleSect::Type t, bool xmlContext)
{
  AUTO_TRACE();
  DocSimpleSect *ss=nullptr;
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
  Token rv = Token::make_RetVal_OK();
  if (xmlContext)
  {
    return ss->parseXml();
  }
  else
  {
    rv = ss->parse(t==DocSimpleSect::User,needsSeparator);
  }
  return (!rv.is(TokenRetval::TK_NEWPARA)) ? rv : Token::make_RetVal_OK();
}

Token DocPara::handleParamSection(const QCString &cmdName,
                                DocParamSect::Type t,
                                bool xmlContext=FALSE,
                                int direction=DocParamSect::Unspecified)
{
  AUTO_TRACE();
  DocParamSect   *ps = nullptr;
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
  Token rv=ps->parse(cmdName,xmlContext,
                   static_cast<DocParamSect::Direction>(direction));
  AUTO_TRACE_EXIT("retval={}",rv.to_string());
  return (!rv.is(TokenRetval::TK_NEWPARA)) ? rv : Token::make_RetVal_OK();
}

void DocPara::handleCite(char cmdChar,const QCString &cmdName)
{
  AUTO_TRACE();
  // get the argument of the cite command.
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after '%c%s' command",
      cmdChar,qPrint(cmdName));
    return;
  }
  parser()->tokenizer.setStateCite();
  tok=parser()->tokenizer.lex();
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command '%c%s'",cmdChar,qPrint(cmdName));
    return;
  }
  else if (!tok.is_any_of(TokenRetval::TK_WORD,TokenRetval::TK_LNKWORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of '%c%s'",
        tok.to_string(),cmdChar,qPrint(cmdName));
    return;
  }
  parser()->context.token->sectionId = parser()->context.token->name;
  children().append<DocCite>(
        parser(),thisVariant(),parser()->context.token->name,parser()->context.context);

  parser()->tokenizer.setStatePara();
}

void DocPara::handleEmoji(char cmdChar,const QCString &cmdName)
{
  AUTO_TRACE();
  // get the argument of the emoji command.
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after '%c%s' command",
      cmdChar,qPrint(cmdName));
    return;
  }
  parser()->tokenizer.setStateEmoji();
  tok=parser()->tokenizer.lex();
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"no emoji name given or unexpected end of comment block while parsing the "
        "argument of command '%c%s'",cmdChar,qPrint(cmdName));
    parser()->tokenizer.setStatePara();
    return;
  }
  else if (!tok.is(TokenRetval::TK_WORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of '%c%s'",
        tok.to_string(),cmdChar,qPrint(cmdName));
    parser()->tokenizer.setStatePara();
    return;
  }
  children().append<DocEmoji>(parser(),thisVariant(),parser()->context.token->name);
  parser()->tokenizer.setStatePara();
}

void DocPara::handleDoxyConfig(char cmdChar,const QCString &cmdName)
{
  // get the argument of the cite command.
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after '%c%s' command",
      cmdChar,qPrint(cmdName));
    return;
  }
  parser()->tokenizer.setStateDoxyConfig();
  tok=parser()->tokenizer.lex();
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command '%c%s'",cmdChar,qPrint(cmdName));
    return;
  }
  else if (!tok.is_any_of(TokenRetval::TK_WORD,TokenRetval::TK_LNKWORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of '%c%s'",
        tok.to_string(),cmdChar,qPrint(cmdName));
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
        warn(parser()->context.fileName,parser()->tokenizer.getLineNr(), "Obsolete setting for '%c%s': '%s'",
              cmdChar,qPrint(cmdName),qPrint(parser()->context.token->name));
        break;
      case ConfigOption::O_Disabled:
        warn(parser()->context.fileName,parser()->tokenizer.getLineNr(),
              "Disabled setting (i.e. not supported in this doxygen executable) for '%c%s': '%s'",
              cmdChar,qPrint(cmdName),qPrint(parser()->context.token->name));
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
    warn(parser()->context.fileName,parser()->tokenizer.getLineNr(), "Unknown option for '%c%s': '%s'",
         cmdChar,qPrint(cmdName),qPrint(parser()->context.token->name));
    children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
  }
  parser()->tokenizer.setStatePara();
}

Token DocPara::handleXRefItem()
{
  AUTO_TRACE();
  Token retval=parser()->tokenizer.lex();
  ASSERT(retval.is(TokenRetval::TK_WHITESPACE));
  parser()->tokenizer.setStateXRefItem();
  retval=parser()->tokenizer.lex();
  if (retval.is(TokenRetval::RetVal_OK))
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

void DocPara::handleShowDate(char cmdChar,const QCString &cmdName)
{
  AUTO_TRACE();
  QCString fmt;
  QCString date;
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after '%c%s' command",
      cmdChar,qPrint(cmdName));
    return;
  }
  parser()->tokenizer.setStateQuotedString();
  tok = parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"invalid <format> argument for command '%c%s'",
      cmdChar,qPrint(cmdName));
    parser()->tokenizer.setStatePara();
    return;
  }
  fmt = parser()->context.token->name;

  parser()->tokenizer.setStateShowDate();
  tok = parser()->tokenizer.lex();

  QCString specDateRaw = tok.is(TokenRetval::TK_WORD) ? parser()->context.token->name : QCString();
  QCString specDate    = specDateRaw.stripWhiteSpace();
  bool specDateOnlyWS  = !specDateRaw.isEmpty() && specDate.isEmpty();
  if (!specDate.isEmpty() && !tok.is_any_of(TokenRetval::TK_WORD,TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"invalid <date_time> argument for command '%c%s'",
      cmdChar,qPrint(cmdName));
    parser()->tokenizer.setStatePara();
    return;
  }

  std::tm dat{};
  int specFormat=0;
  QCString err = dateTimeFromString(specDate,dat,specFormat);
  if (!err.isEmpty())
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"invalid <date_time> argument for command '%c%s': %s",
      cmdChar,qPrint(cmdName),qPrint(err));
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
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"'%c%s' <format> parameter '%s' has %s related markers which are not specified in the <date_time> parameter '%s'. Filling in the current value for %s instead.",
          cmdChar,qPrint(cmdName),
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

void DocPara::handleILine(char cmdChar,const QCString &cmdName)
{
  AUTO_TRACE();
  parser()->tokenizer.setStateILine();
  Token tok = parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"invalid argument for command '%c%s'",
      cmdChar,qPrint(cmdName));
    return;
  }
  parser()->tokenizer.setStatePara();
}

void DocPara::handleIFile(char cmdChar,const QCString &cmdName)
{
  AUTO_TRACE();
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after '%c%s' command",
      cmdChar,qPrint(cmdName));
    return;
  }
  parser()->tokenizer.setStateFile();
  tok=parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
  if (!tok.is(TokenRetval::TK_WORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of '%c%s'",
      tok.to_string(),cmdChar,qPrint(cmdName));
    return;
  }
  parser()->context.fileName = parser()->context.token->name;
  parser()->tokenizer.setStatePara();
}


void DocPara::handleIncludeOperator(const QCString &cmdName,DocIncOperator::Type t)
{
  AUTO_TRACE("cmdName={}",cmdName);
  QCString saveCmdName = cmdName;
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStatePattern();
  tok=parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s", qPrint(saveCmdName));
    return;
  }
  else if (!tok.is(TokenRetval::TK_WORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        tok.to_string(),qPrint(saveCmdName));
    return;
  }
  auto it1 = children().size()>=1 ? std::prev(children().end()) : children().end();
  auto it2 = children().size()>=2 ? std::prev(it1)              : children().end();
  DocNodeVariant *n1 = it1!=children().end() ? &(*it1) : nullptr;
  DocNodeVariant *n2 = it2!=children().end() ? &(*it2) : nullptr;
  //TODO get from context the stripCodeComments()
  bool stripCodeComments = Config_getBool(STRIP_CODE_COMMENTS);
  children().append<DocIncOperator>(parser(),thisVariant(),t,
                                    parser()->context.token->name,
                                    parser()->context.context,
                                    stripCodeComments,
                                    parser()->context.isExample,
                                    parser()->context.exampleName
                                    );
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
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStateFile();
  tok=parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
  if (!tok.is(TokenRetval::TK_WORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        tok.to_string(),qPrint(saveCmdName));
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
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStateLink();
  tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"%s as the argument of %s",
        tok.to_string(),qPrint(saveCmdName));
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

void DocPara::handleRef(char cmdChar,const QCString &cmdName)
{
  AUTO_TRACE("cmdName={}",cmdName);
  QCString saveCmdName = cmdName;
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after '%c%s' command",
      cmdChar,qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStateRef();
  tok=parser()->tokenizer.lex(); // get the reference id
  if (!tok.is(TokenRetval::TK_WORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of '%c%s'",
        tok.to_string(),cmdChar,qPrint(saveCmdName));
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
  Token tok=parser()->tokenizer.lex();
  bool isBlock = false;
  bool trimLeft = false;
  bool localScope = false;
  bool stripCodeComments = Config_getBool(STRIP_CODE_COMMENTS);
  if (tok.is(TokenRetval::TK_WORD) && parser()->context.token->name=="{")
  {
    parser()->tokenizer.setStateOptions();
    parser()->tokenizer.lex();
    parser()->tokenizer.setStatePara();
    StringVector optList=split(parser()->context.token->name.str(),",");
    auto contains = [&optList](const char *kw)
    {
      return std::find(optList.begin(),optList.end(),kw)!=optList.end();
    };
    localScope = contains("local");
    if (contains("nostrip"))
    {
      stripCodeComments = false;
    }
    else if (contains("strip"))
    {
      stripCodeComments = true;
    }
    if (t==DocInclude::Snippet && contains("trimleft"))
    {
      trimLeft = true;
    }

    if (t==DocInclude::Include && contains("lineno"))
    {
      t = DocInclude::IncWithLines;
    }
    else if (t==DocInclude::Snippet && contains("lineno"))
    {
      t = DocInclude::SnippetWithLines;
    }
    else if (t==DocInclude::DontInclude && contains("lineno"))
    {
      t = DocInclude::DontIncWithLines;
    }
    tok=parser()->tokenizer.lex();
    if (!tok.is(TokenRetval::TK_WHITESPACE))
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
          qPrint(saveCmdName));
      return;
    }
  }
  else if (tok.is(TokenRetval::TK_WORD) && parser()->context.token->name=="[")
  {
    parser()->tokenizer.setStateBlock();
    parser()->tokenizer.lex();
    isBlock = (parser()->context.token->name.stripWhiteSpace() == "block");
    parser()->tokenizer.setStatePara();
    parser()->tokenizer.lex();
  }
  else if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after \\%s command",
        qPrint(saveCmdName));
    return;
  }
  parser()->tokenizer.setStateFile();
  tok=parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command %s",qPrint(saveCmdName));
    return;
  }
  else if (!tok.is(TokenRetval::TK_WORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of %s",
        tok.to_string(),qPrint(saveCmdName));
    return;
  }
  QCString fileName = parser()->context.token->name;
  QCString blockId;
  if (t==DocInclude::Snippet || t==DocInclude::SnippetWithLines)
  {
    if (fileName == "this") fileName=parser()->context.fileName;
    parser()->tokenizer.setStateSnippet();
    tok=parser()->tokenizer.lex();
    parser()->tokenizer.setStatePara();
    if (!tok.is(TokenRetval::TK_WORD))
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected block identifier, but found token %s instead while parsing the %s command",
          tok.to_string(),qPrint(saveCmdName));
      return;
    }
    blockId = "["+parser()->context.token->name+"]";
  }

  children().append<DocInclude>(parser(),
                                thisVariant(),
                                fileName,
                                localScope ? parser()->context.context : "",
                                t,
                                stripCodeComments,
                                parser()->context.isExample,
                                parser()->context.exampleName,
                                blockId,isBlock,trimLeft);
  children().get_last<DocInclude>()->parse();
}

void DocPara::handleSection(char cmdChar,const QCString &cmdName)
{
  AUTO_TRACE("cmdName={}",cmdName);
  QCString saveCmdName = cmdName;
  // get the argument of the section command.
  Token tok=parser()->tokenizer.lex();
  if (!tok.is(TokenRetval::TK_WHITESPACE))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"expected whitespace after '%c%s' command",
        cmdChar,qPrint(saveCmdName));
    return;
  }
  tok=parser()->tokenizer.lex();
  if (tok.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected end of comment block while parsing the "
        "argument of command '%c%s'", cmdChar,qPrint(saveCmdName));
    return;
  }
  else if (!tok.is_any_of(TokenRetval::TK_WORD,TokenRetval::TK_LNKWORD))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected token %s as the argument of '%c%s'",
        tok.to_string(),cmdChar,qPrint(saveCmdName));
    return;
  }
  parser()->context.token->sectionId = parser()->context.token->name;
  parser()->tokenizer.setStateSkipTitle();
  parser()->tokenizer.lex();
  parser()->tokenizer.setStatePara();
}

Token DocPara::handleHtmlHeader(const HtmlAttribList &tagHtmlAttribs,int level)
{
  AUTO_TRACE();
  children().append<DocHtmlHeader>(parser(),thisVariant(),tagHtmlAttribs,level);
  Token retval = children().get_last<DocHtmlHeader>()->parse();
  return retval.is(TokenRetval::RetVal_OK) ? Token::make_TK_NEWPARA() : retval;
}

// For XML tags whose content is stored in attributes rather than
// contained within the element, we need a way to inject the attribute
// text into the current paragraph.
bool DocPara::injectToken(Token tok,const QCString &tokText)
{
  AUTO_TRACE();
  parser()->context.token->name = tokText;
  return parser()->defaultHandleToken(thisVariant(),tok,children());
}

Token DocPara::handleStartCode()
{
  AUTO_TRACE();
  Token retval = parser()->tokenizer.lex();
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
  size_t i=0,li=0,l=parser()->context.token->verb.length();
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
  if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
  {
    warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"code section ended without end marker");
  }
  parser()->tokenizer.setStatePara();
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
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


Token DocPara::handleCommand(char cmdChar, const QCString &cmdName)
{
  AUTO_TRACE("cmdName={}",cmdName);
  Token retval = Token::make_RetVal_OK();
  CommandType cmdId = Mappers::cmdMapper->map(cmdName);
  switch (cmdId)
  {
    case CommandType::UNKNOWN:
      {
        std::string str{cmdChar};
        children().append<DocWord>(parser(),thisVariant(),str.c_str() + cmdName);
        if (isAliasCmd(cmdName.view()))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Found unexpanded alias '%c%s'. Check if number of arguments passed is correct.",cmdChar,qPrint(cmdName));
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Found unknown command '%c%s'",cmdChar,qPrint(cmdName));
        }
      }
      break;
    case CommandType::CMD_EMPHASIS:
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Italic,cmdName,TRUE);
              retval=parser()->handleStyleArgument(thisVariant(),children(),cmdName);
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Italic,cmdName,FALSE);
      if (!retval.is(TokenRetval::TK_WORD)) children().append<DocWhiteSpace>(parser(),thisVariant()," ");
      break;
    case CommandType::CMD_BOLD:
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Bold,cmdName,TRUE);
      retval=parser()->handleStyleArgument(thisVariant(),children(),cmdName);
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Bold,cmdName,FALSE);
      if (!retval.is(TokenRetval::TK_WORD)) children().append<DocWhiteSpace>(parser(),thisVariant()," ");
      break;
    case CommandType::CMD_CODE:
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Code,cmdName,TRUE);
      retval=parser()->handleStyleArgument(thisVariant(),children(),cmdName);
      children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Code,cmdName,FALSE);
      if (!retval.is(TokenRetval::TK_WORD)) children().append<DocWhiteSpace>(parser(),thisVariant()," ");
      break;
    case CommandType::CMD_BSLASH:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_BSlash);
      break;
    case CommandType::CMD_AT:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_At);
      break;
    case CommandType::CMD_LESS:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Less);
      break;
    case CommandType::CMD_GREATER:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Greater);
      break;
    case CommandType::CMD_AMP:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Amp);
      break;
    case CommandType::CMD_DOLLAR:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Dollar);
      break;
    case CommandType::CMD_HASH:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Hash);
      break;
    case CommandType::CMD_PIPE:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Pipe);
      break;
    case CommandType::CMD_DCOLON:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_DoubleColon);
      break;
    case CommandType::CMD_PERCENT:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Percent);
      break;
    case CommandType::CMD_NDASH:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      break;
    case CommandType::CMD_MDASH:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      break;
    case CommandType::CMD_QUOTE:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Quot);
      break;
    case CommandType::CMD_PUNT:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Dot);
      break;
    case CommandType::CMD_PLUS:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Plus);
      break;
    case CommandType::CMD_MINUS:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
      break;
    case CommandType::CMD_EQUAL:
      children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Equal);
      break;
    case CommandType::CMD_SA:
      parser()->context.inSeeBlock=TRUE;
      retval = handleSimpleSection(DocSimpleSect::See);
      parser()->context.inSeeBlock=FALSE;
      break;
    case CommandType::CMD_RETURN:
      retval = handleSimpleSection(DocSimpleSect::Return);
      parser()->context.hasReturnCommand=TRUE;
      break;
    case CommandType::CMD_AUTHOR:
      retval = handleSimpleSection(DocSimpleSect::Author);
      break;
    case CommandType::CMD_AUTHORS:
      retval = handleSimpleSection(DocSimpleSect::Authors);
      break;
    case CommandType::CMD_VERSION:
      retval = handleSimpleSection(DocSimpleSect::Version);
      break;
    case CommandType::CMD_SINCE:
      retval = handleSimpleSection(DocSimpleSect::Since);
      break;
    case CommandType::CMD_DATE:
      retval = handleSimpleSection(DocSimpleSect::Date);
      break;
    case CommandType::CMD_NOTE:
      retval = handleSimpleSection(DocSimpleSect::Note);
      break;
    case CommandType::CMD_WARNING:
      retval = handleSimpleSection(DocSimpleSect::Warning);
      break;
    case CommandType::CMD_PRE:
      retval = handleSimpleSection(DocSimpleSect::Pre);
      break;
    case CommandType::CMD_POST:
      retval = handleSimpleSection(DocSimpleSect::Post);
      break;
    case CommandType::CMD_COPYRIGHT:
      retval = handleSimpleSection(DocSimpleSect::Copyright);
      break;
    case CommandType::CMD_INVARIANT:
      retval = handleSimpleSection(DocSimpleSect::Invar);
      break;
    case CommandType::CMD_REMARK:
      retval = handleSimpleSection(DocSimpleSect::Remark);
      break;
    case CommandType::CMD_ATTENTION:
      retval = handleSimpleSection(DocSimpleSect::Attention);
      break;
    case CommandType::CMD_IMPORTANT:
      retval = handleSimpleSection(DocSimpleSect::Important);
      break;
    case CommandType::CMD_PAR:
      retval = handleSimpleSection(DocSimpleSect::User);
      break;
    case CommandType::CMD_LI:
      {
        children().append<DocSimpleList>(parser(),thisVariant());
        retval = children().get_last<DocSimpleList>()->parse();
      }
      break;
    case CommandType::CMD_SECTION:
      {
        handleSection(cmdChar,cmdName);
        retval = Token::make_RetVal_Section();
      }
      break;
    case CommandType::CMD_SUBSECTION:
      {
        handleSection(cmdChar,cmdName);
        retval = Token::make_RetVal_Subsection();
      }
      break;
    case CommandType::CMD_SUBSUBSECTION:
      {
        handleSection(cmdChar,cmdName);
        retval = Token::make_RetVal_Subsubsection();
      }
      break;
    case CommandType::CMD_PARAGRAPH:
      {
        handleSection(cmdChar,cmdName);
        retval = Token::make_RetVal_Paragraph();
      }
      break;
    case CommandType::CMD_SUBPARAGRAPH:
      {
        handleSection(cmdChar,cmdName);
        retval = Token::make_RetVal_SubParagraph();
      }
      break;
    case CommandType::CMD_SUBSUBPARAGRAPH:
      {
        handleSection(cmdChar,cmdName);
        retval = Token::make_RetVal_SubSubParagraph();
      }
      break;
    case CommandType::CMD_ISTARTCODE:
      {
        parser()->tokenizer.setStateICode();
        retval = handleStartCode();
      }
      break;
    case CommandType::CMD_STARTCODE:
      {
        parser()->tokenizer.setStateCode();
        retval = handleStartCode();
      }
      break;
    case CommandType::CMD_HTMLONLY:
      {
        parser()->tokenizer.setStateHtmlOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::HtmlOnly,parser()->context.isExample,parser()->context.exampleName,parser()->context.token->name=="block");
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"htmlonly section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_MANONLY:
      {
        parser()->tokenizer.setStateManOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::ManOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"manonly section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_RTFONLY:
      {
        parser()->tokenizer.setStateRtfOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::RtfOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"rtfonly section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_LATEXONLY:
      {
        parser()->tokenizer.setStateLatexOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::LatexOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"latexonly section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_XMLONLY:
      {
        parser()->tokenizer.setStateXmlOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::XmlOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"xmlonly section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_DBONLY:
      {
        parser()->tokenizer.setStateDbOnly();
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::DocbookOnly,parser()->context.isExample,parser()->context.exampleName);
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"docbookonly section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_ILITERAL:
      {
        DocVerbatim::Type t = DocVerbatim::JavaDocLiteral;
        parser()->tokenizer.setStateILiteralOpt();
        parser()->tokenizer.lex();

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
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
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
    case CommandType::CMD_IVERBATIM:
    case CommandType::CMD_VERBATIM:
      {
        if (cmdId == CommandType::CMD_VERBATIM)
        {
          parser()->tokenizer.setStateVerbatim();
        }
        else
        {
          parser()->tokenizer.setStateIVerbatim();
        }
        retval = parser()->tokenizer.lex();
        children().append<DocVerbatim>(parser(),thisVariant(),parser()->context.context,parser()->context.token->verb,DocVerbatim::Verbatim,parser()->context.isExample,parser()->context.exampleName);
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"verbatim section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_DOT:
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
        parser()->defaultHandleTitleAndSize(CommandType::CMD_DOT,&children().back(),dv->children(),width,height);
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
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"dot section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_MSC:
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
        parser()->defaultHandleTitleAndSize(CommandType::CMD_MSC,&children().back(),dv->children(),width,height);
        parser()->tokenizer.setStateMsc();
        retval = parser()->tokenizer.lex();
        dv->setText(parser()->context.token->verb);
        dv->setWidth(width);
        dv->setHeight(height);
        dv->setLocation(parser()->context.fileName,parser()->tokenizer.getLineNr());
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"msc section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_STARTUML:
      {
        QCString jarPath = Config_getString(PLANTUML_JAR_PATH);
        parser()->tokenizer.setStatePlantUMLOpt();
        parser()->tokenizer.lex();
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
          assert(retval.is(TokenRetval::RetVal_OK));

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
        parser()->defaultHandleTitleAndSize(CommandType::CMD_STARTUML,&children().back(),dv->children(),width,height);
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
        if (retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF))
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"startuml section ended without end marker");
        }
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_ENDPARBLOCK:
      retval = Token::make_RetVal_EndParBlock();
      break;
    case CommandType::CMD_ENDICODE:
    case CommandType::CMD_ENDCODE:
    case CommandType::CMD_ENDHTMLONLY:
    case CommandType::CMD_ENDMANONLY:
    case CommandType::CMD_ENDRTFONLY:
    case CommandType::CMD_ENDLATEXONLY:
    case CommandType::CMD_ENDXMLONLY:
    case CommandType::CMD_ENDDBONLY:
    case CommandType::CMD_ENDLINK:
    case CommandType::CMD_ENDVERBATIM:
    case CommandType::CMD_ENDIVERBATIM:
    case CommandType::CMD_ENDILITERAL:
    case CommandType::CMD_ENDDOT:
    case CommandType::CMD_ENDMSC:
    case CommandType::CMD_ENDUML:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected command %s",qPrint(parser()->context.token->name));
      break;
    case CommandType::CMD_PARAM:
      retval = handleParamSection(cmdName,DocParamSect::Param,FALSE,parser()->context.token->paramDir);
      break;
    case CommandType::CMD_TPARAM:
      retval = handleParamSection(cmdName,DocParamSect::TemplateParam,FALSE,parser()->context.token->paramDir);
      break;
    case CommandType::CMD_RETVAL:
      retval = handleParamSection(cmdName,DocParamSect::RetVal);
      break;
    case CommandType::CMD_EXCEPTION:
      retval = handleParamSection(cmdName,DocParamSect::Exception);
      break;
    case CommandType::CMD_XREFITEM:
      retval = handleXRefItem();
      break;
    case CommandType::CMD_LINEBREAK:
      {
        children().append<DocLineBreak>(parser(),thisVariant());
      }
      break;
    case CommandType::CMD_IANCHOR:
    case CommandType::CMD_ANCHOR:
      {
        parser()->handleAnchor(thisVariant(),children());
      }
      break;
    case CommandType::CMD_IPREFIX:
      {
        parser()->handlePrefix(thisVariant(),children());
      }
      break;
    case CommandType::CMD_ADDINDEX:
      {
        children().append<DocIndexEntry>(parser(),thisVariant(),
                     parser()->context.scope!=Doxygen::globalScope?parser()->context.scope:nullptr,
                     parser()->context.memberDef);
        retval = children().get_last<DocIndexEntry>()->parse();
      }
      break;
    case CommandType::CMD_INTERNAL:
      retval = Token::make_RetVal_Internal();
      break;
    case CommandType::CMD_ENDINTERNAL:
      retval = Token::make_RetVal_EndInternal();
      break;
    case CommandType::CMD_PARBLOCK:
      {
        children().append<DocParBlock>(parser(),thisVariant());
        retval = children().get_last<DocParBlock>()->parse();
      }
      break;
    case CommandType::CMD_COPYDOC:   // fall through
    case CommandType::CMD_COPYBRIEF: // fall through
    case CommandType::CMD_COPYDETAILS:
      //retval = Token::make_RetVal_CopyDoc();
      // these commands should already be resolved by processCopyDoc()
      break;
    case CommandType::CMD_INCLUDE:
      handleInclude(cmdName,DocInclude::Include);
      break;
    case CommandType::CMD_INCWITHLINES:
      handleInclude(cmdName,DocInclude::IncWithLines);
      break;
    case CommandType::CMD_DONTINCLUDE:
      handleInclude(cmdName,DocInclude::DontInclude);
      break;
    case CommandType::CMD_HTMLINCLUDE:
      handleInclude(cmdName,DocInclude::HtmlInclude);
      break;
    case CommandType::CMD_LATEXINCLUDE:
      handleInclude(cmdName,DocInclude::LatexInclude);
      break;
    case CommandType::CMD_RTFINCLUDE:
      handleInclude(cmdName,DocInclude::RtfInclude);
      break;
    case CommandType::CMD_MANINCLUDE:
      handleInclude(cmdName,DocInclude::ManInclude);
      break;
    case CommandType::CMD_XMLINCLUDE:
      handleInclude(cmdName,DocInclude::XmlInclude);
      break;
    case CommandType::CMD_DOCBOOKINCLUDE:
      handleInclude(cmdName,DocInclude::DocbookInclude);
      break;
    case CommandType::CMD_VERBINCLUDE:
      handleInclude(cmdName,DocInclude::VerbInclude);
      break;
    case CommandType::CMD_SNIPPET:
      handleInclude(cmdName,DocInclude::Snippet);
      break;
    case CommandType::CMD_SNIPWITHLINES:
      handleInclude(cmdName,DocInclude::SnippetWithLines);
      break;
    case CommandType::CMD_SKIP:
      handleIncludeOperator(cmdName,DocIncOperator::Skip);
      break;
    case CommandType::CMD_UNTIL:
      handleIncludeOperator(cmdName,DocIncOperator::Until);
      break;
    case CommandType::CMD_SKIPLINE:
      handleIncludeOperator(cmdName,DocIncOperator::SkipLine);
      break;
    case CommandType::CMD_LINE:
      handleIncludeOperator(cmdName,DocIncOperator::Line);
      break;
    case CommandType::CMD_IMAGE:
      parser()->handleImage(thisVariant(),children());
      break;
    case CommandType::CMD_DOTFILE:
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
    case CommandType::CMD_VHDLFLOW:
      handleVhdlFlow();
      break;
    case CommandType::CMD_MSCFILE:
      handleFile<DocMscFile>(cmdName);
      break;
    case CommandType::CMD_DIAFILE:
      handleFile<DocDiaFile>(cmdName);
      break;
    case CommandType::CMD_PLANTUMLFILE:
      handleFile<DocPlantUmlFile>(cmdName);
      break;
    case CommandType::CMD_LINK:
      handleLink(cmdName,FALSE);
      break;
    case CommandType::CMD_JAVALINK:
      handleLink(cmdName,TRUE);
      break;
    case CommandType::CMD_CITE:
      handleCite(cmdChar,cmdName);
      break;
    case CommandType::CMD_EMOJI:
      handleEmoji(cmdChar,cmdName);
      break;
    case CommandType::CMD_DOXYCONFIG:
      handleDoxyConfig(cmdChar,cmdName);
      break;
    case CommandType::CMD_REF: // fall through
    case CommandType::CMD_SUBPAGE:
      handleRef(cmdChar,cmdName);
      break;
    case CommandType::CMD_SECREFLIST:
      {
        children().append<DocSecRefList>(parser(),thisVariant());
        children().get_last<DocSecRefList>()->parse();
      }
      break;
    case CommandType::CMD_SECREFITEM:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected command '%c%s'",cmdChar,qPrint(parser()->context.token->name));
      break;
    case CommandType::CMD_ENDSECREFLIST:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected command '%c%s'",cmdChar,qPrint(parser()->context.token->name));
      break;
    case CommandType::CMD_FORMULA:
      {
        children().append<DocFormula>(parser(),thisVariant(),parser()->context.token->id);
      }
      break;
    //case CommandType::CMD_LANGSWITCH:
    //  retval = handleLanguageSwitch();
    //  break;
    case CommandType::CMD_INTERNALREF:
      //warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"unexpected command %s",qPrint(parser()->context.token->name));
      {
        parser()->handleInternalRef(thisVariant(),children());
        parser()->tokenizer.setStatePara();
      }
      break;
    case CommandType::CMD_INHERITDOC:
      handleInheritDoc();
      break;
    case CommandType::CMD_SHOWDATE:
      handleShowDate(cmdChar,cmdName);
      break;
    case CommandType::CMD_ILINE:
      handleILine(cmdChar,cmdName);
      break;
    case CommandType::CMD_IFILE:
      handleIFile(cmdChar,cmdName);
      break;
    case CommandType::CMD_SETSCOPE:
      {
        parser()->tokenizer.setStateSetScope();
        (void)parser()->tokenizer.lex();
        parser()->context.context = parser()->context.token->name;
        //printf("Found scope='%s'\n",qPrint(parser()->context.context));
        parser()->tokenizer.setStatePara();
      }
      break;
    default:
      // we should not get here!
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected command '%s' in paragraph context",qPrint(cmdName));
      break;
  }
  INTERNAL_ASSERT(retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF,TokenRetval::RetVal_OK,TokenRetval::RetVal_SimpleSec
         TokenRetval::TK_LISTITEM,TokenRetval::TK_ENDLIST,TokenRetval::TK_NEWPARA
         TokenRetval::RetVal_Section,TokenRetval::RetVal_EndList
         TokenRetval::RetVal_Internal,TokenRetval::RetVal_SwitchLang
         TokenRetval::RetVal_EndInternal)
        );
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
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

Token DocPara::handleHtmlStartTag(const QCString &tagName,const HtmlAttribList &tagHtmlAttribs)
{
  AUTO_TRACE("tagName={} #tagHtmlAttrs={}",tagName,tagHtmlAttribs.size());
  Token retval = Token::make_RetVal_OK();
  HtmlTagType tagId = Mappers::htmlTagMapper->map(tagName);
  if (parser()->context.token->emptyTag && !(tagId>HtmlTagType::XML_CmdMask) &&
      tagId!=HtmlTagType::UNKNOWN && tagId!=HtmlTagType::HTML_IMG && tagId!=HtmlTagType::HTML_BR && tagId!=HtmlTagType::HTML_HR && tagId!=HtmlTagType::HTML_P
      && tagId!=HtmlTagType::HTML_DIV && tagId!=HtmlTagType::HTML_SPAN)
  {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"HTML tag ('<%s/>') may not use the 'empty tag' XHTML syntax.",
                     qPrint(tagName));
  }
  switch (tagId)
  {
    case HtmlTagType::HTML_UL:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlList>(parser(),thisVariant(),
                                       tagHtmlAttribs,DocHtmlList::Unordered);
        retval=children().get_last<DocHtmlList>()->parse();
      }
      break;
    case HtmlTagType::HTML_OL:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlList>(parser(),thisVariant(),
                                       tagHtmlAttribs,DocHtmlList::Ordered);
        retval=children().get_last<DocHtmlList>()->parse();
      }
      break;
    case HtmlTagType::HTML_LI:
      if (parser()->context.token->emptyTag) break;
      if (!insideUL(thisVariant()) && !insideOL(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"lonely <li> tag found");
      }
      else
      {
        retval = Token::make_RetVal_ListItem();
      }
      break;
    case HtmlTagType::HTML_BOLD:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Bold,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_S:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::S,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_STRIKE:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Strike,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_DEL:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Del,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_UNDERLINE:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Underline,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_INS:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Ins,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_CODE:
      if (parser()->context.token->emptyTag) break;
      if (parser()->context.xmlComment)
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
    case HtmlTagType::HTML_EMPHASIS:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Italic,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_DIV:
      parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Div,tagName,&parser()->context.token->attribs);
      if (parser()->context.token->emptyTag) parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Div,tagName);
      break;
    case HtmlTagType::HTML_SPAN:
      parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Span,tagName,&parser()->context.token->attribs);
      if (parser()->context.token->emptyTag) parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Span,tagName);
      break;
    case HtmlTagType::HTML_SUB:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Subscript,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_SUP:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Superscript,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_CENTER:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Center,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_SMALL:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Small,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_CITE:
      if (!parser()->context.token->emptyTag) parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Cite,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::HTML_PRE:
      if (parser()->context.token->emptyTag) break;
      parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Preformatted,tagName,&parser()->context.token->attribs);
      setInsidePreformatted(TRUE);
      parser()->tokenizer.setInsidePre(TRUE);
      break;
    case HtmlTagType::HTML_P:
      retval = Token::make_TK_NEWPARA();
      break;
    case HtmlTagType::HTML_DL:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlDescList>(parser(),thisVariant(),tagHtmlAttribs);
        retval=children().get_last<DocHtmlDescList>()->parse();
      }
      break;
    case HtmlTagType::HTML_DT:
      if (insideDL(thisVariant()))
      {
        retval = Token::make_RetVal_DescTitle();
      }
      else
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag <dt> found");
      }
      break;
    case HtmlTagType::HTML_DD:
      if (insideDL(thisVariant()))
      {
        retval = Token::make_RetVal_DescData();
      }
      else
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag <dd> found");
      }
      break;
    case HtmlTagType::HTML_TABLE:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlTable>(parser(),thisVariant(),tagHtmlAttribs);
        retval=children().get_last<DocHtmlTable>()->parse();
      }
      break;
    case HtmlTagType::HTML_TR:
      retval = Token::make_RetVal_TableRow();
      break;
    case HtmlTagType::HTML_TD:
      retval = Token::make_RetVal_TableCell();
      break;
    case HtmlTagType::HTML_TH:
      retval = Token::make_RetVal_TableHCell();
      break;
    case HtmlTagType::HTML_THEAD:
    case HtmlTagType::HTML_TBODY:
    case HtmlTagType::HTML_TFOOT:
      // for time being ignore </t....> tag
      break;
    case HtmlTagType::HTML_CAPTION:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag <caption> found");
      break;
    case HtmlTagType::HTML_BR:
      {
        children().append<DocLineBreak>(parser(),thisVariant(),tagHtmlAttribs);
      }
      break;
    case HtmlTagType::HTML_HR:
      {
        children().append<DocHorRuler>(parser(),thisVariant(),tagHtmlAttribs);
      }
      break;
    case HtmlTagType::HTML_A:
      retval = parser()->handleAHref(thisVariant(),children(),tagHtmlAttribs);
      break;
    case HtmlTagType::HTML_H1:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,1);
      break;
    case HtmlTagType::HTML_H2:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,2);
      break;
    case HtmlTagType::HTML_H3:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,3);
      break;
    case HtmlTagType::HTML_H4:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,4);
      break;
    case HtmlTagType::HTML_H5:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,5);
      break;
    case HtmlTagType::HTML_H6:
      if (!parser()->context.token->emptyTag) retval=handleHtmlHeader(tagHtmlAttribs,6);
      break;
    case HtmlTagType::HTML_IMG:
      {
        parser()->handleImg(thisVariant(),children(),tagHtmlAttribs);
      }
      break;
    case HtmlTagType::HTML_DETAILS:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlDetails>(parser(),thisVariant(),tagHtmlAttribs);
        retval=children().get_last<DocHtmlDetails>()->parse();
      }
      break;
    case HtmlTagType::HTML_BLOCKQUOTE:
      if (!parser()->context.token->emptyTag)
      {
        children().append<DocHtmlBlockQuote>(parser(),thisVariant(),tagHtmlAttribs);
        retval = children().get_last<DocHtmlBlockQuote>()->parse();
      }
      break;

    case HtmlTagType::XML_SUMMARY:
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
              retval = Token::make_TK_NEWPARA();
            }
          }
        }
      }
      break;
    case HtmlTagType::XML_REMARKS:
    case HtmlTagType::XML_EXAMPLE:
      parser()->context.xmlComment=TRUE;
      // fall through
    case HtmlTagType::XML_VALUE:
    case HtmlTagType::XML_PARA:
      if (!children().empty())
      {
        retval = Token::make_TK_NEWPARA();
      }
      break;
    case HtmlTagType::XML_DESCRIPTION:
      if (insideTable(thisVariant()))
      {
        retval = Token::make_RetVal_TableCell();
      }
      break;
    case HtmlTagType::XML_C:
      parser()->handleStyleEnter(thisVariant(),children(),DocStyleChange::Code,tagName,&parser()->context.token->attribs);
      break;
    case HtmlTagType::XML_PARAM:
    case HtmlTagType::XML_TYPEPARAM:
      {
        parser()->context.xmlComment=TRUE;
        QCString paramName;
        if (findAttribute(tagHtmlAttribs,"name",&paramName))
        {
          if (paramName.isEmpty())
          {
            if (Config_getBool(WARN_NO_PARAMDOC))
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"empty 'name' attribute for <param%s> tag.",tagId==HtmlTagType::XML_PARAM?"":"type");
            }
          }
          else
          {
            retval = handleParamSection(paramName,
                tagId==HtmlTagType::XML_PARAM ? DocParamSect::Param : DocParamSect::TemplateParam,
                TRUE);
          }
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing 'name' attribute from <param%s> tag.",tagId==HtmlTagType::XML_PARAM?"":"type");
        }
      }
      break;
    case HtmlTagType::XML_PARAMREF:
    case HtmlTagType::XML_TYPEPARAMREF:
      {
        QCString paramName;
        if (findAttribute(tagHtmlAttribs,"name",&paramName))
        {
          //printf("paramName=%s\n",qPrint(paramName));
          children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Italic,tagName,TRUE);
          children().append<DocWord>(parser(),thisVariant(),paramName);
          children().append<DocStyleChange>(parser(),thisVariant(),parser()->context.nodeStack.size(),DocStyleChange::Italic,tagName,FALSE);
          if (!retval.is(TokenRetval::TK_WORD)) children().append<DocWhiteSpace>(parser(),thisVariant()," ");
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing 'name' attribute from <param%sref> tag.",tagId==HtmlTagType::XML_PARAMREF?"":"type");
        }
      }
      break;
    case HtmlTagType::XML_EXCEPTION:
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
    case HtmlTagType::XML_ITEM:
    case HtmlTagType::XML_LISTHEADER:
      if (insideTable(thisVariant()))
      {
        retval = Token::make_RetVal_TableRow();
      }
      else if (insideUL(thisVariant()) || insideOL(thisVariant()))
      {
        retval = Token::make_RetVal_ListItem();
      }
      else
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"lonely <item> tag found");
      }
      break;
    case HtmlTagType::XML_RETURNS:
      parser()->context.xmlComment=TRUE;
      retval = handleSimpleSection(DocSimpleSect::Return,TRUE);
      parser()->context.hasReturnCommand=TRUE;
      break;
    case HtmlTagType::XML_TERM:
      if (insideTable(thisVariant()))
      {
        retval = Token::make_RetVal_TableCell();
      }
      break;
    case HtmlTagType::XML_SEE:
      // I'm not sure if <see> is the same as <seealso> or if it
      // should you link a member without producing a section. The
      // C# specification is extremely vague about this (but what else
      // can we expect from Microsoft...)
      {
        QCString cref;
        //printf("HtmlTagType::XML_SEE: empty tag=%d\n",parser()->context.token->emptyTag);
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
    case HtmlTagType::XML_SEEALSO:
      {
        parser()->context.xmlComment=TRUE;
        QCString cref;
        if (findAttribute(tagHtmlAttribs,"cref",&cref))
        {
          unescapeCRef(cref);
          // Look for an existing "see" section
          DocNodeVariant *vss=nullptr;
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
          retval = Token::make_RetVal_OK();
        }
        else
        {
          warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing 'cref' attribute from <seealso> tag.");
        }
      }
      break;
    case HtmlTagType::XML_LIST:
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
    case HtmlTagType::XML_INCLUDE:
    case HtmlTagType::XML_PERMISSION:
      // These tags are defined in .Net but are currently unsupported
      parser()->context.xmlComment=TRUE;
      break;
    case HtmlTagType::UNKNOWN:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported xml/html tag <%s> found", qPrint(tagName));
      children().append<DocWord>(parser(),thisVariant(), "<"+tagName+parser()->context.token->attribsStr+">");
      break;
  case HtmlTagType::XML_INHERITDOC:
      handleInheritDoc();
      break;
  default:
      // we should not get here!
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected start tag %s",qPrint(tagName));
      ASSERT(0);
      break;
  }
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

Token DocPara::handleHtmlEndTag(const QCString &tagName)
{
  AUTO_TRACE("tagName={}",tagName);
  HtmlTagType tagId = Mappers::htmlTagMapper->map(tagName);
  Token retval = Token::make_RetVal_OK();
  switch (tagId)
  {
    case HtmlTagType::HTML_UL:
      if (!insideUL(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"found </ul> tag without matching <ul>");
      }
      else
      {
        retval = Token::make_RetVal_EndList();
      }
      break;
    case HtmlTagType::HTML_OL:
      if (!insideOL(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"found </ol> tag without matching <ol>");
      }
      else
      {
        retval = Token::make_RetVal_EndList();
      }
      break;
    case HtmlTagType::HTML_LI:
      if (!insideLI(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"found </li> tag without matching <li>");
      }
      else
      {
        // ignore </li> tags
      }
      break;
    case HtmlTagType::HTML_DETAILS:
      if (!insideDetails(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"found </details> tag without matching <details>");
      }
      else
      {
        retval = Token::make_RetVal_EndHtmlDetails();
      }
      break;
    case HtmlTagType::HTML_BLOCKQUOTE:
      if (!insideBlockQuote(thisVariant()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"found </blockquote> tag without matching <blockquote>");
      }
      else
      {
        retval = Token::make_RetVal_EndBlockQuote();
      }
      break;
    case HtmlTagType::HTML_BOLD:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Bold,tagName);
      break;
    case HtmlTagType::HTML_S:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::S,"s");
      break;
    case HtmlTagType::HTML_STRIKE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Strike,tagName);
      break;
    case HtmlTagType::HTML_DEL:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Del,tagName);
      break;
    case HtmlTagType::HTML_UNDERLINE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Underline,tagName);
      break;
    case HtmlTagType::HTML_INS:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Ins,tagName);
      break;
    case HtmlTagType::HTML_CODE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Code,tagName);
      break;
    case HtmlTagType::HTML_EMPHASIS:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Italic,tagName);
      break;
    case HtmlTagType::HTML_DIV:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Div,tagName);
      break;
    case HtmlTagType::HTML_SPAN:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Span,tagName);
      break;
    case HtmlTagType::HTML_SUB:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Subscript,tagName);
      break;
    case HtmlTagType::HTML_SUP:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Superscript,tagName);
      break;
    case HtmlTagType::HTML_CENTER:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Center,tagName);
      break;
    case HtmlTagType::HTML_SMALL:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Small,tagName);
      break;
    case HtmlTagType::HTML_CITE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Cite,tagName);
      break;
    case HtmlTagType::HTML_PRE:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Preformatted,tagName);
      setInsidePreformatted(FALSE);
      parser()->tokenizer.setInsidePre(FALSE);
      break;
    case HtmlTagType::HTML_P:
      retval = Token::make_TK_NEWPARA();
      break;
    case HtmlTagType::HTML_DL:
      retval = Token::make_RetVal_EndDesc();
      break;
    case HtmlTagType::HTML_DT:
      // ignore </dt> tag
      break;
    case HtmlTagType::HTML_DD:
      // ignore </dd> tag
      break;
    case HtmlTagType::HTML_TABLE:
      retval = Token::make_RetVal_EndTable();
      break;
    case HtmlTagType::HTML_TR:
      // ignore </tr> tag
      break;
    case HtmlTagType::HTML_TD:
      // ignore </td> tag
      break;
    case HtmlTagType::HTML_TH:
      // ignore </th> tag
      break;
    case HtmlTagType::HTML_THEAD:
    case HtmlTagType::HTML_TBODY:
    case HtmlTagType::HTML_TFOOT:
      // for time being ignore </t....> tag
      break;
    case HtmlTagType::HTML_CAPTION:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </caption> found");
      break;
    case HtmlTagType::HTML_BR:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal </br> tag found");
      break;
    case HtmlTagType::HTML_H1:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h1> found");
      break;
    case HtmlTagType::HTML_H2:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h2> found");
      break;
    case HtmlTagType::HTML_H3:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h3> found");
      break;
    case HtmlTagType::HTML_H4:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h4> found");
      break;
    case HtmlTagType::HTML_H5:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h5> found");
      break;
    case HtmlTagType::HTML_H6:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </h6> found");
      break;
    case HtmlTagType::HTML_IMG:
      break;
    case HtmlTagType::HTML_HR:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Illegal </hr> tag found");
      break;
    case HtmlTagType::HTML_A:
      //warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected tag </a> found");
      // ignore </a> tag (can be part of <a name=...></a>
      break;

    case HtmlTagType::XML_TERM:
      break;
    case HtmlTagType::XML_SUMMARY:
      retval = Token::make_TK_NEWPARA();
      break;
    case HtmlTagType::XML_REMARKS:
    case HtmlTagType::XML_PARA:
    case HtmlTagType::XML_VALUE:
    case HtmlTagType::XML_EXAMPLE:
    case HtmlTagType::XML_PARAM:
    case HtmlTagType::XML_LIST:
    case HtmlTagType::XML_TYPEPARAM:
    case HtmlTagType::XML_RETURNS:
    case HtmlTagType::XML_SEE:
    case HtmlTagType::XML_SEEALSO:
    case HtmlTagType::XML_EXCEPTION:
    case HtmlTagType::XML_INHERITDOC:
      retval = Token::make_RetVal_CloseXml();
      break;
    case HtmlTagType::XML_C:
      parser()->handleStyleLeave(thisVariant(),children(),DocStyleChange::Code,tagName);
      break;
    case HtmlTagType::XML_ITEM:
    case HtmlTagType::XML_LISTHEADER:
    case HtmlTagType::XML_INCLUDE:
    case HtmlTagType::XML_PERMISSION:
    case HtmlTagType::XML_DESCRIPTION:
    case HtmlTagType::XML_PARAMREF:
    case HtmlTagType::XML_TYPEPARAMREF:
      // These tags are defined in .Net but are currently unsupported
      break;
    case HtmlTagType::UNKNOWN:
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported xml/html tag </%s> found", qPrint(tagName));
      children().append<DocWord>(parser(),thisVariant(),"</"+tagName+">");
      break;
    default:
      // we should not get here!
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unexpected end tag %s",qPrint(tagName));
      ASSERT(0);
      break;
  }
  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

static bool checkIfHtmlEndTagEndsAutoList(DocParser *parser,const DocNodeVariant *n)
{
  // expected hierarchy:
  // 1.    DocAutoListItem <- n
  // 2.  DocAutoList       <- parent(n)
  // 3. DocPara            <- parent(parent(n))

  // step 1
  if (!std::get_if<DocAutoListItem>(n)) // not inside a auto list item
  {
    return false;
  }

  // step 2
  n = parent(n);
  int indent = 0;
  const auto docAutoList = std::get_if<DocAutoList>(n);
  if (docAutoList) // capture indent
  {
    indent = docAutoList->indent();
  }
  else
  {
    return false;
  }

  // step 3
  n = parent(n);
  const auto docPara = std::get_if<DocPara>(n);
  if (docPara)
  {
    QCString tagNameLower = QCString(parser->context.token->name).lower();
    auto topStyleChange = [](const DocStyleChangeStack &stack) -> const DocStyleChange &
    {
      return std::get<DocStyleChange>(*stack.top());
    };

    if (parser->context.styleStack.empty() ||                                                     // no style change
        (topStyleChange(parser->context.styleStack).tagName()==tagNameLower &&                    // correct style change
         topStyleChange(parser->context.styleStack).position()!=parser->context.nodeStack.size()) // wrong position, so normal close
       )
    {
      // insert an artificial 'end of autolist' marker and parse again
      QCString indentStr;
      indentStr.fill(' ',indent);
      parser->tokenizer.unputString("\\ilinebr "+indentStr+".\\ilinebr"+indentStr+"</"+parser->context.token->name+">");
      return true;
    }
  }
  return false;
}

Token DocPara::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  // handle style commands "inherited" from the previous paragraph
  parser()->handleInitialStyleCommands(thisVariant(),children());
  Token tok=parser()->tokenizer.lex();
  Token retval = Token::make_TK_NONE();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF)) // get the next token
  {
reparsetoken:
    AUTO_TRACE_ADD("token '{}' at {}",tok.to_string(),parser()->tokenizer.getLineNr());
    if (tok.is_any_of(TokenRetval::TK_WORD,TokenRetval::TK_LNKWORD,TokenRetval::TK_SYMBOL,TokenRetval::TK_URL,
                      TokenRetval::TK_COMMAND_AT,TokenRetval::TK_COMMAND_BS,TokenRetval::TK_HTMLTAG)
       )
    {
      AUTO_TRACE_ADD("name={}",parser()->context.token->name);
    }
    switch(tok.value())
    {
      case TokenRetval::TK_WORD:
        children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
        break;
      case TokenRetval::TK_LNKWORD:
        parser()->handleLinkedWord(thisVariant(),children());
        break;
      case TokenRetval::TK_URL:
        children().append<DocURL>(parser(),thisVariant(),parser()->context.token->name,parser()->context.token->isEMailAddr);
        break;
      case TokenRetval::TK_WHITESPACE:
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
      case TokenRetval::TK_LISTITEM:
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
              retval = Token::make_TK_LISTITEM();
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
                                           parser()->context.token->isEnumList,depth,
                                           parser()->context.token->isCheckedList);
            al = children().get_last<DocAutoList>();
            retval = children().get_last<DocAutoList>()->parse();
          } while (retval.is(TokenRetval::TK_LISTITEM) &&                   // new list
              al->indent()==parser()->context.token->indent  // at same indent level
              );

          // check the return value
          if (retval.is(TokenRetval::RetVal_SimpleSec)) // auto list ended due to simple section command
          {
            // Reparse the token that ended the section at this level,
            // so a new simple section will be started at this level.
            // This is the same as unputting the last read token and continuing.
            parser()->context.token->name = parser()->context.token->simpleSectName;
            if (parser()->context.token->name.startsWith("rcs:")) // RCS section
            {
              parser()->context.token->name = parser()->context.token->name.mid(4);
              parser()->context.token->text = parser()->context.token->simpleSectText;
              tok = Token::make_TK_RCSTAG();
            }
            else // other section
            {
              tok = Token::make_TK_COMMAND_BS();
            }
            AUTO_TRACE_ADD("reparsing command {}",parser()->context.token->name);
            goto reparsetoken;
          }
          else if (retval.is(TokenRetval::TK_ENDLIST))
          {
            if (al->indent()>parser()->context.token->indent) // end list
            {
              goto endparagraph;
            }
            else // continue with current paragraph
            {
            }
          }
          else // paragraph ended due to TokenRetval::TK_NEWPARA, TokenRetval::TK_LISTITEM, or EOF
          {
            goto endparagraph;
          }
        }
        break;
      case TokenRetval::TK_ENDLIST:
        AUTO_TRACE_ADD("Found end of list inside of paragraph at line {}",parser()->tokenizer.getLineNr());
        if (std::get_if<DocAutoListItem>(parent()))
        {
          const DocAutoList *al = std::get_if<DocAutoList>(::parent(parent()));
          if (al && al->indent()>=parser()->context.token->indent)
          {
            // end of list marker ends this paragraph
            retval = Token::make_TK_ENDLIST();
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
      case TokenRetval::TK_COMMAND_AT:
        // fall through
      case TokenRetval::TK_COMMAND_BS:
        {
          // see if we have to start a simple section
          CommandType cmd = Mappers::cmdMapper->map(parser()->context.token->name);
          const DocNodeVariant *n=parent();
          while (n && !std::holds_alternative<DocSimpleSect>(*n) &&
                      !std::holds_alternative<DocParamSect>(*n))
          {
            n=::parent(n);
          }
          if (cmd>CommandType::SIMPLESECT_BIT)
          {
            if (n)  // already in a simple section
            {
              // simple section cannot start in this paragraph, need
              // to unwind the stack and remember the command.
              parser()->context.token->simpleSectName = parser()->context.token->name;
              retval = Token::make_RetVal_SimpleSec();
              goto endparagraph;
            }
          }
          // see if we are in a simple list
          n=parent();
          while (n && !std::holds_alternative<DocSimpleListItem>(*n)) n=::parent(n);
          if (n)
          {
            if (cmd==CommandType::CMD_LI)
            {
              retval = Token::make_RetVal_ListItem();
              goto endparagraph;
            }
          }

          // handle the command
          retval=handleCommand(tok.command_to_char(),parser()->context.token->name);
          AUTO_TRACE_ADD("handleCommand returns {}",retval.to_string());

          // check the return value
          if (retval.is(TokenRetval::RetVal_SimpleSec))
          {
            // Reparse the token that ended the section at this level,
            // so a new simple section will be started at this level.
            // This is the same as unputting the last read token and continuing.
            parser()->context.token->name = parser()->context.token->simpleSectName;
            if (parser()->context.token->name.startsWith("rcs:")) // RCS section
            {
              parser()->context.token->name = parser()->context.token->name.mid(4);
              parser()->context.token->text = parser()->context.token->simpleSectText;
              tok = Token::make_TK_RCSTAG();
            }
            else // other section
            {
              tok = Token::make_TK_COMMAND_BS();
            }
            AUTO_TRACE_ADD("reparsing command {}",parser()->context.token->name);
            goto reparsetoken;
          }
          else if (retval.value()>TokenRetval::TK_NONE && retval.value()<TokenRetval::RetVal_OK)
          {
            // the command ended with a new command, reparse this token
            tok = retval;
            goto reparsetoken;
          }
          else if (retval.value()!=TokenRetval::RetVal_OK) // end of file, end of paragraph, start or end of section
                                        // or some auto list marker
          {
            goto endparagraph;
          }
        }
        break;
      case TokenRetval::TK_HTMLTAG:
        {
          if (!parser()->context.token->endTag) // found a start tag
          {
            retval = handleHtmlStartTag(parser()->context.token->name,parser()->context.token->attribs);
          }
          else // found an end tag
          {
            if (checkIfHtmlEndTagEndsAutoList(parser(),parent()))
            {
              break; // new code has been pushed back to the scanner, need to reparse
            }
            retval = handleHtmlEndTag(parser()->context.token->name);
          }
          if (!retval.is(TokenRetval::RetVal_OK))
          {
            goto endparagraph;
          }
        }
        break;
      case TokenRetval::TK_SYMBOL:
        {
          HtmlEntityMapper::SymType s = DocSymbol::decodeSymbol(parser()->context.token->name);
          if (s!=HtmlEntityMapper::Sym_Unknown)
          {
            children().append<DocSymbol>(parser(),thisVariant(),s);
          }
          else
          {
            children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported symbol '%s' found",
                qPrint(parser()->context.token->name));
          }
          break;
        }
      case TokenRetval::TK_NEWPARA:
        retval = Token::make_TK_NEWPARA();
        goto endparagraph;
      case TokenRetval::TK_RCSTAG:
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
            retval = Token::make_RetVal_SimpleSec();
            goto endparagraph;
          }

          // see if we are in a simple list
          children().append<DocSimpleSect>(parser(),thisVariant(),DocSimpleSect::Rcs);
          children().get_last<DocSimpleSect>()->parseRcs();
        }
        break;
      default:
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
            "Found unexpected token (id=%s)",tok.to_string());
        break;
    }
    tok=parser()->tokenizer.lex();
  }
  retval=Token::make_TK_NONE();
endparagraph:
  parser()->handlePendingStyleCommands(thisVariant(),children());
  DocPara *par = std::get_if<DocPara>(parser()->context.nodeStack.top());
  if (!parser()->context.token->endTag && par &&
      retval.is(TokenRetval::TK_NEWPARA) && parser()->context.token->name.lower() == "p")
  {
    par->setAttribs(parser()->context.token->attribs);
  }
  INTERNAL_ASSERT(retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF,TokenRetval::TK_NEWPARA,TokenRetval::TK_LISTITEM,
                                   TokenRetval::TK_ENDLIST,TokenRetval::RetVal_OK)
	);

  AUTO_TRACE_EXIT("retval={}",retval.to_string());
  return retval;
}

//--------------------------------------------------------------------------

Token DocSection::parse()
{
  AUTO_TRACE("start {} level={}", parser()->context.token->sectionId, m_level);
  Token retval = Token::make_RetVal_OK();
  auto ns = AutoNodeStack(parser(),thisVariant());

  if (!m_id.isEmpty())
  {
    const SectionInfo *sec = SectionManager::instance().find(m_id);
    if (sec)
    {
      m_file   = sec->fileName();
      m_anchor = sec->label();
      QCString titleStr = sec->title();
      if (titleStr.isEmpty()) titleStr = sec->label();
      m_title = createDocNode<DocTitle>(parser(),thisVariant());
      DocTitle *title = &std::get<DocTitle>(*m_title);
      title->parseFromString(thisVariant(),titleStr);
    }
  }

  // first parse any number of paragraphs
  bool isFirst=TRUE;
  DocPara *lastPar=nullptr;
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
    if (retval.is(TokenRetval::TK_LISTITEM))
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid list item found");
    }
    if (retval.is(TokenRetval::RetVal_Internal))
    {
      children().append<DocInternal>(parser(),thisVariant());
      retval = children().get_last<DocInternal>()->parse(m_level+1);
      if (retval.is(TokenRetval::RetVal_EndInternal))
      {
        retval = Token::make_RetVal_OK();
      }
    }
  } while (!retval.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF, TokenRetval::RetVal_Section, TokenRetval::RetVal_Subsection,
                             TokenRetval::RetVal_Subsubsection, TokenRetval::RetVal_Paragraph, TokenRetval::RetVal_SubParagraph,
                             TokenRetval::RetVal_SubSubParagraph, TokenRetval::RetVal_EndInternal)
          );

  if (lastPar) lastPar->markLast();

  while (true)
  {
    if (retval.is(TokenRetval::RetVal_Subsection) && m_level<=1)
    {
      // then parse any number of nested sections
      while (retval.is(TokenRetval::RetVal_Subsection)) // more sections follow
      {
        children().append<DocSection>(parser(),thisVariant(),
                                2,
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
      }
      break;
    }
    else if (retval.is(TokenRetval::RetVal_Subsubsection) && m_level<=2)
    {
      if ((m_level <= 1) &&
          !AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
      {
        warn_doc_error(parser()->context.fileName,
                       parser()->tokenizer.getLineNr(),
                       "Unexpected subsubsection command found inside %s!",
                       g_sectionLevelToName[m_level]);
      }
      // then parse any number of nested sections
      while (retval.is(TokenRetval::RetVal_Subsubsection)) // more sections follow
      {
        children().append<DocSection>(parser(),thisVariant(),
                                3,
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
      }
      if (!(m_level < 2 && retval.is(TokenRetval::RetVal_Subsection))) break;
    }
    else if (retval.is(TokenRetval::RetVal_Paragraph) && m_level<=3)
    {
      if ((m_level <= 2) &&
          !AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
                       "Unexpected paragraph command found inside %s!",
                       g_sectionLevelToName[m_level]);
      }
      // then parse any number of nested sections
      while (retval.is(TokenRetval::RetVal_Paragraph)) // more sections follow
      {
        children().append<DocSection>(parser(),thisVariant(),
                                4,
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
      }
      if (!(m_level<3 && (retval.is_any_of(TokenRetval::RetVal_Subsection,TokenRetval::RetVal_Subsubsection)))) break;
    }
    else if (retval.is(TokenRetval::RetVal_SubParagraph) && m_level<=4)
    {
      if ((m_level <= 3) &&
          !AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
                       "Unexpected subparagraph command found inside %s!",
                       g_sectionLevelToName[m_level]);
      }
      // then parse any number of nested sections
      while (retval.is(TokenRetval::RetVal_SubParagraph)) // more sections follow
      {
        children().append<DocSection>(parser(),thisVariant(),
                                5,
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
      }
      if (!(m_level<4 && (retval.is_any_of(TokenRetval::RetVal_Subsection,TokenRetval::RetVal_Subsubsection,TokenRetval::RetVal_Paragraph)))) break;
    }
    else if (retval.is(TokenRetval::RetVal_SubSubParagraph) && m_level<=5)
    {
      if ((m_level <= 4) &&
          !AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),
                       "Unexpected subsubparagraph command found inside %s!",
                       g_sectionLevelToName[m_level]);
      }
      // then parse any number of nested sections
      while (retval.is(TokenRetval::RetVal_SubSubParagraph)) // more sections follow
      {
        children().append<DocSection>(parser(),thisVariant(),
                                6,
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
      }
      if (!(m_level<5 && (retval.is_any_of( TokenRetval::RetVal_Subsection, TokenRetval::RetVal_Subsubsection,
                                            TokenRetval::RetVal_Paragraph, TokenRetval::RetVal_SubParagraph)))) break;
    }
    else
    {
      break;
    }
  }

  INTERNAL_ASSERT(retval.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF,
                                   TokenRetval::RetVal_Section, TokenRetval::RetVal_Subsection,
                                   TokenRetval::RetVal_Subsubsection, TokenRetval::RetVal_Paragraph,
                                   TokenRetval::RetVal_SubParagraph, TokenRetval::RetVal_SubSubParagraph,
                                   TokenRetval::RetVal_Internal, TokenRetval::RetVal_EndInternal)
                 );

  AUTO_TRACE_EXIT("retval={}", retval.to_string());
  return retval;
}

//--------------------------------------------------------------------------

void DocText::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  parser()->tokenizer.setStateText();

  Token tok = parser()->tokenizer.lex();
  while (!tok.is_any_of(TokenRetval::TK_NONE, TokenRetval::TK_EOF)) // get the next token
  {
    switch(tok.value())
    {
      case TokenRetval::TK_WORD:
	children().append<DocWord>(parser(),thisVariant(),parser()->context.token->name);
	break;
      case TokenRetval::TK_WHITESPACE:
        children().append<DocWhiteSpace>(parser(),thisVariant(),parser()->context.token->chars);
	break;
      case TokenRetval::TK_SYMBOL:
        {
          HtmlEntityMapper::SymType s = DocSymbol::decodeSymbol(parser()->context.token->name);
          if (s!=HtmlEntityMapper::Sym_Unknown)
          {
            children().append<DocSymbol>(parser(),thisVariant(),s);
          }
          else
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Unsupported symbol '%s' found",
                qPrint(parser()->context.token->name));
          }
        }
        break;
      case TokenRetval::TK_COMMAND_AT:
        // fall through
      case TokenRetval::TK_COMMAND_BS:
        switch (Mappers::cmdMapper->map(parser()->context.token->name))
        {
          case CommandType::CMD_BSLASH:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_BSlash);
            break;
          case CommandType::CMD_AT:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_At);
            break;
          case CommandType::CMD_LESS:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Less);
            break;
          case CommandType::CMD_GREATER:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Greater);
            break;
          case CommandType::CMD_AMP:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Amp);
            break;
          case CommandType::CMD_DOLLAR:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Dollar);
            break;
          case CommandType::CMD_HASH:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Hash);
            break;
          case CommandType::CMD_DCOLON:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_DoubleColon);
            break;
          case CommandType::CMD_PERCENT:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Percent);
            break;
          case CommandType::CMD_NDASH:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            break;
          case CommandType::CMD_MDASH:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            break;
          case CommandType::CMD_QUOTE:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Quot);
            break;
          case CommandType::CMD_PUNT:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Dot);
            break;
          case CommandType::CMD_PLUS:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Plus);
            break;
          case CommandType::CMD_MINUS:
            children().append<DocSymbol>(parser(),thisVariant(),HtmlEntityMapper::Sym_Minus);
            break;
          case CommandType::CMD_EQUAL:
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
            tok.to_string());
        break;
    }
    tok = parser()->tokenizer.lex();
  }

  parser()->handleUnclosedStyleCommands();

}


//--------------------------------------------------------------------------

void DocRoot::parse()
{
  AUTO_TRACE();
  auto ns = AutoNodeStack(parser(),thisVariant());
  parser()->tokenizer.setStatePara();
  Token retval = Token::make_TK_NONE();

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
    auto checkParagraph = [this,&retval](Token t,int level,const char *sectionType,const char *parentSectionType) {
      if (retval == t)
      {
        if (!AnchorGenerator::instance().isGenerated(parser()->context.token->sectionId.str()))
        {
          warn_doc_error(parser()->context.fileName,
              parser()->tokenizer.getLineNr(),
              "found %s command (id: '%s') outside of %s context!",
              sectionType,qPrint(parser()->context.token->sectionId),parentSectionType);
        }
        while (retval==t)
        {
          if (!parser()->context.token->sectionId.isEmpty())
          {
            const SectionInfo *sec=SectionManager::instance().find(parser()->context.token->sectionId);
            if (sec)
            {
              children().append<DocSection>(parser(),thisVariant(),
                  level,
                  parser()->context.token->sectionId);
              retval = children().get_last<DocSection>()->parse();
            }
            else
            {
              warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid %s id '%s'; ignoring %s",
                  sectionType,qPrint(parser()->context.token->sectionId),sectionType);
              retval = Token::make_TK_NONE();
            }
          }
          else
          {
            warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing id for %s; ignoring %s",sectionType,sectionType);
            retval = Token::make_TK_NONE();
          }
        }
      }
    };
    checkParagraph(Token::make_RetVal_SubSubParagraph(), 6, "subsubparagraph", "subparagraph"  );
    checkParagraph(Token::make_RetVal_SubParagraph(),    5, "subparagraph",    "paragraph"     );
    checkParagraph(Token::make_RetVal_Paragraph(),       4, "paragraph",       "subsubsection" );
    checkParagraph(Token::make_RetVal_Subsubsection(),   3, "subsubsection",   "subsection"    );
    checkParagraph(Token::make_RetVal_Subsection(),      2, "subsection",      "section"       );

    if (retval.is(TokenRetval::TK_LISTITEM))
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid list item found");
    }
    if (retval.is(TokenRetval::RetVal_Internal))
    {
      children().append<DocInternal>(parser(),thisVariant());
      retval = children().get_last<DocInternal>()->parse(1);
    }
  } while (!retval.is_any_of(TokenRetval::TK_NONE,TokenRetval::TK_EOF,TokenRetval::RetVal_Section));
  if (lastPar) lastPar->markLast();

  //printf("DocRoot::parse() retval=%d %d\n",retval,TokenRetval::RetVal_Section);
  // then parse any number of level1 sections
  while (retval.is(TokenRetval::RetVal_Section))
  {
    if (!parser()->context.token->sectionId.isEmpty())
    {
      const SectionInfo *sec=SectionManager::instance().find(parser()->context.token->sectionId);
      if (sec)
      {
        children().append<DocSection>(parser(),thisVariant(),
                                1,
                                parser()->context.token->sectionId);
        retval = children().get_last<DocSection>()->parse();
      }
      else
      {
        warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Invalid section id '%s'; ignoring section",qPrint(parser()->context.token->sectionId));
        retval = Token::make_TK_NONE();
      }
    }
    else
    {
      warn_doc_error(parser()->context.fileName,parser()->tokenizer.getLineNr(),"Missing id for section; ignoring section");
      retval = Token::make_TK_NONE();
    }
  }

  parser()->handleUnclosedStyleCommands();
}
