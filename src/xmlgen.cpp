/******************************************************************************
 *
 * 
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
#include "xmlgen.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "classlist.h"
#include "util.h"
#include "defargs.h"

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

static void writeXMLString(QTextStream &t,const char *s)
{
  t << convertToXML(s);
}

static void writeXMLLink(QTextStream &t,const char *compoundId,const char *memId,
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
    t << "<memberref idref=\"" << compoundId << "_1" << memId << "\">";
    writeXMLString(t,text);
    t << "</memberref>";
  }
}

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


void generateXMLForMember(MemberDef *md,QTextStream &t,Definition *def)
{
  if (md->memberType()==MemberDef::EnumValue) return;

  QCString scopeName;
  if (md->getClassDef()) 
    scopeName=md->getClassDef()->name();
  else if (md->getNamespaceDef()) 
    scopeName=md->getNamespaceDef()->name();
    
  t << "      <memberdef kind=\"";
  //enum { define_t,variable_t,typedef_t,enum_t,function_t } xmlType = function_t;
  QCString memType;
  bool isFunc=FALSE;
  switch (md->memberType())
  {
    case MemberDef::Define:      memType="define";    break;
    case MemberDef::EnumValue:   ASSERT(0);           break;
    case MemberDef::Property:    memType="property";  break;
    case MemberDef::Variable:    memType="variable";  break;
    case MemberDef::Typedef:     memType="typedef";   break;
    case MemberDef::Enumeration: memType="enum";      break;
    case MemberDef::Function:    memType="function";  isFunc=TRUE; break;
    case MemberDef::Signal:      memType="signal";    isFunc=TRUE; break;
    case MemberDef::Prototype:   memType="prototype"; isFunc=TRUE; break;
    case MemberDef::Friend:      memType="friend";    isFunc=TRUE; break;
    case MemberDef::DCOP:        memType="dcop";      isFunc=TRUE; break;
    case MemberDef::Slot:        memType="slot";      isFunc=TRUE; break;
  }
  t << memType << "\" id=\"";
  t << def->getOutputFileBase()
    << "_1"      // encoded `:' character (see util.cpp:convertNameToFile)
    << md->anchor();
  t << "\"";
  t << " virt=\"";
  switch (md->virtualness())
  {
    case Normal:  t << "normal";       break;
    case Virtual: t << "virtual";      break;
    case Pure:    t << "pure-virtual"; break;
    default: ASSERT(0);
  }
  t << "\" prot=\"";
  switch(md->protection())
  {
    case Public:    t << "public";     break;
    case Protected: t << "protected";  break;
    case Private:   t << "private";    break;
  }
  t << "\">" << endl;
  
  if (md->memberType()!=MemberDef::Define &&
      md->memberType()!=MemberDef::Enumeration
     )
  {
    QCString typeStr = replaceAnonymousScopes(md->typeString());
    t << "        <type>";
    linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),typeStr);
    t << "</type>" << endl;
  }

  t << "        <name>";
  writeXMLString(t,md->name());
  t << "</name>" << endl;
  if (isFunc) //function
  {
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
        t << "        <param>" << endl;
        if (!a->attrib.isEmpty())
        {
          t << "          <attributes>";
          writeXMLString(t,a->attrib);
          t << "</attributes>" << endl;
        }
        if (!a->type.isEmpty())
        {
          t << "          <type>";
          linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),a->type);
          t << "</type>" << endl;
        }
        if (!a->name.isEmpty())
        {
          t << "          <declname>";
          writeXMLString(t,a->name); 
          t << "</declname>" << endl;
        }
        if (defArg && !defArg->name.isEmpty() && defArg->name!=a->name)
        {
          t << "          <defname>";
          writeXMLString(t,defArg->name);
          t << "</defname>" << endl;
        }
        if (!a->array.isEmpty())
        {
          t << "          <array>"; 
          writeXMLString(t,a->array); 
          t << "</array>" << endl;
        }
        if (!a->defval.isEmpty())
        {
          t << "          <defval>";
          linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),a->defval);
          t << "</defval>" << endl;
        }
        t << "        </param>" << endl;
        if (defArg) ++defAli;
      }
    }
    delete declAl;
  }
  else if (md->memberType()==MemberDef::Define && 
           md->argsString()!=0) // define
  {
    ArgumentListIterator ali(*md->argumentList());
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      t << "        <param><defname>" << a->type << "</defname></param>" << endl;
    }
  }
  if (!md->initializer().isEmpty())
  {
    t << "        <initializer>";
    linkifyText(TextGeneratorXMLImpl(t),scopeName,md->name(),md->initializer());
    t << "</initializer>" << endl;
  }
  // TODO: exceptions, const volatile
  if (md->memberType()==MemberDef::Enumeration) // enum
  {
    if (md->enumFieldList())
    {
      MemberListIterator emli(*md->enumFieldList());
      MemberDef *emd;
      for (emli.toFirst();(emd=emli.current());++emli)
      {
        t << "        <enumvalue>" << endl;
        t << "          <name>";
        writeXMLString(t,emd->name());
        t << "</name>" << endl;
        if (!emd->initializer().isEmpty())
        {
          t << "          <initializer>";
          writeXMLString(t,emd->initializer());
          t << "</initializer>" << endl;
        }
        t << "        </enumvalue>" << endl;
      }
    }
  }
  t << "      </memberdef>" << endl;
}


void generateXMLClassSection(ClassDef *cd,QTextStream &t,MemberList *ml,const char *kind)
{
  if (ml->count()>0)
  {
    t << "    <sectiondef kind=\"" << kind << "\">" << endl;
    //t << "          <memberlist>" << endl;
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      generateXMLForMember(md,t,cd);
    }
    //t << "          </memberlist>" << endl;
    t << "    </sectiondef>" << endl;
  }
}

void generateXMLForClass(ClassDef *cd,QTextStream &t)
{
  // brief description
  // detailed description
  // template arguments
  // include files
  // inheritance diagram
  // list of direct super classes
  // list of direct sub classes
  // collaboration diagram
  // list of all members
  // user defined member sections
  // standard member sections
  // detailed documentation
  // detailed member documentation
  
  if (cd->name().find('@')!=-1) return; // skip anonymous compounds
  t << "  <compounddef id=\"" 
    << cd->getOutputFileBase() << "\" kind=\"" 
    << cd->compoundTypeString() << "\">" << endl;
  t << "    <compoundname>"; 
  writeXMLString(t,cd->name()); 
  t << "</compoundname>" << endl;
  if (cd->baseClasses()->count()>0)
  {
    //t << "      <basecompoundlist>" << endl;
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "    <basecompoundref idref=\"" 
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
    //t << "      </basecompoundlist>" << endl;
  }
  if (cd->subClasses()->count()>0)
  {
    //t << "      <derivedcompoundlist>" << endl;
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      t << "    <derivedcompoundref idref=\"" 
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
    //t << "      </derivedcompoundlist>" << endl;
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
    //t << "      <sectionlist>" << endl;
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
    //t << "      </sectionlist>" << endl;
  }
  t << "  </compounddef>" << endl;
}

void generateXMLFileSection(FileDef *fd,QTextStream &t,MemberList *ml,const char *kind)
{
  if (ml->count()>0)
  {
    t << "      <sectiondef kind=\"" << kind << "\">" << endl;
    //t << "          <memberlist>" << endl;
    MemberListIterator mli(*ml);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      generateXMLForMember(md,t,fd);
    }
    //t << "          </memberlist>" << endl;
    t << "      </sectiondef>" << endl;
  }
}

void generateXMLForFile(FileDef *fd,QTextStream &t)
{
  t << "  <compounddef id=\"" 
    << fd->getOutputFileBase() << "\" kind=\"file\">" << endl;
  t << "    <compoundname>";
  writeXMLString(t,fd->name());
  t << "</compoundname>" << endl;
  int numMembers = fd->decDefineMembers.count()+fd->decProtoMembers.count()+
                   fd->decTypedefMembers.count()+fd->decEnumMembers.count()+
                   fd->decFuncMembers.count()+fd->decVarMembers.count();
  if (numMembers>0)
  {
    //t << "      <sectionlist>" << endl;
    generateXMLFileSection(fd,t,&fd->decDefineMembers,"define");
    generateXMLFileSection(fd,t,&fd->decProtoMembers,"prototype");
    generateXMLFileSection(fd,t,&fd->decTypedefMembers,"typedef");
    generateXMLFileSection(fd,t,&fd->decEnumMembers,"enum");
    generateXMLFileSection(fd,t,&fd->decFuncMembers,"func");
    generateXMLFileSection(fd,t,&fd->decVarMembers,"var");
    //t << "      </sectionlist>" << endl;
  }
  t << "  </compounddef>" << endl;
}


void generateXML()
{
  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  if (outputDirectory.isEmpty())
  {
    outputDirectory=QDir::currentDirPath();
  }
  else
  {
    QDir dir(outputDirectory);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(outputDirectory))
      {
        err("Error: tag OUTPUT_DIRECTORY: Output directory `%s' does not "
            "exist and cannot be created\n",outputDirectory.data());
        exit(1);
      }
      else if (!Config_getBool("QUIET"))
      {
        err("Notice: Output directory `%s' does not exist. "
            "I have created it for you.\n", outputDirectory.data());
      }
      dir.cd(outputDirectory);
    }
    outputDirectory=dir.absPath();
  }

  QDir dir(outputDirectory);
  if (!dir.exists())
  {
    dir.setPath(QDir::currentDirPath());
    if (!dir.mkdir(outputDirectory))
    {
      err("Cannot create directory %s\n",outputDirectory.data());
      return;
    }
  }
  QDir xmlDir(outputDirectory+"/xml");
  if (!xmlDir.exists() && !xmlDir.mkdir(outputDirectory+"/xml"))
  {
    err("Could not create xml directory in %s\n",outputDirectory.data());
    return;
  }
  
  QCString fileName=outputDirectory+"/xml/doxygen.xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  QTextStream t(&f);
  t << "<?xml version='1.0' encoding='ISO-8859-1' standalone='yes'?>" << endl;
  //t << "<!DOCTYPE doxygen SYSTEM \"doxygen.dtd\">" << endl;
  t << "<doxygen>" << endl;
  if (Doxygen::classSDict.count()+Doxygen::inputNameList.count()>0)
  {
    //t << "  <compoundlist>" << endl;
    ClassSDict::Iterator cli(Doxygen::classSDict);
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
    //t << "  </compoundlist>" << endl;
  }
  t << "</doxygen>" << endl;
}


