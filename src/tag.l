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

%{

/*
 *	includes
 */
#include <stdio.h>
  
#include "qtbc.h"
#include <qfileinfo.h>
  
#include "classdef.h"
#include "filedef.h"
#include "namespacedef.h"
#include "memberdef.h"
#include "doxygen.h"
#include "util.h"
#include "message.h"
#include "defargs.h"
  
#define YY_NO_UNPUT
#define YY_NEVER_INTERACTIVE 1

static int yyLineNr;
static QCString className;
static QCString fileName;
static QCString namespaceName;
static QCString tagName;
static QCString memberName;
static QCString anchorName;
static QCString argString;
static ClassDef *cd;
static FileDef *fd;
static NamespaceDef *nd;

static void addClass(const char *name,const char *fileName)
{
  //printf("adding class %s\n",name);
  if (name!=0 && strlen(name)>0 && classDict[name]==0)
  {
    cd = new ClassDef(fileName,1,name,ClassDef::Class,tagName,fileName);
    fd = 0;
    nd = 0;
    classList.inSort(cd);
    classDict.insert(className,cd);
  }
}

static void addFile(const char *name)
{
  //printf("adding file %s tagName=`%s'\n",name,tagName.data());
  fd = new FileDef(0,name,tagName);
  FileName *mn;
  if ((mn=inputNameDict[name]))
  {
    mn->append(fd); 
  }
  else
  {
    mn = new FileName(name,name);
    mn->append(fd);
    inputNameList.inSort(mn);
    inputNameDict.insert(name,mn);
  }
  cd = 0;
  nd = 0;
  //fileList.inSort(fd);
  //fileDict.insert(fileName,fd);
}

static void addNamespace(const char *name)
{
  if ((nd=namespaceDict[name])==0)
  {
    // TODO: we assume that each namespace is limited to a single tagfile.
    // since namespace are open, this need not to be the case. As a result
    // namespace may contain members that are located in 
    // different namespaces!
    nd = new NamespaceDef("<tagfile>",1,name,tagName);
    namespaceList.inSort(nd);
    namespaceDict.insert(name,nd);
  }
  cd = 0;
  fd = 0;
}

static void addMember(const char *name,const char *anchor,const char *args)
{
  //printf("adding member `%s' `%s'\n",name,anchor);
  if (cd || fd)
  {
    MemberNameDict *mnd=0;
    MemberNameList *mnl=0;
    MemberDef *md;
    ArgumentList *argList = new ArgumentList;
    stringToArgumentList(args,argList);
    md=new MemberDef("<tagfile>",1,
                       0,name,args,0,Public,Normal,FALSE,FALSE,
                       MemberDef::Function,0,argList); 
    delete argList;
    md->setAnchor(anchor);
    md->setReference(tagName);
    if (cd) // member of a class
    {
      md->setMemberClass(cd);
      cd->insertMember(md,-1); /* TODO: store group info */
      mnd=&memberNameDict;
      mnl=&memberNameList;
    }
    else if (nd) // member of a namespace
    {
      md->setNamespace(nd);
      nd->insertMember(md,-1); /* TODO: store group info */
      mnd=&functionNameDict;
      mnl=&functionNameList;
    }
    else // member of a file
    {
      md->setFileDef(fd);
      fd->insertMember(md,-1); /* TODO: store group info */
      mnd=&functionNameDict;
      mnl=&functionNameList;
    }
    MemberName *mn = 0;
    if ((mn=(*mnd)[memberName]))
    {
      //printf("mn->inSort()\n");
      mn->append(md);
    }
    else
    {
      //printf("mn->append()\n");
      mn=new MemberName(memberName);
      mn->append(md);
      //printf("Adding memberName=%s\n",mn->memberName());
      mnl->inSort(mn);
      mnd->insert(memberName,mn);
    }
  }
}

/* -----------------------------------------------------------------
 */

%}

ID [a-z_A-Z0-9]+
FILE [a-z_A-Z0-9\.\-\+\:\\\/]+
SCOPE ({ID}"::")*{ID}

%option noyywrap

%x Pass1
%x Pass2
%x AnchorName
%x ArgString1
%x ArgString2
%x ClassName1
%x ClassName2
%x FileName
%x NamespaceName
%x BaseClasses
%x ClassFile1
%x ClassFile2

%%

<Pass1>^">"				{ // start of a class
  					  BEGIN(ClassName1);
  					}
<Pass1>^"&"				{ // start of a file
  					  BEGIN(FileName);
  					}
<Pass1>^"%"				{ // start of a namespace
  					  BEGIN(NamespaceName);
  					}
<Pass1>^[~a-z_A-Z][^ \n]*/" "		{
  					  memberName=yytext;
					  BEGIN(AnchorName);
  					}
<Pass2>^">"				{
  					  BEGIN(ClassName2);
  					}
<AnchorName>{ID}			{
  					  anchorName=yytext;
					  BEGIN(ArgString1);
  					}
<ArgString1>"\""			{
  					  BEGIN(ArgString2);
  					}
<ArgString2>[^\"\n]*/"\""		{
  					  argString=yytext;
					  addMember(memberName,anchorName,argString);
					  BEGIN(Pass1);
  					}
<FileName>{FILE}/":"			{
  					  fileName=yytext;
					  addFile(yytext);
					  BEGIN(Pass1);
  					}
<NamespaceName>{SCOPE}/":"		{
  					  namespaceName=yytext;
					  addNamespace(yytext);
					  BEGIN(Pass1);
  					}
<ClassName1>{SCOPE}/":"			{
  					  className=yytext;		  
					  BEGIN(ClassFile1);
  					}
<ClassFile1>\"				{
  					  BEGIN(ClassFile2);
  					}
<ClassFile2>[^\"]*/\"			{
  					  addClass(className,yytext); 
					  BEGIN(Pass1);
  					}
<ClassFile2>\n				{
  					  yyLineNr++;
					  BEGIN(Pass1);
  					}
<ClassName2>{ID}/":"			{
  					  cd=getClass(yytext);
					  BEGIN(BaseClasses);
  					}
<BaseClasses>{ID}/"?"			{
  					  ClassDef *bcd=getClass(yytext);
					  if (cd && bcd)
					  {
					    cd->insertBaseClass(bcd,Public,Normal);
					    bcd->insertSuperClass(cd,Public,Normal);
					  }
  					}
<BaseClasses>\n				{
  					  yyLineNr++;
  					  BEGIN(Pass2);
  					}
<*>.					
<*>\n					{ yyLineNr++ ; }

%%

/*@ ----------------------------------------------------------------------------
 */

void parseTagFile(const char *fileName)
{
  FILE *f=fopen(fileName,"r");
  QFileInfo fi(fileName);
  if (!f || !fi.exists()) 
  {
    if (f) fclose(f);
    return;
  }
  tagName = fi.fileName();
  tagYYin = f;

  cd=0;
  yyLineNr      = 1;
  tagYYrestart( tagYYin );
  BEGIN(Pass1);
  tagYYlex();

  rewind(f);
  cd=0;
  yyLineNr      = 1;
  tagYYrestart( tagYYin );
  BEGIN(Pass2);
  tagYYlex();

  fclose(f);
}

//extern "C" { // some bogus code to keep the compiler happy
//  int  tagYYwrap() { return 1 ; }
//}
