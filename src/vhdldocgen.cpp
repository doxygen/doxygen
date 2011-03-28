/******************************************************************************
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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
 * Parser for VHDL subset
 * written by M. Kreis
 * supports VHDL-87
 * does not support VHDL-AMS 
 ******************************************************************************/

// global includes
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <qcstring.h>
#include <qfileinfo.h>
#include <qstringlist.h>

/* --------------------------------------------------------------- */

// local includes
#include "vhdldocgen.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "language.h"
#include "commentscan.h"
#include "index.h"
#include "definition.h"
#include "searchindex.h"
#include "outputlist.h"
#include "parserintf.h"
/* --------------------------------------------------------------- */

//#define theTranslator_vhdlType theTranslator->trVhdlType
#define theTranslator_vhdlType VhdlDocGen::trVhdlType

static QDict<QCString> g_vhdlKeyDict0(17,FALSE); 
static QDict<QCString> g_vhdlKeyDict1(17,FALSE); 
static QDict<QCString> g_vhdlKeyDict2(17,FALSE);

static  QCString g_vhdlkeyword("vhdlkeyword");
static  QCString g_vhdltype("comment");
static  QCString g_vhdllogic("vhdllogic");

// keywords
static const char* g_vhdlKeyWordMap0[] = 
{ 
  "std","ieee","work","standard","textio","std_logic_1164",
  "std_logic_arith","std_logic_misc","std_logic_signed","std_logic_textio",
  "std_logic_unsigned","numeric_bit","numeric_std","math_complex","math_real",
  "vital_primitives","vital_timing","severity_level","time","delay_length",
  "natural", "positive", "bit_vector","file_open_kind","file_open_status",
  "line","text","side", "width","event","rising_edge", "falling_edge",
  "access","after","alias", "all","architecture","array", "assert","attribute",
  "begin","block","body", "buffer", "bus", "case", "component", "configuration", 
  "constant", "disconnect", "downto", "else", "elsif", "end", "entity", "exit",
  "file", "for", "function", "generate", "generic", "group", "guarded", "if", 
  "impure", "in", "inertial", "inout", "is","label", "library", "linkage", 
  "literal", "loop","map", "new", "next", "null", "of", "on", "open", "others", 
  "out", "package", "port", "postponed", "procedure", "process", "pure",
  "range", "record", "register", "reject", "report", "return","select", 
  "severity", "shared", "signal", "subtype", "then", "to", "transport", 
  "type","unaffected", "units", "until", "use","variable", "wait", "when", 
  "while", "with","true","false","protected",0
};

// type
static const char* g_vhdlKeyWordMap1[] = 
{ 
  "natural","unsigned","signed","string","boolean", "bit","character",
  "std_ulogic","std_ulogic_vector","sTd_logic","std_logic_vector","integer",
  "real","zzz",0
};

// logic
static const char* g_vhdlKeyWordMap2[] = 
{
  "abs","and","or","not","mod", "xor","rem","xnor","ror","rol","sla",
  "sll",0
};

VhdlDocGen::VhdlDocGen()
{
}

VhdlDocGen::~VhdlDocGen()
{
}

void VhdlDocGen::init()
{
  int j=0;
  g_vhdlKeyDict0.setAutoDelete(TRUE);
  g_vhdlKeyDict1.setAutoDelete(TRUE);
  g_vhdlKeyDict2.setAutoDelete(TRUE);

  j=0;
  while (g_vhdlKeyWordMap0[j])
  {
    g_vhdlKeyDict0.insert(g_vhdlKeyWordMap0[j],
	               new QCString(g_vhdlKeyWordMap0[j]));
    j++;
  }

  j=0;
  while (g_vhdlKeyWordMap1[j])
  {
    g_vhdlKeyDict1.insert(g_vhdlKeyWordMap1[j],
	               new QCString(g_vhdlKeyWordMap1[j]));
    j++;
  }

  j=0;
  while (g_vhdlKeyWordMap2[j])
  {
    g_vhdlKeyDict2.insert(g_vhdlKeyWordMap2[j],
	               new QCString(g_vhdlKeyWordMap2[j]));
    j++;
  }

}// buildKeyMap

/*!
 * returns the color of a keyword 
 */ 

QCString* VhdlDocGen::findKeyWord(const QCString& word)
{
  if (word.isEmpty() || word.at(0)=='\0') return 0;
  //printf("VhdlDocGen::findKeyWord(%s)\n",word.data());

  if (g_vhdlKeyDict0.find(word.lower()))
    return &g_vhdlkeyword;

  if (g_vhdlKeyDict1.find(word.lower()))
    return &g_vhdltype;

  if (g_vhdlKeyDict2.find(word.lower()))
    return &g_vhdllogic;

  return 0;
}

/*!
 * returns the parsed entry at line xxx
 */


void VhdlDocGen::debugClassName(ClassSDict* mDict)
{
  // for each class
  ClassSDict::Iterator cli(*mDict);
  ClassDef *cd;
  for ( ; (cd=cli.current()) ; ++cli )
  {
    printf("\n -------------------------class----------------------------------------\n");

    QCString nnn=cd->className();
    QCString qref=cd->getReference();
    QCString outBase=cd->getOutputFileBase();
    QCString fileBase=cd->getFileBase();
    QCString compType=cd->compoundTypeString();
    QCString inDoc=cd->documentation();//->inbodyDocumentation();
    printf("\n refernz [%p]",cd);
    printf("\n compType [%s]",compType.data());
    printf("\n Name [%s]",nnn.data());
    printf("\n TYPE[%d ",cd->definitionType());
    printf("\n Ref [%s] ",qref.data());
    printf("\n OutBase [%s] fileBase [%s]",outBase.data(),fileBase.data());

    printf("\n -------------------------------------------------------------------\n");

  }// for
}// Debug Class Name


bool found =FALSE;
static Entry eMerge;

ClassDef *VhdlDocGen::getClass(const char *name)
{
  if (name==0 || name[0]=='\0') return 0;

  ClassDef *cd=0;   
  QCString temp(name);
  //temp=temp.lower();
  temp=temp.stripWhiteSpace();
  cd= Doxygen::classSDict->find(temp.data());
  return cd;
}

/*!
 * adds architectures to their entity
 */
void VhdlDocGen::computeVhdlComponentRelations()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  for (cli.toFirst();cli.current();++cli)
  {
    cli.current()->visited=FALSE;
    ClassDef * cd = cli.current();
    if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ARCHITECTURECLASS || 
        (VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS)
    {
      QCString bName=cd->name();
      int i=bName.find("::");
      if (i>0)
      {
        QCString entityName=bName.left(i);
        entityName.stripPrefix("_");
        ClassDef *classEntity=Doxygen::classSDict->find(entityName);
        // entity for architecutre ?
        if (classEntity)
        {
          // printf("\n entity %s  arch %s",entityName.data(),bName.data());
          classEntity->insertBaseClass(cd,bName,Public,Normal,0);
          cd->insertSubClass(classEntity,Public,Normal,0);
        }
      }
    }
  }
} // computeVhdlComponentRelations


/*
 * returns a reference, if one class [package(body),entity or an architecture is found]
 */

ClassDef* VhdlDocGen::findComponent(int type)
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd=0;

  for ( ; (cd=cli.current()) ; ++cli )
  {
    if (cd->protection()==type)
      return cd;
  }
  return cd;
}

ClassDef* VhdlDocGen::getPackageName(const QCString & name)
{
  ClassDef* cd=0;
  QStringList ql=QStringList::split(".",name,FALSE);
  cd=getClass(name);

  return cd;
}

MemberDef* VhdlDocGen::findMember(const QCString& className, const QCString& memName)
{
  QDict<QCString> packages(17,FALSE);   
  packages.setAutoDelete(TRUE);
  ClassDef* cd;
  MemberDef *mdef=0;

  cd=getClass(className);
  //printf("VhdlDocGen::findMember(%s,%s)=%p\n",className.data(),memName.data(),cd);
  if (cd==0) return 0;

  mdef=VhdlDocGen::findMemberDef(cd,memName,MemberList::variableMembers);
  if (mdef) return mdef;
  mdef=VhdlDocGen::findMemberDef(cd,memName,MemberList::pubMethods);
  if (mdef) return mdef;

  // nothing found so far 
  // if we are an architecture or package body search in entitiy

  if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ARCHITECTURECLASS || 
      (VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS)
  {
    Definition *d = cd->getOuterScope();
     // searching upper/lower case names
 
    QCString tt=d->name();   
    ClassDef *ecd =getClass(tt);
    if (!ecd)
    {
      tt=tt.upper();
      ecd =getClass(tt);
    }
    if (!ecd)
    {
      tt=tt.lower();
      ecd =getClass(tt);  
    }

    if (ecd) //d && d->definitionType()==Definition::TypeClass)
    {
      //ClassDef *ecd = (ClassDef*)d;
      mdef=VhdlDocGen::findMemberDef(ecd,memName,MemberList::variableMembers);
      if (mdef) return mdef;
       mdef=VhdlDocGen::findMemberDef(cd,memName,MemberList::pubMethods);
       if (mdef) return mdef;
    }
    //cd=getClass(getClassName(cd));
    //if (!cd) return 0;
  }
  // nothing found , so we are now searching all included packages 
  VhdlDocGen::findAllPackages(className,packages);
  //cd=getClass(className.data());
  if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ARCHITECTURECLASS || 
      (VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS)
  {
    Definition *d = cd->getOuterScope();
   
    QCString tt=d->name();
    ClassDef *ecd =getClass(tt);
    if (!ecd)
    {
      tt=tt.upper();
      ecd =getClass(tt);
    }
    if (!ecd)
    {
      tt=tt.lower();
      ecd =getClass(tt);  
    }

    if (ecd) //d && d->definitionType()==Definition::TypeClass)
    {
      VhdlDocGen::findAllPackages(ecd->className(),packages); 
    }
  }

  QDictIterator<QCString> packli(packages);
  QCString *curString;
  for (packli.toFirst();(curString=packli.current());++packli)
  {
    if (curString)
    {
      cd=VhdlDocGen::getPackageName(*curString);
      if (!cd)
      {
        *curString=curString->upper();
        cd=VhdlDocGen::getPackageName(*curString);
      }
      if (!cd)
      {
        *curString=curString->lower();
        cd=VhdlDocGen::getPackageName(*curString);  
      }
    }
    if (cd)
    {    
      mdef=VhdlDocGen::findMemberDef(cd,memName,MemberList::variableMembers);
      if (mdef)  return mdef;
      mdef=VhdlDocGen::findMemberDef(cd,memName,MemberList::pubMethods);
      if (mdef) return mdef;
    }
  } // for
  return 0;
}//findMember

/**
 *  This function returns the entity|package
 *  in which the key (type) is found
 */

MemberDef* VhdlDocGen::findMemberDef(ClassDef* cd,const QCString& key,MemberList::ListType type)
{
  //    return cd->getMemberByName(key);//does not work 
  MemberDef *md=0;

  MemberList *ml=    cd->getMemberList(type);
  if (ml==0) return 0;

  MemberListIterator fmni(*ml);

  for (fmni.toFirst();(md=fmni.current());++fmni)
  {
    if (stricmp(key.data(),md->name().data())==0)
      return md;
  } 
  return 0;
}//findMemberDef

/*!
 * finds all included packages of an Entity or Package
 */

void VhdlDocGen::findAllPackages(const QCString& className,QDict<QCString>& qdict)
{
  ClassDef *cdef=getClass(className);
  if (cdef)
  {
    MemberList *mem=cdef->getMemberList(MemberList::variableMembers); 
    MemberDef *md;

    if (mem)
    {
      MemberListIterator fmni(*mem);
      for (fmni.toFirst();(md=fmni.current());++fmni)
      {
	if (VhdlDocGen::isPackage(md))
	{
	  QCString *temp1=new QCString(md->name().data());
	  //*temp1=temp1->lower();
	  QCString p(md->name().data());
	  //p=p.lower();
	  ClassDef* cd=VhdlDocGen::getPackageName(*temp1);
	  if (cd) 
	  { 
	    QCString *ss=qdict.find(*temp1);
	    if (ss==0)
	    {
	      qdict.insert(p,temp1);
	      QCString tmp=cd->className();
	      VhdlDocGen::findAllPackages(tmp,qdict);
	    }
	    else delete temp1;
	  }
	  else delete temp1;
	}
      }//for
    }//if
  }//cdef
}// findAllPackages

/*!
 * returns the function with the matching argument list
 * is called in vhdlcode.l
 */

MemberDef* VhdlDocGen::findFunction(const QList<Argument> &ql,
                                    const QCString& funcname,
				    const QCString& package, bool type)
{
  MemberDef* mdef=0;
  int funcType;
  ClassDef *cdef=getClass(package.data());
  if (cdef==0) return 0;

  if (type)
    funcType=VhdlDocGen::PROCEDURE;
  else
    funcType=VhdlDocGen::FUNCTION;

  MemberList *mem=cdef->getMemberList(MemberList::pubMethods); 

  if (mem)
  {
    MemberListIterator fmni(*mem);
    for (fmni.toFirst();(mdef=fmni.current());++fmni)
    {
      QCString mname=mdef->name();
      if ((VhdlDocGen::isProcedure(mdef) || VhdlDocGen::isVhdlFunction(mdef)) && (VhdlDocGen::compareString(funcname,mname)==0))
      {
	LockingPtr<ArgumentList> alp = mdef->argumentList();

	//  ArgumentList* arg2=mdef->getArgumentList();
	if (alp==0) break;
	ArgumentListIterator ali(*alp.pointer());
	ArgumentListIterator ali1(ql);

	if (ali.count() != ali1.count()) break;

	Argument *arg,*arg1;
	int equ=0;

	for (;(arg=ali.current());++ali)
	{
	  arg1=ali1.current(); ++ali1;
	  equ+=abs(VhdlDocGen::compareString(arg->type,arg1->type));

	  QCString s1=arg->type;
	  QCString s2=arg1->type;
	  VhdlDocGen::deleteAllChars(s1,' ');
	  VhdlDocGen::deleteAllChars(s2,' ');
	  equ+=abs(VhdlDocGen::compareString(s1,s2));
	  s1=arg->attrib;
	  s2=arg1->attrib;
	  VhdlDocGen::deleteAllChars(s1,' ');
	  VhdlDocGen::deleteAllChars(s2,' ');
	  equ+=abs(VhdlDocGen::compareString(s1,s2));
	  // printf("\n 1. type [%s] name [%s] attrib [%s]",arg->type,arg->name,arg->attrib);
	  // printf("\n 2. type [%s] name [%s] attrib [%s]",arg1->type,arg1->name,arg1->attrib);
	} // for
	if (equ==0) return mdef;
      }//if
    }//for       
  }//if 
  return mdef;
} //findFunction

/*!
 * returns the function with the matching argument list
 * is called in vhdscan.l
 */

Entry* VhdlDocGen::findFunction( Entry* root, Entry* func)
{
  //bool found=FALSE;
  Entry *found=0;
  int functype=func->spec;
  EntryListIterator eli(*root->children());
  Entry *rt;
  for (;(rt=eli.current());++eli)
  { 
    if (rt->spec==functype && VhdlDocGen::compareString(rt->name,func->name)==0 && rt!=func )
    {
      if (VhdlDocGen::compareArgList(func->argList,rt->argList))
      {
	found=rt;
	return found;
      }
    }//if1
    if (!found)  
    {
      found = VhdlDocGen::findFunction(rt,func);
    }
  } // for
  return found;
}// findFunction

/*
 * compares two argument list of a fuction|procedure
 */ 

bool VhdlDocGen::compareArgList(ArgumentList* l1,ArgumentList* l2)
{
  if (l1== 0 || l2== 0) return FALSE;

  ArgumentListIterator ali(*l1);
  ArgumentListIterator ali1(*l2);

  if (ali.count() != ali1.count()) return FALSE; 

  Argument *arg,*arg1;
  int equ=0;

  for (;(arg=ali.current());++ali) 
  {
    bool found = FALSE;
    for (ali1.toFirst();(arg1=ali1.current());++ali1) 
    {
      equ=0;  
      QCString s1=arg->type;
      QCString s2=arg1->type;
      VhdlDocGen::deleteAllChars(s1,' '); // remove whitespaces
      VhdlDocGen::deleteAllChars(s2,' ');
      equ+=abs(VhdlDocGen::compareString(s1,s2));
      s1=arg->attrib;
      s2=arg1->attrib;
      VhdlDocGen::deleteAllChars(s1,' ');
      VhdlDocGen::deleteAllChars(s2,' ');        
      equ+=abs(VhdlDocGen::compareString(s1,s2));
      if (equ==0) found=TRUE;
    }
    if (!found) return FALSE;
  }
  return TRUE;
}// compareArgList

/*
 * finds a matching prototype for a function description  
 */ 

Entry* VhdlDocGen::findFunction(Entry* func)
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for (;(cd=cli.current());++cli) 
  {
    MemberList *mf = cd->getMemberList (MemberList::pubMethods);
    if (mf) 
    {
      MemberListIterator fmni(*mf);
      MemberDef *mdd;
      for (fmni.toFirst();(mdd=fmni.current());++fmni)
      {
	int type=mdd->getMemberSpecifiers();
	if (type==VhdlDocGen::PROCEDURE || type==VhdlDocGen::FUNCTION)
	{
	  QCString nnk=mdd->name();
	  QCString ff=func->name;

	  if (stricmp(mdd->name(),ff.data())==0) 
	  {
	    LockingPtr< ArgumentList > lp=mdd->argumentList();
	    ArgumentList *l=lp.pointer(); 
	    if (VhdlDocGen::compareArgList(l,func->argList))
	    {
	      mdd->setDocumentation(func->doc.data(),func->docFile.data(),func->docLine,TRUE);
	      mdd->setBriefDescription(func->brief,func->briefFile,func->briefLine);
	      addMemberToGroups(func,mdd);// do not forget grouping!
	      return func;
	    }  
	  }
	}
      }
    }// if
  }//for
  return 0;
}// findFunction

/*
 * adds the documentation for a function|procedure 
 */ 

void VhdlDocGen::addFuncDoc(EntryNav* rootNav)
{
  Entry *root = rootNav->entry();
  if (root &&  root->spec==VhdlDocGen::DOCUMENT)
  {
    Entry *func=VhdlDocGen::findFunction(root);
    if (!func && Config_getBool("WARNINGS"))
    {
      warn(root->fileName,root->docLine,
      "warning: documentation for unknown function %s found.\n",
      root->name.data()
      );
    }
  }
}// AddFuncDoc

/*! 
 * returns the class title+ref
 */

QCString VhdlDocGen::getClassTitle(const ClassDef *cd)
{
  QCString pageTitle;
  if (cd==0) return "";
  pageTitle+=cd->displayName();
  pageTitle=VhdlDocGen::getClassName(cd);
  int ii=cd->protection();
  pageTitle+=" ";
  pageTitle+=theTranslator_vhdlType(ii+2,TRUE);
  pageTitle+=" ";
  return pageTitle;
} // getClassTitle

/* returns the class name without their prefixes */

QCString VhdlDocGen::getClassName(const ClassDef* cd)
{
  QCString temp;
  if (cd==0) return "";

  if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::PACKBODYCLASS)
  { 
    temp=cd->name();
    temp.stripPrefix("_");
    return temp;
  }  
  //if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ARCHITECTURECLASS)
  //{  
  //  QStringList qlist=QStringList::split("-",cd->className(),FALSE);
  //  if (qlist.count()>1)
  //    return (QCString)qlist[1];
  //  return "";
  //}
  return substitute(cd->className(),"::",".");
}

/*! 
 * writes an inline link form entity|package to architecture|package body and vice verca
 */

void VhdlDocGen::writeInlineClassLink(const ClassDef* cd ,OutputList& ol)
{
  QList<QCString> ql;
  ql.setAutoDelete(TRUE);
  QCString nn=cd->className();
  int ii=(int)cd->protection()+2;

  QCString type;
  if (ii==VhdlDocGen::ENTITY)
    type=theTranslator_vhdlType(VhdlDocGen::ARCHITECTURE,TRUE);
  else if (ii==VhdlDocGen::ARCHITECTURE)
    type=theTranslator_vhdlType(VhdlDocGen::ENTITY,TRUE);
  else if (ii==VhdlDocGen::PACKAGE_BODY)
    type=theTranslator_vhdlType(VhdlDocGen::PACKAGE,TRUE);
  else if (ii==VhdlDocGen::PACKAGE)
    type=theTranslator_vhdlType(VhdlDocGen::PACKAGE_BODY,TRUE);

  //type=type.lower(); 
  type+=" >> ";
  ol.disable(OutputGenerator::RTF); 
  ol.disable(OutputGenerator::Man);
  ol.lineBreak();
  ol.lineBreak();

  if (ii==VhdlDocGen::PACKAGE_BODY) 
  {
    nn.stripPrefix("_");
    cd=getClass(nn.data());
  }
  else  if (ii==VhdlDocGen::PACKAGE) 
  {
    nn.prepend("_");
    cd=getClass(nn.data());
  }
  else if (ii==VhdlDocGen::ARCHITECTURE)
  {
    QStringList qlist=QStringList::split("-",nn,FALSE);
    nn=qlist[1];
    cd=VhdlDocGen::getClass(nn.data());
  }       

  QCString opp;
  if (ii==VhdlDocGen::ENTITY)
  { 
    VhdlDocGen::findAllArchitectures(ql,cd);
    int j=ql.count();
    for (int i=0;i<j;i++)
    {
      QCString *temp=ql.at(i);
      QStringList qlist=QStringList::split("-",*temp,FALSE);
      QCString s1=(QCString)qlist[0];
      QCString s2=(QCString)qlist[1];
      s1.stripPrefix("_");
      if (j==1) s1.resize(0);
      ClassDef*cc = getClass(temp->data());
      if (cc)
      {
	VhdlDocGen::writeVhdlLink(cc,ol,type,s2,s1); 
      }
    }    
  }  
  else
  {
    VhdlDocGen::writeVhdlLink(cd,ol,type,nn,opp);
  }

  ol.enable(OutputGenerator::Man);
  ol.enable(OutputGenerator::RTF);

}// write

/*
 * finds all architectures which belongs to an entiy
 */
void VhdlDocGen::findAllArchitectures(QList<QCString>& qll,const ClassDef *cd)
{
  ClassDef *citer;
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  for ( ; (citer=cli.current()) ; ++cli )
  {
    QCString jj=citer->className();
    if (cd != citer && jj.contains('-')!=-1)
    {
      QStringList ql=QStringList::split("-",jj,FALSE);
      QCString temp=(QCString)ql[1];
      if (stricmp(cd->className().data(),temp.data())==0) 
      {
	QCString *cl=new QCString(jj.data());
	qll.insert(0,cl);
      }
    }           
  }// for
}//findAllArchitectures

/*
 * writes the link entity >> .... or architecture >> ... 
 */

void VhdlDocGen::writeVhdlLink(const ClassDef* ccd ,OutputList& ol,QCString& type,QCString& nn,QCString& behav)
{
  if (ccd==0)  return;
  QCString temp=ccd->getOutputFileBase();
  ol.startBold();
  ol.docify(type.data());
  ol.endBold();
  nn.stripPrefix("_");
  ol.writeObjectLink(ccd->getReference(),ccd->getOutputFileBase(),ccd->anchor(),nn.data());

  if (!behav.isEmpty()) 
  {
    behav.prepend("  ");
    ol.startBold();
    ol.docify(behav.data());
    ol.endBold();
  }
  /*
     if (Config_getBool("SOURCE_BROWSER")) { // writes a source link for latex docu
     ol.pushGeneratorState();
     ol.disableAllBut(OutputGenerator::Latex);
     ol.docify(" | ");
     ol.startEmphasis();
     FileDef* fd=ccd->getFileDef();
     if (fd)
     ol.writeObjectLink(0,fd->getSourceFileBase(),0,theTranslator->trGotoSourceCode().data());
     ol.endEmphasis();
     ol.popGeneratorState();
     }
   */    
  ol.lineBreak();
}

bool VhdlDocGen::compareString(const QCString& s1,const QCString& s2)
{
  QCString str1=s1.stripWhiteSpace();
  QCString str2=s2.stripWhiteSpace();

  return stricmp(str1.data(),str2.data());
}

bool VhdlDocGen::getSigTypeName(QList<QCString>& ql, const char* str,QCString& buffer)
{
  //QCString temp(str);
  //QStringList qlist=QStringList::split(" is ",temp,FALSE);
  //if (qlist.count()!=2) return FALSE;
  //temp.resize(0);
  //temp+=(QCString)qlist[0]+":"+(QCString)qlist[1];
  //return VhdlDocGen::getSigName(ql,temp.data(),buffer);  
  return VhdlDocGen::getSigName(ql,str,buffer);  
}

/*!
 * divides a port input   in its name,direction and type
 * @param ql stores the input name(s)
 * @param str input string
 * @param buffer stores the input direction
 * @returns FALSE if it is a port
 */

bool VhdlDocGen::getSigName(QList<QCString>& ql, 
                            const char* str,QCString& buffer)
{
  int j,ll,index;
  const char *signal = "signal ";
  QCString qmem;
  QCString temp(str);
  QCString st(str);

  //QRegExp semi(",");
  //QRegExp r(":");

  // colon position
  j = temp.find(':');
  if (j < 0) return FALSE; // no input definition
  st=st.left(j); // name only
  index=st.find(signal,0,FALSE);
  if (index > -1) // found "signal "
  {
    qmem=st.remove(index,strlen(signal)); // strip it
    temp=qmem;
    st=qmem;
  }
  else
  {
    qmem=temp;
  }

  ll=st.find(',');

  if (ll>0) // multiple names
  {
    while (TRUE)
    {
      st=st.left(ll).stripWhiteSpace(); // one name

      QCString *sig =new QCString(st);
      ql.insert(0,sig);
      qmem=qmem.right(qmem.length()-ll-1); // strip from list
      st=qmem; // remainder
      ll=st.find(',');
      if (ll<0) // last name
      {
	ll = st.find(':');
	st=st.left(ll).stripWhiteSpace();
	ql.insert(0,new QCString(st));
	break;
      }
    }
  }
  else // single name
  {
    st=st.stripWhiteSpace();
    ql.insert(0,new QCString(st));
  }
  QCString *qdir=new QCString(str);
  st=qdir->mid(j+1); // part after :
  st=st.lower().stripWhiteSpace();
  *qdir=st;
  ql.insert(0,qdir);

  if (st.stripPrefix("inout"))
  {
    buffer+="inout";
    return TRUE;
  }
  if (st.stripPrefix("INOUT"))
  {
    buffer+="inout";
    return TRUE;
  }

  if (st.stripPrefix("out"))
  {
    buffer+="out";
    return TRUE;
  }
  if (st.stripPrefix("OUT"))
  {
    buffer+="out";
    return TRUE;
  }

  if (st.stripPrefix("in"))
  {
    buffer+="in";
    return TRUE;
  }
  if (st.stripPrefix("IN"))
  {
    buffer+="in";
    return TRUE;
  }
  return FALSE;
}

/*!
 * divides a process string in its name and types
 * @param text process text
 * @param name points to the process name
 * @param ql stores the process types
 */

void VhdlDocGen::parseProcessProto(const char* text,
                                  QCString& name,QStringList& ql)
{
  int index,end;
  const char *s=":";
  QCString temp;
  QCString s1(text);
  index=s1.find(s,0,FALSE);
  if (index >=0)
  {
    name=s1.left(index);
    //  strcpy(name,tt.data());
  }

  index=s1.find("(",0,FALSE);
  end=s1.findRev(")",s1.length(),FALSE);
  // end=s1.find(")",0,FALSE);

  if ((end-index)>1) 
  {
    temp=s1.mid(index+1,(end-index-1));
    ql=QStringList::split(",",temp,FALSE);
  }
}//parseProcessProto

/*!
 * strips the "--" prefixes of vhdl comments 
 */
void VhdlDocGen::prepareComment(QCString& qcs)
{
  QCString temp;
  const char* s="--!";
  //const char *start="--!{";
  //const char *end="--!}";
  int index=0; 

#if 0
  index=qcs.find(start,0,TRUE);
  if (index>0)
    temp=qcs.remove(index,strlen(start));
  qcs=temp;

  index=qcs.find(end,0,TRUE);
  if (index>0)
    temp=qcs.remove(index,strlen(end));
  qcs=temp;
#endif
  while (TRUE)
  {
    index=qcs.find(s,0,TRUE);
    if (index<0) break;
    temp=qcs.remove(index,strlen(s));
    qcs=temp;
  }
  qcs=qcs.stripWhiteSpace();
}


/*!
 * parses a function proto
 * @param text function string
 * @param qlist stores the function types
 * @param name points to the function name
 * @param ret Stores the return type
 * @param doc ???
 */
void VhdlDocGen::parseFuncProto(const char* text,QList<Argument>& qlist,
                                QCString& name,QCString& ret,bool doc)
{
  int index,end;
  QCString s1(text);
  QCString temp;

  index=s1.find("(");
  end=s1.findRev(")");

  if ((end-index)>0) 
  {
    QCString tt=s1.mid(index,(end-index+1));
    temp=s1.mid(index+1,(end-index-1));
    getFuncParams(qlist,temp);
  } 
  if (doc)
  {
    name=s1.left(index);
    name=name.stripWhiteSpace();
    if ((end-index)>0) 
    { 
      ret="function";
    }  
    return;
  }
  else
  {  
    QCString s1(text);
    s1=s1.stripWhiteSpace();
    int i=s1.find("(",0,FALSE);
    int s=s1.find(QRegExp("[ \\t]"));
    if (i==-1 || i<s)
      s1=VhdlDocGen::getIndexWord(s1.data(),1);
    else // s<i, s=start of name, i=end of name
      s1=s1.mid(s,(i-s));

    name=s1.stripWhiteSpace();
  }
  index=s1.findRev("return",-1,FALSE);
  if (index !=-1)
  {
    ret=s1.mid(index+6,s1.length());
    ret=ret.stripWhiteSpace();
    VhdlDocGen::deleteCharRev(ret,';');
  }
}

/*
 *  returns the n'th word of a string
 */

QCString VhdlDocGen::getIndexWord(const char* c,int index)
{
  QStringList ql;
  QCString temp(c);
  QRegExp reg("[\\s]");

  ql=QStringList::split(reg,temp,FALSE);

  if (ql.count() > (unsigned int)index)
  {
    return (QCString)ql[index];
  }

  return "";
}


/*!
 * \brief returns the arguments of a function or procedure prototype
 * @param ql list ql stores the arguments
 * @param str prototype
 */

void VhdlDocGen::getFuncParams(QList<Argument>& ql, const char* str)
{

  int len;
  QCString qmem,s1,s2,ttype;
  QCString temp(str);
  temp=temp.stripWhiteSpace();
  if (temp.isEmpty()) return;

  QCString st(str);
  QStringList strList;

  strList=QStringList::split(";",temp,FALSE);
  int kk=strList.count();
  int j=kk;
  while (kk>=1)
  {
    temp=strList[j-kk];
    QStringList tempList,tt;
    tempList=QStringList::split(":",temp,FALSE);
    if (tempList.count()>2)
      ttype=tempList[1];
    else
      ttype=tempList.last();

    ttype=ttype.stripWhiteSpace();

    uint zui=ttype.contains('(',FALSE);
    if (zui == 0)
    {
      tt=QStringList::split(QRegExp("[\\s]"),ttype,FALSE);
    }
    else
    {
      if (ttype.stripPrefix("in"))
	tt.append("in");
      else if (ttype.stripPrefix("out"))
	tt.append("out");
      else if (ttype.stripPrefix("inout"))
	tt.append("inout");
      if (ttype.stripPrefix("IN"))
	tt.append("in");
      else if (ttype.stripPrefix("OUT"))
	tt.append("out");
      else if (ttype.stripPrefix("INOUT"))
	tt.append("inout");

      ttype=ttype.stripWhiteSpace();
      tt.append(ttype);
    }

    s1=tt.first();
    //s1=s1.lower();

    if (tempList.count()>2)
      s2=tt.last()+":"+tempList[2];
    else
      s2=tt.last();

    QCString first=(QCString)tempList.first();

    tempList.clear();
    tt.clear();

    tempList=QStringList::split(",",first,FALSE);
    len=tempList.count();
    ttype.resize(0);
    for (int j=0;j<len;j++)
    {
      Argument *arg=new Argument;
      QCString name=(QCString)tempList[j];
      name=name.stripWhiteSpace();

      tt=QStringList::split(QRegExp("[\\s]"),name,FALSE);
      if (tt.count() > 1)
	ttype=(tt.first()).stripWhiteSpace();

      arg->defval=ttype.copy();
      arg->type=s2.stripWhiteSpace();
      arg->attrib=s1.stripWhiteSpace();
      arg->name=(tt.last()).stripWhiteSpace();

      // printf("--proto \n [%s] [%s] [%s] [%s] [%s]",ttype.data(),arg->type.data(),arg->attrib.data(),arg->name.data(),s1.data());
      ql.append(arg);
    }
    kk--;
  }//while
} // getFuncName

QCString VhdlDocGen::getProtectionName(int prot)
{
  if (prot==VhdlDocGen::ENTITYCLASS)
    return "entity";
  else if (prot==VhdlDocGen::ARCHITECTURECLASS)
    return "architecture";
  else if (prot==VhdlDocGen::PACKAGECLASS)
    return "package";
  else if (prot==VhdlDocGen::PACKBODYCLASS)
    return "package body";

  return "";
}

QCString VhdlDocGen::trTypeString(int type)
{
  switch(type)
  {
    case VhdlDocGen::LIBRARY:        return "Library"; 
    case VhdlDocGen::ENTITY:         return "Entity";
    case VhdlDocGen::PACKAGE_BODY:   return "Package Body";
    case VhdlDocGen::ATTRIBUTE:      return "Attribute";
    case VhdlDocGen::PACKAGE:        return "Package";
    case VhdlDocGen::SIGNAL:         return "Signal";
    case VhdlDocGen::COMPONENT:      return "Component";
    case VhdlDocGen::CONSTANT:       return "Constant";
    case VhdlDocGen::TYPE:           return "Type";
    case VhdlDocGen::SUBTYPE:        return "Subtype";
    case VhdlDocGen::FUNCTION:       return "Function";
    case VhdlDocGen::RECORD:         return "Record";
    case VhdlDocGen::PROCEDURE:      return "Procedure";
    case VhdlDocGen::ARCHITECTURE:   return "Architecture";
    case VhdlDocGen::USE:            return "Package";
    case VhdlDocGen::PROCESS:        return "Process";
    case VhdlDocGen::PORT:           return "Port";
    case VhdlDocGen::GENERIC:        return "Generic";
    case VhdlDocGen::DOCUMENT:       return "Doc"; 
    case VhdlDocGen::UNITS:          return "Units"; 
    //case VhdlDocGen::PORTMAP:        return "Port Map"; 
    case VhdlDocGen::SHAREDVARIABLE: return "Shared Variable"; 
    case VhdlDocGen::GROUP:          return "Group"; 
    case VhdlDocGen::VFILE:          return "File"; 
    case VhdlDocGen::COMPONENT_INST: return "Component Instantiation"; 
    case VhdlDocGen::ALIAS:          return "Alias";
    case VhdlDocGen::CONFIG:         return "Configuration";                                     
    case VhdlDocGen::MISCELLANEOUS:  return "Miscellaneous";                                 
    default:                         return "";
  }
} // convertType

/*!
 * deletes a char backwards in a string 
 */

bool VhdlDocGen::deleteCharRev(QCString &s,char c)
{
  int index=s.findRev(c,-1,FALSE);
  if (index > -1)
  {
    QString qcs=s.remove(index,1);
    s=qcs;
    return TRUE;
  }
  return FALSE;
}

void VhdlDocGen::deleteAllChars(QCString &s,char c)
{
  int index=s.findRev(c,-1,FALSE);
  while (index > -1)
  {
    QString qcs=s.remove(index,1);      
    s=qcs;
    index=s.findRev(c,-1,FALSE);
  }  
}


static int recordCounter=0; 

/*!
 * returns the next number of a record|unit member
 */

QCString VhdlDocGen::getRecordNumber()
{
  char buf[12];
  sprintf(buf,"%d",recordCounter++);
  QCString qcs(&buf[0]);
  return qcs;
}

/*!
 * returns the next number of an anonymous process
 */

QCString VhdlDocGen::getProcessNumber()
{
  static int stringCounter;
  char buf[8];
  QCString qcs("PROCESS_");
  sprintf(buf,"%d",stringCounter++);
  qcs.append(&buf[0]);
  return qcs;
}

/*!
 * writes a colored and formatted string 
 */

void VhdlDocGen::writeFormatString(QCString& qcs,OutputList&ol,const MemberDef* mdef)
{
  QRegExp reg("[\\/\\:\\<\\>\\:\\s\\,\\;\\'\\+\\-\\*\\|\\&\\=\\(\\)\"]");
  qcs+=QCString(" ");// parsing the last sign
  QCString *ss;
  QCString find=qcs;
  QCString temp=qcs;
  char buf[2];
  buf[1]='\0';

  int j;
  int len;
  j = reg.match(temp.data(),0,&len);

  ol.startBold();
  if (j>=0)
  {
    while (j>=0)
    {
      find=find.left(j);
      buf[0]=temp[j];
      ss=VhdlDocGen::findKeyWord(find);
      bool k=VhdlDocGen::isNumber(find); // is this a number
      if (k)
      {
        ol.docify(" ");
        VhdlDocGen::startFonts(find,"vhdldigit",ol);
        ol.docify(" ");
      }
      else if (j != 0 && ss)
      {
	VhdlDocGen::startFonts(find,ss->data(),ol);
      }
      else
      {
	if (j>0)
	{
	  VhdlDocGen::writeStringLink(mdef,find,ol);
	}
      }
      VhdlDocGen::startFonts(&buf[0],"vhdlchar",ol);

      QCString st=temp.remove(0,j+1);
      find=st;
      temp=st;
      j = reg.match(temp.data(),0,&len);
    }//while
  }//if
  else
  {
    VhdlDocGen::startFonts(find,"vhdlchar",ol);     
  }
  ol.endBold();
}// writeFormatString

/*!
 * returns TRUE if this string is a number
 */

bool VhdlDocGen::isNumber(const QCString& s)
{
  static QRegExp regg("[0-9][0-9eEfFbBcCdDaA_.#-]*");
 
  if (s.isEmpty()) return FALSE;
  int j,len;
  j = regg.match(s.data(),0,&len);
  if ((j==0) && (len==(int)s.length())) return TRUE; 
  return FALSE;
 
  #if 0
  int len=s.length();
  if (len==0) return FALSE;
  for (int j=0;j<len;j++)
  {
    if (isdigit((int)(s.at(j) & 0xff))==0)
      return FALSE;
  }
  return TRUE;
  #endif 
}// isNumber

void VhdlDocGen::startFonts(const QCString& q, const char *keyword,OutputList& ol)
{
  ol.startFontClass(keyword);
  ol.docify(q.data());
  ol.endFontClass();
}

/*!
 * inserts white spaces for  better readings  
 * and writes a colored string to the output
 */

void VhdlDocGen::formatString(QCString & qcs, OutputList& ol,const MemberDef* mdef)
{ 
  QCString temp(qcs.length());  
  qcs.stripPrefix(":");
  qcs.stripPrefix("is");
  qcs.stripPrefix("IS");
  qcs.stripPrefix("of");
  qcs.stripPrefix("OF");

  VhdlDocGen::deleteCharRev(qcs,';');
  //char white='\t';
  int len = qcs.length();
  unsigned int index=1;//temp.length();

  for (int j=0;j<len;j++)
  {
    char c=qcs[j];
    char b=c;
    if (j>0) b=qcs[j-1];
    if (c=='"' || c==',' || c==';' || c=='\''|| c=='(' || c==')'  || c==':' ) // || (c==':' && b!='=')) // || (c=='=' && b!='>')) 
    {
      if (temp.at(index-1) != ' ')
      {
	temp+=" ";
      }
      temp+=c;
      temp+=" "; 
    }
    else if (c=='=') 
    {
      if (b==':') // := operator
      {   
	temp.replace(index-1,1,"=");
	temp+=" "; 
      }
      else // = operator
      {
	temp+=" ";      
	temp+=c;
	temp+=" "; 
      }
    }
    else      
    {
      temp+=c;
    }

    index=temp.length();
  }// for
  temp=temp.stripWhiteSpace();
  // printf("\n [%s]",qcs.data());
  VhdlDocGen::writeFormatString(temp,ol,mdef);
}

/*!
 * writes a procedure prototype to the output
 */

void VhdlDocGen::writeProcedureProto(OutputList& ol,const ArgumentList* al,const MemberDef* mdef)
{
  ArgumentListIterator ali(*al);
  Argument *arg;
  bool sem=FALSE;
  int len=al->count();
  ol.docify("( ");    
  if (len > 2)
  {
    ol.lineBreak();
  }
  for (;(arg=ali.current());++ali)
  {
    ol.startBold();
    if (sem && len <3)
      ol.writeChar(',');

    QCString nn=arg->name;
    nn+=": ";

    QCString *str=VhdlDocGen::findKeyWord(arg->defval);
    arg->defval+=" ";
    if (str) 
    {
      VhdlDocGen::startFonts(arg->defval,str->data(),ol);
    }
    else
    {
      VhdlDocGen::startFonts(arg->defval,"vhdlchar",ol); // write type (variable,constant etc.)
    }

    VhdlDocGen::startFonts(nn,"vhdlchar",ol); // write name 
    if (stricmp(arg->attrib.data(),arg->type.data()) != 0)
      VhdlDocGen::startFonts(arg->attrib.lower(),"stringliteral",ol); // write in|out
    ol.docify(" ");
    VhdlDocGen::formatString(arg->type,ol,mdef);
    sem=TRUE;
    ol.endBold();
    if (len > 2)
    { 
      ol.lineBreak();
      ol.docify("  ");
    }
  }//for

  ol.docify(" )");    


}

/*!
 * writes a function prototype to the output
 */

void VhdlDocGen::writeFunctionProto(OutputList& ol,const ArgumentList* al,const MemberDef* mdef)
{
  if (al==0) return;
  ArgumentListIterator ali(*al);
  Argument *arg;
  bool sem=FALSE;
  int len=al->count();
  ol.startBold();
  ol.docify(" ( ");    
  ol.endBold();
  if (len>2)
  {
    ol.lineBreak();
  }
  for (;(arg=ali.current());++ali)
  {
    ol.startBold();
    if (sem && len < 3)
    {
      ol.docify(" , ");
    }
    QCString att=arg->defval;
    if (!att.isEmpty()) 
    { 
      QCString *str=VhdlDocGen::findKeyWord(att);
      att+=" ";
      if (str)
	VhdlDocGen::formatString(att,ol,mdef);
      else
	VhdlDocGen::startFonts(att,"vhdlchar",ol);         
    }  

    QCString nn=arg->name;
    nn+=": ";
    QCString ss=arg->type; //.lower();
    QCString w=ss;//.upper();
    VhdlDocGen::startFonts(nn,"vhdlchar",ol);
    VhdlDocGen::startFonts("in ","stringliteral",ol);
    QCString *str=VhdlDocGen::findKeyWord(ss);
    if (str)
      VhdlDocGen::formatString(w,ol,mdef);
    else
      VhdlDocGen::startFonts(w,"vhdlchar",ol);         

    sem=TRUE;    
    ol.endBold();
    if (len > 2)    
    {
      ol.lineBreak();
    }
  }
  ol.startBold();    
  ol.docify(" )");    
  const char *exp=mdef->excpString();
  if (exp)
  {
    ol.insertMemberAlign();
    ol.docify("[ ");
    ol.docify(exp);
    ol.docify(" ]");
  }
  ol.endBold();  
}

/*!
 * writes a process prototype to the output
 */

void VhdlDocGen::writeProcessProto(OutputList& ol,const ArgumentList* al,const MemberDef* mdef)
{
  if (al==0) return;
  ArgumentListIterator ali(*al);
  Argument *arg;
  bool sem=FALSE;
  ol.startBold();
  ol.docify(" ( ");    
  for (;(arg=ali.current());++ali)
  {
    if (sem)
      ol.docify(" , ");
    QCString nn=arg->name;
    // VhdlDocGen::startFonts(nn,"vhdlchar",ol);
    VhdlDocGen::writeFormatString(nn,ol,mdef);
    sem=TRUE;    
  }    
  ol.docify(" )");    
  ol.endBold();
}


/*!
 * writes a function|procedure documentation to the output
 */

void VhdlDocGen::writeFuncProcDocu(
    const MemberDef *md, 
    OutputList& ol,
    const ArgumentList* al,
    bool /*type*/)
{
  if (al==0) return;
  bool sem=FALSE;    
  ol.enableAll();

  ArgumentListIterator ali(*al);
  int index=ali.count();
  if (index==0)
  {
    ol.docify(" ( ) ");
    return;
  }
  ol.startParameterList(TRUE); 
  Argument *arg;
  bool first=TRUE;
  for (;(arg=ali.current());++ali)
  { 
    ol.startParameterType(first,"");  
    //if (first) ol.writeChar('(');
    if (VhdlDocGen::isProcedure(md))
    {
      startFonts(arg->defval,"keywordtype",ol);
      ol.docify(" ");
    } 
    ol.endParameterType();

    ol.startParameterName(TRUE);
    VhdlDocGen::writeFormatString(arg->name,ol,md);
    ol.docify(" ");

    if (VhdlDocGen::isProcedure(md))
      startFonts(arg->attrib,"stringliteral",ol);
    else if (VhdlDocGen::isVhdlFunction(md))
      startFonts(QCString("in"),"stringliteral",ol);


    ol.docify(" ");
    ol.disable(OutputGenerator::Man);
    ol.startEmphasis();
    ol.enable(OutputGenerator::Man);
    if (!VhdlDocGen::isProcess(md))
      startFonts(arg->type,"vhdlkeyword",ol);
    ol.disable(OutputGenerator::Man);
    ol.endEmphasis();
    ol.enable(OutputGenerator::Man);

    if (--index) 
    {
      ol.docify(" , ");    
      ol.endParameterName(FALSE,FALSE,FALSE);
    }
    else
    {
      //ol.docify(" ) ");
      ol.endParameterName(TRUE,FALSE,TRUE);
    }

    sem=TRUE;        
    first=FALSE;
  }
  //ol.endParameterList(); 

} // writeDocFunProc

/*!
 * returns TRUE if this string is a function prototype or 
 * FALSE if this is a procedure
 */

bool VhdlDocGen::isFunctionProto(QCString& ss)
{
  QCString name=ss;
  QCString proc("procedure");
  QCString func("function");
  name=name.stripWhiteSpace();
  QStringList ql=QStringList::split(QRegExp("[\\s]"),name,FALSE);
  int j=ql.count();
  if (j<2) return FALSE;
  QCString tt=(QCString)ql[0].lower();

  if (tt=="impure" || tt=="pure") tt=ql[1];

  if (VhdlDocGen::compareString(tt,proc)!=0 && VhdlDocGen::compareString(tt,func)!=0) 
    return FALSE;

  QCString temp=(QCString)ql[j-1];
  temp=temp.stripWhiteSpace();
  if (stricmp(temp.data(),"is")==0)
  {
    VhdlDocGen::deleteCharRev(name,'s');
    VhdlDocGen::deleteCharRev(name,'i');
    ss=name;
    return TRUE;
  }
  return FALSE;
}

QCString VhdlDocGen::convertArgumentListToString(const ArgumentList* al,bool func)
{
  QCString argString;
  bool sem=FALSE;
  ArgumentListIterator ali(*al);
  Argument *arg;

  for (;(arg=ali.current());++ali)
  {
    if (sem) argString.append(", ");
    if (func)
    {
      argString+=arg->name;
      argString+=":";
      argString+=arg->type; 
    }
    else
    {
      argString+=arg->defval+" ";
      argString+=arg->name+" :";
      argString+=arg->attrib+" ";
      argString+=arg->type;
    } 
    sem=TRUE;
  }
  return argString;
}


void VhdlDocGen::writeVhdlDeclarations(MemberList* ml,
                        OutputList& ol,GroupDef* gd,ClassDef* cd,FileDef *fd)
{
  static ClassDef *cdef;
  //static GroupDef* gdef;
  if (cd && cdef!=cd)
  { // only one inline link
    VhdlDocGen::writeInlineClassLink(cd,ol);
    cdef=cd;
  }

  /*
  if (gd && gdef==gd) return;
  if (gd && gdef!=gd)
  {
    gdef=gd;
  }
*/
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::LIBRARY,FALSE),0,FALSE,VhdlDocGen::LIBRARY);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::USE,FALSE),0,FALSE,VhdlDocGen::USE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::FUNCTION,FALSE),0,FALSE,VhdlDocGen::FUNCTION);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::COMPONENT,FALSE),0,FALSE,VhdlDocGen::COMPONENT);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::CONSTANT,FALSE),0,FALSE,VhdlDocGen::CONSTANT);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::TYPE,FALSE),0,FALSE,VhdlDocGen::TYPE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::SUBTYPE,FALSE),0,FALSE,VhdlDocGen::SUBTYPE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::GENERIC,FALSE),0,FALSE,VhdlDocGen::GENERIC);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::PORT,FALSE),0,FALSE,VhdlDocGen::PORT);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::PROCESS,FALSE),0,FALSE,VhdlDocGen::PROCESS);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::SIGNAL,FALSE),0,FALSE,VhdlDocGen::SIGNAL);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::ATTRIBUTE,FALSE),0,FALSE,VhdlDocGen::ATTRIBUTE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::PROCEDURE,FALSE),0,FALSE,VhdlDocGen::PROCEDURE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::RECORD,FALSE),0,FALSE,VhdlDocGen::RECORD);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::UNITS,FALSE),0,FALSE,VhdlDocGen::UNITS);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::SHAREDVARIABLE,FALSE),0,FALSE,VhdlDocGen::SHAREDVARIABLE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::VFILE,FALSE),0,FALSE,VhdlDocGen::VFILE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::GROUP,FALSE),0,FALSE,VhdlDocGen::GROUP);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::COMPONENT_INST,FALSE),0,FALSE,VhdlDocGen::COMPONENT_INST);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::ALIAS,FALSE),0,FALSE,VhdlDocGen::ALIAS);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,0,gd,theTranslator_vhdlType(VhdlDocGen::MISCELLANEOUS),0,FALSE,VhdlDocGen::MISCELLANEOUS);
  
  // configurations must be added to global file definitions.  
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,0,fd,gd,theTranslator_vhdlType(VhdlDocGen::CONFIG,FALSE),0,FALSE,VhdlDocGen::CONFIG);
}

static void setGlobalType(MemberList *ml)
{
  if (ml==0) return;
  MemberDef *mdd=0;
  MemberListIterator mmli(*ml);
  for ( ; (mdd=mmli.current()); ++mmli )
  {
    if (stricmp(mdd->argsString(),"configuration")==0)
    {
      mdd->setMemberSpecifiers(VhdlDocGen::CONFIG);        
    }
    else if (stricmp(mdd->typeString(),"library")==0)
    {
      mdd->setMemberSpecifiers(VhdlDocGen::LIBRARY);        
    }
    else if (stricmp(mdd->typeString(),"package")==0)
    {
      mdd->setMemberSpecifiers(VhdlDocGen::USE);        
    } 
    else if (stricmp(mdd->typeString(),"misc")==0)
    {
      mdd->setMemberSpecifiers(VhdlDocGen::MISCELLANEOUS);        
    } 
  }
}

/* writes a vhdl type documentation */
void VhdlDocGen::writeVHDLTypeDocumentation(const MemberDef* mdef, const Definition *d, OutputList &ol)
{
  ClassDef *cd=(ClassDef*)d;
  if (cd==0) return;
  if ((VhdlDocGen::isVhdlFunction(mdef) || VhdlDocGen::isProcedure(mdef) || VhdlDocGen::isProcess(mdef)))
  {
    QCString nn=mdef->typeString();
    nn=nn.stripWhiteSpace();
    QCString na=cd->name();
    MemberDef* memdef=VhdlDocGen::findMember(na,nn);
    if (memdef && memdef->isLinkable()) 
    { 
      ol.docify(" ");
      ol.startBold();
      //ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,mdef->typeString());                
      writeLink(memdef,ol);
      ol.endBold();
      ol.docify(" ");
    }
    else
    {
      ol.docify(" ");
      QCString ttype=mdef->typeString();
      VhdlDocGen::formatString(ttype,ol,mdef);
      ol.docify(" "); 
    }
    ol.docify(mdef->name());
    VhdlDocGen::writeFuncProcDocu(mdef,ol, mdef->argumentList().pointer());
  }

  if(VhdlDocGen::isMisc(mdef))
  {
    writeLink(mdef,ol);
    return;
  }
  if (mdef->isVariable())
  { 
    //ol.docify(mdef->name().data());
    writeLink(mdef,ol);
    ol.docify(" ");
    QCString ttype=mdef->typeString();
    VhdlDocGen::formatString(ttype,ol,mdef);
    ol.docify(" ");
    if (VhdlDocGen::isPort(mdef))
    {
      QCString largs=mdef->argsString();
      VhdlDocGen::formatString(largs,ol,mdef);
      ol.docify(" ");
    }
  }
}

/* writes a vhdl type declaration */

void VhdlDocGen::writeVHDLDeclaration(MemberDef* mdef,OutputList &ol,
    ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
    bool /*inGroup*/) 
{

  LockingPtr<MemberDef> lock(mdef,mdef);

  Definition *d=0;

  /* some vhdl files contain only a configuration  description

     library work;
     configuration cfg_tb_jtag_gotoBackup of tb_jtag_gotoBackup is
     for RTL
     end for;
     end cfg_tb_jtag_gotoBackup;

     in this case library work does not belong to an entity, package ...

   */

  ASSERT(cd!=0 || nd!=0 || fd!=0 || gd!=0 || 
         mdef->getMemberSpecifiers()==VhdlDocGen::LIBRARY || 
         mdef->getMemberSpecifiers()==VhdlDocGen::USE   
        ); // member should belong to something
  if (cd) d=cd; 
  else if (nd) d=nd; 
  else if (fd) d=fd; 
  else if (gd) d=gd; 
  else d=(Definition*)mdef;

  // write tag file information of this member
  if (!Config_getString("GENERATE_TAGFILE").isEmpty())
  {
    Doxygen::tagFile << "    <member kind=\"";
    if      (VhdlDocGen::isGeneric(mdef))      Doxygen::tagFile << "generic";
    if      (VhdlDocGen::isPort(mdef))         Doxygen::tagFile << "port";
    if      (VhdlDocGen::isEntity(mdef))       Doxygen::tagFile << "entity";
    if      (VhdlDocGen::isComponent(mdef))    Doxygen::tagFile << "component";
    if      (VhdlDocGen::isVType(mdef))        Doxygen::tagFile << "type";
    if      (VhdlDocGen::isConstant(mdef))     Doxygen::tagFile << "constant";
    if      (VhdlDocGen::isSubType(mdef))      Doxygen::tagFile << "subtype";
    if      (VhdlDocGen::isVhdlFunction(mdef)) Doxygen::tagFile << "function";
    if      (VhdlDocGen::isProcedure(mdef))    Doxygen::tagFile << "procedure";
    if      (VhdlDocGen::isProcess(mdef))      Doxygen::tagFile << "process";
    if      (VhdlDocGen::isSignals(mdef))      Doxygen::tagFile << "signal";
    if      (VhdlDocGen::isAttribute(mdef))    Doxygen::tagFile << "attribute";
    if      (VhdlDocGen::isRecord(mdef))       Doxygen::tagFile << "record";
    if      (VhdlDocGen::isLibrary(mdef))      Doxygen::tagFile << "library";
    if      (VhdlDocGen::isPackage(mdef))      Doxygen::tagFile << "package";
    if      (VhdlDocGen::isVariable(mdef))     Doxygen::tagFile << "shared variable";
    if      (VhdlDocGen::isFile(mdef))         Doxygen::tagFile << "file";
    if      (VhdlDocGen::isGroup(mdef))        Doxygen::tagFile << "group";
    if      (VhdlDocGen::isCompInst(mdef))     Doxygen::tagFile << "component instantiation";
    if      (VhdlDocGen::isAlias(mdef))        Doxygen::tagFile << "alias";
    if      (VhdlDocGen::isCompInst(mdef))     Doxygen::tagFile << "configuration";

    Doxygen::tagFile << "\">" << endl;
    Doxygen::tagFile << "      <type>" << convertToXML(mdef->typeString()) << "</type>" << endl;
    Doxygen::tagFile << "      <name>" << convertToXML(mdef->name()) << "</name>" << endl;
    Doxygen::tagFile << "      <anchorfile>" << convertToXML(mdef->getOutputFileBase()+Doxygen::htmlFileExtension) << "</anchorfile>" << endl;
    Doxygen::tagFile << "      <anchor>" << convertToXML(mdef->anchor()) << "</anchor>" << endl;

    if (VhdlDocGen::isVhdlFunction(mdef))
      Doxygen::tagFile << "      <arglist>" << convertToXML(VhdlDocGen::convertArgumentListToString(mdef->argumentList().pointer(),TRUE)) << "</arglist>" << endl;
    else if (VhdlDocGen::isProcedure(mdef))
      Doxygen::tagFile << "      <arglist>" << convertToXML(VhdlDocGen::convertArgumentListToString(mdef->argumentList().pointer(),FALSE)) << "</arglist>" << endl;
    else
      Doxygen::tagFile << "      <arglist>" << convertToXML(mdef->argsString()) << "</arglist>" << endl;

    mdef->writeDocAnchorsToTagFile();
    Doxygen::tagFile << "    </member>" << endl;

  }

  // write search index info
  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(mdef->qualifiedName(),mdef->getOutputFileBase(),mdef->anchor());
    Doxygen::searchIndex->addWord(mdef->localName(),TRUE);
    Doxygen::searchIndex->addWord(mdef->qualifiedName(),FALSE);
  }

  QCString cname  = d->name();
  QCString cfname = mdef->getOutputFileBase();

  //HtmlHelp *htmlHelp=0;
  //  bool hasHtmlHelp = Config_getBool("GENERATE_HTML") && Config_getBool("GENERATE_HTMLHELP");
  //  if (hasHtmlHelp) htmlHelp = HtmlHelp::getInstance();

  // search for the last anonymous scope in the member type
  ClassDef *annoClassDef=mdef->getClassDefOfAnonymousType();

  // start a new member declaration
  bool isAnonymous = annoClassDef; // || m_impl->annMemb || m_impl->annEnumType;
  ///printf("startMemberItem for %s\n",name().data());
  ol.startMemberItem( isAnonymous ); //? 1 : m_impl->tArgList ? 3 : 0);

  // If there is no detailed description we need to write the anchor here.
  bool detailsVisible = mdef->isDetailedSectionLinkable();
  if (!detailsVisible) // && !m_impl->annMemb)
  {
    QCString doxyName=mdef->name().copy();
    if (!cname.isEmpty()) doxyName.prepend(cname+"::");
    QCString doxyArgs=mdef->argsString();
    ol.startDoxyAnchor(cfname,cname,mdef->anchor(),doxyName,doxyArgs);

    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Man);
    ol.disable(OutputGenerator::Latex);
    ol.docify("\n");
    ol.popGeneratorState();

  }
  // *** write type
  /*VHDL CHANGE */
  QCString ltype(mdef->typeString()); 
  QCString largs(mdef->argsString());
  int mm=mdef->getMemberSpecifiers();
  //printf(":: ltype=%s largs=%s name=%s mm=%d\n",
  //    ltype.data(),largs.data(),mdef->name().data(),mm);

  ClassDef *kl=0;
  //FileDef *fdd=0;
  LockingPtr<ArgumentList> alp = mdef->argumentList();
  QCString nn;
  if (gd) gd=0;
  switch(mm)
  {
    case VhdlDocGen::MISCELLANEOUS:
      VhdlDocGen::writeCodeFragment(mdef,ol);
      break;
    case VhdlDocGen::PROCEDURE:
    case VhdlDocGen::FUNCTION: 
      ol.startBold();
      VhdlDocGen::formatString(ltype,ol,mdef);
      ol.endBold();
      ol.insertMemberAlign();
      ol.docify(" ");
	
      writeLink(mdef,ol);
      if (alp!=0 && mm==VhdlDocGen::FUNCTION)
	VhdlDocGen::writeFunctionProto(ol,alp.pointer(),mdef);

      if (alp!=0 && mm==VhdlDocGen::PROCEDURE)
	VhdlDocGen::writeProcedureProto(ol,alp.pointer(),mdef);

      break;
    case VhdlDocGen::USE:
      kl=VhdlDocGen::getClass(mdef->name());
      if (kl && ((VhdlDocGen::VhdlClasses)kl->protection()==VhdlDocGen::ENTITYCLASS)) break;
      writeLink(mdef,ol);  
      ol.insertMemberAlign();
      ol.docify("  ");

      if (kl)
      {
        nn=kl->getOutputFileBase();
        ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::Html);
        ol.docify(" ");
        QCString name=theTranslator_vhdlType(VhdlDocGen::PACKAGE,TRUE);
        ol.startBold();
        ol.docify(name.data()); 
        name.resize(0);
        ol.endBold();
        name+=" <"+mdef->name()+">";
        ol.startEmphasis();
        ol.writeObjectLink(kl->getReference(),kl->getOutputFileBase(),0,name.data());
        ol.popGeneratorState();
      }
      break;
    case VhdlDocGen::LIBRARY:
      writeLink(mdef,ol);
      ol.insertMemberAlign();
      break;
    case VhdlDocGen::GENERIC:
    case VhdlDocGen::PORT:
      writeLink(mdef,ol);
      ol.docify(" ");
      ol.insertMemberAlign();
      if (mm==VhdlDocGen::GENERIC)
      {
	ol.startBold();
	VhdlDocGen::formatString(ltype,ol,mdef);
	ol.endBold();
      }
      else
      {
	ol.docify(" ");
	ol.startBold();
	ol.docify(mdef->typeString());
	ol.endBold();
	ol.docify(" ");
	VhdlDocGen::formatString(largs,ol,mdef);
      }
      break;
    case VhdlDocGen::PROCESS:
      writeLink(mdef,ol);  
      ol.insertMemberAlign();
      VhdlDocGen::writeProcessProto(ol,alp.pointer(),mdef);
      break;
    case VhdlDocGen::PACKAGE:
    case VhdlDocGen::ENTITY:
    case VhdlDocGen::COMPONENT:
    case VhdlDocGen::COMPONENT_INST:
    case VhdlDocGen::CONFIG:
      writeLink(mdef,ol);
      ol.insertMemberAlign();
      ol.docify("  ");
       
      ol.startBold();
      ol.docify(ltype);
      ol.endBold();
      ol.docify("  ");
      if (VhdlDocGen::isComponent(mdef) || 
          VhdlDocGen::isConfig(mdef)    ||
          VhdlDocGen::isCompInst(mdef))
      {
        if (VhdlDocGen::isConfig(mdef) || VhdlDocGen::isCompInst(mdef))
        {
          nn=ltype;
        }
        else
        {
	  nn=mdef->name();
        }
	kl=getClass(nn.data());
	if (kl) 
	{
	  nn=kl->getOutputFileBase();
	  ol.pushGeneratorState();
	  ol.disableAllBut(OutputGenerator::Html);
	  ol.startEmphasis();
          QCString name("<Entity ");
          if (VhdlDocGen::isConfig(mdef) || VhdlDocGen::isCompInst(mdef))
          {
            name+=ltype+">";
          }
          else
          {
            name+=mdef->name()+"> ";
          }
	  ol.writeObjectLink(kl->getReference(),kl->getOutputFileBase(),0,name.data());
	  ol.endEmphasis();
	  ol.popGeneratorState();
	}
      } 
      break;
    case VhdlDocGen::SIGNAL:
    case VhdlDocGen::ATTRIBUTE:
    case VhdlDocGen::TYPE:
    case VhdlDocGen::SUBTYPE:
    case VhdlDocGen::CONSTANT:      
    case VhdlDocGen::SHAREDVARIABLE:    
    case VhdlDocGen::VFILE:
    case VhdlDocGen::GROUP: 
    case VhdlDocGen::ALIAS: 
      writeLink(mdef,ol);
      ol.docify(" ");
      ol.insertMemberAlign();
      VhdlDocGen::formatString(ltype,ol,mdef);
      break;
    case VhdlDocGen::RECORD:    
      writeLink(mdef,ol);
      ol.docify(" ");
      ol.startBold();
      if (ltype.isEmpty()) ol.docify("     : record");    
      ol.insertMemberAlign();
      if (!ltype.isEmpty()) 
	VhdlDocGen::formatString(ltype,ol,mdef);
      ol.endBold();
      break;
    case VhdlDocGen::UNITS:
      ol.startBold();
      writeLink(mdef,ol);
      ol.docify(" ");
      if (ltype.isEmpty()) ol.docify("     : unit");    
      ol.insertMemberAlign();
      if (!ltype.isEmpty())
	VhdlDocGen::formatString(ltype,ol,mdef);
      ol.endBold();    
      break;
    default: break;
  }

  bool htmlOn = ol.isEnabled(OutputGenerator::Html);
  if (htmlOn && Config_getBool("HTML_ALIGN_MEMBERS") && !ltype.isEmpty())
  {
    ol.disable(OutputGenerator::Html);
  }
  if (!ltype.isEmpty()) ol.docify(" ");

  if (htmlOn) 
  {
    ol.enable(OutputGenerator::Html);
  }

  if (!detailsVisible)// && !m_impl->annMemb)
  {
    ol.endDoxyAnchor(cfname,mdef->anchor());
  }

  //printf("endMember %s annoClassDef=%p annEnumType=%p\n",
  //    name().data(),annoClassDef,annEnumType);
  ol.endMemberItem();
  if (!mdef->briefDescription().isEmpty() &&   Config_getBool("BRIEF_MEMBER_DESC") /* && !annMemb */)
  {
    ol.startMemberDescription();
    ol.parseDoc(mdef->briefFile(),mdef->briefLine(),
                mdef->getOuterScope()?mdef->getOuterScope():d,
                mdef,mdef->briefDescription(),TRUE,FALSE,0,TRUE,FALSE);
    if (detailsVisible) 
    {
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      //ol.endEmphasis();
      ol.docify(" ");
      if (mdef->getGroupDef()!=0 && gd==0) // forward link to the group
      {
	ol.startTextLink(mdef->getOutputFileBase(),mdef->anchor());
      }
      else // local link
      {
	ol.startTextLink(0,mdef->anchor());
      }
      ol.endTextLink();
      //ol.startEmphasis();
      ol.popGeneratorState();
    }
    //ol.newParagraph();
    ol.endMemberDescription();
  }
  mdef->warnIfUndocumented();

}// end writeVhdlDeclaration


void VhdlDocGen::writeLink(const MemberDef* mdef,OutputList &ol) 
{
  ol.writeObjectLink(mdef->getReference(),
                     mdef->getOutputFileBase(),
		     mdef->anchor(),
		     mdef->name());
}

void VhdlDocGen::writePlainVHDLDeclarations(
    MemberList* mlist,OutputList &ol,
    ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,int specifier)
{

  SDict<QCString> pack(1009);

  ol.pushGeneratorState();

  bool first=TRUE;
  MemberDef *md;
  MemberListIterator mli(*mlist);
  for ( ; (md=mli.current()); ++mli )
  { 
    int mems=md->getMemberSpecifiers();
    if (md->isBriefSectionVisible() && (mems==specifier) && (mems!=VhdlDocGen::LIBRARY) )
    {
      if (first) {ol.startMemberList();first=FALSE;}
      VhdlDocGen::writeVHDLDeclaration(md,ol,cd,nd,fd,gd,FALSE);
    } //if
    else if (md->isBriefSectionVisible() && (mems==specifier))
    {
      if (!pack.find(md->name().data()))
      {
	if (first) ol.startMemberList(),first=FALSE;
	VhdlDocGen::writeVHDLDeclaration(md,ol,cd,nd,fd,gd,FALSE);
	pack.append(md->name().data(),new QCString(md->name().data()));
      }
    } //if
  } //for
  if (!first) ol.endMemberList(); 
  pack.clear();
}//plainDeclaration

bool VhdlDocGen::membersHaveSpecificType(MemberList *ml,int type)
{
  if (ml==0) return FALSE;
  MemberDef *mdd=0;
  MemberListIterator mmli(*ml);
  for ( ; (mdd=mmli.current()); ++mmli )
  {
    if (mdd->getMemberSpecifiers()==type) //is type in class
    {
      return TRUE;
    }
  }
  if (ml->getMemberGroupList())
  {
    MemberGroupListIterator mgli(*ml->getMemberGroupList());
    MemberGroup *mg;
    while ((mg=mgli.current()))
    {
      if (mg->members())
      {
        if (membersHaveSpecificType(mg->members(),type)) return TRUE;
      }
      ++mgli;
    }
  }
  return FALSE;
}

void VhdlDocGen::writeVHDLDeclarations(MemberList* ml,OutputList &ol,
    ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
    const char *title,const char *subtitle,bool /*showEnumValues*/,int type) 
{
  setGlobalType(ml);
  if (!membersHaveSpecificType(ml,type)) return;

  if (title) 
  {
    ol.startMemberHeader(title);
    ol.parseText(title);
    ol.endMemberHeader();
    ol.docify(" ");
  }
  if (subtitle && subtitle[0]!=0) 
  {
    ol.startMemberSubtitle();
    ol.parseDoc("[generated]",-1,0,0,subtitle,FALSE,FALSE,0,TRUE,FALSE);
    ol.endMemberSubtitle();
  } //printf("memberGroupList=%p\n",memberGroupList);

  VhdlDocGen::writePlainVHDLDeclarations(ml,ol,cd,nd,fd,gd,type);

  if (ml->getMemberGroupList())
  {
    MemberGroupListIterator mgli(*ml->getMemberGroupList());
    MemberGroup *mg;
    while ((mg=mgli.current()))
    {
      if (membersHaveSpecificType(mg->members(),type))
      {
        //printf("mg->header=%s\n",mg->header().data());
        bool hasHeader=mg->header()!="[NOHEADER]";
        ol.startMemberGroupHeader(hasHeader);
        if (hasHeader)
        {
          ol.parseText(mg->header());
        }
        ol.endMemberGroupHeader();
        if (!mg->documentation().isEmpty())
        {
          //printf("Member group has docs!\n");
          ol.startMemberGroupDocs();
          ol.parseDoc("[generated]",-1,0,0,mg->documentation()+"\n",FALSE,FALSE);
          ol.endMemberGroupDocs();
        }
        ol.startMemberGroup();
        //printf("--- mg->writePlainDeclarations ---\n");
        VhdlDocGen::writePlainVHDLDeclarations(mg->members(),ol,cd,nd,fd,gd,type);
        ol.endMemberGroup(hasHeader);
      }
      ++mgli;
    }
  }
}// writeVHDLDeclarations

/* strips the prefix for record and unit members*/
void VhdlDocGen::adjustRecordMember(MemberDef *mdef) 
{ //,OutputList & ol) {
  QRegExp regg("[_a-zA-Z]");
  QCString nn=mdef->name();
  int j=nn.find(regg,0);
  if (j>0)
  {
    nn=nn.mid(j,nn.length());
    mdef->setName(nn.data());
  }
}//adjustRecordMember

/* strips the prefix for package and package body */

bool VhdlDocGen::writeClassType( ClassDef * cd,
                                 OutputList &ol ,QCString & cname)
{
  int id=cd->protection();
  QCString qcs = VhdlDocGen::trTypeString(id+2);
  cname=VhdlDocGen::getClassName(cd);
  ol.startBold();
  ol.writeString(qcs.data());
  ol.writeString(" ");
  ol.endBold();
  return FALSE;
}// writeClassLink

QCString VhdlDocGen::trVhdlType(int type,bool sing)
{
  switch(type)
  {
    case VhdlDocGen::LIBRARY:      
      if (sing) return "Library";
      else      return "Libraries"; 
    case VhdlDocGen::PACKAGE:      
      if (sing) return "Package";       
      else      return "Packages";
    case VhdlDocGen::SIGNAL:       
      if (sing) return "Signal";        
      else      return "Signals";
    case VhdlDocGen::COMPONENT:    
      if (sing) return "Component";     
      else      return "Components";        
    case VhdlDocGen::CONSTANT:     
      if (sing) return "Constant";      
      else      return "Constants";
    case VhdlDocGen::ENTITY:       
      if (sing) return "Entity";       
      else      return "Entities";
    case VhdlDocGen::TYPE:         
      if (sing) return "Type";          
      else      return "Types";
    case VhdlDocGen::SUBTYPE:      
      if (sing) return "Subtype";       
      else      return "Subtypes";
    case VhdlDocGen::FUNCTION:     
      if (sing) return "Function";      
      else      return "Functions";
    case VhdlDocGen::RECORD:       
      if (sing) return "Record";       
      else      return "Records";
    case VhdlDocGen::PROCEDURE:   
      if (sing) return "Procedure";     
      else      return "Procedures"; 
    case VhdlDocGen::ARCHITECTURE: 
      if (sing) return "Architecture";  
      else      return "Architectures";
    case VhdlDocGen::ATTRIBUTE:    
      if (sing) return "Attribute";     
      else      return "Attributes";
    case VhdlDocGen::PROCESS:      
      if (sing) return "Process";       
      else      return "Processes";
    case VhdlDocGen::PORT:         
      if (sing) return "Port";          
      else      return "Ports";
    case VhdlDocGen::USE:          
      if (sing) return "Package";       
      else      return "Packages";
    case VhdlDocGen::GENERIC:      
      if (sing) return "Generic";       
      else      return "Generics";
    case VhdlDocGen::PACKAGE_BODY: 
      return "Package Body";
    case VhdlDocGen::DOCUMENT:     
      return "Doc"; 
    case VhdlDocGen::UNITS:        
      return "Units"; 
    case VhdlDocGen::SHAREDVARIABLE:     
      if (sing) return "Shared Variable";       
      return "Shared Variables"; 
    case VhdlDocGen::VFILE:        
      if (sing) return "File";       
      return "Files"; 
    case VhdlDocGen::GROUP:        
      if (sing) return "Group";       
      return "Groups";
    case VhdlDocGen::COMPONENT_INST:       
      if (sing) return "Component Instantiation";       
      else      return "Component Instantiations";
    case VhdlDocGen::ALIAS:        
      if (sing) return "Alias";       
      return "Aliases";
    case VhdlDocGen::CONFIG:        
      if (sing) return "Configuration";       
      return "Configurations";
     case VhdlDocGen::MISCELLANEOUS:
      return "Miscellaneous";  
    default:                       
      return "Class";
  }
}

QCString VhdlDocGen::trDesignUnitHierarchy()
{
  return "Design Unit Hierarchy";
}

QCString VhdlDocGen::trDesignUnitList()
{
  return "Design Unit List";
}

QCString VhdlDocGen::trDesignUnitMembers()
{
  return "Design Unit Members";
}

QCString VhdlDocGen::trDesignUnitListDescription()
{
  return "Here is a list of all design unit members with links to "
         "the Entities and Packages they belong to:"; 
}

QCString VhdlDocGen::trDesignUnitIndex()
{
  return "Design Unit Index";
}

QCString VhdlDocGen::trDesignUnits()
{
  return "Design Units";
}

QCString VhdlDocGen::trFunctionAndProc()
{
  return "Functions/Procedures/Processes";
}



/*! adds  documentation to a function/procedure */
bool VhdlDocGen::writeDoc(EntryNav* rootNav)
{
  Entry *e=rootNav->entry();
  //if (e->section==Entry::Entry::OVERLOADDOC_SEC) 
  if (stricmp(e->type.data(),"function")==0)
  {
    VhdlDocGen::addFuncDoc(rootNav);     
  }

  return FALSE;
}// writeDoc


/* do not insert the same component twice */

bool VhdlDocGen::foundInsertedComponent(const QCString & name,Entry* root)
{
  QListIterator<BaseInfo> bii(*root->extends);
  BaseInfo *bi=0;
  for (bii.toFirst();(bi=bii.current());++bii) 
  {
    if (bi->name==name)
    {
      return TRUE; // 
    }
  }

  return FALSE;
}// found component

/*! writes a link if the string is linkable else a formatted string */

void VhdlDocGen::writeStringLink(const MemberDef *mdef,QCString mem, OutputList& ol)
{
  if (mdef) 
  {
    ClassDef *cd=mdef->getClassDef();
    if (cd)
    { 
      QCString n=cd->name(); 
      MemberDef* memdef=VhdlDocGen::findMember(n,mem);
      if (memdef && memdef->isLinkable()) 
      { 
	ol.startBold();
	writeLink(memdef,ol);                
	ol.endBold();
	ol.docify(" ");
	return;
      }
    }
  }
  VhdlDocGen::startFonts(mem,"vhdlchar",ol);    
}// found component

void VhdlDocGen::writeCodeFragment( MemberDef *mdef,OutputList& ol)
{
  //  Definition d=(Definition)mdef;
  //	QCString fdd=mdef->getDefFileExtension();
  //	QCString scope=mdef->getScopeString();
  QCString codeFragment=mdef->documentation();
  //FileDef *fd=mdef->getFileDef();

  //int start=mdef->getStartBodyLine();
  //int end=mdef->getEndBodyLine();
  QStringList qsl=QStringList::split("\n",codeFragment);

  writeLink(mdef,ol);
  ol.docify(" ");
  ol.insertMemberAlign();
  int len = qsl.count();	
  int j;
  for (j=0;j<len;j++)
  {
    QCString q=(QCString)qsl[j];
    VhdlDocGen::writeFormatString(q,ol,mdef);
    ol.lineBreak(); 
    if (j==2) // only the first three lines are shown
    {
      q="...";
      VhdlDocGen::writeFormatString(q,ol,mdef);
      break;
    }
  }
}

void VhdlDocGen::writeSource(MemberDef *mdef,OutputList& ol,QCString & cname)
{
  //  Definition d=(Definition)mdef;
  QCString fdd=mdef->getDefFileExtension();
  QCString scope=mdef->getScopeString();
  QCString codeFragment=mdef->documentation();
  FileDef *fd=mdef->getFileDef();
  int start=mdef->getStartBodyLine();
  int end=mdef->getEndBodyLine();
  QStringList qsl=QStringList::split("\n",codeFragment);

  ParserInterface *pIntf = Doxygen::parserManager->getParser(fdd.data());
  pIntf->resetCodeParserState();

  ol.startParagraph();
  ol.startCodeFragment();
  pIntf->parseCode(ol,               // codeOutIntf
                   scope,            // scope
                   codeFragment,     // input
                   FALSE,            // isExample
                   0,                // exampleName
                   fd,               // fileDef
                   start,            // startLine
                   end,              // endLine
                   TRUE,             // inlineFragment
                   mdef,             // memberDef
                   FALSE             // show line numbers
                   );
  ol.endCodeFragment();
  ol.endParagraph();

  mdef->writeSourceDef(ol,cname);
  mdef->writeSourceRefs(ol,cname);
  mdef->writeSourceReffedBy(ol,cname);
}

