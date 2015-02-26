/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */
#ifndef _COMPOUNDHANDLER_H
#define _COMPOUNDHANDLER_H

#include <qstring.h>
#include <qlist.h>
#include <qxml.h>
#include <doxmlintf.h>

#include "stringimpl.h"
#include "basehandler.h"
#include "baseiterator.h"

class MainHandler;
class DocHandler;
class ProgramListingHandler;
class GraphHandler;
class MemberHandler;
class CompoundHandler;
class SectionHandler;
class ParamHandler;
class TemplateParamListHandler;
class TitleHandler;
class ListOfAllMembersHandler;

class IncludeHandler : public IInclude, public BaseHandler<IncludeHandler>
{
  public:
    IncludeHandler(IBaseHandler *parent,const char *endtag);
    virtual ~IncludeHandler();

    void startInclude(const QXmlAttributes &attrib);
    void endInclude();

    // IInclude
    virtual const IString * name() const 
    { return &m_name; }
    virtual const IString * refId() const
    { return &m_refId; }
    virtual bool isLocal() const
    { return &m_isLocal; }

  private:
    IBaseHandler *m_parent;
    StringImpl m_name;             // element's content
    StringImpl m_refId;            // refid
    bool m_isLocal;                // local
};

class IncludeIterator : public BaseIterator<IIncludeIterator,IInclude,IncludeHandler>
{
  public:
    IncludeIterator(const QList<IncludeHandler> &list) : 
      BaseIterator<IIncludeIterator,IInclude,IncludeHandler>(list) {}
};


class RelatedCompound : public IRelatedCompound
{
  public:
    RelatedCompound(CompoundHandler *parent,
                    const QString &id,
                    Protection prot,
                    Kind kind
                   ) :
      m_parent(parent), m_id(id), m_protection(prot), m_kind(kind) {}
    virtual ~RelatedCompound() {}
    void setName(const QString &str) { m_name = str; }

    virtual ICompound *compound() const;
    virtual Protection protection() const { return m_protection; }
    virtual Kind kind() const { return m_kind; }
    virtual const IString *name() const { return &m_name; }
    
  private:  
    CompoundHandler *m_parent;
    QString m_id;                  // refid
    Protection m_protection;       // prot
    Kind m_kind;                   // virt
    StringImpl m_name;             // element's content
};

class RelatedCompoundIterator : public BaseIterator<IRelatedCompoundIterator,IRelatedCompound,RelatedCompound>
{
  public:
    RelatedCompoundIterator(const QList<RelatedCompound> &list) : 
      BaseIterator<IRelatedCompoundIterator,IRelatedCompound,RelatedCompound>(list) {}
};


class CompoundHandler : public IClass,
                        public IStruct,
                        public IUnion,
                        public IException,
                        public IInterface,
                        public INamespace,
                        public IFile,
                        public IGroup,
                        public IPage,
                        public IDir,
                        public BaseHandler<CompoundHandler>
{
    friend class RelatedCompound;

  public:
    virtual void startSection(const QXmlAttributes& attrib);
    virtual void startCompound(const QXmlAttributes& attrib);
    virtual void startSuperClass(const QXmlAttributes& attrib);
    virtual void endSuperClass();
    virtual void startSubClass(const QXmlAttributes& attrib);
    virtual void endSubClass();
    virtual void endCompound();
    virtual void endCompoundName();
    virtual void startBriefDesc(const QXmlAttributes& attrib);
    virtual void startDetailedDesc(const QXmlAttributes& attrib);
    virtual void startLocation(const QXmlAttributes& attrib);
    virtual void startProgramListing(const QXmlAttributes& attrib);
    virtual void startInheritanceGraph(const QXmlAttributes& attrib);
    virtual void startCollaborationGraph(const QXmlAttributes& attrib);
    virtual void startIncludeDependencyGraph(const QXmlAttributes& attrib);
    virtual void startIncludedByDependencyGraph(const QXmlAttributes& attrib);
    virtual void startIncludes(const QXmlAttributes& attrib);
    virtual void startIncludedBy(const QXmlAttributes& attrib);
    virtual void startInnerDir(const QXmlAttributes& attrib);
    virtual void startInnerClass(const QXmlAttributes& attrib);
    virtual void startInnerNamespace(const QXmlAttributes& attrib);
    virtual void startInnerFile(const QXmlAttributes& attrib);
    virtual void startInnerGroup(const QXmlAttributes& attrib);
    virtual void startInnerPage(const QXmlAttributes& attrib);
    virtual void startTitle(const QXmlAttributes& attrib);
    virtual void startTemplateParamList(const QXmlAttributes& attrib);
    virtual void startListOfAllMembers(const QXmlAttributes& attrib);
    virtual void addref() { m_refCount++; }

    CompoundHandler(const QString &dirName);
    virtual ~CompoundHandler();
    bool parseXML(const char *compId);
    void initialize(MainHandler *mh);
    void insertMember(MemberHandler *mh);
    ICompound *toICompound() const;

    // ICompound implementation
    const IString *name() const { return &m_name; }
    const IString *id()   const { return &m_id;   }
    CompoundKind kind() const { return m_kind; }
    const IString *kindString() const { return &m_kindString; }
    ISectionIterator *sections() const;
    IDocRoot *briefDescription() const;
    IDocRoot *detailedDescription() const;
    IMember *memberById(const char *id) const;
    IMemberIterator *memberByName(const char *name) const;
    IParamIterator *templateParameters() const;
    void release();

    // IClass implementation
    IGraph *inheritanceGraph() const;
    IGraph *collaborationGraph() const;
    IRelatedCompoundIterator *baseCompounds() const;
    IRelatedCompoundIterator *derivedCompounds() const;
    ICompoundIterator *nestedCompounds() const;
    ICompoundIterator *nestedGroup() const;
    const IString *locationFile() const { return &m_defFile; }
    int locationLine() const { return m_defLine; }
    const IString *locationBodyFile() const { return &m_defBodyFile; }
    int locationBodyStartLine() const { return m_defBodyStart; }
    int locationBodyEndLine() const { return m_defBodyEnd; }
    IMemberReferenceIterator *members() const;

    // IFile implementation
    IGraph *includeDependencyGraph() const; 
    IGraph *includedByDependencyGraph() const;
    IDocProgramListing *source() const;
    IIncludeIterator *includes() const;
    IIncludeIterator *includedBy() const;

    // IPage implementation
    const IDocTitle *title() const;
    
  private:
                                                                // XML elements:
                                                                // -------------
    StringImpl                     m_name;                      // compoundname
    TitleHandler*                  m_titleHandler;              // title
    QList<RelatedCompound>         m_subClasses;                // basecompoundref
    QList<RelatedCompound>         m_superClasses;              // derivedcompoundref
    QList<IncludeHandler>          m_includes;                  // includes
    QList<IncludeHandler>          m_includedBy;                // includedBy
    GraphHandler*                  m_includeDependencyGraph;    // incdepgraph
    GraphHandler*                  m_includedByDependencyGraph; // invincdepgraph
    QList<QString>                 m_innerCompounds;            // innerdir/innerfile/innerclass/innernamespace/innergroup
    TemplateParamListHandler*      m_templateParamList;         // templateparamlist
    QList<SectionHandler>          m_sections;                  // sectiondef
    DocHandler*                    m_brief;                     // briefdescription
    DocHandler*                    m_detailed;                  // detaileddescription
    GraphHandler*                  m_inheritanceGraph;          // inheritancegraph
    GraphHandler*                  m_collaborationGraph;        // collaborationgraph
    ProgramListingHandler*         m_programListing;            // programlisting
                                                                // location
    StringImpl                     m_defFile;                   // - file
    int                            m_defLine;                   // - line
    StringImpl                     m_defBodyFile;               // - bodyfile
    int                            m_defBodyStart;              // - bodystart 
    int                            m_defBodyEnd;                // - bodyend
    ListOfAllMembersHandler*       m_members;                   // listofallmember

                                                                // XML attributes:
                                                                // ---------------
    StringImpl                     m_id;                        // id
    CompoundKind                   m_kind;                      // kind
    StringImpl                     m_kindString;                // kind as a string
    StringImpl                     m_protection;                // prot

    // local variables
    QString                        m_xmlDir;                    // directory where the info is found
    int                            m_refCount;                  // object reference counter
    QDict<MemberHandler>           m_memberDict;                // id->member lookup
    QDict<QList<MemberHandler> >   m_memberNameDict;            // name->memberlist lookup
    MainHandler*                   m_mainHandler;               // parent object
};

void compoundhandler_init();
void compoundhandler_exit();

#endif
