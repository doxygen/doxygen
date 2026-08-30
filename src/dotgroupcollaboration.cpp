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

// own header
#include "dotgroupcollaboration.h"

// other includes
#include "classlist.h"
#include "config.h"
#include "doxygen.h"
#include "filedef.h"
#include "groupdef.h"
#include "namespacedef.h"
#include "pagedef.h"
#include "textstream.h"
#include "util.h"

DotGroupCollaboration::DotGroupCollaboration(const GroupDef* gd)
{
  DString tmp_url = gd->getReference()+"$"+gd->getOutputFileBase();
  DString tooltip = gd->briefDescriptionAsTooltip();
  m_rootNode = new DotNode(this, gd->groupTitle(), tooltip, tmp_url, true );
  m_rootNode->markAsVisible();
  m_usedNodes.emplace(gd->name().str(), m_rootNode);

  m_diskName = gd->getOutputFileBase();

  buildGraph( gd );
}

DotGroupCollaboration::~DotGroupCollaboration()
{
  // delete all created Nodes saved in m_usedNodes map
  for (const auto &[name,node] : m_usedNodes)
  {
    delete node;
  }
}

static void makeURL(const Definition *def,DString &url)
{
  DString fn = def->getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  url = def->getReference()+"$"+fn;
  if (!def->anchor().empty())
  {
    url+="#"+def->anchor();
  }
}

void DotGroupCollaboration::buildGraph(const GroupDef* gd)
{
  DString url;
  //===========================
  // hierarchy.

  // Write parents
  for (const auto &d : gd->partOfGroups())
  {
    DotNode *nnode = nullptr;
    auto it = m_usedNodes.find(d->name().str());
    if ( it==m_usedNodes.end())
    { // add node
      makeURL(d,url);
      DString tooltip = d->briefDescriptionAsTooltip();
      nnode = new DotNode(this, d->groupTitle(), tooltip, url );
      nnode->markAsVisible();
      m_usedNodes.emplace(d->name().str(), nnode);
    }
    else
    {
      nnode = it->second;
    }
    url = "";
    addEdge( nnode, m_rootNode, DotGroupCollaboration::thierarchy, url, url );
  }

  // Add subgroups
  for (const auto &def : gd->getSubGroups())
  {
    DotNode *nnode = nullptr;
    auto it = m_usedNodes.find(def->name().str());
    if ( it==m_usedNodes.end())
    { // add node
      makeURL(def,url);
      DString tooltip = def->briefDescriptionAsTooltip();
      nnode = new DotNode(this, def->groupTitle(), tooltip, url );
      nnode->markAsVisible();
      m_usedNodes.emplace(def->name().str(), nnode);
    }
    else
    {
      nnode = it->second;
    }
    url = "";
    addEdge( m_rootNode, nnode, DotGroupCollaboration::thierarchy, url, url );
  }

  //=======================
  // Write collaboration

  // Add members
  addMemberList( gd->getMemberList(MemberListType::AllMembersList()) );

  // Add classes
  for (const auto &def : gd->getClasses())
  {
    makeURL(def,url);
    addCollaborationMember( def, url, DotGroupCollaboration::tclass );
  }

  // Add namespaces
  for (const auto &def : gd->getNamespaces())
  {
    makeURL(def,url);
    addCollaborationMember( def, url, DotGroupCollaboration::tnamespace );
  }

  // Add files
  for (const auto &def : gd->getFiles())
  {
    makeURL(def,url);
    addCollaborationMember( def, url, DotGroupCollaboration::tfile );
  }

  // Add pages
  for (const auto &def : gd->getPages())
  {
    makeURL(def,url);
    addCollaborationMember( def, url, DotGroupCollaboration::tpages );
  }

  // Add directories
  if ( !gd->getDirs().empty() )
  {
    for(const auto &def : gd->getDirs())
    {
      makeURL(def,url);
      addCollaborationMember( def, url, DotGroupCollaboration::tdir );
    }
  }
}

void DotGroupCollaboration::addMemberList( MemberList* ml )
{
  DString url;
  if ( ml==nullptr || ml->empty() ) return;
  for (const auto &def : *ml)
  {
    makeURL(def,url);
    addCollaborationMember( def, url, DotGroupCollaboration::tmember );
  }
}

DotGroupCollaboration::Edge* DotGroupCollaboration::addEdge(
  DotNode* _pNStart, DotNode* _pNEnd, EdgeType _eType,
  const DString& _label, const DString& _url )
{
  // search a existing link.
  auto it = std::find_if(m_edges.begin(),m_edges.end(),
      [&_pNStart,&_pNEnd,&_eType](const auto &edge)
      { return edge->pNStart==_pNStart && edge->pNEnd==_pNEnd && edge->eType==_eType; });

  if (it==m_edges.end()) // new link
  {
    m_edges.emplace_back(std::make_unique<Edge>(_pNStart,_pNEnd,_eType));
    it = m_edges.end()-1;
  }

  if (!_label.empty()) // add label
  {
    (*it)->links.emplace_back(_label,_url);
  }

  // return found or added edge
  return (*it).get();
}

void DotGroupCollaboration::addCollaborationMember(
  const Definition* def, DString& url, EdgeType eType )
{
  // Create group nodes
  DString tmp_str;
  for (const auto &d : def->partOfGroups())
  {
    auto it = m_usedNodes.find(d->name().str());
    DotNode* nnode = it!=m_usedNodes.end() ? it->second : nullptr;
    if ( nnode != m_rootNode )
    {
      if ( nnode==nullptr )
      { // add node
        tmp_str = d->getReference()+"$"+d->getOutputFileBase();
        DString tooltip = d->briefDescriptionAsTooltip();
        nnode = new DotNode(this, d->groupTitle(), tooltip, tmp_str );
        nnode->markAsVisible();
        m_usedNodes.emplace(d->name().str(), nnode);
      }
      tmp_str = def->qualifiedName();
      addEdge( m_rootNode, nnode, eType, tmp_str, url );
    }
  }
}

DString DotGroupCollaboration::getBaseName() const
{
  return m_diskName;
}

void DotGroupCollaboration::computeTheGraph()
{
  TextStream md5stream;
  writeGraphHeader(md5stream,m_rootNode->label());

  // clean write flags
  for (const auto &[name,node] : m_usedNodes)
  {
    node->clearWriteFlag();
  }

  // write other nodes.
  for (const auto &[name,node] : m_usedNodes)
  {
    node->write(md5stream,GraphType::Inheritance,m_graphFormat,true,false,false);
  }

  // write edges
  for (const auto &edge : m_edges)
  {
    edge->write( md5stream );
  }

  writeGraphFooter(md5stream);

  m_theGraph = md5stream.str();
}

DString DotGroupCollaboration::getMapLabel() const
{
  return escapeCharsInString(m_baseName, false);
}

DString DotGroupCollaboration::writeGraph( TextStream &t,
  GraphOutputFormat graphFormat, EmbeddedOutputFormat textFormat,
  const DString &path, const DString &fileName, const DString &relPath,
  bool generateImageMap,int graphId)
{
  m_doNotAddImageToIndex = textFormat!=EmbeddedOutputFormat::Html;

  return DotGraph::writeGraph(t, graphFormat, textFormat, path, fileName, relPath, generateImageMap, graphId);
}

void DotGroupCollaboration::Edge::write( TextStream &t ) const
{
  const char* linkTypeColor[] = {
    "darkorchid3"
    ,"orange"
    ,"blueviolet"
    ,"darkgreen"
    ,"firebrick4"
    ,"grey75"
    ,"midnightblue"
  };
  DString arrowStyle = "dir=\"none\", style=\"dashed\"";
  t << "  Node" << pNStart->number();
  t << "->";
  t << "Node" << pNEnd->number();

  t << " [shape=plaintext";
  if (!links.empty()) // there are links
  {
    t << ", ";
    // HTML-like edge labels crash on my Mac with Graphviz 2.0! and
    // are not supported by older version of dot.
    //
    //t << label=<<TABLE BORDER=\"0\" CELLBORDER=\"0\">";
    //for (const auto &link : links)
    //{
    //  t << "<TR><TD";
    //  if ( !link.url.empty() )
    //    t << " HREF=\"" << link.url << "\"";
    //  t << ">" << DotNode::convertLabel(link->label) << "</TD></TR>";
    //}
    //t << "</TABLE>>";

    t << "label=\"";
    bool first=true;
    int count=0;
    const int maxLabels = 10;
    for (const auto &link : links)
    {
      if (first) first=false; else t << "\\n";
      t << DotNode::convertLabel(link.label);
      count++;
    }
    if (count==maxLabels) t << "\\n...";
    t << "\"";

  }
  switch( eType )
  {
    case thierarchy:
      arrowStyle = "dir=\"back\", style=\"solid\"";
      break;
    default:
      t << ", color=\"" << linkTypeColor[static_cast<int>(eType)] << "\"";
      break;
  }
  t << ", " << arrowStyle;
  t << "];\n";
}

bool DotGroupCollaboration::isTrivial() const
{
  return m_usedNodes.size() <= 1;
}

bool DotGroupCollaboration::isTooBig() const
{
  return numNodes()>=Config_getInt(DOT_GRAPH_MAX_NODES);
}

int DotGroupCollaboration::numNodes() const
{
  return static_cast<int>(m_usedNodes.size());
}

void DotGroupCollaboration::writeGraphHeader(TextStream &t,const DString &title) const
{
  DotGraph::writeGraphHeader(t, title);
  t << "  rankdir=LR;\n";
}
