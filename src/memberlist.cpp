/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#include <qregexp.h>

#include "memberlist.h"
#include "classdef.h"
#include "message.h"
#include "util.h"
#include "language.h"
#include "doxygen.h"
#include "outputlist.h"
#include "scanner.h"
#include "groupdef.h"

MemberList::MemberList() : QList<MemberDef>()
{
}

MemberList::~MemberList()
{
}

int MemberList::compareItems(GCI item1, GCI item2)
{
  MemberDef *c1=(MemberDef *)item1;
  MemberDef *c2=(MemberDef *)item2;
  return stricmp(c1->name(),c2->name());
}

void MemberList::countDecMembers()
{
  varCnt=funcCnt=enumCnt=enumValCnt=typeCnt=protoCnt=defCnt=friendCnt=0;
  m_count=0;
  MemberDef *md=first();
  while (md)
  {
    if (!(md->memberClass()==0 && md->isStatic() && !Config::extractPrivateFlag) &&
        (!Config::hideMemberFlag || md->hasDocumentation()) &&
        (
         (!Config::hideMemberFlag || !md->documentation().isEmpty() || 
          Config::briefMemDescFlag || Config::repeatBriefFlag
         ) || Config::extractAllFlag || 
         (md->isEnumerate() &&
          md->hasDocumentedEnumValues()
         )
        )
       )
    {
      switch(md->memberType())
      {
        case MemberDef::Variable:    varCnt++,m_count++;  break;
        case MemberDef::Function:    // fall through
        case MemberDef::Signal:      // fall through
        case MemberDef::Slot:        if (!md->isRelated() || md->memberClass())
                                       funcCnt++,m_count++; 
                                     break;
        case MemberDef::Enumeration: enumCnt++,m_count++; break;
        case MemberDef::EnumValue:   enumValCnt++,m_count++; break;
        case MemberDef::Typedef:     typeCnt++,m_count++; break;
        case MemberDef::Prototype:   protoCnt++,m_count++; break;
        case MemberDef::Define:      if (Config::extractAllFlag || 
                                         md->argsString() || 
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
}

void MemberList::countDocMembers(bool inGroup)
{
  /*varCnt=funcCnt=enumCnt=enumValCnt=typeCnt=protoCnt=defCnt=friendCnt=0;*/
  m_count=0;
  MemberDef *md=first();
  while (md)
  {
    //printf("%s MemberList::countDocMembers() details=%d\n",
    //    md->name().data(),md->detailsAreVisible());
    bool visibleIfStatic = 
      !(md->memberClass()==0 && md->isStatic() && !Config::extractPrivateFlag);

    if (visibleIfStatic && 
        (Config::extractAllFlag || md->detailsAreVisible()) &&
        (md->groupId()==-1 || inGroup)
       )
    {
      static QRegExp r("@[0-9]+");
      int dummy;
      switch(md->memberType())
      {
        case MemberDef::Enumeration:
          if (r.match(md->name(),0,&dummy)==-1)
          {
            m_count++;
          }
          break;
        case MemberDef::EnumValue:   
          {
            MemberDef *scope;
            scope=md->getEnumScope();
            if (scope && r.match(scope->name(),0,&dummy)!=-1) 
              m_count++;
          }
          break;
        default:
          m_count++;
          break;
      }

//      QRegExp r("@[0-9]+");
//      int dummy;
//      switch(md->memberType())
//      {
//        case MemberDef::Variable:    
//          varCnt++;     
//          break;
//        case MemberDef::Function:    
//        case MemberDef::Signal:
//        case MemberDef::Slot:
//          funcCnt++;    
//          break;
//        case MemberDef::Enumeration:
//          if (r.match(md->name(),0,&dummy)==-1)
//          {
//            enumCnt++;
//          }
//          break;
//        case MemberDef::EnumValue:   
//          {
//            MemberDef *scope;
//            scope=md->getEnumScope();
//            if (scope && r.match(scope->name(),0,&dummy)!=-1) 
//              enumValCnt++;
//          }
//          break;
//        case MemberDef::Typedef:     
//          typeCnt++;    
//          break;
//        case MemberDef::Prototype:   
//          protoCnt++;   
//          break;
//        case MemberDef::Define:      
//          defCnt++;
//          break;
//        case MemberDef::Friend:
//          friendCnt++;
//      }
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
                       bool inGroup)
{
  countDecMembers();
  if (totalCount()==0) return; // no members in this list
  
  ol.pushGeneratorState();

  int prevGroupId = -1;
  bool sectionPerType = fd || nd || gd;
  if (!sectionPerType) ol.startMemberList();
  MemberDef *md;

  if (sectionPerType && defineCount()>0)
  {
    ol.startMemberHeader();
    parseText(ol,theTranslator->trDefines());
    ol.endMemberHeader();
    ol.startMemberList();
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()); ++mli )
    {
      if (md->isDefine() && 
          (md->argsString() || md->hasDocumentation() || Config::extractAllFlag)
         ) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,prevGroupId,inGroup);
        prevGroupId = md->groupId();
      }
    }
    ol.endMemberList();
  }
  
  if (sectionPerType && protoCount()>0)
  {
    ol.startMemberHeader();
    parseText(ol,theTranslator->trFuncProtos());
    ol.endMemberHeader();
    ol.startMemberList();
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()); ++mli )
    {
      if (md->isPrototype()) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,prevGroupId,inGroup);
        prevGroupId = md->groupId();
      }
    }
    ol.endMemberList();
  }
  
  if (typedefCount()>0)
  {
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
      if (md->isTypedef()) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,prevGroupId,inGroup);
        prevGroupId = md->groupId();
      }
    }
    if (sectionPerType) ol.endMemberList();
  }
 
  // write enums 
  if (enumCount()>0)
  {
    if (sectionPerType) 
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trEnumerations());
      ol.endMemberHeader();
      ol.startMemberList();
    }
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()) ; ++mli )
    {
      /*bool hasDocs=md->hasDocumentation();*/
      QCString type=md->typeString();
      type=type.stripWhiteSpace();
      if (md->isEnumerate() /*&& (hasDocs || !Config::hideMemberFlag)*/) 
      {
        if (!Config::hideMemberFlag ||                // do not hide undocumented members or
            !md->documentation().isEmpty() || // member has detailed descr. or
            md->hasDocumentedEnumValues() ||  // member has documented enum vales.
            Config::briefMemDescFlag ||               // brief descr. is shown or
            Config::repeatBriefFlag                   // brief descr. is repeated.
           )
        {
          OutputList typeDecl(&ol);
          QCString name=md->name().copy();
          int i=name.findRev("::");
          if (i!=-1) name=name.right(name.length()-i-2); // strip scope
          if (name[0]!='@') // not an anonymous enum
          {
            //if (Config::extractAllFlag ||
            //    (md->briefDescription().isEmpty() || !Config::briefMemDescFlag) &&
            //    (!md->documentation().isEmpty() || md->hasDocumentedEnumValues() ||
            //     (!md->briefDescription().isEmpty() && 
            //      !Config::briefMemDescFlag &&
            //      Config::repeatBriefFlag
            //     )
            //    )
            //   )
            if (md->isLinkableInProject() || md->hasDocumentedEnumValues())
            {
              if (!Config::genTagFile.isEmpty())
                tagFile << md->name() << " " << md->anchor() 
                  << " \"\"" << endl;
              md->writeLink(typeDecl,cd,nd,fd,gd,0);
            }
            else
            {
              typeDecl.writeBoldString(name);
            }
            typeDecl.writeChar(' ');
          }

          typeDecl.docify("{ ");
          QList<MemberDef> *fmdl=md->enumFieldList();
          if (fmdl)
          {
            MemberDef *fmd=fmdl->first();
            while (fmd)
            {
              if (fmd->hasDocumentation()) // enum value has docs
              {
                if (!Config::genTagFile.isEmpty())
                  tagFile << fmd->name() << " " << fmd->anchor() 
                    << " \"" << fmd->argsString() << "\"";
                fmd->writeLink(typeDecl,cd,nd,fd,gd,0);
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
              if ((vtype.find(name))!=-1) enumVars++;
            }
          }
          if (enumVars==0) // no variable of this enum type
          {
            int gId = md->groupId();
            const char *gHeader = (gId!=prevGroupId && gId!=-1) ? 
                            memberGroupDict[gId]->header().data() : 0;
            ol.startMemberItem(gId!=-1,0);
            ol.writeString("enum ");
            ol.insertMemberAlign();
            ol+=typeDecl;
            ol.endMemberItem(gId!=-1,md->anchor(),gHeader,FALSE);
            //QCString brief=md->briefDescription();
            //brief=brief.stripWhiteSpace();
            if (!md->briefDescription().isEmpty() && Config::briefMemDescFlag
                && gId==-1 && !inGroup)
            {
              ol.startMemberDescription();
              parseDoc(ol,cd?cd->name().data():0,
                  md->name().data(),md->briefDescription());
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
              ol.disable(OutputGenerator::Man);
              ol.newParagraph();
              ol.enable(OutputGenerator::Man);
            }
          }
          md->warnIfUndocumented();
          prevGroupId = md->groupId();
        }
      } // md->isEnumerate()
    } // enum loop
    if (sectionPerType) ol.endMemberList();
  } // write enums
 
  // write functions
  if (funcCount()>0)
  {
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
          ( md->isFunction() || md->isSignal() || md->isSlot()) &&
          ( !md->isRelated() || md->memberClass() )
         ) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,prevGroupId,inGroup);
        prevGroupId = md->groupId();
      }
    }
    if (sectionPerType) ol.endMemberList();
  }
  
  if (friendCount()>0)
  {
    MemberListIterator mli(*this);
    for ( ; (md=mli.current()) ; ++mli )
    {
      if (md->isFriend()) 
      {
        QCString type=md->typeString();
        //printf("Friend: type=%s name=%s\n",type.data(),md->name().data());
        if (md->hasDocumentation() && type!="friend class")
        {
          md->writeDeclaration(ol,cd,nd,fd,gd,prevGroupId,inGroup);
          prevGroupId = md->groupId();
        }
        else // friend is undocumented as a member but it is a class, 
             // so generate a link to the class if that is documented.
        {
          int gId = md->groupId();
          const char *gHeader = (gId!=prevGroupId && gId!=-1) ? 
                          memberGroupDict[gId]->header().data() : 0;
          ClassDef *cd=getClass(md->name());
          if (md->hasDocumentation()) // friend is documented
          {
            ol.startMemberItem(gId!=-1,0);
            ol.docify("class ");
            ol.insertMemberAlign();
            ol.writeObjectLink(0,0,md->anchor(),md->name());
            ol.endMemberItem(gId!=-1,md->anchor(),gHeader,FALSE);
            prevGroupId = md->groupId();
          }
          else if (cd && cd->isLinkable()) // class is documented
          {
            ol.startMemberItem(gId!=-1,0);
            ol.docify("class ");
            ol.insertMemberAlign();
            ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->name());
            ol.endMemberItem(gId!=-1,md->anchor(),gHeader,FALSE);
            prevGroupId = md->groupId();
          }
          else if (!Config::hideMemberFlag) // no documentation
          {
            ol.startMemberItem(gId!=-1,0);
            ol.docify("class ");
            ol.insertMemberAlign();
            ol.writeBoldString(md->name());
            ol.endMemberItem(gId!=-1,md->anchor(),gHeader,FALSE);
            prevGroupId = md->groupId();
          }
        }
      }
    }
  }

  // write variables
  if (varCount()>0)
  {
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
      if (md->isVariable()) 
      {
        md->writeDeclaration(ol,cd,nd,fd,gd,prevGroupId,inGroup);
        prevGroupId = md->groupId();
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
      if (md->fromAnnonymousScope() && !md->annonymousDeclShown())
      {
        md->setFromAnnonymousScope(FALSE);
        md->writeDeclaration(ol,cd,nd,fd,gd,prevGroupId,inGroup);
        md->setFromAnnonymousScope(TRUE);
        prevGroupId = md->groupId();
      }
    }
  }
 
  if (!sectionPerType) { ol.endMemberList(); /*ol.writeChar('\n');*/ }

  if (prevGroupId!=-1 && !inGroup)
  {
    ol.memberGroupSpacing(TRUE);
    ol.memberGroupSeparator();
  }
  ol.popGeneratorState();
}

void MemberList::writeDeclarations(OutputList &ol,
             ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
             const char *title,const char *subtitle,bool inGroup)
{
  countDecMembers();
  if (totalCount()==0) return;
  if (title) 
  {
    ol.startMemberHeader();
    parseText(ol,title);
    ol.endMemberHeader();
  }
  if (subtitle) 
  {
    ol.startMemberSubtitle();
    parseText(ol,subtitle);
    ol.endMemberSubtitle();
  }

  writePlainDeclarations(ol,cd,nd,fd,gd,inGroup);
}

void MemberList::writeDocumentation(OutputList &ol,
                     const char *scopeName/*, MemberDef::MemberType m*/)
{
  MemberListIterator mli(*this);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    md->writeDocumentation(this,ol,scopeName);
  }
}
