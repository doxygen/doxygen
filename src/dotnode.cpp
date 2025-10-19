/******************************************************************************
*
* Copyright (C) 1997-2019 by Dimitri van Heesch.
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

#include "dotnode.h"
#include "classdef.h"
#include "config.h"
#include "memberlist.h"
#include "membergroup.h"
#include "language.h"
#include "doxygen.h"
#include "util.h"
#include "textstream.h"

/** Helper struct holding the properties of a edge in a dot graph. */
struct EdgeProperties
{
  const char * const *edgeColorMap;
  const char * const *arrowStyleMap;
  const char * const *edgeStyleMap;
};

/*! mapping from protection levels to color names */
static const char *normalEdgeColorMap[] =
{
  "steelblue1",    // Public
  "darkgreen",     // Protected
  "firebrick4",    // Private
  "darkorchid3",   // "use" relation
  "grey75",        // Undocumented
  "orange",        // template relation
  "orange"         // type constraint
};

static const char *normalArrowStyleMap[] =
{
  "empty",         // Public
  "empty",         // Protected
  "empty",         // Private
  "open",          // "use" relation
  nullptr,         // Undocumented
  nullptr          // template relation
};

static const char *normalEdgeStyleMap[] =
{
  "solid",         // inheritance
  "dashed"         // usage
};

static const char *umlEdgeColorMap[] =
{
  "steelblue1",    // Public
  "darkgreen",     // Protected
  "firebrick4",    // Private
  "steelblue1",    // "use" relation
  "grey75",        // Undocumented
  "orange",        // template relation
  "orange"         // type constraint
};

static const char *umlArrowStyleMap[] =
{
  "onormal",         // Public
  "onormal",         // Protected
  "onormal",         // Private
  "odiamond",        // "use" relation
  nullptr,           // Undocumented
  nullptr           // template relation
};

static const char *umlEdgeStyleMap[] =
{
  "solid",         // inheritance
  "solid"          // usage
};

static EdgeProperties normalEdgeProps =
{
  normalEdgeColorMap, normalArrowStyleMap, normalEdgeStyleMap
};

static EdgeProperties umlEdgeProps =
{
  umlEdgeColorMap, umlArrowStyleMap, umlEdgeStyleMap
};

QCString escapeTooltip(const QCString &tooltip)
{
  if (tooltip.isEmpty()) return tooltip;
  QCString result;
  const char *p=tooltip.data();
  char c = 0;
  while ((c=*p++))
  {
    switch(c)
    {
    case '"': result+="\\\""; break;
    case '\\': result+="\\\\"; break;
    default: result+=c; break;
    }
  }
  return result;
}

static void writeBoxMemberList(TextStream &t,
  char prot,const MemberList *ml,const ClassDef *scope,
  bool &lineWritten,
  bool isStatic=FALSE,const StringUnorderedSet *skipNames=nullptr)
{
  constexpr auto tr_start = "<TR><TD VALIGN=\"top\" CELLPADDING=\"1\" CELLSPACING=\"0\">";
  constexpr auto tr_mid   = "</TD><TD VALIGN=\"top\" ALIGN=\"LEFT\" CELLPADDING=\"1\" CELLSPACING=\"0\">";
  constexpr auto tr_end   = "</TD></TR>\n";
  constexpr auto br       = "<BR ALIGN=\"LEFT\"/>";
  if (ml)
  {
    auto hideUndocMembers = Config_getEnum(HIDE_UNDOC_MEMBERS);
    int totalCount=0;
    for (const auto &mma : *ml)
    {
      if (mma->getClassDef()==scope &&
        (skipNames==nullptr || skipNames->find(mma->name().str())==std::end(*skipNames)) &&
          !(hideUndocMembers && !mma->hasDocumentation())
         )
      {
        totalCount++;
      }
    }

    int count=0;
    auto dotUmlDetails = Config_getEnum(DOT_UML_DETAILS);
    for (const auto &mma : *ml)
    {
      if (mma->getClassDef() == scope &&
        (skipNames==nullptr || skipNames->find(mma->name().str())==std::end(*skipNames)) &&
          !(hideUndocMembers && !mma->hasDocumentation())
         )
      {
        int numFields = Config_getInt(UML_LIMIT_NUM_FIELDS);
        if (numFields>0 && (totalCount>numFields*3/2 && count>=numFields))
        {
          t << tr_start << tr_mid << theTranslator->trAndMore(QCString().sprintf("%d",totalCount-count)) << tr_end;
          lineWritten = true;
          break;
        }
        else
        {
          t << tr_start << prot << tr_mid;
          QCString label;
          if (dotUmlDetails==DOT_UML_DETAILS_t::YES)
          {
            label+=mma->typeString();
            label+=" ";
          }
          label+=mma->name();
          if (!mma->isObjCMethod() && (mma->isFunction() || mma->isSlot() || mma->isSignal()))
          {
            if (dotUmlDetails==DOT_UML_DETAILS_t::YES)
            {
              label+=mma->argsString();
            }
            else
            {
              label+="()";
            }
          }
          t << DotNode::convertLabel(label,DotNode::LabelStyle::List);
          t << br << tr_end;
          lineWritten = true;
          count++;
        }
      }
    }
    // write member groups within the memberlist
    for (const auto &mg : ml->getMemberGroupList())
    {
      if (!mg->members().empty())
      {
        writeBoxMemberList(t,prot,&mg->members(),scope,lineWritten,isStatic,skipNames);
      }
    }
  }
}

QCString DotNode::convertLabel(const QCString &l, LabelStyle style)
{
  QCString bBefore("\\_/<({[: =-+@%#~?$"); // break before character set
  QCString bAfter(">]),:;|");              // break after  character set
  if (l.isEmpty()) return QCString();
  QCString result;
  char pc=0;
  uint32_t idx = 0;
  int charsLeft=static_cast<int>(l.length());
  int sinceLast=0;
  int foldLen = Config_getInt(DOT_WRAP_THRESHOLD); // ideal text length
  QCString br;
  QCString br1;
  if (style==LabelStyle::Table)
  {
    result += "<<TABLE CELLBORDER=\"0\" BORDER=\"0\"><TR><TD VALIGN=\"top\" ALIGN=\"LEFT\" CELLPADDING=\"1\" CELLSPACING=\"0\">";
  }
  if (style==LabelStyle::List)
  {
    br = "<BR ALIGN=\"LEFT\"/>";
  }
  else if (style==LabelStyle::Table)
  {
    br1 = "</TD></TR>\n<TR><TD VALIGN=\"top\" ALIGN=\"LEFT\" CELLPADDING=\"1\" CELLSPACING=\"0\">";
    br = br1 + "&nbsp;&nbsp;";
  }
  else // style==LabelStyle::Plain
  {
    br = "\\l";
  }
  while (idx < l.length())
  {
    char c = l[idx++];
    char cs[2] = { c, 0 };
    const char *replacement = cs;
    if (style!=LabelStyle::Plain)
    {
      switch(c)
      {
        case '\\': replacement="\\\\";   break;
        case '\n': replacement="\\n";    break;
        case '<':  replacement="&lt;";   break;
        case '>':  replacement="&gt;";   break;
        case '"':  replacement="&quot;"; break;
        case '\'': replacement="&apos;"; break;
        case '&':  replacement="&amp;";  break;
      }
    }
    else // style==LabelStyle::Plain
    {
      switch(c)
      {
        case '\\': replacement="\\\\"; break;
        case '\n': replacement="\\n";  break;
        case '<':  replacement="\\<";  break;
        case '>':  replacement="\\>";  break;
        case '"':  replacement="\\\""; break;
        case '|':  replacement="\\|";  break;
        case '{':  replacement="\\{";  break;
        case '}':  replacement="\\}";  break;
      }
    }
    // Some heuristics to insert newlines to prevent too long
    // boxes and at the same time prevent ugly breaks
    if (c=='\n')
    {
      if (style==LabelStyle::Table)
      {
        result+=br1;
      }
      else
      {
        result+=replacement;
      }
      foldLen = (3*foldLen+sinceLast+2)/4;
      sinceLast=1;
    }
    else if ((pc!=':' || c!=':') && charsLeft>foldLen/3 && sinceLast>foldLen && bBefore.contains(c))
    {
      result+=br;
      result+=replacement;
      foldLen = (foldLen+sinceLast+1)/2;
      sinceLast=1;
    }
    else if (charsLeft>1+foldLen/4 && sinceLast>foldLen+foldLen/3 &&
      !isupper(c) && isupper(l[idx]))
    {
      result+=replacement;
      result+=br;
      foldLen = (foldLen+sinceLast+1)/2;
      sinceLast=0;
    }
    else if (charsLeft>foldLen/3 && sinceLast>foldLen && bAfter.contains(c) && (c!=':' || l[idx]!=':'))
    {
      result+=replacement;
      result+=br;
      foldLen = (foldLen+sinceLast+1)/2;
      sinceLast=0;
    }
    else
    {
      result+=replacement;
      sinceLast++;
    }
    charsLeft--;
    pc=c;
  }
  if (style==LabelStyle::List)
  {
     result = result.stripWhiteSpace();
  }
  if (style==LabelStyle::Table)
  {
    result += "</TD></TR>\n</TABLE>>";
  }
  return result;
}

static QCString stripProtectionPrefix(const QCString &s)
{
  if (!s.isEmpty() && (s[0]=='-' || s[0]=='+' || s[0]=='~' || s[0]=='#'))
  {
    return s.mid(1);
  }
  else
  {
    return s;
  }
}

DotNode::DotNode(DotGraph *graph,const QCString &lab,const QCString &tip, const QCString &url,
  bool isRoot,const ClassDef *cd)
  : m_graph(graph)
  , m_number(graph->getNextNodeNumber())
  , m_label(lab)
  , m_tooltip(tip)
  , m_url(url)
  , m_isRoot(isRoot)
  , m_classDef(cd)
{
}

void DotNode::addChild(DotNode *n,
  EdgeInfo::Colors edgeColor,
  EdgeInfo::Styles edgeStyle,
  const QCString &edgeLab,
  const QCString &edgeURL,
  int edgeLabCol
)
{
  m_children.push_back(n);
  m_edgeInfo.emplace_back(
      edgeColor,
      edgeStyle,
      edgeLab,
      edgeURL,
      edgeLabCol==-1 ? edgeColor : edgeLabCol);
}

void DotNode::addParent(DotNode *n)
{
  m_parents.push_back(n);
}

void DotNode::removeChild(DotNode *n)
{
  auto it = std::find(m_children.begin(),m_children.end(),n);
  if (it!=m_children.end()) m_children.erase(it);
}

void DotNode::removeParent(DotNode *n)
{
  auto it = std::find(m_parents.begin(),m_parents.end(),n);
  if (it!=m_parents.end()) m_parents.erase(it);
}

void DotNode::deleteNode(DotNodeRefVector &deletedList)
{
  if (m_deleted) return; // avoid recursive loops in case the graph has cycles
  m_deleted=TRUE;
  // delete all parent nodes of this node
  for (const auto &pn : m_parents)
  {
    pn->deleteNode(deletedList);
  }
  // delete all child nodes of this node
  for (const auto &cn : m_children)
  {
    cn->deleteNode(deletedList);
  }
  // add this node to the list of deleted nodes.
  deletedList.push_back(this);
}

void DotNode::setDistance(int distance)
{
  if (distance<m_distance) m_distance = distance;
}

inline int DotNode::findParent( DotNode *n )
{
  auto it = std::find(m_parents.begin(),m_parents.end(),n);
  return it!=m_parents.end() ? static_cast<int>(it-m_parents.begin()) : -1;
}

/*! helper function that deletes all nodes in a connected graph, given
*  one of the graph's nodes
*/
void DotNode::deleteNodes(DotNode *node)
{
  DotNodeRefVector deletedNodes;
  node->deleteNode(deletedNodes); // collect nodes to be deleted.
  for (const auto &dotNode : deletedNodes)
  {
    delete dotNode;
  }
}

void DotNode::writeLabel(TextStream &t, GraphType gt) const
{
  if (m_classDef && Config_getBool(UML_LOOK) && (gt==GraphType::Inheritance || gt==GraphType::Collaboration))
  {
    // Set shape to the plain type.
    // the UML properties and methods are rendered using dot' HTML like table format
    t << "shape=plain,label=";
    // add names shown as relations to a set, so we don't show
    // them as attributes as well
    StringUnorderedSet arrowNames;
    // for each edge
    for (const auto &ei : m_edgeInfo)
    {
      if (!ei.label().isEmpty()) // labels joined by \n
      {
        int i=0;
        int p=0;
        QCString lab;
        while ((i=ei.label().find('\n',p))!=-1)
        {
          lab = stripProtectionPrefix(ei.label().mid(p,i-p));
          arrowNames.insert(lab.str());
          p=i+1;
        }
        lab = stripProtectionPrefix(ei.label().right(ei.label().length()-p));
        arrowNames.insert(lab.str());
      }
    }

    constexpr auto hr_start = "<TR><TD COLSPAN=\"2\" CELLPADDING=\"1\" CELLSPACING=\"0\">";
    constexpr auto hr_end = "</TD></TR>\n";
    constexpr auto sep = "<HR/>\n";
    constexpr auto empty_line = "<TR><TD COLSPAN=\"2\" CELLPADDING=\"1\" CELLSPACING=\"0\">&nbsp;</TD></TR>\n";
    //printf("DotNode::writeBox for %s\n",qPrint(m_classDef->name()));
    t << "<<TABLE CELLBORDER=\"0\" BORDER=\"1\">";
    t << hr_start << convertLabel(m_label,LabelStyle::List) << hr_end;
    auto dotUmlDetails = Config_getEnum(DOT_UML_DETAILS);
    if (dotUmlDetails!=DOT_UML_DETAILS_t::NONE)
    {
      bool lineWritten = false;
      t << sep;
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType::PubAttribs()),m_classDef,lineWritten,FALSE,&arrowNames);
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType::PubStaticAttribs()),m_classDef,lineWritten,TRUE,&arrowNames);
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType::Properties()),m_classDef,lineWritten,FALSE,&arrowNames);
      writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType::PacAttribs()),m_classDef,lineWritten,FALSE,&arrowNames);
      writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType::PacStaticAttribs()),m_classDef,lineWritten,TRUE,&arrowNames);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType::ProAttribs()),m_classDef,lineWritten,FALSE,&arrowNames);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType::ProStaticAttribs()),m_classDef,lineWritten,TRUE,&arrowNames);
      if (Config_getBool(EXTRACT_PRIVATE))
      {
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType::PriAttribs()),m_classDef,lineWritten,FALSE,&arrowNames);
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType::PriStaticAttribs()),m_classDef,lineWritten,TRUE,&arrowNames);
      }
      if (!lineWritten) t << empty_line;
      t << sep;
      lineWritten = false;
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType::PubMethods()),m_classDef,lineWritten);
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType::PubStaticMethods()),m_classDef,lineWritten,TRUE);
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType::PubSlots()),m_classDef,lineWritten);
      writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType::PacMethods()),m_classDef,lineWritten);
      writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType::PacStaticMethods()),m_classDef,lineWritten,TRUE);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType::ProMethods()),m_classDef,lineWritten);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType::ProStaticMethods()),m_classDef,lineWritten,TRUE);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType::ProSlots()),m_classDef,lineWritten);
      if (Config_getBool(EXTRACT_PRIVATE))
      {
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType::PriMethods()),m_classDef,lineWritten);
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType::PriStaticMethods()),m_classDef,lineWritten,TRUE);
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType::PriSlots()),m_classDef,lineWritten);
      }
      if (m_classDef->getLanguage()!=SrcLangExt::Fortran)
      {
        for (const auto &mg : m_classDef->getMemberGroups())
        {
          if (!mg->members().empty())
          {
            writeBoxMemberList(t,'*',&mg->members(),m_classDef,lineWritten,FALSE,&arrowNames);
          }
        }
      }
      if (!lineWritten) t << empty_line;
    }
    t << "</TABLE>>\n";
  }
  else if (Config_getString(DOT_NODE_ATTR).contains("shape=plain"))
  {
    t << "label=";
    if (m_isRoot)
      t << "<<b>" << convertToXML(m_label) << "</b>>";
    else if (m_truncated == Truncated)
      t << "<<i>" << convertToXML(m_label) << "</i>>";
    else
      t << '"' << convertLabel(m_label,LabelStyle::Plain) << '"';
  }
  else // standard look
  {
    t << "label=" << '"' << convertLabel(m_label,LabelStyle::Plain) << '"';
  }
}

void DotNode::writeUrl(TextStream &t) const
{
  if (m_url.isEmpty() || m_url == DotNode::placeholderUrl) return;
  int tagPos = m_url.findRev('$');
  t << ",URL=\"";
  QCString noTagURL = m_url;
  if (tagPos!=-1)
  {
    t << m_url.left(tagPos);
    noTagURL = m_url.mid(tagPos);
  }
  int anchorPos = noTagURL.findRev('#');
  if (anchorPos==-1)
  {
    addHtmlExtensionIfMissing(noTagURL);
    t << noTagURL << "\"";
  }
  else // insert extensiom before anchor
  {
    QCString fn = noTagURL.left(anchorPos);
    addHtmlExtensionIfMissing(fn);
    t << fn << noTagURL.right(noTagURL.length() - anchorPos) << "\"";
  }
}

void DotNode::writeBox(TextStream &t,
                       GraphType gt,
                       GraphOutputFormat /*format*/,
                       bool hasNonReachableChildren) const
{
  const char *labCol = nullptr;
  const char *fillCol = "white";
  if (m_classDef)
  {
    if (m_classDef->hasDocumentation() && hasNonReachableChildren)
    {
      labCol = "red";
      fillCol = "#FFF0F0";
    }
    else if (m_classDef->hasDocumentation() && !hasNonReachableChildren)
    {
      labCol = "gray40";
    }
    else if (!m_classDef->hasDocumentation() && hasNonReachableChildren)
    {
      labCol = "orangered";
    }
    else // (!m_classDef->hasDocumentation() && !hasNonReachableChildren)
    {
      labCol = "grey75";
      if (m_classDef->templateMaster() && m_classDef->isImplicitTemplateInstance() && m_classDef->templateMaster()->hasDocumentation())
      {
        labCol = "gray40";
      }
    }
  }
  else
  {
    labCol = m_url.isEmpty() ? "grey60" :  // non link
    (hasNonReachableChildren ? "red" : "grey40");
    fillCol = m_url.isEmpty() ? "#E0E0E0" :
    (hasNonReachableChildren ? "#FFF0F0" : "white");
  }
  t << "  Node" << m_number << " [";
  t << "id=\"Node" << QCString().sprintf("%06d",m_number) << "\",";
  writeLabel(t,gt);
  t << ",height=0.2,width=0.4";
  if (m_isRoot)
  {
    t << ",color=\"gray40\", fillcolor=\"grey60\", style=\"filled\", fontcolor=\"black\"";
  }
  else
  {
    t << ",color=\"" << labCol << "\"";
    t << ", fillcolor=\"" << fillCol << "\"";
    t << ", style=\"filled\"";
    writeUrl(t);
  }
  if (!m_tooltip.isEmpty())
  {
    t << ",tooltip=\"" << escapeTooltip(m_tooltip) << "\"";
  }
  else
  {
    t << ",tooltip=\" \""; // space in tooltip is required otherwise still something like 'Node0' is used
  }
  t << "];\n";
}

void DotNode::writeArrow(TextStream &t,
                         GraphType gt,
                         GraphOutputFormat /* format */,
                         const DotNode *cn,
                         const EdgeInfo *ei,
                         bool topDown,
                         bool pointBack) const
{
  t << "  Node";
  if (topDown)
    t << cn->number();
  else
    t << m_number;
  t << " -> Node";
  if (topDown)
    t << m_number;
  else
    t << cn->number();
  t << " [";

  const EdgeProperties *eProps = Config_getBool(UML_LOOK) ? &umlEdgeProps : &normalEdgeProps;
  QCString aStyle = eProps->arrowStyleMap[ei->color()];
  bool umlUseArrow = aStyle=="odiamond";

  t << "id=\"edge" << m_graph->getNextEdgeNumber() <<
       "_Node" << QCString().sprintf("%06d",m_number) <<
       "_Node" << QCString().sprintf("%06d",cn->number()) << "\",";
  if (pointBack && !umlUseArrow) t << "dir=\"back\",";
  t << "color=\"" << eProps->edgeColorMap[ei->color()] << "\",";
  t << "style=\"" << eProps->edgeStyleMap[ei->style()] << "\"";
  t << ",tooltip=\" \""; // space in tooltip is required otherwise still something like 'Node0 -> Node1' is used
  if (!ei->label().isEmpty())
  {
    t << ",label=" << convertLabel(ei->label(),LabelStyle::Table) << " ,fontcolor=\"grey\" ";
  }
  if (Config_getBool(UML_LOOK) &&
    eProps->arrowStyleMap[ei->color()] &&
    (gt==GraphType::Inheritance || gt==GraphType::Collaboration)
    )
  {
    bool rev = pointBack;
    if (umlUseArrow) rev=!rev; // UML use relates has arrow on the start side
    if (rev)
      t << ",arrowtail=\"" << eProps->arrowStyleMap[ei->color()] << "\"";
    else
      t << ",arrowhead=\"" << eProps->arrowStyleMap[ei->color()] << "\"";
  }

  t << "];\n";
}

void DotNode::write(TextStream &t,
                    GraphType gt,
                    GraphOutputFormat format,
                    bool topDown,
                    bool toChildren,
                    bool backArrows)
{
  //printf("DotNode::write(%d) name=%s this=%p written=%d visible=%d\n",m_distance,qPrint(m_label),this,m_written,m_visible);
  if (m_written) return; // node already written to the output
  if (!m_visible) return; // node is not visible
  writeBox(t,gt,format,m_truncated==Truncated);
  m_written=TRUE;
  if (toChildren)
  {
    auto it = m_edgeInfo.begin();
    for (const auto &cn : m_children)
    {
      if (cn->isVisible())
      {
        //printf("write arrow %s%s%s\n",qPrint(label()),backArrows?"<-":"->",qPrint(cn->label()));
        writeArrow(t,gt,format,cn,&(*it),topDown,backArrows);
      }
      cn->write(t,gt,format,topDown,toChildren,backArrows);
      ++it;
    }
  }
  else // render parents
  {
    for (const auto &pn : m_parents)
    {
      if (pn->isVisible())
      {
        const auto &children = pn->children();
        auto child_it = std::find(children.begin(),children.end(),this);
        size_t index = child_it - children.begin();
        //printf("write arrow %s%s%s\n",qPrint(label()),backArrows?"<-":"->",qPrint(pn->label()));
        writeArrow(t,
          gt,
          format,
          pn,
          &pn->edgeInfo()[index],
          FALSE,
          backArrows
        );
      }
      pn->write(t,gt,format,TRUE,FALSE,backArrows);
    }
  }
  //printf("end DotNode::write(%d) name=%s\n",distance,qPrint(m_label));
}

void DotNode::writeXML(TextStream &t,bool isClassGraph) const
{
  t << "      <node id=\"" << m_number << "\">\n";
  t << "        <label>" << convertToXML(m_label) << "</label>\n";
  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    int dollarPos = url.find('$');
    if (dollarPos!=-1)
    {
      t << "        <link refid=\"" << convertToXML(url.mid(dollarPos+1)) << "\"";
      if (dollarPos>0)
      {
        t << " external=\"" << convertToXML(url.left(dollarPos)) << "\"";
      }
      t << "/>\n";
    }
  }
  auto it = m_edgeInfo.begin();
  for (const auto &childNode : m_children)
  {
    const EdgeInfo &edgeInfo = *it;
    t << "        <childnode refid=\"" << childNode->number() << "\" relation=\"";
    if (isClassGraph)
    {
      switch(edgeInfo.color())
      {
      case EdgeInfo::Blue:    t << "public-inheritance"; break;
      case EdgeInfo::Green:   t << "protected-inheritance"; break;
      case EdgeInfo::Red:     t << "private-inheritance"; break;
      case EdgeInfo::Purple:  t << "usage"; break;
      case EdgeInfo::Orange:  t << "template-instance"; break;
      case EdgeInfo::Orange2: t << "type-constraint"; break;
      case EdgeInfo::Grey:    ASSERT(0); break;
      }
    }
    else // include graph
    {
      t << "include";
    }
    t << "\">\n";
    if (!edgeInfo.label().isEmpty())
    {
      int p=0;
      int ni=0;
      while ((ni=edgeInfo.label().find('\n',p))!=-1)
      {
        t << "          <edgelabel>"
          << convertToXML(edgeInfo.label().mid(p,ni-p))
          << "</edgelabel>\n";
        p=ni+1;
      }
      t << "          <edgelabel>"
        << convertToXML(edgeInfo.label().right(edgeInfo.label().length()-p))
        << "</edgelabel>\n";
    }
    t << "        </childnode>\n";
    ++it;
  }
  t << "      </node>\n";
}

void DotNode::writeDocbook(TextStream &t,bool isClassGraph) const
{
  t << "      <node id=\"" << m_number << "\">\n";
  t << "        <label>" << convertToXML(m_label) << "</label>\n";
  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    int dollarPos = url.find('$');
    if (dollarPos!=-1)
    {
      t << "        <link refid=\"" << convertToXML(url.mid(dollarPos+1)) << "\"";
      if (dollarPos>0)
      {
        t << " external=\"" << convertToXML(url.left(dollarPos)) << "\"";
      }
      t << "/>\n";
    }
  }
  auto it = m_edgeInfo.begin();
  for (const auto &childNode : m_children)
  {
    const EdgeInfo &edgeInfo = *it;
    t << "        <childnode refid=\"" << childNode->number() << "\" relation=\"";
    if (isClassGraph)
    {
      switch(edgeInfo.color())
      {
      case EdgeInfo::Blue:    t << "public-inheritance"; break;
      case EdgeInfo::Green:   t << "protected-inheritance"; break;
      case EdgeInfo::Red:     t << "private-inheritance"; break;
      case EdgeInfo::Purple:  t << "usage"; break;
      case EdgeInfo::Orange:  t << "template-instance"; break;
      case EdgeInfo::Orange2: t << "type-constraint"; break;
      case EdgeInfo::Grey:    ASSERT(0); break;
      }
    }
    else // include graph
    {
      t << "include";
    }
    t << "\">\n";
    if (!edgeInfo.label().isEmpty())
    {
      int p=0;
      int ni=0;
      while ((ni=edgeInfo.label().find('\n',p))!=-1)
      {
        t << "          <edgelabel>"
          << convertToXML(edgeInfo.label().mid(p,ni-p))
          << "</edgelabel>\n";
        p=ni+1;
      }
      t << "          <edgelabel>"
        << convertToXML(edgeInfo.label().right(edgeInfo.label().length()-p))
        << "</edgelabel>\n";
    }
    t << "        </childnode>\n";
    ++it;
  }
  t << "      </node>\n";
}


void DotNode::writeDEF(TextStream &t) const
{
  const char* nodePrefix = "        node-";

  t << "      node = {\n";
  t << nodePrefix << "id    = " << m_number << ";\n";
  t << nodePrefix << "label = '" << m_label << "';\n";

  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    int dollarPos = url.find('$');
    if (dollarPos!=-1)
    {
      t << nodePrefix << "link = {\n" << "  "
        << nodePrefix << "link-id = '" << url.mid(dollarPos+1) << "';\n";
      if (dollarPos>0)
      {
        t << "  " << nodePrefix << "link-external = '"
          << url.left(dollarPos) << "';\n";
      }
      t << "        };\n";
    }
  }
  auto it = m_edgeInfo.begin();
  for (const auto &childNode : m_children)
  {
    const EdgeInfo &edgeInfo = *it;
    t << "        node-child = {\n";
    t << "          child-id = '" << childNode->number() << "';\n";
    t << "          relation = ";

    switch (edgeInfo.color())
    {
      case EdgeInfo::Blue:    t << "public-inheritance"; break;
      case EdgeInfo::Green:   t << "protected-inheritance"; break;
      case EdgeInfo::Red:     t << "private-inheritance"; break;
      case EdgeInfo::Purple:  t << "usage"; break;
      case EdgeInfo::Orange:  t << "template-instance"; break;
      case EdgeInfo::Orange2: t << "type-constraint"; break;
      case EdgeInfo::Grey:    ASSERT(0); break;
    }
    t << ";\n";

    if (!edgeInfo.label().isEmpty())
    {
      t << "          edgelabel = <<_EnD_oF_dEf_TeXt_\n"
        << edgeInfo.label() << "\n"
        << "_EnD_oF_dEf_TeXt_;\n";
    }
    t << "        }; /* node-child */\n";
    ++it;
  }
  t << "      }; /* node */\n";
}


void DotNode::clearWriteFlag()
{
  m_written=FALSE;
  for (const auto &pn : m_parents)  if (pn->isWritten()) pn->clearWriteFlag();
  for (const auto &cn : m_children) if (cn->isWritten()) cn->clearWriteFlag();
}

void DotNode::colorConnectedNodes(int curColor)
{
  for (const auto &cn : m_children)
  {
    if (cn->subgraphId()==-1) // uncolored child node
    {
      cn->setSubgraphId(curColor);
      cn->markAsVisible();
      cn->colorConnectedNodes(curColor);
      //printf("coloring node %s (%p): %d\n",qPrint(cn->label()),cn,cn->subgraphId());
    }
  }

  for (const auto &pn : m_parents)
  {
    if (pn->subgraphId()==-1) // uncolored parent node
    {
      pn->setSubgraphId(curColor);
      pn->markAsVisible();
      pn->colorConnectedNodes(curColor);
      //printf("coloring node %s (%p): %d\n",qPrint(pn->label()),pn,pn->subgraphId());
    }
  }
}

#define DEBUG_RENUMBERING 0

void DotNode::renumberNodes(int &number)
{
  if (!isRenumbered())
  {
#if DEBUG_RENUMBERING
    static int level = 0;
    printf("%3d: ",subgraphId());
    for (int i = 0; i < level; i++) printf("  ");
    printf("> %s old = %d new = %d\n",qPrint(m_label),m_number,number);
    level++;
#endif
    m_number = number++;
    markRenumbered();
    for (const auto &cn : m_children)
    {
      cn->renumberNodes(number);
    }
    for (const auto &pn : m_parents)
    {
      pn->renumberNodes(number);
    }
#if DEBUG_RENUMBERING
    level--;
    printf("%3d: ",subgraphId());
    for (int i = 0; i < level; i++) printf("  ");
    printf("< %s assigned = %d\n",qPrint(m_label),m_number);
#endif
  }
}




