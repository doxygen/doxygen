/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

    virtual ICompound *compound() const;
    virtual Protection protection() const { return m_protection; }
    virtual Kind kind() const { return m_kind; }
    
  private:  
    CompoundHandler *m_parent;
    QString m_id;
    Protection m_protection;  
    Kind m_kind;
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
                        public BaseHandler<CompoundHandler>
{
    friend class RelatedCompound;

  public:
    virtual void startSection(const QXmlAttributes& attrib);
    virtual void startCompound(const QXmlAttributes& attrib);
    virtual void addSuperClass(const QXmlAttributes& attrib);
    virtual void addSubClass(const QXmlAttributes& attrib);
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
    virtual void startInnerClass(const QXmlAttributes& attrib);
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
    void release();

    // IClass implementation
    IGraph *inheritanceGraph() const;
    IGraph *collaborationGraph() const;
    IRelatedCompoundIterator *baseClasses() const;
    IRelatedCompoundIterator *derivedClasses() const;
    ICompoundIterator *nestedClasses() const;

    // IFile implementation
    IGraph *includeDependencyGraph() const; 
    IGraph *includedByDependencyGraph() const;
    IDocProgramListing *source() const;
    
  private:
    QList<RelatedCompound>  m_superClasses;
    QList<RelatedCompound>  m_subClasses;
    QList<SectionHandler>   m_sections;
    DocHandler             *m_brief;
    DocHandler             *m_detailed;
    ProgramListingHandler  *m_programListing;
    StringImpl              m_id;
    StringImpl              m_kindString;
    CompoundKind            m_kind;
    StringImpl              m_name;
    QString                 m_defFile;
    int                     m_defLine;
    QString                 m_xmlDir;
    int                     m_refCount;
    QDict<MemberHandler>          m_memberDict;
    QDict<QList<MemberHandler> >  m_memberNameDict;
    MainHandler            *m_mainHandler;
    GraphHandler           *m_inheritanceGraph;
    GraphHandler           *m_collaborationGraph;
    GraphHandler           *m_includeDependencyGraph;
    GraphHandler           *m_includedByDependencyGraph;
    QList<QString>          m_innerClasses;
    ProgramListingHandler  *m_source;

};

void compoundhandler_init();
void compoundhandler_exit();

#endif
