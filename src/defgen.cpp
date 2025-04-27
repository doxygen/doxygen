/******************************************************************************
 *
 *
 *
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

#include <stdlib.h>

#include "portable.h"
#include "defgen.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "classlist.h"
#include "util.h"
#include "defargs.h"
#include "outputgen.h"
#include "dot.h"
#include "dotclassgraph.h"
#include "arguments.h"
#include "memberlist.h"
#include "namespacedef.h"
#include "filedef.h"
#include "filename.h"
#include "dir.h"
#include "textstream.h"

#define DEF_DB(x)

static inline void writeDEFString(TextStream &t,const QCString &s)
{
  t << '\'';
  if (!s.isEmpty())
  {
    const char* p=s.data();
    char c = 0;

    while ((c = *(p++)))
    {
      if (c == '\'') t << '\\';
      t << c;
    }
  }
  t << '\'';
}

static void generateDEFForMember(const MemberDef *md,
    TextStream &t,
    const Definition *def,
    const QCString &prefix)
{
  QCString memPrefix;

  // + declaration
  // - reimplements
  // - reimplementedBy
  // - exceptions
  // - const/volatile specifiers
  // - examples
  // + source definition
  // - source references
  // - source referenced by
  // - include code

  if (md->memberType()==MemberType::EnumValue) return;

  QCString scopeName;
  if (md->getClassDef())
    scopeName=md->getClassDef()->name();
  else if (md->getNamespaceDef())
    scopeName=md->getNamespaceDef()->name();

  t << "    " << prefix << "-member = {\n";
  memPrefix = "      ";
  memPrefix.append( prefix );
  memPrefix.append( "-mem-" );

  QCString memType;
  bool isFunc=FALSE;
  switch (md->memberType())
  {
    case MemberType::Define:      memType="define";     break;
    case MemberType::EnumValue:   ASSERT(0);            break;
    case MemberType::Property:    memType="property";   break;
    case MemberType::Event:       memType="event";      break;
    case MemberType::Variable:    memType="variable";   break;
    case MemberType::Typedef:     memType="typedef";    break;
    case MemberType::Enumeration: memType="enum";       break;
    case MemberType::Interface:   memType="interface";  break;
    case MemberType::Service:     memType="service";    break;
    case MemberType::Sequence:    memType="sequence";   break;
    case MemberType::Dictionary:  memType="dictionary"; break;
    case MemberType::Function:    memType="function";   isFunc=TRUE; break;
    case MemberType::Signal:      memType="signal";     isFunc=TRUE; break;
    case MemberType::Friend:      memType="friend";     isFunc=TRUE; break;
    case MemberType::DCOP:        memType="dcop";       isFunc=TRUE; break;
    case MemberType::Slot:        memType="slot";       isFunc=TRUE; break;
  }

  t << memPrefix << "kind = '" << memType << "';\n";
  t << memPrefix << "id   = '"
    << md->getOutputFileBase() << "_1" << md->anchor()
    << "';\n";

  t << memPrefix << "virt = " << to_string_lower(md->virtualness()) << ";\n";
  t << memPrefix << "prot = " << to_string_lower(md->protection()) << ";\n";

  if (md->memberType()!=MemberType::Define &&
      md->memberType()!=MemberType::Enumeration
     )
  {
    QCString typeStr = replaceAnonymousScopes(md->typeString());
    t << memPrefix << "type = <<_EnD_oF_dEf_TeXt_\n" << typeStr << "\n"
      << "_EnD_oF_dEf_TeXt_;\n";
  }

  t << memPrefix << "name = '" << md->name() << "';\n";

  if (isFunc) //function
  {
    const ArgumentList &defAl = md->argumentList();
    ArgumentList declAl = *stringToArgumentList(md->getLanguage(),md->argsString());
    QCString fcnPrefix = "  " + memPrefix + "param-";

    auto defIt = defAl.begin();
    for (const Argument &a : declAl)
    {
      const Argument *defArg = nullptr;
      if (defIt!=defAl.end())
      {
        defArg = &(*defIt);
        ++defIt;
      }
      t << memPrefix << "param = {\n";
      if (!a.attrib.isEmpty())
      {
        t << fcnPrefix << "attributes = ";
        writeDEFString(t,a.attrib);
        t << ";\n";
      }
      if (!a.type.isEmpty())
      {
        t << fcnPrefix << "type = <<_EnD_oF_dEf_TeXt_\n"
          << a.type << "\n_EnD_oF_dEf_TeXt_;\n";
      }
      if (!a.name.isEmpty())
      {
        t << fcnPrefix << "declname = ";
        writeDEFString(t,a.name);
        t << ";\n";
      }
      if (defArg && !defArg->name.isEmpty() && defArg->name!=a.name)
      {
        t << fcnPrefix << "defname = ";
        writeDEFString(t,defArg->name);
        t << ";\n";
      }
      if (!a.array.isEmpty())
      {
        t << fcnPrefix << "array = ";
        writeDEFString(t,a.array);
        t << ";\n";
      }
      if (!a.defval.isEmpty())
      {
        t << fcnPrefix << "defval = <<_EnD_oF_dEf_TeXt_\n"
          << a.defval << "\n_EnD_oF_dEf_TeXt_;\n";
      }
      t << "      }; /*" << fcnPrefix << "-param */\n";
    }
  }
  else if (  md->memberType()==MemberType::Define
      && md->argsString()!=nullptr)
  {
    QCString defPrefix = "  " + memPrefix + "def-";
    for (const Argument &a : md->argumentList())
    {
      t << memPrefix << "param  = {\n";
      t << defPrefix << "name = '" << a.type << "';\n";
      t << "      }; /*" << defPrefix << "-param */\n";
    }
  }

  if (!md->initializer().isEmpty())
  {
    t << memPrefix << "initializer = <<_EnD_oF_dEf_TeXt_\n"
      << md->initializer() << "\n_EnD_oF_dEf_TeXt_;\n";
  }
  // TODO: exceptions, const volatile
  if (md->memberType()==MemberType::Enumeration) // enum
  {
    for (const auto &emd : md->enumFieldList())
    {
      t << memPrefix << "enum = { enum-name = " << emd->name() << ';';
      if (!emd->initializer().isEmpty())
      {
        t << " enum-value = ";
        writeDEFString(t,emd->initializer());
        t << ';';
      }
      t << " };\n";
    }
  }

  t << memPrefix << "desc-file = '" << md->getDefFileName() << "';\n";
  t << memPrefix << "desc-line = '" << md->getDefLine()     << "';\n";
  t << memPrefix << "briefdesc =    <<_EnD_oF_dEf_TeXt_\n"
    << md->briefDescription() << "\n_EnD_oF_dEf_TeXt_;\n";
  t << memPrefix << "documentation = <<_EnD_oF_dEf_TeXt_\n"
    << md->documentation() << "\n_EnD_oF_dEf_TeXt_;\n";

  //printf("md->getReferencesMembers()=%p\n",md->getReferencesMembers());

  QCString refPrefix = "  " + memPrefix + "ref-";
  auto refList = md->getReferencesMembers();
  for (const auto &rmd : refList)
  {
    if (rmd->getStartBodyLine()!=-1 && rmd->getBodyDef())
    {
      t << memPrefix << "referenceto = {\n";
      t << refPrefix << "id = '"
        << rmd->getBodyDef()->getOutputFileBase()
        << "_1"   // encoded ':' character (see util.cpp:convertNameToFile)
        << rmd->anchor() << "';\n";

      t << refPrefix << "line = '"
        << rmd->getStartBodyLine() << "';\n";

      QCString scope = rmd->getScopeString();
      QCString name = rmd->name();
      if (!scope.isEmpty() && scope!=def->name())
      {
        name.prepend(scope+"::");
      }

      t << refPrefix << "name = ";
      writeDEFString(t,name);
      t << ';' << "\n    };\n";
    }
  }
  auto refByList = md->getReferencedByMembers();
  for (const auto &rmd : refByList)
  {
    if (rmd->getStartBodyLine()!=-1 && rmd->getBodyDef())
    {
      t << memPrefix << "referencedby = {\n";
      t << refPrefix << "id = '"
        << rmd->getBodyDef()->getOutputFileBase()
        << "_1"   // encoded ':' character (see util.cpp:convertNameToFile)
        << rmd->anchor() << "';\n";

      t << refPrefix << "line = '"
        << rmd->getStartBodyLine() << "';\n";

      QCString scope = rmd->getScopeString();
      QCString name = rmd->name();
      if (!scope.isEmpty() && scope!=def->name())
      {
        name.prepend(scope+"::");
      }

      t << refPrefix << "name = ";
      writeDEFString(t,name);
      t << ';' << "\n    };\n";
    }
  }

  t << "    }; /* " << prefix << "-member */\n";
}


static void generateDEFClassSection(const ClassDef *cd,
    TextStream &t,
    const MemberList *ml,
    const QCString &kind)
{
  if (cd && ml && !ml->empty())
  {
    t << "  cp-section = {\n";
    t << "    sec-kind = '" << kind << "';\n";

    for (const auto &md : *ml)
    {
      generateDEFForMember(md,t,cd,"sec");
    }
    t << "  }; /* cp-section */\n";
  }
}

static void generateDEFForClass(const ClassDef *cd,TextStream &t)
{
  // + brief description
  // + detailed description
  // - template arguments
  // - include files
  // + inheritance diagram
  // + list of direct super classes
  // + list of direct sub classes
  // + collaboration diagram
  // - list of all members
  // + user defined member sections
  // + standard member sections
  // + detailed member documentation
  // - examples

  if (cd->isReference()) return; // skip external references.
  if (cd->name().find('@')!=-1) return; // skip anonymous compounds.
  if (cd->isImplicitTemplateInstance()) return; // skip generated template instances.

  t << cd->compoundTypeString() << " = {\n";
  t << "  cp-id     = '" << cd->getOutputFileBase() << "';\n";
  t << "  cp-name   = '" << cd->name() << "';\n";

  for (const auto &bcd : cd->baseClasses())
  {
    t << "  cp-ref     = {\n" << "    ref-type = base;\n";
    t << "    ref-id   = '" << bcd.classDef->getOutputFileBase() << "';\n";
    t << "    ref-prot = " << to_string_lower_class(bcd.prot) << ";\n";
    t << "    ref-virt = " << to_string_lower(bcd.virt) << ";\n";
    t << "  };\n";
  }

  for (const auto &bcd : cd->subClasses())
  {
    t << "  cp-ref     = {\n" << "    ref-type = derived;\n";
    t << "    ref-id   = '" << bcd.classDef->getOutputFileBase() << "';\n";
    t << "    ref-prot = " << to_string_lower_class(bcd.prot) << ";\n";
    t << "    ref-virt = " << to_string_lower(bcd.virt) << ";\n";
    t << "  };\n";
  }

  size_t numMembers = 0;
  for (const auto &ml : cd->getMemberLists())
  {
    if (!ml->listType().isDetailed())
    {
      numMembers+=ml->size();
    }
  }
  if (numMembers>0)
  {
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PubTypes()),"public-type");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::Interfaces()),"interfaces");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::Services()),"services");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PubMethods()),"public-func");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PubAttribs()),"public-attrib");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PubSlots()),"public-slot");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::Signals()),"signal");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::DcopMethods()),"dcop-func");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::Properties()),"property");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PubStaticMethods()),"public-static-func");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PubStaticAttribs()),"public-static-attrib");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::ProTypes()),"protected-type");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::ProMethods()),"protected-func");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::ProAttribs()),"protected-attrib");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::ProSlots()),"protected-slot");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::ProStaticMethods()),"protected-static-func");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::ProStaticAttribs()),"protected-static-attrib");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PriTypes()),"private-type");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PriMethods()),"private-func");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PriAttribs()),"private-attrib");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PriSlots()),"private-slot");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PriStaticMethods()),"private-static-func");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::PriStaticAttribs()),"private-static-attrib");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::Friends()),"signal");
    generateDEFClassSection(cd,t,cd->getMemberList(MemberListType::Related()),"related");
  }

  t << "  cp-filename  = '" << cd->getDefFileName() << "';\n";
  t << "  cp-fileline  = '" << cd->getDefLine()     << "';\n";
  t << "  cp-briefdesc = <<_EnD_oF_dEf_TeXt_\n"
    << cd->briefDescription() << "\n_EnD_oF_dEf_TeXt_;\n";

  t << "  cp-documentation = <<_EnD_oF_dEf_TeXt_\n"
    << cd->documentation() << "\n_EnD_oF_dEf_TeXt_;\n";

  DotClassGraph inheritanceGraph(cd,GraphType::Inheritance);
  if (!inheritanceGraph.isTrivial())
  {
    t << "  cp-inheritancegraph = <<_EnD_oF_dEf_TeXt_\n";
    inheritanceGraph.writeDEF(t);
    t << "\n_EnD_oF_dEf_TeXt_;\n";
  }
  DotClassGraph collaborationGraph(cd,GraphType::Collaboration);
  if (!collaborationGraph.isTrivial())
  {
    t << "  cp-collaborationgraph = <<_EnD_oF_dEf_TeXt_\n";
    collaborationGraph.writeDEF(t);
    t << "\n_EnD_oF_dEf_TeXt_;\n";
  }
  t << "}; /* " <<  cd->compoundTypeString() << " */\n";
}

static void generateDEFSection(const Definition *d,
    TextStream &t,
    const MemberList *ml,
    const QCString &kind)
{
  if (ml && !ml->empty())
  {
    t << "    " << kind << " = {\n";
    for (const auto &md : *ml)
    {
      generateDEFForMember(md,t,d,kind);
    }
    t << "    };\n";
  }
}

static void generateDEFForNamespace(const NamespaceDef *nd,TextStream &t)
{
  if (nd->isReference()) return; // skip external references
  t << "  namespace = {\n";
  t << "    ns-id   = '" << nd->getOutputFileBase() << "';\n";
  t << "    ns-name = ";
  writeDEFString(t,nd->name());
  t << ";\n";

  generateDEFSection(nd,t,nd->getMemberList(MemberListType::DecDefineMembers()),"define");
  generateDEFSection(nd,t,nd->getMemberList(MemberListType::DecProtoMembers()),"prototype");
  generateDEFSection(nd,t,nd->getMemberList(MemberListType::DecTypedefMembers()),"typedef");
  generateDEFSection(nd,t,nd->getMemberList(MemberListType::DecSequenceMembers()),"sequence");
  generateDEFSection(nd,t,nd->getMemberList(MemberListType::DecDictionaryMembers()),"dictionary");
  generateDEFSection(nd,t,nd->getMemberList(MemberListType::DecEnumMembers()),"enum");
  generateDEFSection(nd,t,nd->getMemberList(MemberListType::DecFuncMembers()),"func");
  generateDEFSection(nd,t,nd->getMemberList(MemberListType::DecVarMembers()),"var");

  t << "  ns-filename  = '" << nd->getDefFileName() << "';\n";
  t << "  ns-fileline  = '" << nd->getDefLine()     << "';\n";
  t << "  ns-briefdesc = <<_EnD_oF_dEf_TeXt_\n"
    << nd->briefDescription() << "\n_EnD_oF_dEf_TeXt_;\n";

  t << "  ns-documentation = <<_EnD_oF_dEf_TeXt_\n"
    << nd->documentation() << "\n_EnD_oF_dEf_TeXt_;\n";
  t << "  };\n";
}

static void generateDEFForFile(const FileDef *fd,TextStream &t)
{
  if (fd->isReference()) return; // skip external references

  t << "file = {\n";
  t << "  file-id   = '" << fd->getOutputFileBase() << "';\n";
  t << "  file-name = ";
  writeDEFString(t,fd->name());
  t << ";\n";

  generateDEFSection(fd,t,fd->getMemberList(MemberListType::DecDefineMembers()),"define");
  generateDEFSection(fd,t,fd->getMemberList(MemberListType::DecProtoMembers()),"prototype");
  generateDEFSection(fd,t,fd->getMemberList(MemberListType::DecTypedefMembers()),"typedef");
  generateDEFSection(fd,t,fd->getMemberList(MemberListType::DecSequenceMembers()),"sequence");
  generateDEFSection(fd,t,fd->getMemberList(MemberListType::DecDictionaryMembers()),"dictionary");
  generateDEFSection(fd,t,fd->getMemberList(MemberListType::DecEnumMembers()),"enum");
  generateDEFSection(fd,t,fd->getMemberList(MemberListType::DecFuncMembers()),"func");
  generateDEFSection(fd,t,fd->getMemberList(MemberListType::DecVarMembers()),"var");

  t << "  file-full-name  = '" << fd->getDefFileName() << "';\n";
  t << "  file-first-line = '" << fd->getDefLine()     << "';\n";

  t << "  file-briefdesc  = <<_EnD_oF_dEf_TeXt_\n"
    << fd->briefDescription() << "\n_EnD_oF_dEf_TeXt_;\n";

  t << "  file-documentation = <<_EnD_oF_dEf_TeXt_\n"
    << fd->documentation() << "\n_EnD_oF_dEf_TeXt_;\n";

  t << "}; /* file */\n";
}


void generateDEF()
{
  QCString outputDirectory = Config_getString(OUTPUT_DIRECTORY)+"/def";
  Dir defDir(outputDirectory.str());
  if (!defDir.exists() && !defDir.mkdir(outputDirectory.str()))
  {
    err("Could not create def directory in {}\n",outputDirectory);
    return;
  }

  QCString fileName=outputDirectory+"/doxygen.def";
  std::ofstream f = Portable::openOutputStream(fileName);
  if (!f.is_open())
  {
    err("Cannot open file {} for writing!\n",fileName);
    return;
  }
  TextStream t(&f);
  t << "AutoGen Definitions dummy;\n";

  if (Doxygen::classLinkedMap->size()+
      Doxygen::inputNameLinkedMap->size()+
      Doxygen::namespaceLinkedMap->size()>0)
  {
    for (const auto &cd : *Doxygen::classLinkedMap)
    {
      generateDEFForClass(cd.get(),t);
    }
    for (const auto &fn : *Doxygen::inputNameLinkedMap)
    {
      for (const auto &fd : *fn)
      {
        generateDEFForFile(fd.get(),t);
      }
    }
    for (const auto &nd : *Doxygen::namespaceLinkedMap)
    {
      generateDEFForNamespace(nd.get(),t);
    }
  }
  else
  {
    t << "dummy_value = true;\n";
  }
}
