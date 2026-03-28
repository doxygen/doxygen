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

#ifndef MODULE_H
#define MODULE_H

#include "definition.h"
#include "linkedmap.h"
#include "filedef.h"

class Entry;
class ClassDef;
class MemberDef;
class NamespaceDef;
class ConceptDef;
class OutputList;
class MemberList;
class MemberLists;
class MemberGroupList;
class ClassLinkedRefMap;
class ConceptLinkedRefMap;
class TextStream;

class ModuleDef;

struct ImportInfo
{
  ImportInfo(ModuleDef *def,const QCString &name,int l,const QCString &pName,bool isExported=false)
    : moduleDef(def), importName(name), line(l), partitionName(pName), exported(isExported) {}

  ModuleDef     *moduleDef;
  QCString       importName;
  int            line;
  QCString       partitionName;
  bool           exported;
  QCString qualifiedName() const
  {
    QCString result=importName;
    if (!partitionName.isEmpty())
    {
      result+=":"+partitionName;
    }
    return result;
  }
};

using ImportInfoList = std::vector<ImportInfo>;
using ModuleMap = std::unordered_map<std::string,ModuleDef *>;
using ImportInfoMap = std::unordered_map<std::string,ImportInfoList>;


class ModuleDef : public DefinitionMutable, public Definition
{
  public:
    ABSTRACT_BASE_CLASS(ModuleDef)

    enum class Type
    {
      Interface,
      Implementation
    };
    virtual Type moduleType() const = 0;
    virtual QCString partitionName() const = 0;
    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual bool isPrimaryInterface() const = 0;
    virtual MemberList *getMemberList(MemberListType lt) const = 0;
    virtual const MemberLists &getMemberLists() const = 0;
    virtual const MemberGroupList &getMemberGroups() const = 0;
    virtual int countVisibleMembers() const = 0;
    virtual const ClassLinkedRefMap &getClasses()  const = 0;
    virtual const ConceptLinkedRefMap &getConcepts() const  = 0;
    virtual FileDef *getFileDef() const = 0;
    virtual const ImportInfoMap &getImports() const = 0;
    virtual const ImportInfoMap &getExports() const = 0;
    virtual const ModuleMap &partitions() const = 0;
    virtual void writeTagFile(TextStream &tagFile) const = 0;
    virtual FileList getUsedFiles() const = 0;
};

ModuleDef            *toModuleDef(Definition *d);
const ModuleDef      *toModuleDef(const Definition *d);

class ModuleLinkedMap : public LinkedMap<ModuleDef>
{
};

class ModuleLinkedRefMap : public LinkedRefMap<ModuleDef>
{
  public:
    bool declVisible() const;
    void writeDeclaration(OutputList &ol,const QCString &header,bool localNames) const;
};

class ModuleList : public std::vector<ModuleDef *>
{
};

class ModuleManager
{
  public:
    static ModuleManager &instance();
    void createModuleDef(const QCString &fileName, int line, int column, bool exported,
                         const QCString &moduleName, const QCString &partitionName=QCString());
    void clear();
    void addHeader(const QCString &moduleFile,int line,const QCString &headerName,bool isSystem);
    void addImport(const QCString &moduleFile,int line,const QCString &importName,bool isExported,const QCString &partitionName=QCString());
    void addClassToModule    (const Entry *root,ClassDef *cd);
    void addConceptToModule  (const Entry *root,ConceptDef *cd);
    void addMemberToModule   (const Entry *root,MemberDef *md);
    void addDocs(const Entry *root);
    void addTagInfo(const QCString &moduleFile,const QCString &tagName,const QCString &clangId);
    void addListReferences();
    void addRequirementReferences();
    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();
    void findSectionsInDocumentation();
    void sortMemberLists();

    void resolvePartitions();
    void resolveImports();
    void collectExportedSymbols();
    void countMembers();
    void writeDocumentation(OutputList &ol);
    int numDocumentedModules() const;
    ModuleLinkedMap &modules();
    const ModuleLinkedMap &modules() const;
    ModuleDef *getPrimaryInterface(const QCString &moduleName) const;

  private:
    ModuleManager();
   ~ModuleManager() = default;
    NON_COPYABLE(ModuleManager)
    void resolvePartitionsRecursively(ModuleDef *intfMod, ModuleDef *mod);
    void collectExportedSymbolsRecursively(ModuleDef *intfMod, ModuleDef *mod);
    struct Private;
    std::unique_ptr<Private> p;

};

#endif
