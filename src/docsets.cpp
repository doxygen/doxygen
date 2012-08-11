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

#include <qfile.h>
#include "docsets.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"

DocSets::DocSets() : m_nodes(17), m_scopes(17)
{
  m_nf = 0;
  m_tf = 0;
  m_dc = 0;
  m_id = 0;
  m_nodes.setAutoDelete(TRUE);
}

DocSets::~DocSets()
{
  delete m_nf;
  delete m_tf;
}

void DocSets::initialize()
{
  // -- get config options
  QCString projectName = Config_getString("PROJECT_NAME");
  if (projectName.isEmpty()) projectName="root";
  QCString bundleId = Config_getString("DOCSET_BUNDLE_ID");
  if (bundleId.isEmpty()) bundleId="org.doxygen.Project";
  QCString feedName = Config_getString("DOCSET_FEEDNAME");
  if (feedName.isEmpty()) feedName="FeedName";
  QCString publisherId = Config_getString("DOCSET_PUBLISHER_ID");
  if (publisherId.isEmpty()) publisherId="PublisherId";
  QCString publisherName = Config_getString("DOCSET_PUBLISHER_NAME");
  if (publisherName.isEmpty()) publisherName="PublisherName";
  QCString projectNumber = Config_getString("PROJECT_NUMBER");
  if (projectNumber.isEmpty()) projectNumber="ProjectNumber";

  // -- write Makefile
  {
  QCString mfName = Config_getString("HTML_OUTPUT") + "/Makefile";
  QFile makefile(mfName);
  if (!makefile.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",mfName.data());
    exit(1);
  }
  FTextStream ts(&makefile);

  ts << "DOCSET_NAME=" << bundleId << ".docset\n" 
        "DOCSET_CONTENTS=$(DOCSET_NAME)/Contents\n"
        "DOCSET_RESOURCES=$(DOCSET_CONTENTS)/Resources\n"
        "DOCSET_DOCUMENTS=$(DOCSET_RESOURCES)/Documents\n"
        "DESTDIR=~/Library/Developer/Shared/Documentation/DocSets\n"
        "XCODE_INSTALL=$(shell xcode-select -print-path)\n"
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
  QCString plName = Config_getString("HTML_OUTPUT") + "/Info.plist";
  QFile plist(plName);
  if (!plist.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",plName.data());
    exit(1);
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
        "</dict>\n"
        "</plist>\n";
  }

  // -- start Nodes.xml
  QCString notes = Config_getString("HTML_OUTPUT") + "/Nodes.xml";
  m_nf = new QFile(notes);
  if (!m_nf->open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",notes.data());
    exit(1);
  }
  //QCString indexName=Config_getBool("GENERATE_TREEVIEW")?"main":"index";
  QCString indexName="index";
  m_nts.setDevice(m_nf);
  m_nts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  m_nts << "<DocSetNodes version=\"1.0\">" << endl;
  m_nts << "  <TOC>" << endl;
  m_nts << "    <Node>" << endl;
  m_nts << "      <Name>Root</Name>" << endl;
  m_nts << "      <Path>" << indexName << Doxygen::htmlFileExtension << "</Path>" << endl;
  m_nts << "      <Subnodes>" << endl;
  m_dc = 1;
  m_firstNode.resize(m_dc);
  m_firstNode.at(0)=TRUE;

  QCString tokens = Config_getString("HTML_OUTPUT") + "/Tokens.xml";
  m_tf = new QFile(tokens);
  if (!m_tf->open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",tokens.data());
    exit(1);
  }
  m_tts.setDevice(m_tf);
  m_tts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  m_tts << "<Tokens version=\"1.0\">" << endl;
}

void DocSets::finalize()
{
  if (!m_firstNode.at(m_dc-1))
  {
    m_nts << indent() << " </Node>" << endl;
  }
  m_dc--;
  m_nts << "      </Subnodes>" << endl;
  m_nts << "    </Node>" << endl;
  m_nts << "  </TOC>" << endl;
  m_nts << "</DocSetNodes>" << endl;
  m_nf->close();
  delete m_nf;
  m_nf=0;

  m_tts << "</Tokens>" << endl;
  m_tf->close();
  delete m_tf;
  m_tf=0;
}

QCString DocSets::indent()
{
  QCString result;
  result.fill(' ',(m_dc+2)*2);
  return result;
}

void DocSets::incContentsDepth()
{
  //printf("DocSets::incContentsDepth() m_dc=%d\n",m_dc);
  ++m_dc;
  m_nts << indent() << "<Subnodes>" << endl;
  m_firstNode.resize(m_dc);
  if (m_dc>0)
  {
    m_firstNode.at(m_dc-1)=TRUE;
  }
}

void DocSets::decContentsDepth()
{
  if (!m_firstNode.at(m_dc-1))
  {
    m_nts << indent() << " </Node>" << endl;
  }
  m_nts << indent() << "</Subnodes>" << endl;
  --m_dc;
  //printf("DocSets::decContentsDepth() m_dc=%d\n",m_dc);
}

void DocSets::addContentsItem(bool isDir,
                              const char *name, 
                              const char *ref, 
                              const char *file,
                              const char *anchor,
                              bool /* separateIndex */,
                              bool /* addToNavIndex */,
                              Definition * /*def*/)
{
  (void)isDir;
  //printf("DocSets::addContentsItem(%s) m_dc=%d\n",name,m_dc);
  if (ref==0)
  {
    if (!m_firstNode.at(m_dc-1))
    {
      m_nts << indent() << " </Node>" << endl;
    }
    m_firstNode.at(m_dc-1)=FALSE;
    m_nts << indent() << " <Node>" << endl;
    m_nts << indent() << "  <Name>" << convertToXML(name) << "</Name>" << endl;
    if (file && file[0]=='^') // URL marker
    {
      m_nts << indent() << "  <URL>" << convertToXML(&file[1]) 
            << "</URL>" << endl;
    }
    else // relative file
    {
      m_nts << indent() << "  <Path>";
      if (file && file[0]=='!') // user specified file
      {
        m_nts << convertToXML(&file[1]);
      }
      else if (file) // doxygen generated file
      {
        m_nts << file << Doxygen::htmlFileExtension;
      }
      m_nts << "</Path>" << endl;
      if (file && anchor)
      {
        m_nts << indent() << "  <Anchor>" << anchor << "</Anchor>" << endl;
      }
    }
  }
}

void DocSets::addIndexItem(Definition *context,MemberDef *md,const char *)
{
  if (md==0 && context==0) return;

  FileDef *fd      = 0;
  ClassDef *cd     = 0;
  NamespaceDef *nd = 0;

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
    case SrcLangExt_JS:      lang="javascript"; break; // Javascript
    case SrcLangExt_Python:  lang="python"; break;     // Python
    case SrcLangExt_Fortran: lang="fortran"; break;    // Fortran
    case SrcLangExt_VHDL:    lang="vhdl"; break;       // VHDL
    case SrcLangExt_XML:     lang="xml"; break;        // DBUS XML
    case SrcLangExt_Tcl:     lang="tcl"; break;        // Tcl
    case SrcLangExt_Markdown:lang="markdown"; break;   // Markdown
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
      case MemberDef::Define:
        type="macro"; break;
      case MemberDef::Function:
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
      case MemberDef::Variable:
        type="data"; break;
      case MemberDef::Typedef:
        type="tdef"; break;
      case MemberDef::Enumeration:
        type="enum"; break;
      case MemberDef::EnumValue:
        type="econst"; break;
        //case MemberDef::Prototype:
        //  type="prototype"; break;
      case MemberDef::Signal:
        type="signal"; break;
      case MemberDef::Slot:
        type="slot"; break;
      case MemberDef::Friend:
        type="ffunc"; break;
      case MemberDef::DCOP:
        type="dcop"; break;
      case MemberDef::Property:
        if (cd && cd->compoundType()==ClassDef::Protocol) 
          type="intfp";         // interface property
        else 
          type="instp";         // instance property
        break;
      case MemberDef::Event:
        type="event"; break;
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
    MemberDef *declMd = md->memberDeclaration();
    if (declMd==0) declMd = md;
    {
      fd = md->getFileDef();
      if (fd)
      {
        decl = fd->name();
      }
    }
    writeToken(m_tts,md,type,lang,scope,md->anchor(),decl);
  }
  else if (context && context->isLinkable())
  {
    if (fd==0 && context->definitionType()==Definition::TypeFile)
    {
      fd = (FileDef*)context;
    }
    if (cd==0 && context->definitionType()==Definition::TypeClass)
    {
      cd = (ClassDef*)context;
    }
    if (nd==0 && context->definitionType()==Definition::TypeNamespace)
    {
      nd = (NamespaceDef*)context;
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
      IncludeInfo *ii = cd->includeInfo();
      if (ii)
      {
        decl=ii->includeName;
        if (decl.isEmpty())
        {
          decl=ii->local;
        }
      }
    }
    else if (nd)
    {
      scope = nd->name();
      type = "ns";
    }
    if (m_scopes.find(context->getOutputFileBase())==0)
    {
      writeToken(m_tts,context,type,lang,scope,0,decl);
      m_scopes.append(context->getOutputFileBase(),(void*)0x8);
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

