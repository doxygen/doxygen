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

#include <unordered_set>
#include <stack>

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
#include "textstream.h"
#include "portable.h"

struct DocSets::Private
{
  QCString indent();
  std::ofstream ntf;
  TextStream    nts;
  std::ofstream ttf;
  TextStream    tts;
  std::stack<bool> indentStack;
  std::unordered_set<std::string> scopes;
};


DocSets::DocSets() : p(std::make_unique<Private>()) {}
DocSets::~DocSets() = default;

void DocSets::initialize()
{
  // -- get config options
  QCString projectName = Config_getString(PROJECT_NAME);
  if (projectName.isEmpty()) projectName="root";
  QCString bundleId = Config_getString(DOCSET_BUNDLE_ID);
  if (bundleId.isEmpty()) bundleId="org.doxygen.Project";
  QCString feedName = Config_getString(DOCSET_FEEDNAME);
  if (feedName.isEmpty()) feedName="FeedName";
  QCString feedURL = Config_getString(DOCSET_FEEDURL);
  if (feedURL.isEmpty()) feedURL="FeedUrl";
  QCString publisherId = Config_getString(DOCSET_PUBLISHER_ID);
  if (publisherId.isEmpty()) publisherId="PublisherId";
  QCString publisherName = Config_getString(DOCSET_PUBLISHER_NAME);
  if (publisherName.isEmpty()) publisherName="PublisherName";
  QCString projectNumber = Config_getString(PROJECT_NUMBER);
  if (projectNumber.isEmpty()) projectNumber="ProjectNumber";

  // -- write Makefile
  {
    QCString mfName = Config_getString(HTML_OUTPUT) + "/Makefile";
    std::ofstream ts = Portable::openOutputStream(mfName);
    if (!ts.is_open())
    {
      term("Could not open file %s for writing\n",qPrint(mfName));
    }

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
    std::ofstream ts = Portable::openOutputStream(plName);
    if (!ts.is_open())
    {
      term("Could not open file %s for writing\n",qPrint(plName));
    }

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
          "     <key>DocSetFeedUrl</key>\n"
          "     <string>" << feedURL << "</string>\n"
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
  p->ntf = Portable::openOutputStream(notes);
  if (!p->ntf.is_open())
  {
    term("Could not open file %s for writing\n",qPrint(notes));
  }
  p->nts.setStream(&p->ntf);
  //QCString indexName=Config_getBool(GENERATE_TREEVIEW)?"main":"index";
  QCString indexName="index";
  p->nts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  p->nts << "<DocSetNodes version=\"1.0\">\n";
  p->nts << "  <TOC>\n";
  p->nts << "    <Node>\n";
  p->nts << "      <Name>Root</Name>\n";
  p->nts << "      <Path>" << indexName << Doxygen::htmlFileExtension << "</Path>\n";
  p->nts << "      <Subnodes>\n";
  p->indentStack.push(true);

  QCString tokens = Config_getString(HTML_OUTPUT) + "/Tokens.xml";
  p->ttf = Portable::openOutputStream(tokens);
  if (!p->ttf.is_open())
  {
    term("Could not open file %s for writing\n",qPrint(tokens));
  }
  p->tts.setStream(&p->ttf);
  p->tts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  p->tts << "<Tokens version=\"1.0\">\n";
}

void DocSets::finalize()
{
  if (!p->indentStack.top())
  {
    p->nts << p->indent() << " </Node>\n";
  }
  p->indentStack.pop();
  p->nts << "      </Subnodes>\n";
  p->nts << "    </Node>\n";
  p->nts << "  </TOC>\n";
  p->nts << "</DocSetNodes>\n";
  p->nts.flush();
  p->ntf.close();

  p->tts << "</Tokens>\n";
  p->tts.flush();
  p->ttf.close();
}

QCString DocSets::Private::indent()
{
  QCString result;
  result.fill(' ',static_cast<int>(indentStack.size()+2)*2);
  return result;
}

void DocSets::incContentsDepth()
{
  //printf("DocSets::incContentsDepth() depth=%zu\n",p->indentStack.size());
  p->nts << p->indent() << "<Subnodes>\n";
  p->indentStack.push(true);
}

void DocSets::decContentsDepth()
{
  if (!p->indentStack.top())
  {
    p->nts << p->indent() << " </Node>\n";
  }
  p->nts << p->indent() << "</Subnodes>\n";
  p->indentStack.pop();
  //printf("DocSets::decContentsDepth() depth=%zu\n",p->indentStack.size());
}

void DocSets::addContentsItem(bool isDir,
                              const QCString &name,
                              const QCString &ref,
                              const QCString &file,
                              const QCString &anchor,
                              bool /* separateIndex */,
                              bool /* addToNavIndex */,
                              const Definition * /*def*/)
{
  (void)isDir;
  //printf("DocSets::addContentsItem(%s) depth=%zu\n",name,p->indentStack.size());
  if (ref==nullptr)
  {
    if (!p->indentStack.top())
    {
      p->nts << p->indent() << " </Node>\n";
    }
    p->indentStack.top()=false;
    p->nts << p->indent() << " <Node>\n";
    p->nts << p->indent() << "  <Name>" << convertToXML(name) << "</Name>\n";
    if (!file.isEmpty() && file[0]=='^') // URL marker
    {
      p->nts << p->indent() << "  <URL>" << convertToXML(&file[1])
            << "</URL>\n";
    }
    else // relative file
    {
      p->nts << p->indent() << "  <Path>";
      if (!file.isEmpty() && file[0]=='!') // user specified file
      {
        p->nts << convertToXML(&file[1]);
      }
      else if (!file.isEmpty()) // doxygen generated file
      {
        QCString fn = file;
        addHtmlExtensionIfMissing(fn);
        p->nts << fn;
      }
      p->nts << "</Path>\n";
      if (!file.isEmpty() && !anchor.isEmpty())
      {
        p->nts << p->indent() << "  <Anchor>" << anchor << "</Anchor>\n";
      }
    }
  }
}

void DocSets::addIndexItem(const Definition *context,const MemberDef *md,
                           const QCString &,const QCString &)
{
  if (md==nullptr && context==nullptr) return;

  const FileDef *fd      = nullptr;
  const ClassDef *cd     = nullptr;
  const NamespaceDef *nd = nullptr;

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
  SrcLangExt langExt = SrcLangExt::Cpp;
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
    case SrcLangExt::Cpp:
    case SrcLangExt::ObjC:
      {
        if (md && (md->isObjCMethod() || md->isObjCProperty()))
          lang="occ";  // Objective C/C++
        else if (fd && fd->name().lower().endsWith(".c"))
          lang="c";    // Plain C
        else if (cd==nullptr && nd==nullptr)
          lang="c";    // Plain C symbol outside any class or namespace
        else
          lang="cpp";  // C++
      }
      break;
    case SrcLangExt::IDL:     lang="idl"; break;        // IDL
    case SrcLangExt::CSharp:  lang="csharp"; break;     // C#
    case SrcLangExt::PHP:     lang="php"; break;        // PHP4/5
    case SrcLangExt::D:       lang="d"; break;          // D
    case SrcLangExt::Java:    lang="java"; break;       // Java
    case SrcLangExt::JS:      lang="javascript"; break; // JavaScript
    case SrcLangExt::Python:  lang="python"; break;     // Python
    case SrcLangExt::Fortran: lang="fortran"; break;    // Fortran
    case SrcLangExt::VHDL:    lang="vhdl"; break;       // VHDL
    case SrcLangExt::XML:     lang="xml"; break;        // DBUS XML
    case SrcLangExt::SQL:     lang="sql"; break;        // Sql
    case SrcLangExt::Markdown:lang="markdown"; break;   // Markdown
    case SrcLangExt::Slice:   lang="slice"; break;      // Slice
    case SrcLangExt::Lex:     lang="lex"; break;        // Lex
    case SrcLangExt::Unknown: lang="unknown"; break;    // should not happen!
  }

  if (context && md)
  {
    switch (md->memberType())
    {
      case MemberType::Define:
        type="macro"; break;
      case MemberType::Function:
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
      case MemberType::Variable:
        type="data"; break;
      case MemberType::Typedef:
        type="tdef"; break;
      case MemberType::Enumeration:
        type="enum"; break;
      case MemberType::EnumValue:
        type="econst"; break;
        //case MemberDef::Prototype:
        //  type="prototype"; break;
      case MemberType::Signal:
        type="signal"; break;
      case MemberType::Slot:
        type="slot"; break;
      case MemberType::Friend:
        type="ffunc"; break;
      case MemberType::DCOP:
        type="dcop"; break;
      case MemberType::Property:
        if (cd && cd->compoundType()==ClassDef::Protocol)
          type="intfp";         // interface property
        else
          type="instp";         // instance property
        break;
      case MemberType::Event:
        type="event"; break;
      case MemberType::Interface:
        type="ifc"; break;
      case MemberType::Service:
        type="svc"; break;
      case MemberType::Sequence:
        type="sequence"; break;
      case MemberType::Dictionary:
        type="dictionary"; break;
    }
    scope = md->getScopeString();
    fd = md->getFileDef();
    if (fd)
    {
      decl = fd->name();
    }
    writeToken(p->tts,md,type,lang,scope,md->anchor(),decl);
  }
  else if (context && context->isLinkable())
  {
    if (fd==nullptr && context->definitionType()==Definition::TypeFile)
    {
      fd = toFileDef(context);
    }
    if (cd==nullptr && context->definitionType()==Definition::TypeClass)
    {
      cd = toClassDef(context);
    }
    if (nd==nullptr && context->definitionType()==Definition::TypeNamespace)
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
        if (scope.endsWith("-p")) scope=scope.left(scope.length()-2);
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
      writeToken(p->tts,context,type,lang,scope,QCString(),decl);
      p->scopes.insert(context->getOutputFileBase().str());
    }
  }
}

void DocSets::writeToken(TextStream &t,
                         const Definition *d,
                         const QCString &type,
                         const QCString &lang,
                         const QCString &scope,
                         const QCString &anchor,
                         const QCString &decl)
{
  t << "  <Token>\n";
  t << "    <TokenIdentifier>\n";
  QCString name = d->name();
  if (name.endsWith("-p"))  name=name.left(name.length()-2);
  t << "      <Name>" << convertToXML(name) << "</Name>\n";
  if (!lang.isEmpty())
  {
    t << "      <APILanguage>" << lang << "</APILanguage>\n";
  }
  if (!type.isEmpty())
  {
    t << "      <Type>" << type << "</Type>\n";
  }
  if (!scope.isEmpty())
  {
    t << "      <Scope>" << convertToXML(scope) << "</Scope>\n";
  }
  t << "    </TokenIdentifier>\n";
  QCString fn = d->getOutputFileBase();
  addHtmlExtensionIfMissing(fn);
  t << "    <Path>" << fn << "</Path>\n";
  if (!anchor.isEmpty())
  {
    t << "    <Anchor>" << anchor << "</Anchor>\n";
  }
  QCString tooltip = d->briefDescriptionAsTooltip();
  if (!tooltip.isEmpty())
  {
    t << "    <Abstract>" << convertToXML(tooltip) << "</Abstract>\n";
  }
  if (!decl.isEmpty())
  {
    t << "    <DeclaredIn>" << convertToXML(decl) << "</DeclaredIn>\n";
  }
  t << "  </Token>\n";
}

void DocSets::addIndexFile(const QCString &name)
{
  (void)name;
}

