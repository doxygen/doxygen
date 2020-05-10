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

#include "dotnode.h"
#include "classlist.h"
#include "doxygen.h"
#include "namespacedef.h"
#include "pagedef.h"
#include "util.h"
#include "config.h"

#define DOT_TRANSPARENT       Config_getBool(DOT_TRANSPARENT)

DotGroupCollaboration::DotGroupCollaboration(const GroupDef* gd)
{
  QCString tmp_url = gd->getReference()+"$"+gd->getOutputFileBase();
  m_usedNodes = new QDict<DotNode>(1009);
  QCString tooltip = gd->briefDescriptionAsTooltip();
  m_rootNode = new DotNode(getNextNodeNumber(), gd->groupTitle(), tooltip, tmp_url, TRUE );
  m_rootNode->markAsVisible();
  m_usedNodes->insert(gd->name(), m_rootNode );
  m_edges.setAutoDelete(TRUE);

  m_diskName = gd->getOutputFileBase();

  buildGraph( gd );
}

DotGroupCollaboration::~DotGroupCollaboration()
{
  // delete all created Nodes saved in m_usedNodes:QDict
  if(m_usedNodes != NULL)
  {
    QDictIterator<DotNode> it(*m_usedNodes);
    for(;it.current(); ++it)
    {
      delete it.current();
    }
  }

  delete m_usedNodes;
}

void DotGroupCollaboration::buildGraph(const GroupDef* gd)
{
  QCString tmp_url;
  //===========================
  // hierarchy.

  // Write parents
  const GroupList *groups = gd->partOfGroups();
  if ( groups )
  {
    GroupListIterator gli(*groups);
    const GroupDef *d;
    for (gli.toFirst();(d=gli.current());++gli)
    {
      DotNode* nnode = m_usedNodes->find(d->name());
      if ( !nnode )
      { // add node
        tmp_url = d->getReference()+"$"+d->getOutputFileBase();
        QCString tooltip = d->briefDescriptionAsTooltip();
        nnode = new DotNode(getNextNodeNumber(), d->groupTitle(), tooltip, tmp_url );
        nnode->markAsVisible();
        m_usedNodes->insert(d->name(), nnode );
      }
      tmp_url = "";
      addEdge( nnode, m_rootNode, DotGroupCollaboration::thierarchy, tmp_url, tmp_url );
    }
  }

  // Add subgroups
  if ( gd->getSubGroups() && gd->getSubGroups()->count() )
  {
    QListIterator<GroupDef> defli(*gd->getSubGroups());
    const GroupDef *def;
    for (;(def=defli.current());++defli)
    {
      DotNode* nnode = m_usedNodes->find(def->name());
      if ( !nnode )
      { // add node
        tmp_url = def->getReference()+"$"+def->getOutputFileBase();
        QCString tooltip = def->briefDescriptionAsTooltip();
        nnode = new DotNode(getNextNodeNumber(), def->groupTitle(), tooltip, tmp_url );
        nnode->markAsVisible();
        m_usedNodes->insert(def->name(), nnode );
      }
      tmp_url = "";
      addEdge( m_rootNode, nnode, DotGroupCollaboration::thierarchy, tmp_url, tmp_url );
    }
  }

  //=======================
  // Write collaboration

  // Add members
  addMemberList( gd->getMemberList(MemberListType_allMembersList) );

  // Add classes
  if ( gd->getClasses() && gd->getClasses()->count() )
  {
    ClassSDict::Iterator defli(*gd->getClasses());
    ClassDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      if (!def->anchor().isEmpty())
      {
        tmp_url+="#"+def->anchor();
      }
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tclass );          
    }
  }

  // Add namespaces
  if ( gd->getNamespaces() && gd->getNamespaces()->count() )
  {
    NamespaceSDict::Iterator defli(*gd->getNamespaces());
    NamespaceDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tnamespace );          
    }
  }

  // Add files
  if ( gd->getFiles() && gd->getFiles()->count() )
  {
    QListIterator<FileDef> defli(*gd->getFiles());
    const FileDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tfile );          
    }
  }

  // Add pages
  if ( gd->getPages() && gd->getPages()->count() )
  {
    PageSDict::Iterator defli(*gd->getPages());
    PageDef *def;
    for (;(def=defli.current());++defli)
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tpages );          
    }
  }

  // Add directories
  if ( gd->getDirs() && gd->getDirs()->size() )
  {
    for(const auto def : *(gd->getDirs()))
    {
      tmp_url = def->getReference()+"$"+def->getOutputFileBase()+Doxygen::htmlFileExtension;
      addCollaborationMember( def, tmp_url, DotGroupCollaboration::tdir );          
    }
  }
}

void DotGroupCollaboration::addMemberList( MemberList* ml )
{
  if ( !( ml && ml->count()) ) return;
  MemberListIterator defli(*ml);
  MemberDef *def;
  for (;(def=defli.current());++defli)
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
  QListIterator<Edge> lli(m_edges);
  Edge* newEdge = 0;
  for ( lli.toFirst(); (newEdge=lli.current()); ++lli)
  {
    if ( newEdge->pNStart==_pNStart && 
      newEdge->pNEnd==_pNEnd &&
      newEdge->eType==_eType 
      )
    { // edge already found
      break;
    }
  }
  if ( newEdge==0 ) // new link
  {
    newEdge = new Edge(_pNStart,_pNEnd,_eType);
    m_edges.append( newEdge );
  } 

  if (!_label.isEmpty())
  {
    newEdge->links.append(new Link(_label,_url));
  }

  return newEdge;
}

void DotGroupCollaboration::addCollaborationMember( 
  const Definition* def, QCString& url, EdgeType eType )
{
  // Create group nodes
  if ( !def->partOfGroups() )
    return;
  GroupListIterator gli(*def->partOfGroups());
  GroupDef *d;
  QCString tmp_str;
  for (;(d=gli.current());++gli)
  {
    DotNode* nnode = m_usedNodes->find(d->name());
    if ( nnode != m_rootNode )
    {
      if ( nnode==0 )
      { // add node
        tmp_str = d->getReference()+"$"+d->getOutputFileBase();
        QCString tooltip = d->briefDescriptionAsTooltip();
        nnode = new DotNode(getNextNodeNumber(), d->groupTitle(), tooltip, tmp_str );
        nnode->markAsVisible();
        m_usedNodes->insert(d->name(), nnode );
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
  QDictIterator<DotNode> dni(*m_usedNodes);
  DotNode *pn;
  for (dni.toFirst();(pn=dni.current());++dni)
  {
    pn->clearWriteFlag();
  }

  // write other nodes.
  for (dni.toFirst();(pn=dni.current());++dni)
  {
    pn->write(md5stream,Inheritance,m_graphFormat,TRUE,FALSE,FALSE);
  }

  // write edges
  QListIterator<Edge> eli(m_edges);
  Edge* edge;
  for (eli.toFirst();(edge=eli.current());++eli)
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
  if (links.count()>0) // there are links
  {
    t << ", ";
    // HTML-like edge labels crash on my Mac with Graphviz 2.0! and
    // are not supported by older version of dot.
    //
    //t << label=<<TABLE BORDER=\"0\" CELLBORDER=\"0\">";
    //QListIterator<Link> lli(links);
    //Link *link;
    //for( lli.toFirst(); (link=lli.current()); ++lli)
    //{
    //  t << "<TR><TD";
    //  if ( !link->url.isEmpty() )
    //    t << " HREF=\"" << link->url << "\"";
    //  t << ">" << link->label << "</TD></TR>";
    //}
    //t << "</TABLE>>";

    t << "label=\"";
    QListIterator<Link> lli(links);
    Link *link;
    bool first=TRUE;
    int count=0;
    const int maxLabels = 10;
    for( lli.toFirst(); (link=lli.current()) && count<maxLabels; ++lli,++count)
    {
      if (first) first=FALSE; else t << "\\n"; 
      t << DotNode::convertLabel(link->label);
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
  return m_usedNodes->count() <= 1;
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
  if (DOT_TRANSPARENT)
  {
    t << "  bgcolor=\"transparent\";" << endl;
  }
  t << "  edge [fontname=\"" << fontName << "\",fontsize=\"" << fontSize << "\","
    "labelfontname=\"" << fontName << "\",labelfontsize=\"" << fontSize << "\"];\n";
  t << "  node [fontname=\"" << fontName << "\",fontsize=\"" << fontSize << "\",shape=box];\n";
  t << "  rankdir=LR;\n";
}
