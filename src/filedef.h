/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef FILEDEF_H
#define FILEDEF_H

#include "index.h"
#include <qlist.h>
#include <qintdict.h>
#include "config.h"
#include "definition.h"

class MemberList;
class FileList;
class ClassList;
class ClassDef;
class MemberDef;
class OutputList;
class DefineList;
class NamespaceDef;
class NamespaceList;
class NamespaceDict;

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

    /*! Returns the unique file name (this may include part of the path). */
    QCString name() const 
    { 
      if (Config::fullPathNameFlag) 
        return filename; 
      else 
        return Definition::name(); 
    } 
    
    QCString getOutputFileBase() const { return diskname; }
    
    /*! Returns the absolute path including the file name. */
    QCString absFilePath() const { return filepath; }
    
    /*! Returns the name of the verbatim copy of this file (if any). */
    const char *includeName() const { return incName; }
    
    void addSourceRef(int line,Definition *d,const char *anchor);
    Definition *getSourceDefinition(int lineNr);
    QCString getSourceAnchor(int lineNr);

    /*! Sets the name of the include file to \a n. */
    void setIncludeName(const char *n_) { incName=n_; }
    
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
    void insertMember(MemberDef *fd);
    void insertClass(ClassDef *cd);
    void insertNamespace(NamespaceDef *nd);
    void computeAnchors();

    void addUsingDirective(NamespaceDef *nd);
    NamespaceList *getUsedNamespaces() const { return usingList; }

  private: 
    MemberList *memList;
    ClassList  *classList;
    FileList   *includeList;
    NamespaceDict *namespaceDict;
    NamespaceList *namespaceList;
    NamespaceList *usingList;
    DefineList *defineList;
    QCString path;
    QCString filepath;
    QCString diskname;
    QCString filename;
    QCString incName;
    QIntDict<Definition> *srcDefDict;
    QIntDict<QCString> *srcAnchorDict;
};

/*! \class FileList filedef.h
    \brief This class is list of file definitions. 
    
    It is based on QList.
*/

class FileList : public QList<FileDef>
{ 
  public:
    FileList();
   ~FileList();
   
   int compareItems(GCI item1,GCI item2);
};

/*! \class FileListIterator filedef.h
    \brief This class represents a file list iterator.
    
    It is based on QListIterator.
*/

class FileListIterator : public QListIterator<FileDef>
{
  public:
    FileListIterator(const FileList &list);
};

#endif
