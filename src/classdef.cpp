/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
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

//static inline MemberList *createNewMemberList(MemberList::ListType lt)
//{
//  MemberList *result = new MemberList(lt);
//  return result;
//}

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

    /*! class name with outer class scope, but without namespace scope. */
    QCString className;

    /*! If this class is a Objective-C category, then this points to the
     *  class which is extended.
     */
    ClassDef *categoryOf;

    QList<MemberList> memberLists;

    /* user defined member groups */
    MemberGroupSDict *memberGroupSDict;

    /*! Indicated whether this class exists because it is used by
     *  some other class only (TRUE) or if some class inherits from
     *  it (FALSE). This is need to remove used-only classes from
     *  the inheritance tree.
     */
    bool artificial;

    /*! Is this an abstact class? */
    bool isAbstract;

    /*! Is the class part of an unnamed namespace? */
    bool isStatic;

    /*! Is the class part implemented in Objective C? */
    bool isObjC;

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

    bool usedOnly;
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
  artificial = FALSE;
  isAbstract = FALSE;
  isStatic = FALSE;
  isTemplArg = FALSE;
  membersMerged = FALSE;
  categoryOf = 0;
  usedOnly = FALSE;
  QCString ns;
  extractNamespaceName(name,className,ns);
  //printf("m_name=%s m_className=%s ns=%s\n",m_name.data(),m_className.data(),ns.data());

  if (((QCString)defFileName).right(5)!=".java" && 
      guessSection(defFileName)==Entry::SOURCE_SEC)
  {
    isLocal=TRUE;
  }
  else
  {
    isLocal=FALSE;
  }
}

ClassDefImpl::ClassDefImpl()
{
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
}

// constructs a new class definition
ClassDef::ClassDef(
    const char *defFileName,int defLine,
    const char *nm,CompoundType ct,
    const char *lref,const char *fName,
    bool isSymbol) 
 : Definition(defFileName,defLine,removeRedundantWhiteSpace(nm),0,0,isSymbol) 
{
  visited=FALSE;
  setReference(lref);
  m_impl = new ClassDefImpl;
  m_impl->compType = ct;
  m_impl->isObjC   = FALSE;
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

QCString ClassDef::displayName() const
{
  static bool hideScopeNames = Config_getBool("HIDE_SCOPE_NAMES");
  static bool optimizeOutputForJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  QCString n;
  if (hideScopeNames)
  {
    n=m_impl->className;
  }
  else
  {
    n=qualifiedNameWithTemplateParameters();
  }
  if (optimizeOutputForJava)
  {
    n=substitute(n,"::",".");
  }
  if (m_impl->compType==ClassDef::Protocol && n.right(2)=="-p")
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
  if (m_impl->inherits==0)
  {
    m_impl->inherits = new BaseClassList;
    m_impl->inherits->setAutoDelete(TRUE);
  }
  m_impl->inherits->append(new BaseClassDef(cd,n,p,s,t));
}

// inserts a sub class in the inherited list
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
}

void ClassDef::addMembersToMemberGroup()
{
  QListIterator<MemberList> mli(m_impl->memberLists);
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberList::detailedLists)==0)
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
  //printf("adding %s::%s\n",name().data(),md->name().data());
  if (!isReference())
  {
    static bool extractPrivate = Config_getBool("EXTRACT_PRIVATE");

    /********************************************/
    /* insert member in the declaration section */
    /********************************************/
    if (md->isRelated() && (extractPrivate || prot!=Private))
    {
      addMemberToList(MemberList::related,md);
    }
    else if (md->isFriend())
    {
      addMemberToList(MemberList::friends,md);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberDef::Signal: // Qt specific
          addMemberToList(MemberList::signals,md);
          break;
        case MemberDef::DCOP:   // KDE2 specific
          addMemberToList(MemberList::dcopMethods,md);
          break;
        case MemberDef::Property:
          addMemberToList(MemberList::properties,md);
          break;
        case MemberDef::Event:
          addMemberToList(MemberList::events,md);
          break;
        case MemberDef::Slot:   // Qt specific
          switch (prot)
          {
            case Protected: 
            case Package: // slots in packages are not possible!
              addMemberToList(MemberList::proSlots,md);
              break;
            case Public:    
              addMemberToList(MemberList::pubSlots,md);
              break;
            case Private:   
              addMemberToList(MemberList::priSlots,md);
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
                  addMemberToList(MemberList::proStaticAttribs,md);
                  break;
                case Package: 
                  addMemberToList(MemberList::pacStaticAttribs,md);
                  break;
                case Public:    
                  addMemberToList(MemberList::pubStaticAttribs,md);
                  break;
                case Private:   
                  addMemberToList(MemberList::priStaticAttribs,md);
                  break;
              }
            }
            else // function
            {
              switch (prot)
              {
                case Protected: 
                  addMemberToList(MemberList::proStaticMethods,md);
                  break;
                case Package: 
                  addMemberToList(MemberList::pacStaticMethods,md);
                  break;
                case Public:    
                  addMemberToList(MemberList::pubStaticMethods,md);
                  break;
                case Private:   
                  addMemberToList(MemberList::priStaticMethods,md);
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
                  addMemberToList(MemberList::proAttribs,md);
                  break;
                case Package:
                  addMemberToList(MemberList::pacAttribs,md);
                  break;
                case Public:    
                  addMemberToList(MemberList::pubAttribs,md);
                  break;
                case Private:   
                  addMemberToList(MemberList::priAttribs,md);
                  break;
              }
            }
            else if (md->isTypedef() || md->isEnumerate() || md->isEnumValue())
            {
              switch (prot)
              {
                case Protected: 
                  addMemberToList(MemberList::proTypes,md);
                  break;
                case Package: 
                  addMemberToList(MemberList::pacTypes,md);
                  break;
                case Public:    
                  addMemberToList(MemberList::pubTypes,md);
                  break;
                case Private:   
                  addMemberToList(MemberList::priTypes,md);
                  break;
              }
            }
            else // member function
            {
              switch (prot)
              {
                case Protected: 
                  addMemberToList(MemberList::proMethods,md);
                  break;
                case Package: 
                  addMemberToList(MemberList::pacMethods,md);
                  break;
                case Public:    
                  addMemberToList(MemberList::pubMethods,md);
                  break;
                case Private:   
                  addMemberToList(MemberList::priMethods,md);
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
    if ((md->isRelated() && (extractPrivate || prot!=Private)) || md->isFriend())
    {
      addMemberToList(MemberList::relatedMembers,md);
    }
    else
    {
      switch (md->memberType())
      {
        case MemberDef::Property:
          addMemberToList(MemberList::propertyMembers,md);
          break;
        case MemberDef::Event:
          addMemberToList(MemberList::eventMembers,md);
          break;
        case MemberDef::Signal: // fall through
        case MemberDef::DCOP:
          addMemberToList(MemberList::functionMembers,md);
          break;
        case MemberDef::Slot:
          switch (prot)
          {
            case Protected: 
            case Package: 
            case Public:    
              addMemberToList(MemberList::functionMembers,md);
              break;
            case Private:   
              if (extractPrivate)
              {
                addMemberToList(MemberList::functionMembers,md);
              }
              break;
          }
          break;
        default: // any of the other members
          if (prot!=Private || extractPrivate)
          {
            switch (md->memberType())
            {
              case MemberDef::Typedef:
                addMemberToList(MemberList::typedefMembers,md);
                break;
              case MemberDef::Enumeration:
                addMemberToList(MemberList::enumMembers,md);
                break;
              case MemberDef::EnumValue:
                addMemberToList(MemberList::enumValMembers,md);
                break;
              case MemberDef::Function:
                if (md->isConstructor() || md->isDestructor())
                {
                  MemberList *ml = createMemberList(MemberList::constructors);
                  ml->append(md);
                  md->setSectionList(this,ml);
                }
                else
                {
                  addMemberToList(MemberList::functionMembers,md);
                }
                break;
              case MemberDef::Variable:
                addMemberToList(MemberList::variableMembers,md);
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

  ::addClassMemberNameToIndex(md);
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
  ClassDef *context = Config_getBool("INLINE_INHERITED_MEMB") ? this : 0;
  const char *letters = "abcdefghijklmnopqrstuvwxyz0123456789";
  QListIterator<MemberList> mli(m_impl->memberLists);
  MemberList *ml;
  int index = 0;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberList::detailedLists)==0)
    {
      setAnchors(context,letters[index++],ml);
    }
  }

  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->setAnchors(context);
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
    if ((ml->listType()&MemberList::detailedLists)==0)
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
  if (force && includeName) m_impl->incInfo->includeName = includeName;
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
      (Config_getBool("SOURCE_BROWSER") && getStartBodyLine()!=-1 && getBodyDef()) ||
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
    if (exampleFlag && m_impl->exampleSDict)
    {
      ol.startSimpleSect(BaseOutputDocInterface::Examples,0,0,theTranslator->trExamples()+": ");
      ol.writeDescItem();
      ol.newParagraph();
      writeExample(ol,m_impl->exampleSDict);
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
    
void ClassDef::showUsedFiles(OutputList &ol)
{
  if (Config_getBool("SHOW_USED_FILES"))
  {
    ol.writeRuler();
    ol.parseText(theTranslator->trGeneratedFromFiles(
          m_impl->isObjC && m_impl->compType==Interface ? Class : m_impl->compType,
          m_impl->files.count()==1));

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

        ol.writeListItem();
        QCString path=fd->getPath().copy();
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

        
      }
      file=m_impl->files.next();
    }
    if (!first) ol.endItemList();
  }
}


// write all documentation for this class
void ClassDef::writeDocumentation(OutputList &ol)
{
  QCString pageType = " ";
  pageType += compoundTypeString();
  toupper(pageType.at(1));
  QCString pageTitle = theTranslator->trCompoundReference(displayName(),
              m_impl->compType == Interface && m_impl->isObjC ? Class : m_impl->compType,
              m_impl->tempArgs != 0);
  
  startFile(ol,getOutputFileBase(),name(),pageTitle,HLI_ClassVisible);  
  if (getOuterScope()!=Doxygen::globalScope)
  {
    writeNavigationPath(ol);
  }
  startTitle(ol,getOutputFileBase());
  ol.parseText(pageTitle);
  addGroupListToTitle(ol,this);
  endTitle(ol,getOutputFileBase(),name());

  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeString("<!-- doxytag: class=\"");
    ol.docify(name());
    ol.writeString("\" -->");
    if (m_impl->inherits && m_impl->inherits->count()>0)
    {
      BaseClassListIterator bli(*m_impl->inherits);
      ol.writeString("<!-- doxytag: inherits=\"");
      BaseClassDef *bcd=0;
      bool first=TRUE;
      for (bli.toFirst();(bcd=bli.current());++bli)
      {
        if (!first) ol.writeString(",");
        ol.docify(bcd->classDef->name());
        first=FALSE;
      }
      ol.writeString("\" -->");
    }
    ol.popGeneratorState();
  }


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
  
  if (m_impl->incInfo && Config_getBool("SHOW_INCLUDE_FILES"))
  {
    QCString nm=m_impl->incInfo->includeName.isEmpty() ? 
      (m_impl->incInfo->fileDef ?
       m_impl->incInfo->fileDef->docName().data() : "" 
      ) :
      m_impl->incInfo->includeName.data();
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

  
  if (Config_getBool("CLASS_DIAGRAMS")) ol.disableAllBut(OutputGenerator::Man);

  
  // write super classes
  int count;
  if (m_impl->inherits && (count=m_impl->inherits->count())>0)
  {
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
  if (m_impl->inheritedBy && (count=m_impl->inheritedBy->count())>0)
  {
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
  if (m_impl->allMemberNameInfoSDict && 
      !Config_getBool("OPTIMIZE_OUTPUT_FOR_C")
     )
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.startTextLink(getMemberListFileName(),0);
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


  ///////////////////////////////////////////////////////////////////////////
  //// Member declarations + brief descriptions
  ///////////////////////////////////////////////////////////////////////////

  // write member groups
  ol.startMemberSections();

  // write user defined member groups
  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      if (!mg->allMembersInSameSection() || !m_impl->subGrouping) // group is in its own section
      {
        mg->writeDeclarations(ol,this,0,0,0);
      }
      else // add this group to the corresponding member section
      {
        //printf("addToDeclarationSection(%s)\n",mg->header().data());
        //mg->addToDeclarationSection();
      }
    }
  }

  // public types
  writeMemberDeclarations(ol,MemberList::pubTypes,theTranslator->trPublicTypes()); 

  // public methods
  writeMemberDeclarations(ol,MemberList::pubSlots,theTranslator->trPublicSlots()); 
  writeMemberDeclarations(ol,MemberList::signals,theTranslator->trSignals()); 
  writeMemberDeclarations(ol,MemberList::dcopMethods,theTranslator->trDCOPMethods()); 
  writeMemberDeclarations(ol,MemberList::pubMethods,theTranslator->trPublicMembers()); 
  writeMemberDeclarations(ol,MemberList::pubStaticMethods,theTranslator->trStaticPublicMembers()); 

  // public attribs
  writeMemberDeclarations(ol,MemberList::pubAttribs,theTranslator->trPublicAttribs()); 
  writeMemberDeclarations(ol,MemberList::pubStaticAttribs,theTranslator->trStaticPublicAttribs()); 
  
  // protected types
  writeMemberDeclarations(ol,MemberList::proTypes,theTranslator->trProtectedTypes()); 

  // protected methods
  writeMemberDeclarations(ol,MemberList::proSlots,theTranslator->trProtectedSlots()); 
  writeMemberDeclarations(ol,MemberList::proMethods,theTranslator->trProtectedMembers()); 
  writeMemberDeclarations(ol,MemberList::proStaticMethods,theTranslator->trStaticProtectedMembers());

  // protected attribs
  writeMemberDeclarations(ol,MemberList::proAttribs,theTranslator->trProtectedAttribs()); 
  writeMemberDeclarations(ol,MemberList::proStaticAttribs,theTranslator->trStaticProtectedAttribs()); 

  // package types
  writeMemberDeclarations(ol,MemberList::pacTypes,theTranslator->trPackageTypes()); 

  // package methods
  writeMemberDeclarations(ol,MemberList::pacMethods,theTranslator->trPackageMembers()); 
  writeMemberDeclarations(ol,MemberList::pacStaticMethods,theTranslator->trStaticPackageMembers()); 

  // package attribs
  writeMemberDeclarations(ol,MemberList::pacAttribs,theTranslator->trPackageAttribs()); 
  writeMemberDeclarations(ol,MemberList::pacStaticAttribs,theTranslator->trStaticPackageAttribs()); 

  // package
  writeMemberDeclarations(ol,MemberList::properties,theTranslator->trProperties()); 

  // events
  writeMemberDeclarations(ol,MemberList::events,theTranslator->trEvents()); 

  if (Config_getBool("EXTRACT_PRIVATE"))
  {
    // private types
    writeMemberDeclarations(ol,MemberList::priTypes,theTranslator->trPrivateTypes()); 

    // private members
    writeMemberDeclarations(ol,MemberList::priSlots,theTranslator->trPrivateSlots()); 
    writeMemberDeclarations(ol,MemberList::priMethods,theTranslator->trPrivateMembers()); 
    writeMemberDeclarations(ol,MemberList::priStaticMethods,theTranslator->trStaticPrivateMembers()); 

    // private attribs
    writeMemberDeclarations(ol,MemberList::priAttribs,theTranslator->trPrivateAttribs()); 
    writeMemberDeclarations(ol,MemberList::priStaticAttribs,theTranslator->trStaticPrivateAttribs()); 
  }

  // friends
  writeMemberDeclarations(ol,MemberList::friends,theTranslator->trFriends());

  // related functions
  writeMemberDeclarations(ol,MemberList::related,theTranslator->trRelatedFunctions(),
                                              theTranslator->trRelatedSubscript()
                         ); 
  //if (related) related->writeDeclarations(ol,this,0,0,0,
  //                theTranslator->trRelatedFunctions(),
  //                theTranslator->trRelatedSubscript()
  //               ); 

  // nested classes
  if (m_impl->innerClasses) m_impl->innerClasses->writeDeclaration(ol,0,0,TRUE);

  ol.endMemberSections();
    
  // write detailed description
  if (!Config_getBool("DETAILS_AT_TOP")) {
    writeDetailedDescription(ol,pageType,exampleFlag);
  }
  
  writeMemberDocumentation(ol);

  /////////////////////////////////////////////////////////////////////
  ol.startTextBlock();

  // write the list of used files (not for man pages)
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);

  showUsedFiles(ol);

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
  /////////////////////////////////////////////////////////////////////

  if (!Config_getString("GENERATE_TAGFILE").isEmpty()) 
  {
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "  </compound>" << endl;
  }
 
  endFile(ol);

  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    writeMemberPages(ol);
  }
}

void ClassDef::writeMemberDocumentation(OutputList &ol)
{
  ///////////////////////////////////////////////////////////////////////////
  //// Member definitions + detailed documentation
  ///////////////////////////////////////////////////////////////////////////

  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.disable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = TRUE;
  }
  
  writeMemberDocumentation(ol,MemberList::typedefMembers,theTranslator->trMemberTypedefDocumentation());
  writeMemberDocumentation(ol,MemberList::enumMembers,theTranslator->trMemberEnumerationDocumentation());
  writeMemberDocumentation(ol,MemberList::constructors,theTranslator->trConstructorDocumentation());
  writeMemberDocumentation(ol,MemberList::functionMembers,theTranslator->trMemberFunctionDocumentation());
  writeMemberDocumentation(ol,MemberList::relatedMembers,theTranslator->trRelatedFunctionDocumentation());
  writeMemberDocumentation(ol,MemberList::variableMembers,theTranslator->trMemberDataDocumentation());
  writeMemberDocumentation(ol,MemberList::propertyMembers,theTranslator->trPropertyDocumentation());
  writeMemberDocumentation(ol,MemberList::eventMembers,theTranslator->trEventDocumentation());

  if (Config_getBool("SEPARATE_MEMBER_PAGES"))
  {
    ol.enable(OutputGenerator::Html);
    Doxygen::suppressDocWarnings = FALSE;
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
    if (ml->listType()&MemberList::detailedLists)
    {
      ml->writeDocumentationPage(ol,name(),this);
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
            ol.writeString(md->name());
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
  if (m_impl->allMemberNameInfoSDict==0 || 
      Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) return;
  // only for HTML
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  QCString memListFile = getMemberListFileName();
  startFile(ol,memListFile,memListFile,
            theTranslator->trMemberList(),HLI_ClassVisible);
  startTitle(ol,0);
  ol.parseText(displayName()+" "+theTranslator->trMemberList());
  endTitle(ol,0,0);
  ol.parseText(theTranslator->trThisIsTheListOfAllMembers());
  ol.writeObjectLink(getReference(),getOutputFileBase(),0,displayName());
  ol.parseText(theTranslator->trIncludingInheritedMembers());
  
  //ol.startItemList();
  ol.writeString("<p><table>\n");
  
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
            (prot!=Public || (virt!=Normal && !m_impl->isObjC) || 
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
                !m_impl->isObjC)             sl.append("virtual");
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
  if (m_impl->memberGroupSDict)
  {
    MemberGroupSDict::Iterator mgli(*m_impl->memberGroupSDict);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      mg->setInGroup(inGroup);
      mg->writePlainDeclarations(ol,this,0,0,0);
    }
  }

  writePlainMemberDeclaration(ol,MemberList::pubTypes,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pubMethods,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pubAttribs,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pubSlots,inGroup);
  writePlainMemberDeclaration(ol,MemberList::signals,inGroup);
  writePlainMemberDeclaration(ol,MemberList::dcopMethods,inGroup);
  writePlainMemberDeclaration(ol,MemberList::properties,inGroup);
  writePlainMemberDeclaration(ol,MemberList::events,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pubStaticMethods,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pubStaticAttribs,inGroup);
  writePlainMemberDeclaration(ol,MemberList::proTypes,inGroup);
  writePlainMemberDeclaration(ol,MemberList::proMethods,inGroup);
  writePlainMemberDeclaration(ol,MemberList::proAttribs,inGroup);
  writePlainMemberDeclaration(ol,MemberList::proSlots,inGroup);
  writePlainMemberDeclaration(ol,MemberList::proStaticMethods,inGroup);
  writePlainMemberDeclaration(ol,MemberList::proStaticAttribs,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pacTypes,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pacMethods,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pacAttribs,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pacStaticMethods,inGroup);
  writePlainMemberDeclaration(ol,MemberList::pacStaticAttribs,inGroup);
  if (Config_getBool("EXTRACT_PRIVATE"))
  {
    writePlainMemberDeclaration(ol,MemberList::priTypes,inGroup);
    writePlainMemberDeclaration(ol,MemberList::priMethods,inGroup);
    writePlainMemberDeclaration(ol,MemberList::priAttribs,inGroup);
    writePlainMemberDeclaration(ol,MemberList::priSlots,inGroup);
    writePlainMemberDeclaration(ol,MemberList::priStaticMethods,inGroup);
    writePlainMemberDeclaration(ol,MemberList::priStaticAttribs,inGroup);
  }
  writePlainMemberDeclaration(ol,MemberList::friends,inGroup);
  writePlainMemberDeclaration(ol,MemberList::related,inGroup);
}

/*! a link to this class is possible within this project */
bool ClassDef::isLinkableInProject() const
{ 
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->isLinkableInProject();
  }
  else
  {
    return !name().isEmpty() &&    /* no name */
      !m_impl->artificial && !isHidden() &&
      name().find('@')==-1 && /* anonymous compound */
      (m_impl->prot!=Private || Config_getBool("EXTRACT_PRIVATE")) && /* private */
      (!m_impl->isLocal || Config_getBool("EXTRACT_LOCAL_CLASSES")) && /* local */
      hasDocumentation() &&   /* documented */ 
      !isReference() &&         /* not an external reference */
      (!m_impl->isStatic || Config_getBool("EXTRACT_STATIC"));
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
  static bool extractPrivate   = Config_getBool("EXTRACT_PRIVATE");
  static bool hideUndocClasses = Config_getBool("HIDE_UNDOC_CLASSES");
  static bool extractStatic    = Config_getBool("EXTRACT_STATIC");

  return // show all classes or a subclass is visible
      (allExternals || hasNonReferenceSuperClass()) &&
      // and not an annonymous compound
      name().find('@')==-1 &&
      // not an artifically introduced class
      !m_impl->artificial &&
      // and not privately inherited
      (m_impl->prot!=Private || extractPrivate) &&
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
  //printf("isBaseClass(cd=%s) looking for %s\n",cd->name().data(),bcd->name().data());
  if (level>256)
  {
    err("Possible recursive class relation while inside %s and looking for %s\n",name().data(),bcd->name().data());
    return FALSE;
  }
  if (baseClasses())
  {
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
  m_impl->membersMerged=TRUE;
  //printf("  mergeMembers for %s\n",name().data());
  bool inlineInheritedMembers = Config_getBool("INLINE_INHERITED_MEMB" );
  if (baseClasses())
  {
    BaseClassListIterator bcli(*baseClasses());
    BaseClassDef *bcd;
    for ( ; (bcd=bcli.current()) ; ++bcli )
    {
      ClassDef *bClass=bcd->classDef; 

      // merge the members in the base class of this inheritance branch first
      bClass->mergeMembers();

      MemberNameInfoSDict *srcMnd  = bClass->memberNameInfoSDict();
      MemberNameInfoSDict *dstMnd  =         m_impl->allMemberNameInfoSDict;

      if (srcMnd && dstMnd)
      {
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
    }
  }
  //printf("  end mergeMembers\n");
}

//----------------------------------------------------------------------------

/*! Merges the members of a Objective-C category into this class.
 */
void ClassDef::mergeCategory(ClassDef *category)
{
  category->setCategoryOf(this);
    
  MemberNameInfoSDict *srcMnd  = category->memberNameInfoSDict();
  MemberNameInfoSDict *dstMnd  =           m_impl->allMemberNameInfoSDict;

  if (srcMnd && dstMnd)
  {
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
}

//----------------------------------------------------------------------------

void ClassDef::addUsedClass(ClassDef *cd,const char *accessName)
{
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
     //printf("Adding used class %s to class %s\n",
     //    cd->name().data(),name().data());
  }
  ucd->addAccessor(accessName);
}

void ClassDef::addUsedByClass(ClassDef *cd,const char *accessName)
{
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
  ucd->addAccessor(accessName);
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
            if (cd==0) cd=getResolvedClass(usedClassName,0,&templSpec); // TODO: also try inbetween scopes!
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
    if (cd==0) cd=getClass(type.mid(i,l)); // TODO: also try inbetween scopes!
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
  if (m_impl->compType==Interface && m_impl->isObjC) return "class";
  switch (m_impl->compType)
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
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->getOutputFileBase();
  }
  else if (isReference())
  {
    return m_impl->fileName;
  }
  else
  {
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
    return convertNameToFile(m_impl->fileName+"-source"); 
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
    int startLine, const QCString &templSpec,bool &freshInstance)
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
    templateClass = new ClassDef(
        fileName,startLine,name()+templSpec,ClassDef::Class);
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
    Debug::print(Debug::Classes,0,"      New template variable instance class `%s'`%s'\n",name().data(),templSpec);
    templateClass = new ClassDef("<code>",1,name()+templSpec,
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
  static bool optimizeOutputJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
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
  if (optimizeOutputJava)
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
  return m_impl->className;
};

void ClassDef::addListReferences()
{
  if (!isLinkableInProject()) return;
  //printf("ClassDef(%s)::addListReferences()\n",name().data());
  {
    LockingPtr< QList<ListItemInfo> > xrefItems = xrefListItems();
    addRefItem(xrefItems.pointer(),
             theTranslator->trClass(TRUE,TRUE),
             getOutputFileBase(),displayName()
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
    if (ml->listType()&MemberList::detailedLists)
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

MemberList *ClassDef::createMemberList(MemberList::ListType lt)
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

MemberList *ClassDef::getMemberList(MemberList::ListType lt)
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

void ClassDef::addMemberToList(MemberList::ListType lt,MemberDef *md)
{
  static bool sortBriefDocs = Config_getBool("SORT_BRIEF_DOCS");
  MemberList *ml = createMemberList(lt);
  if (sortBriefDocs)
    ml->inSort(md);
  else
    ml->append(md);
  md->setSectionList(this,ml);
}

void ClassDef::writeMemberDeclarations(OutputList &ol,MemberList::ListType lt,const QCString &title,
               const char *subTitle)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDeclarations(ol,this,0,0,0,title,subTitle); 
}

void ClassDef::writeMemberDocumentation(OutputList &ol,MemberList::ListType lt,const QCString &title)
{
  MemberList * ml = getMemberList(lt);
  if (ml) ml->writeDocumentation(ol,name(),this,title);
}

void ClassDef::writePlainMemberDeclaration(OutputList &ol,MemberList::ListType lt,bool inGroup)
{
  MemberList * ml = getMemberList(lt);
  if (ml) 
  {
    ml->setInGroup(inGroup);
    ml->writePlainDeclarations(ol,this,0,0,0); 
  }
}

bool ClassDef::isLocal() const 
{ 
  return m_impl->isLocal; 
}

bool ClassDef::isArtificial() const 
{ 
  return m_impl->artificial; 
}

ClassSDict *ClassDef::getInnerClasses() 
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
  return m_impl->isAbstract; 
}

bool ClassDef::isObjectiveC() const 
{ 
  return m_impl->isObjC; 
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

void ClassDef::setClassIsArtificial() 
{ 
  m_impl->artificial = TRUE; 
}

void ClassDef::setIsStatic(bool b) 
{ 
  m_impl->isStatic=b; 
}

void ClassDef::setIsObjectiveC(bool b) 
{ 
  m_impl->isObjC=b; 
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

