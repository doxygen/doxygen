/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
 * supports VHDL-87/93/2002
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
#include "memberdef.h"
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
#include "vhdlscanner.h"
#include "layout.h"
#include "arguments.h"
#include "portable.h"

#define theTranslator_vhdlType VhdlDocGen::trVhdlType

static QDict<QCString> g_vhdlKeyDict0(17,FALSE);
static QDict<QCString> g_vhdlKeyDict1(17,FALSE);
static QDict<QCString> g_vhdlKeyDict2(17,FALSE);

static QDict<QCString> g_xilinxUcfDict(17,FALSE);

static void initUCF(Entry* root,const char* type,QCString &  qcs,int line,QCString & fileName,QCString & brief);
static void writeUCFLink(const MemberDef* mdef,OutputList &ol);
static void assignConfiguration(VhdlConfNode* ,QCString);
static void assignBinding(VhdlConfNode* conf,QCString label);
static void addInstance(ClassDef* entity, ClassDef* arch, ClassDef *inst,Entry *cur,ClassDef* archBind=NULL);

//---------- create svg ------------------------------------------------------------- 
static void createSVG();
static void startDot(FTextStream &t);
static void startTable(FTextStream &t,const QCString &className);
static QList<MemberDef>* getPorts(ClassDef *cd);
static void writeVhdlEntityToolTip(FTextStream& t,ClassDef *cd);
static void endDot(FTextStream &t);
static void writeTable(QList<MemberDef>* port,FTextStream & t);
static void endTabel(FTextStream &t);
static void writeClassToDot(FTextStream &t,ClassDef* cd);
static void writeVhdlDotLink(FTextStream &t,const QCString &a,const QCString &b,const QCString &style);
static void writeVhdlPortToolTip(FTextStream& t,QList<MemberDef>* port,ClassDef *cd);





//--------------------------------------------------------------------------------------------------
static void codify(FTextStream &t,const char *str)
{

  if (str)
  { 
    const char *p=str;
    char c;
      while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '<':  t << "&lt;"; 
                   break;
        case '>':  t << "&gt;"; 
                   break;
        case '&':  t << "&amp;"; 
                   break;
        case '\'': t << "&#39;";
                   break;
        case '"':  t << "&quot;"; 
                   break;
        default:   t << c;                  
                   break;
      }
    }
  }
}

static void createSVG()
{
    QCString ov =Config_getString("HTML_OUTPUT");
    QCString dir="-o \""+ov+"/vhdl_design_overview.html\"";
    ov+="/vhdl_design.dot";

    QRegExp ep("[\\s]");
    QCString vlargs="-Tsvg \""+ov+"\" "+dir ;

    if (portable_system("dot",vlargs)!=0)
    {
      err("could not create dot file");
    }
}

// Creates a svg image. All in/out/inout  ports are shown with  brief description and direction.
// Brief descriptions for entities are shown too.
void VhdlDocGen::writeOverview()
{
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  bool found=false;
  for ( ; (cd=cli.current()) ; ++cli )
  {
    if ((VhdlDocGen::VhdlClasses)cd->protection()==VhdlDocGen::ENTITYCLASS )
    {
      found=true;
      break;
    }
  }

  if (!found) return;

  QCString ov =Config_getString("HTML_OUTPUT");
  QCString fileName=ov+"/vhdl_design.dot";
  QFile f(fileName);
  QStringList qli;
  FTextStream  t(&f);

  if (!f.open(IO_WriteOnly))
  {
    fprintf(stderr,"Warning: Cannot open file %s for writing\n",fileName.data());
    return;
  }

  startDot(t);

  for (cli.toFirst() ; (cd=cli.current()) ; ++cli )
  {
    if ((VhdlDocGen::VhdlClasses)cd->protection()!=VhdlDocGen::ENTITYCLASS )
    {
      continue;
    }

    QList<MemberDef>* port= getPorts(cd);
    if (port==0) 
    {
      continue;
    }
    if (port->count()==0)
    {
      delete port;
      port=NULL;
      continue;
    }

    startTable(t,cd->name());
    writeClassToDot(t,cd);
    writeTable(port,t);
    endTabel(t);

    writeVhdlPortToolTip(t,port,cd);
    writeVhdlEntityToolTip(t,cd);
    delete port;

    BaseClassList *bl=cd->baseClasses();
    if (bl)
    {
      BaseClassListIterator bcli(*bl);
      BaseClassDef *bcd;
      for ( ; (bcd=bcli.current()) ; ++bcli )
      {
        ClassDef *bClass=bcd->classDef; 
        QCString dotn=cd->name()+":";
        dotn+=cd->name();
        QCString csc=bClass->name()+":";
        csc+=bClass->name();
        //  fprintf(stderr,"\n <%s| %s>",dotn.data(),csc.data());
        writeVhdlDotLink(t,dotn,csc,0);
      }
    }// if bl
  }// for

  endDot(t);
  //  writePortLinks(t);
  f.close();
  createSVG();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

static void startDot(FTextStream &t)
{
  t << " digraph G { \n"; 
  t << "rankdir=LR \n";
  t << "concentrate=true\n";
  t << "stylesheet=\"doxygen.css\"\n";
}

static void endDot(FTextStream &t)
{
  t <<" } \n"; 
}

static void startTable(FTextStream &t,const QCString &className)
{
  t << className <<" [ shape=none , fontname=\"arial\",  fontcolor=\"blue\" , \n"; 
  t << "label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n";
}

static void writeVhdlDotLink(FTextStream &t,
    const QCString &a,const QCString &b,const QCString &style)
{
  t << a << "->" << b;
  if (!style.isEmpty())
  {
    t << "[style=" << style << "];\n";
  }
  t << "\n";
}


static QCString formatBriefNote(const QCString &brief,ClassDef * cd)
{
  QRegExp ep("[\n]");
  QCString vForm;  
  QCString repl("<BR ALIGN=\"LEFT\"/>");
  QCString file=cd->getDefFileName();

  int k=cd->briefLine();

  QStringList qsl=QStringList::split(ep,brief);
  for(uint j=0;j<qsl.count();j++)
  {
    QCString qcs=qsl[j].data();
    vForm+=parseCommentAsText(cd,NULL,qcs,file,k);
    k++;
    vForm+='\n';
  }

  vForm.replace(ep,repl.data());
  return vForm;
}


static void writeVhdlPortToolTip(FTextStream& t,QList<MemberDef>* port,ClassDef *cd)
{

  uint len=port->count();
  MemberDef *md;
  return; //????

  for (uint j=0;j<len;j++)
  {
    md=(MemberDef*)port->at(j);
    QCString brief=md->briefDescriptionAsTooltip();
    if (brief.isEmpty()) continue;

    QCString node="node";
    node+=VhdlDocGen::getRecordNumber();
    t << node <<"[shape=box margin=0.1, label=<\n";
    t<<"<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"2\" >\n ";
    t<<"<TR><TD BGCOLOR=\"lightcyan\"> ";
    t<<brief;
    t<<" </TD></TR></TABLE>>];";
    QCString dotn=cd->name()+":";
    dotn+=md->name();
    //  writeVhdlDotLink(t,dotn,node,"dotted");
  }
}


static void writeVhdlEntityToolTip(FTextStream& t,ClassDef *cd)
{

  QCString brief=cd->briefDescription();

  if (brief.isEmpty()) return;  

  brief=formatBriefNote(brief,cd);

  QCString node="node";
  node+=VhdlDocGen::getRecordNumber();
  t << node <<"[shape=none margin=0.1, label=<\n";
  t << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"2\" >\n ";
  t << "<TR><TD BGCOLOR=\"lightcyan\"> ";
  t << brief;
  t << " </TD></TR></TABLE>>];";
  QCString dotn=cd->name()+":";
  dotn+=cd->name();
  writeVhdlDotLink(t,dotn,node,"dotted");
}

static void writeColumn(FTextStream &t,MemberDef *md,bool start)
{
  QCString toolTip;

  static QRegExp reg("[%]");
  bool bidir=(md!=0 &&( stricmp(md->typeString(),"inout")==0));

  if (md)
  {
    toolTip=md->briefDescriptionAsTooltip();
    if (!toolTip.isEmpty())
    {
      QCString largs = md->argsString();
      if (!largs.isEmpty())
        largs=largs.replace(reg," ");
      toolTip+=" [";
      toolTip+=largs;
      toolTip+="]";	 
    }
  }
  if (start) 
  {
    t <<"<TR>\n";
  }

  t << "<TD ALIGN=\"LEFT\" ";
  if (md)
  {
    t << "href=\"";
    t << md->getOutputFileBase()<< Doxygen::htmlFileExtension;
    t << "#" << md->anchor();
    t<<"\" ";

    t<<" TOOLTIP=\"";
    if(!toolTip.isEmpty())
      codify(t,toolTip.data());
    else{
      QCString largs = md->argsString();
      if(!largs.isEmpty()){ 
        largs=largs.replace(reg," ");
        codify(t,largs.data());
      }
    }
    t << "\" ";

    t << " PORT=\"";
    t << md->name();
    t << "\" ";
  }
  if (!toolTip.isEmpty())
  {
    // if (!toolTip.isEmpty()) 

    if (bidir)
      t << "BGCOLOR=\"orange\">";
    else
      t << "BGCOLOR=\"azure\">";
  }
  else if (bidir)
  {
    t << "BGCOLOR=\"pink\">";
  }
  else
  {
    t << "BGCOLOR=\"lightgrey\">";
  }
  if (md)
  {
    t << md->name();
  }
  else
  {
    t << " \n";
  }
  t << "</TD>\n";

  if (!start)
  {
    t << "</TR>\n";
  }
}

static void endTabel(FTextStream &t)
{
  t << "</TABLE>>\n";
  t << "] \n"; 
}

static void writeClassToDot(FTextStream &t,ClassDef* cd)
{
  t << "<TR><TD COLSPAN=\"2\" BGCOLOR=\"yellow\" ";
  t << "PORT=\"";
  t << cd->name();
  t << "\" ";
  t << "href=\"";
  t << cd->getOutputFileBase() << Doxygen::htmlFileExtension;
  t << "\" ";
  t << ">";
  t << cd->name();
  t << " </TD></TR>\n"; 
}

static QList<MemberDef>* getPorts(ClassDef *cd)
{
  MemberDef* md;
  QList<MemberDef> *portList=new QList<MemberDef>;
  MemberList *ml=cd->getMemberList(MemberList::variableMembers);

  if (ml==0) return NULL;

  MemberListIterator fmni(*ml);

  for (fmni.toFirst();(md=fmni.current());++fmni)
  {
    if (md->getMemberSpecifiers()==VhdlDocGen::PORT)
    {
      portList->append(md);
    }
  } 

  return portList;
}

//writeColumn(FTextStream &t,QCString name,bool start)

static void writeTable(QList<MemberDef>* port,FTextStream & t)
{
  QCString space(" ");
  MemberDef *md;
  uint len=port->count();

  QList<MemberDef> inPorts;
  QList<MemberDef> outPorts;

  uint j;
  for (j=0;j<len;j++)
  {
    md=(MemberDef*)port->at(j);
    QCString qc=md->typeString();
    if(qc=="in")
    {
      inPorts.append(md);
    }
    else
    {
      outPorts.append(md);
    }
  }

  int inp  = inPorts.count();
  int outp = outPorts.count();
  int maxLen;

  if (inp>=outp) 
  {
    maxLen=inp;
  }
  else
  {
    maxLen=outp;
  }

  int i;
  for(i=0;i<maxLen;i++)
  {
    //write inports
    if (i<inp)
    {
      md=(MemberDef*)inPorts.at(i);
      writeColumn(t,md,true);
    }
    else
    {
      writeColumn(t,NULL,true);
    }

    if (i<outp)
    {
      md=(MemberDef*)outPorts.at(i);
      writeColumn(t,md,false);
    }
    else
    {
      writeColumn(t,NULL,false);
    }
  }	
}

//--------------------------------------------------------------------------------------------------


VhdlDocGen::VhdlDocGen()
{
}

VhdlDocGen::~VhdlDocGen()
{
}

void VhdlDocGen::init()
{

 // vhdl keywords inlcuded VHDL 2008
const char* g_vhdlKeyWordMap0[] =
{
  "abs","access","after","alias","all","and","architecture","array","assert","assume","assume_guarantee","attribute",
  "begin","block","body","buffer","bus",
  "case","component","configuration","constant","context","cover",
  "default","disconnect","downto",
  "else","elsif","end","entity","exit",
  "fairness","file","for","force","function",
  "generate","generic","group","guarded",
  "if","impure","in","inertial","inout","is",
  "label","library","linkage","literal","loop",
  "map","mod",
  "nand","new","next","nor","not","null",
  "of","on","open","or","others","out",
  "package","parameter","port","postponed","procedure","process","property","proctected","pure",
  "range","record","register","reject","release","restrict","restrict_guarantee","rem","report","rol","ror","return",
  "select","sequence","severity","signal","shared","sla","sll","sra","srl","strong","subtype",
  "then","to","transport","type",
  "unaffected","units","until","use",
  "variable","vmode","vprop","vunit",
  "wait","when","while","with",
  "xor","xnor",
  0
};

// type
const char* g_vhdlKeyWordMap1[] =
{
  "natural","unsigned","signed","string","boolean", "bit","bit_vector","character",
  "std_ulogic","std_ulogic_vector","std_logic","std_logic_vector","integer",
  "real","float","ufixed","sfixed","time",0
};

// logic
const char* g_vhdlKeyWordMap2[] =
{
  "abs","and","or","not","mod", "xor","rem","xnor","ror","rol","sla",
  "sll",0
};

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
  static  QCString g_vhdlkeyword("vhdlkeyword");
  static  QCString g_vhdltype("comment");
  static  QCString g_vhdllogic("vhdllogic");

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
    {
      return md;
    }
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
    const QCString& package, bool /*type*/)
{
  MemberDef* mdef=0;
  //int funcType;
  ClassDef *cdef=getClass(package.data());
  if (cdef==0) return 0;

  //if (type)
  //  funcType=VhdlDocGen::PROCEDURE;
  //else
  //  funcType=VhdlDocGen::FUNCTION;

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
    type+=theTranslator_vhdlType(VhdlDocGen::ARCHITECTURE,TRUE);
  else if (ii==VhdlDocGen::ARCHITECTURE)
    type+=theTranslator_vhdlType(VhdlDocGen::ENTITY,TRUE);
  else if (ii==VhdlDocGen::PACKAGE_BODY)
    type+=theTranslator_vhdlType(VhdlDocGen::PACKAGE,TRUE);
  else if (ii==VhdlDocGen::PACKAGE)
    type+=theTranslator_vhdlType(VhdlDocGen::PACKAGE_BODY,TRUE);
  else
    type+="";

  //type=type.lower();
  type+=" >> ";
  ol.disable(OutputGenerator::RTF);
  ol.disable(OutputGenerator::Man);

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
    nn=qlist[1].utf8();
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
      QCString s1=qlist[0].utf8();
      QCString s2=qlist[1].utf8();
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
      QCString temp=ql[1].utf8();
      if (stricmp(cd->className().data(),temp.data())==0)
      {
        QCString *cl=new QCString(jj.data());
        qll.insert(0,cl);
      }
    }
  }// for
}//findAllArchitectures

ClassDef* VhdlDocGen::findArchitecture(const ClassDef *cd)
{
  ClassDef *citer;
  QCString nn=cd->name();
  ClassSDict::Iterator cli(*Doxygen::classSDict);

  for ( ; (citer=cli.current()) ; ++cli )
  {
    QCString jj=citer->name();
    QStringList ql=QStringList::split(":",jj,FALSE);
    if (ql.count()>1)
    {
      if (ql[0].utf8()==nn )
      {
        return citer;
      }
    }
  }
  return 0;
}
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
  ol.writeObjectLink(ccd->getReference(),ccd->getOutputFileBase(),0,nn.data());

  if (!behav.isEmpty())
  {
    behav.prepend("  ");
    ol.startBold();
    ol.docify(behav.data());
    ol.endBold();
  }

  ol.lineBreak();
}

bool VhdlDocGen::compareString(const QCString& s1,const QCString& s2)
{
  QCString str1=s1.stripWhiteSpace();
  QCString str2=s2.stripWhiteSpace();

  return stricmp(str1.data(),str2.data());
}


/*!
 * strips the "--" prefixes of vhdl comments
 */
void VhdlDocGen::prepareComment(QCString& qcs)
{
  const char* s="--!";
  //const char *start="--!{";
  //const char *end="--!}";
  int index=0;

  while (TRUE)
  {
    index=qcs.find(s,0,TRUE);
    if (index<0) break;
    qcs=qcs.remove(index,strlen(s));
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
  (void)qlist; //unused
  int index,end;
  QCString s1(text);
  QCString temp;

  index=s1.find("(");
  end=s1.findRev(")");

  if ((end-index)>0)
  {
    QCString tt=s1.mid(index,(end-index+1));
    temp=s1.mid(index+1,(end-index-1));
    //getFuncParams(qlist,temp);
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
  QRegExp reg("[\\s:|]");

  ql=QStringList::split(reg,temp,FALSE);

  if (ql.count() > (unsigned int)index)
  {
    return ql[index].utf8();
  }

  return "";
}


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
    case VhdlDocGen::UNITS:          return "Units";
                                     //case VhdlDocGen::PORTMAP:        return "Port Map";
    case VhdlDocGen::SHAREDVARIABLE: return "Shared Variable";
    case VhdlDocGen::GROUP:          return "Group";
    case VhdlDocGen::VFILE:          return "File";
    case VhdlDocGen::INSTANTIATION: return "Instantiation";
    case VhdlDocGen::ALIAS:          return "Alias";
    case VhdlDocGen::CONFIG:         return "Configuration";
    case VhdlDocGen::MISCELLANEOUS:  return "Miscellaneous";
    case VhdlDocGen::UCF_CONST:      return "Constraints";
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
    QCString qcs=s.remove(index,1);
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
    QCString qcs=s.remove(index,1);
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

void VhdlDocGen::writeFormatString(const QCString& s,OutputList&ol,const MemberDef* mdef)
{
  QRegExp reg("[\\[\\]\\.\\/\\:\\<\\>\\:\\s\\,\\;\\'\\+\\-\\*\\|\\&\\=\\(\\)\"]");
  QCString qcs = s;
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
      if (!find.isEmpty() && find.at(0)=='"')
      {
        int ii=find.find('"',2);
        if (ii>1)
        {
          QCString com=find.left(ii+1);
          VhdlDocGen::startFonts(com,"keyword",ol);
          temp=find.remove(0,ii+1);
        }
      }
      else
      {
        temp=st;
      }
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
  static QRegExp regg("[0-9][0-9eEfFbBcCdDaA_.#-+?xXzZ]*");

  if (s.isEmpty()) return FALSE;
  int j,len;
  j = regg.match(s.data(),0,&len);
  if ((j==0) && (len==(int)s.length())) return TRUE;
  return FALSE;

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

void VhdlDocGen::formatString(const QCString &s, OutputList& ol,const MemberDef* mdef)
{
  QCString qcs = s;
  QCString temp(qcs.length());
  qcs.stripPrefix(":");
  qcs.stripPrefix("is");
  qcs.stripPrefix("IS");
  qcs.stripPrefix("of");
  qcs.stripPrefix("OF");

  // VhdlDocGen::deleteCharRev(qcs,';');
  //char white='\t';
  int len = qcs.length();
  unsigned int index=1;//temp.length();

  for (int j=0;j<len;j++)
  {
    char c=qcs[j];
    char b=c;
    if (j>0) b=qcs[j-1];
    if (c=='"' || c==',' || c=='\''|| c=='(' || c==')'  || c==':' || c=='[' || c==']' ) // || (c==':' && b!='=')) // || (c=='=' && b!='>'))
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
    QCString att=arg->defval;
    bool bGen=att.stripPrefix("gen!");

    if (sem && len < 3)
    {
      ol.docify(" , ");
    }

    if (bGen) {
      VhdlDocGen::formatString(QCString("generic "),ol,mdef);
    }
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
    QCString ss=arg->type.stripWhiteSpace(); //.lower();
    QCString w=ss.stripWhiteSpace();//.upper();
    VhdlDocGen::startFonts(nn,"vhdlchar",ol);
    VhdlDocGen::startFonts("in ","stringliteral",ol);
    QCString *str=VhdlDocGen::findKeyWord(ss);
    if (str)
      VhdlDocGen::formatString(w,ol,mdef);
    else
      VhdlDocGen::startFonts(w,"vhdlchar",ol);

    if (arg->attrib)
      VhdlDocGen::startFonts(arg->attrib,"vhdlchar",ol);


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
    ol.startBold();
    ol.docify("[ ");
    ol.docify(exp);
    ol.docify(" ]");
    ol.endBold();
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
    {
      ol.docify(" , ");
    }
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
  //bool sem=FALSE;
  ol.enableAll();

  ArgumentListIterator ali(*al);
  int index=ali.count();
  if (index==0)
  {
    ol.docify(" ( ) ");
    return;
  }
  ol.startParameterList(TRUE);
  //ol.startParameterName(FALSE);
  Argument *arg;
  bool first=TRUE;
  for (;(arg=ali.current());++ali)
  {
    ol.startParameterType(first,"");
    //   if (first) ol.writeChar('(');
    QCString attl=arg->defval;
    bool bGen=attl.stripPrefix("gen!");
    if (bGen)
      VhdlDocGen::writeFormatString(QCString("generic "),ol,md);


    if (VhdlDocGen::isProcedure(md))
    {
      startFonts(arg->defval,"keywordtype",ol);
      ol.docify(" ");
    }
    ol.endParameterType();

    ol.startParameterName(TRUE);
    VhdlDocGen::writeFormatString(arg->name,ol,md);
    ol.docify(" : ");

    if (VhdlDocGen::isProcedure(md))
    {
      startFonts(arg->attrib,"stringliteral",ol);
    }
    else if (VhdlDocGen::isVhdlFunction(md))
    {
      startFonts(QCString("in"),"stringliteral",ol);
    }

    ol.docify(" ");
    ol.disable(OutputGenerator::Man);
    ol.startEmphasis();
    ol.enable(OutputGenerator::Man);
    if (!VhdlDocGen::isProcess(md))
    {
     // startFonts(arg->type,"vhdlkeyword",ol);
		VhdlDocGen::writeFormatString(arg->type,ol,md);
    }
    ol.disable(OutputGenerator::Man);
    ol.endEmphasis();
    ol.enable(OutputGenerator::Man);

    if (--index)
    {
      ol.docify(" , ");
    }
    else
    {
      //    ol.docify(" ) ");
      ol.endParameterName(TRUE,FALSE,TRUE);
      break;
    }
    ol.endParameterName(FALSE,FALSE,FALSE);

    //sem=TRUE;
    first=FALSE;
  }
  //ol.endParameterList();

} // writeDocFunProc




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
    OutputList& ol,GroupDef* gd,ClassDef* cd,FileDef *fd,NamespaceDef* nd)
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
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::LIBRARY,FALSE),0,FALSE,VhdlDocGen::LIBRARY);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::USE,FALSE),0,FALSE,VhdlDocGen::USE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::FUNCTION,FALSE),0,FALSE,VhdlDocGen::FUNCTION);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::COMPONENT,FALSE),0,FALSE,VhdlDocGen::COMPONENT);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::CONSTANT,FALSE),0,FALSE,VhdlDocGen::CONSTANT);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::TYPE,FALSE),0,FALSE,VhdlDocGen::TYPE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::SUBTYPE,FALSE),0,FALSE,VhdlDocGen::SUBTYPE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::GENERIC,FALSE),0,FALSE,VhdlDocGen::GENERIC);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::PORT,FALSE),0,FALSE,VhdlDocGen::PORT);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::PROCESS,FALSE),0,FALSE,VhdlDocGen::PROCESS);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::SIGNAL,FALSE),0,FALSE,VhdlDocGen::SIGNAL);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::ATTRIBUTE,FALSE),0,FALSE,VhdlDocGen::ATTRIBUTE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::PROCEDURE,FALSE),0,FALSE,VhdlDocGen::PROCEDURE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::RECORD,FALSE),0,FALSE,VhdlDocGen::RECORD);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::UNITS,FALSE),0,FALSE,VhdlDocGen::UNITS);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::SHAREDVARIABLE,FALSE),0,FALSE,VhdlDocGen::SHAREDVARIABLE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::VFILE,FALSE),0,FALSE,VhdlDocGen::VFILE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::GROUP,FALSE),0,FALSE,VhdlDocGen::GROUP);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::INSTANTIATION,FALSE),0,FALSE,VhdlDocGen::INSTANTIATION);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::ALIAS,FALSE),0,FALSE,VhdlDocGen::ALIAS);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::MISCELLANEOUS),0,FALSE,VhdlDocGen::MISCELLANEOUS);

  // configurations must be added to global file definitions.
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::CONFIG,FALSE),0,FALSE,VhdlDocGen::CONFIG);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,theTranslator_vhdlType(VhdlDocGen::UCF_CONST,FALSE),0,FALSE,VhdlDocGen::UCF_CONST);

}

static void setGlobalType(MemberList *ml)
{
  if (ml==0) return;
  MemberDef *mdd=0;
  MemberListIterator mmli(*ml);
  for ( ; (mdd=mmli.current()); ++mmli )
  {
    QCString l=mdd->typeString();

    if (strcmp(mdd->argsString(),"package")==0)
    {
 	mdd->setMemberSpecifiers(VhdlDocGen::INSTANTIATION);
    }
    else if (strcmp(mdd->argsString(),"configuration")==0)
    {
      mdd->setMemberSpecifiers(VhdlDocGen::CONFIG);
    }
    else if (strcmp(mdd->typeString(),"library")==0)
    {
      mdd->setMemberSpecifiers(VhdlDocGen::LIBRARY);
    }
    else if (strcmp(mdd->typeString(),"use")==0)
    {
      mdd->setMemberSpecifiers(VhdlDocGen::USE);
    }
    else if (stricmp(mdd->typeString(),"misc")==0)
    {
      mdd->setMemberSpecifiers(VhdlDocGen::MISCELLANEOUS);
    }
    else if (stricmp(mdd->typeString(),"ucf_const")==0)
    {
      mdd->setMemberSpecifiers(VhdlDocGen::UCF_CONST);
    }
  }
}

/* writes a vhdl type documentation */
void VhdlDocGen::writeVHDLTypeDocumentation(const MemberDef* mdef, const Definition *d, OutputList &ol)
{
  ClassDef *cd=(ClassDef*)d;

  if (cd==0) return;

  QCString ttype=mdef->typeString();
  QCString largs=mdef->argsString();

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
      writeLink(memdef,ol);
      ol.endBold();
      ol.docify(" ");
    }
    else
    {
      ol.docify(" ");
      VhdlDocGen::formatString(ttype,ol,mdef);
      ol.docify(" ");
    }
    ol.docify(mdef->name());
    VhdlDocGen::writeFuncProcDocu(mdef,ol, mdef->argumentList().pointer());
  }


  if (mdef->isVariable())
  {
    if (VhdlDocGen::isConstraint(mdef))
    {
      writeLink(mdef,ol);
      ol.docify(" ");

      largs=largs.replace(QRegExp("#")," ");
      VhdlDocGen::formatString(largs,ol,mdef);
      return;
    }
    else
    {
      writeLink(mdef,ol);
      if (VhdlDocGen::isLibrary(mdef) || VhdlDocGen::isPackage(mdef))
      {
        return;
      }
      ol.docify(" ");
    }

    // QCString largs=mdef->argsString();

    bool c=largs=="context";
    bool brec=largs.stripPrefix("record")  ;

    if (!brec && !c)
      VhdlDocGen::formatString(ttype,ol,mdef);

    if (c || brec || largs.stripPrefix("units"))
    {
      if (c)
	  largs=ttype;
      VhdlDocGen::writeRecUnitDocu(mdef,ol,largs);
      return;
    }

    ol.docify(" ");
    if (VhdlDocGen::isPort(mdef) || VhdlDocGen::isGeneric(mdef))
    {
      // QCString largs=mdef->argsString();
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
  static QRegExp reg("[%]");
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
    if (VhdlDocGen::isGeneric(mdef))      Doxygen::tagFile << "generic";
    if (VhdlDocGen::isPort(mdef))         Doxygen::tagFile << "port";
    if (VhdlDocGen::isEntity(mdef))       Doxygen::tagFile << "entity";
    if (VhdlDocGen::isComponent(mdef))    Doxygen::tagFile << "component";
    if (VhdlDocGen::isVType(mdef))        Doxygen::tagFile << "type";
    if (VhdlDocGen::isConstant(mdef))     Doxygen::tagFile << "constant";
    if (VhdlDocGen::isSubType(mdef))      Doxygen::tagFile << "subtype";
    if (VhdlDocGen::isVhdlFunction(mdef)) Doxygen::tagFile << "function";
    if (VhdlDocGen::isProcedure(mdef))    Doxygen::tagFile << "procedure";
    if (VhdlDocGen::isProcess(mdef))      Doxygen::tagFile << "process";
    if (VhdlDocGen::isSignals(mdef))      Doxygen::tagFile << "signal";
    if (VhdlDocGen::isAttribute(mdef))    Doxygen::tagFile << "attribute";
    if (VhdlDocGen::isRecord(mdef))       Doxygen::tagFile << "record";
    if (VhdlDocGen::isLibrary(mdef))      Doxygen::tagFile << "library";
    if (VhdlDocGen::isPackage(mdef))      Doxygen::tagFile << "package";
    if (VhdlDocGen::isVariable(mdef))     Doxygen::tagFile << "shared variable";
    if (VhdlDocGen::isFile(mdef))         Doxygen::tagFile << "file";
    if (VhdlDocGen::isGroup(mdef))        Doxygen::tagFile << "group";
    if (VhdlDocGen::isCompInst(mdef))     Doxygen::tagFile << " instantiation";
    if (VhdlDocGen::isAlias(mdef))        Doxygen::tagFile << "alias";
    if (VhdlDocGen::isCompInst(mdef))     Doxygen::tagFile << "configuration";

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
    Doxygen::searchIndex->setCurrentDoc(mdef,mdef->anchor(),FALSE);
    Doxygen::searchIndex->addWord(mdef->localName(),TRUE);
    Doxygen::searchIndex->addWord(mdef->qualifiedName(),FALSE);
  }

  QCString cname  = d->name();
  QCString cfname = d->getOutputFileBase();

  //HtmlHelp *htmlHelp=0;
  //  bool hasHtmlHelp = Config_getBool("GENERATE_HTML") && Config_getBool("GENERATE_HTMLHELP");
  //  if (hasHtmlHelp) htmlHelp = HtmlHelp::getInstance();

  // search for the last anonymous scope in the member type
  ClassDef *annoClassDef=mdef->getClassDefOfAnonymousType();

  // start a new member declaration
  bool isAnonymous = annoClassDef; // || m_impl->annMemb || m_impl->annEnumType;
  ///printf("startMemberItem for %s\n",name().data());
  ol.startMemberItem( mdef->anchor(), isAnonymous ); //? 1 : m_impl->tArgList ? 3 : 0);

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
  bool bRec,bUnit;
  QCString ltype(mdef->typeString());
  ltype=ltype.replace(reg," ");
  QCString largs(mdef->argsString());
  largs=largs.replace(reg," ");
  int mm=mdef->getMemberSpecifiers();
  mdef->setType(ltype.data());
  mdef->setArgsString(largs.data());
  //ClassDef * plo=mdef->getClassDef();
  ClassDef *kl=0;
  LockingPtr<ArgumentList> alp = mdef->argumentList();
  QCString nn;
  //VhdlDocGen::adjustRecordMember(mdef);
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
      if (largs=="context")
      {
        VhdlDocGen::writeRecorUnit(ltype,ol,mdef);
      }

      break;

    case VhdlDocGen::GENERIC:
    case VhdlDocGen::PORT:
    case VhdlDocGen::ALIAS:

      writeLink(mdef,ol);
      ol.docify(" ");
      ol.insertMemberAlign();
      if (mm==VhdlDocGen::GENERIC)
      {
        ol.startBold();
        VhdlDocGen::formatString(largs,ol,mdef);
        ol.endBold();
      }
      else
      {
        ol.docify(" ");
        ol.startBold();
        VhdlDocGen::formatString(ltype,ol,mdef);
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
    case VhdlDocGen::INSTANTIATION:
    case VhdlDocGen::CONFIG:
      if (VhdlDocGen::isCompInst(mdef) )
      {
        nn=largs;
        if(nn.stripPrefix("function") || nn.stripPrefix("package"))
        {
          VhdlDocGen::formatString(largs,ol,mdef);
          ol.insertMemberAlign();
          writeLink(mdef,ol);
          ol.docify(" ");
          VhdlDocGen::formatString(ltype,ol,mdef);
          break;
        }

        largs.prepend("::");
        largs.prepend(mdef->name().data());
        ol.writeObjectLink(mdef->getReference(),
            cfname,
            mdef->anchor(),
            mdef->name());
      }
      else
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
          nn=mdef->getOutputFileBase();
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
    case VhdlDocGen::UCF_CONST:
      mm=mdef->name().findRev('_');
      if (mm>0)
      {
        mdef->setName(mdef->name().left(mm));
      }
      writeUCFLink(mdef,ol);
      break;
    case VhdlDocGen::SIGNAL:
    case VhdlDocGen::ATTRIBUTE:
    case VhdlDocGen::SUBTYPE:
    case VhdlDocGen::CONSTANT:
    case VhdlDocGen::SHAREDVARIABLE:
    case VhdlDocGen::VFILE:
    case VhdlDocGen::GROUP:
      writeLink(mdef,ol);
      ol.docify(" ");
      ol.insertMemberAlign();
      VhdlDocGen::formatString(ltype,ol,mdef);
      break;
    case VhdlDocGen::TYPE:
      bRec=largs.stripPrefix("record") ;
      bUnit=largs.stripPrefix("units") ;
      ol.startBold();
      if (bRec)
        ol.docify("record: ");
      if (bUnit)
        ol.docify("units: ");
      writeLink(mdef,ol);
      ol.insertMemberAlign();
      if (!bRec)
        VhdlDocGen::formatString(ltype,ol,mdef);
      if (bUnit) ol.lineBreak();
      if (bRec || bUnit)
        writeRecorUnit(largs,ol,mdef);
      ol.endBold();
      break;

    default: break;
  }

  bool htmlOn = ol.isEnabled(OutputGenerator::Html);
  if (htmlOn && /*Config_getBool("HTML_ALIGN_MEMBERS") &&*/ !ltype.isEmpty())
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
    ol.startMemberDescription(mdef->anchor());
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

#if 0
/* strips the prefix for record and unit members*/
void VhdlDocGen::adjustRecordMember(MemberDef *mdef)
{ //,OutputList & ol) {
  QRegExp regg("[_a-zA-Z\"]");
  QCString nn=mdef->name();
  int j=nn.find(regg,0);
  if (j>0)
  {
    nn=nn.mid(j,nn.length());
    mdef->setName(nn.data());
  }
}//adjustRecordMember
#endif
/* strips the prefix for package and package body */

bool VhdlDocGen::writeClassType( ClassDef *& cd,
    OutputList &ol ,QCString & cname)
{
  //static ClassDef *prev = 0;
  //if (prev == cd)  return TRUE;
  //if (cd != prev) prev=cd;

  int id=cd->protection();
  QCString qcs = VhdlDocGen::trTypeString(id+2);
  cname=VhdlDocGen::getClassName(cd);
  ol.startBold();
  ol.writeString(qcs.data());
  ol.writeString(" ");
  ol.endBold();
  //ol.insertMemberAlign();
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
      if (sing) return "use clause";
      else      return "Use Clauses";
    case VhdlDocGen::GENERIC:
      if (sing) return "Generic";
      else      return "Generics";
    case VhdlDocGen::PACKAGE_BODY:
      return "Package Body";
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
    case VhdlDocGen::INSTANTIATION:
      if (sing) return "Instantiation";
      else      return "Instantiations";
    case VhdlDocGen::ALIAS:
      if (sing) return "Alias";
      return "Aliases";
    case VhdlDocGen::CONFIG:
      if (sing) return "Configuration";
      return "Configurations";
    case VhdlDocGen::MISCELLANEOUS:
      return "Miscellaneous";
    case VhdlDocGen::UCF_CONST:
      return "Constraints";
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
    "the Entities  they belong to:";
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
  int len= qsl.count();
  for(int j=0;j<len;j++)
  {
    QCString q=qsl[j].utf8();
    VhdlDocGen::writeFormatString(q,ol,mdef);
    ol.lineBreak();
    if (j==2) // only the first three lines are shown
    {
      q = "...";
      VhdlDocGen::writeFormatString(q,ol,mdef);
      break;
    }
  }
}

void VhdlDocGen::writeSource(MemberDef *mdef,OutputList& ol,QCString & cname)
{
  QCString codeFragment=mdef->documentation();
  int start=mdef->getStartBodyLine();
  QStringList qsl=QStringList::split("\n",codeFragment);
  ol.startCodeFragment();
  int len = qsl.count();
  QCString lineNumber;
  int j;
  for (j=0;j<len;j++)
  {
    lineNumber.sprintf("%05d",start++);
    lineNumber+=" ";
    ol.startBold();
    ol.docify(lineNumber.data());
    ol.endBold();
    ol.insertMemberAlign();
    QCString q=qsl[j].utf8();
    VhdlDocGen::writeFormatString(q,ol,mdef);
    ol.lineBreak();
  }
  ol.endCodeFragment();

  mdef->writeSourceDef(ol,cname);
  mdef->writeSourceRefs(ol,cname);
  mdef->writeSourceReffedBy(ol,cname);
}


QCString VhdlDocGen::convertFileNameToClassName(QCString name)
{

  QCString n=name;
  n=n.remove(0,6);

  int i=0;

  while((i=n.find("__"))>0)
  {
    n=n.remove(i,1);
  }

  while((i=n.find("_1"))>0)
  {
    n=n.replace(i,2,":");
  }

  return n;
}

void VhdlDocGen::parseUCF(const char*  input,  Entry* entity,QCString fileName,bool altera)
{
  QCString ucFile(input);
  int lineNo=0;
  QCString newLine="\n";
  QCString comment("#!");
  QCString brief;

  while(!ucFile.isEmpty())
  {
    int i=ucFile.find("\n");
    if (i<0) break;
    lineNo++;
    QCString temp=ucFile.left(i);
    temp=temp.stripWhiteSpace();
    bool bb=temp.stripPrefix("//");

    if (!temp.isEmpty())
    {
      if (temp.stripPrefix(comment) )
      {
        brief+=temp;
        brief.append("\\n");
      }
      else if (!temp.stripPrefix("#") && !bb)
      {
        if (altera)
        {
          int i=temp.find("-name");
          if (i>0)
            temp=temp.remove(0,i+5);

          temp.stripPrefix("set_location_assignment");

          initUCF(entity,0,temp,lineNo,fileName,brief);
        }
        else
        {
          QRegExp ee("[\\s=]");
          int i=temp.find(ee);
          QCString ff=temp.left(i);
          temp.stripPrefix(ff.data());
          ff.append("#");
          if (!temp.isEmpty())
          {
            initUCF(entity,ff.data(),temp,lineNo,fileName,brief);
          }
        }
      }
    }//temp

    ucFile=ucFile.remove(0,i+1);
  }// while
}

static void initUCF(Entry* root,const char*  type,QCString &  qcs,int line,QCString & fileName,QCString & brief)
{
  if (qcs.isEmpty())return;
  QRegExp sp("\\s");
  QRegExp reg("[\\s=]");
  QCString n;
  // bool bo=(stricmp(type,qcs.data())==0);

  VhdlDocGen::deleteAllChars(qcs,';');
  qcs=qcs.stripWhiteSpace();

  int i= qcs.find(reg);
  if (i<0) return;
  if (i==0)
  {
    n=type;
    VhdlDocGen::deleteAllChars(n,'#');
    type="";
  }
  else
  {
    n=qcs.left(i);
  }
  qcs=qcs.remove(0,i+1);
  //  qcs.prepend("|");

  qcs.stripPrefix("=");

  Entry* current=new Entry;
  current->spec=VhdlDocGen::UCF_CONST;
  current->section=Entry::VARIABLE_SEC;
  current->bodyLine=line;
  current->fileName=fileName;
  current->type="ucf_const";
  //if (!bo)
  //current->args=type;
  current->args+=qcs;
  current->lang=  SrcLangExt_VHDL ;

  // adding dummy name for constraints like VOLTAGE=5,TEMPERATURE=20 C
  if (n.isEmpty())
  {
    n="dummy";
    n+=VhdlDocGen::getRecordNumber();
  }

  current->name= n+"_";
  current->name.append(VhdlDocGen::getRecordNumber().data());

  if (!brief.isEmpty())
  {
    current->brief=brief;
    current->briefLine=line;
    current->briefFile=fileName;
    brief.resize(0);
  }

  root->addSubEntry(current);
}


static void writeUCFLink(const MemberDef* mdef,OutputList &ol)
{

  QCString largs(mdef->argsString());
  QCString n= VhdlDocGen::splitString(largs, '#');
  // VhdlDocGen::adjustRecordMember(mdef);
  bool equ=(n.length()==largs.length());

  if (!equ)
  {
    ol.writeString(n.data());
    ol.docify(" ");
    ol.insertMemberAlign();
  }

  if (mdef->name().contains("dummy")==0)
    VhdlDocGen::writeLink(mdef,ol);
  if (equ)
    ol.insertMemberAlign();
  ol.docify(" ");
  VhdlDocGen::formatString(largs,ol,mdef);
}

QCString VhdlDocGen::splitString(QCString& str,  char c)
{
  QCString n=str;
  int i=str.find(c);
  if (i>0)
  {
    n=str.left(i);
    str=str.remove(0,i+1);
  }
  return n;
}

bool VhdlDocGen::findConstraintFile(LayoutNavEntry *lne)
{
  FileName *fn=Doxygen::inputNameList->first();
  //LayoutNavEntry *cc = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::Files);

  LayoutNavEntry *kk = lne->parent();//   find(LayoutNavEntry::Files);
  // LayoutNavEntry *kks = kk->parent();//   find(LayoutNavEntry::Files);
  QCString file;
  QCString co("Constraints");

  if (Config_getBool("HAVE_DOT") && Config_getEnum("DOT_IMAGE_FORMAT")=="svg")
  {
     QCString ov = theTranslator->trDesignOverview();
     QCString ofile("vhdl_design_overview");
     LayoutNavEntry *oo=new LayoutNavEntry( lne,LayoutNavEntry::MainPage,true,ofile,ov,"");  
     kk->addChild(oo); 
  }

  while (fn)
  {
    FileDef *fd=fn->first();
    if (fd->name().contains(".ucf") || fd->name().contains(".qsf"))
    {
      file = convertNameToFile(fd->name().data(),FALSE,FALSE);
      LayoutNavEntry *ucf=new LayoutNavEntry(lne,LayoutNavEntry::MainPage,TRUE,file,co,"");
      kk->addChild(ucf);
      break;
    }
    fn=Doxygen::inputNameList->next();
  }
  return  FALSE;
}

void VhdlDocGen::writeAlphbeticalClass(OutputList& ol,const ClassDef* cd,const QCString & cname)
{
  if (cname.contains("::")==0)
  {
    ClassDef*oo=	VhdlDocGen::findArchitecture(cd);
    ol.writeObjectLink(cd->getReference(),
        cd->getOutputFileBase(),0,cname);
    if (oo)
    {
      ol.docify(" [");
      ol.writeObjectLink(oo->getReference(),
          oo->getOutputFileBase(),0,"arch");
      ol.docify("] ");
    }
  }
}




//        for cell_inst : [entity] work.proto [ (label|expr) ]
QCString  VhdlDocGen::parseForConfig(QCString & entity,QCString & arch)
{
  int index;
  QCString label;
  QCString ent("entity");
  if (!entity.contains(":")) return "";

  QRegExp exp("[:()\\s]");
  QStringList ql=QStringList::split(exp,entity,FALSE);
  //int ii=ql.findIndex(ent);
  assert(ql.count()>=2);
  label = ql[0].utf8();
  entity = ql[1].utf8();
  if ((index=entity.findRev("."))>=0)
  {
    entity.remove(0,index+1);
  }

  if (ql.count()==3)
  {
    arch= ql[2].utf8();
    ql=QStringList::split(exp,arch,FALSE);
    if (ql.count()>1) // expression
      arch="";
  }
  return label; // label
}


//        use (configuration|entity|open) work.test [(cellfor)];

QCString  VhdlDocGen::parseForBinding(QCString & entity,QCString & arch)
{
  int index;
  QRegExp exp("[()\\s]");

  QCString label="";
  QStringList ql=QStringList::split(exp,entity,FALSE);

  if (ql.contains("open"))
    return "open";

  label=ql[0].utf8();

  entity = ql[1].utf8();
  if ((index=entity.findRev("."))>=0)
    entity.remove(0,index+1);

  if (ql.count()==3)
    arch=ql[2].utf8();
  return label;
}

ClassDef*  VhdlDocGen::findArchitecture(QCString identifier, QCString entity_name)
{
  QCString archName=entity_name+"::"+identifier;
  return  Doxygen::classSDict->find(archName.data());
}


//@param arch bit0:flipflop
//@param binding  e.g entity work.foo(bar)
//@param label  |label0|label1
//                          label0:architecture name
//@param confVhdl of configuration file (identifier::entity_name) or
//               the architecture if isInlineConf TRUE
//@param isInlineConf
//@param confN List of configurations

void assignBinding(VhdlConfNode * conf,QCString label)
{
  QList<Entry> instList= getVhdlInstList();
  QListIterator<Entry> eli(instList);
  Entry *cur;
  ClassDef *archClass;
  QCString archName,entityName;
  bool allOthers=FALSE;

  if (conf->isInlineConf)
  {
    archClass=Doxygen::classSDict->find(conf->confVhdl.data());
  }
  else
  {
    archName= VhdlDocGen::getIndexWord(label.data(),0);
    entityName= VhdlDocGen::getIndexWord(conf->confVhdl.data(),1);
    archClass=VhdlDocGen::findArchitecture(archName,entityName);
  }

  if (!archClass)
  {
    fprintf(stderr,"\n architecture %s not found ! ",conf->confVhdl.data());
    return;
  }

  archName=archClass->name();

  QCString allOt=VhdlDocGen::getIndexWord(conf->arch.data(),0);

  if (allOt=="all" || allOt=="others")
    allOthers=TRUE;

  for (;(cur=eli.current());++eli)
  {
    if (conf->isInlineConf &&  (conf->confVhdl!=cur->args))
      continue;

    if (!conf->isInlineConf &&  (archName!=cur->args))
      continue;

    if (cur->exception==label || conf->isInlineConf)
    {
      QCString sign,archy;

      if (allOthers==FALSE)
      {
        archy=conf->arch;
        sign=cur->name+":"+cur->type;
      }
      else
      {
        sign=cur->type;
        archy=VhdlDocGen::getIndexWord(conf->arch.data(),1);
      }


      if (archy==sign && !cur->stat)
      {
        // fprintf(stderr," \n label [%s] [%s] [%s]",cur->exception.data(),cur->type.data(),cur->name.data());
        QCString ent1=conf->binding;
        QCString arch1;
        QCString rr=VhdlDocGen::parseForBinding(ent1,arch1);
        arch1=ent1+"::"+arch1;
        //ClassDef *archBind=Doxygen::classSDict->find(arch1.data());
        ClassDef *ent=Doxygen::classSDict->find(ent1.data());
        QCString	  inst=VhdlDocGen::getIndexWord(cur->args.data(),0);
        ClassDef *cd=Doxygen::classSDict->find(inst.data());

        if (cd==0 || ent==0)
          continue;

        addInstance(ent,archClass,cd,cur);
        cur->stat=TRUE;
      }
    }
  }//for
}//assignBinding


void assignConfiguration(VhdlConfNode* rootNode,QCString label)
{
  if (rootNode==NULL) return;
  uint iter;

  if (!rootNode->isBinding())
  {
    //   printf("\n ARCH %s  BIND %s \n",rootNode->arch.data(),rootNode->binding.data());
    assignBinding(rootNode,label);
    return;
  }
  else
    label+="|"+rootNode->arch;


  for(iter=0;iter<rootNode->confN.count();iter++)
  {
    VhdlConfNode* conf= (VhdlConfNode *)rootNode->confN.at(iter);
    assignConfiguration(conf,label);
  }
}

/*

// file foo.vhd
// enitity foo
//        .....
// end entity

// file foo_arch.vhd
// architecture xxx of foo is
//          ........
//  end architecture

 */
void VhdlDocGen::computeVhdlComponentRelations()
{

  QCString entity,inst,arch,vhd;

  QList<VhdlConfNode> confList =  getVhdlConfiguration();
  for (uint iter=0;iter<confList.count(); iter++)
  {
    VhdlConfNode* conf= (VhdlConfNode *)confList.at(iter);
    assignConfiguration(conf,"");
  }

  QList<Entry> qsl= getVhdlInstList();
  QListIterator<Entry> eli(qsl);
  Entry *cur;

  for (eli.toFirst();(cur=eli.current());++eli)
  {
    if (cur->stat ) //  was bind
      continue;

    if (cur->includeName=="entity" || cur->includeName=="component" )
    {
      entity=cur->includeName+" "+cur->type;
      QCString rr=VhdlDocGen::parseForBinding(entity,arch);
    }
    else if (cur->includeName.isEmpty())
    {
      entity=cur->type;
    }
    ClassDef *classEntity=Doxygen::classSDict->find(entity);
    inst=VhdlDocGen::getIndexWord(cur->args.data(),0);
    ClassDef *cd=Doxygen::classSDict->find(inst);
    ClassDef *ar=Doxygen::classSDict->find(cur->args);

    if (cd==0 || classEntity==0 )
      continue;

    addInstance(classEntity,ar,cd,cur);

  }
}

static void addInstance(ClassDef* classEntity, ClassDef* ar,
                        ClassDef *cd , Entry *cur,ClassDef* /*archBind*/)
{
  if (classEntity==cd) return;

  QCString bName=classEntity->name();
  //printf("addInstance %s to %s\n", cd->name().data(), classEntity->name().data());
  QCString n1=cur->type;

  if (!cd->isBaseClass(classEntity, true, 0))
  {
    cd->insertBaseClass(classEntity,n1,Public,Normal,0);
  }
  else
  {
    VhdlDocGen::addBaseClass(cd,classEntity);
  }

  if (!VhdlDocGen::isSubClass(classEntity,cd,true,0))
  {
    classEntity->insertSubClass(cd,Public,Normal,0);
  }

  if (ar==0) return;

  QCString uu=cur->name;
  MemberDef *md=new MemberDef(
      ar->getDefFileName(), cur->startLine,
      cur->type,uu,uu, 0,
      Public, Normal, cur->stat,Member,
      MemberDef::Variable,
      0,
      0);

  if (ar->getOutputFileBase()) 
  {
    TagInfo tg;
    tg.anchor = 0;
    tg.fileName = ar->getOutputFileBase();
    tg.tagName = 0;
    md->setTagInfo(&tg);
  }

  //fprintf(stderr,"\n%s%s%s\n",md->name().data(),cur->brief.data(),cur->doc.data());

  md->setLanguage(SrcLangExt_VHDL);
  md->setMemberSpecifiers(VhdlDocGen::INSTANTIATION);
  md->setBriefDescription(cur->brief,cur->briefFile,cur->briefLine);
  md->setBodySegment(cur->startLine,-1) ;
  md->setDocumentation(cur->doc.data(),cur->docFile.data(),cur->docLine); 
  FileDef *fd=ar->getFileDef();
  md->setBodyDef(fd);
  ar->insertMember(md);
  //    printf("\nMemberreference [%p]",md);
}


void  VhdlDocGen::writeRecorUnit(QCString & largs,OutputList& ol ,const MemberDef *mdef)
{
  QStringList ql=QStringList::split("#",largs,FALSE);
  uint len=ql.count();
  for(uint i=0;i<len;i++)
  {
    QCString n=ql[i].utf8();
    VhdlDocGen::formatString(n,ol,mdef);
    if ((len-i)>1) ol.lineBreak();
  }
}


void VhdlDocGen::writeRecUnitDocu(
    const MemberDef *md,
    OutputList& ol,
    QCString largs
    )
{

  QStringList ql=QStringList::split("#",largs,FALSE);
  uint len=ql.count();
  ol.startParameterList(TRUE);
  bool first=TRUE;
  for(uint i=0;i<len;i++)
  {
    QCString n=ql[i].utf8();
    ol.startParameterType(first,"");
    VhdlDocGen::formatString(n,ol,md);
    if ((len-i)>1)
    {
      ol.endParameterName(FALSE,FALSE,FALSE);
    }
    else
    {
      ol.endParameterName(TRUE,FALSE,TRUE);
    }

    first=FALSE;
  }
}//#

void VhdlDocGen::writeCodeFragment(OutputList& ol,int start, QCString & codeFragment,const MemberDef* mdef)
{
  QStringList qsl=QStringList::split("\n",codeFragment);
  ol.startCodeFragment();
  int len = qsl.count();
  QCString lineNumber;
  int j;
  for (j=0;j<len;j++)
  {
    lineNumber.sprintf("%05d",start++);
    lineNumber+=" ";
    ol.startBold();
    ol.docify(lineNumber.data());
    ol.endBold();
    ol.insertMemberAlign();
    QCString q=qsl[j].utf8();
    VhdlDocGen::writeFormatString(q,ol,mdef);
    ol.docify("\n");
  }
  ol.endCodeFragment();
}

bool VhdlDocGen::isSubClass(ClassDef* cd,ClassDef *scd, bool followInstances,int level)
{
  bool found=FALSE;
  //printf("isBaseClass(cd=%s) looking for %s\n",name().data(),bcd->name().data());
  if (level>255)
  {
    err("Possible recursive class relation while inside %s and looking for %s\n",qPrint(cd->name()),qPrint(scd->name()));
    abort();
    return FALSE;
  }

  if (cd->subClasses())
  {
    // Beware: trying to optimise the iterator away using ->first() & ->next()
    // causes bug 625531
    BaseClassListIterator bcli(*cd->subClasses());
    for ( ; bcli.current() && !found ; ++bcli)
    {
      ClassDef *ccd=bcli.current()->classDef;
      if (!followInstances && ccd->templateMaster()) ccd=ccd->templateMaster();
      //printf("isSubClass() subclass %s\n",ccd->name().data());
      if (ccd==scd)
      {
        found=TRUE;
      }
      else 
      {
        if (level <256)
        {
          found=ccd->isBaseClass(scd,followInstances,level+1);
        }
      }
    }
  }
  return found;
}

void VhdlDocGen::addBaseClass(ClassDef* cd,ClassDef *ent)
{
  if (cd->baseClasses())
  {
    BaseClassListIterator bcli(*cd->baseClasses());
    for ( ; bcli.current()  ; ++bcli)
    {
      ClassDef *ccd=bcli.current()->classDef;
      if (ccd==ent) 
      {
        QCString n = bcli.current()->usedName;
        int i = n.find('(');
        if(i<0)
        {
          bcli.current()->usedName.append("(2)");
          return;
        }
        static QRegExp reg("[0-9]+");
        QCString s=n.left(i);
        QCString r=n.right(n.length()-i);
        QCString t=r;
        VhdlDocGen::deleteAllChars(r,')');
        VhdlDocGen::deleteAllChars(r,'(');
        r.setNum(r.toInt()+1);
        t.replace(reg,r.data());
        s.append(t.data());
        bcli.current()->usedName=s;
        bcli.current()->templSpecifiers=t;
      }
    }
  }
}

