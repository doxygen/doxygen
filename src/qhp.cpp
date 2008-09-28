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

#include <string.h>

static QCString makeFileName(char const * withoutExtension)
{
  if (!withoutExtension) return QCString();
  return QCString(withoutExtension)+".html";
}

static QCString makeRef(char const * withoutExtension, char const * anchor)
{
  if (!withoutExtension) return QCString(); 
  QCString result = makeFileName(withoutExtension);
  if (!anchor) return result;
  return result+"#"+anchor;
}

Qhp::Qhp() : m_prevSectionLevel(0), m_sectionLevel(0)
{
  m_toc.setIndentLevel(0);
  m_doc.setIndentLevel(2);
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
  QCString projectTitle    = getFullProjectName();
  QCString filterIdent     = projectTitle;
  QCString filterAttribute = projectTitle;

  char const * rootAttributes[] = 
  { "version", "1.0", 0 };
  char const * customFilterAttributes[] = 
  { "name", filterIdent, 0 };

  m_doc.open("QtHelpProject", rootAttributes);
  m_doc.openCloseContent("namespace", nameSpace);
  m_doc.openCloseContent("virtualFolder", virtualFolder);

  m_doc.open("customFilter", customFilterAttributes);
  m_doc.openCloseContent("filterAttribute", filterAttribute);
  m_doc.close("customFilter");
  m_doc.open("filterSection");
  m_doc.openCloseContent("filterAttribute", filterAttribute);

  m_toc.open("toc");
  m_index.open("keywords");
  m_files.open("files");
}

void Qhp::finalize()
{
  // Finish TOC
  handlePrevSection();
  m_toc.close("toc");
  m_doc.insert(m_toc);

  // Finish index  
  m_index.close("keywords");
  m_doc.insert(m_index);

  // Finish files
  addFile("doxygen.css");
  addFile("doxygen.png");
  addFile("tab_b.gif");
  addFile("tab_l.gif");
  addFile("tab_r.gif");
  addFile("tabs.css");
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

void Qhp::addContentsItem(bool /*isDir*/, char const * name, 
                          char const * /*ref*/, char const * file, 
                          char const * /*anchor*/)
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

void Qhp::addIndexItem(char const * level1, char const * level2,
                       char const * contRef, char const * /*memRef*/, 
                       char const * anchor, const MemberDef * /*md*/)
{
  /*
  <keyword name="foo" id="MyApplication::foo" ref="doc.html#foo"/>
  */
  QCString ref = makeRef(contRef, anchor);
  QCString id(level1);
  id += "::";
  id += level2;
  char const * attributes[] =
  { "name", level2, 
    "id",   id, 
    "ref",  ref, 
    0
  };
  m_index.openClose("keyword", attributes);
}

void
Qhp::addIndexFile(char const * name)
{
  addFile(name);
}

/*static*/ QCString
Qhp::getQhpFileName()
{

  return "index.qhp";
}

/*static*/ QCString
Qhp::getFullProjectName()
{
  QCString const projectName = Config_getString("PROJECT_NAME");
  QCString const versionText = Config_getString("PROJECT_NUMBER");
  return projectName + (versionText.isEmpty()
      ? QCString("")
      : QCString(" ") + versionText);
}

void
Qhp::handlePrevSection()
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

  // Replace "Main Page" with <project_name> in TOC
  QCString finalTitle;
  if (m_prevSectionLevel==0 && m_prevSectionTitle=="Main Page")
  {
    finalTitle = getFullProjectName();
  }
  if (finalTitle.isEmpty())
  {
    finalTitle = m_prevSectionTitle;
  }
  QCString finalRef = makeFileName(m_prevSectionRef);

  char const * const attributes[] =
  { "title", finalTitle,
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

  clearPrevSection();
}

void Qhp::setPrevSection(char const * title, char const * ref, int level)
{
  m_prevSectionTitle = title;
  m_prevSectionRef   = ref;
  m_prevSectionLevel = level;
}

void Qhp::clearPrevSection()
{
  m_prevSectionTitle.resize(0);
  m_prevSectionRef.resize(0);
  m_prevSectionLevel = m_sectionLevel;
}

void Qhp::addFile(char const * fileName)
{
  m_files.openCloseContent("file", fileName);
}
