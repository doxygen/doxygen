/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

#ifndef DIRDEF_H
#define DIRDEF_H

#include "qtbc.h"
#include "sortdict.h"
#include "definition.h"

class FileList;
class ClassSDict;
class QStrList;
class FileDef;

class DirDef : public Definition
{
  public:
    DirDef(const char *path);
    virtual ~DirDef();
    virtual DefType definitionType() { return TypeDir; }
    virtual QCString getOutputFileBase() const { return ""; }
    virtual bool isLinkableInProject() const { return FALSE; }
    virtual bool isLinkable() const { return FALSE; }
    QCString displayName() const { return m_dispName; }
    void addSubDir(DirDef *subdir);
    FileList *   getFiles() const        { return m_fileList; }
    ClassSDict * getClasses() const      { return m_classSDict; }
    DirDef *parent() const { return m_parent; }
    void addFile(FileDef *fd);

    static DirDef *mergeDirectoryInTree(const QCString &path);

  private:
    static DirDef *createNewDir(const char *path);
    static bool matchPath(const QCString &path,QStrList &l);

    QList<DirDef> m_subdirs;
    DirDef *m_parent;
    QCString m_dispName;
    FileList *m_fileList;                 // list of files in the group
    ClassSDict *m_classSDict;             // list of classes in the group
};

void buildDirectories();

#endif
