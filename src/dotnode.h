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

#ifndef DOTNODE_H
#define DOTNODE_H

#include <vector>
#include <map>
#include <deque>
#include <iostream>

#include "types.h"
#include "dotgraph.h"

class ClassDef;
class DotNode;
class TextStream;

/** Attributes of an edge of a dot graph */
class EdgeInfo
{
  public:
    enum Colors { Blue=0, Green=1, Red=2, Purple=3, Grey=4, Orange=5, Orange2=6 };
    enum Styles { Solid=0, Dashed=1 };
    EdgeInfo(Colors color,Styles style,const QCString &lab,const QCString &url,int labColor)
        : m_color(color), m_style(style), m_label(lab), m_url(url), m_labColor(labColor) {}
    int color() const      { return m_color; }
    int style() const      { return m_style; }
    QCString label() const { return m_label; }
    QCString url() const   { return m_url; }
    int labelColor() const { return m_labColor; }
    static constexpr Colors protectionToColor(Protection prot)
    {
      switch (prot)
      {
        case Protection::Public:    return Blue;
        case Protection::Protected: return Green;
        case Protection::Private:   return Red;
        case Protection::Package:   return Purple;
      }
      return Blue;
    }
  private:
    int m_color;
    int m_style;
    QCString m_label;
    QCString m_url;
    int m_labColor;
};

using DotNodeRefVector = std::vector<DotNode*>;
using EdgeInfoVector = std::vector<EdgeInfo>;

/** A node in a dot graph */
class DotNode
{
  public:
    enum class LabelStyle { Plain, List, Table };
    static constexpr auto placeholderUrl = "-";
    static void deleteNodes(DotNode* node);
    static QCString convertLabel(const QCString&, LabelStyle=LabelStyle::Plain);
    DotNode(DotGraph *graph,const QCString &lab,const QCString &tip,const QCString &url,
        bool rootNode=FALSE,const ClassDef *cd=nullptr);

    enum TruncState { Unknown, Truncated, Untruncated };

    void addChild(DotNode *n,
                  EdgeInfo::Colors edgeColor=EdgeInfo::Purple,
                  EdgeInfo::Styles edgeStyle=EdgeInfo::Solid,
                  const QCString &edgeLab=QCString(),
                  const QCString &edgeURL=QCString(),
                  int edgeLabCol=-1);
    void addParent(DotNode *n);
    void deleteNode(DotNodeRefVector &deletedList);
    void removeChild(DotNode *n);
    void removeParent(DotNode *n);
    int  findParent( DotNode *n );

    void write(TextStream &t,GraphType gt,GraphOutputFormat f,
               bool topDown,bool toChildren,bool backArrows);
    void writeXML(TextStream &t,bool isClassGraph) const;
    void writeDocbook(TextStream &t,bool isClassGraph) const;
    void writeDEF(TextStream &t) const;
    void writeLabel(TextStream &t, GraphType gt) const;
    void writeUrl(TextStream &t) const;
    void writeBox(TextStream &t,GraphType gt,GraphOutputFormat f,
                  bool hasNonReachableChildren) const;
    void writeArrow(TextStream &t,GraphType gt,GraphOutputFormat f,const DotNode *cn,
                    const EdgeInfo *ei,bool topDown, bool pointBack=TRUE) const;

    QCString label() const         { return m_label; }
    int  number() const            { return m_number; }
    bool isVisible() const         { return m_visible; }
    TruncState isTruncated() const { return m_truncated; }
    int distance() const           { return m_distance; }
    int subgraphId() const         { return m_subgraphId; }
    bool isRenumbered() const      { return m_renumbered; }
    bool hasDocumentation() const  { return m_hasDoc; }
    bool isWritten() const         { return m_written; }

    void clearWriteFlag();
    void renumberNodes(int &number);
    void markRenumbered()          { m_renumbered = true; }
    DotNode& markHasDocumentation() { m_hasDoc = true; return *this;}
    void setSubgraphId(int id)     { m_subgraphId = id; }

    void colorConnectedNodes(int curColor);
    void setDistance(int distance);
    void markAsVisible(bool b=TRUE) { m_visible=b; }
    DotNode& markAsTruncated(bool b=TRUE) { m_truncated=b ? Truncated : Untruncated; return *this;}
    const DotNodeRefVector &children() const { return m_children; }
    const DotNodeRefVector &parents() const { return m_parents; }
    const EdgeInfoVector &edgeInfo() const { return m_edgeInfo; }
    DotNode &setNodeId(int number) { m_number=number; return *this; }

  private:
    DotGraph        *m_graph;
    int              m_number;
    QCString         m_label;                //!< label text
    QCString         m_tooltip;              //!< node's tooltip
    QCString         m_url;                  //!< url of the node (format: remote$local)
    DotNodeRefVector m_parents;              //!< list of parent nodes (incoming arrows)
    DotNodeRefVector m_children;             //!< list of child nodes (outgoing arrows)
    EdgeInfoVector   m_edgeInfo;             //!< edge info for each child
    bool             m_deleted    = false;   //!< used to mark a node as deleted
    bool             m_written    = false;   //!< used to mark a node as written
    bool             m_hasDoc     = false;   //!< used to mark a node as documented
    bool             m_isRoot;               //!< indicates if this is a root node
    const ClassDef * m_classDef;             //!< class representing this node (can be 0)
    bool             m_visible    = false;   //!< is the node visible in the output
    TruncState       m_truncated  = Unknown; //!< does the node have non-visible children/parents
    int              m_distance   = 1000;    //!< shortest path to the root node
    bool             m_renumbered = false;   //!< indicates if the node has been renumbered (to prevent endless loops)
    int              m_subgraphId = -1;
};

class DotNodeMap : public std::map<std::string,DotNode*>
{
};

class DotNodeDeque : public std::deque<DotNode*>
{
};

QCString escapeTooltip(const QCString &tooltip);

#endif
