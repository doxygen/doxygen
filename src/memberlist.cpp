/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#include <qregexp.h>

#include "memberlist.h"
#include "classdef.h"
#include "message.h"
#include "util.h"
#include "language.h"
#include "doxygen.h"
#include "outputlist.h"
#include "doc.h"
#include "groupdef.h"

MemberList::MemberList() : QList<MemberDef>()
{
  memberGroupList=0;
}

MemberList::~MemberList()
{
  delete memberGroupList;
}

int MemberList::compareItems(GCI item1, GCI item2)
{
  MemberDef *c1=(MemberDef *)item1;
  MemberDef *c2=(MemberDef *)item2;
  return strcmp(c1->name(),c2->name());
}

void MemberList::countDecMembers(bool inGroup,bool countSubGroups,bool sectionPerType)
{
  //printf("----- countDecMembers ----\n");
  varCnt=funcCnt=enumCnt=enumValCnt=typeCnt=protoCnt=defCnt=friendCnt=0;
  m_count=0;
  MemberDef *md=first();
  while (md)
  {
    //printf("md=%p md->name()=`%s' inGroup=%d getMemberGroup()=%p\n",
    //    md,md->name().data(),inGroup,md->getMemberGroup());
    if (!(md->getClassDef()==0 && md->isStatic() && !Config::extractStaticFlag) &&
        (!Config::hideMemberFlag || md->hasDocumentation()) &&
        (
         (!Config::hideMemberFlag || !md->documentation().isEmpty() || 
          Config::briefMemDescFlag || Config::repeatBriefFlag
         ) || Config::extractAllFlag || 
         (md->isEnumerate() &&
          md->hasDocumentedEnumValues()
         )
        ) && 
        inGroup==md->visibleMemberGroup(sectionPerType) &&
        !(inGroup && md->protection()==Private && !Config::extractPrivateFlag)
       )
    {
      switch(md->memberType())
      {
        case MemberDef::Variable:    // fall through
        case MemberDef::Property:    varCnt++,m_count++;  
                                     break;
        case MemberDef::Function:    // fall through
        case MemberDef::Signal:      // fall through
        case MemberDef::DCOP:        // fall through
        case MemberDef::Slot:        if (!md->isRelated() || md->getClassDef())
                                       funcCnt++,m_count++; 
                                     break;
        case MemberDef::Enumeration: enumCnt++,m_count++; break;
        case MemberDef::EnumValue:   enumValCnt++,m_count++; break;
        case MemberDef::Typedef:     typeCnt++,m_count++; break;
        case MemberDef::Prototype:   protoCnt++,m_count++; break;
        case MemberDef::Define:      if (Config::extractAllFlag || 
                                         md->argsString() || 
                                         !md->initializer().isEmpty() ||
                                         md->hasDocumentation() 
                                        ) defCnt++,m_count++;     
                                     break;
        case MemberDef::Friend:      friendCnt++,m_count++;  break;
        default:
          err("Error: Unknown member type found!");
      }
    }
    md=next();
  }
  if (memberGroupList && countSubGroups)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      int mgCount = mg->countDecMembers(sectionPerType);
      //printf("memberGroupList adding %d inGroup=%d\n",mgCount,inGroup);
      m_count+=mgCount;
    }
  }
  //printf("----- end countDecMembers ----\n");

  //printf("MemberList::countDecMembers(%d)=%d\n",inGroup,m_count);
}

void MemberList::countDocMembers(bool listOfGroup)
{
  /*varCnt=funcCnt=enumCnt=enumValCnt=typeCnt=protoCnt=defCnt=friendCnt=0;*/
  m_count=0;
  MemberDef *md=first();
  while (md)
  {
    //printf("%s MemberList::countDocMembers() details=%d\n",
    //    md->name().data(),md->detailsAreVisible());
    bool visibleIfStatic = 
      !(md->getClassDef()==0 && md->isStatic() && !Config::extractStaticFlag);

    bool inOwnGroup = (md->getGroupDef()!=0 && !listOfGroup);
    
    if (visibleIfStatic && !inOwnGroup &&
        (Config::extractAllFlag || md->detailsAreVisible()) 
       )
    {
      if (md->memberType()!=MemberDef::EnumValue) m_count++;
    }
    md=next();
  }
}

bool MemberList::insert(uint index,const MemberDef *md)
{
  return QList<MemberDef>::insert(index,md);
}

void MemberList::inSort(const MemberDef *md)
{
  QList<MemberDef>::inSort(md);
}

void MemberList::append(const MemberDef *md)
{
  QList<MemberDef>::append(md);
}

MemberListIterator::MemberListIterator(const QList<MemberDef> &l) :
  QListIterator<MemberDef>(l) 
{
}

void MemberList::writePlainDeclarations(OutputList &ol,
                       ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
                       bool inGroup,bool countSubGroups
                      )
{
  bool sectionPerType = !inGroup && (fd || nd || gd);
  //printf("----- writePlainDeclaration() ----\n");
  countDecMembers(inGroup,countSubGroups,sectionPerType);
  if (totalCount()==0) return; // no members in this list
  //printf("----> writePlainDeclaration() inGroup=%d totalCount()=%d\n",inGroup,totalCount());
  
  ol.pushGeneratorState();

  //int prevGroupId = -1;
  if (!sectionPerType) ol.startMemberList();
  MemberDef *md;

  if (defineCount()>0)
  {
    if (sectionPerType)
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trDefines());
      ol.endMemberHeader();
      ol.startMemberList();
    }
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()); ++mli )
    {
      //printf("md->isDefined()=%d inGroup=%d md->getMemberGroup()=%p\n",
      //   md->isDefine(),inGroup,md->getMemberGroup());
      if (md->isDefine() && 
          (md->argsString() || md->hasDocumentation() || 
           !md->initializer().isEmpty() ||
           Config::extractAllFlag) &&
          inGroup==md->visibleMemberGroup(sectionPerType)         
         ) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,inGroup);
      }
    }
    if (sectionPerType)
    {
      ol.endMemberList();
    }
  }
  
  if (protoCount()>0)
  {
    //printf("There are %d prototypes\n",protoCount());
    if (sectionPerType)
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trFuncProtos());
      ol.endMemberHeader();
      ol.startMemberList();
    }
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()); ++mli )
    {
      if (md->isPrototype() && inGroup==md->visibleMemberGroup(sectionPerType)) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,inGroup);
      }
    }
    if (sectionPerType)
    {
      ol.endMemberList();
    }
  }
  
  if (typedefCount()>0)
  {
    //printf("There are %d typedefs\n",typedefCount());
    if (sectionPerType) 
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trTypedefs());
      ol.endMemberHeader();
      //ol.writeMemberHeader("Typedefs");
      ol.startMemberList();
    }
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()) ; ++mli )
    {
      if (md->isTypedef() && inGroup==md->visibleMemberGroup(sectionPerType)) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,inGroup);
      }
    }
    if (sectionPerType) ol.endMemberList();
  }
 
  // write enums 
  if (enumCount()>0)
  {
    //printf("There are %d enums\n",enumCount());
    if (sectionPerType) 
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trEnumerations());
      ol.endMemberHeader();
      ol.startMemberList();
    }
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()) ; ++mli ) // iterate through the members
    {
      // see if member is hidden by protection
      if (md->protection()==Private && !Config::extractPrivateFlag) continue;
      
      QCString type=md->typeString();
      type=type.stripWhiteSpace();  // TODO: is this really needed?

      // filter out enums that are in a group iff inGroup holds
      if (md->isEnumerate() && inGroup==md->visibleMemberGroup(sectionPerType) /*&& (hasDocs || !Config::hideMemberFlag)*/) 
      {
        // filter out invisible enums
        if ( !Config::hideMemberFlag ||        // do not hide undocumented members or
             !md->documentation().isEmpty() || // member has detailed descr. or
             md->hasDocumentedEnumValues() ||  // member has documented enum vales.
             ( 
               !md->briefDescription().isEmpty() &&
               Config::briefMemDescFlag              // brief descr. is shown or
             )                                        
           )
        {
          OutputList typeDecl(&ol);
          QCString name=md->name().copy();
          int i=name.findRev("::");
          if (i!=-1) name=name.right(name.length()-i-2); // strip scope
          if (name[0]!='@') // not an anonymous enum
          {
            if (md->isLinkableInProject() || md->hasDocumentedEnumValues())
            {
              if (!Config::genTagFile.isEmpty())
              {
                tagFile << "    <member>" << endl;
                tagFile << "      <name>" << convertToXML(md->name()) << "</name>" << endl; 
                tagFile << "      <anchor>" << convertToXML(md->anchor()) << "</anchor>" << endl; 
                tagFile << "      <arglist>" << convertToXML(md->argsString()) << "</arglist>" << endl; 
                tagFile << "    </member>" << endl;
              }
              md->writeLink(typeDecl,cd,nd,fd,gd);
            }
            else
            {
              typeDecl.writeBoldString(name);
            }
            typeDecl.writeChar(' ');
          }

          int enumMemCount=0;
          
          typeDecl.docify("{ ");
          QList<MemberDef> *fmdl=md->enumFieldList();
          if (fmdl)
          {
            MemberDef *fmd=fmdl->first();
            while (fmd)
            {
              /* in html we start a new line after a number of items */
              if (fmdl->count()>(uint)Config::enumValuesPerLine
                  && (enumMemCount%(uint)Config::enumValuesPerLine)==0
                 )
              {
                typeDecl.pushGeneratorState();
                typeDecl.disableAllBut(OutputGenerator::Html);
                typeDecl.lineBreak(); 
                typeDecl.writeString("&nbsp;&nbsp;");
                typeDecl.popGeneratorState();
              }

              if (fmd->hasDocumentation()) // enum value has docs
              {
              if (!Config::genTagFile.isEmpty())
              {
                tagFile << "    <member>" << endl;
                tagFile << "      <name>" << convertToXML(fmd->name()) << "</name>" << endl; 
                tagFile << "      <anchor>" << convertToXML(fmd->anchor()) << "</anchor>" << endl; 
                tagFile << "      <arglist>" << convertToXML(fmd->argsString()) << "</arglist>" << endl; 
                tagFile << "    </member>" << endl;
              }
                fmd->writeLink(typeDecl,cd,nd,fd,gd);
              }
              else // no docs for this enum value
              {
                typeDecl.writeBoldString(fmd->name());
              }
              if (!fmd->initializer().isEmpty()) // enum value has initializer
              {
                typeDecl.writeString(" = ");
                parseText(typeDecl,fmd->initializer());
              }
              
              fmd=fmdl->next();
              if (fmd) typeDecl.writeString(", ");
              typeDecl.disable(OutputGenerator::Man);
              typeDecl.writeString("\n"); // to prevent too long lines in LaTeX
              typeDecl.enable(OutputGenerator::Man);
              enumMemCount++;
            }
            if (fmdl->count()>(uint)Config::enumValuesPerLine)
            {
              typeDecl.pushGeneratorState();
              typeDecl.disableAllBut(OutputGenerator::Html);
              typeDecl.lineBreak(); 
              typeDecl.popGeneratorState();
            }
          }
          typeDecl.docify(" }");
          md->setEnumDecl(typeDecl);
          int enumVars=0;
          MemberListIterator vmli(*this);
          MemberDef *vmd;
          if (name[0]=='@') // anonymous enum => append variables
          {
            for ( ; (vmd=vmli.current()) ; ++vmli)
            {
              QCString vtype=vmd->typeString();
              if ((vtype.find(name))!=-1) 
              {
                enumVars++;
                vmd->setAnonymousEnumType(md);
              }
            }
          }
          // if this is an annoymous enum and there are variable of this
          // enum type (i.e. enumVars>0), then we do not show the enum here.
          if (enumVars==0) // show enum here
          {
            ol.startMemberItem(0);
            ol.writeString("enum ");
            ol.insertMemberAlign();
            ol+=typeDecl; // append the enum values.
            ol.endMemberItem(FALSE);
            if (!md->briefDescription().isEmpty() && Config::briefMemDescFlag)
            {
              ol.startMemberDescription();
              parseDoc(ol,
                       md->getDefFileName(),md->getDefLine(),
                       cd?cd->name().data():0,md->name().data(),
                       md->briefDescription()
                      );
              if (!md->documentation().isEmpty() || md->hasDocumentedEnumValues())
              {
                ol.disableAllBut(OutputGenerator::Html);
                ol.endEmphasis();
                ol.docify(" ");
                ol.startTextLink(0,md->anchor());
                parseText(ol,theTranslator->trMore());
                ol.endTextLink();
                ol.startEmphasis();
                ol.enableAll();
              }
              ol.endMemberDescription();
            }
          }
          md->warnIfUndocumented();
        }
      } // md->isEnumerate()
    } // enum loop
    if (sectionPerType) ol.endMemberList();
  } // write enums
 
  // write functions
  if (funcCount()>0)
  {
    //printf("There are %d functions\n",funcCount());
    if (sectionPerType) 
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trFunctions());
      ol.endMemberHeader();
      ol.startMemberList();
    }
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()) ; ++mli )
    {
      if (
          ( md->isFunction() || md->isSignal() || md->isSlot() || md->isDCOP()) &&
          ( !md->isRelated() || md->getClassDef() ) &&
          inGroup==md->visibleMemberGroup(sectionPerType)
         ) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,inGroup);
      }
    }
    if (sectionPerType) ol.endMemberList();
  }
  
  if (friendCount()>0)
  {
    //printf("There are %d friends\n",friendCount());
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()) ; ++mli )
    {
      if (md->isFriend() && inGroup==md->visibleMemberGroup(sectionPerType)) 
      {
        QCString type=md->typeString();
        //printf("Friend: type=%s name=%s\n",type.data(),md->name().data());
        if (md->hasDocumentation() && type!="friend class")
        {
          md->writeDeclaration(ol,cd,nd,fd,gd,inGroup);
        }
        else // friend is undocumented as a member but it is a class, 
             // so generate a link to the class if that is documented.
        {
          ClassDef *cd=getClass(md->name());
          if (md->hasDocumentation()) // friend is documented
          {
            ol.startMemberItem(0);
            ol.docify("class ");
            ol.insertMemberAlign();
            ol.writeObjectLink(0,0,md->anchor(),md->name());
            ol.endMemberItem(FALSE);
          }
          else if (cd && cd->isLinkable()) // class is documented
          {
            ol.startMemberItem(0);
            ol.docify("class ");
            ol.insertMemberAlign();
            ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->name());
            ol.endMemberItem(FALSE);
          }
          else if (!Config::hideMemberFlag) // no documentation
          {
            ol.startMemberItem(0);
            ol.docify("class ");
            ol.insertMemberAlign();
            ol.writeBoldString(md->name());
            ol.endMemberItem(FALSE);
          }
        }
      }
    }
  }

  // write variables
  if (varCount()>0)
  {
    //printf("There are %d variables\n",varCount());
    if (sectionPerType) 
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trVariables());
      ol.endMemberHeader();
      ol.startMemberList();
    }
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()) ; ++mli )
    {
      if ((md->isVariable() || md->isProperty()) && 
          inGroup==md->visibleMemberGroup(sectionPerType)) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,inGroup);
      }
    }
    if (sectionPerType) ol.endMemberList();
  }

  // handle members that are inside annonymous compounds and for which
  // no variables of the annonymous compound type exist.
  if (cd)
  {
    MemberListIterator mli(*this);
    for  ( ; (md=mli.current()) ; ++mli )
    {
      if (md->fromAnnonymousScope() && !md->annonymousDeclShown()
          && inGroup==md->visibleMemberGroup(sectionPerType))
      {
        //printf("annonymous compound members\n");
        md->setFromAnnonymousScope(FALSE);
        md->writeDeclaration(ol,cd,nd,fd,gd,inGroup);
        md->setFromAnnonymousScope(TRUE);
      }
    }
  }
 
  if (!sectionPerType) { ol.endMemberList(); /*ol.writeChar('\n');*/ }

  ol.popGeneratorState();
  //printf("----- end writePlainDeclaration() ----\n");
}

void MemberList::writeDeclarations(OutputList &ol,
             ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
             const char *title,const char *subtitle,
             bool inGroup,bool countSubGroups)
{
  //printf("MemberList::writeDeclaration(title=`%s',subtitle=`%s')\n",title,subtitle);
  //printf("----- writeDeclaration() ----\n");
  bool sectionPerType = !inGroup && (fd || nd || gd);
  countDecMembers(FALSE,countSubGroups,sectionPerType); // count member not in group
  int normalMembers = totalCount();
  int ingroupMembers = 0;
  //printf("Normal members %d\n",normalMembers);
  if (inGroup)
  {
    countDecMembers(TRUE,countSubGroups,sectionPerType); // count member inside group
    ingroupMembers = totalCount();
  }
  int totalMembers = normalMembers + ingroupMembers;
  //printf("Total members %d\n",totalMembers);
  if (totalMembers==0) return;
  if (title) 
  {
    ol.startMemberHeader();
    parseText(ol,title);
    ol.endMemberHeader();
  }
  if (subtitle && subtitle[0]!=0) 
  {
    //printf("subtitle=`%s'\n",subtitle);
    ol.startMemberSubtitle();
    if (inGroup)
      parseDoc(ol,"<generated>",1,0,0,subtitle);
    else
      parseText(ol,subtitle);
    ol.endMemberSubtitle();
  }

  writePlainDeclarations(ol,cd,nd,fd,gd,inGroup,FALSE);
  
  if (memberGroupList)
  {
    MemberGroupListIterator mgli(*memberGroupList);
    MemberGroup *mg;
    while ((mg=mgli.current()))
    {
      bool hasHeader=mg->header()!="[NOHEADER]";
      ol.startMemberGroupHeader(hasHeader);
      if (hasHeader)
      {
        parseText(ol,mg->header());
      }
      ol.endMemberGroupHeader();
      if (!mg->documentation().isEmpty())
      {
        //printf("Member group has docs!\n");
        ol.startMemberGroupDocs();
        parseDoc(ol,"<generated>",1,0,0,mg->documentation());
        ol.endMemberGroupDocs();
      }
      ol.startMemberGroup();
      mg->writePlainDeclarations(ol,cd,nd,fd,gd);
      ++mgli;
      ol.endMemberGroup(hasHeader);
    }
  }
  //printf("----- end writeDeclaration() ----\n");

}

void MemberList::writeDocumentation(OutputList &ol,
                     const char *scopeName, Definition *container)
{
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    md->writeDocumentation(this,ol,scopeName,container);
  }
}

void MemberList::addMemberGroup(MemberGroup *mg)
{
  if (memberGroupList==0)
  {
    memberGroupList=new MemberGroupList;
  }
  memberGroupList->append(mg);
}


