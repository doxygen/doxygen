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

#include "ftextstream.h"
#include "classdef.h"
#include "config.h"
#include "memberlist.h"
#include "membergroup.h"
#include "language.h"
#include "doxygen.h"
#include "util.h"

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
  QCString result;
  const char *p=tooltip.data();
  if (p==0) return result;
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

static void writeBoxMemberList(FTextStream &t,
  char prot,MemberList *ml,const ClassDef *scope,
  bool isStatic=FALSE,const StringUnorderedSet *skipNames=nullptr)
{
  if (ml)
  {
    MemberListIterator mlia(*ml);
    MemberDef *mma;
    int totalCount=0;
    for (mlia.toFirst();(mma = mlia.current());++mlia)
    {
      if (mma->getClassDef()==scope &&
        (skipNames==nullptr || skipNames->find(mma->name().str())==std::end(*skipNames)))
      {
        totalCount++;
      }
    }

    int count=0;
    for (mlia.toFirst();(mma = mlia.current());++mlia)
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
          t << DotNode::convertLabel(mma->typeString());
          t << " ";
          t << DotNode::convertLabel(mma->name());
          if (!mma->isObjCMethod() &&
            (mma->isFunction() || mma->isSlot() || mma->isSignal())) t << DotNode::convertLabel(mma->argsString());
          t << "\\l";
          count++;
        }
      }
    }
    // write member groups within the memberlist
    MemberGroupList *mgl = ml->getMemberGroupList();
    if (mgl)
    {
      MemberGroupListIterator mgli(*mgl);
      MemberGroup *mg;
      for (mgli.toFirst();(mg=mgli.current());++mgli)
      {
        if (mg->members())
        {
          writeBoxMemberList(t,prot,mg->members(),scope,isStatic,skipNames);
        }
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
  int foldLen=117; // ideal text length
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

DotNode::DotNode(int n,const char *lab,const char *tip, const char *url,
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
  delete m_children;
  delete m_parents;
  delete m_edgeInfo;
}

void DotNode::addChild(DotNode *n,
  int edgeColor,
  int edgeStyle,
  const char *edgeLab,
  const char *edgeURL,
  int edgeLabCol
)
{
  if (m_children==0)
  {
    m_children = new QList<DotNode>;
    m_edgeInfo = new QList<EdgeInfo>;
    m_edgeInfo->setAutoDelete(TRUE);
  }
  m_children->append(n);
  EdgeInfo *ei = new EdgeInfo(
      edgeColor,
      edgeStyle,
      edgeLab,
      edgeURL,
      edgeLabCol==-1 ? edgeColor : edgeLabCol);
  m_edgeInfo->append(ei);
}

void DotNode::addParent(DotNode *n)
{
  if (m_parents==0)
  {
    m_parents = new QList<DotNode>;
  }
  m_parents->append(n);
}

void DotNode::removeChild(DotNode *n)
{
  if (m_children) m_children->remove(n);
}

void DotNode::removeParent(DotNode *n)
{
  if (m_parents) m_parents->remove(n);
}

void DotNode::deleteNode(DotNodeList &deletedList,SDict<DotNode> *skipNodes)
{
  if (m_deleted) return; // avoid recursive loops in case the graph has cycles
  m_deleted=TRUE;
  if (m_parents!=0) // delete all parent nodes of this node
  {
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      //pn->removeChild(this);
      pn->deleteNode(deletedList,skipNodes);
    }
  }
  if (m_children!=0) // delete all child nodes of this node
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      //cn->removeParent(this);
      cn->deleteNode(deletedList,skipNodes);
    }
  }
  // add this node to the list of deleted nodes.
  //printf("skipNodes=%p find(%p)=%p\n",skipNodes,this,skipNodes ? skipNodes->find((int)this) : 0);
  if (skipNodes==0 || skipNodes->find((char*)this)==0)
  {
    //printf("deleting\n");
    deletedList.append(this);
  }
}

void DotNode::setDistance(int distance)
{
  if (distance<m_distance) m_distance = distance;
}

inline int DotNode::findParent( DotNode *n )
{
  if ( !m_parents ) return -1;
  return m_parents->find(n);
}

/*! helper function that deletes all nodes in a connected graph, given
*  one of the graph's nodes
*/
void DotNode::deleteNodes(DotNode *node,SDict<DotNode> *skipNodes)
{
  //printf("deleteNodes skipNodes=%p\n",skipNodes);
  static DotNodeList deletedNodes;
  deletedNodes.setAutoDelete(TRUE);
  node->deleteNode(deletedNodes,skipNodes); // collect nodes to be deleted.
  deletedNodes.clear(); // actually remove the nodes.
}

void DotNode::writeBox(FTextStream &t,
                       GraphType gt,
                       GraphOutputFormat /*format*/,
                       bool hasNonReachableChildren) const
{
  const char *labCol =
    m_url.isEmpty() ? "grey75" :  // non link
    (hasNonReachableChildren ? "red" : "black");
  t << "  Node" << m_number << " [label=\"";

  if (m_classDef && Config_getBool(UML_LOOK) && (gt==Inheritance || gt==Collaboration))
  {
    // add names shown as relations to a set, so we don't show
    // them as attributes as well
    StringUnorderedSet arrowNames;
    if (m_edgeInfo)
    {
      // for each edge
      QListIterator<EdgeInfo> li(*m_edgeInfo);
      EdgeInfo *ei;
      for (li.toFirst();(ei=li.current());++li)
      {
        if (!ei->label().isEmpty()) // labels joined by \n
        {
          int i=ei->label().find('\n');
          int p=0;
          QCString lab;
          while ((i=ei->label().find('\n',p))!=-1)
          {
            lab = stripProtectionPrefix(ei->label().mid(p,i-p));
            arrowNames.insert(lab.str());
            p=i+1;
          }
          lab = stripProtectionPrefix(ei->label().right(ei->label().length()-p));
          arrowNames.insert(lab.str());
        }
      }
    }

    //printf("DotNode::writeBox for %s\n",m_classDef->name().data());
    t << "{" << convertLabel(m_label);
    t << "\\n|";
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
    if (m_classDef->getLanguage()!=SrcLangExt_Fortran &&
      m_classDef->getMemberGroupSDict())
    {
      MemberGroupSDict::Iterator mgdi(*m_classDef->getMemberGroupSDict());
      MemberGroup *mg;
      for (mgdi.toFirst();(mg=mgdi.current());++mgdi)
      {
        if (mg->members())
        {
          writeBoxMemberList(t,'*',mg->members(),m_classDef,FALSE,&arrowNames);
        }
      }
    }
    t << "}";
  }
  else // standard look
  {
    t << convertLabel(m_label);
  }
  t << "\",height=0.2,width=0.4";
  if (m_isRoot)
  {
    t << ",color=\"black\", fillcolor=\"grey75\", style=\"filled\", fontcolor=\"black\"";
  }
  else
  {
    if (!Config_getBool(DOT_TRANSPARENT))
    {
      t << ",color=\"" << labCol << "\", fillcolor=\"";
      t << "white";
      t << "\", style=\"filled\"";
    }
    else
    {
      t << ",color=\"" << labCol << "\"";
    }
    if (!m_url.isEmpty())
    {
      int anchorPos = m_url.findRev('#');
      if (anchorPos==-1)
      {
        t << ",URL=\"" << m_url << Doxygen::htmlFileExtension << "\"";
      }
      else
      {
        t << ",URL=\"" << m_url.left(anchorPos) << Doxygen::htmlFileExtension
          << m_url.right(m_url.length()-anchorPos) << "\"";
      }
    }
  }
  if (!m_tooltip.isEmpty())
  {
    t << ",tooltip=\"" << escapeTooltip(m_tooltip) << "\"";
  }
  else
  {
    t << ",tooltip=\" \""; // space in tooltip is required otherwise still something like 'Node0' is used
  }
  t << "];" << endl;
}

void DotNode::writeArrow(FTextStream &t,
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
  t << "color=\"" << eProps->edgeColorMap[ei->color()]
    << "\",fontsize=\"" << Config_getInt(DOT_FONTSIZE) << "\",";
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

  if (format==GOF_BITMAP) t << ",fontname=\"" << Config_getString(DOT_FONTNAME) << "\"";
  t << "];" << endl;
}

void DotNode::write(FTextStream &t,
                    GraphType gt,
                    GraphOutputFormat format,
                    bool topDown,
                    bool toChildren,
                    bool backArrows) const
{
  //printf("DotNode::write(%d) name=%s this=%p written=%d visible=%d\n",m_distance,m_label.data(),this,m_written,m_visible);
  if (m_written) return; // node already written to the output
  if (!m_visible) return; // node is not visible
  writeBox(t,gt,format,m_truncated==Truncated);
  m_written=TRUE;
  QList<DotNode> *nl = toChildren ? m_children : m_parents;
  if (nl)
  {
    if (toChildren)
    {
      QListIterator<DotNode>  dnli1(*nl);
      QListIterator<EdgeInfo> dnli2(*m_edgeInfo);
      const DotNode *cn;
      for (dnli1.toFirst();(cn=dnli1.current());++dnli1,++dnli2)
      {
        if (cn->isVisible())
        {
          //printf("write arrow %s%s%s\n",label().data(),backArrows?"<-":"->",cn->label().data());
          writeArrow(t,gt,format,cn,dnli2.current(),topDown,backArrows);
        }
        cn->write(t,gt,format,topDown,toChildren,backArrows);
      }
    }
    else // render parents
    {
      QListIterator<DotNode> dnli(*nl);
      DotNode *pn;
      for (dnli.toFirst();(pn=dnli.current());++dnli)
      {
        if (pn->isVisible())
        {
          //printf("write arrow %s%s%s\n",label().data(),backArrows?"<-":"->",pn->label().data());
          writeArrow(t,
            gt,
            format,
            pn,
            pn->edgeInfo()->at(pn->children()->findRef(this)),
            FALSE,
            backArrows
          );
        }
        pn->write(t,gt,format,TRUE,FALSE,backArrows);
      }
    }
  }
  //printf("end DotNode::write(%d) name=%s\n",distance,m_label.data());
}

void DotNode::writeXML(FTextStream &t,bool isClassGraph) const
{
  t << "      <node id=\"" << m_number << "\">" << endl;
  t << "        <label>" << convertToXML(m_label) << "</label>" << endl;
  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    const char *refPtr = url.data();
    char *urlPtr = strchr(url.rawData(),'$');
    if (urlPtr)
    {
      *urlPtr++='\0';
      t << "        <link refid=\"" << convertToXML(urlPtr) << "\"";
      if (*refPtr!='\0')
      {
        t << " external=\"" << convertToXML(refPtr) << "\"";
      }
      t << "/>" << endl;
    }
  }
  if (m_children)
  {
    QListIterator<DotNode> nli(*m_children);
    QListIterator<EdgeInfo> eli(*m_edgeInfo);
    DotNode *childNode;
    EdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        <childnode refid=\"" << childNode->number() << "\" relation=\"";
      if (isClassGraph)
      {
        switch(edgeInfo->color())
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
      t << "\">" << endl;
      if (!edgeInfo->label().isEmpty())
      {
        int p=0;
        int ni;
        while ((ni=edgeInfo->label().find('\n',p))!=-1)
        {
          t << "          <edgelabel>"
            << convertToXML(edgeInfo->label().mid(p,ni-p))
            << "</edgelabel>" << endl;
          p=ni+1;
        }
        t << "          <edgelabel>"
          << convertToXML(edgeInfo->label().right(edgeInfo->label().length()-p))
          << "</edgelabel>" << endl;
      }
      t << "        </childnode>" << endl;
    }
  }
  t << "      </node>" << endl;
}

void DotNode::writeDocbook(FTextStream &t,bool isClassGraph) const
{
  t << "      <node id=\"" << m_number << "\">" << endl;
  t << "        <label>" << convertToXML(m_label) << "</label>" << endl;
  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    const char *refPtr = url.data();
    char *urlPtr = strchr(url.rawData(),'$');
    if (urlPtr)
    {
      *urlPtr++='\0';
      t << "        <link refid=\"" << convertToXML(urlPtr) << "\"";
      if (*refPtr!='\0')
      {
        t << " external=\"" << convertToXML(refPtr) << "\"";
      }
      t << "/>" << endl;
    }
  }
  if (m_children)
  {
    QListIterator<DotNode> nli(*m_children);
    QListIterator<EdgeInfo> eli(*m_edgeInfo);
    DotNode *childNode;
    EdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        <childnode refid=\"" << childNode->number() << "\" relation=\"";
      if (isClassGraph)
      {
        switch(edgeInfo->color())
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
      t << "\">" << endl;
      if (!edgeInfo->label().isEmpty())
      {
        int p=0;
        int ni;
        while ((ni=edgeInfo->label().find('\n',p))!=-1)
        {
          t << "          <edgelabel>"
            << convertToXML(edgeInfo->label().mid(p,ni-p))
            << "</edgelabel>" << endl;
          p=ni+1;
        }
        t << "          <edgelabel>"
          << convertToXML(edgeInfo->label().right(edgeInfo->label().length()-p))
          << "</edgelabel>" << endl;
      }
      t << "        </childnode>" << endl;
    }
  }
  t << "      </node>" << endl;
}


void DotNode::writeDEF(FTextStream &t) const
{
  const char* nodePrefix = "        node-";

  t << "      node = {" << endl;
  t << nodePrefix << "id    = " << m_number << ';' << endl;
  t << nodePrefix << "label = '" << m_label << "';" << endl;

  if (!m_url.isEmpty())
  {
    QCString url(m_url);
    const char *refPtr = url.data();
    char *urlPtr = strchr(url.rawData(),'$');
    if (urlPtr)
    {
      *urlPtr++='\0';
      t << nodePrefix << "link = {" << endl << "  "
        << nodePrefix << "link-id = '" << urlPtr << "';" << endl;

      if (*refPtr!='\0')
      {
        t << "  " << nodePrefix << "link-external = '"
          << refPtr << "';" << endl;
      }
      t << "        };" << endl;
    }
  }
  if (m_children)
  {
    QListIterator<DotNode> nli(*m_children);
    QListIterator<EdgeInfo> eli(*m_edgeInfo);
    DotNode *childNode;
    EdgeInfo *edgeInfo;
    for (;(childNode=nli.current());++nli,++eli)
    {
      edgeInfo=eli.current();
      t << "        node-child = {" << endl;
      t << "          child-id = '" << childNode->number() << "';" << endl;
      t << "          relation = ";

      switch(edgeInfo->color())
      {
      case EdgeInfo::Blue:    t << "public-inheritance"; break;
      case EdgeInfo::Green:   t << "protected-inheritance"; break;
      case EdgeInfo::Red:     t << "private-inheritance"; break;
      case EdgeInfo::Purple:  t << "usage"; break;
      case EdgeInfo::Orange:  t << "template-instance"; break;
      case EdgeInfo::Orange2: t << "type-constraint"; break;
      case EdgeInfo::Grey:    ASSERT(0); break;
      }
      t << ';' << endl;

      if (!edgeInfo->label().isEmpty())
      {
        t << "          edgelabel = <<_EnD_oF_dEf_TeXt_" << endl
          << edgeInfo->label() << endl
          << "_EnD_oF_dEf_TeXt_;" << endl;
      }
      t << "        }; /* node-child */" << endl;
    } /* for (;childNode...) */
  }
  t << "      }; /* node */" << endl;
}


void DotNode::clearWriteFlag()
{
  m_written=FALSE;
  if (m_parents!=0)
  {
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      if (pn->isWritten())
      {
        pn->clearWriteFlag();
      }
    }
  }
  if (m_children!=0)
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      if (cn->isWritten())
      {
        cn->clearWriteFlag();
      }
    }
  }
}

void DotNode::colorConnectedNodes(int curColor)
{
  if (m_children)
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      if (cn->subgraphId()==-1) // uncolored child node
      {
        cn->setSubgraphId(curColor);
        cn->markAsVisible();
        cn->colorConnectedNodes(curColor);
        //printf("coloring node %s (%p): %d\n",cn->label().data(),cn,cn->subgraphId());
      }
    }
  }

  if (m_parents)
  {
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      if (pn->subgraphId()==-1) // uncolored parent node
      {
        pn->setSubgraphId(curColor);
        pn->markAsVisible();
        pn->colorConnectedNodes(curColor);
        //printf("coloring node %s (%p): %d\n",pn->label().data(),pn,pn->subgraphId());
      }
    }
  }
}

void DotNode::renumberNodes(int &number)
{
  m_number = number++;
  if (m_children)
  {
    QListIterator<DotNode> dnlic(*m_children);
    DotNode *cn;
    for (dnlic.toFirst();(cn=dnlic.current());++dnlic)
    {
      if (!cn->isRenumbered())
      {
        cn->markRenumbered();
        cn->renumberNodes(number);
      }
    }
  }
}

const DotNode *DotNode::findDocNode() const
{
  if (!m_url.isEmpty()) return this;
  //printf("findDocNode(): '%s'\n",m_label.data());
  if (m_parents)
  {
    QListIterator<DotNode> dnli(*m_parents);
    DotNode *pn;
    for (dnli.toFirst();(pn=dnli.current());++dnli)
    {
      if (!pn->hasDocumentation())
      {
        pn->markHasDocumentation();
        const DotNode *dn = pn->findDocNode();
        if (dn) return dn;
      }
    }
  }
  if (m_children)
  {
    QListIterator<DotNode> dnli(*m_children);
    DotNode *cn;
    for (dnli.toFirst();(cn=dnli.current());++dnli)
    {
      if (!cn->hasDocumentation())
      {
        cn->markHasDocumentation();
        const DotNode *dn = cn->findDocNode();
        if (dn) return dn;
      }
    }
  }
  return 0;
}

//--------------------------------------------------------------

int DotNodeList::compareValues(const DotNode *n1,const DotNode *n2) const
{
  return qstricmp(n1->label(),n2->label());
}



