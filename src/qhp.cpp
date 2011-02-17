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
#include "filedef.h"

#include <qstringlist.h>
#include <string.h>

static QCString makeFileName(const char * withoutExtension)
{
  if (!withoutExtension) return QCString();
  return QCString(withoutExtension)+".html";
}

static QCString makeRef(const char * withoutExtension, const char * anchor)
{
  if (!withoutExtension) return QCString(); 
  QCString result = makeFileName(withoutExtension);
  if (!anchor) return result;
  return result+"#"+anchor;
}

Qhp::Qhp() : m_prevSectionLevel(0), m_sectionLevel(0)
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
  QCString nameSpace       = Config_getString("QHP_NAMESPACE");
  QCString virtualFolder   = Config_getString("QHP_VIRTUAL_FOLDER");

  const char * rootAttributes[] =
  { "version", "1.0", 0 };

  m_doc.open("QtHelpProject", rootAttributes);
  m_doc.openCloseContent("namespace", nameSpace);
  m_doc.openCloseContent("virtualFolder", virtualFolder);

  // Add custom filter
  QCString filterName = Config_getString("QHP_CUST_FILTER_NAME");
  if (!filterName.isEmpty())
  {
    const char * tagAttributes[] = 
    { "name", filterName, 0 };
    m_doc.open("customFilter", tagAttributes);

    QStringList customFilterAttributes = QStringList::split(QChar(' '), Config_getString("QHP_CUST_FILTER_ATTRS"));
    for (int i = 0; i < (int)customFilterAttributes.count(); i++)
    {
      m_doc.openCloseContent("filterAttribute", customFilterAttributes[i]);
    }
    m_doc.close("customFilter");
  }

  m_doc.open("filterSection");

  // Add section attributes
  QStringList sectionFilterAttributes = QStringList::split(QChar(' '),
      Config_getString("QHP_SECT_FILTER_ATTRS"));
  if (!sectionFilterAttributes.contains(QString("doxygen")))
  {
    sectionFilterAttributes << "doxygen";
  }
  for (int i = 0; i < (int)sectionFilterAttributes.count(); i++)
  {
    m_doc.openCloseContent("filterAttribute", sectionFilterAttributes[i]);
  }

  m_toc.open("toc");

  // Add extra root node
  QCString fullProjectname = getFullProjectName();
  const char * const attributes[] =
  { "title", fullProjectname,
    "ref",   "index.html",
    NULL
  };
  m_toc.open("section", attributes);
  m_prevSectionLevel = 1;
  m_sectionLevel = 1;

  m_index.open("keywords");
  m_files.open("files");
}

void Qhp::finalize()
{
  // Finish TOC
  handlePrevSection();
  for (int i = m_prevSectionLevel; i > 0; i--)
  {
    m_toc.close("section");
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

  QCString fileName = Config_getString("HTML_OUTPUT") + "/" + getQhpFileName();
  QFile file(fileName);
  if (!file.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n", fileName.data());
    exit(1);
  }
  m_doc.dumpTo(file);
}

void Qhp::incContentsDepth()
{
  m_sectionLevel++;
}

void Qhp::decContentsDepth()
{
  if (m_sectionLevel <= 0)
  {
    return;
  }
  m_sectionLevel--;
}

void Qhp::addContentsItem(bool /*isDir*/, const char * name, 
                          const char * /*ref*/, const char * file, 
                          const char * /*anchor*/)
{
  // Backup difference before modification
  int diff = m_prevSectionLevel - m_sectionLevel;

  handlePrevSection();
  setPrevSection(name, file, m_sectionLevel);

  // Close sections as needed
  for (; diff > 0; diff--)
  {
    m_toc.close("section");
  }
}

void Qhp::addIndexItem(Definition *context,MemberDef *md,
                       const char *word)
{
  (void)word;

  if (md) // member
  {
    static bool separateMemberPages = Config_getBool("SEPARATE_MEMBER_PAGES");
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
    QCString level2  = word ? QCString(word) : md->name();
    QCString contRef = separateMemberPages ? cfname : cfiname;
    QCString anchor  = md->anchor();

    QCString ref;

    // <keyword name="foo" id="MyApplication::foo" ref="doc.html#foo"/>
    ref = makeRef(contRef, anchor);
    QCString id = level1+"::"+level2;
    const char * attributes[] =
    {
      "name", level2,
      "id",   id,
      "ref",  ref,
      0
    };
    m_index.openClose("keyword", attributes);
  }
  else if (context) // container
  {
    // <keyword name="Foo" id="Foo" ref="doc.html"/>
    QCString contRef = context->getOutputFileBase();
    QCString level1  = word ? QCString(word) : context->name();
    QCString ref = makeFileName(contRef);
    const char * attributes[] =
    {
      "name", level1,
      "id",   level1,
      "ref",  ref,
      0
    };
    m_index.openClose("keyword", attributes);
  }
}

void Qhp::addIndexFile(const char * name)
{
  addFile(name);
}

QCString Qhp::getQhpFileName()
{
  return "index.qhp";
}

QCString Qhp::getFullProjectName()
{
  QCString projectName = Config_getString("PROJECT_NAME");
  QCString versionText = Config_getString("PROJECT_NUMBER");
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
    return;
  }

  // We skip "Main Page" as our extra root is pointing to that
  if (!((m_prevSectionLevel==1) && (m_prevSectionTitle=="Main Page")))
  {
    QCString finalRef = makeFileName(m_prevSectionRef);

    const char * const attributes[] =
    { "title", m_prevSectionTitle,
      "ref",   finalRef,
      NULL
    };

    if (m_prevSectionLevel < m_sectionLevel)
    {
      // Section with children
      m_toc.open("section", attributes);
    }
    else
    {
      // Section without children
      m_toc.openClose("section", attributes);
    }
  }

  clearPrevSection();
}

void Qhp::setPrevSection(const char * title, const char * ref, int level)
{
  m_prevSectionTitle = title;
  m_prevSectionRef   = ref;
  m_prevSectionLevel = level;
}

void Qhp::clearPrevSection()
{
  m_prevSectionTitle.resize(0);
  m_prevSectionRef.resize(0);
}

void Qhp::addFile(const char * fileName)
{
  m_files.openCloseContent("file", fileName);
}

void Qhp::addImageFile(const char *fileName)
{
  addFile(fileName);
}

void Qhp::addStyleSheetFile(const char *fileName)
{
  addFile(fileName);
}

