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

#include "basehandler.h"
#include "baseiterator.h"

class MainHandler;
class DocHandler;
class ProgramListingHandler;
class GraphHandler;
class MemberHandler;

class CompoundHandler : public ICompound, public BaseHandler<CompoundHandler>
{
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
    virtual void addref() { m_refCount++; }

    CompoundHandler(const QString &dirName);
    virtual ~CompoundHandler();
    bool parseXML(const QString &compId);
    void initialize(MainHandler *mh);
    void insertMember(MemberHandler *mh);

    // ICompound implementation
    QString name() const { return m_name; }
    QString id()   const { return m_id;   }
    CompoundKind kind() const { return m_kind; }
    QString kindString() const { return m_kindString; }
    ISectionIterator *sections() const;
    IDocRoot *briefDescription() const;
    IDocRoot *detailedDescription() const;
    IMember *memberById(const QString &id) const;
    IMemberIterator *memberByName(const QString &name) const;
    void release();

  private:
    struct RelatedClass
    {
      RelatedClass(const QString &id,const QString &prot,const QString &virt) :
        m_id(id),m_protection(prot),m_virtualness(virt) {}

      QString m_id;
      QString m_protection;
      QString m_virtualness;
    };
    QList<RelatedClass>     m_superClasses;
    QList<RelatedClass>     m_subClasses;
    QList<ISection>         m_sections;
    DocHandler             *m_brief;
    DocHandler             *m_detailed;
    ProgramListingHandler  *m_programListing;
    QString                 m_id;
    QString                 m_kindString;
    CompoundKind            m_kind;
    QString                 m_name;
    QString                 m_defFile;
    int                     m_defLine;
    QString                 m_xmlDir;
    int                     m_refCount;
    QDict<MemberHandler>          m_memberDict;
    QDict<QList<MemberHandler> >  m_memberNameDict;
    MainHandler            *m_mainHandler;
    GraphHandler           *m_inheritanceGraph;
    GraphHandler           *m_collaborationGraph;

};

void compoundhandler_init();
void compoundhandler_exit();

#endif
