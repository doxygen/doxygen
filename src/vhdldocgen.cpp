/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
 * supports VHDL-87/93/2008
 * does not support VHDL-AMS
 ******************************************************************************/

// global includes
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <map>
#include <algorithm>
#include <unordered_set>
#include <mutex>

/* --------------------------------------------------------------- */

// local includes
#include "qcstring.h"
#include "vhdldocgen.h"
#include "message.h"
#include "config.h"
#include "doxygen.h"
#include "util.h"
#include "language.h"
#include "commentscan.h"
#include "definition.h"
#include "searchindex.h"
#include "outputlist.h"
#include "parserintf.h"
#include "layout.h"
#include "arguments.h"
#include "portable.h"
#include "memberlist.h"
#include "memberdef.h"
#include "groupdef.h"
#include "classlist.h"
#include "namespacedef.h"
#include "filename.h"
#include "membergroup.h"
#include "memberdef.h"
#include "membername.h"
#include "plantuml.h"
#include "vhdljjparser.h"
#include "VhdlParser.h"
#include "regex.h"
#include "plantuml.h"
#include "textstream.h"
#include "moduledef.h"

//#define DEBUGFLOW
#define theTranslator_vhdlType theTranslator->trVhdlType

static void initUCF(Entry* root,const QCString &type,QCString &qcs,int line,const QCString & fileName,QCString & brief);
static void writeUCFLink(const MemberDef* mdef,OutputList &ol);
static void addInstance(ClassDefMutable* entity, ClassDefMutable* arch, ClassDefMutable *inst,
                        const std::shared_ptr<Entry> &cur);

static const MemberDef *flowMember=nullptr;

void VhdlDocGen::setFlowMember( const MemberDef* mem)
{
  flowMember=mem;
}

 const MemberDef* VhdlDocGen::getFlowMember()
{
  return flowMember;
}

//--------------------------------------------------------------------------------------------------

static void writeLink(const MemberDef* mdef,OutputList &ol)
{
  ol.writeObjectLink(mdef->getReference(),
      mdef->getOutputFileBase(),
      mdef->anchor(),
      mdef->name());
}

static void startFonts(const QCString& q, const char *keyword,OutputList& ol)
{
  auto &codeOL = ol.codeGenerators();
  codeOL.startFontClass(keyword);
  codeOL.codify(q);
  codeOL.endFontClass();
}

static QCString splitString(QCString& str,char c)
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

static int compareString(const QCString& s1,const QCString& s2)
{
  return qstricmp(s1.stripWhiteSpace(),s2.stripWhiteSpace());
}

//--------------------------------------------------------------------------------------------------

 // vhdl keywords included VHDL 2008
static const std::unordered_set< std::string > g_vhdlKeyWordSet0 =
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
  "package","parameter","port","postponed","procedure","process","property","protected","pure",
  "range","record","register","reject","release","restrict","restrict_guarantee","rem","report","rol","ror","return",
  "select","sequence","severity","signal","shared","sla","sll","sra","srl","strong","subtype",
  "then","to","transport","type",
  "unaffected","units","until","use",
  "variable","vmode","vprop","vunit",
  "wait","when","while","with",
  "xor","xnor"
};


// type
static const std::unordered_set< std::string> g_vhdlKeyWordSet1 =
{
  "natural","unsigned","signed","string","boolean", "bit","bit_vector","character",
  "std_ulogic","std_ulogic_vector","std_logic","std_logic_vector","integer",
  "real","float","ufixed","sfixed","time","positive"
};

// logic
static const std::unordered_set< std::string > g_vhdlKeyWordSet2 =
{
  "abs","and","or","not","mod","xor","rem","xnor","ror","rol","sla","sll"
};

// predefined attributes
static const std::unordered_set< std::string > g_vhdlKeyWordSet3 =
{
  "base","left","right","high","low","ascending",
  "image","value","pos","val","succ","pred","leftof","rightof","left","right","high","low",
  "range","reverse_range","length","ascending","delayed","stable","quiet","transaction","event",
  "active","last_event","last_active","last_value","driving","driving_value","simple_name","instance_name","path_name"
};

void VhdlDocGen::init()
{
}

/*!
 * returns the color of a keyword
 */
const char* VhdlDocGen::findKeyWord(const QCString& kw)
{
  std::string word=kw.lower().str();

  if (word.empty()) return nullptr;

  if (g_vhdlKeyWordSet0.find(word)!=g_vhdlKeyWordSet0.end())
    return "keywordflow";

  if (g_vhdlKeyWordSet1.find(word)!=g_vhdlKeyWordSet1.end())
    return "keywordtype";

  if (g_vhdlKeyWordSet2.find(word)!=g_vhdlKeyWordSet2.end())
    return "vhdllogic";

  if (g_vhdlKeyWordSet3.find(word)!=g_vhdlKeyWordSet3.end())
    return "vhdlkeyword";

  return nullptr;
}

ClassDef *VhdlDocGen::getClass(const QCString &name)
{
  if (name.isEmpty()) return nullptr;
  return Doxygen::classLinkedMap->find(QCString(name).stripWhiteSpace());
}

ClassDef* VhdlDocGen::getPackageName(const QCString & name)
{
  return getClass(name);
}

static std::recursive_mutex                        g_vhdlMutex;
static std::map<std::string,const MemberDef*>      g_varMap;
static std::vector<ClassDef*>                      g_classList;
static std::map<ClassDef*,std::vector<ClassDef*> > g_packages;

const MemberDef* VhdlDocGen::findMember(const QCString& className, const QCString& memName)
{
  std::lock_guard lock(g_vhdlMutex);
  ClassDef *ecd=nullptr;
  const MemberDef *mdef=nullptr;

  ClassDef *cd=getClass(className);
  //printf("VhdlDocGen::findMember(%s,%s)=%p\n",qPrint(className),qPrint(memName),cd);
  if (cd==nullptr) return nullptr;

  mdef=VhdlDocGen::findMemberDef(cd,memName,MemberListType::VariableMembers());
  if (mdef) return mdef;
  mdef=VhdlDocGen::findMemberDef(cd,memName,MemberListType::PubMethods());
  if (mdef) return mdef;

  // nothing found so far
  // if we are an architecture or package body search in entity

  if (VhdlDocGen::convert(cd->protection())==VhdlDocGen::ARCHITECTURECLASS ||
      VhdlDocGen::convert(cd->protection())==VhdlDocGen::PACKBODYCLASS)
  {
    Definition *d = cd->getOuterScope();
    // searching upper/lower case names

    QCString tt=d->name();
    ecd =getClass(tt);
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
      mdef=VhdlDocGen::findMemberDef(ecd,memName,MemberListType::VariableMembers());
      if (mdef) return mdef;
      mdef=VhdlDocGen::findMemberDef(cd,memName,MemberListType::PubMethods());
      if (mdef) return mdef;
    }
   }


  if (VhdlDocGen::convert(cd->protection())==VhdlDocGen::ARCHITECTURECLASS ||
      VhdlDocGen::convert(cd->protection())==VhdlDocGen::PACKBODYCLASS)
  {
    Definition *d = cd->getOuterScope();

    QCString tt=d->name();
    ClassDef *acd =getClass(tt);
    if (!acd)
    {
      tt=tt.upper();
      acd =getClass(tt);
    }
    if (!acd)
    {
      tt=tt.lower();
      acd =getClass(tt);
    }
    if (acd) //d && d->definitionType()==Definition::TypeClass)
    {
      if(g_packages.find(acd)==g_packages.end())
      {
        VhdlDocGen::findAllPackages(acd);
      }
    }
  }
  else
  {
    ecd=cd;
    if (g_packages.find(ecd)==g_packages.end()) VhdlDocGen::findAllPackages(ecd);
  }

  if (ecd)
  {
    auto cList_it = g_packages.find(ecd);
    if (cList_it!=g_packages.end())
    {
      for (const auto &cdp : cList_it->second)
      {
        mdef=VhdlDocGen::findMemberDef(cdp,memName,MemberListType::VariableMembers());
        if (mdef) return mdef;
        mdef=VhdlDocGen::findMemberDef(cdp,memName,MemberListType::PubMethods());
        if (mdef) return mdef;
      }
    }
  }
  return nullptr;

}//findMember

/**
 *  This function returns the entity|package
 *  in which the key (type) is found
 */
const MemberDef* VhdlDocGen::findMemberDef(ClassDef* cd,const QCString& key,MemberListType type)
{
  std::lock_guard lock(g_vhdlMutex);
  QCString keyType=cd->symbolName()+"@"+key;
  //printf("\n %s | %s | %s",qPrint(cd->symbolName()),key.data(,),qPrint(keyType));

  auto it = g_varMap.find(keyType.str());
  if (it!=g_varMap.end())
  {
    return it->second;
  }
  if (std::find(g_classList.begin(),g_classList.end(),cd)!=g_classList.end())
  {
    return nullptr;
  }
  const MemberList *ml=cd->getMemberList(type);
  g_classList.push_back(cd);
  if (!ml)
  {
    return nullptr;
  }
  //int l=ml->count();
  //	fprintf(stderr,"\n loading entity %s %s: %d",qPrint(cd->symbolName()),qPrint(keyType),l);

  for (const auto &md : *ml)
  {
    QCString tkey=cd->symbolName()+"@"+md->name();
    if (g_varMap.find(tkey.str())==g_varMap.end())
    {
      g_varMap.emplace(tkey.str(),md);
    }
  }
  it=g_varMap.find(keyType.str());
  if (it!=g_varMap.end())
  {
    return it->second;
  }
  return nullptr;
}//findMemberDef

/*!
 * finds all included packages of an Entity or Package
 */

void VhdlDocGen::findAllPackages( ClassDef *cdef)
{
  std::lock_guard lock(g_vhdlMutex);
  if (g_packages.find(cdef)!=g_packages.end()) return;
  std::vector<ClassDef*> cList;
  MemberList *mem=cdef->getMemberList(MemberListType::VariableMembers());
  if (mem)
  {
    for (const auto &md : *mem)
    {
      if (VhdlDocGen::isPackage(md))
      {
        ClassDef* cd=VhdlDocGen::getPackageName(md->name());
        if (cd)
        {
          cList.push_back(cd);
          VhdlDocGen::findAllPackages(cd);
          g_packages.emplace(cdef,cList);
        }
      }
    }//for
  }

}// findAllPackages

/*!
 * returns the function with the matching argument list
 * is called in vhdlcode.l
 */

const MemberDef* VhdlDocGen::findFunction(const QCString& funcname, const QCString& package)
{
  ClassDef *cdef=getClass(package);
  if (cdef==nullptr) return nullptr;

  MemberList *mem=cdef->getMemberList(MemberListType::PubMethods());
  if (mem)
  {
    for (const auto &mdef : *mem)
    {
      QCString mname=mdef->name();
      if ((VhdlDocGen::isProcedure(mdef) || VhdlDocGen::isVhdlFunction(mdef)) && (compareString(funcname,mname)==0))
      {
        return mdef;
      }//if
    }//for
  }//if
  return nullptr;
} //findFunction



static VhdlSpecifier getSpecifierTypeFromClass(const ClassDef *cd)
{
  VhdlDocGen::VhdlClasses ii=VhdlDocGen::convert(cd->protection());

  QCString type;
  if (ii==VhdlDocGen::ENTITYCLASS)
    return VhdlSpecifier::ENTITY;
  else if (ii==VhdlDocGen::ARCHITECTURECLASS)
    return VhdlSpecifier::ARCHITECTURE;
  else if (ii==VhdlDocGen::PACKBODYCLASS)
    return VhdlSpecifier::PACKAGE_BODY;
  else if (ii==VhdlDocGen::PACKAGECLASS)
    return VhdlSpecifier::PACKAGE;
  return VhdlSpecifier::UNKNOWN;
}

/*!
 * returns the class title+ref
 */

QCString VhdlDocGen::getClassTitle(const ClassDef *cd)
{
  QCString pageTitle;
  if (cd==nullptr) return "";
  pageTitle=VhdlDocGen::getClassName(cd);
  pageTitle+=" ";
  pageTitle+=theTranslator_vhdlType(getSpecifierTypeFromClass(cd),TRUE);
  return pageTitle;
} // getClassTitle

/* returns the class name without their prefixes */

QCString VhdlDocGen::getClassName(const ClassDef* cd)
{
  QCString temp;
  if (cd==nullptr) return "";

  if (VhdlDocGen::convert(cd->protection())==VhdlDocGen::PACKBODYCLASS)
  {
    temp=cd->name();
    temp.stripPrefix("_");
    return temp;
  }

  return substitute(cd->className(),"::",".");
}

/*!
 * writes an inline link form entity|package to architecture|package body and vice verca
 */

void VhdlDocGen::writeInlineClassLink(const ClassDef* cd ,OutputList& ol)
{
  std::vector<QCString> ql;
  QCString nn=cd->className();
  VhdlClasses ii=convert(cd->protection());

  QCString type = theTranslator_vhdlType(getSpecifierTypeFromClass(cd),TRUE);

  //type=type.lower();
  type+=" >> ";
  ol.disable(OutputType::RTF);
  ol.disable(OutputType::Man);

  if (ii==VhdlDocGen::PACKBODYCLASS)
  {
    nn.stripPrefix("_");
    cd=getClass(nn);
  }
  else  if (ii==VhdlDocGen::PACKAGECLASS)
  {
    nn.prepend("_");
    cd=getClass(nn);
  }
  else if (ii==VhdlDocGen::ARCHITECTURECLASS)
  {
    StringVector qlist=split(nn.str(),"-");
    if (qlist.size()>1)
    {
      nn=qlist[1];
      cd=VhdlDocGen::getClass(nn);
    }
  }

  QCString opp;
  if (ii==VhdlDocGen::ENTITYCLASS)
  {
    VhdlDocGen::findAllArchitectures(ql,cd);
    for (const auto &s : ql)
    {
      StringVector qlist=split(s.str(),"-");
      if (qlist.size()>2)
      {
        QCString s1(qlist[0]);
        QCString s2(qlist[1]);
        s1.stripPrefix("_");
        if (ql.size()==1) s1.clear();
        ClassDef *cc = getClass(s);
        if (cc)
        {
          VhdlDocGen::writeVhdlLink(cc,ol,type,s2,s1);
        }
      }
    }
  }
  else
  {
    VhdlDocGen::writeVhdlLink(cd,ol,type,nn,opp);
  }

  ol.enable(OutputType::Man);
  ol.enable(OutputType::RTF);

}// write

/*
 * finds all architectures which belongs to an entity
 */
void VhdlDocGen::findAllArchitectures(std::vector<QCString>& qll,const ClassDef *cd)
{
  for (const auto &citer : *Doxygen::classLinkedMap)
  {
    QCString className=citer->className();
    int pos = -1;
    if (cd != citer.get() && (pos=className.find('-'))!=-1)
    {
      QCString postfix=className.mid(pos+1);
      if (qstricmp(cd->className(),postfix)==0)
      {
        qll.push_back(className);
      }
    }
  }// for
}//findAllArchitectures

const ClassDef* VhdlDocGen::findArchitecture(const ClassDef *cd)
{
  for (const auto &citer : *Doxygen::classLinkedMap)
  {
    QCString jj=citer->name();
    StringVector ql=split(jj.str(),":");
    if (ql.size()>1)
    {
      if (ql[0]==cd->name())
      {
        return citer.get();
      }
    }
  }
  return nullptr;
}
/*
 * writes the link entity >> .... or architecture >> ...
 */

void VhdlDocGen::writeVhdlLink(const ClassDef* ccd ,OutputList& ol,QCString& type,QCString& nn,QCString& behav)
{
  if (ccd==nullptr)  return;
  ol.startBold();
  ol.docify(type);
  ol.endBold();
  nn.stripPrefix("_");
  ol.writeObjectLink(ccd->getReference(),ccd->getOutputFileBase(),QCString(),nn);

  if (!behav.isEmpty())
  {
    behav.prepend("  ");
    ol.startBold();
    ol.docify(behav);
    ol.endBold();
  }

  ol.lineBreak();
}


/*!
 * strips the "--!" prefixes of vhdl comments
 */
void VhdlDocGen::prepareComment(QCString& qcs)
{
  qcs=qcs.stripWhiteSpace();
  if (qcs.isEmpty()) return;

  const char* sc="--!";
  if (qcs.startsWith(sc)) qcs = qcs.mid(qstrlen(sc));
  static const reg::Ex re(R"(\n[ \t]*--!)");
  std::string s = qcs.str();
  reg::Iterator iter(s,re);
  reg::Iterator end;
  std::string result;
  size_t p=0;
  size_t sl=s.length();
  for ( ; iter!=end ; ++iter)
  {
    const auto &match = *iter;
    size_t i = match.position();
    result+="\n";
    result+=s.substr(p,i-p);
    p = match.position()+match.length();
  }
  if (p<sl)
  {
    result+="\n";
    result+=s.substr(p);
  }

  qcs = result;
  qcs=qcs.stripWhiteSpace();
}


/*!
 * parses a function proto
 * @param text function string
 * @param name points to the function name
 * @param ret Stores the return type
 * @param doc ???
 */
void VhdlDocGen::parseFuncProto(const QCString &text,QCString& name,QCString& ret,bool doc)
{
  QCString s1(text);
  QCString temp;

  int index=s1.find("(");
  if (index<0) index=0;
  int end=s1.findRev(")");

  if ((end-index)>0)
  {
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
    s1=s1.stripWhiteSpace();
    int i=s1.find('(');
    int s=s1.find(' ');
    if (s==-1) s=s1.find('\t');
    if (i==-1 || i<s)
      s1=VhdlDocGen::getIndexWord(s1,1);
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

QCString VhdlDocGen::getIndexWord(const QCString &c,int index)
{
  static const reg::Ex reg(R"([\s:|])");
  auto ql=split(c.str(),reg);

  if (index < static_cast<int>(ql.size()))
  {
    return ql[index];
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

/*!
 * deletes a char backwards in a string
 */

bool VhdlDocGen::deleteCharRev(QCString &s,char c)
{
  int index=s.findRev(c,-1,FALSE);
  if (index > -1)
  {
    s = s.remove(index,1);
    return TRUE;
  }
  return FALSE;
}

void VhdlDocGen::deleteAllChars(QCString &s,char c)
{
  int index=s.findRev(c,-1,FALSE);
  while (index > -1)
  {
    s = s.remove(index,1);
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
  qsnprintf(buf,12,"%d",recordCounter++);
  QCString qcs(&buf[0]);
  return qcs;
}

/*!
 * returns the next number of an anonymous process
 */

QCString VhdlDocGen::getProcessNumber()
{
  static int stringCounter;
  QCString qcs("PROCESS_");
  char buf[8];
  qsnprintf(buf,8,"%d",stringCounter++);
  qcs.append(&buf[0]);
  return qcs;
}

/*!
 * writes a colored and formatted string
 */

void VhdlDocGen::writeFormatString(const QCString& s,OutputList&ol,const MemberDef* mdef)
{
  static const reg::Ex reg(R"([\[\]./<>:\s,;'+*|&=()\"-])");
  QCString qcs = s;
  qcs+=QCString(" ");// parsing the last sign
  QCString find=qcs;
  QCString temp=qcs;
  char buf[2];
  buf[1]='\0';

  int j = findIndex(temp.str(),reg);

  ol.startBold();
  if (j>=0)
  {
    while (j>=0)
    {
      find=find.left(j);
      buf[0]=temp[j];
      const char *ss=VhdlDocGen::findKeyWord(find);
      bool k=isNumber(find.str()); // is this a number
      if (k)
      {
        ol.docify(" ");
        startFonts(find,"vhdldigit",ol);
        ol.docify(" ");
      }
      else if (j != 0 && ss)
      {
        startFonts(find,ss,ol);
      }
      else
      {
        if (j>0)
        {
          VhdlDocGen::writeStringLink(mdef,find,ol);
        }
      }
      startFonts(&buf[0],"vhdlchar",ol);

      QCString st=temp.remove(0,j+1);
      find=st;
      if (!find.isEmpty() && find.at(0)=='"')
      {
        int ii=find.find('"',2);
        if (ii>1)
        {
          QCString com=find.left(ii+1);
          startFonts(com,"keyword",ol);
          temp=find.remove(0,ii+1);
        }
      }
      else
      {
        temp=st;
      }
      j = findIndex(temp.str(),reg);
    }//while
  }//if
  else
  {
    startFonts(find,"vhdlchar",ol);
  }
  ol.endBold();
}// writeFormatString

/*!
 * returns TRUE if this string is a number
 */
bool VhdlDocGen::isNumber(const std::string& s)
{
  static const reg::Ex regg(R"([0-9][0-9eEfFbBcCdDaA_.#+?xXzZ-]*)");
  return reg::match(s,regg);
}// isNumber


/*!
 * inserts white spaces for  better readings
 * and writes a colored string to the output
 */

void VhdlDocGen::formatString(const QCString &s, OutputList& ol,const MemberDef* mdef)
{
  QCString qcs = s;
  QCString temp;
  qcs.stripPrefix(":");
  qcs.stripPrefix("is");
  qcs.stripPrefix("IS");
  qcs.stripPrefix("of");
  qcs.stripPrefix("OF");

  size_t len = qcs.length();
  size_t index=1;

  for (size_t j=0;j<len;j++)
  {
    char c=qcs[j];
    char b=c;
    if (j>0) b=qcs[j-1];
    if (c=='"' || c==',' || c=='\''|| c=='(' || c==')'  || c==':' || c=='[' || c==']' ) // || (c==':' && b!='=')) // || (c=='=' && b!='>'))
    {
      if (temp.length()>=index && temp.at(index-1) != ' ')
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
  // printf("\n [%s]",qPrint(qcs));
  VhdlDocGen::writeFormatString(temp,ol,mdef);
}

/*!
 * writes a procedure prototype to the output
 */

void VhdlDocGen::writeProcedureProto(OutputList& ol,const ArgumentList &al,const MemberDef* mdef)
{
  bool sem=FALSE;
  size_t len=al.size();
  ol.docify("( ");
  if (len > 2)
  {
    ol.lineBreak();
  }
  for (const Argument &arg : al)
  {
    ol.startBold();
    if (sem && len <3)
      ol.writeChar(',');

    QCString nn=arg.name;
    nn+=": ";

    QCString defval = arg.defval;
    const char *str=VhdlDocGen::findKeyWord(defval);
    defval+=" ";
    if (str)
    {
      startFonts(defval,str,ol);
    }
    else
    {
      startFonts(defval,"vhdlchar",ol); // write type (variable,constant etc.)
    }

    startFonts(nn,"vhdlchar",ol); // write name
    if (qstricmp(arg.attrib,arg.type) != 0)
    {
      startFonts(arg.attrib.lower(),"stringliteral",ol); // write in|out
    }
    ol.docify(" ");
    VhdlDocGen::formatString(arg.type,ol,mdef);
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

void VhdlDocGen::writeFunctionProto(OutputList& ol,const ArgumentList &al,const MemberDef* mdef)
{
  if (!al.hasParameters()) return;
  bool sem=FALSE;
  size_t len=al.size();
  ol.startBold();
  ol.docify(" ( ");
  ol.endBold();
  if (len>2)
  {
    ol.lineBreak();
  }
  for (const Argument &arg : al)
  {
    ol.startBold();
    QCString att=arg.defval;
     bool bGen=att.stripPrefix("generic");

    if (sem && len < 3)
    {
      ol.docify(" , ");
    }

    if (bGen)
    {
      VhdlDocGen::formatString(QCString("generic "),ol,mdef);
    }
    if (!att.isEmpty())
    {
      const char *str=VhdlDocGen::findKeyWord(att);
      att+=" ";
      if (str)
        VhdlDocGen::formatString(att,ol,mdef);
      else
        startFonts(att,"vhdlchar",ol);
    }

    QCString nn=arg.name;
    nn+=": ";
    QCString ss=arg.type.stripWhiteSpace(); //.lower();
    QCString w=ss.stripWhiteSpace();//.upper();
    startFonts(nn,"vhdlchar",ol);
    startFonts("in ","stringliteral",ol);
    const char *str=VhdlDocGen::findKeyWord(ss);
    if (str)
      VhdlDocGen::formatString(w,ol,mdef);
    else
      startFonts(w,"vhdlchar",ol);

    if (!arg.attrib.isEmpty())
      startFonts(arg.attrib,"vhdlchar",ol);

    sem=TRUE;
    ol.endBold();
    if (len > 2)
    {
      ol.lineBreak();
    }
  }
  ol.startBold();
  ol.docify(" )");
  QCString exp=mdef->excpString();
  if (!exp.isEmpty())
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

void VhdlDocGen::writeProcessProto(OutputList& ol,const ArgumentList &al,const MemberDef* mdef)
{
  if (!al.hasParameters()) return;
  bool sem=FALSE;
  ol.startBold();
  ol.docify(" ( ");
  for (const Argument &arg : al)
  {
    if (sem)
    {
      ol.docify(" , ");
    }
    QCString nn=arg.name;
    // startFonts(nn,"vhdlchar",ol);
    VhdlDocGen::writeFormatString(nn,ol,mdef);
    sem=TRUE;
  }
  ol.docify(" )");
  ol.endBold();
}


/*!
 * writes a function|procedure documentation to the output
 */

bool VhdlDocGen::writeFuncProcDocu(
    const MemberDef *md,
    OutputList& ol,
    const ArgumentList &al,
    bool /*type*/)
{
  //bool sem=FALSE;
  ol.enableAll();

  size_t index=al.size();
  if (index==0)
  {
    ol.docify(" ( ) ");
    return FALSE;
  }
  ol.endMemberDocName();
  ol.startParameterList(TRUE);
  //ol.startParameterName(FALSE);
  bool first=TRUE;
  for (const Argument &arg : al)
  {
    ol.startParameterType(first,"");
    //   if (first) ol.writeChar('(');
    QCString attl=arg.defval;

    //bool bGen=attl.stripPrefix("generic");
    //if (bGen)
    //  VhdlDocGen::writeFormatString(QCString("generic "),ol,md);


    if (VhdlDocGen::isProcedure(md) || VhdlDocGen::isVhdlFunction(md) )
    {
      startFonts(arg.defval,"keywordtype",ol);
      ol.docify(" ");
    }
    ol.endParameterType();

    ol.startParameterName(TRUE);
    VhdlDocGen::writeFormatString(arg.name,ol,md);

    if (VhdlDocGen::isProcedure(md))
    {
      startFonts(arg.attrib,"stringliteral",ol);
    }
    else if (VhdlDocGen::isVhdlFunction(md))
    {
      startFonts(QCString("in"),"stringliteral",ol);
    }

    ol.docify(" ");
    ol.disable(OutputType::Man);
    ol.startEmphasis();
    ol.enable(OutputType::Man);
    if (!VhdlDocGen::isProcess(md))
    {
     // startFonts(arg.type,"vhdlkeyword",ol);
		VhdlDocGen::writeFormatString(arg.type,ol,md);
    }
    ol.disable(OutputType::Man);
    ol.endEmphasis();
    ol.enable(OutputType::Man);

    if (--index)
    {
      ol.docify(" , ");
    }
    else
    {
      //    ol.docify(" ) ");
      ol.endParameterName();
      ol.startParameterExtra();
      ol.endParameterExtra(true,false,true);
      break;
    }
    ol.endParameterName();
    ol.startParameterExtra();
    ol.endParameterExtra(false,false,false);

    //sem=TRUE;
    first=FALSE;
  }
  //ol.endParameterList();
  return TRUE;

} // writeDocFunProc




QCString VhdlDocGen::convertArgumentListToString(const ArgumentList &al,bool func)
{
  QCString argString;
  bool sem=FALSE;

  for (const Argument &arg : al)
  {
    if (sem) argString.append(", ");
    if (func)
    {
      argString+=arg.name;
      argString+=":";
      argString+=arg.type;
    }
    else
    {
      argString+=arg.defval+" ";
      argString+=arg.name+" :";
      argString+=arg.attrib+" ";
      argString+=arg.type;
    }
    sem=TRUE;
  }
  return argString;
}


void VhdlDocGen::writeVhdlDeclarations(const MemberList* ml,
    OutputList& ol,const GroupDef* gd,const ClassDef* cd,const FileDef *fd,const NamespaceDef* nd,const ModuleDef *mod)
{
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::LIBRARY,FALSE),QCString(),FALSE,VhdlSpecifier::LIBRARY);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::USE,FALSE),QCString(),FALSE,VhdlSpecifier::USE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::FUNCTION,FALSE),QCString(),FALSE,VhdlSpecifier::FUNCTION);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::COMPONENT,FALSE),QCString(),FALSE,VhdlSpecifier::COMPONENT);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::CONSTANT,FALSE),QCString(),FALSE,VhdlSpecifier::CONSTANT);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::TYPE,FALSE),QCString(),FALSE,VhdlSpecifier::TYPE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::SUBTYPE,FALSE),QCString(),FALSE,VhdlSpecifier::SUBTYPE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::GENERIC,FALSE),QCString(),FALSE,VhdlSpecifier::GENERIC);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::PORT,FALSE),QCString(),FALSE,VhdlSpecifier::PORT);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::PROCESS,FALSE),QCString(),FALSE,VhdlSpecifier::PROCESS);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::SIGNAL,FALSE),QCString(),FALSE,VhdlSpecifier::SIGNAL);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::ATTRIBUTE,FALSE),QCString(),FALSE,VhdlSpecifier::ATTRIBUTE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::PROCEDURE,FALSE),QCString(),FALSE,VhdlSpecifier::PROCEDURE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::RECORD,FALSE),QCString(),FALSE,VhdlSpecifier::RECORD);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::UNITS,FALSE),QCString(),FALSE,VhdlSpecifier::UNITS);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::SHAREDVARIABLE,FALSE),QCString(),FALSE,VhdlSpecifier::SHAREDVARIABLE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::VFILE,FALSE),QCString(),FALSE,VhdlSpecifier::VFILE);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::GROUP,FALSE),QCString(),FALSE,VhdlSpecifier::GROUP);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::INSTANTIATION,FALSE),QCString(),FALSE,VhdlSpecifier::INSTANTIATION);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::ALIAS,FALSE),QCString(),FALSE,VhdlSpecifier::ALIAS);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::MISCELLANEOUS,TRUE),QCString(),FALSE,VhdlSpecifier::MISCELLANEOUS);

  // configurations must be added to global file definitions.
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::CONFIG,FALSE),QCString(),FALSE,VhdlSpecifier::CONFIG);
  VhdlDocGen::writeVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,theTranslator_vhdlType(VhdlSpecifier::UCF_CONST,FALSE),QCString(),FALSE,VhdlSpecifier::UCF_CONST);

}

void VhdlDocGen::correctMemberProperties(MemberDefMutable *md)
{
  if (md->argsString()=="package")
  {
    md->setVhdlSpecifiers(VhdlSpecifier::INSTANTIATION);
  }
  else if (md->argsString()=="configuration")
  {
    md->setVhdlSpecifiers(VhdlSpecifier::CONFIG);
  }
  else if (md->typeString()=="library")
  {
    md->setVhdlSpecifiers(VhdlSpecifier::LIBRARY);
  }
  else if (md->typeString()=="use")
  {
    md->setVhdlSpecifiers(VhdlSpecifier::USE);
  }
  else if (md->typeString().lower()=="misc")
  {
    md->setVhdlSpecifiers(VhdlSpecifier::MISCELLANEOUS);
  }
  else if (md->typeString().lower()=="ucf_const")
  {
    md->setVhdlSpecifiers(VhdlSpecifier::UCF_CONST);
  }

  if (md->getVhdlSpecifiers()==VhdlSpecifier::UCF_CONST)
  {
    int mm=md->name().findRev('_');
    if (mm>0)
    {
      md->setName(md->name().left(mm));
    }
  }
  else if (md->getVhdlSpecifiers()==VhdlSpecifier::TYPE)
  {
    QCString largs=md->argsString();
    bool bRec=largs.stripPrefix("record") ;
    bool bUnit=largs.stripPrefix("units") ;
    if (bRec || bUnit)
    {
      md->setType("");
    }
  }
}

/* writes a vhdl type documentation */
bool VhdlDocGen::writeVHDLTypeDocumentation(const MemberDef* mdef, const Definition *d, OutputList &ol)
{
  const ClassDef *cd=toClassDef(d);
  bool hasParams = FALSE;

  if (cd==nullptr) return hasParams;

  QCString ttype=mdef->typeString();
  QCString largs=mdef->argsString();

  if ((VhdlDocGen::isVhdlFunction(mdef) || VhdlDocGen::isProcedure(mdef) || VhdlDocGen::isProcess(mdef)))
  {
    QCString nn=mdef->typeString();
    nn=nn.stripWhiteSpace();
    QCString na=cd->name();
    const MemberDef* memdef=VhdlDocGen::findMember(na,nn);
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
    hasParams = VhdlDocGen::writeFuncProcDocu(mdef,ol, mdef->argumentList());
  }


  if (mdef->isVariable())
  {
    if (VhdlDocGen::isConstraint(mdef))
    {
      writeLink(mdef,ol);
      ol.docify(" ");

      largs=substitute(largs,"#"," ");
      VhdlDocGen::formatString(largs,ol,mdef);
      return hasParams;
    }
    else
    {
      writeLink(mdef,ol);
      if (VhdlDocGen::isLibrary(mdef) || VhdlDocGen::isPackage(mdef))
      {
        return hasParams;
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
      return hasParams;
    }

    ol.docify(" ");
    if (VhdlDocGen::isPort(mdef) || VhdlDocGen::isGeneric(mdef))
    {
      // QCString largs=mdef->argsString();
      VhdlDocGen::formatString(largs,ol,mdef);
      ol.docify(" ");
    }
  }
  return hasParams;
}

void VhdlDocGen::writeTagFile(MemberDefMutable *mdef,TextStream &tagFile)
{
  tagFile << "    <member kind=\"";
  if (VhdlDocGen::isGeneric(mdef))      tagFile << "generic";
  if (VhdlDocGen::isPort(mdef))         tagFile << "port";
  if (VhdlDocGen::isEntity(mdef))       tagFile << "entity";
  if (VhdlDocGen::isComponent(mdef))    tagFile << "component";
  if (VhdlDocGen::isVType(mdef))        tagFile << "type";
  if (VhdlDocGen::isConstant(mdef))     tagFile << "constant";
  if (VhdlDocGen::isSubType(mdef))      tagFile << "subtype";
  if (VhdlDocGen::isVhdlFunction(mdef)) tagFile << "function";
  if (VhdlDocGen::isProcedure(mdef))    tagFile << "procedure";
  if (VhdlDocGen::isProcess(mdef))      tagFile << "process";
  if (VhdlDocGen::isSignals(mdef))      tagFile << "signal";
  if (VhdlDocGen::isAttribute(mdef))    tagFile << "attribute";
  if (VhdlDocGen::isRecord(mdef))       tagFile << "record";
  if (VhdlDocGen::isLibrary(mdef))      tagFile << "library";
  if (VhdlDocGen::isPackage(mdef))      tagFile << "package";
  if (VhdlDocGen::isVariable(mdef))     tagFile << "shared variable";
  if (VhdlDocGen::isFile(mdef))         tagFile << "file";
  if (VhdlDocGen::isGroup(mdef))        tagFile << "group";
  if (VhdlDocGen::isCompInst(mdef))     tagFile << "instantiation";
  if (VhdlDocGen::isAlias(mdef))        tagFile << "alias";
  if (VhdlDocGen::isCompInst(mdef))     tagFile << "configuration";

  QCString fn = mdef->getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  tagFile << "\">\n";
  tagFile << "      <type>" << convertToXML(mdef->typeString()) << "</type>\n";
  tagFile << "      <name>" << convertToXML(mdef->name()) << "</name>\n";
  tagFile << "      <anchorfile>" << convertToXML(fn) << "</anchorfile>\n";
  tagFile << "      <anchor>" << convertToXML(mdef->anchor()) << "</anchor>\n";

  if (VhdlDocGen::isVhdlFunction(mdef))
    tagFile << "      <arglist>" << convertToXML(VhdlDocGen::convertArgumentListToString(mdef->argumentList(),TRUE)) << "</arglist>\n";
  else if (VhdlDocGen::isProcedure(mdef))
    tagFile << "      <arglist>" << convertToXML(VhdlDocGen::convertArgumentListToString(mdef->argumentList(),FALSE)) << "</arglist>\n";
  else
    tagFile << "      <arglist>" << convertToXML(mdef->argsString()) << "</arglist>\n";

  mdef->writeDocAnchorsToTagFile(tagFile);
  tagFile << "    </member>\n";
}

/* writes a vhdl type declaration */

void VhdlDocGen::writeVHDLDeclaration(MemberDefMutable* mdef,OutputList &ol,
    const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
    bool /*inGroup*/)
{
  const Definition *d=nullptr;

  ASSERT(cd!=nullptr || nd!=nullptr || fd!=nullptr || gd!=nullptr || mod!=nullptr ||
      mdef->getVhdlSpecifiers()==VhdlSpecifier::LIBRARY ||
      mdef->getVhdlSpecifiers()==VhdlSpecifier::USE
      ); // member should belong to something
  if (cd) d=cd;
  else if (nd) d=nd;
  else if (fd) d=fd;
  else if (mod) d=mod;
  else if (gd) d=gd;
  else d=mdef;

  // write search index info
  if (Doxygen::searchIndex.enabled())
  {
    Doxygen::searchIndex.setCurrentDoc(mdef,mdef->anchor(),FALSE);
    Doxygen::searchIndex.addWord(mdef->localName(),TRUE);
    Doxygen::searchIndex.addWord(mdef->qualifiedName(),FALSE);
  }

  QCString cname  = d->name();
  QCString cfname = d->getOutputFileBase();

  //HtmlHelp *htmlHelp=nullptr;
  //  bool hasHtmlHelp = Config_getBool(GENERATE_HTML) && Config_getBool(GENERATE_HTMLHELP);
  //  if (hasHtmlHelp) htmlHelp = HtmlHelp::getInstance();

  // search for the last anonymous scope in the member type
  ClassDef *annoClassDef=mdef->getClassDefOfAnonymousType();

  // start a new member declaration
  OutputGenerator::MemberItemType memType = annoClassDef!=nullptr ? OutputGenerator::MemberItemType::AnonymousStart :
                                                              OutputGenerator::MemberItemType::Normal;
  ///printf("startMemberItem for %s\n",qPrint(name()));
  VhdlSpecifier mm=mdef->getVhdlSpecifiers();
  if (mm==VhdlSpecifier::MISCELLANEOUS)
      memType=OutputGenerator::MemberItemType::Templated;

   ol.startMemberItem( mdef->anchor(), memType );

  // If there is no detailed description we need to write the anchor here.
  bool detailsVisible = mdef->hasDetailedDescription();
  if (!detailsVisible)
  {
    QCString doxyName=mdef->name();
    if (!cname.isEmpty()) doxyName.prepend(cname+"::");
    QCString doxyArgs=mdef->argsString();
    ol.startDoxyAnchor(cfname,cname,mdef->anchor(),doxyName,doxyArgs);
    ol.addLabel(cfname,mdef->anchor());

    ol.pushGeneratorState();
    ol.disable(OutputType::Man);
    ol.disable(OutputType::Latex);
    ol.docify("\n");
    ol.popGeneratorState();

  }
  // *** write type
  /*VHDL CHANGE */

  QCString ltype(mdef->typeString());
  QCString largs(mdef->argsString());

  ClassDef *kl=nullptr;
  const ArgumentList &al = mdef->argumentList();
  QCString nn;
  //VhdlDocGen::adjustRecordMember(mdef);
  if (gd) gd=nullptr;
  switch (mm)
  {
    case VhdlSpecifier::MISCELLANEOUS:
      VhdlDocGen::writeSource(mdef,ol,nn);
      break;
    case VhdlSpecifier::PROCEDURE:
    case VhdlSpecifier::FUNCTION:
      ol.startBold();
      VhdlDocGen::formatString(ltype,ol,mdef);
      ol.endBold();
      ol.insertMemberAlign();
      ol.docify(" ");

      writeLink(mdef,ol);
      if (al.hasParameters() && mm==VhdlSpecifier::FUNCTION)
        VhdlDocGen::writeFunctionProto(ol,al,mdef);

      if (al.hasParameters() && mm==VhdlSpecifier::PROCEDURE)
        VhdlDocGen::writeProcedureProto(ol,al,mdef);

      break;
    case VhdlSpecifier::USE:
      kl=VhdlDocGen::getClass(mdef->name());
      if (kl && (VhdlDocGen::convert(kl->protection())==VhdlDocGen::ENTITYCLASS)) break;
      writeLink(mdef,ol);
      ol.insertMemberAlign();
      ol.docify("  ");

      if (kl)
      {
        nn=kl->getOutputFileBase();
        ol.pushGeneratorState();
        ol.disableAllBut(OutputType::Html);
        ol.docify(" ");
        QCString name=theTranslator_vhdlType(VhdlSpecifier::PACKAGE,TRUE);
        ol.startBold();
        ol.docify(name);
        name.clear();
        ol.endBold();
        name+=" <"+mdef->name()+">";
        ol.startEmphasis();
        ol.writeObjectLink(kl->getReference(),kl->getOutputFileBase(),QCString(),name);
        ol.popGeneratorState();
      }
      break;
    case VhdlSpecifier::LIBRARY:
      writeLink(mdef,ol);
      ol.insertMemberAlign();
      if (largs=="context")
      {
        VhdlDocGen::writeRecordUnit(ltype,largs,ol,mdef);
      }

      break;

    case VhdlSpecifier::GENERIC:
    case VhdlSpecifier::PORT:
    case VhdlSpecifier::ALIAS:

      writeLink(mdef,ol);
      ol.docify(" ");
      if (mm==VhdlSpecifier::GENERIC)
      {
        ol.insertMemberAlign();
        ol.startBold();
        VhdlDocGen::formatString(largs,ol,mdef);
        ol.endBold();
      }
      else
      {
        ol.insertMemberAlignLeft(memType, false);
        ol.docify(" ");
        ol.startBold();
        VhdlDocGen::formatString(ltype,ol,mdef);
        ol.endBold();
        ol.insertMemberAlign();
        ol.docify(" ");
        VhdlDocGen::formatString(largs,ol,mdef);
      }
      break;
    case VhdlSpecifier::PROCESS:
      writeLink(mdef,ol);
      ol.insertMemberAlign();
      VhdlDocGen::writeProcessProto(ol,al,mdef);
      break;
    case VhdlSpecifier::PACKAGE:
    case VhdlSpecifier::ENTITY:
    case VhdlSpecifier::COMPONENT:
    case VhdlSpecifier::INSTANTIATION:
    case VhdlSpecifier::CONFIG:
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
        largs.prepend(mdef->name());
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
          nn=ltype;
        }
        else
        {
          nn=mdef->name();
        }
        kl=getClass(nn);
        if (kl)
        {
          nn=kl->getOutputFileBase();
          ol.pushGeneratorState();
          ol.disableAllBut(OutputType::Html);
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
          ol.writeObjectLink(kl->getReference(),kl->getOutputFileBase(),QCString(),name);
          ol.endEmphasis();
          ol.popGeneratorState();
        }
      }
      break;
    case VhdlSpecifier::UCF_CONST:
      writeUCFLink(mdef,ol);
      break;
    case VhdlSpecifier::SIGNAL:
    case VhdlSpecifier::ATTRIBUTE:
    case VhdlSpecifier::SUBTYPE:
    case VhdlSpecifier::CONSTANT:
    case VhdlSpecifier::SHAREDVARIABLE:
    case VhdlSpecifier::VFILE:
    case VhdlSpecifier::GROUP:
    case VhdlSpecifier::TYPE:
      writeLink(mdef,ol);
      ol.docify(" ");
      ol.insertMemberAlign();
      VhdlDocGen::formatString(ltype,ol,mdef);
      break;
    case VhdlSpecifier::RECORD:
    case VhdlSpecifier::UNITS:
     writeRecordUnit(largs,ltype,ol,mdef);
      break;

    default: break;
  }

  bool htmlOn = ol.isEnabled(OutputType::Html);
  if (htmlOn && /*Config_getBool(HTML_ALIGN_MEMBERS) &&*/ !ltype.isEmpty())
  {
    ol.disable(OutputType::Html);
  }
  if (!ltype.isEmpty()) ol.docify(" ");

  if (htmlOn)
  {
    ol.enable(OutputType::Html);
  }

  if (!detailsVisible)
  {
    ol.endDoxyAnchor(cfname,mdef->anchor());
  }

  ol.endMemberItem(memType);
  if (!mdef->briefDescription().isEmpty() &&   Config_getBool(BRIEF_MEMBER_DESC) /* && !annMemb */)
  {
    QCString s=mdef->briefDescription();
    ol.startMemberDescription(mdef->anchor(), QCString(), mm == VhdlSpecifier::PORT);
    ol.generateDoc(mdef->briefFile(),
                   mdef->briefLine(),
                   mdef->getOuterScope()?mdef->getOuterScope():d,
                   mdef,
                   s,
                   DocOptions()
                   .setIndexWords(true)
                   .setSingleLine(true));
    if (detailsVisible)
    {
      ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Html);
      ol.docify(" ");
      if (mdef->getGroupDef()!=nullptr && gd==nullptr) // forward link to the group
      {
        ol.startTextLink(mdef->getOutputFileBase(),mdef->anchor());
      }
      else // local link
      {
        ol.startTextLink(QCString(),mdef->anchor());
      }
      ol.endTextLink();
      ol.popGeneratorState();
    }
    ol.endMemberDescription();
  }
  mdef->warnIfUndocumented();

}// end writeVhdlDeclaration


void VhdlDocGen::writePlainVHDLDeclarations(
    const MemberList* mlist,OutputList &ol,
    const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
    VhdlSpecifier specifier)
{

  StringSet pack;

  bool first=TRUE;
  for (const auto &imd : *mlist)
  {
    MemberDefMutable *md = toMemberDefMutable(imd);
    if (md)
    {
      VhdlSpecifier mems=md->getVhdlSpecifiers();
      if (md->isBriefSectionVisible() && (mems==specifier) && (mems!=VhdlSpecifier::LIBRARY) )
      {
        if (first) { ol.startMemberList();first=FALSE; }
        VhdlDocGen::writeVHDLDeclaration(md,ol,cd,nd,fd,gd,mod,FALSE);
      } //if
      else if (md->isBriefSectionVisible() && (mems==specifier))
      {
        if (pack.find(md->name().str())==pack.end())
        {
          if (first) ol.startMemberList(),first=FALSE;
          VhdlDocGen::writeVHDLDeclaration(md,ol,cd,nd,fd,gd,mod,FALSE);
          pack.insert(md->name().str());
        }
      } //if
    } //if
  } //for
  if (!first) ol.endMemberList();
}//plainDeclaration

static bool membersHaveSpecificType(const MemberList *ml,VhdlSpecifier type)
{
  if (ml==nullptr) return FALSE;
  for (const auto &mdd : *ml)
  {
    if (mdd->getVhdlSpecifiers()==type) //is type in class
    {
      return TRUE;
    }
  }
  for (const auto &mg : ml->getMemberGroupList())
  {
    if (!mg->members().empty())
    {
      if (membersHaveSpecificType(&mg->members(),type)) return TRUE;
    }
  }
  return FALSE;
}

void VhdlDocGen::writeVHDLDeclarations(const MemberList* ml,OutputList &ol,
    const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
    const QCString &title,const QCString &subtitle,bool /*showEnumValues*/,VhdlSpecifier type)
{
  if (!membersHaveSpecificType(ml,type)) return;

  if (!title.isEmpty())
  {
    ol.startMemberHeader(convertToId(title),type == VhdlSpecifier::PORT ? 3 : 2);
    ol.parseText(title);
    ol.endMemberHeader();
    ol.docify(" ");
  }
  if (!subtitle.isEmpty())
  {
    ol.startMemberSubtitle();
    ol.generateDoc("[generated]",
                   -1,
                   nullptr,
                   nullptr,
                   subtitle,
                   DocOptions()
                   .setSingleLine(true));
    ol.endMemberSubtitle();
  } //printf("memberGroupList=%p\n",memberGroupList);

  VhdlDocGen::writePlainVHDLDeclarations(ml,ol,cd,nd,fd,gd,mod,type);

  int groupId=0;
  for (const auto &mg : ml->getMemberGroupList())
  {
    if (membersHaveSpecificType(&mg->members(),type))
    {
      //printf("mg->header=%s\n",qPrint(mg->header()));
      bool hasHeader=!mg->header().isEmpty();
      QCString groupAnchor = QCString(ml->listType().toLabel())+"-"+QCString().setNum(groupId++);
      ol.startMemberGroupHeader(groupAnchor,hasHeader);
      if (hasHeader)
      {
        ol.parseText(mg->header());
      }
      ol.endMemberGroupHeader();
      if (!mg->documentation().isEmpty())
      {
        //printf("Member group has docs!\n");
        ol.startMemberGroupDocs();
        ol.generateDoc("[generated]",
                       -1,
                       nullptr,
                       nullptr,
                       mg->documentation()+"\n",
                       DocOptions());
        ol.endMemberGroupDocs();
      }
      ol.startMemberGroup();
      //printf("--- mg->writePlainDeclarations ---\n");
      VhdlDocGen::writePlainVHDLDeclarations(&mg->members(),ol,cd,nd,fd,gd,mod,type);
      ol.endMemberGroup(hasHeader);
    }
  }
}// writeVHDLDeclarations


bool VhdlDocGen::writeClassType( const ClassDef * cd,
    OutputList &ol ,QCString & cname)
{
  QCString qcs = theTranslator->trVhdlType(getSpecifierTypeFromClass(cd),TRUE);
  cname=VhdlDocGen::getClassName(cd);
  ol.startBold();
  ol.writeString(qcs);
  ol.writeString(" ");
  ol.endBold();
  //ol.insertMemberAlign();
  return FALSE;
}// writeClassLink


/*! writes a link if the string is linkable else a formatted string */

void VhdlDocGen::writeStringLink(const MemberDef *mdef,QCString mem, OutputList& ol)
{
  if (mdef)
  {
    const ClassDef *cd=mdef->getClassDef();
    if (cd)
    {
      QCString n=cd->name();
      const MemberDef* memdef=VhdlDocGen::findMember(n,mem);
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
  startFonts(mem,"vhdlchar",ol);
}// found component



void VhdlDocGen::writeSource(const MemberDef* mdef,OutputList& ol,const QCString &cname)
{
  auto intf = Doxygen::parserManager->getCodeParser(".vhd");
 // pIntf->resetCodeParserState();

  QCString codeFragment=mdef->documentation();

  if (cname.isEmpty())
  {
    writeLink(mdef,ol);
    int fi=0;
    int j=0;
    do
    {
     fi=codeFragment.find("\n",++fi);
    } while(fi>=0 && j++ <3);

    // show only the first four lines
    if (j==4)
    {
      codeFragment=codeFragment.left(fi);
      codeFragment.append("\n    ....    ");
    }
  }

  codeFragment.prepend("\n");
  ol.pushGeneratorState();
  auto &codeOL = ol.codeGenerators();
  codeOL.startCodeFragment("DoxyCode");
  intf->parseCode(codeOL,           // codeOutIntf
                  QCString(),       // scope
                  codeFragment,     // input
                  SrcLangExt::VHDL,  // lang
                  Config_getBool(STRIP_CODE_COMMENTS),
                  CodeParserOptions()
                  .setFileDef(mdef->getFileDef())
                  .setStartLine(mdef->getStartBodyLine())
                  .setEndLine(mdef->getEndBodyLine())
                  .setInlineFragment(true)
                  .setMemberDef(mdef)
                 );

  codeOL.endCodeFragment("DoxyCode");
  ol.popGeneratorState();

  if (cname.isEmpty()) return;

  MemberDefMutable *mdm = toMemberDefMutable(const_cast<MemberDef*>(mdef));
  if (mdm)
  {
    mdm->writeSourceDef(ol);
    if (mdef->hasReferencesRelation()) mdm->writeSourceRefs(ol,cname);
    if (mdef->hasReferencedByRelation()) mdm->writeSourceReffedBy(ol,cname);
  }
}



QCString VhdlDocGen::convertFileNameToClassName(const QCString &name)
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

void VhdlDocGen::parseUCF(const QCString &input,Entry* entity,const QCString &fileName,bool altera)
{
  QCString ucFile(input);
  int lineNo=0;
  QCString comment("#!");
  QCString brief;

  while (!ucFile.isEmpty())
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
          int in=temp.find("-name");
          if (in>0)
          {
            temp=temp.remove(0,in+5);
          }

          temp.stripPrefix("set_location_assignment");

          initUCF(entity,QCString(),temp,lineNo,fileName,brief);
        }
        else
        {
          static const reg::Ex ee(R"([\s=])");
          int in=findIndex(temp.str(),ee);
          if (in<0) in=0;
          QCString ff=temp.left(in);
          temp.stripPrefix(ff);
          ff.append("#");
          if (!temp.isEmpty())
          {
            initUCF(entity,ff,temp,lineNo,fileName,brief);
          }
        }
      }
    }//temp

    ucFile=ucFile.remove(0,i+1);
  }// while
}

static void initUCF(Entry* root,const QCString &type,QCString &qcs,
                    int line,const QCString &fileName,QCString & brief)
{
  if (qcs.isEmpty())return;
  QCString n;

  VhdlDocGen::deleteAllChars(qcs,';');
  qcs=qcs.stripWhiteSpace();

  static const reg::Ex reg(R"([\s=])");
  int i = findIndex(qcs.str(),reg);
  if (i<0) return;
  if (i==0)
  {
    n=type;
    VhdlDocGen::deleteAllChars(n,'#');
  }
  else
  {
    n=qcs.left(i);
  }
  qcs=qcs.remove(0,i+1);
  //  qcs.prepend("|");

  qcs.stripPrefix("=");

  std::shared_ptr<Entry> current = std::make_shared<Entry>();
  current->vhdlSpec=VhdlSpecifier::UCF_CONST;
  current->section=EntryType::makeVariable();
  current->bodyLine=line;
  current->fileName=fileName;
  current->type="ucf_const";
  current->args+=qcs;
  current->lang=  SrcLangExt::VHDL ;

  // adding dummy name for constraints like VOLTAGE=5,TEMPERATURE=20 C
  if (n.isEmpty())
  {
    n="dummy";
    n+=VhdlDocGen::getRecordNumber();
  }

  current->name= n+"_";
  current->name.append(VhdlDocGen::getRecordNumber());

  if (!brief.isEmpty())
  {
    current->brief=brief;
    current->briefLine=line;
    current->briefFile=fileName;
    brief.clear();
  }

  root->moveToSubEntryAndKeep(current);
}


static void writeUCFLink(const MemberDef* mdef,OutputList &ol)
{

  QCString largs(mdef->argsString());
  QCString n= splitString(largs, '#');
  // VhdlDocGen::adjustRecordMember(mdef);
  bool equ=(n.length()==largs.length());

  if (!equ)
  {
    ol.writeString(n);
    ol.docify(" ");
    ol.insertMemberAlign();
  }

  if (mdef->name().contains("dummy")==0)
  {
    writeLink(mdef,ol);
  }
  if (equ)
  {
    ol.insertMemberAlign();
  }
  ol.docify(" ");
  VhdlDocGen::formatString(largs,ol,mdef);
}

//        for cell_inst : [entity] work.proto [ (label|expr) ]
QCString VhdlDocGen::parseForConfig(QCString & entity,QCString & arch)
{
  if (!entity.contains(":")) return "";

  static const reg::Ex exp(R"([:()\s])");
  auto ql=split(entity.str(),exp);
  if (ql.size()<2)
  {
    return "";
  }
  QCString label(ql[0]);
  entity = ql[1];
  int index = entity.findRev(".");
  if (index!=-1)
  {
    entity.remove(0,index+1);
  }

  if (ql.size()==3)
  {
    arch = ql[2];
    ql=split(arch.str(),exp);
    if (ql.size()>1) // expression
    {
      arch="";
    }
  }
  return label; // label
}

//        use (configuration|entity|open) work.test [(cellfor)];

QCString  VhdlDocGen::parseForBinding(QCString & entity,QCString & arch)
{
  static const reg::Ex exp(R"([()\s])");

  auto ql = split(entity.str(),exp);

  if (findIndex(ql,"open")!=-1)
  {
    return "open";
  }

  if (ql.size()<2)
  {
    return "";
  }

  std::string label=ql[0];
  entity = ql[1];
  int index=entity.findRev(".");
  if (index!=-1)
  {
    entity.remove(0,index+1);
  }

  if (ql.size()==3)
  {
    arch=ql[2];
  }
  return label;
}



// find class with upper/lower letters
ClassDef* VhdlDocGen::findVhdlClass(const QCString &className )
{
 for (const auto &cd : *Doxygen::classLinkedMap)
 {
   if (qstricmp(className.data(),qPrint(cd->name()))==0)
   {
     return cd.get();
   }
 }
 return nullptr;
}


/*

// file foo.vhd
// entity foo
//        .....
// end entity

// file foo_arch.vhd
// architecture xxx of foo is
//          ........
//  end architecture

*/
void VhdlDocGen::computeVhdlComponentRelations()
{

  QCString entity,arch,inst;

  for (const auto &cur : getVhdlInstList())
  {
    if (cur->isStatic ) //  was bind
    {
      continue;
    }

    if (cur->includeName=="entity" || cur->includeName=="component" )
    {
      entity=cur->includeName+" "+cur->type;
      QCString rr=VhdlDocGen::parseForBinding(entity,arch);
    }
    else if (cur->includeName.isEmpty())
    {
      entity=cur->type;
    }

    ClassDefMutable *classEntity= toClassDefMutable(VhdlDocGen::findVhdlClass(entity));
    inst=VhdlDocGen::getIndexWord(cur->args,0);
    ClassDefMutable *cd=toClassDefMutable(Doxygen::classLinkedMap->find(inst));
    ClassDefMutable *ar=toClassDefMutable(Doxygen::classLinkedMap->find(cur->args));

    if (cd==nullptr)
    {
      continue;
    }

    addInstance(classEntity,ar,cd,cur);
  }

}

static void addInstance(ClassDefMutable* classEntity, ClassDefMutable* ar,
                        ClassDefMutable *cd , const std::shared_ptr<Entry> &cur)
{

  QCString bName,n1;
  if (ar==nullptr) return;

  if (classEntity==nullptr)
  {
    //add component inst
    n1=cur->type;
    goto ferr;
  }

  if (classEntity==cd) return;

  bName=classEntity->name();
  // fprintf(stderr,"\naddInstance %s to %s %s %s\n",qPrint( classEntity->name()),qPrint(cd->name()),qPrint(ar->name()),cur->name);
  n1=classEntity->name();

  if (!cd->isBaseClass(classEntity, true))
  {
    cd->insertBaseClass(classEntity,n1,Protection::Public,Specifier::Normal,QCString());
  }
  else
  {
    VhdlDocGen::addBaseClass(cd,classEntity);
  }

  if (!VhdlDocGen::isSubClass(classEntity,cd,true,0))
  {
    classEntity->insertSubClass(cd,Protection::Public,Specifier::Normal,QCString());
    classEntity->setLanguage(SrcLangExt::VHDL);
  }

ferr:
  QCString uu=cur->name;
  auto md = createMemberDef(
      ar->getDefFileName(), cur->startLine,cur->startColumn,
      n1,uu,uu, QCString(),
      Protection::Public,
      Specifier::Normal,
      cur->isStatic,
      Relationship::Member,
      MemberType::Variable,
      ArgumentList(),
      ArgumentList(),
      "");
  auto mmd = toMemberDefMutable(md.get());

  if (!ar->getOutputFileBase().isEmpty())
  {
    TagInfo tg;
    tg.anchor = nullptr;
    tg.fileName = ar->getOutputFileBase();
    tg.tagName = nullptr;
    mmd->setTagInfo(&tg);
  }

  //fprintf(stderr,"\n%s%s%s\n",qPrint(md->name()),qPrint(cur->brief),qPrint(cur->doc));

  mmd->setLanguage(SrcLangExt::VHDL);
  mmd->setVhdlSpecifiers(VhdlSpecifier::INSTANTIATION);
  mmd->setBriefDescription(cur->brief,cur->briefFile,cur->briefLine);
  mmd->setBodySegment(cur->startLine,cur->startLine,-1) ;
  mmd->setDocumentation(cur->doc,cur->docFile,cur->docLine);
  FileDef *fd=ar->getFileDef();
  mmd->setBodyDef(fd);
  ar->insertMember(md.get());
  MemberName *mn = Doxygen::functionNameLinkedMap->add(uu);
  mn->push_back(std::move(md));

}


void  VhdlDocGen::writeRecordUnit(QCString &/* largs */,QCString & ltype,OutputList& ol ,MemberDefMutable *mdef)
{
  int i=mdef->name().find('~');
  if (i>0)
  {
    //sets the real record member name
    mdef->setName(mdef->name().left(i));
  }

  writeLink(mdef,ol);
  ol.startBold();
  ol.insertMemberAlign();
  if (!ltype.isEmpty())
  {
    VhdlDocGen::formatString(ltype,ol,mdef);
  }
  ol.endBold();
}


void VhdlDocGen::writeRecUnitDocu(
    const MemberDef *md,
    OutputList& ol,
    QCString largs)
{

  StringVector ql=split(largs.str(),"#");
  size_t len=ql.size();
  ol.startParameterList(TRUE);
  bool first=TRUE;

  for(size_t i=0;i<len;i++)
  {
    QCString n = ql[i];
    ol.startParameterType(first,"");
    ol.endParameterType();
    ol.startParameterName(TRUE);
    VhdlDocGen::formatString(n,ol,md);
    ol.endParameterName();
    ol.startParameterExtra();
    if ((len-i)>1)
    {
      ol.endParameterExtra(false,false,false);
    }
    else
    {
      ol.endParameterExtra(true,false,true);
    }

    first=FALSE;
  }

}//#



bool VhdlDocGen::isSubClass(ClassDef* cd,ClassDef *scd, bool followInstances,int level)
{
  bool found=FALSE;
  //printf("isBaseClass(cd=%s) looking for %s\n",qPrint(name()),qPrint(bcd->name()));
  if (level>255)
  {
    err("Possible recursive class relation while inside {} and looking for {}\n",cd->name(),scd->name());
    abort();
  }

  for (const auto &bcd :cd->subClasses())
  {
    const ClassDef *ccd=bcd.classDef;
    if (!followInstances && ccd->templateMaster()) ccd=ccd->templateMaster();
    //printf("isSubClass() subclass %s\n",qPrint(ccd->name()));
    if (ccd==scd)
    {
      found=true;
    }
    else
    {
      if (level <256)
      {
        level = ccd->isBaseClass(scd,followInstances);
        if (level>0)
        {
          found=true;
        }
      }
    }
  }
  return found;
}

void VhdlDocGen::addBaseClass(ClassDef* cd,ClassDef *ent)
{
  BaseClassList bcl = cd->baseClasses();
  for (auto &bcd : bcl)
  {
    ClassDef *ccd = bcd.classDef;
    if (ccd==ent)
    {
      QCString n = bcd.usedName;
      int i = n.find('(');
      if(i<0)
      {
        bcd.usedName.append("(2)");
        return;
      }
      static const reg::Ex reg(R"(\d+)");
      QCString s=n.left(i);
      QCString r=n.right(n.length()-i);
      std::string t=r.str();
      VhdlDocGen::deleteAllChars(r,')');
      VhdlDocGen::deleteAllChars(r,'(');
      r.setNum(r.toInt()+1);
      reg::replace(t, reg, r.str());
      s.append(t);
      bcd.usedName=s;
      bcd.templSpecifiers=t;
    }
  }
  cd->updateBaseClasses(bcl);
}


static std::vector<const MemberDef*> mdList;

static const MemberDef* findMemFlow(const MemberDef* mdef)
{
  for (const auto &md : mdList)
  {
    if (md->name()==mdef->name() &&  md->getStartBodyLine()==mdef->getStartBodyLine())
    {
      return md;
    }
  }
  return nullptr;
}

void VhdlDocGen::createFlowChart(const MemberDef *mdef)
{
  if (mdef==nullptr) return;

  QCString codeFragment;
  const MemberDef* mm=nullptr;
  if ((mm=findMemFlow(mdef))!=nullptr)
  {
    // don't create the same flowchart twice
    VhdlDocGen::setFlowMember(mm);
    return;
  }
  else
  {
    mdList.push_back(mdef);
  }

  //fprintf(stderr,"\n create flow mem %s %p\n",qPrint(mdef->name()),mdef);

  int actualStart= mdef->getStartBodyLine();
  int actualEnd=mdef->getEndBodyLine();
  const FileDef* fd=mdef->getFileDef();
  bool b=readCodeFragment( fd->absFilePath(), false, actualStart, actualEnd, codeFragment);
  if (!b) return;

  auto parser { Doxygen::parserManager->getOutlineParser(".vhd") };
  VhdlDocGen::setFlowMember(mdef);
  std::shared_ptr<Entry> root = std::make_shared<Entry>();
  StringVector filesInSameTu;
  parser->parseInput("",codeFragment.data(),root,nullptr);
}

void VhdlDocGen::resetCodeVhdlParserState()
{
  std::lock_guard lock(g_vhdlMutex);
  g_varMap.clear();
  g_classList.clear();
  g_packages.clear();
}

bool VhdlDocGen::isConstraint(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::UCF_CONST; }
bool VhdlDocGen::isConfig(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::CONFIG; }
bool VhdlDocGen::isAlias(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::ALIAS; }
bool VhdlDocGen::isLibrary(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::LIBRARY; }
bool VhdlDocGen::isGeneric(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::GENERIC; }
bool VhdlDocGen::isPort(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::PORT; }
bool VhdlDocGen::isComponent(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::COMPONENT; }
bool VhdlDocGen::isPackage(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::USE; }
bool VhdlDocGen::isEntity(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::ENTITY; }
bool VhdlDocGen::isConstant(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::CONSTANT; }
bool VhdlDocGen::isVType(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::TYPE; }
bool VhdlDocGen::isSubType(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::SUBTYPE; }
bool VhdlDocGen::isVhdlFunction(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::FUNCTION; }
bool VhdlDocGen::isProcess(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::PROCESS; }
bool VhdlDocGen::isSignal(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::SIGNAL; }
bool VhdlDocGen::isAttribute(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::ATTRIBUTE; }
bool VhdlDocGen::isSignals(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::SIGNAL; }
bool VhdlDocGen::isProcedure(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::PROCEDURE; }
bool VhdlDocGen::isRecord(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::RECORD; }
bool VhdlDocGen::isArchitecture(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::ARCHITECTURE; }
bool VhdlDocGen::isUnit(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::UNITS; }
bool VhdlDocGen::isPackageBody(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::PACKAGE_BODY; }
bool VhdlDocGen::isVariable(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::SHAREDVARIABLE; }
bool VhdlDocGen::isFile(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::VFILE; }
bool VhdlDocGen::isGroup(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::GROUP; }
bool VhdlDocGen::isCompInst(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::INSTANTIATION; }
bool VhdlDocGen::isMisc(const MemberDef *mdef)
{ return mdef->getVhdlSpecifiers()==VhdlSpecifier::MISCELLANEOUS; }



//############################## Flowcharts #################################################

#define STARTL   (FlowChart::WHILE_NO     | FlowChart::IF_NO    | \
                  FlowChart::FOR_NO       | FlowChart::CASE_NO  | \
                  FlowChart::LOOP_NO      | WHEN_NO)
#define DECLN    (FlowChart::WHEN_NO      | \
                  FlowChart::ELSIF_NO     | FlowChart::IF_NO    | \
                  FlowChart::FOR_NO       | FlowChart::WHILE_NO | \
                  FlowChart::CASE_NO      | FlowChart::LOOP_NO )
#define STARTFIN (FlowChart::START_NO     | FlowChart::END_NO)
#define LOOP     (FlowChart::FOR_NO       | FlowChart::WHILE_NO | \
                  FlowChart::LOOP_NO )
#define ENDCL    (FlowChart::END_CASE     | FlowChart::END_LOOP)
#define EEND     (FlowChart::ENDIF_NO     | FlowChart::ELSE_NO )
#define IFF      (FlowChart::ELSIF_NO     | FlowChart::IF_NO)
#define EXITNEXT (FlowChart::EXIT_NO      | FlowChart::NEXT_NO )
#define EMPTY    (EEND                    | FlowChart::ELSIF_NO)
#define EE       (FlowChart::ELSE_NO      | FlowChart::ELSIF_NO)
#define EMPTNODE (ENDCL | EEND            | FlowChart::ELSIF_NO)
#define FLOWLEN (flowList.size()-1)

static int ifcounter=0;
static int nodeCounter=0;

static struct
{
   // link colors
   const char *textNodeLink;
   const char *yesNodeLink;
   const char *noNodeLink;

   // node colors
   const char* comment;
   const char* decisionNode;
   const char* varNode;
   const char *startEndNode;
   const char* textNode;
} flowCol =
{ "green",       // textNodeLink
  "red",         // yesNodeLink
  "black",       // noNodeLink
  "khaki",       // comment
  "0.7 0.3 1.0", // decisionNode
  "lightyellow", // varNode
  "white",       // startEndNode
  "lightcyan"    // textNode
};

std::vector<FlowChart> flowList;

#ifdef DEBUGFLOW
static std::map<std::string,int> g_keyMap;
#endif

void alignText(QCString & q)
{
  if (q.length()<=80) return;

  if (q.length()>200)
  {
    q.resize(200);
  }

  q.append(" ...");

  QCString str(q);
  QCString temp;

  while (str.length()>80)
  {
    int j=std::max(str.findRev(' ',80),str.findRev('|',80));
    if (j<=0)
    {
      temp+=str;
      q=temp;
      return;
    }
    else
    {
      QCString qcs=str.left(j);
      temp+=qcs+"\\";
      temp+="n";
      str.remove(0,j);
    }
  }//while

 q=temp+str;
// #endif
}

void FlowChart::printNode(const FlowChart& flo)
{
  QCString ui="-";
  std::string q;
  std::string t;

  ui.fill('-',255);

  if (flo.type & STARTL)
  {
    if (flo.stamp>0)
    {
      q=ui.left(2*flo.stamp).str();
    }
    else
    {
      q=" ";
    }
    QCString nn=flo.exp.stripWhiteSpace();
    printf("\nYES: %s%s[%d,%d]",qPrint(q),qPrint(nn),flo.stamp,flo.id);
  }
  else
  {
    if (flo.type & COMMENT_NO)
    {
      t=flo.label.str();
    }
    else
    {
      t=flo.text.str();
    }
    static const reg::Ex ep(R"(\s)");
    t = reg::replace(t,ep,std::string());
    if (t.empty())
    {
      t=" ";
    }
    if (flo.stamp>0)
    {
      q=ui.left(2*flo.stamp).str();
    }
    else
    {
      q=" ";
    }
    if (flo.type & EMPTNODE)
    {
      printf("\n NO: %s%s[%d,%d]",qPrint(q),FlowChart::getNodeType(flo.type),flo.stamp,flo.id);
    }
    else if (flo.type & COMMENT_NO)
    {
      printf("\n NO: %s%s[%d,%d]",qPrint(t),FlowChart::getNodeType(flo.type),flo.stamp,flo.id);
    }
    else
    {
      printf("\n NO: %s[%d,%d]",qPrint(t),flo.stamp,flo.id);
    }
  }
}

void  FlowChart::printFlowTree()
{
  for (const auto &flowChart : flowList)
  {
    printNode(flowChart);
  }
}

void  FlowChart::colTextNodes()
{
  FlowChart *flno = nullptr;
  bool found=FALSE;
  for (size_t j=0;j<flowList.size();j++)
  {
    FlowChart &flo = flowList[j];
    if (flo.type&TEXT_NO)
    {
      if (!found)
      {
        flno=&flo;
      }
      else
      {
        flno->text+=flo.text;
        flowList.erase(flowList.begin()+j);
        if (j>0) j=j-1;
      }
      found=TRUE;
    }
    else
    {
      found=FALSE;
    }
  }

  // find if..endif without text
  //       if..elseif without text
  if (!flowList.empty())
  {
    for (size_t j=0;j<flowList.size()-1;j++)
    {
      const FlowChart &flo = flowList[j];
      int kind = flo.type;
      if ( (kind & IFF) || (flo.type & ELSE_NO))
      {
        const FlowChart &ftemp = flowList[j+1];
        if (ftemp.type & EMPTY)
        {
          FlowChart fc(TEXT_NO,"empty ",QCString());
          fc.stamp = flo.stamp;
          flowList.insert(flowList.begin()+j+1,fc);
        }
      }
    }
  }

}// colTextNode

QCString FlowChart::getNodeName(int n)
{
  QCString node;
  node.setNum(n);
  return node.prepend("node");
}

void FlowChart::delFlowList()
{
  ifcounter=0;
  nodeCounter=0;
  flowList.clear();
}

void FlowChart::alignCommentNode(TextStream &t,QCString com)
{
  size_t max=0;
  QCString s;
  StringVector ql=split(com.str(),"\n");
  for (size_t j=0;j<ql.size();j++)
  {
    s=ql[j];
    if (max<s.length()) max=s.length();
  }

  s=ql.back();
  int diff=static_cast<int>(max-s.length());

  QCString n;
  if (diff>0)
  {
    n.fill(' ',2*diff);
    n.append(".");
    s+=n;
    ql.pop_back();
    ql.push_back(s.str());
  }

  for (size_t j=0;j<ql.size();j++)
  {
    s=ql[j];
    if (j<ql.size()-1)
    {
      s+="\n";
    }
    FlowChart::codify(t,s);
  }
}


void FlowChart::buildCommentNodes(TextStream & t)
{
  size_t size=flowList.size();
  bool begin=false;

  if (size>0)
  {
    for (uint32_t j=0;j < size-1 ;j++)
    {
      FlowChart &fll = flowList[j];
      if (fll.type & COMMENT_NO)
      {
        FlowChart &to=flowList[j+1];
        if (to.type & COMMENT_NO)
        {
          to.label = fll.label+"\n"+to.label;
          flowList.erase(flowList.begin()+j);
          if (size>0) size--;
          if (j>0) j--;
        }
      }
    }// for
  }

  for (size_t j=0;j <flowList.size() ;j++)
  {
    const FlowChart &fll=flowList[j];

    if (fll.type & BEGIN_NO)
    {
      begin = true;
      continue;
    }

    if (fll.type & COMMENT_NO)
    {
      const FlowChart *to = nullptr;
      if (!begin)
      {
        //  comment between function/process .. begin is linked to start node
        to = &flowList[0];
      }
      else if (j>0 && flowList[j-1].line==fll.line)
      {
        to = &flowList[j-1];
      }
      else
      {
        to = &flowList[j+1];
      }
      t << getNodeName(fll.id);
      t << "[shape=none, label=<\n";
      t << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"2\" >\n ";
      t << "<TR><TD BGCOLOR=\"";
      t << flowCol.comment;
      t << "\" > ";

      FlowChart::alignCommentNode(t,fll.label);
      t << " </TD></TR></TABLE>>];";
      writeEdge(t,fll.id,to->id,2);
    }
  }// for

  // delete comment nodes;
  size=flowList.size();
  for (size_t j=0; j<size; j++)
  {
    FlowChart &fll=flowList[j];
    if (fll.type & (COMMENT_NO | BEGIN_NO))
    {
      size_t diff=FLOWLEN-(j+1);

      if ((fll.type & COMMENT_NO) && diff > 1)
      {
        flowList[j+1].label = fll.label;
      }

      flowList.erase(flowList.begin()+j);

      if (size>0) size--;
      if (j>0) j--;
    }
  }// for;
}

void FlowChart::codify(TextStream &t,const QCString &str)
{
  if (!str.isEmpty())
  {
    const char *p=str.data();
    while (*p)
    {
      char c=*p++;
      switch(c)
      {
        case '<':  t << "&lt;"; break;
        case '>':  t << "&gt;"; break;
        case '&':  t << "&amp;"; break;
        case '\'': t << "&#39;"; break;
        case '"':  t << "&quot;"; break;
        case '\n': t <<"<BR ALIGN=\"LEFT\"/>"; break;
        default:   t << c; break;
      }
    }
  }
}//codify

FlowChart::FlowChart(int typ,const QCString &t,const QCString &ex,const QCString &lab)
{
  stamp=ifcounter;

  if (typ & STARTL)
  {
    ifcounter++;
  }

  text=t;
  exp=ex;
  type=typ;
  label=lab;

  if (typ & (ELSE_NO | ELSIF_NO))
  {
    stamp--;
  }

  if (typ & (START_NO | END_NO | VARIABLE_NO))
  {
    stamp=0;
  }

  id=nodeCounter++;
}

void FlowChart::addFlowChart(int type,const QCString &text,const QCString &exp, const QCString &label)
{
  if (!VhdlDocGen::getFlowMember()) return;

  QCString typeString(text);
  QCString expression(exp);


  if (!text.isEmpty())
  {
    typeString=substitute(typeString,";","\n");
  }

  if (!exp.isEmpty())
  {
    expression=substitute(expression,"\"","\\\"");
  }

  if (type & VARIABLE_NO)
  {
  // Ignore the empty section of the VHDL variable definition.
  // This is section between `process` and `begin` keywords, where any source text is missing, probably a bug in the VHDL source parser.
    if(text.isEmpty()) return;

    flowList.insert(flowList.begin(),FlowChart(type,typeString,expression,label));
    flowList.front().line=1; // TODO: use getLine(); of the parser
  }
  else if (type & START_NO)
  {
    flowList.insert(flowList.begin(),FlowChart(type,typeString,expression,label));
    flowList.front().line=1; // TODO: use getLine(); of the parser
  }
  else
  {
    flowList.emplace_back(type,typeString,expression,label);
    flowList.back().line=1;  // TODO: use getLine(); of the parser
  }
}

void FlowChart::moveToPrevLevel()
{
  if (!VhdlDocGen::getFlowMember()) return;
  ifcounter--;
}

QCString FlowChart::printPlantUmlNode(const FlowChart &flo,bool ca,bool endL)
{
  QCString t;
  QCString exp=flo.exp.stripWhiteSpace();
  QCString text=flo.text.stripWhiteSpace();
  switch (flo.type)
  {
    case START_NO:   t=":"+text+"|"; break;
    case IF_NO :     t="\nif ("+exp+") then (yes)"; break;
    case ELSIF_NO:   t="\nelseif ("+exp+") then (yes)"; break;
    case ELSE_NO:    t="\nelse"; break;
    case CASE_NO:    t="\n:"+exp+";"; break;
    case WHEN_NO:    t="\n";
                     if (!ca) t+="else";
                     t+="if ("+exp+") then (yes)";
                     break;
    case EXIT_NO:    break;
    case END_NO:     if (text.contains(" function")==0) t="\n:"+text+";";
                     break;
    case TEXT_NO:    t="\n:"+text+"]"; break;
    case ENDIF_NO:   t="\nendif"; break;
    case FOR_NO:     t="\nwhile ("+exp+") is (yes)"; break;
    case WHILE_NO:   t="\nwhile ("+exp+") is (yes)"; break;
    case END_LOOP:   t="\nendwhile"; break;
    case END_CASE:   t="\nendif\n:end case;"; break;
    case VARIABLE_NO:t="\n:"+text+";"; break;
    case RETURN_NO:  t="\n:"+text+";";
                     if (!endL) t+="\nstop";
                     break;
    case LOOP_NO:    t="\nwhile (infinite loop)"; break;
    case NEXT_NO:    break;
    case EMPTY_NO:   break;
    case COMMENT_NO: t="\n note left \n "+flo.label+"\nend note \n"; break;
    case BEGIN_NO:   t="\n:begin;"; break;
    default:         assert(false); break;
  }
  return t;
}

void  FlowChart::printUmlTree()
{
  int caseCounter = 0;
  int whenCounter = 0;

  QCString qcs;
  size_t size=flowList.size();
  for (size_t j=0;j<size;j++)
  {
    bool endList = j==FLOWLEN;
    const FlowChart &flo = flowList[j];
    if (flo.type==CASE_NO)
    {
      caseCounter++;
      whenCounter=0;
    }

    if (flo.type==END_CASE)
    {
      caseCounter--;
    }

    bool ca = (caseCounter>0 && whenCounter==0);

    qcs+=printPlantUmlNode(flo,ca,endList);

    if (flo.type==WHEN_NO)
    {
      whenCounter++;
    }

  }
  qcs+="\n";

  QCString htmlOutDir = Config_getString(HTML_OUTPUT);

  QCString n=convertNameToFileName();
  auto baseNameVector=PlantumlManager::instance().writePlantUMLSource(htmlOutDir,n,qcs,PlantumlManager::PUML_SVG,"uml",n,1,true);
  for (const auto &baseName: baseNameVector)
  {
    PlantumlManager::instance().generatePlantUMLOutput(baseName,htmlOutDir,PlantumlManager::PUML_SVG);
  }
}

QCString FlowChart::convertNameToFileName()
{
  return VhdlDocGen::getFlowMember()->name();
}

const char* FlowChart::getNodeType(int c)
{
  switch(c)
  {
    case IF_NO:        return "if ";
    case ELSIF_NO:     return "elsif ";
    case ELSE_NO:      return "else ";
    case CASE_NO:      return "case ";
    case WHEN_NO:      return "when ";
    case EXIT_NO:      return "exit ";
    case END_NO:       return "end ";
    case TEXT_NO:      return "text ";
    case START_NO:     return "start  ";
    case ENDIF_NO:     return "endif  ";
    case FOR_NO:       return "for ";
    case WHILE_NO:     return "while  ";
    case END_LOOP:     return "end_loop  ";
    case END_CASE:     return "end_case  ";
    case VARIABLE_NO:  return "variable_decl  ";
    case RETURN_NO:    return "return  ";
    case LOOP_NO:      return "infinite loop  ";
    case NEXT_NO:      return "next  ";
    case COMMENT_NO:   return "comment  ";
    case EMPTY_NO:     return "empty  ";
    case BEGIN_NO:     return "<begin>  ";
    default: return "--failure--";
  }
}

void FlowChart::createSVG()
{
  QCString qcs("/");
  QCString ov = Config_getString(HTML_OUTPUT);

  qcs+=FlowChart::convertNameToFileName()+".svg";

  //const  MemberDef *m=VhdlDocGen::getFlowMember();
  //if (m)
  //  fprintf(stderr,"\n creating flowchart  : %s  %s in file %s \n",theTranslator->trVhdlType(m->getMemberSpecifiers(),TRUE),qPrint(m->name()),qPrint(m->getFileDef()->name()));

  QCString dir=" -o \""+ov+qcs+"\"";
  ov+="/flow_design.dot";

  QCString vlargs="-Tsvg \""+ov+"\" "+dir ;

  if (Portable::system(Doxygen::verifiedDotPath,vlargs)!=0)
  {
    err("could not create dot file\n");
  }
}

void FlowChart::startDot(TextStream &t)
{
  t << " digraph G { \n";
  t << "rankdir=TB \n";
  t << "concentrate=true\n";
  t << "stylesheet=\"doxygen.css\"\n";
}

void FlowChart::endDot(TextStream &t)
{
  t << " } \n";
}

void FlowChart::writeFlowChart()
{
  //  assert(VhdlDocGen::flowMember);

  QCString ov = Config_getString(HTML_OUTPUT);
  QCString fileName = ov+"/flow_design.dot";
  std::ofstream f = Portable::openOutputStream(fileName);
  if (!f.is_open())
  {
    err("Cannot open file {} for writing\n",fileName);
    return;
  }
  TextStream t(&f);

  colTextNodes();
  //  buildCommentNodes(t);

#ifdef DEBUGFLOW
   printFlowTree();
#endif

  if (!Config_getString(PLANTUML_JAR_PATH).isEmpty())
  {
    printUmlTree();
    delFlowList();
    t.flush();
    f.close();
    return;
  }

  startDot(t);
  buildCommentNodes(t);
  for (const auto &fll : flowList)
  {
    writeShape(t,fll);
  }
  writeFlowLinks(t);

  FlowChart::endDot(t);
  delFlowList();
  t.flush();
  f.close();
  FlowChart::createSVG();
}// writeFlowChart

void FlowChart::writeShape(TextStream &t,const FlowChart &fl)
{
  if (fl.type & EEND) return;
  QCString var;
  if (fl.type & LOOP)
  {
    var=" loop";
  }
  else if (fl.type & IFF)
  {
    var=" then";
  }
  else
  {
    var="";
  }

  t << getNodeName(fl.id);

#ifdef DEBUGFLOW
  QCString qq(getNodeName(fl.id));
  g_keyMap.emplace(qq.str(),fl.id);
#endif

  bool dec=(fl.type & DECLN);
  bool exit=(fl.type & EXITNEXT);
  if (exit && !fl.exp.isEmpty())
  {
    dec=TRUE;
  }
  if (dec)
  {
    QCString exp=fl.exp;
    alignText(exp);

    t << " [shape=diamond,style=filled,color=\"";
    t << flowCol.decisionNode;
    t << "\",label=\" ";
    QCString kl;
    if (exit) kl=fl.text+"  ";

    if (!fl.label.isEmpty())
    {
      kl+=fl.label+":"+exp+var;
    }
    else
    {
      kl+=exp+var;
    }

    FlowChart::alignCommentNode(t,kl);
    t << "\"]\n";
  }
  else if (fl.type & ENDCL)
  {
    QCString val=fl.text;
    t << " [shape=ellipse ,label=\""+val+"\"]\n";
  }
  else if (fl.type & STARTFIN)
  {
    QCString val=fl.text;
    t << "[shape=box , style=rounded label=<\n";
    t << "<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" >\n ";
    t << "<TR><TD BGCOLOR=\"";
    t<< flowCol.startEndNode;
    t<< "\"> ";
    FlowChart::alignCommentNode(t,val);
    t << " </TD></TR></TABLE>>];";
  }
  else
  {
    if (fl.text.isEmpty()) return;
    bool isVar=(fl.type & FlowChart::VARIABLE_NO);
    QCString q=fl.text;

    if (exit)
    {
      q+=" "+fl.label;
    }

    int z=q.findRev("\n");

    if (z==static_cast<int>(q.length())-1)
    {
      q=q.remove(z,2);
    }
    t << "[shape=none margin=0.1, label=<\n";
    t << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"2\" >\n ";
    if (isVar)
    {
      t << "<TR><TD BGCOLOR=\"" << flowCol.varNode << "\" > ";
    }
    else
    {
      t << "<TR><TD BGCOLOR=\"" << flowCol.textNode << "\" > ";
    }
    FlowChart::alignCommentNode(t,q);
    t << " </TD></TR></TABLE>>];";
  }
}


void FlowChart::writeEdge(TextStream &t,const FlowChart &fl_from,const FlowChart &fl_to,int i)
{
  bool b=fl_from.type & STARTL;
  bool c=fl_to.type & STARTL;

#ifdef DEBUGFLOW
  QCString s1(getNodeName(fl_from.id));
  QCString s2(getNodeName(fl_to.id));
  auto it = g_keyMap.find(s1.str());
  auto it1 = g_keyMap.find(s2.str());
  // checks if the link is connected to a valid node
  assert(it!=g_keyMap.end());
  assert(it1!=g_keyMap.end());
#endif

  writeEdge(t,fl_from.id,fl_to.id,i,b,c);
}

void FlowChart::writeEdge(TextStream &t,int fl_from,int fl_to,int i,bool bFrom,bool bTo)
{
  QCString label,col;

  if (i==0)
  {
    col=flowCol.yesNodeLink;
    label="yes";
  }
  else if (i==1)
  {
    col=flowCol.noNodeLink;
    label="no";
  }
  else
  {
    col=flowCol.textNodeLink;
    label="";
  }

  t << "edge [color=\""+col+"\",label=\""+label+"\"]\n";
  t << getNodeName(fl_from);
  if (bFrom) t << ":s";
  t << "->";
  t << getNodeName(fl_to);
  if (bTo) t << ":n";
  t << "\n";
}

void FlowChart::alignFuncProc( QCString & q,const ArgumentList &al,bool isFunc)
{
  size_t index=al.size();
  if (index==0) return;

  size_t len=q.length()+VhdlDocGen::getFlowMember()->name().length();
  QCString prev,temp;
  prev.fill(' ',static_cast<int>(len)+1);

  q+="\n";
  for (const Argument &arg : al)
  {
    QCString attl=arg.defval+" ";
    attl+=arg.name+" ";

    if (!isFunc)
    {
      attl+=arg.attrib+" ";
    }
    else
    {
      attl+=" in ";
    }
    attl+=arg.type;
    if (--index) attl+=",\n"; else attl+="\n";

    attl.prepend(prev);
    temp+=attl;
  }

  q+=temp;
}

size_t FlowChart::findNextLoop(size_t index,int stamp)
{
  for (size_t j=index+1; j<flowList.size(); j++)
  {
    const FlowChart &flo = flowList[j];
    if (flo.stamp==stamp)
    {
      continue;
    }
    if (flo.type&END_LOOP)
    {
      return j;
    }
  }
  return flowList.size()-1;
}

size_t FlowChart::findPrevLoop(size_t index,int stamp,bool endif)
{
  for (size_t j=index;j>0;j--)
  {
    const FlowChart &flo = flowList[j];
    if (flo.type & LOOP)
    {
      if (flo.stamp==stamp && endif)
      {
        return j;
      }
      else
      {
        if (flo.stamp<stamp)
        {
          return j;
        }
      }
    }
  }
  return flowList.size()-1;
}

size_t FlowChart::findLabel(size_t index,const QCString &label)
{
  for (size_t j=index;j>0;j--)
  {
    const FlowChart &flo = flowList[j];
    if ((flo.type & LOOP) && !flo.label.isEmpty() && qstricmp(flo.label,label)==0)
    {
      return j;
    }
  }
  err("could not find label: '{}'\n",label);
  return 0;
}

size_t FlowChart::findNode(size_t index,int stamp,int type)
{
  for (size_t j=index+1; j<flowList.size(); j++)
  {
    const FlowChart &flo = flowList[j];
    if (flo.type==type && flo.stamp==stamp)
    {
      return j;
    }
  }
  return 0;
}// findNode

size_t FlowChart::getNextNode(size_t index,int stamp)
{
  for (size_t j=index+1; j<flowList.size(); j++)
  {
    const FlowChart &flo = flowList[j];
    int kind = flo.type;
    int s    = flo.stamp;
    if (s>stamp)
    {
      continue;
    }
    if (kind & ENDIF_NO)
    {
      if (s<stamp && stamp>0)
      {
        stamp--;
        continue;
      }
    }
    if (kind & (ELSE_NO | ELSIF_NO))
    {
      if (s<stamp && stamp>0)
      {
        stamp--;
      }
      j=findNode(j,stamp,ENDIF_NO);
      continue;
    }
    if (kind & WHEN_NO)
    {
      if (s<stamp && stamp>0)
      {
        stamp--;
      }
      return findNode(j,stamp-1,END_CASE);
    }
    return j;
  }
  return FLOWLEN;
}

size_t FlowChart::getNextIfLink(const FlowChart &fl,size_t index)
{
  int stamp=fl.stamp;
  size_t start = index+1;
  size_t endifNode  = findNode(start,stamp,ENDIF_NO);
  size_t elseifNode = findNode(start,stamp,ELSIF_NO);
  size_t elseNode   = findNode(start,stamp,ELSE_NO);

  if (elseifNode>0 && elseifNode<endifNode)
  {
    return elseifNode;
  }

  if (elseNode>0 && elseNode<endifNode)
  {
    return elseNode+1;
  }

  stamp=flowList[endifNode].stamp;
  return getNextNode(endifNode,stamp);
}

void FlowChart::writeFlowLinks(TextStream &t)
{
  size_t size=flowList.size();
  if (size<2) return;

  // write start link
  writeEdge(t,flowList[0],flowList[1],2);

  for (size_t j=0;j<size;j++)
  {
    const FlowChart &fll = flowList[j];
    int kind  = fll.type;
    int stamp = fll.stamp;
    if (kind & EEND)
    {
      continue;
    }

    if (kind & IFF)
    {
      writeEdge(t,fll,flowList[j+1],0);
      size_t z=getNextIfLink(fll,j);
      // assert(z>-1);
      writeEdge(t,fll,flowList[z],1);
    }
    else if (kind & LOOP_NO)
    {
      writeEdge(t,fll,flowList[j+1],2);
      continue;
    }
    else if (kind & (CASE_NO | FOR_NO | WHILE_NO))
    {
      if (kind & CASE_NO)
      {
        writeEdge(t,fll,flowList[j+1],2);
        continue;
      }
      else
      {
        writeEdge(t,fll,flowList[j+1],0);
      }

      kind=END_LOOP;
      size_t z=findNode(j+1,fll.stamp,kind);
      z=getNextNode(z,flowList[z].stamp);

      // assert(z>-1);
      writeEdge(t,fll,flowList[z],1);
      continue;
    }
    else if (kind & (TEXT_NO | VARIABLE_NO))
    {
      size_t z=getNextNode(j,stamp);
      writeEdge(t,fll,flowList[z],2);
    }
    else if (kind & WHEN_NO)
    {
      // default value
      if (qstricmp(fll.text.simplifyWhiteSpace(),"others")==0)
      {
        writeEdge(t,fll,flowList[j+1],2);
        continue;
      }


      writeEdge(t,fll,flowList[j+1],0);
      size_t u=findNode(j,stamp,WHEN_NO);
      size_t v=findNode(j,stamp-1,END_CASE);

      if (u>0 && u<v)
      {
        writeEdge(t,fll,flowList[u],1);
      }
      else
      {
        writeEdge(t,fll,flowList[v],1);
      }
    }
    else if (kind & END_CASE)
    {
      size_t z=FlowChart::getNextNode(j,fll.stamp);
      writeEdge(t,fll,flowList[z],2);
    }
    else if (kind & END_LOOP)
    {
      size_t z=findPrevLoop(j,fll.stamp,true);
      writeEdge(t,fll,flowList[z],2);
    }
    else if (kind & RETURN_NO)
    {
      writeEdge(t,fll,flowList[size-1],2);
    }
    else if (kind & (EXIT_NO | NEXT_NO))
    {
      size_t z = 0;
      bool b = kind==NEXT_NO;
      if (!fll.exp.isEmpty())
      {
        writeEdge(t,fll,flowList[j+1],1);
      }
      if (!fll.label.isEmpty())
      {
        z=findLabel(j,fll.label);
        if (b)
        {
          writeEdge(t,fll,flowList[z],0);
        }
        else
        {
          z=findNode(z,flowList[z].stamp,END_LOOP);
          z=getNextNode(z,flowList[z].stamp);
          writeEdge(t,fll,flowList[z],0);
        }
        continue;
      }
      else
      {
        if (b)
        {
          z=findPrevLoop(j,fll.stamp);
          writeEdge(t,fll,flowList[z],0);
          continue;
        }
        else
        {
          z =findNextLoop(j,fll.stamp-1);
        }
        z=getNextNode(z,flowList[z].stamp);
      }
      writeEdge(t,fll,flowList[z],0);
    }
  } //for
} //writeFlowLinks
