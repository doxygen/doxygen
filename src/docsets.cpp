/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#include <set>
#include <stack>

#include <qfile.h>
#include "docsets.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "groupdef.h"
#include "classdef.h"
#include "filedef.h"
#include "memberdef.h"
#include "namespacedef.h"
#include "util.h"
#include "ftextstream.h"

struct DocSets::Private
{
  QCString indent();
  QFile nf;
  QFile tf;
  FTextStream nts;
  FTextStream tts;
  std::stack<bool> indentStack;
  std::set<std::string> scopes;
};


DocSets::DocSets() : p(std::make_unique<Private>())
{
}

DocSets::~DocSets()
{
}

void DocSets::initialize()
{
  // -- get config options
  QCString projectName = Config_getString(PROJECT_NAME);
  if (projectName.isEmpty()) projectName="root";
  QCString bundleId = Config_getString(DOCSET_BUNDLE_ID);
  if (bundleId.isEmpty()) bundleId="org.doxygen.Project";
  QCString feedName = Config_getString(DOCSET_FEEDNAME);
  if (feedName.isEmpty()) feedName="FeedName";
  QCString publisherId = Config_getString(DOCSET_PUBLISHER_ID);
  if (publisherId.isEmpty()) publisherId="PublisherId";
  QCString publisherName = Config_getString(DOCSET_PUBLISHER_NAME);
  if (publisherName.isEmpty()) publisherName="PublisherName";
  QCString projectNumber = Config_getString(PROJECT_NUMBER);
  if (projectNumber.isEmpty()) projectNumber="ProjectNumber";

  // -- write Makefile
  {
  QCString mfName = Config_getString(HTML_OUTPUT) + "/Makefile";
  QFile makefile(mfName);
  if (!makefile.open(IO_WriteOnly))
  {
    term("Could not open file %s for writing\n",mfName.data());
  }
  FTextStream ts(&makefile);

  ts << "DOCSET_NAME=" << bundleId << ".docset\n"
        "DOCSET_CONTENTS=$(DOCSET_NAME)/Contents\n"
        "DOCSET_RESOURCES=$(DOCSET_CONTENTS)/Resources\n"
        "DOCSET_DOCUMENTS=$(DOCSET_RESOURCES)/Documents\n"
        "DESTDIR=~/Library/Developer/Shared/Documentation/DocSets\n"
        "XCODE_INSTALL=\"$(shell xcode-select -print-path)\"\n"
        "\n"
        "all: docset\n"
        "\n"
        "docset:\n"
        "\tmkdir -p $(DOCSET_DOCUMENTS)\n"
        "\tcp Nodes.xml $(DOCSET_RESOURCES)\n"
        "\tcp Tokens.xml $(DOCSET_RESOURCES)\n"
        "\tcp Info.plist $(DOCSET_CONTENTS)\n"
        "\ttar --exclude $(DOCSET_NAME) \\\n"
        "\t    --exclude Nodes.xml \\\n"
        "\t    --exclude Tokens.xml \\\n"
        "\t    --exclude Info.plist \\\n"
        "\t    --exclude Makefile -c -f - . \\\n"
        "\t    | (cd $(DOCSET_DOCUMENTS); tar xvf -)\n"
        "\t$(XCODE_INSTALL)/usr/bin/docsetutil index $(DOCSET_NAME)\n"
        "\trm -f $(DOCSET_DOCUMENTS)/Nodes.xml\n"
        "\trm -f $(DOCSET_DOCUMENTS)/Info.plist\n"
        "\trm -f $(DOCSET_DOCUMENTS)/Makefile\n"
        "\trm -f $(DOCSET_RESOURCES)/Nodes.xml\n"
        "\trm -f $(DOCSET_RESOURCES)/Tokens.xml\n"
        "\n"
        "clean:\n"
        "\trm -rf $(DOCSET_NAME)\n"
        "\n"
        "install: docset\n"
        "\tmkdir -p $(DESTDIR)\n"
        "\tcp -R $(DOCSET_NAME) $(DESTDIR)\n"
        "\n"
        "uninstall:\n"
        "\trm -rf $(DESTDIR)/$(DOCSET_NAME)\n"
        "\n"
        "always:\n";
  }

  // -- write Info.plist
  {
  QCString plName = Config_getString(HTML_OUTPUT) + "/Info.plist";
  QFile plist(plName);
  if (!plist.open(IO_WriteOnly))
  {
    term("Could not open file %s for writing\n",plName.data());
  }
  FTextStream ts(&plist);

  ts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\"\n"
        "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
        "<plist version=\"1.0\">\n"
        "<dict>\n"
        "     <key>CFBundleName</key>\n"
        "     <string>" << projectName << "</string>\n"
        "     <key>CFBundleIdentifier</key>\n"
        "     <string>" << bundleId << "</string>\n"
        "     <key>CFBundleVersion</key>\n"
        "     <string>" << projectNumber << "</string>\n"
        "     <key>DocSetFeedName</key>\n"
        "     <string>" << feedName << "</string>\n"
        "     <key>DocSetPublisherIdentifier</key>\n"
        "     <string>" << publisherId << "</string>\n"
        "     <key>DocSetPublisherName</key>\n"
        "     <string>" << publisherName << "</string>\n"
        // markers for Dash
        "     <key>DashDocSetFamily</key>\n"
        "     <string>doxy</string>\n"
        "     <key>DocSetPlatformFamily</key>\n"
        "     <string>doxygen</string>\n"
        "</dict>\n"
        "</plist>\n";
  }

  // -- start Nodes.xml
  QCString notes = Config_getString(HTML_OUTPUT) + "/Nodes.xml";
  p->nf.setName(notes);
  if (!p->nf.open(IO_WriteOnly))
  {
    term("Could not open file %s for writing\n",notes.data());
  }
  //QCString indexName=Config_getBool(GENERATE_TREEVIEW)?"main":"index";
  QCString indexName="index";
  p->nts.setDevice(&p->nf);
  p->nts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  p->nts << "<DocSetNodes version=\"1.0\">" << endl;
  p->nts << "  <TOC>" << endl;
  p->nts << "    <Node>" << endl;
  p->nts << "      <Name>Root</Name>" << endl;
  p->nts << "      <Path>" << indexName << Doxygen::htmlFileExtension << "</Path>" << endl;
  p->nts << "      <Subnodes>" << endl;
  p->indentStack.push(true);

  QCString tokens = Config_getString(HTML_OUTPUT) + "/Tokens.xml";
  p->tf.setName(tokens);
  if (!p->tf.open(IO_WriteOnly))
  {
    term("Could not open file %s for writing\n",tokens.data());
  }
  p->tts.setDevice(&p->tf);
  p->tts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  p->tts << "<Tokens version=\"1.0\">" << endl;
}

void DocSets::finalize()
{
  if (!p->indentStack.top())
  {
    p->nts << p->indent() << " </Node>" << endl;
  }
  p->indentStack.pop();
  p->nts << "      </Subnodes>" << endl;
  p->nts << "    </Node>" << endl;
  p->nts << "  </TOC>" << endl;
  p->nts << "</DocSetNodes>" << endl;
  p->nf.close();

  p->tts << "</Tokens>" << endl;
  p->tf.close();
}

QCString DocSets::Private::indent()
{
  QCString result;
  result.fill(' ',(indentStack.size()+2)*2);
  return result;
}

void DocSets::incContentsDepth()
{
  //printf("DocSets::incContentsDepth() depth=%zu\n",p->indentStack.size());
  p->nts << p->indent() << "<Subnodes>" << endl;
  p->indentStack.push(true);
}

void DocSets::decContentsDepth()
{
  if (!p->indentStack.top())
  {
    p->nts << p->indent() << " </Node>" << endl;
  }
  p->nts << p->indent() << "</Subnodes>" << endl;
  p->indentStack.pop();
  //printf("DocSets::decContentsDepth() depth=%zu\n",p->indentStack.size());
}

void DocSets::addContentsItem(bool isDir,
                              const char *name,
                              const char *ref,
                              const char *file,
                              const char *anchor,
                              bool /* separateIndex */,
                              bool /* addToNavIndex */,
                              const Definition * /*def*/)
{
  (void)isDir;
  //printf("DocSets::addContentsItem(%s) depth=%zu\n",name,p->indentStack.size());
  if (ref==0)
  {
    if (!p->indentStack.top())
    {
      p->nts << p->indent() << " </Node>" << endl;
    }
    p->indentStack.top()=false;
    p->nts << p->indent() << " <Node>" << endl;
    p->nts << p->indent() << "  <Name>" << convertToXML(name) << "</Name>" << endl;
    if (file && file[0]=='^') // URL marker
    {
      p->nts << p->indent() << "  <URL>" << convertToXML(&file[1])
            << "</URL>" << endl;
    }
    else // relative file
    {
      p->nts << p->indent() << "  <Path>";
      if (file && file[0]=='!') // user specified file
      {
        p->nts << convertToXML(&file[1]);
      }
      else if (file) // doxygen generated file
      {
        p->nts << file << Doxygen::htmlFileExtension;
      }
      p->nts << "</Path>" << endl;
      if (file && anchor)
      {
        p->nts << p->indent() << "  <Anchor>" << anchor << "</Anchor>" << endl;
      }
    }
  }
}

void DocSets::addIndexItem(const Definition *context,const MemberDef *md,
                           const char *,const char *)
{
  if (md==0 && context==0) return;

  const FileDef *fd      = 0;
  const ClassDef *cd     = 0;
  const NamespaceDef *nd = 0;

  if (md)
  {
    fd = md->getFileDef();
    cd = md->getClassDef();
    nd = md->getNamespaceDef();
    if (!md->isLinkable()) return; // internal symbol
  }

  QCString scope;
  QCString type;
  QCString decl;

  // determine language
  QCString lang;
  SrcLangExt langExt = SrcLangExt_Cpp;
  if (md)
  {
    langExt = md->getLanguage();
  }
  else if (context)
  {
    langExt = context->getLanguage();
  }
  switch (langExt)
  {
    case SrcLangExt_Cpp:
    case SrcLangExt_ObjC:
      {
        if (md && (md->isObjCMethod() || md->isObjCProperty()))
          lang="occ";  // Objective C/C++
        else if (fd && fd->name().right(2).lower()==".c")
          lang="c";    // Plain C
        else if (cd==0 && nd==0)
          lang="c";    // Plain C symbol outside any class or namespace
        else
          lang="cpp";  // C++
      }
      break;
    case SrcLangExt_IDL:     lang="idl"; break;        // IDL
    case SrcLangExt_CSharp:  lang="csharp"; break;     // C#
    case SrcLangExt_PHP:     lang="php"; break;        // PHP4/5
    case SrcLangExt_D:       lang="d"; break;          // D
    case SrcLangExt_Java:    lang="java"; break;       // Java
    case SrcLangExt_JS:      lang="javascript"; break; // JavaScript
    case SrcLangExt_Python:  lang="python"; break;     // Python
    case SrcLangExt_Fortran: lang="fortran"; break;    // Fortran
    case SrcLangExt_VHDL:    lang="vhdl"; break;       // VHDL
    case SrcLangExt_XML:     lang="xml"; break;        // DBUS XML
    case SrcLangExt_SQL:     lang="sql"; break;        // Sql
    case SrcLangExt_Markdown:lang="markdown"; break;   // Markdown
    case SrcLangExt_Slice:   lang="slice"; break;      // Slice
    case SrcLangExt_Unparsed:lang="unparsed"; break;   // unparsed
    case SrcLangExt_Unknown: lang="unknown"; break;    // should not happen!
  }

  if (md)
  {
    if (context==0)
    {
      if (md->getGroupDef())
        context = md->getGroupDef();
      else if (md->getFileDef())
        context = md->getFileDef();
    }
    if (context==0) return; // should not happen

    switch (md->memberType())
    {
      case MemberType_Define:
        type="macro"; break;
      case MemberType_Function:
        if (cd && (cd->compoundType()==ClassDef::Interface ||
              cd->compoundType()==ClassDef::Class))
        {
          if (md->isStatic())
            type="clm";         // class member
          else
            type="instm";       // instance member
        }
        else if (cd && cd->compoundType()==ClassDef::Protocol)
        {
          if (md->isStatic())
            type="intfcm";     // interface class member
          else
            type="intfm";      // interface member
        }
        else
          type="func";
        break;
      case MemberType_Variable:
        type="data"; break;
      case MemberType_Typedef:
        type="tdef"; break;
      case MemberType_Enumeration:
        type="enum"; break;
      case MemberType_EnumValue:
        type="econst"; break;
        //case MemberDef::Prototype:
        //  type="prototype"; break;
      case MemberType_Signal:
        type="signal"; break;
      case MemberType_Slot:
        type="slot"; break;
      case MemberType_Friend:
        type="ffunc"; break;
      case MemberType_DCOP:
        type="dcop"; break;
      case MemberType_Property:
        if (cd && cd->compoundType()==ClassDef::Protocol)
          type="intfp";         // interface property
        else
          type="instp";         // instance property
        break;
      case MemberType_Event:
        type="event"; break;
      case MemberType_Interface:
        type="ifc"; break;
      case MemberType_Service:
        type="svc"; break;
      case MemberType_Sequence:
        type="sequence"; break;
      case MemberType_Dictionary:
        type="dictionary"; break;
    }
    cd = md->getClassDef();
    nd = md->getNamespaceDef();
    if (cd)
    {
      scope = cd->qualifiedName();
    }
    else if (nd)
    {
      scope = nd->name();
    }
    const MemberDef *declMd = md->memberDeclaration();
    if (declMd==0) declMd = md;
    {
      fd = md->getFileDef();
      if (fd)
      {
        decl = fd->name();
      }
    }
    writeToken(p->tts,md,type,lang,scope,md->anchor(),decl);
  }
  else if (context && context->isLinkable())
  {
    if (fd==0 && context->definitionType()==Definition::TypeFile)
    {
      fd = toFileDef(context);
    }
    if (cd==0 && context->definitionType()==Definition::TypeClass)
    {
      cd = toClassDef(context);
    }
    if (nd==0 && context->definitionType()==Definition::TypeNamespace)
    {
      nd = toNamespaceDef(context);
    }
    if (fd)
    {
      type="file";
    }
    else if (cd)
    {
      scope = cd->qualifiedName();
      if (cd->isTemplate())
      {
        type="tmplt";
      }
      else if (cd->compoundType()==ClassDef::Protocol)
      {
        type="intf";
        if (scope.right(2)=="-p") scope=scope.left(scope.length()-2);
      }
      else if (cd->compoundType()==ClassDef::Interface)
      {
        type="cl";
      }
      else if (cd->compoundType()==ClassDef::Category)
      {
        type="cat";
      }
      else
      {
        type = "cl";
      }
      const IncludeInfo *ii = cd->includeInfo();
      if (ii)
      {
        decl=ii->includeName;
      }
    }
    else if (nd)
    {
      scope = nd->name();
      type = "ns";
    }
    if (p->scopes.find(context->getOutputFileBase().str())==p->scopes.end())
    {
      writeToken(p->tts,context,type,lang,scope,0,decl);
      p->scopes.insert(context->getOutputFileBase().str());
    }
  }
}

void DocSets::writeToken(FTextStream &t,
                         const Definition *d,
                         const QCString &type,
                         const QCString &lang,
                         const char *scope,
                         const char *anchor,
                         const char *decl)
{
  t << "  <Token>" << endl;
  t << "    <TokenIdentifier>" << endl;
  QCString name = d->name();
  if (name.right(2)=="-p")  name=name.left(name.length()-2);
  t << "      <Name>" << convertToXML(name) << "</Name>" << endl;
  if (!lang.isEmpty())
  {
    t << "      <APILanguage>" << lang << "</APILanguage>" << endl;
  }
  if (!type.isEmpty())
  {
    t << "      <Type>" << type << "</Type>" << endl;
  }
  if (scope)
  {
    t << "      <Scope>" << convertToXML(scope) << "</Scope>" << endl;
  }
  t << "    </TokenIdentifier>" << endl;
  t << "    <Path>" << d->getOutputFileBase()
                    << Doxygen::htmlFileExtension << "</Path>" << endl;
  if (anchor)
  {
    t << "    <Anchor>" << anchor << "</Anchor>" << endl;
  }
  QCString tooltip = d->briefDescriptionAsTooltip();
  if (!tooltip.isEmpty())
  {
    t << "    <Abstract>" << convertToXML(tooltip) << "</Abstract>" << endl;
  }
  if (decl)
  {
    t << "    <DeclaredIn>" << convertToXML(decl) << "</DeclaredIn>" << endl;
  }
  t << "  </Token>" << endl;
}

void DocSets::addIndexFile(const char *name)
{
  (void)name;
}

