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

#include <stdio.h>
#include <qregexp.h>
#include "memberdef.h"
#include "membername.h"
#include "doxygen.h"
#include "util.h"
#include "code.h"
#include "message.h"
#include "htmlhelp.h"
#include "language.h"
#include "outputlist.h"
#include "example.h"
#include "membergroup.h"
#include "doc.h"
#include "groupdef.h"
#include "defargs.h"
#include "xml.h"

//-----------------------------------------------------------------------------

static QCString addTemplateNames(const QCString &s,const QCString &n,const QCString &t)
{
  QCString result;
  QCString clRealName=n;
  int p=0,i;
  if ((i=clRealName.find('<'))!=-1)
  {
    clRealName=clRealName.left(i); // strip template specialization
  }
  if ((i=clRealName.findRev("::"))!=-1)
  {
    clRealName=clRealName.right(clRealName.length()-i-2);
  }
  while ((i=s.find(clRealName,p))!=-1)
  {
    result+=s.mid(p,i-p);
    uint j=clRealName.length()+i;
    if (s.length()==j || (s.at(j)!='<' && !isId(s.at(j))))
    { // add template names
      //printf("Adding %s+%s\n",clRealName.data(),t.data());
      result+=clRealName+t; 
    }
    else 
    { // template names already present
      //printf("Adding %s\n",clRealName.data());
      result+=clRealName;
    }
    p=i+clRealName.length();
  }
  result+=s.right(s.length()-p);
  //printf("addTemplateNames(%s,%s,%s)=%s\n",s.data(),n.data(),t.data(),result.data());
  return result;
}

static void writeDefArgumentList(OutputList &ol,ClassDef *cd,
                                 const QCString &scopeName,MemberDef *md)
{
  ArgumentList *argList=md->argumentList();
  if (argList==0) return; // member has no function like argument list
  if (!md->isDefine()) ol.docify(" ");
  ol.docify("("); // start argument list
  ol.endMemberDocName();
  Argument *a=argList->first();
  QCString cName;
  if (md->scopeDefTemplateArguments())
  {
    cName=tempArgListToString(md->scopeDefTemplateArguments());
  }
  if (cd)
  {
    cName=cd->name();
    int il=cName.find('<');
    int ir=cName.findRev('>');
    if (il!=-1 && ir!=-1 && ir>il)
    {
      cName=cName.mid(il,ir-il+1);
      //printf("1. cName=%s\n",cName.data());
    }
    else if (cd->templateArguments())
    {
      cName=tempArgListToString(cd->templateArguments()); 
      //printf("2. cName=%s\n",cName.data());
    }
    else // no template specifier
    {
      cName.resize(0);
    }
  }
  //printf("~~~ %s cName=%s\n",md->name().data(),cName.data());

  //if (!md->isDefine()) ol.startParameter(TRUE); else ol.docify(" ");
  ol.startParameter(TRUE); 
  bool first=TRUE;
  while (a)
  {
    QRegExp re(")(");
    int vp;
    if (!a->attrib.isEmpty()) // argument has an IDL attribute
    {
      ol.docify(a->attrib+" ");
    }
    if ((vp=a->type.find(re))!=-1) // argument type is a function pointer
    {
      QCString n=a->type.left(vp);
      if (!cName.isEmpty()) n=addTemplateNames(n,cd->name(),cName);
      linkifyText(TextGeneratorOLImpl(ol),scopeName,md->name(),n);
    }
    else // non-function pointer type
    {
      QCString n=a->type;
      if (!cName.isEmpty()) n=addTemplateNames(n,cd->name(),cName);
      linkifyText(TextGeneratorOLImpl(ol),scopeName,md->name(),n);
    }
    if (!a->name.isEmpty()) // argument has a name
    { 
      ol.docify(" ");
      ol.disable(OutputGenerator::Man);
      ol.startEmphasis();
      ol.enable(OutputGenerator::Man);
      ol.docify(a->name);
      ol.disable(OutputGenerator::Man);
      ol.endEmphasis();
      ol.enable(OutputGenerator::Man);
    }
    if (!a->array.isEmpty())
    {
      ol.docify(a->array);
    }
    if (vp!=-1) // write the part of the argument type 
                // that comes after the name
    {
      linkifyText(TextGeneratorOLImpl(ol),scopeName,md->name(),a->type.right(a->type.length()-vp));
    }
    if (!a->defval.isEmpty()) // write the default value
    {
      QCString n=a->defval;
      if (!cName.isEmpty()) n=addTemplateNames(n,cd->name(),cName);
      ol.docify(" = ");
      linkifyText(TextGeneratorOLImpl(ol),scopeName,md->name(),n); 
    }
    a=argList->next();
    if (a) 
    {
      ol.docify(", "); // there are more arguments
      if (!md->isDefine()) 
      {
        ol.endParameter(first);
        ol.startParameter(FALSE);
      }
    }
    first=FALSE;
  }
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  if (!first) ol.writeString("&nbsp;");
  ol.popGeneratorState();
  ol.docify(")"); // end argument list
  if (argList->constSpecifier)
  {
    ol.docify(" const");
  }
  if (argList->volatileSpecifier)
  {
    ol.docify(" volatile");
  }
}

static void writeTemplatePrefix(OutputList &ol,ArgumentList *al,bool br=TRUE)
{
  ol.docify("template<");
  Argument *a=al->first();
  while (a)
  {
    ol.docify(a->type);
    ol.docify(" ");
    ol.docify(a->name);
    if (a->defval.length()!=0)
    {
      ol.docify(" = ");
      ol.docify(a->defval);
    } 
    a=al->next();
    if (a) ol.docify(", ");
  }
  ol.docify("> ");
  if (br)
  {
  //  ol.pushGeneratorState();
  //  ol.disable(OutputGenerator::Man);
  //  ol.disable(OutputGenerator::Latex);
  //  ol.lineBreak();
  //  ol.popGeneratorState();
  }
}

//-----------------------------------------------------------------------------

/*! Creates a new member definition.
 *  Members can be function/variables/enums/etc. inside a class or inside a 
 *  file.
 *
 * \param df File containing the definition of this member.
 * \param dl Line at which the member definition was found.
 * \param t  A string representing the type of the member.
 * \param n  A string representing the name of the member.
 * \param a  A string representing the arguments of the member.
 * \param p  The type of protection of the member, possible values are:
 *           \c Public, \c Protected, \c Private.
 * \param v  The `virtualness' of the member, possible values are:
 *           \c Normal, \c Virtual, \c Pure.
 * \param s  A boolean that is true if the member is static.
 * \param r  A boolean that is true if the member is only related.
 * \param mt The kind of member. See #MemberDef::MemberType for a list of 
 *           all types.
 */

MemberDef::MemberDef(const char *df,int dl,
                     const char *t,const char *na,const char *a,const char *e,
                     Protection p,Specifier v,bool s,bool r,MemberType mt,
                     const ArgumentList *tal,const ArgumentList *al
                    ) : Definition(df,dl,na)
{
  //printf("++++++ MemberDef(%s file=%s,line=%d) ++++++ \n",na,df,dl);
  classDef=0;
  fileDef=0;
  //fileDec=0;
  redefines=0;
  redefinedBy=0;
  nspace=0;
  memDef=0;
  memDec=0;
  group=0;
  grpId=-1;
  exampleList=0;
  exampleDict=0;
  enumFields=0;
  enumScope=0;
  enumDeclList=0;
  scopeTAL=0;
  membTAL=0;
  initLines=0;
  type=t;
  args=a;
  if (type.isEmpty()) decl=name()+args; else decl=type+" "+name()+args;
  declLine=0;
  memberGroup=0;
  virt=v;
  prot=p;
  related=r;
  stat=s;
  mtype=mt;
  exception=e;
  eUsed=FALSE;
  proto=FALSE;
  annScope=FALSE;
  memSpec=FALSE;
  annMemb=0;
  annUsed=FALSE;
  annShown=FALSE;
  annEnumType=0;
  indDepth=0;
  section=0;
  maxInitLines=defMaxInitLines;
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

#if 0
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
#endif

QCString MemberDef::getOutputFileBase() const
{
  if (classDef)
  {
    return classDef->getOutputFileBase();
  }
  else if (fileDef)
  {
    return fileDef->getOutputFileBase();
  }
  //else if (fileDec)
  //{
  //  return fileDec->getOutputFileBase();
  //}
  else if (nspace)
  {
    return nspace->getOutputFileBase();
  }
  warn(defFileName,defLine,
       "Warning: Internal inconsistency: member %s does not belong to any"
       " container!",name().data()
      );
  return "dummy";
}

static void copyArgumentList(const ArgumentList *src,ArgumentList *dst)
{
  ArgumentListIterator tali(*src);
  Argument *a;
  for (;(a=tali.current());++tali)
  {
    dst->append(new Argument(*a));
  }
  dst->constSpecifier    = src->constSpecifier;
  dst->volatileSpecifier = src->volatileSpecifier;
  dst->pureSpecifier     = src->pureSpecifier;
}

void MemberDef::setScopeDefTemplateArguments(ArgumentList *tal)
{
  // copy function arguments (if any)
  if (tal)
  {
    scopeTAL = new ArgumentList;
    scopeTAL->setAutoDelete(TRUE);
    copyArgumentList(tal,scopeTAL);
  }
}

void MemberDef::setMemberDefTemplateArguments(ArgumentList *tal)
{
  // copy function arguments (if any)
  if (tal)
  {
    membTAL = new ArgumentList;
    membTAL->setAutoDelete(TRUE);
    copyArgumentList(tal,membTAL);
  }
}

void MemberDef::writeLink(OutputList &ol,ClassDef *cd,NamespaceDef *nd,
                      FileDef *fd,GroupDef *gd)
{
  Definition *d=0;
  if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else if (gd) d=gd;
  if (d==0) { err("Member %s without group! Please report this bug!\n",name().data()); return; }
  if (group!=0 && gd==0) // forward link to the group
  {
    ol.writeObjectLink(group->getReference(),group->getOutputFileBase(),anchor(),name());
  }
  else // local link
  {
    ol.writeObjectLink(d->getReference(),d->getOutputFileBase(),anchor(),name());
  }
}


void MemberDef::writeDeclaration(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               bool inGroup
               )
{
  int i,l;
  bool hasDocs=hasDocumentation();
  //printf("%s MemberDef::writeDeclaration(): hasDocs %d\n",name().data(),hasDocs);
  //if (cd)
  //{
  //  printf("MemberDef: %s in class %s annScope=%d annMemb=%p\n",
  //    name().data(),cd->name().data(),annScope,annMemb);
  //}

  // hide members in anonymous scopes 
  // (they are displayed by there parent placeholder)
  if (annScope) return;
  // hide undocumented members unless overwritten by the configuration
  if (!hasDocs && Config::hideMemberFlag) return;
  // hide members with no detailed desciption and brief descriptions explicitly
  // disabled.
  if (Config::hideMemberFlag && documentation().isEmpty() && 
      !Config::briefMemDescFlag && !Config::repeatBriefFlag 
     ) return;
  // hide static file & namespace members unless extract static is on
  if (cd==0 && isStatic() && !Config::extractStaticFlag) return;

  // hide private member that are put into a member group. Non-grouped
  // members are not rendered anyway.
  //printf("md->name()=`%s' Protection=%d\n",name().data(),protection());
  if (inGroup && protection()==Private && !Config::extractPrivateFlag) return;

  QCString ltype=type.copy();
  // strip `static' keyword from ltype
  if (ltype.left(7)=="static ") ltype=ltype.right(ltype.length()-7);
  // strip `friend' keyword from ltype
  if (ltype.left(7)=="friend ") ltype=ltype.right(ltype.length()-7);
  static QRegExp r("@[0-9]+");
  i=-1;
  if ((ltype.isEmpty() || (i=r.match(ltype,0,&l))==-1) || !enumUsed())
  {
    
    if (!Config::genTagFile.isEmpty())
    {
      tagFile << name() << " " << anchor() << " \""
              << argsString() << "\"\n";
    }
      
    Definition *d=0;
    ASSERT (cd!=0 || nd!=0 || fd!=0 || gd!=0); // member should belong to something
    if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else d=gd;
    QCString cname  = d->name();
    QCString cfname = d->getOutputFileBase();

    //int gId = inGroup ? -1 : groupId();
    //MemberGroup *mg = (gId!=prevGroupId && gId!=-1) ? memberGroupDict[gId] : 0;
    //const char *gHeader = 0;
    //const char *gFile = 0;
    //if (mg)
    //{
    //  gHeader=mg->header();
    //  gFile=mg->getOutputFileBase();
    //}
    //
    //if (!inGroup)
    //{
    //  if (prevGroupId==-1 && gId!=-1)
    //  {
    //    ol.memberGroupSpacing(FALSE);
    //    ol.memberGroupSeparator();
    //  }
    //  else if (prevGroupId!=-1 && gId==-1)
    //  {
    //    ol.memberGroupSpacing(TRUE);
    //    ol.memberGroupSeparator();
    //  }
    //  else if (prevGroupId!=-1 && gId!=-1 && prevGroupId!=gId)
    //  {
    //    ol.memberGroupSpacing(TRUE);
    //    ol.memberGroupSeparator();
    //  }
    //}
    
    HtmlHelp *htmlHelp=0;
    bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
    if (hasHtmlHelp) htmlHelp = HtmlHelp::getInstance();
    
    // search for the last anonymous scope in the member type
    ClassDef *annoClassDef=0;
    //while (i!=-1 && cname.find(ltype.mid(i,l))!=-1)
    //{
    //  i=r.match(ltype,i+l,&l);
    //}
    int il=i-1,ir=i+l;
    if (i!=-1) // found anonymous scope in type
    {
      // extract anonymous scope
      while (il>=0 && (isId(ltype.at(il)) || ltype.at(il)==':' || ltype.at(il)=='@')) il--;
      if (il>0) il++;
      while (ir<(int)ltype.length() && (isId(ltype.at(ir)) || ltype.at(ir)==':' || ltype.at(ir)=='@')) ir++;

      //QCString annName = ltype.mid(i,l);
      QCString annName = ltype.mid(il,ir-il);
      
      // if inside a class or namespace try to prepend the scope name
      if ((cd || nd) && annName.left(cname.length())!=cname) 
      {
        QCString ts=stripAnnonymousNamespaceScope(cname+"::"+annName);
        //printf("Member::writeDeclaration: Trying %s\n",ts.data());
        annoClassDef=getClass(ts);
      }
      // if not found yet, try without scope name
      if (annoClassDef==0)
      {
        QCString ts=stripAnnonymousNamespaceScope(annName);
        //printf("Member::writeDeclaration: Trying %s\n",ts.data());
        annoClassDef=getClass(ts);
      }
    }

    // start a new member declaration
    ol.startMemberItem((annoClassDef || annMemb || annEnumType) ? 1 : 0);
    
    // If there is no detailed description we need to write the anchor here.
    bool detailsVisible = detailsAreVisible();
    if (!detailsVisible && !Config::extractAllFlag && !annMemb)
    {
      QCString doxyName=name().copy();
      if (!cname.isEmpty()) doxyName.prepend(cname+"::");
      ol.startDoxyAnchor(cfname,anchor(),doxyName);
      ol.addToIndex(name(),cname);
      ol.addToIndex(cname,name());
      if (hasHtmlHelp)
      {
        htmlHelp->addIndexItem(cname,name(),cfname,anchor());
      }
      ol.pushGeneratorState();
      ol.disable(OutputGenerator::Man);
      ol.docify("\n");
      ol.popGeneratorState();
    }

    //printf("member name=%s indDepth=%d\n",name().data(),indDepth);
    if (annoClassDef || annMemb)
    {
      int j;
      for (j=0;j<indDepth;j++) 
      {
        ol.writeNonBreakableSpace();
      }
    }
    
    if (tArgList)
    {
      writeTemplatePrefix(ol,tArgList,FALSE);
    }
    
    if (i!=-1) // member has an anonymous type
    {
      //printf("annoClassDef=%p annMemb=%p scopeName=`%s' anonymous=`%s'\n",
      //    annoClassDef,annMemb,cname.data(),ltype.mid(i,l).data());

      if (annoClassDef) // type is an anonymous compound
      {
        //printf("class found!\n");
        annoClassDef->writeDeclaration(ol,annMemb,inGroup);
        ol.startMemberItem(2);
        int j;
        for (j=0;j<indDepth;j++) 
        {
          ol.writeNonBreakableSpace();
        }
        QCString varName=ltype.right(ltype.length()-ir).stripWhiteSpace();
        ol.docify("}");
        if (varName.isEmpty() && (name().isEmpty() || name().at(0)=='@')) 
        {
          ol.docify(";"); 
        }
        else 
        {
          ol.docify(varName);
        }
      }
      else
      {
        if (getAnonymousEnumType()) // type is an anonymous enum
        {
          linkifyText(TextGeneratorOLImpl(ol),cname,name(),ltype.left(i),TRUE); 
          ol+=*getAnonymousEnumType()->enumDecl();
          linkifyText(TextGeneratorOLImpl(ol),cname,name(),ltype.right(ltype.length()-i-l),TRUE); 
        }
        else
        {
          ltype = ltype.left(i) + " { ... } " + ltype.right(ltype.length()-i-l);
          linkifyText(TextGeneratorOLImpl(ol),cname,name(),ltype,TRUE); 
        }
      }
    }
    else
    {
      linkifyText(TextGeneratorOLImpl(ol),cname,name(),ltype,TRUE); 
    }
    bool htmlOn = ol.isEnabled(OutputGenerator::Html);
    if (htmlOn && Config::htmlAlignMemberFlag && !ltype.isEmpty())
    {
      ol.disable(OutputGenerator::Html);
    }
    if (!ltype.isEmpty()) ol.docify(" ");
    if (htmlOn) 
    {
      ol.enable(OutputGenerator::Html);
    }

    if (annMemb) 
    {
      //bool latexOn = ol.isEnabled(OutputGenerator::Latex);
      //bool manOn   = ol.isEnabled(OutputGenerator::Man);
      //if (latexOn) ol.disable(OutputGenerator::Latex);
      //if (manOn)   ol.disable(OutputGenerator::Man);
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.writeNonBreakableSpace();
      ol.popGeneratorState();
      //if (latexOn) ol.enable(OutputGenerator::Latex);
      //if (manOn)   ol.enable(OutputGenerator::Man);
    }
    else
    {
      ol.insertMemberAlign();
    }
    
    // write name
    if (!name().isEmpty() && name().at(0)!='@')
    {
      if (isLinkable())
      {
        if (annMemb)
        {
          //printf("anchor=%s ann_anchor=%s\n",anchor(),annMemb->anchor());
          annMemb->writeLink(ol,
              annMemb->getClassDef(),
              annMemb->getNamespaceDef(),
              annMemb->getFileDef(),
              annMemb->getGroupDef()
                            );
          annMemb->annUsed=annUsed=TRUE;
        }
        else
          //printf("writeLink %s->%d\n",name.data(),hasDocumentation());
          writeLink(ol,cd,nd,fd,gd);
      }
      else // there is a brief member description and brief member 
        // descriptions are enabled or there is no detailed description.
      {
        if (annMemb) annMemb->annUsed=annUsed=TRUE;
        ol.writeBoldString(name());
      }
    }

    // if member template specifiers are not part of the name, but they are
    // present, we add them
    if (tArgList && !(name().find('<')!=-1 && name().find('>')!=-1)
        && cd && cd->templateArguments())
    {
      ol.docify(tempArgListToString(tArgList));
    }

    if (argsString()) 
    {
      if (!isDefine()) ol.writeString(" ");
      //ol.docify(argsString());
      linkifyText(TextGeneratorOLImpl(ol),cname,name(),argsString()); 
    }
    
    if (excpString())
    {
      ol.writeString(" ");
      ol.docify(excpString());
    }

    if (!bitfields.isEmpty()) // add bitfields
    {
      linkifyText(TextGeneratorOLImpl(ol),cname,name(),bitfields.simplifyWhiteSpace());
    }
    else if (!init.isEmpty() && initLines==0 && maxInitLines>0) // add initializer
    {
      if (!isDefine()) 
      {
        ol.writeString(" = "); 
        linkifyText(TextGeneratorOLImpl(ol),cname,name(),init.simplifyWhiteSpace());
      }
      else 
      {
        ol.writeNonBreakableSpace();
        linkifyText(TextGeneratorOLImpl(ol),cname,name(),init);
      }
    }

    if (!detailsVisible && !Config::extractAllFlag && !annMemb)
    {
      ol.endDoxyAnchor(cfname,anchor());
    }

    ol.endMemberItem((annoClassDef!=0 && indDepth==0) || annEnumType);
    
    //ol.endMemberItem(gId!=-1,gFile,gHeader,annoClassDef || annMemb);
    // write brief description
    if (!briefDescription().isEmpty() && Config::briefMemDescFlag && !annMemb)
    {
      ol.startMemberDescription();
      parseDoc(ol,defFileName,defLine,cname,name(),briefDescription());
      if (/*!documentation().isEmpty()*/ detailsAreVisible()) 
      {
        ol.disableAllBut(OutputGenerator::Html);
        ol.endEmphasis();
        ol.docify(" ");
        ol.startTextLink(0,anchor());
        parseText(ol,theTranslator->trMore());
        ol.endTextLink();
        ol.startEmphasis();
        ol.enableAll();
      }
      //ol.newParagraph();
      ol.endMemberDescription();
    }
  }
  warnIfUndocumented();
}

/*! Writes the "detailed documentation" section of this member to
 *  all active output formats.
 */
void MemberDef::writeDocumentation(MemberList *ml,OutputList &ol,
                                   const char *scopeName,
                                   Definition *container
                                  )
{
  // hide global static functions unless extractStaticFlag is enabled
  if (getClassDef()==0 && isStatic() && !Config::extractStaticFlag) return;
  // hide member that are documented in their own group
  if (group!=0 && container->definitionType()!=TypeGroup) return;
  
  bool hasDocs = detailsAreVisible();
  //printf("MemberDef::writeDocumentation(): type=`%s' def=`%s'\n",type.data(),definition());
  if (
      Config::extractAllFlag || hasDocs 
      || /* member is part of an anonymous scope that is the type of
          * another member in the list.
          */ 
       (!hasDocs && !briefDescription().isEmpty() && annUsed)
     )
  {
    // get definition. 
    QCString cname  = container->name();
    QCString cfname = container->getOutputFileBase();  

    // get member name
    QCString doxyName=name().copy();
    // prepend scope if there is any. TODO: make this optional for C only docs
    if (scopeName) doxyName.prepend((QCString)scopeName+"::");

    QCString ldef = definition();
    if (isEnumerate()) 
    {
      if (name().at(0)=='@')
      {
        ldef = "anonymous enum";
      }
      else
      {
        ldef.prepend("enum ");
      }
    }
    int i=0,l;
    static QRegExp r("@[0-9]+");

    if (isEnumValue()) return;

    ol.pushGeneratorState();

    bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
    HtmlHelp *htmlHelp = 0;
    if (hasHtmlHelp) htmlHelp = HtmlHelp::getInstance();

    if ((isVariable() || isTypedef()) && (i=r.match(ldef,0,&l))!=-1)
    {
      // find enum type and insert it in the definition
      MemberListIterator vmli(*ml);
      MemberDef *vmd;
      bool found=FALSE;
      for ( ; (vmd=vmli.current()) && !found ; ++vmli)
      {
        if (vmd->isEnumerate() && ldef.mid(i,l)==vmd->name())
        {
          ol.startDoxyAnchor(cfname,anchor(),doxyName);
          ol.startMemberDoc(cname,name(),anchor(),name());
          if (hasHtmlHelp)
          {
            htmlHelp->addIndexItem(cname,name(),cfname,anchor());
          }
          linkifyText(TextGeneratorOLImpl(ol),scopeName,name(),ldef.left(i));
          ol+=*vmd->enumDecl();
          linkifyText(TextGeneratorOLImpl(ol),scopeName,name(),ldef.right(ldef.length()-i-l));

          found=TRUE;
        }
      }
      if (!found) // anonymous compound
      {
        //printf("Annonymous compound `%s'\n",cname.data());
        ol.startDoxyAnchor(cfname,anchor(),doxyName);
        ol.startMemberDoc(cname,name(),anchor(),name());
        if (hasHtmlHelp)
        {
          htmlHelp->addIndexItem(cname,name(),cfname,anchor());
        }
        // strip anonymous compound names from definition
        int si=ldef.find(' '),pi,ei=i+l;
        if (si==-1) si=0;
        while ((pi=r.match(ldef,i+l,&l))!=-1) ei=i=pi+l;
        // first si characters of ldef contain compound type name
        ol.docify(ldef.left(si));
        ol.docify(" { ... } ");
        // last ei characters of ldef contain pointer/reference specifiers
        int ni=ldef.find("::",si);
        if (ni>=ei) ei=ni+2;
        linkifyText(TextGeneratorOLImpl(ol),scopeName,name(),ldef.right(ldef.length()-ei));
      }
    }
    else
    {
      ol.startDoxyAnchor(cfname,anchor(),doxyName);
      ol.startMemberDoc(cname,name(),anchor(),name());
      if (hasHtmlHelp)
      {
        htmlHelp->addIndexItem(cname,name(),cfname,anchor());
      }

      ClassDef *cd=getClassDef();
      ArgumentList *scopeAl=scopeDefTemplateArguments();
      if (scopeAl==0 && cd) scopeAl=cd->templateArguments(); 

      ArgumentList *membAl=memberDefTemplateArguments();
      if (membAl==0) membAl=templateArguments();

      //if (cd && (!isRelated() || templateArguments()!=0) && 
      //    ((al=scopeDefTemplateArguments()) || (al=cd->templateArguments()))
      //   ) 
      if (!Config::hideScopeNames)
      {
        if (scopeAl && !related) // class template prefix
        {
          ol.startMemberDocPrefixItem();
          writeTemplatePrefix(ol,scopeAl);
          ol.endMemberDocPrefixItem();
        }
        if (scopeAl && membAl) ol.docify(" ");

        if (membAl) // function template prefix
        {
          ol.startMemberDocPrefixItem();
          writeTemplatePrefix(ol,membAl);
          ol.endMemberDocPrefixItem();
        }
        if (cd)
        {
          QCString cName=cd->name();
          //printf("cName=%s\n",cName.data());
          int il=cName.find('<');
          int ir=cName.findRev('>');
          if (il!=-1 && ir!=-1 && ir>il)
          {
            ldef=addTemplateNames(ldef,
                cName.left(il),          /* class without template spec */
                cName.mid(il,ir-il+1)    /* templ spec */
                                ); 
          }
          else if (scopeAl)
          {
            ldef=addTemplateNames(ldef,cName,tempArgListToString(scopeAl));
          }
        }
      }
      ol.startMemberDocName();
      linkifyText(TextGeneratorOLImpl(ol),scopeName,name(),ldef);
      writeDefArgumentList(ol,cd,scopeName,this);
      if (!init.isEmpty() && initLines==0 && maxInitLines>0) // add initializer
      {
        if (!isDefine()) 
        {
          ol.docify(" = "); 
          linkifyText(TextGeneratorOLImpl(ol),scopeName,name(),init.simplifyWhiteSpace());
        }
        else 
        {
          ol.writeNonBreakableSpace();
          linkifyText(TextGeneratorOLImpl(ol),scopeName,name(),init);
        }
      }
      if (excpString()) // add exception list
      {
        ol.docify(" ");
        linkifyText(TextGeneratorOLImpl(ol),scopeName,name(),excpString());
      }
    }

    Specifier lvirt=virtualness();
    MemberDef *rmd=reimplements();
    while (rmd && lvirt==Normal)
    {
      lvirt = rmd->virtualness()==Normal ? Normal : Virtual;
      rmd  = rmd->reimplements();
    }

    if (protection()!=Public || lvirt!=Normal ||
        isFriend() || isRelated() || isExplicit() ||
        isMutable() || (isInline() && Config::inlineInfoFlag) ||
        isSignal() || isSlot() ||
        isStatic()
       )
    {
      // write the member specifier list
      ol.writeLatexSpacing();
      ol.startTypewriter();
      ol.docify(" [");
      QStrList sl;
      if (isFriend()) sl.append("friend");
      else if (isRelated()) sl.append("related");
      else
      {
        if      (Config::inlineInfoFlag && isInline())              
                                          sl.append("inline");
        if      (isExplicit())            sl.append("explicit");
        if      (isMutable())             sl.append("mutable");
        if      (isStatic())              sl.append("static");
        if      (protection()==Protected) sl.append("protected");
        else if (protection()==Private)   sl.append("private");
        if      (lvirt==Virtual)          sl.append("virtual");
        else if (lvirt==Pure)             sl.append("pure virtual");
        if      (isSignal())              sl.append("signal");
        if      (isSlot())                sl.append("slot");
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
    if (!isDefine()) ol.endParameter(TRUE);
    ol.endMemberDoc();
    ol.endDoxyAnchor(cfname,anchor());
    ol.startIndent();
    
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::RTF);
    ol.newParagraph();
    ol.popGeneratorState();

    /* write multi-line initializer (if any) */
    if (initLines>0 && initLines<maxInitLines)
    {
      ol.startBold();
      parseText(ol,theTranslator->trInitialValue());
      ol.endBold();
      initParseCodeContext();
      ol.startCodeFragment();
      parseCode(ol,scopeName,init,FALSE,0);
      ol.endCodeFragment();
    }
    
    /* write brief description */
    if (!briefDescription().isEmpty() && 
        (Config::repeatBriefFlag  
         /* || (!Config::briefMemDescFlag && documentation().isEmpty())*/
        ) /* || !annMemb */
       )  
    { 
      parseDoc(ol,defFileName,defLine,scopeName,name(),briefDescription());
      ol.newParagraph();
    }

    /* write detailed description */
    if (!documentation().isEmpty())
    { 
      parseDoc(ol,defFileName,defLine,scopeName,name(),documentation()+"\n");
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::RTF);
      ol.newParagraph();
      ol.popGeneratorState();
    }

    //printf("***** argList=%p name=%s docs=%s hasDocs=%d\n",
    //     argList, 
    //     argList?argList->hasDocumentation():-1);
    if (argList && argList->hasDocumentation())
    {
      //printf("***** argumentList is documented\n");
      ol.startDescList();
      ol.startBold();
      parseText(ol,theTranslator->trParameters()+": ");
      ol.endBold();
      ol.endDescTitle();
      ol.writeDescItem();
      ol.startDescTable();
      ArgumentListIterator ali(*argList);
      Argument *a;
      for (ali.toFirst();(a=ali.current());++ali)
      {
        if (a->hasDocumentation())
        {
          ol.startDescTableTitle();
          ol.startEmphasis();
          ol.docify(a->name);
          ol.endEmphasis();
          ol.endDescTableTitle();
          ol.startDescTableData();
          parseDoc(ol,defFileName,defLine,scopeName,name(),a->docs);
          ol.endDescTableData();
        }
      }
      ol.endDescTable();
      ol.endDescList();
    }
    
    if (isEnumerate())
    {
      bool first=TRUE;
      MemberList *fmdl=enumFieldList();
      if (fmdl)
      {
        MemberDef *fmd=fmdl->first();
        while (fmd)
        {
          if (fmd->isLinkable())
          {
            if (first)
            {
              //ol.newParagraph();
              ol.startDescList();
              ol.startBold();
              parseText(ol,theTranslator->trEnumerationValues());
              ol.docify(":");
              ol.endBold();
              ol.endDescTitle();
              ol.writeDescItem();
              //ol.startItemList();
              ol.startDescTable();
            }
            ol.addToIndex(fmd->name(),cname);
            ol.addToIndex(cname,fmd->name());
            if (Config::generateHtml && Config::htmlHelpFlag)
            {
              HtmlHelp::getInstance()->addIndexItem(cname,fmd->name(),cfname,fmd->anchor());
            }
            //ol.writeListItem();
            ol.startDescTableTitle();
            ol.startDoxyAnchor(cfname,fmd->anchor(),fmd->name());
            first=FALSE;
            ol.startEmphasis();
            ol.docify(fmd->name());
            ol.endEmphasis();
            ol.disableAllBut(OutputGenerator::Man);
            ol.writeString(" ");
            ol.enableAll();
            ol.endDoxyAnchor(cfname,fmd->anchor());
            ol.endDescTableTitle();
            //ol.newParagraph();
            ol.startDescTableData();

            if (!fmd->briefDescription().isEmpty())
            { 
              parseDoc(ol,defFileName,defLine,scopeName,fmd->name(),fmd->briefDescription());
              //ol.newParagraph();
            }
            if (!fmd->briefDescription().isEmpty() && 
                !fmd->documentation().isEmpty())
            {
              ol.newParagraph();
            }
            if (!fmd->documentation().isEmpty())
            { 
              parseDoc(ol,defFileName,defLine,scopeName,fmd->name(),fmd->documentation()+"\n");
            }
            ol.endDescTableData();
          }
          fmd=fmdl->next();
        }
      }
      if (!first) 
      { 
        //ol.endItemList(); 
        ol.endDescTable();
        ol.endDescList();
        ol.writeChar('\n'); 
      }
    }

    MemberDef *bmd=reimplements();
    ClassDef  *bcd=0; 
    if (bmd && (bcd=bmd->getClassDef()))
    {
#if 0
      if (lvirt!=Normal) // search for virtual member of the deepest base class
      {
        MemberDef *lastBmd=bmd;
        while (lastBmd) 
        {
          ClassDef *lastBcd = lastBmd->getClassDef();
          if (lastBmd->virtualness()!=Normal && 
              lastBmd->isLinkable() &&
              lastBcd->isLinkable()
             ) { bmd=lastBmd; bcd=lastBcd; }
          lastBmd=lastBmd->reimplements();
        }
      }
#endif
      // write class that contains a member that is reimplemented by this one
      if (bcd->isLinkable())
      {
        ol.newParagraph();

        QCString reimplFromLine = theTranslator->trReimplementedFromList(1);
        int markerPos = reimplFromLine.find("@0");
        if (markerPos!=-1) // should always pass this.
        {
          parseText(ol,reimplFromLine.left(markerPos)); //text left from marker
          if (bmd->isLinkable()) // replace marker with link
          {
            ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
                bmd->anchor(),bcd->name());
            if ( bcd->isLinkableInProject()/* && !Config::pdfHyperFlag*/ ) 
            {
              writePageRef(ol,bcd->getOutputFileBase(),bmd->anchor());
            }
          }
          else
          {
            ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
                0,bcd->name());
            if (bcd->isLinkableInProject()/* && !Config::pdfHyperFlag*/ )
            {
              writePageRef(ol,bcd->getOutputFileBase(),0);
            }
          }
          parseText(ol,reimplFromLine.right(
                reimplFromLine.length()-markerPos-2)); // text right from marker

          ol.disableAllBut(OutputGenerator::RTF);
          ol.newParagraph();
          ol.enableAll();
        }
        else
        {
          err("Error: translation error: no marker in trReimplementsFromList()\n");
        }
      }

      //ol.writeString(".");
    }
    MemberList *bml=reimplementedBy();
    if (bml)
    {
      MemberListIterator mli(*bml);
      MemberDef *bmd=0;
      uint count=0;
      ClassDef *bcd=0;
      for (mli.toFirst();(bmd=mli.current()) && (bcd=bmd->getClassDef());++mli)
      {
        // count the members that directly inherit from md and for
        // which the member and class are visible in the docs.
        if ( bmd->isLinkable() && bcd->isLinkable() ) count++;
      }
      if (count>0)
      {
        mli.toFirst();
        // write the list of classes that overwrite this member
        ol.newParagraph();

        QCString reimplInLine = 
          theTranslator->trReimplementedInList(count);
        static QRegExp marker("@[0-9]+");
        int index=0,newIndex,matchLen;
        // now replace all markers in reimplInLine with links to the classes
        while ((newIndex=marker.match(reimplInLine,index,&matchLen))!=-1)
        {
          parseText(ol,reimplInLine.mid(index,newIndex-index));
          bool ok;
          uint entryIndex = reimplInLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
          //bmd=bml->at(entryIndex);

          count=0;
          // find the entryIndex-th documented entry in the inheritance list.
          for (mli.toLast();(bmd=mli.current()) && (bcd=bmd->getClassDef());--mli)
          {
            if ( bmd->isLinkable() && bcd->isLinkable()) 
            {
              if (count==entryIndex) break;
              count++;
            }
          }

          if (ok && bcd && bmd) // write link for marker
          {
            ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
                bmd->anchor(),bcd->name());
            if (bcd->isLinkableInProject()/* && !Config::pdfHyperFlag*/ ) 
            {
              writePageRef(ol,bcd->getOutputFileBase(),bmd->anchor());
            }
          }
          ++mli;
          index=newIndex+matchLen;
        } 
        parseText(ol,reimplInLine.right(reimplInLine.length()-index));
        ol.disableAllBut(OutputGenerator::RTF);
        ol.newParagraph();
        ol.enableAll();
      }
    }
    // write the list of examples that use this member
    if (hasExamples())
    {
      ol.startDescList();
      ol.startBold();
      parseText(ol,theTranslator->trExamples()+": ");
      //ol.writeBoldString("Examples: ");
      ol.endBold();
      ol.endDescTitle();
      ol.writeDescItem();
      writeExample(ol,getExampleList());
      //ol.endDescItem();
      ol.endDescList();
    }
    // write reference to the source
    writeSourceDef(ol,cname);
    writeSourceRefs(ol,cname);
    writeInlineCode(ol,cname);

    ol.endIndent();
    // enable LaTeX again
    //if (Config::extractAllFlag && !hasDocs) ol.enable(OutputGenerator::Latex); 
    ol.popGeneratorState();

  }
}

void MemberDef::warnIfUndocumented()
{
  if (memberGroup) return;
  ClassDef     *cd = getClassDef();
  NamespaceDef *nd = getNamespaceDef();
  FileDef      *fd = getFileDef();
  GroupDef     *gd = getGroupDef();
  Definition *d=0;
  const char *t=0;
  if (cd) 
    t="class", d=cd; 
  else if (nd) 
    t="namespace", d=nd; 
  else if (gd)
    t="group", d=gd;
  else
    t="file", d=fd;

  if (d && d->isLinkable() && !isLinkable() && name().find('@')==-1)
   warn_undoc(defFileName,defLine,"Warning: Member %s of %s %s is not documented.",
        name().data(),t,d->name().data());
}


bool MemberDef::isLinkableInProject()
{
  return !name().isEmpty() && name().at(0)!='@' &&
         ((hasDocumentation() && !isReference())  
         ) && 
         (prot!=Private || Config::extractPrivateFlag || isFriend()) && // not a private class member
         (classDef!=0 || Config::extractStaticFlag || !isStatic()); // not a static file/namespace member
}

bool MemberDef::isLinkable()
{
  return isLinkableInProject() || isReference();
}

bool MemberDef::detailsAreVisible() const          
{ 
  return !documentation().isEmpty() || // has detailed docs
         (Config::sourceBrowseFlag && startBodyLine!=-1 && bodyDef) ||  // has reference to sources 
         (mtype==Enumeration && docEnumValues) ||  // has enum values
         (mtype==EnumValue && !briefDescription().isEmpty()) || // is doc enum value
         (!briefDescription().isEmpty() && 
           (!Config::briefMemDescFlag || Config::alwaysDetailsFlag) && 
           Config::repeatBriefFlag // has brief description inside detailed area
         ) ||
         (initLines>0 && initLines<maxInitLines) ||
         (argList!=0 && argList->hasDocumentation())
         ;
}

void MemberDef::setEnumDecl(OutputList &ed) 
{ 
  enumDeclList=new OutputList(&ed); 
  *enumDeclList+=ed;
}

bool MemberDef::hasDocumentation()
{ 
  return Definition::hasDocumentation() || (argList!=0 && argList->hasDocumentation()); 
}

void MemberDef::setMemberGroup(MemberGroup *grp)
{
  memberGroup = grp;
}

bool MemberDef::visibleMemberGroup(bool hideNoHeader) 
{ 
  return memberGroup!=0 && 
          (!hideNoHeader || memberGroup->header()!="[NOHEADER]"); 
}

QCString MemberDef::getScopeString() const
{
  QCString result;
  if (getClassDef()) result=getClassDef()->name();
  else if (getNamespaceDef()) result=getNamespaceDef()->name();
  return result;
}

void MemberDef::generateXML(QTextStream &t,Definition *def)
{
  if (mtype==EnumValue) return;

  QCString scopeName;
  if (getClassDef()) 
    scopeName=getClassDef()->name();
  else if (getNamespaceDef()) 
    scopeName=getNamespaceDef()->name();
    
  t << "            <";
  enum { define_t,variable_t,typedef_t,enum_t,function_t } xmlType = function_t;
  switch (mtype)
  {
    case Define:      t << "definedef";   xmlType=define_t;   break;
    case EnumValue:   // fall through
    case Property:    // fall through
    case Variable:    t << "variabledef"; xmlType=variable_t; break;
    case Typedef:     t << "typedef";     xmlType=typedef_t;  break;
    case Enumeration: t << "enumdef";     xmlType=enum_t;     break;
    case Function:    // fall through
    case Signal:      // fall through
    case Prototype:   // fall through
    case Friend:      // fall through
    case DCOP:        // fall through
    case Slot:        t << "functiondef"; xmlType=function_t; break;
  }
  t << " id=\"";
  t << def->getOutputFileBase()
    << "__"      // can we change this to a non ID char? 
                 // : do not seem allowed for some parsers!
    << anchor();
  t << "\"";
  if (xmlType==function_t && virtualness()!=Normal) 
    // functions has an extra "virt" attribute
  {
    t << " virt=\"";
    switch (virtualness())
    {
      case Virtual: t << "virtual";      break;
      case Pure:    t << "pure-virtual"; break;
      default: ASSERT(0);
    }
    t << "\"";
  }
  t << ">" << endl;
  
  if (xmlType!=define_t && xmlType!=enum_t &&    // These don't have types.
      (xmlType!=function_t || !type.isEmpty()) // Type is optional here.
     )
  {
    QCString typeStr = replaceAnonymousScopes(type);
    if (xmlType==typedef_t && typeStr.left(8)=="typedef ") 
      typeStr=typeStr.right(typeStr.length()-8);
    if (xmlType==function_t && typeStr.left(8)=="virtual ") 
      typeStr=typeStr.right(typeStr.length()-8);
    t << "              <type>";
    linkifyText(TextGeneratorXMLImpl(t),scopeName,name(),typeStr);
    t << "</type>" << endl;
  }

  t << "              <name>";
  writeXMLString(t,name());
  t << "</name>" << endl;
  if (xmlType==function_t) //function
  {
    t << "              <paramlist>" << endl;
    ArgumentList *declAl = new ArgumentList;
    ArgumentList *defAl = argList;
    stringToArgumentList(args,declAl);
    if (declAl->count()>0)
    {
      ArgumentListIterator declAli(*declAl);
      ArgumentListIterator defAli(*defAl);
      Argument *a;
      for (declAli.toFirst();(a=declAli.current());++declAli)
      {
        Argument *defArg = defAli.current();
        t << "                <param>" << endl;
        if (!a->attrib.isEmpty())
        {
          t << "                  <attributes>";
          writeXMLString(t,a->attrib);
          t << "</attributes>" << endl;
        }
        if (!a->type.isEmpty())
        {
          t << "                  <type>";
          linkifyText(TextGeneratorXMLImpl(t),scopeName,name(),a->type);
          t << "</type>" << endl;
        }
        if (!a->name.isEmpty())
        {
          t << "                  <declname>";
          writeXMLString(t,a->name); 
          t << "</declname>" << endl;
        }
        if (defArg && !defArg->name.isEmpty() && defArg->name!=a->name)
        {
          t << "                  <defname>";
          writeXMLString(t,defArg->name);
          t << "</defname>" << endl;
        }
        if (!a->array.isEmpty())
        {
          t << "                  <array>"; 
          writeXMLString(t,a->array); 
          t << "</array>" << endl;
        }
        if (!a->defval.isEmpty())
        {
          t << "                  <defval>";
          linkifyText(TextGeneratorXMLImpl(t),scopeName,name(),a->defval);
          t << "</defval>" << endl;
        }
        t << "                </param>" << endl;
        if (defArg) ++defAli;
      }
    }
    delete declAl;
    t << "              </paramlist>" << endl;
  }
  else if (xmlType==define_t && !args.isEmpty()) // define
  {
    t << "              <defparamlist>" << endl;
    ArgumentListIterator ali(*argList);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      t << "                <defarg>" << a->type << "</defarg>" << endl;
    }
    t << "              </defparamlist>" << endl;
    if (!init.isEmpty())
    {
      t << "             <initializer>";
      linkifyText(TextGeneratorXMLImpl(t),scopeName,name(),init);
      t << "</initializer>" << endl;
    }
  }
  else if (xmlType==enum_t) // enum
  {
    t << "              <enumvaluelist>" << endl;
    MemberListIterator emli(*enumFields);
    MemberDef *emd;
    for (emli.toFirst();(emd=emli.current());++emli)
    {
      t << "                <enumvalue>" << endl;
      t << "                  <name>";
      writeXMLString(t,emd->name());
      t << "</name>" << endl;
      if (!emd->init.isEmpty())
      {
        t << "                  <initializer>";
        writeXMLString(t,emd->init);
        t << "</initializer>" << endl;
      }
      t << "                </enumvalue>" << endl;
    }
    t << "              </enumvaluelist>" << endl;
  }
  t << "            </";
  switch (mtype)
  {
    case Define:      t << "definedef";   break;
    case EnumValue:   // fall through
    case Property:    // fall through
    case Variable:    t << "variabledef"; break;
    case Typedef:     t << "typedef";     break;
    case Enumeration: t << "enumdef";     break;
    case Function:    // fall through
    case Signal:      // fall through
    case Prototype:   // fall through
    case Friend:      // fall through
    case DCOP:        // fall through
    case Slot:        t << "functiondef"; break;
  }
  t << ">" << endl;
}

Definition *MemberDef::getCompoundDef() const
{
    NamespaceDef *nd=getNamespaceDef();
    ClassDef     *cd=getClassDef();
    FileDef      *fd=getFileDef();
    GroupDef     *gd=getGroupDef();
    Definition   *d = 0;
    if (cd) d=cd; else if (nd) d=nd; else if (gd) d=gd; else d=fd;
    ASSERT(d!=0);
    return d;
}

QCString MemberDef::anchor() const
{
  if (enumScope) return enumScope->anchor()+anc;
  return anc;
}
