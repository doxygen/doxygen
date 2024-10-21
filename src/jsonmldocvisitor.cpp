/******************************************************************************
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

#include "jsonmldocvisitor.h"
#include "textstream.h"

JsonMLDocVisitor::JsonMLDocVisitor(QCString &t)
      : m_insideString(false)
      , m_lvl(0)
      , m_ic(0)
      , m_t(t)
{
}
bool JsonMLDocVisitor::needsComma()
{
  if (m_lvl < 0) return false;
  if (m_ic & (1<<m_lvl)) return true;
  return false;
}

void JsonMLDocVisitor::beginScope()
{
  m_lvl++;
}

void JsonMLDocVisitor::addComma()
{
  m_ic |= 1 << m_lvl;
}

void JsonMLDocVisitor::endScope()
{
  m_lvl--;
}

bool isEmptyQuoted(QCString &tmpstr)
{
    QCString wows = tmpstr.removeWhiteSpace();
    if (wows == "\"\"" || wows == ",\"\"")
    {
      return true;
    }
    return false;
}

void JsonMLDocVisitor::json_string_start()
{
  if (m_insideString) return;
  m_tmpstr = "";
  if (needsComma()) m_tmpstr.append(',');
  m_tmpstr.append('"');
  m_insideString = true;
}

void JsonMLDocVisitor::json_string_end()
{
  if (!m_insideString) return;

  m_tmpstr.append('"');
  m_insideString = false;
  if (!isEmptyQuoted(m_tmpstr))
  {
    m_t.append(m_tmpstr);
    m_tmpstr = "";
    addComma();
  }
}

void JsonMLDocVisitor::json_string(const char*str)
{
  m_tmpstr = "";
  m_tmpstr.append('"');
  m_tmpstr.append(str);
  m_tmpstr.append('"');
  if (!isEmptyQuoted(m_tmpstr))
  {
    m_t.append(m_tmpstr);
    m_tmpstr = "";
    addComma();
  }
}

void JsonMLDocVisitor::json_object_start()
{
  json_string_end();
  if (needsComma()) m_t.append(',');
  m_t.append('{');
  beginScope();
}

void JsonMLDocVisitor::json_object_end()
{
  json_string_end();
  m_t.append('}');
  endScope();
  addComma();
}

void JsonMLDocVisitor::json_array_start()
{
  json_string_end();
  if (needsComma()) m_t.append(',');
  m_t.append('[');
  beginScope();
}

void JsonMLDocVisitor::json_array_end()
{
  json_string_end();
  m_t.append(']');
  endScope();
  addComma();
}

void JsonMLDocVisitor::operator()(const DocWhiteSpace &w)
{
  json_string_start();
  m_tmpstr.append(' ');
}

//--------------------------------------
// visitor functions for leaf nodes
//--------------------------------------
void JsonMLDocVisitor::operator()(const DocWord &w )
{
  json_string_start();
  filter(w.word());
}

void JsonMLDocVisitor::operator()(const DocIndexEntry &ie)
{
  json_array_start();
  json_string("indexentry");

  json_array_start();
  json_string("primaryie");
  m_tmpstr.append(',');
  m_tmpstr.append('"');
  filter(ie.entry());
  m_tmpstr.append('"');
  m_t.append(m_tmpstr);
  json_array_end();

  json_array_start();
  json_string("secondaryie");
  json_array_end();

  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocAnchor &anc)
{
  json_array_start();
  json_string("anchor");
  json_object_start();
  json_string("id");
  m_t.append(':');
  m_t.append('"');
  m_t.append(anc.file());
  m_t.append("_1");
  m_t.append(anc.anchor());
  m_t.append('"');
  json_object_end();
  json_array_end();
}

void JsonMLDocVisitor::startSimpleSect(const DocSimpleSect &s)
{
  json_array_start();
  json_string("simplesect");
  json_object_start();
  json_string("kind");
  m_t.append(':');
  switch(s.type())
  {
    case DocSimpleSect::See:
      json_string("see"); break;
    case DocSimpleSect::Return:
      json_string("return"); break;
    case DocSimpleSect::Author:
      json_string("author"); break;
    case DocSimpleSect::Authors:
      json_string("authors"); break;
    case DocSimpleSect::Version:
      json_string("version"); break;
    case DocSimpleSect::Since:
      json_string("since"); break;
    case DocSimpleSect::Date:
      json_string("date"); break;
    case DocSimpleSect::Note:
      json_string("note"); break;
    case DocSimpleSect::Warning:
      json_string("warning"); break;
    case DocSimpleSect::Pre:
      json_string("pre"); break;
    case DocSimpleSect::Post:
      json_string("post"); break;
    case DocSimpleSect::Copyright:
      json_string("copyright"); break;
    case DocSimpleSect::Invar:
      json_string("invariant"); break;
    case DocSimpleSect::Remark:
      json_string("remark"); break;
    case DocSimpleSect::Attention:
      json_string("attention"); break;
    case DocSimpleSect::Important:
      json_string("important"); break;
    case DocSimpleSect::User:
      json_string("par"); break;
    case DocSimpleSect::Rcs:
      json_string("rcs"); break;
    case DocSimpleSect::Unknown:  break;
  }
  json_object_end();
}

void JsonMLDocVisitor::endSimpleSect(const DocSimpleSect &)
{
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocSimpleSectSep &sep)
{
  const DocSimpleSect *sect = std::get_if<DocSimpleSect>(sep.parent());
  if (sect)
  {
    endSimpleSect(*sect);
    startSimpleSect(*sect);
  }
}

void JsonMLDocVisitor::operator()(const DocRoot &r)
{
  visitChildren(r);
}

void JsonMLDocVisitor::operator()(const DocSimpleSect &s)
{
  startSimpleSect(s);
  if (s.title())
  {
    std::visit(*this,*s.title());
  }
  visitChildren(s);
  endSimpleSect(s);
}


void JsonMLDocVisitor::operator()(const DocPara &p)
{
  json_array_start();
  json_string("para");
  visitChildren(p);
  json_string_end();
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocRef &ref)
{
  json_array_start();
  json_string("ref");
  json_object_start();
  json_string("refid");
  m_t.append(':');
  m_t.append('"');
  m_t.append(ref.file());
  if (!ref.anchor().isEmpty())
  {
    m_t.append("_1");
    m_t.append(ref.anchor());
  }
  m_t.append('"');
  m_t.append(',');

  json_string("kindref");
  m_t.append(':');
  if (!ref.anchor().isEmpty())
    json_string("member");
  else
    json_string("compound");
  json_object_end();
  visitChildren(ref);
  json_string_end();
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocTitle &t)
{
  json_array_start();
  json_string("title");
  visitChildren(t);
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocAutoList &l)
{
  json_array_start();
  if (l.isEnumList())
  {
    json_string("orderedlist");
  }
  else
  {
    json_string("itemizedlist");
  }
  visitChildren(l);
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocAutoListItem &li)
{
  json_array_start();
  json_string("listitem");
  visitChildren(li);
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocSimpleList &li)
{
  json_array_start();
  json_string("itemizedlist");
  visitChildren(li);
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocSimpleListItem &li)
{
  json_array_start();
  json_string("listitem");
  std::visit(*this,*li.paragraph());
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocStyleChange &s)
{
  switch (s.style())
  {
    case DocStyleChange::Bold:
      if (s.enable()) {json_array_start(); json_string("bold");} else json_array_end();
      break;
    case DocStyleChange::S:
      if (s.enable()) {json_array_start(); json_string("s");} else json_array_end();
      break;
    case DocStyleChange::Strike:
      if (s.enable()) {json_array_start(); json_string("strike");} else json_array_end();
      break;
    case DocStyleChange::Del:
      if (s.enable()) {json_array_start(); json_string("del");} else json_array_end();
      break;
    case DocStyleChange::Underline:
      if (s.enable()) {json_array_start(); json_string("underline");} else json_array_end();
      break;
    case DocStyleChange::Ins:
      if (s.enable()) {json_array_start(); json_string("ins");} else json_array_end();
      break;
    case DocStyleChange::Italic:
      if (s.enable()) {json_array_start(); json_string("emphasis");} else json_array_end();
      break;
    case DocStyleChange::Code:
      if (s.enable()) {json_array_start(); json_string("computeroutput");} else json_array_end();
      break;
    case DocStyleChange::Subscript:
      if (s.enable()) {json_array_start(); json_string("subscript");} else json_array_end();
      break;
    case DocStyleChange::Superscript:
      if (s.enable()) {json_array_start(); json_string("superscript");} else json_array_end();
      break;
    case DocStyleChange::Center:
      if (s.enable()) {json_array_start(); json_string("center");} else json_array_end();
      break;
    case DocStyleChange::Small:
      if (s.enable()) {json_array_start(); json_string("small");} else json_array_end();
      break;
    case DocStyleChange::Cite:
      if (s.enable()) {json_array_start(); json_string("cite");} else json_array_end();
      break;
    case DocStyleChange::Preformatted:
      if (s.enable())
      {
        json_array_start();
        json_string("preformatted");
      }
      else
      {
        json_array_end();
      }
      break;
    case DocStyleChange::Div:  /* HTML only */ break;
    case DocStyleChange::Span: /* HTML only */ break;
  }
}

void JsonMLDocVisitor::filter(const QCString &str)
{
  if (str.isEmpty())
    return;
  const char *p=str.data();
  char c;
  while (*p)
  {
    c=*p++;
    switch(c)
    {
      case '\n':  m_tmpstr.append(' ');      break;
      default:    m_tmpstr.append(c);
    }
  }
}

void JsonMLDocVisitor::operator()(const DocHtmlDescList &dl)
{
  json_array_start();
  json_string("variablelist");
  visitChildren(dl);
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocParamSect &s)
{
  json_array_start();
  json_string("parameterlist");
  json_object_start();
  json_string("kind");
  m_t.append(':');
  m_t.append('"');
  switch(s.type())
  {
    case DocParamSect::Param:
      m_t.append("param"); break;
    case DocParamSect::RetVal:
      m_t.append("retval"); break;
    case DocParamSect::Exception:
      m_t.append("exception"); break;
    case DocParamSect::TemplateParam:
      m_t.append("templateparam"); break;
    default:
      ASSERT(0);
  }
  m_t.append('"');
  json_object_end();
  visitChildren(s);
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocParamList &pl)
{
  json_array_start();
  json_string("parameteritem");
  json_array_start();
  json_string("parameternamelist");

  json_array_start();
  for (const auto &param : pl.parameters())
  {
    //if (!pl.paramTypes().empty())
    //{
      //m_t << "<parametertype>";
      //for (const auto &type : pl.paramTypes())
      //{
        //DocWord       *word       = dynamic_cast<DocWord*      >(type.get());
        //DocLinkedWord *linkedWord = dynamic_cast<DocLinkedWord*>(type.get());
        //DocSeparator  *sep        = dynamic_cast<DocSeparator* >(type.get());
        //if (word)
        //{
          //visit(word);
        //}
        //else if (linkedWord)
        //{
          //visit(linkedWord);
        //}
        //else if (sep)
        //{
          //m_t << "</parametertype>\n";
          //m_t << "<parametertype>";
        //}
      //}
      //m_t << "</parametertype>\n";
    //}
    json_string("parametername");
    json_object_start();
    json_string("direction");
    m_t.append(':');
    m_t.append('"');
    if (pl.direction()!=DocParamSect::Unspecified)
    {
      if (pl.direction()==DocParamSect::In)
      {
        m_t.append("in");
      }
      else if (pl.direction()==DocParamSect::Out)
      {
        m_t.append("out");
      }
      else if (pl.direction()==DocParamSect::InOut)
      {
        m_t.append("inout");
      }
    }
    m_t.append('"');
    std::visit(*this,param);
    json_object_end();
  }
  json_array_end();
  json_array_end();

  json_array_start();
  json_string("parameterdescription");
  for (const auto &par : pl.paragraphs())
  {
    std::visit(*this,par);
  }
  json_array_end();
  json_array_end();
}

void JsonMLDocVisitor::operator()(const DocParBlock &pb)
{
  json_array_start();
  json_string("parblock");
  visitChildren(pb);
  json_array_end();
}
