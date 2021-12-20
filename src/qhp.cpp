/*
 * Copyright (C) 2008 by Sebastian Pipping.
 * Copyright (C) 2008 Dimitri van Heesch.
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
 * Sebastian Pipping <sebastian@pipping.org>
 */

#include "qhp.h"
#include "qhpxmlwriter.h"
#include "message.h"
#include "config.h"
#include "memberdef.h"
#include "groupdef.h"
#include "doxygen.h"
#include "filedef.h"
#include "util.h"

#include <fstream>
#include <string.h>

static QCString makeFileName(const QCString & withoutExtension)
{
  QCString result=withoutExtension;
  if (!result.isEmpty())
  {
    if (result.at(0)=='!') // relative URL -> strip marker
    {
      result=result.mid(1);
    }
    else // add specified HTML extension
    {
      addHtmlExtensionIfMissing(result);
    }
  }
  return result;
}

static QCString makeRef(const QCString & withoutExtension, const QCString & anchor)
{
  //printf("QHP::makeRef(%s,%s)\n",withoutExtension,anchor);
  if (withoutExtension.isEmpty()) return QCString();
  QCString result = makeFileName(withoutExtension);
  if (anchor.isEmpty()) return result;
  return result+"#"+anchor;
}

Qhp::Qhp()
{
  m_doc.setIndentLevel(0);
  m_toc.setIndentLevel(2);
  m_index.setIndentLevel(2);
  m_files.setIndentLevel(2);
}

Qhp::~Qhp()
{
  clearPrevSection();
}

void Qhp::initialize()
{
  /*
  <QtHelpProject version="1.0">
    <namespace>mycompany.com.myapplication.1_0</namespace>
    <virtualFolder>doc</virtualFolder>
    <customFilter name="My Application 1.0">
      <filterAttribute>myapp</filterAttribute>
      <filterAttribute>1.0</filterAttribute>
    </customFilter>
    <filterSection>
      <filterAttribute>myapp</filterAttribute>
      <filterAttribute>1.0</filterAttribute>
  ..
  */
  QCString nameSpace       = Config_getString(QHP_NAMESPACE);
  QCString virtualFolder   = Config_getString(QHP_VIRTUAL_FOLDER);

  m_doc.declaration("1.0", "UTF-8");

  const char * rootAttributes[] =
  { "version", "1.0", 0 };

  m_doc.open("QtHelpProject", rootAttributes);
  m_doc.openCloseContent("namespace", nameSpace);
  m_doc.openCloseContent("virtualFolder", virtualFolder);

  // Add custom filter
  QCString filterName = Config_getString(QHP_CUST_FILTER_NAME);
  if (!filterName.isEmpty())
  {
    const char * tagAttributes[] =
    { "name", filterName.data(), 0 };
    m_doc.open("customFilter", tagAttributes);

    StringVector customFilterAttributes =
        split(Config_getString(QHP_CUST_FILTER_ATTRS).str(), " ");
    for (const auto &attr : customFilterAttributes)
    {
      m_doc.openCloseContent("filterAttribute", attr.c_str());
    }
    m_doc.close("customFilter");
  }

  m_doc.open("filterSection");

  // Add section attributes
  StringVector sectionFilterAttributes =
      split(Config_getString(QHP_SECT_FILTER_ATTRS).str(), " ");
  if (std::find(sectionFilterAttributes.begin(), sectionFilterAttributes.end(), "doxygen") ==
      sectionFilterAttributes.end())
  {
    sectionFilterAttributes.push_back("doxygen");
  }
  for (const auto &attr : sectionFilterAttributes)
  {
    m_doc.openCloseContent("filterAttribute", attr.c_str());
  }

  m_toc.open("toc");

  // Add extra root node
  QCString fullProjectname = getFullProjectName();
  QCString indexFile = "index"+Doxygen::htmlFileExtension;
  const char * const attributes[] =
  { "title", fullProjectname.data(),
    "ref",   indexFile.data(),
    NULL
  };
  m_toc.open("section", attributes);
  m_openCount=1;
  m_prevSectionTitle = getFullProjectName();
  m_prevSectionLevel = 1;
  m_sectionLevel = 1;

  m_index.open("keywords");
  m_files.open("files");
}

void Qhp::finalize()
{
  // Finish TOC
  handlePrevSection();
  while (m_openCount>0)
  {
    m_toc.close("section");
    m_openCount--;
  }
  m_toc.close("toc");
  m_doc.insert(m_toc);

  // Finish index
  m_index.close("keywords");
  m_doc.insert(m_index);

  // Finish files
  m_files.close("files");
  m_doc.insert(m_files);

  m_doc.close("filterSection");
  m_doc.close("QtHelpProject");

  QCString fileName = Config_getString(HTML_OUTPUT) + "/" + getQhpFileName();
  std::ofstream file(fileName.str(),std::ofstream::out | std::ofstream::binary);
  if (!file.is_open())
  {
    term("Could not open file %s for writing\n", fileName.data());
  }
  TextStream t(&file);
  m_doc.dumpTo(t);
}

void Qhp::incContentsDepth()
{
  m_sectionLevel++;
}

void Qhp::decContentsDepth()
{
  if (m_sectionLevel<=0 || (m_sectionLevel==1 && m_skipMainPageSection))
  {
    m_skipMainPageSection=FALSE;
    return;
  }
  m_sectionLevel--;
}

void Qhp::addContentsItem(bool /*isDir*/, const QCString & name,
                          const QCString & /*ref*/, const QCString & file,
                          const QCString &anchor, bool /* separateIndex */,
                          bool /* addToNavIndex */,
                          const Definition * /*def*/)
{
  //printf("Qhp::addContentsItem(%s) %d\n",name,m_sectionLevel);
  // Backup difference before modification

  QCString f = file;
  if (!f.isEmpty() && f.at(0)=='^') return; // absolute URL not supported

  int diff = m_prevSectionLevel - m_sectionLevel;

  handlePrevSection();
  setPrevSection(name, f, anchor, m_sectionLevel);

  // Close sections as needed
  //printf("Qhp::addContentsItem() closing %d sections\n",diff);
  while (diff>0)
  {
    m_toc.close("section");
    m_openCount--;
    diff--;
  }
}

void Qhp::addIndexItem(const Definition *context,const MemberDef *md,
                       const QCString &sectionAnchor,const QCString &word)
{
  (void)word;
  //printf("addIndexItem(%s %s %s\n",
  //       context?context->name().data():"<none>",
  //       md?md->name().data():"<none>",
  //       qPrint(word));

  if (md) // member
  {
    static bool separateMemberPages = Config_getBool(SEPARATE_MEMBER_PAGES);
    if (context==0) // global member
    {
      if (md->getGroupDef())
        context = md->getGroupDef();
      else if (md->getFileDef())
        context = md->getFileDef();
    }
    if (context==0) return; // should not happen
    QCString cfname  = md->getOutputFileBase();
    QCString cfiname = context->getOutputFileBase();
    QCString level1  = context->name();
    QCString level2  = !word.isEmpty() ? word : md->name();
    QCString contRef = separateMemberPages ? cfname : cfiname;
    QCString anchor  = !sectionAnchor.isEmpty() ? sectionAnchor : md->anchor();

    QCString ref;

    // <keyword name="foo" id="MyApplication::foo" ref="doc.html#foo"/>
    ref = makeRef(contRef, anchor);
    QCString id = level1+"::"+level2;
    const char * attributes[] =
    {
      "name", level2.data(),
      "id",   id.data(),
      "ref",  ref.data(),
      0
    };
    m_index.openClose("keyword", attributes);
  }
  else if (context) // container
  {
    // <keyword name="Foo" id="Foo" ref="doc.html#Foo"/>
    QCString contRef = context->getOutputFileBase();
    QCString level1  = !word.isEmpty() ? word : context->name();
    QCString ref = makeRef(contRef,sectionAnchor);
    const char * attributes[] =
    {
      "name", level1.data(),
      "id",   level1.data(),
      "ref",  ref.data(),
      0
    };
    m_index.openClose("keyword", attributes);
  }
}

void Qhp::addIndexFile(const QCString & name)
{
  addFile(name);
}

QCString Qhp::getQhpFileName()
{
  return "index.qhp";
}

QCString Qhp::getFullProjectName()
{
  QCString projectName = Config_getString(PROJECT_NAME);
  QCString versionText = Config_getString(PROJECT_NUMBER);
  if (projectName.isEmpty()) projectName="Root";
  return projectName + (versionText.isEmpty()
      ? QCString("")
      : QCString(" ") + versionText);
}

void Qhp::handlePrevSection()
{
  /*
  <toc>
    <section title="My Application Manual" ref="index.html">
      <section title="Chapter 1" ref="doc.html#chapter1"/>
      <section title="Chapter 2" ref="doc.html#chapter2"/>
      <section title="Chapter 3" ref="doc.html#chapter3"/>
    </section>
  </toc>
  */

  if (m_prevSectionTitle.isNull())
  {
    m_prevSectionTitle=" "; // should not happen...
  }

  // We skip "Main Page" as our extra root is pointing to that
  if (!((m_prevSectionLevel==1) && (m_prevSectionTitle==getFullProjectName())))
  {
    QCString finalRef = makeRef(m_prevSectionBaseName, m_prevSectionAnchor);

    const char * const attributes[] =
    { "title", m_prevSectionTitle.data(),
      "ref",   finalRef.data(),
      NULL
    };

    if (m_prevSectionLevel < m_sectionLevel)
    {
      // Section with children
      m_toc.open("section", attributes);
      m_openCount++;
    }
    else
    {
      // Section without children
      m_toc.openClose("section", attributes);
    }
  }
  else
  {
    m_skipMainPageSection=TRUE;
  }

  clearPrevSection();
}

void Qhp::setPrevSection(const QCString & title, const QCString & basename, const QCString & anchor, int level)
{
  m_prevSectionTitle = title;
  m_prevSectionBaseName = basename;
  m_prevSectionAnchor = anchor;
  m_prevSectionLevel = level;
}

void Qhp::clearPrevSection()
{
  m_prevSectionTitle.resize(0);
  m_prevSectionBaseName.resize(0);
  m_prevSectionAnchor.resize(0);
}

void Qhp::addFile(const QCString & fileName)
{
  m_files.openCloseContent("file", fileName);
}

void Qhp::addImageFile(const QCString &fileName)
{
  addFile(fileName);
}

void Qhp::addStyleSheetFile(const QCString &fileName)
{
  addFile(fileName);
}

