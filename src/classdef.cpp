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

#include <stdio.h>
#include <qfile.h>
#include <qregexp.h>
#include "classdef.h"
#include "classlist.h"
#include "entry.h"
#include "doxygen.h"
#include "membername.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "diagram.h"
#include "language.h"
#include "htmlhelp.h"
#include "example.h"
#include "outputlist.h"
#include "dot.h"
#include "defargs.h"
#include "debug.h"
#include "docparser.h"
#include "searchindex.h"


// constructs a new class definition
ClassDef::ClassDef(
    const char *defFileName,int defLine,
    const char *nm,CompoundType ct,
    const char *lref,const char *fName,
    bool isSymbol) 
 : Definition(defFileName,defLine,removeRedundantWhiteSpace(nm),0,0,isSymbol) 
{
  m_compType=ct;
  QCString compoundName=compoundTypeString();
  if (fName)
  {
    m_fileName=stripExtension(fName);
  }
  else
  {
    m_fileName=compoundName+m_name;
  }
  if (lref) 
  {
    m_exampleSDict = 0;
  }
  else
  {
    m_exampleSDict = new ExampleSDict(17);
  }
  m_memListFileName=convertNameToFile(compoundName+m_name+"-members");
  m_inherits      = new BaseClassList;
  m_inherits->setAutoDelete(TRUE);
  m_inheritedBy   = new BaseClassList;
  m_inheritedBy->setAutoDelete(TRUE);
  m_allMemberNameInfoSDict = new MemberNameInfoSDict(257);
  m_allMemberNameInfoSDict->setAutoDelete(TRUE);
  visited=FALSE;
  setReference(lref);
  m_incInfo=0;
  m_tempArgs=0;
  m_prot=Public;
  m_nspace=0;
  m_fileDef=0;
  m_usesImplClassDict=0;
  m_usedByImplClassDict=0;
  m_usesIntfClassDict=0;
  memberGroupSDict = new MemberGroupSDict;
  memberGroupSDict->setAutoDelete(TRUE);
  m_innerClasses = new ClassSDict(17);
  m_subGrouping=Config_getBool("SUBGROUPING");
  m_templateInstances = 0;
  m_variableInstances = 0;
  m_templateMaster =0;
  m_templBaseClassNames = 0;
  m_artificial = FALSE;
  m_isAbstract = FALSE;
  m_isStatic = FALSE;
  m_isObjC = FALSE;
  m_membersMerged = FALSE;
  m_categoryOf = 0;
  QCString ns;
  extractNamespaceName(m_name,m_className,ns);
  //printf("m_name=%s m_className=%s ns=%s\n",m_name.data(),m_className.data(),ns.data());

  if (((QCString)defFileName).right(5)!=".java" && 
      guessSection(defFileName)==Entry::SOURCE_SEC)
  {
    m_isLocal=TRUE;
  }
  else
  {
    m_isLocal=FALSE;
  }
}

// destroy the class definition
ClassDef::~ClassDef()
{
  delete m_inherits;
  delete m_inheritedBy;
  delete m_allMemberNameInfoSDict;
  delete m_exampleSDict;
  delete m_usesImplClassDict;
  delete m_usedByImplClassDict;
  delete m_usesIntfClassDict;
  delete m_incInfo;
  delete memberGroupSDict;
  delete m_innerClasses;
  delete m_templateInstances;
  delete m_variableInstances;
  delete m_templBaseClassNames;
  delete m_tempArgs;
}

QCString ClassDef::displayName() const
{
  static bool hideScopeNames = Config_getBool("HIDE_SCOPE_NAMES");
  static bool optimizeOutputForJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  QCString n;
  if (hideScopeNames)
  {
    n=m_className;
  }
  else
  {
    n=qualifiedNameWithTemplateParameters();
  }
  if (optimizeOutputForJava)
  {
    n=substitute(n,"::",".");
  }
  if (m_compType==ClassDef::Protocol && n.right(2)=="-p")
  {
    n="< "+n.left(n.length()-2)+" >";
  }
  return n;
}

// inserts a base class in the inheritance list
void ClassDef::insertBaseClass(ClassDef *cd,const char *n,Protection p,
                               Specifier s,const char *t)
{
  //printf("*** insert base class %s into %s\n",cd->name().data(),name().data());
  //inherits->inSort(new BaseClassDef(cd,p,s,t));
  m_inherits->append(new BaseClassDef(cd,n,p,s,t));
}

// inserts a sub class in the inherited list
void ClassDef::insertSubClass(ClassDef *cd,Protection p,
                                Specifier s,const char *t)
{
  //printf("*** insert sub class %s into %s\n",cd->name().data(),name().data());
  m_inheritedBy->inSort(new BaseClassDef(cd,0,p,s,t));
}

void ClassDef::addMembersToMemberGroup()
{
  ::addMembersToMemberGroup(&pubTypes,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pubMethods,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pubAttribs,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pubSlots,memberGroupSDict,this);
  ::addMembersToMemberGroup(&signals,memberGroupSDict,this);
  ::addMembersToMemberGroup(&dcopMethods,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pubStaticMethods,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pubStaticAttribs,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pacTypes,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pacMethods,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pacAttribs,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pacStaticMethods,memberGroupSDict,this);
  ::addMembersToMemberGroup(&pacStaticAttribs,memberGroupSDict,this);
  ::addMembersToMemberGroup(&proTypes,memberGroupSDict,this);
  ::addMembersToMemberGroup(&proMethods,memberGroupSDict,this);
  ::addMembersToMemberGroup(&proAttribs,memberGroupSDict,this);
  ::addMembersToMemberGroup(&proSlots,memberGroupSDict,this);
  ::addMembersToMemberGroup(&proStaticMethods,memberGroupSDict,this);
  ::addMembersToMemberGroup(&proStaticAttribs,memberGroupSDict,this);
  ::addMembersToMemberGroup(&priTypes,memberGroupSDict,this);
  ::addMembersToMemberGroup(&priMethods,memberGroupSDict,this);
  ::addMembersToMemberGroup(&priAttribs,memberGroupSDict,this);
  ::addMembersToMemberGroup(&priSlots,memberGroupSDict,this);
  ::addMembersToMemberGroup(&priStaticMethods,memberGroupSDict,this);
  ::addMembersToMemberGroup(&priStaticAttribs,memberGroupSDict,this);
  ::addMembersToMemberGroup(&friends,memberGroupSDict,this);
  ::addMembersToMemberGroup(&related,memberGroupSDict,this);
  ::addMembersToMemberGroup(&properties,memberGroupSDict,this);
  ::addMembersToMemberGroup(&events,memberGroupSDict,this);

  // add members inside sections to their groups
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    if (mg->allMembersInSameSection() && m_subGrouping) 
    {
      //printf("addToDeclarationSection(%s)\n",mg->header().data());
      mg->addToDeclarationSection();
    }
  }
}

// adds new member definition to the class
void ClassDef::internalInsertMember(MemberDef *md,
                                    Protection prot,
                                    bool addToAllList
                                   )
{
  //printf("adding %s::%s\n",name().data(),md->name().data());
  if (!isReference())
  {
    /********************************************/
    /* insert member in the declaration section */
    /********************************************/
    if (md->isRelated() && 
        (Config_getBool("EXTRACT_PRIVATE") || prot!=Private))
    {
      if (Config_getBool("SORT_BRIEF_DOCS"))
        related.inSort(md);
      else
        related.append(md);
      md->setSectionList(this,&related);
    }
    else if (md->isFriend())
    {
      if (Config_getBool("SORT_BRIEF_DOCS"))
        friends.inSort(md);
      else
        friends.append(md);
      md->setSectionList(this,&friends);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberDef::Signal: // Qt specific
          if (Config_getBool("SORT_BRIEF_DOCS"))
            signals.inSort(md);
          else
            signals.append(md);
          md->setSectionList(this,&signals);
          break;
        case MemberDef::DCOP:   // KDE2 specific
          if (Config_getBool("SORT_BRIEF_DOCS"))
            dcopMethods.inSort(md);
          else
            dcopMethods.append(md);
          md->setSectionList(this,&dcopMethods);
          break;
        case MemberDef::Property:
          if (Config_getBool("SORT_BRIEF_DOCS"))
            properties.inSort(md);
          else
            properties.append(md);
          md->setSectionList(this,&properties);
          break;
        case MemberDef::Event:
          if (Config_getBool("SORT_BRIEF_DOCS"))
            events.inSort(md);
          else
            events.append(md);
          md->setSectionList(this,&events);
          break;
        case MemberDef::Slot:   // Qt specific
          switch (prot)
          {
            case Protected: 
            case Package: // slots in packages are not possible!
              if (Config_getBool("SORT_BRIEF_DOCS"))
                proSlots.inSort(md);
              else
                proSlots.append(md); 
              md->setSectionList(this,&proSlots);
              break;
              break;
            case Public:    
              if (Config_getBool("SORT_BRIEF_DOCS"))
                pubSlots.inSort(md);
              else
                pubSlots.append(md); 
              md->setSectionList(this,&pubSlots);
              break;
            case Private:   
              if (Config_getBool("SORT_BRIEF_DOCS"))
                priSlots.inSort(md);
              else
                priSlots.append(md);
              md->setSectionList(this,&priSlots);
              break;
          }
          break;
        default: // any of the other members
          if (md->isStatic())
          {
            if (md->isVariable())
            {
              switch (prot)
              {
                case Protected: 
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    proStaticAttribs.inSort(md);
                  else
                    proStaticAttribs.append(md); 
                  md->setSectionList(this,&proStaticAttribs);
                  break;
                case Package: 
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pacStaticAttribs.inSort(md);
                  else
                    pacStaticAttribs.append(md); 
                  md->setSectionList(this,&pacStaticAttribs);
                  break;
                case Public:    
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pubStaticAttribs.inSort(md);
                  else
                    pubStaticAttribs.append(md); 
                  md->setSectionList(this,&pubStaticAttribs);
                  break;
                case Private:   
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    priStaticAttribs.inSort(md);
                  else
                    priStaticAttribs.append(md); 
                  md->setSectionList(this,&priStaticAttribs);
                  break;
              }
            }
            else // function
            {
              switch (prot)
              {
                case Protected: 
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    proStaticMethods.inSort(md);
                  else
                    proStaticMethods.append(md); 
                  md->setSectionList(this,&proStaticMethods);
                  break;
                case Package: 
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pacStaticMethods.inSort(md);
                  else
                    pacStaticMethods.append(md); 
                  md->setSectionList(this,&pacStaticMethods);
                  break;
                case Public:    
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pubStaticMethods.inSort(md);
                  else
                    pubStaticMethods.append(md); 
                  md->setSectionList(this,&pubStaticMethods);
                  break;
                case Private:   
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    priStaticMethods.inSort(md);
                  else
                    priStaticMethods.append(md); 
                  md->setSectionList(this,&priStaticMethods);
                  break;
              }
            }
          }
          else // not static
          {
            if (md->isVariable())
            {
              switch (prot)
              {
                case Protected: 
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    proAttribs.inSort(md);
                  else
                    proAttribs.append(md); 
                  md->setSectionList(this,&proAttribs);
                  break;
                case Package:
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pacAttribs.inSort(md);
                  else
                    pacAttribs.append(md);
                  md->setSectionList(this,&pacAttribs);
                  break;
                case Public:    
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pubAttribs.inSort(md);
                  else
                    pubAttribs.append(md); 
                  md->setSectionList(this,&pubAttribs);
                  break;
                case Private:   
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    priAttribs.inSort(md);
                  else
                    priAttribs.append(md); 
                  md->setSectionList(this,&priAttribs);
                  break;
              }
            }
            else if (md->isTypedef() || md->isEnumerate() || md->isEnumValue())
            {
              switch (prot)
              {
                case Protected: 
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    proTypes.inSort(md);
                  else
                    proTypes.append(md); 
                  md->setSectionList(this,&proTypes); 
                  break;
                case Package: 
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pacTypes.inSort(md);
                  else
                    pacTypes.append(md); 
                  md->setSectionList(this,&pacTypes); 
                  break;
                case Public:    
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pubTypes.inSort(md);
                  else
                    pubTypes.append(md); 
                  md->setSectionList(this,&pubTypes); 
                  break;
                case Private:   
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    priTypes.inSort(md);
                  else
                    priTypes.append(md); 
                  md->setSectionList(this,&priTypes); 
                  break;
              }
            }
            else // member function
            {
              switch (prot)
              {
                case Protected: 
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    proMethods.inSort(md);
                  else
                    proMethods.append(md); 
                  md->setSectionList(this,&proMethods); 
                  break;
                case Package: 
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pacMethods.inSort(md);
                  else
                    pacMethods.append(md); 
                  md->setSectionList(this,&pacMethods); 
                  break;
                case Public:    
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    pubMethods.inSort(md);
                  else
                    pubMethods.append(md); 
                  md->setSectionList(this,&pubMethods); 
                  break;
                case Private:   
                  if (Config_getBool("SORT_BRIEF_DOCS"))
                    priMethods.inSort(md);
                  else
                    priMethods.append(md); 
                  md->setSectionList(this,&priMethods); 
                  break;
              }
            }
          }
          break; 
      }
    }

    /*******************************************************/
    /* insert member in the detailed documentation section */
    /*******************************************************/
    if ((md->isRelated() && 
          (Config_getBool("EXTRACT_PRIVATE") || prot!=Private)
        ) || md->isFriend()
       )
    {
      if (Config_getBool("SORT_MEMBER_DOCS"))
        relatedMembers.inSort(md);
      else
        relatedMembers.append(md);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberDef::Property:
          if (Config_getBool("SORT_MEMBER_DOCS"))
            propertyMembers.inSort(md);
          else
            propertyMembers.append(md);
          break;
        case MemberDef::Event:
          if (Config_getBool("SORT_MEMBER_DOCS"))
            eventMembers.inSort(md);
          else
            eventMembers.append(md);
          break;
        case MemberDef::Signal: // fall through
        case MemberDef::DCOP:
          if (Config_getBool("SORT_MEMBER_DOCS"))
            functionMembers.inSort(md);
          else
            functionMembers.append(md);
          break;
        case MemberDef::Slot:
          switch (prot)
          {
            case Protected: 
            case Package: 
            case Public:    
              if (Config_getBool("SORT_MEMBER_DOCS"))
                functionMembers.inSort(md); 
              else
                functionMembers.append(md);
              break;
            case Private:   
              if (Config_getBool("EXTRACT_PRIVATE"))
              {
                if (Config_getBool("SORT_MEMBER_DOCS"))
                  functionMembers.inSort(md); 
                else
                  functionMembers.append(md);
              }
              break;
          }
          break;
        default: // any of the other members
          if (prot!=Private || Config_getBool("EXTRACT_PRIVATE"))
          {
            switch (md->memberType())
            {
              case MemberDef::Typedef:
                if (Config_getBool("SORT_MEMBER_DOCS"))
                  typedefMembers.inSort(md);
                else
                  typedefMembers.append(md);
                break;
              case MemberDef::Enumeration:
                if (Config_getBool("SORT_MEMBER_DOCS"))
                  enumMembers.inSort(md);
                else
                  enumMembers.append(md);
                break;
              case MemberDef::EnumValue:
                if (Config_getBool("SORT_MEMBER_DOCS"))
                  enumValMembers.inSort(md);
                else
                  enumValMembers.append(md);
                break;
              case MemberDef::Function:
                if (md->isConstructor() || md->isDestructor())
                {
                  constructors.append(md);
                }
                else
                {
                  if (Config_getBool("SORT_MEMBER_DOCS"))
                    functionMembers.inSort(md);
                  else
                    functionMembers.append(md);
                }
                break;
              case MemberDef::Variable:
                if (Config_getBool("SORT_MEMBER_DOCS"))
                  variableMembers.inSort(md);
                else
                  variableMembers.append(md);
                break;
              default:
                err("Unexpected member type %d found!\n",md->memberType());
            }
          }
          break; 
      }
    }

    /*************************************************/
    /* insert member in the appropriate member group */
    /*************************************************/
    // Note: this must be done AFTER inserting the member in the 
    // regular groups
    //addMemberToGroup(md,groupId);
    
  }

  if (md->virtualness()==Pure)
  {
    m_isAbstract=TRUE;
  }

  if (addToAllList && 
      !(Config_getBool("HIDE_FRIEND_COMPOUNDS") &&
        md->isFriend() &&
        (QCString(md->typeString())=="friend class" || 
         QCString(md->typeString())=="friend struct" ||
         QCString(md->typeString())=="friend union")))
  {
    //printf("=======> adding member %s to class %s\n",md->name().data(),name().data());
    MemberInfo *mi = new MemberInfo((MemberDef *)md,
                                     prot,md->virtualness(),FALSE);
    MemberNameInfo *mni=0;
    if ((mni=m_allMemberNameInfoSDict->find(md->name())))
    {
      mni->append(mi);
    }
    else
    {
      mni = new MemberNameInfo(md->name());
      mni->append(mi);
      m_allMemberNameInfoSDict->append(mni->memberName(),mni);
    }
  }
}

void ClassDef::insertMember(MemberDef *md)
{
  internalInsertMember(md,md->protection(),TRUE);
}

// compute the anchors for all members
void ClassDef::computeAnchors()
{
  ClassDef *context = Config_getBool("INLINE_INHERITED_MEMB") ? this : 0;
  setAnchors(context,'a',&pubMethods);
  setAnchors(context,'b',&proMethods);
  setAnchors(context,'c',&pacMethods);
  setAnchors(context,'d',&priMethods);
  setAnchors(context,'e',&pubStaticMethods);
  setAnchors(context,'f',&proStaticMethods);
  setAnchors(context,'g',&pacStaticMethods);
  setAnchors(context,'h',&priStaticMethods);
  setAnchors(context,'i',&pubSlots);
  setAnchors(context,'j',&proSlots);
  setAnchors(context,'k',&priSlots);
  setAnchors(context,'l',&signals);
  setAnchors(context,'m',&related);
  setAnchors(context,'n',&friends);
  setAnchors(context,'o',&pubAttribs);
  setAnchors(context,'p',&proAttribs);
  setAnchors(context,'q',&pacAttribs);
  setAnchors(context,'r',&priAttribs);
  setAnchors(context,'s',&pubStaticAttribs);
  setAnchors(context,'t',&proStaticAttribs);
  setAnchors(context,'u',&pacStaticAttribs);
  setAnchors(context,'v',&priStaticAttribs);
  setAnchors(context,'w',&pubTypes);
  setAnchors(context,'x',&proTypes);
  setAnchors(context,'y',&priTypes);
  setAnchors(context,'z',&dcopMethods);
  setAnchors(context,'0',&properties);
  setAnchors(context,'1',&events);
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->setAnchors(context);
  }
}

void ClassDef::distributeMemberGroupDocumentation()
{
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->distributeMemberGroupDocumentation();
  }
}

void ClassDef::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,0,docFile());
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->findSectionsInDocumentation();
  }
  pubTypes.findSectionsInDocumentation();
  pubMethods.findSectionsInDocumentation();
  pubAttribs.findSectionsInDocumentation();
  pubSlots.findSectionsInDocumentation();
  signals.findSectionsInDocumentation();
  dcopMethods.findSectionsInDocumentation();
  pubStaticMethods.findSectionsInDocumentation();
  pubStaticAttribs.findSectionsInDocumentation();
  pacTypes.findSectionsInDocumentation();
  pacMethods.findSectionsInDocumentation();
  pacAttribs.findSectionsInDocumentation();
  pacStaticMethods.findSectionsInDocumentation();
  pacStaticAttribs.findSectionsInDocumentation();
  proTypes.findSectionsInDocumentation();
  proMethods.findSectionsInDocumentation();
  proAttribs.findSectionsInDocumentation();
  proSlots.findSectionsInDocumentation();
  proStaticMethods.findSectionsInDocumentation();
  proStaticAttribs.findSectionsInDocumentation();
  priTypes.findSectionsInDocumentation();
  priMethods.findSectionsInDocumentation();
  priAttribs.findSectionsInDocumentation();
  priSlots.findSectionsInDocumentation();
  priStaticMethods.findSectionsInDocumentation();
  priStaticAttribs.findSectionsInDocumentation();
  friends.findSectionsInDocumentation();
  related.findSectionsInDocumentation();
  properties.findSectionsInDocumentation();
  events.findSectionsInDocumentation();
}


// add a file name to the used files set
void ClassDef::insertUsedFile(const char *f)
{
  if (m_files.find(f)==-1) m_files.append(f);
  if (m_templateInstances)
  {
    QDictIterator<ClassDef> qdi(*m_templateInstances);
    ClassDef *cd;
    for (qdi.toFirst();(cd=qdi.current());++qdi)
    {
      cd->insertUsedFile(f);
    }
  }
}

static void writeInheritanceSpecifier(OutputList &ol,BaseClassDef *bcd)
{
  if (bcd->prot!=Public || bcd->virt!=Normal)
  {
    ol.startTypewriter();
    ol.docify(" [");
    QStrList sl;
    if (bcd->prot==Protected)    sl.append("protected");
    else if (bcd->prot==Private) sl.append("private");
    if (bcd->virt==Virtual)      sl.append("virtual");
    const char *s=sl.first();
    while (s)
    {
      ol.docify(s);
      s=sl.next();
      if (s) ol.docify(", ");
    }
    ol.docify("]");
    ol.endTypewriter();
  }
}

void ClassDef::setIncludeFile(FileDef *fd,
             const char *includeName,bool local, bool force)
{
  //printf("ClassDef::setIncludeFile(%p,%s,%d,%d)\n",fd,includeName,local,force);
  if (!m_incInfo) m_incInfo=new IncludeInfo;
  if ((includeName && m_incInfo->includeName.isEmpty()) ||
      (fd!=0 && m_incInfo->fileDef==0)
     )
  {
    //printf("Setting file info\n");
    m_incInfo->fileDef     = fd;
    m_incInfo->includeName = includeName;
    m_incInfo->local       = local;
  }
  if (force && includeName) m_incInfo->includeName = includeName;
}

// TODO: fix this: a nested template class can have multiple outer templates
//ArgumentList *ClassDef::outerTemplateArguments() const
//{
//  int ti;
//  ClassDef *pcd=0;
//  int pi=0;
//  if (m_tempArgs) return m_tempArgs;
//  // find the outer most class scope
//  while ((ti=name().find("::",pi))!=-1 && 
//      (pcd=getClass(name().left(ti)))==0
//        ) pi=ti+2;
//  if (pcd)
//  {
//    return pcd->templateArguments();
//  }
//  return 0;
//}

static void searchTemplateSpecs(/*in*/  Definition *d,
                                /*out*/ QList<ArgumentList> &result,
                                /*out*/ QCString &name)
{
  if (d->definitionType()==Definition::TypeClass)
  {
    if (d->getOuterScope())
    {
      searchTemplateSpecs(d->getOuterScope(),result,name);
    }
    ClassDef *cd=(ClassDef *)d;
    if (!name.isEmpty()) name+="::";
    name+=d->localName();
    bool isSpecialization = d->localName().find('<')!=-1;
    if (cd->templateArguments()) 
    {
      result.append(cd->templateArguments());
      if (!isSpecialization)
      {
        name+=tempArgListToString(cd->templateArguments());
      }
    }
  }
  else
  {
    name+=d->qualifiedName();
  }
}

static void writeTemplateSpec(OutputList &ol,Definition *d,
            const QCString &type)
{
  QList<ArgumentList> specs;
  QCString name;
  searchTemplateSpecs(d,specs,name);
  if (specs.count()>0) // class has template scope specifiers
  {
    ol.startSubsubsection(); 
    QListIterator<ArgumentList> spi(specs);
    ArgumentList *al;
    for (spi.toFirst();(al=spi.current());++spi)
    {
      ol.docify("template<");
      Argument *a=al->first();
      while (a)
      {
        ol.docify(a->type);
        if (!a->name.isEmpty())
        {
          ol.docify(" ");
          ol.docify(a->name);
        }
        if (a->defval.length()!=0)
        {
          ol.docify(" = ");
          ol.docify(a->defval);
        } 
        a=al->next();
        if (a) ol.docify(", ");
      }
      ol.docify(">");
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.lineBreak();
      ol.popGeneratorState();
    }
    ol.docify(type.lower()+" "+name);
    ol.endSubsubsection();
    ol.writeString("\n");
  }
}

// write the detailed description for this class
void ClassDef::writeDetailedDescription(OutputList &ol, const QCString &pageType, bool exampleFlag)
{
  if ((!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) || 
      !documentation().isEmpty() || 
      /*(Config_getBool("SOURCE_BROWSER") && startBodyLine!=-1 && bodyDef) ||*/
      exampleFlag)
  {
    ol.writeRuler();
    ol.pushGeneratorState();
      ol.disable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::RTF);
      ol.writeAnchor(0,"_details");
    ol.popGeneratorState();
    ol.startGroupHeader();
    ol.parseText(theTranslator->trDetailedDescription());
    ol.endGroupHeader();
    ol.startTextBlock();
    
    writeTemplateSpec(ol,this,pageType);
    
    // repeat brief description
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF"))
    {
      ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
    }
    if (!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF") &&
        !documentation().isEmpty())
    {
      ol.pushGeneratorState();
        ol.disable(OutputGenerator::Man);
        ol.newParagraph();
        ol.enableAll();
        ol.disableAllBut(OutputGenerator::Man);
        ol.writeString("\n\n");
      ol.popGeneratorState();
    }
    // write documentation
    if (!documentation().isEmpty())
    {
      ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::RTF);
        ol.newParagraph();
      ol.popGeneratorState();
      ol.parseDoc(docFile(),docLine(),this,0,documentation()+"\n",TRUE,FALSE);
    }
    // write examples
    if (exampleFlag)
    {
      ol.startSimpleSect(BaseOutputDocInterface::Examples,0,0,theTranslator->trExamples()+": ");
      ol.writeDescItem();
      ol.newParagraph();
      writeExample(ol,m_exampleSDict);
      ol.endSimpleSect();
    }
    ol.newParagraph();
    writeSourceDef(ol,name());
    ol.endTextBlock();
  }
  else
  {
    writeTemplateSpec(ol,this,pageType);
  }
}
    
// write all documentation for this class
void ClassDef::writeDocumentation(OutputList &ol)
{
  QCString pageType = " ";
  pageType += compoundTypeString();
  toupper(pageType.at(1));
  QCString pageTitle = theTranslator->trCompoundReference(displayName(),
              m_compType == Interface && m_isObjC ? Class : m_compType,
              m_tempArgs != 0);
  
  startFile(ol,getOutputFileBase(),name(),pageTitle);  
  if (getOuterScope()!=Doxygen::globalScope)
  {
    writeNavigationPath(ol);
  }
  startTitle(ol,getOutputFileBase());
  ol.parseText(pageTitle);
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),name());

  if (Config_getBool("SEARCHENGINE"))
  {
    Doxygen::searchIndex->setCurrentDoc(pageTitle,getOutputFileBase());
    Doxygen::searchIndex->addWord(localName(),TRUE);
  }

  ol.startTextBlock();
  
  //printf("Class %s brief=`%s' doc=`%s'\n",name().data(),briefDescription().data(),documentation().data());
  
  bool exampleFlag=hasExamples();
  // write brief description
  if (!briefDescription().isEmpty())
  {
    if (!Config_getBool("DETAILS_AT_TOP")) 
    {
      ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),TRUE,FALSE);
      ol.writeString(" \n");
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.startTextLink(0,"_details");
    
      if (Config_getBool("REPEAT_BRIEF") || 
          !documentation().isEmpty() || 
          exampleFlag
         )
      {
        ol.parseText(theTranslator->trMore());
      }
      ol.endTextLink();
      ol.popGeneratorState();

      ol.pushGeneratorState();
      ol.disable(OutputGenerator::RTF);
      ol.newParagraph();
      ol.popGeneratorState();
    }
  }
  ol.writeSynopsis();
  
  if (m_incInfo && Config_getBool("SHOW_INCLUDE_FILES"))
  {
    QCString nm=m_incInfo->includeName.isEmpty() ? 
      (m_incInfo->fileDef ?
       m_incInfo->fileDef->docName().data() : "" 
      ) :
      m_incInfo->includeName.data();
    if (!nm.isEmpty())
    {
      ol.startTypewriter();
      bool isIDLorJava = nm.right(4)==".idl" || 
                         nm.right(5)==".pidl" || 
                         nm.right(5)==".java";
      if (isIDLorJava)
      {
        ol.docify("import ");
      }
      else if (isObjectiveC())
      {
        ol.docify("#import ");
      }
      else
      {
        ol.docify("#include ");
      }
      if (m_incInfo->local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify("<");
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Html);
      ol.docify(nm);
      ol.disableAllBut(OutputGenerator::Html);
      ol.enable(OutputGenerator::Html);
      if (m_incInfo->fileDef)
      {
        ol.writeObjectLink(0,m_incInfo->fileDef->includeName(),0,nm);
      }
      else
      {
        ol.docify(nm);
      }
      ol.popGeneratorState();
      if (m_incInfo->local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify(">");
      if (isIDLorJava) 
        ol.docify(";");
      ol.endTypewriter();
      ol.newParagraph();
    }
  }


  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"" << compoundTypeString();
    Doxygen::tagFile << "\"";
    if (isObjectiveC()) { Doxygen::tagFile << " objc=\"yes\""; }
    Doxygen::tagFile << ">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
    if (m_tempArgs)
    {
      ArgumentListIterator ali(*m_tempArgs);
      Argument *a;
      for (;(a=ali.current());++ali)
      {
        Doxygen::tagFile << "    <templarg>" << convertToXML(a->name) << "</templarg>" << endl;
      }
    }
  }

  
  if (Config_getBool("CLASS_DIAGRAMS")) ol.disableAllBut(OutputGenerator::Man);

  
  // write super classes
  int count;
  if ((count=m_inherits->count())>0)
  {
    //parseText(ol,theTranslator->trInherits()+" ");

    QCString inheritLine = theTranslator->trInheritsList(m_inherits->count());
    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(inheritLine,index,&matchLen))!=-1)
    {
      ol.parseText(inheritLine.mid(index,newIndex-index));
      bool ok;
      uint entryIndex = inheritLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
      BaseClassDef *bcd=m_inherits->at(entryIndex);
      if (ok && bcd)
      {
        ClassDef *cd=bcd->classDef;
        if (cd->isLinkable())
        {
          if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
          {
            Doxygen::tagFile << "    <base";
            if (bcd->prot==Protected)
            {
              Doxygen::tagFile << " protection=\"protected\"";
            }
            else if (bcd->prot==Private)
            {
              Doxygen::tagFile << " protection=\"private\"";
            }
            if (bcd->virt==Virtual)
            {
              Doxygen::tagFile << " virtualness=\"virtual\"";
            }
            Doxygen::tagFile << ">" << convertToXML(cd->name()) << "</base>" << endl;
          }
          ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->displayName()+bcd->templSpecifiers);
        }
        else
        {
          ol.docify(cd->displayName());
        }
      }
      else
      {
        err("Error: invalid marker %d in inherits list!\n",entryIndex);
      }
      index=newIndex+matchLen;
    } 
    ol.parseText(inheritLine.right(inheritLine.length()-index));
    ol.newParagraph();
  }

  // write subclasses
  if ((count=m_inheritedBy->count())>0)
  {
    QCString inheritLine = theTranslator->trInheritedByList(m_inheritedBy->count());
    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(inheritLine,index,&matchLen))!=-1)
    {
      ol.parseText(inheritLine.mid(index,newIndex-index));
      bool ok;
      uint entryIndex = inheritLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
      BaseClassDef *bcd=m_inheritedBy->at(entryIndex);
      if (ok && bcd)
      {
        ClassDef *cd=bcd->classDef;
        if (cd->isLinkable())
        {
          ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->displayName());
        }
        else
        {
          ol.docify(cd->displayName());
        }
        writeInheritanceSpecifier(ol,bcd);
      }
      index=newIndex+matchLen;
    } 
    ol.parseText(inheritLine.right(inheritLine.length()-index));
    ol.newParagraph();
  }

  if (Config_getBool("CLASS_DIAGRAMS")) ol.enableAll();
  

  count=0;
  BaseClassDef *ibcd;
  ibcd=m_inheritedBy->first();
  while (ibcd)
  {
    ClassDef *icd=ibcd->classDef;
    if ( icd->isVisibleInHierarchy()) count++;
    ibcd=m_inheritedBy->next();
  }
  ibcd=m_inherits->first();
  while (ibcd)
  {
    ClassDef *icd=ibcd->classDef;
    if ( icd->isVisibleInHierarchy()) count++;
    ibcd=m_inherits->next();
  }

  
  if (Config_getBool("HAVE_DOT") && Config_getBool("CLASS_GRAPH"))
    // write class diagram using dot
  {
    DotClassGraph inheritanceGraph(this,DotNode::Inheritance,Config_getInt("MAX_DOT_GRAPH_DEPTH"));
    if (!inheritanceGraph.isTrivial())
    {
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.startDotGraph();
      ol.parseText(theTranslator->trClassDiagram(displayName()));
      ol.endDotGraph(inheritanceGraph);
      if (Config_getBool("GENERATE_LEGEND"))
      {
        ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::Html);
        ol.writeString("<center><font size=\"2\">[");
        ol.startHtmlLink(relativePathToRoot(0)+"graph_legend"+Doxygen::htmlFileExtension);
        ol.docify(theTranslator->trLegend());
        ol.endHtmlLink();
        ol.writeString("]</font></center>");
        ol.popGeneratorState();
      }
      ol.popGeneratorState();
    }
  }
  else if (Config_getBool("CLASS_DIAGRAMS") && count>0) 
    // write class diagram using build-in generator
  {
    ClassDiagram diagram(this); // create a diagram of this class.
    ol.startClassDiagram();
    ol.disable(OutputGenerator::Man);
    ol.parseText(theTranslator->trClassDiagram(displayName()));
    ol.enable(OutputGenerator::Man);
    ol.endClassDiagram(diagram,getOutputFileBase(),displayName());
  } 

  if (Config_getBool("HAVE_DOT") && Config_getBool("COLLABORATION_GRAPH"))
  {
    DotClassGraph usageImplGraph(this,DotNode::Collaboration,Config_getInt("MAX_DOT_GRAPH_DEPTH"));
    if (!usageImplGraph.isTrivial())
    {
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.startDotGraph();
      ol.parseText(theTranslator->trCollaborationDiagram(displayName()));
      ol.endDotGraph(usageImplGraph);
      if (Config_getBool("GENERATE_LEGEND"))
      {
        ol.disableAllBut(OutputGenerator::Html);
        ol.writeString("<center><font size=\"2\">[");
        ol.startHtmlLink(relativePathToRoot(0)+"graph_legend"+Doxygen::htmlFileExtension);
        ol.docify(theTranslator->trLegend());
        ol.endHtmlLink();
        ol.writeString("]</font></center>");
      }
      ol.popGeneratorState();
    }
  }

  // write link to list of all members (HTML only)
  if (m_allMemberNameInfoSDict->count()>0 && 
      !Config_getBool("OPTIMIZE_OUTPUT_FOR_C")
     )
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(m_memListFileName,0);
    ol.parseText(theTranslator->trListOfAllMembers());
    ol.endTextLink();
    ol.enableAll();
    ol.popGeneratorState();
  }

  ol.endTextBlock();
  
  // write detailed description if the user wants it near the top
  if (Config_getBool("DETAILS_AT_TOP")) {
    writeDetailedDescription(ol,pageType,exampleFlag);
  }

  // write member groups
  ol.startMemberSections();

  // write user defined member groups
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    if (!mg->allMembersInSameSection() || !m_subGrouping) // group is in its own section
    {
      mg->writeDeclarations(ol,this,0,0,0);
    }
    else // add this group to the corresponding member section
    {
      //printf("addToDeclarationSection(%s)\n",mg->header().data());
      //mg->addToDeclarationSection();
    }
  }

  // public types
  pubTypes.writeDeclarations(ol,this,0,0,0,theTranslator->trPublicTypes(),0); 

  // public methods
  pubSlots.writeDeclarations(ol,this,0,0,0,theTranslator->trPublicSlots(),0); 
  signals.writeDeclarations(ol,this,0,0,0,theTranslator->trSignals(),0); 
  dcopMethods.writeDeclarations(ol,this,0,0,0,theTranslator->trDCOPMethods(),0); 
  pubMethods.writeDeclarations(ol,this,0,0,0,theTranslator->trPublicMembers(),0); 
  pubStaticMethods.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPublicMembers(),0); 

  // public attribs
  pubAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trPublicAttribs(),0); 
  pubStaticAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPublicAttribs(),0); 
  
  // protected types
  proTypes.writeDeclarations(ol,this,0,0,0,theTranslator->trProtectedTypes(),0); 

  // protected methods
  proSlots.writeDeclarations(ol,this,0,0,0,theTranslator->trProtectedSlots(),0); 
  proMethods.writeDeclarations(ol,this,0,0,0,theTranslator->trProtectedMembers(),0); 
  proStaticMethods.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticProtectedMembers(),0); 

  // protected attribs
  proAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trProtectedAttribs(),0); 
  proStaticAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticProtectedAttribs(),0); 

  // package types
  pacTypes.writeDeclarations(ol,this,0,0,0,theTranslator->trPackageTypes(),0); 

  // package methods
  pacMethods.writeDeclarations(ol,this,0,0,0,theTranslator->trPackageMembers(),0); 
  pacStaticMethods.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPackageMembers(),0); 

  // package attribs
  pacAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trPackageAttribs(),0); 
  pacStaticAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPackageAttribs(),0); 

  // package
  properties.writeDeclarations(ol,this,0,0,0,theTranslator->trProperties(),0); 

  // events
  events.writeDeclarations(ol,this,0,0,0,theTranslator->trEvents(),0); 

  if (Config_getBool("EXTRACT_PRIVATE"))
  {
    // private types
    priTypes.writeDeclarations(ol,this,0,0,0,theTranslator->trPrivateTypes(),0); 

    // private members
    priSlots.writeDeclarations(ol,this,0,0,0,theTranslator->trPrivateSlots(),0); 
    priMethods.writeDeclarations(ol,this,0,0,0,theTranslator->trPrivateMembers(),0); 
    priStaticMethods.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPrivateMembers(),0); 

    // private attribs
    priAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trPrivateAttribs(),0); 
    priStaticAttribs.writeDeclarations(ol,this,0,0,0,theTranslator->trStaticPrivateAttribs(),0); 
  }

  // friends
  friends.writeDeclarations(ol,this,0,0,0,theTranslator->trFriends(),0);

  // related functions
  related.writeDeclarations(ol,this,0,0,0,
                  theTranslator->trRelatedFunctions(),
                  theTranslator->trRelatedSubscript()
                 ); 

  // nested classes
  m_innerClasses->writeDeclaration(ol,0,0,TRUE);
  
  ol.endMemberSections();
    
  // write detailed description
  if (!Config_getBool("DETAILS_AT_TOP")) {
    writeDetailedDescription(ol,pageType,exampleFlag);
  }
  
  
  typedefMembers.writeDocumentation(ol,name(),this,
                         theTranslator->trMemberTypedefDocumentation());

  enumMembers.writeDocumentation(ol,name(),this,
                         theTranslator->trMemberEnumerationDocumentation());
  
  constructors.writeDocumentation(ol,name(),this,
                         theTranslator->trConstructorDocumentation());

  functionMembers.writeDocumentation(ol,name(),this,
                         theTranslator->trMemberFunctionDocumentation());

  relatedMembers.writeDocumentation(ol,name(),this,
                         theTranslator->trRelatedFunctionDocumentation());

  variableMembers.writeDocumentation(ol,name(),this,
                         theTranslator->trMemberDataDocumentation());
  
  propertyMembers.writeDocumentation(ol,name(),this,
                         theTranslator->trPropertyDocumentation());
  
  eventMembers.writeDocumentation(ol,name(),this,
                         theTranslator->trEventDocumentation());

  ol.startTextBlock();

  // write the list of used files (not for man pages)
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);

  if (Config_getBool("SHOW_USED_FILES"))
  {
    ol.writeRuler();
    ol.parseText(theTranslator->trGeneratedFromFiles(
          m_isObjC && m_compType==Interface ? Class : m_compType,
          m_files.count()==1));

    bool first=TRUE;
    const char *file = m_files.first();
    while (file)
    {
      bool ambig;
      FileDef *fd=findFileDef(Doxygen::inputNameDict,file,ambig);
      if (fd)
      {
        if (first)
        {
          first=FALSE;   
          ol.startItemList();
        }

        ol.writeListItem();
        QCString path=fd->getPath().copy();
        if (Config_getBool("FULL_PATH_NAMES"))
        {
          ol.docify(stripFromPath(path));
        }

        // for HTML 
        ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::Html);
        if (fd->generateSourceFile())
        {
          ol.writeObjectLink(0,fd->getSourceFileBase(),0,fd->name());
        }
        else if (fd->isLinkable())
        {
          ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,
              fd->name());
        }
        else
        {
          ol.docify(fd->name());
        }
        ol.popGeneratorState();

        // for other output formats
        ol.pushGeneratorState();
        ol.disable(OutputGenerator::Html);
        if (fd->isLinkable())
        {
          ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,
              fd->name());
        }
        else
        {
          ol.docify(fd->name());
        }

        ol.popGeneratorState();

        
      }
      file=m_files.next();
    }
    if (!first) ol.endItemList();
  }

  // write Author section (Man only)
  ol.enable(OutputGenerator::Man);
  ol.disableAllBut(OutputGenerator::Man);
  ol.writeString("\n");
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString("PROJECT_NAME")));
  ol.popGeneratorState();

  ol.endTextBlock();

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }
 
  endFile(ol);

}

void ClassDef::writeDocumentationForInnerClasses(OutputList &ol)
{
  // write inner classes after the parent, so the tag files contain
  // the definition in proper order!
  if (m_innerClasses)
  {
    ClassSDict::Iterator cli(*m_innerClasses);
    ClassDef *innerCd;
    for (cli.toFirst();(innerCd=cli.current());++cli)
    {
      if (innerCd->isLinkableInProject() && innerCd->templateMaster()==0 &&
          (innerCd->protection()!=Private || Config_getBool("EXTRACT_PRIVATE"))
         )
      {
        msg("Generating docs for nested compound %s...\n",innerCd->name().data());
        innerCd->writeDocumentation(ol);
        innerCd->writeMemberList(ol);
      }
      innerCd->writeDocumentationForInnerClasses(ol);
    }
  }
}

// write the list of all (inherited) members for this class
void ClassDef::writeMemberList(OutputList &ol)
{
  if (m_allMemberNameInfoSDict->count()==0 || 
      Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) return;
  // only for HTML
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  startFile(ol,m_memListFileName,m_memListFileName,
            theTranslator->trMemberList());
  startTitle(ol,0);
  ol.parseText(displayName()+" "+theTranslator->trMemberList());
  endTitle(ol,0,0);
  ol.parseText(theTranslator->trThisIsTheListOfAllMembers());
  ol.writeObjectLink(getReference(),getOutputFileBase(),0,displayName());
  ol.parseText(theTranslator->trIncludingInheritedMembers());
  
  //ol.startItemList();
  ol.writeString("<p><table>\n");
  
  //MemberNameInfo *mni=m_allMemberNameInfoList->first();
  MemberNameInfoSDict::Iterator mnii(*m_allMemberNameInfoSDict); 
  MemberNameInfo *mni;
  for (mnii.toFirst();(mni=mnii.current());++mnii)
  {
    MemberInfo *mi=mni->first();
    while (mi)
    {
      MemberDef *md=mi->memberDef;
      ClassDef  *cd=md->getClassDef();
      Protection prot = mi->prot;
      Specifier virt=md->virtualness();
      
      //printf("%s: Member %s of class %s md->protection()=%d mi->prot=%d prot=%d inherited=%d\n",
      //    name().data(),md->name().data(),cd->name().data(),md->protection(),mi->prot,prot,mi->inherited);


      if (cd && !md->name().isEmpty() && md->name()[0]!='@')
      {
        bool memberWritten=FALSE;
        if (cd->isLinkable() && md->isLinkable()) 
          // create a link to the documentation
        {
          QCString name=mi->ambiguityResolutionScope+md->name();
          //ol.writeListItem();
          ol.writeString("  <tr class=\"memlist\"><td>");
          if (cd->isObjectiveC())
          {
            if (md->isObjCMethod())
            {
              if (md->isStatic())
                ol.writeString("+&nbsp;</td><td>");
              else
                ol.writeString("-&nbsp;</td><td>");
            }
            else
              ol.writeString("</td><td>");
          }
          if (md->isObjCMethod())
          {
            ol.writeObjectLink(md->getReference(),
                md->getOutputFileBase(),
                md->anchor(),md->name());
          }
          else
          {
            //Definition *bd = md->getGroupDef();
            //if (bd==0) bd=cd;
            ol.writeObjectLink(md->getReference(),
                md->getOutputFileBase(),
                md->anchor(),name);

            if ( md->isFunction() || md->isSignal() || md->isSlot() ||
                (md->isFriend() && md->argsString())) 
              ol.docify(md->argsString());
            else if (md->isEnumerate())
              ol.parseText(" "+theTranslator->trEnumName());
            else if (md->isEnumValue())
              ol.parseText(" "+theTranslator->trEnumValue());
            else if (md->isTypedef())
              ol.docify(" typedef");
            else if (md->isFriend() && !strcmp(md->typeString(),"friend class"))
              ol.docify(" class");
            //ol.writeString("\n");
          }
          ol.writeString("</td>");
          memberWritten=TRUE;
        }
        else if (!Config_getBool("HIDE_UNDOC_MEMBERS") && 
                  (md->protection()!=Private || Config_getBool("EXTRACT_PRIVATE") || md->isFriend()) 
                ) // no documentation, 
                  // generate link to the class instead.
        {
          //ol.writeListItem();
          ol.writeString("  <tr bgcolor=\"#f0f0f0\"><td>");
          if (cd->isObjectiveC())
          {
            if (md->isObjCMethod())
            {
              if (md->isStatic())
                ol.writeString("+&nbsp;</td><td>");
              else
                ol.writeString("-&nbsp;</td><td>");
            }
            else
              ol.writeString("</td><td>");
          }
          ol.startBold();
          ol.docify(md->name());
          ol.endBold();
          if (!md->isObjCMethod())
          {
            if ( md->isFunction() || md->isSignal() || md->isSlot() ) 
              ol.docify(md->argsString());
            else if (md->isEnumerate())
              ol.parseText(" "+theTranslator->trEnumName());
            else if (md->isEnumValue())
              ol.parseText(" "+theTranslator->trEnumValue());
            else if (md->isTypedef())
              ol.docify(" typedef");
          }
          ol.writeString(" (");
          ol.parseText(theTranslator->trDefinedIn()+" ");
          if (cd->isLinkable())
          {
            ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->displayName());
          }
          else
          {
            ol.startBold();
            ol.docify(cd->displayName());
            ol.endBold();
          }
          ol.writeString(")");
          ol.writeString("</td>");
          memberWritten=TRUE;
        }
        if (memberWritten)
        {
          ol.writeString("<td>");
          ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),
                             0,cd->displayName());
          ol.writeString("</td>");
          ol.writeString("<td>");
        }
        if (
            (prot!=Public || (virt!=Normal && !m_isObjC) || 
             md->isFriend() || md->isRelated() || md->isExplicit() ||
             md->isMutable() || (md->isInline() && Config_getBool("INLINE_INFO")) ||
             md->isSignal() || md->isSlot() ||
             md->isStatic()
            )
            && memberWritten)
        {
          ol.startTypewriter();
          ol.docify(" [");
          QStrList sl;
          if (md->isFriend()) sl.append("friend");
          else if (md->isRelated()) sl.append("related");
          else
          {
            if (Config_getBool("INLINE_INFO") && md->isInline())        
                                       sl.append("inline");
            if (md->isExplicit())      sl.append("explicit");
            if (md->isMutable())       sl.append("mutable");
            if (prot==Protected)       sl.append("protected");
            else if (prot==Private)    sl.append("private");
            else if (prot==Package)    sl.append("package");
            if (virt==Virtual && 
                !m_isObjC)             sl.append("virtual");
            else if (virt==Pure)       sl.append("pure virtual");
            if (md->isStatic())        sl.append("static");
            if (md->isSignal())        sl.append("signal");
            if (md->isSlot())          sl.append("slot");
          }
          const char *s=sl.first();
          while (s)
          {
            ol.docify(s);
            s=sl.next();
            if (s) ol.docify(", ");
          }
          ol.docify("]");
          ol.endTypewriter();
        }
        if (memberWritten)
        {
          ol.writeString("</td>");
          ol.writeString("</tr>\n");
        }
      }
      mi=mni->next();
    }
  }
  //ol.endItemList();

  ol.writeString("</table>");
  
  endFile(ol);
  ol.popGeneratorState();
}


// add a reference to an example
bool ClassDef::addExample(const char *anchor,const char *nameStr,
    const char *file)
{
  if (m_exampleSDict && !m_exampleSDict->find(nameStr))
  {
    Example *e=new Example;
    e->anchor=anchor;
    e->name=nameStr;
    e->file=file;
    m_exampleSDict->inSort(nameStr,e);
    return TRUE;
  }
  return FALSE;
}

// returns TRUE if this class is used in an example
bool ClassDef::hasExamples()
{
  if (m_exampleSDict==0) 
    return FALSE;
  else
    return m_exampleSDict->count()>0;
}


void ClassDef::setTemplateArguments(ArgumentList *al)
{
  if (al==0) return;
  if (!m_tempArgs) delete m_tempArgs; // delete old list if needed
  m_tempArgs=new ArgumentList; 
  ArgumentListIterator ali(*al);
  Argument *a;
  for (;(a=ali.current());++ali)
  {
    m_tempArgs->append(new Argument(*a));
  }
}

/*! Returns \c TRUE iff this class or a class inheriting from this class
 *  is \e not defined in an external tag file. 
 */
bool ClassDef::hasNonReferenceSuperClass()
{
  bool found=!isReference(); 
  if (found) return TRUE; // we're done if this class is not a reference
  BaseClassListIterator bcli(*m_inheritedBy);
  for ( ; bcli.current() && !found ; ++bcli ) // for each super class
  {
    ClassDef *bcd=bcli.current()->classDef;
    // recurse into the super class branch
    found = found || bcd->hasNonReferenceSuperClass(); 
    if (!found)
    {
      // look for template instances that might have non-reference super classes
      QDict<ClassDef> *cil = bcd->getTemplateInstances();
      if (cil)
      {
        QDictIterator<ClassDef> tidi(*cil);
        for ( ; tidi.current() && !found ; ++tidi) // for each template instance
        {
          // recurse into the template instance branch
          found = found || tidi.current()->hasNonReferenceSuperClass();
        }
      }
    }
  }
  return found;
}

/*! called from MemberDef::writeDeclaration() to (recusively) write the 
 *  definition of an annonymous struct, union or class.
 */
void ClassDef::writeDeclaration(OutputList &ol,MemberDef *md,bool inGroup)
{
  //ol.insertMemberAlign();
  //printf("ClassName=`%s' inGroup=%d\n",name().data(),inGroup);

  //if (inGroup && md && md->getClassDef()==this) return;

  ol.docify(compoundTypeString());
  int ri=name().findRev("::");
  if (ri==-1) ri=name().length();
  QCString cn=name().right(name().length()-ri-2);
  if (!cn.isEmpty() && cn.at(0)!='@' && md)
  { 
    ol.docify(" ");
    if (isLinkable())
    {
      ol.writeObjectLink(0,0,md->anchor(),cn);
    }
    else
    {
      ol.startBold();
      ol.docify(cn);
      ol.endBold();
    }
  }
  ol.docify(" {");
  ol.endMemberItem(); 

  // write user defined member groups
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->setInGroup(inGroup);
    mg->writePlainDeclarations(ol,this,0,0,0);
  }

  pubTypes.setInGroup(inGroup);
  pubTypes.writePlainDeclarations(ol,this,0,0,0); 
  pubMethods.setInGroup(inGroup);
  pubMethods.writePlainDeclarations(ol,this,0,0,0); 
  pubAttribs.setInGroup(inGroup);
  pubAttribs.writePlainDeclarations(ol,this,0,0,0); 
  pubSlots.setInGroup(inGroup);
  pubSlots.writePlainDeclarations(ol,this,0,0,0); 
  signals.setInGroup(inGroup);
  signals.writePlainDeclarations(ol,this,0,0,0); 
  dcopMethods.setInGroup(inGroup);
  dcopMethods.writePlainDeclarations(ol,this,0,0,0); 
  properties.setInGroup(inGroup);
  properties.writePlainDeclarations(ol,this,0,0,0); 
  events.setInGroup(inGroup);
  events.writePlainDeclarations(ol,this,0,0,0); 
  pubStaticMethods.setInGroup(inGroup);
  pubStaticMethods.writePlainDeclarations(ol,this,0,0,0); 
  pubStaticAttribs.setInGroup(inGroup);
  pubStaticAttribs.writePlainDeclarations(ol,this,0,0,0); 
  proTypes.setInGroup(inGroup);
  proTypes.writePlainDeclarations(ol,this,0,0,0); 
  proMethods.setInGroup(inGroup);
  proMethods.writePlainDeclarations(ol,this,0,0,0); 
  proAttribs.setInGroup(inGroup);
  proAttribs.writePlainDeclarations(ol,this,0,0,0); 
  proSlots.setInGroup(inGroup);
  proSlots.writePlainDeclarations(ol,this,0,0,0); 
  proStaticMethods.setInGroup(inGroup);
  proStaticMethods.writePlainDeclarations(ol,this,0,0,0); 
  proStaticAttribs.setInGroup(inGroup);
  proStaticAttribs.writePlainDeclarations(ol,this,0,0,0); 
  pacTypes.setInGroup(inGroup);
  pacTypes.writePlainDeclarations(ol,this,0,0,0); 
  pacMethods.setInGroup(inGroup);
  pacMethods.writePlainDeclarations(ol,this,0,0,0); 
  pacAttribs.setInGroup(inGroup);
  pacAttribs.writePlainDeclarations(ol,this,0,0,0); 
  pacStaticMethods.setInGroup(inGroup);
  pacStaticMethods.writePlainDeclarations(ol,this,0,0,0); 
  pacStaticAttribs.setInGroup(inGroup);
  pacStaticAttribs.writePlainDeclarations(ol,this,0,0,0); 
  if (Config_getBool("EXTRACT_PRIVATE"))
  {
    priTypes.setInGroup(inGroup);
    priTypes.writePlainDeclarations(ol,this,0,0,0); 
    priMethods.setInGroup(inGroup);
    priMethods.writePlainDeclarations(ol,this,0,0,0); 
    priAttribs.setInGroup(inGroup); 
    priAttribs.writePlainDeclarations(ol,this,0,0,0); 
    priSlots.setInGroup(inGroup); 
    priSlots.writePlainDeclarations(ol,this,0,0,0); 
    priStaticMethods.setInGroup(inGroup); 
    priStaticMethods.writePlainDeclarations(ol,this,0,0,0); 
    priStaticAttribs.setInGroup(inGroup); 
    priStaticAttribs.writePlainDeclarations(ol,this,0,0,0); 
  }
  friends.setInGroup(inGroup);
  friends.writePlainDeclarations(ol,this,0,0,0);
  related.setInGroup(inGroup);
  related.writePlainDeclarations(ol,this,0,0,0); 
}

/*! a link to this class is possible within this project */
bool ClassDef::isLinkableInProject() const
{ 
  if (m_templateMaster)
  {
    return m_templateMaster->isLinkableInProject();
  }
  else
  {
    return !name().isEmpty() &&    /* no name */
      !m_artificial &&
      name().find('@')==-1 && /* anonymous compound */
      (m_prot!=Private || Config_getBool("EXTRACT_PRIVATE")) && /* private */
      (!m_isLocal || Config_getBool("EXTRACT_LOCAL_CLASSES")) && /* local */
      hasDocumentation() &&   /* documented */ 
      !isReference() &&         /* not an external reference */
      (!m_isStatic || Config_getBool("EXTRACT_STATIC"));
  }
}

bool ClassDef::isLinkable() const
{
  if (m_templateMaster)
  {
    return m_templateMaster->isLinkable();
  }
  else
  {
    return isLinkableInProject() || isReference();
  }
}


/*! the class is visible in a class diagram, or class hierarchy */
bool ClassDef::isVisibleInHierarchy() 
{ 
    return // show all classes or a subclass is visible
      (Config_getBool("ALLEXTERNALS") || hasNonReferenceSuperClass()) &&
      // and not an annonymous compound
      name().find('@')==-1 &&
      // not an artifically introduced class
      !m_artificial &&
      // and not privately inherited
      (m_prot!=Private || Config_getBool("EXTRACT_PRIVATE")) &&
      // documented or shown anyway or documentation is external 
      (hasDocumentation() || 
       !Config_getBool("HIDE_UNDOC_CLASSES") || 
       (m_templateMaster && m_templateMaster->hasDocumentation()) || 
       isReference()
      ) &&
      // is not part of an unnamed namespace or shown anyway
      (!m_isStatic || Config_getBool("EXTRACT_STATIC"));
}

bool ClassDef::hasDocumentation() const
{
  return Definition::hasDocumentation();
}

//----------------------------------------------------------------------
// recursive function:
// returns TRUE iff class definition `bcd' represents an (in)direct base 
// class of class definition `cd'.

bool ClassDef::isBaseClass(ClassDef *bcd, bool followInstances,int level)
{
  bool found=FALSE;
  //printf("isBaseClass(cd=%s) looking for %s\n",cd->name().data(),bcd->name().data());
  if (level>256)
  {
    err("Possible recursive class relation while inside %s and looking for %s\n",name().data(),bcd->name().data());
    return FALSE;
  }
  BaseClassListIterator bcli(*baseClasses());
  for ( ; bcli.current() && !found ; ++bcli)
  {
    ClassDef *ccd=bcli.current()->classDef;
    if (!followInstances && ccd->templateMaster()) ccd=ccd->templateMaster();
    //printf("isBaseClass() baseclass %s\n",ccd->name().data());
    if (ccd==bcd) 
      found=TRUE;
    else 
      found=ccd->isBaseClass(bcd,followInstances,level+1);
  }
  return found;
}

//----------------------------------------------------------------------------

static bool isStandardFunc(MemberDef *md)
{
  ClassDef *cd=md->getClassDef();
  if (cd->templateMaster()) cd=cd->templateMaster();
  return md->name()=="operator=" || // assignment operator
         md->name()==cd->localName() || // constructor
         (md->name().find('~')!=-1 && 
          md->name().find("operator")==-1); // destructor
}

/*! 
 * recusively merges the `all members' lists of a class base 
 * with that of this class. Must only be called for classes without
 * subclasses!
 */

void ClassDef::mergeMembers()
{
  if (m_membersMerged) return;
  m_membersMerged=TRUE;
  //printf("  mergeMembers for %s\n",name().data());
  bool inlineInheritedMembers = Config_getBool("INLINE_INHERITED_MEMB" );
  BaseClassListIterator bcli(*baseClasses());
  BaseClassDef *bcd;
  for ( ; (bcd=bcli.current()) ; ++bcli )
  {
    ClassDef *bClass=bcd->classDef; 

    // merge the members in the base class of this inheritance branch first
    bClass->mergeMembers();

    MemberNameInfoSDict *srcMnd  = bClass->m_allMemberNameInfoSDict;
    MemberNameInfoSDict *dstMnd  =         m_allMemberNameInfoSDict;

    MemberNameInfoSDict::Iterator srcMnili(*srcMnd);
    MemberNameInfo *srcMni;
    for ( ; (srcMni=srcMnili.current()) ; ++srcMnili)
    {
      //printf("Base member name %s\n",srcMni->memberName());
      MemberNameInfo *dstMni;
      if ((dstMni=dstMnd->find(srcMni->memberName())))
        // a member with that name is already in the class.
        // the member may hide or reimplement the one in the sub class
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
          bool hidden=FALSE;
          MemberNameInfoIterator dstMnii(*dstMni); 
          MemberInfo *dstMi;
          ClassDef *srcCd = srcMd->getClassDef();
          for ( ; (dstMi=dstMnii.current()) && !found; ++dstMnii )
          {
            MemberDef *dstMd = dstMi->memberDef;
            if (srcMd!=dstMd) // different members
            {
              ClassDef *dstCd = dstMd->getClassDef();
              //printf("  Is %s a base class of %s?\n",srcCd->name().data(),dstCd->name().data());
              if (srcCd==dstCd || dstCd->isBaseClass(srcCd,TRUE)) 
                // member is in the same or a base class
              {
                found=matchArguments(srcMd->argumentList(),
                    dstMd->argumentList());
                //printf("  Yes, matching (%s<->%s): %d\n",
                //    argListToString(srcMd->argumentList()).data(),
                //    argListToString(dstMd->argumentList()).data(),
                //    found);
                hidden = hidden  || !found;
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
              // if scope paths are equal or
              // if base class is an interface (and thus implicitly virtual).
              //printf("same member found srcMi->virt=%d dstMi->virt=%d\n",srcMi->virt,dstMi->virt);
              if ((srcMi->virt!=Normal && dstMi->virt!=Normal) ||
                  bClass->name()+"::"+srcMi->scopePath == dstMi->scopePath ||
                  dstMd->getClassDef()->compoundType()==Interface
                 ) 
              {
                found=TRUE;
              }
              else // member can be reached via multiple paths in the 
                // inheritance tree
              {
                //printf("$$ Existing member %s %s add scope %s\n",
                //    dstMi->ambiguityResolutionScope.data(),
                //    dstMd->name().data(),
                //    dstMi->scopePath.left(dstMi->scopePath.find("::")+2).data());

                QCString scope=dstMi->scopePath.left(dstMi->scopePath.find("::")+2);
                if (scope!=dstMi->ambiguityResolutionScope.left(scope.length()))
                {
                  dstMi->ambiguityResolutionScope.prepend(scope);
                }
                ambigue=TRUE;
              }
            }
          }
          //printf("member %s::%s hidden %d ambigue %d srcMi->ambigClass=%p\n",
          //    srcCd->name().data(),srcMd->name().data(),hidden,ambigue,srcMi->ambigClass);

          // TODO: fix the case where a member is hidden by inheritance
          //       of a member with the same name but with another prototype,
          //       while there is more than one path to the member in the 
          //       base class due to multiple inheritance. In this case
          //       it seems that the member is not reachable by prefixing a 
          //       scope name either (according to my compiler). Currently, 
          //       this case is shown anyway.
          if (!found && srcMd->protection()!=Private)
          {
            Protection prot=srcMd->protection();
            if (bcd->prot==Protected && prot==Public)       prot=bcd->prot;
            else if (bcd->prot==Private)                    prot=bcd->prot;
            
            if (inlineInheritedMembers)
            {
              if (!isStandardFunc(srcMd))
              {
                //printf("    insertMember `%s'\n",srcMd->name().data());
                internalInsertMember(srcMd,prot,FALSE);
              }
            }

            Specifier virt=srcMi->virt;
            if (srcMi->virt==Normal && bcd->virt!=Normal) virt=bcd->virt;
            
            MemberInfo *newMi = new MemberInfo(srcMd,prot,virt,TRUE);
            newMi->scopePath=bClass->name()+"::"+srcMi->scopePath;
            if (ambigue)
            {
              //printf("$$ New member %s %s add scope %s::\n",
              //     srcMi->ambiguityResolutionScope.data(),
              //     srcMd->name().data(),
              //     bClass->name().data());

              QCString scope=bClass->name()+"::";
              if (scope!=srcMi->ambiguityResolutionScope.left(scope.length()))
              {
                newMi->ambiguityResolutionScope=
                  scope+srcMi->ambiguityResolutionScope.copy();
              }
            }
            if (hidden)
            {
              if (srcMi->ambigClass==0)
              {
                newMi->ambigClass=bClass;
                newMi->ambiguityResolutionScope=bClass->name()+"::";
              }
              else
              {
                newMi->ambigClass=srcMi->ambigClass;
                newMi->ambiguityResolutionScope=srcMi->ambigClass->name()+"::";
              }
            }
            dstMni->append(newMi);
          }
        }
      }
      else // base class has a member that is not in the sub class => copy
      {
        // create a deep copy of the list (only the MemberInfo's will be 
        // copied, not the actual MemberDef's)
        MemberNameInfo *newMni = 0;
        newMni = new MemberNameInfo(srcMni->memberName()); 

        // copy the member(s) from the base to the sub class
        MemberNameInfoIterator mnii(*srcMni);
        MemberInfo *mi;
        for (;(mi=mnii.current());++mnii)
        {
          Protection prot = mi->prot;
          if (bcd->prot==Protected)
          {
            if (prot==Public) prot=Protected;
          }
          else if (bcd->prot==Private)
          {
            prot=Private;
          }
          //printf("%s::%s: prot=%d bcd->prot=%d result=%d\n",
          //    name().data(),mi->memberDef->name().data(),mi->prot,
          //    bcd->prot,prot);

          if (mi->prot!=Private)
          {
            Specifier virt=mi->virt;
            if (mi->virt==Normal && bcd->virt!=Normal) virt=bcd->virt;

            if (inlineInheritedMembers)
            {
              if (!isStandardFunc(mi->memberDef))
              {
                //printf("    insertMember `%s'\n",mi->memberDef->name().data());
                internalInsertMember(mi->memberDef,prot,FALSE);
              }
            }
            //printf("Adding!\n");
            MemberInfo *newMi=new MemberInfo(mi->memberDef,prot,virt,TRUE);
            newMi->scopePath=bClass->name()+"::"+mi->scopePath;
            newMi->ambigClass=mi->ambigClass;
            newMi->ambiguityResolutionScope=mi->ambiguityResolutionScope.copy();
            newMni->append(newMi);
          }
        }

        // add it to the dictionary
        dstMnd->append(newMni->memberName(),newMni);
      }
    }
  }
  //printf("  end mergeMembers\n");
}

//----------------------------------------------------------------------------

/*! Merges the members of a Objective-C category into this class.
 */
void ClassDef::mergeCategory(ClassDef *category)
{
  category->m_categoryOf = this;
    
  MemberNameInfoSDict *srcMnd  = category->m_allMemberNameInfoSDict;
  MemberNameInfoSDict *dstMnd  =           m_allMemberNameInfoSDict;

  MemberNameInfoSDict::Iterator srcMnili(*srcMnd);
  MemberNameInfo *srcMni;
  for ( ; (srcMni=srcMnili.current()) ; ++srcMnili)
  {
    MemberNameInfo *dstMni=dstMnd->find(srcMni->memberName());
    if (dstMni) // method is already defined in the class
    {
      // TODO: we should remove the other member and insert this one.
    }
    else // new method name
    {
      // create a deep copy of the list (only the MemberInfo's will be 
      // copied, not the actual MemberDef's)
      MemberNameInfo *newMni = 0;
      newMni = new MemberNameInfo(srcMni->memberName()); 

      // copy the member(s) from the category to this class
      MemberNameInfoIterator mnii(*srcMni);
      MemberInfo *mi;
      for (;(mi=mnii.current());++mnii)
      {
        //printf("Adding!\n");
        MemberInfo *newMi=new MemberInfo(mi->memberDef,mi->prot,mi->virt,mi->inherited);
        newMi->scopePath=mi->scopePath;
        newMi->ambigClass=mi->ambigClass;
        newMi->ambiguityResolutionScope=mi->ambiguityResolutionScope.copy();
        newMni->append(newMi);
      }

      // add it to the dictionary
      dstMnd->append(newMni->memberName(),newMni);
    }
  }
}

//----------------------------------------------------------------------------

void ClassDef::addUsedClass(ClassDef *cd,const char *accessName)
{
  if (m_usesImplClassDict==0) 
  {
    m_usesImplClassDict = new UsesClassDict(17); 
    m_usesImplClassDict->setAutoDelete(TRUE);
  }
  UsesClassDef *ucd=m_usesImplClassDict->find(cd->name());
  if (ucd==0)
  {
     ucd = new UsesClassDef(cd);
     m_usesImplClassDict->insert(cd->name(),ucd);
     //printf("Adding used class %s to class %s\n",
     //    cd->name().data(),name().data());
  }
  ucd->addAccessor(accessName);
}

void ClassDef::addUsedByClass(ClassDef *cd,const char *accessName)
{
  if (m_usedByImplClassDict==0) 
  {
    m_usedByImplClassDict = new UsesClassDict(17); 
    m_usedByImplClassDict->setAutoDelete(TRUE);
  }
  UsesClassDef *ucd=m_usedByImplClassDict->find(cd->name());
  if (ucd==0)
  {
     ucd = new UsesClassDef(cd);
     m_usedByImplClassDict->insert(cd->name(),ucd);
     //printf("Adding used by class %s to class %s\n",
     //    cd->name().data(),name().data());
  }
  ucd->addAccessor(accessName);
}


#if 0
/*! Builds up a dictionary of all classes that are used by the state of this 
 *  class (the "implementation"). 
 *  Must be called before mergeMembers() is called!
 */

void ClassDef::determineImplUsageRelation()
{
  MemberNameInfoSDict::Iterator mnili(*m_allMemberNameInfoSDict);
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
        QCString type=removeRedundantWhiteSpace(md->typeString());
        //printf("in class %s found var type=`%s' name=`%s'\n",
        //            name().data(),type.data(),md->name().data());
        int pos=0;
        QCString usedClassName;
        QCString templSpec;
        bool found=FALSE;
        while (extractClassNameFromType(type,pos,usedClassName,templSpec) && !found)
        {
          //printf("usedClassName=`%s' templSpec=%s\n",usedClassName.data(),templSpec.data());
          // check if usedClassName is a template argument of its class
          ClassDef *cd=md->getClassDef();
          if (cd && cd->templateArguments())
          {
            ArgumentListIterator ali(*cd->templateArguments());
            Argument *arg;
            int count=0;
            for (ali.toFirst();(arg=ali.current());++ali,++count)
            {
              if (arg->name==usedClassName) // type is a template argument
              {
                found=TRUE;
                if (m_usesImplClassDict==0) m_usesImplClassDict = new UsesClassDict(257); 
                cd = new ClassDef(cd->getDefFileName(),cd->getDefLine(),
                    usedClassName,ClassDef::Class);
                cd->setIsTemplateBaseClass(count);
                UsesClassDef *ucd = new UsesClassDef(cd);
                m_usesImplClassDict->insert(cd->name(),ucd);
                ucd->templSpecifiers = templSpec;
                ucd->addAccessor(md->name());
                Doxygen::hiddenClasses.append(cd);
                //printf("Adding used template argument %s to class %s\n",
                //    cd->name().data(),name().data());
                //printf("Adding accessor %s to class %s\n",
                //    md->name().data(),ucd->classDef->name().data());
              }
            }
          }

          if (!found)
          {
            cd=0;
            if (getNamespaceDef()!=0)
            {
              cd=getResolvedClass(getNamespaceDef()->name()+"::"+usedClassName,0,&templSpec);
            }
            if (cd==0) cd=getResolvedClass(name()+"::"+usedClassName,0,&templSpec);
            if (cd==0) cd=getResolvedClass(usedClassName,0,&templSpec); // TODO: also try inbetween scopes!
            //printf("Search for class %s result=%p\n",usedClassName.data(),cd);
            if (cd) // class exists 
            {
              found=TRUE;
              if (m_usesImplClassDict==0) 
              {
                m_usesImplClassDict = new UsesClassDict(257); 
                m_usesImplClassDict->setAutoDelete(TRUE);
              }
              UsesClassDef *ucd=m_usesImplClassDict->find(cd->name());
              if (ucd==0 || ucd->templSpecifiers!=templSpec)
              {
                ucd = new UsesClassDef(cd);
                m_usesImplClassDict->insert(cd->name(),ucd);
                ucd->templSpecifiers = templSpec;
                //printf("Adding used class %s to class %s\n",
                //    cd->name().data(),name().data());
              }
              ucd->addAccessor(md->name());
              //printf("Adding accessor %s to class %s\n",
              //    md->name().data(),ucd->classDef->name().data());
            }
          }
        }
      }
    }
  }
#ifdef DUMP
  if (m_usesClassDict)
  {
    msg("Class %s uses the following classes:\n",name().data());
    UsesClassDictIterator ucdi(*m_usesClassDict);
    UsesClassDef *ucd;
    for (;(ucd=ucdi.current());++ucdi)
    {
      msg("  %s via ",ucd->classDef->name().data());
      QDictIterator<void> dvi(*ucd->accessors); 
      const char *s;
      for (;(s=dvi.currentKey());++dvi)
      {
        msg("%s ",s);
      }
      msg("\n");
    }
  }
#endif
}

//----------------------------------------------------------------------------

// I have disabled this code because the graphs it renders quickly become
// too large to be of practical use.

void ClassDef::addUsedInterfaceClasses(MemberDef *md,const char *typeStr)
{
  QCString type = typeStr;
  static const QRegExp re("[a-z_A-Z][a-z_A-Z0-9:]*");
  int p=0,i,l;
  while ((i=re.match(type,p,&l))!=-1) // for each class name in the type
  {
    ClassDef *cd=getClass(name()+"::"+type.mid(i,l));
    if (cd==0) cd=getClass(type.mid(i,l)); // TODO: also try inbetween scopes!
    if (cd && cd!=this && !isBaseClass(cd))
    {
      if (m_usesIntfClassDict==0) 
      {
        m_usesIntfClassDict = new UsesClassDict(257); 
      }
      UsesClassDef *ucd=m_usesIntfClassDict->find(cd->name());
      if (ucd==0)
      {
        ucd = new UsesClassDef(cd);
        m_usesIntfClassDict->insert(cd->name(),ucd);
        //printf("in class `%s' adding used intf class `%s'\n",
        //  name().data(),cd->name().data());
      }
      ucd->addAccessor(md->name());
      //printf("in class `%s' adding accessor `%s' to class `%s'\n",
      //    name().data(),md->name().data(),ucd->classDef->name().data());
    }
    p=i+l;
  }
}

void ClassDef::determineIntfUsageRelation()
{
  MemberNameInfoSDict::Iterator mnili(*m_allMemberNameInfoList);
  MemberNameInfo *mni;
  for (;(mni=mnili.current());++mnili)
  {
    MemberNameInfoIterator mnii(*mni);
    MemberInfo *mi;
    for (mnii.toFirst();(mi=mnii.current());++mnii)
    {
      MemberDef *md=mi->memberDef;
      
      // compute the protection level for this member
      Protection protect=md->protection();
      if (mi->prot==Protected) // inherited protection
      {
        if (protect==Public) protect=Protected;
        else if (protect==Protected) protect=Private;
      }
      
      if (!md->name().isEmpty() && md->name()[0]!='@' && 
          (mi->prot!=Private && protect!=Private)
         )
      {
        // add classes found in the return type
        addUsedInterfaceClasses(md,md->typeString());
        ArgumentList *al = md->argumentList();
        if (al) // member has arguments
        {
          // add classes found in the types of the argument list
          ArgumentListIterator ali(*al);
          Argument *a;
          for (;(a=ali.current());++ali)
          {
            if (!a->type.isEmpty() && a->type.at(0)!='@')
            {
              addUsedInterfaceClasses(md,a->type);
            }
          }
        }
      }
    }
  }
}
#endif

QCString ClassDef::compoundTypeString() const
{
  if (m_compType==Interface && m_isObjC) return "class";
  switch (m_compType)
  {
    case Class:     return "class";
    case Struct:    return "struct";
    case Union:     return "union";
    case Interface: return "interface";
    case Protocol:  return "protocol";
    case Category:  return "category";
    case Exception: return "exception";
    default:        return "unknown";
  }
}

QCString ClassDef::getOutputFileBase() const 
{ 
  if (m_templateMaster)
  {
    return m_templateMaster->getOutputFileBase();
  }
  else if (isReference())
  {
    return m_fileName;
  }
  else
  {
    return convertNameToFile(m_fileName); 
  }
}

QCString ClassDef::getInstanceOutputFileBase() const 
{ 
  if (isReference())
  {
    return m_fileName;
  }
  else
  {
    return convertNameToFile(m_fileName); 
  }
}

QCString ClassDef::getFileBase() const 
{ 
  if (m_templateMaster)
  {
    return m_templateMaster->getFileBase();
  }
  else
  {
    return m_fileName; 
  }
}

QCString ClassDef::getSourceFileBase() const 
{ 
  if (m_templateMaster)
  {
    return m_templateMaster->getSourceFileBase();
  }
  else
  {
    return convertNameToFile(m_fileName+"-source"); 
  }
}

void ClassDef::setGroupDefForAllMembers(GroupDef *gd,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs)
{
  gd->addClass(this);
  //printf("ClassDef::setGroupDefForAllMembers(%s)\n",gd->name().data());
  MemberNameInfoSDict::Iterator mnili(*m_allMemberNameInfoSDict);
  MemberNameInfo *mni;
  for (;(mni=mnili.current());++mnili)
  {
    MemberNameInfoIterator mnii(*mni);
    MemberInfo *mi;
    for (mnii.toFirst();(mi=mnii.current());++mnii)
    {
      MemberDef *md=mi->memberDef;
      md->setGroupDef(gd,pri,fileName,startLine,hasDocs);
      gd->insertMember(md,TRUE);
      ClassDef *innerClass = md->getClassDefOfAnonymousType();
      if (innerClass) innerClass->setGroupDefForAllMembers(gd,pri,fileName,startLine,hasDocs);
    }
  }
}

void ClassDef::addInnerCompound(Definition *d)
{
  m_innerClasses->inSort(d->localName(),(ClassDef *)d);
}

Definition *ClassDef::findInnerCompound(const char *name)
{
  if (name==0) return 0;
  return m_innerClasses->find(name);
}

//void ClassDef::initTemplateMapping()
//{
//  m_templateMapping->clear();
//  ArgumentList *al = templateArguments();
//  if (al)
//  {
//    ArgumentListIterator ali(*al);
//    Argument *arg;
//    for (ali.toFirst();(arg=ali.current());++ali)
//    {
//      setTemplateArgumentMapping(arg->name,arg->defval);
//    }
//  }
//}
//void ClassDef::setTemplateArgumentMapping(const char *formal,const char *actual)
//{
//  //printf("ClassDef::setTemplateArgumentMapping(%s,%s)\n",formal,actual);
//  if (m_templateMapping && formal)
//  {
//    if (m_templateMapping->find(formal))
//    {
//      m_templateMapping->remove(formal);
//    }
//    m_templateMapping->insert(formal,new QCString(actual));
//  }
//}
//
//QCString ClassDef::getTemplateArgumentMapping(const char *formal) const
//{
//  if (m_templateMapping && formal)
//  {
//    QCString *s = m_templateMapping->find(formal);
//    if (s)
//    {
//      return *s;
//    }
//  }
//  return "";
//}

ClassDef *ClassDef::insertTemplateInstance(const QCString &fileName,
    int startLine, const QCString &templSpec,bool &freshInstance)
{
  freshInstance = FALSE;
  if (m_templateInstances==0) 
  {
    m_templateInstances = new QDict<ClassDef>(17);
  }
  ClassDef *templateClass=m_templateInstances->find(templSpec);
  if (templateClass==0)
  {
    Debug::print(Debug::Classes,0,"      New template instance class `%s'`%s'\n",name().data(),templSpec.data());
    templateClass = new ClassDef(
        fileName,startLine,name()+templSpec,ClassDef::Class);
    templateClass->setTemplateMaster(this);
    templateClass->setOuterScope(getOuterScope());
    m_templateInstances->insert(templSpec,templateClass);
    freshInstance=TRUE;
  }
  return templateClass;
}

ClassDef *ClassDef::getVariableInstance(const char *templSpec)
{
  if (m_variableInstances==0) 
  {
    m_variableInstances = new QDict<ClassDef>(17);
    m_variableInstances->setAutoDelete(TRUE);
  }
  ClassDef *templateClass=m_variableInstances->find(templSpec);
  if (templateClass==0)
  {
    Debug::print(Debug::Classes,0,"      New template variable instance class `%s'`%s'\n",name().data(),templSpec);
    templateClass = new ClassDef("<code>",1,name()+templSpec,
                        ClassDef::Class,0,0,FALSE);
    templateClass->addMembersToTemplateInstance( this, templSpec );
    templateClass->setTemplateMaster(this);
    m_variableInstances->insert(templSpec,templateClass);
  }
  return templateClass;
}

void ClassDef::setTemplateBaseClassNames(QDict<int> *templateNames)
{
  if (templateNames==0) return;
  if (m_templBaseClassNames==0)
  {
    m_templBaseClassNames = new QDict<int>(17);
    m_templBaseClassNames->setAutoDelete(TRUE);
  }
  // make a deep copy of the dictionary.
  QDictIterator<int> qdi(*templateNames);
  for (;qdi.current();++qdi)
  {
    if (m_templBaseClassNames->find(qdi.currentKey())==0)
    {
      m_templBaseClassNames->insert(qdi.currentKey(),new int(*qdi.current()));
    }
  }
}

QDict<int> *ClassDef::getTemplateBaseClassNames() const
{
  return m_templBaseClassNames;
}

void ClassDef::addMembersToTemplateInstance(ClassDef *cd,const char *templSpec)
{
  //printf("%s::addMembersToTemplateInstance(%s,%s)\n",name().data(),cd->name().data(),templSpec);
  MemberNameInfoSDict::Iterator mnili(*cd->m_allMemberNameInfoSDict);
  MemberNameInfo *mni;
  for (;(mni=mnili.current());++mnili)
  {
    MemberNameInfoIterator mnii(*mni);
    MemberInfo *mi;
    for (mnii.toFirst();(mi=mnii.current());++mnii)
    {
      ArgumentList *actualArguments = new ArgumentList;
      stringToArgumentList(templSpec,actualArguments);
      MemberDef *md = mi->memberDef;
      MemberDef *imd = md->createTemplateInstanceMember(
                          cd->templateArguments(),actualArguments);
      delete actualArguments;
      //printf("%s->setMemberClass(%p)\n",imd->name().data(),this);
      imd->setMemberClass(this);
      imd->setTemplateMaster(md);
      imd->setDocumentation(md->documentation(),md->docFile(),md->docLine());
      imd->setBriefDescription(md->briefDescription(),md->briefFile(),md->briefLine());
      imd->setInbodyDocumentation(md->inbodyDocumentation(),md->inbodyFile(),md->inbodyLine());
      imd->setMemberSpecifiers(md->getMemberSpecifiers());
      insertMember(imd);
      //printf("Adding member=%s %s%s to class %s templSpec %s\n",
      //    imd->typeString(),imd->name().data(),imd->argsString(),
      //    imd->getClassDef()->name().data(),templSpec);
      // insert imd in the list of all members
      //printf("Adding member=%s class=%s\n",imd->name().data(),name().data());
      MemberName *mn = Doxygen::memberNameSDict[imd->name()];
      if (mn==0)
      {
        mn = new MemberName(imd->name());
        Doxygen::memberNameSDict.append(imd->name(),mn);
      }
      mn->append(imd);
    }
  }
}

QCString ClassDef::getReference() const
{
  if (m_templateMaster)
  {
    return m_templateMaster->getReference();
  }
  else
  {
    return Definition::getReference();
  }
}

bool ClassDef::isReference() const
{
  if (m_templateMaster)
  {
    return m_templateMaster->isReference();
  }
  else
  {
    return Definition::isReference();
  }
}

void ClassDef::getTemplateParameterLists(QList<ArgumentList> &lists) const
{
  Definition *d=getOuterScope();
  if (d)
  {
    if (d->definitionType()==Definition::TypeClass)
    {
      ClassDef *cd=(ClassDef *)d;
      cd->getTemplateParameterLists(lists);
    }
  }
  if (templateArguments())
  {
    lists.append(templateArguments());
  }
}

QCString ClassDef::qualifiedNameWithTemplateParameters(
    QList<ArgumentList> *actualParams) const
{
  //printf("qualifiedNameWithTemplateParameters() localName=%s\n",localName().data());
  QCString scName;
  Definition *d=getOuterScope();
  if (d)
  {
    if (d->definitionType()==Definition::TypeClass)
    {
      ClassDef *cd=(ClassDef *)d;
      scName = cd->qualifiedNameWithTemplateParameters(actualParams);
    }
    else
    {
      scName = d->qualifiedName();
    }
  }

  QCString scopeSeparator;
  if (Config_getBool("OPTIMIZE_OUTPUT_JAVA")) 
    scopeSeparator=".";
  else
    scopeSeparator="::";

  if (!scName.isEmpty()) scName+=scopeSeparator;
  scName+=localName();
  ArgumentList *al=0;
  bool isSpecialization = localName().find('<')!=-1;
  if (templateArguments())
  {
    if (actualParams && (al=actualParams->current()))
    {
      if (!isSpecialization)
      {
        scName+=tempArgListToString(al);
      }
      actualParams->next();
    }
    else
    {
      if (!isSpecialization)
      {
        scName+=tempArgListToString(templateArguments());
      }
    }
  }
  //printf("qualifiedNameWithTemplateParameters: scope=%s qualifiedName=%s\n",name().data(),scName.data());
  return scName;
}

QCString ClassDef::className() const
{
  return m_className;
};

void ClassDef::addListReferences()
{
  if (!isLinkableInProject()) return;
  //printf("ClassDef(%s)::addListReferences()\n",name().data());
  addRefItem(xrefListItems(),
             theTranslator->trClass(TRUE,TRUE),
             getOutputFileBase(),displayName()
            );
  MemberGroupSDict::Iterator mgli(*memberGroupSDict);
  MemberGroup *mg;
  for (;(mg=mgli.current());++mgli)
  {
    mg->addListReferences(this);
  }
  constructors.addListReferences(this);
  typedefMembers.addListReferences(this);
  enumMembers.addListReferences(this);
  enumValMembers.addListReferences(this);
  functionMembers.addListReferences(this);
  relatedMembers.addListReferences(this);
  variableMembers.addListReferences(this);
  propertyMembers.addListReferences(this);
  eventMembers.addListReferences(this);
}

MemberDef *ClassDef::getMemberByName(const QCString &name) 
{
  MemberDef *xmd = 0;
  MemberNameInfo *mni = m_allMemberNameInfoSDict->find(name);
  if (mni)
  {
    const int maxInheritanceDepth = 100000;
    int mdist=maxInheritanceDepth;
    MemberNameInfoIterator mnii(*mni);
    MemberInfo *mi;
    for (mnii.toFirst();(mi=mnii.current());++mnii)
    {
      ClassDef *mcd=mi->memberDef->getClassDef();
      int m=minClassDistance(this,mcd);
      //printf("found member in %s linkable=%d m=%d\n",
      //    mcd->name().data(),mcd->isLinkable(),m);
      if (m<mdist && mcd->isLinkable())
      {
        mdist=m;
        xmd=mi->memberDef;
      }
    }
  }
  //printf("getMemberByName(%s)=%p\n",name.data(),xmd);
  return xmd;
}

