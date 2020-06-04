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
  "midnightblue",  // Public
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
  0,               // Undocumented
  0                // template relation
};

static const char *normalEdgeStyleMap[] =
{
  "solid",         // inheritance
  "dashed"         // usage
};

static const char *umlEdgeColorMap[] =
{
  "midnightblue",  // Public
  "darkgreen",     // Protected
  "firebrick4",    // Private
  "grey25",        // "use" relation
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
  0,                 // Undocumented
  0                  // template relation
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

static QCString escapeTooltip(const QCString &tooltip)
{
  if (tooltip.isEmpty()) return tooltip;
  QCString result;
  const char *p=tooltip.data();
  char c;
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
  bool isStatic=FALSE,const StringUnorderedSet *skipNames=nullptr)
{
  if (ml)
  {
    int totalCount=0;
    for (const auto &mma : *ml)
    {
      if (mma->getClassDef()==scope &&
        (skipNames==nullptr || skipNames->find(mma->name().str())==std::end(*skipNames)))
      {
        totalCount++;
      }
    }

    int count=0;
    auto dotUmlDetails = Config_getEnum(DOT_UML_DETAILS);
    for (const auto &mma : *ml)
    {
      if (mma->getClassDef() == scope &&
        (skipNames==nullptr || skipNames->find(mma->name().str())==std::end(*skipNames)))
      {
        int numFields = Config_getInt(UML_LIMIT_NUM_FIELDS);
        if (numFields>0 && (totalCount>numFields*3/2 && count>=numFields))
        {
          t << theTranslator->trAndMore(QCString().sprintf("%d",totalCount-count)) << "\\l";
          break;
        }
        else
        {
          t << prot << " ";
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
          t << DotNode::convertLabel(label);
          t << "\\l";
          count++;
        }
      }
    }
    // write member groups within the memberlist
    for (const auto &mg : ml->getMemberGroupList())
    {
      if (!mg->members().empty())
      {
        writeBoxMemberList(t,prot,&mg->members(),scope,isStatic,skipNames);
      }
    }
  }
}

QCString DotNode::convertLabel(const QCString &l)
{
  QCString bBefore("\\_/<({[: =-+@%#~?$"); // break before character set
  QCString bAfter(">]),:;|");              // break after  character set
  QCString p(l);
  if (p.isEmpty()) return QCString();
  QCString result;
  char c,pc=0;
  uint idx = 0;
  int len=p.length();
  int charsLeft=len;
  int sinceLast=0;
  int foldLen = Config_getInt(DOT_WRAP_THRESHOLD); // ideal text length
  while (idx < p.length())
  {
    c = p[idx++];
    QCString replacement;
    switch(c)
    {
      case '\\': replacement="\\\\"; break;
      case '\n': replacement="\\n"; break;
      case '<':  replacement="\\<"; break;
      case '>':  replacement="\\>"; break;
      case '|':  replacement="\\|"; break;
      case '{':  replacement="\\{"; break;
      case '}':  replacement="\\}"; break;
      case '"':  replacement="\\\""; break;
      default:   replacement+=c; break;
    }
    // Some heuristics to insert newlines to prevent too long
    // boxes and at the same time prevent ugly breaks
    if (c=='\n')
    {
      result+=replacement;
      foldLen = (3*foldLen+sinceLast+2)/4;
      sinceLast=1;
    }
    else if ((pc!=':' || c!=':') && charsLeft>foldLen/3 && sinceLast>foldLen && bBefore.contains(c))
    {
      result+="\\l";
      result+=replacement;
      foldLen = (foldLen+sinceLast+1)/2;
      sinceLast=1;
    }
    else if (charsLeft>1+foldLen/4 && sinceLast>foldLen+foldLen/3 &&
      !isupper(c) && isupper(p[idx]))
    {
      result+=replacement;
      result+="\\l";
      foldLen = (foldLen+sinceLast+1)/2;
      sinceLast=0;
    }
    else if (charsLeft>foldLen/3 && sinceLast>foldLen && bAfter.contains(c) && (c!=':' || p[idx]!=':'))
    {
      result+=replacement;
      result+="\\l";
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

DotNode::DotNode(int n,const QCString &lab,const QCString &tip, const QCString &url,
  bool isRoot,const ClassDef *cd)
  : m_number(n)
  , m_label(lab)
  , m_tooltip(tip)
  , m_url(url)
  , m_isRoot(isRoot)
  , m_classDef(cd)
{
}

DotNode::~DotNode()
{
}

void DotNode::addChild(DotNode *n,
  int edgeColor,
  int edgeStyle,
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
  t << "label=";
  if (m_classDef && Config_getBool(UML_LOOK) && (gt==Inheritance || gt==Collaboration))
  {
    // add names shown as relations to a set, so we don't show
    // them as attributes as well
    StringUnorderedSet arrowNames;
    // for each edge
    for (const auto &ei : m_edgeInfo)
    {
      if (!ei.label().isEmpty()) // labels joined by \n
      {
        int i;
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

    //printf("DotNode::writeBox for %s\n",qPrint(m_classDef->name()));
    t << "\"{" << convertLabel(m_label) << "\\n";
    auto dotUmlDetails = Config_getEnum(DOT_UML_DETAILS);
    if (dotUmlDetails!=DOT_UML_DETAILS_t::NONE)
    {
      t << "|";
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubAttribs),m_classDef,FALSE,&arrowNames);
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubStaticAttribs),m_classDef,TRUE,&arrowNames);
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_properties),m_classDef,FALSE,&arrowNames);
      writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacAttribs),m_classDef,FALSE,&arrowNames);
      writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacStaticAttribs),m_classDef,TRUE,&arrowNames);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proAttribs),m_classDef,FALSE,&arrowNames);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proStaticAttribs),m_classDef,TRUE,&arrowNames);
      if (Config_getBool(EXTRACT_PRIVATE))
      {
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priAttribs),m_classDef,FALSE,&arrowNames);
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priStaticAttribs),m_classDef,TRUE,&arrowNames);
      }
      t << "|";
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubMethods),m_classDef);
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubStaticMethods),m_classDef,TRUE);
      writeBoxMemberList(t,'+',m_classDef->getMemberList(MemberListType_pubSlots),m_classDef);
      writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacMethods),m_classDef);
      writeBoxMemberList(t,'~',m_classDef->getMemberList(MemberListType_pacStaticMethods),m_classDef,TRUE);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proMethods),m_classDef);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proStaticMethods),m_classDef,TRUE);
      writeBoxMemberList(t,'#',m_classDef->getMemberList(MemberListType_proSlots),m_classDef);
      if (Config_getBool(EXTRACT_PRIVATE))
      {
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priMethods),m_classDef);
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priStaticMethods),m_classDef,TRUE);
        writeBoxMemberList(t,'-',m_classDef->getMemberList(MemberListType_priSlots),m_classDef);
      }
      if (m_classDef->getLanguage()!=SrcLangExt_Fortran)
      {
        for (const auto &mg : m_classDef->getMemberGroups())
        {
          if (!mg->members().empty())
          {
            writeBoxMemberList(t,'*',&mg->members(),m_classDef,FALSE,&arrowNames);
          }
        }
      }
    }
    t << "}\"";
  }
  else if (Config_getString(DOT_NODE_ATTR).contains("shape=plain"))
  {
    if (m_isRoot)
      t << "<<b>" << convertToXML(m_label) << "</b>>";
    else if (m_truncated == Truncated)
      t << "<<i>" << convertToXML(m_label) << "</i>>";
    else
      t << '"' << convertLabel(m_label) << '"';
  }
  else // standard look
  {
    t << '"' << convertLabel(m_label) << '"';
  }
}

void DotNode::writeUrl(TextStream &t) const
{
  if (m_url.isEmpty())
    return;
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
    t << addHtmlExtensionIfMissing(noTagURL) << "\"";
  }
  else
  {
    t << addHtmlExtensionIfMissing(noTagURL.left(anchorPos))
      << noTagURL.right(noTagURL.length() - anchorPos) << "\"";
  }
}

void DotNode::writeBox(TextStream &t,
                       GraphType gt,
                       GraphOutputFormat /*format*/,
                       bool hasNonReachableChildren) const
{
  const char *labCol;
  if (m_classDef)
  {
    if (m_classDef->hasDocumentation() && hasNonReachableChildren)
      labCol = "red";
    else if (m_classDef->hasDocumentation() && !hasNonReachableChildren)
      labCol = "black";
    else if (!m_classDef->hasDocumentation() && hasNonReachableChildren)
      labCol = "orangered";
    else // (!m_classDef->hasDocumentation() && !hasNonReachableChildren)
    {
      labCol = "grey75";
      if (m_classDef->templateMaster() && m_classDef->templateMaster()->hasDocumentation())
        labCol = "black";
    }
  }
  else
  {
    labCol = m_url.isEmpty() ? "grey75" :  // non link
    (hasNonReachableChildren ? "red" : "black");
  }
  t << "  Node" << m_number << " [";
  writeLabel(t,gt);
  t << ",height=0.2,width=0.4";
  if (m_isRoot)
  {
    t << ",color=\"black\", fillcolor=\"grey75\", style=\"filled\", fontcolor=\"black\"";
  }
  else
  {
    t << ",color=\"" << labCol << "\"";
    if (!Config_getBool(DOT_TRANSPARENT))
    {
      t << ", fillcolor=\"white\"";
      t << ", style=\"filled\"";
    }
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
                         GraphOutputFormat format,
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

  if (pointBack && !umlUseArrow) t << "dir=\"back\",";
  t << "color=\"" << eProps->edgeColorMap[ei->color()] << "\",";
  t << "style=\"" << eProps->edgeStyleMap[ei->style()] << "\"";
  if (!ei->label().isEmpty())
  {
    t << ",label=\" " << convertLabel(ei->label()) << "\" ";
  }
  if (Config_getBool(UML_LOOK) &&
    eProps->arrowStyleMap[ei->color()] &&
    (gt==Inheritance || gt==Collaboration)
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
                    bool backArrows) const
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
      int ni;
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
      int ni;
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




