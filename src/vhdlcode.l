/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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
/******************************************************************************
 * Parser for syntax highlighting and references for vhdl subset
 * written by M. Kreis
 * supports VHDL-87/93/2008
 ******************************************************************************/
%option never-interactive
%option case-insensitive
%option prefix="vhdlcodeYY"
%option reentrant
%option extra-type="struct vhdlcodeYY_state *"
%top{
#include <stdint.h>
}

%{

#include <unordered_set>
#include <string>

/*
 *        includes
 */
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "vhdlcode.h"
#include "entry.h"
#include "doxygen.h"
#include "message.h"
#include "outputlist.h"
#include "util.h"
#include "membername.h"
#include "searchindex.h"
#include "vhdldocgen.h"
#include "arguments.h"
#include "config.h"
#include "classdef.h"
#include "filedef.h"
#include "tooltip.h"
#include "regex.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

#define USE_STATE2STRING 0

// Toggle for some debugging info
//#define DBG_CTX(x) fprintf x
#define DBG_CTX(x) do { } while(0)


/* -----------------------------------------------------------------
 *        statics
 */

// ----------------- <vhdl> ----------------------------------

struct vhdlcodeYY_state
{
  bool          isFuncProto = false;
  bool          isComponent = false;
  bool          isPackageBody = false;
  bool          isProto = false;
  bool          startCode = false;
  QCString      prevString;
  QCString      currClass;
  std::unordered_set<std::string> vhdlKeyDict;
  QCString      tempComp;
  QCString      PortMapComp;
  const MemberDef *   vhdlMember = 0;
  QCString      funcProto;

  CodeOutputInterface * code = 0;
  const char *  inputString = 0;     //!< the code fragment as text
  yy_size_t     inputPosition = 0;   //!< read offset during parsing
  int           inputLines = 0;      //!< number of line in the code fragment
  int           yyLineNr = 0;        //!< current line number
  bool          needsTermination = false;
  const Definition *searchCtx = 0;

  bool          exampleBlock = false;
  QCString      exampleName;
  QCString      exampleFile;

  bool          currArch = false;

  const FileDef *     sourceFileDef = 0;
  const Definition *  currentDefinition = 0;
  const MemberDef *   currentMemberDef = 0;
  bool          includeCodeFragment = false;
  const char *  currentFontClass = 0;

  bool          lexInit = false;
  int           braceCount = 0;
};


static void writeFont(yyscan_t yyscanner,const char *s,const char* text);
static void generateMemLink(yyscan_t yyscanner,CodeOutputInterface &ol,QCString &clName,QCString& memberName);
static bool writeColoredWord(yyscan_t yyscanner,QCString& word );
static void generateClassOrGlobalLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *clName, bool typeOnly=false, const char *curr_class=0);
static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor);
static bool checkVhdlString(yyscan_t yyscanner,QCString &name);
static void addToSearchIndex(yyscan_t yyscanner,const char *text);
static void startCodeLine(yyscan_t yyscanner);
static void endCodeLine(yyscan_t yyscanner);
static void nextCodeLine(yyscan_t yyscanner);
static void writeWord(yyscan_t yyscanner,const char *word,const char* curr_class=0,bool classLink=false);
static void codifyLines(yyscan_t yyscanner,const char *text,const char *cl=0,bool classlink=false,bool comment=false);
static void writeMultiLineCodeLink(yyscan_t yyscanner,CodeOutputInterface &ol,
                  const Definition *d,
                  const char *text);
static void generateFuncLink(yyscan_t yyscanner,CodeOutputInterface &ol,const MemberDef* mdef);
static int  countLines(yyscan_t yyscanner);
static void endFontClass(yyscan_t yyscanner);
static void startFontClass(yyscan_t yyscanner,const char *s);
static void appStringLower(QCString& qcs,const char* text);
static void codifyMapLines(yyscan_t yyscanner,const char *text);
static void writeFuncProto(yyscan_t yyscanner);
static void writeProcessProto(yyscan_t yyscanner);
static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size);

#if USE_STATE2STRING
static const char *stateToString(int state);
#endif
//-------------------------------------------------------------------


#undef        YY_INPUT
#define       YY_INPUT(buf,result,max_size) result=yyread(yyscanner,buf,max_size);

%}


B             [ \t]
BN            [ \t\n\r]
STRING      ["][^"\n]*["]
NAME          [a-z_A-Z][ a-z_A-Z0-9]*
FUNCNAME      [a-z_A-Z"][a-z_A-Z0-9+*"/=<>-]*
ID            "$"?[a-z_A-Z][a-z_A-Z0-9]*
SPECSIGN      [:;, +*&\/=<>'\t]*
DIGITSS       [0-9]+|[0-9]+("#")*[0-9_a-fA-F\+\.\-]+("#")*
ALLTYPESMAP   {B}*[_a-zA-Z0-9. ]+{BN}*
ALLTYPESMAP1  {BN}*[_a-zA-Z0-9.() ]+{BN}*

ARCHITECTURE  ^{B}*("architecture"){BN}+{FUNCNAME}{BN}+("of"){BN}+{FUNCNAME}{BN}+("is")
PROCESS       ({BN}*{FUNCNAME}{BN}*[:]+{BN}*("process"){BN}*[(]*)|[^a-zA-Z]("process "|"process("){BN}*[ (]*|[^a-zA-Z]("process"){BN}+

END1          {B}*("end "){BN}+("if"|"case"|"loop"|"generate"|"for")
END2          [^a-zA-Z_]("end"){BN}*[;]
END3          {BN}*[^a-zA-Z]("end"){BN}+{FUNCNAME}{BN}*[;]
END4          {B}*("end"){BN}+"function"{BN}+{FUNCNAME}{BN}*[;]
ENDEFUNC      {END3}|{END4}|{END2}

KEYWORD       ("of"|"new"|"event"|"break"|"case"|"end"|"loop"|"else"|"for"|"goto"|"if"|"return"|"generate"|"is"|"while"|"in")
TYPEKW        ^{B}*("type"|"subtype"|"constant"|"attribute"|"signal"|"variable","alias","configuration")
FUNC          ^{B}*("function"|"procedure"){BN}*{FUNCNAME}{BN}*("(")

ARITHOP       "+"|"-"|"/"|"*"|"%"|"/="|":="
ASSIGNOP      "="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^="|"|="
LOGICOP       "=="|"!="|">"|"<"|">="|"<="|"&&"|"||"|"!"
BITOP         "&"|"|"|"^"|"<<"|">>"|"~"
OPERATOR      {ARITHOP}|{ASSIGNOP}|{LOGICOP}|{BITOP}

PORT          {B}*("port"){BN}*("(")
GENERIC       {B}*("generic"){BN}*("(")

BRACEOPEN     [(]{1}
BRACECLOSE    [)]{1}

TEXTT         {B}*"--"[^\n]*

MAPCOMPONENT1 ({ALLTYPESMAP}[:]{ALLTYPESMAP}{TEXTT}*{BN}+("port"|"generic"){BN}+("map"){BN}*("("){1})
MAPCOMPONENT2 {BN}*("port"|"generic"){BN}+("map"){BN}*("("){1}
MAPCOMPONENT3 ({ALLTYPESMAP}[:]{BN}*{ALLTYPESMAP1}{TEXTT}*{BN}+("port"|"generic"){BN}+("map"){BN}*("("){1})
MAPCOMPONENT4 ({ALLTYPESMAP}[:]{BN}*("entity"|"component"|"configuration"){BN}+{ALLTYPESMAP1}{TEXTT}*{BN}*("port"|"generic"){BN}*("map"){BN}*("("){1})

XILINX      "INST"|"NET"|"PIN"|"BLKNM"|"BUFG"|"COLLAPSE"|"CPLD"|"COMPGRP"|"CONFIG"|"CONFIG_MODE"|"COOL_CLK"|"DATA_GATE"|"DCI_VALUE"|"DISABLE"|"DRIVE"|"DROP_SPEC"|"ENABLE"|"FAST"|"FEEDBACK"|"FILE"|"FLOAT"|"FROM-THRU-TO"|"FROM-TO"|"HBLKNM"|"HU_SET"|"INREG"|"IOB"|"IOBDELAY"|"IOSTANDARD"|"KEEP"|"KEEPER"|"LOC"|"LOCATE"|"LOCK_PINS"|"MAP"|"MAXDELAY"|"MAXPT"|"MAXSKEW"|"NODELAY"|"NOREDUCE"|"OFFSET"|"OPEN_DRAIN"|"OPT_EFFORT"|"OPTIMIZE"|"PERIOD"|"PIN"|"PRIORITY"|"PROHIBIT"|"PULLDOWN"|"PULLUP"|"PWR_MODE"|"REG"|"RLOC"|"RLOC_ORIGIN"|"RLOC_RANGE"|"SAVE NET"|"FLAG"|"SYSTEM_JITTER"|"TEMPERATURE"|"TIMEGRP"|"TIMESPEC"|"VOLTAGE"

%option noyywrap
%option nounput

%x Bases
%x ParseType
%x ParseFuncProto
%x ParseComponent
%x ParsePackage
%x ParseProcessProto
%x ClassName
%x PackageName
%x ClassVar
%x ClassesName
%x Map
%x End
%x Body

%%

.                                   {
                                      BEGIN(Bases);
                                    }

<Map>{BRACEOPEN}                    {
                                      yyextra->braceCount++;
                                      writeFont(yyscanner,"vhdlchar",yytext);
                                      BEGIN(Map);
                                    }

<Map>[^()\n,--]*                    { /* write and link a port map lines */
                                      QCString tt(yytext);
                                      VhdlDocGen::deleteAllChars(tt,',');
                                      auto ql = split(tt.str(),"=>");
                                      if (ql.size()>=2)
                                      {
                                        unsigned int index=0;
                                        QCString t1=ql[0];
                                        char cc=t1.at(index);
                                        while (cc==' ' || cc=='\t')
                                        {
                                          char c2[2];
                                          c2[0]=cc;
                                          c2[1]=0;
                                          yyextra->code->codify(c2);
                                          index++;
                                          if (index>=t1.size()) break;
                                          cc=t1.at(index);
                                        }

                                        QCString s1=t1;
                                        s1=s1.stripWhiteSpace();

                                        //         if (!yyextra->PortMapComp.isEmpty())
                                        generateMemLink(yyscanner,*yyextra->code,yyextra->PortMapComp,s1);
                                        while (index++<t1.size())
                                        {
                                          cc=t1.at(index);
                                          if (cc==' ' || cc=='\t')
                                          {
                                            char c2[2];
                                            c2[0]=cc;
                                            c2[1]=0;
                                            yyextra->code->codify(c2);
                                          }
                                        }
                                        codifyLines(yyscanner,"=>");
                                        index=0;
                                        QCString s2=ql[1];
                                        t1=s2;
                                        cc=t1.at(index);
                                        while (cc==' ' || cc=='\t')
                                        {
                                          char c2[2];
                                          c2[0]=cc;
                                          c2[1]=0;
                                          yyextra->code->codify(c2);
                                          index++;
                                          if (index>=t1.size()) break;
                                          cc=t1.at(index);
                                        }
                                        s2=s2.stripWhiteSpace();
                                        if (!checkVhdlString(yyscanner,s2))
                                        {
                                          generateMemLink(yyscanner,*yyextra->code,yyextra->currClass,s2);
                                        }
                                        while (index++<t1.size())
                                        {
                                          if (t1.at(index)==' ')
                                          {
                                            yyextra->code->codify(" ");
                                          }
                                        }
                                      }
                                      else
                                      {
                                        codifyLines(yyscanner,yytext,yyextra->currClass.data());
                                      }
                                      BEGIN(Map);
                                    }

<Map>"\n"|","                       {
                                      codifyLines(yyscanner,yytext);
                                      BEGIN(Map);
                                    }

<Map>{BRACECLOSE}                   {
                                      yyextra->braceCount--;
                                      writeFont(yyscanner,"vhdlchar",yytext);
                                      if (yyextra->braceCount==0)
                                      {
                                        BEGIN(Bases);
                                      }
                                    }

<ParseFuncProto>{NAME}              {
                                      QCString tmp(yytext);
                                      tmp=tmp.stripWhiteSpace();
                                      appStringLower(yyextra->prevString,yytext);
                                      yyextra->vhdlKeyDict.insert(yyextra->prevString.str());
                                      if (!writeColoredWord(yyscanner,tmp))
                                      {
                                        generateMemLink(yyscanner,*yyextra->code,yyextra->currClass,tmp);
                                      }
                                      BEGIN(Bases);
                                    }

<ParseType>{STRING}                 {
                                      QCString qcs(yytext);
                                      VhdlDocGen::deleteAllChars(qcs,'"');
                                      VhdlDocGen::deleteAllChars(qcs,' ');
                                      if (VhdlDocGen::isNumber(qcs.str()))
                                      {
                                        writeFont(yyscanner,"vhdllogic",yytext);
                                      }
                                      else
                                      {
                                        writeFont(yyscanner,"keyword",yytext);
                                      }
                                    }

<ParseType>"\n"                     {
                                      yyextra->funcProto.append(yytext);
                                      if (yyextra->isProto)
                                      {
                                        codifyLines(yyscanner,yytext);
                                      }
                                      BEGIN(ParseType);
                                    }


<ParseType>{TEXTT}                  {
                                      yyextra->funcProto.append(yytext);
                                      if (yyextra->isProto)
                                      {
                                        writeFont(yyscanner,"keyword",yytext);
                                      }
                                      BEGIN(ParseType);
                                    }

<ParseType>{ENDEFUNC}               {
                                      QCString tt(yytext);
                                      codifyLines(yyscanner,yytext,yyextra->currClass.data());
                                      tt=tt.lower();
                                      VhdlDocGen::deleteAllChars(tt,';');
                                      tt.stripWhiteSpace();
                                      static const reg::Ex regg(R"(\s+)"); // any number of whitespace
                                      auto ql = split(tt.str(),regg);
                                      int index=findIndex(ql,"if")+1;
                                      index+=findIndex(ql,"case")+1;
                                      index+=findIndex(ql,"loop")+1;
                                      index+=findIndex(ql,"generate")+1;
                                      if (index==0)
                                      {
                                        BEGIN(Bases);
                                      }
                                      else
                                      {
                                        BEGIN(ParseType);
                                      }
                                    }

<ParseType>{END1}                   {
                                      codifyLines(yyscanner,yytext,yyextra->currClass.data());
                                      yyextra->vhdlKeyDict.clear();
                                    }

<ParseType>^{B}*("begin "|"begin")  {
                                      codifyLines(yyscanner,yytext,yyextra->currClass.data());
                                      yyextra->isFuncProto=false;
                                    }

<ParseType>{SPECSIGN}               {
                                      yyextra->funcProto.append(yytext);
                                      if (yyextra->isProto)
                                      {
                                        codifyLines(yyscanner,yytext,yyextra->currClass.data());
                                      }
                                    }

<ParseType>["_a-zA-Z0-9]*           {
                                      QCString val(yytext);
                                      yyextra->funcProto.append(yytext);
                                      appStringLower(yyextra->prevString,yytext);

                                      if (yyextra->isFuncProto && yyextra->braceCount==0)
                                      {
                                        yyextra->vhdlKeyDict.insert(yyextra->prevString.str());
                                      }

                                      if (yyextra->isProto)
                                      {
                                        if (!writeColoredWord(yyscanner,val))
                                        {
                                          if (!yyextra->isFuncProto &&
                                              yyextra->vhdlKeyDict.find(yyextra->prevString.str())==yyextra->vhdlKeyDict.end())
                                          {
                                            val=val.stripWhiteSpace();
                                            if (VhdlDocGen::isNumber(val.str()))
                                            {
                                              startFontClass(yyscanner,"vhdllogic");
                                              codifyLines(yyscanner,yytext,yyextra->currClass.data());
                                              endFontClass(yyscanner);
                                            }
                                            else
                                            {
                                              generateMemLink(yyscanner,*yyextra->code,yyextra->currClass,val);
                                            }
                                          }
                                          else
                                          {
                                            codifyLines(yyscanner,yytext,yyextra->currClass.data());
                                          }
                                        }
                                      }
                                      BEGIN(ParseType);
                                    }

<ParseType>{BRACEOPEN}              {
                                      yyextra->braceCount++;
                                      yyextra->funcProto+='(';
                                      if (yyextra->isProto)
                                      {
                                        writeFont(yyscanner,"vhdlchar",yytext);
                                      }
                                      BEGIN(ParseType);
                                    }

<ParseType>{BRACECLOSE}             {
                                      yyextra->braceCount--;
                                      yyextra->funcProto+=')';
                                      if (yyextra->isProto)
                                      {
                                        writeFont(yyscanner,"vhdlchar",yytext);
                                      }
                                      if (yyextra->braceCount==0 && !yyextra->isProto)// && !yyextra->isPackageBody)
                                      {
                                        yyextra->isProto=true;
                                        appStringLower(yyextra->prevString,yytext);
                                        writeFuncProto(yyscanner);
                                        BEGIN(Bases);
                                      }
                                      if (yyextra->isPackageBody)
                                      {
                                        BEGIN(ParseType);
                                      }
                                    }


<ClassesName>{FUNCNAME}             {
                                      appStringLower(yyextra->prevString,yytext);
                                      yyextra->currClass.resize(0);
                                      yyextra->currClass.append(yytext);
                                      yyextra->currClass=yyextra->currClass.stripWhiteSpace();

                                      generateClassOrGlobalLink(yyscanner,*yyextra->code,yytext);
                                      BEGIN(Bases);
                                    }


<ParseComponent>{BRACEOPEN}         {
                                      yyextra->braceCount++;
                                      yyextra->code->codify(yytext);
                                    }


<ParseComponent>{BRACECLOSE}        {
                                      yyextra->braceCount--;
                                      yyextra->code->codify(yytext);
                                      if (yyextra->braceCount==0 && !yyextra->isComponent)
                                      {
                                        yyextra->tempComp.resize(0);
                                        BEGIN(Bases);
                                      }
                                      else
                                      {
                                        BEGIN(ParseComponent);
                                      }
                                    }

<ParseComponent>{B}*"-"             {
                                      if (strlen(yytext)>=2) // found text ?
                                      {
                                        writeFont(yyscanner,"keyword",yytext);
                                      }
                                      else
                                      {
                                        writeFont(yyscanner,"vhdlchar",yytext);
                                      }
                                    }

<ParseComponent>{SPECSIGN}          {
                                      codifyLines(yyscanner,yytext);
                                    }



<ParseComponent>"\n"|" "            {
                                      codifyLines(yyscanner,yytext);
                                    }

<ParseComponent>{DIGITSS}           {
                                      startFontClass(yyscanner,"vhdllogic");
                                      codifyLines(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                    }

<ParseComponent>{PORT}              {
                                      codifyLines(yyscanner,yytext);
                                      yyextra->braceCount=1;
                                      yyextra->isComponent=false;
                                    }

<ParseComponent>{GENERIC}           {
                                      codifyLines(yyscanner,yytext);
                                      yyextra->braceCount=1;
                                    }

<ParseComponent>[_a-zA_Z][_a-zA-Z0-9]*  {
                                      QCString temp(yytext);
                                      appStringLower(yyextra->prevString,yytext);
                                      if (!checkVhdlString(yyscanner,temp))
                                      {
                                        if (!writeColoredWord(yyscanner,yyextra->prevString))
                                        {
                                          generateMemLink(yyscanner,*yyextra->code,yyextra->tempComp,temp);
                                        }
                                      }
                                    }

<ParseComponent>{STRING}            {
                                      QCString temp(yytext);
                                      if (!checkVhdlString(yyscanner,temp))
                                      {
                                        codifyLines(yyscanner,yytext);
                                      }
                                    }


<ParseProcessProto>[^()]*           {
                                      yyextra->funcProto.append(yytext);
                                    }



<ParseProcessProto>{BRACEOPEN}      {
                                      yyextra->funcProto.append(yytext);
                                      yyextra->braceCount++;
                                    }

<ParseProcessProto>{BRACECLOSE}     {
                                      yyextra->funcProto.append(yytext);
                                      yyextra->braceCount--;
                                      if (yyextra->braceCount==0)
                                      {
                                        writeProcessProto(yyscanner);
                                        BEGIN(Bases);
                                      }
                                    }

<ParsePackage>[^:;]*                { //found package
                                      StringVector strl=split(yytext,".");
                                      if (strl.size()>2)
                                      {
                                        std::string s1=strl[0];
                                        std::string s2=strl[1];
                                        std::string s3=strl[2];
                                        s1.append(".");
                                        s3.insert(0,".");
                                        codifyLines(yyscanner,s1.c_str(),yyextra->currClass.data());
                                        ClassDef *cd=VhdlDocGen::getPackageName(s2.c_str());
                                        if (cd)
                                        {
                                          generateClassOrGlobalLink(yyscanner,*yyextra->code,s2.c_str());
                                        }
                                        else
                                        {
                                          codifyLines(yyscanner,s2.c_str());
                                        }
                                        codifyLines(yyscanner,s3.c_str());
                                      }
                                      else
                                      {
                                        writeFont(yyscanner,"keywordflow",yytext);
                                      }
                                      BEGIN(Bases);
                                    }

<Bases>{MAPCOMPONENT1}|{MAPCOMPONENT2}|{MAPCOMPONENT3}|{MAPCOMPONENT4}  { // found port or generic map
                                      QCString tt(yytext);
                                      int j=tt.find('.');

                                      if (j>0)
                                      {
                                        QCString left=tt.left(j+1);
                                        codifyLines(yyscanner,left.data());
                                        tt=tt.right(tt.length()-j-1);
                                        left=VhdlDocGen::getIndexWord(tt.data(),0);
                                        if (!left.isEmpty())
                                        {
                                          if (left.contains('('))
                                          {
                                            j=left.find('(',false);
                                            QCString name=left.left(j);
                                            generateClassOrGlobalLink(yyscanner,*yyextra->code,name.data());
                                            yyextra->PortMapComp=name;
                                            name=tt.right(tt.length()-name.length());
                                            codifyLines(yyscanner,name.data());
                                          }
                                          else
                                          {
                                            generateClassOrGlobalLink(yyscanner,*yyextra->code,left.data());
                                            tt.stripPrefix(left.data()); //=tt.right(tt.length()-left.length()-1);

                                            yyextra->PortMapComp=left;
                                            codifyLines(yyscanner,tt.data());
                                          }
                                        }
                                      }
                                      else
                                      {
                                        if (tt.contains(':',false))
                                        {
                                          codifyMapLines(yyscanner,tt.data());
                                        }
                                        else
                                        {
                                          codifyLines(yyscanner,tt.data());
                                        }
                                      }
                                      yyextra->braceCount=1;
                                      BEGIN(Map);
                                    }

<Bases>^{B}*("component"){BN}+{FUNCNAME}  { // found component
                                      appStringLower(yyextra->prevString,yytext);
                                      QCString temp=VhdlDocGen::getIndexWord(yytext,1);
                                      temp=temp.stripWhiteSpace();
                                      VhdlDocGen::deleteAllChars(temp,'\n');
                                      yyextra->tempComp=temp;
                                      codifyLines(yyscanner,yytext,temp.data(),true);
                                      yyextra->braceCount=0;
                                      yyextra->isComponent=true;
                                      BEGIN(ParseComponent);
                                    }



<Bases>{ARCHITECTURE}               { // found architecture
                                      yyextra->PortMapComp.resize(0);
                                      QCString temp = VhdlDocGen::getIndexWord(yytext,3);
                                      yyextra->currArch = true;
                                      temp+="::";
                                      temp+=VhdlDocGen::getIndexWord(yytext,1);
                                      yyextra->currClass=temp;
                                      VhdlDocGen::deleteAllChars(temp,'\n');
                                      codifyLines(yyscanner,yytext,temp.data(),true);
                                      yyextra->isPackageBody=false;
                                    }


<Bases>^{B}*("package "){BN}*("body"){BN}*{FUNCNAME}  { // found package body
                                      QCString ss(yytext);
                                      QCString temp=VhdlDocGen::getIndexWord(yytext,2);
                                      StringVector ql=split(yytext,temp.str());
                                      std::string ll=ql[0];
                                      codifyLines(yyscanner,ll.c_str(),yyextra->currClass.data());
                                      temp=temp.stripWhiteSpace();
                                      temp.prepend("_");
                                      generateClassOrGlobalLink(yyscanner,*yyextra->code,temp.data());
                                      yyextra->currClass.resize(0);
                                      yyextra->currClass=temp;
                                      yyextra->isProto=false;
                                      yyextra->isPackageBody=true;
                                    }

<Bases>{PROCESS}                    { // found process
                                      yyextra->isFuncProto=true;
                                      yyextra->funcProto.resize(0);
                                      yyextra->funcProto.append(yytext);
                                      yyextra->vhdlKeyDict.clear();
                                      appStringLower(yyextra->prevString,yytext);
                                      if (yyextra->prevString.contains('('))
                                      {
                                        yyextra->braceCount=1;
                                        BEGIN(ParseProcessProto);
                                      }
                                      else
                                      {
                                        writeProcessProto(yyscanner);
                                      }
                                    }

<Bases>("end"){BN}+("process")      { // end of process
                                      yyextra->isFuncProto=false;
                                      codifyLines(yyscanner,yytext);
                                      BEGIN(Bases);
                                    }


<Bases>^{B}*("begin "|"begin")      {
                                      yyextra->isFuncProto=false;
                                      writeFont(yyscanner,"vhdlkeyword",yytext);
                                    }

<Bases>^{B}*("use"|"library"){BN}+  { //found package or library
                                      writeFont(yyscanner,"vhdlkeyword",yytext);
                                      BEGIN(ParsePackage);
                                    }


<Bases>^{B}*("use"){BN}+("configuration")[^\n]* {
                                      codifyLines(yyscanner,yytext);
                                    }

<Bases>{FUNC}                       {  // found function|procedure
                                      yyextra->vhdlKeyDict.clear();
                                      yyextra->funcProto.resize(0);
                                      yyextra->isProto=false;
                                      yyextra->funcProto.append(yytext);
                                      yyextra->braceCount=1;
                                      BEGIN(ParseType);
                                    }

<Bases>^{B}*("entity"|"package"){BN}+ {
                                      appStringLower(yyextra->prevString,yytext);
                                      writeFont(yyscanner,"keywordflow",yytext);
                                      yyextra->isPackageBody=false;
                                      BEGIN(ClassesName);
                                    }

<Bases>"end"{BN}+"architecture"{BN}+{FUNCNAME} {
                                      codifyLines(yyscanner,yytext,yyextra->currClass.data(),true);
                                      yyextra->currArch = false;
                                    }
<Bases>"end"{BN}+{FUNCNAME}         {
                                      if (yyextra->currArch)
                                      {
                                        codifyLines(yyscanner,yytext,yyextra->currClass.data(),true);
                                        yyextra->currArch = false;
                                      }
                                      else
                                      {
                                        REJECT;
                                      }
                                    }
<Bases>"end"                        {
                                       appStringLower(yyextra->prevString,yytext);
                                       QCString temp(yytext);
                                       temp=temp.stripWhiteSpace();

                                       writeColoredWord(yyscanner,temp);
                                       BEGIN(End);
                                    }
<End>{ID}                           {
                                      appStringLower(yyextra->prevString,yytext);
                                      QCString temp(yytext);
                                      temp=temp.stripWhiteSpace();

                                      if (!writeColoredWord(yyscanner,temp))
                                      {
                                        generateClassOrGlobalLink(yyscanner,*yyextra->code,temp.data());
                                      }
                                    }
<End>";"                            {
                                      codifyLines(yyscanner,yytext);
                                      BEGIN(Bases);
                                    }
<Bases>{KEYWORD}                    { // found keyword
                                      QCString qcs(yytext);
                                      if (!writeColoredWord(yyscanner,qcs))
                                      {
                                        startFontClass(yyscanner,"vhdlchar");
                                        yyextra->code->codify(yytext);
                                        endFontClass(yyscanner);
                                      }
                                    }


<Bases>{ID}                         {
                                      appStringLower(yyextra->prevString,yytext);
                                      QCString temp(yytext);
                                      temp=temp.stripWhiteSpace();

                                      if (!writeColoredWord(yyscanner,temp))
                                      {
                                        startFontClass(yyscanner,"vhdlchar");
                                        generateMemLink(yyscanner,*yyextra->code,yyextra->currClass,temp);
                                        endFontClass(yyscanner);
                                      }
                                    }

<Bases,ParseComponent>{DIGITSS}     {
                                      startFontClass(yyscanner,"vhdllogic");
                                      codifyLines(yyscanner,yytext);
                                      endFontClass(yyscanner);
                                    }

<Bases>^{B}*("use"){BN}+("entity"|"component")[^\n]* {
                                      codifyLines(yyscanner,yytext,yyextra->currClass.data(),true);
                                    }


<Bases>{TYPEKW}                     {
                                      codifyLines(yyscanner,yytext);
                                      if (yyextra->isFuncProto)
                                      {
                                        BEGIN(ParseFuncProto);
                                      }
                                      else
                                      {
                                        BEGIN(Bases);
                                      }
                                    }

<Bases>{OPERATOR}                   {
                                      startFontClass(yyscanner,"vhdlchar");
                                      yyextra->code->codify(yytext);
                                      endFontClass(yyscanner);
                                    }

<Bases>","|"."|":"|"'"|"("|")"      {
                                      startFontClass(yyscanner,"vhdlchar");
                                      yyextra->code->codify(yytext);
                                      endFontClass(yyscanner);
                                    }

<Bases>{STRING}                     {
                                      QCString qcs(yytext);
                                      VhdlDocGen::deleteAllChars(qcs,'"');
                                      VhdlDocGen::deleteAllChars(qcs,' ');

                                      if (VhdlDocGen::isNumber(qcs.str()))
                                      {
                                        writeFont(yyscanner,"vhdllogic",yytext);
                                      }
                                      else
                                      {
                                        writeFont(yyscanner,"keyword",yytext);
                                      }
                                    }

<Bases>{B}*"#"[^\n]*                {
                                      writeFont(yyscanner,"keyword",yytext);
                                    }

<Bases>^{B}*{XILINX}/[^a-zA-Z0-9_]  {
                                      writeWord(yyscanner,yytext);
                                      //codifyLines(yyscanner,yytext,yyextra->currClass.data(),true);
                                    }

<Bases>^{B}*"set_"[^\n]*            {
                                      writeWord(yyscanner,yytext);
                                    }

<*>\n                               {
                                      codifyLines(yyscanner,yytext);
                                      BEGIN(Bases);
                                    }

<*>[\x80-\xFF]*                     { // keep utf8 characters together...
                                      yyextra->code->codify(yytext);
                                    }
<*>.                                {
                                      yyextra->code->codify(yytext);
                                    }

<*>\n{TEXTT}                        { // found normal or special comment on its own line
                                      QCString text(yytext);
                                      int i=text.find("--");
                                      if (text.mid(i,3)=="--!") // && // hide special comment
                                      {
                                        if (!Config_getBool(STRIP_CODE_COMMENTS))
                                        {
                                          codifyLines(yyscanner,text,0,false,true);
                                        }
                                        else yyextra->yyLineNr++; // skip complete line, but count line
                                      }
                                      else // normal comment
                                      {
                                        codifyLines(yyscanner,text,0,false,true);
                                      }
                                    }
<*>{TEXTT}                          { // found normal or special comment after something
                                      QCString text(yytext);
                                      int i=text.find("--");
                                      if (text.mid(i,3)=="--!")
                                      {
                                        // hide special comment
                                        if (!Config_getBool(STRIP_CODE_COMMENTS))
                                        {
                                          codifyLines(yyscanner,text,0,false,true);
                                        }
                                      }
                                      else // normal comment
                                      {
                                        codifyLines(yyscanner,text,0,false,true);
                                      }
                                    }

%%

/*@ ----------------------------------------------------------------------------
 */

static yy_size_t yyread(yyscan_t yyscanner,char *buf,yy_size_t max_size)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  yy_size_t inputPosition = yyextra->inputPosition;
  const char *s = yyextra->inputString + inputPosition;
  yy_size_t c=0;
  while( c < max_size && *s)
  {
    *buf++ = *s++;
    c++;
  }
  yyextra->inputPosition += c;
  return c;
}

static void setCurrentDoc(yyscan_t yyscanner,const QCString &anchor)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    if (yyextra->searchCtx)
    {
      yyextra->code->setCurrentDoc(yyextra->searchCtx,yyextra->searchCtx->anchor(),false);
    }
    else
    {
      yyextra->code->setCurrentDoc(yyextra->sourceFileDef,anchor,true);
    }
  }
}

static bool checkVhdlString(yyscan_t yyscanner,QCString &name)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (name.isEmpty()) return false;

  int len=name.length();
  if (name.at(0)=='"' && name.at(len-1)=='"' && len > 2)
  {
    std::string inside = name.str().substr(1,len-2);
    static const reg::Ex regg(R"(\s+)"); // any number of whitespace
    auto qrl=split(inside,regg);
    if (VhdlDocGen::isNumber(qrl[0]))
    {
      yyextra->code->codify("\"");
      startFontClass(yyscanner,"vhdllogic");
      yyextra->code->codify(inside.c_str());
      endFontClass(yyscanner);
      yyextra->code->codify("\"");
    }
    else
    {
      startFontClass(yyscanner,"keyword");
      yyextra->code->codify(name.data());
      endFontClass(yyscanner);
    }
    return true;
  }

  if (VhdlDocGen::isNumber(name.str()))
  {
    startFontClass(yyscanner,"vhdllogic");
    yyextra->code->codify(name.data());
    endFontClass(yyscanner);
    return true;
  }
  return false;
}

static void addToSearchIndex(yyscan_t yyscanner,const char *text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (Doxygen::searchIndex)
  {
    yyextra->code->addWord(text,false);
  }
}


/*! start a new line of code, inserting a line number if yyextra->sourceFileDef
 * is true. If a definition starts at the current line, then the line
 * number is linked to the documentation of that definition.
 */
static void startCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //if (yyextra->currentFontClass) { yyextra->code->endFontClass(); }
  if (yyextra->sourceFileDef)
  {
    //QCString lineNumber,lineAnchor;
    //lineNumber.sprintf("%05d",yyextra->yyLineNr);
    //lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
    //  if ((yyextra->yyLineNr % 500) == 0)
    //         fprintf(stderr,"\n starting Line %d:",yyextra->yyLineNr);
    const Definition *d = yyextra->sourceFileDef->getSourceDefinition(yyextra->yyLineNr);
    //printf("startCodeLine %d d=%s\n", yyextra->yyLineNr,d ? d->name().data() : "<null>");
    if (!yyextra->includeCodeFragment && d)
    {
      yyextra->currentDefinition = d;
      yyextra->currentMemberDef = yyextra->sourceFileDef->getSourceMember(yyextra->yyLineNr);
      if (!yyextra->tempComp.isEmpty() && yyextra->currentMemberDef )
      {
        //ClassDef *cf=VhdlDocGen::getClass(yyextra->tempComp.data());
        QCString nn=yyextra->currentMemberDef->name();
        const MemberDef* mdeff=VhdlDocGen::findMember(yyextra->tempComp,nn);
        if (mdeff)
        {
          yyextra->currentMemberDef=mdeff;
        }
      }

      QCString lineAnchor;
      lineAnchor.sprintf("l%05d",yyextra->yyLineNr);
      if (yyextra->currentMemberDef)
      {
        yyextra->code->writeLineNumber(yyextra->currentMemberDef->getReference(),
                                yyextra->currentMemberDef->getOutputFileBase(),
                                yyextra->currentMemberDef->anchor(),yyextra->yyLineNr);
        setCurrentDoc(yyscanner,lineAnchor);
      }
      else if (d->isLinkableInProject())
      {
        yyextra->code->writeLineNumber(d->getReference(),
                                d->getOutputFileBase(),
                                0,yyextra->yyLineNr);
        setCurrentDoc(yyscanner,lineAnchor);
      }
    }
    else
    {
      yyextra->code->writeLineNumber(0,0,0,yyextra->yyLineNr);
    }
  }
  yyextra->code->startCodeLine(yyextra->sourceFileDef);
  yyextra->startCode=true;
  if (yyextra->currentFontClass)
  {
    yyextra->code->startFontClass(yyextra->currentFontClass);
  }
}

static void endCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  endFontClass(yyscanner);
  yyextra->code->endCodeLine();
}

static void nextCodeLine(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->startCode)
  {
    endCodeLine(yyscanner);    // </div>
  }
  const char *fc = yyextra->currentFontClass;
  if (yyextra->yyLineNr<yyextra->inputLines)
  {
    yyextra->currentFontClass = fc;
    startCodeLine(yyscanner);  //<div>
  }
}

/*! writes a word to the output.
 *  If curr_class is defined, the word belongs to a class
 *  and will be linked.
 */

static void writeWord(yyscan_t yyscanner,const char *word,const char* curr_class,bool classLink)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  bool found=false;
  QCString temp;
  QCString tclass(curr_class);
  QCString ttt(word);
  if (ttt.isEmpty()) return;
  for (unsigned int j=0;j<ttt.length();j++)
  {
    char c=ttt.at(j);
    if (c==' '|| c==',' || c==';' || c==':' || c=='(' || c==')' || c=='\r' || c=='\t' || c=='.')
    {
      if (found)
      {
        if (!writeColoredWord(yyscanner,temp)) // is it a keyword ?
        {
          //if (VhdlDocGen::findKeyWord(temp))
          // writeFont(yyscanner,"vhdlkeyword",temp.data());
          //printf("writeWord: %s\n",temp.data());
          if (!tclass.isEmpty())
          {
            if (!classLink)
            {
              generateMemLink(yyscanner,*yyextra->code,tclass,temp);
            }
            else
            {
              generateClassOrGlobalLink(yyscanner,*yyextra->code,temp,false,curr_class);
            }
          }
          else
          {
            if (!checkVhdlString(yyscanner,temp))
            {
              yyextra->code->codify(temp.data());
            }
          }
        }
        temp.resize(0);
        found=false;
      }

      char cc[2];
      cc[0]=c;
      cc[1]=0;
      yyextra->code->codify(cc);
    }
    else
    {
      found=true;
      temp+=c;
    }
  } // for

  if (!temp.isEmpty())
  {
    if (!writeColoredWord(yyscanner,temp))
    {
      if (!tclass.isEmpty())
      {
        if (!classLink)
        {
          generateMemLink(yyscanner,*yyextra->code,tclass,temp); // generateMemLink(yyscanner,*yyextra->code,yyextra->currClass,left);
        }
        else
        {
          generateClassOrGlobalLink(yyscanner,*yyextra->code,temp,false,curr_class);
        }
      }
      else
      {
         QCString qc(temp.data());
         if (VhdlDocGen::isNumber(qc.str()))
         {
           startFontClass(yyscanner,"vhdllogic");
           yyextra->code->codify(temp.data());
           endFontClass(yyscanner);
         }
         else
         {
           yyextra->code->codify(temp.data());
         }
      }
    }
  }
}// writeWord


/*! write a code fragment 'text' that may span multiple lines, inserting
 * line numbers for each line.
 */
static void codifyLines(yyscan_t yyscanner,const char *text,const char *cl,bool classlink,bool comment)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (text==0) return;
  //printf("codifyLines(%d,\"%s\")\n",yyextra->yyLineNr,text);
  const char *p=text,*sp=p;
  char c;
  bool done=false;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') {}
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      QCString line = sp;
      line = line.left((int)(p-sp)-1);
      if (comment)
      {
        writeFont(yyscanner,"comment",line.data());
      }
      else
      {
        writeWord(yyscanner,line,cl,classlink);
      }
      nextCodeLine(yyscanner);
    }
    else
    {
      if (comment)
      {
        writeFont(yyscanner,"comment",sp);
      }
      else
      {
        writeWord(yyscanner,sp,cl,classlink);
      }
      done=true;
    }
  }
}

/*! writes a link to a fragment \a text that may span multiple lines, inserting
 * line numbers for each line. If \a text contains newlines, the link will be
 * split into multiple links with the same destination, one for each line.
 */
static void writeMultiLineCodeLink(yyscan_t yyscanner,CodeOutputInterface &ol,
                  const Definition *d,
                  const char *text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  static bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  TooltipManager::instance().addTooltip(ol,d);
  QCString ref  = d->getReference();
  QCString file = d->getOutputFileBase();
  QCString anchor = d->anchor();
  QCString tooltip;
  if (!sourceTooltips) // fall back to simple "title" tooltips
  {
    tooltip = d->briefDescriptionAsTooltip();
  }
  bool done=false;
  char *p=(char *)text;
  while (!done)
  {
    char *sp=p;
    char c;
    while ((c=*p++) && c!='\n') {}
    if (c=='\n')
    {
      yyextra->yyLineNr++;
      *(p-1)='\0';
      // printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      nextCodeLine(yyscanner);
    }
    else
    {
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      done=true;
    }
  }
}

/*! writes a link to a function or procedure
 */
static void generateFuncLink(yyscan_t yyscanner,CodeOutputInterface &ol,const MemberDef* mdef)
{
  //printf("generateFuncLink(FuncName=%s)\n",mdef->name().data());
  QCString memberName=mdef->name();

  if (mdef->isLinkable()) // is it a linkable class
  {
    writeMultiLineCodeLink(yyscanner,ol,mdef,mdef->name());
    addToSearchIndex(yyscanner,memberName);
    return;
  }
  codifyLines(yyscanner,memberName.data());
  addToSearchIndex(yyscanner,memberName);
} // generateFuncLink


static void generateMemLink(yyscan_t yyscanner,CodeOutputInterface &ol,QCString &clName,QCString& memberName)
{
  if (memberName.isEmpty()) return;
  if (clName.isEmpty())
  {
    codifyLines(yyscanner,memberName.data());

   return;
  }

  QCString className=clName;

  //MemberDef *comp=0;
  //bool isLocal=false;

  const MemberDef *md=VhdlDocGen::findMember(className,memberName);
  ClassDef *po=VhdlDocGen::getClass(className.data());

  if (md==0 && po && (VhdlDocGen::VhdlClasses)po->protection()==VhdlDocGen::PACKBODYCLASS)
  {
    QCString temp=className;//.stripPrefix("_");
    temp.stripPrefix("_");
    md=VhdlDocGen::findMember(temp,memberName);
  }

  if (md && md->isLinkable()) // is it a linkable class
  {
    writeMultiLineCodeLink(yyscanner,ol,md,memberName);
    addToSearchIndex(yyscanner,memberName);
    return;
  }
  // nothing found, just write out the word
  codifyLines(yyscanner,memberName.data());
  addToSearchIndex(yyscanner,memberName);
}// generateMemLink


static void generateClassOrGlobalLink(yyscan_t yyscanner,CodeOutputInterface &ol,const char *clName, bool /*typeOnly*/, const char *curr_class)
{
  QCString className=clName;

  if (className.isEmpty()) return;

  ClassDef *cd=0;
  //MemberDef *md=0;
  //bool isLocal=false;
  className.stripPrefix("_");
  cd = getClass(className.data());
  if (!cd && curr_class)
  {
    QCString cls = curr_class;
    QCString suffix = "::";
    suffix+=clName;
    if (cls.right(suffix.length())==suffix)
    {
      cd = getClass(curr_class);
    }
  }

  while (cd)
  {
    //className.stripPrefix("_");
    QCString temp(clName);
    temp.stripPrefix("_");
    if (cd && cd->isLinkable()) // is it a linkable class
    {
      //if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ARCHITECTURECLASS)
      //{
      //  temp=VhdlDocGen::getClassName(cd);
      //}
      writeMultiLineCodeLink(yyscanner,ol,cd,temp);
      addToSearchIndex(yyscanner,className);
      return;
    }
    Definition *d = cd->getOuterScope();
    if (d && d->definitionType()==Definition::TypeClass)
    {
      cd = toClassDef(d);
    }
    else
    {
      cd = 0;
    }
  }

  // nothing found, just write out the word
  codifyLines(yyscanner,clName);
  addToSearchIndex(yyscanner,clName);
}// generateClasss or global link


/*! counts the number of lines in the input */
static int countLines(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  const char *p=yyextra->inputString;
  char c;
  int count=1;
  while ((c=*p))
  {
    p++ ;
    if (c=='\n') count++;
  }
  if (p>yyextra->inputString && *(p-1)!='\n')
  { // last line does not end with a \n, so we add an extra
    // line and explicitly terminate the line after parsing.
    count++,
    yyextra->needsTermination=true;
  }
  return count;
}

static void endFontClass(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (yyextra->currentFontClass)
  {
    yyextra->code->endFontClass();
    yyextra->currentFontClass=0;
  }
}

static void startFontClass(yyscan_t yyscanner,const char *s)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (s==0) return;
  endFontClass(yyscanner);
  yyextra->code->startFontClass(s);
  yyextra->currentFontClass=s;
}

static void writeFont(yyscan_t yyscanner,const char *s,const char* text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (s==0 || text==0) return;
  //printf("writeFont(yyscanner,%d,\"%s\")\n",yyextra->yyLineNr,text);
  yyextra->code->startFontClass(s);
  yyextra->code->codify(text);
  yyextra->code->endFontClass();
}

//----------------------------------------------------------------------------

static void appStringLower(QCString& qcs,const char* text)
{
  qcs.resize(0);
  qcs.append(text);
  qcs=qcs.stripWhiteSpace();
}

/* writes and links a port map statement */
static void codifyMapLines(yyscan_t yyscanner,const char *text)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  if (text==0) return;
  QCString temp;
  //bool dot=false;
  int wordCounter=0;
  QCString ctemp;
  //printf("codifyMapLines(%d,\"%s\")\n",yyextra->yyLineNr,text);
  const char *p=text; //,*sp=p;
  char c;
  bool done=false;
  while (!done)
  {
    //sp=p;
    while ((c=*p++) &&  c!='\n' && c!=':' && c != ' ' && c != '(' && c!='\0' && c!='\t')
    {
      if (c!=0x9)
        temp+=c;
    }
    if (c=='\0') return;
    if (!temp.isEmpty()) wordCounter++;

    if (!temp.isEmpty())
    {
      // different kinds of component instantiations
      // xxx:yyy (generic/port) map(
      // xxx:(entity/component/configuration) yyy (generic/port) map(
      // xxx: entity yyy(zzz) (generic/port) map(
      if (wordCounter==2 || wordCounter==3)
      {
        QCString q=temp.lower(); // consider (upper/lower) cases
        if (q=="entity" || q=="component" || q=="configuration" || q=="port" || q=="generic")
        {
          generateMemLink(yyscanner,*yyextra->code,yyextra->currClass,temp);
        }
        else
        {
          yyextra->PortMapComp=temp;
          generateClassOrGlobalLink(yyscanner,*yyextra->code,temp);
        }
      }
      else
      {
        generateMemLink(yyscanner,*yyextra->code,yyextra->currClass,temp);
      }
    }
    ctemp.fill(c,1);
    codifyLines(yyscanner,ctemp.data());
    ctemp.resize(0);
    temp.resize(0);
  }//while
}//codifyMapLines

/*
* writes a function|procedure prototype and links the function|procedure name
*/

static void writeFuncProto(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  QCString name,ret;
  VhdlDocGen::parseFuncProto(yyextra->funcProto,name,ret,false);

  if (name.isEmpty())
  {
    codifyLines(yyscanner,yyextra->funcProto.data(),yyextra->currClass.data());
    return;
  }
  StringVector qlist=split(yyextra->funcProto.str(),name.str());
  QCString temp=qlist[0];
  codifyLines(yyscanner,temp.data(),yyextra->currClass.data());
  yyextra->funcProto.stripPrefix(temp.data());
  temp.resize(0);
  temp=yyextra->currClass;
  if (yyextra->isPackageBody)
  {
    temp.stripPrefix("_");// _{package body name}
  }
  const MemberDef *mdef=VhdlDocGen::findFunction(name,temp);

  if (mdef)
  {
    generateFuncLink(yyscanner,*yyextra->code,mdef);
    yyextra->funcProto.stripPrefix(name.data());
    codifyLines(yyscanner,yyextra->funcProto.data(),yyextra->currClass.data());
  }
  else
  {
    codifyLines(yyscanner,yyextra->funcProto.data(),yyextra->currClass.data());
  }
}// writeFuncProto

/* writes a process prototype to the output */

static void writeProcessProto(yyscan_t yyscanner)
{
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  codifyLines(yyscanner,yyextra->funcProto.data(),yyextra->currClass.data());
  yyextra->vhdlKeyDict.clear();
}// writeProcessProto

/* writes a keyword */

static bool writeColoredWord(yyscan_t yyscanner,QCString& word )
{
  QCString qcs=word.lower();
  const char *ss=VhdlDocGen::findKeyWord(qcs);
  if (ss)
  {
    writeFont(yyscanner,ss,word.data());
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------------

struct VHDLCodeParser::Private
{
  yyscan_t yyscanner;
  vhdlcodeYY_state state;
};

VHDLCodeParser::VHDLCodeParser() : p(std::make_unique<Private>())
{
  vhdlcodeYYlex_init_extra(&p->state,&p->yyscanner);
#ifdef FLEX_DEBUG
  vhdlcodeYYset_debug(1,p->yyscanner);
#endif
  resetCodeParserState();
}

VHDLCodeParser::~VHDLCodeParser()
{
  vhdlcodeYYlex_destroy(p->yyscanner);
}

void VHDLCodeParser::resetCodeParserState()
{
  p->state.vhdlKeyDict.clear();
}

void VHDLCodeParser::parseCode(CodeOutputInterface &od,
                               const char *className,
                               const QCString &s,
                               SrcLangExt,
                               bool exBlock,
                               const char *exName,
                               FileDef *fd,
                               int startLine,
                               int endLine,
                               bool inlineFragment,
                               const MemberDef *memberDef,
                               bool,
                               const Definition *searchCtx,
                               bool /* collectXRefs */)
{
  yyscan_t yyscanner = p->yyscanner;
  struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
  //printf("***parseCode() exBlock=%d exName=%s fd=%p\n",exBlock,exName,fd);
  if (s.isEmpty()) return;
  printlex(yy_flex_debug, true, __FILE__, fd ? fd->fileName().data(): NULL);
  if (memberDef)
  {
    const ClassDef *dd=memberDef->getClassDef();
    if (dd) yyextra->currClass=dd->name();
  }
  resetCodeParserState();
  yyextra->code = &od;
  yyextra->inputString   = s;
  yyextra->inputPosition = 0;
  yyextra->currentFontClass = 0;
  yyextra->needsTermination = false;
  yyextra->searchCtx = searchCtx;

  if (startLine!=-1)
    yyextra->yyLineNr    = startLine;
  else
    yyextra->yyLineNr    = 1;

  if (endLine!=-1)
    yyextra->inputLines  = endLine+1;
  else
    yyextra->inputLines  = yyextra->yyLineNr + countLines(yyscanner) - 1;


  // yyextra->theCallContext.clear();
  yyextra->exampleBlock  = exBlock;
  yyextra->exampleName   = exName;
  yyextra->sourceFileDef = fd;
  bool cleanupSourceDef = false;
  if (exBlock && fd==0)
  {
    // create a dummy filedef for the example
    yyextra->sourceFileDef = createFileDef("",exName);
    cleanupSourceDef = true;
  }
  if (yyextra->sourceFileDef)
  {
    setCurrentDoc(yyscanner,"l00001");
  }
  yyextra->currentDefinition = 0;
  yyextra->currentMemberDef = 0;
  yyextra->vhdlMember=0;
  if (!yyextra->exampleName.isEmpty())
  {
    yyextra->exampleFile = convertNameToFile(yyextra->exampleName+"-example");
  }
  yyextra->includeCodeFragment = inlineFragment;
  startCodeLine(yyscanner);
  if (!yyextra->lexInit)
  {
    VhdlDocGen::init();
    yyextra->lexInit=true;
  }
  /*int iLine=*/countLines(yyscanner);
  vhdlcodeYYrestart( 0, yyscanner );
  BEGIN( Bases );
  vhdlcodeYYlex(yyscanner);
  if (yyextra->needsTermination)
  {
    endCodeLine(yyscanner);
  }
  if (cleanupSourceDef)
  {
    // delete the temporary file definition used for this example
    delete yyextra->sourceFileDef;
    yyextra->sourceFileDef=0;
  }
  yyextra->startCode=false;

  // write the tooltips
  TooltipManager::instance().writeTooltips(od);

  printlex(yy_flex_debug, false, __FILE__, fd ? fd->fileName().data(): NULL);
}

#if USE_STATE2STRING
#include "vhdlcode.l.h"
#endif

