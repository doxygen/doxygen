/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#include <stdlib.h>

#include "qtbc.h"
#include "xml.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "classlist.h"
#include "util.h"
#include "defargs.h"

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

const char dtd_data[]=
#include "xml_dtd.h"
;

class TextGeneratorXMLImpl : public TextGeneratorIntf
{
  public:
    TextGeneratorXMLImpl(QTextStream &t): m_t(t) {}
    void writeString(const char *s) const
    {
      writeXMLString(m_t,s); 
    }
    void writeBreak() const {}
    void writeLink(const char *extRef,const char *file,
                   const char *anchor,const char *text
                  ) const
    {
      if (extRef==0) 
      { writeXMLLink(m_t,file,anchor,text); } 
      else // external references are not supported for XML
      { writeXMLString(m_t,text); }
    }
  private:
    QTextStream &m_t;
};


void generateDTD()
{
  QCString fileName=Config_getString("OUTPUT_DIRECTORY")+"/xml/doxygen.dtd";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t << dtd_data;
}

void writeXMLString(QTextStream &t,const char *s)
{
  t << convertToXML(s);
}

void writeXMLLink(QTextStream &t,const char *compoundId,const char *memId,
                  const char *text)
{
  if (memId==0)
  {
    t << "<compoundref idref=\"" << compoundId << "\">";
    writeXMLString(t,text);
    t << "</compoundref>";
  }
  else
  {
    t << "<memberref idref=\"" << compoundId << ":" << memId << "\">";
    writeXMLString(t,text);
    t << "</memberref>";
  }
}

void generateXMLForMember(MemberDef *md,QTextStream &t,Definition *def)
{
  if (md->memberType()==MemberDef::EnumValue) return;

  QCString scopeName;
  if (md->getClassDef()) 
    scopeName=md->getClassDef()->name();
  else if (md->getNamespaceDef()) 
    scopeName=md->getNamespaceDef()->name();
    
  t << "            <";
  enum { define_t,variable_t,typedef_t,enum_t,function_t } xmlType = function_t;
  switch (md->memberType())
  {
    case MemberDef::Define:      t << "definedef";   xmlType=define_t;   break;
    case MemberDef::EnumValue:   // fall through
    case MemberDef::Property:    // fall through
    case MemberDef::Variable:    t << "variabledef"; xmlType=variable_t; break;
    case MemberDef::Typedef:     t << "typedef";     xmlType=typedef_t;  break;
    case MemberDef::Enumeration: t << "enumdef";     xmlType=enum_t;     break;
    case MemberDef::Function:    // fall through
    case MemberDef::Signal:      // fall through
    case MemberDef::Prototype:   // fall through
    case MemberDef::Friend:      // fall through
    case MemberDef::DCOP:        // fall through
    case MemberDef::Slot:        t << "functiondef"; xmlType=function_t; break;
  }
  t << " id=\"";
  t << def->getOutputFileBase()
    << "__"      // can we change this to a non ID char? 
                 // : do not seem allowed for some parsers!
    << md->anchor();
  t << "\"";
  if (xmlType==function_t && md->virtualness()!=Normal) 
    // functions has an extra "virt" attribute
  {
    t << " virt=\"";
    switch (md->virtualness())
    {
      case Virtual: t << "virtual";      break;
      case Pure:    t << "pure-virtual"; break;
      default: ASSERT(0);
    }
    t << "\"";
  }
  t << ">" << endl;
  
  if (xmlType!=define_t && xmlType!=enum_t &&    // These don't have types.
      (xmlType!=function_t || md->typeString()!=0) // Type is optional here.
     )
  {
    QCString typeStr = replaceAnonymousScopes(md->typeString());
    if (xmlType==typedef_t && typeStr.left(8)=="typedef ") 
      typeStr=typeStr.right(typeStr.length()-8);
    if (xmlType==function_t && typeStr.left(8)=="virtual ") 
      typeStr=typeStr.right(typeStr.length()-8);
    t << "              <type>";
    linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),typeStr);
    t << "</type>" << endl;
  }

  t << "              <name>";
  writeXMLString(t,md->name());
  t << "</name>" << endl;
  if (xmlType==function_t) //function
  {
    t << "              <paramlist>" << endl;
    ArgumentList *declAl = new ArgumentList;
    ArgumentList *defAl = md->argumentList();
    stringToArgumentList(md->argsString(),declAl);
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
          linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),a->type);
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
          linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),a->defval);
          t << "</defval>" << endl;
        }
        t << "                </param>" << endl;
        if (defArg) ++defAli;
      }
    }
    delete declAl;
    t << "              </paramlist>" << endl;
  }
  else if (xmlType==define_t && md->argsString()!=0) // define
  {
    t << "              <defparamlist>" << endl;
    ArgumentListIterator ali(*md->argumentList());
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      t << "                <defarg>" << a->type << "</defarg>" << endl;
    }
    t << "              </defparamlist>" << endl;
    if (!md->initializer().isEmpty())
    {
      t << "             <initializer>";
      linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),md->initializer());
      t << "</initializer>" << endl;
    }
  }
  else if (xmlType==enum_t) // enum
  {
    t << "              <enumvaluelist>" << endl;
    if (md->enumFieldList())
    {
      MemberListIterator emli(*md->enumFieldList());
      MemberDef *emd;
      for (emli.toFirst();(emd=emli.current());++emli)
      {
        t << "                <enumvalue>" << endl;
        t << "                  <name>";
        writeXMLString(t,emd->name());
        t << "</name>" << endl;
        if (!emd->initializer().isEmpty())
        {
          t << "                  <initializer>";
          writeXMLString(t,emd->initializer());
          t << "</initializer>" << endl;
        }
        t << "                </enumvalue>" << endl;
      }
    }
    t << "              </enumvaluelist>" << endl;
  }
  t << "            </";
  switch (md->memberType())
  {
    case MemberDef::Define:      t << "definedef";   break;
    case MemberDef::EnumValue:   // fall through
    case MemberDef::Property:    // fall through
    case MemberDef::Variable:    t << "variabledef"; break;
    case MemberDef::Typedef:     t << "typedef";     break;
    case MemberDef::Enumeration: t << "enumdef";     break;
    case MemberDef::Function:    // fall through
    case MemberDef::Signal:      // fall through
    case MemberDef::Prototype:   // fall through
    case MemberDef::Friend:      // fall through
    case MemberDef::DCOP:        // fall through
    case MemberDef::Slot:        t << "functiondef"; break;
  }
  t << ">" << endl;
}


void generateXMLClassSection(ClassDef *cd,QTextStream &t,MemberList *ml,const char *type)
{
  if (ml->count()>0)
  {
    t << "        <sectiondef type=\"" << type << "\">" << endl;
    t << "          <memberlist>" << endl;
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      generateXMLForMember(md,t,cd);
    }
    t << "          </memberlist>" << endl;
    t << "        </sectiondef>" << endl;
  }
}

void generateXMLForClass(ClassDef *cd,QTextStream &t)
{
  if (cd->name().find('@')!=-1) return; // skip anonymous compounds
  t << "    <compounddef id=\"" 
    << cd->getOutputFileBase() << "\" type=\"" 
    << cd->compoundTypeString() << "\">" << endl;
  t << "      <compoundname>"; 
  writeXMLString(t,cd->name()); 
  t << "</compoundname>" << endl;
  if (cd->baseClasses()->count()>0)
  {
    t << "      <basecompoundlist>" << endl;
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "        <basecompoundref idref=\"" 
        << bcd->classDef->getOutputFileBase()
        << "\" prot=\"";
      switch (bcd->prot)
      {
        case Public:    t << "public";    break;
        case Protected: t << "protected"; break;
        case Private:   t << "private";   break;
      }
      t << "\" virt=\"";
      switch(bcd->virt)
      {
        case Normal:  t << "non-virtual";  break;
        case Virtual: t << "virtual";      break;
        case Pure:    t <<"pure-virtual"; break;
      }
      t << "\"/>" << endl;
    }
    t << "      </basecompoundlist>" << endl;
  }
  if (cd->subClasses()->count()>0)
  {
    t << "      <derivedcompoundlist>" << endl;
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "        <derivedcompoundref idref=\"" 
        << bcd->classDef->getOutputFileBase()
        << "\" prot=\"";
      switch (bcd->prot)
      {
        case Public:    t << "public";    break;
        case Protected: t << "protected"; break;
        case Private:   t << "private";   break;
      }
      t << "\" virt=\"";
      switch(bcd->virt)
      {
        case Normal:  t << "non-virtual";  break;
        case Virtual: t << "virtual";      break;
        case Pure:    t << "pure-virtual"; break;
      }
      t << "\"/>" << endl;
    }
    t << "      </derivedcompoundlist>" << endl;
  }
  int numMembers = 
    cd->pubTypes.count()+cd->pubMembers.count()+cd->pubAttribs.count()+
    cd->pubSlots.count()+cd->signals.count()+cd->dcopMethods.count()+
    cd->pubStaticMembers.count()+
    cd->pubStaticAttribs.count()+cd->proTypes.count()+cd->proMembers.count()+
    cd->proAttribs.count()+cd->proSlots.count()+cd->proStaticMembers.count()+
    cd->proStaticAttribs.count()+cd->priTypes.count()+cd->priMembers.count()+
    cd->priAttribs.count()+cd->priSlots.count()+cd->priStaticMembers.count()+
    cd->priStaticAttribs.count()+cd->friends.count()+cd->related.count();
  if (numMembers>0)
  {
    t << "      <sectionlist>" << endl;
    generateXMLClassSection(cd,t,&cd->pubTypes,"public-type");
    generateXMLClassSection(cd,t,&cd->pubMembers,"public-func");
    generateXMLClassSection(cd,t,&cd->pubAttribs,"public-attrib");
    generateXMLClassSection(cd,t,&cd->pubSlots,"public-slot");
    generateXMLClassSection(cd,t,&cd->signals,"signal");
    generateXMLClassSection(cd,t,&cd->dcopMethods,"dcop-func");
    generateXMLClassSection(cd,t,&cd->properties,"property");
    generateXMLClassSection(cd,t,&cd->pubStaticMembers,"public-static-func");
    generateXMLClassSection(cd,t,&cd->pubStaticAttribs,"public-static-attrib");
    generateXMLClassSection(cd,t,&cd->proTypes,"protected-type");
    generateXMLClassSection(cd,t,&cd->proMembers,"protected-func");
    generateXMLClassSection(cd,t,&cd->proAttribs,"protected-attrib");
    generateXMLClassSection(cd,t,&cd->proSlots,"protected-slot");
    generateXMLClassSection(cd,t,&cd->proStaticMembers,"protected-static-func");
    generateXMLClassSection(cd,t,&cd->proStaticAttribs,"protected-static-attrib");
    generateXMLClassSection(cd,t,&cd->priTypes,"private-type");
    generateXMLClassSection(cd,t,&cd->priMembers,"private-func");
    generateXMLClassSection(cd,t,&cd->priAttribs,"private-attrib");
    generateXMLClassSection(cd,t,&cd->priSlots,"private-slot");
    generateXMLClassSection(cd,t,&cd->priStaticMembers,"private-static-func");
    generateXMLClassSection(cd,t,&cd->priStaticAttribs,"private-static-attrib");
    generateXMLClassSection(cd,t,&cd->friends,"signal");
    generateXMLClassSection(cd,t,&cd->related,"related");
    t << "      </sectionlist>" << endl;
  }
  t << "    </compounddef>" << endl;
}

void generateXMLFileSection(FileDef *fd,QTextStream &t,MemberList *ml,const char *type)
{
  if (ml->count()>0)
  {
    t << "        <sectiondef type=\"" << type << "\">" << endl;
    t << "          <memberlist>" << endl;
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      generateXMLForMember(md,t,fd);
    }
    t << "          </memberlist>" << endl;
    t << "        </sectiondef>" << endl;
  }
}

void generateXMLForFile(FileDef *fd,QTextStream &t)
{
  t << "    <compounddef id=\"" 
    << fd->getOutputFileBase() << "\" type=\"file\">" << endl;
  t << "      <compoundname>";
  writeXMLString(t,fd->name());
  t << "</compoundname>" << endl;
  int numMembers = fd->decDefineMembers.count()+fd->decProtoMembers.count()+
                   fd->decTypedefMembers.count()+fd->decEnumMembers.count()+
                   fd->decFuncMembers.count()+fd->decVarMembers.count();
  if (numMembers>0)
  {
    t << "      <sectionlist>" << endl;
    generateXMLFileSection(fd,t,&fd->decDefineMembers,"define");
    generateXMLFileSection(fd,t,&fd->decProtoMembers,"prototype");
    generateXMLFileSection(fd,t,&fd->decTypedefMembers,"typedef");
    generateXMLFileSection(fd,t,&fd->decEnumMembers,"enum");
    generateXMLFileSection(fd,t,&fd->decFuncMembers,"func");
    generateXMLFileSection(fd,t,&fd->decVarMembers,"var");
    t << "      </sectionlist>" << endl;
  }
  t << "    </compounddef>" << endl;
}


void generateXML()
{
  if (Config_getString("OUTPUT_DIRECTORY").isEmpty())
    Config_getString("OUTPUT_DIRECTORY")=QDir::currentDirPath();
  else
  {
    QDir dir(Config_getString("OUTPUT_DIRECTORY"));
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(Config_getString("OUTPUT_DIRECTORY")))
      {
        err("Error: tag OUTPUT_DIRECTORY: Output directory `%s' does not "
            "exist and cannot be created\n",Config_getString("OUTPUT_DIRECTORY").data());
        exit(1);
      }
      else if (!Config_getBool("QUIET"))
      {
        err("Notice: Output directory `%s' does not exist. "
            "I have created it for you.\n", Config_getString("OUTPUT_DIRECTORY").data());
      }
      dir.cd(Config_getString("OUTPUT_DIRECTORY"));
    }
    Config_getString("OUTPUT_DIRECTORY")=dir.absPath();
  }

  QDir dir(Config_getString("OUTPUT_DIRECTORY"));
  if (!dir.exists())
  {
    dir.setPath(QDir::currentDirPath());
    if (!dir.mkdir(Config_getString("OUTPUT_DIRECTORY")))
    {
      err("Cannot create directory %s\n",Config_getString("OUTPUT_DIRECTORY").data());
      return;
    }
  }
  QDir xmlDir(Config_getString("OUTPUT_DIRECTORY")+"/xml");
  if (!xmlDir.exists() && !xmlDir.mkdir(Config_getString("OUTPUT_DIRECTORY")+"/xml"))
  {
    err("Could not create xml directory in %s\n",Config_getString("OUTPUT_DIRECTORY").data());
    return;
  }
  generateDTD();
  
  QCString fileName=Config_getString("OUTPUT_DIRECTORY")+"/xml/doxygen.xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t << "<?xml version='1.0' encoding='ISO-8859-1' standalone='no'?>" << endl;
  t << "<!DOCTYPE doxygen SYSTEM \"doxygen.dtd\">" << endl;
  t << "<doxygen>" << endl;
  if (Doxygen::classList.count()+Doxygen::inputNameList.count()>0)
  {
    t << "  <compoundlist>" << endl;
    ClassListIterator cli(Doxygen::classList);
    ClassDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      generateXMLForClass(cd,t);
    }
    FileNameListIterator fnli(Doxygen::inputNameList);
    FileName *fn;
    for (;(fn=fnli.current());++fnli)
    {
      FileNameIterator fni(*fn);
      FileDef *fd;
      for (;(fd=fni.current());++fni)
      {
        generateXMLForFile(fd,t);
      }
    }
    t << "  </compoundlist>" << endl;
  }
  t << "</doxygen>" << endl;
}


