/******************************************************************************
 *
 * $Id:$
 *
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _MAINHANDLER_H
#define _MAINHANDLER_H

#include <qlist.h>

#include <doxmlintf.h>
#include "basehandler.h"

class CompoundHandler;
struct CompoundEntry;

struct IndexEntry
{
    QString id;
    QString name;
};

struct MemberEntry : public IndexEntry
{
    CompoundEntry *compound;
};

struct CompoundEntry : public IndexEntry
{
    CompoundEntry(int size) : memberDict(size) 
    { memberDict.setAutoDelete(TRUE); }
    QDict<MemberEntry> memberDict;
};

class MainHandler : public IDoxygen, public BaseHandler<MainHandler>
{
  public:
    virtual void startCompound(const QXmlAttributes& attrib);
    virtual void startMember(const QXmlAttributes& attrib);
    virtual void endMember();
    virtual void startName(const QXmlAttributes& attrib);
    virtual void endName();
    MainHandler();
    virtual ~MainHandler();

    // IDoxygen
    ICompoundIterator *compounds() const;
    ICompound *compoundById(const char *id) const;
    virtual ICompound *compoundByName(const char *name) const;
    virtual ICompound *memberById(const char *id) const;
    virtual ICompoundIterator *memberByName(const char *name) const;

    virtual void release();
    void setDebugLevel(int level);
    bool readXMLDir(const char *dirName);
    void dump();
    void unloadCompound(CompoundHandler *ch);

  private:
    CompoundEntry               *m_curCompound;
    MemberEntry                 *m_curMember;
    QList<CompoundEntry>         m_compounds;
    QDict<CompoundEntry>         m_compoundDict;
    QDict<CompoundEntry>         m_compoundNameDict;
    QDict<MemberEntry>           m_memberDict;
    QDict<QList<CompoundEntry> > m_memberNameDict;
    QString                      m_xmlDirName;
    QDict<CompoundHandler>       m_compoundsLoaded;
    bool                         m_insideMember;
};

#endif
