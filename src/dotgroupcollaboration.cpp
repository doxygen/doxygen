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

#include "dotgroupcollaboration.h"
#include "classlist.h"
#include "doxygen.h"
#include "namespacedef.h"
#include "pagedef.h"
#include "util.h"
#include "config.h"

DotGroupCollaboration::DotGroupCollaboration(const GroupDef* gd)
{
  QCString tmp_url = gd->getReference()+"$"+gd->getOutputFileBase();
  QCString tooltip = gd->briefDescriptionAsTooltip();
  m_rootNode = new DotNode(getNextNodeNumber(), gd->groupTitle(), tooltip, tmp_url, TRUE );
  m_rootNode->markAsVisible();
  m_usedNodes.insert(std::make_pair(gd->name().str(), m_rootNode));

  m_diskName = gd->getOutputFileBase();

  buildGraph( gd );
}

DotGroupCollaboration::~DotGroupCollaboration()
{
  // delete all created Nodes saved in m_usedNodes map
  for (const auto &kv : m_usedNodes)
  {
    delete kv.second;
  }
}

void DotGroupCollaboration::buildGraph(const GroupDef* gd)
{
  QCString tmp_url;
  //===========================
  // hierarchy.

  // Write parents
  for (const auto &d : gd->partOfGroups())
  {
    DotNode *nnode = 0;
    auto it = m_usedNodes.find(d->name().str());
    if ( it==m_usedNodes.end())
    { // add node
      tmp_url = d->getReference()+"$"+d->getOutputFileBase();
      QCString tooltip = d->briefDescriptionAsTooltip();
      nnode = new DotNode(getNextNodeNumber(), d->groupTitle(), tooltip, tmp_url );
      nnode->markAsVisible();
      m_usedNodes.insert(std::make_pair(d->name().str(), nnode));
    }
    else
    {
      nnode = it->second;
    }
    tmp_url = "";
    addEdge( nnode, m_rootNode, DotGroupCollaboration::thierarchy, tmp_url, tmp_url );
  }

  // Add subgroups
  for (const auto &def : gd->getSubGroups())
  {
    DotNode *nnode = 0;
    auto it = m_usedNodes.find(def->name().str());
    if ( it==m_usedNodes.end())
    { // add node
      tmp_url = def->getReference()+"$"+def->getOutputFileBase();
      QCString tooltip = def->briefDescriptionAsTooltip();
      nnode = new DotNode(getNextNodeNumber(), def->groupTitle(), tooltip, tmp_url );
      nnode->markAsVisible();
      m_usedNodes.insert(std::make_pair(def->name().str(), nnode));
    }
    else
    {
      nnode = it->second;
    }
    tmp_url = "";
    addEdge( m_rootNode, nnode, DotGroupCollaboration::thierarchy, tmp_url, tmp_url );
  }

  //=======================
  // Write collaboration

  // Add members
  addMemberList( gd->getMemberList(MemberListType_allMembersList) );

  // Add classes
  for (const auto &def : gd->getClasses())
  {
    tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
    if (!def->anchor().isEmpty())
    {
      tmp_url+="#"+def->anchor();
    }
    addCollaborationMember( def, tmp_url, DotGroupCollaboration::tclass );
  }

  // Add namespaces
  for (const auto &def : gd->getNamespaces())
  {
    tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
    addCollaborationMember( def, tmp_url, DotGroupCollaboration::tnamespace );
  }

  // Add files
  for (const auto &def : gd->getFiles())
  {
    tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
    addCollaborationMember( def, tmp_url, DotGroupCollaboration::tfile );
  }

  // Add pages
  for (const auto &def : gd->getPages())
  {
    tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
    addCollaborationMember( def, tmp_url, DotGroupCollaboration::tpages );
  }

  // Add directories
  if ( !gd->getDirs().empty() )
  {
    for(const auto def : gd->getDirs())
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tdir );
    }
  }
}

void DotGroupCollaboration::addMemberList( MemberList* ml )
{
  if ( ml==0 || ml->empty() ) return;
  for (const auto &def : *ml)
  {
    QCString tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension
      +"#"+def->anchor();
    addCollaborationMember( def, tmp_url, DotGroupCollaboration::tmember );
  }
}

DotGroupCollaboration::Edge* DotGroupCollaboration::addEdge(
  DotNode* _pNStart, DotNode* _pNEnd, EdgeType _eType,
  const QCString& _label, const QCString& _url )
{
  // search a existing link.
  Edge *newEdge;
  for (const auto &edge : m_edges)
  {
    if ( edge->pNStart==_pNStart && edge->pNEnd==_pNEnd && edge->eType==_eType)
    { // edge already found
      newEdge = edge.get();
      break;
    }
  }
  if ( newEdge==0 ) // new link
  {
    m_edges.emplace_back(std::make_unique<Edge>(_pNStart,_pNEnd,_eType));
    newEdge = m_edges.back().get();
  }

  if (!_label.isEmpty())
  {
    newEdge->links.emplace_back(_label,_url);
  }

  return newEdge;
}

void DotGroupCollaboration::addCollaborationMember(
  const Definition* def, QCString& url, EdgeType eType )
{
  // Create group nodes
  QCString tmp_str;
  for (const auto &d : def->partOfGroups())
  {
    auto it = m_usedNodes.find(d->name().str());
    DotNode* nnode = it!=m_usedNodes.end() ? it->second : 0;
    if ( nnode != m_rootNode )
    {
      if ( nnode==0 )
      { // add node
        tmp_str = d->getReference()+"$"+d->getOutputFileBase();
        QCString tooltip = d->briefDescriptionAsTooltip();
        nnode = new DotNode(getNextNodeNumber(), d->groupTitle(), tooltip, tmp_str );
        nnode->markAsVisible();
        m_usedNodes.insert(std::make_pair(d->name().str(), nnode));
      }
      tmp_str = def->qualifiedName();
      addEdge( m_rootNode, nnode, eType, tmp_str, url );
    }
  }
}

QCString DotGroupCollaboration::getBaseName() const
{
  return m_diskName;
}

void DotGroupCollaboration::computeTheGraph()
{
  FTextStream md5stream(&m_theGraph);
  writeGraphHeader(md5stream,m_rootNode->label());

  // clean write flags
  for (const auto &kv : m_usedNodes)
  {
    kv.second->clearWriteFlag();
  }

  // write other nodes.
  for (const auto &kv : m_usedNodes)
  {
    kv.second->write(md5stream,Inheritance,m_graphFormat,TRUE,FALSE,FALSE);
  }

  // write edges
  for (const auto &edge : m_edges)
  {
    edge->write( md5stream );
  }

  writeGraphFooter(md5stream);

}

QCString DotGroupCollaboration::getMapLabel() const
{
  return escapeCharsInString(m_baseName, FALSE);
}

QCString DotGroupCollaboration::writeGraph( FTextStream &t,
  GraphOutputFormat graphFormat, EmbeddedOutputFormat textFormat,
  const char *path, const char *fileName, const char *relPath,
  bool generateImageMap,int graphId)
{
  m_doNotAddImageToIndex = TRUE;

  return DotGraph::writeGraph(t, graphFormat, textFormat, path, fileName, relPath, generateImageMap, graphId);
}

void DotGroupCollaboration::Edge::write( FTextStream &t ) const
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
  QCString arrowStyle = "dir=\"none\", style=\"dashed\"";
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
    //  if ( !link.url.isEmpty() )
    //    t << " HREF=\"" << link.url << "\"";
    //  t << ">" << DotNode::convertLabel(link->label) << "</TD></TR>";
    //}
    //t << "</TABLE>>";

    t << "label=\"";
    bool first=TRUE;
    int count=0;
    const int maxLabels = 10;
    for (const auto &link : links)
    {
      if (first) first=FALSE; else t << "\\n";
      t << DotNode::convertLabel(link.label);
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
    t << ", color=\"" << linkTypeColor[(int)eType] << "\"";
    break;
  }
  t << ", " << arrowStyle;
  t << "];" << endl;
}

bool DotGroupCollaboration::isTrivial() const
{
  return m_usedNodes.size() <= 1;
}

void DotGroupCollaboration::writeGraphHeader(FTextStream &t,const QCString &title) const
{
  int fontSize      = Config_getInt(DOT_FONTSIZE);
  QCString fontName = Config_getString(DOT_FONTNAME);
  t << "digraph ";
  if (title.isEmpty())
  {
    t << "\"Dot Graph\"";
  }
  else
  {
    t << "\"" << convertToXML(title) << "\"";
  }
  t << endl;
  t << "{" << endl;
  if (Config_getBool(DOT_TRANSPARENT))
  {
    t << "  bgcolor=\"transparent\";" << endl;
  }
  t << "  edge [fontname=\"" << fontName << "\",fontsize=\"" << fontSize << "\","
    "labelfontname=\"" << fontName << "\",labelfontsize=\"" << fontSize << "\"];\n";
  t << "  node [fontname=\"" << fontName << "\",fontsize=\"" << fontSize << "\",shape=box];\n";
  t << "  rankdir=LR;\n";
}
