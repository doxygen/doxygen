
#include "dotnode.h"

#include "ftextstream.h"
#include "classdef.h"
#include "config.h"
#include "memberlist.h"
#include "membergroup.h"
#include "language.h"
#include "doxygen.h"
#include "util.h"

#define UML_LIMIT_NUM_FIELDS  Config_getInt(UML_LIMIT_NUM_FIELDS)
#define UML_LOOK              Config_getBool(UML_LOOK)
#define EXTRACT_PRIVATE       Config_getBool(EXTRACT_PRIVATE)
#define DOT_TRANSPARENT       Config_getBool(DOT_TRANSPARENT)

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
  bool isStatic=FALSE,const QDict<void> *skipNames=0)
{
  (void)isStatic;
  if (ml)
  {
    MemberListIterator mlia(*ml);
    MemberDef *mma;
    int totalCount=0;
    for (mlia.toFirst();(mma = mlia.current());++mlia)
    {
      if (mma->getClassDef()==scope && 
        (skipNames==0 || skipNames->find(mma->name())==0))
      {
        totalCount++;
      }
    }

    int count=0;
    for (mlia.toFirst();(mma = mlia.current());++mlia)
    {
      if (mma->getClassDef() == scope &&
        (skipNames==0 || skipNames->find(mma->name())==0))
      {
        if (UML_LIMIT_NUM_FIELDS>0 && (totalCount>UML_LIMIT_NUM_FIELDS*3/2 && count>=UML_LIMIT_NUM_FIELDS))
        {
          t << theTranslator->trAndMore(QCString().sprintf("%d",totalCount-count)) << "\\l";
          break;
        }
        else
        {
          t << prot << " ";
          t << DotNode::convertLabel(mma->name());
          if (!mma->isObjCMethod() && 
            (mma->isFunction() || mma->isSlot() || mma->isSignal())) t << "()";
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
  QString bBefore("\\_/<({[: =-+@%#~?$"); // break before character set
  QString bAfter(">]),:;|");              // break after  character set
  QString p(l);
  if (p.isEmpty()) return QCString();
  QString result;
  QChar c,pc=0;
  uint idx = 0;
  int len=p.length();
  int charsLeft=len;
  int sinceLast=0;
  int foldLen=17; // ideal text length
  while (idx < p.length())
  {
    c = p[idx++];
    QString replacement;
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
    default:   replacement=c; break;
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
      !isupper(c) && p[idx].category()==QChar::Letter_Uppercase)
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
  return result.utf8();
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
  : m_subgraphId(-1)
  , m_number(n)
  , m_label(lab)
  , m_tooltip(tip)
  , m_url(url)
  , m_parents(0)
  , m_children(0)
  , m_edgeInfo(0)
  , m_deleted(FALSE)
  , m_written(FALSE)
  , m_hasDoc(FALSE)
  , m_isRoot(isRoot)
  , m_classDef(cd)
  , m_visible(FALSE)
  , m_truncated(Unknown)
  , m_distance(1000)
  , m_renumbered(false)
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
  EdgeInfo *ei = new EdgeInfo;
  ei->m_color = edgeColor;
  ei->m_style = edgeStyle; 
  ei->m_label = edgeLab;
  ei->m_url   = edgeURL;
  if (edgeLabCol==-1)
    ei->m_labColor=edgeColor;
  else
    ei->m_labColor=edgeLabCol;
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
  bool hasNonReachableChildren
)
{
  const char *labCol = 
    m_url.isEmpty() ? "grey75" :  // non link
    (
    (hasNonReachableChildren) ? "red" : "black"
      );
  t << "  Node" << m_number << " [label=\"";

  if (m_classDef && UML_LOOK && (gt==Inheritance || gt==Collaboration))
  {
    // add names shown as relations to a dictionary, so we don't show
    // them as attributes as well
    QDict<void> arrowNames(17);
    if (m_edgeInfo)
    {
      // for each edge
      QListIterator<EdgeInfo> li(*m_edgeInfo);
      EdgeInfo *ei;
      for (li.toFirst();(ei=li.current());++li)
      {
        if (!ei->m_label.isEmpty()) // labels joined by \n
        {
          int li=ei->m_label.find('\n');
          int p=0;
          QCString lab;
          while ((li=ei->m_label.find('\n',p))!=-1)
          {
            lab = stripProtectionPrefix(ei->m_label.mid(p,li-p));
            arrowNames.insert(lab,(void*)0x8);
            p=li+1;
          }
          lab = stripProtectionPrefix(ei->m_label.right(ei->m_label.length()-p));
          arrowNames.insert(lab,(void*)0x8);
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
    if (EXTRACT_PRIVATE)
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
    if (EXTRACT_PRIVATE)
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
    if (!DOT_TRANSPARENT)
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
  DotNode *cn,
  EdgeInfo *ei,
  bool topDown,
  bool pointBack
)
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

  const EdgeProperties *eProps = UML_LOOK ? &umlEdgeProps : &normalEdgeProps;
  QCString aStyle = eProps->arrowStyleMap[ei->m_color];
  bool umlUseArrow = aStyle=="odiamond";

  if (pointBack && !umlUseArrow) t << "dir=\"back\",";
  t << "color=\"" << eProps->edgeColorMap[ei->m_color] 
    << "\",fontsize=\"" << DotGraph::DOT_FONTSIZE << "\",";
  t << "style=\"" << eProps->edgeStyleMap[ei->m_style] << "\"";
  if (!ei->m_label.isEmpty())
  {
    t << ",label=\" " << convertLabel(ei->m_label) << "\" ";
  }
  if (UML_LOOK &&
    eProps->arrowStyleMap[ei->m_color] && 
    (gt==Inheritance || gt==Collaboration)
    )
  {
    bool rev = pointBack;
    if (umlUseArrow) rev=!rev; // UML use relates has arrow on the start side
    if (rev) 
      t << ",arrowtail=\"" << eProps->arrowStyleMap[ei->m_color] << "\""; 
    else 
      t << ",arrowhead=\"" << eProps->arrowStyleMap[ei->m_color] << "\"";
  }

  if (format==GOF_BITMAP) t << ",fontname=\"" << DotGraph::DOT_FONTNAME << "\"";
  t << "];" << endl; 
}

void DotNode::write(FTextStream &t,
  GraphType gt,
  GraphOutputFormat format,
  bool topDown,
  bool toChildren,
  bool backArrows
)
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
      DotNode *cn;
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
            pn->m_edgeInfo->at(pn->m_children->findRef(this)),
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

void DotNode::writeXML(FTextStream &t,bool isClassGraph)
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
      t << "        <childnode refid=\"" << childNode->m_number << "\" relation=\"";
      if (isClassGraph)
      {
        switch(edgeInfo->m_color)
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
      if (!edgeInfo->m_label.isEmpty()) 
      {
        int p=0;
        int ni;
        while ((ni=edgeInfo->m_label.find('\n',p))!=-1)
        {
          t << "          <edgelabel>" 
            << convertToXML(edgeInfo->m_label.mid(p,ni-p))
            << "</edgelabel>" << endl;
          p=ni+1;
        }
        t << "          <edgelabel>" 
          << convertToXML(edgeInfo->m_label.right(edgeInfo->m_label.length()-p))
          << "</edgelabel>" << endl;
      }
      t << "        </childnode>" << endl;
    } 
  }
  t << "      </node>" << endl;
}

void DotNode::writeDocbook(FTextStream &t,bool isClassGraph)
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
      t << "        <childnode refid=\"" << childNode->m_number << "\" relation=\"";
      if (isClassGraph)
      {
        switch(edgeInfo->m_color)
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
      if (!edgeInfo->m_label.isEmpty())
      {
        int p=0;
        int ni;
        while ((ni=edgeInfo->m_label.find('\n',p))!=-1)
        {
          t << "          <edgelabel>"
            << convertToXML(edgeInfo->m_label.mid(p,ni-p))
            << "</edgelabel>" << endl;
          p=ni+1;
        }
        t << "          <edgelabel>"
          << convertToXML(edgeInfo->m_label.right(edgeInfo->m_label.length()-p))
          << "</edgelabel>" << endl;
      }
      t << "        </childnode>" << endl;
    }
  }
  t << "      </node>" << endl;
}


void DotNode::writeDEF(FTextStream &t)
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
      t << "          child-id = '" << childNode->m_number << "';" << endl;
      t << "          relation = ";

      switch(edgeInfo->m_color)
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

      if (!edgeInfo->m_label.isEmpty()) 
      {
        t << "          edgelabel = <<_EnD_oF_dEf_TeXt_" << endl
          << edgeInfo->m_label << endl
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
      if (pn->m_written)
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
      if (cn->m_written)
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
      if (cn->m_subgraphId==-1) // uncolored child node
      {
        cn->m_subgraphId=curColor;
        cn->markAsVisible();
        cn->colorConnectedNodes(curColor);
        //printf("coloring node %s (%p): %d\n",cn->m_label.data(),cn,cn->m_subgraphId);
      }
    }
  }

  if (m_parents)
  {
    QListIterator<DotNode> dnlip(*m_parents);
    DotNode *pn;
    for (dnlip.toFirst();(pn=dnlip.current());++dnlip)
    {
      if (pn->m_subgraphId==-1) // uncolored parent node
      {
        pn->m_subgraphId=curColor;
        pn->markAsVisible();
        pn->colorConnectedNodes(curColor);
        //printf("coloring node %s (%p): %d\n",pn->m_label.data(),pn,pn->m_subgraphId);
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
      if (!cn->m_renumbered)
      {
        cn->m_renumbered = true;
        cn->renumberNodes(number);
      }
    }
  }
}

const DotNode *DotNode::findDocNode() const
{
  if (!m_url.isEmpty()) return this;
  //printf("findDocNode(): `%s'\n",m_label.data());
  if (m_parents)
  {
    QListIterator<DotNode> dnli(*m_parents);
    DotNode *pn;
    for (dnli.toFirst();(pn=dnli.current());++dnli)
    {
      if (!pn->m_hasDoc)
      {
        pn->m_hasDoc=TRUE;
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
      if (!cn->m_hasDoc)
      {
        cn->m_hasDoc=TRUE;
        const DotNode *dn = cn->findDocNode();
        if (dn) return dn;
      }
    }
  }
  return 0;
}
