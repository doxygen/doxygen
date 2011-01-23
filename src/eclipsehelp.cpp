/******************************************************************************
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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
#include "eclipsehelp.h"
#include "util.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"

EclipseHelp::EclipseHelp() : m_depth(0), m_endtag(FALSE), m_tocfile(0) 
{
}

EclipseHelp::~EclipseHelp() 
{
}

void EclipseHelp::indent()
{
  int i;
  for (i=0; i<m_depth; i++)
  {
    m_tocstream << "  ";
  }
}

void EclipseHelp::closedTag()
{
  if (m_endtag) 
  {
    m_tocstream << "/>" << endl;
    m_endtag = FALSE;
  }
}

void EclipseHelp::openedTag()
{
  if (m_endtag) 
  {
    m_tocstream << ">" << endl;
    m_endtag = FALSE;
  }
}

/*!
 * \brief Initialize the Eclipse generator
 *
 * This method opens the XML TOC file and writes headers of the files.
 * \sa finalize()
 */
void EclipseHelp::initialize() 
{
  // -- read path prefix from the configuration
  //m_pathprefix = Config_getString("ECLIPSE_PATHPREFIX");
  //if (m_pathprefix.isEmpty()) m_pathprefix = "html/";

  // -- open the contents file 
  QCString name = Config_getString("HTML_OUTPUT") + "/toc.xml";
  m_tocfile = new QFile(name);
  if (!m_tocfile->open(IO_WriteOnly)) 
  {
    err("Could not open file %s for writing\n", name.data());
    exit(1);
  }

  // -- initialize its text stream
  m_tocstream.setDevice(m_tocfile);
  //m_tocstream.setEncoding(FTextStream::UnicodeUTF8);

  // -- write the opening tag
  QCString title = Config_getString("PROJECT_NAME");
  if (title.isEmpty())
  {
    title = "Doxygen generated documentation";
  }
  m_tocstream << "<toc label=\"" << convertToXML(title) 
              << "\" topic=\"" << convertToXML(m_pathprefix) 
              << "index" << Doxygen::htmlFileExtension << "\">" << endl;
  ++ m_depth;
}

/*!
 * \brief Finish generation of the Eclipse specific help files
 *
 * This method writes footers of the files and closes them.
 * \sa initialize()
 */
void EclipseHelp::finalize() 
{
  closedTag(); // -- close previous tag

  // -- write ending tag 
  --m_depth;
  m_tocstream << "</toc>" << endl;

  // -- close the content file
  m_tocstream.unsetDevice();
  m_tocfile->close();
  delete m_tocfile; m_tocfile = 0;

  QCString name = Config_getString("HTML_OUTPUT") + "/plugin.xml";
  QFile pluginFile(name);
  if (pluginFile.open(IO_WriteOnly))
  {
    QString docId = Config_getString("ECLIPSE_DOC_ID");
    FTextStream t(&pluginFile);
    t << "<plugin name=\""  << docId << "\" id=\"" << docId << "\"" << endl;
    t << "        version=\"1.0.0\" provider-name=\"Doxygen\">" << endl;
    t << "  <extension point=\"org.eclipse.help.toc\">" << endl;
    t << "    <toc file=\"toc.xml\" primary=\"true\" />" << endl;
    t << "  </extension>" << endl;
    t << "</plugin>" << endl;
  }
}

/*!
 * \brief Increase the level of content hierarchy
 */
void EclipseHelp::incContentsDepth() 
{
  openedTag();
  ++m_depth;
}

/*!
 * \brief Decrease the level of content hierarchy
 *
 * It closes currently opened topic tag.
 */
void EclipseHelp::decContentsDepth() 
{
  // -- end of the opened topic
  closedTag();
  --m_depth;
  indent();
  m_tocstream << "</topic>" << endl;
}

/*!
 * \brief Add an item to the content
 *
 * @param isDir Flag whether the argument \a file is a directory or a file entry
 * @param name Name of the item
 * @param ref URL of the item
 * @param file Name of a file which the item is defined in (without extension)
 * @param anchor Name of an anchor of the item.
 */
void EclipseHelp::addContentsItem(
    bool /* isDir */,
    const char *name,
    const char * /* ref */,
    const char *file,
    const char *anchor) 
{
  // -- write the topic tag 
  closedTag();
  indent();
  m_tocstream << "<topic label=\"" << convertToXML(name) << "\"";
  if (file) 
  { 
    m_tocstream << " href=\"" << convertToXML(m_pathprefix) 
                << file << Doxygen::htmlFileExtension;
    if (anchor)
    {
      m_tocstream << "#" << anchor;
    }
    m_tocstream << "\"";
  }
  m_endtag = TRUE;
}

void EclipseHelp::addIndexItem(
    Definition * /* context */,
    MemberDef * /* md */,
    const char * /* title */)
{
}

void EclipseHelp::addIndexFile(const char * /* name */) 
{
}

void EclipseHelp::addImageFile(const char * /* name */) 
{
}

void EclipseHelp::addStyleSheetFile(const char * /* name */) 
{
}

