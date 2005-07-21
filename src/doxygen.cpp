/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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

#include "qtbc.h"
#include <qfileinfo.h>
#include <qfile.h>
#include <qdir.h>
#include <qdict.h>
#include <qregexp.h>
#include <qstrlist.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <qtextcodec.h>

#include "version.h"
#include "doxygen.h"
#include "scanner.h"
#include "entry.h"
#include "index.h"
#include "logos.h"
#include "instdox.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "pre.h"
#include "tagreader.h"
#include "dot.h"
#include "docparser.h"
#include "dirdef.h"

#include "outputlist.h"
#include "declinfo.h"
#include "htmlgen.h"
#include "latexgen.h"
#include "mangen.h"
#include "language.h"
#include "debug.h"
#include "htmlhelp.h"
#include "ftvhelp.h"
#include "defargs.h"
#include "rtfgen.h"
#include "xmlgen.h"
#include "defgen.h"
#include "perlmodgen.h"
#include "reflist.h"
#include "pagedef.h"
#include "bufstr.h"
#include "commentcnv.h"
#include "cmdmapper.h"
#include "searchindex.h"
#include "parserintf.h"
#include "htags.h"
#include "pyscanner.h"

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif

static QDict<Entry> classEntries(1009);

ClassSDict     Doxygen::classSDict(1009);         
ClassSDict     Doxygen::hiddenClasses(257);

NamespaceSDict  Doxygen::namespaceSDict(20);      

MemberNameSDict Doxygen::memberNameSDict(10000);   

MemberNameSDict Doxygen::functionNameSDict(10000);   

FileNameList   Doxygen::inputNameList;           // all input files
FileNameDict  *Doxygen::inputNameDict;          

GroupSDict      Doxygen::groupSDict(17);          

FormulaList    Doxygen::formulaList;             // all formulas
FormulaDict    Doxygen::formulaDict(1009);       // all formulas
FormulaDict    Doxygen::formulaNameDict(1009);   // the label name of all formulas

PageSDict      *Doxygen::pageSDict = new PageSDict(1009);          // all doc pages
PageSDict      *Doxygen::exampleSDict = new PageSDict(1009);       // all examples
SectionDict    Doxygen::sectionDict(257);        // all page sections
StringDict     Doxygen::aliasDict(257);          // aliases
FileNameDict   *Doxygen::includeNameDict;        // include names
FileNameDict   *Doxygen::exampleNameDict;        // examples
FileNameDict   *Doxygen::imageNameDict;          // images
FileNameDict   *Doxygen::dotFileNameDict;        // dot files
StringDict     Doxygen::namespaceAliasDict(257); // all namespace aliases
StringDict     Doxygen::tagDestinationDict(257); // all tag locations
                                        // a member group
QDict<void>    Doxygen::expandAsDefinedDict(257); // all macros that should be expanded

QIntDict<MemberGroupInfo> Doxygen::memGrpInfoDict(1009); // dictionary of the member groups heading

PageDef       *Doxygen::mainPage = 0;           
bool           Doxygen::insideMainPage = FALSE; // are we generating docs for the main page?
QTextStream    Doxygen::tagFile;
NamespaceDef  *Doxygen::globalScope;
  
QDict<RefList> *Doxygen::xrefLists = new QDict<RefList>; // dictionary of cross-referenced item lists

bool           Doxygen::parseSourcesNeeded = FALSE;
double         Doxygen::sysElapsedTime = 0.0;
QTime          Doxygen::runningTime;
SearchIndex *  Doxygen::searchIndex=0;
SDict<DefinitionList> *Doxygen::symbolMap;
bool           Doxygen::outputToWizard=FALSE;
QDict<int> *   Doxygen::htmlDirMap = 0;
QCache<LookupInfo> Doxygen::lookupCache(20000,20000);
DirSDict       Doxygen::directories(17);
SDict<DirRelation> Doxygen::dirRelations(257);
ParserManager *Doxygen::parserManager = 0;

static StringList     inputFiles;         
static StringDict     excludeNameDict(1009);   // sections
static QDict<void>    compoundKeywordDict(7);  // keywords recognised as compounds
static OutputList     *outputList = 0;         // list of output generating objects


void clearAll()
{
  inputFiles.clear();      
  excludeNameDict.clear();  
  delete outputList; outputList=0;

  Doxygen::classSDict.clear();       
  Doxygen::namespaceSDict.clear();   
  Doxygen::pageSDict->clear();         
  Doxygen::exampleSDict->clear();      
  Doxygen::inputNameList.clear();   
  Doxygen::formulaList.clear();     
  Doxygen::classSDict.clear();        
  Doxygen::sectionDict.clear();       
  Doxygen::inputNameDict->clear();    
  Doxygen::includeNameDict->clear();  
  Doxygen::exampleNameDict->clear();  
  Doxygen::imageNameDict->clear();     
  Doxygen::dotFileNameDict->clear();     
  Doxygen::formulaDict.clear();      
  Doxygen::formulaNameDict.clear();  
  Doxygen::tagDestinationDict.clear();
  delete Doxygen::mainPage; Doxygen::mainPage=0;
}

void statistics()
{
  fprintf(stderr,"--- inputNameDict stats ----\n");
  Doxygen::inputNameDict->statistics();
  fprintf(stderr,"--- includeNameDict stats ----\n");
  Doxygen::includeNameDict->statistics();
  fprintf(stderr,"--- exampleNameDict stats ----\n");
  Doxygen::exampleNameDict->statistics();
  fprintf(stderr,"--- imageNameDict stats ----\n");
  Doxygen::imageNameDict->statistics();
  fprintf(stderr,"--- dotFileNameDict stats ----\n");
  Doxygen::dotFileNameDict->statistics();
  fprintf(stderr,"--- excludeNameDict stats ----\n");
  excludeNameDict.statistics();
  fprintf(stderr,"--- aliasDict stats ----\n");
  Doxygen::aliasDict.statistics();
  fprintf(stderr,"--- typedefDict stats ----\n");
  fprintf(stderr,"--- namespaceAliasDict stats ----\n");
  Doxygen::namespaceAliasDict.statistics();
  fprintf(stderr,"--- formulaDict stats ----\n");
  Doxygen::formulaDict.statistics();
  fprintf(stderr,"--- formulaNameDict stats ----\n");
  Doxygen::formulaNameDict.statistics();
  fprintf(stderr,"--- tagDestinationDict stats ----\n");
  Doxygen::tagDestinationDict.statistics();
  fprintf(stderr,"--- compoundKeywordDict stats ----\n");
  compoundKeywordDict.statistics();
  fprintf(stderr,"--- expandAsDefinedDict stats ----\n");
  Doxygen::expandAsDefinedDict.statistics();
  fprintf(stderr,"--- memGrpInfoDict stats ----\n");
  Doxygen::memGrpInfoDict.statistics();
}



static void addMemberDocs(Entry *root,MemberDef *md, const char *funcDecl,
                   ArgumentList *al,bool over_load,NamespaceSDict *nl=0);
static void findMember(Entry *root,
                       QCString funcDecl,
                       bool overloaded,
                       bool isFunc
                      );

const char idMask[] = "[A-Za-z_][A-Za-z_0-9]*";
QCString spaces;
QCString Doxygen::htmlFileExtension;

//----------------------------------------------------------------------------

static void addRelatedPage(Entry *root)
{
  GroupDef *gd=0;
  QListIterator<Grouping> gli(*root->groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict[g->groupname])) break;
  }
  //printf("---> addRelatedPage() %s gd=%p\n",root->name.data(),gd);
  QCString doc;
  if (root->brief.isEmpty())
  {
    doc=root->doc;
  }
  else
  {
    doc=root->brief+"\n\n"+root->doc;
  }
  PageDef *pd = addRelatedPage(root->name,root->args,doc,root->anchors,
      root->fileName,root->startLine,
      root->sli,
      gd,root->tagInfo
     );
  if (pd)
  {
#if 0
    Definition *ctx = 0;

    // find the page's context
    if (root->parent->section & Entry::COMPOUND_MASK ) // inside class
    {
       QCString fullName=removeRedundantWhiteSpace(root->parent->name);
       fullName=stripAnonymousNamespaceScope(fullName);
       fullName=stripTemplateSpecifiersFromScope(fullName);
       ctx=getClass(fullName);
    }
    if (ctx==0 && root->parent->section == Entry::NAMESPACE_SEC ) // inside namespace
    {
      QCString nscope=removeAnonymousScopes(root->parent->name);
      if (!nscope.isEmpty())
      {
        ctx = getResolvedNamespace(nscope);
      }
    }
    if (ctx==0) // inside file
    {
      bool ambig;
      ctx=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
    }
    pd->setOuterScope(ctx);
#endif
    pd->addSectionsToDefinition(root->anchors);
    //pi->context = ctx;
  }
}

static void buildGroupListFiltered(Entry *root,bool additional)
{
  if (root->section==Entry::GROUPDOC_SEC && !root->name.isEmpty())
  {
    //printf("Found group %s title=`%s type=%d'\n",
    //    root->name.data(),root->type.data(),root->groupDocType);
    
    if ((root->groupDocType==Entry::GROUPDOC_NORMAL && !additional) ||
        (root->groupDocType!=Entry::GROUPDOC_NORMAL && additional))
    {
      GroupDef *gd;

      if ((gd=Doxygen::groupSDict[root->name]))
      {
        if ( root->groupDocType==Entry::GROUPDOC_NORMAL )
        {
          warn(root->fileName,root->startLine,
              "Warning: group %s already documented. "
              "Skipping documentation.",
              root->name.data());
        }
        else
        {
          if ( !gd->hasGroupTitle() )
            gd->setGroupTitle( root->type );
          else if ( root->type.length() > 0 && root->name != root->type && gd->groupTitle() != root->type )
            warn( root->fileName,root->startLine,
                "group %s: ignoring title \"%s\" that does not match old title \"%s\"\n",
                root->name.data(), root->type.data(), gd->groupTitle() );
          if ( gd->briefDescription().isEmpty() )
            gd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          if ( !root->doc.stripWhiteSpace().isEmpty() )
            gd->setDocumentation( gd->documentation().isEmpty() ? root->doc :
                gd->documentation() + "\n\n" + root->doc,
                root->docFile, root->docLine );
          gd->addSectionsToDefinition(root->anchors);
          gd->setRefItems(root->sli);
          //addGroupToGroups(root,gd);
        }
      }
      else
      {
        if (root->tagInfo)
        {
          gd = new GroupDef(root->fileName,root->startLine,root->name,root->type,root->tagInfo->fileName);
          gd->setReference(root->tagInfo->tagName);
        }
        else
        {
          gd = new GroupDef(root->fileName,root->startLine,root->name,root->type);
        }
        gd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        gd->setDocumentation(root->doc,root->docFile,root->docLine);
        gd->addSectionsToDefinition(root->anchors);
        Doxygen::groupSDict.append(root->name,gd);
        gd->setRefItems(root->sli);
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildGroupListFiltered(e,additional);
  }
}

static void buildGroupList(Entry *root)
{
  // first process the @defgroups blocks
  buildGroupListFiltered(root,FALSE);
  // then process the @addtogroup, @weakgroup blocks
  buildGroupListFiltered(root,TRUE);
}

static void organizeSubGroupsFiltered(Entry *root,bool additional)
{
  if (root->section==Entry::GROUPDOC_SEC && !root->name.isEmpty())
  {
    if ((root->groupDocType==Entry::GROUPDOC_NORMAL && !additional) ||
        (root->groupDocType!=Entry::GROUPDOC_NORMAL && additional))
    {
      GroupDef *gd;
      if ((gd=Doxygen::groupSDict[root->name]))
      {
        //printf("adding %s to group %s\n",root->name.data(),gd->name().data());
        addGroupToGroups(root,gd);
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    organizeSubGroupsFiltered(e,additional);
  }
}

static void organizeSubGroups(Entry *root)
{
  //printf("Defining groups\n");
  // first process the @defgroups blocks
  organizeSubGroupsFiltered(root,FALSE);
  //printf("Additional groups\n");
  // then process the @addtogroup, @weakgroup blocks
  organizeSubGroupsFiltered(root,TRUE);
}

//----------------------------------------------------------------------

static void buildFileList(Entry *root)
{
  if (((root->section==Entry::FILEDOC_SEC) ||
        ((root->section & Entry::FILE_MASK) && Config_getBool("EXTRACT_ALL"))) &&
      !root->name.isEmpty() && !root->tagInfo // skip any file coming from tag files
     )
  {
    bool ambig;
    FileDef *fd=findFileDef(Doxygen::inputNameDict,root->name,ambig);
    //printf("**************** root->name=%s fd=%p\n",root->name.data(),fd);
    if (fd && !ambig)
    {
      if ((!root->doc.isEmpty() && !fd->documentation().isEmpty()) ||
          (!root->brief.isEmpty() && !fd->briefDescription().isEmpty()))
      {
        warn(
            root->fileName,root->startLine,
            "Warning: file %s already documented. "
            "Skipping documentation.",
            root->name.data()
            );
      }
      else
      {
        //printf("Adding documentation!\n");
        // using FALSE in setDocumentation is small hack to make sure a file 
        // is documented even if a \file command is used without further 
        // documentation
        fd->setDocumentation(root->doc,root->docFile,root->docLine,FALSE);
        fd->setBriefDescription(root->brief,root->briefFile,root->briefLine); 
        fd->addSectionsToDefinition(root->anchors);
        fd->setRefItems(root->sli);
        QListIterator<Grouping> gli(*root->groups);
        Grouping *g;
        for (;(g=gli.current());++gli)
        {
          GroupDef *gd=0;
          if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict[g->groupname]))
          {
            gd->addFile(fd);
            //printf("File %s: in group %s\n",fd->name().data(),s->data());
          }
        }
      }
    }
    else
    {
      const char *fn = root->fileName.data();
      QCString text;
      text.sprintf("Warning: the name `%s' supplied as "
          "the second argument in the \\file statement ",
          root->name.data()
                  );
      if (ambig) // name is ambigious
      {
        text+="matches the following input files:\n";
        text+=showFileDefMatches(Doxygen::inputNameDict,root->name);
        text+="Please use a more specific name by "
          "including a (larger) part of the path!";
      }
      else // name is not an input file
      {
        text+="is not an input file";
      }
      warn(fn,root->startLine,text);
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildFileList(e);
  }
}

static void addIncludeFile(ClassDef *cd,FileDef *ifd,Entry *root)
{
  if ( 
      (!root->doc.stripWhiteSpace().isEmpty() || 
       !root->brief.stripWhiteSpace().isEmpty() || 
       Config_getBool("EXTRACT_ALL")
      ) && root->protection!=Private
     )
  { 
    //printf(">>>>>> includeFile=%s\n",root->includeFile.data());

    bool ambig;
    FileDef *fd=0;
    // see if we need to include a verbatim copy of the header file
    //printf("root->includeFile=%s\n",root->includeFile.data());
    if (!root->includeFile.isEmpty() && 
        (fd=findFileDef(Doxygen::inputNameDict,root->includeFile,ambig))==0
       )
    { // explicit request
      QCString text;
      text.sprintf("Warning: the name `%s' supplied as "
                  "the second argument in the \\class statement ",
                  root->includeFile.data()
                 );
      if (ambig) // name is ambigious
      {
        text+="matches the following input files:\n";
        text+=showFileDefMatches(Doxygen::inputNameDict,root->includeFile);
        text+="Please use a more specific name by "
            "including a (larger) part of the path!";
      }
      else // name is not an input file
      {
        text+="is not an input file";
      }
      warn(root->fileName,root->startLine,text);
    }
    else if (root->includeFile.isEmpty() && ifd &&
        // see if the file extension makes sense
        guessSection(ifd->name())==Entry::HEADER_SEC)
    { // implicit assumption
      fd=ifd;
    }

    // if a file is found, we mark it as a source file.
    if (fd)
    {
      QCString iName = !root->includeName.isEmpty() ? 
                       root->includeName.data() : root->includeFile.data();
      bool local=FALSE;
      if (!iName.isEmpty()) // user specified include file
      {
        local = iName.at(0)=='"'; // is it a local include file
        if (local || iName.at(0)=='<')
        {
          iName=iName.mid(1,iName.length()-2); // strip quotes or brackets
        }
      }
      else if (!Config_getList("STRIP_FROM_INC_PATH").isEmpty()) 
      {
        iName=stripFromIncludePath(fd->absFilePath());
      }
      else // use name of the file containing the class definition
      {
        iName=fd->name();
      }
      if (fd->generateSourceFile()) // generate code for header
      {
        cd->setIncludeFile(fd,iName,local,!root->includeName.isEmpty());
      }
      else // put #include in the class documentation without link
      {
        cd->setIncludeFile(0,iName,local,TRUE);
      }
    }
  }
}

#if 0
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
        QCString nsName = stripAnonymousNamespaceScope(e->name);
        //printf("addNameSpace() trying: %s\n",nsName.data());
        if (!nsName.isEmpty() && nsName.at(0)!='@' &&
            (nd=getResolvedNamespace(nsName))
           )
        {
          cd->setNamespace(nd);
          cd->setOuterScope(nd);
          nd->insertClass(cd);
          return TRUE;
        }
      }
      e=e->parent;
    } 
  }
  return FALSE;
}
#endif

static Definition *findScope(Entry *root,int level=0)
{
  if (root==0) return 0;
  //printf("start findScope name=%s\n",root->name.data());
  Definition *result=0;
  if (root->section&Entry::SCOPE_MASK)
  {
    result = findScope(root->parent,level+1); // traverse to the root of the tree
    if (result)
    {
      //printf("Found %s inside %s at level %d\n",root->name.data(),result->name().data(),level);
      // TODO: look at template arguments
      result = result->findInnerCompound(root->name);
    }
    else // reached the global scope
    {
      // TODO: look at template arguments
      result = Doxygen::globalScope->findInnerCompound(root->name);
      //printf("Found in globalScope %s at level %d\n",result->name().data(),level);
    }
  }
  //printf("end findScope(%s,%d)=%s\n",root->name.data(),
  //       level,result==0 ? "<none>" : result->name().data());
  return result;
}

static Definition *findScopeFromQualifiedName(Definition *startScope,const QCString &n)
{
  //printf("findScopeFromQualifiedName(%s,%s)\n",startScope ? startScope->name().data() : 0, n.data());
  Definition *resultScope=startScope;
  if (resultScope==0) resultScope=Doxygen::globalScope;
  QCString scope=stripTemplateSpecifiersFromScope(n,FALSE);
  int l1=0,i1;
  i1=getScopeFragment(scope,0,&l1);
  if (i1==-1) return resultScope;
  int p=i1+l1,l2=0,i2;
  while ((i2=getScopeFragment(scope,p,&l2))!=-1)
  {
    QCString nestedNameSpecifier = scope.mid(i1,l1);
    //Definition *oldScope = resultScope;
    resultScope = resultScope->findInnerCompound(nestedNameSpecifier);
    if (resultScope==0) 
    {
      //printf("name %s not found in scope %s\n",nestedNameSpecifier.data(),oldScope->name().data());
      return 0;
    }
    i1=i2;
    l1=l2;
    p=i2+l2;
  }
  //printf("scope %s\n",resultScope->name().data());
  return resultScope;
}

ArgumentList *getTemplateArgumentsFromName(
                  const QCString &name,
                  const QList<ArgumentList> *tArgLists)
{
  if (tArgLists==0) return 0;
  
  QListIterator<ArgumentList> ali(*tArgLists);
  // for each scope fragment, check if it is a template and advance through
  // the list if so.
  int i,p=0;
  while ((i=name.find("::",p))!=-1)
  {
    NamespaceDef *nd = Doxygen::namespaceSDict[name.left(i)];
    if (nd==0)
    {
      ClassDef *cd = getClass(name.left(i));
      if (cd)
      {
        if (cd->templateArguments())
        {
          ++ali;
        }
      }
    }
    p=i+2;
  }
  return ali.current();
}

static ClassDef::CompoundType convertToCompoundType(int section)
{
    ClassDef::CompoundType sec=ClassDef::Class; 
    switch(section)
    {
      case Entry::UNION_SEC: 
      case Entry::UNIONDOC_SEC: 
        sec=ClassDef::Union; 
        break;
      case Entry::STRUCT_SEC:
      case Entry::STRUCTDOC_SEC: 
        sec=ClassDef::Struct; 
        break;
      case Entry::INTERFACE_SEC:
      case Entry::INTERFACEDOC_SEC:
        sec=ClassDef::Interface; 
        break;
      case Entry::PROTOCOL_SEC:
      case Entry::PROTOCOLDOC_SEC:
        sec=ClassDef::Protocol; 
        break;
      case Entry::CATEGORY_SEC:
      case Entry::CATEGORYDOC_SEC:
        sec=ClassDef::Category; 
        break;
      case Entry::EXCEPTION_SEC:
      case Entry::EXCEPTIONDOC_SEC:
        sec=ClassDef::Exception; 
        break;
    }
    return sec;
}


static void addClassToContext(Entry *root)
{
//  QCString fullName=removeRedundantWhiteSpace(root->name);
//
//  if (fullName.isEmpty())
//  {
//    // this should not be called
//    warn(root->fileName,root->startLine,
//        "Warning: invalid class name found!"
//        );
//    return;
//  }
//  Debug::print(Debug::Classes,0,"  Found class with raw name %s\n",fullName.data());
//
//  fullName=stripAnonymousNamespaceScope(fullName);
//  fullName=stripTemplateSpecifiersFromScope(fullName);
//
//  Debug::print(Debug::Classes,0,"  Found class with name %s\n",fullName.data());

  bool ambig;

  //NamespaceDef *nd = 0;
  FileDef      *fd = findFileDef(Doxygen::inputNameDict,root->fileName,ambig);

  QCString scName;
  if (root->parent->section&Entry::SCOPE_MASK)
  {
     scName=root->parent->name;
  }
  // name without parent's scope
  QCString fullName = root->name;

  // strip off any template parameters (but not those for specializations)
  fullName=stripTemplateSpecifiersFromScope(fullName);

  // name with scope
  QCString qualifiedName = scName.isEmpty() ? fullName : scName+"::"+fullName;

  ClassDef *cd = getClass(qualifiedName);
  
  Debug::print(Debug::Classes,0, "  Found class with name %s (qualifiedName=%s -> cd=%p)\n",
      cd ? cd->name().data() : root->name.data(), qualifiedName.data(),cd);
  
  if (cd) 
  {
    fullName=cd->name();
    Debug::print(Debug::Classes,0,"  Existing class %s!\n",cd->name().data());
    //if (cd->templateArguments()==0)
    //{
    //  //printf("existing ClassDef tempArgList=%p specScope=%s\n",root->tArgList,root->scopeSpec.data());
    //  cd->setTemplateArguments(tArgList);
    //}
    if (!root->doc.isEmpty() || !root->brief.isEmpty() || 
        (root->bodyLine!=-1 && Config_getBool("SOURCE_BROWSER"))
       ) 
      // block contains something that ends up in the docs
    { 
      if (!root->doc.isEmpty() && !cd->documentation().isEmpty())
      {
        warn(
            root->fileName,root->startLine,
            "Warning: class %s already has a detailed description. "
            "Skipping the one found here.",
            fullName.data()
            );
      }
      else if (!root->doc.isEmpty())
      {
        cd->setDocumentation(root->doc,root->docFile,root->docLine);
      }
      if (!root->brief.isEmpty() && !cd->briefDescription().isEmpty())
      {
        warn(
            root->fileName,root->startLine,
            "Warning: class %s already has a brief description\n"
            "         skipping the one found here.",
            fullName.data()
            );
      }
      else if (!root->brief.isEmpty())
      {
        cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      }
      if (root->bodyLine!=-1 && cd->getStartBodyLine()==-1)
      {
        cd->setBodySegment(root->bodyLine,root->endBodyLine);
        cd->setBodyDef(fd);
      }
      //cd->setName(fullName); // change name to match docs
    }

    if (cd->templateArguments()==0) 
    {
      // this happens if a template class declared with @class is found
      // before the actual definition.
      ArgumentList *tArgList = 
        getTemplateArgumentsFromName(cd->name(),root->tArgLists);
      cd->setTemplateArguments(tArgList);
    }

    cd->setCompoundType(convertToCompoundType(root->section));
  }
  else // new class
  {
    ClassDef::CompoundType sec = convertToCompoundType(root->section);
    Debug::print(Debug::Classes,0,"  New class `%s' (sec=0x%08x)! #tArgLists=%d\n",
        fullName.data(),root->section,root->tArgLists ? (int)root->tArgLists->count() : -1);
    QCString className;
    QCString namespaceName;
    extractNamespaceName(fullName,className,namespaceName);

    //printf("New class: namespace `%s' name=`%s' brief=`%s' docs=`%s'\n",
    //    className.data(),namespaceName.data(),root->brief.data(),root->doc.data());

    QCString tagName;
    QCString refFileName;
    if (root->tagInfo)
    {
      tagName     = root->tagInfo->tagName;
      refFileName = root->tagInfo->fileName;
    }
    cd=new ClassDef(root->fileName,root->startLine,fullName,sec,
        tagName,refFileName);
    cd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
    cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
    cd->setIsObjectiveC(root->objc);
    //printf("new ClassDef %s tempArgList=%p specScope=%s\n",fullName.data(),root->tArgList,root->scopeSpec.data());

    ArgumentList *tArgList = 
      getTemplateArgumentsFromName(fullName,root->tArgLists);
    //printf("class %s template args=%s\n",fullName.data(),
    //    tArgList ? tempArgListToString(tArgList).data() : "<none>");
    cd->setTemplateArguments(tArgList);
    cd->setProtection(root->protection);
    cd->setIsStatic(root->stat);

    // file definition containing the class cd
    cd->setBodySegment(root->bodyLine,root->endBodyLine);
    cd->setBodyDef(fd);

    // see if the class is found inside a namespace 
    //bool found=addNamespace(root,cd);


    // the empty string test is needed for extract all case
    cd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
    cd->insertUsedFile(root->fileName);


    // add class to the list
    //printf("ClassDict.insert(%s)\n",resolveDefines(fullName).data());
    Doxygen::classSDict.append(fullName,cd);

  }

  cd->addSectionsToDefinition(root->anchors);
  if (!root->subGrouping) cd->setSubGrouping(FALSE);
  if (cd->hasDocumentation())
  {
    addIncludeFile(cd,fd,root);
  }
  if (fd && (root->section & Entry::COMPOUND_MASK)) 
  {
    //printf(">> Inserting class `%s' in file `%s' (root->fileName=`%s')\n",
    //    cd->name().data(),
    //    fd->name().data(),
    //    root->fileName.data()
    //   );
    cd->setFileDef(fd);
    fd->insertClass(cd);
  }
  addClassToGroups(root,cd);
  cd->setRefItems(root->sli);
}
            
//----------------------------------------------------------------------
// build a list of all classes mentioned in the documentation
// and all classes that have a documentation block before their definition.
static void buildClassList(Entry *root)
{
  if (
        ((root->section & Entry::COMPOUND_MASK) || 
         root->section==Entry::OBJCIMPL_SEC) && !root->name.isEmpty()
     )
  {
    addClassToContext(root);
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildClassList(e);
  }
}

static void buildClassDocList(Entry *root)
{
  if (
       (root->section & Entry::COMPOUNDDOC_MASK) && !root->name.isEmpty()
     )
  {
    addClassToContext(root);
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildClassDocList(e);
  }
}

Definition *buildScopeFromQualifiedName(const QCString name,int level)
{
  int i=0;
  int p=0,l;
  Definition *prevScope=Doxygen::globalScope;
  QCString fullScope;
  while (i<level)
  {
    int idx=getScopeFragment(name,p,&l);
    QCString nsName = name.mid(idx,l);
    if (!fullScope.isEmpty()) fullScope+="::";
    fullScope+=nsName;
    NamespaceDef *nd=Doxygen::namespaceSDict.find(fullScope);
    if (nd==0)
    {
      // introduce bogus namespace
      //printf("adding dummy namespace %s to %s\n",nsName.data(),prevScope->name().data());
      nd=new NamespaceDef(
        "[generated]",1,fullScope);

      // add namespace to the list
      Doxygen::namespaceSDict.inSort(fullScope,nd);
    }
    prevScope->addInnerCompound(nd);
    nd->setOuterScope(prevScope);
    p=idx+l+2;
    prevScope=nd;
    i++;
  }
  return prevScope;
}

static void resolveClassNestingRelations()
{
  ClassSDict::Iterator cli(Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli) cli.current()->visited=FALSE;

  int nestingLevel=0;
  bool done=FALSE;
  while (!done)
  {
    // iterate over all classes searching for a class with right nesting
    // level (starting with 0 and going up until no more classes are found)
    done=TRUE;
    ClassSDict::Iterator cli(Doxygen::classSDict);
    ClassDef *cd=0;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      //printf("nested relation for class %s\n",cd->name().data());
      if (cd->name().contains("::")==nestingLevel)
      {
        cd->visited=TRUE;
        //printf("Level=%d processing=%s\n",nestingLevel,cd->name().data());
        // also add class to the correct structural context 
        Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,cd->name());
        if (d==0)
        {
          Definition *d = buildScopeFromQualifiedName(cd->name(),cd->name().contains("::"));
          d->addInnerCompound(cd);
          cd->setOuterScope(d);
          //warn(cd->getDefFileName(),cd->getDefLine(),
          //    "Warning: Internal inconsistency: scope for class %s not "
          //    "found!\n",cd->name().data()
          //    );
        }
        else
        {
          //printf("****** adding %s to scope %s\n",cd->name().data(),d->name().data());
          d->addInnerCompound(cd);
          cd->setOuterScope(d);
        }
      }
      if (!cd->visited) done=FALSE;
    }
    nestingLevel++;
    //printf("nestingLevel=%d\n",nestingLevel);
  }
}


//----------------------------------------------------------------------
// build a list of all namespaces mentioned in the documentation
// and all namespaces that have a documentation block before their definition.
static void buildNamespaceList(Entry *root)
{
  if (
       (root->section==Entry::NAMESPACE_SEC ||
        root->section==Entry::NAMESPACEDOC_SEC ||
        root->section==Entry::PACKAGEDOC_SEC
       ) && 
       !root->name.isEmpty()
     )
  {
    QCString fullName = root->name;
    if (root->section==Entry::PACKAGEDOC_SEC)
    {
      fullName=substitute(fullName,".","::");
    }
    
    fullName = stripAnonymousNamespaceScope(fullName);
    if (!fullName.isEmpty())
    {
      //printf("Found namespace %s in %s at line %d\n",root->name.data(),
      //        root->fileName.data(), root->startLine);
      NamespaceDef *nd;
      if ((nd=Doxygen::namespaceSDict[fullName])) // existing namespace
      {
        if (!root->doc.isEmpty() || !root->brief.isEmpty()) // block contains docs
        { 
          if (nd->documentation().isEmpty() && !root->doc.isEmpty())
          {
            nd->setDocumentation(root->doc,root->docFile,root->docLine);
            nd->setName(fullName); // change name to match docs
            nd->addSectionsToDefinition(root->anchors);
          }
          else if (!nd->documentation().isEmpty() && !root->doc.isEmpty())
          {
            warn(
                 root->fileName,root->startLine,
                 "Warning: namespace %s already has a detailed description. "
                 "Skipping the documentation found here.",
                 fullName.data());
          }
          if (nd->briefDescription().isEmpty() && !root->brief.isEmpty())
          {
            nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
            nd->setName(fullName); // change name to match docs
          }
          else if (!nd->briefDescription().isEmpty() && !root->brief.isEmpty())
          {
            warn(root->fileName,root->startLine,
                 "Warning: namespace %s already has a brief description. "
                 "Skipping the documentation found here.",
                 fullName.data()
                );
          }
        }

        bool ambig;
        // file definition containing the namespace nd
        FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
        // insert the namespace in the file definition
        if (fd) fd->insertNamespace(nd);
        addNamespaceToGroups(root,nd);
        nd->setRefItems(root->sli);
      }
      else // fresh namespace
      {
        QCString tagName;
        QCString tagFileName;
        if (root->tagInfo)
        {
          tagName=root->tagInfo->tagName;
          tagFileName=root->tagInfo->fileName;
        }
        NamespaceDef *nd=new NamespaceDef(root->fileName,root->startLine,fullName,tagName,tagFileName);
        nd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
        nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        nd->addSectionsToDefinition(root->anchors);

        //printf("Adding namespace to group\n");
        addNamespaceToGroups(root,nd);
        nd->setRefItems(root->sli);

        bool ambig;
        // file definition containing the namespace nd
        FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
        // insert the namespace in the file definition
        if (fd) fd->insertNamespace(nd);

        // the empty string test is needed for extract all case
        nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        nd->insertUsedFile(root->fileName);
        nd->setBodySegment(root->bodyLine,root->endBodyLine);
        nd->setBodyDef(fd);
        // add class to the list
        Doxygen::namespaceSDict.inSort(fullName,nd);

        // also add namespace to the correct structural context 
        Definition *d = findScopeFromQualifiedName(Doxygen::globalScope,fullName);
        //printf("adding namespace %s to context %s\n",nd->name().data(),d?d->name().data():"none");
        if (d==0)
        {
          Definition *d = buildScopeFromQualifiedName(fullName,fullName.contains("::"));
          d->addInnerCompound(nd);
          nd->setOuterScope(d);
          // TODO: Due to the order in which the tag file is written
          // a nested class can be found before its parent!
        }
        else
        {
          d->addInnerCompound(nd);
          nd->setOuterScope(d);
        }
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

static void findUsingDirectives(Entry *root)
{
  if (root->section==Entry::USINGDIR_SEC)
  {
    //printf("Found using directive %s at line %d of %s\n",
    //    root->name.data(),root->startLine,root->fileName.data());
    QCString name=substitute(root->name,".","::");
    bool ambig;
    if (!name.isEmpty())
    {
      NamespaceDef *usingNd = 0;
      NamespaceDef *nd = 0;
      FileDef      *fd = findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
      QCString nsName;

      // see if the using statement was found inside a namespace or inside
      // the global file scope.
      if (root->parent->section == Entry::NAMESPACE_SEC)
      {
        nsName=stripAnonymousNamespaceScope(root->parent->name);
        if (!nsName.isEmpty())
        {
          nd = getResolvedNamespace(nsName);
        }
      }

      // find the scope in which the `using' namespace is defined by prepending
      // the possible scopes in which the using statement was found, starting
      // with the most inner scope and going to the most outer scope (i.e. 
      // file scope).
      int scopeOffset = nsName.length();
      do
      {
        QCString scope=scopeOffset>0 ? 
                      nsName.left(scopeOffset)+"::" : QCString();
        usingNd = getResolvedNamespace(scope+name);
        //printf("Trying with scope=`%s' usingNd=%p\n",(scope+name).data(),usingNd);
        if (scopeOffset==0)
        {
          scopeOffset=-1;
        }
        else if ((scopeOffset=nsName.findRev("::",scopeOffset-1))==-1)
        {
          scopeOffset=0;
        }
      } while (scopeOffset>=0 && usingNd==0);

      //printf("%s -> %p\n",name.data(),usingNd);

      // add the namespace the correct scope
      if (usingNd)
      {
        //printf("using fd=%p nd=%p\n",fd,nd);
        if (nd)
        {
          //printf("Inside namespace %s\n",nd->name().data());
          nd->addUsingDirective(usingNd);
        }
        else if (fd)
        {
          //printf("Inside file %s\n",fd->name().data());
          fd->addUsingDirective(usingNd);
        }
      }
      else // unknown namespace, but add it anyway.
      {
        NamespaceDef *nd=new NamespaceDef(root->fileName,root->startLine,name);
        nd->setDocumentation(root->doc,root->docFile,root->docLine); // copy docs to definition
        nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        nd->addSectionsToDefinition(root->anchors);

        QListIterator<Grouping> gli(*root->groups);
        Grouping *g;
        for (;(g=gli.current());++gli)
        {
          GroupDef *gd=0;
          if (!g->groupname.isEmpty() && (gd=Doxygen::groupSDict[g->groupname]))
            gd->addNamespace(nd);
        }

        // insert the namespace in the file definition
        if (fd) 
        {
          fd->insertNamespace(nd);
          fd->addUsingDirective(nd);
        }

        // the empty string test is needed for extract all case
        nd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        nd->insertUsedFile(root->fileName);
        // add class to the list
        Doxygen::namespaceSDict.inSort(name,nd);
        nd->setRefItems(root->sli);
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    findUsingDirectives(e);
  }
}

//----------------------------------------------------------------------

static void findUsingDeclarations(Entry *root)
{
  if (root->section==Entry::USINGDECL_SEC &&
      !(root->parent->section&Entry::COMPOUND_MASK) // not a class/struct member
     )
  {
    //printf("Found using declaration %s at line %d of %s inside section %x\n",
    //    root->name.data(),root->startLine,root->fileName.data(),
    //    root->parent->section);
    bool ambig;
    if (!root->name.isEmpty())
    {
      ClassDef *usingCd = 0;
      NamespaceDef *nd = 0;
      FileDef      *fd = findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
      QCString scName;

      // see if the using statement was found inside a namespace or inside
      // the global file scope.
      if (root->parent->section == Entry::NAMESPACE_SEC)
      {
        scName=root->parent->name.copy();
        if (!scName.isEmpty())
        {
          nd = getResolvedNamespace(scName);
        }
      }

      // Assume the using statement was used to import a class.
      // Find the scope in which the `using' namespace is defined by prepending
      // the possible scopes in which the using statement was found, starting
      // with the most inner scope and going to the most outer scope (i.e. 
      // file scope).

      QCString name = substitute(root->name,".","::");
      MemberDef *mtd=0;
      usingCd = getResolvedClass(nd,fd,name,&mtd);

      //printf("%s -> %p\n",root->name.data(),usingCd);
      if (usingCd==0) // definition not in the input => add an artificial class
      {
        Debug::print(Debug::Classes,0,"  New using class `%s' (sec=0x%08x)! #tArgLists=%d\n",
             name.data(),root->section,root->tArgLists ? (int)root->tArgLists->count() : -1);
        usingCd = new ClassDef(
                     "<using>",1,
                     root->name,ClassDef::Class);
        Doxygen::hiddenClasses.append(root->name,usingCd);
        usingCd->setClassIsArtificial();
      }
      else
      {
        Debug::print(Debug::Classes,0,"  Found used class %s in scope=%s\n",
            usingCd->name().data(),nd?nd->name().data():fd->name().data());
      }

      if (mtd) // add the typedef to the correct scope
      {
        if (nd)
        {
          //printf("Inside namespace %s\n",nd->name().data());
          nd->addUsingDeclaration(mtd);
        }
        else if (fd)
        {
          //printf("Inside file %s\n",nd->name().data());
          fd->addUsingDeclaration(mtd);
        }
      }
      else if (usingCd) // add the class to the correct scope
      {
        if (nd)
        {
          //printf("Inside namespace %s\n",nd->name().data());
          nd->addUsingDeclaration(usingCd);
        }
        else if (fd)
        {
          //printf("Inside file %s\n",nd->name().data());
          fd->addUsingDeclaration(usingCd);
        }
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    findUsingDeclarations(e);
  }
}

//----------------------------------------------------------------------

static void findUsingDeclImports(Entry *root)
{
  if (root->section==Entry::USINGDECL_SEC &&
      (root->parent->section&Entry::COMPOUND_MASK) // in a class/struct member
     )
  {
    //printf("Found using declaration %s at line %d of %s inside section %x\n",
    //    root->name.data(),root->startLine,root->fileName.data(),
    //    root->parent->section);
    QCString fullName=removeRedundantWhiteSpace(root->parent->name);
    fullName=stripAnonymousNamespaceScope(fullName);
    fullName=stripTemplateSpecifiersFromScope(fullName);
    ClassDef *cd = getClass(fullName);
    if (cd)
    {
      //printf("found class %s\n",cd->name().data());
      int i=root->name.find("::");
      if (i!=-1)
      {
        QCString scope=root->name.left(i);
        QCString memName=root->name.right(root->name.length()-i-2);
        ClassDef *bcd = getResolvedClass(cd,0,scope); // todo: file in fileScope parameter
        if (bcd)
        {
          //printf("found class %s\n",bcd->name().data());
          MemberNameInfoSDict *mndict=bcd->memberNameInfoSDict();
          MemberNameInfo *mni = mndict->find(memName);
          if (mni)
          {
            MemberNameInfoIterator mnii(*mni); 
            MemberInfo *mi;
            for ( ; (mi=mnii.current()) ; ++mnii )
            {
              MemberDef *md = mi->memberDef;
              if (md && md->protection()!=Private)
              {
                //printf("found member %s\n",mni->memberName());
                MemberDef *newMd = new MemberDef(
                            root->fileName,root->startLine,
                            md->typeString(),memName,md->argsString(),
                            md->excpString(),root->protection,root->virt,
                            md->isStatic(),FALSE,md->memberType(),
                            md->templateArguments(),md->argumentList()
                           );
                cd->insertMember(newMd);
                newMd->setMemberClass(cd);
                if (!root->doc.isEmpty() || !root->brief.isEmpty())
                {
                  newMd->setDocumentation(root->doc,root->docFile,root->docLine);
                  newMd->setBriefDescription(root->brief,root->briefFile,root->briefLine);
                  newMd->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
                }
                else
                {
                  newMd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
                  newMd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
                  newMd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
                }
                newMd->setDefinition(md->definition());
                newMd->enableCallGraph(root->callGraph);
                newMd->setBitfields(md->bitfieldString());
                newMd->addSectionsToDefinition(root->anchors);
                newMd->setBodySegment(md->getStartBodyLine(),md->getEndBodyLine());
                newMd->setBodyDef(md->getBodyDef());
                newMd->setInitializer(md->initializer());
                newMd->setMaxInitLines(md->initializerLines());
                newMd->setMemberGroupId(root->mGrpId);
                newMd->setMemberSpecifiers(md->getMemberSpecifiers());
              }
            }
          }
        }
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    findUsingDeclImports(e);
  }
}

//----------------------------------------------------------------------

static void findIncludedUsingDirectives()
{
  // first mark all files as not visited
  FileNameListIterator fnli(Doxygen::inputNameList); 
  FileName *fn;
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      fd->visited=FALSE;
    }
  }
  // then recursively add using directives found in #include files
  // to files that have not been visited.
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (fni.toFirst();(fd=fni.current());++fni)
    {
      if (!fd->visited) 
      {
        //printf("----- adding using directives for file %s\n",fd->name().data());
        fd->addIncludedUsingDirectives();
      }
    }
  }
}

//----------------------------------------------------------------------

static MemberDef *addVariableToClass(
    Entry *root,
    ClassDef *cd,
    MemberDef::MemberType mtype,
    const QCString &name,
    bool fromAnnScope,
    MemberDef *fromAnnMemb,
    Protection prot,
    bool related)
{
  QCString qualScope = cd->qualifiedNameWithTemplateParameters();
  QCString scopeSeparator="::";
  if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
  {
    qualScope = substitute(qualScope,"::",".");
    scopeSeparator=".";
  }
  Debug::print(Debug::Variables,0,
      "  class variable:\n"
      "    %s' %s'::`%s' `%s' prot=`%d ann=%d init=%s\n",
      root->type.data(),
      qualScope.data(), 
      name.data(),
      root->args.data(),
      root->protection,
      fromAnnScope,
      root->initializer.data()
              );

  QCString def;
  if (!root->type.isEmpty())
  {
    if (related || mtype==MemberDef::Friend || Config_getBool("HIDE_SCOPE_NAMES"))
    {
      def=root->type+" "+name+root->args;
    }
    else
    {
      def=root->type+" "+qualScope+scopeSeparator+name+root->args;
    }
  }
  else
  {
    if (Config_getBool("HIDE_SCOPE_NAMES"))
    {
      def=name+root->args;
    }
    else
    {
      def=qualScope+scopeSeparator+name+root->args;
    }
  }
  def.stripPrefix("static ");

  // see if the member is already found in the same scope
  // (this may be the case for a static member that is initialized
  //  outside the class)
  MemberName *mn=Doxygen::memberNameSDict[name];
  if (mn)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      if (md->getClassDef()==cd && root->type==md->typeString()) 
        // member already in the scope
      {
        addMemberDocs(root,md,def,0,FALSE);
        return md;
      }
    } 
  }
  // new member variable, typedef or enum value
  MemberDef *md=new MemberDef(
      root->fileName,root->startLine,
      root->type,name,root->args,0,
      prot,Normal,root->stat,related,
      mtype,0,0);
  md->setTagInfo(root->tagInfo);
  md->setMemberClass(cd); // also sets outer scope (i.e. getOuterScope())
  //md->setDefFile(root->fileName);
  //md->setDefLine(root->startLine);
  md->setDocumentation(root->doc,root->docFile,root->docLine);
  md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  md->setDefinition(def);
  md->setBitfields(root->bitfields);
  md->addSectionsToDefinition(root->anchors);
  md->setFromAnonymousScope(fromAnnScope);
  md->setFromAnonymousMember(fromAnnMemb);
  //md->setIndentDepth(indentDepth);
  md->setBodySegment(root->bodyLine,root->endBodyLine);
  md->setInitializer(root->initializer);
  md->setMaxInitLines(root->initLines);
  md->setMemberGroupId(root->mGrpId);
  md->setMemberSpecifiers(root->memSpec);
  md->setReadAccessor(root->read);
  md->setWriteAccessor(root->write);
  md->enableCallGraph(root->callGraph);
  addMemberToGroups(root,md);
  //if (root->mGrpId!=-1) 
  //{
  //  printf("memberdef %s in memberGroup %d\n",name.data(),root->mGrpId);
  //  md->setMemberGroup(memberGroupDict[root->mGrpId]);
  //
  bool ambig;
  md->setBodyDef(findFileDef(Doxygen::inputNameDict,root->fileName,ambig));

  //printf("Adding member=%s\n",md->name().data());
  // add the member to the global list
  if (mn)
  {
    mn->append(md);
  }
  else // new variable name
  {
    mn = new MemberName(name);
    mn->append(md);
    //printf("Adding memberName=%s\n",mn->memberName());
    //Doxygen::memberNameDict.insert(name,mn);
    //Doxygen::memberNameList.append(mn);
    Doxygen::memberNameSDict.append(name,mn);
    // add the member to the class
  }
  cd->insertMember(md);
  md->setRefItems(root->sli);

  //TODO: insert FileDef instead of filename strings.
  cd->insertUsedFile(root->fileName);
  root->section = Entry::EMPTY_SEC;
  return md;
}

//----------------------------------------------------------------------

static MemberDef *addVariableToFile(
    Entry *root,
    MemberDef::MemberType mtype,
    const QCString &scope,
    const QCString &name,
    bool fromAnnScope,
    /*int indentDepth,*/
    MemberDef *fromAnnMemb)
{
  Debug::print(Debug::Variables,0,
      "  global variable:\n"
      "    type=`%s' scope=`%s' name=`%s' args=`%s' prot=`%d mtype=%d\n",
      root->type.data(),
      scope.data(), 
      name.data(),
      root->args.data(),
      root->protection,
      mtype
              );

  bool ambig;
  FileDef *fd = findFileDef(Doxygen::inputNameDict,root->fileName,ambig);

  // see if the function is inside a namespace
  NamespaceDef *nd = 0;
  if (!scope.isEmpty())
  {
    QCString nscope=removeAnonymousScopes(scope);
    if (!nscope.isEmpty())
    {
      nd = getResolvedNamespace(nscope);
    }
  }
  QCString def;

  // determine the definition of the global variable
  if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@' && 
      !Config_getBool("HIDE_SCOPE_NAMES")
     )
    // variable is inside a namespace, so put the scope before the name
  {
    static bool optimizeForJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
    QCString sep="::";
    if (optimizeForJava) sep=".";
    
    if (!root->type.isEmpty())
    {
      def=root->type+" "+nd->name()+sep+name+root->args;
    }
    else
    {
      def=nd->name()+sep+name+root->args;
    }
  }
  else
  {
    if (!root->type.isEmpty())
    {
      if (name.at(0)=='@') // dummy variable representing annonymous union
        def=root->type;
      else
        def=root->type+" "+name+root->args;
    }
    else
    {
      def=name+root->args;
    }
  }
  def.stripPrefix("static ");

  MemberName *mn=Doxygen::functionNameSDict[name];
  if (mn)
  {
    QCString nscope=removeAnonymousScopes(scope);
    NamespaceDef *nd=0;
    if (!nscope.isEmpty())
    {
      nd = getResolvedNamespace(nscope);
    }
    MemberNameIterator mni(*mn);
    MemberDef *md;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      if (
          ((nd==0 && md->getNamespaceDef()==0 && md->getFileDef() && 
            root->fileName==md->getFileDef()->absFilePath()
           ) // both variable names in the same file
           || (nd!=0 && md->getNamespaceDef()==nd) // both in same namespace
          )
          && !md->isDefine() // function style #define's can be "overloaded" by typedefs or variables
         )
        // variable already in the scope
      {
        Debug::print(Debug::Variables,0,
           "    variable already found: scope=%s\n",md->getOuterScope()->name().data());
        addMemberDocs(root,md,def,0,FALSE);
        md->setRefItems(root->sli);
        return md;
      }
    } 
  }
  Debug::print(Debug::Variables,0,
    "    new variable!\n");
  // new global variable, enum value or typedef
  MemberDef *md=new MemberDef(
      root->fileName,root->startLine,
      root->type,name,root->args,0,
      Public, Normal,root->stat,FALSE,
      mtype,0,0);
  md->setTagInfo(root->tagInfo);
  md->setDocumentation(root->doc,root->docFile,root->docLine);
  md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  md->addSectionsToDefinition(root->anchors);
  md->setFromAnonymousScope(fromAnnScope);
  md->setFromAnonymousMember(fromAnnMemb);
  md->setInitializer(root->initializer);
  md->setMaxInitLines(root->initLines);
  md->setMemberGroupId(root->mGrpId);
  md->setDefinition(def);
  md->enableCallGraph(root->callGraph);
  md->setExplicitExternal(root->explicitExternal);
  //md->setOuterScope(fd);
  if (!root->explicitExternal)
  {
    md->setBodySegment(root->bodyLine,root->endBodyLine);
    md->setBodyDef(fd);
  }
  addMemberToGroups(root,md);

  md->setRefItems(root->sli);
  if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
  {
    nd->insertMember(md); 
    md->setNamespace(nd);
  }

  // add member to the file (we do this even if we have already inserted
  // it into the namespace. 
  if (fd)
  {
    fd->insertMember(md);
    md->setFileDef(fd); 
  }

  // add member definition to the list of globals 
  if (mn)
  {
    mn->append(md);
  }
  else
  {
    mn = new MemberName(name);
    mn->append(md);
    Doxygen::functionNameSDict.append(name,mn);
  }
  root->section = Entry::EMPTY_SEC;
  return md;
}

/*! See if the return type string \a type is that of a function pointer 
 *  \returns -1 if this is not a function pointer variable or
 *           the index at which the brace of (...*name) was found.
 */
static int findFunctionPtr(const QCString &type,int *pLength=0)
{
  static const QRegExp re("([^)]*)");
  int i=-1,l;
  if (!type.isEmpty() &&             // return type is non-empty
      (i=re.match(type,0,&l))!=-1 &&     // contains a (*
      type.find("operator")==-1 &&   // not an operator
      type.find(")(")==-1            // not a function pointer return type
     )
  {
    if (pLength) *pLength=l;
    return i;
  }
  else
  {
    return -1;
  }
}


/*! Returns TRUE iff \a type is a class within scope \a context.
 *  Used to detect variable declarations that look like function prototypes.
 */
static bool isVarWithConstructor(Entry *root)
{
  static QRegExp initChars("[0-9\"'&*!^]+");
  static QRegExp idChars("[a-z_A-Z][a-z_A-Z0-9]*");
  bool result=FALSE;
  bool typeIsClass;
  QCString type;
  Definition *ctx = 0;
  FileDef *fd = 0;
  bool ambig;
  int ti;

  if (root->parent && root->parent->section&Entry::COMPOUND_MASK)
  { // inside a class
    result=FALSE;
    goto done;
  }
  else if ((fd = findFileDef(Doxygen::inputNameDict,root->fileName,ambig)) &&
       fd->name().right(2)==".c"
     )
  { // inside a .c file
    result=FALSE;
    goto done;
  }
  if (root->type.isEmpty()) 
  {
    result=FALSE;
    goto done;
  }
  if (root->parent->name) ctx=Doxygen::namespaceSDict.find(root->parent->name);
  type = root->type;
  // remove qualifiers
  findAndRemoveWord(type,"const");
  findAndRemoveWord(type,"static");
  findAndRemoveWord(type,"volatile");
  //if (type.left(6)=="const ") type=type.right(type.length()-6);
  typeIsClass=getResolvedClass(ctx,fd,type)!=0;
  if (!typeIsClass && (ti=type.find('<'))!=-1)
  {
    typeIsClass=getResolvedClass(ctx,fd,type.left(ti))!=0;
  }
  if (typeIsClass) // now we still have to check if the arguments are 
                   // types or values. Since we do not have complete type info
                   // we need to rely on heuristics :-(
  {
    //printf("typeIsClass\n");
    ArgumentList *al = root->argList;
    if (al==0 || al->isEmpty()) 
    {
      result=FALSE; // empty arg list -> function prototype.
      goto done;
    }
    ArgumentListIterator ali(*al);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      if (!a->name.isEmpty() || !a->defval.isEmpty()) 
      {
        if (a->name.find(initChars)==0)
        {
          result=TRUE;
        }
        else
        {
          result=FALSE; // arg has (type,name) pair -> function prototype
        }
        goto done;
      }
      if (a->type.isEmpty() || getResolvedClass(ctx,fd,a->type)!=0) 
      {
        result=FALSE; // arg type is a known type
        goto done;
      }
      if (a->type.find(initChars)==0) 
      {
        result=TRUE; // argument type starts with typical initializer char
        goto done;
      }
      QCString resType=resolveTypeDef(ctx,a->type);
      if (resType.isEmpty()) resType=a->type;
      int len;
      if (idChars.match(resType,0,&len)==0) // resType starts with identifier
      {
        resType=resType.left(len);
        //printf("resType=%s\n",resType.data());
        if (resType=="int"    || resType=="long" || resType=="float" || 
            resType=="double" || resType=="char" || resType=="signed" || 
            resType=="const"  || resType=="unsigned" || resType=="void") 
        {
          result=FALSE; // type keyword -> function prototype
          goto done;
        }
      }
    }
    result=TRUE;
  }
done:
  //printf("isVarWithConstructor(%s,%s)=%d\n",root->parent->name.data(),
  //                                       root->type.data(),result);
  return result;
}

//----------------------------------------------------------------------
// Searches the Entry tree for Variable documentation sections.
// If found they are stored in their class or in the global list.

static void buildVarList(Entry *root)
{
  if (!root->name.isEmpty() &&
      (root->type.isEmpty() || compoundKeywordDict.find(root->type)==0) &&
      (
       (root->section==Entry::VARIABLE_SEC    // it's a variable
       ) ||
       (root->section==Entry::FUNCTION_SEC && // or maybe a function pointer variable 
        findFunctionPtr(root->type)!=-1
       ) ||
       (root->section==Entry::FUNCTION_SEC && // class variable initialized by constructor
        isVarWithConstructor(root)
       )
      ) 
     ) // documented variable
  {
    Debug::print(Debug::Variables,0,
                  "VARIABLE_SEC: \n"
                  "  type=`%s' name=`%s' args=`%s' bodyLine=`%d' mGrpId=%d\n",
                   root->type.data(),
                   root->name.data(),
                   root->args.data(),
                   root->bodyLine,
                   root->mGrpId
                );
    //printf("root->parent->name=%s\n",root->parent->name.data());

    if (root->type.isEmpty() && root->name.find("operator")==-1 &&
        (root->name.find('*')!=-1 || root->name.find('&')!=-1))
    {
      // recover from parse error caused by redundant braces 
      // like in "int *(var[10]);", which is parsed as
      // type="" name="int *" args="(var[10])"

      root->type=root->name;
      static const QRegExp reName("[a-z_A-Z][a-z_A-Z0-9]*");
      int l;
      int i=root->args.isEmpty() ? -1 : reName.match(root->args,0,&l);
      root->name=root->args.mid(i,l);
      root->args=root->args.mid(i+l,root->args.find(')',i+l)-i-l);
      //printf("new: type=`%s' name=`%s' args=`%s'\n",
      //    root->type.data(),root->name.data(),root->args.data());
    }
    else
    {
      int i=findFunctionPtr(root->type);
      if (i!=-1) // function pointer
      {
        int ai = root->type.find('[',i);
        if (ai>i) // function pointer array
        {
          root->args.prepend(root->type.right(root->type.length()-ai));
          root->type=root->type.left(ai);
        }
        else if (root->type.find(')',i)!=-1) // function ptr, not variable like "int (*bla)[10]"
        {
          root->type=root->type.left(root->type.length()-1);
          root->args.prepend(")");
          //printf("root->type=%s root->args=%s\n",root->type.data(),root->args.data());
        }
      }
    }
    
    QCString scope,name=removeRedundantWhiteSpace(root->name);

    // find the scope of this variable 
    Entry *p = root->parent;
    while ((p->section & Entry::SCOPE_MASK))
    {
      QCString scopeName = p->name.copy();
      if (!scopeName.isEmpty())
      {
        scope.prepend(scopeName);
        break;
      }
      p=p->parent;
    }
    
    MemberDef::MemberType mtype;
    QCString type=root->type.stripWhiteSpace();
    ClassDef *cd=0;

    if (root->name.findRev("::")!=-1) 
    {
      if (root->type=="friend class" || root->type=="friend struct" || 
          root->type=="friend union")
      {
         cd=getClass(scope);
         if (cd)
         {
           addVariableToClass(root,  // entry
                              cd,    // class to add member to
                              MemberDef::Friend, // type of member
                              name, // name of the member
                              FALSE,  // from Anonymous scope
                              0,      // anonymous member
                              Public, // protection
                              FALSE   // related to a class
                             );
         }
      }
      goto nextMember;
               /* skip this member, because it is a 
                * static variable definition (always?), which will be
                * found in a class scope as well, but then we know the
                * correct protection level, so only then it will be
                * inserted in the correct list!
                */
    }

    if (type=="@") 
      mtype=MemberDef::EnumValue;
    else if (type.left(8)=="typedef ") 
      mtype=MemberDef::Typedef;
    else if (type.left(7)=="friend ")
      mtype=MemberDef::Friend;
    else if (root->mtype==Property)
      mtype=MemberDef::Property;
    else if (root->mtype==Event)
      mtype=MemberDef::Event;
    else
      mtype=MemberDef::Variable;

    QCString classScope=stripAnonymousNamespaceScope(scope);
    classScope=stripTemplateSpecifiersFromScope(classScope,FALSE);
    QCString annScopePrefix=scope.left(scope.length()-classScope.length());

    bool isRelated=FALSE;
    if (!root->relates.isEmpty()) // related variable
    {
      isRelated=TRUE;
      if (getClass(root->relates)==0 && !scope.isEmpty())
        scope=mergeScopes(scope,root->relates);
      else 
        scope=root->relates.copy();
    }
    
    if (!scope.isEmpty() && !name.isEmpty() && (cd=getClass(scope)))
    {
      MemberDef *md=0;

      // if cd is an annonymous scope we insert the member 
      // into a non-annonymous scope as well. This is needed to
      // be able to refer to it using \var or \fn

      //int indentDepth=0;
      int si=scope.find('@');
      //int anonyScopes = 0;
      bool added=FALSE;
      if (si!=-1) // anonymous scope
      {
        QCString pScope;
        ClassDef *pcd=0;
        pScope = scope.left(QMAX(si-2,0));
        if (!pScope.isEmpty())
          pScope.prepend(annScopePrefix);
        else if (annScopePrefix.length()>2)
          pScope=annScopePrefix.left(annScopePrefix.length()-2);
        if (name.at(0)!='@')
        {
          if (!pScope.isEmpty() && (pcd=getClass(pScope)))
          {
            md=addVariableToClass(root,  // entry
                                  pcd,   // class to add member to
                                  mtype, // member type
                                  name,  // member name
                                  TRUE,  // from anonymous scope
                                  0,     // from anonymous member
                                  root->protection,
                                  isRelated
                                 );
            added=TRUE;
          }
          else // anonymous scope inside namespace or file => put variable in the global scope
          {
            if (mtype==MemberDef::Variable)
            {
              md=addVariableToFile(root,mtype,pScope,name,TRUE,0); 
            }
            added=TRUE;
          }
        }
      }
      //printf("name=`%s' scope=%s scope.right=%s indentDepth=%d anonyScopes=%d\n",
      //                   name.data(),scope.data(),
      //                   scope.right(scope.length()-si).data(),
      //                   indentDepth,
      //                   anonyScopes);
      addVariableToClass(root,   // entry
                         cd,     // class to add member to
                         mtype,  // member type
                         name,   // name of the member
                         FALSE,  // from anonymous scope
                         md,     // from anonymous member
                         root->protection, 
                         isRelated);
    }
    else if (!name.isEmpty()) // global variable
    {
      //printf("Inserting member in global scope %s!\n",scope.data());
      addVariableToFile(root,mtype,scope,name,FALSE,/*0,*/0);
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

static void addMethodToClass(Entry *root,ClassDef *cd,
                  const QCString &rname,/*const QCString &scope,*/bool isFriend)
{
  int l,i;
  static QRegExp re("([a-z_A-Z0-9: ]*[ *]*[ ]*");

  if (!root->type.isEmpty() && (i=re.match(root->type,0,&l))!=-1) // function variable
  {
    root->args+=root->type.right(root->type.length()-i-l);
    root->type=root->type.left(i+l);
  }

  QCString name=removeRedundantWhiteSpace(rname);
  if (name.left(2)=="::") name=name.right(name.length()-2);

  MemberDef::MemberType mtype;
  if (isFriend)                 mtype=MemberDef::Friend;
  else if (root->mtype==Signal) mtype=MemberDef::Signal;
  else if (root->mtype==Slot)   mtype=MemberDef::Slot;
  else if (root->mtype==DCOP)   mtype=MemberDef::DCOP;
  else                          mtype=MemberDef::Function;

  // strip redundant template specifier for constructors
  if ((i=name.find('<'))!=-1 && name.find('>')!=-1)
  {
    name=name.left(i); 
  }

  //printf("root->name=`%s; root->args=`%s' root->argList=`%s'\n", 
  //    root->name.data(),root->args.data(),argListToString(root->argList).data()
  //   );

  // adding class member
  MemberDef *md=new MemberDef(
      root->fileName,root->startLine,
      root->type,name,root->args,root->exception,
      root->protection,root->virt,root->stat,!root->relates.isEmpty(),
      mtype,root->tArgLists ? root->tArgLists->last() : 0,root->argList);
  md->setTagInfo(root->tagInfo);
  md->setMemberClass(cd);
  md->setDocumentation(root->doc,root->docFile,root->docLine);
  md->setDocsForDefinition(!root->proto);
  md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
  md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
  md->setBodySegment(root->bodyLine,root->endBodyLine);
  md->setMemberSpecifiers(root->memSpec);
  md->setMemberGroupId(root->mGrpId);
  bool ambig;
  FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
  md->setBodyDef(fd);
  md->setFileDef(fd);
  //md->setScopeTemplateArguments(root->tArgList);
  md->addSectionsToDefinition(root->anchors);
  QCString def;
  QCString qualScope = cd->qualifiedNameWithTemplateParameters();
  QCString scopeSeparator="::";
  if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
  {
    qualScope = substitute(qualScope,"::",".");
    scopeSeparator=".";
  }
  if (!root->relates.isEmpty() || isFriend || Config_getBool("HIDE_SCOPE_NAMES"))
  {
    if (!root->type.isEmpty())
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
    if (!root->type.isEmpty())
    {
      if (root->argList)
      {
        def=root->type+" "+qualScope+scopeSeparator+name;
      }
      else
      {
        def=root->type+" "+qualScope+scopeSeparator+name+root->args;
      }
    }
    else
    {
      if (root->argList)
      {
        def=qualScope+scopeSeparator+name;
      }
      else
      {
        def=qualScope+scopeSeparator+name+root->args;
      }
    }
  }
  if (def.left(7)=="friend ") def=def.right(def.length()-7);
  md->setDefinition(def);
  md->enableCallGraph(root->callGraph);

  Debug::print(Debug::Functions,0,
      "  Func Member:\n"
      "    `%s' `%s'::`%s' `%s' proto=%d\n"
      "    def=`%s'\n",
      root->type.data(),
      qualScope.data(),
      rname.data(),
      root->args.data(),
      root->proto,
      def.data()
              );

  // add member to the global list of all members
  //printf("Adding member=%s class=%s\n",md->name().data(),cd->name().data());
  MemberName *mn;
  if ((mn=Doxygen::memberNameSDict[name]))
  {
    mn->append(md);
  }
  else
  {
    mn = new MemberName(name);
    mn->append(md);
    Doxygen::memberNameSDict.append(name,mn);
  }

  // add member to the class cd
  cd->insertMember(md);
  // add file to list of used files
  cd->insertUsedFile(root->fileName);

  addMemberToGroups(root,md);
  root->section = Entry::EMPTY_SEC;
  md->setRefItems(root->sli);
}


static void buildFunctionList(Entry *root)
{
  if (root->section==Entry::FUNCTION_SEC)
  {
    Debug::print(Debug::Functions,0,
                 "FUNCTION_SEC:\n"
                 "  `%s' `%s'::`%s' `%s' relates=`%s' relatesDup=`%d' file=`%s' line=`%d' bodyLine=`%d' #tArgLists=%d mGrpId=%d memSpec=%d proto=%d docFile=%s\n",
                 root->type.data(),
                 root->parent->name.data(),
                 root->name.data(),
                 root->args.data(),
                 root->relates.data(),
                 root->relatesDup,
                 root->fileName.data(),
                 root->startLine,
                 root->bodyLine,
                 root->tArgLists ? (int)root->tArgLists->count() : -1,
                 root->mGrpId,
                 root->memSpec,
                 root->proto,
                 root->docFile.data()
                );

    bool isFriend=root->type.find("friend ")!=-1;
    QCString rname = removeRedundantWhiteSpace(root->name);
    //printf("rname=%s\n",rname.data());

    if (!rname.isEmpty())
    {
      
      ClassDef *cd=0;
      // check if this function's parent is a class
      static QRegExp re("([a-z_A-Z0-9: ]*[ *]*[ ]*");
      //printf("root->parent=`%s' %x cd=%p root->type.find(re,0)=%d\n",
      //    root->parent->name.data(),root->parent->section,getClass(root->parent->name),
      //    root->type.find(re,0));
      QCString scope=root->parent->name; //stripAnonymousNamespaceScope(root->parent->name);
      scope=stripTemplateSpecifiersFromScope(scope,FALSE);
      //printf("scope=%s\n",scope.data());

      cd=getClass(scope);
      //printf("cd=%p\n",cd);
      if (cd && scope+"::"==rname.left(scope.length()+2)) // found A::f inside A
      {
        // strip scope from name
        rname=rname.right(rname.length()-root->parent->name.length()-2); 
      }

      NamespaceDef *nd = 0;
      bool isMember=FALSE;
      int memIndex=rname.findRev("::");
      if (memIndex!=-1)
      {
        int ts=rname.find('<');
        int te=rname.find('>');
        if (ts==-1 || te==-1)
        {
          nd = Doxygen::namespaceSDict.find(rname.left(memIndex));
          isMember = nd==0;
          if (nd)
          {
            // strip namespace scope from name
            scope=rname.left(memIndex);
            rname=rname.right(rname.length()-memIndex-2);
          }
        }
        else
        {
          isMember=memIndex<ts || memIndex>te;
        }
      }

      if (root->parent && 
          !root->parent->name.isEmpty() &&
          (root->parent->section & Entry::COMPOUND_MASK) && 
          cd &&
          // do some fuzzy things to exclude function pointers 
          (root->type.isEmpty() || root->type.find(re,0)==-1 || 
           root->type.find(")(")!=-1 || root->type.find("operator")!=-1
          )
         )
      {
        Debug::print(Debug::Functions,0,"  --> member %s of class %s!\n",
            rname.data(),cd->name().data());
        addMethodToClass(root,cd,rname,isFriend);
      }
      else if (root->parent && 
               !((root->parent->section & Entry::COMPOUND_MASK) 
                 || root->parent->section==Entry::OBJCIMPL_SEC
                ) &&
               !isMember &&
               (root->relates.isEmpty() || root->relatesDup) &&
               root->type.left(7)!="extern " && root->type.left(8)!="typedef " 
              )
      // no member => unrelated function 
      {
        /* check the uniqueness of the function name in the file.
         * A file could contain a function prototype and a function definition
         * or even multiple function prototypes.
         */
        bool found=FALSE;
        MemberName *mn;
        MemberDef *md=0;
        if ((mn=Doxygen::functionNameSDict[rname]))
        {
          Debug::print(Debug::Functions,0,"  --> function %s already found!\n",rname.data());
          MemberNameIterator mni(*mn);
          for (mni.toFirst();(!found && (md=mni.current()));++mni)
          {
            NamespaceDef *mnd = md->getNamespaceDef();
            NamespaceDef *rnd = 0;
            if (!root->parent->name.isEmpty())
            {
              rnd = getResolvedNamespace(root->parent->name);
            }
            FileDef *mfd = md->getFileDef();
            QCString nsName,rnsName;
            if (mnd)  nsName = mnd->name().copy();
            if (rnd) rnsName = rnd->name().copy();
            bool ambig;
            FileDef *rfd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
            //printf("matching arguments for %s%s %s%s\n",
            //    md->name().data(),md->argsString(),rname.data(),argListToString(root->argList).data());
            if ( 
                matchArguments2(md->getOuterScope(),mfd,md->argumentList(),
                                rnd ? rnd : Doxygen::globalScope,rfd,root->argList,
                                FALSE)
               )
            {
              GroupDef *gd=0;
              if (root->groups->first()!=0)
              {
                gd = Doxygen::groupSDict[root->groups->first()->groupname.data()];
              }
              //printf("match!\n");
              // see if we need to create a new member
              found=(mnd && rnd && nsName==rnsName) ||   // members are in the same namespace
                    ((mnd==0 && rnd==0 && mfd!=0 &&       // no external reference and
                      mfd->absFilePath()==root->fileName // prototype in the same file
                     ) 
                    ); 
              // otherwise, allow a duplicate global member with the same argument list
              if (!found && gd && gd==md->getGroupDef())
              {
                // member is already in the group, so we don't want to add it again.
                found=TRUE;
              }
              
              //printf("combining function with prototype found=%d in namespace %s\n",
              //    found,nsName.data());

              if (found)
              {
                // merge argument lists
                //mergeArguments(root->argList,md->argumentList());
                // merge documentation
                if (md->documentation().isEmpty() && !root->doc.isEmpty())
                {
                  md->setDocumentation(root->doc,root->docFile,root->docLine);
                  md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
                  md->setDocsForDefinition(!root->proto);
                  ArgumentList *argList = new ArgumentList;
                  stringToArgumentList(root->args,argList);
                  if (root->proto)
                  {
                    //printf("setDeclArgumentList to %p\n",argList);
                    md->setDeclArgumentList(argList);
                  }
                  else
                  {
                    md->setArgumentList(argList);
                  }
                }
                else if (!md->documentation().isEmpty() && !root->doc.isEmpty() && mnd==rnd)
                {
                  warn(root->docFile,root->docLine,"Warning: member %s: ignoring the detailed description found here, since another one was found at line %d of file %s!",md->name().data(),md->docLine(),md->docFile().data());
                  //printf("md->docs=[%s] root->docs=[%s]\n",md->documentation().data(),root->doc.data());
                }

                if (md->briefDescription().isEmpty() && !root->brief.isEmpty())
                {
                  md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
                  md->setArgsString(root->args);
                }
                else if (!md->briefDescription().isEmpty() && !root->brief.isEmpty() && mnd==rnd)
                {
                  warn(root->briefFile,root->briefLine,"Warning: member %s: ignoring the brief description found here, since another one was found at line %d of file %s!",md->name().data(),md->briefLine(),md->briefFile().data());
                }

                md->addSectionsToDefinition(root->anchors);

                md->enableCallGraph(md->hasCallGraph() || root->callGraph);

                // merge ingroup specifiers
                if (md->getGroupDef()==0 && root->groups->first()!=0)
                {
                  addMemberToGroups(root,md);
                }
                else if (md->getGroupDef()!=0 && root->groups->count()==0)
                {
                  //printf("existing member is grouped, new member not\n");
                  root->groups->append(new Grouping(md->getGroupDef()->name(), md->getGroupPri()));
                }
                else if (md->getGroupDef()!=0 && root->groups->first()!=0)
                {
                  //printf("both members are grouped\n");
                }
              }
            }
          }
        }
        if (!found) /* global function is unique with respect to the file */
        {
          Debug::print(Debug::Functions,0,"  --> new function %s found!\n",rname.data());
          //printf("New function type=`%s' name=`%s' args=`%s' bodyLine=%d\n",
          //       root->type.data(),rname.data(),root->args.data(),root->bodyLine);
          
          // new global function
          ArgumentList *tArgList = root->tArgLists ? root->tArgLists->last() : 0;
          QCString name=removeRedundantWhiteSpace(rname);
          md=new MemberDef(
              root->fileName,root->startLine,
              root->type,name,root->args,root->exception,
              root->protection,root->virt,root->stat,FALSE,
              MemberDef::Function,tArgList,root->argList);

          md->setTagInfo(root->tagInfo);
          //md->setDefFile(root->fileName);
          //md->setDefLine(root->startLine);
          md->setDocumentation(root->doc,root->docFile,root->docLine);
          md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
          md->setPrototype(root->proto);
          md->setDocsForDefinition(!root->proto);
          //md->setBody(root->body);
          md->setBodySegment(root->bodyLine,root->endBodyLine);
          bool ambig;
          FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
          md->setBodyDef(fd);
          md->addSectionsToDefinition(root->anchors);
          md->setMemberSpecifiers(root->memSpec);
          md->setMemberGroupId(root->mGrpId);

          // see if the function is inside a namespace that was not part of
          // the name already (in that case nd should be non-zero already)
          if (nd==0 && root->parent->section == Entry::NAMESPACE_SEC )
          {
            QCString nscope=removeAnonymousScopes(root->parent->name);
            if (!nscope.isEmpty())
            {
              nd = getResolvedNamespace(nscope);
            }
          }

          if (!scope.isEmpty())
          {
            if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
            {
              scope = substitute(scope,"::",".")+".";
            }
            else
            {
              scope+="::";
            }
          }

          QCString def;
          if (!root->type.isEmpty())
          {
            if (root->argList)
            {
              def=root->type+" "+scope+name;
            }
            else
            {
              def=root->type+" "+scope+name+root->args;
            }
          }
          else
          {
            if (root->argList)
            {
              def=scope+name.copy();
            }
            else
            {
              def=scope+name+root->args;
            }
          }
          Debug::print(Debug::Functions,0,
                     "  Global Function:\n"
                     "    `%s' `%s'::`%s' `%s' proto=%d\n"
                     "    def=`%s'\n",
                     root->type.data(),
                     root->parent->name.data(),
                     rname.data(),
                     root->args.data(),
                     root->proto,
                     def.data()
                    );
          md->setDefinition(def);
          md->enableCallGraph(root->callGraph);
          //if (root->mGrpId!=-1) 
          //{
          //  md->setMemberGroup(memberGroupDict[root->mGrpId]);
          //}

          md->setRefItems(root->sli);
          if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
          {
            // add member to namespace
            nd->insertMember(md); 
            md->setNamespace(nd);
          }
          if (fd)
          {
            // add member to the file (we do this even if we have already
            // inserted it into the namespace)
            fd->insertMember(md);
            md->setFileDef(fd); 
          }

          // add member to the list of file members
          //printf("Adding member=%s\n",md->name().data());
          MemberName *mn;
          if ((mn=Doxygen::functionNameSDict[name]))
          {
            mn->append(md);
          }
          else 
          {
            mn = new MemberName(name);
            mn->append(md);
            Doxygen::functionNameSDict.append(name,mn);
          }
          addMemberToGroups(root,md);
          if (!root->relatesDup) // if this is a relatesalso command, allow find
                                 // Member to pick it up
          {
            root->section = Entry::EMPTY_SEC; // Otherwise we have finished 
                                              // with this entry.

          }
        }
        else
        {
          bool ambig;
          FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
          if (fd)
          {
            // add member to the file (we do this even if we have already
            // inserted it into the namespace)
            fd->insertMember(md);
          }
        }

        //printf("unrelated function %d `%s' `%s' `%s'\n",
        //    root->parent->section,root->type.data(),rname.data(),root->args.data());
      }
    }
    else if (rname.isEmpty())
    {
        warn(root->fileName,root->startLine,
             "Warning: Illegal member name found."
            );
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildFunctionList(e);
  }
}

//----------------------------------------------------------------------

static void findFriends()
{
  //printf("findFriends()\n");
  MemberNameSDict::Iterator fnli(Doxygen::functionNameSDict);
  MemberName *fn;
  for (;(fn=fnli.current());++fnli) // for each global function name
  {
    //printf("Function name=`%s'\n",fn->memberName());
    MemberName *mn;
    if ((mn=Doxygen::memberNameSDict[fn->memberName()]))
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
          if ((mmd->isFriend() || (mmd->isRelated() && mmd->isFunction())) &&
              matchArguments2(mmd->getOuterScope(), mmd->getFileDef(), mmd->argumentList(),
                              fmd->getOuterScope(), fmd->getFileDef(), fmd->argumentList(),
                              TRUE
                             )
                             
             ) // if the member is related and the arguments match then the 
               // function is actually a friend.
          {
            mergeArguments(mmd->argumentList(),fmd->argumentList());
            if (!fmd->documentation().isEmpty())
            {
              mmd->setDocumentation(fmd->documentation(),fmd->docFile(),fmd->docLine());
            }
            else if (!mmd->documentation().isEmpty())
            {
              fmd->setDocumentation(mmd->documentation(),mmd->docFile(),mmd->docLine());
            }
            if (mmd->briefDescription().isEmpty() && !fmd->briefDescription().isEmpty())
            {
              mmd->setBriefDescription(fmd->briefDescription(),fmd->briefFile(),fmd->briefLine());
            }
            else if (!mmd->briefDescription().isEmpty() && !fmd->briefDescription().isEmpty())
            {
              fmd->setBriefDescription(mmd->briefDescription(),mmd->briefFile(),mmd->briefLine());
            }
            if (!fmd->inbodyDocumentation().isEmpty())
            {
              mmd->setInbodyDocumentation(fmd->inbodyDocumentation(),fmd->inbodyFile(),fmd->inbodyLine());
            }
            else if (!mmd->inbodyDocumentation().isEmpty())
            {
              fmd->setInbodyDocumentation(mmd->inbodyDocumentation(),mmd->inbodyFile(),mmd->inbodyLine());
            }
            //printf("body mmd %d fmd %d\n",mmd->getStartBodyLine(),fmd->getStartBodyLine());
            if (mmd->getStartBodyLine()==-1 && fmd->getStartBodyLine()!=-1)
            {
              mmd->setBodySegment(fmd->getStartBodyLine(),fmd->getEndBodyLine());
              mmd->setBodyDef(fmd->getBodyDef());
              //mmd->setBodyMember(fmd);
            }
            else if (mmd->getStartBodyLine()!=-1 && fmd->getStartBodyLine()==-1)
            {
              fmd->setBodySegment(mmd->getStartBodyLine(),mmd->getEndBodyLine());
              fmd->setBodyDef(mmd->getBodyDef());
              //fmd->setBodyMember(mmd);
            }
            mmd->setDocsForDefinition(fmd->isDocsForDefinition());

            mmd->enableCallGraph(mmd->hasCallGraph() || fmd->hasCallGraph());
            fmd->enableCallGraph(mmd->hasCallGraph() || fmd->hasCallGraph());
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

static void transferArgumentDocumentation(ArgumentList *decAl,ArgumentList *defAl)
{
  if (decAl && defAl)
  {
    ArgumentListIterator decAli(*decAl);
    ArgumentListIterator defAli(*defAl);
    Argument *decA,*defA;
    for (decAli.toFirst(),defAli.toFirst();
        (decA=decAli.current()) && (defA=defAli.current());
        ++decAli,++defAli)
    {
      //printf("Argument decA->name=%s (doc=%s) defA->name=%s (doc=%s)\n",
      //    decA->name.data(),decA->docs.data(),
      //    defA->name.data(),defA->docs.data()
      //      );
      if (decA->docs.isEmpty() && !defA->docs.isEmpty())
      {
        decA->docs = defA->docs.copy();
      }
      else if (defA->docs.isEmpty() && !decA->docs.isEmpty())
      {
        defA->docs = decA->docs.copy();
      }
    }
  }
}

static void transferFunctionDocumentation()
{
  //printf("transferFunctionDocumentation()\n");

  // find matching function declaration and definitions.
  MemberNameSDict::Iterator mnli(Doxygen::functionNameSDict);
  MemberName *mn;
  for (;(mn=mnli.current());++mnli)
  {
    //printf("memberName=%s count=%d\n",mn->memberName(),mn->count());
    MemberDef *mdef=0,*mdec=0;
    MemberNameIterator mni1(*mn);
    /* find a matching function declaration and definition for this function */
    for (;(mdec=mni1.current());++mni1)
    {
      if (mdec->isPrototype() ||
          (mdec->isVariable() && mdec->isExternal()) 
         )
      {
        MemberNameIterator mni2(*mn);
        for (;(mdef=mni2.current());++mni2)
        {
          if (
              (mdef->isFunction() && !mdef->isStatic() && !mdef->isPrototype()) ||
              (mdef->isVariable() && !mdef->isExternal() && !mdef->isStatic())
             )
          {
            //printf("mdef=(%p,%s) mdec=(%p,%s)\n",
            //    mdef, mdef ? mdef->name().data() : "",
            //    mdec, mdec ? mdec->name().data() : "");
            if (mdef && mdec && 
                matchArguments2(mdef->getOuterScope(),mdef->getFileDef(),mdef->argumentList(),
                                mdec->getOuterScope(),mdec->getFileDef(),mdec->argumentList(),
                                TRUE
                               )
               ) /* match found */
            {
              //printf("Found member %s: definition in %s (doc=`%s') and declaration in %s (doc=`%s')\n",
              //    mn->memberName(),
              //    mdef->getFileDef()->name().data(),mdef->documentation().data(),
              //    mdec->getFileDef()->name().data(),mdec->documentation().data()
              //    );

              // first merge argument documentation
              transferArgumentDocumentation(mdec->argumentList(),mdef->argumentList());

              /* copy documentation between function definition and declaration */
              if (!mdec->briefDescription().isEmpty())
              {
                mdef->setBriefDescription(mdec->briefDescription(),mdec->briefFile(),mdec->briefLine());
              }
              else if (!mdef->briefDescription().isEmpty())
              {
                mdec->setBriefDescription(mdef->briefDescription(),mdef->briefFile(),mdef->briefLine());
              }
              if (!mdef->documentation().isEmpty())
              {
                //printf("transfering docs mdef->mdec (%s->%s)\n",mdef->argsString(),mdec->argsString());
                mdec->setDocumentation(mdef->documentation(),mdef->docFile(),mdef->docLine());
                mdec->setDocsForDefinition(mdef->isDocsForDefinition());
                if (mdef->argumentList())
                {
                  ArgumentList *mdefAl = new ArgumentList;
                  stringToArgumentList(mdef->argsString(),mdefAl);
                  transferArgumentDocumentation(mdef->argumentList(),mdefAl);
                  mdec->setArgumentList(mdefAl);
                }
              }
              else if (!mdec->documentation().isEmpty())
              {
                //printf("transfering docs mdec->mdef (%s->%s)\n",mdec->argsString(),mdef->argsString());
                mdef->setDocumentation(mdec->documentation(),mdec->docFile(),mdec->docLine());
                mdef->setDocsForDefinition(mdec->isDocsForDefinition());
                if (mdec->argumentList())
                {
                  ArgumentList *mdecAl = new ArgumentList;
                  stringToArgumentList(mdec->argsString(),mdecAl);
                  transferArgumentDocumentation(mdec->argumentList(),mdecAl);
                  mdef->setDeclArgumentList(mdecAl);
                }
              }
              if (!mdef->inbodyDocumentation().isEmpty())
              {
                mdec->setInbodyDocumentation(mdef->inbodyDocumentation(),mdef->inbodyFile(),mdef->inbodyLine());
              }
              else if (!mdec->inbodyDocumentation().isEmpty())
              {
                mdef->setInbodyDocumentation(mdec->inbodyDocumentation(),mdec->inbodyFile(),mdec->inbodyLine());
              }
              if (mdec->getStartBodyLine()!=-1 && mdef->getStartBodyLine()==-1)
              {
                //printf("body mdec->mdef %d-%d\n",mdec->getStartBodyLine(),mdef->getEndBodyLine());
                mdef->setBodySegment(mdec->getStartBodyLine(),mdec->getEndBodyLine());
                mdef->setBodyDef(mdec->getBodyDef());
                //mdef->setBodyMember(mdec);
              }
              else if (mdef->getStartBodyLine()!=-1 && mdec->getStartBodyLine()==-1)
              {
                //printf("body mdef->mdec %d-%d\n",mdef->getStartBodyLine(),mdec->getEndBodyLine());
                mdec->setBodySegment(mdef->getStartBodyLine(),mdef->getEndBodyLine());
                mdec->setBodyDef(mdef->getBodyDef());
                //mdec->setBodyMember(mdef);
              }
              mdec->mergeMemberSpecifiers(mdef->getMemberSpecifiers());
              mdef->mergeMemberSpecifiers(mdec->getMemberSpecifiers());


              // copy group info.
              if (mdec->getGroupDef()==0 && mdef->getGroupDef()!=0)
              {
                mdec->setGroupDef(mdef->getGroupDef(),
                                  mdef->getGroupPri(),
                                  mdef->docFile(),
                                  mdef->docLine(),
                                  mdef->hasDocumentation(),
                                  mdef
                                 );
              }
              else if (mdef->getGroupDef()==0 && mdec->getGroupDef()!=0)
              {
                mdef->setGroupDef(mdec->getGroupDef(),
                                  mdec->getGroupPri(),
                                  mdec->docFile(),
                                  mdec->docLine(),
                                  mdec->hasDocumentation(),
                                  mdec
                                 );
              }

              mdec->mergeRefItems(mdef);
              mdef->mergeRefItems(mdec);

              mdef->setMemberDeclaration(mdec);
              mdec->setMemberDefinition(mdef);

              mdef->enableCallGraph(mdec->hasCallGraph() || mdef->hasCallGraph());
              mdec->enableCallGraph(mdec->hasCallGraph() || mdef->hasCallGraph());
            }
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

static void transferFunctionReferences()
{
  MemberNameSDict::Iterator mnli(Doxygen::functionNameSDict);
  MemberName *mn;
  for (;(mn=mnli.current());++mnli)
  {
    MemberDef *md,*mdef=0,*mdec=0;
    MemberNameIterator mni(*mn);
    /* find a matching function declaration and definition for this function */
    for (;(md=mni.current());++mni)
    {
      if (md->isPrototype()) 
        mdec=md;
      else if (md->isVariable() && md->isExternal()) 
        mdec=md;
      
      if (md->isFunction() && !md->isStatic() && !md->isPrototype()) 
        mdef=md;
      else if (md->isVariable() && !md->isExternal() && !md->isStatic())
        mdef=md;
    }
    if (mdef && mdec && 
        matchArguments2(mdef->getOuterScope(),mdef->getFileDef(),mdef->argumentList(),
                        mdec->getOuterScope(),mdec->getFileDef(),mdec->argumentList(),
                        TRUE
                       )
       ) /* match found */
    {
      MemberSDict *defDict = mdef->getReferencesMembers();
      MemberSDict *decDict = mdec->getReferencesMembers();
      if (defDict)
      {
        MemberSDict::Iterator msdi(*defDict);
        MemberDef *rmd;
        for (msdi.toFirst();(rmd=msdi.current());++msdi)
        {
          if (decDict==0 || decDict->find(rmd->name())==0)
          {
            mdec->addSourceReferences(rmd);
          }
        }
      }
      if (decDict)
      {
        MemberSDict::Iterator msdi(*decDict);
        MemberDef *rmd;
        for (msdi.toFirst();(rmd=msdi.current());++msdi)
        {
          if (defDict==0 || defDict->find(rmd->name())==0)
          {
            mdef->addSourceReferences(rmd);
          }
        }
      }

      defDict = mdef->getReferencedByMembers();
      decDict = mdec->getReferencedByMembers();
      if (defDict)
      {
        MemberSDict::Iterator msdi(*defDict);
        MemberDef *rmd;
        for (msdi.toFirst();(rmd=msdi.current());++msdi)
        {
          if (decDict==0 || decDict->find(rmd->name())==0)
          {
            mdec->addSourceReferencedBy(rmd);
          }
        }
      }
      if (decDict)
      {
        MemberSDict::Iterator msdi(*decDict);
        MemberDef *rmd;
        for (msdi.toFirst();(rmd=msdi.current());++msdi)
        {
          if (defDict==0 || defDict->find(rmd->name())==0)
          {
            mdef->addSourceReferencedBy(rmd);
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------

static void transferRelatedFunctionDocumentation()
{
  // find match between function declaration and definition for 
  // related functions
  MemberNameSDict::Iterator mnli(Doxygen::functionNameSDict);
  MemberName *mn;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md;
    MemberNameIterator mni(*mn);
    /* find a matching function declaration and definition for this function */
    for (mni.toFirst();(md=mni.current());++mni) // for each global function
    {
      //printf("  Function `%s'\n",md->name().data());
      MemberName *rmn;
      if ((rmn=Doxygen::memberNameSDict[md->name()])) // check if there is a member with the same name
      {
        //printf("  Member name found\n");
        MemberDef *rmd;
        MemberNameIterator rmni(*rmn);
        for (rmni.toFirst();(rmd=rmni.current());++rmni) // for each member with the same name
        {
          //printf("  Member found: related=`%d'\n",rmd->isRelated());
          if (rmd->isRelated() && // related function
              matchArguments2( md->getOuterScope(), md->getFileDef(), md->argumentList(),
                              rmd->getOuterScope(),rmd->getFileDef(),rmd->argumentList(),
                              TRUE
                             )
             )
          {
            //printf("  Found related member `%s'\n",md->name().data());
            if (rmd->relatedAlso())
              md->setRelatedAlso(rmd->relatedAlso());
            else
              md->makeRelated();
          } 
        }
      } 
    }
  }
}

//----------------------------------------------------------------------

/*! make a dictionary of all template arguments of class cd
 * that are part of the base class name. 
 * Example: A template class A with template arguments <R,S,T> 
 * that inherits from B<T,T,S> will have T and S in the dictionary.
 */
static QDict<int> *getTemplateArgumentsInName(ArgumentList *templateArguments,const QCString &name)
{
  QDict<int> *templateNames = new QDict<int>(17);
  templateNames->setAutoDelete(TRUE);
  static QRegExp re("[a-z_A-Z][a-z_A-Z0-9:]*");
  if (templateArguments)
  {
    ArgumentListIterator ali(*templateArguments);
    Argument *arg;
    int count=0;
    for (ali.toFirst();(arg=ali.current());++ali,count++)
    {
      int i,p=0,l;
      while ((i=re.match(name,p,&l))!=-1)
      {
        QCString n = name.mid(i,l);
        if (n==arg->name)
        {
          if (templateNames->find(n)==0)
          {
            templateNames->insert(n,new int(count));
          }
        }
        p=i+l;
      }
    }
  }
  return templateNames;
}

/*! Searches a class from within \a context and \a cd and returns its
 *  definition if found (otherwise 0 is returned).
 */
static ClassDef *findClassWithinClassContext(Definition *context,ClassDef *cd,const QCString &name)
{
  
#if 0
  ClassDef *result=0;

  FileDef *fd=cd->getFileDef();
  // try using of namespaces in namespace scope
  NamespaceDef *nd=cd->getNamespaceDef();
  if (nd) // class is inside a namespace
  {
    QCString fName = nd->name()+"::"+name;
    result = getResolvedClass(cd,fd,fName); 
    if (result && result!=cd) 
    {
      return result;
    }
    NamespaceSDict *nl = nd->getUsedNamespaces();
    if (nl) // try to prepend any of the using namespace scopes.
    {
      NamespaceSDict::Iterator nli(*nl);
      NamespaceDef *nd;
      for (nli.toFirst() ; (nd=nli.current()) ; ++nli)
      {
        fName = nd->name()+"::"+name;
        result = getResolvedClass(cd,fd,fName);
        if (result && result!=cd) return result;
      }
    }
    SDict<Definition> *cl = nd->getUsedClasses();
    if (cl)
    {
      SDict<Definition>::Iterator cli(*cl);
      Definition *ucd;
      for (cli.toFirst(); (ucd=cli.current()) ; ++cli)
      {
        if (ucd->definitionType()==Definition::TypeClass && 
            rightScopeMatch(ucd->name(),name))
        {
          return (ClassDef *)ucd;
        }
      }
    }
    // TODO: check any inbetween namespaces as well!
    if (fd) // and in the global namespace
    {
      SDict<Definition> *cl = fd->getUsedClasses();
      if (cl)
      {
        SDict<Definition>::Iterator cli(*cl);
        Definition *ucd;
        for (cli.toFirst(); (ucd=cli.current()); ++cli)
        {
          if (ucd->definitionType()==Definition::TypeClass && 
              rightScopeMatch(ucd->name(),name))
          {
            return (ClassDef *)ucd;
          }
        }
      }
    }
  }

  // try using of namespaces in file scope
  if (fd)
  {
    // look for the using statement in this file in which the
    // class was found
    NamespaceSDict *nl = fd->getUsedNamespaces();
    if (nl) // try to prepend any of the using namespace scopes.
    {
      NamespaceSDict::Iterator nli(*nl);
      NamespaceDef *nd;
      for (nli.toFirst() ; (nd=nli.current()) ; ++nli)
      {
        QCString fName = nd->name()+"::"+name;
        result=getResolvedClass(cd,fd,fName);
        if (result && result!=cd)
        {
          return result;
        }
      }
    }
    SDict<Definition> *cl = fd->getUsedClasses();
    if (cl)
    {
      SDict<Definition>::Iterator cli(*cl);
      Definition *ucd;
      for (cli.toFirst(); (ucd=cli.current()) ; ++cli)
      {
        if (ucd->definitionType()==Definition::TypeClass && 
           rightScopeMatch(ucd->name(),name))
        {
          return (ClassDef *)ucd;
        }
      }
    }
  }
#endif

  FileDef *fd=cd->getFileDef();
  ClassDef *result=0;
  if (context && cd!=context)
  {
    result = getResolvedClass(context,0,name);
  }
  if (result==0)
  {
    result = getResolvedClass(cd,fd,name);
  }
  //printf("** Trying to find %s within context %s class %s result=%s\n",
  //       name.data(),
  //       context ? context->name().data() : "<none>",
  //       cd      ? cd->name().data()      : "<none>",
  //       result  ? result->name().data()  : "<none>"
  //      );
  return result;
}

enum FindBaseClassRelation_Mode 
{ 
  TemplateInstances, 
  DocumentedOnly, 
  Undocumented 
};

static bool findClassRelation(
                           Entry *root,
                           Definition *context,
                           ClassDef *cd,
                           BaseInfo *bi,
                           QDict<int> *templateNames,
                           /*bool insertUndocumented*/
                           FindBaseClassRelation_Mode mode,
                           bool isArtificial
                          );


static void findUsedClassesForClass(Entry *root,
                           Definition *context,
                           ClassDef *masterCd,
                           ClassDef *instanceCd,
                           bool isArtificial,
                           ArgumentList *actualArgs=0,
                           QDict<int> *templateNames=0
                           )
{
  masterCd->visited=TRUE;
  ArgumentList *formalArgs = masterCd->templateArguments();
  MemberNameInfoSDict::Iterator mnili(*masterCd->memberNameInfoSDict());
  MemberNameInfo *mni;
  for (;(mni=mnili.current());++mnili)
  {
    MemberNameInfoIterator mnii(*mni);
    MemberInfo *mi;
    for (mnii.toFirst();(mi=mnii.current());++mnii)
    {
      MemberDef *md=mi->memberDef;
      if (md->isVariable()) // for each member variable in this class
      {
        //printf("Found variable %s in class %s\n",md->name().data(),masterCd->name().data());
        QCString type=removeRedundantWhiteSpace(md->typeString());
        int pos=0;
        QCString usedClassName;
        QCString templSpec;
        bool found=FALSE;
        // the type can contain template variables, replace them if present
        if (actualArgs)
        {
          type = substituteTemplateArgumentsInString(type,formalArgs,actualArgs);
        }

        //printf("  template substitution gives=%s\n",type.data());
        while (!found && extractClassNameFromType(type,pos,usedClassName,templSpec)!=-1)
        {
          // find the type (if any) that matches usedClassName
          ClassDef *typeCd = getResolvedClass(masterCd,masterCd->getFileDef(),usedClassName);
          //printf("====>  usedClassName=%s -> typeCd=%s\n",
          //     usedClassName.data(),typeCd?typeCd->name().data():"<none>");
          if (typeCd)
          {
            usedClassName = typeCd->name();
          }
          
          int sp=usedClassName.find('<');
          if (sp==-1) sp=0;
          int si=usedClassName.findRev("::",sp);
          if (si!=-1)
          {
            // replace any namespace aliases
            replaceNamespaceAliases(usedClassName,si);
          }
          // add any template arguments to the class
          QCString usedName = removeRedundantWhiteSpace(usedClassName+templSpec);
          //printf("    usedName=%s\n",usedName.data());

          bool delTempNames=FALSE;
          if (templateNames==0)
          {
            templateNames = getTemplateArgumentsInName(formalArgs,usedName);
            delTempNames=TRUE;
          }
          BaseInfo bi(usedName,Public,Normal);
          findClassRelation(root,context,instanceCd,&bi,templateNames,TemplateInstances,isArtificial);

          if (masterCd->templateArguments())
          {
            ArgumentListIterator ali(*masterCd->templateArguments());
            Argument *arg;
            int count=0;
            for (ali.toFirst();(arg=ali.current());++ali,++count)
            {
              if (arg->name==usedName) // type is a template argument
              {
                found=TRUE;
                Debug::print(Debug::Classes,0,"  New used class `%s'\n", usedName.data());
                
                ClassDef *usedCd = Doxygen::hiddenClasses.find(usedName);
                if (usedCd==0)
                {
                  usedCd = new ClassDef(
                     masterCd->getDefFileName(),masterCd->getDefLine(),
                     usedName,ClassDef::Class);
                  Doxygen::hiddenClasses.append(usedName,usedCd);
                }
                if (usedCd)
                {
                  if (isArtificial) usedCd->setClassIsArtificial();
                  Debug::print(Debug::Classes,0,"    Adding used class `%s' (1)\n", usedCd->name().data());
                  instanceCd->addUsedClass(usedCd,md->name());
                  usedCd->addUsedByClass(instanceCd,md->name());
                }
              }
            }
          }

          if (!found)
          {
            ClassDef *usedCd=findClassWithinClassContext(context,masterCd,usedName);
            //printf("Looking for used class %s: result=%s master=%s\n",
            //    usedName.data(),usedCd?usedCd->name().data():"<none>",masterCd?masterCd->name().data():"<none>");

            if (usedCd) 
            {
              found=TRUE;
              Debug::print(Debug::Classes,0,"    Adding used class `%s' (2)\n", usedCd->name().data());
              instanceCd->addUsedClass(usedCd,md->name()); // class exists 
              usedCd->addUsedByClass(instanceCd,md->name());
            }
          }
          if (delTempNames)
          {
            delete templateNames;
            templateNames=0;
          }
        }
        if (!found && !type.isEmpty()) // used class is not documented in any scope
        {
          ClassDef *usedCd = Doxygen::hiddenClasses.find(type);
          if (usedCd==0 && !Config_getBool("HIDE_UNDOC_RELATIONS"))
          {
             if (type.right(2)=="(*") // type is a function pointer
             {
               type+=md->argsString();
             }
             Debug::print(Debug::Classes,0,"  New undocumented used class `%s'\n", type.data());
             usedCd = new ClassDef(
               masterCd->getDefFileName(),masterCd->getDefLine(),
               type,ClassDef::Class);
             Doxygen::hiddenClasses.append(type,usedCd);
          }
          if (usedCd)
          {
            if (isArtificial) usedCd->setClassIsArtificial();
            Debug::print(Debug::Classes,0,"    Adding used class `%s' (3)\n", usedCd->name().data());
            instanceCd->addUsedClass(usedCd,md->name()); 
            usedCd->addUsedByClass(instanceCd,md->name());
          }
        }
      }
    }
  }
}

static void findBaseClassesForClass(
      Entry *root,
      Definition *context,
      ClassDef *masterCd,
      ClassDef *instanceCd,
      FindBaseClassRelation_Mode mode,
      bool isArtificial,
      ArgumentList *actualArgs=0,
      QDict<int> *templateNames=0
    )
{
  //if (masterCd->visited) return;
  masterCd->visited=TRUE;
  // The base class could ofcouse also be a non-nested class
  ArgumentList *formalArgs = masterCd->templateArguments();
  QListIterator<BaseInfo> bii(*root->extends);
  BaseInfo *bi=0;
  for (bii.toFirst();(bi=bii.current());++bii)
  {
    //printf("masterCd=%s bi->name=%s #actualArgs=%d\n",
    //    masterCd->localName().data(),bi->name.data(),actualArgs?(int)actualArgs->count():-1);
    bool delTempNames=FALSE;
    if (templateNames==0)
    {
      templateNames = getTemplateArgumentsInName(formalArgs,bi->name);
      delTempNames=TRUE;
    }
    BaseInfo tbi(bi->name,bi->prot,bi->virt);
    if (actualArgs) // substitute the formal template arguments of the base class
    {
      tbi.name = substituteTemplateArgumentsInString(bi->name,formalArgs,actualArgs);
    }
    //printf("bi->name=%s tbi.name=%s\n",bi->name.data(),tbi.name.data());

    if (mode==DocumentedOnly)
    {
      // find a documented base class in the correct scope
      if (!findClassRelation(root,context,instanceCd,&tbi,templateNames,DocumentedOnly,isArtificial))
      {
        if (!Config_getBool("HIDE_UNDOC_RELATIONS"))
        {
          // no documented base class -> try to find an undocumented one
          findClassRelation(root,context,instanceCd,&tbi,templateNames,Undocumented,isArtificial);
        }
      }
    }
    else if (mode==TemplateInstances)
    {
      findClassRelation(root,context,instanceCd,&tbi,templateNames,TemplateInstances,isArtificial);
    }
    if (delTempNames)
    {
      delete templateNames;
      templateNames=0;
    }  
  }
}

//----------------------------------------------------------------------

static bool findTemplateInstanceRelation(Entry *root,
            Definition *context,
            ClassDef *templateClass,const QCString &templSpec,
            QDict<int> *templateNames,
            bool isArtificial)
{
  Debug::print(Debug::Classes,0,"    derived from template %s with parameters %s\n",
         templateClass->name().data(),templSpec.data());
  //printf("findTemplateInstanceRelation(base=%s templSpec=%s templateNames=",
  //    templateClass->name().data(),templSpec.data());
  //if (templateNames)
  //{
  //  QDictIterator<int> qdi(*templateNames);
  //  int *tempArgIndex;
  //  for (;(tempArgIndex=qdi.current());++qdi)
  //  {
  //    printf("(%s->%d) ",qdi.currentKey().data(),*tempArgIndex);
  //  }
  //}
  //printf("\n");
  
  bool existingClass = (templSpec==tempArgListToString(templateClass->templateArguments()));
  if (existingClass) return TRUE;

  bool freshInstance=FALSE;
  ClassDef *instanceClass = templateClass->insertTemplateInstance(
                     root->fileName,root->startLine,templSpec,freshInstance);
  if (isArtificial) instanceClass->setClassIsArtificial();
  instanceClass->setIsObjectiveC(root->objc);

  if (freshInstance)
  {
    Debug::print(Debug::Classes,0,"      found fresh instance!\n");
    Doxygen::classSDict.append(instanceClass->name(),instanceClass);
    instanceClass->setTemplateBaseClassNames(templateNames);

    // search for new template instances caused by base classes of 
    // instanceClass 
    Entry *templateRoot = classEntries.find(templateClass->name());
    if (templateRoot)
    {
      Debug::print(Debug::Classes,0,"        template root found %s templSpec=%s!\n",
          templateRoot->name.data(),templSpec.data());
      ArgumentList *templArgs = new ArgumentList;
      stringToArgumentList(templSpec,templArgs);
      findBaseClassesForClass(templateRoot,context,templateClass,instanceClass,
          TemplateInstances,isArtificial,templArgs,templateNames);

      findUsedClassesForClass(templateRoot,context,templateClass,instanceClass,
          isArtificial,templArgs,templateNames);
      delete templArgs;
    }
    else
    {
      Debug::print(Debug::Classes,0,"        no template root entry found!\n");
      // TODO: what happened if we get here?
    }

    //Debug::print(Debug::Classes,0,"    Template instance %s : \n",instanceClass->name().data());
    //ArgumentList *tl = templateClass->templateArguments();
  }
  else
  {
    Debug::print(Debug::Classes,0,"      instance already exists!\n");
  }
  return TRUE;
}

static bool isRecursiveBaseClass(const QCString &scope,const QCString &name)
{
  QCString n=name;
  int index=n.find('<');
  if (index!=-1)
  {
    n=n.left(index);
  }
  bool result = rightScopeMatch(scope,n);
  return result;
}

static bool findClassRelation(
                           Entry *root,
                           Definition *context,
                           ClassDef *cd,
                           BaseInfo *bi,
                           QDict<int> *templateNames,
                           FindBaseClassRelation_Mode mode,
                           bool isArtificial
                          )
{
  //printf("findClassRelation(class=%s base=%s templateNames=",
  //    cd->name().data(),bi->name.data());
  //if (templateNames)
  //{
  //  QDictIterator<int> qdi(*templateNames);
  //  int *tempArgIndex;
  //  for (;(tempArgIndex=qdi.current());++qdi)
  //  {
  //    printf("(%s->%d) ",qdi.currentKey().data(),*tempArgIndex);
  //  }
  //}
  //printf("\n");

  QCString biName=bi->name;
  bool explicitGlobalScope=FALSE;
  if (biName.left(2)=="::") // explicit global scope
  {
     biName=biName.right(biName.length()-2);
     explicitGlobalScope=TRUE;
  }
  //printf("biName=`%s'\n",biName.data());

  Entry *parentNode=root->parent;
  bool lastParent=FALSE;
  do // for each parent scope, starting with the largest scope 
     // (in case of nested classes)
  {
    QCString scopeName= parentNode ? parentNode->name.data() : "";
    int scopeOffset=explicitGlobalScope ? 0 : scopeName.length();
    do // try all parent scope prefixes, starting with the largest scope
    {
      //printf("scopePrefix=`%s' biName=`%s'\n",
      //    scopeName.left(scopeOffset).data(),biName.data());

      QCString baseClassName=biName;
      if (scopeOffset>0)
      {
        baseClassName.prepend(scopeName.left(scopeOffset)+"::");
      }
      baseClassName=stripTemplateSpecifiersFromScope
                          (removeRedundantWhiteSpace(baseClassName));
      MemberDef *baseClassTypeDef=0;
      QCString templSpec;
      ClassDef *baseClass=getResolvedClass(explicitGlobalScope ? 0 : cd,
                                           cd->getFileDef(), // todo: is this ok?
                                           baseClassName,
                                           &baseClassTypeDef,
                                           &templSpec,
                                           mode==Undocumented
                                          );
      //printf("baseClassName=%s baseClass=%p cd=%p explicitGlobalScope=%d\n",
      //    baseClassName.data(),baseClass,cd,explicitGlobalScope);
      //printf("    root->name=`%s' baseClassName=`%s' baseClass=%s templSpec=%s\n",
      //                    root->name.data(),
      //                    baseClassName.data(),
      //                    baseClass?baseClass->name().data():"<none>",
      //                    templSpec.data()
      //      );
      //if (baseClassName.left(root->name.length())!=root->name ||
      //    baseClassName.at(root->name.length())!='<'
      //   ) // Check for base class with the same name.
      //     // If found then look in the outer scope for a match
      //     // and prevent recursion.
      if (!isRecursiveBaseClass(root->name,baseClassName) || explicitGlobalScope)
      {
        Debug::print(
            Debug::Classes,0,"    class relation %s inherited/used by %s found (%s and %s)\n",
            baseClassName.data(),
            root->name.data(),
            (bi->prot==Private)?"private":((bi->prot==Protected)?"protected":"public"),
            (bi->virt==Normal)?"normal":"virtual"
           );

        int i=baseClassName.find('<');
        int si=baseClassName.findRev("::",i==-1 ? baseClassName.length() : i);
        if (si==-1) si=0;
        if (baseClass==0 && i!=-1) 
          // base class has template specifiers
        {
          // TODO: here we should try to find the correct template specialization
          // but for now, we only look for the unspecializated base class.
          // locate end of template
          int e=i+1;
          int brCount=1;
          int typeLen = baseClassName.length();
          while (e<typeLen && brCount!=0)
          {
            if (baseClassName.at(e)=='<') 
            {
              if (e<typeLen-1 && baseClassName.at(e+1)=='<') e++; else brCount++;
            }
            if (baseClassName.at(e)=='>') 
            {
              if (e<typeLen-1 && baseClassName.at(e+1)=='>') e++; else brCount--;
            }
            e++;
          }
          if (brCount==0) // end of template was found at e
          {
            templSpec=baseClassName.mid(i,e-i);
            baseClassName=baseClassName.left(i)+baseClassName.right(baseClassName.length()-e);
            baseClass=getResolvedClass(cd,cd->getFileDef(),baseClassName);
            //printf("baseClass=%p -> baseClass=%s templSpec=%s\n",
            //      baseClass,baseClassName.data(),templSpec.data());
          }
        }

        //printf("cd=%p baseClass=%p\n",cd,baseClass);
        bool found=baseClass!=0 && (baseClass!=cd || mode==TemplateInstances);
        if (!found && si!=-1)
        {
          // replace any namespace aliases
          replaceNamespaceAliases(baseClassName,si);
          baseClass=getResolvedClass(cd,cd->getFileDef(),baseClassName);
          found=baseClass!=0 && baseClass!=cd;
        }
        
        //printf("root->name=%s biName=%s baseClassName=%s\n",
        //        root->name.data(),biName.data(),baseClassName.data());

        //FileDef *fd=cd->getFileDef();
        //NamespaceDef *nd=cd->getNamespaceDef();
        if (!found)
        {
          baseClass=findClassWithinClassContext(context,cd,baseClassName);
          //printf("findClassWithinClassContext(%s,%s)=%p\n",
          //    cd->name().data(),baseClassName.data(),baseClass);
          found = baseClass!=0 && baseClass!=cd;

        }
        bool isATemplateArgument = templateNames!=0 && templateNames->find(biName)!=0;
        if (found)
        {
          Debug::print(Debug::Classes,0,"    Documented class `%s' templSpec=%s\n",biName.data(),templSpec.data());
          // add base class to this class

          // if templSpec is not empty then we should "instantiate"
          // the template baseClass. A new ClassDef should be created
          // to represent the instance. To be able to add the (instantiated)
          // members and documentation of a template class 
          // (inserted in that template class at a later stage), 
          // the template should know about its instances. 
          // the instantiation process, should be done in a recursive way, 
          // since instantiating a template may introduce new inheritance 
          // relations.
          if (!templSpec.isEmpty() && mode==TemplateInstances)
          {
            findTemplateInstanceRelation(root,context,baseClass,templSpec,templateNames,isArtificial);
          }
          else if (mode==DocumentedOnly || mode==Undocumented)
          {
            QCString usedName;
            if (baseClassTypeDef) 
            {
              usedName=biName;
              //printf("***** usedName=%s templSpec=%s\n",usedName.data(),templSpec.data());
            }
            cd->insertBaseClass(baseClass,usedName,bi->prot,bi->virt,templSpec);
            // add this class as super class to the base class
            baseClass->insertSubClass(cd,bi->prot,bi->virt,templSpec);
          }
          return TRUE;
        }
        else if (mode==Undocumented && (scopeOffset==0 || isATemplateArgument))
        {
          Debug::print(Debug::Classes,0,
                       "    New undocumented base class `%s' baseClassName=%s\n",
                       biName.data(),baseClassName.data()
                      );
          baseClass=0;
          if (isATemplateArgument)
          {
            baseClass=Doxygen::hiddenClasses.find(baseClassName);
            if (baseClass==0)
            {
              baseClass=new ClassDef(root->fileName,root->startLine,
                                 baseClassName,ClassDef::Class);
              Doxygen::hiddenClasses.append(baseClassName,baseClass);
              if (isArtificial) baseClass->setClassIsArtificial();
            }
          }
          else
          {
            baseClass=Doxygen::classSDict.find(baseClassName);
            if (baseClass==0)
            {
              baseClass=new ClassDef(root->fileName,root->startLine,
                  baseClassName,ClassDef::Class);
              Doxygen::classSDict.append(baseClassName,baseClass);
              if (isArtificial) baseClass->setClassIsArtificial();
            }
          }
          // add base class to this class
          cd->insertBaseClass(baseClass,biName,bi->prot,bi->virt,templSpec);
          // add this class as super class to the base class
          baseClass->insertSubClass(cd,bi->prot,bi->virt,templSpec);
          // the undocumented base was found in this file
          baseClass->insertUsedFile(root->fileName);
          baseClass->setOuterScope(Doxygen::globalScope);
          return TRUE;
        }
        else
        {
          Debug::print(Debug::Classes,0,"    Base class `%s' not found\n",biName.data());
        }
      }
      else
      {
        if (mode!=TemplateInstances)
        {
          warn(root->fileName,root->startLine,
              "Detected potential recursive class relation "
              "between class %s and base class %s!\n",
              root->name.data(),baseClassName.data()
              );
        }
        // for mode==TemplateInstance this case is quite common and
        // indicates a relation between a template class and a template 
        // instance with the same name.
      }
      if (scopeOffset==0)
      {
        scopeOffset=-1;
      }
      else if ((scopeOffset=scopeName.findRev("::",scopeOffset-1))==-1)
      {
        scopeOffset=0;
      }
      //printf("new scopeOffset=`%d'",scopeOffset);
    } while (scopeOffset>=0);

    if (parentNode==0)
    {
      lastParent=TRUE;
    }
    else
    {
      parentNode=parentNode->parent;
    }
  } while (lastParent);

  return FALSE;
}

//----------------------------------------------------------------------
// Computes the base and super classes for each class in the tree

static bool isClassSection(Entry *root)
{
  return 
    (
      (
        (
          // is it a compound (class, struct, union, interface ...)
          root->section & Entry::COMPOUND_MASK 
        ) 
        || 
        (
         // is it a documentation block with inheritance info.
         (root->section & Entry::COMPOUNDDOC_MASK) && root->extends->count()>0 
        )
      )
      && !root->name.isEmpty() // sanity check
   );
}


/*! Builds a dictionary of all entry nodes in the tree starting with \a root
 */
static void findClassEntries(Entry *root)
{
  if (isClassSection(root))
  {
    classEntries.insert(root->name,root);
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    findClassEntries(e);
  }
}

/*! Using the dictionary build by findClassEntries(), this 
 *  function will look for additional template specialization that
 *  exists as inheritance relations only. These instances will be
 *  added to the template they are derived from.
 */
static void findInheritedTemplateInstances()
{
  ClassSDict::Iterator cli(Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli) cli.current()->visited=FALSE;
  QDictIterator<Entry> edi(classEntries);
  Entry *root;
  for (;(root=edi.current());++edi)
  {
    ClassDef *cd;
    // strip any annonymous scopes first 
    QCString bName=stripAnonymousNamespaceScope(root->name);
    bName=stripTemplateSpecifiersFromScope(bName);
    Debug::print(Debug::Classes,0,"  Class %s : \n",bName.data());
    if ((cd=getClass(bName)))
    {
      //printf("Class %s %d\n",cd->name().data(),root->extends->count());
      findBaseClassesForClass(root,cd,cd,cd,TemplateInstances,FALSE);
    }
  }
}

static void findUsedTemplateInstances()
{
  ClassSDict::Iterator cli(Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli) cli.current()->visited=FALSE;
  QDictIterator<Entry> edi(classEntries);
  Entry *root;
  for (;(root=edi.current());++edi)
  {
    ClassDef *cd;
    // strip any annonymous scopes first 
    QCString bName=stripAnonymousNamespaceScope(root->name);
    bName=stripTemplateSpecifiersFromScope(bName);
    Debug::print(Debug::Classes,0,"  Class %s : \n",bName.data());
    if ((cd=getClass(bName)))
    {
      findUsedClassesForClass(root,cd,cd,cd,TRUE);
    }
  }
}

static void computeClassRelations()
{
  ClassSDict::Iterator cli(Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli) cli.current()->visited=FALSE;
  QDictIterator<Entry> edi(classEntries);
  Entry *root;
  for (;(root=edi.current());++edi)
  {
    ClassDef *cd;

    // strip any annonymous scopes first 
    QCString bName=stripAnonymousNamespaceScope(root->name);
    bName=stripTemplateSpecifiersFromScope(bName);
    Debug::print(Debug::Classes,0,"  Class %s : \n",bName.data());
    if ((cd=getClass(bName)))
    {
      findBaseClassesForClass(root,cd,cd,cd,DocumentedOnly,FALSE);
    }
    if ((cd==0 || (!cd->hasDocumentation() && !cd->isReference())) && 
        bName.right(2)!="::")
    {
      if (!root->name.isEmpty() && root->name.find('@')==-1 && // normal name
          (guessSection(root->fileName)==Entry::HEADER_SEC || 
           Config_getBool("EXTRACT_LOCAL_CLASSES")) && // not defined in source file
          (root->protection!=Private || Config_getBool("EXTRACT_PRIVATE")) && // hidden by protection
          !Config_getBool("HIDE_UNDOC_CLASSES") // undocumented class are visible
         )
        warn_undoc(
                   root->fileName,root->startLine,
                   "Warning: Compound %s is not documented.",
                   root->name.data()
             );
    }
  }
}

static void computeTemplateClassRelations()
{
  QDictIterator<Entry> edi(classEntries);
  Entry *root;
  for (;(root=edi.current());++edi)
  {
    QCString bName=stripAnonymousNamespaceScope(root->name);
    bName=stripTemplateSpecifiersFromScope(bName);
    ClassDef *cd=getClass(bName);
    // strip any annonymous scopes first 
    QDict<ClassDef> *templInstances = 0;
    if (cd && (templInstances=cd->getTemplateInstances()))
    {
      Debug::print(Debug::Classes,0,"  Template class %s : \n",cd->name().data());
      QDictIterator<ClassDef> tdi(*templInstances);
      ClassDef *tcd;
      for (tdi.toFirst();(tcd=tdi.current());++tdi) // for each template instance
      {
        Debug::print(Debug::Classes,0,"    Template instance %s : \n",tcd->name().data());
        QCString templSpec = tdi.currentKey();
        ArgumentList *templArgs = new ArgumentList;
        stringToArgumentList(templSpec,templArgs);
        QList<BaseInfo> *baseList=root->extends;
        BaseInfo *bi=baseList->first();
        while (bi) // for each base class of the template
        {
          // check if the base class is a template argument
          BaseInfo tbi(bi->name,bi->prot,bi->virt);
          ArgumentList *tl = cd->templateArguments();
          if (tl)
          {
            QDict<int> *baseClassNames = tcd->getTemplateBaseClassNames();
            QDict<int> *templateNames = getTemplateArgumentsInName(tl,bi->name);
            // for each template name that we inherit from we need to
            // substitute the formal with the actual arguments
            QDict<int> *actualTemplateNames = new QDict<int>(17);
            actualTemplateNames->setAutoDelete(TRUE);
            QDictIterator<int> qdi(*templateNames);
            for (qdi.toFirst();qdi.current();++qdi)
            {
              int templIndex = *qdi.current();
              Argument *actArg = 0;
              if (templIndex<(int)templArgs->count()) 
              {
                actArg=templArgs->at(templIndex);
              }
              if (actArg!=0 &&
                  baseClassNames!=0 &&
                  baseClassNames->find(actArg->type)!=0 &&
                  actualTemplateNames->find(actArg->type)==0
                 )
              {
                actualTemplateNames->insert(actArg->type,new int(templIndex));
              }
            }
            delete templateNames;
            
            tbi.name = substituteTemplateArgumentsInString(bi->name,tl,templArgs);
            // find a documented base class in the correct scope
            if (!findClassRelation(root,cd,tcd,&tbi,actualTemplateNames,DocumentedOnly,FALSE))
            {
              // no documented base class -> try to find an undocumented one
              findClassRelation(root,cd,tcd,&tbi,actualTemplateNames,Undocumented,FALSE);
            }
            delete actualTemplateNames;
          }
          bi=baseList->next();
        }
        delete templArgs;
      } // class has no base classes
    }
  }
}

//-----------------------------------------------------------------------
// compute the references (anchors in HTML) for each function in the file

static void computeMemberReferences()
{
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    cd->computeAnchors();
  } 
  FileName *fn=Doxygen::inputNameList.first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->computeAnchors();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList.next();
  }
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd=0;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    nd->computeAnchors();
  }
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->computeAnchors();
  }
}

//----------------------------------------------------------------------

static void addListReferences()
{
  MemberNameSDict::Iterator mnli(Doxygen::memberNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md=0;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      md->visited=FALSE;
    }
  }
  MemberNameSDict::Iterator fnli(Doxygen::functionNameSDict);
  for (fnli.toFirst();(mn=fnli.current());++fnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md=0;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      md->visited=FALSE;
    }
  }

  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    cd->addListReferences();
  } 
  FileName *fn=Doxygen::inputNameList.first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->addListReferences();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList.next();
  }
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd=0;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    nd->addListReferences();
  }
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->addListReferences();
  }
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    QCString name = pd->name();
    if (pd->getGroupDef())
    {
      name = pd->getGroupDef()->getOutputFileBase().copy();
    }
    addRefItem(pd->xrefListItems(),
               theTranslator->trPage(TRUE,TRUE),
               name,pd->title());
  }
}

//----------------------------------------------------------------------
// Copy the documentation in entry `root' to member definition `md' and
// set the function declaration of the member to `funcDecl'. If the boolean 
// over_load is set the standard overload text is added. 

static void addMemberDocs(Entry *root,
                   MemberDef *md, const char *funcDecl,
                   ArgumentList *al,
                   bool over_load,
                   NamespaceSDict *
                  )
{
  //printf("addMemberDocs: `%s'::`%s' `%s' funcDecl=`%s' memSpec=%d\n",
  //     root->parent->name.data(),md->name().data(),md->argsString(),funcDecl,root->memSpec);
  QCString fDecl=funcDecl;
  // strip extern specifier
  fDecl.stripPrefix("extern ");
  md->setDefinition(fDecl);
  md->enableCallGraph(root->callGraph);
  ClassDef     *cd=md->getClassDef();
  NamespaceDef *nd=md->getNamespaceDef();
  QCString fullName;
  if (cd) 
    fullName = cd->name();
  else if (nd) 
    fullName = nd->name();

  if (!fullName.isEmpty()) fullName+="::";
  fullName+=md->name();
  bool ambig;
  FileDef *rfd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);

  // TODO determine scope based on root not md
  Definition *rscope = md->getOuterScope();
  
  if (al)
  {
    //printf("merging arguments (1) docs=%d\n",root->doc.isEmpty());
    if ((!root->doc.isEmpty() || !root->proto))
    {
      mergeArguments(md->argumentList(),al,TRUE);
    }
  }
  else
  {
    if ( (!root->doc.isEmpty() || !root->proto) &&
          matchArguments2( md->getOuterScope(), md->getFileDef(), md->argumentList(),
                           rscope,rfd,root->argList,
                           TRUE
                         )
       ) 
    {
      //printf("merging arguments (2)\n");
      mergeArguments(md->argumentList(),root->argList,TRUE);
    }
  }
  if (over_load)  // the \overload keyword was used
  {
    QCString doc=getOverloadDocs();
    if (!root->doc.isEmpty())
    {
      doc+="<p>";
      doc+=root->doc;
    }
    md->setDocumentation(doc,root->docFile,root->docLine); 
    md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
    md->setDocsForDefinition(!root->proto);
  }
  else  
  {
    //printf("Adding docs md->docs=`%s' root->docs=`%s'!\n",
    //     md->documentation().data(),root->doc.data());
    // documentation outside a compound overrides the documentation inside it
    if ( /* !md->isStatic() && !root->stat &&   do not replace doc of a static */
        (
         md->documentation().isEmpty() ||    /* no docs yet */
         (root->parent->name.isEmpty() &&     /* or overwrite prototype docs */
          !root->proto && md->isPrototype()  /* with member definition docs */
         )
        ) && !root->doc.isEmpty() 
       )
    {
      //printf("overwrite!\n");
      md->setDocumentation(root->doc,root->docFile,root->docLine);
      md->setDocsForDefinition(!root->proto);
    }

    //printf("Adding brief md->brief=`%s' root->brief=`%s'!\n",
    //     md->briefDescription().data(),root->brief.data());
    // brief descriptions inside a compound override the documentation 
    // outside it
    if ( /* !md->isStatic() && !root->stat &&  do not replace doc of static */
        ( 
         md->briefDescription().isEmpty() ||  /* no docs yet */
         !root->parent->name.isEmpty()         /* member of a class */
        ) && !root->brief.isEmpty()
       )
    {
      //printf("overwrite!\n");
      md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
    }

    if (
        (md->inbodyDocumentation().isEmpty() ||
         !root->parent->name.isEmpty()
        ) && !root->inbodyDocs.isEmpty()
       )
    {
      md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
    }
  }

  if (md->initializer().isEmpty() && !root->initializer.isEmpty())
  {
    md->setInitializer(root->initializer);
    md->setMaxInitLines(root->initLines);
  }

  if (rfd)
  {

    if ((md->getStartBodyLine()==-1 && root->bodyLine!=-1) 
       // || (md->isVariable() && !root->explicitExternal)
       )
    {
      //printf("Setting new body segment [%d,%d]\n",root->bodyLine,root->endBodyLine);
      md->setBodySegment(root->bodyLine,root->endBodyLine);
      md->setBodyDef(rfd);
    }

    md->setRefItems(root->sli);
  }

  md->enableCallGraph(md->hasCallGraph() || root->callGraph);

  md->mergeMemberSpecifiers(root->memSpec);
  md->addSectionsToDefinition(root->anchors);
  addMemberToGroups(root,md);
  if (cd) cd->insertUsedFile(root->fileName);
  //printf("root->mGrpId=%d\n",root->mGrpId);
  if (root->mGrpId!=-1)
  {
    if (md->getMemberGroupId()!=-1)
    {
      if (md->getMemberGroupId()!=root->mGrpId)
      {
        warn(
             root->fileName,root->startLine,
             "Warning: member %s belongs to two different groups. The second "
             "one found here will be ignored.",
             md->name().data()
            );
      }
    }
    else // set group id
    {
      //printf("setMemberGroupId=%d md=%s\n",root->mGrpId,md->name().data());
      md->setMemberGroupId(root->mGrpId);
    }
  }
}

//----------------------------------------------------------------------
// find a class definition given the scope name and (optionally) a 
// template list specifier

static ClassDef *findClassDefinition(FileDef *fd,NamespaceDef *nd,
                         const char *scopeName)
{
  ClassDef *tcd = getResolvedClass(nd,fd,scopeName,0,0,TRUE);
  return tcd;
}


//----------------------------------------------------------------------
// Adds the documentation contained in `root' to a global function
// with name `name' and argument list `args' (for overloading) and
// function declaration `decl' to the corresponding member definition.

static bool findGlobalMember(Entry *root, 
                           const QCString &namespaceName,
                           const char *name, 
                           const char *tempArg,
                           const char *, 
                           const char *decl)
{
  Debug::print(Debug::FindMembers,0,
       "2. findGlobalMember(namespace=%s,name=%s,tempArg=%s,decl=%s)\n",
          namespaceName.data(),name,tempArg,decl);
  QCString n=name;
  if (n.isEmpty()) return FALSE;
  if (n.find("::")!=-1) return FALSE; // skip undefined class members
  MemberName *mn=Doxygen::functionNameSDict[n+tempArg]; // look in function dictionary
  if (mn==0)
  {
    mn=Doxygen::functionNameSDict[n]; // try without template arguments
  }
  if (mn) // function name defined
  {
    Debug::print(Debug::FindMembers,0,"3. Found function scope\n");
    //int count=0;
    MemberNameIterator mni(*mn);
    MemberDef *md;
    bool found=FALSE;
    for (mni.toFirst();(md=mni.current()) && !found;++mni)
    {
      bool ambig;
      NamespaceDef *nd=md->getNamespaceDef();

      //printf("Namespace namespaceName=%s nd=%s\n",
      //    namespaceName.data(),nd ? nd->name().data() : "<none>");

      FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
      //printf("File %s\n",fd ? fd->name().data() : "<none>");
      NamespaceSDict *nl    = fd ? fd->getUsedNamespaces() : 0;
      //SDict<Definition> *cl = fd ? fd->getUsedClasses()    : 0;
      //printf("NamespaceList %p\n",nl);

      // search in the list of namespaces that are imported via a 
      // using declaration
      bool viaUsingDirective = nl && nd && nl->find(nd->qualifiedName())!=0;

      if ((namespaceName.isEmpty() && nd==0) ||  // not in a namespace
          (nd && nd->name()==namespaceName) ||   // or in the same namespace 
          viaUsingDirective                      // member in `using' namespace
         )     
      {
        Debug::print(Debug::FindMembers,0,"4. Try to add member `%s' to scope `%s'\n",
            md->name().data(),namespaceName.data());
        QCString nsName = nd ? nd->name().data() : "";

        NamespaceDef *rnd = 0;
        if (!namespaceName.isEmpty()) rnd = Doxygen::namespaceSDict[namespaceName];
        
        bool matching=
          (md->argumentList()==0 && root->argList->count()==0) ||
          md->isVariable() || md->isTypedef() || /* in case of function pointers */
          matchArguments2(md->getOuterScope(),md->getFileDef(),md->argumentList(),
                          rnd ? rnd : Doxygen::globalScope,fd,root->argList,
                          FALSE);

        // for static members we also check if the comment block was found in 
        // the same file. This is needed because static members with the same
        // name can be in different files. Thus it would be wrong to just
        // put the comment block at the first syntactically matching member.
        if (matching && md->isStatic() && 
            md->getDefFileName()!=root->fileName && 
            mn->count()>1)
        {
          matching = FALSE;
        }

        if (matching) // add docs to the member
        {
          Debug::print(Debug::FindMembers,0,"5. Match found\n");
          addMemberDocs(root,md,decl,root->argList,FALSE);
          found=TRUE;
        }
      }
    } 
    if (!found && !root->relatesDup) // no match
    {
      QCString fullFuncDecl=decl;
      if (root->argList) fullFuncDecl+=argListToString(root->argList,TRUE);
      warn(root->fileName,root->startLine,
           "Warning: no matching file member found for \n%s",fullFuncDecl.data());   
      if (mn->count()>0)
      {
        warn_cont("Possible candidates:\n");
        for (mni.toFirst();(md=mni.current());++mni)
        {
          warn_cont("  %s\n",md->declaration());
        }
      }
    }
  }
  else // got docs for an undefined member!
  {
    if (root->type!="friend class" && 
        root->type!="friend struct" &&
        root->type!="friend union")
    {
      warn(root->fileName,root->startLine,
           "Warning: documented function `%s' was not defined.",decl
          );
    }
  }
  return TRUE;
}

static QCString substituteTemplatesInString(
    const QList<ArgumentList> &srcTempArgLists,
    const QList<ArgumentList> &dstTempArgLists,
    const QCString &src
    )
{
  QCString dst;
  QRegExp re(idMask);
  //printf("type=%s\n",sa->type.data());

  int i,p=0,l; 
  while ((i=re.match(src,p,&l))!=-1) // for each word in srcType
  {
    bool found=FALSE;
    dst+=src.mid(p,i-p);
    QCString name=src.mid(i,l);

    QListIterator<ArgumentList> srclali(srcTempArgLists);
    QListIterator<ArgumentList> dstlali(dstTempArgLists);
    for (;srclali.current() && !found;++srclali,++dstlali)
    {
      ArgumentListIterator tsali(*srclali.current());
      ArgumentListIterator tdali(*dstlali.current());
      Argument *tsa =0,*tda=0;

      for (tsali.toFirst();(tsa=tsali.current()) && !found;++tsali)
      {
        tda = tdali.current();
        if (tda && name==tsa->name)
        {
          name=tda->name; // substitute
          found=TRUE;
        }
        if (tda) ++tdali;
      }
    }
    dst+=name; 
    p=i+l;
  }
  dst+=src.right(src.length()-p);
  return dst;
}

static void substituteTemplatesInArgList(
                  const QList<ArgumentList> &srcTempArgLists,
                  const QList<ArgumentList> &dstTempArgLists,
                  ArgumentList *src,
                  ArgumentList *dst)
{
  ArgumentListIterator sali(*src);
  Argument *sa=0;
  Argument *da=dst->first();

  for (sali.toFirst();(sa=sali.current());++sali) // for each member argument
  {
    QCString dstType = substituteTemplatesInString(
                                  srcTempArgLists,dstTempArgLists,sa->type);
    QCString dstArray = substituteTemplatesInString(
                                  srcTempArgLists,dstTempArgLists,sa->array);
    if (da==0)
    {
      da=new Argument(*sa);
      dst->append(da);
      da->type=dstType;
      da->array=dstArray;
      da=0;
    }
    else
    {
      da->type=dstType;
      da->type=dstArray;
      da=dst->next();
    }
  }
  dst->constSpecifier    = src->constSpecifier;
  dst->volatileSpecifier = src->volatileSpecifier;
  dst->pureSpecifier     = src->pureSpecifier;
  //printf("substituteTemplatesInArgList: replacing %s with %s\n",
  //    argListToString(src).data(),argListToString(dst).data());
}



/*! This function tries to find a member (in a documented class/file/namespace) 
 * that corresponds to the function/variable declaration given in \a funcDecl.
 *
 * The boolean \a overloaded is used to specify whether or not a standard
 * overload documentation line should be generated.
 *
 * The boolean \a isFunc is a hint that indicates that this is a function
 * instead of a variable or typedef.
 */
static void findMember(Entry *root,
                       QCString funcDecl,
                       bool overloaded,
                       bool isFunc
                      )
{
  Debug::print(Debug::FindMembers,0,
               "findMember(root=%p,funcDecl=`%s',related=`%s',overload=%d,"
               "isFunc=%d mGrpId=%d tArgList=%p (#=%d) "
               "memSpec=%d isObjC=%d\n",
               root,funcDecl.data(),root->relates.data(),overloaded,isFunc,root->mGrpId,
               root->tArgLists,root->tArgLists ? root->tArgLists->count() : 0,
               root->memSpec,root->objc
              );

  QCString scopeName;
  QCString className;
  QCString namespaceName;
  QCString funcType;
  QCString funcName;
  QCString funcArgs;
  QCString funcTempList;
  QCString exceptions;
  QCString funcSpec;
  bool isRelated=FALSE;
  bool isFriend=FALSE;
  bool done;
  do
  {
    done=TRUE;
    if (funcDecl.stripPrefix("friend ")) // treat friends as related members
    {
      isFriend=TRUE;
      done=FALSE;
    }
    if (funcDecl.stripPrefix("inline "))
    {
      root->memSpec|=Entry::Inline;
      done=FALSE;
    }
    if (funcDecl.stripPrefix("explicit "))
    {
      root->memSpec|=Entry::Explicit;
      done=FALSE;
    }
    if (funcDecl.stripPrefix("mutable "))
    {
      root->memSpec|=Entry::Mutable;
      done=FALSE;
    }
    if (funcDecl.stripPrefix("virtual "))
    {
      done=FALSE;
    }
  } while (!done);

  // delete any ; from the function declaration
  int sep;
  while ((sep=funcDecl.find(';'))!=-1)
  {
    funcDecl=(funcDecl.left(sep)+funcDecl.right(funcDecl.length()-sep-1)).stripWhiteSpace();
  }
  
  // make sure the first character is a space to simplify searching.
  if (!funcDecl.isEmpty() && funcDecl[0]!=' ') funcDecl.prepend(" ");
  
  // remove some superfluous spaces
  funcDecl= substitute(
              substitute(
                substitute(funcDecl,"~ ","~"),
                ":: ","::"
              ),
              " ::","::"
            ).stripWhiteSpace();
  
  //printf("funcDecl=`%s'\n",funcDecl.data());
  if (isFriend && funcDecl.left(6)=="class ")
  {
    //printf("friend class\n");
    funcDecl=funcDecl.right(funcDecl.length()-6);
    funcName = funcDecl.copy();
  }
  else if (isFriend && funcDecl.left(7)=="struct ")
  {
    funcDecl=funcDecl.right(funcDecl.length()-7);
    funcName = funcDecl.copy();
  }
  else
  {
    // extract information from the declarations
    parseFuncDecl(funcDecl,root->objc,scopeName,funcType,funcName,
                funcArgs,funcTempList,exceptions
               );
  }
  //printf("scopeName=`%s' funcType=`%s' funcName=`%s' funcArgs=`%s'\n",
  //    scopeName.data(),funcType.data(),funcName.data(),funcArgs.data());

  // the class name can also be a namespace name, we decide this later.
  // if a related class name is specified and the class name could
  // not be derived from the function declaration, then use the
  // related field.
  //printf("scopeName=`%s' className=`%s' namespaceName=`%s'\n",
  //    scopeName.data(),className.data(),namespaceName.data());
  if (!root->relates.isEmpty()) 
  {                             // related member, prefix user specified scope
    isRelated=TRUE;
    if (getClass(root->relates)==0 && !scopeName.isEmpty())
      scopeName= mergeScopes(scopeName,root->relates);
    else 
      scopeName = root->relates.copy();
  }

  if (root->relates.isEmpty() && root->parent && 
      (root->parent->section&Entry::SCOPE_MASK) &&
      !root->parent->name.isEmpty()) // see if we can combine scopeName 
                                     // with the scope in which it was found
  {
    QCString joinedName = root->parent->name+"::"+scopeName;
    if (!scopeName.isEmpty() && 
        (getClass(joinedName) || Doxygen::namespaceSDict[joinedName]))
    {
      scopeName = joinedName;
    }
    else
    {
      scopeName = mergeScopes(root->parent->name,scopeName);
    }
  }
  else // see if we can prefix a namespace or class that is used from the file
  {
     bool ambig;
     FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
     if (fd)
     {
       NamespaceSDict *fnl = fd->getUsedNamespaces();
       if (fnl)
       {
         QCString joinedName;
         NamespaceDef *fnd;
         NamespaceSDict::Iterator nsdi(*fnl);
         for (nsdi.toFirst();(fnd=nsdi.current());++nsdi)
         {
           joinedName = fnd->name()+"::"+scopeName;
           if (Doxygen::namespaceSDict[joinedName])
           {
             scopeName=joinedName;
             break;
           }
         }
       }
     }
  }
  scopeName=stripTemplateSpecifiersFromScope(
      removeRedundantWhiteSpace(scopeName),FALSE,&funcSpec); 

  // funcSpec contains the last template specifiers of the given scope.
  // If this method does not have any template arguments or they are 
  // empty while funcSpec is not empty we assume this is a 
  // specialization of a method. If not, we clear the funcSpec and treat
  // this as a normal method of a template class.
  if (!(root->tArgLists && 
        root->tArgLists->count()>0 &&
        root->tArgLists->first()->count()==0
       )
     ) 
  {
    funcSpec.resize(0);
  }
  
  // split scope into a namespace and a class part
  extractNamespaceName(scopeName,className,namespaceName,TRUE);
  //printf("scopeName=`%s' className=`%s' namespaceName=`%s'\n",
  //       scopeName.data(),className.data(),namespaceName.data());
  
  namespaceName=removeAnonymousScopes(namespaceName);
  //printf("namespaceName=`%s' className=`%s'\n",namespaceName.data(),className.data());
  // merge class and namespace scopes again
  scopeName.resize(0);
  if (!namespaceName.isEmpty())
  {
    if (className.isEmpty())
    {
      scopeName=namespaceName;
    }
    else if (!getClass(className)) // class name only exists in a namespace
    {
      scopeName=namespaceName+"::"+className;
    }
    else
    {
      scopeName=className;
    }
  }
  else if (!className.isEmpty())
  {
    scopeName=className;
  }
  //printf("new scope=`%s'\n",scopeName.data());

  QCString tempScopeName=scopeName;
  ClassDef *cd=getClass(scopeName);
  if (cd)
  {
    if (root->tArgLists) root->tArgLists->first();
    if (funcSpec.isEmpty())
    {
      tempScopeName=cd->qualifiedNameWithTemplateParameters(root->tArgLists);
    }
    else
    {
      tempScopeName=scopeName+funcSpec;
    }
  }
  //printf("scopeName=%s cd=%p root->tArgLists=%p result=%s\n",
  //    scopeName.data(),cd,root->tArgLists,tempScopeName.data());
  
  //printf("scopeName=`%s' className=`%s'\n",scopeName.data(),className.data());
  // rebuild the function declaration (needed to get the scope right).
  if (!scopeName.isEmpty() && !isRelated && !isFriend && !Config_getBool("HIDE_SCOPE_NAMES"))
  {
    if (!funcType.isEmpty())
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
    if (!funcType.isEmpty()) // but with a type
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
  
  if (funcType=="template class" && !funcTempList.isEmpty())
    return;   // ignore explicit template instantiations
  
  Debug::print(Debug::FindMembers,0,
           "findMember() Parse results:\n"
           "  namespaceName=`%s'\n"
           "  className=`%s`\n"
           "  funcType=`%s'\n"
           "  funcSpec=`%s'\n"
           "  funcName=`%s'\n"
           "  funcArgs=`%s'\n"
           "  funcTempList=`%s'\n"
           "  funcDecl=`%s'\n"
           "  related=`%s'\n" 
           "  exceptions=`%s'\n"
           "  isRelated=%d\n"
           "  isFriend=%d\n"
           "  isFunc=%d\n\n",
           namespaceName.data(),className.data(),
           funcType.data(),funcSpec.data(),funcName.data(),funcArgs.data(),funcTempList.data(),
           funcDecl.data(),root->relates.data(),exceptions.data(),isRelated,isFriend,
           isFunc
          );

  MemberName *mn=0;
  if (!funcName.isEmpty()) // function name is valid
  { 
    Debug::print(Debug::FindMembers,0,
                 "1. funcName=`%s'\n",funcName.data());
    if (funcName.left(9)=="operator ") // strip class scope from cast operator
    {
      funcName = substitute(funcName,className+"::","");
    }
    if (!funcTempList.isEmpty()) // try with member specialization
    {
      mn=Doxygen::memberNameSDict[funcName+funcTempList];
    }
    if (mn==0) // try without specialization
    {
      mn=Doxygen::memberNameSDict[funcName];
    }
    if (!isRelated && mn) // function name already found
    {
      Debug::print(Debug::FindMembers,0,
                   "2. member name exists (%d members with this name)\n",mn->count());
      if (!className.isEmpty()) // class name is valid
      {
        if (funcSpec.isEmpty()) // not a member specialization
        {
          int count=0;
          MemberNameIterator mni(*mn);
          MemberDef *md;
          bool memFound=FALSE;
          for (mni.toFirst();!memFound && (md=mni.current());++mni)
          {
            ClassDef *cd=md->getClassDef();
            Debug::print(Debug::FindMembers,0,
                "3. member definition found, "
                "scope needed=`%s' scope=`%s' args=`%s' fileName=%s\n",
                scopeName.data(),cd ? cd->name().data() : "<none>",
                md->argsString(),
                root->fileName.data());
            //printf("Member %s (member scopeName=%s) (this scopeName=%s) classTempList=%s\n",md->name().data(),cd->name().data(),scopeName.data(),classTempList.data());
            bool ambig;
            FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
            NamespaceDef *nd=0;
            if (!namespaceName.isEmpty()) nd=getResolvedNamespace(namespaceName);

            ClassDef *tcd=findClassDefinition(fd,nd,scopeName);
            if (tcd==0 && stripAnonymousNamespaceScope(cd->name())==scopeName)
            {
              // don't be fooled by anonymous scopes
              tcd=cd;
            }
            //printf("Looking for %s inside nd=%s result=%p\n",
            //    scopeName.data(),nd?nd->name().data():"<none>",tcd);

            if (cd && tcd==cd) // member's classes match
            {
              Debug::print(Debug::FindMembers,0,
                  "4. class definition %s found\n",cd->name().data());
              //int ci;
              //ArgumentList *classTemplArgs = cd->templateArguments();
              //ArgumentList *funcTemplArgs  = md->memberDefTemplateArguments();
              //if ((ci=cd->name().find("::"))!=-1) // nested class
              //{
              //  ClassDef *parentClass = getClass(cd->name().left(ci));
              //  if (parentClass) 
              //    classTemplArgs = parentClass->templateArguments();
              //}
              ////printf("cd->name=%s classTemplArgs=%s\n",cd->name().data(),
              ////     argListToString(classTemplArgs).data());


              // get the template parameter lists found at the member declaration
              QList<ArgumentList> declTemplArgs;
              cd->getTemplateParameterLists(declTemplArgs);
              if (md->templateArguments())
              {
                declTemplArgs.append(md->templateArguments());
              }

              // get the template parameter lists found at the member definition
              QList<ArgumentList> *defTemplArgs = root->tArgLists;
              //printf("defTemplArgs=%p\n",defTemplArgs);

              // do we replace the decl argument lists with the def argument lists?
              bool substDone=FALSE;
              ArgumentList *argList=0;

              /* substitute the occurrences of class template names in the 
               * argument list before matching 
               */
              if (declTemplArgs.count()>0 && defTemplArgs &&
                  declTemplArgs.count()==defTemplArgs->count() &&
                  md->argumentList()
                 )
              {
                /* the function definition has template arguments
                 * and the class definition also has template arguments, so
                 * we must substitute the template names of the class by that
                 * of the function definition before matching.
                 */
                argList = new ArgumentList;
                substituteTemplatesInArgList(declTemplArgs,*defTemplArgs,
                    md->argumentList(),argList);

                substDone=TRUE;
              }
              else /* no template arguments, compare argument lists directly */
              {
                argList = md->argumentList();
              }

              Debug::print(Debug::FindMembers,0,
                  "5. matching `%s'<=>`%s' className=%s namespaceName=%s\n",
                  argListToString(argList,TRUE).data(),argListToString(root->argList,TRUE).data(),
                  className.data(),namespaceName.data()
                  );

              bool matching=
                md->isVariable() || md->isTypedef() || // needed for function pointers
                (md->argumentList()==0 && root->argList->count()==0) || 
                matchArguments2(
                    md->getClassDef(),md->getFileDef(),argList, 
                    cd,fd,root->argList,
                    TRUE);

              Debug::print(Debug::FindMembers,0,
                  "6. match results of matchArguments2 = %d\n",matching);

              if (substDone) // found a new argument list
              {
                if (matching) // replace member's argument list
                {
                  md->setDefinitionTemplateParameterLists(root->tArgLists);
                  md->setArgumentList(argList);
                }
                else // no match -> delete argument list
                {
                  delete argList;
                }
              }
              if (matching) 
              {
                //printf("addMemberDocs root->inLine=%d md->isInline()=%d\n",
                //         root->inLine,md->isInline());
                addMemberDocs(root,md,funcDecl,0,overloaded,0/* TODO */);
                count++;
                memFound=TRUE;
              }
            } 
          } 
          if (count==0 && root->parent && root->parent->section==Entry::OBJCIMPL_SEC)
          {
            goto localObjCMethod;
          }
          if (count==0 && !(isFriend && funcType=="class"))
          {
            int candidates=0;
            if (mn->count()>0)
            {
              //printf("Assume template class\n");
              for (mni.toFirst();(md=mni.current());++mni)
              {
                ClassDef *cd=md->getClassDef();
                //printf("cd->name()==%s className=%s\n",cd->name().data(),className.data());
                if (cd!=0 && rightScopeMatch(cd->name(),className)) 
                {
                  if (root->tArgLists && md->templateArguments() &&
                      root->tArgLists->getLast()->count()<=md->templateArguments()->count())
                  { // assume we have found a template specialization
                    // for which there is only a definition, no declaration in
                    // the class. TODO: we should actually check whether
                    // the arguments match!
                    addMethodToClass(root,cd,md->name(),/*cd->name(),*/isFriend);
                    return;
                  }
                  candidates++;
                }
              }
            }

            warn(root->fileName,root->startLine,
                "Warning: no matching class member found for"
                );   

            if (root->tArgLists)
            {
              QListIterator<ArgumentList> alli(*root->tArgLists);
              ArgumentList *al;
              for (;(al=alli.current());++alli)
              {
                warn_cont("  template %s\n",tempArgListToString(al).data());
              }
            }
            QCString fullFuncDecl=funcDecl.copy();
            if (isFunc) fullFuncDecl+=argListToString(root->argList,TRUE);

            warn_cont("  %s\n",fullFuncDecl.data());

            if (candidates>0)
            {
              warn_cont("Possible candidates:\n");
              for (mni.toFirst();(md=mni.current());++mni)
              {
                ClassDef *cd=md->getClassDef();
                if (cd!=0 && rightScopeMatch(cd->name(),className))
                {
                  if (md->templateArguments())
                  {
                    warn_cont("  template %s\n",tempArgListToString(md->templateArguments()).data());
                  }
                  warn_cont("  ");
                  if (md->typeString()) 
                  {
                    warn_cont("%s ",md->typeString());
                  }
                  QCString qScope = cd->qualifiedNameWithTemplateParameters();
                  if (!qScope.isEmpty()) warn_cont("%s::%s",qScope.data(),md->name().data());
                  if (md->argsString()) warn_cont("%s",md->argsString());
                  warn_cont("\n");
                }
              }
            }
          }
        }
        else if (cd) // member specialization
        {
          MemberDef::MemberType mtype=MemberDef::Function;
          ArgumentList *tArgList = new ArgumentList;
          //  getTemplateArgumentsFromName(cd->name()+"::"+funcName,root->tArgLists);
          MemberDef *md=new MemberDef(
              root->fileName,root->startLine,
              funcType,funcName,funcArgs,exceptions,
              root->protection,root->virt,root->stat,FALSE,
              mtype,tArgList,root->argList);
          //printf("new specialized member %s args=`%s'\n",md->name().data(),funcArgs.data());
          md->setTagInfo(root->tagInfo);
          md->setMemberClass(cd);
          md->setTemplateSpecialization(TRUE);
          md->setDefinition(funcDecl);
          md->enableCallGraph(root->callGraph);
          md->setDocumentation(root->doc,root->docFile,root->docLine);
          md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
          md->setDocsForDefinition(!root->proto);
          md->setPrototype(root->proto);
          md->addSectionsToDefinition(root->anchors);
          md->setBodySegment(root->bodyLine,root->endBodyLine);
          bool ambig;
          FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
          md->setBodyDef(fd);
          md->setMemberSpecifiers(root->memSpec);
          md->setMemberGroupId(root->mGrpId);
          mn->append(md);
          cd->insertMember(md);
          md->setRefItems(root->sli);
        }
        else
        {
          //printf("*** Specialized member %s of unknown scope %s%s found!\n",
          //        scopeName.data(),funcName.data(),funcArgs.data());
        }
      }
      else if (overloaded) // check if the function belongs to only one class 
      {
        // for unique overloaded member we allow the class to be
        // omitted, this is to be Qt compatable. Using this should 
        // however be avoided, because it is error prone
        MemberNameIterator mni(*mn);
        MemberDef *md=mni.toFirst();
        ASSERT(md);
        ClassDef *cd=md->getClassDef();
        ASSERT(cd);
        QCString className=cd->name().copy();
        ++mni;
        bool unique=TRUE;
        for (;(md=mni.current());++mni)
        {
          ClassDef *cd=md->getClassDef();
          if (className!=cd->name()) unique=FALSE; 
        } 
        if (unique)
        {
          MemberDef::MemberType mtype;
          if      (root->mtype==Signal)  mtype=MemberDef::Signal;
          else if (root->mtype==Slot)    mtype=MemberDef::Slot;
          else if (root->mtype==DCOP)    mtype=MemberDef::DCOP;
          else                 mtype=MemberDef::Function;
          
          // new overloaded member function
          ArgumentList *tArgList = 
            getTemplateArgumentsFromName(cd->name()+"::"+funcName,root->tArgLists);
          //printf("new related member %s args=`%s'\n",md->name().data(),funcArgs.data());
          MemberDef *md=new MemberDef(
              root->fileName,root->startLine,
              funcType,funcName,funcArgs,exceptions,
              root->protection,root->virt,root->stat,TRUE,
              mtype,tArgList,root->argList);
          md->setTagInfo(root->tagInfo);
          md->setMemberClass(cd);
          md->setDefinition(funcDecl);
          md->enableCallGraph(root->callGraph);
          QCString doc=getOverloadDocs();
          doc+="<p>";
          doc+=root->doc;
          md->setDocumentation(doc,root->docFile,root->docLine);
          md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
          md->setDocsForDefinition(!root->proto);
          md->setPrototype(root->proto);
          md->addSectionsToDefinition(root->anchors);
          md->setBodySegment(root->bodyLine,root->endBodyLine);
          bool ambig;
          FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
          md->setBodyDef(fd);
          md->setMemberSpecifiers(root->memSpec);
          md->setMemberGroupId(root->mGrpId);
          mn->append(md);
          cd->insertMember(md);
          cd->insertUsedFile(root->fileName);
          md->setRefItems(root->sli);
        }
      }
      else // unrelated function with the same name as a member
      {
        if (!findGlobalMember(root,namespaceName,funcName,funcTempList,funcArgs,funcDecl))
        {
          QCString fullFuncDecl=funcDecl.copy();
          if (isFunc) fullFuncDecl+=argListToString(root->argList,TRUE);
          warn(root->fileName,root->startLine,
               "Warning: Cannot determine class for function\n%s",
               fullFuncDecl.data()
              );   
        }
      }
    }
    else if (isRelated && !root->relates.isEmpty())
    {
      Debug::print(Debug::FindMembers,0,"2. related function\n"
              "  scopeName=%s className=%s\n",scopeName.data(),className.data());
      if (className.isEmpty()) className=root->relates.copy();
      ClassDef *cd;
      //printf("scopeName=`%s' className=`%s'\n",scopeName.data(),className.data());
      if ((cd=getClass(scopeName)))
      {
        bool newMember=TRUE; // assume we have a new member
        bool newMemberName=FALSE; 
        bool isDefine=FALSE;
        {
          MemberName *mn = Doxygen::functionNameSDict[funcName];
          if (mn)
          {
            MemberDef *md = mn->first();
            while (md && !isDefine)
            {
              isDefine = isDefine || md->isDefine();
              md = mn->next();
            }
          }
        }

        bool ambig;
        FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);

        if ((mn=Doxygen::memberNameSDict[funcName])==0)
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
              !matchArguments2(rmd->getOuterScope(),rmd->getFileDef(),rmd->argumentList(),
                               cd,fd,root->argList,
                               TRUE);
            if (newMember) rmd=mn->next();
          }
          if (!newMember && rmd) // member already exists as rmd -> add docs
          {
            //printf("addMemberDocs for related member %s\n",root->name.data());
            //rmd->setMemberDefTemplateArguments(root->mtArgList);
            addMemberDocs(root,rmd,funcDecl,0,overloaded);
          }
        }

        if (newMember) // need to create a new member
        {
          MemberDef::MemberType mtype;
          if (isDefine)
            mtype=MemberDef::Define;
          else if (root->mtype==Signal)  
            mtype=MemberDef::Signal;
          else if (root->mtype==Slot) 
            mtype=MemberDef::Slot;
          else if (root->mtype==DCOP)
            mtype=MemberDef::DCOP;
          else
            mtype=MemberDef::Function;

          //printf("New related name `%s' `%d'\n",funcName.data(),
          //    root->argList ? (int)root->argList->count() : -1);

          // new related (member) function
          ArgumentList *tArgList = 
            getTemplateArgumentsFromName(scopeName+"::"+funcName,root->tArgLists);
          MemberDef *md=new MemberDef(
              root->fileName,root->startLine,
              funcType,funcName,funcArgs,exceptions,
              root->protection,root->virt,root->stat,TRUE,
              mtype,tArgList,funcArgs.isEmpty() ? 0 : root->argList);
          md->setTagInfo(root->tagInfo);
          //printf("Related member name=`%s' decl=`%s' bodyLine=`%d'\n",
          //       funcName.data(),funcDecl.data(),root->bodyLine);

          // try to find the matching line number of the body from the
          // global function list 
          bool found=FALSE;
          if (root->bodyLine==-1)
          {
            MemberName *rmn=Doxygen::functionNameSDict[funcName];
            if (rmn)
            {
              MemberDef *rmd=rmn->first();
              while (rmd && !found) // see if we got another member with matching arguments
              {
                // check for matching argument lists
                if (
                    matchArguments2(rmd->getOuterScope(),rmd->getFileDef(),rmd->argumentList(),
                                    cd,fd,root->argList,
                                    TRUE)
                   )
                {
                  found=TRUE;
                }
                if (!found) rmd=rmn->next();
              }
              if (rmd) // member found -> copy line number info
              {
                md->setBodySegment(rmd->getStartBodyLine(),rmd->getEndBodyLine());
                md->setBodyDef(rmd->getBodyDef());
                //md->setBodyMember(rmd);
              }
            }
          }
          if (!found) // line number could not be found or is available in this
                      // entry
          {
            md->setBodySegment(root->bodyLine,root->endBodyLine);
            md->setBodyDef(fd);
          }

          //if (root->mGrpId!=-1) 
          //{
          //  md->setMemberGroup(memberGroupDict[root->mGrpId]);
          //}
          md->setMemberClass(cd);
          md->setMemberSpecifiers(root->memSpec);
          md->setDefinition(funcDecl);
          md->enableCallGraph(root->callGraph);
          md->setDocumentation(root->doc,root->docFile,root->docLine);
          md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
          md->setDocsForDefinition(!root->proto);
          md->setPrototype(root->proto);
          md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          md->addSectionsToDefinition(root->anchors);
          md->setMemberGroupId(root->mGrpId);
          //md->setMemberDefTemplateArguments(root->mtArgList);
          mn->append(md);
          cd->insertMember(md);
          cd->insertUsedFile(root->fileName);
          md->setRefItems(root->sli);
          if (root->relatesDup) md->setRelatedAlso(cd);
          addMemberToGroups(root,md);
          //printf("Adding member=%s\n",md->name().data());
          if (newMemberName)
          {
            //Doxygen::memberNameList.append(mn);
            //Doxygen::memberNameDict.insert(funcName,mn);
            Doxygen::memberNameSDict.append(funcName,mn);
          }
        }
        if (root->relatesDup)
        {
          if (!findGlobalMember(root,namespaceName,funcName,funcTempList,funcArgs,funcDecl))
          {
            QCString fullFuncDecl=funcDecl.copy();
            if (isFunc) fullFuncDecl+=argListToString(root->argList,TRUE);
            warn(root->fileName,root->startLine,
               "Warning: Cannot determine file/namespace for relatedalso function\n%s",
               fullFuncDecl.data()
              );   
          }
        }
      }
      else
      {
        warn_undoc(root->fileName,root->startLine,
                   "Warning: class `%s' for related function `%s' is not "
                   "documented.", 
                   className.data(),funcName.data()
                  );
      }
    }
    else if (root->parent && root->parent->section==Entry::OBJCIMPL_SEC)
    {
localObjCMethod:
      ClassDef *cd;
      //printf("scopeName=`%s' className=`%s'\n",scopeName.data(),className.data());
      if (Config_getBool("EXTRACT_LOCAL_METHODS") && (cd=getClass(scopeName)))
      {
        bool ambig;
        //printf("Local objective C method `%s' of class `%s' found\n",root->name.data(),cd->name().data());
        MemberDef *md=new MemberDef(
            root->fileName,root->startLine,
            funcType,funcName,funcArgs,exceptions,
            root->protection,root->virt,root->stat,FALSE,
            MemberDef::Function,0,root->argList);
        md->setTagInfo(root->tagInfo);
        md->makeImplementationDetail();
        md->setMemberClass(cd);
        md->setDefinition(funcDecl);
        md->enableCallGraph(root->callGraph);
        md->setDocumentation(root->doc,root->docFile,root->docLine);
        md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
        md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
        md->setDocsForDefinition(!root->proto);
        md->setPrototype(root->proto);
        md->addSectionsToDefinition(root->anchors);
        md->setBodySegment(root->bodyLine,root->endBodyLine);
        FileDef *fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
        md->setBodyDef(fd);
        md->setMemberSpecifiers(root->memSpec);
        md->setMemberGroupId(root->mGrpId);
        cd->insertMember(md);
        cd->insertUsedFile(root->fileName);
        md->setRefItems(root->sli);
        if ((mn=Doxygen::memberNameSDict[root->name]))
        {
          mn->append(md);
        }
        else 
        {
          mn = new MemberName(root->name);
          mn->append(md);
          Doxygen::memberNameSDict.append(root->name,mn);
        }
      }
      else
      {
        // local objective C method found for class without interface
      }
    }
    else // unrelated not overloaded member found
    {
      bool globMem = findGlobalMember(root,namespaceName,funcName,funcTempList,funcArgs,funcDecl);
      if (className.isEmpty() && !globMem)
      {
        warn(root->fileName,root->startLine,
             "Warning: class for member `%s' cannot "
             "be found.", funcName.data()
            ); 
      }
      else if (!className.isEmpty() && !globMem)
      {
        warn(root->fileName,root->startLine,
             "Warning: member `%s' of class `%s' cannot be found",
             funcName.data(),className.data());
      }
    }
  }
  else
  {
    // this should not be called
    warn(root->fileName,root->startLine,
         "Warning: member with no name found.");
  }
  return;
} 

//----------------------------------------------------------------------
// find the members corresponding to the different documentation blocks
// that are extracted from the sources.

static void findMemberDocumentation(Entry *root)
{
  int i=-1,l;
  Debug::print(Debug::FindMembers,0,
         "findMemberDocumentation(): root->type=`%s' root->inside=`%s' root->name=`%s' root->args=`%s' section=%x root->memSpec=%d root->mGrpId=%d\n",
          root->type.data(),root->inside.data(),root->name.data(),root->args.data(),root->section,root->memSpec,root->mGrpId
         );
  bool isFunc=TRUE;

  if (root->relatesDup && !root->relates.isEmpty())
  {
     QCString tmp = root->relates;
     root->relates.resize(0);
     findMemberDocumentation(root);
     root->relates = tmp;
  }
  
  if ( // detect func variable/typedef to func ptr
      (i=findFunctionPtr(root->type,&l))!=-1 
     )
  {
    //printf("Fixing function pointer!\n");
    // fix type and argument
    root->args.prepend(root->type.right(root->type.length()-i-l));
    root->type=root->type.left(i+l);
    //printf("Results type=%s,name=%s,args=%s\n",root->type.data(),root->name.data(),root->args.data());
    isFunc=FALSE;
  }
  else if ((root->type.left(8)=="typedef " && root->args.find('(')!=-1)) 
       // detect function types marked as functions
  {
    isFunc=FALSE;
  }

  //printf("Member %s isFunc=%d\n",root->name.data(),isFunc);
  if (root->section==Entry::MEMBERDOC_SEC)
  {
    //printf("Documentation for inline member `%s' found args=`%s'\n",
    //    root->name.data(),root->args.data());
    //if (root->relates.length()) printf("  Relates %s\n",root->relates.data());
    if (root->type.isEmpty())
    {
      findMember(root,root->name+root->args+root->exception,FALSE,isFunc);
    }
    else
    {
      findMember(root,root->type+" "+root->name+root->args+root->exception,FALSE,isFunc);
    }
  }
  else if (root->section==Entry::OVERLOADDOC_SEC) 
  {
    //printf("Overloaded member %s found\n",root->name.data());
    findMember(root,root->name,TRUE,isFunc);
  }
  else if 
    ((root->section==Entry::FUNCTION_SEC      // function
      ||   
      (root->section==Entry::VARIABLE_SEC &&  // variable
      !root->type.isEmpty() &&                // with a type
      compoundKeywordDict.find(root->type)==0 // that is not a keyword 
                                              // (to skip forward declaration of class etc.)
      )
     ) 
    )
  {
    //printf("Documentation for member `%s' found args=`%s' excp=`%s'\n",
    //    root->name.data(),root->args.data(),root->exception.data());
    //if (root->relates.length()) printf("  Relates %s\n",root->relates.data());
    //printf("Inside=%s\n Relates=%s\n",root->inside.data(),root->relates.data());
    if (root->type=="friend class" || root->type=="friend struct" || 
        root->type=="friend union")
    {
      findMember(root,
                 root->type+" "+
                 root->name,
                 FALSE,FALSE);
                  
    }
    else if (!root->type.isEmpty())
    {
      findMember(root,
          root->type+" "+
          root->inside+
          root->name+
          root->args+
          root->exception,
          FALSE,isFunc);
    }
    else
    {
      findMember(root,
          root->inside+
          root->name+
          root->args+
          root->exception,
          FALSE,isFunc);
    }
  }
  else if (root->section==Entry::DEFINE_SEC && !root->relates.isEmpty())
  {
    findMember(root,root->name+root->args,FALSE,!root->args.isEmpty());
  }
  else if (root->section==Entry::VARIABLEDOC_SEC)
  {
    //printf("Documentation for variable %s found\n",root->name.data());
    //if (!root->relates.isEmpty()) printf("  Relates %s\n",root->relates.data());
    findMember(root,root->name,FALSE,FALSE);
  }
  else
  {
    // skip section 
    //printf("skip section\n");
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    if (e->section!=Entry::ENUM_SEC) findMemberDocumentation(e);
  }
}

//----------------------------------------------------------------------

static void findObjCMethodDefinitions(Entry *root)
{
  EntryListIterator eli(*root->sublist);
  Entry *objCImpl;
  for (;(objCImpl=eli.current());++eli)
  {
    if (objCImpl->section==Entry::OBJCIMPL_SEC)
    {
      //printf("Found ObjC class implementation %s\n",objCImpl->name.data());
      EntryListIterator seli(*objCImpl->sublist);
      Entry *objCMethod;
      for (;(objCMethod=seli.current());++seli)
      {
        if (objCMethod->section==Entry::FUNCTION_SEC)
        {
          //Printf("  Found ObjC method definition %s\n",objCMethod->name.data());
          findMember(objCMethod, objCMethod->type+" "+objCImpl->name+"::"+objCMethod->name+" "+objCMethod->args, FALSE,TRUE);
          objCMethod->section=Entry::EMPTY_SEC;
        }
      }
    }
  }
}

//----------------------------------------------------------------------
// find and add the enumeration to their classes, namespaces or files

static void findEnums(Entry *root)
{
  if (root->section==Entry::ENUM_SEC)
    // non anonymous enumeration
  {
    MemberDef      *md=0;
    ClassDef       *cd=0;
    FileDef        *fd=0;
    NamespaceDef   *nd=0;
    MemberNameSDict *mnsd=0;
    bool isGlobal;
    bool isRelated=FALSE;
    //printf("Found enum with name `%s' relates=%s\n",root->name.data(),root->relates.data());
    int i;

    QCString name;
    QCString scope;

    if ((i=root->name.findRev("::"))!=-1) // scope is specified
    {
      scope=root->name.left(i); // extract scope
      name=root->name.right(root->name.length()-i-2); // extract name
      if ((cd=getClass(scope))==0) nd=getResolvedNamespace(scope);
    }
    else // no scope, check the scope in which the docs where found
    {
      if (( root->parent->section & Entry::SCOPE_MASK )
          && !root->parent->name.isEmpty()
         ) // found enum docs inside a compound
      {
        scope=root->parent->name;
        if ((cd=getClass(scope))==0) nd=getResolvedNamespace(scope);
      }
      name=root->name.copy();
    }

    if (!root->relates.isEmpty()) 
    {   // related member, prefix user specified scope
      isRelated=TRUE;
      if (getClass(root->relates)==0 && !scope.isEmpty())
        scope=mergeScopes(scope,root->relates);
      else 
        scope=root->relates.copy();
      if ((cd=getClass(scope))==0) nd=getResolvedNamespace(scope);
    }

    if (cd && !name.isEmpty()) // found a enum inside a compound
    {
      //printf("Enum `%s'::`%s'\n",cd->name(),name.data());
      fd=0;
      mnsd=&Doxygen::memberNameSDict;
      isGlobal=FALSE;
    }
    else if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@') // found enum inside namespace
    {
      mnsd=&Doxygen::functionNameSDict;
      isGlobal=TRUE;
    }
    else // found a global enum
    {
      bool ambig;
      fd=findFileDef(Doxygen::inputNameDict,root->fileName,ambig);
      mnsd=&Doxygen::functionNameSDict;
      isGlobal=TRUE;
    }

    if (!name.isEmpty())
    {
      // new enum type
      md = new MemberDef(
          root->fileName,root->startLine,
          0,name,0,0,
          root->protection,Normal,FALSE,isRelated,MemberDef::Enumeration,
          0,0);
      md->setTagInfo(root->tagInfo);
      if (!isGlobal) md->setMemberClass(cd); else md->setFileDef(fd);
      md->setBodySegment(root->bodyLine,root->endBodyLine);
      bool ambig;
      md->setBodyDef(findFileDef(Doxygen::inputNameDict,root->fileName,ambig));
      //printf("Enum %s definition at line %d of %s: protection=%d\n",
      //    root->name.data(),root->bodyLine,root->fileName.data(),root->protection);
      md->addSectionsToDefinition(root->anchors);
      md->setMemberGroupId(root->mGrpId);
      md->enableCallGraph(root->callGraph);
      md->setRefItems(root->sli);
      //printf("found enum %s nd=%p\n",name.data(),nd);
      bool defSet=FALSE;
      if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
      {
        if (isRelated || Config_getBool("HIDE_SCOPE_NAMES"))
        {
          md->setDefinition(name);  
        }
        else
        {
          md->setDefinition(nd->name()+"::"+name);  
        }
        //printf("definition=%s\n",md->definition());
        defSet=TRUE;
        nd->insertMember(md);
        md->setNamespace(nd);
      }

      // even if we have already added the enum to a namespace, we still
      // also want to add it to other appropriate places such as file
      // or class.
      if (isGlobal)
      {
        if (!defSet) md->setDefinition(name);
        if (fd==0 && root->parent)
        {
          bool ambig;
          QCString filePathName = root->parent->fileName;
          fd=findFileDef(Doxygen::inputNameDict,filePathName,ambig);
        }
        if (fd) 
        {
          fd->insertMember(md);
          md->setFileDef(fd);
        }
      }
      else if (cd)
      {
        if (isRelated || Config_getBool("HIDE_SCOPE_NAMES"))
        {
          md->setDefinition(name);  
        }
        else
        {
          md->setDefinition(cd->name()+"::"+name);  
        }
        cd->insertMember(md);
        cd->insertUsedFile(root->fileName);
      }
      md->setDocumentation(root->doc,root->docFile,root->docLine);
      md->setDocsForDefinition(!root->proto);
      md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);

      //printf("Adding member=%s\n",md->name().data());
      MemberName *mn;
      if ((mn=(*mnsd)[name]))
      {
        // this is used if the same enum is in multiple namespaces/classes
        mn->append(md);
      }
      else // new enum name
      {
        mn = new MemberName(name);
        mn->append(md);
        mnsd->append(name,mn);
        //printf("add %s to new memberName. Now %d members\n",
        //       name.data(),mn->count());
      }
      addMemberToGroups(root,md);

      EntryListIterator eli(*root->sublist);
      Entry *e;
      for (;(e=eli.current());++eli)
      {
        //printf("e->name=%s isRelated=%d\n",e->name.data(),isRelated);
        MemberName *fmn=0;
        MemberNameSDict *emnsd = isRelated ? &Doxygen::functionNameSDict : mnsd;
        if (!e->name.isEmpty() && (fmn=(*emnsd)[e->name])) 
           // get list of members with the same name as the field
        {
          MemberNameIterator fmni(*fmn);
          MemberDef *fmd;
          for (fmni.toFirst(); (fmd=fmni.current()) ; ++fmni) 
          {
            if (fmd->isEnumValue())
            {
              //printf("found enum value with same name\n");
              if (nd && !nd->name().isEmpty() && nd->name().at(0)!='@')
              {
                NamespaceDef *fnd=fmd->getNamespaceDef();
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
              else if (isRelated && cd) // reparent enum value to
                                        // match the enum's scope
              {
                md->insertEnumField(fmd);   // add field def to list
                fmd->setEnumScope(md);      // cross ref with enum name
                fmd->setEnumClassScope(cd); // cross ref with enum name
                fmd->setOuterScope(cd);
                fmd->makeRelated();
                cd->insertMember(fmd);
              }
              else
              {
                ClassDef *fcd=fmd->getClassDef();
                if (fcd==cd) // enum value is inside a class
                {
                  //printf("Inserting enum field %s in enum scope %s\n",
                  //    fmd->name().data(),md->name().data());
                  md->insertEnumField(fmd); // add field def to list
                  fmd->setEnumScope(md);    // cross ref with enum name
                }
              }
            } 
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

static void findEnumDocumentation(Entry *root)
{
  if (root->section==Entry::ENUMDOC_SEC
      && !root->name.isEmpty()
      && root->name[0]!='@'        // skip anonymous enums
     )
  {
    //printf("Found docs for enum with name `%s' in context %s\n",
    //    root->name.data(),root->parent->name.data());
    int i;
    QCString name;
    QCString scope;
    if ((i=root->name.findRev("::"))!=-1) // scope is specified as part of the name
    {
      name=root->name.right(root->name.length()-i-2); // extract name
      scope=root->name.left(i); // extract scope
      //printf("Scope=`%s' Name=`%s'\n",scope.data(),name.data());
    }
    else // just the name
    {
      name=root->name;
    }
    if (( root->parent->section & Entry::SCOPE_MASK )
        && !root->parent->name.isEmpty()
       ) // found enum docs inside a compound
    {
      if (!scope.isEmpty()) scope.prepend("::");
      scope.prepend(root->parent->name);
    }
    ClassDef *cd=getClass(scope);

    if (!name.isEmpty())
    {
      bool found=FALSE;
      if (cd)
      {
        //printf("Enum: scope=`%s' name=`%s'\n",cd->name(),name.data());
        QCString className=cd->name().copy();
        MemberName *mn=Doxygen::memberNameSDict[name];
        if (mn)
        {
          MemberNameIterator mni(*mn);
          MemberDef *md;
          for (mni.toFirst();(md=mni.current()) && !found;++mni)
          {
            ClassDef *cd=md->getClassDef();
            if (cd && cd->name()==className)
            {
              // documentation outside a compound overrides the documentation inside it
              if (!md->documentation() || root->parent->name.isEmpty()) 
              {
                md->setDocumentation(root->doc,root->docFile,root->docLine);
                md->setDocsForDefinition(!root->proto);
              }

              // brief descriptions inside a compound override the documentation 
              // outside it
              if (!md->briefDescription() || !root->parent->name.isEmpty())
              {
                md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
              }

              if (!md->inbodyDocumentation() || !root->parent->name.isEmpty())
              {
                md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
              }

              if (root->mGrpId!=-1 && md->getMemberGroupId()==-1)
              {
                md->setMemberGroupId(root->mGrpId);
              }
              
              md->addSectionsToDefinition(root->anchors);

              GroupDef *gd=md->getGroupDef();
              if (gd==0 &&root->groups->first()!=0) // member not grouped but out-of-line documentation is
              {
                addMemberToGroups(root,md);
              }

              found=TRUE;
            }
          }
        }
        else
        {
          //printf("MemberName %s not found!\n",name.data());
        }
      }
      else // enum outside class 
      {
        //printf("Enum outside class: %s grpId=%d\n",name.data(),root->mGrpId);
        MemberDef  *md;
        MemberName *mn=Doxygen::functionNameSDict[name];
        if (mn && (md=mn->getFirst()))
        {
          md->setDocumentation(root->doc,root->docFile,root->docLine);
          md->setDocsForDefinition(!root->proto);
          md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
          md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
          md->addSectionsToDefinition(root->anchors);
          md->setMemberGroupId(root->mGrpId);
          
          GroupDef *gd=md->getGroupDef();
          if (gd==0 &&root->groups->first()!=0) // member not grouped but out-of-line documentation is
          {
            addMemberToGroups(root,md);
          }
          
          found=TRUE;
        }
      } 
      if (!found)
      {
        warn(root->fileName,root->startLine,
             "Warning: Documentation for undefined enum `%s' found.",
             name.data()
            );
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
static void findDEV(const MemberNameSDict &mnsd)
{
  MemberName *mn;
  MemberNameSDict::Iterator mnli(mnsd);
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
        MemberList *fmdl = md->enumFieldList();
        int documentedEnumValues=0;
        if (fmdl) // enum has values
        {
          MemberListIterator fmni(*fmdl);
          MemberDef *fmd;
          // for each enum value
          for (fmni.toFirst();(fmd=fmni.current());++fmni)
          {
            if (fmd->isLinkableInProject()) documentedEnumValues++;
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
static void findDocumentedEnumValues()
{
  findDEV(Doxygen::memberNameSDict);
  findDEV(Doxygen::functionNameSDict); 
}


//----------------------------------------------------------------------
// computes the relation between all members. For each member `m'
// the members that override the implementation of `m' are searched and
// the member that `m' overrides is searched.

static void computeMemberRelations()
{
  MemberNameSDict::Iterator mnli(Doxygen::memberNameSDict);
  MemberName *mn;
  for ( ; (mn=mnli.current()) ; ++mnli ) // for each member name
  {
    MemberNameIterator mdi(*mn);
    MemberDef *md;
    for ( ; (md=mdi.current()) ; ++mdi ) // for each member with a specific arg list
    {
      MemberNameIterator bmdi(*mn);
      MemberDef *bmd;
      for ( ; (bmd=bmdi.current()) ; ++bmdi ) // for each other member with that signature
      {
        ClassDef *bmcd = bmd->getClassDef();
        ClassDef *mcd  = md->getClassDef();
        //printf("Check relation between `%s'::`%s' (%p) and `%s'::`%s' (%p)\n",
        //       mcd->name().data(),md->name().data(),md,
        //       bmcd->name().data(),bmd->name().data(),bmd
        //      );
        if (md!=bmd && bmcd && mcd && bmcd!=mcd && mcd->isBaseClass(bmcd,TRUE))
        {
          //printf(" Base argList=`%s'\n Super argList=`%s'\n",
          //        argListToString(bmd->argumentList()).data(),
          //        argListToString(md->argumentList()).data()
          //      );
          if ( 
              matchArguments2(bmd->getOuterScope(),bmd->getFileDef(),bmd->argumentList(),
                               md->getOuterScope(), md->getFileDef(), md->argumentList(),
                              TRUE
                             ) 
             )
          {
            //printf("  match found!\n");
            if (mcd && bmcd && 
                mcd->isLinkable() && bmcd->isLinkable()
               )
            {
              MemberDef *rmd;
              if ((rmd=md->reimplements())==0 ||
                  minClassDistance(mcd,bmcd)<minClassDistance(mcd,rmd->getClassDef())
                 )
              {
                //printf("setting (new) reimplements member\n");
                md->setReimplements(bmd);
              }
              //printf("%s: add reimplements member %s\n",mcd->name().data(),bmcd->name().data());
              //md->setImplements(bmd);
              //printf("%s: add reimplementedBy member %s\n",bmcd->name().data(),mcd->name().data());
              bmd->insertReimplementedBy(md);
            }
          }  
        }
      }
    }
  }  
}


//----------------------------------------------------------------------------
//static void computeClassImplUsageRelations()
//{
//  ClassDef *cd;
//  ClassSDict::Iterator cli(Doxygen::classSDict);
//  for (;(cd=cli.current());++cli)
//  {
//    cd->determineImplUsageRelation();
//  }
//}

//----------------------------------------------------------------------------

static void createTemplateInstanceMembers()
{
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  // for each class
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    // that is a template
    QDict<ClassDef> *templInstances = cd->getTemplateInstances();
    if (templInstances)
    {
      QDictIterator<ClassDef> qdi(*templInstances);
      ClassDef *tcd=0;
      // for each instance of the template
      for (qdi.toFirst();(tcd=qdi.current());++qdi)
      {
        tcd->addMembersToTemplateInstance(cd,qdi.currentKey());
      }
    }
  }
}

//----------------------------------------------------------------------------

// builds the list of all members for each class

static void buildCompleteMemberLists()
{
  ClassDef *cd;
  // merge members of categories into the class they extend
  ClassSDict::Iterator cli(Doxygen::classSDict);
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    int i=cd->name().find('(');
    if (i!=-1) // it is an Objective-C category
    {
      QCString baseName=cd->name().left(i);
      ClassDef *baseClass=Doxygen::classSDict.find(baseName);
      if (baseClass)
      {
        //printf("*** merging members of category %s into %s\n",
        //    cd->name().data(),baseClass->name().data());
        baseClass->mergeCategory(cd);
      }
    }
  }
  // merge the member list of base classes into the inherited classes.
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    if (// !cd->isReference() && // not an external class
         cd->subClasses()->count()==0 && // is a root of the hierarchy
         cd->baseClasses()->count()>0) // and has at least one base class
    {
      //printf("*** merging members for %s\n",cd->name().data());
      cd->mergeMembers();
    }
  }
  // now sort the member list of all classes.
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    cd->memberNameInfoSDict()->sort();
  }
}

//----------------------------------------------------------------------------

static void generateFileSources()
{
  if (documentedHtmlFiles==0) return;
  
  if (Doxygen::inputNameList.count()>0)
  {
    FileNameListIterator fnli(Doxygen::inputNameList); 
    FileName *fn;
    for (;(fn=fnli.current());++fnli)
    {
      FileNameIterator fni(*fn);
      FileDef *fd;
      for (;(fd=fni.current());++fni)
      {
        if (fd->generateSourceFile())
        {
          msg("Generating code for file %s...\n",fd->docName().data());
          fd->writeSource(*outputList);
        }
        else if (!fd->isReference() && 
             (Doxygen::parseSourcesNeeded || Config_getBool("CALL_GRAPH")))
        {
          msg("Parsing code for file %s...\n",fd->docName().data());
          fd->parseSource();
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

static void generateFileDocs()
{
  if (documentedHtmlFiles==0) return;
  
  if (Doxygen::inputNameList.count()>0)
  {
    FileNameListIterator fnli(Doxygen::inputNameList);
    FileName *fn;
    for (fnli.toFirst();(fn=fnli.current());++fnli)
    {
      FileNameIterator fni(*fn);
      FileDef *fd;
      for (fni.toFirst();(fd=fni.current());++fni)
      {
        bool doc = fd->isLinkableInProject();
        if (doc)
        {
          msg("Generating docs for file %s...\n",fd->docName().data());
          fd->writeDocumentation(*outputList);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

static void addSourceReferences()
{
  // add source references for class definitions
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    FileDef *fd=cd->getBodyDef();
    if (fd && cd->isLinkableInProject() && cd->getStartBodyLine()!=-1)
    {
      fd->addSourceRef(cd->getStartBodyLine(),cd,0);
    }
  }
  // add source references for namespace definitions
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd=0;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    FileDef *fd=nd->getBodyDef();
    if (fd && nd->isLinkableInProject() && nd->getStartBodyLine()!=-1)
    {
      fd->addSourceRef(nd->getStartBodyLine(),nd,0);
    }
  }
  
  // add source references for member names
  MemberNameSDict::Iterator mnli(Doxygen::memberNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md=0;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      //printf("class member %s\n",md->name().data());
      FileDef *fd=md->getBodyDef();
      if (fd && 
          md->getStartBodyLine()!=-1 &&
          md->isLinkableInProject() &&
          fd->generateSourceFile())
      {
        //printf("Found member `%s' in file `%s' at line `%d' def=%s\n",
        //    md->name().data(),fd->name().data(),md->getStartBodyLine(),md->getOuterScope()->name().data()); 
        fd->addSourceRef(md->getStartBodyLine(),md->getOuterScope(),md);
      }
    }
  }
  MemberNameSDict::Iterator fnli(Doxygen::functionNameSDict);
  for (fnli.toFirst();(mn=fnli.current());++fnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md=0;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      FileDef *fd=md->getBodyDef();
      //printf("member %s body=[%d,%d] fd=%p\n",md->name().data(),
      //    md->getStartBodyLine(),md->getEndBodyLine(),fd);
      if (fd && 
          md->getStartBodyLine()!=-1 && 
          md->isLinkableInProject() && 
          fd->generateSourceFile()
         )
      {
        //printf("Found member `%s' in file `%s' at line `%d' def=%s\n",
        //    md->name().data(),fd->name().data(),md->getStartBodyLine(),md->getOuterScope()->name().data()); 
        fd->addSourceRef(md->getStartBodyLine(),md->getOuterScope(),md);
      }  
    }
  }
}

//----------------------------------------------------------------------------
// generate the documentation of all classes
  
static void generateClassList(ClassSDict &classSDict)
{
  ClassSDict::Iterator cli(classSDict);
  for ( ; cli.current() ; ++cli )
  {
    ClassDef *cd=cli.current();
   
    //printf("cd=%s getOuterScope=%p global=%p\n",cd->name().data(),cd->getOuterScope(),Doxygen::globalScope);
    if (cd->getOuterScope()==0 || // <-- should not happen, but can if we read an old tag file
        cd->getOuterScope()==Doxygen::globalScope // only look at global classes
       ) 
    {
      // skip external references, anonymous compounds and 
      // template instances 
      if ( cd->isLinkableInProject() && cd->templateMaster()==0)
      {
        msg("Generating docs for compound %s...\n",cd->name().data());

        cd->writeDocumentation(*outputList);
        cd->writeMemberList(*outputList);
      }
      // even for undocumented classes, the inner classes can be documented.
      cd->writeDocumentationForInnerClasses(*outputList);
    }
  }
}

static void generateClassDocs()
{
  // write the installdox script if necessary
  if (Config_getBool("GENERATE_HTML") && 
      (Config_getList("TAGFILES").count()>0 || 
       Config_getBool("SEARCHENGINE")
      )
     ) 
  {
    writeInstallScript();
  }
  
  msg("Generating annotated compound index...\n");
  writeAnnotatedIndex(*outputList);

  if (Config_getBool("ALPHABETICAL_INDEX"))
  {
    msg("Generating alphabetical compound index...\n");
    writeAlphabeticalIndex(*outputList);
  }

  msg("Generating hierarchical class index...\n");
  writeHierarchicalIndex(*outputList);

  msg("Generating member index...\n");
  writeClassMemberIndex(*outputList);

  if (Doxygen::exampleSDict->count()>0)
  {
    msg("Generating example index...\n");
  }

  generateClassList(Doxygen::classSDict);
  generateClassList(Doxygen::hiddenClasses);
}

//----------------------------------------------------------------------------

static void inheritDocumentation()
{
  MemberNameSDict::Iterator mnli(Doxygen::memberNameSDict);
  MemberName *mn;
  //int count=0;
  for (;(mn=mnli.current());++mnli)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md;
    for (;(md=mni.current());++mni)
    {
      //printf("%04d Member `%s'\n",count++,md->name().data());
      if (md->documentation().isEmpty() && md->briefDescription().isEmpty())
      { // no documentation yet
        MemberDef *bmd = md->reimplements();
        while (bmd && bmd->documentation().isEmpty() && 
                      bmd->briefDescription().isEmpty()
              )
        { // search up the inheritance tree for a documentation member
          //printf("bmd=%s class=%s\n",bmd->name().data(),bmd->getClassDef()->name().data());
          bmd = bmd->reimplements();
        }
        if (bmd) // copy the documentation from the reimplemented member
        {
          md->setInheritsDocsFrom(bmd);
          md->setDocumentation(bmd->documentation(),bmd->docFile(),bmd->docLine());
          md->setDocsForDefinition(bmd->isDocsForDefinition());
          md->setBriefDescription(bmd->briefDescription(),bmd->briefFile(),bmd->briefLine());
          md->setInbodyDocumentation(bmd->inbodyDocumentation(),bmd->inbodyFile(),bmd->inbodyLine());
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

static void combineUsingRelations()
{
  // for each file
  FileNameListIterator fnli(Doxygen::inputNameList);
  FileName *fn;
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (fni.toFirst();(fd=fni.current());++fni)
    {
      fd->visited=FALSE;
    }
  }
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (fni.toFirst();(fd=fni.current());++fni)
    {
      fd->combineUsingRelations();
    }
  }

  // for each namespace
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (nli.toFirst() ; (nd=nli.current()) ; ++nli )
  {
    nd->visited=FALSE;
  }
  for (nli.toFirst() ; (nd=nli.current()) ; ++nli )
  {
    nd->combineUsingRelations();
  }
}

//----------------------------------------------------------------------------
  
static void addMembersToMemberGroup()
{
  // for each class
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  for ( ; (cd=cli.current()) ; ++cli )
  {
    cd->addMembersToMemberGroup();
  }
  // for each file
  FileName *fn=Doxygen::inputNameList.first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->addMembersToMemberGroup();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList.next();
  }
  // for each namespace
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for ( ; (nd=nli.current()) ; ++nli )
  {
    nd->addMembersToMemberGroup();
  }
  // for each group
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->addMembersToMemberGroup();
  }
}

//----------------------------------------------------------------------------

static void distributeMemberGroupDocumentation()
{
  // for each class
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  for ( ; (cd=cli.current()) ; ++cli )
  {
    cd->distributeMemberGroupDocumentation();
  }
  // for each file
  FileName *fn=Doxygen::inputNameList.first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->distributeMemberGroupDocumentation();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList.next();
  }
  // for each namespace
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for ( ; (nd=nli.current()) ; ++nli )
  {
    nd->distributeMemberGroupDocumentation();
  }
  // for each group
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->distributeMemberGroupDocumentation();
  }
}

//----------------------------------------------------------------------------

static void findSectionsInDocumentation()
{
  // for each class
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  for ( ; (cd=cli.current()) ; ++cli )
  {
    cd->findSectionsInDocumentation();
  }
  // for each file
  FileName *fn=Doxygen::inputNameList.first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      fd->findSectionsInDocumentation();
      fd=fn->next();
    }
    fn=Doxygen::inputNameList.next();
  }
  // for each namespace
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for ( ; (nd=nli.current()) ; ++nli )
  {
    nd->findSectionsInDocumentation();
  }
  // for each group
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    gd->findSectionsInDocumentation();
  }
  // for each page
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    pd->findSectionsInDocumentation();
  }
  if (Doxygen::mainPage) Doxygen::mainPage->findSectionsInDocumentation();
}

static void flushCachedTemplateRelations()
{
  // remove all references to classes from the cache
  // as there can be new template instances in the inheritance path
  // to this class. Optimization: only remove those classes that
  // have inheritance instances as direct or indirect sub classes.
  QCacheIterator<LookupInfo> ci(Doxygen::lookupCache);
  LookupInfo *li=0;
  for (ci.toFirst();(li=ci.current());++ci)
  {
    if (li->classDef)
    {
      Doxygen::lookupCache.remove(ci.currentKey());
    }
  }
  // remove all cached typedef resolutions whose target is a
  // template class as this may now be a template instance
  MemberNameSDict::Iterator fnli(Doxygen::functionNameSDict);
  MemberName *fn;
  for (;(fn=fnli.current());++fnli) // for each global function name
  {
    MemberNameIterator fni(*fn);
    MemberDef *fmd;
    for (;(fmd=fni.current());++fni) // for each function with that name
    {
      if (fmd->isTypedefValCached())
      {
        ClassDef *cd = fmd->getCachedTypedefVal();
        if (cd->isTemplate()) fmd->invalidateTypedefValCache();
      }
    }
  }
  MemberNameSDict::Iterator mnli(Doxygen::memberNameSDict);
  for (;(fn=mnli.current());++mnli) // for each class method name
  {
    MemberNameIterator mni(*fn);
    MemberDef *fmd;
    for (;(fmd=mni.current());++mni) // for each function with that name
    {
      if (fmd->isTypedefValCached())
      {
        ClassDef *cd = fmd->getCachedTypedefVal();
        if (cd->isTemplate()) fmd->invalidateTypedefValCache();
      }
    }
  }
}


//----------------------------------------------------------------------------

static void findDefineDocumentation(Entry *root)
{
  if ((root->section==Entry::DEFINEDOC_SEC ||
       root->section==Entry::DEFINE_SEC) && !root->name.isEmpty()
     )
  {
    //printf("found define `%s' `%s' brief=`%s' doc=`%s'\n",
    //       root->name.data(),root->args.data(),root->brief.data(),root->doc.data());

    if (root->tagInfo && !root->name.isEmpty()) // define read from a tag file
    {
      MemberDef *md=new MemberDef("<tagfile>",1,
                    "#define",root->name,root->args,0,
                    Public,Normal,FALSE,FALSE,MemberDef::Define,0,0);
      md->setTagInfo(root->tagInfo);
      bool ambig;
      QCString filePathName = root->parent->fileName;
      FileDef *fd=findFileDef(Doxygen::inputNameDict,filePathName,ambig);
      //printf("Searching for `%s' fd=%p\n",filePathName.data(),fd);
      md->setFileDef(fd);
      //printf("Adding member=%s\n",md->name().data());
      MemberName *mn;
      if ((mn=Doxygen::functionNameSDict[root->name]))
      {
        mn->append(md);
      }
      else 
      {
        mn = new MemberName(root->name);
        mn->append(md);
        //Doxygen::functionNameDict.insert(root->name,mn);
        //Doxygen::functionNameList.append(mn);
        Doxygen::functionNameSDict.append(root->name,mn);
      }
    }
    MemberName *mn=Doxygen::functionNameSDict[root->name];
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
            {
              md->setDocumentation(root->doc,root->docFile,root->docLine);
              md->setDocsForDefinition(!root->proto);
            }
            if (md->briefDescription().isEmpty())
            {
              md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
            }
            if (md->inbodyDocumentation().isEmpty())
            {
              md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
            }
            md->setBodySegment(root->bodyLine,root->endBodyLine);
            bool ambig;
            md->setBodyDef(findFileDef(Doxygen::inputNameDict,root->fileName,ambig));
            md->addSectionsToDefinition(root->anchors);
            md->setMaxInitLines(root->initLines);
            md->setRefItems(root->sli);
            if (root->mGrpId!=-1) md->setMemberGroupId(root->mGrpId);
            addMemberToGroups(root,md);
          }
          md=mn->next();
        }
      }
      else if (count>1 && 
               (!root->doc.isEmpty() || 
                !root->brief.isEmpty() || 
                root->bodyLine!=-1
               )
              ) 
        // multiple defines don't know where to add docs
        // but maybe they are in different files together with their documentation
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
              {
                md->setDocumentation(root->doc,root->docFile,root->docLine);
                md->setDocsForDefinition(!root->proto);
              }
              if (md->briefDescription().isEmpty())
              {
                md->setBriefDescription(root->brief,root->briefFile,root->briefLine);
              }
              if (md->inbodyDocumentation().isEmpty())
              {
                md->setInbodyDocumentation(root->inbodyDocs,root->inbodyFile,root->inbodyLine);
              }
              md->setBodySegment(root->bodyLine,root->endBodyLine);
              bool ambig;
              md->setBodyDef(findFileDef(Doxygen::inputNameDict,root->fileName,ambig));
              md->addSectionsToDefinition(root->anchors);
              md->setRefItems(root->sli);
              if (root->mGrpId!=-1) md->setMemberGroupId(root->mGrpId);
              addMemberToGroups(root,md);
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
    else if (!root->doc.isEmpty() || !root->brief.isEmpty()) // define not found
    {
      warn(root->fileName,root->startLine,
           "Warning: documentation for unknown define %s found.\n",
           root->name.data()
          );
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

static void findDirDocumentation(Entry *root)
{
  if (root->section == Entry::DIRDOC_SEC)
  {
    QCString normalizedName = root->name;
    normalizedName = substitute(normalizedName,"\\","/");
    if (normalizedName.at(normalizedName.length()-1)!='/')
    {
      normalizedName+='/';
    }
    DirDef *dir,*matchingDir=0;
    SDict<DirDef>::Iterator sdi(Doxygen::directories);
    for (sdi.toFirst();(dir=sdi.current());++sdi)
    {
      //printf("Dir: %s<->%s\n",dir->name().data(),normalizedName.data());
      if (dir->name().right(normalizedName.length())==normalizedName)
      {
        if (matchingDir)
        {
           warn(root->fileName,root->startLine,
             "Warning: \\dir command matches multiple directories.\n"
             "  Applying the command for directory %s\n"
             "  Ignoring the command for directory %s\n",
             matchingDir->name().data(),dir->name().data()
           );
        }
        else
        {
          matchingDir=dir;
        }
      }
    }
    if (matchingDir)
    {
      //printf("Match for with dir %s\n",matchingDir->name().data());
      matchingDir->setBriefDescription(root->brief,root->briefFile,root->briefLine);
      matchingDir->setDocumentation(root->doc,root->docFile,root->docLine);
      addDirToGroups(root,matchingDir);
    }
    else
    {
      warn(root->fileName,root->startLine,"Warning: No matching "
          "directory found for command \\dir %s\n",root->name.data());
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    findDirDocumentation(e);
  }
}


//----------------------------------------------------------------------------
// create a (sorted) list of separate documentation pages

static void buildPageList(Entry *root)
{
  if (root->section == Entry::PAGEDOC_SEC)
  {
    //printf("buildPageList %s\n",root->name.data());
    if (!root->name.isEmpty())
    {
      addRelatedPage(root);
    }
  }
  else if (root->section == Entry::MAINPAGEDOC_SEC)
  {
    QCString title=root->args.stripWhiteSpace();
    if (title.isEmpty()) title=theTranslator->trMainPage();
    addRefItem(root->sli,"page",
               Config_getBool("GENERATE_TREEVIEW")?"main":"index",
               title
              );
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildPageList(e);
  }
}

static void findMainPage(Entry *root)
{
  if (root->section == Entry::MAINPAGEDOC_SEC)
  {
    if (Doxygen::mainPage==0)
    {
      //printf("Found main page! \n======\n%s\n=======\n",root->doc.data());
      QCString title=root->args.stripWhiteSpace();
      QCString indexName=Config_getBool("GENERATE_TREEVIEW")?"main":"index";
      Doxygen::mainPage = new PageDef(root->fileName,root->startLine,
                              indexName, root->doc,title);
      //setFileNameForSections(root->anchors,"index",Doxygen::mainPage);
      Doxygen::mainPage->setFileName(indexName);
          
      // a page name is a label as well!
      SectionInfo *si=new SectionInfo(
          indexName,
          Doxygen::mainPage->name(),
          Doxygen::mainPage->title(),
          SectionInfo::Section);
      Doxygen::sectionDict.insert(indexName,si);
      Doxygen::mainPage->addSectionsToDefinition(root->anchors);
    }
    else
    {
      warn(root->fileName,root->startLine,
           "Warning: found more than one \\mainpage comment block! Skipping this "
           "block."
          );
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    findMainPage(e);
  }
}

static void computePageRelations(Entry *root)
{
  if ((root->section==Entry::PAGEDOC_SEC || 
       root->section==Entry::MAINPAGEDOC_SEC
      )
      && !root->name.isEmpty()
     )
  {
    PageDef *pd = root->section==Entry::PAGEDOC_SEC ?
                    Doxygen::pageSDict->find(root->name) : 
                    Doxygen::mainPage; 
    if (pd)
    {
      QListIterator<BaseInfo> bii(*root->extends);
      BaseInfo *bi;
      for (bii.toFirst();(bi=bii.current());++bii)
      {
        PageDef *subPd = Doxygen::pageSDict->find(bi->name);
        if (subPd)
        {
          pd->addInnerCompound(subPd);
          //printf("*** Added subpage relation: %s->%s\n",
          //    pd->name().data(),subPd->name().data());
        }
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    computePageRelations(e);
  }
}

static void checkPageRelations()
{
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    Definition *ppd = pd->getOuterScope();
    while (ppd)
    {
      if (ppd==pd)
      {
        err("Warning: page defined at line %d of file %s with label %s is a subpage "
            "of itself! Please remove this cyclic dependency.\n",
            pd->docLine(),pd->docFile().data(),pd->name().data());
        exit(1);
      }
      ppd=ppd->getOuterScope();
    }
  }
}

//----------------------------------------------------------------------------

static void resolveUserReferences()
{
  QDictIterator<SectionInfo> sdi(Doxygen::sectionDict);
  SectionInfo *si;
  for (;(si=sdi.current());++sdi)
  {
    //printf("si->label=`%s' si->definition=%s si->fileName=`%s'\n",
    //        si->label.data(),si->definition?si->definition->name().data():"<none>",
    //        si->fileName.data());
    PageDef *pd=0;

    // hack: the items of a todo/test/bug/deprecated list are all fragments from 
    // different files, so the resulting section's all have the wrong file 
    // name (not from the todo/test/bug/deprecated list, but from the file in 
    // which they are defined). We correct this here by looking at the 
    // generated section labels!
    QDictIterator<RefList> rli(*Doxygen::xrefLists);
    RefList *rl;
    for (rli.toFirst();(rl=rli.current());++rli)
    {
      QCString label="_"+rl->listName(); // "_todo", "_test", ...
      if (si->label.left(label.length())==label)
      {
        si->fileName=rl->listName();
        si->generated=TRUE;
        break;
      }
    }

    //printf("start: si->label=%s si->fileName=%s\n",si->label.data(),si->fileName.data());
    if (!si->generated)
    {
      // if this section is in a page and the page is in a group, then we
      // have to adjust the link file name to point to the group.
      if (!si->fileName.isEmpty() && 
          (pd=Doxygen::pageSDict->find(si->fileName)) &&
          pd->getGroupDef())
      {
        si->fileName=pd->getGroupDef()->getOutputFileBase().copy();
      }

      if (si->definition)
      {
        // TODO: there should be one function in Definition that returns
        // the file to link to, so we can avoid the following tests.
        GroupDef *gd=0;
        if (si->definition->definitionType()==Definition::TypeMember)
        {
          gd = ((MemberDef *)si->definition)->getGroupDef();
        }

        if (gd)
        {
          si->fileName=gd->getOutputFileBase().copy();
        }
        else
        {
          //si->fileName=si->definition->getOutputFileBase().copy();
          //printf("Setting si->fileName to %s\n",si->fileName.data());
        }
      }
    }
    //printf("end: si->label=%s si->fileName=%s\n",si->label.data(),si->fileName.data());
  }
}


//----------------------------------------------------------------------------
// generate all separate documentation pages


static void generatePageDocs()
{
  //printf("documentedPages=%d real=%d\n",documentedPages,Doxygen::pageSDict->count());
  if (documentedPages==0) return;
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (!pd->getGroupDef() && !pd->isReference())
    {
      msg("Generating docs for page %s...\n",pd->name().data());
      pd->writeDocumentation(*outputList);
    }
  }
}

//----------------------------------------------------------------------------
// create a (sorted) list & dictionary of example pages

static void buildExampleList(Entry *root)
{
  if (root->section == Entry::EXAMPLE_SEC)
  {
    if (!root->name.isEmpty()) 
    {
      if (Doxygen::exampleSDict->find(root->name))
      {
        warn(root->fileName,root->startLine,
             "Warning: Example %s was already documented. Ignoring "
             "documentation found here.",
             root->name.data()
            );
      }
      else
      {
        PageDef *pd=new PageDef(root->fileName,root->startLine,
                                root->name,root->doc,root->args);
        pd->setFileName(convertNameToFile(pd->name()+"-example"));
        pd->addSectionsToDefinition(root->anchors);
        //pi->addSections(root->anchors);
        
        Doxygen::exampleSDict->inSort(root->name,pd);
        //we don't add example to groups 
        //addExampleToGroups(root,pd);
      }
    }
  }
  EntryListIterator eli(*root->sublist);
  Entry *e;
  for (;(e=eli.current());++eli)
  {
    buildExampleList(e);
  }
}

//----------------------------------------------------------------------------
// generate the example documentation 

static void generateExampleDocs()
{
  outputList->disable(OutputGenerator::Man);
  PageSDict::Iterator pdi(*Doxygen::exampleSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    msg("Generating docs for example %s...\n",pd->name().data());
    QCString n=pd->getOutputFileBase();
    startFile(*outputList,n,n,pd->name());
    startTitle(*outputList,n);
    outputList->docify(pd->name());
    endTitle(*outputList,n,0);
    outputList->parseDoc(pd->docFile(),                            // file
                         pd->docLine(),                            // startLine
                         pd,                                       // context
                         0,                                        // memberDef
                         pd->documentation()+"\n\n\\include "+pd->name(),          // docs
                         TRUE,                                     // index words
                         TRUE,                                     // is example
                         pd->name()
                        );
    endFile(*outputList);
  }
  outputList->enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------
// generate module pages

static void generateGroupDocs()
{
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    if (!gd->isReference())
    {
      gd->writeDocumentation(*outputList);
    }
  }
}

//----------------------------------------------------------------------------

//static void generatePackageDocs()
//{
//  writePackageIndex(*outputList);
//  
//  if (Doxygen::packageDict.count()>0)
//  {
//    PackageSDict::Iterator pdi(Doxygen::packageDict);
//    PackageDef *pd;
//    for (pdi.toFirst();(pd=pdi.current());++pdi)
//    {
//      pd->writeDocumentation(*outputList);
//    }
//  }
//}

//----------------------------------------------------------------------------
// generate module pages

static void generateNamespaceDocs()
{
  writeNamespaceIndex(*outputList);
  
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd;
  // for each namespace...
  for (;(nd=nli.current());++nli)
  {
    if (nd->isLinkableInProject())
    {
      msg("Generating docs for namespace %s\n",nd->name().data());
      nd->writeDocumentation(*outputList);
    }
    // for each class in the namespace...
    ClassSDict::Iterator cli(*nd->classSDict);
    for ( ; cli.current() ; ++cli )
    {
      ClassDef *cd=cli.current();
      if ( cd->isLinkableInProject() && 
           cd->templateMaster()==0
         ) // skip external references, anonymous compounds and 
           // template instances and nested classes
      {
        msg("Generating docs for compound %s...\n",cd->name().data());

        cd->writeDocumentation(*outputList);
        cd->writeMemberList(*outputList);
      }
      cd->writeDocumentationForInnerClasses(*outputList);
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

//static void generateSearchIndex()
//{
//  if (Config_getBool("SEARCHENGINE") && Config_getBool("GENERATE_HTML"))
//  {
//    // create search index
//    QCString fileName;
//    writeSearchButton(Config_getString("HTML_OUTPUT"));
//
//#if !defined(_WIN32)
//    // create cgi script
//    fileName = Config_getString("HTML_OUTPUT")+"/"+Config_getString("CGI_NAME");
//    QFile f(fileName);
//    if (f.open(IO_WriteOnly))
//    {
//      QTextStream t(&f);
//      t << "#!/bin/sh"   << endl
//        << "DOXYSEARCH=" << Config_getString("BIN_ABSPATH") << "/doxysearch" << endl
//        << "DOXYPATH=\"" << Config_getString("DOC_ABSPATH") << " ";
//
//      QStrList &extDocPaths=Config_getList("EXT_DOC_PATHS");
//      char *s= extDocPaths.first();
//      while (s)
//      {
//        t << s << " ";
//        s=extDocPaths.next();
//      }
//
//      t << "\"" << endl 
//        << "if [ -f $DOXYSEARCH ]" << endl
//        << "then" << endl
//        << "  $DOXYSEARCH $DOXYPATH" << endl 
//        << "else" << endl
//        << "  echo \"Content-Type: text/html\"" << endl
//        << "  echo \"\"" << endl
//        << "  echo \"<h2>Error: $DOXYSEARCH not found. Check cgi script!</h2>\"" << endl
//        << "fi" << endl;
//
//      f.close();
//      struct stat stat_struct;
//      stat(fileName,&stat_struct);
//      chmod(fileName,stat_struct.st_mode|S_IXUSR|S_IXGRP|S_IXOTH);
//    }
//    else
//    {
//      err("Error: Cannot open file %s for writing\n",fileName.data());
//    }
//#else /* Windows platform */
//    // create cgi program
//    fileName = Config_getString("CGI_NAME").copy();
//    if (fileName.right(4)==".cgi") 
//      fileName=fileName.left(fileName.length()-4);
//    fileName+=".c";
//    fileName.prepend(Config_getString("HTML_OUTPUT")+"/");
//    QFile f(fileName);
//    if (f.open(IO_WriteOnly))
//    {
//      QTextStream t(&f);
//      t << "#include <stdio.h>" << endl;
//      t << "#include <stdlib.h>" << endl;
//      t << "#include <process.h>" << endl;
//      t << endl;
//      t << "const char *DOXYSEARCH = \"" << 
//           fixSlashes(Config_getString("BIN_ABSPATH")) << "\\\\doxysearch.exe\";" << endl;
//      t << "const char *DOXYPATH = \"" << 
//           fixSlashes(Config_getString("DOC_ABSPATH")) << "\";" << endl;
//      t << endl;
//      t << "int main(void)" << endl;
//      t << "{" << endl;
//      t << "  char buf[1024];" << endl;
//      t << "  sprintf(buf,\"%s %s\",DOXYSEARCH,DOXYPATH);" << endl; 
//      t << "  if (system(buf))" << endl;
//      t << "  {" << endl;
//      t << "    printf(\"Content-Type: text/html\\n\\n\");" << endl;
//      t << "    printf(\"<h2>Error: failed to execute %s</h2>\\n\",DOXYSEARCH);" << endl;
//      t << "    exit(1);" << endl;
//      t << "  }" << endl;
//      t << "  return 0;" << endl;
//      t << "}" << endl;
//      f.close();
//    }
//    else
//    {
//      err("Error: Cannot open file %s for writing\n",fileName.data());
//    }
//#endif /* !defined(_WIN32) */
//    
//    // create config file
//    fileName = Config_getString("HTML_OUTPUT")+"/search.cfg";
//    f.setName(fileName);
//    if (f.open(IO_WriteOnly))
//    {
//      QTextStream t(&f);
//      t << Config_getString("DOC_URL") << "/" << endl 
//        << Config_getString("CGI_URL") << "/" << Config_getString("CGI_NAME") << endl;
//      f.close();
//    }
//    else
//    {
//      err("Error: Cannot open file %s for writing\n",fileName.data());
//    }
//    //outputList->generateExternalIndex();
//    outputList->pushGeneratorState();
//    outputList->disableAllBut(OutputGenerator::Html);
//    startFile(*outputList,"header"+Doxygen::htmlFileExtension,0,"Search Engine",TRUE);
//    outputList->endPlainFile();
//    outputList->startPlainFile("footer"+Doxygen::htmlFileExtension);
//    endFile(*outputList,TRUE);
//    outputList->popGeneratorState();
//  }
//}

//----------------------------------------------------------------------------

static bool openOutputFile(const char *outFile,QFile &f)
{
  bool fileOpened=FALSE;
  bool writeToStdout=(outFile[0]=='-' && outFile[1]=='\0');
  if (writeToStdout) // write to stdout
  {
    fileOpened = f.open(IO_WriteOnly,stdout);
  }
  else // write to file
  {
    QFileInfo fi(outFile);
    if (fi.exists()) // create a backup
    {
      QDir dir=fi.dir();
      QFileInfo backup(fi.fileName()+".bak");
      if (backup.exists()) // remove existing backup
        dir.remove(backup.fileName());
      dir.rename(fi.fileName(),fi.fileName()+".bak");
    } 
    f.setName(outFile);
    fileOpened = f.open(IO_WriteOnly|IO_Translate);
  }
  return fileOpened;
}

/*! Generate a template version of the configuration file.
 *  If the \a shortList parameter is TRUE a configuration file without
 *  comments will be generated.
 */
static void generateConfigFile(const char *configFile,bool shortList,
                               bool updateOnly=FALSE)
{
  QFile f;
  bool fileOpened=openOutputFile(configFile,f);
  bool writeToStdout=(configFile[0]=='-' && configFile[1]=='\0');
  if (fileOpened)
  {
    QTextStream t(&f);
    Config::instance()->writeTemplate(t,shortList,updateOnly);
    if (!writeToStdout)
    {
      if (!updateOnly)
      {
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
        msg("\n\nConfiguration file `%s' updated.\n\n",configFile);
      }
    }
  }
  else
  {
    err("Error: Cannot open file %s for writing\n",configFile);
    exit(1);
  }
}

//----------------------------------------------------------------------------
// read and parse a tag file

//static bool readLineFromFile(QFile &f,QCString &s)
//{
//  char c=0;
//  s.resize(0);
//  while (!f.atEnd() && (c=f.getch())!='\n') s+=c;
//  return f.atEnd();
//}

//----------------------------------------------------------------------------

static void readTagFile(Entry *root,const char *tl)
{
  QCString tagLine = tl;
  QCString fileName;
  QCString destName;
  int eqPos = tagLine.find('=');
  if (eqPos!=-1) // tag command contains a destination
  {
    fileName = tagLine.left(eqPos).stripWhiteSpace();
    destName = tagLine.right(tagLine.length()-eqPos-1).stripWhiteSpace();
    QFileInfo fi(fileName);
    Doxygen::tagDestinationDict.insert(fi.fileName(),new QCString(destName));
    //printf("insert tagDestination %s->%s\n",fi.fileName().data(),destName.data());
  }
  else
  {
    fileName = tagLine;
  }
    
  QFileInfo fi(fileName);
  if (!fi.exists() || !fi.isFile())
  {
    err("Error: Tag file `%s' does not exist or is not a file. Skipping it...\n",
        fileName.data());
    return;
  }

  if (!destName.isEmpty())
    msg("Reading tag file `%s', location `%s'...\n",fileName.data(),destName.data());
  else
    msg("Reading tag file `%s'...\n",fileName.data());

  parseTagFile(root,fi.absFilePath(),fi.fileName());
}

//----------------------------------------------------------------------------
// returns TRUE if the name of the file represented by `fi' matches
// one of the file patterns in the `patList' list.

static bool patternMatch(QFileInfo *fi,QStrList *patList)
{
  bool found=FALSE;
  if (patList)
  { 
    QCString pattern=patList->first();
    while (!pattern.isEmpty() && !found)
    {
      int i=pattern.find('=');
      if (i!=-1) pattern=pattern.left(i); // strip of the extension specific filter name

      //printf("Matching `%s' against pattern `%s'\n",fi->fileName().data(),pattern);
#if defined(_WIN32) // windows
      QRegExp re(pattern,FALSE,TRUE); // case insensitive match 
#else                // unix
      QRegExp re(pattern,TRUE,TRUE);  // case sensitive match
#endif
      found = found || re.match(fi->fileName())!=-1 || 
                       re.match(fi->filePath())!=-1 ||
                       re.match(fi->absFilePath())!=-1;
      pattern=patList->next();
    }
  }
  return found;
}

//----------------------------------------------------------------------------
// reads a file into an array and filters out any 0x00 and 0x06 bytes,
// because these are special for the parser.

static void copyAndFilterFile(const char *fileName,BufStr &dest)
{
  // try to open file
  int size=0;
  uint oldPos = dest.curPos();
  //printf(".......oldPos=%d\n",oldPos);

  QFileInfo fi(fileName);
  if (!fi.exists()) return;
  QCString filterName = getFileFilter(fileName);
  if (filterName.isEmpty())
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
    QCString cmd=filterName+" \""+fileName+"\"";
    FILE *f=popen(cmd,"r");
    if (!f)
    {
      err("Error: could not execute filter %s\n",filterName.data());
      return;
    }
    const int bufSize=1024;
    char buf[bufSize];
    int numRead;
    while ((numRead=fread(buf,1,bufSize,f))>0) 
    {
      //printf(">>>>>>>>Reading %d bytes\n",numRead);
      dest.addArray(buf,numRead),size+=numRead;
    }
    pclose(f);
  }
  // filter unwanted bytes from the resulting data
  uchar conv[256];
  int i;
  for (i=0;i<256;i++) conv[i]=i;
  conv[0x06]=0x20; // replace the offending characters with spaces
  conv[0x00]=0x20;
  // remove any special markers from the input
  uchar *p=(uchar *)dest.data()+oldPos;
  for (i=0;i<size;i++,p++) *p=conv[*p];
  // and translate CR's
  int newSize=filterCRLF(dest.data()+oldPos,size);
  //printf("filter char at %p size=%d newSize=%d\n",dest.data()+oldPos,size,newSize);
  if (newSize!=size) // we removed chars
  {
    dest.shrink(oldPos+newSize); // resize the array
    //printf(".......resizing from %d to %d result=[%s]\n",oldPos+size,oldPos+newSize,dest.data());
  }
}

//----------------------------------------------------------------------------
static void copyStyleSheet()
{
  QCString &htmlStyleSheet = Config_getString("HTML_STYLESHEET");
  if (!htmlStyleSheet.isEmpty())
  {
    QFile cssf(htmlStyleSheet);
    QFileInfo cssfi(htmlStyleSheet);
    if (cssf.open(IO_ReadOnly))
    {
      QCString destFileName = Config_getString("HTML_OUTPUT")+"/"+cssfi.fileName().data();
      QFile df(destFileName);
      if (df.open(IO_WriteOnly))
      {
        char *buffer = new char[cssf.size()];
        cssf.readBlock(buffer,cssf.size());
        df.writeBlock(buffer,cssf.size());
        df.flush();
        delete[] buffer;
      }
      else
      {
        err("Error: could not write to style sheet %s\n",destFileName.data());
      }
    }
    else
    {
      err("Error: could not open user specified style sheet %s\n",Config_getString("HTML_STYLESHEET").data());
      htmlStyleSheet.resize(0); // revert to the default
    }
  }
}

static void parseFiles(Entry *root)
{
  
  QCString *s=inputFiles.first();
  while (s)
  {
    QCString fileName=*s;
    QCString extension;
    int ei = fileName.findRev('.');
    if (ei!=-1) extension=fileName.right(fileName.length()-ei);
    ParserInterface *parser = Doxygen::parserManager->getParser(extension);

    QFileInfo fi(fileName);
    BufStr preBuf(fi.size()+4096);
    BufStr *bufPtr = &preBuf;

    if (Config_getBool("ENABLE_PREPROCESSING") && 
        parser->needsPreprocessing(extension))
    {
      msg("Preprocessing %s...\n",s->data());
      preprocessFile(fileName,*bufPtr);
    }
    else
    {
      msg("Reading %s...\n",s->data());
      copyAndFilterFile(fileName,*bufPtr);
    }

    bufPtr->addChar('\n'); /* to prevent problems under Windows ? */

    BufStr convBuf(bufPtr->curPos()+1024);

    convertCppComments(&preBuf,&convBuf,fileName);

    convBuf.addChar('\0');

    parser->parseInput(fileName,convBuf.data(),root);

    s=inputFiles.next();
  }
}

//----------------------------------------------------------------------------
// Read all files matching at least one pattern in `patList' in the 
// directory represented by `fi'.
// The directory is read iff the recusiveFlag is set.
// The contents of all files is append to the input string

static int readDir(QFileInfo *fi,
            FileNameList *fnList,
            FileNameDict *fnDict,
            StringDict  *exclDict,
            QStrList *patList,
            QStrList *exclPatList,
            StringList *resultList,
            StringDict *resultDict,
            bool errorIfNotExist,
            bool recursive,
            QDict<void> *killDict
           )
{
  QDir dir((const char *)fi->absFilePath());
  dir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden );
  int totalSize=0;
  //printf("readDir `%s'\n",fi->absFilePath().data());
  //printf("killDict=%p count=%d\n",killDict,killDict->count());
  
  const QFileInfoList *list = dir.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *cfi;
  
  while ((cfi=it.current()))
  {
    if (exclDict==0 || exclDict->find(cfi->absFilePath())==0) 
    { // file should not be excluded
      //printf("killDict->find(%s)\n",cfi->absFilePath().data());
      if (!cfi->exists() || !cfi->isReadable())
      {
        if (errorIfNotExist)
        {
          err("Error: source %s is not a readable file or directory... skipping.\n",cfi->absFilePath().data());
        }
      }
      else if (cfi->isFile() && 
          (!Config_getBool("EXCLUDE_SYMLINKS") || !cfi->isSymLink()) &&
          (patList==0 || patternMatch(cfi,patList)) && 
          !patternMatch(cfi,exclPatList) &&
          (killDict==0 || killDict->find(cfi->absFilePath())==0)
         )
      {
        totalSize+=cfi->size()+cfi->absFilePath().length()+4;
        QCString name=convertToQCString(cfi->fileName());
        //printf("New file %s\n",name.data());
        if (fnDict)
        {
          FileDef  *fd=new FileDef(cfi->dirPath()+"/",name);
          FileName *fn=0;
          if (!name.isEmpty() && (fn=(*fnDict)[name]))
          {
            fn->append(fd);
          }
          else
          {
            fn = new FileName(cfi->absFilePath(),name);
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
        if (killDict) killDict->insert(cfi->absFilePath(),(void *)0x8);
      }
      else if (recursive &&
          (!Config_getBool("EXCLUDE_SYMLINKS") || !cfi->isSymLink()) &&
          cfi->isDir() && cfi->fileName()!="." && 
          cfi->fileName()!="..")
      {
        cfi->setFile(cfi->absFilePath());
        totalSize+=readDir(cfi,fnList,fnDict,exclDict,
            patList,exclPatList,resultList,resultDict,errorIfNotExist,
            recursive,killDict);
      }
    }
    ++it;
  }
  return totalSize;
}


//----------------------------------------------------------------------------
// read a file or all files in a directory and append their contents to the
// input string. The names of the files are appended to the `fiList' list.

static int readFileOrDirectory(const char *s,
                        FileNameList *fnList,
                        FileNameDict *fnDict,
                        StringDict *exclDict,
                        QStrList *patList,
                        QStrList *exclPatList,
                        StringList *resultList,
                        StringDict *resultDict,
                        bool recursive,
                        bool errorIfNotExist=TRUE,
                        QDict<void> *killDict = 0
                       )
{
  //printf("killDict=%p count=%d\n",killDict,killDict->count());
  // strip trailing slashes
  QCString fs = s;
  char lc = fs.at(fs.length()-1);
  if (lc=='/' || lc=='\\') fs = fs.left(fs.length()-1);

  QFileInfo fi(fs);
  //printf("readFileOrDirectory(%s)\n",s);
  int totalSize=0;
  {
    if (exclDict==0 || exclDict->find(fi.absFilePath())==0)
    {
      if (!fi.exists() || !fi.isReadable())
      {
        if (errorIfNotExist)
        {
          err("Error: source %s is not a readable file or directory... skipping.\n",s);
        }
      }
      else if (!Config_getBool("EXCLUDE_SYMLINKS") || !fi.isSymLink())
      {
        if (fi.isFile())
        {
          //printf("killDict->find(%s)\n",fi.absFilePath().data());
          if (killDict==0 || killDict->find(fi.absFilePath())==0)
          {
            totalSize+=fi.size()+fi.absFilePath().length()+4; //readFile(&fi,fiList,input); 
            //fiList->inSort(new FileInfo(fi));
            QCString name=convertToQCString(fi.fileName());
            //printf("New file %s\n",name.data());
            if (fnDict)
            {
              FileDef  *fd=new FileDef(fi.dirPath(TRUE)+"/",name);
              FileName *fn=0;
              if (!name.isEmpty() && (fn=(*fnDict)[name]))
              {
                fn->append(fd);
              }
              else
              {
                fn = new FileName(fi.absFilePath(),name);
                fn->append(fd);
                if (fnList) fnList->inSort(fn);
                fnDict->insert(name,fn);
              }
            }
            QCString *rs=0;
            if (resultList || resultDict)
            {
              rs=new QCString(fi.absFilePath());
              if (resultList) resultList->append(rs);
              if (resultDict) resultDict->insert(fi.absFilePath(),rs);
            }

            if (killDict) killDict->insert(fi.absFilePath(),(void *)0x8);
          }
        }
        else if (fi.isDir()) // readable dir
        {
          totalSize+=readDir(&fi,fnList,fnDict,exclDict,patList,
              exclPatList,resultList,resultDict,errorIfNotExist,
              recursive,killDict);
        }
      }
    }
  }
  return totalSize;
}

//----------------------------------------------------------------------------

static void readFormulaRepository()
{
  QFile f(Config_getString("HTML_OUTPUT")+"/formula.repository");
  if (f.open(IO_ReadOnly)) // open repository
  {
    msg("Reading formula repository...\n");
    QTextStream t(&f);
    QCString line;
    while (!t.eof())
    {
      line=t.readLine();
      int se=line.find(':'); // find name and text separator.
      if (se==-1)
      {
        err("Error: formula.repository is corrupted!\n");
        break;
      }
      else
      {
        QCString formName = line.left(se);
        QCString formText = line.right(line.length()-se-1); 
        Formula *f=new Formula(formText);
        Doxygen::formulaList.append(f);
        Doxygen::formulaDict.insert(formText,f);
        Doxygen::formulaNameDict.insert(formName,f);
      }
    }
  }
}

//----------------------------------------------------------------------------

static QDict<void> aliasesProcessed;

static QCString expandAliasesRec(const QCString s)
{
  QCString result;
  static QRegExp cmdPat("[\\\\@][a-z_A-Z][a-z_A-Z0-9]*");
  QCString value=s;
  int i,p=0,l;
  while ((i=cmdPat.match(value,p,&l))!=-1)
  {
    result+=value.mid(p,i-p);
    QCString cmd=value.mid(i+1,l-1);
    //printf("Found command '%s'\n",cmd.data());
    QCString *aliasText=Doxygen::aliasDict.find(cmd);
    if (aliasesProcessed.find(cmd)==0 && aliasText) // expand the alias
    {
      aliasesProcessed.insert(cmd,(void *)0x8);
      result+=expandAliasesRec(*aliasText);
      aliasesProcessed.remove(cmd);
    }
    else // command is not an alias
    {
      result+=value.mid(i,l);
    }
    p=i+l;
  }
  result+=value.right(value.length()-p);

  //printf("expandAliases '%s'->'%s'\n",s.data(),result.data());
  return result;
}

static void expandAliases()
{
  QDictIterator<QCString> adi(Doxygen::aliasDict);
  QCString *s;
  for (adi.toFirst();(s=adi.current());++adi)
  {
    aliasesProcessed.clear();
    // avoid expanding this command recursively
    aliasesProcessed.insert(adi.currentKey(),(void *)0x8);
    // expand embedded commands
    *s = expandAliasesRec(*s);
  }
}

//----------------------------------------------------------------------------

static void escapeAliases()
{
  QDictIterator<QCString> adi(Doxygen::aliasDict);
  QCString *s;
  for (adi.toFirst();(s=adi.current());++adi)
  {
    QCString value=*s,newValue;
    int in,p=0;
    // for each \n in the alias command value
    while ((in=value.find("\\n",p))!=-1)
    {
      newValue+=value.mid(p,in-p);
      // expand \n's except if \n is part of a built-in command.
      if (value.mid(in,5)!="\\note" && 
          value.mid(in,5)!="\\name" && 
          value.mid(in,10)!="\\namespace" && 
          value.mid(in,14)!="\\nosubgrouping"
         ) 
      {
        newValue+="\\_linebr ";
      }
      else 
      {
        newValue+="\\n";
      }
      p=in+2;
    }
    newValue+=value.mid(p,value.length()-p);
    *s=newValue;
    //printf("Alias %s has value %s\n",adi.currentKey().data(),s->data());
  }
}

//----------------------------------------------------------------------------

static void readAliases()
{ 
  // add aliases to a dictionary
  Doxygen::aliasDict.setAutoDelete(TRUE);
  QStrList &aliasList = Config_getList("ALIASES");
  const char *s=aliasList.first();
  while (s)
  {
    if (Doxygen::aliasDict[s]==0)
    {
      QCString alias=s;
      int i=alias.find('=');
      if (i>0)
      {
        QCString name=alias.left(i).stripWhiteSpace();
        QCString value=alias.right(alias.length()-i-1);
        //printf("Alias: found name=`%s' value=`%s'\n",name.data(),value.data()); 
        if (!name.isEmpty())
        {
          QCString *dn=Doxygen::aliasDict[name];
          if (dn==0) // insert new alias
          {
            Doxygen::aliasDict.insert(name,new QCString(value));
          }
          else // overwrite previous alias
          {
            *dn=value;
          }
        }
      }
    }
    s=aliasList.next();
  }
  expandAliases();
  escapeAliases();
  aliasesProcessed.clear();
}

//----------------------------------------------------------------------------
// print the usage of doxygen

static void usage(const char *name)
{
  msg("Doxygen version %s\nCopyright Dimitri van Heesch 1997-2005\n\n",versionString);
  msg("You can use doxygen in a number of ways:\n\n");
  msg("1) Use doxygen to generate a template configuration file:\n");
  msg("    %s [-s] -g [configName]\n\n",name);
  msg("    If - is used for configName doxygen will write to standard output.\n\n");
  msg("2) Use doxygen to update an old configuration file:\n");
  msg("    %s [-s] -u [configName]\n\n",name);
  msg("3) Use doxygen to generate documentation using an existing ");
  msg("configuration file:\n");
  msg("    %s [configName]\n\n",name);
  msg("    If - is used for configName doxygen will read from standard input.\n\n");
  msg("4) Use doxygen to generate a template style sheet file for RTF, HTML or Latex.\n");
  msg("    RTF:   %s -w rtf styleSheetFile\n",name);
  msg("    HTML:  %s -w html headerFile footerFile styleSheetFile [configFile]\n",name);
  msg("    LaTeX: %s -w latex headerFile styleSheetFile [configFile]\n\n",name);
  msg("5) Use doxygen to generate an rtf extensions file\n");
  msg("    RTF:   %s -e rtf extensionsFile\n\n",name);
  msg("If -s is specified the comments in the config file will be omitted.\n");
  msg("If configName is omitted `Doxyfile' will be used as a default.\n\n");
  exit(1);
}

//----------------------------------------------------------------------------
// read the argument of option `c' from the comment argument list and
// update the option index `optind'.

static const char *getArg(int argc,char **argv,int &optind)
{
  char *s=0;
  if (strlen(&argv[optind][2])>0)
    s=&argv[optind][2];
  else if (optind+1<argc && argv[optind+1][0]!='-')
    s=argv[++optind];
  return s;
}

//----------------------------------------------------------------------------

extern void commentScanTest();

void initDoxygen()
{
#if QT_VERSION >= 200
  setlocale(LC_ALL,"");
  setlocale(LC_NUMERIC,"C");
#endif 
  Doxygen::symbolMap = new SDict<DefinitionList>(1000);
  Doxygen::symbolMap->setAutoDelete(TRUE);
  Doxygen::globalScope = new NamespaceDef("<globalScope>",1,"<globalScope>");
  
  Doxygen::runningTime.start();
  initPreprocessor();

  ParserInterface *defaultParser = new CLanguageScanner;
  Doxygen::parserManager = new ParserManager(defaultParser);
  Doxygen::parserManager->registerParser(".py",new PythonLanguageScanner);

  // register any additional parsers here...

  Doxygen::sectionDict.setAutoDelete(TRUE);
  Doxygen::inputNameList.setAutoDelete(TRUE);
  Doxygen::memberNameSDict.setAutoDelete(TRUE);
  Doxygen::functionNameSDict.setAutoDelete(TRUE);
  Doxygen::hiddenClasses.setAutoDelete(TRUE);
  Doxygen::classSDict.setAutoDelete(TRUE);
  Doxygen::pageSDict->setAutoDelete(TRUE);
  Doxygen::exampleSDict->setAutoDelete(TRUE);
  excludeNameDict.setAutoDelete(TRUE);
  Doxygen::memGrpInfoDict.setAutoDelete(TRUE);
  Doxygen::tagDestinationDict.setAutoDelete(TRUE);
  Doxygen::lookupCache.setAutoDelete(TRUE);
  Doxygen::directories.setAutoDelete(TRUE);
  Doxygen::dirRelations.setAutoDelete(TRUE);
}

void cleanUpDoxygen()
{
  delete Doxygen::inputNameDict;
  delete Doxygen::includeNameDict;
  delete Doxygen::exampleNameDict;
  delete Doxygen::imageNameDict;
  delete Doxygen::dotFileNameDict;
  delete Doxygen::mainPage;
  delete Doxygen::pageSDict;  
  delete Doxygen::exampleSDict;
  delete Doxygen::globalScope;
  delete Doxygen::xrefLists;
  delete Doxygen::parserManager;
  cleanUpPreprocessor();
  Config::deleteInstance();
  QTextCodec::deleteAllCodecs();
  delete theTranslator;
  delete outputList;
  Mappers::freeMappers();
  //delete Doxygen::symbolMap; <- we cannot do this unless all static lists 
  //                              (such as Doxygen::namespaceSDict)
  //                              with objects based on Definition are made
  //                              dynamic first
}

void readConfiguration(int argc, char **argv)
{
  /**************************************************************************
   *             Handle arguments                                           *
   **************************************************************************/

  int optind=1;
  const char *configName=0;
  const char *debugLabel;
  const char *formatName;
  bool genConfig=FALSE;
  bool shortList=FALSE;
  bool updateConfig=FALSE;
  while (optind<argc && argv[optind][0]=='-' && 
               (isalpha(argv[optind][1]) || argv[optind][1]=='?' || 
                argv[optind][1]=='-')
        )
  {
    switch(argv[optind][1])
    {
      case 'g':
        genConfig=TRUE;
        configName=getArg(argc,argv,optind);
        if (strcmp(argv[optind+1],"-")==0)
        { configName="-"; optind++; }
        if (!configName) 
        { configName="Doxyfile"; }
        break;
      case 'd':
        debugLabel=getArg(argc,argv,optind);
        Debug::setFlag(debugLabel);
        break;
      case 's':
        shortList=TRUE;
        break;
      case 'u':
        updateConfig=TRUE;
        break;
      case 'e':
        formatName=getArg(argc,argv,optind);
        if (!formatName)
        {
          err("Error:option -e is missing format specifier rtf.\n");
          cleanUpDoxygen();
          exit(1);
        }
        if (stricmp(formatName,"rtf")==0)
        {
          if (optind+1>=argc)
          {
            err("Error: option \"-e rtf\" is missing an extensions file name\n");
            cleanUpDoxygen();
            exit(1);
          }
          QFile f;
          if (openOutputFile(argv[optind+1],f))
          {
            RTFGenerator::writeExtensionsFile(f);
          }
          cleanUpDoxygen();
          exit(1);
        }
        err("Error: option \"-e\" has invalid format specifier.\n");
        cleanUpDoxygen();
        exit(1);
        break; 
      case 'w':
        formatName=getArg(argc,argv,optind);
        if (!formatName)
        {
          err("Error: option -w is missing format specifier rtf, html or latex\n");
          cleanUpDoxygen();
          exit(1);
        } 
        if (stricmp(formatName,"rtf")==0)
        {
          if (optind+1>=argc)
          {
            err("Error: option \"-w rtf\" is missing a style sheet file name\n");
            cleanUpDoxygen();
            exit(1);
          }
          QFile f;
          if (openOutputFile(argv[optind+1],f))
          {
            RTFGenerator::writeStyleSheetFile(f);
          }
          cleanUpDoxygen();
          exit(1);
        }
        else if (stricmp(formatName,"html")==0)
        {
          if (optind+4<argc)
          {
            if (!Config::instance()->parse(argv[optind+4]))
            {
              err("Error opening or reading configuration file %s!\n",argv[optind+4]);
              cleanUpDoxygen();
              exit(1);
            }
            Config::instance()->substituteEnvironmentVars();
            Config::instance()->convertStrToVal();
            Config::instance()->check();
          }
          else
          {
            Config::instance()->init();
          }
          if (optind+3>=argc)
          {
            err("Error: option \"-w html\" does not have enough arguments\n");
            cleanUpDoxygen();
            exit(1);
          }

          QCString outputLanguage=Config_getEnum("OUTPUT_LANGUAGE");
          if (!setTranslator(outputLanguage))
          {
            err("Error: Output language %s not supported! Using English instead.\n", outputLanguage.data());
          }

          QFile f;
          if (openOutputFile(argv[optind+1],f))
          {
            HtmlGenerator::writeHeaderFile(f);
          }
          f.close();
          if (openOutputFile(argv[optind+2],f))
          {
            HtmlGenerator::writeFooterFile(f);
          }
          f.close();
          if (openOutputFile(argv[optind+3],f))
          {
            HtmlGenerator::writeStyleSheetFile(f);
          } 
          cleanUpDoxygen();
          exit(0);
        }
        else if (stricmp(formatName,"latex")==0)
        {
          if (optind+3<argc) // use config file to get settings
          {
            if (!Config::instance()->parse(argv[optind+3]))
            {
              err("Error opening or reading configuration file %s!\n",argv[optind+3]);
              exit(1);
            }
            Config::instance()->substituteEnvironmentVars();
            Config::instance()->convertStrToVal();
            Config::instance()->check();
          }
          else // use default config
          {
            Config::instance()->init();
          }
          if (optind+2>=argc)
          {
            err("Error: option \"-w latex\" does not have enough arguments\n");
            cleanUpDoxygen();
            exit(1);
          }

          QCString outputLanguage=Config_getEnum("OUTPUT_LANGUAGE");
          if (!setTranslator(outputLanguage))
          {
            err("Error: Output language %s not supported! Using English instead.\n", outputLanguage.data());
          }

          QFile f;
          if (openOutputFile(argv[optind+1],f))
          {
            LatexGenerator::writeHeaderFile(f);
          }
          f.close();
          if (openOutputFile(argv[optind+2],f))
          {
            LatexGenerator::writeStyleSheetFile(f);
          }
          cleanUpDoxygen();
          exit(0);
        }
        else 
        {
          err("Error: Illegal format specifier %s: should be one of rtf, html, or latex\n",formatName);
          cleanUpDoxygen();
          exit(1);
        }
        break;
      case '-':
        if (strcmp(&argv[optind][2],"help")==0)
        {
          usage(argv[0]);
        }
        else if (strcmp(&argv[optind][2],"version")==0)
        {
          msg("%s\n",versionString); 
          cleanUpDoxygen();
          exit(0);
        }
        break;
      case 'b':
        setvbuf(stdout,NULL,_IONBF,0);
        Doxygen::outputToWizard=TRUE;
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

  Config::instance()->init();

  if (genConfig)
  {
    generateConfigFile(configName,shortList);
    cleanUpDoxygen();
    exit(0);
  }

  QFileInfo configFileInfo1("Doxyfile"),configFileInfo2("doxyfile");
  if (optind>=argc)
  { 
    if (configFileInfo1.exists()) 
    {
      configName="Doxyfile";
    }
    else if (configFileInfo2.exists())
    {
      configName="doxyfile";
    }
    else
    {
      err("Doxyfile not found and no input file specified!\n");
      usage(argv[0]);
    }
  }
  else
  {
    QFileInfo fi(argv[optind]);
    if (fi.exists() || strcmp(argv[optind],"-")==0)
    {
      configName=argv[optind];
    }
    else
    {
      err("Error: configuration file %s not found!\n",argv[optind]);
      usage(argv[0]);
    }
  }


  if (!Config::instance()->parse(configName))
  {
    err("Error: could not open or read configuration file %s!\n",configName);
    cleanUpDoxygen();
    exit(1);
  }

  if (updateConfig)
  {
    generateConfigFile(configName,shortList,TRUE);
    cleanUpDoxygen();
    exit(0);
  }
  
  Config::instance()->substituteEnvironmentVars();
  Config::instance()->convertStrToVal();
  Config::instance()->check();
  initWarningFormat();
  QCString outputLanguage=Config_getEnum("OUTPUT_LANGUAGE");
  if (!setTranslator(outputLanguage))
  {
    err("Error: Output language %s not supported! Using English instead.\n",
       outputLanguage.data());
  }
  QStrList &includePath = Config_getList("INCLUDE_PATH");
  char *s=includePath.first();
  while (s)
  {
    QFileInfo fi(s);
    addSearchDir(fi.absFilePath());
    s=includePath.next();
  }

  /* Set the global html file extension. */ 
  Doxygen::htmlFileExtension = Config_getString("HTML_FILE_EXTENSION");

  /* Perlmod wants to know the path to the config file.*/
  QFileInfo configFileInfo(configName);
  setPerlModDoxyfile(configFileInfo.absFilePath());

  Doxygen::xrefLists->setAutoDelete(TRUE);

}

void parseInput()
{
  Doxygen::inputNameDict     = new FileNameDict(10007);
  Doxygen::includeNameDict   = new FileNameDict(10007);
  Doxygen::exampleNameDict   = new FileNameDict(1009);
  Doxygen::exampleNameDict->setAutoDelete(TRUE);
  Doxygen::imageNameDict     = new FileNameDict(257);
  Doxygen::dotFileNameDict   = new FileNameDict(257);

  /**************************************************************************
   *            Initialize some global constants
   **************************************************************************/
  
  int &tabSize = Config_getInt("TAB_SIZE");
  spaces.resize(tabSize+1);
  int sp;for (sp=0;sp<tabSize;sp++) spaces.at(sp)=' ';
  spaces.at(tabSize)='\0';

  compoundKeywordDict.insert("template class",(void *)8);
  compoundKeywordDict.insert("template struct",(void *)8);
  compoundKeywordDict.insert("class",(void *)8);
  compoundKeywordDict.insert("struct",(void *)8);
  compoundKeywordDict.insert("union",(void *)8);
  compoundKeywordDict.insert("interface",(void *)8);
  compoundKeywordDict.insert("exception",(void *)8);

  bool alwaysRecursive = Config_getBool("RECURSIVE");

  /**************************************************************************
   *             Read and preprocess input                                  *
   **************************************************************************/

  // gather names of all files in the include path
  msg("Searching for include files...\n");
  QStrList &includePathList = Config_getList("INCLUDE_PATH");
  char *s=includePathList.first();
  while (s)
  {
    QStrList &pl = Config_getList("INCLUDE_FILE_PATTERNS");
    if (pl.count()==0) 
    {
      pl = Config_getList("FILE_PATTERNS");
    }
    readFileOrDirectory(s,0,Doxygen::includeNameDict,0,&pl,
                        &Config_getList("EXCLUDE_PATTERNS"),0,0,
                        alwaysRecursive);
    s=includePathList.next(); 
  }
  
  msg("Searching for example files...\n");
  QStrList &examplePathList = Config_getList("EXAMPLE_PATH");
  s=examplePathList.first();
  while (s)
  {
    readFileOrDirectory(s,0,Doxygen::exampleNameDict,0,
                        &Config_getList("EXAMPLE_PATTERNS"),
                        0,0,0,
                        (alwaysRecursive || Config_getBool("EXAMPLE_RECURSIVE")));
    s=examplePathList.next(); 
  }

  msg("Searching for images...\n");
  QStrList &imagePathList=Config_getList("IMAGE_PATH");
  s=imagePathList.first();
  while (s)
  {
    readFileOrDirectory(s,0,Doxygen::imageNameDict,0,0,
                        0,0,0,
                        alwaysRecursive);
    s=imagePathList.next(); 
  }

  msg("Searching for dot files...\n");
  QStrList &dotFileList=Config_getList("DOTFILE_DIRS");
  s=dotFileList.first();
  while (s)
  {
    readFileOrDirectory(s,0,Doxygen::dotFileNameDict,0,0,
                        0,0,0,
                        alwaysRecursive);
    s=dotFileList.next(); 
  }

  msg("Searching for files to exclude\n");
  QStrList &excludeList = Config_getList("EXCLUDE");
  s=excludeList.first();
  while (s)
  {
    readFileOrDirectory(s,0,0,0,&Config_getList("FILE_PATTERNS"),
                        0,0,&excludeNameDict,
                        alwaysRecursive,
                        FALSE);
    s=excludeList.next();
  }

  /**************************************************************************
   *             Determine Input Files                                      *
   **************************************************************************/

  msg("Reading input files...\n");
  QDict<void> *killDict = new QDict<void>(10007);
  int inputSize=0;
  QStrList &inputList=Config_getList("INPUT");
  inputFiles.setAutoDelete(TRUE);
  s=inputList.first();
  while (s)
  {
    QCString path=s;
    uint l = path.length();
    // strip trailing slashes
    if (path.at(l-1)=='\\' || path.at(l-1)=='/') path=path.left(l-1);

    inputSize+=readFileOrDirectory(
        path,&Doxygen::inputNameList,
        Doxygen::inputNameDict,&excludeNameDict,
        &Config_getList("FILE_PATTERNS"),
        &Config_getList("EXCLUDE_PATTERNS"),
        &inputFiles,0,
        alwaysRecursive,
        TRUE,
        killDict);
    s=inputList.next();
  }
  delete killDict;
  
  // add predefined macro name to a dictionary
  QStrList &expandAsDefinedList =Config_getList("EXPAND_AS_DEFINED");
  s=expandAsDefinedList.first();
  while (s)
  {
    if (Doxygen::expandAsDefinedDict[s]==0)
    {
      Doxygen::expandAsDefinedDict.insert(s,(void *)666);
    }
    s=expandAsDefinedList.next();
  }

  // read aliases and store them in a dictionary
  readAliases();

  /**************************************************************************
   *             Handle Tag Files                                           *
   **************************************************************************/

  Entry *root=new Entry;
  msg("Reading and parsing tag files\n");
  
  QStrList &tagFileList = Config_getList("TAGFILES");
  s=tagFileList.first();
  while (s)
  {
    readTagFile(root,s);
    s=tagFileList.next();
  }
  
  /**************************************************************************
   *            Check/create output directorties                            *
   **************************************************************************/

  QCString &outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  if (outputDirectory.isEmpty()) 
  {
    outputDirectory=QDir::currentDirPath();
  }
  else
  {
    QDir dir(outputDirectory);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(outputDirectory))
      {
        err("Error: tag OUTPUT_DIRECTORY: Output directory `%s' does not "
	    "exist and cannot be created\n",outputDirectory.data());
        delete root;
        cleanUpDoxygen();
        exit(1);
      }
      else if (!Config_getBool("QUIET"))
      {
	err("Notice: Output directory `%s' does not exist. "
	    "I have created it for you.\n", outputDirectory.data());
      }
      dir.cd(outputDirectory);
    }
    outputDirectory=dir.absPath();
  }

  QCString &htmlOutput = Config_getString("HTML_OUTPUT");
  bool &generateHtml = Config_getBool("GENERATE_HTML");
  if (htmlOutput.isEmpty() && generateHtml)
  {
    htmlOutput=outputDirectory+"/html";
  }
  else if (htmlOutput && htmlOutput[0]!='/' && htmlOutput[1]!=':')
  {
    htmlOutput.prepend(outputDirectory+'/');
  }
  QDir htmlDir(htmlOutput);
  if (generateHtml && !htmlDir.exists() && !htmlDir.mkdir(htmlOutput))
  {
    err("Could not create output directory %s\n",htmlOutput.data());
    delete root;
    cleanUpDoxygen();
    exit(1);
  }
  
  QCString &xmlOutput = Config_getString("XML_OUTPUT");
  bool &generateXml = Config_getBool("GENERATE_XML");
  if (xmlOutput.isEmpty() && generateXml)
  {
    xmlOutput=outputDirectory+"/xml";
  }
  else if (xmlOutput && xmlOutput[0]!='/' && xmlOutput[1]!=':')
  {
    xmlOutput.prepend(outputDirectory+'/');
  }
  QDir xmlDir(xmlOutput);
  if (generateXml && !xmlDir.exists() && !xmlDir.mkdir(xmlOutput))
  {
    err("Could not create output directory %s\n",xmlOutput.data());
    delete root;
    cleanUpDoxygen();
    exit(1);
  }
  
  QCString &latexOutput = Config_getString("LATEX_OUTPUT");
  bool &generateLatex = Config_getBool("GENERATE_LATEX");
  if (latexOutput.isEmpty() && generateLatex)
  {
    latexOutput=outputDirectory+"/latex";
  }
  else if (latexOutput && latexOutput[0]!='/' && latexOutput[1]!=':')
  {
    latexOutput.prepend(outputDirectory+'/');
  }
  QDir latexDir(latexOutput);
  if (generateLatex && !latexDir.exists() && !latexDir.mkdir(latexOutput))
  {
    err("Could not create output directory %s\n",latexOutput.data());
    delete root;
    cleanUpDoxygen();
    exit(1);
  }
  
  QCString &rtfOutput = Config_getString("RTF_OUTPUT");
  bool &generateRtf = Config_getBool("GENERATE_RTF");
  if (rtfOutput.isEmpty() && generateRtf)
  {
    rtfOutput=outputDirectory+"/rtf";
  }
  else if (rtfOutput && rtfOutput[0]!='/' && rtfOutput[1]!=':')
  {
    rtfOutput.prepend(outputDirectory+'/');
  }
  QDir rtfDir(rtfOutput);
  if (generateRtf && !rtfDir.exists() && !rtfDir.mkdir(rtfOutput))
  {
    err("Could not create output directory %s\n",rtfOutput.data());
    delete root;
    cleanUpDoxygen();
    exit(1);
  }

  QCString &manOutput = Config_getString("MAN_OUTPUT");
  bool &generateMan = Config_getBool("GENERATE_MAN");
  if (manOutput.isEmpty() && generateMan)
  {
    manOutput=outputDirectory+"/man";
  }
  else if (manOutput && manOutput[0]!='/' && manOutput[1]!=':')
  {
    manOutput.prepend(outputDirectory+'/');
  }
  QDir manDir(manOutput);
  if (generateMan && !manDir.exists() && !manDir.mkdir(manOutput))
  {
    err("Could not create output directory %s\n",manOutput.data());
    delete root;
    cleanUpDoxygen();
    exit(1);
  }

  // Notice: the order of the function calls below is very important!
  
  if (Config_getBool("GENERATE_HTML"))
  {
    readFormulaRepository();
  }
  
  /**************************************************************************
   *             Read Input Files                                           *
   **************************************************************************/
  
#if 0
  QCString tmpName = Config_getString("OUTPUT_DIRECTORY")+
                     "/doxygen_scratchfile.tmp";

  // read and preprocess all input files
  readFiles(tmpName);

  QFileInfo fi(tmpName);
  if (fi.size()==0)
  {
    err("No input read, no output generated!\n");
    QDir().remove(tmpName);
    delete root;
    cleanUpDoxygen();
    exit(1);
  }
  else
  {
    msg("Read %d bytes\n",fi.size());
  }
  
  msg("Parsing input...\n");
  parseMain(root,tmpName);            // build a tree of entries

  // remove temp file
  QDir().remove(tmpName);
#endif
  parseFiles(root);

  /**************************************************************************
   *             Gather information                                         * 
   **************************************************************************/
  
  msg("Building group list...\n");
  buildGroupList(root);
  organizeSubGroups(root);

  msg("Building directory list...\n");
  buildDirectories();
  findDirDocumentation(root);
  
  msg("Building namespace list...\n");
  buildNamespaceList(root);
  findUsingDirectives(root);
  
  msg("Building file list...\n");
  buildFileList(root);
  //generateFileTree();
  
  msg("Searching for included using directives...\n");
  findIncludedUsingDirectives();

  msg("Building class list...\n");
  buildClassList(root);

  msg("Associating documentation with classes...\n");
  buildClassDocList(root);

  msg("Computing nesting relations for classes...\n");
  resolveClassNestingRelations();
  // calling buildClassList may result in cached relations that
  // become invalid after resolveClassNestingRelation(), that's why
  // we need to clear the cache here
  Doxygen::lookupCache.clear();

  msg("Searching for members imported via using declarations...\n");
  findUsingDeclImports(root);
  findUsingDeclarations(root);

  msg("Building example list...\n");
  buildExampleList(root);
  
  msg("Searching for documented variables...\n");
  buildVarList(root);

  msg("Building member list...\n"); // using class info only !
  buildFunctionList(root);

  msg("Searching for friends...\n");
  findFriends();
  
  msg("Searching for documented defines...\n");
  findDefineDocumentation(root); 
  
  msg("Computing template instances...\n");
  findClassEntries(root);         
  findInheritedTemplateInstances();       
  findUsedTemplateInstances();       

  msg("Flushing cached template relations that have become invalid...\n");
  flushCachedTemplateRelations();
  
  msg("Creating members for template instances...\n");
  createTemplateInstanceMembers();

  msg("Computing class relations...\n");
  computeTemplateClassRelations(); 
  computeClassRelations();        
  classEntries.clear();          

  msg("Searching for enumerations...\n");
  findEnums(root);
  findEnumDocumentation(root);
  
  msg("Searching for member function documentation...\n");
  findObjCMethodDefinitions(root);
  findMemberDocumentation(root); // may introduce new members !
  transferRelatedFunctionDocumentation();
  transferFunctionDocumentation();
  
  msg("Building page list...\n");
  buildPageList(root);

  msg("Search for main page...\n");
  findMainPage(root);

  msg("Computing page relations...\n");
  computePageRelations(root);
  checkPageRelations();

  msg("Sorting lists...\n");
  Doxygen::memberNameSDict.sort();
  Doxygen::functionNameSDict.sort();
  Doxygen::hiddenClasses.sort();
  Doxygen::classSDict.sort();
  
  msg("Freeing entry tree\n");
  delete root;
  
  msg("Determining which enums are documented\n");
  findDocumentedEnumValues();

  msg("Computing member relations...\n");
  computeMemberRelations();

  msg("Building full member lists recursively...\n");
  buildCompleteMemberLists();
  
  msg("Adding members to member groups.\n");
  addMembersToMemberGroup();

  if (Config_getBool("DISTRIBUTE_GROUP_DOC"))
  {
    msg("Distributing member group documentation.\n");
    distributeMemberGroupDocumentation();
  }
  
  msg("Computing member references...\n");
  computeMemberReferences(); 

  if (Config_getBool("INHERIT_DOCS"))
  {
    msg("Inheriting documentation...\n");
    inheritDocumentation();
  }

  // compute the shortest possible names of all files
  // without loosing the uniqueness of the file names.
  msg("Generating disk names...\n");
  Doxygen::inputNameList.generateDiskNames();
  
  msg("Adding source references...\n");
  addSourceReferences();

  msg("Adding todo/test/bug list items...\n");
  addListReferences();

  if (Config_getBool("SHOW_DIRECTORIES"))
  {
    msg("Computing dependencies between directories...\n");
    computeDirDependencies();
  }
}

void generateOutput()
{
  /**************************************************************************
   *            Initialize output generators                                *
   **************************************************************************/

  //// dump all symbols
  //SDict<DefinitionList>::Iterator sdi(Doxygen::symbolMap);
  //DefinitionList *dl;
  //for (sdi.toFirst();(dl=sdi.current());++sdi)
  //{
  //  DefinitionListIterator dli(*dl);
  //  Definition *d;
  //  printf("Symbol: ");
  //  for (dli.toFirst();(d=dli.current());++dli)
  //  {
  //    printf("%s ",d->qualifiedName().data());
  //  }
  //  printf("\n");
  //}

  initDocParser();

  //{
  //  QCString fileName = Config_getString("HTML_OUTPUT")+"/filetree.html";
  //  QFile f(fileName);
  //  if (f.open(IO_WriteOnly))
  //  {
  //    QTextStream t(&f);
  //    t << "<html>\n";
  //    t << "  <head>\n";
  //    t << "    <style type=\"text/css\">\n";
  //    t << "    <!--\n";
  //    t << "    .directory { font-size: 10pt; font-weight: bold; }\n";
  //    t << "    .directory h3 { margin: 0px; margin-top: 1em; font-size: 11pt; }\n";
  //    t << "    .directory p { margin: 0px; white-space: nowrap; }\n";
  //    t << "    .directory div { display: visible; margin: 0px; }\n";
  //    t << "    .directory img { vertical-align: middle; }\n";
  //    t << "    -->\n";
  //    t << "    </style>\n";
  //    t << "  </head>\n";
  //    t << "  <body>\n";
  //    generateFileTree(t);
  //    t << "  </body>\n";
  //    t << "</html>\n";
  //  }
  //}
  
  outputList = new OutputList(TRUE);
  if (Config_getBool("GENERATE_HTML"))  
  {
    outputList->add(new HtmlGenerator);
    HtmlGenerator::init();
    if (Config_getBool("GENERATE_HTMLHELP")) HtmlHelp::getInstance()->initialize();
    if (Config_getBool("GENERATE_TREEVIEW")) FTVHelp::getInstance()->initialize();
    copyStyleSheet();
  }
  if (Config_getBool("GENERATE_LATEX")) 
  {
    outputList->add(new LatexGenerator);
    LatexGenerator::init();
  }
  if (Config_getBool("GENERATE_MAN"))
  {
    outputList->add(new ManGenerator);
    ManGenerator::init();
  }
  if (Config_getBool("GENERATE_RTF"))
  {
    outputList->add(new RTFGenerator);
    RTFGenerator::init();
  }

  if (Config_getBool("USE_HTAGS"))
  {
    Htags::useHtags = TRUE;
    QCString htmldir = Config_getString("HTML_OUTPUT");
    if (!Htags::execute(htmldir))
       err("Error: USE_HTAGS is YES but htags(1) failed. \n");
    if (!Htags::loadFilemap(htmldir))
       err("Error: htags(1) ended normally but failed to load the filemap. \n");
  }
  
  /**************************************************************************
   *                        Generate documentation                          *
   **************************************************************************/

  QFile *tag=0;
  QCString &generateTagFile = Config_getString("GENERATE_TAGFILE");
  if (!generateTagFile.isEmpty())
  {
    tag=new QFile(generateTagFile);
    if (!tag->open(IO_WriteOnly))
    {
      err("Error: cannot open tag file %s for writing\n",
          generateTagFile.data()
         );
      cleanUpDoxygen();
      exit(1);
    }
    Doxygen::tagFile.setDevice(tag);
    Doxygen::tagFile.setEncoding(QTextStream::Latin1);
    Doxygen::tagFile << "<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>" << endl;
    Doxygen::tagFile << "<tagfile>" << endl;
  }

  if (Config_getBool("GENERATE_HTML"))  writeDoxFont(Config_getString("HTML_OUTPUT"));
  if (Config_getBool("GENERATE_LATEX")) writeDoxFont(Config_getString("LATEX_OUTPUT"));
  if (Config_getBool("GENERATE_RTF"))   writeDoxFont(Config_getString("RTF_OUTPUT"));

  msg("Generating style sheet...\n");
  //printf("writing style info\n");
  outputList->writeStyleInfo(0); // write first part
  outputList->disableAllBut(OutputGenerator::Latex);
  outputList->parseText(
            theTranslator->trGeneratedAt(dateToString(TRUE),Config_getString("PROJECT_NAME"))
          );
  outputList->writeStyleInfo(1); // write second part
  //parseText(*outputList,theTranslator->trWrittenBy());
  outputList->writeStyleInfo(2); // write third part
  outputList->parseText(
            theTranslator->trGeneratedAt(dateToString(TRUE),Config_getString("PROJECT_NAME"))
          );
  outputList->writeStyleInfo(3); // write fourth part
  //parseText(*outputList,theTranslator->trWrittenBy());
  outputList->writeStyleInfo(4); // write last part
  outputList->enableAll();
  
  //statistics();
  
  // count the number of documented elements in the lists we have built. 
  // If the result is 0 we do not generate the lists and omit the 
  // corresponding links in the index.
  msg("Counting data structures...\n");
  countDataStructures();
 
  msg("Resolving user defined references...\n");
  resolveUserReferences();

  msg("Combining using relations...\n");
  combineUsingRelations();

  msg("Finding anchors and sections in the documentation...\n");
  findSectionsInDocumentation();

  msg("Generating index page...\n"); 
  writeIndex(*outputList);

  msg("Generating file index...\n");
  writeFileIndex(*outputList);

  msg("Generating example documentation...\n");
  generateExampleDocs();

  msg("Generating file sources...\n");
  if (!Htags::useHtags)
  {
    generateFileSources();
  }
  transferFunctionReferences();

  msg("Generating file documentation...\n");
  generateFileDocs();
  
  msg("Generating class documentation...\n");
  generateClassDocs();
  
  msg("Generating page documentation...\n");
  generatePageDocs();
  
  msg("Generating group documentation...\n");
  generateGroupDocs();

  if (Config_getBool("SHOW_DIRECTORIES"))
  {
    msg("Generating directory documentation...\n");
    generateDirDocs(*outputList);
  }

  msg("Generating namespace index...\n");
  generateNamespaceDocs();
  
  msg("Generating group index...\n");
  writeGroupIndex(*outputList);
 
  if (Config_getBool("SHOW_DIRECTORIES"))
  {
    msg("Generating directory index...\n");
    writeDirIndex(*outputList);
  }

  msg("Generating example index...\n");
  writeExampleIndex(*outputList);
  
  msg("Generating file member index...\n");
  writeFileMemberIndex(*outputList);
  
  msg("Generating namespace member index...\n");
  writeNamespaceMemberIndex(*outputList);

  msg("Generating page index...\n");
  writePageIndex(*outputList);
  
  if (Config_getBool("GENERATE_LEGEND"))
  {
    msg("Generating graph info page...\n");
    writeGraphInfo(*outputList);
  }

  //writeDirDependencyGraph(Config_getString("HTML_OUTPUT"));
  
  if (Config_getBool("GENERATE_RTF"))
  {
    msg("Combining RTF output...\n");
    if (!RTFGenerator::preProcessFileInplace(Config_getString("RTF_OUTPUT"),"refman.rtf"))
    {
      err("An error occurred during post-processing the RTF files!\n");
    }
  }
  
  if (Config_getBool("HAVE_DOT") && Config_getBool("GRAPHICAL_HIERARCHY"))
  {
    msg("Generating graphical class hierarchy...\n");
    writeGraphicalClassHierarchy(*outputList);
  }

  if (Doxygen::formulaList.count()>0 && Config_getBool("GENERATE_HTML"))
  {
    msg("Generating bitmaps for formulas in HTML...\n");
    Doxygen::formulaList.generateBitmaps(Config_getString("HTML_OUTPUT"));
  }
  
  if (Config_getBool("GENERATE_HTML") && Config_getBool("GENERATE_HTMLHELP"))  
  {
    HtmlHelp::getInstance()->finalize();
  }
  if (Config_getBool("GENERATE_HTML") && Config_getBool("GENERATE_TREEVIEW"))  
  {
    FTVHelp::getInstance()->finalize();
  }
  if (!Config_getString("GENERATE_TAGFILE").isEmpty())
  {
    Doxygen::tagFile << "</tagfile>" << endl;
    delete tag;
  }

  if (Config_getBool("GENERATE_HTML") && Config_getBool("DOT_CLEANUP")) removeDoxFont(Config_getString("HTML_OUTPUT"));
  if (Config_getBool("GENERATE_RTF") && Config_getBool("DOT_CLEANUP"))  removeDoxFont(Config_getString("RTF_OUTPUT"));
  if (Config_getBool("GENERATE_XML"))
  {
    msg("Generating XML output...\n");
    generateXML();
  }
  if (Config_getBool("GENERATE_AUTOGEN_DEF"))
  {
    msg("Generating AutoGen DEF output...\n");
    generateDEF();
  }
  if (Config_getBool("GENERATE_PERLMOD"))
  {
    msg("Generating Perl module output...\n");
    generatePerlMod();
  }
  if (Config_getBool("GENERATE_HTMLHELP") && !Config_getString("HHC_LOCATION").isEmpty())
  {
    msg("Running html help compiler...\n");
    QString oldDir = QDir::currentDirPath();
    QDir::setCurrent(Config_getString("HTML_OUTPUT"));
    if (iSystem(Config_getString("HHC_LOCATION"), "index.hhp", FALSE))
    {
      err("Error: failed to run html help compiler on index.hhp");
    }
    QDir::setCurrent(oldDir);
  }
  if (Config_getBool("SEARCHENGINE"))
  {
    msg("Generating search index\n");
    HtmlGenerator::writeSearchPage();
    Doxygen::searchIndex->write(Config_getString("HTML_OUTPUT")+"/search.idx");
  }
  if (Debug::isFlagSet(Debug::Time))
  {
    msg("Total elapsed time: %.3f seconds\n(of which %.3f seconds waiting for external tools to finish)\n",
         ((double)Doxygen::runningTime.elapsed())/1000.0,
         Doxygen::sysElapsedTime
        );
  }
  cleanUpDoxygen();
}

