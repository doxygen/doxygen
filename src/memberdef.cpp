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

#include <stdio.h>
#include <qregexp.h>
#include "memberdef.h"
#include "membername.h"
#include "doxygen.h"
#include "util.h"

/*! Creates a new member definition.
 *  Members can be function/variables/enums/etc. inside a class or inside a 
 *  file.
 *
 * \param t A string representing the type of the member.
 * \param n A string representing the name of the member.
 * \param a A string representing the arguments of the member.
 * \param p The type of protection of the member, possible values are:
 *          \c Public, \c Protected, \c Private.
 * \param v The `virtualness' of the member, possible values are:
 *          \c Normal, \c Virtual, \c Pure.
 * \param s A boolean that is true if the member is static.
 * \param r A boolean that is true if the member is only related.
 * \param mt The kind of member. See #MemberDef::MemberType for a list of 
 *           all types.
 */

MemberDef::MemberDef(const char *t,const char *na,const char *a,const char *e,
                     Protection p,Specifier v,bool s,bool r,MemberType mt,
                     const ArgumentList *tal,const ArgumentList *al
                    ) : Definition(substituteClassNames(na))
{
  //printf("++++++ MemberDef(%s,%s,%s) ++++++ \n",t,na,a);
  classDef=0;
  fileDef=0;
  fileDec=0;
  redefines=0;
  redefinedBy=0;
  nspace=0;
  memDef=0;
  memDec=0;
  exampleList=0;
  exampleDict=0;
  enumFields=0;
  enumScope=0;
  enumDeclList=0;
  type=substituteClassNames(t);
  args=substituteClassNames(a);
  if (type.isNull()) decl=name()+args; else decl=type+" "+name()+args;
  declLine=0;
  defLine=0;
  virt=v;
  prot=p;
  related=r;
  stat=s;
  mtype=mt;
  exception=e;
  eUsed=FALSE;
  proto=FALSE;
  docEnumValues=FALSE;
  // copy function template arguments (if any)
  if (tal)
  {
    tArgList = new ArgumentList;
    tArgList->setAutoDelete(TRUE);
    ArgumentListIterator ali(*tal);
    Argument *a;
    for (;(a=ali.current());++ali)
    {
      tArgList->append(new Argument(*a));
    }
  }
  else
  {
    tArgList=0;
  }
  // copy function arguments (if any)
  if (al)
  {
    argList = new ArgumentList;
    argList->setAutoDelete(TRUE);
    ArgumentListIterator ali(*al);
    Argument *a;
    for (;(a=ali.current());++ali)
    {
      argList->append(new Argument(*a));
    }
    argList->constSpecifier    = al->constSpecifier;
    argList->volatileSpecifier = al->volatileSpecifier;
    argList->pureSpecifier     = al->pureSpecifier;
  }
  else
  {
    argList=0;
  }

}

MemberDef::~MemberDef()
{
  delete redefinedBy;
  delete exampleList;
  delete exampleDict;
  delete enumFields;
  delete argList;
  delete tArgList;
}

void MemberDef::insertReimplementedBy(MemberDef *md)
{
  if (redefinedBy==0) redefinedBy = new MemberList;
  redefinedBy->inSort(md);
}

void MemberDef::insertEnumField(MemberDef *md)
{
  if (enumFields==0) enumFields=new MemberList;
  enumFields->append(md);
}

bool MemberDef::addExample(const char *anchor,const char *nameStr,
                           const char *file)
{
  //printf("%s::addExample(%s,%s,%s)\n",name.data(),anchor,nameStr,file);
  if (exampleDict==0) exampleDict = new ExampleDict;
  if (exampleList==0) exampleList = new ExampleList;
  if (exampleDict->find(nameStr)==0) 
  {
    //printf("Add reference to example %s to member %s\n",nameStr,name.data());
    Example *e=new Example;
    e->anchor=anchor;
    e->name=nameStr;
    e->file=file;
    exampleDict->insert(nameStr,e);
    exampleList->inSort(e); 
    return TRUE;
  }
  return FALSE; 
}

bool MemberDef::hasExamples()
{
  if (exampleList==0) 
    return FALSE;
  else
    return exampleList->count()>0;
}


void MemberDef::writeExample(OutputList &ol)
{
  Example *e=exampleList->first();
  while (e)
  {
    ol.writeObjectLink(0,e->file,e->anchor,e->name);
    e=exampleList->next();
    if (e)
    {
      if (exampleList->at()==(int)exampleList->count()-1)
        ol.writeString(" and ");
      else
        ol.writeString(", ");
    }
  }
  ol.writeString(".");
}

