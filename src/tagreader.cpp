/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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
#include <variant>

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
#include "anchor.h"
#include "moduledef.h"
#include "construct.h"

// ----------------- private part -----------------------------------------------

namespace {

/** Information about an linkable anchor */
class TagAnchorInfo
{
  public:
    TagAnchorInfo(const QCString &f,
                  const QCString &l,
                  const QCString &t=QCString())
      : label(l), fileName(f), title(t) {}
    QCString label;
    QCString fileName;
    QCString title;
};

/** Container for enum values that are scoped within an enum */
class TagEnumValueInfo
{
  public:
    QCString name;
    QCString file;
    QCString anchor;
    QCString clangid;
};

/** Container for include info that can be read from a tagfile */
class TagIncludeInfo
{
  public:
    QCString id;
    QCString name;
    QCString text;
    bool isLocal;
    bool isImported;
    bool isModule;
    bool isObjC;
};

/** Container for member specific info that can be read from a tagfile */
class TagMemberInfo
{
  public:
    QCString type;
    QCString name;
    QCString anchorFile;
    QCString anchor;
    QCString arglist;
    QCString kind;
    QCString clangId;
    std::vector<TagAnchorInfo> docAnchors;
    Protection prot = Protection::Public;
    Specifier virt = Specifier::Normal;
    bool isStatic = false;
    std::vector<TagEnumValueInfo> enumValues;
    int lineNr;
};

/** Base class for all compound types */
struct TagCompoundInfo
{
  std::vector<TagMemberInfo> members;
  QCString name;
  QCString filename;
  std::vector<TagAnchorInfo> docAnchors;
  int lineNr = 0;
};


/** Container for class specific info that can be read from a tagfile */
struct TagClassInfo : public TagCompoundInfo
{
  enum class Kind { None=-1, Class, Struct, Union, Interface, Exception, Protocol, Category, Enum, Service, Singleton };
  TagClassInfo(Kind k) : kind(k) {}
  QCString clangId;
  QCString anchor;
  std::vector<BaseInfo> bases;
  StringVector templateArguments;
  StringVector classList;
  Kind kind;
  bool isObjC = false;
};

using TagClassInfoPtr = std::unique_ptr<TagClassInfo>;

/** Container for concept specific info that can be read from a tagfile */
struct TagConceptInfo : public TagCompoundInfo
{
  QCString clangId;
};

using TagConceptInfoPtr = std::unique_ptr<TagConceptInfo>;

/** Container for module specific info that can be read from a tagfile */
struct TagModuleInfo : public TagCompoundInfo
{
  QCString clangId;
};

using TagModuleInfoPtr = std::unique_ptr<TagModuleInfo>;


/** Container for namespace specific info that can be read from a tagfile */
struct TagNamespaceInfo : public TagCompoundInfo
{
  QCString clangId;
  StringVector classList;
  StringVector conceptList;
  StringVector namespaceList;
};

using TagNamespaceInfoPtr = std::unique_ptr<TagNamespaceInfo>;

/** Container for package specific info that can be read from a tagfile */
struct TagPackageInfo : public TagCompoundInfo
{
  StringVector classList;
};

using TagPackageInfoPtr = std::unique_ptr<TagPackageInfo>;

/** Container for file specific info that can be read from a tagfile */
struct TagFileInfo : public TagCompoundInfo
{
  QCString path;
  StringVector classList;
  StringVector conceptList;
  StringVector namespaceList;
  std::vector<TagIncludeInfo> includes;
};

using TagFileInfoPtr = std::unique_ptr<TagFileInfo>;

/** Container for group specific info that can be read from a tagfile */
struct TagGroupInfo : public TagCompoundInfo
{
  QCString title;
  StringVector subgroupList;
  StringVector classList;
  StringVector conceptList;
  StringVector namespaceList;
  StringVector fileList;
  StringVector pageList;
  StringVector dirList;
  StringVector moduleList;
};

using TagGroupInfoPtr = std::unique_ptr<TagGroupInfo>;

/** Container for page specific info that can be read from a tagfile */
struct TagPageInfo : public TagCompoundInfo
{
  QCString title;
  StringVector subpages;
};

using TagPageInfoPtr = std::unique_ptr<TagPageInfo>;

/** Container for directory specific info that can be read from a tagfile */
struct TagDirInfo : public TagCompoundInfo
{
  QCString path;
  StringVector subdirList;
  StringVector fileList;
};

using TagDirInfoPtr = std::unique_ptr<TagDirInfo>;

/** Container for requirement specific info that can be read from a tagfile */
struct TagRequirementInfo : public TagCompoundInfo
{
  QCString id;
  QCString title;
  QCString fileName;
};

using TagRequirementInfoPtr = std::unique_ptr<TagRequirementInfo>;

/** Variant class that holds a unique pointer to one of the specific container types */
class TagCompoundVariant
{
  public:
    using VariantT = std::variant< std::monostate,         // 0
                                   TagClassInfoPtr,        // 1
                                   TagConceptInfoPtr,      // 2
                                   TagNamespaceInfoPtr,    // 3
                                   TagPackageInfoPtr,      // 4
                                   TagFileInfoPtr,         // 5
                                   TagGroupInfoPtr,        // 6
                                   TagPageInfoPtr,         // 7
                                   TagDirInfoPtr,          // 8
                                   TagModuleInfoPtr,       // 9
                                   TagRequirementInfoPtr>; // 10

    enum class Type : uint8_t
    {
      Uninitialized = 0,
      Class         = 1,
      Concept       = 2,
      Namespace     = 3,
      Package       = 4,
      File          = 5,
      Group         = 6,
      Page          = 7,
      Dir           = 8,
      Module        = 9,
      Requirement   = 10
    };

    TagCompoundVariant() {}
    explicit TagCompoundVariant(VariantT &&v) : m_variant(std::move(v)) {}
    ONLY_DEFAULT_MOVABLE(TagCompoundVariant)

    /** Generic non-const getter */
    template<class R>
    R *get()
    {
      std::unique_ptr<R> *p = std::get_if<std::unique_ptr<R>>(&m_variant);
      return p ? p->get() : nullptr;
    }
    /** Generic const getter */
    template<class R>
    const R *get() const
    {
      const std::unique_ptr<R> *p = std::get_if<std::unique_ptr<R>>(&m_variant);
      return p ? p->get() : nullptr;
    }

    /** Generic factory method to create a variant holding a unique pointer to a given compound type */
    template<class R,typename... Args>
    static TagCompoundVariant make(Args&&... args)
    {
      return TagCompoundVariant(VariantT(std::make_unique<R>(std::forward<Args>(args)...)));
    }

    /** @name convenience const and non-const getters for each variant component
     *  @{
     */
          TagClassInfo       *getClassInfo()             { return get<TagClassInfo      >(); }
    const TagClassInfo       *getClassInfo()       const { return get<TagClassInfo      >(); }
          TagConceptInfo     *getConceptInfo()           { return get<TagConceptInfo    >(); }
    const TagConceptInfo     *getConceptInfo()     const { return get<TagConceptInfo    >(); }
          TagNamespaceInfo   *getNamespaceInfo()         { return get<TagNamespaceInfo  >(); }
    const TagNamespaceInfo   *getNamespaceInfo()   const { return get<TagNamespaceInfo  >(); }
          TagPackageInfo     *getPackageInfo()           { return get<TagPackageInfo    >(); }
    const TagPackageInfo     *getPackageInfo()     const { return get<TagPackageInfo    >(); }
          TagFileInfo        *getFileInfo()              { return get<TagFileInfo       >(); }
    const TagFileInfo        *getFileInfo()        const { return get<TagFileInfo       >(); }
          TagGroupInfo       *getGroupInfo()             { return get<TagGroupInfo      >(); }
    const TagGroupInfo       *getGroupInfo()       const { return get<TagGroupInfo      >(); }
          TagPageInfo        *getPageInfo()              { return get<TagPageInfo       >(); }
    const TagPageInfo        *getPageInfo()        const { return get<TagPageInfo       >(); }
          TagDirInfo         *getDirInfo()               { return get<TagDirInfo        >(); }
    const TagDirInfo         *getDirInfo()         const { return get<TagDirInfo        >(); }
          TagModuleInfo      *getModuleInfo()            { return get<TagModuleInfo     >(); }
    const TagModuleInfo      *getModuleInfo()      const { return get<TagModuleInfo     >(); }
          TagRequirementInfo *getRequirementInfo()       { return get<TagRequirementInfo>(); }
    const TagRequirementInfo *getRequirementInfo() const { return get<TagRequirementInfo>(); }
    /** @} */

    /** Convenience method to get the shared compound info */
    TagCompoundInfo *getCompoundInfo()
    {
      switch(type())
      {
        case Type::Uninitialized: return nullptr;
        case Type::Class:         return getClassInfo();
        case Type::Concept:       return getConceptInfo();
        case Type::Namespace:     return getNamespaceInfo();
        case Type::Package:       return getPackageInfo();
        case Type::File:          return getFileInfo();
        case Type::Group:         return getGroupInfo();
        case Type::Page:          return getPageInfo();
        case Type::Dir:           return getDirInfo();
        case Type::Module:        return getModuleInfo();
        case Type::Requirement:   return getRequirementInfo();
      }
      return nullptr;
    }
    Type type() const
    {
      return static_cast<Type>(m_variant.index());
    }

  private:
    VariantT m_variant;
};


/** Tag file parser.
 *
 *  Reads an XML-structured tagfile and builds up the structure in
 *  memory. The method buildLists() is used to transfer/translate
 *  the structures to the doxygen engine.
 */
class TagFileParser
{
#define p_warn(fmt,...) do {                                             \
     warn(m_locator->fileName(),m_locator->lineNr(),fmt,##__VA_ARGS__);  \
   } while(0)

  public:

    explicit TagFileParser(const char *tagName) : m_tagName(tagName) {}

    void setDocumentLocator ( const XMLLocator * locator )
    {
      m_locator = locator;
    }

    void startDocument()
    {
      m_state = Invalid;
    }

    void startElement( const QCString &name, const XMLHandlers::Attributes& attrib );
    void endElement( const QCString &name );
    void characters ( const QCString & ch ) { m_curString+=ch; }
    void error( const QCString &fileName,int lineNr,const QCString &msg)
    {
      warn(fileName,lineNr,"{}",msg);
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
        case InRequirement:
        case InModule:
        case InPackage:
          m_tagFileCompounds.push_back(std::move(m_curCompound));
          break;
        default:
          p_warn("tag 'compound' was not expected!");
          break;
      }
    }

    void startMember( const XMLHandlers::Attributes& attrib)
    {
      m_curMember = TagMemberInfo();
      m_curMember.kind   = XMLHandlers::value(attrib,"kind");
      QCString protStr   = XMLHandlers::value(attrib,"protection");
      QCString virtStr   = XMLHandlers::value(attrib,"virtualness");
      QCString staticStr = XMLHandlers::value(attrib,"static");
      m_curMember.lineNr = m_locator->lineNr();
      if (protStr=="protected")
      {
        m_curMember.prot = Protection::Protected;
      }
      else if (protStr=="private")
      {
        m_curMember.prot = Protection::Private;
      }
      if (virtStr=="virtual")
      {
        m_curMember.virt = Specifier::Virtual;
      }
      else if (virtStr=="pure")
      {
        m_curMember.virt = Specifier::Pure;
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
          {
            TagCompoundInfo *info = m_curCompound.getCompoundInfo();
            if (info)
            {
              info->members.push_back(m_curMember);
            }
          }
          break;
        default:
          p_warn("Unexpected tag 'member' found");
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
        p_warn("Found 'enumvalue' tag outside of member tag");
      }
    }

    void endEnumValue()
    {
      m_curEnumValue.name = QCString(m_curString).stripWhiteSpace().str();
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
        case InModule:
          if (AnchorGenerator::looksGenerated(m_curString.str())) return;
          break;
        default:
          p_warn("Unexpected tag 'docanchor' found");
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
        case InModule:
          {
            TagCompoundInfo *info = m_curCompound.getCompoundInfo();
            if (info)
            {
              info->docAnchors.emplace_back(m_fileName,m_curString,m_title);
            }
          }
          break;
        case InMember:
          m_curMember.docAnchors.emplace_back(m_fileName,m_curString,m_title);
          break;
        default: break; // will not be reached
      }
    }

    void endClass()
    {
      switch(m_state)
      {
        case InClass:
          {
            TagClassInfo *info = m_curCompound.getClassInfo();
            if (info) info->classList.push_back(m_curString.str());
          }
          break;
        case InFile:
          {
            TagFileInfo *info = m_curCompound.getFileInfo();
            if (info) info->classList.push_back(m_curString.str());
          }
          break;
        case InNamespace:
          {
            TagNamespaceInfo *info = m_curCompound.getNamespaceInfo();
            if (info) info->classList.push_back(m_curString.str());
          }
          break;
        case InGroup:
          {
            TagGroupInfo *info = m_curCompound.getGroupInfo();
            if (info) info->classList.push_back(m_curString.str());
          }
          break;
        case InPackage:
          {
            TagPackageInfo *info = m_curCompound.getPackageInfo();
            if (info) info->classList.push_back(m_curString.str());
          }
          break;
        default:
          p_warn("Unexpected tag 'class' found");
          break;
      }
    }

    void endConcept()
    {
      switch(m_state)
      {
        case InNamespace:
          {
            TagNamespaceInfo *info = m_curCompound.getNamespaceInfo();
            if (info) info->conceptList.push_back(m_curString.str());
          }
          break;
        case InFile:
          {
            TagFileInfo *info = m_curCompound.getFileInfo();
            if (info) info->conceptList.push_back(m_curString.str());
          }
          break;
        case InGroup:
          {
            TagGroupInfo *info = m_curCompound.getGroupInfo();
            if (info) info->conceptList.push_back(m_curString.str());
          }
          break;
        default:
          p_warn("Unexpected tag 'concept' found");
          break;
      }
    }

    void endModule()
    {
      switch(m_state)
      {
        case InGroup:
          {
            TagGroupInfo *info = m_curCompound.getGroupInfo();
            if (info) info->moduleList.push_back(m_curString.str());
          }
          break;
        default:
          p_warn("Unexpected tag 'module' found");
          break;
      }
    }

    void endNamespace()
    {
      switch(m_state)
      {
        case InNamespace:
          {
            TagNamespaceInfo *info = m_curCompound.getNamespaceInfo();
            if (info) info->namespaceList.push_back(m_curString.str());
          }
          break;
        case InFile:
          {
            TagFileInfo *info = m_curCompound.getFileInfo();
            if (info) info->namespaceList.push_back(m_curString.str());
          }
          break;
        case InGroup:
          {
            TagGroupInfo *info = m_curCompound.getGroupInfo();
            if (info) info->namespaceList.push_back(m_curString.str());
          }
          break;
        default:
          p_warn("Unexpected tag 'namespace' found");
          break;
      }
    }

    void endFile()
    {
      switch(m_state)
      {
        case InGroup:
          {
            TagGroupInfo *info = m_curCompound.getGroupInfo();
            if (info) info->fileList.push_back(m_curString.str());
          }
          break;
        case InDir:
          {
            TagDirInfo *info = m_curCompound.getDirInfo();
            if (info) info->fileList.push_back(m_curString.str());
          }
          break;
        default:
          p_warn("Unexpected tag 'file' found");
          break;
      }
    }

    void endPage()
    {
      switch(m_state)
      {
        case InGroup:
          {
            TagGroupInfo *info = m_curCompound.getGroupInfo();
            if (info) info->fileList.push_back(m_curString.str());
          }
          break;
        default:
          p_warn("Unexpected tag 'page' found");
          break;
      }
    }

    void endSubpage()
    {
      switch(m_state)
      {
        case InPage:
          {
            TagPageInfo *info = m_curCompound.getPageInfo();
            if (info) info->subpages.push_back(m_curString.str());
          }
          break;
        default:
          p_warn("Unexpected tag 'subpage' found");
          break;
      }
    }

    void endDir()
    {
      switch(m_state)
      {
        case InDir:
          {
            TagDirInfo *info = m_curCompound.getDirInfo();
            if (info) info->subdirList.push_back(m_curString.str());
          }
          break;
        default:
          p_warn("Unexpected tag 'dir' found");
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
        p_warn("Unexpected tag 'type' found");
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
        case InModule:
          {
            TagCompoundInfo *info = m_curCompound.getCompoundInfo();
            if (info) info->name = m_curString;
          }
          break;
        case InMember:
          m_curMember.name = m_curString;
          break;
        default:
          p_warn("Unexpected tag 'name' found");
          break;
      }
    }

    void endId()
    {
      switch (m_state)
      {
        case InRequirement:
          {
            TagRequirementInfo *info = m_curCompound.getRequirementInfo();
            if (info) info->id = m_curString;
          }
          break;
       default:
          p_warn("Unexpected tag 'id' found");
          break;
      }
    }

    void startBase(const XMLHandlers::Attributes& attrib )
    {
      m_curString="";
      TagClassInfo *info = m_curCompound.getClassInfo();
      if (m_state==InClass && info)
      {
        QCString protStr = XMLHandlers::value(attrib,"protection");
        QCString virtStr = XMLHandlers::value(attrib,"virtualness");
        Protection prot = Protection::Public;
        Specifier  virt = Specifier::Normal;
        if (protStr=="protected")
        {
          prot = Protection::Protected;
        }
        else if (protStr=="private")
        {
          prot = Protection::Private;
        }
        if (virtStr=="virtual")
        {
          virt = Specifier::Virtual;
        }
        info->bases.emplace_back(m_curString,prot,virt);
      }
      else
      {
        p_warn("Unexpected tag 'base' found");
      }
    }

    void endBase()
    {
      TagClassInfo *info = m_curCompound.getClassInfo();
      if (m_state==InClass && info)
      {
        info->bases.back().name = m_curString;
      }
      else
      {
        p_warn("Unexpected tag 'base' found");
      }
    }

    void startIncludes(const XMLHandlers::Attributes& attrib )
    {
      m_curIncludes = TagIncludeInfo();
      m_curIncludes.id         = XMLHandlers::value(attrib,"id");
      m_curIncludes.name       = XMLHandlers::value(attrib,"name");
      m_curIncludes.isLocal    = XMLHandlers::value(attrib,"local")=="yes";
      m_curIncludes.isImported = XMLHandlers::value(attrib,"imported")=="yes";
      m_curIncludes.isModule   = XMLHandlers::value(attrib,"module")=="yes";
      m_curIncludes.isObjC     = XMLHandlers::value(attrib,"objc")=="yes";
      m_curString="";
    }

    void endIncludes()
    {
      m_curIncludes.text = m_curString;
      TagFileInfo *info = m_curCompound.getFileInfo();
      if (m_state==InFile && info)
      {
        info->includes.push_back(m_curIncludes);
      }
      else
      {
        p_warn("Unexpected tag 'includes' found");
      }
    }

    void endTemplateArg()
    {
      TagClassInfo *info = m_curCompound.getClassInfo();
      if (m_state==InClass && info)
      {
        info->templateArguments.push_back(m_curString.str());
      }
      else
      {
        p_warn("Unexpected tag 'templarg' found");
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
        case InRequirement:
        case InModule:
          {
            TagCompoundInfo *info = m_curCompound.getCompoundInfo();
            if (info) info->filename = m_curString;
          }
          break;
        default:
          p_warn("Unexpected tag 'filename' found");
          break;
      }
    }

    void endPath()
    {
      switch (m_state)
      {
        case InFile:
          {
            TagFileInfo *info = m_curCompound.getFileInfo();
            if (info) info->path = m_curString;
          }
          break;
        case InDir:
          {
            TagDirInfo *info = m_curCompound.getDirInfo();
            if (info) info->path = m_curString;
          }
          break;
        default:
          p_warn("Unexpected tag 'path' found");
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
        TagClassInfo *info = m_curCompound.getClassInfo();
        if (info) info->anchor = m_curString;
      }
      else
      {
        p_warn("Unexpected tag 'anchor' found");
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
        TagClassInfo *info = m_curCompound.getClassInfo();
        if (info) info->clangId =  m_curString;
      }
      else if (m_state==InNamespace)
      {
        TagNamespaceInfo *info = m_curCompound.getNamespaceInfo();
        if (info) info->clangId = m_curString;
      }
      else
      {
        p_warn("Unexpected tag 'clangid' found");
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
        p_warn("Unexpected tag 'anchorfile' found");
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
        p_warn("Unexpected tag 'arglist' found");
      }
    }

    void endTitle()
    {
      switch (m_state)
      {
        case InGroup:
          {
            TagGroupInfo *info = m_curCompound.getGroupInfo();
            if (info) info->title = m_curString;
          }
          break;
        case InPage:
          {
            TagPageInfo *info = m_curCompound.getPageInfo();
            if (info) info->title = m_curString;
          }
          break;
        case InRequirement:
          {
            TagRequirementInfo *info = m_curCompound.getRequirementInfo();
            if (info) info->title = m_curString;
          }
          break;
        default:
          p_warn("Unexpected tag 'title' found");
          break;
      }
    }

    void endSubgroup()
    {
      if (m_state==InGroup)
      {
        TagGroupInfo *info = m_curCompound.getGroupInfo();
        if (info) info->subgroupList.push_back(m_curString.str());
      }
      else
      {
        p_warn("Unexpected tag 'subgroup' found");
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
                 InModule,
                 InTempArgList,
                 InRequirement
               };
  private:

    struct ClassNode
    {
      ClassNode(const std::string &n) : name(n) {}
      std::string name;
      const TagClassInfo *tci = nullptr;
      std::unordered_map<std::string,std::unique_ptr<ClassNode>> children;
    };

    void buildClassEntry(const std::shared_ptr<Entry> &root, const TagClassInfo *tci);
    void buildClassTree(const std::shared_ptr<Entry> &root, const ClassNode &node);
    //------------------------------------

    std::vector< TagCompoundVariant > m_tagFileCompounds;
    TagCompoundVariant                m_curCompound;

    TagMemberInfo              m_curMember;
    TagEnumValueInfo           m_curEnumValue;
    TagIncludeInfo             m_curIncludes;

    QCString                   m_curString;
    QCString                   m_tagName;
    QCString                   m_fileName;
    QCString                   m_title;
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
  { "id",          { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endId           ) } },
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
  { "module",      { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endModule       ) } },
  { "namespace",   { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endNamespace    ) } },
  { "file",        { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endFile         ) } },
  { "dir",         { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endDir          ) } },
  { "page",        { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endPage         ) } },
  { "subpage",     { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endSubpage      ) } },
  { "docanchor",   { startCb(&TagFileParser::startDocAnchor    ), endCb(&TagFileParser::endDocAnchor    ) } },
  { "tagfile",     { startCb(&TagFileParser::startIgnoreElement), endCb(&TagFileParser::endIgnoreElement) } },
  { "templarg",    { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endTemplateArg  ) } },
  { "type",        { startCb(&TagFileParser::startStringValue  ), endCb(&TagFileParser::endType         ) } }
};

//---------------------------------------------------------------------------------------------------------------

struct CompoundFactory
{
  using CreateFunc = std::function<TagCompoundVariant()>;
  CompoundFactory(TagFileParser::State s,const CreateFunc &f) : state(s), make_instance(f) {}
  TagFileParser::State state;
  CreateFunc make_instance;
};

static const std::map< std::string, CompoundFactory > g_compoundFactory =
{
  // kind tag        state                         creation function
  { "class",       { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Class);     } } },
  { "struct",      { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Struct);    } } },
  { "union",       { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Union);     } } },
  { "interface",   { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Interface); } } },
  { "enum",        { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Enum);      } } },
  { "exception",   { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Exception); } } },
  { "protocol",    { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Protocol);  } } },
  { "category",    { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Category);  } } },
  { "service",     { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Service);   } } },
  { "singleton",   { TagFileParser::InClass,       []() { return TagCompoundVariant::make<TagClassInfo>(TagClassInfo::Kind::Singleton); } } },
  { "file",        { TagFileParser::InFile,        []() { return TagCompoundVariant::make<TagFileInfo>();                               } } },
  { "namespace",   { TagFileParser::InNamespace,   []() { return TagCompoundVariant::make<TagNamespaceInfo>();                          } } },
  { "concept",     { TagFileParser::InConcept,     []() { return TagCompoundVariant::make<TagConceptInfo>();                            } } },
  { "module",      { TagFileParser::InModule,      []() { return TagCompoundVariant::make<TagModuleInfo>();                             } } },
  { "group",       { TagFileParser::InGroup,       []() { return TagCompoundVariant::make<TagGroupInfo>();                              } } },
  { "page",        { TagFileParser::InPage,        []() { return TagCompoundVariant::make<TagPageInfo>();                               } } },
  { "package",     { TagFileParser::InPackage,     []() { return TagCompoundVariant::make<TagPackageInfo>();                            } } },
  { "dir",         { TagFileParser::InDir,         []() { return TagCompoundVariant::make<TagDirInfo>();                                } } },
  { "requirement", { TagFileParser::InRequirement, []() { return TagCompoundVariant::make<TagRequirementInfo>();                        } } }
};

//---------------------------------------------------------------------------------------------------------------

void TagFileParser::startElement( const QCString &name, const XMLHandlers::Attributes& attrib )
{
  //printf("startElement '%s'\n",qPrint(name));
  auto it = g_elementHandlers.find(name.str());
  if (it!=std::end(g_elementHandlers))
  {
    it->second.startCb(*this,attrib);
  }
  else
  {
    p_warn("Unknown start tag '{}' found!",name);
  }
}

void TagFileParser::endElement( const QCString &name )
{
  //printf("endElement '%s'\n",qPrint(name));
  auto it = g_elementHandlers.find(name.str());
  if (it!=std::end(g_elementHandlers))
  {
    it->second.endCb(*this);
  }
  else
  {
    p_warn("Unknown end tag '{}' found!",name);
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
    TagCompoundInfo *info = m_curCompound.getCompoundInfo();
    if (info) info->lineNr = m_locator->lineNr();
  }
  else
  {
    p_warn("Unknown compound attribute '{}' found!",kind);
    m_state = Invalid;
  }

  TagClassInfo *classInfo = m_curCompound.getClassInfo();
  if (isObjC=="yes" && classInfo)
  {
    classInfo->isObjC = TRUE;
  }
}

/*! Dumps the internal structures. For debugging only! */
void TagFileParser::dump()
{
  Debug::print(Debug::Tag,0,"-------- Results --------\n");
  //============== CLASSES
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp.type()==TagCompoundVariant::Type::Class)
    {
      const TagClassInfo *cd = comp.getClassInfo();
      Debug::print(Debug::Tag,0,"class '{}'\n",cd->name);
      Debug::print(Debug::Tag,0,"  filename '{}'\n",cd->filename);
      for (const BaseInfo &bi : cd->bases)
      {
        Debug::print(Debug::Tag,0, "  base: {}\n",bi.name);
      }

      for (const auto &md : cd->members)
      {
        Debug::print(Debug::Tag,0,"  member:\n");
        Debug::print(Debug::Tag,0,"    kind: '{}'\n",md.kind);
        Debug::print(Debug::Tag,0,"    name: '{}'\n",md.name);
        Debug::print(Debug::Tag,0,"    anchor: '{}'\n",md.anchor);
        Debug::print(Debug::Tag,0,"    arglist: '{}'\n",md.arglist);
      }
    }
  }
  //============== CONCEPTS
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp.type()==TagCompoundVariant::Type::Concept)
    {
      const TagConceptInfo *cd = comp.getConceptInfo();

      Debug::print(Debug::Tag,0,"concept '{}'\n",cd->name);
      Debug::print(Debug::Tag,0,"  filename '{}'\n",cd->filename);
    }
  }
  //============== MODULES
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp.type()==TagCompoundVariant::Type::Module)
    {
      const TagModuleInfo *mi = comp.getModuleInfo();

      Debug::print(Debug::Tag,0,"module '{}'\n",mi->name);
      Debug::print(Debug::Tag,0,"  filename '{}'\n",mi->filename);
    }
  }
  //============== NAMESPACES
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp.type()==TagCompoundVariant::Type::Namespace)
    {
      const TagNamespaceInfo *nd = comp.getNamespaceInfo();

      Debug::print(Debug::Tag,0,"namespace '{}'\n",nd->name);
      Debug::print(Debug::Tag,0,"  filename '{}'\n",nd->filename);
      for (const auto &cls : nd->classList)
      {
        Debug::print(Debug::Tag,0, "  class: {}\n",cls);
      }

      for (const auto &md : nd->members)
      {
        Debug::print(Debug::Tag,0,"  member:\n");
        Debug::print(Debug::Tag,0,"    kind: '{}'\n",md.kind);
        Debug::print(Debug::Tag,0,"    name: '{}'\n",md.name);
        Debug::print(Debug::Tag,0,"    anchor: '{}'\n",md.anchor);
        Debug::print(Debug::Tag,0,"    arglist: '{}'\n",md.arglist);
      }
    }
  }

  //============== FILES
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp.type()==TagCompoundVariant::Type::File)
    {
      const TagFileInfo *fd = comp.getFileInfo();

      Debug::print(Debug::Tag,0,"file '{}'\n",fd->name);
      Debug::print(Debug::Tag,0,"  filename '{}'\n",fd->filename);
      for (const auto &ns : fd->namespaceList)
      {
        Debug::print(Debug::Tag,0, "  namespace: {}\n",ns);
      }
      for (const auto &cs : fd->classList)
      {
        Debug::print(Debug::Tag,0, "  class: {} \n",cs);
      }

      for (const auto &md : fd->members)
      {
        Debug::print(Debug::Tag,0,"  member:\n");
        Debug::print(Debug::Tag,0,"    kind: '{}'\n",md.kind);
        Debug::print(Debug::Tag,0,"    name: '{}'\n",md.name);
        Debug::print(Debug::Tag,0,"    anchor: '{}'\n",md.anchor);
        Debug::print(Debug::Tag,0,"    arglist: '{}'\n",md.arglist);
      }

      for (const auto &ii : fd->includes)
      {
        Debug::print(Debug::Tag,0,"  includes id: {} name: {}\n",ii.id,ii.name);
      }
    }
  }

  //============== GROUPS
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp.type()==TagCompoundVariant::Type::Group)
    {
      const TagGroupInfo *gd = comp.getGroupInfo();
      Debug::print(Debug::Tag,0,"group '{}'\n",gd->name);
      Debug::print(Debug::Tag,0,"  filename '{}'\n",gd->filename);

      for (const auto &ns : gd->namespaceList)
      {
        Debug::print(Debug::Tag,0, "  namespace: {}\n",ns);
      }
      for (const auto &cs : gd->classList)
      {
        Debug::print(Debug::Tag,0, "  class: {}\n",cs);
      }
      for (const auto &fi : gd->fileList)
      {
        Debug::print(Debug::Tag,0, "  file: {}\n",fi);
      }
      for (const auto &sg : gd->subgroupList)
      {
        Debug::print(Debug::Tag,0, "  subgroup: {}\n",sg);
      }
      for (const auto &pg : gd->pageList)
      {
        Debug::print(Debug::Tag,0, "  page: {}\n",pg);
      }

      for (const auto &md : gd->members)
      {
        Debug::print(Debug::Tag,0,"  member:\n");
        Debug::print(Debug::Tag,0,"    kind: '{}'\n",md.kind);
        Debug::print(Debug::Tag,0,"    name: '{}'\n",md.name);
        Debug::print(Debug::Tag,0,"    anchor: '{}'\n",md.anchor);
        Debug::print(Debug::Tag,0,"    arglist: '{}'\n",md.arglist);
      }
    }
  }

  //============== PAGES
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp.type()==TagCompoundVariant::Type::Page)
    {
      const TagPageInfo *pd = comp.getPageInfo();
      Debug::print(Debug::Tag,0,"page '{}'\n",pd->name);
      Debug::print(Debug::Tag,0,"  title '{}'\n",pd->title);
      Debug::print(Debug::Tag,0,"  filename '{}'\n",pd->filename);
    }
  }

  //============== DIRS
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp.type()==TagCompoundVariant::Type::Dir)
    {
      const TagDirInfo *dd = comp.getDirInfo();
      {
        Debug::print(Debug::Tag,0,"dir '{}'\n",dd->name);
        Debug::print(Debug::Tag,0,"  path '{}'\n",dd->path);
        for (const auto &fi : dd->fileList)
        {
          Debug::print(Debug::Tag,0, "  file: {}\n",fi);
        }
        for (const auto &sd : dd->subdirList)
        {
          Debug::print(Debug::Tag,0, "  subdir: {}\n",sd);
        }
      }
    }
  }

  //============== REQUIREMENTS
  for (const auto &comp : m_tagFileCompounds)
  {
    if (comp.type()==TagCompoundVariant::Type::Requirement)
    {
      const TagRequirementInfo *rq = comp.getRequirementInfo();
      Debug::print(Debug::Tag,0,"requirement '{}'\n",rq->id);
      Debug::print(Debug::Tag,0,"  title '{}'\n",rq->title);
      Debug::print(Debug::Tag,0,"  filename '{}'\n",rq->filename);
    }
  }
  Debug::print(Debug::Tag,0,"-------------------------\n");
}

void TagFileParser::addDocAnchors(const std::shared_ptr<Entry> &e,const std::vector<TagAnchorInfo> &l)
{
  for (const auto &ta : l)
  {
    if (SectionManager::instance().find(ta.label)==nullptr)
    {
      //printf("New sectionInfo file=%s anchor=%s\n",
      //    qPrint(ta->fileName),qPrint(ta->label));
      SectionInfo *si=SectionManager::instance().add(
          ta.label,ta.fileName,-1,ta.title,
          SectionType::Anchor,0,m_tagName);
      e->anchors.push_back(si);
    }
    else
    {
      //printf("Replace sectionInfo file=%s anchor=%s\n",
      //    qPrint(ta->fileName),qPrint(ta->label));
      SectionManager::instance().replace(
          ta.label,ta.fileName,-1,ta.title,
          SectionType::Anchor,0,m_tagName);
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
      me->argList = *stringToArgumentList(SrcLangExt::Cpp,me->args);
    }
    if (tmi.enumValues.size()>0)
    {
      me->spec.setStrong(true);
      for (const auto &evi : tmi.enumValues)
      {
        std::shared_ptr<Entry> ev = std::make_shared<Entry>();
        ev->type       = "@";
        ev->name       = evi.name;
        ev->id         = evi.clangid;
        ev->section = EntryType::makeVariable();
        ev->tagInfoData.tagName    = m_tagName;
        ev->tagInfoData.anchor     = evi.anchor;
        ev->tagInfoData.fileName   = evi.file;
        ev->hasTagInfo    = TRUE;
        me->moveToSubEntryAndKeep(ev);
      }
    }
    me->protection = tmi.prot;
    me->virt       = tmi.virt;
    me->isStatic   = tmi.isStatic;
    me->fileName   = ce->fileName;
    me->id         = tmi.clangId;
    me->startLine  = tmi.lineNr;
    if (ce->section.isGroupDoc())
    {
      me->groups.emplace_back(ce->name,Grouping::GROUPING_INGROUP);
    }
    addDocAnchors(me,tmi.docAnchors);
    me->tagInfoData.tagName    = m_tagName;
    me->tagInfoData.anchor     = tmi.anchor;
    me->tagInfoData.fileName   = tmi.anchorFile;
    me->hasTagInfo    = TRUE;
    if (tmi.kind=="define")
    {
      me->type="#define";
      me->section = EntryType::makeDefine();
    }
    else if (tmi.kind=="enumvalue")
    {
      me->section = EntryType::makeVariable();
      me->mtype = MethodTypes::Method;
    }
    else if (tmi.kind=="property")
    {
      me->section = EntryType::makeVariable();
      me->mtype = MethodTypes::Property;
    }
    else if (tmi.kind=="event")
    {
      me->section = EntryType::makeVariable();
      me->mtype = MethodTypes::Event;
    }
    else if (tmi.kind=="variable")
    {
      me->section = EntryType::makeVariable();
      me->mtype = MethodTypes::Method;
    }
    else if (tmi.kind=="typedef")
    {
      me->section = EntryType::makeVariable();
      me->type.prepend("typedef ");
      me->mtype = MethodTypes::Method;
    }
    else if (tmi.kind=="enumeration")
    {
      me->section = EntryType::makeEnum();
      me->mtype = MethodTypes::Method;
    }
    else if (tmi.kind=="function")
    {
      me->section = EntryType::makeFunction();
      me->mtype = MethodTypes::Method;
    }
    else if (tmi.kind=="signal")
    {
      me->section = EntryType::makeFunction();
      me->mtype = MethodTypes::Signal;
    }
    else if (tmi.kind=="prototype")
    {
      me->section = EntryType::makeFunction();
      me->mtype = MethodTypes::Method;
    }
    else if (tmi.kind=="friend")
    {
      me->section = EntryType::makeFunction();
      me->type.prepend("friend ");
      me->mtype = MethodTypes::Method;
    }
    else if (tmi.kind=="dcop")
    {
      me->section = EntryType::makeFunction();
      me->mtype = MethodTypes::DCOP;
    }
    else if (tmi.kind=="slot")
    {
      me->section = EntryType::makeFunction();
      me->mtype = MethodTypes::Slot;
    }
    ce->moveToSubEntryAndKeep(me);
  }
}

void TagFileParser::buildClassEntry(const std::shared_ptr<Entry> &root, const TagClassInfo *tci)
{
  std::shared_ptr<Entry> ce = std::make_shared<Entry>();
  ce->section = EntryType::makeClass();
  switch (tci->kind)
  {
    case TagClassInfo::Kind::Class:     break;
    case TagClassInfo::Kind::Struct:    ce->spec = TypeSpecifier().setStruct(true);    break;
    case TagClassInfo::Kind::Union:     ce->spec = TypeSpecifier().setUnion(true);     break;
    case TagClassInfo::Kind::Interface: ce->spec = TypeSpecifier().setInterface(true); break;
    case TagClassInfo::Kind::Enum:      ce->spec = TypeSpecifier().setEnum(true);      break;
    case TagClassInfo::Kind::Exception: ce->spec = TypeSpecifier().setException(true); break;
    case TagClassInfo::Kind::Protocol:  ce->spec = TypeSpecifier().setProtocol(true);  break;
    case TagClassInfo::Kind::Category:  ce->spec = TypeSpecifier().setCategory(true);  break;
    case TagClassInfo::Kind::Service:   ce->spec = TypeSpecifier().setService(true);   break;
    case TagClassInfo::Kind::Singleton: ce->spec = TypeSpecifier().setSingleton(true); break;
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
  ce->startLine            = tci->lineNr;
  ce->fileName             = m_tagName;
  ce->hasTagInfo           = TRUE;
  ce->id                   = tci->clangId;
  ce->lang                 = tci->isObjC ? SrcLangExt::ObjC : SrcLangExt::Unknown;
  // transfer base class list
  ce->extends  = tci->bases;
  if (!tci->templateArguments.empty())
  {
    ArgumentList al;
    for (const auto &argName : tci->templateArguments)
    {
      Argument a;
      a.type = "class";
      a.name = argName;
      al.push_back(a);
    }
    ce->tArgLists.push_back(al);
  }

  buildMemberList(ce,tci->members);
  root->moveToSubEntryAndKeep(ce);
}

void TagFileParser::buildClassTree(const std::shared_ptr<Entry> &root,const ClassNode &node)
{
  if (node.tci)
  {
    buildClassEntry(root,node.tci);
  }
  for (const auto &child : node.children)
  {
    buildClassTree(root,*child.second);
  }
}

/*! Injects the info gathered by the XML parser into the Entry tree.
 *  This tree contains the information extracted from the input in a
 *  "unrelated" form.
 */
void TagFileParser::buildLists(const std::shared_ptr<Entry> &root)
{
  // First reorganize the entries in m_tagFileCompounds such that
  // outer scope is processed before the nested class scope.
  // To solve issue #11569, where a class nested in a specialization is
  // processed first, which later causes the wrong class to be used
  ClassNode classRoot("");
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagClassInfo *tci = comp.getClassInfo();
    if (tci)
    {
      ClassNode *current = &classRoot;
      auto parts = split(tci->name.str(),"::");
      for (size_t i=0; i<parts.size(); ++i)
      {
        const auto &part = parts[i];
        if (current->children.find(part)==current->children.end()) // new child node
        {
          current->children[part] = std::make_unique<ClassNode>(part);
        }
        current = current->children[part].get();
        if (i==parts.size()-1)
        {
          current->tci = tci;
        }
      }
    }
  }

  // now process the classes following the tree structure
  buildClassTree(root,classRoot);


  // build file list
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagFileInfo *tfi = comp.getFileInfo();
    if (tfi)
    {
      std::shared_ptr<Entry> fe = std::make_shared<Entry>();
      fe->section = guessSection(tfi->name);
      fe->name     = tfi->name;
      addDocAnchors(fe,tfi->docAnchors);
      fe->tagInfoData.tagName  = m_tagName;
      fe->tagInfoData.fileName = tfi->filename;
      fe->hasTagInfo = TRUE;

      QCString fullName = m_tagName+":"+tfi->path+stripPath(tfi->name);
      fe->fileName  = fullName;
      fe->startLine = tfi->lineNr;
      //printf("createFileDef() filename=%s\n",qPrint(tfi->filename));
      QCString tagid = m_tagName+":"+tfi->path;
      auto fd = createFileDef(tagid, tfi->name,m_tagName, tfi->filename);
      FileName *mn = Doxygen::inputNameLinkedMap->find(tfi->name);
      if (mn)
      {
        mn->push_back(std::move(fd));
      }
      else
      {
        mn = Doxygen::inputNameLinkedMap->add(tfi->name,fullName);
        mn->push_back(std::move(fd));
      }
      buildMemberList(fe,tfi->members);
      root->moveToSubEntryAndKeep(fe);
    }
  }

  // build concept list
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagConceptInfo *tci = comp.getConceptInfo();
    if (tci)
    {
      std::shared_ptr<Entry> ce = std::make_shared<Entry>();
      ce->section = EntryType::makeConcept();
      ce->name     = tci->name;
      addDocAnchors(ce,tci->docAnchors);
      ce->tagInfoData.tagName  = m_tagName;
      ce->tagInfoData.fileName = tci->filename;
      ce->startLine   = tci->lineNr;
      ce->fileName    = m_tagName;
      ce->hasTagInfo  = TRUE;
      ce->id       = tci->clangId;

      root->moveToSubEntryAndKeep(ce);
    }
  }

  // build module list
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagModuleInfo *tmi = comp.getModuleInfo();
    if (tmi)
    {
      auto &mm = ModuleManager::instance();
      mm.createModuleDef(tmi->filename,tmi->lineNr,1,true,tmi->name,QCString());
      mm.addTagInfo(tmi->filename,m_tagName,tmi->clangId);

      ModuleDef *mod = mm.getPrimaryInterface(tmi->name);
      if (mod && !tmi->docAnchors.empty())
      {
        std::vector<const SectionInfo *> anchorList;
        for (const auto &ta : tmi->docAnchors)
        {
          if (SectionManager::instance().find(ta.label)==nullptr)
          {
            //printf("New sectionInfo file=%s anchor=%s\n",
            //    qPrint(ta->fileName),qPrint(ta->label));
            SectionInfo *si=SectionManager::instance().add(
                ta.label,ta.fileName,-1,ta.title,
                SectionType::Anchor,0,m_tagName);
            anchorList.push_back(si);
          }
          else
          {
            p_warn("Duplicate anchor {} found",ta.label);
          }
        }
        mod->addSectionsToDefinition(anchorList);
      }
    }
  }


  // build namespace list
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagNamespaceInfo *tni = comp.getNamespaceInfo();
    if (tni)
    {
      std::shared_ptr<Entry> ne = std::make_shared<Entry>();
      ne->section = EntryType::makeNamespace();
      ne->name     = tni->name;
      addDocAnchors(ne,tni->docAnchors);
      ne->tagInfoData.tagName  = m_tagName;
      ne->tagInfoData.fileName = tni->filename;
      ne->startLine   = tni->lineNr;
      ne->fileName    = m_tagName;
      ne->hasTagInfo  = TRUE;
      ne->id       = tni->clangId;

      buildMemberList(ne,tni->members);
      root->moveToSubEntryAndKeep(ne);
    }
  }

  // build package list
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagPackageInfo *tpgi = comp.getPackageInfo();
    if (tpgi)
    {
      std::shared_ptr<Entry> pe = std::make_shared<Entry>();
      pe->section = EntryType::makePackage();
      pe->name     = tpgi->name;
      addDocAnchors(pe,tpgi->docAnchors);
      pe->tagInfoData.tagName  = m_tagName;
      pe->tagInfoData.fileName = tpgi->filename;
      pe->startLine   = tpgi->lineNr;
      pe->fileName    = m_tagName;
      pe->hasTagInfo  = TRUE;

      buildMemberList(pe,tpgi->members);
      root->moveToSubEntryAndKeep(pe);
    }
  }

  // build group list
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagGroupInfo *tgi = comp.getGroupInfo();
    if (tgi)
    {
      std::shared_ptr<Entry> ge = std::make_shared<Entry>();
      ge->section = EntryType::makeGroupDoc();
      ge->name     = tgi->name;
      ge->type     = tgi->title;
      addDocAnchors(ge,tgi->docAnchors);
      ge->tagInfoData.tagName  = m_tagName;
      ge->tagInfoData.fileName = tgi->filename;
      ge->startLine   = tgi->lineNr;
      ge->fileName    = m_tagName;
      ge->hasTagInfo  = TRUE;

      buildMemberList(ge,tgi->members);
      root->moveToSubEntryAndKeep(ge);
    }
  }

  for (const auto &comp : m_tagFileCompounds)
  {
    const TagGroupInfo *tgi = comp.getGroupInfo();
    if (tgi)
    {
      // set subgroup relations bug_774118
      for (const auto &sg : tgi->subgroupList)
      {
        const auto &children = root->children();
        auto i = std::find_if(children.begin(),children.end(),
            [&](const std::shared_ptr<Entry> &e) { return e->name == sg; });
        if (i!=children.end())
        {
          (*i)->groups.emplace_back(tgi->name,Grouping::GROUPING_INGROUP);
        }
      }
    }
  }

  // build page list
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagPageInfo *tpi = comp.getPageInfo();
    if (tpi)
    {
      std::shared_ptr<Entry> pe = std::make_shared<Entry>();
      bool isIndex = (stripExtensionGeneral(tpi->filename,getFileNameExtension(tpi->filename))=="index");
      pe->section  = isIndex ? EntryType::makeMainpageDoc() : EntryType::makePageDoc();
      pe->name     = tpi->name;
      pe->args     = tpi->title;
      for (const auto &subpage : tpi->subpages)
      {
        // we add subpage labels as a kind of "inheritance" relation to prevent
        // needing to add another list to the Entry class.
        pe->extends.emplace_back(stripExtension(subpage),Protection::Public,Specifier::Normal);
      }
      addDocAnchors(pe,tpi->docAnchors);
      pe->tagInfoData.tagName  = m_tagName;
      pe->tagInfoData.fileName = stripExtension(tpi->filename);
      pe->startLine   = tpi->lineNr;
      pe->fileName    = m_tagName;
      pe->hasTagInfo  = TRUE;
      root->moveToSubEntryAndKeep(pe);
    }
  }

  // build requirement list
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagRequirementInfo *tri = comp.getRequirementInfo();
    if (tri)
    {
      std::shared_ptr<Entry> pe = std::make_shared<Entry>();
      pe->section               = EntryType::makeRequirementDoc();
      pe->name                  = tri->id;
      pe->type                  = tri->title;
      pe->tagInfoData.tagName   = m_tagName;
      pe->tagInfoData.fileName  = tri->filename;
      pe->startLine             = tri->lineNr;
      pe->fileName              = m_tagName;
      pe->hasTagInfo            = true;
      //printf("Reading requirement '%s' from tag file. title=%s\n",qPrint(pe->name),qPrint(pe->type));
      root->moveToSubEntryAndKeep(pe);
    }
  }
}

void TagFileParser::addIncludes()
{
  for (const auto &comp : m_tagFileCompounds)
  {
    const TagFileInfo *tfi = comp.getFileInfo();
    if (tfi)
    {
      //printf("tag file tagName=%s path=%s name=%s\n",qPrint(m_tagName),qPrint(tfi->path),qPrint(tfi->name));
      FileName *fn = Doxygen::inputNameLinkedMap->find(tfi->name);
      if (fn)
      {
        for (const auto &fd : *fn)
        {
          //printf("input file path=%s name=%s\n",qPrint(fd->getPath()),qPrint(fd->name()));
          if (fd->getPath()==QCString(m_tagName+":"+tfi->path))
          {
            //printf("found\n");
            for (const auto &ii : tfi->includes)
            {
              //printf("ii->name='%s'\n",qPrint(ii->name));
              FileName *ifn = Doxygen::inputNameLinkedMap->find(ii.name);
              ASSERT(ifn!=nullptr);
              if (ifn)
              {
                for (const auto &ifd : *ifn)
                {
                  //printf("ifd->getOutputFileBase()=%s ii->id=%s\n",
                  //        qPrint(ifd->getOutputFileBase()),qPrint(ii->id));
                  if (ifd->getOutputFileBase()==ii.id)
                  {
                    IncludeKind kind = IncludeKind::IncludeSystem;
                    if (ii.isModule)
                    {
                      kind = IncludeKind::ImportModule;
                    }
                    else if (ii.isImported)
                    {
                      kind = ii.isLocal ? IncludeKind::ImportLocal : IncludeKind::ImportSystem;
                    }
                    else if (ii.isLocal)
                    {
                      kind = IncludeKind::IncludeLocal;
                    }
                    fd->addIncludeDependency(ifd.get(),ii.text,kind);
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

} // namespace

// ----------------- public part -----------------------------------------------

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
  parser.parse(fullName,inputStr.data(),Debug::isFlagSet(Debug::Lex_xml),
               [&]() { DebugLex::print(Debug::Lex_xml,"Entering","libxml/xml.l",fullName); },
               [&]() { DebugLex::print(Debug::Lex_xml,"Finished", "libxml/xml.l",fullName); }
              );
  tagFileParser.buildLists(root);
  tagFileParser.addIncludes();
  if (Debug::isFlagSet(Debug::Tag))
  {
    tagFileParser.dump();
  }
}
