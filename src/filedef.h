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
#include "config.h"
#include "definition.h"

class MemberList;
class FileList;
class ClassList;
class MemberDef;
class ClassDef;
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
    
    /*! Returns nameString with all slashes replaced by underscores. */
    //const char *diskName() const { return diskname; }
    QCString getOutputFileBase() const { return diskname; }
    
    /*! Returns the absolute path including the file name. */
    QCString absFilePath() const { return filepath; }
    
    /*! Returns the name of the verbatim copy of this file (if any). */
    const char *includeName() const { return incName; }
    
    /*! Returns the documentation that was available for this file. */
    //const char *documentation() const { return doc; }
    
    /*! Returns the brief description that was given for this file. */
    //const char *briefDescription() const { return brief; }
    
    /*! Sets the name of the include file to \a n. */
    void setIncludeName(const char *n_) { incName=n_; }
    
    /*! Sets the name of the include file to \a n. */
    //void setBriefDescription(const char *b) { brief=b; }
    
    /*! Sets the documentaton of this file to \a d. */
    //void setDocumentation(const char *d) { doc=d; }
    
    /*! Returns the absolute path of this file. */ 
    QCString getPath() const { return path; }
    
    /*! Returns true iff any documentation for this file is found. */
    //bool hasDocumentation() 
    //  { return extractAllFlag || !doc.isNull() || !brief.isNull(); }
    
    /*! Returns true iff this file was externally defined 
        (i.e. read from a tag file) */ 
    //bool isReference() { return !reference.isNull(); }
    
    /*! Returns the reference name of the external file, if any or 0
        if the file is not defined. */ 
    //const char *getReference() { return reference; }
    
    //void setFileType(FileType ft) { ftype = ft; }
    //FileType fileType() const { return ftype; } 
    
    bool isLinkableInProject()
    {
      return hasDocumentation() && !isReference();
    }

    bool isLinkable()
    {
      return isLinkableInProject() || isReference();
    }

    
    void writeDocumentation(OutputList &ol);
    friend void generatedFileNames();
    void insertMember(MemberDef *fd);
    void insertClass(ClassDef *cd);
    void insertNamespace(NamespaceDef *nd);
    void computeAnchors();

  private: 
    MemberList *memList;
    ClassList  *classList;
    FileList   *includeList;
    NamespaceDict *namespaceDict;
    NamespaceList *namespaceList;
    DefineList *defineList;
    //QCString n;
    //QCString doc;
    //QCString brief;
    //QCString reference;
    QCString path;
    QCString filepath;
    QCString diskname;
    QCString filename;
    QCString incName;
    //FileType ftype;
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
