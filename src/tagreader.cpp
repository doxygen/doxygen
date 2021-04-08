/******************************************************************************
 *
 *
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
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include "tagreader.h"

#include <map>
#include <functional>
#include <utility>
#include <algorithm>

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "xml.h"
#include "entry.h"
#include "doxygen.h"
#include "util.h"
#include "message.h"
#include "defargs.h"
#include "arguments.h"
#include "filedef.h"
#include "filename.h"
#include "section.h"
#include "containers.h"
#include "debug.h"

/** Information about an linkable anchor */
class TagAnchorInfo
{
  public:
    TagAnchorInfo(const std::string &f,
                  const std::string &l,
                  const std::string &t=std::string())
      : label(l), fileName(f), title(t) {}
    std::string label;
    std::string fileName;
    std::string title;
};

/** Container for enum values that are scoped within an enum */
class TagEnumValueInfo
{
  public:
    std::string name;
    std::string file;
    std::string anchor;
    std::string clangid;
};

/** Container for include info that can be read from a tagfile */
class TagIncludeInfo
{
  public:
    std::string id;
    std::string name;
    std::string text;
    bool isLocal;
    bool isImported;
};

/** Container for member specific info that can be read from a tagfile */
class TagMemberInfo
{
  public:
    std::string type;
    std::string name;
    std::string anchorFile;
    std::string anchor;
    std::string arglist;
    std::string kind;
    std::string clangId;
    std::vector<TagAnchorInfo> docAnchors;
    Protection prot = Public;
    Specifier virt = Normal;
    bool isStatic = false;
    std::vector<TagEnumValueInfo> enumValues;
};

/** Base class for all compound types */
class TagCompoundInfo
{
  public:
    enum class CompoundType { Class, Concept, Namespace, Package, File, Group, Page, Dir };
    explicit TagCompoundInfo(CompoundType type) : m_type(type) {}
    virtual ~TagCompoundInfo() {}
    CompoundType compoundType() const { return m_type; }
    std::vector<TagMemberInfo> members;
    std::string name;
    std::string filename;
    std::vector<TagAnchorInfo> docAnchors;
  private:
    CompoundType m_type;
};

/** Container for class specific info that can be read from a tagfile */
class TagClassInfo : public TagCompoundInfo
{
  public:
    enum class Kind { None=-1, Class, Struct, Union, Interface, Exception, Protocol, Category, Enum, Service, Singleton };
    TagClassInfo(Kind k) : TagCompoundInfo(CompoundType::Class), kind(k) {}
    std::string clangId;
    std::string anchor;
    std::vector<BaseInfo> bases;
    StringVector templateArguments;
    StringVector classList;
    Kind kind;
    bool isObjC = false;
    static TagClassInfo *get(std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<TagClassInfo*>(t.get());
    }
    static const TagClassInfo *get(const std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<const TagClassInfo*>(t.get());
    }
};

/** Container for concept specific info that can be read from a tagfile */
class TagConceptInfo : public TagCompoundInfo
{
  public:
    TagConceptInfo() :TagCompoundInfo(CompoundType::Concept) {}
    std::string clangId;
    static TagConceptInfo *get(std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<TagConceptInfo*>(t.get());
    }
    static const TagConceptInfo *get(const std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<const TagConceptInfo*>(t.get());
    }
};

/** Container for namespace specific info that can be read from a tagfile */
class TagNamespaceInfo : public TagCompoundInfo
{
  public:
    TagNamespaceInfo() :TagCompoundInfo(CompoundType::Namespace) {}
    std::string clangId;
    StringVector classList;
    StringVector conceptList;
    StringVector namespaceList;
    static TagNamespaceInfo *get(std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<TagNamespaceInfo*>(t.get());
    }
    static const TagNamespaceInfo *get(const std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<const TagNamespaceInfo*>(t.get());
    }
};

/** Container for package specific info that can be read from a tagfile */
class TagPackageInfo : public TagCompoundInfo
{
  public:
    TagPackageInfo() : TagCompoundInfo(CompoundType::Package) { }
    StringVector classList;
    static TagPackageInfo *get(std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<TagPackageInfo*>(t.get());
    }
    static const TagPackageInfo *get(const std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<const TagPackageInfo*>(t.get());
    }
};

/** Container for file specific info that can be read from a tagfile */
class TagFileInfo : public TagCompoundInfo
{
  public:
    TagFileInfo() : TagCompoundInfo(CompoundType::File) { }
    std::string path;
    StringVector classList;
    StringVector conceptList;
    StringVector namespaceList;
    std::vector<TagIncludeInfo> includes;
    static TagFileInfo *get(std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<TagFileInfo*>(t.get());
    }
    static const TagFileInfo *get(const std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<const TagFileInfo*>(t.get());
    }
};

/** Container for group specific info that can be read from a tagfile */
class TagGroupInfo : public TagCompoundInfo
{
  public:
    TagGroupInfo() : TagCompoundInfo(CompoundType::Group) { }
    std::string title;
    StringVector subgroupList;
    StringVector classList;
    StringVector conceptList;
    StringVector namespaceList;
    StringVector fileList;
    StringVector pageList;
    StringVector dirList;
    static TagGroupInfo *get(std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<TagGroupInfo*>(t.get());
    }
    static const TagGroupInfo *get(const std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<const TagGroupInfo*>(t.get());
    }
};

/** Container for page specific info that can be read from a tagfile */
class TagPageInfo : public TagCompoundInfo
{
  public:
    TagPageInfo() : TagCompoundInfo(CompoundType::Page) {}
    std::string title;
    static TagPageInfo *get(std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<TagPageInfo*>(t.get());
    }
    static const TagPageInfo *get(const std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<const TagPageInfo*>(t.get());
    }
};

/** Container for directory specific info that can be read from a tagfile */
class TagDirInfo : public TagCompoundInfo
{
  public:
    TagDirInfo() : TagCompoundInfo(CompoundType::Dir) {}
    std::string path;
    StringVector subdirList;
    StringVector fileList;
    static TagDirInfo *get(std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<TagDirInfo*>(t.get());
    }
    static const TagDirInfo *get(const std::unique_ptr<TagCompoundInfo> &t)
    {
      return dynamic_cast<const TagDirInfo*>(t.get());
    }
};

/** Tag file parser.
 *
 *  Reads an XML-structured tagfile and builds up the structure in
 *  memory. The method buildLists() is used to transfer/translate
 *  the structures to the doxygen engine.
 */
class TagFileParser
{
  public:
    TagFileParser(const char *tagName) : m_tagName(tagName) {}

    void setDocumentLocator ( const XMLLocator * locator )
    {
      m_locator = locator;
    }

    void startDocument()
    {
      m_state = Invalid;
    }

    void startElement( const std::string &name, const XMLHandlers::Attributes& attrib );
    void endElement( const std::string &name );
    void characters ( const std::string & ch ) { m_curString+=ch; }
    void error( const std::string &fileName,int lineNr,const std::string &msg)
    {
      ::warn(fileName.c_str(),lineNr,"%s",msg.c_str());
    }

    void dump();
    void buildLists(const std::shared_ptr<Entry> &root);
    void addIncludes();
    void startCompound( const XMLHandlers::Attributes& attrib );

    void endCompound()
    {
      switch (m_state)
      {
        case InClass:
        case InConcept:
        case InFile:
        case InNamespace:
        case InGroup:
        case InPage:
        case InDir:
        case InPackage:
          m_tagFileCompounds.push_back(std::move(m_curCompound));
          break;
        default:
          warn("tag 'compound' was not expected!");
          break;
      }
    }

    void startMember( const XMLHandlers::Attributes& attrib)
    {
      m_curMember = TagMemberInfo();
      m_curMember.kind      = XMLHandlers::value(attrib,"kind");
      std::string protStr   = XMLHandlers::value(attrib,"protection");
      std::string virtStr   = XMLHandlers::value(attrib,"virtualness");
      std::string staticStr = XMLHandlers::value(attrib,"static");
      if (protStr=="protected")
      {
        m_curMember.prot = Protected;
      }
      else if (protStr=="private")
      {
        m_curMember.prot = Private;
      }
      if (virtStr=="virtual")
      {
        m_curMember.virt = Virtual;
      }
      else if (virtStr=="pure")
      {
        m_curMember.virt = Pure;
      }
      if (staticStr=="yes")
      {
        m_curMember.isStatic = TRUE;
      }
      m_stateStack.push(m_state);
      m_state = InMember;
    }

    void endMember()
    {
      m_state = m_stateStack.top();
      m_stateStack.pop();
      switch(m_state)
      {
        case InClass:
        case InFile:
        case InNamespace:
        case InGroup:
        case InPackage:
          m_curCompound->members.push_back(m_curMember);
          break;
        default:
          warn("Unexpected tag 'member' found");
          break;
      }
    }

    void startEnumValue( const XMLHandlers::Attributes& attrib)
    {
      if (m_state==InMember)
      {
        m_curString = "";
        m_curEnumValue = TagEnumValueInfo();
        m_curEnumValue.file    = XMLHandlers::value(attrib,"file");
        m_curEnumValue.anchor  = XMLHandlers::value(attrib,"anchor");
        m_curEnumValue.clangid = XMLHandlers::value(attrib,"clangid");
        m_stateStack.push(m_state);
        m_state = InEnumValue;
      }
      else
      {
        warn("Found 'enumvalue' tag outside of member tag");
      }
    }

    void endEnumValue()
    {
      m_curEnumValue.name = QCString(m_curString).stripWhiteSpace();
      m_state = m_stateStack.top();
      m_stateStack.pop();
      if (m_state==InMember)
      {
        m_curMember.enumValues.push_back(m_curEnumValue);
        m_curEnumValue=TagEnumValueInfo();
      }
    }

    void endDocAnchor()
    {
      // Check whether or not the tag is automatically generate, in that case ignore the tag.
      switch(m_state)
      {
        case InClass:
        case InConcept:
        case InFile:
        case InNamespace:
        case InGroup:
        case InPage:
        case InMember:
        case InPackage:
        case InDir:
          if (m_curString.rfind("autotoc_md",0)==0) return;
          break;
        default:
          warn("Unexpected tag 'docanchor' found");
          return;
      }
      switch(m_state)
      {
        case InClass:
        case InConcept:
        case InFile:
        case InNamespace:
        case InGroup:
        case InPage:
        case InPackage:
        case InDir:
          m_curCompound->docAnchors.push_back(TagAnchorInfo(m_fileName,m_curString,m_title));
          break;
        case InMember:
          m_curMember.docAnchors.push_back(TagAnchorInfo(m_fileName,m_curString,m_title));
          break;
        default: break; // will not be reached
      }
    }

    void endClass()
    {
      switch(m_state)
      {
        case InClass:
          TagClassInfo::get(m_curCompound)->classList.push_back(m_curString);
          break;
        case InFile:
          TagFileInfo::get(m_curCompound)->classList.push_back(m_curString);
          break;
        case InNamespace:
          TagNamespaceInfo::get(m_curCompound)->classList.push_back(m_curString);
          break;
        case InGroup:
          TagGroupInfo::get(m_curCompound)->classList.push_back(m_curString);
          break;
        case InPackage:
          TagPackageInfo::get(m_curCompound)->classList.push_back(m_curString);
          break;
        default:
          warn("Unexpected tag 'class' found");
          break;
      }
    }

    void endConcept()
    {
      switch(m_state)
      {
        case InNamespace:
          TagNamespaceInfo::get(m_curCompound)->conceptList.push_back(m_curString);
          break;
        case InFile:
          TagFileInfo::get(m_curCompound)->conceptList.push_back(m_curString);
          break;
        case InGroup:
          TagGroupInfo::get(m_curCompound)->conceptList.push_back(m_curString);
          break;
        default:
          warn("Unexpected tag 'concept' found");
          break;
      }
    }

    void endNamespace()
    {
      switch(m_state)
      {
        case InNamespace:
          TagNamespaceInfo::get(m_curCompound)->namespaceList.push_back(m_curString);
          break;
        case InFile:
          TagFileInfo::get(m_curCompound)->namespaceList.push_back(m_curString);
          break;
        case InGroup:
          TagGroupInfo::get(m_curCompound)->namespaceList.push_back(m_curString);
          break;
        default:
          warn("Unexpected tag 'namespace' found");
          break;
      }
    }

    void endFile()
    {
      switch(m_state)
      {
        case InGroup:
          TagGroupInfo::get(m_curCompound)->fileList.push_back(m_curString);
          break;
        case InDir:
          TagDirInfo::get(m_curCompound)->fileList.push_back(m_curString);
          break;
        default:
          warn("Unexpected tag 'file' found");
          break;
      }
    }

    void endPage()
    {
      switch(m_state)
      {
        case InGroup:
          TagGroupInfo::get(m_curCompound)->fileList.push_back(m_curString);
          break;
        default:
          warn("Unexpected tag 'page' found");
          break;
      }
    }

    void endDir()
    {
      switch(m_state)
      {
        case InDir:
          TagDirInfo::get(m_curCompound)->subdirList.push_back(m_curString);
          break;
        default:
          warn("Unexpected tag 'dir' found");
          break;
      }
    }

    void startStringValue(const XMLHandlers::Attributes& )
    {
      m_curString = "";
    }

    void startDocAnchor(const XMLHandlers::Attributes& attrib )
    {
      m_fileName  = XMLHandlers::value(attrib,"file");
      m_title     = XMLHandlers::value(attrib,"title");
      m_curString = "";
    }

    void endType()
    {
      if (m_state==InMember)
      {
        m_curMember.type = m_curString;
      }
      else
      {
        warn("Unexpected tag 'type' found");
      }
    }

    void endName()
    {
      switch (m_state)
      {
        case InClass:
        case InConcept:
        case InFile:
        case InNamespace:
        case InGroup:
        case InPage:
        case InDir:
        case InPackage:
           m_curCompound->name = m_curString;
           break;
        case InMember:
           m_curMember.name = m_curString;
           break;
        default:
           warn("Unexpected tag 'name' found");
           break;
      }
    }

    void startBase(const XMLHandlers::Attributes& attrib )
    {
      m_curString="";
      if (m_state==InClass && m_curCompound)
      {
        std::string protStr = XMLHandlers::value(attrib,"protection");
        std::string virtStr = XMLHandlers::value(attrib,"virtualness");
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
        TagClassInfo::get(m_curCompound)->bases.push_back(BaseInfo(m_curString.c_str(),prot,virt));
      }
      else
      {
        warn("Unexpected tag 'base' found");
      }
    }

    void endBase()
    {
      if (m_state==InClass && m_curCompound)
      {
        TagClassInfo::get(m_curCompound)->bases.back().name = m_curString;
      }
      else
      {
        warn("Unexpected tag 'base' found");
      }
    }

    void startIncludes(const XMLHandlers::Attributes& attrib )
    {
      m_curIncludes = TagIncludeInfo();
      m_curIncludes.id         = XMLHandlers::value(attrib,"id");
      m_curIncludes.name       = XMLHandlers::value(attrib,"name");
      m_curIncludes.isLocal    = XMLHandlers::value(attrib,"local")=="yes";
      m_curIncludes.isImported = XMLHandlers::value(attrib,"imported")=="yes";
      m_curString="";
    }

    void endIncludes()
    {
      m_curIncludes.text = m_curString;
      if (m_state==InFile && m_curCompound)
      {
        TagFileInfo::get(m_curCompound)->includes.push_back(m_curIncludes);
      }
      else
      {
        warn("Unexpected tag 'includes' found");
      }
    }

    void endTemplateArg()
    {
      if (m_state==InClass && m_curCompound)
      {
        TagClassInfo::get(m_curCompound)->templateArguments.push_back(m_curString);
      }
      else
      {
        warn("Unexpected tag 'templarg' found");
      }
    }

    void endFilename()
    {
      switch (m_state)
      {
        case InClass:
        case InConcept:
        case InNamespace:
        case InFile:
        case InGroup:
        case InPage:
        case InPackage:
        case InDir:
          m_curCompound->filename = m_curString;
          break;
        default:
          warn("Unexpected tag 'filename' found");
          break;
      }
    }

    void endPath()
    {
      switch (m_state)
      {
        case InFile:
          TagFileInfo::get(m_curCompound)->path = m_curString;
          break;
        case InDir:
          TagDirInfo::get(m_curCompound)->path = m_curString;
          break;
        default:
          warn("Unexpected tag 'path' found");
          break;
      }
    }

    void endAnchor()
    {
      if (m_state==InMember)
      {
        m_curMember.anchor = m_curString;
      }
      else if (m_state==InClass)
      {
        TagClassInfo::get(m_curCompound)->anchor = m_curString;
      }
      else
      {
        warn("Unexpected tag 'anchor' found");
      }
    }

    void endClangId()
    {
      if (m_state==InMember)
      {
        m_curMember.clangId = m_curString;
      }
      else if (m_state==InClass)
      {
        TagClassInfo::get(m_curCompound)->clangId =  m_curString;
      }
      else if (m_state==InNamespace)
      {
        TagNamespaceInfo::get(m_curCompound)->clangId = m_curString;
      }
      else
      {
        warn("Unexpected tag 'clangid' found");
      }
    }



    void endAnchorFile()
    {
      if (m_state==InMember)
      {
        m_curMember.anchorFile = m_curString;
      }
      else
      {
        warn("Unexpected tag 'anchorfile' found");
      }
    }

    void endArglist()
    {
      if (m_state==InMember)
      {
        m_curMember.arglist = m_curString;
      }
      else
      {
        warn("Unexpected tag 'arglist' found");
      }
    }

    void endTitle()
    {
      switch (m_state)
      {
        case InGroup:
          TagGroupInfo::get(m_curCompound)->title = m_curString;
          break;
        case InPage:
          TagPageInfo::get(m_curCompound)->title = m_curString;
          break;
        default:
          warn("Unexpected tag 'title' found");
          break;
      }
    }

    void endSubgroup()
    {
      if (m_state==InGroup)
      {
        TagGroupInfo::get(m_curCompound)->subgroupList.push_back(m_curString);
      }
      else
      {
        warn("Unexpected tag 'subgroup' found");
      }
    }

    void startIgnoreElement(const XMLHandlers::Attributes& )
    {
    }

    void endIgnoreElement()
    {
    }

    void buildMemberList(const std::shared_ptr<Entry> &ce,const std::vector<TagMemberInfo> &members);
    void addDocAnchors(const std::shared_ptr<Entry> &e,const std::vector<TagAnchorInfo> &l);


    enum State { Invalid,
                 InClass,
                 InConcept,
                 InFile,
                 InNamespace,
                 InGroup,
                 InPage,
                 InMember,
                 InEnumValue,
                 InPackage,
                 InDir,
                 InTempArgList
               };
  private:

    void warn(const char *fmt)
    {
      std::string fileName = m_locator->fileName();
      ::warn(fileName.c_str(),m_locator->lineNr(),"%s", fmt);
    }

    void warn(const char *fmt,const char *s)
    {
      std::string fileName = m_locator->fileName();
      ::warn(fileName.c_str(),m_locator->lineNr(),fmt,s);
    }


    //------------------------------------

    std::vector< std::unique_ptr<TagCompoundInfo> > m_tagFileCompounds;
    std::unique_ptr<TagCompoundInfo> m_curCompound;

    TagMemberInfo              m_curMember;
    TagEnumValueInfo           m_curEnumValue;
    TagIncludeInfo             m_curIncludes;

    std::string                m_curString;
    std::string                m_tagName;
    std::string                m_fileName;
    std::string                m_title;
    State                      m_state = Invalid;
    std::stack<State>          m_stateStack;
    const XMLLocator          *m_locator = nullptr;
};

//---------------------------------------------------------------------------------------------------------------

struct ElementCallbacks
{
  using StartCallback = std::function<void(TagFileParser&,const XMLHandlers::Attributes&)>;
  using EndCallback   = std::function<void(TagFileParser&)>;

  StartCallback startCb;
  EndCallback   endCb;
};

ElementCallbacks::StartCallback startCb(void (TagFileParser::*fn)(const XMLHandlers::Attributes &))
{
  return [fn](TagFileParser &parser,const XMLHandlers::Attributes &attr) { (parser.*fn)(attr); };
}

ElementCallbacks::EndCallback endCb(void (TagFileParser::*fn)())
{
  return [fn](TagFileParser &parser) { (parser.*fn)(); };
}

static const std::map< std::string, ElementCallbacks > g_elementHandlers =
{
  // name,         start element callback,                      end element callback
  { "compound",    { startCb(&TagFileParser::startCompound     ), endCb(&TagFileParser::endCompound     ) } },
  { "member",      { startCb(&TagFileParser::startMember       ), endCb(&TagFileParser::endMember       ) } },
  { "enumvalue",   { startCb(&TagFileParser::startEnumValue    ), endCb(&TagFileParser::endEnumValue    ) } },
  { "name",        { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endName         ) } },
  { "base",        { startCb(&TagFileParser::startBase         ), endCb(&TagFileParser::endBase         ) } },
  { "filename",    { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endFilename     ) } },
  { "includes",    { startCb(&TagFileParser::startIncludes     ), endCb(&TagFileParser::endIncludes     ) } },
  { "path",        { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endPath         ) } },
  { "anchorfile",  { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endAnchorFile   ) } },
  { "anchor",      { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endAnchor       ) } },
  { "clangid",     { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endClangId      ) } },
  { "arglist",     { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endArglist      ) } },
  { "title",       { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endTitle        ) } },
  { "subgroup",    { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endSubgroup     ) } },
  { "class",       { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endClass        ) } },
  { "concept",     { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endConcept      ) } },
  { "namespace",   { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endNamespace    ) } },
  { "file",        { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endFile         ) } },
  { "dir",         { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endDir          ) } },
  { "page",        { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endPage         ) } },
  { "docanchor",   { startCb(&TagFileParser::startDocAnchor    ), endCb(&TagFileParser::endDocAnchor    ) } },
  { "tagfile",     { startCb(&TagFileParser::startIgnoreElement), endCb(&TagFileParser::endIgnoreElement) } },
  { "templarg",    { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endTemplateArg  ) } },
  { "type",        { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endType         ) } }
};

//---------------------------------------------------------------------------------------------------------------

struct CompoundFactory
{
  using CreateFunc = std::function<std::unique_ptr<TagCompoundInfo>()>;
  CompoundFactory(TagFileParser::State s,CreateFunc f) : state(s), make_instance(f) {}
  TagFileParser::State state;
  CreateFunc make_instance;
};

static const std::map< std::string, CompoundFactory > g_compoundFactory =
{
  // kind tag      state        creation function
  { "class",     { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Class);     } } },
  { "struct",    { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Struct);    } } },
  { "union",     { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Union);     } } },
  { "interface", { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Interface); } } },
  { "enum",      { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Enum);      } } },
  { "exception", { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Exception); } } },
  { "protocol",  { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Protocol);  } } },
  { "category",  { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Category);  } } },
  { "service",   { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Service);   } } },
  { "singleton", { TagFileParser::InClass,     []() { return std::make_unique<TagClassInfo>(TagClassInfo::Kind::Singleton); } } },
  { "file",      { TagFileParser::InFile,      []() { return std::make_unique<TagFileInfo>();                               } } },
  { "namespace", { TagFileParser::InNamespace, []() { return std::make_unique<TagNamespaceInfo>();                          } } },
  { "concept",   { TagFileParser::InConcept,   []() { return std::make_unique<TagConceptInfo>();                            } } },
  { "group",     { TagFileParser::InGroup,     []() { return std::make_unique<TagGroupInfo>();                              } } },
  { "page",      { TagFileParser::InPage,      []() { return std::make_unique<TagPageInfo>();                               } } },
  { "package",   { TagFileParser::InPackage,   []() { return std::make_unique<TagPackageInfo>();                            } } },
  { "dir",       { TagFileParser::InDir,       []() { return std::make_unique<TagDirInfo>();                                } } }
};

//---------------------------------------------------------------------------------------------------------------

void TagFileParser::startElement( const std::string &name, const XMLHandlers::Attributes& attrib )
{
  //printf("startElement '%s'\n",name.data());
  auto it = g_elementHandlers.find(name);
  if (it!=std::end(g_elementHandlers))
  {
    it->second.startCb(*this,attrib);
  }
  else
  {
    warn("Unknown start tag '%s' found!",name.data());
  }
}

void TagFileParser::endElement( const std::string &name )
{
  //printf("endElement '%s'\n",name.data());
  auto it = g_elementHandlers.find(name);
  if (it!=std::end(g_elementHandlers))
  {
    it->second.endCb(*this);
  }
  else
  {
    warn("Unknown end tag '%s' found!",name.data());
  }
}

void TagFileParser::startCompound( const XMLHandlers::Attributes& attrib )
{
  m_curString = "";
  std::string kind   = XMLHandlers::value(attrib,"kind");
  std::string isObjC = XMLHandlers::value(attrib,"objc");

  auto it = g_compoundFactory.find(kind);
  if (it!=g_compoundFactory.end())
  {
    m_curCompound = it->second.make_instance();
    m_state       = it->second.state;
  }
  else
  {
    warn("Unknown compound attribute '%s' found!",kind.c_str());
    m_state = Invalid;
  }

  if (isObjC=="yes" && m_curCompound &&
      m_curCompound->compoundType()==TagCompoundInfo::CompoundType::Class)
  {
    TagClassInfo::get(m_curCompound)->isObjC = TRUE;
  }
}

/*! Dumps the internal structures. For debugging only! */
void TagFileParser::dump()
{
  msg("Result:\n");
  //============== CLASSES
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Class)
    {
      const TagClassInfo *cd = TagClassInfo::get(comp);
      msg("class '%s'\n",cd->name.data());
      msg("  filename '%s'\n",cd->filename.data());
      for (const BaseInfo &bi : cd->bases)
      {
        msg( "  base: %s \n", bi.name.isEmpty() ? "" : bi.name.data() );
      }

      for (const auto &md : cd->members)
      {
        msg("  member:\n");
        msg("    kind: '%s'\n",md.kind.c_str());
        msg("    name: '%s'\n",md.name.c_str());
        msg("    anchor: '%s'\n",md.anchor.c_str());
        msg("    arglist: '%s'\n",md.arglist.c_str());
      }
    }
  }
  //============== CONCEPTS
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Concept)
    {
      const TagConceptInfo *cd = TagConceptInfo::get(comp);

      msg("concept '%s'\n",cd->name.data());
      msg("  filename '%s'\n",cd->filename.data());
    }
  }
  //============== NAMESPACES
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Namespace)
    {
      const TagNamespaceInfo *nd = TagNamespaceInfo::get(comp);

      msg("namespace '%s'\n",nd->name.data());
      msg("  filename '%s'\n",nd->filename.data());
      for (const auto &cls : nd->classList)
      {
        msg( "  class: %s \n", cls.c_str() );
      }

      for (const auto &md : nd->members)
      {
        msg("  member:\n");
        msg("    kind: '%s'\n",md.kind.data());
        msg("    name: '%s'\n",md.name.data());
        msg("    anchor: '%s'\n",md.anchor.data());
        msg("    arglist: '%s'\n",md.arglist.data());
      }
    }
  }

  //============== FILES
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::File)
    {
      const TagFileInfo *fd = TagFileInfo::get(comp);

      msg("file '%s'\n",fd->name.data());
      msg("  filename '%s'\n",fd->filename.data());
      for (const auto &ns : fd->namespaceList)
      {
        msg( "  namespace: %s \n", ns.c_str() );
      }
      for (const auto &cs : fd->classList)
      {
        msg( "  class: %s \n", cs.c_str() );
      }

      for (const auto &md : fd->members)
      {
        msg("  member:\n");
        msg("    kind: '%s'\n",md.kind.data());
        msg("    name: '%s'\n",md.name.data());
        msg("    anchor: '%s'\n",md.anchor.data());
        msg("    arglist: '%s'\n",md.arglist.data());
      }

      for (const auto &ii : fd->includes)
      {
        msg("  includes id: %s name: %s\n",ii.id.data(),ii.name.data());
      }
    }
  }

  //============== GROUPS
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Group)
    {
      const TagGroupInfo *gd = TagGroupInfo::get(comp);
      msg("group '%s'\n",gd->name.data());
      msg("  filename '%s'\n",gd->filename.data());

      for (const auto &ns : gd->namespaceList)
      {
        msg( "  namespace: %s \n", ns.c_str() );
      }
      for (const auto &cs : gd->classList)
      {
        msg( "  class: %s \n", cs.c_str() );
      }
      for (const auto &fi : gd->fileList)
      {
        msg( "  file: %s \n", fi.c_str() );
      }
      for (const auto &sg : gd->subgroupList)
      {
        msg( "  subgroup: %s \n", sg.c_str() );
      }
      for (const auto &pg : gd->pageList)
      {
        msg( "  page: %s \n", pg.c_str() );
      }

      for (const auto &md : gd->members)
      {
        msg("  member:\n");
        msg("    kind: '%s'\n",md.kind.data());
        msg("    name: '%s'\n",md.name.data());
        msg("    anchor: '%s'\n",md.anchor.data());
        msg("    arglist: '%s'\n",md.arglist.data());
      }
    }
  }

  //============== PAGES
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Page)
    {
      const TagPageInfo *pd = TagPageInfo::get(comp);
      msg("page '%s'\n",pd->name.data());
      msg("  title '%s'\n",pd->title.data());
      msg("  filename '%s'\n",pd->filename.data());
    }
  }

  //============== DIRS
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Dir)
    {
      const TagDirInfo *dd = TagDirInfo::get(comp);
      {
        msg("dir '%s'\n",dd->name.data());
        msg("  path '%s'\n",dd->path.data());
        for (const auto &fi : dd->fileList)
        {
          msg( "  file: %s \n", fi.c_str() );
        }
        for (const auto &sd : dd->subdirList)
        {
          msg( "  subdir: %s \n", sd.c_str() );
        }
      }
    }
  }
}

void TagFileParser::addDocAnchors(const std::shared_ptr<Entry> &e,const std::vector<TagAnchorInfo> &l)
{
  for (const auto &ta : l)
  {
    if (SectionManager::instance().find(ta.label.c_str())==0)
    {
      //printf("New sectionInfo file=%s anchor=%s\n",
      //    ta->fileName.data(),ta->label.data());
      SectionInfo *si=SectionManager::instance().add(
          ta.label.c_str(),ta.fileName.c_str(),-1,ta.title.c_str(),
          SectionType::Anchor,0,m_tagName.c_str());
      e->anchors.push_back(si);
    }
    else
    {
      warn("Duplicate anchor %s found",ta.label.data());
    }
  }
}

void TagFileParser::buildMemberList(const std::shared_ptr<Entry> &ce,const std::vector<TagMemberInfo> &members)
{
  for (const auto &tmi : members)
  {
    std::shared_ptr<Entry> me = std::make_shared<Entry>();
    me->type       = tmi.type;
    me->name       = tmi.name;
    me->args       = tmi.arglist;
    if (!me->args.isEmpty())
    {
      me->argList = *stringToArgumentList(SrcLangExt_Cpp,me->args);
    }
    if (tmi.enumValues.size()>0)
    {
      me->spec |= Entry::Strong;
      for (const auto &evi : tmi.enumValues)
      {
        std::shared_ptr<Entry> ev = std::make_shared<Entry>();
        ev->type       = "@";
        ev->name       = evi.name;
        ev->id         = evi.clangid;
        ev->section    = Entry::VARIABLE_SEC;
        ev->tagInfoData.tagName    = m_tagName;
        ev->tagInfoData.anchor     = evi.anchor;
        ev->tagInfoData.fileName   = evi.file;
        ev->hasTagInfo    = TRUE;
        me->moveToSubEntryAndKeep(ev);
      }
    }
    me->protection = tmi.prot;
    me->virt       = tmi.virt;
    me->stat       = tmi.isStatic;
    me->fileName   = ce->fileName;
    me->id         = tmi.clangId;
    if (ce->section == Entry::GROUPDOC_SEC)
    {
      me->groups.push_back(Grouping(ce->name,Grouping::GROUPING_INGROUP));
    }
    addDocAnchors(me,tmi.docAnchors);
    me->tagInfoData.tagName    = m_tagName;
    me->tagInfoData.anchor     = tmi.anchor;
    me->tagInfoData.fileName   = tmi.anchorFile;
    me->hasTagInfo    = TRUE;
    if (tmi.kind=="define")
    {
      me->type="#define";
      me->section = Entry::DEFINE_SEC;
    }
    else if (tmi.kind=="enumvalue")
    {
      me->section = Entry::VARIABLE_SEC;
      me->mtype = Method;
    }
    else if (tmi.kind=="property")
    {
      me->section = Entry::VARIABLE_SEC;
      me->mtype = Property;
    }
    else if (tmi.kind=="event")
    {
      me->section = Entry::VARIABLE_SEC;
      me->mtype = Event;
    }
    else if (tmi.kind=="variable")
    {
      me->section = Entry::VARIABLE_SEC;
      me->mtype = Method;
    }
    else if (tmi.kind=="typedef")
    {
      me->section = Entry::VARIABLE_SEC; //Entry::TYPEDEF_SEC;
      me->type.prepend("typedef ");
      me->mtype = Method;
    }
    else if (tmi.kind=="enumeration")
    {
      me->section = Entry::ENUM_SEC;
      me->mtype = Method;
    }
    else if (tmi.kind=="function")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Method;
    }
    else if (tmi.kind=="signal")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Signal;
    }
    else if (tmi.kind=="prototype")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Method;
    }
    else if (tmi.kind=="friend")
    {
      me->section = Entry::FUNCTION_SEC;
      me->type.prepend("friend ");
      me->mtype = Method;
    }
    else if (tmi.kind=="dcop")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = DCOP;
    }
    else if (tmi.kind=="slot")
    {
      me->section = Entry::FUNCTION_SEC;
      me->mtype = Slot;
    }
    ce->moveToSubEntryAndKeep(me);
  }
}

static QCString stripPath(const QCString &s)
{
  int i=s.findRev('/');
  if (i!=-1)
  {
    return s.right(s.length()-i-1);
  }
  else
  {
    return s;
  }
}

/*! Injects the info gathered by the XML parser into the Entry tree.
 *  This tree contains the information extracted from the input in a
 *  "unrelated" form.
 */
void TagFileParser::buildLists(const std::shared_ptr<Entry> &root)
{
  // build class list
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Class)
    {
      const TagClassInfo *tci = TagClassInfo::get(comp);
      std::shared_ptr<Entry> ce = std::make_shared<Entry>();
      ce->section = Entry::CLASS_SEC;
      switch (tci->kind)
      {
        case TagClassInfo::Kind::Class:     break;
        case TagClassInfo::Kind::Struct:    ce->spec = Entry::Struct;    break;
        case TagClassInfo::Kind::Union:     ce->spec = Entry::Union;     break;
        case TagClassInfo::Kind::Interface: ce->spec = Entry::Interface; break;
        case TagClassInfo::Kind::Enum:      ce->spec = Entry::Enum;      break;
        case TagClassInfo::Kind::Exception: ce->spec = Entry::Exception; break;
        case TagClassInfo::Kind::Protocol:  ce->spec = Entry::Protocol;  break;
        case TagClassInfo::Kind::Category:  ce->spec = Entry::Category;  break;
        case TagClassInfo::Kind::Service:   ce->spec = Entry::Service;   break;
        case TagClassInfo::Kind::Singleton: ce->spec = Entry::Singleton; break;
        case TagClassInfo::Kind::None:      // should never happen, means not properly initialized
                                      assert(tci->kind != TagClassInfo::Kind::None);
                                      break;
      }
      ce->name     = tci->name;
      if (tci->kind==TagClassInfo::Kind::Protocol)
      {
        ce->name+="-p";
      }
      addDocAnchors(ce,tci->docAnchors);
      ce->tagInfoData.tagName  = m_tagName;
      ce->tagInfoData.anchor   = tci->anchor;
      ce->tagInfoData.fileName = tci->filename;
      ce->hasTagInfo  = TRUE;
      ce->id       = tci->clangId;
      ce->lang     = tci->isObjC ? SrcLangExt_ObjC : SrcLangExt_Unknown;
      // transfer base class list
      ce->extends  = tci->bases;
      if (!tci->templateArguments.empty())
      {
        ArgumentList al;
        for (const auto &argName : tci->templateArguments)
        {
          Argument a;
          a.type = "class";
          a.name = argName.c_str();
          al.push_back(a);
        }
        ce->tArgLists.push_back(al);
      }

      buildMemberList(ce,tci->members);
      root->moveToSubEntryAndKeep(ce);
    }
  }

  // build file list
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::File)
    {
      const TagFileInfo *tfi = TagFileInfo::get(comp);

      std::shared_ptr<Entry> fe = std::make_shared<Entry>();
      fe->section = guessSection(tfi->name.c_str());
      fe->name     = tfi->name.c_str();
      addDocAnchors(fe,tfi->docAnchors);
      fe->tagInfoData.tagName  = m_tagName;
      fe->tagInfoData.fileName = tfi->filename;
      fe->hasTagInfo = TRUE;

      std::string fullName = m_tagName+":"+tfi->path+stripPath(tfi->name).str();
      fe->fileName = fullName;
      //printf("createFileDef() filename=%s\n",tfi->filename.data());
      std::string tagid = m_tagName+":"+tfi->path;
      std::unique_ptr<FileDef> fd { createFileDef(tagid.c_str(),
          tfi->name.c_str(),m_tagName.c_str(),
          tfi->filename.c_str()) };
      FileName *mn;
      if ((mn=Doxygen::inputNameLinkedMap->find(tfi->name.c_str())))
      {
        mn->push_back(std::move(fd));
      }
      else
      {
        mn = Doxygen::inputNameLinkedMap->add(tfi->name.c_str(),fullName.c_str());
        mn->push_back(std::move(fd));
      }
      buildMemberList(fe,tfi->members);
      root->moveToSubEntryAndKeep(fe);
    }
  }

  // build concept list
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Concept)
    {
      const TagConceptInfo *tci = TagConceptInfo::get(comp);

      std::shared_ptr<Entry> ce = std::make_shared<Entry>();
      ce->section  = Entry::CONCEPT_SEC;
      ce->name     = tci->name;
      addDocAnchors(ce,tci->docAnchors);
      ce->tagInfoData.tagName  = m_tagName;
      ce->tagInfoData.fileName = tci->filename;
      ce->hasTagInfo  = TRUE;
      ce->id       = tci->clangId;

      root->moveToSubEntryAndKeep(ce);
    }
  }

  // build namespace list
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Namespace)
    {
      const TagNamespaceInfo *tni = TagNamespaceInfo::get(comp);

      std::shared_ptr<Entry> ne = std::make_shared<Entry>();
      ne->section  = Entry::NAMESPACE_SEC;
      ne->name     = tni->name;
      addDocAnchors(ne,tni->docAnchors);
      ne->tagInfoData.tagName  = m_tagName;
      ne->tagInfoData.fileName = tni->filename;
      ne->hasTagInfo  = TRUE;
      ne->id       = tni->clangId;

      buildMemberList(ne,tni->members);
      root->moveToSubEntryAndKeep(ne);
    }
  }

  // build package list
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Package)
    {
      const TagPackageInfo *tpgi = TagPackageInfo::get(comp);

      std::shared_ptr<Entry> pe = std::make_shared<Entry>();
      pe->section  = Entry::PACKAGE_SEC;
      pe->name     = tpgi->name;
      addDocAnchors(pe,tpgi->docAnchors);
      pe->tagInfoData.tagName  = m_tagName;
      pe->tagInfoData.fileName = tpgi->filename;
      pe->hasTagInfo  = TRUE;

      buildMemberList(pe,tpgi->members);
      root->moveToSubEntryAndKeep(pe);
    }
  }

  // build group list
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Group)
    {
      const TagGroupInfo *tgi = TagGroupInfo::get(comp);

      std::shared_ptr<Entry> ge = std::make_shared<Entry>();
      ge->section  = Entry::GROUPDOC_SEC;
      ge->name     = tgi->name;
      ge->type     = tgi->title;
      addDocAnchors(ge,tgi->docAnchors);
      ge->tagInfoData.tagName  = m_tagName;
      ge->tagInfoData.fileName = tgi->filename;
      ge->hasTagInfo  = TRUE;

      buildMemberList(ge,tgi->members);
      root->moveToSubEntryAndKeep(ge);
    }
  }

  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Group)
    {
      const TagGroupInfo *tgi = TagGroupInfo::get(comp);
      // set subgroup relations bug_774118

      for (const auto &sg : tgi->subgroupList)
      {
        const auto &children = root->children();
        auto i = std::find_if(children.begin(),children.end(),
            [&](const std::shared_ptr<Entry> &e) { return e->name == sg.c_str(); });
        if (i!=children.end())
        {
          (*i)->groups.push_back(Grouping(tgi->name.c_str(),Grouping::GROUPING_INGROUP));
        }
      }
    }
  }

  // build page list
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::Page)
    {
      const TagPageInfo *tpi = TagPageInfo::get(comp);

      std::shared_ptr<Entry> pe = std::make_shared<Entry>();
      bool isIndex = (stripExtensionGeneral(tpi->filename.c_str(),getFileNameExtension(tpi->filename.c_str()))=="index");
      pe->section  = isIndex ? Entry::MAINPAGEDOC_SEC : Entry::PAGEDOC_SEC;
      pe->name     = tpi->name;
      pe->args     = tpi->title;
      addDocAnchors(pe,tpi->docAnchors);
      pe->tagInfoData.tagName  = m_tagName;
      pe->tagInfoData.fileName = tpi->filename;
      pe->hasTagInfo  = TRUE;
      root->moveToSubEntryAndKeep(pe);
    }
  }
}

void TagFileParser::addIncludes()
{
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp->compoundType()==TagCompoundInfo::CompoundType::File)
    {
      const TagFileInfo *tfi = TagFileInfo::get(comp);
      //printf("tag file tagName=%s path=%s name=%s\n",m_tagName.data(),tfi->path.data(),tfi->name.data());
      FileName *fn = Doxygen::inputNameLinkedMap->find(tfi->name.c_str());
      if (fn)
      {
        for (const auto &fd : *fn)
        {
          //printf("input file path=%s name=%s\n",fd->getPath().data(),fd->name().data());
          if (fd->getPath()==QCString(m_tagName+":"+tfi->path))
          {
            //printf("found\n");
            for (const auto &ii : tfi->includes)
            {
              //printf("ii->name='%s'\n",ii->name.data());
              FileName *ifn = Doxygen::inputNameLinkedMap->find(ii.name.c_str());
              ASSERT(ifn!=0);
              if (ifn)
              {
                for (const auto &ifd : *ifn)
                {
                  //printf("ifd->getOutputFileBase()=%s ii->id=%s\n",
                  //        ifd->getOutputFileBase().data(),ii->id.data());
                  if (ifd->getOutputFileBase()==QCString(ii.id))
                  {
                    fd->addIncludeDependency(ifd.get(),ii.text.c_str(),ii.isLocal,ii.isImported);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

void parseTagFile(const std::shared_ptr<Entry> &root,const char *fullName)
{
  TagFileParser tagFileParser(fullName);
  QCString inputStr = fileToString(fullName);
  XMLHandlers handlers;
  // connect the generic events handlers of the XML parser to the specific handlers of the tagFileParser object
  handlers.startDocument = [&tagFileParser]()                                                              { tagFileParser.startDocument(); };
  handlers.startElement  = [&tagFileParser](const std::string &name,const XMLHandlers::Attributes &attrs)  { tagFileParser.startElement(name,attrs); };
  handlers.endElement    = [&tagFileParser](const std::string &name)                                       { tagFileParser.endElement(name); };
  handlers.characters    = [&tagFileParser](const std::string &chars)                                      { tagFileParser.characters(chars); };
  handlers.error         = [&tagFileParser](const std::string &fileName,int lineNr,const std::string &msg) { tagFileParser.error(fileName,lineNr,msg); };
  XMLParser parser(handlers);
  tagFileParser.setDocumentLocator(&parser);
  parser.parse(fullName,inputStr,Debug::isFlagSet(Debug::Lex));
  tagFileParser.buildLists(root);
  tagFileParser.addIncludes();
  //tagFileParser.dump();
}
