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

%{

/*
 *	includes
 */
#include <stdio.h>
#include <qstring.h>
#include <qfileinf.h>
  
#include "classdef.h"
#include "filedef.h"
#include "memberdef.h"
#include "doxygen.h"
#include "util.h"
#include "message.h"
  
#define YY_NO_UNPUT
#define YY_NEVER_INTERACTIVE 1

static int yyLineNr;
static QString className;
static QString fileName;
static QString tagName;
static QString memberName;
static QString anchorName;
static QString argString;
static ClassDef *cd;
static FileDef *fd;

static void addClass(const char *name,const char *fileName)
{
  //printf("adding class %s\n",name);
  if (name!=0 && strlen(name)>0 && classDict[name]==0)
  {
    cd = new ClassDef(name,ClassDef::Class,tagName,fileName);
    fd = 0;
    classList.inSort(cd);
    classDict.insert(className,cd);
  }
}

static void addFile(const char *name)
{
  //printf("adding file %s\n",name);
  fd = new FileDef(0,name,tagName);
  FileName *mn;
  if ((mn=inputNameDict[name]))
  {
    mn->append(fd); 
  }
  else
  {
    mn = new FileName(name);
    mn->append(fd);
    inputNameList.inSort(mn);
    inputNameDict.insert(name,mn);
  }
  cd = 0;
  //fileList.inSort(fd);
  //fileDict.insert(fileName,fd);
}

static void addMember(const char *name,const char *anchor,const char *args)
{
  //printf("adding member %s\n",name);
  if (cd || fd)
  {
    MemberNameDict *mnd=0;
    MemberNameList *mnl=0;
    MemberDef *md;
    md=new MemberDef(0,name,args,0,Public,Normal,FALSE,FALSE,
                       MemberDef::Function,0,0); 
    md->setReference(anchor);
    if (cd)
    {
      //md=new MemberDef(cd,name,args,anchor,tagName);
      md->setMemberClass(cd);
      cd->insertMember(md); 
      //printf("Adding member %s %s to class\n",name,args);
      mnd=&memberNameDict;
      mnl=&memberNameList;
    }
    else
    {
      //md=new MemberDef(&unrelatedClass,name,args,anchor,tagName);
      md->setFileDef(fd);
      fd->insertMember(md);
      //printf("Adding global member %s %s\n",name,args);
      mnd=&functionNameDict;
      mnl=&functionNameList;
    }
    MemberName *mn;
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

ID [a-z_A-Z][a-z_A-Z0-9]*
FILE [a-z_A-Z0-9\.\-\+\:\\\/]+

%x Pass1
%x Pass2
%x AnchorName
%x ArgString1
%x ArgString2
%x ClassName1
%x ClassName2
%x FileName
%x BaseClasses
%x ClassFile1
%x ClassFile2

%%

<Pass1>^">"				{
  					  BEGIN(ClassName1);
  					}
<Pass1>^"&"				{
  					  BEGIN(FileName);
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
<ClassName1>{ID}/":"			{
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
  if (!f || !fi.exists());
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

extern "C" { // some bogus code to keep the compiler happy
  int  tagYYwrap() { return 1 ; }
}
