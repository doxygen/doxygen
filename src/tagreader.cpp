/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#include "tagreader.h"

#include <stdio.h>
#include <qxml.h>
#include <qstack.h>
#include <qdict.h>
#include <qfileinfo.h>
#include <qlist.h>
#include <qstring.h>
#include <qstringlist.h>

#include "entry.h"
#include "classdef.h"
#include "doxygen.h"
#include "util.h"
#include "message.h"

/*! Container for member specific info that can be read from a tagfile */
class TagMemberInfo
{
  public:
    TagMemberInfo() : prot(Public), virt(Normal), isStatic(FALSE) {}
    QString name;
    QString anchor;
    QString arglist;
    QString kind;
    Protection prot;
    Specifier virt;
    bool isStatic; 
};

/*! Container for class specific info that can be read from a tagfile */
class TagClassInfo
{
  public:
    enum Kind { Class, Struct, Union, Interface, Exception };
    TagClassInfo() { bases=0, members.setAutoDelete(TRUE); }
   ~TagClassInfo() { delete bases; }
    QString name;
    QString filename;
    QList<BaseInfo> *bases;
    QList<TagMemberInfo> members;
    Kind kind;
};

/*! Container for namespace specific info that can be read from a tagfile */
class TagNamespaceInfo
{
  public:
    TagNamespaceInfo() { members.setAutoDelete(TRUE); }
    QString name;
    QString filename;
    QList<TagMemberInfo> members;
    QStringList classList;
};

/*! Container for file specific info that can be read from a tagfile */
class TagFileInfo
{
  public:
    TagFileInfo() { members.setAutoDelete(TRUE); }
    QString name;
    QString path;
    QString filename;
    QList<TagMemberInfo> members;
    QStringList classList;
    QStringList namespaceList;
};

/*! Container for group specific info that can be read from a tagfile */
class TagGroupInfo
{
  public:
    TagGroupInfo() { members.setAutoDelete(TRUE); }
    QString name;
    QString title;
    QString filename;
    QList<TagMemberInfo> members;
    QStringList subgroupList;
    QStringList classList;
    QStringList namespaceList;
    QStringList fileList;
    QStringList pageList;
};

/*! Container for page specific info that can be read from a tagfile */
class TagPageInfo
{
  public:
    QString name;
    QString title;
    QString filename;
};


/*! Tag file parser. 
 *  Reads an XML-structured tagfile and builds up the structure in
 *  memory. The method buildLists() is used to transfer/translate 
 *  the structures to the doxygen engine.
 */
class TagFileParser : public QXmlDefaultHandler
{
    enum State { Invalid,
                 InClass,
                 InFile,
                 InNamespace,
                 InGroup,
                 InPage,
                 InMember
               };
    class StartElementHandler
    {
        typedef void (TagFileParser::*Handler)(const QXmlAttributes &attrib); 
      public:
        StartElementHandler(TagFileParser *parent, Handler h) : m_parent(parent), m_handler(h) {}
        void operator()(const QXmlAttributes &attrib) { (m_parent->*m_handler)(attrib); }
      private:
        TagFileParser *m_parent;
        Handler m_handler;
    };
    class EndElementHandler
    {
        typedef void (TagFileParser::*Handler)(); 
      public:
        EndElementHandler(TagFileParser *parent, Handler h) : m_parent(parent), m_handler(h) {}
        void operator()() { (m_parent->*m_handler)(); }
      private:
        TagFileParser *m_parent;
        Handler m_handler;
    };

  public:
    TagFileParser(const char *tagName) : m_tagName(tagName) {}

    void startCompound( const QXmlAttributes& attrib )
    {
      m_curString = "";
      QString kind = attrib.value("kind");
      if (kind=="class")
      {
        m_curClass = new TagClassInfo;
        m_curClass->kind = TagClassInfo::Class;
        m_state = InClass;
      }
      else if (kind=="struct")
      {
        m_curClass = new TagClassInfo;
        m_curClass->kind = TagClassInfo::Struct;
        m_state = InClass;
      }
      else if (kind=="union")
      {
        m_curClass = new TagClassInfo;
        m_curClass->kind = TagClassInfo::Union;
        m_state = InClass;
      }
      else if (kind=="interface")
      {
        m_curClass = new TagClassInfo;
        m_curClass->kind = TagClassInfo::Interface;
        m_state = InClass;
      }
      else if (kind=="exception")
      {
        m_curClass = new TagClassInfo;
        m_curClass->kind = TagClassInfo::Exception;
        m_state = InClass;
      }
      else if (kind=="file")
      {
        m_curFile = new TagFileInfo;
        m_state = InFile;
      }
      else if (kind=="namespace")
      {
        m_curNamespace = new TagNamespaceInfo;
        m_state = InNamespace;
      }
      else if (kind=="group")
      {
        m_curGroup = new TagGroupInfo;
        m_state = InGroup;
      }
      else if (kind=="page")
      {
        m_curPage = new TagPageInfo;
        m_state = InPage;
      }
      else
      {
        err("Error: Unknown compound attribute `%s' found!\n",kind.data());
      }
    }
    void endCompound()
    {
      switch (m_state)
      {
        case InClass:     m_tagFileClasses.append(m_curClass); 
                          m_curClass=0; break; 
        case InFile:      m_tagFileFiles.append(m_curFile); 
                          m_curFile=0; break; 
        case InNamespace: m_tagFileNamespaces.append(m_curNamespace); 
                          m_curNamespace=0; break; 
        case InGroup:     m_tagFileGroups.append(m_curGroup); 
                          m_curGroup=0; break; 
        case InPage:      m_tagFilePages.append(m_curPage); 
                          m_curPage=0; break; 
        default:
                          err("Error: tag `compound' was not expected!\n");
      }
    }
    void startMember( const QXmlAttributes& attrib)
    {
      m_curMember = new TagMemberInfo;
      m_curMember->kind = attrib.value("kind");
      QString protStr = attrib.value("protection");
      QString virtStr = attrib.value("virtualness");
      QString staticStr = attrib.value("static");
      if (protStr=="protected")
      {
        m_curMember->prot = Protected;
      }
      else if (protStr=="private")
      {
        m_curMember->prot = Private;
      }
      if (virtStr=="virtual")
      {
        m_curMember->virt = Virtual;
      }
      else if (virtStr=="pure")
      {
        m_curMember->virt = Pure;
      }
      if (staticStr=="yes")
      {
        m_curMember->isStatic = TRUE;
      }
      m_stateStack.push(new State(m_state));
      m_state = InMember;
    }
    void endMember()
    {
      m_state = *m_stateStack.pop();
      switch(m_state)
      {
        case InClass:     m_curClass->members.append(m_curMember); break;
        case InFile:      m_curFile->members.append(m_curMember); break;
        case InNamespace: m_curNamespace->members.append(m_curMember); break;
        case InGroup:     m_curGroup->members.append(m_curMember); break;
        default:   err("Error: Unexpected tag `member' found\n"); break; 
      }
    }
    void endClass()
    {
      switch(m_state)
      {
        case InFile:      m_curFile->classList.append(m_curString); break;
        case InNamespace: m_curNamespace->classList.append(m_curString); break;
        case InGroup:     m_curGroup->classList.append(m_curString); break;
        default:   err("Error: Unexpected tag `class' found\n"); break; 
      }
    }
    void endNamespace()
    {
      switch(m_state)
      {
        case InFile:      m_curFile->namespaceList.append(m_curString); break;
        case InGroup:     m_curGroup->namespaceList.append(m_curString); break;
        default:   err("Error: Unexpected tag `namespace' found\n"); break; 
      }
    }
    void endFile()
    {
      switch(m_state)
      {
        case InGroup:      m_curGroup->fileList.append(m_curString); break;
        default:   err("Error: Unexpected tag `file' found\n"); break; 
      }
    }
    void endPage()
    {
      switch(m_state)
      {
        case InGroup:      m_curGroup->fileList.append(m_curString); break;
        default:   err("Error: Unexpected tag `page' found\n"); break; 
      }
    }
    void startStringValue(const QXmlAttributes& )
    {
      m_curString = "";
    }
    void endName()
    {
      switch (m_state)
      {
        case InClass:     m_curClass->name     = m_curString; break;
        case InFile:      m_curFile->name      = m_curString; break;
        case InNamespace: m_curNamespace->name = m_curString; break;
        case InGroup:     m_curGroup->name     = m_curString; break;
        case InPage:      m_curPage->name      = m_curString; break;
        case InMember:    m_curMember->name    = m_curString; break;
        default: err("Error: Unexpected tag `name' found\n"); break; 
      }
    }
    void startBase(const QXmlAttributes& attrib )
    {
      m_curString="";
      if (m_state==InClass && m_curClass)
      {
        QString protStr = attrib.value("protection");
        QString virtStr = attrib.value("virtualness");
        Protection prot = Public;
        Specifier  virt = Normal;
        if (protStr=="protected")
        {
          prot = Protected;
        }
        else if (protStr=="private")
        {
          prot = Private;
        }
        if (virtStr=="virtual")
        {
          virt = Virtual;
        }
        if (m_curClass->bases==0) m_curClass->bases = new QList<BaseInfo>;
        m_curClass->bases->append(new BaseInfo(m_curString,prot,virt));
      }
      else
      {
        err("Error: Unexpected tag `base' found\n");
      }
    }
    void endBase()
    {
      if (m_state==InClass && m_curClass)
      {
        m_curClass->bases->getLast()->name = m_curString;
      }
      else
      {
        err("Error: Unexpected tag `base' found\n");
      }
    }
    void endFilename()
    {
      switch (m_state)
      {
        case InClass:     m_curClass->filename     = m_curString;    break;
        case InNamespace: m_curNamespace->filename = m_curString;    break;
        case InFile:      m_curFile->filename      = m_curString;    break;
        case InGroup:     m_curGroup->filename     = m_curString;    break;
        case InPage:      m_curPage->filename      = m_curString;    break;
        default: err("Error: Unexpected tag `filename' found\n"); break; 
      }
    }
    void endPath()
    {
      switch (m_state)
      {
        case InFile:      m_curFile->path          = m_curString;    break;
        default: err("Error: Unexpected tag `path' found\n");     break; 
      }
    }
    void endAnchor()
    {
      if (m_state==InMember)
      {
        m_curMember->anchor = m_curString; 
      }
      else
      {
        err("Error: Unexpected tag `anchor' found\n");
      }
    }
    void endArglist()
    {
      if (m_state==InMember)
      {
        m_curMember->arglist = m_curString; 
      }
      else
      {
        err("Error: Unexpected tag `arglist' found\n");
      }
    }
    void endTitle()
    {
      switch (m_state)
      {
        case InGroup:     m_curGroup->title     = m_curString;    break;
        case InPage:      m_curPage->title      = m_curString;    break;
        default: err("Error: Unexpected tag `title' found\n"); break; 
      }
    }
    void endSubgroup()
    {
      if (m_state==InGroup)
      {
        m_curGroup->subgroupList.append(m_curString);
      }
      else
      {
        err("Error: Unexpected tag `subgroup' found\n");
      }
    }
    void startIgnoreElement(const QXmlAttributes& )
    {
    }
    void endIgnoreElement()
    {
    }

    bool startDocument()
    {
      m_curClass=0;
      m_curNamespace=0;
      m_curFile=0;
      m_curGroup=0;
      m_curPage=0;
      m_state = Invalid;
      m_stateStack.setAutoDelete(TRUE);
      m_tagFileClasses.setAutoDelete(TRUE);
      m_tagFileFiles.setAutoDelete(TRUE);
      m_tagFileNamespaces.setAutoDelete(TRUE);
      m_tagFileGroups.setAutoDelete(TRUE);
      m_tagFilePages.setAutoDelete(TRUE);
      m_startElementHandlers.insert("compound",  new StartElementHandler(this,&TagFileParser::startCompound));
      m_startElementHandlers.insert("member",    new StartElementHandler(this,&TagFileParser::startMember));
      m_startElementHandlers.insert("name",      new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("base",      new StartElementHandler(this,&TagFileParser::startBase));
      m_startElementHandlers.insert("filename",  new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("path",      new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("anchor",    new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("arglist",   new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("title",     new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("subgroup",  new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("class",     new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("namespace", new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("file",      new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("page",      new StartElementHandler(this,&TagFileParser::startStringValue));
      m_startElementHandlers.insert("tagfile",   new StartElementHandler(this,&TagFileParser::startIgnoreElement));
      m_endElementHandlers.insert("compound",    new EndElementHandler(this,&TagFileParser::endCompound));
      m_endElementHandlers.insert("member",      new EndElementHandler(this,&TagFileParser::endMember));
      m_endElementHandlers.insert("name",        new EndElementHandler(this,&TagFileParser::endName));
      m_endElementHandlers.insert("base",        new EndElementHandler(this,&TagFileParser::endBase));
      m_endElementHandlers.insert("filename",    new EndElementHandler(this,&TagFileParser::endFilename));
      m_endElementHandlers.insert("path",        new EndElementHandler(this,&TagFileParser::endPath));
      m_endElementHandlers.insert("anchor",      new EndElementHandler(this,&TagFileParser::endAnchor));
      m_endElementHandlers.insert("arglist",     new EndElementHandler(this,&TagFileParser::endArglist));
      m_endElementHandlers.insert("title",       new EndElementHandler(this,&TagFileParser::endTitle));
      m_endElementHandlers.insert("subgroup",    new EndElementHandler(this,&TagFileParser::endSubgroup));
      m_endElementHandlers.insert("class"   ,    new EndElementHandler(this,&TagFileParser::endClass));
      m_endElementHandlers.insert("namespace",   new EndElementHandler(this,&TagFileParser::endNamespace));
      m_endElementHandlers.insert("file",        new EndElementHandler(this,&TagFileParser::endFile));
      m_endElementHandlers.insert("page",        new EndElementHandler(this,&TagFileParser::endPage));
      m_endElementHandlers.insert("tagfile",     new EndElementHandler(this,&TagFileParser::endIgnoreElement));
      return TRUE;
    }
    bool startElement( const QString&, const QString&, 
                       const QString&name, const QXmlAttributes& attrib )
    {
      //printf("startElement `%s'\n",name.data());
      StartElementHandler *handler = m_startElementHandlers[name];
      if (handler)
      {
        (*handler)(attrib);
      }
      else 
      {
        err("Error: Unknown tag `%s' found!\n",name.data());
      }
      return TRUE;
    }
    bool endElement( const QString&, const QString&, const QString& name )
    {
      //printf("endElement `%s'\n",name.data());
      EndElementHandler *handler = m_endElementHandlers[name];
      if (handler)
      {
        (*handler)();
      }
      else 
      {
        err("Error: Unknown tag `%s' found!\n",name.data());
      }
      return TRUE;
    }
    bool characters ( const QString & ch ) 
    {
      m_curString+=ch;
      return TRUE;
    }
    void dump();
    void buildLists(Entry *root);
    
  private:
    void buildMemberList(Entry *ce,QList<TagMemberInfo> &members);
    QList<TagClassInfo>        m_tagFileClasses;
    QList<TagFileInfo>         m_tagFileFiles;
    QList<TagNamespaceInfo>    m_tagFileNamespaces;
    QList<TagGroupInfo>        m_tagFileGroups;
    QList<TagPageInfo>         m_tagFilePages;
    QDict<StartElementHandler> m_startElementHandlers;
    QDict<EndElementHandler>   m_endElementHandlers;
    TagClassInfo              *m_curClass;
    TagFileInfo               *m_curFile;
    TagNamespaceInfo          *m_curNamespace;
    TagGroupInfo              *m_curGroup;
    TagPageInfo               *m_curPage;
    TagMemberInfo             *m_curMember;
    QCString                   m_curString;
    QString                    m_tagName;
    State                      m_state;
    QStack<State>              m_stateStack;
};

/*! Error handler for the XML tag file parser. 
 *  Basically dumps all fatal error to stderr using err().
 */
class TagFileErrorHandler : public QXmlErrorHandler
{
  public:
    virtual ~TagFileErrorHandler() {}
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
      err("Fatal error at line %d column %d: %s\n",
          exception.lineNumber(),exception.columnNumber(),
          exception.message().data());
      return FALSE;
    }
    QString errorString() { return ""; }

  private:
    QString errorMsg;
};

/*! Dumps the internal structures. For debugging only! */
void TagFileParser::dump()
{
  printf("Result:\n");
  QListIterator<TagClassInfo> lci(m_tagFileClasses);

  //============== CLASSES
  TagClassInfo *cd;
  for (;(cd=lci.current());++lci)
  {
    printf("class `%s'\n",cd->name.data());
    printf("  filename `%s'\n",cd->filename.data());
    if (cd->bases)
    {
      QListIterator<BaseInfo> bii(*cd->bases);
      BaseInfo *bi;
      for ( bii.toFirst() ; (bi=bii.current()) ; ++bii) 
      {
        printf( "  base: %s \n", bi->name.data() );
      }
    }

    QListIterator<TagMemberInfo> mci(cd->members);
    TagMemberInfo *md;
    for (;(md=mci.current());++mci)
    {
      printf("  member:\n");
      printf("    kind: `%s'\n",md->kind.data());
      printf("    name: `%s'\n",md->name.data());
      printf("    anchor: `%s'\n",md->anchor.data());
      printf("    arglist: `%s'\n",md->arglist.data());
    }
  }
  //============== NAMESPACES
  QListIterator<TagNamespaceInfo> lni(m_tagFileNamespaces);
  TagNamespaceInfo *nd;
  for (;(nd=lni.current());++lni)
  {
    printf("namespace `%s'\n",nd->name.data());
    printf("  filename `%s'\n",nd->filename.data());
    QStringList::Iterator it;
    for ( it = nd->classList.begin(); 
        it != nd->classList.end(); ++it ) 
    {
      printf( "  class: %s \n", (*it).latin1() );
    }

    QListIterator<TagMemberInfo> mci(nd->members);
    TagMemberInfo *md;
    for (;(md=mci.current());++mci)
    {
      printf("  member:\n");
      printf("    kind: `%s'\n",md->kind.data());
      printf("    name: `%s'\n",md->name.data());
      printf("    anchor: `%s'\n",md->anchor.data());
      printf("    arglist: `%s'\n",md->arglist.data());
    }
  }
  //============== FILES
  QListIterator<TagFileInfo> lfi(m_tagFileFiles);
  TagFileInfo *fd;
  for (;(fd=lfi.current());++lfi)
  {
    printf("file `%s'\n",fd->name.data());
    printf("  filename `%s'\n",fd->filename.data());
    QStringList::Iterator it;
    for ( it = fd->namespaceList.begin(); 
        it != fd->namespaceList.end(); ++it ) 
    {
      printf( "  namespace: %s \n", (*it).latin1() );
    }
    for ( it = fd->classList.begin(); 
        it != fd->classList.end(); ++it ) 
    {
      printf( "  class: %s \n", (*it).latin1() );
    }

    QListIterator<TagMemberInfo> mci(fd->members);
    TagMemberInfo *md;
    for (;(md=mci.current());++mci)
    {
      printf("  member:\n");
      printf("    kind: `%s'\n",md->kind.data());
      printf("    name: `%s'\n",md->name.data());
      printf("    anchor: `%s'\n",md->anchor.data());
      printf("    arglist: `%s'\n",md->arglist.data());
    }
  }

  //============== GROUPS
  QListIterator<TagGroupInfo> lgi(m_tagFileGroups);
  TagGroupInfo *gd;
  for (;(gd=lgi.current());++lgi)
  {
    printf("group `%s'\n",gd->name.data());
    printf("  filename `%s'\n",gd->filename.data());
    QStringList::Iterator it;
    for ( it = gd->namespaceList.begin(); 
        it != gd->namespaceList.end(); ++it ) 
    {
      printf( "  namespace: %s \n", (*it).latin1() );
    }
    for ( it = gd->classList.begin(); 
        it != gd->classList.end(); ++it ) 
    {
      printf( "  class: %s \n", (*it).latin1() );
    }
    for ( it = gd->fileList.begin(); 
        it != gd->fileList.end(); ++it ) 
    {
      printf( "  file: %s \n", (*it).latin1() );
    }
    for ( it = gd->subgroupList.begin(); 
        it != gd->subgroupList.end(); ++it ) 
    {
      printf( "  subgroup: %s \n", (*it).latin1() );
    }
    for ( it = gd->pageList.begin(); 
        it != gd->pageList.end(); ++it ) 
    {
      printf( "  page: %s \n", (*it).latin1() );
    }

    QListIterator<TagMemberInfo> mci(gd->members);
    TagMemberInfo *md;
    for (;(md=mci.current());++mci)
    {
      printf("  member:\n");
      printf("    kind: `%s'\n",md->kind.data());
      printf("    name: `%s'\n",md->name.data());
      printf("    anchor: `%s'\n",md->anchor.data());
      printf("    arglist: `%s'\n",md->arglist.data());
    }
  }
  //============== PAGES
  QListIterator<TagPageInfo> lpi(m_tagFilePages);
  TagPageInfo *pd;
  for (;(pd=lpi.current());++lpi)
  {
    printf("page `%s'\n",pd->name.data());
    printf("  title `%s'\n",pd->title.data());
    printf("  filename `%s'\n",pd->filename.data());
  }
}

void TagFileParser::buildMemberList(Entry *ce,QList<TagMemberInfo> &members)
{
  QListIterator<TagMemberInfo> mii(members);
  TagMemberInfo *tmi;
  for (;(tmi=mii.current());++mii)
  {
    Entry *me      = new Entry;
    me->name       = tmi->name;
    me->args       = tmi->arglist;
    me->protection = tmi->prot;
    me->virt       = tmi->virt;
    me->stat       = tmi->isStatic;
    TagInfo *ti    = new TagInfo;
    ti->tagName    = m_tagName;
    ti->anchor     = tmi->anchor;
    me->tagInfo    = ti;
    if (tmi->kind=="define")
    {
      me->type="#define";
      me->section = Entry::DEFINE_SEC;
    }
    else if (tmi->kind=="enumvalue")
    {
      me->section = Entry::VARIABLE_SEC;
      me->mtype = Method;
    }
    else if (tmi->kind=="property")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Property;
    }
    else if (tmi->kind=="variable")
    {
      me->section = Entry::VARIABLE_SEC;
      me->mtype = Method;
    }
    else if (tmi->kind=="typedef")
    {
      me->section = Entry::TYPEDEF_SEC;
      me->mtype = Method;
    }
    else if (tmi->kind=="enumeration")
    {
      me->section = Entry::ENUM_SEC;
      me->mtype = Method;
    }
    else if (tmi->kind=="function")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Method;
    }
    else if (tmi->kind=="signal")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Signal;
    }
    else if (tmi->kind=="prototype")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Method;
    }
    else if (tmi->kind=="friend")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Method;
    }
    else if (tmi->kind=="dcop")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = DCOP;
    }
    else if (tmi->kind=="slot")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Slot;
    }
    ce->addSubEntry(me);
  }
}

void TagFileParser::buildLists(Entry *root)
{
  // build class list
  TagClassInfo *tci = m_tagFileClasses.first();
  while (tci)
  {
    Entry *ce = new Entry;
    switch (tci->kind)
    {
      case TagClassInfo::Class:     ce->section = Entry::CLASS_SEC;     break;
      case TagClassInfo::Struct:    ce->section = Entry::STRUCT_SEC;    break;
      case TagClassInfo::Union:     ce->section = Entry::UNION_SEC;     break;
      case TagClassInfo::Interface: ce->section = Entry::INTERFACE_SEC; break;
      case TagClassInfo::Exception: ce->section = Entry::EXCEPTION_SEC; break;
    }
    ce->name     = tci->name;
    TagInfo *ti  = new TagInfo;
    ti->tagName  = m_tagName;
    ti->fileName = tci->filename;
    ce->tagInfo = ti;
    // transfer base class list
    if (tci->bases)
    {
      ce->extends = tci->bases; tci->bases = 0;
    }

    buildMemberList(ce,tci->members);
    root->addSubEntry(ce);
    tci = m_tagFileClasses.next();
  }

  // build file list
  TagFileInfo *tfi = m_tagFileFiles.first();
  while (tfi)
  {
    Entry *fe = new Entry;
    fe->section = guessSection(tfi->name);
    fe->name     = tfi->name;
    TagInfo *ti  = new TagInfo;
    ti->tagName  = m_tagName;
    ti->fileName = tfi->filename;
    fe->tagInfo  = ti;
    
    QString fullName = m_tagName+":"+tfi->path+tfi->name;
    fe->fileName = fullName;
    FileDef *fd = new FileDef(m_tagName+":"+tfi->path,tfi->name,m_tagName);
    FileName *mn;
    if ((mn=inputNameDict->find(tfi->name)))
    {
      mn->append(fd);
    }
    else
    {
      mn = new FileName(fullName,tfi->name);
      mn->append(fd);
      inputNameList.inSort(mn);
      inputNameDict->insert(tfi->name,mn);
    }
    buildMemberList(fe,tfi->members);
    root->addSubEntry(fe);
    tfi = m_tagFileFiles.next();
  }

  // build namespace list
  TagNamespaceInfo *tni = m_tagFileNamespaces.first();
  while (tni)
  {
    Entry *ne    = new Entry;
    ne->section  = Entry::NAMESPACE_SEC;
    ne->name     = tni->name;
    TagInfo *ti  = new TagInfo;
    ti->tagName  = m_tagName;
    ti->fileName = tni->filename;
    ne->tagInfo  = ti;

    buildMemberList(ne,tni->members);
    root->addSubEntry(ne);
    tni = m_tagFileNamespaces.next();
  }

  // build group list
  TagGroupInfo *tgi = m_tagFileGroups.first();
  while (tgi)
  {
    Entry *ge    = new Entry;
    ge->section  = Entry::GROUPDOC_SEC;
    ge->name     = tgi->name;
    ge->type     = tgi->title;
    TagInfo *ti  = new TagInfo;
    ti->tagName  = m_tagName;
    ti->fileName = tgi->filename;
    ge->tagInfo  = ti;

    buildMemberList(ge,tgi->members);
    root->addSubEntry(ge);
    tgi = m_tagFileGroups.next();
  }

  // build page list
  TagPageInfo *tpi = m_tagFilePages.first();
  while (tpi)
  {
    Entry *pe    = new Entry;
    pe->section  = Entry::PAGEDOC_SEC;
    pe->name     = tpi->name;
    pe->args     = tpi->title;
    TagInfo *ti  = new TagInfo;
    ti->tagName  = m_tagName;
    ti->fileName = tpi->filename;
    pe->tagInfo  = ti;

    root->addSubEntry(pe);
    tpi = m_tagFilePages.next();
  }
}

void parseTagFile(Entry *root,const char *fullName,const char *tagName)
{
  QFileInfo fi(fullName);
  if (!fi.exists()) return;
  TagFileParser handler( tagName );
  TagFileErrorHandler errorHandler;
  QFile xmlFile( fullName );
  QXmlInputSource source( xmlFile );
  QXmlSimpleReader reader;
  reader.setContentHandler( &handler );
  reader.setErrorHandler( &errorHandler );
  reader.parse( source );
  handler.buildLists(root);
}

