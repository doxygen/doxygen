/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#ifndef FILEDEF_H
#define FILEDEF_H

#include "index.h"
#include <qlist.h>
#include <qintdict.h>
#include <qdict.h>
#include "config.h"
#include "definition.h"
#include "memberlist.h"
#include "util.h"

class FileDef;
class FileList;
class ClassList;
class ClassDef;
class MemberDef;
class OutputList;
class NamespaceDef;
class NamespaceList;
class NamespaceDict;
class MemberGroupList;
class MemberGroupDict;
class PackageDef;

struct IncludeInfo
{
  IncludeInfo() { fileDef=0; local=FALSE; }
  ~IncludeInfo() {}
  FileDef *fileDef;
  QCString includeName;
  bool local;
};

/*! \class FileDef filedef.h
    \brief A File definition.
    
    An object of this class contains all file information that is gathered.
    This includes the members and compounds defined in the file.
    
    The member writeDocumentation() can be used to generate the page of
    documentation to HTML and LaTeX.
*/

class FileDef : public Definition
{
  friend class FileName;

  public:
    //enum FileType { Source, Header, Unknown };

    FileDef(const char *p,const char *n,const char *ref=0);
   ~FileDef();
    DefType definitionType() { return TypeFile; }

    /*! Returns the unique file name (this may include part of the path). */
    QCString name() const 
    { 
      if (Config_getBool("FULL_PATH_NAMES")) 
        return filename; 
      else 
        return Definition::name(); 
    } 
    
    QCString getOutputFileBase() const 
    { return convertNameToFile(diskname); }

    QCString getFileBase() const
    { return diskname; }

    QCString getSourceFileBase() const
    { return convertNameToFile(diskname+"-source"); }
    
    /*! Returns the name of the verbatim copy of this file (if any). */
    QCString includeName() const 
    { return convertNameToFile(diskname+"-source"); }
    
    /*! Returns the absolute path including the file name. */
    QCString absFilePath() const { return filepath; }
    
    
    /*! Returns the name as it is used in the documentation */
    QCString docName() const { return docname; }
    
    void addSourceRef(int line,Definition *d,MemberDef *md);
    Definition *getSourceDefinition(int lineNr);
    MemberDef *getSourceMember(int lineNr);

    /* Sets the name of the include file to \a n. */
    //void setIncludeName(const char *n_) { incName=n_; }
    
    /*! Returns the absolute path of this file. */ 
    QCString getPath() const { return path; }
    
    bool isLinkableInProject()
    {
      return hasDocumentation() && !isReference();
    }

    bool isLinkable()
    {
      return isLinkableInProject() || isReference();
    }

    void writeDocumentation(OutputList &ol);
    void writeSource(OutputList &ol);
    friend void generatedFileNames();
    void insertMember(MemberDef *md);
    void insertClass(ClassDef *cd);
    void insertNamespace(NamespaceDef *nd);
    void computeAnchors();

    void setPackageDef(PackageDef *pd) { package=pd; }
    PackageDef *packageDef() const { return package; }
    
    void addUsingDirective(NamespaceDef *nd);
    NamespaceList *getUsedNamespaces() const { return usingDirList; }
    void addUsingDeclaration(ClassDef *cd);
    ClassList *getUsedClasses() const { return usingDeclList; }

    bool generateSourceFile() const;

    void addIncludeDependency(FileDef *fd,const char *incName,bool local);
    void addIncludedByDependency(FileDef *fd,const char *incName,bool local);
    QList<IncludeInfo> *includeFileList() const { return includeList; }
    QList<IncludeInfo> *includedByFileList() const { return includedByList; }

    void addMembersToMemberGroup();
    void distributeMemberGroupDocumentation();

    //void generateXML(QTextStream &t);
    //void generateXMLSection(QTextStream &t,MemberList *ml,const char *type);

    MemberList allMemberList;
    MemberList defineMembers;
    MemberList protoMembers;
    MemberList typedefMembers;
    MemberList enumMembers;
    MemberList funcMembers;
    MemberList varMembers;

  protected:
    void addMemberListToGroup(MemberList *,bool (MemberDef::*)() const);

  private: 
    
    ClassList  *classList;
    QDict<IncludeInfo> *includeDict;
    QList<IncludeInfo> *includeList;
    QDict<IncludeInfo> *includedByDict;
    QList<IncludeInfo> *includedByList;
    NamespaceDict *namespaceDict;
    NamespaceList *namespaceList;
    NamespaceList *usingDirList;
    ClassList *usingDeclList;
    //DefineList *defineList;
    QCString path;
    QCString filepath;
    QCString diskname;
    QCString filename;
    QCString docname;
    //QCString incName;
    QIntDict<Definition> *srcDefDict;
    QIntDict<MemberDef> *srcMemberDict;
    bool isSource;

    /* user defined member groups */
    MemberGroupList *memberGroupList;
    MemberGroupDict *memberGroupDict;

    PackageDef *package;
};


class FileList : public QList<FileDef>
{
  public:
    FileList() : m_pathName("tmp") {}
    FileList(const char *path) : QList<FileDef>(), m_pathName(path) {}
   ~FileList() {}
    QCString path() const { return m_pathName; }
    int compareItems(GCI item1,GCI item2)
    {
      FileDef *md1 = (FileDef *)item1;
      FileDef *md2 = (FileDef *)item2;
      return stricmp(md1->name(),md2->name());
    }
  private:
    QCString m_pathName;
};

class OutputNameList : public QList<FileList>
{
  public:
    OutputNameList() : QList<FileList>() {}
   ~OutputNameList() {}
    int compareItems(GCI item1,GCI item2)
    {
      FileList *fl1 = (FileList *)item1;
      FileList *fl2 = (FileList *)item2;
      return stricmp(fl1->path(),fl2->path());
    }
};

class OutputNameDict : public QDict<FileList>
{
  public:
    OutputNameDict(int size) : QDict<FileList>(size) {}
   ~OutputNameDict() {}
};

#endif
