/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include <qxml.h>
#include "mainhandler.h"
#include "compoundhandler.h"
#include "sectionhandler.h"
#include "graphhandler.h"
#include "dochandler.h"
#include "memberhandler.h"
#include "debug.h"


class ErrorHandler : public QXmlErrorHandler
{
    public:
      virtual ~ErrorHandler() {}
      bool warning( const QXmlParseException & )
      {
        return FALSE;
      }
      bool error( const QXmlParseException & )
      {
        return FALSE;
      }
      bool fatalError( const QXmlParseException &exception )
      {
        debug(1,"Fatal error at line %d column %d: %s\n",
            exception.lineNumber(),exception.columnNumber(),
            exception.message().data());
        return FALSE;
      }
      QString errorString() { return ""; }

    private:
      QString errorMsg;
};

//--------------------------------------------------------------------------

class CompoundEntryIterator : public ICompoundIterator, 
                              public QListIterator<CompoundEntry>
{
  public:
    CompoundEntryIterator(const MainHandler *m,const QList<CompoundEntry> &list) :
      QListIterator<CompoundEntry>(list), m_mainHandler(m) {}
    virtual ~CompoundEntryIterator() {}

    virtual void toFirst()
    { 
      QListIterator<CompoundEntry>::toFirst(); 
    }
    virtual void toLast()
    { 
      QListIterator<CompoundEntry>::toLast(); 
    }
    virtual void toNext()
    { 
      QListIterator<CompoundEntry>::operator++(); 
    }
    virtual void toPrev()
    { 
      QListIterator<CompoundEntry>::operator--(); 
    }
    virtual ICompound *current() const
    { 
      CompoundEntry *ch = QListIterator<CompoundEntry>::current(); 
      return ch ? m_mainHandler->compoundById(ch->id.utf8()) : 0;
    }
    virtual void release()
    { delete this; }

  private:
    const MainHandler *m_mainHandler;
};

//--------------------------------------------------------------------------

MainHandler::MainHandler() : m_compoundDict(2999), m_compoundNameDict(2999),
                             m_memberDict(12251), m_memberNameDict(12251),
                             m_compoundsLoaded(1009)
{
  m_compounds.setAutoDelete(TRUE);
  m_memberNameDict.setAutoDelete(TRUE);
  addStartHandler("doxygenindex"); 
  addEndHandler("doxygenindex");
  addStartHandler("compound",this,&MainHandler::startCompound);
  addEndHandler("compound");
  addStartHandler("member",this,&MainHandler::startMember);
  addEndHandler("member",this,&MainHandler::endMember);
  addStartHandler("name",this,&MainHandler::startName);
  addEndHandler("name",this,&MainHandler::endName);
  m_curCompound = 0;
  m_insideMember = FALSE;
}

MainHandler::~MainHandler()
{
  debug(2,"MainHandler::~MainHandler()\n");
}

void MainHandler::startCompound(const QXmlAttributes& attrib)
{
  m_curCompound = new CompoundEntry(257);
  m_curCompound->id = attrib.value("refid");
  m_compounds.append(m_curCompound);
  m_compoundDict.insert(m_curCompound->id.utf8(),m_curCompound);
}

void MainHandler::startName(const QXmlAttributes& /*attrib*/)
{
  m_curString = "";
}

void MainHandler::endName()
{
  if (m_insideMember)
  {
    m_curMember->name = m_curString;
  }
  else
  {
    m_curCompound->name = m_curString;
    m_compoundNameDict.insert(m_curString.utf8(),m_curCompound);
  }
}

void MainHandler::startMember(const QXmlAttributes& attrib)
{
  m_insideMember = TRUE;
  m_curMember = new MemberEntry;
  m_curMember->id = attrib.value("refid");
  m_curMember->compound = m_curCompound;
  m_memberDict.insert(m_curMember->id.utf8(),m_curMember);
}

void MainHandler::endMember()
{
  m_curCompound->memberDict.insert(m_curMember->name.utf8(),m_curMember);
  QList<CompoundEntry> *cel=0;
  if ((cel=m_memberNameDict.find(m_curMember->name.utf8()))==0)
  {
    cel = new QList<CompoundEntry>;
    m_memberNameDict.insert(m_curMember->name.utf8(),cel);
  }
  cel->append(m_curCompound);
  m_insideMember = FALSE;
}

void MainHandler::setDebugLevel(int level)
{
  ::setDebugLevel(level);
}

void MainHandler::dump()
{
  QListIterator<CompoundEntry> cli(m_compounds);
  CompoundEntry *ce;
  for (cli.toFirst();(ce=cli.current());++cli)
  {
    debug(2,"compound id=`%s' name=`%s'\n",ce->id.data(),ce->name.data());
    QDictIterator<MemberEntry> mdi(ce->memberDict);
    MemberEntry *me;
    for (mdi.toFirst();(me=mdi.current());++mdi)
    {
      debug(2,"  member id=`%s' name=`%s'\n",me->id.data(),me->name.data());
    }
  }
}

bool MainHandler::readXMLDir(const char * xmlDirName)
{
  m_xmlDirName = xmlDirName;
  QString xmlFileName=m_xmlDirName+"/index.xml";
  QFile xmlFile(xmlFileName);
  //printf("Trying %s xmlFile.exists()=%d isReadable()=%d\n",
  //    xmlFileName.data(),xmlFile.exists(),xmlFile.isReadable());
  if (xmlFile.exists())
  {
    ErrorHandler errorHandler;
    QXmlInputSource source( xmlFile );
    QXmlSimpleReader reader;
    reader.setContentHandler( this );
    reader.setErrorHandler( &errorHandler );
    reader.parse( source );
    dump();
    return TRUE;
  }
  return FALSE;
}

ICompoundIterator *MainHandler::compounds() const
{
  return new CompoundEntryIterator(this,m_compounds);
}

ICompound *MainHandler::compoundById(const char *id) const
{
  QString ids = id;
  if (ids.isEmpty()) return 0;
  CompoundHandler *ch = m_compoundsLoaded[ids.utf8()];
  if (ch) // compound already in memory
  {
    ch->addref(); // returning alias -> increase reference counter
    return ch->toICompound(); 
  }
  CompoundEntry *ce = m_compoundDict.find(ids.utf8());
  if (ce==0) return 0; // id not found
  // create and load a new compound
  ch = new CompoundHandler(m_xmlDirName);
  if (!ch->parseXML(id))
  {
    // compound could not be initialized.
    delete ch;
    return 0;
  }

  // we disregard the constness here, because the object stays conceptually
  // unchanged.
  MainHandler *that = (MainHandler *)this;
  ch->initialize(that);
  //printf("loading compound %s in memory\n",id);
  that->m_compoundsLoaded.insert(id,ch);
  return ch->toICompound();
}

void MainHandler::unloadCompound(CompoundHandler *ch)
{
  //printf("unloading compound %s from memory\n",ch->id()->latin1());
  bool result = m_compoundsLoaded.remove(ch->id()->latin1()); 
  if (!result) debug(1,"Failed to unload component!\n");
}

ICompound *MainHandler::compoundByName(const char *name) const
{
  QString nameStr = name;
  if (nameStr.isEmpty()) return 0;
  CompoundEntry *ce = m_compoundNameDict[name];
  if (ce==0) return 0; // name not found
  return compoundById(ce->id.utf8());
}

ICompound *MainHandler::memberById(const char *id) const
{
  QString ids = id;
  if (ids.isEmpty()) return 0;
  MemberEntry *me = m_memberDict[id];
  if (me==0) return 0; // id not found
  return compoundById(me->compound->id.utf8());
}

ICompoundIterator *MainHandler::memberByName(const char *name) const
{
  QString nameStr = name;
  if (nameStr.isEmpty()) return 0;
  QList<CompoundEntry> *cel = m_memberNameDict[name];
  if (cel==0) return 0; // name not found
  return new CompoundEntryIterator(this,*cel);
}

IDoxygen *createObjectModel()
{
  compoundhandler_init();
  sectionhandler_init();
  memberhandler_init();
  dochandler_init();
  graphhandler_init();
  return new MainHandler;
}

void MainHandler::release()
{
  //printf("MainHandler::release()\n");
  QDictIterator<CompoundHandler> chi(m_compoundsLoaded);
  CompoundHandler *ch;
  for (chi.toFirst();(ch=chi.current());++chi)
  {
    debug(1,"Compound %s not released\n",ch->name()->latin1());
  }
  graphhandler_exit();
  dochandler_exit();
  memberhandler_exit();
  sectionhandler_exit();
  compoundhandler_exit();
  delete this;
}

