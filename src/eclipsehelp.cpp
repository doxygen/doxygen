/******************************************************************************
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

#include "eclipsehelp.h"
#include "util.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "portable.h"

struct EclipseHelp::Private
{
  int depth = 0;
  bool endtag = false;
  int openTags = 0;

  std::ofstream tocstream;
  QCString pathprefix;

  /* -- formatting helpers */
  void indent()
  {
    for (int i=0; i<depth; i++)
    {
      tocstream << "  ";
    }
  }
  void closedTag()
  {
    if (endtag)
    {
      tocstream << "/>\n";
      endtag = FALSE;
    }
  }
  void openedTag()
  {
    if (endtag)
    {
      tocstream << ">\n";
      endtag = FALSE;
      ++openTags;
    }
  }
};

EclipseHelp::EclipseHelp() : p(std::make_unique<Private>()) {}
EclipseHelp::~EclipseHelp() = default;

/*!
 * \brief Initialize the Eclipse generator
 *
 * This method opens the XML TOC file and writes headers of the files.
 * \sa finalize()
 */
void EclipseHelp::initialize()
{
  // -- open the contents file
  QCString name = Config_getString(HTML_OUTPUT) + "/toc.xml";
  p->tocstream = Portable::openOutputStream(name);
  if (!p->tocstream.is_open())
  {
    term("Could not open file {} for writing\n", name);
  }

  // -- write the opening tag
  QCString title = Config_getString(PROJECT_NAME);
  if (title.isEmpty())
  {
    title = "Doxygen generated documentation";
  }
  p->tocstream << "<toc label=\"" << convertToXML(title)
              << "\" topic=\"" << convertToXML(p->pathprefix)
              << "index" << Doxygen::htmlFileExtension << "\">\n";
  ++ p->depth;
}

/*!
 * \brief Finish generation of the Eclipse specific help files
 *
 * This method writes footers of the files and closes them.
 * \sa initialize()
 */
void EclipseHelp::finalize()
{
  p->closedTag(); // -- close previous tag

  // -- write ending tag
  --p->depth;
  p->tocstream << "</toc>\n";

  // -- close the content file
  p->tocstream.close();

  QCString name = Config_getString(HTML_OUTPUT) + "/plugin.xml";
  std::ofstream t = Portable::openOutputStream(name);
  if (t.is_open())
  {
    QCString docId = Config_getString(ECLIPSE_DOC_ID);
    t << "<plugin name=\""  << docId << "\" id=\"" << docId << "\"\n";
    t << "        version=\"1.0.0\" provider-name=\"Doxygen\">\n";
    t << "  <extension point=\"org.eclipse.help.toc\">\n";
    t << "    <toc file=\"toc.xml\" primary=\"true\" />\n";
    t << "  </extension>\n";
    t << "</plugin>\n";
  }
}

/*!
 * \brief Increase the level of content hierarchy
 */
void EclipseHelp::incContentsDepth()
{
  p->openedTag();
  ++p->depth;
}

/*!
 * \brief Decrease the level of content hierarchy
 *
 * It closes currently opened topic tag.
 */
void EclipseHelp::decContentsDepth()
{
  // -- end of the opened topic
  p->closedTag();
  --p->depth;

  if (p->openTags==p->depth)
  {
    --p->openTags;
    p->indent();
    p->tocstream << "</topic>\n";
  }
}

/*!
 * \brief Add an item to the content
 *
 * @param isDir Flag whether the argument \a file is a directory or a file entry
 * @param name Name of the item
 * @param ref URL of the item
 * @param file Name of a file which the item is defined in (without extension)
 * @param anchor Name of an anchor of the item.
 * @param separateIndex not used.
 * @param addToNavIndex not used.
 * @param def not used.
 * @param nameAsHtml name parameter in HTML format
 */
void EclipseHelp::addContentsItem(
    bool /* isDir */,
    const QCString &name,
    const QCString & /* ref */,
    const QCString &file,
    const QCString &anchor,
    bool /* separateIndex */,
    bool /* addToNavIndex */,
    const Definition * /*def*/,
    const QCString & /* nameAsHtml */)
{
  // -- write the topic tag
  p->closedTag();
  if (!file.isEmpty())
  {
    QCString fn = file;
    addHtmlExtensionIfMissing(fn);
    switch (file[0]) // check for special markers (user defined URLs)
    {
      case '^':
        // URL not supported by eclipse toc.xml
	break;

      case '!':
        p->indent();
        p->tocstream << "<topic label=\"" << convertToXML(name) << "\"";
        p->tocstream << " href=\"" << convertToXML(p->pathprefix) << &file[1] << "\"";
        p->endtag = TRUE;
	break;

      default:
        p->indent();
        p->tocstream << "<topic label=\"" << convertToXML(name) << "\"";
        p->tocstream << " href=\"" << convertToXML(p->pathprefix) << fn;
        if (!anchor.isEmpty())
        {
          p->tocstream << "#" << anchor;
        }
        p->tocstream << "\"";
        p->endtag = TRUE;
	break;
    }
  }
  else
  {
    p->indent();
    p->tocstream << "<topic label=\"" << convertToXML(name) << "\"";
    p->endtag = TRUE;
  }
}

void EclipseHelp::addIndexItem(
    const Definition * /* context */,
    const MemberDef * /* md */,
    const QCString & /* sectionAnchor */,
    const QCString & /* title */)
{
}

void EclipseHelp::addIndexFile(const QCString & /* name */)
{
}

void EclipseHelp::addImageFile(const QCString & /* name */)
{
}

void EclipseHelp::addStyleSheetFile(const QCString & /* name */)
{
}

