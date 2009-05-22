/******************************************************************************
 *
 * Copyright (C) 1997-2008 by Dimitri van Heesch.
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

#include "docsets.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include <qfile.h>


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

  // -- write Makefile
  {
  QCString mfName = Config_getString("HTML_OUTPUT") + "/Makefile";
  QFile makefile(mfName);
  if (!makefile.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",mfName.data());
    exit(1);
  }
  QTextStream ts(&makefile);
  ts.setEncoding(QTextStream::UnicodeUTF8);

  ts << "DOCSET_NAME=" << bundleId << ".docset\n" 
        "DOCSET_CONTENTS=$(DOCSET_NAME)/Contents\n"
        "DOCSET_RESOURCES=$(DOCSET_CONTENTS)/Resources\n"
        "DOCSET_DOCUMENTS=$(DOCSET_RESOURCES)/Documents\n"
        "DOCSET_INSTALL=~/Library/Developer/Shared/Documentation/DocSets\n"
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
        "install: docset\n"
        "\tmkdir -p $(DOCSET_INSTALL)\n"
        "\tcp -R $(DOCSET_NAME) $(DOCSET_INSTALL)\n"
        "\n"
        "uninstall:\n"
        "\trm -rf $(DOCSET_INSTALL)\n"
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
  QTextStream ts(&plist);
  ts.setEncoding(QTextStream::UnicodeUTF8);

  ts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" 
        "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\"\n" 
        "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n" 
        "<plist version=\"1.0\">\n" 
        "<dict>\n" 
        "     <key>CFBundleName</key>\n" 
        "     <string>" << projectName << "</string>\n" 
        "     <key>CFBundleIdentifier</key>\n"
        "     <string>" << bundleId << ".docset</string>\n" 
        "     <key>DocSetFeedName</key>\n" 
        "     <string>" << feedName << "</string>\n"
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
  QCString indexName=usingTreeIndex()?"main":"index";
  m_nts.setDevice(m_nf);
  m_nts.setEncoding(QTextStream::UnicodeUTF8);
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
  m_tts.setEncoding(QTextStream::UnicodeUTF8);
  m_tts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  m_tts << "<Tokens version=\"1.0\">" << endl;
}

void DocSets::finalize()
{
  m_nts << indent() << " </Node>" << endl;
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
}

void DocSets::addContentsItem(bool isDir,
                              const char *name, 
                              const char *ref, 
                              const char *file,
                              const char *anchor)
{
  (void)isDir;
  if (file && ref==0)
  {
    if (!m_firstNode.at(m_dc-1))
    {
      m_nts << indent() << " </Node>" << endl;
    }
    m_firstNode.at(m_dc-1)=FALSE;
    m_nts << indent() << " <Node>" << endl;
    m_nts << indent() << "  <Name>" << convertToXML(name) << "</Name>" << endl;
    m_nts << indent() << "  <Path>" << file << Doxygen::htmlFileExtension;
    if (anchor)
    {
      m_nts << "#" << anchor;
    }
    m_nts << "</Path>" << endl;
  }
}

void DocSets::addIndexItem(const char *, const char *, 
                           const char *, const char *,
                           const char *,const MemberDef *md)
{
  if (!md->isLinkable()) return; // internal symbol

  ClassDef *cd     = md->getClassDef();
  NamespaceDef *nd = md->getNamespaceDef();
  FileDef  *fd     = md->getFileDef();

  // determine language
  QCString lang;
  SrcLangExt langExt = SrcLangExt_Cpp;
  if (fd) langExt = getLanguageFromFileName(fd->name());
  switch (langExt)
  {
    case SrcLangExt_Cpp:
      {
        if (md->isObjCMethod())                        
          lang="occ";  // Objective C/C++
        else if (fd && fd->name().right(2).lower()==".c") 
          lang="c";    // Plain C
        else                                      
          lang="cpp";  // C++
      }
      break;
    case SrcLangExt_ObjC:   lang="occ"; break;        // Objective C++
    case SrcLangExt_IDL:    lang="idl"; break;        // IDL
    case SrcLangExt_CSharp: lang="csharp"; break;     // C#
    case SrcLangExt_PHP:    lang="php"; break;        // PHP4/5
    case SrcLangExt_D:      lang="d"; break;          // D
    case SrcLangExt_Java:   lang="java"; break;       // Java
    case SrcLangExt_JS:     lang="javascript"; break; // Javascript
    case SrcLangExt_Python: lang="python"; break;     // Python
    case SrcLangExt_F90:    lang="fortran"; break;    // Fortran
    case SrcLangExt_VHDL:   lang="vhdl"; break;       // VHDL
  }

  // determine scope
  QCString scope;
  QCString type;
  QCString decl;
  Definition *d = 0;
  if (fd && fd->isLinkable() && m_scopes.find(fd->getOutputFileBase())==0)
  {
    writeToken(m_tts,fd,"file",lang,0,0,0);
    m_scopes.append(fd->getOutputFileBase(),(void*)0x8);
  }
  if (cd) 
  {
    scope = cd->qualifiedName();
    if (cd->isTemplate()) 
      type="tmplt";
    else if (cd->compoundType()==ClassDef::Protocol) 
      type="intf";
    else if (cd->compoundType()==ClassDef::Interface) 
      type="cl";
    else if (cd->compoundType()==ClassDef::Category)
      type="cat";
    else 
      type = "cl";
    d = cd;
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
    d = cd;
  }
  if (d && d->isLinkable() && m_scopes.find(d->getOutputFileBase())==0)
  {
    writeToken(m_tts,d,type,lang,0,0,decl);
    m_scopes.append(d->getOutputFileBase(),(void*)0x8);
  }

  switch (md->memberType())
  {
    case MemberDef::Define:
      type="macro"; break;
    case MemberDef::Function:
      if (cd && cd->compoundType()==ClassDef::Interface) 
        type="intfm";
      else if (cd && cd->compoundType()==ClassDef::Class) 
        type="clm"; 
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
      type="property"; break;
    case MemberDef::Event:
      type="event"; break;
  }
  writeToken(m_tts,md,type,lang,scope,md->anchor());
}

void DocSets::writeToken(QTextStream &t,
                         const Definition *d,
                         const QCString &type,
                         const QCString &lang,
                         const char *scope,
                         const char *anchor,
                         const char *decl)
{
  t << "  <Token>" << endl;
  t << "    <TokenIdentifier>" << endl;
  QString name = d->name();
  if (name.right(2)=="-p")  name=name.left(name.length()-2);
  t << "      <Name>" << convertToXML(name) << "</Name>" << endl;
  t << "      <APILanguage>" << lang << "</APILanguage>" << endl;
  t << "      <Type>" << type << "</Type>" << endl;
  t << "      <Scope>" << convertToXML(scope) << "</Scope>" << endl;
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
    t << "    <Abstract>" << tooltip << "</Abstract>" << endl;
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

