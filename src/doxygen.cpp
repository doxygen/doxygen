/******************************************************************************
 *
 * $Id$
 *
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

#include "qtbc.h"
#include <qfileinf.h>
#include <qfile.h>
#include <qdir.h>
#include <qdict.h>
#include <qregexp.h>
#include <qstrlist.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "version.h"
#include "doxygen.h"
#include "scanner.h"
#include "entry.h"
#include "index.h"
#include "logos.h"
#include "instdox.h"
#include "message.h"
#include "code.h"
#include "config.h"
#include "util.h"
#include "pre.h"
#include "tag.h"

#include "outputlist.h"
#include "declinfo.h"
#include "htmlgen.h"
#include "latexgen.h"
#include "mangen.h"
#include "language.h"
#include "debug.h"

#if defined(_MSC_VER)
#define popen _popen
#endif


// lists
ClassList      classList;          // list of all documented classes
NamespaceList  namespaceList;      // list of all namespaces
PageList       exampleList;        // list of all example files
PageList       pageList;           // list of all related documentation pages
MemberNameList memberNameList;     // list of class member + related functions
MemberNameList functionNameList;   // list of all unrelated functions
//MemberNameList namespaceNameList;  // list of namespace members;
FileNameList   inputNameList;      // list of all input files
StringList     inputFiles;         
FileList       includeFiles;
GroupList      groupList;          // list of all groups
FormulaList    formulaList;        // list of all formulas

// dictionaries
PageDict       pageDict(1009);          // dictionary of all doc pages
PageDict       exampleDict(1009);       // dictionary of all examples
ClassDict      classDict(1009);         // dictionary of all documented classes
NamespaceDict  namespaceDict(257);      // dictionary of all documented namespaces
MemberNameDict memberNameDict(10007);   // dictionary of all class member names
MemberNameDict functionNameDict(10007); // dictionary of all functions
//MemberNameDict namespaceNameDict(10007);// dictionaty of all namespace member names
StringDict     substituteDict(1009);    // dictionary of class name substitutes
SectionDict    sectionDict(257);        // dictionary of all page sections
FileNameDict   inputNameDict(1009);     // dictionary of sections
StringDict     excludeNameDict(1009);   // dictionary of sections
FileNameDict   includeNameDict(1009);   // dictionary of include names
FileNameDict   exampleNameDict(1009);   // dictionary of examples
FileDict       includeDict(1009);       // dictionary of include files
DefineDict     defineDict(10007);       // dictionary of all defines
StringDict     typedefDict(1009);       // dictionary of all typedefs
GroupDict      groupDict(257);          // dictionary of all groups
FormulaDict    formulaDict(1009);       // dictionary of all formulas
FormulaDict    formulaNameDict(1009);   // dictionary of the label name of all formulas

OutputList     *outputList;           // list of output generating objects
  
//bool           unrelatedFunctionsUsed;

//ClassDef unrelatedClass("nothing",ClassDef::Class); 
                                       // dummy class for unrelated functions

int annotatedClasses;
int hierarchyClasses;
int documentedFunctions;
int documentedMembers;
int documentedFiles;
int documentedGroups;
int documentedNamespaces;
int documentedNamespaceMembers;

QTextStream tagFile;

void addMemberDocs(Entry *root,MemberDef *md, const char *funcDecl,
                        bool over_load);

const char idMask[] = "[A-Za-z_][A-Za-z_0-9]*";

//----------------------------------------------------------------------
// Returns the standard string that is generated when the \overload
// command is used.

const char *getOverloadDocs()
{
  return "This is an overloaded member function, "
         "provided for convenience. It differs from the above "
         "function only in what argument(s) it accepts.";
}
      
//----------------------------------------------------------------------------


void buildGroupList(Entry *root)
{
  if (root->section==Entry::GROUPDOC_SEC && root->name.length()>0)
  {
    //printf("Found group %s title=`%s'\n",root->name.data(),root->type.data());
    
    GroupDef *gd;
    
    if ((gd=groupDict[root->name]))
    {
      warn("Warning: group %s already documented\n"
           "         skipping documentation in file %s at line %d\n",
           root->name.data(),root->fileName.data(),root->startLine);
    }
    else
    {
      gd = new GroupDef(root->name,root->type);
      gd->setBriefDescription(root->brief);
      gd->setDocumentation(root->doc);
      gd->addSectionsToDefinition(root->anchors);
      groupList.inSort(gd);
      groupDict.insert(root->name,gd);
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildGroupList(e);
  }
}

//----------------------------------------------------------------------

//void computeGroupRelations(Entry *root)
//{
//  if (root->section==Entry::GROUPDOC_SEC && root->name.length()>0)
//  {
//    GroupDef *gd;
//    
//    if ((gd=groupDict[root->name]))
//    {
//      QListIterator<QCString> sli(*root->groups);
//      QCString *s;
//      for (;(s=sli.current());++sli)
//      {
//        GroupDef *pgd;
//        if (s->length()>0 && (pgd=groupDict[*s]))
//        {
//          pgd->addGroup(gd);
//          printf("Group %s: in group %s\n",gd->groupName(),s->data());
//        }
//      }
//    }
//  }
//  EntryListIterator eli(*root->sublist);
//  Entry *e;
//  for (;(e=eli.current());++eli)
//  {
//    computeGroupRelations(e);
//  }
//}

//----------------------------------------------------------------------

void buildFileList(Entry *root)
{
  if (((root->section==Entry::FILEDOC_SEC) ||
      ((root->section & Entry::FILE_MASK) && Config::extractAllFlag)) &&
      root->name.length()>0
     )
  {
    bool ambig;
    FileDef *fd=findFileDef(&inputNameDict,root->name,ambig);
    if (fd && !ambig)
    {
      if ((root->doc.length()>0 && fd->documentation()) ||
          (root->brief.length()>0 && fd->briefDescription()))
      {
        warn("Warning: file %s already documented\n"
            "         skipping documentation in file %s at line %d\n",
            root->name.data(),root->fileName.data(),root->startLine);
      }
      else
      {
        fd->setDocumentation(root->doc);
        fd->setBriefDescription(root->brief); 
        fd->addSectionsToDefinition(root->anchors);
        QListIterator<QCString> sli(*root->groups);
        QCString *s;
        for (;(s=sli.current());++sli)
        {
          GroupDef *gd=0;
          if (s->length()>0 && (gd=groupDict[*s]))
          {
            gd->addFile(fd);
            //printf("File %s: in group %s\n",fd->name().data(),s->data());
          }
        }
      }
    }
    else
    {
      warn("Warning: the name `%s' supplied as "
           "the second argument in the \\file statement in file "
           "%s at line %d ",
           root->name.data(),
           root->fileName.data(),
           root->startLine);
      if (ambig) // name is ambigious
      {
        warn("matches the following input files:\n");
        showFileDefMatches(&inputNameDict,root->name);
        warn("Please use a more specific name by "
             "including a (larger) part of the path!\n");
      }
      else // name is not an input file
        warn("is not an input file\n");
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildFileList(e);
  }
}

void addIncludeFile(ClassDef *cd,FileDef *ifd,Entry *root)
{
  //printf(">>>>>> includeFile=%s\n",root->includeFile.data());

  bool ambig;
  FileDef *fd=0;
  // see if we need to include a verbatim copy of the header file
  if (!root->includeFile.isNull() && 
      (fd=findFileDef(&inputNameDict,root->includeFile,ambig))==0
     )
  { // explicit request
    warn("Warning: the name `%s' supplied as "
        "the second argument in the \\class statement in file "
        "%s at line %d ",
        root->includeFile.data(),
        root->fileName.data(),
        root->startLine);
    if (ambig) // name is ambigious
    {
      warn("matches the following input files:\n");
      showFileDefMatches(&inputNameDict,root->includeFile);
      warn("Please use a more specific name by "
          "including a (larger) part of the path!\n");
    }
    else // name is not an input file
      warn("is not an input file\n");
  }
  else if (root->includeFile.length()==0 && 
      // see if the file extension makes sense
      guessSection(root->includeFile)==Entry::HEADER_SEC)
  { // implicit assumption
    fd=ifd;
  }

  // if a file is found, we mark it for verbatim inclusion.
  if (fd)
  {
    //printf("fd->name()=%s fd->absFilePath()=%s\n",fd->name().data(),
    //                                              fd->absFilePath().data());
    // set include file definition
    cd->setIncludeFile(fd);
    // set include supplied name 
    cd->setIncludeName(root->includeName);
    fd->setIncludeName(cd->getOutputFileBase()+"-include");
    if (includeDict[fd->absFilePath()]==0) // include not inserted earlier
    {
      includeFiles.inSort(fd);
      includeDict.insert(fd->absFilePath(),fd);
    }
  }
}

/*! Input is a scopeName, output is the scopename split into a
 *  namespace part (as large as possible) and a classname part.
 */
void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName)
{
  QCString clName=scopeName.copy();
  QCString nsName;
  if (clName.length()>0 && namespaceDict[clName])
  { // the whole name is a namespace
    namespaceName=clName.copy();
    className.resize(0);
    return;
  }
  int i,p=clName.length()-2;
  while (p>=0 && (i=clName.findRev("::",p))!=-1) 
    // see if the first part is a namespace
  {
    if (i>0 && namespaceDict[clName.left(i)])
    {
      namespaceName=clName.left(i);
      className=clName.right(clName.length()-i-2);
      return;
    } 
    p=i-2; // try a smaller piece of the scope
  }
  className=scopeName.copy();
  namespaceName.resize(0);
  return;
}

static bool addNamespace(Entry *root,ClassDef *cd)
{
  // see if this class is defined inside a namespace
  if (root->section & Entry::COMPOUND_MASK)
  {
    Entry *e = root->parent;
    while (e)
    {
      if (e->section==Entry::NAMESPACE_SEC)
      {
        NamespaceDef *nd=0;
        if (!e->name.isEmpty() && e->name.at(0)!='@' &&
            (nd=namespaceDict[e->name])
           )
        {
          cd->setNamespace(nd);
          nd->insertClass(cd);
          return TRUE;
        }
      }
      e=e->parent;
    } 
  }
  return FALSE;
}


//----------------------------------------------------------------------
// build a list of all classes mentioned in the documentation
// and all classes that have a documentation block before their definition.
void buildClassList(Entry *root)
{
  if (
       ((root->section & Entry::COMPOUNDDOC_MASK) ||
       ((root->section & Entry::COMPOUND_MASK))) && 
       root->name.length()>0
     )
  {
    QCString fullName=root->name.copy();
    if (fullName.length()==0)
    {
      // this should not be called
      warn("Warning: invalid class found in file %s at %d\n",
                  root->fileName.data(),root->startLine);
    }
    else 
    {
      fullName=stripAnnonymousScope(fullName);

      bool ambig;
      ClassDef *cd;
      if ((cd=getClass(fullName))) 
      {
        if (cd->templateArguments()==0)
        {
          //printf("existing ClassDef tempArgList=%p\n",root->tArgList);
          cd->setTemplateArguments(root->tArgList);
        }
        if (root->doc.length()>0 || root->brief.length()>0) // block contains docs
        { 
          if (cd->documentation()) // class already documented
          {
            warn("Warning: class %s already documented\n"
                 "         skipping documentation in file %s at line %d\n",
                 fullName.data(),root->fileName.data(),root->startLine);
          }
          else // class not documented, use docs in block
          {
            cd->setDocumentation(root->doc);
            cd->setBriefDescription(root->brief);
            cd->addSectionsToDefinition(root->anchors);
            cd->setName(fullName); // change name to match docs
          }
        }
        if (cd->includeFile()==0)
        {
          addIncludeFile(cd,
                         findFileDef(&inputNameDict,root->fileName,ambig),
                         root
                        );
        }
        addNamespace(root,cd);
      }
      else // new class
      {
        ClassDef::CompoundType sec=ClassDef::Class; 
        switch(root->section)
        {
          case Entry::UNION_SEC: 
          case Entry::UNIONDOC_SEC: 
            sec=ClassDef::Union; break;
          case Entry::STRUCT_SEC:
          case Entry::STRUCTDOC_SEC: 
            sec=ClassDef::Struct; break;
        }
        ClassDef *cd=new ClassDef(fullName,sec);
        cd->setDocumentation(root->doc); // copy docs to definition
        cd->setBriefDescription(root->brief);
        cd->setTemplateArguments(root->tArgList);
        cd->setProtection(root->protection);
        cd->addSectionsToDefinition(root->anchors);

        QListIterator<QCString> sli(*root->groups);
        QCString *s;
        for (;(s=sli.current());++sli)
        {
          GroupDef *gd=0;
          if (s->length()>0 && (gd=groupDict[*s]))
          {
            gd->addClass(cd);
            //printf("Compound %s: in group %s\n",cd->name().data(),s->data());
          }
        }

        bool found=addNamespace(root,cd);

        // file definition containing the class cd
        FileDef *ifd=findFileDef(&inputNameDict,root->fileName,ambig);

        addIncludeFile(cd,ifd,root);
        
        // if the class is not in a namespace then we insert 
        // it in the file definition
        if (!found && ifd) ifd->insertClass(cd);
        
        // the empty string test is needed for extract all case
        cd->setBriefDescription(root->brief);
        cd->insertUsedFile(root->fileName);
        // add class to the list
        classList.inSort(cd);
        //printf("ClassDict.insert(%s)\n",resolveDefines(fullName).data());
        //classDict.insert(resolveDefines(fullName),cd);
        classDict.insert(fullName,cd);
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildClassList(e);
  }
}

//----------------------------------------------------------------------
// build a list of all namespaces mentioned in the documentation
// and all namespaces that have a documentation block before their definition.
void buildNamespaceList(Entry *root)
{
  if (
       (root->section==Entry::NAMESPACE_SEC) ||
       (root->section==Entry::NAMESPACEDOC_SEC) && 
       root->name.length()>0
     )
  {
    QCString fullName=root->name.copy();
    if (fullName.length()==0)
    {
      // this should not be called
      warn("Warning: invalid namespace found in file %s at %d\n",
                  root->fileName.data(),root->startLine);
    }
    else 
    {
      //printf("Found namespace %s in %s at line %d\n",root->name.data(),
      //        root->fileName.data(), root->startLine);
      NamespaceDef *nd;
      if ((nd=namespaceDict[fullName])) 
      {
        if (root->doc.length()>0 || root->brief.length()>0) // block contains docs
        { 
          if (nd->documentation().isEmpty() && root->doc.length()>0)
          {
            nd->setDocumentation(root->doc);
            nd->setName(fullName); // change name to match docs
            nd->addSectionsToDefinition(root->anchors);
          }
          else if (!nd->documentation().isEmpty() && root->doc.length()>0)
          {
            warn("Warning: namespace %s already has a detailed description,\n"
                 "         skipping documentation in file %s at line %d\n",
                 fullName.data(),root->fileName.data(),root->startLine);
          }
          if (nd->briefDescription().isEmpty() && root->brief.length()>0)
          {
            nd->setBriefDescription(root->brief);
            nd->setName(fullName); // change name to match docs
          }
          else if (!nd->briefDescription().isEmpty() && root->brief.length()>0)
          {
            warn("Warning: namespace %s already has a brief description,\n"
                 "         skipping documentation in file %s at line %d\n",
                 fullName.data(),root->fileName.data(),root->startLine);
          }
        }
      }
      else /* if (root->doc.length()>0 || 
               root->brief.length()>0 || 
               Config::extractAllFlag
              )
           */ 
      {
        NamespaceDef *nd=new NamespaceDef(fullName);
        nd->setDocumentation(root->doc); // copy docs to definition
        nd->setBriefDescription(root->brief);
        nd->addSectionsToDefinition(root->anchors);

        QListIterator<QCString> sli(*root->groups);
        QCString *s;
        for (;(s=sli.current());++sli)
        {
          GroupDef *gd=0;
          if (s->length()>0 && (gd=groupDict[*s]))
            gd->addNamespace(nd);
        }

        bool ambig;
        // file definition containing the class cd
        FileDef *fd=findFileDef(&inputNameDict,root->fileName,ambig);
        // insert the namespace in the file definition
        if (fd) fd->insertNamespace(nd);

        // the empty string test is needed for extract all case
        nd->setBriefDescription(root->brief);
        nd->insertUsedFile(root->fileName);
        // add class to the list
        namespaceList.inSort(nd);
        namespaceDict.insert(fullName,nd);
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildNamespaceList(e);
  }
}


//----------------------------------------------------------------------
// Searches the Entry tree for Variable documentation sections.
// If found they are stored in their class or in the global list.

void buildVarList(Entry *root)
{
  QRegExp re("([^)]*)");
  int i=-1;
  if (root->name.length()>0 &&
      root->type!="class" &&
      (
       (root->section==Entry::VARIABLE_SEC 
       ) ||
       (root->section==Entry::FUNCTION_SEC && // function variable 
        !root->type.isNull() && root->type.find(re,0)!=-1 && 
         root->type.find("operator")==-1 && root->type.find(")(")==-1 
       )
      ) 
     ) // documented variable
  {
    Debug::print(Debug::Variables,0,
                  "VARIABLE_SEC: \n"
                  "  type=`%s' name=`%s' args=`%s'\n",
                   root->type.data(),
                   root->name.data(),
                   root->args.data()
                );
    //printf("root->parent->name=%s\n",root->parent->name.data());

    if (root->type.isNull() && root->name.find("operator")==-1 &&
        (root->name.find('*')!=-1 || root->name.find('&')!=-1))
    {
      // recover from parse error caused by redundant braces
      root->type=root->name;
      QRegExp re("[a-z_A-Z][a-z_A-Z0-9]*");
      int i,l;
      i=re.match(root->args,0,&l);
      root->name=root->args.mid(i,l);
      root->args=root->args.mid(i+l,
          root->args.find(')',i+l)-i-l);
      //printf("new: type=`%s' name=`%s' args=`%s'\n",
      //    root->type.data(),root->name.data(),root->args.data());
    }
    else
    {
      QRegExp re("([^)]*)");
      i=root->type.find(re,0);
      if (i!=-1) // function variable
      {
        root->type=root->type.left(root->type.length()-1);
        root->args.prepend(")");
      }
    }
    
    QCString scope,name=root->name.copy();
    bool stat=root->stat;
    Entry *p = root->parent;
    while ((p->section & Entry::SCOPE_MASK))
    {
      QCString scopeName = stripAnnonymousScope(p->name);
      if (!scopeName.isEmpty())
      {
        scope.prepend(scopeName);
        break;
      }
      //if (p->name.length()>0 && p->name[0]!='@')
      //{
      //  if (!scope.isEmpty()) scope.prepend("::"); 
      //  scope.prepend(p->name);
      //  break;
      //}
      p=p->parent;
    }
    
    //printf("scope=%s\n",scope.data()); 
    
    int ni;
    if ((ni=root->name.findRev("::"))!=-1) goto nextMember;
               /* skip this member, because it is a 
                * static variable definition (always?), which will be
                * found in a class scope as well, but then we know the
                * correct protection level, so only then it will be
                * inserted in the correct list!
                */

    MemberDef::MemberType mtype;
//    NamespaceDef *nd = 0;
    QCString type=root->type.stripWhiteSpace();
    if (type=="@") 
      mtype=MemberDef::EnumValue;
    else if (type.left(8)=="typedef ") 
      mtype=MemberDef::Typedef;
    else if (type.left(7)=="friend ")
      mtype=MemberDef::Friend;
    else
      mtype=MemberDef::Variable;

    ClassDef *cd=0;
    if (scope.length()>0 && name.length()>0 && (cd=getClass(scope)))
    {
      
      Debug::print(Debug::Variables,0,
                   "  class variable:\n"
                   "    %s' %s'::`%s' `%s' prot=`%d\n",
                   root->type.data(),
                   scope.data(), 
                   name.data(),
                   root->args.data(),
                   root->protection
                  );
      // add template names, if the class is a non-specialized template
      //if (scope.find('<')==-1 && cd->templateArguments())
      //{
      //  scope+=cd->getTemplateNameString();
      //}
      // generate member definition.
      QCString def;
      if (root->type.length()>0)
      {
        if (mtype==MemberDef::Friend)
        {
          def=root->type+" "+name+root->args;
        }
        else
        {
          def=root->type+" "+scope+"::"+name+root->args;
        }
      }
      else
      {
        def=scope+"::"+name+root->args;
      }
      if (def.left(7)=="static ") def=def.right(def.length()-7);

      // see if the member is already found in the same scope
      // (this may be the case for a static member that is initialized
      //  outside the class)
      bool found=FALSE;
      MemberName *mn=memberNameDict[name];
      if (mn)
      {
        MemberDef *md=mn->first();
        while (md && !found)
        {
          if (md->memberClass()==cd) // member already in the scope
          {
            addMemberDocs(root,md,def,FALSE);
            found=TRUE; 
          }
          md=mn->next();
        } 
      }
      if (!found) // found a fresh variable
      {
        // new member variable, typedef or enum value
        MemberDef *md=new MemberDef(root->type,name,root->args,0,
            root->protection,Normal,stat,FALSE,
            mtype,0,0);
        md->setMemberClass(cd);
        md->setDefFile(root->fileName);
        md->setDefLine(root->startLine);
        md->setDocumentation(root->doc);
        md->setBriefDescription(root->brief);
        md->setDefinition(def);
        md->addSectionsToDefinition(root->anchors);

        // add the member to the global list
        if (mn)
        {
          //printf("Member already found! %s\n",md->name());
          //addMemberDocs(root,mn->first(),def,FALSE);
          //delete md;
          mn->inSort(md);
        }
        else // new variable name
        {
          mn = new MemberName(name);
          mn->inSort(md);
          //printf("Adding memberName=%s\n",mn->memberName());
          memberNameDict.insert(name,mn);
          memberNameList.inSort(mn);
          // add the member to the class
        }
        cd->insertMember(md);

        //TODO: insert FileDef instead of filename strings.
        cd->insertUsedFile(root->fileName);
      }
    }
    else if (name.length()>0) // global variable
    {
      Debug::print(Debug::Variables,0,
                   "  global variable:\n"
                   "    type=`%s' scope=`%s' name=`%s' args=`%s' prot=`%d\n",
                   root->type.data(),
                   scope.data(), 
                   name.data(),
                   root->args.data(),
                   root->protection
                  );
      
      // new global variable, enum value or typedef
      MemberDef *md=new MemberDef(root->type,name,root->args,0,
                           Public, Normal,root->stat,FALSE,
                           mtype,0,0);
      md->setDefFile(root->fileName);
      md->setDefLine(root->startLine);
      md->setDocumentation(root->doc);
      md->setBriefDescription(root->brief);
      md->addSectionsToDefinition(root->anchors);
      QCString def;

      // see if the function is inside a namespace
      NamespaceDef *nd = 0;
      if (scope.length()>0)
      {
        nd = namespaceDict[scope];
      }
      if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
      {
        nd->insertMember(md); 
        md->setNamespace(nd);
      }
      else
      {
        // find file definition
        FileDef *fd=0;
        bool ambig;
        if (root->fileName.length()>0 && 
            (fd=findFileDef(&inputNameDict,root->fileName,ambig))
           )
        {
          fd->insertMember(md);
          md->setFileDef(fd); 
        }
      }
      
      // determine the definition of the global variable
      if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@') 
         // variable is inside a namespace, so put the scope before the name
      {
        if (root->type.length()>0)
        {
          def=root->type+" "+nd->name()+"::"+name+root->args;
        }
        else
        {
          def=nd->name()+"::"+name+root->args;
        }
      }
      else
      {
        if (root->type.length()>0)
        {
          def=root->type+" "+name+root->args;
        }
        else
        {
          def=name+root->args;
        }
      }
      if (def.left(7)=="static ") def=def.right(def.length()-7);
      md->setDefinition(def);

      MemberName *mn;
      // add member definition to the list of globals 
      if ((mn=functionNameDict[name]))
      {
        mn->inSort(md);
      }
      else
      {
        mn = new MemberName(name);
        mn->inSort(md);
        functionNameDict.insert(name,mn);
        functionNameList.inSort(mn);
      }
    }
  }
nextMember:
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    if (e->section!=Entry::ENUM_SEC) buildVarList(e);
  }
}

//----------------------------------------------------------------------
// Searches the Entry tree for Function sections.
// If found they are stored in their class or in the global list.

void buildMemberList(Entry *root)
{
  if (root->section==Entry::FUNCTION_SEC)
  {
    Debug::print(Debug::Functions,0,
                 "FUNCTION_SEC:\n"
                 "  `%s' `%s'::`%s' `%s' relates=`%s' file=`%s' #targs=%d docs=`%s'\n",
                 root->type.data(),
                 root->parent->name.data(),
                 root->name.data(),
                 root->args.data(),
                 root->relates.data(),
                 root->fileName.data(),
                 root->tArgList ? (int)root->tArgList->count() : -1,
                 root->doc.data()
                );

    bool isFriend=root->type.find("friend ")!=-1;

    if (root->name.length()>0)
    {
      
      ClassDef *cd=0;
      // check if this function's parent is a class
      QRegExp re("([a-zA-Z0-9: ]*[ *]*[ ]*");
      //printf("root->parent=`%s' cd=%p root->type.find(re,0)=%d\n",
      //    root->parent->name.data(),getClass(root->parent->name),
      //    root->type.find(re,0));
      QCString scope=stripAnnonymousScope(root->parent->name.copy());
      int i;
      if (root->parent && 
          root->parent->name.length()>0 &&
          (root->parent->section & Entry::COMPOUND_MASK) && 
          (cd=getClass(scope)) &&
          // do some fuzzy things to exclude function pointers 
          (root->type.isNull() || root->type.find(re,0)==-1 || 
           root->type.find(")(")!=-1 || root->type.find("operator")!=-1
          )
         )
      {
        int l;
        if ((i=re.match(root->type,0,&l))!=-1) // function variable
        {
          root->args+=root->type.right(root->type.length()-i-l);
          root->type=root->type.left(i+l);
        }

        QCString name=removeRedundantWhiteSpace(root->name);
        if (name.left(2)=="::") name=name.right(name.length()-2);

        MemberDef::MemberType mtype;
        if (isFriend)        mtype=MemberDef::Friend;
        else if (root->sig)  mtype=MemberDef::Signal;
        else if (root->slot) mtype=MemberDef::Slot;
        else                 mtype=MemberDef::Function;

        //if (Config::includeSourceFlag && !root->body.isEmpty())
        //{
        //  printf("Function: %s\n-----------------\n%s\n------------------\n",
        //         root->name.data(),root->body.data());
        //}

        // new member function, signal or slot.
        //printf("new member: %s class template args=`%s'\n",
        //          root->args.data(),argListToString(cd->templateArguments()).data());
        MemberDef *md=new MemberDef(root->type,name,root->args,root->exception,
                             root->protection,root->virt,root->stat,root->relates.length()>0,
                             mtype,root->tArgList,root->argList);
        md->setMemberClass(cd);
        md->setDefFile(root->fileName);
        md->setDefLine(root->startLine);
        md->setDocumentation(root->doc);
        md->setBriefDescription(root->brief);
        md->setBody(root->body);
        //md->setScopeTemplateArguments(cd->templateArguments());
        md->addSectionsToDefinition(root->anchors);
        QCString def;
        if (root->relates.length()>0 || isFriend)
        {
          if (root->type.length()>0)
          {
            if (root->argList)
            {
              def=root->type+" "+name;
            }
            else
            {
              def=root->type+" "+name+root->args;
            }
          }
          else
          {
            if (root->argList)
            {
              def=name;
            }
            else
            {
              def=name+root->args;
            }
          }
        }
        else
        {
          if (root->type.length()>0)
          {
            if (root->argList)
            {
              def=root->type+" "+scope+"::"+name;
            }
            else
            {
              def=root->type+" "+scope+"::"+name+root->args;
            }
          }
          else
          {
            if (root->argList)
            {
              def=scope+"::"+name;
            }
            else
            {
              def=scope+"::"+name+root->args;
            }
          }
        }
        if (def.left(7)=="friend ") def=def.right(def.length()-7);
        md->setDefinition(def);

        Debug::print(Debug::Functions,0,
                     "  Func Member:\n"
                     "    `%s' `%s'::`%s' `%s' proto=%d\n"
                     "    def=`%s'\n",
                     root->type.data(),
                     root->parent->name.data(),
                     root->name.data(),
                     root->args.data(),
                     root->proto,
                     def.data()
                    );

        // add member to the global list of all members
        MemberName *mn;
        if ((mn=memberNameDict[name]))
        {
          mn->inSort(md);
        }
        else
        {
          mn = new MemberName(name);
          mn->inSort(md);
          //printf("Adding memberName=%s\n",mn->memberName());
          memberNameDict.insert(name,mn);
          memberNameList.inSort(mn);
        }

        // add member to the class cd
        cd->insertMember(md);
        // add file to list of used files
        cd->insertUsedFile(root->fileName);
      }
      else if (root->parent && 
               !(root->parent->section & Entry::COMPOUND_MASK) &&
               root->name.find("::")==-1 &&
               root->relates.length()==0 &&
               root->type.left(7)!="extern " &&
               root->type.left(8)!="typedef "
              )
      // no member => unrelated function 
      {
        /* check the uniqueness of the function name in the file.
         * A file could contain a function prototype and a function definition
         * or even multiple function prototypes.
         */
        bool found=FALSE;
        MemberName *mn;
        //MemberDef *fmd;
        if ((mn=functionNameDict[root->name]))
        {
          MemberDef *md=mn->first();
          while (md && !found)
          {
            if (md->getFileDef() && 
                md->getFileDef()->absFilePath()==root->fileName && 
                /*matchArguments(md->argsString(),root->args)*/
                matchArguments(md->argumentList(),root->argList)
               )
            {
              // function already found in the same file, one is probably
              // a prototype.
              found=TRUE;
              if (!md->documentation() && !root->doc.isEmpty())
              {
                md->setDocumentation(root->doc);
              }
              if (!md->briefDescription() && !root->brief.isEmpty())
              {
                md->setBriefDescription(root->brief);
              }
              md->addSectionsToDefinition(root->anchors);
            }
            md=mn->next();
          }
        }
        if (!found) /* global function is unique with respect to the file */
        {
          //printf("New function type=`%s' name=`%s' args=`%s'\n",
          //       root->type.data(),root->name.data(),root->args.data());
          
          // new global function
          QCString name=removeRedundantWhiteSpace(root->name);
          MemberDef *md=new MemberDef(root->type,name,root->args,root->exception,
              root->protection,root->virt,root->stat,FALSE,
              MemberDef::Function,root->tArgList,root->argList);
          md->setDefFile(root->fileName);
          md->setDefLine(root->startLine);
          md->setDocumentation(root->doc);
          md->setBriefDescription(root->brief);
          md->setPrototype(root->proto);
          md->setBody(root->body);
          md->addSectionsToDefinition(root->anchors);
          QCString def;
          if (root->type.length()>0)
          {
            if (root->argList)
            {
              def=root->type+" "+name;
            }
            else
            {
              def=root->type+" "+name+root->args;
            }
          }
          else
          {
            if (root->argList)
            {
              def=name.copy();
            }
            else
            {
              def=name+root->args;
            }
          }
          Debug::print(Debug::Functions,0,
                     "  Global Function:\n"
                     "    `%s' `%s'::`%s' `%s' proto=%d\n"
                     "    def=`%s'\n",
                     root->type.data(),
                     root->parent->name.data(),
                     root->name.data(),
                     root->args.data(),
                     root->proto,
                     def.data()
                    );
          md->setDefinition(def);

          // see if the function is inside a namespace
          NamespaceDef *nd = 0;
          if (root->parent->section == Entry::NAMESPACE_SEC )
          {
            nd = namespaceDict[root->parent->name];
          }

          if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
          {
            nd->insertMember(md); 
            md->setNamespace(nd);
          }
          else
          {
            // find file definition
            FileDef *fd=0;
            bool ambig;
            if (root->fileName.length()>0 && 
                (fd=findFileDef(&inputNameDict,root->fileName,ambig))
               )
            {
              // add member to the file
              fd->insertMember(md);
              md->setFileDef(fd); 
            }
          }

          // add member to the list of file members
          MemberName *mn;
          if ((mn=functionNameDict[name]))
          {
            mn->inSort(md);
          }
          else 
          {
            mn = new MemberName(name);
            mn->inSort(md);
            functionNameDict.insert(name,mn);
            functionNameList.inSort(mn);
          }
        }
        else
        {
          //printf("Function already found!\n");
        }

        //printf("unrelated function %d `%s' `%s' `%s'\n",
        //    root->parent->section,root->type.data(),root->name.data(),root->args.data());
      }
    }
    else if (root->name.length()==0)
    {
        warn("Warning: Illegal member name found in file %s at line %d\n",
               root->fileName.data(),root->startLine);
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildMemberList(e);
  }
}

//----------------------------------------------------------------------

void findFriends()
{
  //printf("findFriends()\n");
  MemberNameListIterator fnli(functionNameList);
  MemberName *fn;
  for (;(fn=fnli.current());++fnli) // for each global function name
  {
    //printf("Function name=`%s'\n",fn->memberName());
    MemberName *mn;
    if ((mn=memberNameDict[fn->memberName()]))
    { // there are members with the same name
      //printf("Function name is also a member name\n");
      MemberNameIterator fni(*fn);
      MemberDef *fmd;
      for (;(fmd=fni.current());++fni) // for each function with that name
      {
        MemberNameIterator mni(*mn);
        MemberDef *mmd;
        for (;(mmd=mni.current());++mni) // for each member with that name
        {
          //printf("Checking for matching arguments 
          //        mmd->isRelated()=%d mmd->isFriend()=%d mmd->isFunction()=%d\n",
          //    mmd->isRelated(),mmd->isFriend(),mmd->isFunction());
          NamespaceDef *nd=mmd->getNamespace();
          if ((mmd->isFriend() || (mmd->isRelated() && mmd->isFunction())) &&
              matchArguments(mmd->argumentList(),
                             fmd->argumentList(),
                             mmd->memberClass()->name(),
                             nd ? nd->name().data() : 0
                            )
             ) // if the member is related and the arguments match then the 
               // function is actually a friend.
          {
            //printf("Found friend function\n");
            mergeArguments(mmd->argumentList(),fmd->argumentList());
            if (fmd->documentation())
              mmd->setDocumentation(fmd->documentation());
            else if (mmd->documentation())
              fmd->setDocumentation(mmd->documentation());
            if (!mmd->briefDescription() && fmd->briefDescription())
              mmd->setBriefDescription(fmd->briefDescription());
            else if (mmd->briefDescription() && !fmd->briefDescription())
              fmd->setBriefDescription(mmd->briefDescription());
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

void transferFunctionDocumentation()
{
  //printf("transferFunctionDocumentation()\n");
  MemberNameListIterator mnli(functionNameList);
  MemberName *mn;
  for (;(mn=mnli.current());++mnli)
  {
    MemberDef *md,*mdef=0,*mdec=0;
    MemberNameIterator mni(*mn);
    /* find a matching function declaration and definition for this function */
    for (;(md=mni.current());++mni)
    {
      if (md->isPrototype()) mdec=md;
      if (md->isFunction() && !md->isStatic() && !md->isPrototype()) mdef=md;
    }
    if (mdef && mdec && 
        matchArguments(mdef->argumentList(),mdec->argumentList())
       ) /* match found */
    {
      //printf("Found member %s: def in %s and dec in %s\n",
      //    mn->memberName(),mdef->getFileDef()->name(),mdec->getFileDef()->name());
      
      /* copy documentation between function definition and declaration */
      if (mdec->briefDescription())
      {
        mdef->setBriefDescription(mdec->briefDescription());
      }
      else if (mdef->briefDescription())
      {
        mdec->setBriefDescription(mdef->briefDescription());
      }
      if (mdef->documentation())
      {
        mdec->setDocumentation(mdef->documentation());
      }
      else if (mdec->documentation())
      {
        mdef->setDocumentation(mdec->documentation());
      }
    }
  }
}


//----------------------------------------------------------------------
// Computes the base and super classes for each class in the tree

void computeClassRelations(Entry *root)
{
  if (
      (
       (
        root->section & Entry::COMPOUND_MASK
       ) 
       ||
       (
        (root->section & Entry::COMPOUNDDOC_MASK) && root->extends->count()>0 
       )
      )
      &&
       root->name.length()>0
     )
  {
    ClassDef *cd;
    if ((cd=getClass(root->name)))
    {
      //printf("Class %s %d\n",cd->name().data(),root->extends->count());
      if (!cd->visited)
      {
        cd->visited=TRUE; // mark class as used (in case the are multiple classes
                          // with the same name!)
        if (root->extends->count()>0) // there are base classes
        {
          QCString scopePrefix;
          Entry *p=root->parent;
          bool found=FALSE;
          // For nested classes the base class could also be nested!
          // To find the correct scope, we try to prepend the scope to the base
          // name, starting with the largest, most inner scope.
          while (p->section&Entry::COMPOUND_MASK && !found)
          {
            scopePrefix=p->name+"::";
            QList<BaseInfo> *baseList=root->extends;
            BaseInfo *bi=baseList->first();
            while (bi && !found) // for each base class
            {
              QCString cName=scopePrefix+bi->name;
              //printf("Base class %s\n",cName.data());
              ClassDef *baseClass=getClass(cName);
              if (baseClass) // base class is documented 
              {
                //printf("Adding!\n");
                // add base class to this class
                cd->insertBaseClass(baseClass,bi->prot,bi->virt);
                // add this class as super class to the base class
                baseClass->insertSuperClass(cd,bi->prot,bi->virt);
                found=TRUE;
              }
              //else // base class not documented
              //{
              //  warn("Warning: Base class %s referred to in file %s at line %d is not "
              //      "documented\n",s->data(),root->fileName.data(),root->startLine);
              //}
              bi=baseList->next();
            }
            p=p->parent; 
          }
          if (!found)
          {
            // The base class could ofcouse also be a non-nested class
            QList<BaseInfo> *baseList=root->extends;
            BaseInfo *bi=baseList->first();
            while (bi) // for each base class
            {
              ClassDef *baseClass=getClass(bi->name);
              //printf("baseClass %s of %s found (%s and %s)\n",
              //      bi->name.data(),
              //      root->name.data(),
              //      (bi->prot==Private)?"private":((bi->prot==Protected)?"protected":"public"),
              //      (bi->virt==Normal)?"normal":"virtual"
              //      );
              int i;
              QCString templSpec,baseClassName=bi->name.copy();
              if (!baseClass && (i=bi->name.find('<'))!=-1) 
                // base class has template specifiers
              {
                // TODO: here we should try to find the correct template specialization
                // but for now, we only look for the unspecializated base class.
                baseClassName=bi->name.left(i);
                baseClass=getClass(baseClassName);
                templSpec=bi->name.right(bi->name.length()-i);
              }
              if (baseClass) // base class is documented 
              {
                // add base class to this class
                cd->insertBaseClass(baseClass,bi->prot,bi->virt,templSpec);
                // add this class as super class to the base class
                baseClass->insertSuperClass(cd,bi->prot,bi->virt,templSpec);
              }
              else // base class not documented
              {
                NamespaceDef *nd=cd->getNamespace();
                //printf("Found undocumented base class `%s' namespace scope=`%s'\n",
                //    bi->name.data(),nd ? nd->name().data() : "<none>");
                if (nd && (baseClass=getClass(nd->name()+"::"+baseClassName))) 
                  // class is defined inside namespace
                {
                  // add base class to this class
                  cd->insertBaseClass(baseClass,bi->prot,bi->virt,templSpec);
                  // add this class as super class to the base class
                  baseClass->insertSuperClass(cd,bi->prot,bi->virt,templSpec);
                }
                else // undocumented base class
                {
                  //printf(">>> Undocumented base class = %s\n",bi->name.data());
                  baseClass=new ClassDef(baseClassName,ClassDef::Class);
                  // add base class to this class
                  cd->insertBaseClass(baseClass,bi->prot,bi->virt,templSpec);
                  // add this class as super class to the base class
                  baseClass->insertSuperClass(cd,bi->prot,bi->virt,templSpec);
                  // the undocumented base was found in this file
                  baseClass->insertUsedFile(root->fileName);
                  // add class to the list
                  classList.inSort(baseClass);
                  //printf("ClassDict.insert(%s)\n",resolveDefines(fullName).data());
                  //classDict.insert(resolveDefines(bi->name),baseClass);
                  classDict.insert(bi->name,baseClass);
                }
              }
              bi=baseList->next();
            }
          }
        }
//       else // class has no base classes
//        {
//          QCString resName=resolveDefines(root->name);
//          int i;
//          // Check if this class is a template instance of another class.
//          // If this is the case, we act as if this class `inherits' from the
//          // template class.
//          if ((i=resName.find('<'))!=-1) 
//          {
//            ClassDef *baseClass=getClass(resName.left(i));
//            if (baseClass)
//            {
//              // add base class to this class
//              cd->insertBaseClass(baseClass,Public,Normal);
//              // add this class as super class to the base class
//              baseClass->insertSuperClass(cd,Public,Normal);
//            }
//          }
//        }
      } // else class is already found
    }
    else if (root->name.right(2)!="::")
    {
      if (root->name.length()>0 && root->name[0]!='@')
        warn("Warning: Compound %s\n"
             "         defined in file %s at line %d\n"
             "         is not documented\n",
              root->name.data(),root->fileName.data(),root->startLine);
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    computeClassRelations(e);
  }
}

//-----------------------------------------------------------------------
// compute the references (anchors in HTML) for each member in the class

void computeMemberReferences()
{
  ClassDef *cd=classList.first();
  while (cd)
  {
    cd->computeAnchors();
    cd=classList.next();
  } 
}

//-----------------------------------------------------------------------
// compute the references (anchors in HTML) for each function in the file

void computeFunctionReferences()
{
  FileName *fn=inputNameList.first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->computeAnchors();
      fd=fn->next();
    }
    fn=inputNameList.next();
  }
  NamespaceDef *nd=namespaceList.first();
  while (nd)
  {
    nd->computeAnchors();
    nd=namespaceList.next();
  }
}



//----------------------------------------------------------------------
// Copy the documentation in entry `root' to member definition `md' and
// set the function declaration of the member to `funcDecl'. If the boolean 
// over_load is set the standard overload text is added. 

void addMemberDocs(Entry *root,MemberDef *md, const char *funcDecl,
                        bool over_load)
{
  //printf("addMemberDocs: `%s'::`%s' `%s' funcDecl=`%s'\n",
  //     root->parent->name.data(),md->name().data(),md->argsString(),funcDecl);
  QCString fDecl=funcDecl;
  // strip extern specifier
  if (fDecl.left(7)=="extern ") fDecl=fDecl.right(fDecl.length()-7);
  md->setDefinition(fDecl);
  ClassDef     *cd=md->memberClass();
  NamespaceDef *nd=md->getNamespace();
  if (matchArguments(md->argumentList(),root->argList,
        cd ? cd->name().data() : 0,
        nd ? nd->name().data() : 0
        )
     ) 
  {
    mergeArguments(md->argumentList(),root->argList);
  }
  if (over_load)  // the \overload keyword was used
  {
    QCString doc=getOverloadDocs();
    if (!root->doc.isNull())
    {
      doc+="<p>";
      doc+=root->doc;
    }
    md->setDocumentation(doc); 
  }
  else  
  {
    // documentation outside a compound overrides the documentation inside it
    if ( /* !md->isStatic() && !root->stat &&   do not replace doc of a static */
        (
         md->documentation().isEmpty() ||    /* no docs yet */
         (root->parent->name.isNull() &&     /* or overwrite prototype docs */
          !root->proto && md->isPrototype()  /* with member definition docs */
         )
        ) && root->doc.length()>0 
       )
    {
      md->setDocumentation(root->doc);
    }

    // brief descriptions inside a compound override the documentation 
    // outside it
    if ( /* !md->isStatic() && !root->stat &&  do not replace doc of static */
        ( 
         md->briefDescription().isEmpty() ||  /* no docs yet */
         !root->parent->name.isNull()         /* member of a class */
        ) && root->brief.length()>0
       )
    {
      md->setBriefDescription(root->brief);
    }
    
    if (md->bodyCode().isEmpty() && !root->body.isEmpty()) /* no body yet */
    {
      md->setBody(root->body);
    }
  }
  md->setDefFile(root->fileName);
  md->setDefLine(root->startLine);
  md->addSectionsToDefinition(root->anchors);
  if (cd) cd->insertUsedFile(root->fileName);
}

//----------------------------------------------------------------------
// Adds the documentation contained in `root' to a global function
// with name `name' and argument list `args' (for overloading) and
// function declaration `decl' to the corresponding member definition.

bool findUnrelatedFunction(Entry *root, 
                           const QCString &namespaceName,
                           const char *name, 
                           const char *, 
                           const char *decl)
{
  MemberName *mn=0;
  QCString n=name;
  if (n.find("::")!=-1) return FALSE; // skip undefined class members
  //printf("findUnrelatedFunction(%s)\n",name);
  if (n.length()>0 && (mn=functionNameDict[n])) // function name defined
  {
    int count=0;
    MemberDef *md=mn->first();
    while (md)
    {
      NamespaceDef *nd=md->getNamespace();
      QCString nsName = nd ? nd->name().data() : "";
      if (namespaceName.length()==0 || 
          nsName==namespaceName)
      {
        //printf("Searching for match between %s and %s\n",
        //    argListToString(md->argumentList()).data(),
        //    argListToString(root->argList).data());
        bool matching=
          /*matchArguments(md->argsString(),args);*/
          (md->argumentList()==0 && root->argList->count()==0) ||
          matchArguments(md->argumentList(),root->argList,0,nsName);
        if (matching) // add docs to the member
        {
          //printf("Match found\n");
          addMemberDocs(root,md,decl,FALSE);
          count++;
        }
      }
      md=mn->next();
    } 
    if (count==0) // more than one match (each member will get the same docs)!
    {
      warn("Warning: no matching members found for \n%s\n"
           "in file %s at line %d\n",
           decl,root->fileName.data(),root->startLine);   
    }
#if 0
    else if (count>1) // no match!
    {
      warn("Warning: multiple matching members for\n%s\n",decl);
      if (mn->count()>0) // there is a member with that name
      {
        warn("Possible candidates are:\n");
        MemberDef *md=mn->first();
        while (md) // list all possible members with the same name
        {
          warn("  %s%s\n",md->name(),md->argsString());
          md=mn->next();
        }
      }
    }
#endif
  }
  else // got docs for an undefined member!
  {
    warn("Warning: documented function `%s' in file %s at line %d "
         "was not defined \n",decl,
         root->fileName.data(),root->startLine);
  }
  return TRUE;
}

void substituteTemplateArgNames(ArgumentList *src,
                  const QCString &s,
                  ArgumentList *tempArgs,
                  ArgumentList *dst)
{
  ArgumentListIterator ali(*src);
  Argument *a=0;
  for (ali.toFirst();(a=ali.current());++ali) // for each member argument
  {
    QCString type=a->type.copy();
    bool isReplaced=FALSE;
    QRegExp re(idMask);
    int i,p=0,l,c=0;
    while ((i=re.match(s,p,&l))!=-1) // for each template name found at the
                                     // member definition
    {
      Argument *ta = tempArgs->at(c);
      if (ta) // get matching template argument of the class
      {
        QCString dstName=s.mid(i,l);
        QCString srcName=ta->type.copy();
        int bi;
        if ((bi=srcName.findRev(' '))!=-1) // search for separator
        {
          // strip the type specifier (usuall class or typename)
          srcName=srcName.right(srcName.length()-bi-1);
        }

        //if (srcName.left(6)=="class ")    srcName=srcName.right(srcName.length()-6);
        //if (srcName.left(9)=="typename ") srcName=srcName.right(srcName.length()-9);
        //printf("Template Name = `%s' -> `%s'\n",srcName.data(),dstName.data());
        if (srcName!=dstName) /* we need to substitute */
        {
          int ti,tp=0;
          QCString result;
          int sl=srcName.length();
          while ((ti=type.find(srcName,tp))!=-1)
          {
            result+=type.mid(tp,ti-tp);
            if (
                (ti==0 || !isId(type.at(ti-1))) && 
                (ti+sl==(int)type.length() || !isId(type.at(ti+sl)))
               ) /* idenitifier -> replace */
            {
              result+=dstName;
            }
            else /* substring of an identifier */
            {
              result+=srcName;
            }
            tp=ti+sl;
          }
          result+=type.right(type.length()-tp);
          type=result;
          isReplaced = TRUE;
        }
      }
      p=i+l;
      c++;
    }
    Argument *na = new Argument(*a);
    if (isReplaced)
    {
      //printf("Template Arg: `%s' -> `%s'\n",na->type.data(),type.data());
      na->type=type;
    }
    else
    {
      //printf("Template Arg `%s' not changed\n",a->type.data());
    }
    dst->append(na);
  }
  //printf("substituteTemplateArgNames(src=`%s',tempNameStr=`%s',tempArgs=`%s',dest=`%s')\n",
  //    argListToString(src).data(),
  //    s.data(),
  //    argListToString(tempArgs).data(),
  //    argListToString(dst).data()
  //   );
  dst->constSpecifier    = src->constSpecifier;
  dst->volatileSpecifier = src->volatileSpecifier;
  dst->pureSpecifier     = src->pureSpecifier;

}

//----------------------------------------------------------------------
// This function tries to find a member (in a documented class/file/namespace) 
// that corresponds to the function declaration given in `funcDecl'.
//
// The related field may be used to specify a related class name.
// It is only used if the class name cannot be extracted from the function
// declaration.
//
// The boolean overloaded is used to specify whether or not a standard
// overload documentation line should be generated.

void findMember(Entry *root,QCString funcDecl,QCString related,bool overloaded,
                bool isFunc)
{
  Debug::print(Debug::FindMembers,0,
               "findMember(root=%p,funcDecl=`%s',related=`%s',overload=%d,isFunc=%d\n====\ndoc=%s\n====\n)\n",
               root,funcDecl.data(),related.data(),overloaded,isFunc,root->doc.data()
              );
  if (Config::includeSourceFlag && !root->body.isEmpty())
  {
    //printf("Function: %s\n-----------------\n%s\n------------------\n",
    //root->name.data(),root->body.data());
  }

  QCString scopeName;
  QCString className;
  QCString namespaceName;
  QCString classTempList;
  QCString funcType;
  QCString funcName;
  QCString funcArgs;
  QCString funcTempList;
  QCString exceptions;
  bool isRelated=FALSE;
  bool isFriend=FALSE;
  
  if (funcDecl.left(7)=="friend ") // treat friends as related members
  {
    funcDecl=funcDecl.right(funcDecl.length()-7);
    isFriend=TRUE;
  }
  
  // delete any ; from the function declaration
  int sep;
  while ((sep=funcDecl.find(';'))!=-1)
  {
    funcDecl=(funcDecl.left(sep)+funcDecl.right(funcDecl.length()-sep-1)).stripWhiteSpace();
  }
  
  // make sure the first character is a space to simplify searching.
  if (funcDecl.length()>0 && funcDecl[0]!=' ') funcDecl.prepend(" ");
  
  // remove some superfluous spaces
  funcDecl=substitute(
             substitute(
               substitute(funcDecl,"~ ","~"),
               ":: ","::"
             ),
             " ::","::"
           );
  
  // extract information from the declarations
  parseFuncDecl(funcDecl,scopeName,classTempList,funcType,funcName,
                funcArgs,funcTempList,exceptions
               );
  
  // the class name can also be a namespace name, we decide this later.
  // if a related class name is specified and the class name could
  // not be derived from the function declaration, then use the
  // related field.
  //printf("scopeName=`%s' classTempList=`%s' className=`%s'\n",
  //    scopeName.data(),classTempList.data(),className.data());
  if (/*scopeName.isEmpty() &&*/ !related.isEmpty() && !isRelated)
  {
    isRelated=TRUE;
    //scopeName=resolveDefines(related);
    if (!scopeName.isEmpty() && scopeName!=related)
      scopeName+="::"+related;
    else 
      scopeName=related.copy();
  }
  else if (/*scopeName.isEmpty() &&*/ related.isEmpty() && root->parent && 
           !root->parent->name.isNull())
  {
    Entry *p=root->parent;
    while (p) // get full scope as class name
    {
      //printf("++++++ scope=`%s'\n",p->name.data());
      if (((p->section & Entry::COMPOUND_MASK) || 
           p->section == Entry::NAMESPACE_SEC
          ) && !p->name.isEmpty() && p->name[0]!='@'
         )
      {
        if (scopeName.left(p->name.length())==p->name) 
          break; // scope already present, so stop now
        // prepend name to scope
        if (!scopeName.isEmpty()) scopeName.prepend("::");
        scopeName.prepend(p->name);
      }
      p=p->parent;
    } 
  }

  if (scopeName.length()>0 && scopeName.find('<')==-1 && classTempList.length()==0 )
  { // class is a template, but no template name list found
    ClassDef *cd=getClass(scopeName); 
    if (cd) // class exists
    {
      classTempList = cd->getTemplateNameString();
    }
  }
  
  // see if (part of) the scope name is a namespace name
  extractNamespaceName(scopeName,className,namespaceName);

  QCString tempScopeName=scopeName.copy();
  int ti;
  int spi = namespaceName.isEmpty() ? 0 : namespaceName.length()+2;
  if ((ti=tempScopeName.find("::",spi))!=-1 && !classTempList.isEmpty())
  {
    // insert template parameters after the first scope name
    tempScopeName=tempScopeName.left(ti)+classTempList+
                  tempScopeName.right(tempScopeName.length()-ti); 
  }
  else
  {
    tempScopeName+=classTempList;
  }
  

  if (root->tArgList==0 && !classTempList.isEmpty())
  {
    // no template specifiers found during parsing (because \fn was used), 
    // but there are template names in the scope, so we build the template 
    // specifiers from that.
    root->tArgList = new ArgumentList;
    QRegExp re(idMask);
    int i,p=0,l;
    while ((i=re.match(classTempList,p,&l))!=-1) // for each template name found 
    {
      Argument *a = new Argument;
      a->type = "class "+classTempList.mid(i,l);
      root->tArgList->append(a);
      p=i+l;
    }
  }
  
  //printf("scopeName=`%s' className=`%s'\n",scopeName.data(),className.data());
  // rebuild the function declaration (needed to get the scope right).
  if (scopeName.length()>0 && !isRelated && !isFriend)
  {
    if (funcType.length()>0)
    {
      if (isFunc) // a function -> we use argList for the arguments
      {
        funcDecl=funcType+" "+tempScopeName+"::"+funcName+funcTempList;
      }
      else
      {
        funcDecl=funcType+" "+tempScopeName+"::"+funcName+funcArgs;
      }
    }
    else
    {
      if (isFunc) // a function => we use argList for the arguments
      {
        funcDecl=tempScopeName+"::"+funcName+funcTempList;
      }
      else // variable => add `argument' list
      {
        funcDecl=tempScopeName+"::"+funcName+funcArgs;
      }
    }
  }
  else // build declaration without scope
  {
    if (funcType.length()>0) // but with a type
    {
      if (isFunc) // function => omit argument list
      {
        funcDecl=funcType+" "+funcName+funcTempList;
      }
      else // variable => add `argument' list
      {
        funcDecl=funcType+" "+funcName+funcArgs;
      }
    }
    else // no type
    {
      if (isFunc)
      {
        funcDecl=funcName+funcTempList;
      }
      else
      {
        funcDecl=funcName+funcArgs;
      }
    }
  }
  
  QCString fullFuncDecl=funcDecl.copy();
  if (isFunc) fullFuncDecl+=argListToString(root->argList);
  
  //printf("scopeName=`%s' className=`%s'\n",scopeName.data(),className.data());
  //printf("scopeName=`%s' className=`%s'\n",scopeName.data(),className.data());

  // destructor => do backward class name substitution if needed
  //if (funcName.length()>0 && funcName[0]=='~') 
  //  funcName="~"+resolveDefines(className);
  // constructor => do backward class name substitution if needed
  //if (funcName==className) funcName=resolveDefines(className);
  //if (funcDecl.left(7)=="inline ") funcDecl=funcDecl.right(funcDecl.length()-7);
  
  Debug::print(Debug::FindMembers,0,
           "findMember() Parse results:\n"
           "  namespaceName=`%s'\n"
           "  className=`%s`\n"
           "  classTempList=`%s'\n"
           "  funcType=`%s'\n"
           "  funcName=`%s'\n"
           "  funcArgs=`%s'\n"
           "  funcTempList=`%s'\n"
           "  funcDecl=`%s'\n"
           "  related=`%s'\n" 
           "  exceptions=`%s'\n"
           "  isRelated=%d\n"
           "  isFiend=%d\n"
           "  isFunc=%d\n\n",
           namespaceName.data(),className.data(),classTempList.data(),
           funcType.data(),funcName.data(),funcArgs.data(),funcTempList.data(),
           funcDecl.data(),related.data(),exceptions.data(),isRelated,isFriend,
           isFunc
          );

  MemberName *mn;
  if (funcName.length()>0) // function name is valid
  { 
    if (!isRelated && (mn=memberNameDict[funcName])) // function name already found
    {
      if (className.length()>0) // class name is valid
      {
        int count=0;
        MemberDef *md=mn->first(); // for each member with that name
        while (md)
        {
          ClassDef *cd=md->memberClass();
          //printf("Member %s (member scopeName=%s) (this scopeName=%s) classTempList=%s\n",md->name().data(),cd->name().data(),scopeName.data(),classTempList.data());
          ClassDef *tcd=0;
          if (classTempList.length()>0) // try to find the correct specialization
          {
            tcd=getClass(scopeName+classTempList);
            if (!tcd) tcd=getClass(scopeName); // try general class
          }
          else
          {
            tcd=getClass(scopeName);
          }
          if (cd && tcd==cd) // member's classes match
          {
            int ci;
            ArgumentList *classTemplArgs = cd->templateArguments();
            if ((ci=cd->name().find("::"))!=-1) // nested class
            {
              ClassDef *parentClass = getClass(cd->name().left(ci));
              if (parentClass) 
                classTemplArgs = parentClass->templateArguments();
            }
            //printf("cd->name=%s classTemplArgs=%s\n",cd->name().data(),
            //     argListToString(classTemplArgs).data());
            ArgumentList *argList = 0;
            bool substDone=FALSE;
            if (!classTempList.isEmpty() &&
                classTemplArgs &&
                md->argumentList()
               )
            {
              /* the function definition has template arguments
               * and the class also has template arguments, so
               * we must substitute the template names if they are
               * different before doing the match
               */
              argList = new ArgumentList;
              argList->setAutoDelete(TRUE);
              substituteTemplateArgNames(
                  md->argumentList(),      /* source argument list  */
                  classTempList,           /* template names source */
                  classTemplArgs,          /* template names dest   */  
                  argList                  /* dest argument list    */
                 );
              substDone=TRUE;
            }
            else /* no template arguments, compare argument lists directly */
            {
              argList = md->argumentList();
            }
            bool matching=
              md->isVariable() || md->isTypedef() || // needed for function pointers
              (md->argumentList()==0 && root->argList->count()==0) || 
              matchArguments(argList, root->argList,className,namespaceName);

            if (substDone) // found a new argument list
            {
              //printf("root->tArgList=`%s'\n",argListToString(root->tArgList).data());
              if (matching) // replace member's argument list
              {
                md->setScopeTemplateArguments(root->tArgList);
                md->setArgumentList(argList);
              }
              else // no match -> delete argument list
                delete argList;
            }
            if (matching) 
            {
              addMemberDocs(root,md,funcDecl,overloaded);
              count++;
            }
          } 
          md=mn->next();
        } 
        if (count==0) 
          warn("Warning: no matching member found for \n%s\n"
                 "in file %s at line %d\n",
                 fullFuncDecl.data(),root->fileName.data(),root->startLine);   
#if 0
        else if (count>1)
        {
          warn("Warning: multiple matching members for\n%s\n",funcDecl.data());
          int matchCount=0;
          MemberDef *md=mn->first(); 
          while (md)
          {
            if (md->memberClass() && className==md->memberClass()->name()) 
              matchCount++;
            md=mn->next();
          } 
          if (matchCount>0)
          {
            warn("Possible candidates are:\n");
            md=mn->first();
            while (md)
            {
              ClassDef *cd=md->memberClass();
              if (cd && className==cd->name())
                warn("  %s::%s%s\n",md->memberClass()->name(),
                    md->name(),md->argsString());
              md=mn->next();
            }
          }
        }
#endif
      }
      else if (overloaded) // check if the function belongs to only one class 
      {
        // for unique overloaded member we allow the class to be
        // omitted, this is to be Qt compatable. Using this should 
        // however be avoided, because it is error prone
        MemberDef *md=mn->first();
        ASSERT(md);
        ClassDef *cd=md->memberClass();
        ASSERT(cd);
        QCString className=cd->name().copy();
        md=mn->next();
        bool unique=TRUE;
        while (md)
        {
          ClassDef *cd=md->memberClass();
          if (className!=cd->name()) unique=FALSE; 
          md=mn->next();
        } 
        if (unique)
        {
          MemberDef::MemberType mtype;
          if      (root->sig)  mtype=MemberDef::Signal;
          else if (root->slot) mtype=MemberDef::Slot;
          else                 mtype=MemberDef::Function;
          
          // new overloaded member function
          MemberDef *md=new MemberDef(funcType,funcName,funcArgs,exceptions,
                            root->protection,root->virt,root->stat,TRUE,
                            mtype,root->tArgList,root->argList);
          md->setMemberClass(cd);
          md->setDefinition(funcDecl);
          QCString doc=getOverloadDocs();
          doc+="<p>";
          doc+=root->doc;
          md->setDocumentation(doc);
          //md->setDecFile(root->fileName);
          //md->setDecLine(root->startLine);
          md->setDefFile(root->fileName);
          md->setDefLine(root->startLine);
          md->setPrototype(root->proto);
          md->addSectionsToDefinition(root->anchors);
          md->setBody(root->body);
          mn->inSort(md);
          cd->insertMember(md);
          cd->insertUsedFile(root->fileName);
        }
      }
      else // unrelated function with the same name as a member
      {
        if (!findUnrelatedFunction(root,namespaceName,funcName+funcTempList,funcArgs,funcDecl))
        {
          warn("Warning: Cannot determine class for function\n%s\n"
               "in file %s at line %d\n",fullFuncDecl.data(),
               root->fileName.data(),root->startLine);   
        }
      }
    }
    else if (isRelated && related.length()>0)
    {
      if (className.isEmpty()) className=related.copy();
      ClassDef *cd;
      if ((cd=getClass(scopeName)))
      {
        bool newMember=TRUE; // assume we have a new member
        bool newMemberName=FALSE; 
        if ((mn=memberNameDict[funcName])==0)
        {
          mn=new MemberName(funcName);
          newMemberName=TRUE; // we create a new member name
        }
        else
        {
          MemberDef *rmd=mn->first();
          while (rmd && newMember) // see if we got another member with matching arguments
          {
            newMember=newMember && 
              !matchArguments(rmd->argumentList(),root->argList,className,namespaceName);
            if (newMember) rmd=mn->next();
          }
          if (!newMember && rmd) // member already exists as rmd -> add docs
          {
            addMemberDocs(root,rmd,funcDecl,overloaded);
          }
        }
        if (newMember) // need to create a new member
        {
          MemberDef::MemberType mtype;
          if (root->sig)  
            mtype=MemberDef::Signal;
          else if (root->slot) 
            mtype=MemberDef::Slot;
          else                 
            mtype=MemberDef::Function;

          // new related (member) function
          MemberDef *md=new MemberDef(funcType,funcName,funcArgs,exceptions,
              root->protection,root->virt,root->stat,TRUE,
              mtype,root->tArgList,root->argList);
          //printf("Related member name=`%s' decl=`%s'\n",funcName.data(),funcDecl.data());
          md->setMemberClass(cd);
          md->setDefinition(funcDecl);
          md->setDocumentation(root->doc);
          md->setBriefDescription(root->brief);
          md->setDefFile(root->fileName);
          md->setDefLine(root->startLine);
          md->setPrototype(root->proto);
          md->setBody(root->body);
          md->addSectionsToDefinition(root->anchors);
          mn->inSort(md);
          cd->insertMember(md);
          cd->insertUsedFile(root->fileName);
          if (newMemberName)
          {
            //printf("Adding memberName=%s\n",mn->memberName());
            memberNameList.inSort(mn);
            memberNameDict.insert(funcName,mn);
          }
        }
      }
      else
      {
        warn("Warning: class `%s' for related function `%s' is not "
            "documented\n", className.data(),funcName.data());
      }
    }
    else // unrelated not overloaded member found
    {
      if (className.length()==0 && 
          !findUnrelatedFunction(root,namespaceName,funcName+funcTempList,funcArgs,funcDecl))
      {
        warn("Warning: class for member %s (file %s at line %d) cannot "
             "be found\n", funcName.data(),root->fileName.data(),
             root->startLine); 
      }
    }
  }
  else
  {
    // this should not be called
    warn("Warning: member with no name found in %s at line %d\n",
              root->fileName.data(),root->startLine);
  }
  return;
} 

//----------------------------------------------------------------------
// find the members corresponding to the different documentation blocks
// that are extracted from the sources.

void findMemberDocumentation(Entry *root)
{
  int i,l;
  QRegExp re("([a-zA-Z0-9: ]*\\*+[ \\*]*");
  Debug::print(Debug::FindMembers,0,
         "root->type=`%s' root->name=`%s' root->args=`%s' section=%x\n",
          root->type.data(),root->name.data(),root->args.data(),root->section
         );
  bool isFunc=TRUE;
  if ((i=re.match(root->type,0,&l))!=-1) // func variable/typedef to func ptr
  {
    root->args+=root->type.right(root->type.length()-i-l);
    root->type=root->type.left(i+l);
    isFunc=FALSE;
  }
  else if (root->name.find(re)!=-1 && root->name.find("operator")==-1) 
    // func ptr entered with \fn, \var or \typedef
  {
    isFunc=FALSE;
  }
  else if ((root->type.isEmpty() && root->name.left(8)=="typedef ")
           || root->args.find('(')==-1)
  {
    isFunc=FALSE;
  }
  //printf("Member %s isFunc=%d\n",root->name.data(),isFunc);
  if (root->section==Entry::MEMBERDOC_SEC)
  {
    //printf("Documentation for inline member `%s' found args=`%s'\n",
    //    root->name.data(),root->args.data());
    //if (root->relates.length()) printf("  Relates %s\n",root->relates.data());
    findMember(root,root->name+root->args+root->exception,root->relates,FALSE,isFunc);
  }
  else if (root->section==Entry::OVERLOADDOC_SEC) 
  {
    //printf("Overloaded member %s found\n",root->name.data());
    findMember(root,root->name,root->relates,TRUE,isFunc);
  }
  else if 
    (root->section==Entry::FUNCTION_SEC && 
      (!root->doc.isEmpty() || !root->brief.isEmpty() || 
       !root->body.isEmpty() /*|| Config::extractAllFlag*/
      )
    )
  {
    //printf("Documentation for member `%s' found args=`%s' excp=`%s'\n",
    //    root->name.data(),root->args.data(),root->exception.data());
    //if (root->relates.length()) printf("  Relates %s\n",root->relates.data());
    //printf("Inside=%s\n Relates=%s\n",root->inside.data(),root->relates.data());
    if (root->type.length()>0)
    {
      findMember(root,
          root->type+" "+root->inside+root->name+root->args+root->exception,
          root->relates,
          FALSE,isFunc);
    }
    else
    {
      findMember(root,
          root->inside+root->name+root->args+root->exception,
          root->relates,
          FALSE,isFunc);
    }
  }
  else if (root->section==Entry::VARIABLEDOC_SEC)
  {
    //printf("Documentation for variable %s found\n",root->name.data());
    //if (root->relates.length()>0) printf("  Relates %s\n",root->relates.data());
    findMember(root,root->name,root->relates,FALSE,FALSE);
  }
  else
  {
    // skip section 
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    if (e->section!=Entry::ENUM_SEC) findMemberDocumentation(e);
  }
}

//----------------------------------------------------------------------
// find and add the enumeration to their classes, namespaces or files

void findEnums(Entry *root)
{
  if (root->section==Entry::ENUM_SEC)
    // non anonymous enumeration
  {
    MemberDef      *md=0;
    ClassDef       *cd=0;
    FileDef        *fd=0;
    NamespaceDef   *nd=0;
    MemberNameDict *mnd=0;
    MemberNameList *mnl=0;
    bool isGlobal;
    //printf("Found enum with name `%s'\n",root->name.data());
    int i;

    QCString name;
    if ((i=root->name.findRev("::"))!=-1) // scope is specified
    {
      QCString scope=root->name.left(i); // extract scope
      name=root->name.right(root->name.length()-i-2); // extract name
      if ((cd=getClass(scope))==0) nd=namespaceDict[scope];
    }
    else // no scope, check the scope in which the docs where found
    {
      if (( root->parent->section & Entry::SCOPE_MASK )
          && root->parent->name.length()>0
         ) // found enum docs inside a compound
      {
        QCString scope=root->parent->name;
        if ((cd=getClass(scope))==0) nd=namespaceDict[scope];
      }
      name=root->name.copy();
    }
    if (cd && name.length()>0) // found a enum inside a compound
    {
      //printf("Enum `%s'::`%s'\n",cd->name(),name.data());
      fd=0;
      mnd=&memberNameDict;
      mnl=&memberNameList;
      isGlobal=FALSE;
    }
    else if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@') // found enum inside namespace
    {
      mnd=&functionNameDict;
      mnl=&functionNameList;
      isGlobal=TRUE;
    }
    else // found a global enum
    {
      bool ambig;
      fd=findFileDef(&inputNameDict,root->fileName,ambig);
      mnd=&functionNameDict;
      mnl=&functionNameList;
      isGlobal=TRUE;
    }
    if (name.length()>0)
    {
      // new enum type
      md = new MemberDef(0,name,0,0,root->protection,Normal,FALSE,FALSE,
               MemberDef::Enumeration,0,0);
      if (!isGlobal) md->setMemberClass(cd); else md->setFileDef(fd);
      md->setDefFile(root->fileName);
      md->setDefLine(root->startLine);
      md->addSectionsToDefinition(root->anchors);
      if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
      {
        md->setDefinition(nd->name()+"::"+name);  
        nd->insertMember(md);
        md->setNamespace(nd);
      }
      else if (isGlobal)
      {
        md->setDefinition(name);
        fd->insertMember(md);
      }
      else if (cd)
      {
        md->setDefinition(cd->name()+"::"+name);
        cd->insertMember(md);
        cd->insertUsedFile(root->fileName);
      }
      md->setDocumentation(root->doc);
      md->setBriefDescription(root->brief);

      MemberName *mn;
      if ((mn=(*mnd)[name]))
      {
        // this is used if the same enum is in multiple namespaces/classes
        mn->inSort(md);
      }
      else // new enum name
      {
        mn = new MemberName(name);
        mn->inSort(md);
        mnd->insert(name,mn);
        mnl->inSort(mn);
        //printf("add %s to new memberName. Now %d members\n",
        //       name.data(),mn->count());
      }

      EntryListIterator eli(*root->sublist);
      Entry *e;
      for (;(e=eli.current());++eli)
      {
        MemberName *fmn=0;
        if (e->name.length()>0 && (fmn=(*mnd)[e->name])) 
           // get list of members with the same name as the field
        {
          MemberDef *fmd=fmn->first();
          while (fmd) // search for the scope with the right name
          {
            if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
            {
              NamespaceDef *fnd=fmd->getNamespace();
              if (fnd==nd) // enum value is inside a namespace
              {
                md->insertEnumField(fmd);
                fmd->setEnumScope(md);
              }
            }
            else if (isGlobal)
            {
              FileDef *ffd=fmd->getFileDef();
              if (ffd==fd) // enum value has file scope
              {
                md->insertEnumField(fmd);
                fmd->setEnumScope(md);
              }
            }
            else
            {
              ClassDef *fcd=fmd->memberClass();
              if (fcd==cd) // enum value is inside a class
              {
                md->insertEnumField(fmd); // add field def to list
                fmd->setEnumScope(md);    // cross ref with enum name
              }
            }
            fmd=fmn->next();
          } 
        }
      }
    }
  }
  else
  {
    EntryListIterator eli(*root->sublist);
    Entry *e;
    for (;(e=eli.current());++eli)
    {
      findEnums(e);
    }
  }
}

//----------------------------------------------------------------------
// find the documentation blocks for the enumerations

void findEnumDocumentation(Entry *root)
{
  if (root->section==Entry::ENUMDOC_SEC
      && root->name.length()>0
      && root->name[0]!='@'        // skip anonymous enums
     )
  {
    //printf("Found docs for enum with name `%s'\n",root->name.data());
    int i;
    ClassDef *cd=0;
    QCString name;
    if ((i=root->name.findRev("::"))!=-1) // scope is specified
    {
      QCString scope=root->name.left(i); // extract scope
      name=root->name.right(root->name.length()-i-2); // extract name
      cd=getClass(scope);
      //printf("Scope=`%s' Name=`%s'\n",scope.data(),name.data());
    }
    else // no scope, check the scope in which the docs where found
    {
      if (( root->parent->section & Entry::COMPOUND_MASK )
          && root->parent->name.length()>0
         ) // found enum docs inside a compound
      {
        cd=getClass(root->parent->name);
      }
      name=root->name.copy();
    }
    if (name.length()>0)
    {
      bool found=FALSE;
      if (cd)
      {
        //printf("Enum: scope=`%s' name=`%s'\n",cd->name(),name.data());
        QCString className=cd->name().copy();
        MemberName *mn=memberNameDict[name];
        if (mn)
        {
          MemberDef  *md=mn->first();
          while (md && !found)
          {
            ClassDef *cd=md->memberClass();
            if (cd && cd->name()==className)
            {
              // documentation outside a compound overrides the documentation inside it
              if (!md->documentation() || root->parent->name.length()==0) 
              {
                md->setDocumentation(root->doc);
              }

              // brief descriptions inside a compound override the documentation 
              // outside it
              if (!md->briefDescription() || root->parent->name.length()>0)
              {
                md->setBriefDescription(root->brief);
              }
              md->addSectionsToDefinition(root->anchors);
              found=TRUE;
            }
            md=mn->next();
          }
        }
        else
        {
          //printf("MemberName %s not found!\n",name.data());
        }
      }
      else // enum outside class 
      {
        MemberDef  *md;
        MemberName *mn=functionNameDict[name];
        if (mn && (md=mn->first()))
        {
          md->setDocumentation(root->doc);
          md->setBriefDescription(root->brief);
          md->addSectionsToDefinition(root->anchors);
          found=TRUE;
        }
      } 
      if (!found)
      {
        warn("Warning: Documentation for undefined enum `%s' found at"
             " line %d of file %s\n",name.data(),
               root->startLine,root->fileName.data());
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    findEnumDocumentation(e);
  }
}

// seach for each enum (member or function) in mnl if it has documented 
// enum values.
static void findDEV(const MemberNameList &mnl)
{
  MemberName *mn;
  MemberNameListIterator mnli(mnl);
  // for each member name
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md;
    MemberNameIterator mni(*mn);
    // for each member definition
    for (mni.toFirst();(md=mni.current());++mni)
    {
      if (md->isEnumerate()) // member is an enum
      {
        QList<MemberDef> *fmdl = md->enumFieldList();
        int documentedEnumValues=0;
        if (fmdl) // enum has values
        {
          MemberDef *fmd=fmdl->first();
          // for each enum value
          while (fmd)
          {
            if (fmd->hasDocumentation()) documentedEnumValues++;
            fmd=fmdl->next();
          }
        }
        // at least one enum value is documented
        if (documentedEnumValues>0) md->setDocumentedEnumValues(TRUE);
      }
    }
  }
}

// seach for each enum (member or function) if it has documented enum 
// values.
void findDocumentedEnumValues()
{
  findDEV(memberNameList);
  findDEV(functionNameList); 
}

//----------------------------------------------------------------------
// recursive function:
// returns TRUE iff class definition `bcd' represents an (in)direct base 
// class of class definition `cd'.

bool isBaseClass(ClassDef *bcd,ClassDef *cd)
{
  bool found=FALSE;
  //printf("isBaseClass(cd=%s) looking for %s\n",cd->name().data(),bcd->name().data());
  BaseClassListIterator bcli(*cd->baseClasses());
  for ( ; bcli.current() && !found ; ++bcli)
  {
    ClassDef *ccd=bcli.current()->classDef;
    //printf("isBaseClass() baseclass %s\n",ccd->name().data());
    if (ccd==bcd) 
      found=TRUE;
    else 
      found=isBaseClass(bcd,ccd);
  }
  return found;
}

//----------------------------------------------------------------------
// computes the relation between all members. For each member `m'
// the members that override the implementation of `m' are searched and
// the member that `m' overrides is searched.

void computeMemberRelations()
{
  MemberName *mn=memberNameList.first();
  while (mn) // for each member name
  {
    MemberNameIterator mdi(*mn);
    for ( ; mdi.current() ; ++mdi) // for each member with a specific arg list
    {
      MemberDef *md=mdi.current();
      MemberNameIterator bmdi(*mn);
      for ( ; bmdi.current() ; ++bmdi) // for each other member with that signature
      {
        MemberDef *bmd=bmdi.current();
        ClassDef *bmcd = bmd->memberClass();
        ClassDef *mcd = md->memberClass();
        //printf("Check relation between `%s'::`%s' and `%s'::`%s'\n",
        //       mcd->name().data(),md->name().data(),
        //       bmcd->name().data(),bmd->name().data()
        //      );
        if (md!=bmd && bmcd && mcd && isBaseClass(bmcd,mcd))
        {
          //printf(" Base argList=`%s'\n Super argList=`%s'\n",
          //        argListToString(bmd->argumentList()).data(),
          //        argListToString(md->argumentList()).data()
          //      );
          if ( matchArguments(bmd->argumentList(),md->argumentList()) )
          {
            //printf("  match found!\n");
            if (mcd && bmcd && 
                mcd->isVisibleExt() && bmcd->isVisibleExt()
               )
            {
              md->setReimplements(bmd);
              bmd->insertReimplementedBy(md);
            }
          }  
        }
      }
    }
    mn=memberNameList.next();
  }  
}

//----------------------------------------------------------------------------
// recusively merges the `all members' lists of class cd's base classes
// with that of class `cd' itself.

void mergeMembers(ClassDef *cd,BaseClassList *bcl)
{
  //if (mcd->flag==TRUE)
  //{
  //  err("Error: Cyclic inhertance dependency found for class %s\n",mcd->name());
  // return;
  //}
  //mcd->flag=TRUE;

  BaseClassListIterator bcli(*bcl);
  BaseClassDef *bcd;
  for ( ; (bcd=bcli.current()) ; ++bcli )
  {
    ClassDef *bClass=bcd->classDef; 
    // merge the members of bClass with the onces from cd
    
    mergeMembers(bClass,bClass->baseClasses());
    // the all member list of the branch until bClass is now complete
    // so we can merge it with cd
    
    MemberNameInfoList *srcMnl  = bClass->memberNameInfoList();
    MemberNameInfoDict *dstMnd  =     cd->memberNameInfoDict();
    MemberNameInfoList *dstMnl  =     cd->memberNameInfoList();
    
    MemberNameInfoListIterator srcMnili(*srcMnl);
    MemberNameInfo *srcMni;
    for ( ; (srcMni=srcMnili.current()) ; ++srcMnili)
    {
      //printf("Base member name %s\n",srcMni->memberName());
      MemberNameInfo *dstMni;
      if ((dstMni=dstMnd->find(srcMni->memberName())))
        // a member with that name is already in the class.
        // the member may hide or reimplement the one in the super class
        // or there may be another path to the base class that is already 
        // visited via another branch in the class hierarchy.
      {
        MemberNameInfoIterator srcMnii(*srcMni); 
        MemberInfo *srcMi;
        for ( ; (srcMi=srcMnii.current()) ; ++srcMnii )
        {
          MemberDef *srcMd = srcMi->memberDef;
          bool found=FALSE;
          bool ambigue=FALSE;
          MemberNameInfoIterator dstMnii(*dstMni); 
          MemberInfo *dstMi;
          for ( ; (dstMi=dstMnii.current()) && !found; ++dstMnii )
          {
            MemberDef *dstMd = dstMi->memberDef;
            if (srcMd!=dstMd) // different members
            {
              ClassDef *srcCd = srcMd->memberClass();
              ClassDef *dstCd = dstMd->memberClass();
              //printf("Is %s a base class of %s?\n",srcCd->name(),dstCd->name());
              if (srcCd==dstCd || isBaseClass(srcCd,dstCd)) 
                // member is in the same or a base class
              {
                found = 
                  /*matchArguments(srcMd->argsString(),dstMd->argsString());*/
                  matchArguments(srcMd->argumentList(),dstMd->argumentList());
                ambigue=!found;
              }
              else // member is in a non base class => multiple inheritance
                   // using the same base class.
              {
                //printf("$$ Existing member %s %s add scope %s\n",
                //    dstMi->ambiguityResolutionScope.data(),
                //    dstMd->name().data(),
                //    dstMi->scopePath.left(dstMi->scopePath.find("::")+2).data());
                         
                QCString scope=dstMi->scopePath.left(dstMi->scopePath.find("::")+2);
                if (scope!=dstMi->ambiguityResolutionScope.left(scope.length()))
                  dstMi->ambiguityResolutionScope.prepend(scope);
                ambigue=TRUE;
              }
            }
            else // same members
            {
              // do not add if base class is virtual or 
              // if scope paths are equal
              if ((srcMi->virt==Virtual && dstMi->virt==Virtual) ||
                  bClass->name()+"::"+srcMi->scopePath == dstMi->scopePath
                 ) found=TRUE;
              else // member can be reached via multiple paths in the 
                   // inheritance tree
              {
                //printf("$$ Existing member %s %s add scope %s\n",
                //    dstMi->ambiguityResolutionScope.data(),
                //    dstMd->name().data(),
                //    dstMi->scopePath.left(dstMi->scopePath.find("::")+2).data());

                QCString scope=dstMi->scopePath.left(dstMi->scopePath.find("::")+2);
                if (scope!=dstMi->ambiguityResolutionScope.left(scope.length()))
                  dstMi->ambiguityResolutionScope.prepend(scope);
                ambigue=TRUE;
              }
            }
          }
          if (!found && srcMd->protection()!=Private)
          {
            Specifier virt=srcMi->virt;
            if (srcMi->virt==Normal && bcd->virt!=Normal) virt=bcd->virt;
            MemberInfo *newMi = new MemberInfo(srcMd,bcd->prot,virt);
            newMi->scopePath=bClass->name()+"::"+srcMi->scopePath;
            if (ambigue)
            {
              //printf("$$ New member %s %s add scope %s::\n",
              //     srcMi->ambiguityResolutionScope.data(),
              //     srcMd->name().data(),
              //     bClass->name().data());

              QCString scope=bClass->name().copy(); scope+="::";
              if (scope!=srcMi->ambiguityResolutionScope.left(scope.length()))
                newMi->ambiguityResolutionScope=
                  scope+srcMi->ambiguityResolutionScope;
            }
            dstMni->append(newMi);
          }
        }
      }
      else // base class has a member that is not in the super class => copy
      {
        // create a deep copy of the list (only the MemberInfo's will be 
        // copied, not the actual MemberDef's)
        MemberNameInfo *newMni = new MemberNameInfo(srcMni->memberName()); 
        
        // copy the member(s) from the base to the super class
        MemberNameInfoIterator mnii(*srcMni);
        MemberInfo *mi;
        for (;(mi=mnii.current());++mnii)
        {
          if (mi->memberDef->protection()!=Private)
          {
            Specifier virt=mi->virt;
            if (mi->virt==Normal && bcd->virt!=Normal) virt=bcd->virt;
            MemberInfo *newMi=new MemberInfo(mi->memberDef,bcd->prot,virt);
            //if (mi->memberDef->memberClass()!=bClass)
            newMi->scopePath=bClass->name()+"::"+mi->scopePath;
            newMi->ambiguityResolutionScope=mi->ambiguityResolutionScope.copy();
            newMni->append(newMi);
          }
        }
        
        // add it to the list and dictionary
        dstMnl->inSort(newMni);
        dstMnd->insert(newMni->memberName(),newMni);
      }
    }
  }
}

//----------------------------------------------------------------------------
// builds the list of all members for each class

void buildCompleteMemberLists()
{
  ClassDef *cd;
  ClassListIterator cli(classList);
  for (;(cd=cli.current());++cli)
  {
    //if (!cd->isReference()) printf("Building member for class %s\n",cd->name());
    //ClassListIterator vcli(classList);
    //for (;(vcd=vcli.current());++vcli) vcd->flag = FALSE;
    if (!cd->isReference() && // not an external class
         cd->superClasses()->count()==0 && // is a root of the hierarchy
         cd->baseClasses()->count()>0) // and has at least one base class
    {
      //printf("merging members for class %s\n",cd->name());
      mergeMembers(cd,cd->baseClasses());
    }
  }
}

//----------------------------------------------------------------------------

void generateFileDocs()
{
  if (documentedFiles==0) return;
  writeFileIndex(*outputList);
  
  if (inputNameList.count()>0)
  {
    FileName *fn=inputNameList.first();
    while (fn)
    {
      FileDef *fd=fn->first();
      while (fd)
      {
        if (!fd->isReference() && fd->hasDocumentation())
        {
          msg("Generating docs for file %s...\n",fd->name().data());
          fd->writeDocumentation(*outputList);
        }
        fd=fn->next();
      }
      fn=inputNameList.next();
    }
  }
}

//----------------------------------------------------------------------------
// generate the documentation of all classes
  
void generateClassDocs()
{
  // write the installdox script if necessary
  if (Config::generateHtml && 
      (Config::tagFileList.count()>0 || Config::searchEngineFlag)) 
    writeInstallScript();
  
  msg("Generating index page...\n"); 
  writeIndex(*outputList);

  msg("Generating compound index...\n");
  writeAnnotatedIndex(*outputList);

  msg("Generating hierarchical class index...\n");
  writeHierarchicalIndex(*outputList);

  if (includeFiles.count()>0)
  {
    msg("Generating header index...\n");
    writeHeaderIndex(*outputList);
  }

  msg("Generating member index...\n");
  writeMemberIndex(*outputList);

  if (exampleList.count()>0)
  {
    msg("Generating example index...\n");
  }

  ClassListIterator cli(classList);
  for ( ; cli.current() ; ++cli )
  {
    ClassDef *cd=cli.current();
    if (!cd->isReference() && 
        //!cd->name().isEmpty() && 
        //cd->name().at(0)!='@' && 
        //(cd->protection()!=Private || Config::extractPrivateFlag) &&
        //(cd->hasDocumentation() || !Config::hideClassFlag)
        cd->isVisible()
       ) 
         // skip external references and anonymous compounds
    {
      msg("Generating docs for compound %s...\n",cd->name().data());
      
      cd->writeDocumentation(*outputList);
      cd->writeMemberList(*outputList);
      if (Config::verbatimHeaderFlag) cd->writeIncludeFile(*outputList);
    }
  }
}

//----------------------------------------------------------------------------

void findDefineDocumentation(Entry *root)
{
  if ((root->section==Entry::DEFINEDOC_SEC ||
       root->section==Entry::DEFINE_SEC) && root->name.length()>0
     )
  {
    //printf("found define `%s' `%s' brief=`%s' doc=`%s'\n",
    //       root->name.data(),root->args.data(),root->brief.data(),root->doc.data());
    MemberName *mn=functionNameDict[root->name];
    if (mn)
    {
      int count=0;
      MemberDef *md=mn->first();
      while (md)
      {
        if (md->memberType()==MemberDef::Define) count++;
        md=mn->next();
      }
      if (count==1)
      {
        md=mn->first();
        while (md)
        {
          if (md->memberType()==MemberDef::Define)
          {
            if (md->documentation().isEmpty())
              md->setDocumentation(root->doc);
            if (md->briefDescription().isEmpty())
              md->setBriefDescription(root->brief);
            md->addSectionsToDefinition(root->anchors);
          }
          md=mn->next();
        }
      }
      else if (count>1 && (root->doc.length()>0 || root->brief.length()>0)) 
        // multiple defines don't know where to add docs
      {
        md=mn->first();
        while (md)
        {
          if (md->memberType()==MemberDef::Define)
          {
            FileDef *fd=md->getFileDef();
            if (fd && fd->absFilePath()==root->fileName) 
              // doc and define in the same file assume they belong together.
            {
              if (md->documentation().isEmpty())
                md->setDocumentation(root->doc);
              if (md->briefDescription().isEmpty())
                md->setBriefDescription(root->brief);
              md->addSectionsToDefinition(root->anchors);
            }
          }
          md=mn->next();
        }
        //warn("Warning: define %s found in the following files:\n",root->name.data());
        //warn("Cannot determine where to add the documentation found "
        //     "at line %d of file %s. \n",
        //     root->startLine,root->fileName.data());
      }
    }
    else // define not found
    {
      warn("Warning: documentation for unknown define %s found at line %d of "
           "file %s\n",root->name.data(),root->startLine,root->fileName.data());
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    findDefineDocumentation(e);
  }
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// create a (sorted) list of separate documentation pages

void buildPageList(Entry *root)
{
  if (root->section == Entry::PAGEDOC_SEC)
  {
    if (root->name.length()>0)
    {
      PageInfo *pi=0;
      if ((pi=pageDict[root->name]))
      {
        //warn("Warning: Page %s was already documented. Ignoring documentation "
        //     "at line %d of %s\n",root->name.data(),root->startLine,
        //                          root->fileName.data());
        
        // append documentation block to the page.
        pi->doc+="\n\n"+root->doc;
      }
      else
      {
        QCString baseName=root->name.copy();
        if (baseName.right(4)==".tex") 
          baseName=baseName.left(baseName.length()-4);
        else if (baseName.right(5)==".html")
          baseName=baseName.left(baseName.length()-5);
        pi=new PageInfo(baseName, root->doc,
                        root->args.stripWhiteSpace());
        QCString pageName;
        if (Config::caseSensitiveNames)
          pageName=pi->name.copy();
        else
          pageName=pi->name.lower();
        setFileNameForSections(root->anchors,pageName);

        pageList.append(pi);
        pageDict.insert(baseName,pi);
        if (pi->title.length()>0)
        {
          //outputList->writeTitle(pi->name,pi->title);
          
          // a page name is a label as well!
          SectionInfo *si=new SectionInfo(
              pi->name,pi->title,SectionInfo::Section);
          si->fileName=pageName+".html";
          //printf("  SectionInfo: sec=%p sec->fileName=%s\n",si,si->fileName.data());
          //printf("Adding section info %s\n",pi->name.data());
          sectionDict.insert(pageName,si);
        }
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildPageList(e);
  }
}

//----------------------------------------------------------------------------

void resolveUserReferences()
{
  QDictIterator<SectionInfo> sdi(sectionDict);
  SectionInfo *si;
  for (;(si=sdi.current());++sdi)
  {
    if (si->definition)
    {
      //printf("si=`%s' def=`%s' file=`%s'\n",
      //         si->label.data(),
      //         si->definition->name().data(),
      //         si->definition->getOutputFileBase().data());
      si->fileName=si->definition->getOutputFileBase().copy();
    }
  }
}


//----------------------------------------------------------------------------
// generate all separate documentation pages

void generatePageDocs()
{
  PageInfo *pi=pageList.first();
  while (pi)
  {
    msg("Generating docs for page %s...\n",pi->name.data());
    outputList->disable(OutputGenerator::Man);
    QCString pageName;
    if (Config::caseSensitiveNames)
      pageName=pi->name.copy();
    else
      pageName=pi->name.lower();
    
    startFile(*outputList,pageName,pi->title);
    SectionInfo *si=0;
    if (pi->title.length()>0 && pi->name.length()>0 &&
        (si=sectionDict[pi->name])!=0)
    {
      outputList->writeSection(si->label,si->title,FALSE);
    }
    parseDoc(*outputList,0,0,pi->doc);
    endFile(*outputList);
    outputList->enable(OutputGenerator::Man);
    pi=pageList.next();
  }
}

//----------------------------------------------------------------------------
// create a (sorted) list & dictionary of example pages

void buildExampleList(Entry *root)
{
  if (root->section == Entry::EXAMPLE_SEC)
  {
    if (root->name.length()>0) 
    {
      if (exampleDict[root->name])
      {
        warn("Warning: Example %s was already documented. Ignoring "
             "documentation at line %d of %s\n",root->name.data(),
             root->startLine,root->fileName.data());
      }
      else
      {
        PageInfo *pi=new PageInfo(root->name,root->doc,root->args);
        setFileNameForSections(root->anchors,root->name);
        exampleList.inSort(pi);
        exampleDict.insert(root->name,pi);
      }
    }
  }
  //EntryList *entryList=root->sublist;
  //Entry *e = entryList->first();
  //while (e)
  //{
  //  buildExampleList(e);
  //  e=entryList->next(); 
  //}
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildExampleList(e);
  }
}

//----------------------------------------------------------------------------
// generate the example documentation 

void generateExampleDocs()
{
  PageInfo *pi=exampleList.first();
  while (pi)
  {
    msg("Generating docs for example %s...\n",pi->name.data());
    QCString n=convertSlashes(pi->name,TRUE)+"-example";
    startFile(*outputList,n,"Example Documentation");
    //outputList->writeTitle(pi->name,pi->name);
    parseExample(*outputList,pi->doc+"\n\\include "+pi->name,pi->name);
    endFile(*outputList);
    pi=exampleList.next();
  }
}

//----------------------------------------------------------------------------
// generate module pages

void generateGroupDocs()
{
  GroupListIterator gli(groupList);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    //printf("group %s #members=%d\n",gd->name().data(),gd->countMembers());
    if (gd->countMembers()>0) gd->writeDocumentation(*outputList);
  }
}

//----------------------------------------------------------------------------
// generate module pages

void generateNamespaceDocs()
{
  writeNamespaceIndex(*outputList);
  
  NamespaceListIterator nli(namespaceList);
  NamespaceDef *nd;
  for (;(nd=nli.current());++nli)
  {
    if ((nd->getReference() || nd->hasDocumentation()) &&
        !nd->name().isEmpty() && nd->name().at(0)!='@')
    {
      msg("Generating docs for namespace %s\n",nd->name().data());
      nd->writeDocumentation(*outputList);
    }
  }
}

#if defined(_WIN32)
static QCString fixSlashes(QCString &s)
{
  QCString result;
  uint i;
  for (i=0;i<s.length();i++)
  {
    switch(s.at(i))
    {
      case '/': 
      case '\\': 
        result+="\\\\"; 
        break;
      default:
        result+=s.at(i);
    }
  }
  return result;
}
#endif


//----------------------------------------------------------------------------
// generate files for the search engine

void generateSearchIndex()
{
  if (Config::searchEngineFlag && Config::generateHtml)
  {
    // create search index
    QCString fileName;
    writeSearchButton(Config::htmlOutputDir);

#if !defined(_WIN32)
    // create cgi script
    fileName = Config::htmlOutputDir+"/"+Config::cgiName;
    QFile f(fileName);
    if (f.open(IO_WriteOnly))
    {
      QTextStream t(&f);
      t << "#!/bin/sh"   << endl
        << "DOXYSEARCH=" << Config::binAbsPath << "/doxysearch" << endl
        << "DOXYPATH="   << Config::docAbsPath << " ";

      char *s=Config::extDocPathList.first();
      while (s)
      {
        t << s << " ";
        s=Config::extDocPathList.next();
      }

      t << endl 
        << "if [ -f $DOXYSEARCH ]" << endl
        << "then" << endl
        << "  $DOXYSEARCH $DOXYPATH" << endl 
        << "else" << endl
        << "  echo \"Content-Type: text/html\"" << endl
        << "  echo \"\"" << endl
        << "  echo \"<h2>Error: $DOXYSEARCH not found. Check cgi script!</h2>\"" << endl
        << "fi" << endl;

      f.close();
      struct stat stat_struct;
      stat(fileName,&stat_struct);
      chmod(fileName,stat_struct.st_mode|S_IXUSR|S_IXGRP|S_IXOTH);
    }
    else
    {
      err("Error: Cannot open file %s for writing\n",fileName.data());
    }
#else /* Windows platform */
    // create cgi program
    fileName = Config::cgiName.copy();
    if (fileName.right(4)==".cgi") 
      fileName=fileName.left(fileName.length()-4);
    fileName+=".c";
    fileName.prepend(Config::htmlOutputDir+"/");
    QFile f(fileName);
    if (f.open(IO_WriteOnly))
    {
      QTextStream t(&f);
      t << "#include <stdio.h>" << endl;
      t << "#include <stdlib.h>" << endl;
      t << "#include <process.h>" << endl;
      t << endl;
      t << "const char *DOXYSEARCH = \"" << 
           fixSlashes(Config::binAbsPath) << "\\\\doxysearch.exe\";" << endl;
      t << "const char *DOXYPATH = \"" << 
           fixSlashes(Config::docAbsPath) << "\";" << endl;
      t << endl;
      t << "int main(void)" << endl;
      t << "{" << endl;
      t << "  char buf[1024];" << endl;
      t << "  sprintf(buf,\"%s %s\",DOXYSEARCH,DOXYPATH);" << endl; 
      t << "  if (system(buf))" << endl;
      t << "  {" << endl;
      t << "    printf(\"Content-Type: text/html\\n\\n\");" << endl;
      t << "    printf(\"<h2>Error: failed to execute %s</h2>\\n\",DOXYSEARCH);" << endl;
      t << "    exit(1);" << endl;
      t << "  }" << endl;
      t << "  return 0;" << endl;
      t << "}" << endl;
      f.close();
    }
    else
    {
      err("Error: Cannot open file %s for writing\n",fileName.data());
    }
#endif /* !defined(_WIN32) */
    
    // create config file
    fileName = Config::htmlOutputDir+"/search.cfg";
    f.setName(fileName);
    if (f.open(IO_WriteOnly))
    {
      QTextStream t(&f);
      t << Config::docURL << endl << Config::cgiURL << "/" << Config::cgiName << endl;
      f.close();
    }
    else
    {
      err("Error: Cannot open file %s for writing\n",fileName.data());
    }
    //outputList->generateExternalIndex();
    outputList->disableAllBut(OutputGenerator::Html);
    startFile(*outputList,"header.html","Search Engine",TRUE);
    outputList->endPlainFile();
    outputList->startPlainFile("footer.html");
    endFile(*outputList,TRUE);
    outputList->enableAll();
  }
}

//----------------------------------------------------------------------------
// generate the configuration file

void generateConfigFile(const char *configFile,bool shortList)
{
  QFileInfo fi(configFile);
  QFile f(configFile);
 
  if (fi.exists()) // create a backup
  {
    QDir dir=fi.dir();
    dir.rename(fi.fileName(),fi.fileName()+".bak");
  } 
  if (f.open(IO_WriteOnly))
  {
    writeTemplateConfig(&f,shortList);
    f.close();
    msg("\n\nConfiguration file `%s' created.\n\n",configFile);
    msg("Now edit the configuration file and enter\n\n");
    if (strcmp(configFile,"Doxyfile") || strcmp(configFile,"doxyfile"))
      msg("  doxygen %s\n\n",configFile);
    else
      msg("  doxygen\n\n");
    msg("to generate the documentation for your project\n\n");
  }
  else
  {
    err("Error: Cannot open file %s for writing\n");
    exit(1);
  }
}

//----------------------------------------------------------------------------
// read and parse a tag file

bool readLineFromFile(QFile &f,QCString &s)
{
  char c=0;
  s.resize(0);
  while (!f.atEnd() && (c=f.getch())!='\n') s+=c;
  return f.atEnd();
}

void readTagFile(const char *file)
{
  QFileInfo fi(file);
  if (!fi.exists() || !fi.isFile())
  {
    err("Error: Tag file `%s' does not exist or is not a file\n",file);
    exit(1);
  }
  msg("Reading tag file %s...\n",file);
  parseTagFile(file);
}

//----------------------------------------------------------------------------
// returns TRUE if the name of the file represented by `fi' matches
// one of the file patterns in the `patList' list.

bool patternMatch(QFileInfo *fi,QStrList *patList)
{
  bool found=FALSE;
  if (patList)
  { 
    char *pattern=patList->first();
    while (pattern && !found)
    {
#if defined(_WIN32) // windows
      QRegExp re(pattern,FALSE,TRUE); // case insensitive match 
#else                // unix
      QRegExp re(pattern,TRUE,TRUE);  // case sensitive match
#endif
      found = found || re.match(fi->fileName())!=-1;
      pattern=patList->next();
    }
  }
  return found;
}

//----------------------------------------------------------------------------
// reads a file into an array and filters out any 0x00 and 0x06 bytes,
// because these are special for the parser.

void copyAndFilterFile(const char *fileName,BufStr &dest)
{
  // try to open file
  int size=0;
  uint oldPos = dest.curPos();

  QFileInfo fi(fileName);
  if (!fi.exists()) return;
  if (Config::inputFilter.isEmpty())
  {
    QFile f(fileName);
    if (!f.open(IO_ReadOnly))
    {
      err("Error: could not open file %s\n",fileName);
      return;
    }
    size=fi.size();
    // read the file
    dest.skip(size);
    if (f.readBlock(dest.data()+oldPos,size)!=size)
    {
      err("Error while reading file %s\n",fileName);
      return;
    }
  }
  else
  {
    int c;
//    char *p=dest;
    QCString cmd=Config::inputFilter+" "+fileName;
    FILE *f=popen(cmd,"r");
    if (!f)
    {
      err("Error: could not execute filter %s\n",Config::inputFilter.data());
      return;
    }
    while ((c=fgetc(f))!=EOF) dest.addChar(c),size++;
  }
  // filter unwanted bytes from the resulting data
  uchar *p=(uchar *)dest.data()+oldPos;
  uchar conv[256];
  int i;
  for (i=0;i<256;i++) conv[i]=i;
  conv[0x06]=0x20; // replace the offending characters with spaces
  conv[0x00]=0x20;
  // remove any special markers from the input
  for (i=0;i<size;i++,p++) *p=conv[*p];
  // adjust pointer
}

//----------------------------------------------------------------------------
// Reads a file to a string.
// The name of the file is written in front of the file's contents and
// between 0x06 markers


void readFiles(BufStr &output)
{
  QCString *s=inputFiles.first();
//  char *p=output.data();
  while (s)
  {
    QCString fileName=*s;

    //int fileSize=fi->fileInfo()->size();
    int fileNameSize=fileName.length();
    //int streamLength=fileSize+fileNameSize+4; 

    //QCString fileText(streamLength);

    // add begin filename marker
//    *p++=0x06; 
    output.addChar(0x06);
    // copy filename
//    memcpy(p,fileName.data(),fileNameSize);
//    p+=fileNameSize;
    output.addArray(fileName.data(),fileNameSize);
    
    // add end filename marker
//    *p++=0x06;
    output.addChar(0x06);
//    *p++='\n'; // to make ^ work while scanning the first line of a file!
    output.addChar('\n');
    if (Config::preprocessingFlag)
    {
      msg("Preprocessing %s...\n",s->data());
      preprocessFile(fileName,output);
    }
    else
    {
      msg("Reading %s...\n",s->data());
      copyAndFilterFile(fileName,output);
    }

    s=inputFiles.next();
  }
//  *p++='\0';
  output.addChar('\n'); /* to prevent problems under Windows ? */
  output.addChar(0);
  //printf("Output after preprocessing:\n---------\n%s\n----------\n",output.data());
  //printf("Final length = %d\n",p-output.data());
}

//----------------------------------------------------------------------------
// Read all files matching at least one pattern in `patList' in the 
// directory represented by `fi'.
// The directory is read iff the recusiveFlag is set.
// The contents of all files is append to the input string

int readDir(QFileInfo *fi,
            FileNameList *fnList,
            FileNameDict *fnDict,
            StringDict  *exclDict,
            QStrList *patList,
            QStrList *exclPatList,
            StringList *resultList,
            StringDict *resultDict
           )
{
  QDir dir((const char *)fi->absFilePath());
  dir.setFilter( QDir::Files | QDir::Dirs );
  int totalSize=0;
  
  const QFileInfoList *list = dir.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *cfi;
  
  while ((cfi=it.current()))
  {
    if (exclDict==0 || exclDict->find(cfi->absFilePath())==0) 
    { // file should not be excluded
      if (!cfi->exists() || !cfi->isReadable())
      {
         err("Error: source %s is not a readable file or directory... skipping.\n",cfi->absFilePath().data());
      }
      else if (cfi->isFile() && 
          patternMatch(cfi,patList) && !patternMatch(cfi,exclPatList))
      {
        totalSize+=cfi->size()+cfi->absFilePath().length()+4;
        QCString name=convertToQCString(cfi->fileName());
        if (fnDict)
        {
          FileDef  *fd=new FileDef(cfi->dirPath()+"/",name);
          FileName *fn=0;
          if (name.length()>0 && (fn=(*fnDict)[name]))
          {
            fn->append(fd);
          }
          else
          {
            fn = new FileName(name);
            fn->append(fd);
            if (fnList) fnList->inSort(fn);
            fnDict->insert(name,fn);
          }
        }
        QCString *rs=0;
        if (resultList || resultDict)
        {
          rs=new QCString(cfi->absFilePath());
        }
        if (resultList) resultList->append(rs);
        if (resultDict) resultDict->insert(cfi->absFilePath(),rs);
      }
      else if (Config::recursiveFlag && cfi->isDir() && cfi->fileName()!="." && 
          cfi->fileName()!="..")
      {
        cfi->setFile(cfi->absFilePath());
        totalSize+=readDir(cfi,fnList,fnDict,exclDict,
            patList,exclPatList,resultList,resultDict);
      }
    }
    ++it;
  }
  return totalSize;
}

//----------------------------------------------------------------------------
// read the file with name `name' into a string.

QCString readExampleFile(const char *name)
{
  QCString example;
  QFileInfo fi(name);
  if (fi.exists())
  {
    QFile f((const char *)fi.absFilePath());
    if (f.open(IO_ReadOnly))
    {
      example.resize(fi.size()+1);
      if ((int)fi.size()!=f.readBlock(example.data(),fi.size()))
      {
        err("Error while reading file %s\n",fi.absFilePath().data());
        //exit(1);
        return "";
      }
      example.at(fi.size())='\0';
    }
    else
    {
      err("Error opening file %s\n",fi.absFilePath().data());
      //exit(1);
      return "";
    }
  }
  else
  {
    err("Error: example file %s does not exist\n",name);
    exit(1);
  }
  return example;
}

//----------------------------------------------------------------------------
// read a file or all files in a directory and append their contents to the
// input string. The names of the files are appended to the `fiList' list.

int readFileOrDirectory(const char *s,
                        FileNameList *fnList,
                        FileNameDict *fnDict,
                        StringDict *exclDict,
                        QStrList *patList,
                        QStrList *exclPatList,
                        StringList *resultList,
                        StringDict *resultDict
                       )
{
  QFileInfo fi(s);
  int totalSize=0;
  {
    if (exclDict==0 || exclDict->find(fi.absFilePath())==0)
    {
      if (!fi.exists() || !fi.isReadable())
      {
        err("Error: source %s is not a readable file or directory... skipping.\n",s);
      }
      else if (fi.isFile()) 
      {
        totalSize+=fi.size()+fi.absFilePath().length()+4; //readFile(&fi,fiList,input); 
        //fiList->inSort(new FileInfo(fi));
        QCString name=convertToQCString(fi.fileName());
        if (fnDict)
        {
          FileDef  *fd=new FileDef(fi.dirPath(TRUE)+"/",name);
          FileName *fn=0;
          if (name.length()>0 && (fn=(*fnDict)[name]))
          {
            fn->append(fd);
          }
          else
          {
            fn = new FileName(name);
            fn->append(fd);
            if (fnList) fnList->inSort(fn);
            fnDict->insert(name,fn);
          }
        }
        QCString *rs=0;
        if (resultList || resultDict)
        {
          rs=new QCString(fi.absFilePath());
        }
        if (resultList) resultList->append(rs);
        if (resultDict) resultDict->insert(fi.absFilePath(),rs);
      }
      else if (fi.isDir()) // readable dir
        totalSize+=readDir(&fi,fnList,fnDict,exclDict,patList,
            exclPatList,resultList,resultDict);
    }
  }
  return totalSize;
}

//----------------------------------------------------------------------------

void readFormulaRepository()
{
  QFile f(Config::htmlOutputDir+"/formula.repository");
  if (f.open(IO_ReadOnly)) // open repository
  {
    QTextStream t(&f);
    QCString line;
    while (!t.eof())
    {
      line=t.readLine();
      int se=line.find(':'); // find name and text separator.
      if (se==-1)
      {
        warn("Warning: formula.repository is corrupted!\n");
        break;
      }
      else
      {
        QCString formName = line.left(se);
        QCString formText = line.right(line.length()-se-1); 
        Formula *f=new Formula(formText);
        formulaList.append(f);
        formulaDict.insert(formText,f);
        formulaNameDict.insert(formName,f);
      }
    }
  }
}

//----------------------------------------------------------------------------
// print the usage of doxygen

void usage(const char *name)
{
  msg("Doxygen version %s\nCopyright Dimitri van Heesch 1997-1999\n\n",versionString);
  msg("You can use doxygen in two ways:\n\n");
  msg("1) Use doxygen to generate a template configuration file:\n");
  msg("    %s [-s] -g [configName]\n\n",name);
  msg("    if -s is specified the comments in the config file will be omitted.\n\n");
  msg("2) Use doxygen to generate documentation using an existing ");
  msg("configuration file:\n");
  msg("    %s [configName]\n\n",name);
  msg("If configName is omitted `Doxyfile' will be used as a default.\n\n");
  exit(1);
}

//----------------------------------------------------------------------------
// read the argument of option `c' from the comment argument list and
// update the option index `optind'.

const char *getArg(int argc,char **argv,int &optind)
{
  char *s=0;
  if (strlen(&argv[optind][2])>0)
    s=&argv[optind][2];
  else if (optind+1<argc)
    s=argv[++optind];
  return s;
}

//----------------------------------------------------------------------------

int main(int argc,char **argv)
{

  initPreprocessor();
  
  /**************************************************************************
   *             Handle arguments                                           *
   **************************************************************************/

  char *s;
  int optind=1;
  const char *configName=0;
  const char *debugLabel;
  bool genConfig=FALSE;
  bool shortList=FALSE;
  while (optind<argc && argv[optind][0]=='-')
  {
    switch(argv[optind][1])
    {
      case 'g':
        genConfig=TRUE;
        configName=getArg(argc,argv,optind);
        if (!configName) configName="Doxyfile";
        break;
      case 'd':
        debugLabel=getArg(argc,argv,optind);
        Debug::setFlag(debugLabel);
        break;
      case 's':
        shortList=TRUE;
        break;
      case 'h':
      case '?':
        usage(argv[0]);
        break;
      default:
        err("Unknown option -%c\n",argv[optind][1]);
        usage(argv[0]);
    }
    optind++;
  }
  
  /**************************************************************************
   *            Parse or generate the config file                           *
   **************************************************************************/

  if (genConfig)
  {
    generateConfigFile(configName,shortList);
    exit(1);
  }
  
  QFileInfo configFileInfo1("Doxyfile"),configFileInfo2("doxyfile");
  QCString config;
  if (optind>=argc)
  { 
    if (configFileInfo1.exists()) 
      config=fileToString("Doxyfile");
    else if (configFileInfo2.exists())
    {
      config=fileToString("doxyfile");
    }
    else
    {
      err("Doxyfile not found and no input file specified!\n");
      usage(argv[0]);
    }
  }
  else
    config=fileToString(argv[1]);

  parseConfig(config); 
  checkConfig();
  
  /**************************************************************************
   *            Initialize output generators                                *
   **************************************************************************/

  outputList = new OutputList(TRUE);
  if (Config::generateHtml)  
  {
    outputList->add(new HtmlGenerator);
    HtmlGenerator::init();
  }
  if (Config::generateLatex) 
  {
    outputList->add(new LatexGenerator);
    LatexGenerator::init();
  }
  if (Config::generateMan)
  {
    outputList->add(new ManGenerator);
    ManGenerator::init();
  }
  
  /**************************************************************************
   *             Read and preprocess input                                  *
   **************************************************************************/

  // gather names of all files in the include path
  msg("Searching for include files...\n");
  s=Config::includePath.first();
  while (s)
  {
    readFileOrDirectory(s,0,&includeNameDict,0,&Config::filePatternList,
                        &Config::excludePatternList,0,0);
    s=Config::includePath.next(); 
  }
  
  msg("Searching for example files...\n");
  s=Config::examplePath.first();
  while (s)
  {
    readFileOrDirectory(s,0,&exampleNameDict,0,&Config::filePatternList,
                        &Config::excludePatternList,0,0);
    s=Config::examplePath.next(); 
  }

  msg("Searching for files to exclude\n");
  s=Config::excludeSources.first();
  while (s)
  {
    readFileOrDirectory(s,0,0,0,&Config::filePatternList,
                        0,0,&excludeNameDict);
    s=Config::excludeSources.next();
  }

  msg("Reading input files...\n");
  int inputSize=0;
  s=Config::inputSources.first();
  while (s)
  {
    inputSize+=readFileOrDirectory(s,&inputNameList,
                                   &inputNameDict,&excludeNameDict,
                                   &Config::filePatternList,
                                   &Config::excludePatternList,
                                   &inputFiles,0);
    s=Config::inputSources.next();
  }
  //msg("Input size %d bytes\n",inputSize);
  
  BufStr input(inputSize+1); // Add one byte extra for \0 termination
  readFiles(input);

  if (input.length()==0)
  {
    warn("No input read, no output generated!\n");
    exit(1);
  }
  else
  {
    msg("Read %d bytes\n",input.length());
  }

  /**************************************************************************
   *             Handle Tag Files                                           *
   **************************************************************************/

  msg("Reading tag files\n");
  
  s=Config::tagFileList.first();
  while (s)
  {
    readTagFile(s);
    s=Config::tagFileList.next();
  }
  
  QFile *tag =new QFile(Config::genTagFile);
  if (Config::genTagFile.length()>0)
  {
    if (!tag->open(IO_WriteOnly))
    {
      err("Error: cannot open tag file %s for writing\n",
          Config::genTagFile.data()
         );
      exit(1);
    }
    tagFile.setDevice(tag);
  }
  
  /**************************************************************************
   *             Gather information                                         * 
   **************************************************************************/
  
  // Notice: the order of the function calls below is very important!
  
  if (Config::generateHtml)
  {
    msg("Reading formula repository...\n");
    readFormulaRepository();
  }
  
  Entry *root=new Entry;
  root->program=input;

  msg("Parsing input...\n");
  parseMain(root);            // build a tree of entries

  msg("Freeing input...\n");
  input.resize(0);
  
  msg("Building namespace list...\n");
  buildNamespaceList(root);
  
  msg("Building group list...\n");
  buildGroupList(root);

  //msg("Computing group relations...\n");
  //computeGroupRelations(root); 
  
  msg("Building file list...\n");
  buildFileList(root);
  
  msg("Building class list...\n");
  buildClassList(root);
  
  msg("Building example list...\n");
  buildExampleList(root);
  
  msg("Building page list...\n");
  buildPageList(root);
  
//  msg("Adding compounds to file pages...\n");
//  findClassDefsInFiles(root);

  msg("Building member list...\n"); // using class info only !
  buildMemberList(root);
  transferFunctionDocumentation();

  msg("Searching for friends...\n");
  findFriends();
  
  msg("Searching for documented variables...\n");
  buildVarList(root);
  
  msg("Searching for documented defines...\n");
  findDefineDocumentation(root); 
  
  msg("Computing class relations...\n");
  computeClassRelations(root);

  msg("Searching for enumerations...\n");
  findEnums(root);
  findEnumDocumentation(root);
//  msg("Searching for function prototypes...\n");
//  findPrototypes(root); // may introduce new members !
  
  msg("Searching for member function documentation...\n");
  findMemberDocumentation(root); // may introduce new members !
  

  msg("Freeing entry tree\n");
  delete root;
  
  msg("Determining which enums are documented\n");
  findDocumentedEnumValues();

  msg("Computing member references...\n");
  computeMemberReferences(); 

  msg("Computing function references...\n");
  computeFunctionReferences();
  
  msg("Computing member relations...\n");
  computeMemberRelations();

  msg("Building full member lists recursively...\n");
  buildCompleteMemberLists();
  
  //unrelatedFunctionsUsed=hasUnrelatedFunctions();
  
  /**************************************************************************
   *                        Generate documentation                          *
   **************************************************************************/

  // count the number of documented elements in the lists we have built. 
  // If the result is 0 we do not generate the lists and omit the 
  // corresponding links in the index.
  msg("Counting data structures...\n");
  annotatedClasses           = countAnnotatedClasses();
  hierarchyClasses           = countClassHierarchy();
  documentedMembers          = countClassMembers();
  documentedFunctions        = countFileMembers();
  documentedFiles            = countFiles();
  documentedGroups           = countGroups();
  documentedNamespaces       = countNamespaces();
  documentedNamespaceMembers = countNamespaceMembers();
 
  // compute the shortest possible names of all files
  // without loosing the uniqueness of the file names.
  msg("Generating disk names...\n");
  inputNameList.generateDiskNames();
  
  msg("Resolving user defined references...\n");
  resolveUserReferences();

  msg("Generating example documentation...\n");
  generateExampleDocs();

  msg("Generating file documentation...\n");
  generateFileDocs();
  
  msg("Generating class documentation...\n");
  generateClassDocs();
  
  msg("Generating page documentation...\n");
  generatePageDocs();
  
  msg("Generating group documentation...\n");
  generateGroupDocs();

  msg("Generating namespace index...\n");
  generateNamespaceDocs();
  
  msg("Generating group index...\n");
  writeGroupIndex(*outputList);
  
  msg("Generating example index...\n");
  writeExampleIndex(*outputList);
  
  msg("Generating file member index...\n");
  writeFileMemberIndex(*outputList);
  
  msg("Generating namespace member index...\n");
  writeNamespaceMemberIndex(*outputList);

//  msg("Generating define index...\n");
//  writeDefineIndex(*outputList);
  
  msg("Generating page index...\n");
  writePageIndex(*outputList);
  
  msg("Generating search index...\n");
  generateSearchIndex();
  
  msg("Generating style sheet...\n");
  outputList->writeStyleInfo(0); // write first part
  outputList->disableAllBut(OutputGenerator::Latex);
  parseText(*outputList,
            theTranslator->trGeneratedAt(dateToString(TRUE),Config::projectName)
          );
  outputList->writeStyleInfo(1); // write second part
  parseText(*outputList,theTranslator->trWrittenBy());
  outputList->writeStyleInfo(2); // write third part
  parseText(*outputList,
            theTranslator->trGeneratedAt(dateToString(TRUE),Config::projectName)
          );
  outputList->writeStyleInfo(3); // write fourth part
  parseText(*outputList,theTranslator->trWrittenBy());
  outputList->writeStyleInfo(4); // write last part
  outputList->enableAll();
  
  if (formulaList.count()>0 && Config::generateHtml)
  {
    msg("Generating bitmaps for formulas in HTML...\n");
    formulaList.generateBitmaps(Config::htmlOutputDir);
  }
  
  if (Config::searchEngineFlag || Config::tagFileList.count()>0)
  {
    msg("\nNow copy the file\n\n     %s\n\nto the directory where the CGI binaries are "
        "located and don't forget to run\n\n",(Config::htmlOutputDir+"/"+Config::cgiName).data());
    msg("     %s/installdox\n\nto replace any dummy links.\n\n",
        Config::htmlOutputDir.data());
  }
  
  delete tag;
  return 0;
}
