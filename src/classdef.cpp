/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
#include "vhdldocgen.h"
#include "layout.h"
#include "arguments.h"
#include "memberlist.h"
#include "groupdef.h"
#include "filedef.h"
#include "namespacedef.h"
#include "membergroup.h"

//-----------------------------------------------------------------------------

/** Private data associated with a ClassDef object. */
class ClassDefImpl
{
  public:
    ClassDefImpl();
   ~ClassDefImpl();
    void init(const char *defFileName, const char *name,
              const QCString &ctStr, const char *fName);

    /*! file name that forms the base for the output file containing the
     *  class documentation. For compatibility with Qt (e.g. links via tag 
     *  files) this name cannot be derived from the class name directly.
     */
    QCString fileName;                   

    /*! Include information about the header file should be included
     *  in the documentation. 0 by default, set by setIncludeFile().
     */
    IncludeInfo *incInfo;                

    /*! List of base class (or super-classes) from which this class derives
     *  directly. 
     */
    BaseClassList *inherits;

    /*! List of sub-classes that directly derive from this class 
     */
    BaseClassList *inheritedBy;

    /*! Namespace this class is part of 
     *  (this is the inner most namespace in case of nested namespaces)
     */
    NamespaceDef  *nspace;              

    /*! File this class is defined in */
    FileDef *fileDef;

    /*! List of all members (including inherited members) */
    MemberNameInfoSDict *allMemberNameInfoSDict;

    /*! Template arguments of this class */
    ArgumentList *tempArgs;

    /*! Type constraints for template parameters */
    ArgumentList *typeConstraints;

    /*! Files that were used for generating the class documentation. */
    QStrList files;

    /*! Examples that use this class */
    ExampleSDict *exampleSDict;

    /*! Holds the kind of "class" this is. */
    ClassDef::CompoundType compType;

    /*! The protection level in which this class was found. 
     *  Typically Public, but for nested classes this can also be Protected
     *  or Private.
     */ 
    Protection prot;

    /*! The inner classes contained in this class. Will be 0 if there are
     *  no inner classes.
     */
    ClassSDict *innerClasses;

    /* classes for the collaboration diagram */
    UsesClassDict *usesImplClassDict;
    UsesClassDict *usedByImplClassDict;
    UsesClassDict *usesIntfClassDict;

    /*! Template instances that exists of this class, the key in the
     *  dictionary is the template argument list.
     */ 
    QDict<ClassDef> *templateInstances;

    /*! Template instances that exists of this class, as defined by variables.
     *  We do NOT want to document these individually. The key in the
     *  dictionary is the template argument list.
     */ 
    QDict<ClassDef> *variableInstances;

    QDict<int> *templBaseClassNames;

    /*! The class this class is an instance of. */
    ClassDef *templateMaster;

    /*! local class name which could be a typedef'ed alias name. */
    QCString className;

    /*! If this class is a Objective-C category, then this points to the
     *  class which is extended.
     */
    ClassDef *categoryOf;

    QList<MemberList> memberLists;

    /* user defined member groups */
    MemberGroupSDict *memberGroupSDict;

    /*! Is this an abstact class? */
    bool isAbstract;

    /*! Is the class part of an unnamed namespace? */
    bool isStatic;

    /*! TRUE if classes members are merged with those of the base classes. */
    bool membersMerged;

    /*! TRUE if the class is defined in a source file rather than a header file. */
    bool isLocal;

    bool isTemplArg;

    /*! Does this class group its user-grouped members
     *  as a sub-section of the normal (public/protected/..) 
     *  groups?
     */
    bool subGrouping; 

    /** Reason of existence is a "use" relation */
    bool usedOnly;

    /** List of titles to use for the summary */
    SDict<QCString> vhdlSummaryTitles;

    /** Is this a simple (non-nested) C structure? */
    bool isSimple;

    /** Does this class overloaded the -> operator? */
    MemberDef *arrowOperator;

    ClassList *taggedInnerClasses;
    ClassDef *tagLessRef;

    /** Does this class represent a Java style enum? */
    bool isJavaEnum;

    bool isGeneric;

    uint64 spec;
};

void ClassDefImpl::init(const char *defFileName, const char *name,
                        const QCString &ctStr, const char *fName)
{
  if (fName)
  {
    fileName=stripExtension(fName);
  }
  else
  {
    fileName=ctStr+name;
  }
  exampleSDict = 0;
  inherits    = 0;
  inheritedBy = 0;
  allMemberNameInfoSDict = 0;
  incInfo=0;
  tempArgs=0;
  typeConstraints=0;
  prot=Public;
  nspace=0;
  fileDef=0;
  usesImplClassDict=0;
  usedByImplClassDict=0;
  usesIntfClassDict=0;
  memberGroupSDict = 0;
  innerClasses = 0;
  subGrouping=Config_getBool("SUBGROUPING");
  templateInstances = 0;
  variableInstances = 0;
  templateMaster =0;
  templBaseClassNames = 0;
  isAbstract = FALSE;
  isStatic = FALSE;
  isTemplArg = FALSE;
  membersMerged = FALSE;
  categoryOf = 0;
  usedOnly = FALSE;
  isSimple = Config_getBool("INLINE_SIMPLE_STRUCTS");
  arrowOperator = 0;
  taggedInnerClasses = 0;
  tagLessRef = 0;
  spec=0;
  //QCString ns;
  //extractNamespaceName(name,className,ns);
  //printf("m_name=%s m_className=%s ns=%s\n",m_name.data(),m_className.data(),ns.data());

  // we cannot use getLanguage at this point, as setLanguage has not been called.
  SrcLangExt lang = getLanguageFromFileName(defFileName);
  if ((lang==SrcLangExt_Cpp || lang==SrcLangExt_ObjC) && 
      guessSection(defFileName)==Entry::SOURCE_SEC)
  {
    isLocal=TRUE;
  }
  else
  {
    isLocal=FALSE;
  }
  isGeneric = lang==SrcLangExt_CSharp && QCString(name).find('<')!=-1;
}

ClassDefImpl::ClassDefImpl() : vhdlSummaryTitles(17)
{
  vhdlSummaryTitles.setAutoDelete(TRUE);
}

ClassDefImpl::~ClassDefImpl()
{
  delete inherits;
  delete inheritedBy;
  delete allMemberNameInfoSDict;
  delete exampleSDict;
  delete usesImplClassDict;
  delete usedByImplClassDict;
  delete usesIntfClassDict;
  delete incInfo;
  delete memberGroupSDict;
  delete innerClasses;
  delete templateInstances;
  delete variableInstances;
  delete templBaseClassNames;
  delete tempArgs;
  delete typeConstraints;
  delete taggedInnerClasses;
}

// constructs a new class definition
ClassDef::ClassDef(
    const char *defFileName,int defLine,int defColumn,
    const char *nm,CompoundType ct,
    const char *lref,const char *fName,
    bool isSymbol,bool isJavaEnum) 
 : Definition(defFileName,defLine,defColumn,removeRedundantWhiteSpace(nm),0,0,isSymbol)
{
  visited=FALSE;
  setReference(lref);
  m_impl = new ClassDefImpl;
  m_impl->compType = ct;
  m_impl->isJavaEnum = isJavaEnum;
  m_impl->init(defFileName,name(),compoundTypeString(),fName);
}

// destroy the class definition
ClassDef::~ClassDef()
{
  delete m_impl;
}

QCString ClassDef::getMemberListFileName() const
{
  return convertNameToFile(compoundTypeString()+name()+"-members");
}

QCString ClassDef::displayName(bool includeScope) const
{
  //static bool optimizeOutputForJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  SrcLangExt lang = getLanguage();
  //static bool vhdlOpt = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  QCString n;
  if (lang==SrcLangExt_VHDL)
  {
    n = VhdlDocGen::getClassName(this);
  }
  else 
  {
    if (includeScope)
    {
      n=qualifiedNameWithTemplateParameters();
    }
    else
    {
      n=className();
    }
  }
  QCString sep=getLanguageSpecificSeparator(lang);
  if (sep!="::")
  {
    n=substitute(n,"::",sep);
  }
  if (m_impl->compType==ClassDef::Protocol && n.right(2)=="-p")
  {
    n="<"+n.left(n.length()-2)+">";
  }
  //else if (n.right(2)=="-g")
  //{
  //  n = n.left(n.length()-2);
  //}
  //printf("ClassDef::displayName()=%s\n",n.data());
  return n;
}

// inserts a base/super class in the inheritance list
void ClassDef::insertBaseClass(ClassDef *cd,const char *n,Protection p,
                               Specifier s,const char *t)
{
  //printf("*** insert base class %s into %s\n",cd->name().data(),name().data());
  //inherits->inSort(new BaseClassDef(cd,p,s,t));
  if (m_impl->inherits==0)
  {
    m_impl->inherits = new BaseClassList;
    m_impl->inherits->setAutoDelete(TRUE);
  }
  m_impl->inherits->append(new BaseClassDef(cd,n,p,s,t));
  m_impl->isSimple = FALSE;
}

// inserts a derived/sub class in the inherited-by list
void ClassDef::insertSubClass(ClassDef *cd,Protection p,
                                Specifier s,const char *t)
{
  //printf("*** insert sub class %s into %s\n",cd->name().data(),name().data());
  if (m_impl->inheritedBy==0)
  {
    m_impl->inheritedBy   = new BaseClassList;
    m_impl->inheritedBy->setAutoDelete(TRUE);
  }
  m_impl->inheritedBy->inSort(new BaseClassDef(cd,0,p,s,t));
  m_impl->isSimple = FALSE;
}

void ClassDef::addMembersToMemberGroup()
{
  QListIterator<MemberList> mli(m_impl->memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberListType_detailedLists)==0)
    {
      ::addMembersToMemberGroup(ml,&m_impl->memberGroupSDict,this);
    }
  }

  // add members inside sections to their groups
  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      if (mg->allMembersInSameSection() && m_impl->subGrouping) 
      {
        //printf("addToDeclarationSection(%s)\n",mg->header().data());
        mg->addToDeclarationSection();
      }
    }
  }
}

// adds new member definition to the class
void ClassDef::internalInsertMember(MemberDef *md,
                                    Protection prot,
                                    bool addToAllList
                                   )
{
  //printf("insertInternalMember(%s) isHidden()=%d\n",md->name().data(),md->isHidden());
  if (md->isHidden()) return;

  if (getLanguage()==SrcLangExt_VHDL)
  {
    QCString title=VhdlDocGen::trVhdlType(md->getMemberSpecifiers(),FALSE);
    if (!m_impl->vhdlSummaryTitles.find(title))
    {
      m_impl->vhdlSummaryTitles.append(title,new QCString(title));
    }
  }

  if (1 /*!isReference()*/) // changed to 1 for showing members of external
                            // classes when HAVE_DOT and UML_LOOK are enabled.
  {
    bool isSimple=FALSE;

    /********************************************/
    /* insert member in the declaration section */
    /********************************************/
    if (md->isRelated() && protectionLevelVisible(prot))
    {
      addMemberToList(MemberListType_related,md,TRUE);
    }
    else if (md->isFriend())
    {
      addMemberToList(MemberListType_friends,md,TRUE);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberType_Service: // UNO IDL
          addMemberToList(MemberListType_services,md,TRUE);
          break;
        case MemberType_Interface: // UNO IDL
          addMemberToList(MemberListType_interfaces,md,TRUE);
          break;
        case MemberType_Signal: // Qt specific
          addMemberToList(MemberListType_signals,md,TRUE);
          break;
        case MemberType_DCOP:   // KDE2 specific
          addMemberToList(MemberListType_dcopMethods,md,TRUE);
          break;
        case MemberType_Property:
          addMemberToList(MemberListType_properties,md,TRUE);
          break;
        case MemberType_Event:
          addMemberToList(MemberListType_events,md,TRUE);
          break;
        case MemberType_Slot:   // Qt specific
          switch (prot)
          {
            case Protected: 
            case Package: // slots in packages are not possible!
              addMemberToList(MemberListType_proSlots,md,TRUE);
              break;
            case Public:    
              addMemberToList(MemberListType_pubSlots,md,TRUE);
              break;
            case Private:   
              addMemberToList(MemberListType_priSlots,md,TRUE);
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
                  addMemberToList(MemberListType_proStaticAttribs,md,TRUE);
                  break;
                case Package: 
                  addMemberToList(MemberListType_pacStaticAttribs,md,TRUE);
                  break;
                case Public:    
                  addMemberToList(MemberListType_pubStaticAttribs,md,TRUE);
                  break;
                case Private:   
                  addMemberToList(MemberListType_priStaticAttribs,md,TRUE);
                  break;
              }
            }
            else // function
            {
              switch (prot)
              {
                case Protected: 
                  addMemberToList(MemberListType_proStaticMethods,md,TRUE);
                  break;
                case Package: 
                  addMemberToList(MemberListType_pacStaticMethods,md,TRUE);
                  break;
                case Public:    
                  addMemberToList(MemberListType_pubStaticMethods,md,TRUE);
                  break;
                case Private:   
                  addMemberToList(MemberListType_priStaticMethods,md,TRUE);
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
                  addMemberToList(MemberListType_proAttribs,md,TRUE);
                  break;
                case Package:
                  addMemberToList(MemberListType_pacAttribs,md,TRUE);
                  break;
                case Public:    
                  addMemberToList(MemberListType_pubAttribs,md,TRUE);
                  isSimple=!md->isFunctionPtr();
                  break;
                case Private:   
                  addMemberToList(MemberListType_priAttribs,md,TRUE);
                  break;
              }
            }
            else if (md->isTypedef() || md->isEnumerate() || md->isEnumValue())
            {
              switch (prot)
              {
                case Protected: 
                  addMemberToList(MemberListType_proTypes,md,TRUE);
                  break;
                case Package: 
                  addMemberToList(MemberListType_pacTypes,md,TRUE);
                  break;
                case Public:    
                  addMemberToList(MemberListType_pubTypes,md,TRUE);
                  isSimple=QCString(md->typeString()).find(")(")==-1;
                  break;
                case Private:   
                  addMemberToList(MemberListType_priTypes,md,TRUE);
                  break;
              }
            }
            else // member function
            {
              switch (prot)
              {
                case Protected: 
                  addMemberToList(MemberListType_proMethods,md,TRUE);
                  break;
                case Package: 
                  addMemberToList(MemberListType_pacMethods,md,TRUE);
                  break;
                case Public:    
                  addMemberToList(MemberListType_pubMethods,md,TRUE);
                  break;
                case Private:   
                  addMemberToList(MemberListType_priMethods,md,TRUE);
                  break;
              }
            }
          }
          break; 
      }
    }
    if (!isSimple) // not a simple field -> not a simple struct
    {
      m_impl->isSimple = FALSE;
    }
    //printf("adding %s simple=%d total_simple=%d\n",name().data(),isSimple,m_impl->isSimple);

    /*******************************************************/
    /* insert member in the detailed documentation section */
    /*******************************************************/
    if ((md->isRelated() && protectionLevelVisible(prot)) || md->isFriend())
    {
      addMemberToList(MemberListType_relatedMembers,md,FALSE);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberType_Service: // UNO IDL
          addMemberToList(MemberListType_serviceMembers,md,FALSE);
          break;
        case MemberType_Interface: // UNO IDL
          addMemberToList(MemberListType_interfaceMembers,md,FALSE);
          break;
        case MemberType_Property:
          addMemberToList(MemberListType_propertyMembers,md,FALSE);
          break;
        case MemberType_Event:
          addMemberToList(MemberListType_eventMembers,md,FALSE);
          break;
        case MemberType_Signal: // fall through
        case MemberType_DCOP:
          addMemberToList(MemberListType_functionMembers,md,FALSE);
          break;
        case MemberType_Slot:
          if (protectionLevelVisible(prot))
          {
            addMemberToList(MemberListType_functionMembers,md,FALSE);
          }
          break;
        default: // any of the other members
          if (protectionLevelVisible(prot))
          {
            switch (md->memberType())
            {
              case MemberType_Typedef:
                addMemberToList(MemberListType_typedefMembers,md,FALSE);
                break;
              case MemberType_Enumeration:
                addMemberToList(MemberListType_enumMembers,md,FALSE);
                break;
              case MemberType_EnumValue:
                addMemberToList(MemberListType_enumValMembers,md,FALSE);
                break;
              case MemberType_Function:
                if (md->isConstructor() || md->isDestructor())
                {
                  MemberList *ml = createMemberList(MemberListType_constructors);
                  ml->append(md);
                }
                else
                {
                  addMemberToList(MemberListType_functionMembers,md,FALSE);
                }
                break;
              case MemberType_Variable:
                addMemberToList(MemberListType_variableMembers,md,FALSE);
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
    m_impl->isAbstract=TRUE;
  }

  if (md->name()=="operator->")
  {
    m_impl->arrowOperator=md;
  }

  //::addClassMemberNameToIndex(md);
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
    if (m_impl->allMemberNameInfoSDict==0)
    {
      m_impl->allMemberNameInfoSDict = new MemberNameInfoSDict(17);
      m_impl->allMemberNameInfoSDict->setAutoDelete(TRUE);
    }
    if ((mni=m_impl->allMemberNameInfoSDict->find(md->name())))
    {
      mni->append(mi);
    }
    else
    {
      mni = new MemberNameInfo(md->name());
      mni->append(mi);
      m_impl->allMemberNameInfoSDict->append(mni->memberName(),mni);
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
  //ClassDef *context = Config_getBool("INLINE_INHERITED_MEMB") ? this : 0;
  //const char *letters = "abcdefghijklmnopqrstuvwxyz0123456789";
  QListIterator<MemberList> mli(m_impl->memberLists);
  MemberList *ml;
  //int index = 0;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberListType_detailedLists)==0)
    {
      setAnchors(ml);
    }
  }

  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->setAnchors();
    }
  }
}

void ClassDef::distributeMemberGroupDocumentation()
{
  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->distributeMemberGroupDocumentation();
    }
  }
}

void ClassDef::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,0,docFile());
  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->findSectionsInDocumentation();
    }
  }
  QListIterator<MemberList> mli(m_impl->memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberListType_detailedLists)==0)
    {
      ml->findSectionsInDocumentation();
    }
  }
}


// add a file name to the used files set
void ClassDef::insertUsedFile(const char *f)
{
  if (m_impl->files.find(f)==-1) m_impl->files.append(f);
  if (m_impl->templateInstances)
  {
    QDictIterator<ClassDef> qdi(*m_impl->templateInstances);
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
    if      (bcd->prot==Protected) sl.append("protected");
    else if (bcd->prot==Private)   sl.append("private");
    if      (bcd->virt==Virtual)   sl.append("virtual");
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
  if (!m_impl->incInfo) m_impl->incInfo=new IncludeInfo;
  if ((includeName && m_impl->incInfo->includeName.isEmpty()) ||
      (fd!=0 && m_impl->incInfo->fileDef==0)
     )
  {
    //printf("Setting file info\n");
    m_impl->incInfo->fileDef     = fd;
    m_impl->incInfo->includeName = includeName;
    m_impl->incInfo->local       = local;
  }
  if (force && includeName) 
  {
    m_impl->incInfo->includeName = includeName;
    m_impl->incInfo->local       = local;
  }
}

// TODO: fix this: a nested template class can have multiple outer templates
//ArgumentList *ClassDef::outerTemplateArguments() const
//{
//  int ti;
//  ClassDef *pcd=0;
//  int pi=0;
//  if (m_impl->tempArgs) return m_impl->tempArgs;
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
    QCString clName = d->localName();
    if (/*clName.right(2)=="-g" ||*/ clName.right(2)=="-p")
    {
      clName = clName.left(clName.length()-2);
    }
    name+=clName;
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

void ClassDef::writeBriefDescription(OutputList &ol,bool exampleFlag)
{
  if (!briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
  {
    ol.startParagraph();
    ol.parseDoc(briefFile(),briefLine(),this,0,
                briefDescription(),TRUE,FALSE,0,TRUE,FALSE);
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::RTF);
    ol.writeString(" \n");
    ol.enable(OutputGenerator::RTF);
    ol.popGeneratorState();

    if (Config_getBool("REPEAT_BRIEF") || 
        !documentation().isEmpty() || 
        exampleFlag
       )
    {
      writeMoreLink(ol,anchor());
    }

    ol.endParagraph();
  }
  ol.writeSynopsis();
}

void ClassDef::writeDetailedDocumentationBody(OutputList &ol)
{
  static bool repeatBrief = Config_getBool("REPEAT_BRIEF");

  ol.startTextBlock();

  if (getLanguage()==SrcLangExt_Cpp)
  {
    writeTemplateSpec(ol,this,compoundTypeString());
  }

  // repeat brief description
  if (!briefDescription().isEmpty() && repeatBrief)
  {
    ol.parseDoc(briefFile(),briefLine(),this,0,briefDescription(),FALSE,FALSE);
  }
  if (!briefDescription().isEmpty() && repeatBrief &&
      !documentation().isEmpty())
  {
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Html);
    ol.writeString("\n\n");
    ol.popGeneratorState();
  }
  // write documentation
  if (!documentation().isEmpty())
  {
    ol.parseDoc(docFile(),docLine(),this,0,documentation(),TRUE,FALSE);
  }
  // write type constraints
  writeTypeConstraints(ol,this,m_impl->typeConstraints);

  // write examples
  if (hasExamples() && m_impl->exampleSDict)
  {
    ol.startSimpleSect(BaseOutputDocInterface::Examples,0,0,theTranslator->trExamples()+": ");
    ol.startDescForItem();
    //ol.startParagraph();
    writeExample(ol,m_impl->exampleSDict);
    //ol.endParagraph();
    ol.endDescForItem();
    ol.endSimpleSect();
  }
  //ol.newParagraph();
  writeSourceDef(ol,name());
  ol.endTextBlock();
}

// write the detailed description for this class
void ClassDef::writeDetailedDescription(OutputList &ol, const QCString &/*pageType*/, bool exampleFlag, 
                                        const QCString &title,const QCString &anchor)
{
  if ((!briefDescription().isEmpty() && Config_getBool("REPEAT_BRIEF")) || 
      !documentation().isEmpty() || 
      (Config_getBool("SOURCE_BROWSER") && getStartBodyLine()!=-1 && getBodyDef()) ||
      exampleFlag)
  {
    ol.pushGeneratorState();
      ol.disable(OutputGenerator::Html);
      ol.writeRuler();
    ol.popGeneratorState();

    ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.writeAnchor(0,anchor.isEmpty() ? QCString("details") : anchor);
    ol.popGeneratorState();

    if (!anchor.isEmpty())
    {
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Html);
      ol.disable(OutputGenerator::Man);
      ol.writeAnchor(getOutputFileBase(),anchor);
      ol.popGeneratorState();
    }

    ol.startGroupHeader();
    ol.parseText(title);
    ol.endGroupHeader();

    writeDetailedDocumentationBody(ol);
  }
  else
  {
    //writeTemplateSpec(ol,this,pageType);
  }
}
    
void ClassDef::showUsedFiles(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);

  SrcLangExt lang = getLanguage();
  ol.writeRuler();
  if (lang==SrcLangExt_Fortran)
  {
    ol.parseText(theTranslator->trGeneratedFromFilesFortran(
          getLanguage()==SrcLangExt_ObjC && m_impl->compType==Interface ? Class : m_impl->compType,
          m_impl->files.count()==1));
  }
  else if (isJavaEnum())
  {
    ol.parseText(theTranslator->trEnumGeneratedFromFiles(m_impl->files.count()==1));
  }
  else if (m_impl->compType==Service)
  {
    ol.parseText(theTranslator->trServiceGeneratedFromFiles(m_impl->files.count()==1));
  }
  else if (m_impl->compType==Singleton)
  {
    ol.parseText(theTranslator->trSingletonGeneratedFromFiles(m_impl->files.count()==1));
  }
  else
  {
    ol.parseText(theTranslator->trGeneratedFromFiles(
          getLanguage()==SrcLangExt_ObjC && m_impl->compType==Interface ? Class : m_impl->compType,
          m_impl->files.count()==1));  
  }

  bool first=TRUE;
  const char *file = m_impl->files.first();
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

      ol.startItemListItem();
      QCString path=fd->getPath();
      if (Config_getBool("FULL_PATH_NAMES"))
      {
        ol.docify(stripFromPath(path));
      }

      QCString fname = fd->name();
      if (!fd->getVersion().isEmpty()) // append version if available
      {
        fname += " (" + fd->getVersion() + ")";
      }

      // for HTML 
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      if (fd->generateSourceFile())
      {
        ol.writeObjectLink(0,fd->getSourceFileBase(),0,fname);
      }
      else if (fd->isLinkable())
      {
        ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,
            fname);
      }
      else
      {
        ol.docify(fname);
      }
      ol.popGeneratorState();

      // for other output formats
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Html);
      if (fd->isLinkable())
      {
        ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,
            fname);
      }
      else
      {
        ol.docify(fname);
      }
      ol.popGeneratorState();

      ol.endItemListItem();
    }
    file=m_impl->files.next();
  }
  if (!first) ol.endItemList();

  ol.popGeneratorState();
}


void ClassDef::writeInheritanceGraph(OutputList &ol)
{
  // count direct inheritance relations
  int count=0;
  BaseClassDef *ibcd;
  if (m_impl->inheritedBy)
  {
    ibcd=m_impl->inheritedBy->first();
    while (ibcd)
    {
      ClassDef *icd=ibcd->classDef;
      if ( icd->isVisibleInHierarchy()) count++;
      ibcd=m_impl->inheritedBy->next();
    }
  }
  if (m_impl->inherits)
  {
    ibcd=m_impl->inherits->first();
    while (ibcd)
    {
      ClassDef *icd=ibcd->classDef;
      if ( icd->isVisibleInHierarchy()) count++;
      ibcd=m_impl->inherits->next();
    }
  }

  
  bool renderDiagram = FALSE;
  if (Config_getBool("HAVE_DOT") && 
      (Config_getBool("CLASS_DIAGRAMS") || Config_getBool("CLASS_GRAPH")))
    // write class diagram using dot
  {
    DotClassGraph inheritanceGraph(this,DotNode::Inheritance);
    if (!inheritanceGraph.isTrivial() && !inheritanceGraph.isTooBig())
    {
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.startDotGraph();
      ol.parseText(theTranslator->trClassDiagram(displayName()));
      ol.endDotGraph(inheritanceGraph);
      ol.popGeneratorState();
      renderDiagram = TRUE;
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
    renderDiagram = TRUE;
  } 

  if (renderDiagram) // if we already show the inheritance relations graphically,
                     // then hide the text version
  {
    ol.disableAllBut(OutputGenerator::Man);
  }

  if (m_impl->inherits && (count=m_impl->inherits->count())>0)
  {
    ol.startParagraph();
    //parseText(ol,theTranslator->trInherits()+" ");

    QCString inheritLine = theTranslator->trInheritsList(m_impl->inherits->count());
    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(inheritLine,index,&matchLen))!=-1)
    {
      ol.parseText(inheritLine.mid(index,newIndex-index));
      bool ok;
      uint entryIndex = inheritLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
      BaseClassDef *bcd=m_impl->inherits->at(entryIndex);
      if (ok && bcd)
      {
        ClassDef *cd=bcd->classDef;

        // use the class name but with the template arguments as given
        // in the inheritance relation
        QCString displayName = insertTemplateSpecifierInScope(
            cd->displayName(),bcd->templSpecifiers);

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
            Doxygen::tagFile << ">" << convertToXML(cd->name()) 
                             << "</base>" << endl;
          }
          ol.writeObjectLink(cd->getReference(),
                             cd->getOutputFileBase(),
                             cd->anchor(),
                             displayName);
        }
        else
        {
          ol.docify(displayName);
        }
      }
      else
      {
        err("error: invalid marker %d in inherits list!\n",entryIndex);
      }
      index=newIndex+matchLen;
    } 
    ol.parseText(inheritLine.right(inheritLine.length()-index));
    ol.endParagraph();
  }

  // write subclasses
  if (m_impl->inheritedBy && (count=m_impl->inheritedBy->count())>0)
  {
    ol.startParagraph();
    QCString inheritLine = theTranslator->trInheritedByList(m_impl->inheritedBy->count());
    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(inheritLine,index,&matchLen))!=-1)
    {
      ol.parseText(inheritLine.mid(index,newIndex-index));
      bool ok;
      uint entryIndex = inheritLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
      BaseClassDef *bcd=m_impl->inheritedBy->at(entryIndex);
      if (ok && bcd)
      {
        ClassDef *cd=bcd->classDef;
        if (cd->isLinkable())
        {
          ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),cd->anchor(),cd->displayName());
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
    ol.endParagraph();
  }

  if (renderDiagram) 
  {
    ol.enableAll();
  }
}

void ClassDef::writeCollaborationGraph(OutputList &ol)
{
  if (Config_getBool("HAVE_DOT") /*&& Config_getBool("COLLABORATION_GRAPH")*/)
  {
    DotClassGraph usageImplGraph(this,DotNode::Collaboration);
    if (!usageImplGraph.isTrivial())
    {
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.startDotGraph();
      ol.parseText(theTranslator->trCollaborationDiagram(displayName()));
      ol.endDotGraph(usageImplGraph);
      ol.popGeneratorState();
    }
  }
}

void ClassDef::writeIncludeFiles(OutputList &ol)
{
  if (m_impl->incInfo /*&& Config_getBool("SHOW_INCLUDE_FILES")*/)
  {
    QCString nm=m_impl->incInfo->includeName.isEmpty() ? 
      (m_impl->incInfo->fileDef ?
       m_impl->incInfo->fileDef->docName().data() : "" 
      ) :
      m_impl->incInfo->includeName.data();
    if (!nm.isEmpty())
    {
      ol.startParagraph();
      ol.startTypewriter();
      SrcLangExt lang = getLanguage();
      bool isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
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
      if (m_impl->incInfo->local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify("<");
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Html);
      ol.docify(nm);
      ol.disableAllBut(OutputGenerator::Html);
      ol.enable(OutputGenerator::Html);
      if (m_impl->incInfo->fileDef)
      {
        ol.writeObjectLink(0,m_impl->incInfo->fileDef->includeName(),0,nm);
      }
      else
      {
        ol.docify(nm);
      }
      ol.popGeneratorState();
      if (m_impl->incInfo->local || isIDLorJava)
        ol.docify("\"");
      else
        ol.docify(">");
      if (isIDLorJava) 
        ol.docify(";");
      ol.endTypewriter();
      ol.endParagraph();
    }
  }
}

#if 0
void ClassDef::writeAllMembersLink(OutputList &ol)
{
  // write link to list of all members (HTML only)
  if (m_impl->allMemberNameInfoSDict &&
      !Config_getBool("OPTIMIZE_OUTPUT_FOR_C")
     )
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.startParagraph();
    ol.startTextLink(getMemberListFileName(),0);
    ol.parseText(theTranslator->trListOfAllMembers());
    ol.endTextLink();
    ol.endParagraph();
    ol.enableAll();
    ol.popGeneratorState();
  }
}
#endif

void ClassDef::writeMemberGroups(OutputList &ol,bool showInline)
{
  // write user defined member groups
  if (m_impl->memberGroupSDict)
  {
    m_impl->memberGroupSDict->sort();
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      if (!mg->allMembersInSameSection() || !m_impl->subGrouping) // group is in its own section
      {
        mg->writeDeclarations(ol,this,0,0,0,showInline);
      }
      else // add this group to the corresponding member section
      {
        //printf("addToDeclarationSection(%s)\n",mg->header().data());
        //mg->addToDeclarationSection();
      }
    }
  }
}

void ClassDef::writeNestedClasses(OutputList &ol,const QCString &title)
{
  // nested classes
  if (m_impl->innerClasses) 
  {
    m_impl->innerClasses->writeDeclaration(ol,0,title,TRUE);
  }
}

void ClassDef::writeInlineClasses(OutputList &ol)
{
  if (m_impl->innerClasses) 
  {
    m_impl->innerClasses->writeDocumentation(ol,this);
  }
}

void ClassDef::startMemberDocumentation(OutputList &ol)
{
  //printf("%s: ClassDef::startMemberDocumentation()\n",name().data());
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
}

void ClassDef::endMemberDocumentation(OutputList &ol)
{
  //printf("%s: ClassDef::endMemberDocumentation()\n",name().data());
  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.enable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = FALSE;
  }
}

void ClassDef::startMemberDeclarations(OutputList &ol)
{
  //printf("%s: ClassDef::startMemberDeclarations()\n",name().data());
  ol.startMemberSections();
}

void ClassDef::endMemberDeclarations(OutputList &ol)
{
  //printf("%s: ClassDef::endMemberDeclarations()\n",name().data());
  static bool inlineInheritedMembers = Config_getBool("INLINE_INHERITED_MEMB");
  if (!inlineInheritedMembers && countAdditionalInheritedMembers()>0)
  {
    ol.startMemberHeader("inherited");
    ol.parseText(theTranslator->trAdditionalInheritedMembers());
    ol.endMemberHeader();
    writeAdditionalInheritedMembers(ol);
  }
  ol.endMemberSections();
}

void ClassDef::writeAuthorSection(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Man);
  ol.writeString("\n");
  ol.startGroupHeader();
  ol.parseText(theTranslator->trAuthor(TRUE,TRUE));
  ol.endGroupHeader();
  ol.parseText(theTranslator->trGeneratedAutomatically(Config_getString("PROJECT_NAME")));
  ol.popGeneratorState();
}


void ClassDef::writeSummaryLinks(OutputList &ol)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Class));
  LayoutDocEntry *lde;
  bool first=TRUE;
  SrcLangExt lang = getLanguage();
  
  if (lang!=SrcLangExt_VHDL)
  {
    for (eli.toFirst();(lde=eli.current());++eli)
    {
      if (lde->kind()==LayoutDocEntry::ClassNestedClasses && 
          m_impl->innerClasses  &&
          m_impl->innerClasses->declVisible()
         )
      {
        LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
        ol.writeSummaryLink(0,"nested-classes",ls->title(lang),first);
        first=FALSE;
      }
      else if (lde->kind()==LayoutDocEntry::ClassAllMembersLink &&
               m_impl->allMemberNameInfoSDict &&
               !Config_getBool("OPTIMIZE_OUTPUT_FOR_C")
              )
      {
        ol.writeSummaryLink(getMemberListFileName(),"all-members-list",theTranslator->trListOfAllMembers(),first);
        first=FALSE;
      }
      else if (lde->kind()== LayoutDocEntry::MemberDecl)
      {
        LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
        MemberList * ml = getMemberList(lmd->type);
        if (ml && ml->declVisible())
        {
          ol.writeSummaryLink(0,ml->listTypeAsString(),lmd->title(lang),first);
          first=FALSE;
        }
      }
    }
  }
  else // VDHL only
  {
    SDict<QCString>::Iterator li(m_impl->vhdlSummaryTitles);
    for (li.toFirst();li.current();++li)
    {
      ol.writeSummaryLink(0,li.current()->data(),li.current()->data(),first);
      first=FALSE;
    }
  }
  if (!first)
  {
    ol.writeString("  </div>\n");
  }
  ol.popGeneratorState();
}

void ClassDef::writeTagFileMarker()
{
  // write section to the tag file
  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    Doxygen::tagFile << "  <compound kind=\"" << compoundTypeString();
    Doxygen::tagFile << "\"";
    if (isObjectiveC()) { Doxygen::tagFile << " objc=\"yes\""; }
    Doxygen::tagFile << ">" << endl;
    Doxygen::tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << Doxygen::htmlFileExtension << "</filename>" << endl;
    if (!anchor().isEmpty())
    {
      Doxygen::tagFile << "    <anchor>" << convertToXML(anchor()) << "</anchor>" << endl;
    }
    if (m_impl->tempArgs)
    {
      ArgumentListIterator ali(*m_impl->tempArgs);
      Argument *a;
      for (;(a=ali.current());++ali)
      {
        Doxygen::tagFile << "    <templarg>" << convertToXML(a->name) << "</templarg>" << endl;
      }
    }
  }
}

/** Write class documentation inside another container (i.e. a group) */
void ClassDef::writeInlineDocumentation(OutputList &ol)
{
  bool isSimple = m_impl->isSimple;

  ol.addIndexItem(name(),0);
  //printf("ClassDef::writeInlineDocumentation(%s)\n",name().data());
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Class));
  LayoutDocEntry *lde;

  // part 1: anchor and title
  QCString s = compoundTypeString()+" "+name(); 

  // part 1a
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  { // only HTML only
    ol.writeAnchor(0,anchor());
    ol.startMemberDoc(0,0,0,0,FALSE);
    ol.startMemberDocName(FALSE);
    ol.parseText(s);
    ol.endMemberDocName();
    ol.endMemberDoc(FALSE);
    ol.writeString("</div>");
    ol.startIndent();
  }
  ol.popGeneratorState();

  // part 1b
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);
  ol.disable(OutputGenerator::Man);
  { // for LaTeX/RTF only
    ol.writeAnchor(getOutputFileBase(),anchor());
  }
  ol.popGeneratorState();

  // part 1c
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);
  {
    // for LaTeX/RTF/Man
    ol.startGroupHeader(1);
    ol.parseText(s);
    ol.endGroupHeader(1);
  }
  ol.popGeneratorState();

  SrcLangExt lang=getLanguage();

  // part 2: the header and detailed description
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc: 
        {
          // since we already shown the brief description in the
          // declaration part of the container, so we use this to
          // show the details on top.
          writeDetailedDocumentationBody(ol);
        }
        break;
      case LayoutDocEntry::ClassInheritanceGraph: 
        writeInheritanceGraph(ol); 
        break; 
      case LayoutDocEntry::ClassCollaborationGraph: 
        writeCollaborationGraph(ol); 
        break; 
      case LayoutDocEntry::MemberDeclStart: 
        if (!isSimple) startMemberDeclarations(ol);
        break; 
      case LayoutDocEntry::MemberDecl:
        {
          LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
          if (!isSimple) writeMemberDeclarations(ol,lmd->type,lmd->title(lang),lmd->subtitle(lang),TRUE);
        }
        break;
      case LayoutDocEntry::MemberGroups:
        if (!isSimple) writeMemberGroups(ol,TRUE);
        break;
      case LayoutDocEntry::MemberDeclEnd: 
        if (!isSimple) endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::MemberDefStart: 
        if (!isSimple) startMemberDocumentation(ol);
        break; 
      case LayoutDocEntry::MemberDef: 
        {
          LayoutDocEntryMemberDef *lmd = (LayoutDocEntryMemberDef*)lde;
          if (isSimple)
          {
            writeSimpleMemberDocumentation(ol,lmd->type);
          }
          else
          {
            writeMemberDocumentation(ol,lmd->type,lmd->title(lang),TRUE);
          }
        }
        break; 
      case LayoutDocEntry::MemberDefEnd: 
        if (!isSimple) endMemberDocumentation(ol);
        break;
      default:
        break;
    }
  }

  // part 3: close the block
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  { // HTML only
    ol.endIndent();
  }
  ol.popGeneratorState();

  // part 4: write tag file information
  writeTagFileMarker();
}

void ClassDef::writeMoreLink(OutputList &ol,const QCString &anchor)
{
  // TODO: clean up this mess by moving it to
  // the output generators...
  static bool pdfHyperlinks = Config_getBool("PDF_HYPERLINKS");
  static bool rtfHyperlinks = Config_getBool("RTF_HYPERLINKS");
  static bool usePDFLatex   = Config_getBool("USE_PDFLATEX");

  // HTML only
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  ol.docify(" ");
  ol.startTextLink(getOutputFileBase(),
      anchor.isEmpty() ? QCString("details") : anchor);
  ol.parseText(theTranslator->trMore());
  ol.endTextLink();
  ol.popGeneratorState();

  if (!anchor.isEmpty())
  {
    ol.pushGeneratorState();
    // LaTeX + RTF
    ol.disable(OutputGenerator::Html);
    ol.disable(OutputGenerator::Man);
    if (!(usePDFLatex && pdfHyperlinks))
    {
      ol.disable(OutputGenerator::Latex);
    }
    if (!rtfHyperlinks)
    {
      ol.disable(OutputGenerator::RTF);
    }
    ol.docify(" ");
    ol.startTextLink(getOutputFileBase(), anchor);
    ol.parseText(theTranslator->trMore());
    ol.endTextLink();
    // RTF only
    ol.disable(OutputGenerator::Latex);
    ol.writeString("\\par");
    ol.popGeneratorState();
  }
}


void ClassDef::writeDeclarationLink(OutputList &ol,bool &found,const char *header,bool localNames)
{
  //static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  //static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  static bool hideUndocClasses = Config_getBool("HIDE_UNDOC_CLASSES");
  static bool extractLocalClasses = Config_getBool("EXTRACT_LOCAL_CLASSES");
  bool isLink = isLinkable();
  SrcLangExt lang = getLanguage();
  if (isLink || 
      (!hideUndocClasses && 
       (!isLocal() || extractLocalClasses)
      )
     )
  {
    if (!found) // first class
    {
      ol.startMemberHeader("nested-classes");
      if (header)
      {
        ol.parseText(header);
      }
      else if (lang==SrcLangExt_VHDL)
      {
        ol.parseText(VhdlDocGen::trVhdlType(VhdlDocGen::ARCHITECTURE,FALSE));
      }
      else
      {
        ol.parseText(lang==SrcLangExt_Fortran ? 
            theTranslator->trDataTypes() :
            theTranslator->trCompounds());
      }
      ol.endMemberHeader();
      ol.startMemberList();
      found=TRUE;
    }
    if (!Config_getString("GENERATE_TAGFILE").isEmpty() &&
        !isReference())  // skip classes found in tag files
    {
      Doxygen::tagFile << "    <class kind=\"" << compoundTypeString() 
        << "\">" << convertToXML(name()) << "</class>" << endl;
    }
    ol.startMemberDeclaration();
    ol.startMemberItem(anchor(),FALSE);
    QCString ctype = compoundTypeString();
    QCString cname = displayName(!localNames);

    if (lang!=SrcLangExt_VHDL) // for VHDL we swap the name and the type
    {
      ol.writeString(ctype);
      ol.writeString(" ");
      ol.insertMemberAlign();
    }
    if (isLink) 
    {
      ol.writeObjectLink(getReference(),
          getOutputFileBase(),
          anchor(),
          cname
          );
    }
    else 
    {
      ol.startBold();
      ol.docify(cname);
      ol.endBold();
    }
    if (lang==SrcLangExt_VHDL) // now write the type
    {
      ol.writeString(" ");
      ol.insertMemberAlign();
      ol.writeString(VhdlDocGen::getProtectionName((VhdlDocGen::VhdlClasses)protection()));
    }
    ol.endMemberItem();

    // add the brief description if available
    if (!briefDescription().isEmpty() && Config_getBool("BRIEF_MEMBER_DESC"))
    {
      ol.startMemberDescription(anchor());
      ol.parseDoc(briefFile(),briefLine(),this,0,
          briefDescription(),FALSE,FALSE,0,TRUE,FALSE);
      if (isLinkableInProject())
      {
        writeMoreLink(ol,anchor());
      }
      ol.endMemberDescription();
    }
    ol.endMemberDeclaration(anchor(),0);
  }
}

void ClassDef::addClassAttributes(OutputList &ol)
{
  QStrList sl;
  if (isFinal())    sl.append("final");
  if (isSealed())   sl.append("sealed");
  if (isAbstract()) sl.append("abstract");
  if (getLanguage()==SrcLangExt_IDL && isPublished()) sl.append("published");

  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  if (sl.count()>0)
  {
    ol.startLabels();
    const char *s=sl.first();
    while (s)
    {
      const char *ns = sl.next();
      ol.writeLabel(s,ns==0);
      s=ns;
    }
    ol.endLabels();
  }
  ol.popGeneratorState();
}

void ClassDef::writeDocumentationContents(OutputList &ol,const QCString & /*pageTitle*/)
{
  ol.startContents();

  QCString pageType = " ";
  pageType += compoundTypeString();
  toupper(pageType.at(1));

  writeTagFileMarker();

  Doxygen::indexList->addIndexItem(this,0);

  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    Doxygen::searchIndex->addWord(localName(),TRUE);
  }
  bool exampleFlag=hasExamples();

  //---------------------------------------- start flexible part -------------------------------

  SrcLangExt lang = getLanguage();

  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Class));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    switch (lde->kind())
    {
      case LayoutDocEntry::BriefDesc: 
        writeBriefDescription(ol,exampleFlag);
        break; 
      case LayoutDocEntry::ClassIncludes: 
        writeIncludeFiles(ol);
        break;
      case LayoutDocEntry::ClassInheritanceGraph: 
        writeInheritanceGraph(ol); 
        break; 
      case LayoutDocEntry::ClassCollaborationGraph: 
        writeCollaborationGraph(ol); 
        break; 
      case LayoutDocEntry::ClassAllMembersLink: 
        //writeAllMembersLink(ol); // this is now part of the summary links
        break;
      case LayoutDocEntry::MemberDeclStart: 
        startMemberDeclarations(ol);
        break; 
      case LayoutDocEntry::MemberGroups: 
        writeMemberGroups(ol);
        break;
      case LayoutDocEntry::MemberDecl: 
        {
          LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
          writeMemberDeclarations(ol,lmd->type,lmd->title(lang),lmd->subtitle(lang));
        }
        break; 
      case LayoutDocEntry::ClassNestedClasses: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeNestedClasses(ol,ls->title(lang));
        }
        break;
      case LayoutDocEntry::MemberDeclEnd: 
        endMemberDeclarations(ol);
        break;
      case LayoutDocEntry::DetailedDesc: 
        {
          LayoutDocEntrySection *ls = (LayoutDocEntrySection*)lde;
          writeDetailedDescription(ol,pageType,exampleFlag,ls->title(lang));
        }
        break; 
      case LayoutDocEntry::MemberDefStart: 
        startMemberDocumentation(ol);
        break; 
      case LayoutDocEntry::ClassInlineClasses:
        writeInlineClasses(ol);
        break;
      case LayoutDocEntry::MemberDef: 
        {
          LayoutDocEntryMemberDef *lmd = (LayoutDocEntryMemberDef*)lde;
          writeMemberDocumentation(ol,lmd->type,lmd->title(lang));
        }
        break; 
      case LayoutDocEntry::MemberDefEnd: 
        endMemberDocumentation(ol);
        break;
      case LayoutDocEntry::ClassUsedFiles: 
        showUsedFiles(ol);
        break;
      case LayoutDocEntry::AuthorSection: 
        writeAuthorSection(ol);
        break;
      case LayoutDocEntry::NamespaceNestedNamespaces:
      case LayoutDocEntry::NamespaceNestedConstantGroups:
      case LayoutDocEntry::NamespaceClasses:
      case LayoutDocEntry::NamespaceInlineClasses:
      case LayoutDocEntry::FileClasses:
      case LayoutDocEntry::FileNamespaces:
      case LayoutDocEntry::FileConstantGroups:
      case LayoutDocEntry::FileIncludes:
      case LayoutDocEntry::FileIncludeGraph:
      case LayoutDocEntry::FileIncludedByGraph: 
      case LayoutDocEntry::FileSourceLink:
      case LayoutDocEntry::FileInlineClasses:
      case LayoutDocEntry::GroupClasses: 
      case LayoutDocEntry::GroupInlineClasses: 
      case LayoutDocEntry::GroupNamespaces:
      case LayoutDocEntry::GroupDirs: 
      case LayoutDocEntry::GroupNestedGroups: 
      case LayoutDocEntry::GroupFiles:
      case LayoutDocEntry::GroupGraph: 
      case LayoutDocEntry::GroupPageDocs:
      case LayoutDocEntry::DirSubDirs:
      case LayoutDocEntry::DirFiles:
      case LayoutDocEntry::DirGraph:
        err("Internal inconsistency: member %d should not be part of "
            "LayoutDocManager::Class entry list\n",lde->kind());
        break;
    }
  }

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }
  ol.endContents();
}

// write all documentation for this class
void ClassDef::writeDocumentation(OutputList &ol)
{
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
  //static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");
  //static bool vhdlOpt    = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  QCString pageTitle;
  SrcLangExt lang = getLanguage();
    
  if (lang==SrcLangExt_Fortran)
  {
    pageTitle = theTranslator->trCompoundReferenceFortran(displayName(),
              m_impl->compType,
              m_impl->tempArgs != 0);  
  }
  else if (lang==SrcLangExt_VHDL)
  {
    pageTitle = VhdlDocGen::getClassTitle(this)+" Reference";
  }
  else if (isJavaEnum())
  {
    pageTitle = theTranslator->trEnumReference(displayName());
  }
  else if (m_impl->compType==Service)
  {
    pageTitle = theTranslator->trServiceReference(displayName());
  }
  else if (m_impl->compType==Singleton)
  {
    pageTitle = theTranslator->trSingletonReference(displayName());
  }
  else
  {
    pageTitle = theTranslator->trCompoundReference(displayName(),
              m_impl->compType == Interface && getLanguage()==SrcLangExt_ObjC ? Class : m_impl->compType,
              m_impl->tempArgs != 0);
  }
  
  startFile(ol,getOutputFileBase(),name(),pageTitle,HLI_ClassVisible,!generateTreeView);  
  if (!generateTreeView)
  {
    if (getOuterScope()!=Doxygen::globalScope)
    {
      writeNavigationPath(ol);
    }
    ol.endQuickIndices();
  }

  startTitle(ol,getOutputFileBase(),this);
  ol.parseText(pageTitle);
  addClassAttributes(ol);
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),displayName());
  writeDocumentationContents(ol,pageTitle);
    
  endFileWithNavPath(this,ol);

  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    writeMemberPages(ol);
  }
}

void ClassDef::writeMemberPages(OutputList &ol)
{
  ///////////////////////////////////////////////////////////////////////////
  //// Member definitions on separate pages
  ///////////////////////////////////////////////////////////////////////////

  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  
  QListIterator<MemberList> mli(m_impl->memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberListType_detailedLists)
    {
      ml->writeDocumentationPage(ol,displayName(),this);
    }
  }

  ol.popGeneratorState();
}

void ClassDef::writeQuickMemberLinks(OutputList &ol,MemberDef *currentMd) const
{
  static bool createSubDirs=Config_getBool("CREATE_SUBDIRS");

  ol.writeString("      <div class=\"navtab\">\n");
  ol.writeString("        <table>\n");

  if (m_impl->allMemberNameInfoSDict)
  {
    MemberNameInfoSDict::Iterator mnili(*m_impl->allMemberNameInfoSDict);
    MemberNameInfo *mni;
    for (;(mni=mnili.current());++mnili)
    {
      MemberNameInfoIterator mnii(*mni);
      MemberInfo *mi;
      for (mnii.toFirst();(mi=mnii.current());++mnii)
      {
        MemberDef *md=mi->memberDef;
        if (md->getClassDef()==this && md->isLinkable())
        {
          ol.writeString("          <tr><td class=\"navtab\">");
          if (md->isLinkableInProject())
          {
            if (md==currentMd) // selected item => highlight
            {
              ol.writeString("<a class=\"qindexHL\" ");
            }
            else
            {
              ol.writeString("<a class=\"qindex\" ");
            }
            ol.writeString("href=\"");
            if (createSubDirs) ol.writeString("../../");
            ol.writeString(md->getOutputFileBase()+Doxygen::htmlFileExtension+"#"+md->anchor());
            ol.writeString("\">");
            ol.writeString(convertToHtml(md->name()));
            ol.writeString("</a>");
          }
          ol.writeString("</td></tr>\n");
        }
      }
    }
  }

  ol.writeString("        </table>\n");
  ol.writeString("      </div>\n");
}



void ClassDef::writeDocumentationForInnerClasses(OutputList &ol)
{
  // write inner classes after the parent, so the tag files contain
  // the definition in proper order!
  if (m_impl->innerClasses)
  {
    ClassSDict::Iterator cli(*m_impl->innerClasses);
    ClassDef *innerCd;
    for (cli.toFirst();(innerCd=cli.current());++cli)
    {
      if (innerCd->isLinkableInProject() && innerCd->templateMaster()==0 &&
          protectionLevelVisible(innerCd->protection()) &&
         !innerCd->isEmbeddedInOuterScope()
         )
      {
        msg("Generating docs for nested compound %s...\n",qPrint(innerCd->name()));
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
  static bool cOpt    = Config_getBool("OPTIMIZE_OUTPUT_FOR_C");
  //static bool vhdlOpt = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  static bool generateTreeView = Config_getBool("GENERATE_TREEVIEW");
  if (m_impl->allMemberNameInfoSDict==0 || cOpt) return;
  // only for HTML
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  QCString memListFile = getMemberListFileName();
  startFile(ol,memListFile,memListFile,theTranslator->trMemberList(),
            HLI_ClassVisible,!generateTreeView,getOutputFileBase());  
  if (!generateTreeView)
  {
    if (getOuterScope()!=Doxygen::globalScope)
    {
      writeNavigationPath(ol);
    }
    ol.endQuickIndices();
  }
  startTitle(ol,0);
  ol.parseText(displayName()+" "+theTranslator->trMemberList());
  endTitle(ol,0,0);
  ol.startContents();
  ol.startParagraph();
  ol.parseText(theTranslator->trThisIsTheListOfAllMembers());
  ol.writeObjectLink(getReference(),getOutputFileBase(),anchor(),displayName());
  ol.parseText(theTranslator->trIncludingInheritedMembers());
  ol.endParagraph();
  
  //ol.startItemList();
  ol.writeString("<table class=\"directory\">\n");
  
  int idx=0;
  //MemberNameInfo *mni=m_impl->allMemberNameInfoList->first();
  MemberNameInfoSDict::Iterator mnii(*m_impl->allMemberNameInfoSDict); 
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
          ol.writeString("  <tr");
          if ((idx&1)==0) ol.writeString(" class=\"even\"");
          idx++;
          ol.writeString("><td class=\"entry\">");
          if (cd->isObjectiveC())
          {
            if (md->isObjCMethod())
            {
              if (md->isStatic())
                ol.writeString("+&#160;</td><td>");
              else
                ol.writeString("-&#160;</td><td>");
            }
            else
              ol.writeString("</td><td class=\"entry\">");
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
            else if (md->isFriend() && !qstrcmp(md->typeString(),"friend class"))
              ol.docify(" class");
            //ol.writeString("\n");
          }
          ol.writeString("</td>");
          memberWritten=TRUE;
        }
        else if (!Config_getBool("HIDE_UNDOC_MEMBERS") && 
                  (protectionLevelVisible(md->protection()) || md->isFriend()) 
                ) // no documentation, 
                  // generate link to the class instead.
        {
          //ol.writeListItem();
          ol.writeString("  <tr bgcolor=\"#f0f0f0\"");
          if ((idx&1)==0) ol.writeString(" class=\"even\"");
          idx++;
          ol.writeString("><td class=\"entry\">");
          if (cd->isObjectiveC())
          {
            if (md->isObjCMethod())
            {
              if (md->isStatic())
                ol.writeString("+&#160;</td><td class=\"entry\">");
              else
                ol.writeString("-&#160;</td><td class=\"entry\">");
            }
            else
              ol.writeString("</td><td class=\"entry\">");
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
            ol.writeObjectLink(
                cd->getReference(),
                cd->getOutputFileBase(),
                cd->anchor(),
                cd->displayName());
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
          ol.writeString("<td class=\"entry\">");
          ol.writeObjectLink(cd->getReference(),
                             cd->getOutputFileBase(),
                             cd->anchor(),
                             md->category() ? 
                                md->category()->displayName() : 
                                cd->displayName());
          ol.writeString("</td>");
          ol.writeString("<td class=\"entry\">");
        }
        SrcLangExt lang = md->getLanguage();
        if (
            (prot!=Public || (virt!=Normal && getLanguage()!=SrcLangExt_ObjC) || 
             md->isFriend() || md->isRelated() || md->isExplicit() ||
             md->isMutable() || (md->isInline() && Config_getBool("INLINE_INFO")) ||
             md->isSignal() || md->isSlot() ||
             (getLanguage()==SrcLangExt_IDL &&
              (md->isOptional() || md->isAttribute() || md->isUNOProperty())) ||
             md->isStatic() || lang==SrcLangExt_VHDL
            )
            && memberWritten)
        {
          ol.writeString("<span class=\"mlabel\">");
          QStrList sl;
          if (lang==SrcLangExt_VHDL) 
          {
            sl.append(VhdlDocGen::trVhdlType(md->getMemberSpecifiers())); //append vhdl type
          }
          else if (md->isFriend()) sl.append("friend");
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
            if (virt==Virtual && getLanguage()!=SrcLangExt_ObjC)             
                                       sl.append("virtual");
            else if (virt==Pure)       sl.append("pure virtual");
            if (md->isStatic())        sl.append("static");
            if (md->isSignal())        sl.append("signal");
            if (md->isSlot())          sl.append("slot");
// this is the extra member page
            if (md->isOptional())      sl.append("optional");
            if (md->isAttribute())     sl.append("attribute");
            if (md->isUNOProperty())   sl.append("property");
            if (md->isReadonly())      sl.append("readonly");
            if (md->isBound())         sl.append("bound");
            if (md->isRemovable())     sl.append("removable");
            if (md->isConstrained())   sl.append("constrained");
            if (md->isTransient())     sl.append("transient");
            if (md->isMaybeVoid())     sl.append("maybevoid");
            if (md->isMaybeDefault())  sl.append("maybedefault");
            if (md->isMaybeAmbiguous())sl.append("maybeambiguous");
          }
          const char *s=sl.first();
          while (s)
          {
            ol.docify(s);
            s=sl.next();
            if (s) ol.writeString("</span><span class=\"mlabel\">");
          }
          ol.writeString("</span>");
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
  if (m_impl->exampleSDict==0)
  {
    m_impl->exampleSDict = new ExampleSDict;
    m_impl->exampleSDict->setAutoDelete(TRUE);
  }
  if (!m_impl->exampleSDict->find(nameStr))
  {
    Example *e=new Example;
    e->anchor=anchor;
    e->name=nameStr;
    e->file=file;
    m_impl->exampleSDict->inSort(nameStr,e);
    return TRUE;
  }
  return FALSE;
}

// returns TRUE if this class is used in an example
bool ClassDef::hasExamples()
{
  if (m_impl->exampleSDict==0) 
    return FALSE;
  else
    return m_impl->exampleSDict->count()>0;
}


void ClassDef::setTemplateArguments(ArgumentList *al)
{
  if (al==0) return;
  if (!m_impl->tempArgs) delete m_impl->tempArgs; // delete old list if needed
  m_impl->tempArgs=new ArgumentList; 
  ArgumentListIterator ali(*al);
  Argument *a;
  for (;(a=ali.current());++ali)
  {
    m_impl->tempArgs->append(new Argument(*a));
  }
}

void ClassDef::setTypeConstraints(ArgumentList *al)
{
  if (al==0) return;
  if (!m_impl->typeConstraints) delete m_impl->typeConstraints;
  m_impl->typeConstraints = new ArgumentList;
  ArgumentListIterator ali(*al);
  Argument *a;
  for (;(a=ali.current());++ali)
  {
    m_impl->typeConstraints->append(new Argument(*a));
  }
}

/*! Returns \c TRUE iff this class or a class inheriting from this class
 *  is \e not defined in an external tag file. 
 */
bool ClassDef::hasNonReferenceSuperClass()
{
  bool found=!isReference() && isLinkableInProject() && !isHidden(); 
  if (found) 
  {
    return TRUE; // we're done if this class is not a reference
  }
  if (m_impl->inheritedBy)
  {
    BaseClassListIterator bcli(*m_impl->inheritedBy);
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
  }
  return found;
}

/*! called from MemberDef::writeDeclaration() to (recusively) write the 
 *  definition of an anonymous struct, union or class.
 */
void ClassDef::writeDeclaration(OutputList &ol,MemberDef *md,bool inGroup,
    ClassDef *inheritedFrom,const char *inheritId)
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
    if (cn.right(2)=="-p" /*|| cn.right(2)=="-g"*/)
    {
      cn = cn.left(cn.length()-2);
    }
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
  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->setInGroup(inGroup);
      mg->writePlainDeclarations(ol,this,0,0,0,inheritedFrom,inheritId);
    }
  }

  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Class));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
      writePlainMemberDeclaration(ol,lmd->type,inGroup,inheritedFrom,inheritId);
    }
  }
}

/*! a link to this class is possible within this project */
bool ClassDef::isLinkableInProject() const
{ 
  static bool extractLocal   = Config_getBool("EXTRACT_LOCAL_CLASSES");
  static bool extractStatic  = Config_getBool("EXTRACT_STATIC");
  static bool hideUndoc      = Config_getBool("HIDE_UNDOC_CLASSES");
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->isLinkableInProject();
  }
  else
  {
    return !name().isEmpty() &&                    /* has a name */
      !isArtificial() && !isHidden() &&            /* not hidden */
      name().find('@')==-1 &&                      /* not anonymous */
      protectionLevelVisible(m_impl->prot)      && /* private/internal */
      (!m_impl->isLocal      || extractLocal)   && /* local */
      (hasDocumentation()    || !hideUndoc)     && /* documented */ 
      (!m_impl->isStatic     || extractStatic)  && /* static */
      !isReference();                              /* not an external reference */
  }
}

bool ClassDef::isLinkable() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->isLinkable();
  }
  else
  {
    return isLinkableInProject() || isReference();
  }
}


/*! the class is visible in a class diagram, or class hierarchy */
bool ClassDef::isVisibleInHierarchy() 
{ 
  static bool allExternals     = Config_getBool("ALLEXTERNALS");
  static bool hideUndocClasses = Config_getBool("HIDE_UNDOC_CLASSES");
  static bool extractStatic    = Config_getBool("EXTRACT_STATIC");

  return // show all classes or a subclass is visible
      (allExternals || hasNonReferenceSuperClass()) &&
      // and not an anonymous compound
      name().find('@')==-1 &&
      // not an artificially introduced class
      /*!isArtificial() &&*/  // 1.8.2: allowed these to appear
      // and not privately inherited
      protectionLevelVisible(m_impl->prot) &&
      // documented or shown anyway or documentation is external 
      (hasDocumentation() || 
       !hideUndocClasses || 
       (m_impl->templateMaster && m_impl->templateMaster->hasDocumentation()) || 
       isReference()
      ) &&
      // is not part of an unnamed namespace or shown anyway
      (!m_impl->isStatic || extractStatic);
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
  //printf("isBaseClass(cd=%s) looking for %s\n",name().data(),bcd->name().data());
  if (level>256)
  {
    err("Possible recursive class relation while inside %s and looking for base class %s\n",qPrint(name()),qPrint(bcd->name()));
    return FALSE;
  }
  if (baseClasses())
  {
    // Beware: trying to optimise the iterator away using ->first() & ->next()
    // causes bug 625531
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
  }
  return found;
}

//----------------------------------------------------------------------

bool ClassDef::isSubClass(ClassDef *cd,int level)
{
  bool found=FALSE;
  if (level>256)
  {
    err("Possible recursive class relation while inside %s and looking for derived class %s\n",qPrint(name()),qPrint(cd->name()));
    return FALSE;
  }
  if (subClasses())
  {
    BaseClassListIterator bcli(*subClasses());
    for ( ; bcli.current() && !found ; ++bcli)
    {
      ClassDef *ccd=bcli.current()->classDef;
      if (ccd==cd) 
        found=TRUE;
      else 
        found=ccd->isSubClass(cd,level+1);
    }
  }
  return found;
}

//----------------------------------------------------------------------------

static bool isStandardFunc(MemberDef *md)
{
  return md->name()=="operator=" || // assignment operator
         md->isConstructor() ||     // constructor
         md->isDestructor();        // destructor
}

/*! 
 * recusively merges the `all members' lists of a class base 
 * with that of this class. Must only be called for classes without
 * subclasses!
 */
void ClassDef::mergeMembers()
{
  if (m_impl->membersMerged) return;

  //static bool optimizeOutputForJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  //static bool vhdlOpt = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
  SrcLangExt lang = getLanguage();
  QCString sep=getLanguageSpecificSeparator(lang,TRUE);
  int sepLen = sep.length();

  m_impl->membersMerged=TRUE;
  //printf("  mergeMembers for %s\n",name().data());
  bool inlineInheritedMembers = Config_getBool("INLINE_INHERITED_MEMB" );
  if (baseClasses())
  {
    //printf("  => has base classes!\n");
    BaseClassListIterator bcli(*baseClasses());
    BaseClassDef *bcd;
    for ( ; (bcd=bcli.current()) ; ++bcli )
    {
      ClassDef *bClass=bcd->classDef; 

      // merge the members in the base class of this inheritance branch first
      bClass->mergeMembers();

      MemberNameInfoSDict *srcMnd  = bClass->memberNameInfoSDict();
      MemberNameInfoSDict *dstMnd  = m_impl->allMemberNameInfoSDict;

      if (srcMnd)
      {
        MemberNameInfoSDict::Iterator srcMnili(*srcMnd);
        MemberNameInfo *srcMni;
        for ( ; (srcMni=srcMnili.current()) ; ++srcMnili)
        {
          //printf("    Base member name %s\n",srcMni->memberName());
          MemberNameInfo *dstMni;
          if (dstMnd!=0 && (dstMni=dstMnd->find(srcMni->memberName())))
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
                    LockingPtr<ArgumentList> srcAl = srcMd->argumentList();
                    LockingPtr<ArgumentList> dstAl = dstMd->argumentList();
                    found=matchArguments2(
                        srcMd->getOuterScope(),srcMd->getFileDef(),srcAl.pointer(),
                        dstMd->getOuterScope(),dstMd->getFileDef(),dstAl.pointer(),
                        TRUE
                        );
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

                    QCString scope=dstMi->scopePath.left(dstMi->scopePath.find(sep)+sepLen);
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
                      bClass->name()+sep+srcMi->scopePath == dstMi->scopePath ||
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

                    QCString scope=dstMi->scopePath.left(dstMi->scopePath.find(sep)+sepLen);
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
              if (!found && srcMd->protection()!=Private && !srcMd->isFriend())
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
                newMi->scopePath=bClass->name()+sep+srcMi->scopePath;
                if (ambigue)
                {
                  //printf("$$ New member %s %s add scope %s::\n",
                  //     srcMi->ambiguityResolutionScope.data(),
                  //     srcMd->name().data(),
                  //     bClass->name().data());

                  QCString scope=bClass->name()+sep;
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
                    newMi->ambiguityResolutionScope=bClass->name()+sep;
                  }
                  else
                  {
                    newMi->ambigClass=srcMi->ambigClass;
                    newMi->ambiguityResolutionScope=srcMi->ambigClass->name()+sep;
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
              if (!mi->memberDef->isFriend()) // don't inherit friends
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
                  newMi->scopePath=bClass->name()+sep+mi->scopePath;
                  newMi->ambigClass=mi->ambigClass;
                  newMi->ambiguityResolutionScope=mi->ambiguityResolutionScope.copy();
                  newMni->append(newMi);
                }
              }
            }

            if (dstMnd==0)
            {
              m_impl->allMemberNameInfoSDict = new MemberNameInfoSDict(17);
              m_impl->allMemberNameInfoSDict->setAutoDelete(TRUE);
              dstMnd = m_impl->allMemberNameInfoSDict;
            }
            // add it to the dictionary
            dstMnd->append(newMni->memberName(),newMni);
          }
        }
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
  static bool extractLocalMethods = Config_getBool("EXTRACT_LOCAL_METHODS");
  bool makePrivate = category->isLocal();
  // in case extract local methods is not enabled we don't add the methods
  // of the category in case it is defined in the .m file.
  if (makePrivate && !extractLocalMethods) return;
  bool isExtension = category->isExtension();

  category->setCategoryOf(this);
  if (isExtension)
  {
    category->setArtificial(TRUE);

    // copy base classes/protocols from extension
    if (category->baseClasses())
    {
      BaseClassListIterator bcli(*category->baseClasses());
      BaseClassDef *bcd;
      for ( ; (bcd=bcli.current()) ; ++bcli )
      {
        insertBaseClass(bcd->classDef,bcd->usedName,bcd->prot,bcd->virt,bcd->templSpecifiers);
        // correct bcd->classDef so that they do no longer derive from
        // category, but from this class!
        if (bcd->classDef->subClasses())
        {
          BaseClassListIterator scli(*bcd->classDef->subClasses());
          BaseClassDef *scd;
          for ( ; (scd=scli.current()) ; ++scli )
          {
            if (scd->classDef==category)
            {
              scd->classDef=this;
            }
          }
        }
      }
    }

  }
  // make methods private for categories defined in the .m file
  //printf("%s::mergeCategory makePrivate=%d\n",name().data(),makePrivate);
    
  MemberNameInfoSDict *srcMnd  = category->memberNameInfoSDict();
  MemberNameInfoSDict *dstMnd  = m_impl->allMemberNameInfoSDict;

  if (srcMnd && dstMnd)
  {
    MemberNameInfoSDict::Iterator srcMnili(*srcMnd);
    MemberNameInfo *srcMni;
    for ( ; (srcMni=srcMnili.current()) ; ++srcMnili)
    {
      MemberNameInfo *dstMni=dstMnd->find(srcMni->memberName());
      if (dstMni) // method is already defined in the class
      {
        //printf("Existing member %s\n",srcMni->memberName());
        MemberInfo *dstMi = dstMni->getFirst();
        MemberInfo *srcMi = srcMni->getFirst();
        //if (dstMi)
        //{
        //  Protection prot = dstMi->prot;
        //  if (makePrivate || isExtension) 
        //  {
        //    prot = Private;
        //    removeMemberFromLists(dstMi->memberDef);
        //    internalInsertMember(dstMi->memberDef,prot,FALSE);
        //  }
        //}
        if (srcMi && dstMi)
        {
          combineDeclarationAndDefinition(srcMi->memberDef,dstMi->memberDef);
          dstMi->memberDef->setCategory(category);
          dstMi->memberDef->setCategoryRelation(srcMi->memberDef);
          srcMi->memberDef->setCategoryRelation(dstMi->memberDef);
        }
      }
      else // new method name
      {
        //printf("New member %s\n",srcMni->memberName());
        // create a deep copy of the list
        MemberNameInfo *newMni = 0;
        newMni = new MemberNameInfo(srcMni->memberName()); 

        // copy the member(s) from the category to this class
        MemberNameInfoIterator mnii(*srcMni);
        MemberInfo *mi;
        for (;(mi=mnii.current());++mnii)
        {
          //printf("Adding '%s'\n",mi->memberDef->name().data());
          Protection prot = mi->prot;
          //if (makePrivate) prot = Private;
          MemberDef *newMd = mi->memberDef->deepCopy();
          //printf("Copying member %s\n",mi->memberDef->name().data());
          newMd->moveTo(this);

          MemberInfo *newMi=new MemberInfo(newMd,prot,mi->virt,mi->inherited);
          newMi->scopePath=mi->scopePath;
          newMi->ambigClass=mi->ambigClass;
          newMi->ambiguityResolutionScope=mi->ambiguityResolutionScope;
          newMni->append(newMi);

          // also add the newly created member to the global members list
          if (newMd)
          {
            MemberName *mn;
            QCString name = newMd->name();
            if ((mn=Doxygen::memberNameSDict->find(name)))
            {
              mn->append(newMd);
            }
            else
            {
              mn = new MemberName(newMd->name());
              mn->append(newMd);
              Doxygen::memberNameSDict->append(name,mn);
            }
          }

          newMd->setCategory(category);
          newMd->setCategoryRelation(mi->memberDef);
          mi->memberDef->setCategoryRelation(newMd);
          if (makePrivate || isExtension) 
          {
            newMd->makeImplementationDetail();
          }
          internalInsertMember(newMd,prot,FALSE);
        }

        // add it to the dictionary
        dstMnd->append(newMni->memberName(),newMni);
      }
    }
  }
}

//----------------------------------------------------------------------------

void ClassDef::addUsedClass(ClassDef *cd,const char *accessName,
               Protection prot)
{
  static bool extractPrivate = Config_getBool("EXTRACT_PRIVATE");
  static bool umlLook = Config_getBool("UML_LOOK");
  if (prot==Private && !extractPrivate) return;
  //printf("%s::addUsedClass(%s,%s)\n",name().data(),cd->name().data(),accessName);
  if (m_impl->usesImplClassDict==0) 
  {
    m_impl->usesImplClassDict = new UsesClassDict(17); 
    m_impl->usesImplClassDict->setAutoDelete(TRUE);
  }
  UsesClassDef *ucd=m_impl->usesImplClassDict->find(cd->name());
  if (ucd==0)
  {
     ucd = new UsesClassDef(cd);
     m_impl->usesImplClassDict->insert(cd->name(),ucd);
     //printf("Adding used class %s to class %s via accessor %s\n",
     //    cd->name().data(),name().data(),accessName);
  }
  QCString acc = accessName;
  if (umlLook)
  {
    switch(prot)
    {
      case Public:    acc.prepend("+"); break;
      case Private:   acc.prepend("-"); break;
      case Protected: acc.prepend("#"); break;
      case Package:   acc.prepend("~"); break;
    }
  }
  ucd->addAccessor(acc);
}

void ClassDef::addUsedByClass(ClassDef *cd,const char *accessName,
               Protection prot)
{
  static bool extractPrivate = Config_getBool("EXTRACT_PRIVATE");
  static bool umlLook = Config_getBool("UML_LOOK");
  if (prot==Private && !extractPrivate) return;
  //printf("%s::addUsedByClass(%s,%s)\n",name().data(),cd->name().data(),accessName);
  if (m_impl->usedByImplClassDict==0) 
  {
    m_impl->usedByImplClassDict = new UsesClassDict(17); 
    m_impl->usedByImplClassDict->setAutoDelete(TRUE);
  }
  UsesClassDef *ucd=m_impl->usedByImplClassDict->find(cd->name());
  if (ucd==0)
  {
     ucd = new UsesClassDef(cd);
     m_impl->usedByImplClassDict->insert(cd->name(),ucd);
     //printf("Adding used by class %s to class %s\n",
     //    cd->name().data(),name().data());
  }
  QCString acc = accessName;
  if (umlLook)
  {
    switch(prot)
    {
      case Public:    acc.prepend("+"); break;
      case Private:   acc.prepend("-"); break;
      case Protected: acc.prepend("#"); break;
      case Package:   acc.prepend("~"); break;
    }
  }
  ucd->addAccessor(acc);
}


#if 0
/*! Builds up a dictionary of all classes that are used by the state of this 
 *  class (the "implementation"). 
 *  Must be called before mergeMembers() is called!
 */

void ClassDef::determineImplUsageRelation()
{
  MemberNameInfoSDict::Iterator mnili(*m_impl->allMemberNameInfoSDict);
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
        while (extractClassNameFromType(type,pos,usedClassName,templSpec)!=-1 && !found)
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
                if (m_impl->usesImplClassDict==0) m_impl->usesImplClassDict = new UsesClassDict(257); 
                cd = new ClassDef(cd->getDefFileName(),cd->getDefLine(),
                    usedClassName,ClassDef::Class);
                cd->setIsTemplateBaseClass(count);
                UsesClassDef *ucd = new UsesClassDef(cd);
                m_impl->usesImplClassDict->insert(cd->name(),ucd);
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
            if (cd==0) cd=getResolvedClass(usedClassName,0,&templSpec); // TODO: also try in-between scopes!
            //printf("Search for class %s result=%p\n",usedClassName.data(),cd);
            if (cd) // class exists 
            {
              found=TRUE;
              if (m_impl->usesImplClassDict==0) 
              {
                m_impl->usesImplClassDict = new UsesClassDict(257); 
                m_impl->usesImplClassDict->setAutoDelete(TRUE);
              }
              UsesClassDef *ucd=m_impl->usesImplClassDict->find(cd->name());
              if (ucd==0 || ucd->templSpecifiers!=templSpec)
              {
                ucd = new UsesClassDef(cd);
                m_impl->usesImplClassDict->insert(cd->name(),ucd);
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
  if (m_impl->usesClassDict)
  {
    msg("Class %s uses the following classes:\n",name().data());
    UsesClassDictIterator ucdi(*m_impl->usesClassDict);
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
    if (cd==0) cd=getClass(type.mid(i,l)); // TODO: also try in-between scopes!
    if (cd && cd!=this && !isBaseClass(cd))
    {
      if (m_impl->usesIntfClassDict==0) 
      {
        m_impl->usesIntfClassDict = new UsesClassDict(257); 
      }
      UsesClassDef *ucd=m_impl->usesIntfClassDict->find(cd->name());
      if (ucd==0)
      {
        ucd = new UsesClassDef(cd);
        m_impl->usesIntfClassDict->insert(cd->name(),ucd);
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
  MemberNameInfoSDict::Iterator mnili(*m_impl->allMemberNameInfoList);
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
  if (getLanguage()==SrcLangExt_Fortran)
  {
    switch (m_impl->compType)
    {
      case Class:     return "module";
      case Struct:    return "type";
      case Union:     return "union";
      case Interface: return "interface";
      case Protocol:  return "protocol";
      case Category:  return "category";
      case Exception: return "exception";
      default:        return "unknown";
    } 
  }
  else
  {
    switch (m_impl->compType)
    {
      case Class:     return isJavaEnum() ? "enum" : "class";
      case Struct:    return "struct";
      case Union:     return "union";
      case Interface: return getLanguage()==SrcLangExt_ObjC ? "class" : "interface";
      case Protocol:  return "protocol";
      case Category:  return "category";
      case Exception: return "exception";
      case Service:   return "service";
      case Singleton: return "singleton";
      default:        return "unknown";
    }
  }
}

QCString ClassDef::getOutputFileBase() const 
{ 
  if (!Doxygen::generatingXmlOutput)
  {
    static bool inlineGroupedClasses = Config_getBool("INLINE_GROUPED_CLASSES");
    static bool inlineSimpleClasses = Config_getBool("INLINE_SIMPLE_STRUCTS");
    Definition *scope=0;
    if (inlineGroupedClasses && partOfGroups()!=0)
    {
      // point to the group that embeds this class
      return partOfGroups()->at(0)->getOutputFileBase();
    }
    else if (inlineSimpleClasses && m_impl->isSimple && partOfGroups()!=0)
    {
      // point to simple struct inside a group
      return partOfGroups()->at(0)->getOutputFileBase();
    }
    else if (inlineSimpleClasses && m_impl->isSimple && (scope=getOuterScope()))
    {
      if (scope==Doxygen::globalScope && getFileDef() && getFileDef()->isLinkableInProject()) // simple struct embedded in file
      {
        return getFileDef()->getOutputFileBase();
      }
      else if (scope->isLinkableInProject()) // simple struct embedded in other container (namespace/group/class)
      {
        return getOuterScope()->getOutputFileBase();
      }
    }
  }
  if (m_impl->templateMaster)
  {
    // point to the template of which this class is an instance
    return m_impl->templateMaster->getOutputFileBase();
  }
  else if (isReference())
  {
    // point to the external location
    return m_impl->fileName;
  }
  else
  {
    // normal locally defined class
    return convertNameToFile(m_impl->fileName); 
  }
}

QCString ClassDef::getInstanceOutputFileBase() const 
{ 
  if (isReference())
  {
    return m_impl->fileName;
  }
  else
  {
    return convertNameToFile(m_impl->fileName); 
  }
}

QCString ClassDef::getFileBase() const 
{ 
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->getFileBase();
  }
  else
  {
    return m_impl->fileName; 
  }
}

QCString ClassDef::getSourceFileBase() const 
{ 
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->getSourceFileBase();
  }
  else
  {
    return convertNameToFile(m_impl->fileName)+"_source"; 
  }
}

void ClassDef::setGroupDefForAllMembers(GroupDef *gd,Grouping::GroupPri_t pri,const QCString &fileName,int startLine,bool hasDocs)
{
  gd->addClass(this);
  //printf("ClassDef::setGroupDefForAllMembers(%s)\n",gd->name().data());
  if (m_impl->allMemberNameInfoSDict==0) return;
  MemberNameInfoSDict::Iterator mnili(*m_impl->allMemberNameInfoSDict);
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
  //printf("**** %s::addInnerCompound(%s)\n",name().data(),d->name().data());
  if (d->definitionType()==Definition::TypeClass) // only classes can be
                                                  // nested in classes.
  {
    if (m_impl->innerClasses==0)
    {
      m_impl->innerClasses = new ClassSDict(17);
    }
    m_impl->innerClasses->inSort(d->localName(),(ClassDef *)d);
  }
}

Definition *ClassDef::findInnerCompound(const char *name)
{
  Definition *result=0;
  if (name==0) return 0;
  if (m_impl->innerClasses)
  {
    result = m_impl->innerClasses->find(name);
  }
  return result;
}

//void ClassDef::initTemplateMapping()
//{
//  m_impl->templateMapping->clear();
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
//  if (m_impl->templateMapping && formal)
//  {
//    if (m_impl->templateMapping->find(formal))
//    {
//      m_impl->templateMapping->remove(formal);
//    }
//    m_impl->templateMapping->insert(formal,new QCString(actual));
//  }
//}
//
//QCString ClassDef::getTemplateArgumentMapping(const char *formal) const
//{
//  if (m_impl->templateMapping && formal)
//  {
//    QCString *s = m_impl->templateMapping->find(formal);
//    if (s)
//    {
//      return *s;
//    }
//  }
//  return "";
//}

ClassDef *ClassDef::insertTemplateInstance(const QCString &fileName,
    int startLine, int startColumn, const QCString &templSpec,bool &freshInstance)
{
  freshInstance = FALSE;
  if (m_impl->templateInstances==0) 
  {
    m_impl->templateInstances = new QDict<ClassDef>(17);
  }
  ClassDef *templateClass=m_impl->templateInstances->find(templSpec);
  if (templateClass==0)
  {
    Debug::print(Debug::Classes,0,"      New template instance class `%s'`%s'\n",name().data(),templSpec.data());
    QCString tcname = removeRedundantWhiteSpace(localName()+templSpec);
    templateClass = new ClassDef(
        fileName,startLine,startColumn,tcname,ClassDef::Class);
    templateClass->setTemplateMaster(this);
    templateClass->setOuterScope(getOuterScope());
    templateClass->setHidden(isHidden());
    m_impl->templateInstances->insert(templSpec,templateClass);
    freshInstance=TRUE;
  }
  return templateClass;
}

ClassDef *ClassDef::getVariableInstance(const char *templSpec)
{
  if (m_impl->variableInstances==0) 
  {
    m_impl->variableInstances = new QDict<ClassDef>(17);
    m_impl->variableInstances->setAutoDelete(TRUE);
  }
  ClassDef *templateClass=m_impl->variableInstances->find(templSpec);
  if (templateClass==0)
  {
    Debug::print(Debug::Classes,0,"      New template variable instance class `%s'`%s'\n",qPrint(name()),qPrint(templSpec));
    QCString tcname = removeRedundantWhiteSpace(name()+templSpec);
    templateClass = new ClassDef("<code>",1,1,tcname,
                        ClassDef::Class,0,0,FALSE);
    templateClass->addMembersToTemplateInstance( this, templSpec );
    templateClass->setTemplateMaster(this);
    m_impl->variableInstances->insert(templSpec,templateClass);
  }
  return templateClass;
}

void ClassDef::setTemplateBaseClassNames(QDict<int> *templateNames)
{
  if (templateNames==0) return;
  if (m_impl->templBaseClassNames==0)
  {
    m_impl->templBaseClassNames = new QDict<int>(17);
    m_impl->templBaseClassNames->setAutoDelete(TRUE);
  }
  // make a deep copy of the dictionary.
  QDictIterator<int> qdi(*templateNames);
  for (;qdi.current();++qdi)
  {
    if (m_impl->templBaseClassNames->find(qdi.currentKey())==0)
    {
      m_impl->templBaseClassNames->insert(qdi.currentKey(),new int(*qdi.current()));
    }
  }
}

QDict<int> *ClassDef::getTemplateBaseClassNames() const
{
  return m_impl->templBaseClassNames;
}

void ClassDef::addMembersToTemplateInstance(ClassDef *cd,const char *templSpec)
{
  //printf("%s::addMembersToTemplateInstance(%s,%s)\n",name().data(),cd->name().data(),templSpec);
  if (cd->memberNameInfoSDict()==0) return;
  MemberNameInfoSDict::Iterator mnili(*cd->memberNameInfoSDict());
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
      imd->setMemberGroupId(md->getMemberGroupId());
      insertMember(imd);
      //printf("Adding member=%s %s%s to class %s templSpec %s\n",
      //    imd->typeString(),imd->name().data(),imd->argsString(),
      //    imd->getClassDef()->name().data(),templSpec);
      // insert imd in the list of all members
      //printf("Adding member=%s class=%s\n",imd->name().data(),name().data());
      MemberName *mn = Doxygen::memberNameSDict->find(imd->name());
      if (mn==0)
      {
        mn = new MemberName(imd->name());
        Doxygen::memberNameSDict->append(imd->name(),mn);
      }
      mn->append(imd);
    }
  }
}

QCString ClassDef::getReference() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->getReference();
  }
  else
  {
    return Definition::getReference();
  }
}

bool ClassDef::isReference() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->isReference();
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
  //static bool optimizeOutputJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  static bool hideScopeNames = Config_getBool("HIDE_SCOPE_NAMES");
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
    else if (!hideScopeNames)
    {
      scName = d->qualifiedName();
    }
  }

  SrcLangExt lang = getLanguage();
  QCString scopeSeparator = getLanguageSpecificSeparator(lang);
  if (!scName.isEmpty()) scName+=scopeSeparator;

  bool isSpecialization = localName().find('<')!=-1;

  QCString clName = className();
  //bool isGeneric = getLanguage()==SrcLangExt_CSharp;
  //if (isGeneric && clName.right(2)=="-g") 
  //{
  //  clName = clName.left(clName.length()-2);
  //}
  //printf("m_impl->lang=%d clName=%s\n",m_impl->lang,clName.data());
  scName+=clName;
  ArgumentList *al=0;
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
  if (m_impl->className.isEmpty())
  {
    return localName();
  }
  else
  {
    return m_impl->className;
  }
};

void ClassDef::setClassName(const char *name)
{
  m_impl->className = name;
}

void ClassDef::addListReferences()
{
  SrcLangExt lang = getLanguage();
  if (!isLinkableInProject()) return;
  //printf("ClassDef(%s)::addListReferences()\n",name().data());
  {
    LockingPtr< QList<ListItemInfo> > xrefItems = xrefListItems();
    addRefItem(xrefItems.pointer(),
             qualifiedName(),
             lang==SrcLangExt_Fortran ? theTranslator->trType(TRUE,TRUE)
                                      : theTranslator->trClass(TRUE,TRUE),
             getOutputFileBase(),
             displayName(),
             0
            );
  }
  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->addListReferences(this);
    }
  }
  QListIterator<MemberList> mli(m_impl->memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()&MemberListType_detailedLists)
    {
      ml->addListReferences(this);
    }
  }
}

MemberDef *ClassDef::getMemberByName(const QCString &name) const
{
  MemberDef *xmd = 0;
  if (m_impl->allMemberNameInfoSDict)
  {
    MemberNameInfo *mni = m_impl->allMemberNameInfoSDict->find(name);
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
  }
  //printf("getMemberByName(%s)=%p\n",name.data(),xmd);
  return xmd;
}

bool ClassDef::isAccessibleMember(MemberDef *md)
{
  return md->getClassDef() && isBaseClass(md->getClassDef(),TRUE);
}

MemberList *ClassDef::createMemberList(MemberListType lt)
{
  m_impl->memberLists.setAutoDelete(TRUE);
  QListIterator<MemberList> mli(m_impl->memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if (ml->listType()==lt)
    {
      return ml;
    }
  }
  // not found, create a new member list
  ml = new MemberList(lt);
  m_impl->memberLists.append(ml);
  return ml;
}

MemberList *ClassDef::getMemberList(MemberListType lt)
{
  MemberList *ml = m_impl->memberLists.first();
  while (ml)
  {
    if (ml->listType()==lt)
    {
      return ml;
    }
    ml = m_impl->memberLists.next();
  }
  return 0;
}

void ClassDef::addMemberToList(MemberListType lt,MemberDef *md,bool isBrief)
{
  static bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  static bool sortMemberDocs = Config_getBool("SORT_MEMBER_DOCS");
  MemberList *ml = createMemberList(lt);
  ml->setNeedsSorting((isBrief && sortBriefDocs) || (!isBrief && sortMemberDocs));
  ml->append(md);

  // for members in the declaration lists we set the section, needed for member grouping
  if ((ml->listType()&MemberListType_detailedLists)==0) md->setSectionList(this,ml);
}

void ClassDef::sortMemberLists()
{
  MemberList *ml = m_impl->memberLists.first();
  while (ml)
  {
    if (ml->needsSorting()) { ml->sort(); ml->setNeedsSorting(FALSE); }
    ml = m_impl->memberLists.next();
  }
}

static void convertProtectionLevel(
                   MemberListType inListType,
                   Protection inProt,
                   int *outListType1,
                   int *outListType2
                  )
{
  // default representing Public inheritance
  *outListType1=inListType;
  *outListType2=-1;
  if (inProt==Public)
  {
    switch (inListType) // in the private section of the derived class,
                        // the private section of the base class should not
                        // be visible
    {
      case MemberListType_priMethods:       
      case MemberListType_priStaticMethods: 
      case MemberListType_priSlots:         
      case MemberListType_priAttribs:
      case MemberListType_priStaticAttribs:
      case MemberListType_priTypes:
        *outListType1=-1;
        *outListType2=-1;
        break;
      default:
        break;
    }
  }
  else if (inProt==Protected) // Protected inheritance
  {
    switch (inListType) // in the protected section of the derived class,
                        // both the public and protected members are shown
                        // as protected
    {
      case MemberListType_pubMethods:       
      case MemberListType_pubStaticMethods: 
      case MemberListType_pubSlots:         
      case MemberListType_pubAttribs:       
      case MemberListType_pubStaticAttribs: 
      case MemberListType_pubTypes:         
      case MemberListType_priMethods:       
      case MemberListType_priStaticMethods: 
      case MemberListType_priSlots:         
      case MemberListType_priAttribs:
      case MemberListType_priStaticAttribs:
      case MemberListType_priTypes:
        *outListType1=-1;
        *outListType2=-1;
        break;

      case MemberListType_proMethods:       
        *outListType1=MemberListType_pubMethods;
        *outListType2=MemberListType_proMethods;
        break;
      case MemberListType_proStaticMethods: 
        *outListType1=MemberListType_pubStaticMethods;
        *outListType2=MemberListType_proStaticMethods;
        break;
      case MemberListType_proSlots:         
        *outListType1=MemberListType_pubSlots;
        *outListType1=MemberListType_proSlots;
        break;
      case MemberListType_proAttribs:       
        *outListType1=MemberListType_pubAttribs;
        *outListType2=MemberListType_proAttribs;
        break;
      case MemberListType_proStaticAttribs: 
        *outListType1=MemberListType_pubStaticAttribs;
        *outListType2=MemberListType_proStaticAttribs;
        break;
      case MemberListType_proTypes:         
        *outListType1=MemberListType_pubTypes;
        *outListType2=MemberListType_proTypes;
        break;
      default: 
        break;
    }
  }
  else if (inProt==Private)
  {
    switch (inListType) // in the private section of the derived class,
                        // both the public and protected members are shown
                        // as private
    {
      case MemberListType_pubMethods:       
      case MemberListType_pubStaticMethods: 
      case MemberListType_pubSlots:         
      case MemberListType_pubAttribs:       
      case MemberListType_pubStaticAttribs: 
      case MemberListType_pubTypes:         
      case MemberListType_proMethods:       
      case MemberListType_proStaticMethods: 
      case MemberListType_proSlots:         
      case MemberListType_proAttribs:
      case MemberListType_proStaticAttribs:
      case MemberListType_proTypes:
        *outListType1=-1;
        *outListType2=-1;
        break;

      case MemberListType_priMethods:       
        *outListType1=MemberListType_pubMethods;
        *outListType2=MemberListType_proMethods;
        break;
      case MemberListType_priStaticMethods: 
        *outListType1=MemberListType_pubStaticMethods;
        *outListType2=MemberListType_proStaticMethods;
        break;
      case MemberListType_priSlots:         
        *outListType1=MemberListType_pubSlots;
        *outListType1=MemberListType_proSlots;
        break;
      case MemberListType_priAttribs:
        *outListType1=MemberListType_pubAttribs;
        *outListType2=MemberListType_proAttribs;
        break;
      case MemberListType_priStaticAttribs:
        *outListType1=MemberListType_pubStaticAttribs;
        *outListType2=MemberListType_proStaticAttribs;
        break;
      case MemberListType_priTypes:
        *outListType1=MemberListType_pubTypes;
        *outListType2=MemberListType_proTypes;
        break;
      default: 
        break;
    }
  }
  //printf("convertProtectionLevel(type=%d prot=%d): %d,%d\n",
  //    inListType,inProt,*outListType1,*outListType2);
}

int ClassDef::countInheritedDecMembersRec(MemberListType lt,
                                          ClassDef *inheritedFrom)
{
  //printf("> %s::countedInheritedDecMembersRec(%d)\n",name().data(),lt);
  int count=0;
  if (m_impl->inherits)
  {
    BaseClassListIterator it(*m_impl->inherits);
    BaseClassDef *ibcd;
    for (it.toFirst();(ibcd=it.current());++it)
    {
      ClassDef *icd=ibcd->classDef;
      int lt1,lt2;
      // an inherited member with protection level lt 
      // could have come from a section with protection levels lt1 or lt2
      // in the bass class (e.g. for protected inheritance, the protected
      // member comes from protected and public methods in the base class)
      convertProtectionLevel(lt,ibcd->prot,&lt1,&lt2);
      MemberList *ml1 = icd->getMemberList((MemberListType)lt1);
      MemberList *ml2 = icd->getMemberList((MemberListType)lt2);
      if (ml1)
      {
        count+=icd->countMembersIncludingGrouped((MemberListType)lt1,inheritedFrom,TRUE);
      }
      if (ml2) 
      {
        count+=icd->countMembersIncludingGrouped((MemberListType)lt2,inheritedFrom,TRUE);
      }
      if (lt1!=-1)
      {
        count+=icd->countInheritedDecMembersRec((MemberListType)lt1,inheritedFrom);
      }
      if (lt2!=-1)
      {
        count+=icd->countInheritedDecMembersRec((MemberListType)lt2,inheritedFrom);
      }
    }
  }
  //printf("< %s::countedInheritedDecMembersRec(%d) count=%d\n",name().data(),lt,count);
  return count;
}

int ClassDef::countInheritedDecMembers(MemberListType lt)
{
  int count=0;
  MemberList *ml = getMemberList(lt);
  if (ml)
  {
    count = ml->countInheritableMembers(this);
  }
  if (count==0) // for this class the (non-private) member list is empty
                // see if we need to create a section for it under
                // Additional Inherited Members
  {
    count = countInheritedDecMembersRec(lt,this);
  }
  else // member list is not empty, so we will add the inherited members there
  {
    count = 0;
  }
  return count;
}

int ClassDef::countAdditionalInheritedMembers()
{
  int totalCount=0;
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Class));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
      if (lmd->type!=MemberListType_friends) // friendship is not inherited
      {
        totalCount+=countInheritedDecMembers(lmd->type);
      }
    }
  }
  //printf("countAdditonalInheritedMembers()=%d\n",totalCount);
  return totalCount;
}

void ClassDef::writeAdditionalInheritedMembers(OutputList &ol)
{
  //printf("**** writeAdditionalInheritedMembers()\n");
  QListIterator<LayoutDocEntry> eli(
      LayoutDocManager::instance().docEntries(LayoutDocManager::Class));
  LayoutDocEntry *lde;
  for (eli.toFirst();(lde=eli.current());++eli)
  {
    if (lde->kind()==LayoutDocEntry::MemberDecl)
    {
      LayoutDocEntryMemberDecl *lmd = (LayoutDocEntryMemberDecl*)lde;
      MemberList *ml = getMemberList(lmd->type);
      if (ml==0 || ml->numDecMembers()==0)
      {
        QPtrDict<void> visited(17);
        writeInheritedMemberDeclarations(ol,lmd->type,lmd->title(getLanguage()),this,TRUE,&visited);
      }
    }
  }
}

int ClassDef::countMembersIncludingGrouped(MemberListType lt,
              ClassDef *inheritedFrom,bool additional)
{
  int count=0;
  MemberList *ml = getMemberList(lt);
  if (ml) 
  { 
    count=ml->countInheritableMembers(inheritedFrom);
  }
  //printf("%s:countMembersIncludingGrouped: count=%d\n",name().data(),count);
  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      bool hasOwnSection = !mg->allMembersInSameSection() || 
                           !m_impl->subGrouping; // group is in its own section
      if ((additional && hasOwnSection) || (!additional && !hasOwnSection))
      {
        count+=mg->countGroupedInheritedMembers(lt);
      }
    }
  }
  //printf("%s:countMembersIncludingGrouped(lt=%d,%s)=%d\n",
  //    name().data(),lt,ml?ml->listTypeAsString().data():"<none>",count);
  return count;
}

void ClassDef::writeInheritedMemberDeclarations(OutputList &ol,
               MemberListType lt,const QCString &title,
               ClassDef *inheritedFrom,bool invert,
               QPtrDict<void> *visitedClasses)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  bool process = countMembersIncludingGrouped(lt,inheritedFrom,FALSE)>0;
  //printf("%s: writeInheritedMemberDec: lt=%d process=%d invert=%d\n",
  //    name().data(),lt,process,invert);
  if (process^invert)
  {
    if (m_impl->inherits)
    {
      BaseClassListIterator it(*m_impl->inherits);
      BaseClassDef *ibcd;
      for (it.toFirst();(ibcd=it.current());++it)
      {
        ClassDef *icd=ibcd->classDef;
        int lt1,lt2;
        convertProtectionLevel(lt,ibcd->prot,&lt1,&lt2);
        //printf("%s:convert %d->(%d,%d)\n",icd->name().data(),lt,lt1,lt2);
        if (visitedClasses->find(icd)==0) 
        {
          visitedClasses->insert(icd,icd); // guard for multiple virtual inheritance
          if (lt1!=-1)
          {
            icd->writeMemberDeclarations(ol,(MemberListType)lt1,
                title,QCString(),FALSE,inheritedFrom,lt2,invert,visitedClasses);
          }
        }
      }
    }
  }
  ol.popGeneratorState();
}

void ClassDef::writeMemberDeclarations(OutputList &ol,MemberListType lt,const QCString &title,
               const char *subTitle,bool showInline,ClassDef *inheritedFrom,int lt2,bool invert,QPtrDict<void> *visitedClasses)
{
  //printf("%s: ClassDef::writeMemberDeclarations for %s\n",name().data(),ml->listTypeAsString().data());
  MemberList * ml = getMemberList(lt);
  if (getLanguage()==SrcLangExt_VHDL) // use specific declarations function
  {
    if (ml)
    {
      VhdlDocGen::writeVhdlDeclarations(ml,ol,0,this,0,0);
    }
  }
  else
  {
    //printf("%s::writeMemberDeclarations(%s)\n",name().data(),title.data());
    //static bool optimizeVhdl = Config_getBool("OPTIMIZE_OUTPUT_VHDL");
    if (ml) 
    {
      ml->writeDeclarations(ol,this,0,0,0,title,subTitle,definitionType(),FALSE,showInline,inheritedFrom); 
      if (lt2!=-1)
      {
        MemberList * ml2 = getMemberList((MemberListType)lt2);
        if (ml2)
        {
          ml2->writeDeclarations(ol,this,0,0,0,0,0,definitionType(),FALSE,showInline,inheritedFrom); 
        }
      }
    }
    static bool inlineInheritedMembers = Config_getBool("INLINE_INHERITED_MEMB");
    if (!inlineInheritedMembers) // show inherited members as separate lists
    {
      QPtrDict<void> visited(17);
      writeInheritedMemberDeclarations(ol,lt,title,
                                       inheritedFrom ? inheritedFrom : this,
                                       invert,visitedClasses==0 ? &visited: visitedClasses);
    }
  }
}

void ClassDef::addGroupedInheritedMembers(OutputList &ol,MemberListType lt,
                        ClassDef *inheritedFrom,const QCString &inheritId)
{
  //printf("** %s::addGroupedInheritedMembers(%p) inheritId=%s\n",name().data(),m_impl->memberGroupSDict,inheritId.data());
  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      if (!mg->allMembersInSameSection() || !m_impl->subGrouping) // group is in its own section
      {
        mg->addGroupedInheritedMembers(ol,this,lt,inheritedFrom,inheritId);
      }
    }
  }
}

void ClassDef::writeMemberDocumentation(OutputList &ol,MemberListType lt,const QCString &title,bool showInline)
{
  //printf("%s: ClassDef::writeMemberDocumentation()\n",name().data());
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,displayName(),this,title,FALSE,showInline);
}

void ClassDef::writeSimpleMemberDocumentation(OutputList &ol,MemberListType lt)
{
  //printf("%s: ClassDef::writeSimpleMemberDocumentation()\n",name().data());
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeSimpleDocumentation(ol,this);
}

void ClassDef::writePlainMemberDeclaration(OutputList &ol,
         MemberListType lt,bool inGroup,
         ClassDef *inheritedFrom,const char *inheritId)
{
  //printf("%s: ClassDef::writePlainMemberDeclaration()\n",name().data());
  MemberList * ml = getMemberList(lt);
  if (ml) 
  {
    ml->setInGroup(inGroup);
    ml->writePlainDeclarations(ol,this,0,0,0,definitionType(),inheritedFrom,inheritId); 
  }
}

bool ClassDef::isLocal() const 
{ 
  return m_impl->isLocal; 
}

ClassSDict *ClassDef::getClassSDict() 
{ 
  return m_impl->innerClasses; 
}

ClassDef::CompoundType ClassDef::compoundType() const 
{ 
  return m_impl->compType; 
} 

BaseClassList *ClassDef::baseClasses() const
{ 
  return m_impl->inherits; 
}

BaseClassList *ClassDef::subClasses() const
{ 
  return m_impl->inheritedBy; 
}

MemberNameInfoSDict *ClassDef::memberNameInfoSDict() const
{ 
  return m_impl->allMemberNameInfoSDict; 
}

Protection ClassDef::protection() const 
{ 
  return m_impl->prot; 
}

ArgumentList *ClassDef::templateArguments() const 
{ 
  return m_impl->tempArgs; 
}

NamespaceDef *ClassDef::getNamespaceDef() const
{ 
  return m_impl->nspace; 
}

FileDef *ClassDef::getFileDef() const 
{ 
  return m_impl->fileDef; 
}

QDict<ClassDef> *ClassDef::getTemplateInstances() const 
{ 
  return m_impl->templateInstances; 
}

ClassDef *ClassDef::templateMaster() const 
{ 
  return m_impl->templateMaster; 
} 

bool ClassDef::isTemplate() const 
{ 
  return m_impl->tempArgs!=0; 
}

IncludeInfo *ClassDef::includeInfo() const 
{ 
  return m_impl->incInfo; 
}

UsesClassDict *ClassDef::usedImplementationClasses() const 
{ 
  return m_impl->usesImplClassDict; 
}

UsesClassDict *ClassDef::usedByImplementationClasses() const 
{ 
  return m_impl->usedByImplClassDict; 
}

UsesClassDict *ClassDef::usedInterfaceClasses() const
{
  return m_impl->usesIntfClassDict;
}

bool ClassDef::isTemplateArgument() const
{
  return m_impl->isTemplArg;
}

bool ClassDef::isAbstract() const 
{ 
  return m_impl->isAbstract || (m_impl->spec&Entry::Abstract); 
}

bool ClassDef::isFinal() const
{
  return m_impl->spec&Entry::Final;
}

bool ClassDef::isSealed() const
{
  return m_impl->spec&Entry::Sealed;
}

bool ClassDef::isPublished() const
{
  return m_impl->spec&Entry::Published;
}

bool ClassDef::isObjectiveC() const 
{ 
  return getLanguage()==SrcLangExt_ObjC; 
}

bool ClassDef::isCSharp() const 
{ 
  return getLanguage()==SrcLangExt_CSharp; 
}

ClassDef *ClassDef::categoryOf() const 
{ 
  return m_impl->categoryOf; 
}

const QList<MemberList> &ClassDef::getMemberLists() const 
{ 
  return m_impl->memberLists; 
}

MemberGroupSDict *ClassDef::getMemberGroupSDict() const 
{ 
  return m_impl->memberGroupSDict; 
}

void ClassDef::setNamespace(NamespaceDef *nd) 
{ 
  m_impl->nspace = nd; 
}

void ClassDef::setFileDef(FileDef *fd) 
{ 
  m_impl->fileDef=fd; 
}

void ClassDef::setSubGrouping(bool enabled) 
{ 
  m_impl->subGrouping = enabled; 
}

void ClassDef::setProtection(Protection p) 
{ 
  m_impl->prot=p; 
}

void ClassDef::setIsStatic(bool b) 
{ 
  m_impl->isStatic=b; 
}

void ClassDef::setCompoundType(CompoundType t) 
{ 
  m_impl->compType = t; 
} 

void ClassDef::setTemplateMaster(ClassDef *tm) 
{ 
  m_impl->templateMaster=tm; 
}

void ClassDef::makeTemplateArgument(bool b) 
{ 
  m_impl->isTemplArg = b; 
}

void ClassDef::setCategoryOf(ClassDef *cd)
{
  m_impl->categoryOf = cd;
}

void ClassDef::setUsedOnly(bool b)
{
  m_impl->usedOnly = b;
}

bool ClassDef::isUsedOnly() const
{
  return m_impl->usedOnly;
}

bool ClassDef::isSimple() const
{
  return m_impl->isSimple;
}

MemberDef *ClassDef::isSmartPointer() const
{
  return m_impl->arrowOperator;
}

void ClassDef::reclassifyMember(MemberDef *md,MemberType t)
{
  md->setMemberType(t);
  MemberList *ml = m_impl->memberLists.first();
  while (ml)
  {
    ml->remove(md);
    ml = m_impl->memberLists.next();
  }
  insertMember(md);
}

QCString ClassDef::anchor() const
{
  QCString anc;
  if (isEmbeddedInOuterScope() && !Doxygen::generatingXmlOutput)
  {
    if (m_impl->templateMaster)
    {
      // point to the template of which this class is an instance
      anc = m_impl->templateMaster->getOutputFileBase();
    }
    else if (isReference())
    {
      // point to the external location
      anc = m_impl->fileName;
    }
    else
    {
      // normal locally defined class
      anc = convertNameToFile(m_impl->fileName); 
    }
  }
  return anc;
}

bool ClassDef::isEmbeddedInOuterScope() const
{
  static bool inlineGroupedClasses = Config_getBool("INLINE_GROUPED_CLASSES");
  static bool inlineSimpleClasses = Config_getBool("INLINE_SIMPLE_STRUCTS");

  Definition *container = getOuterScope();

  bool containerLinkable = 
    container && 
       (
        (container==Doxygen::globalScope && getFileDef() && getFileDef()->isLinkableInProject()) || // global class in documented file
        container->isLinkableInProject() // class in documented scope
       ); 

  // inline because of INLINE_GROUPED_CLASSES=YES ?
  bool b1 = (inlineGroupedClasses && partOfGroups()!=0); // a grouped class
  // inline because of INLINE_SIMPLE_STRUCTS=YES ?
  bool b2 = (inlineSimpleClasses && m_impl->isSimple && // a simple class
             (containerLinkable || // in a documented container
              partOfGroups()!=0    // or part of a group
             )
           );
  //printf("%s::isEmbeddedInOuterScope(): inlineGroupedClasses=%d "
  //       "inlineSimpleClasses=%d partOfGroups()=%p m_impl->isSimple=%d "
  //       "getOuterScope()=%s b1=%d b2=%d\n",
  //    name().data(),inlineGroupedClasses,inlineSimpleClasses,
  //    partOfGroups().pointer(),m_impl->isSimple,getOuterScope()?getOuterScope()->name().data():"<none>",b1,b2);
  return b1 || b2;  // either reason will do
}

const ClassList *ClassDef::taggedInnerClasses() const
{
  return m_impl->taggedInnerClasses;
}

void ClassDef::addTaggedInnerClass(ClassDef *cd)
{
  if (m_impl->taggedInnerClasses==0)
  {
    m_impl->taggedInnerClasses = new ClassList;
  }
  m_impl->taggedInnerClasses->append(cd);
}

ClassDef *ClassDef::tagLessReference() const
{
  return m_impl->tagLessRef;
}

void ClassDef::setTagLessReference(ClassDef *cd)
{
  m_impl->tagLessRef = cd;
}

void ClassDef::removeMemberFromLists(MemberDef *md)
{
  MemberList *ml = m_impl->memberLists.first();
  while (ml)
  {
    ml->remove(md);
    ml = m_impl->memberLists.next();
  }
}

bool ClassDef::isJavaEnum() const
{
  return m_impl->isJavaEnum;
}

bool ClassDef::isGeneric() const
{
  return m_impl->isGeneric;
}

void ClassDef::setClassSpecifier(uint64 spec)
{
  m_impl->spec = spec;
}

bool ClassDef::isExtension() const
{
  QCString n = name();
  int si = n.find('(');
  int ei = n.find(')');
  bool b = ei>si && n.mid(si+1,ei-si-1).stripWhiteSpace().isEmpty(); 
  return b;
}


