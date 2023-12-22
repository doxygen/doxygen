/*
 * Copyright (C) 1997-2023 Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 */

#include <algorithm>
#include <memory>
#include <string.h>
#include <vector>
#include <cassert>

#include "config.h"
#include "debug.h"
#include "doxygen.h"
#include "groupdef.h"
#include "memberdef.h"
#include "message.h"
#include "sitemap.h"
#include "textstream.h"
#include "util.h"
#include "portable.h"
#include "language.h"
#include "version.h"

//-------------------------------------------------------------------------------------------

class Sitemap::Private
{
  public:
    std::ofstream docFile;
    TextStream doc;
};

Sitemap::Sitemap() : p(std::make_unique<Private>()) {}
Sitemap::~Sitemap() = default;
Sitemap::Sitemap(Sitemap &&) = default;

void Sitemap::initialize()
{
  QCString fileName = Config_getString(HTML_OUTPUT) + "/" + sitemapFileName;

  p->docFile = Portable::openOutputStream(fileName);
  if (!p->docFile.is_open())
  {
    term("Could not open file %s for writing\n", qPrint(fileName));
  }
  p->doc.setStream(&p->docFile);

  p->doc << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  p->doc << "<urlset\n";
  p->doc << "  xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\"\n";
  p->doc << "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
  p->doc << "  xsi:schemaLocation=\"http://www.sitemaps.org/schemas/sitemap/0.9\n";
  p->doc << "      http://www.sitemaps.org/schemas/sitemap/0.9/sitemap.xsd\">\n";
}

void Sitemap::finalize()
{
  p->doc << "</urlset>\n";
  p->doc.flush();
  p->docFile.close();
}

void Sitemap::addIndexFile(const QCString & fileName)
{
  QCString fn = fileName;
  QCString sidemapUrl = Config_getString(SITEMAP_URL);
  addHtmlExtensionIfMissing(fn);
  p->doc << "  <url>\n";
  p->doc << "    <loc>" << convertToXML(sidemapUrl + fn) << "</loc>\n";
  p->doc << "  </url>\n";
}

//-------------------------------------------------------------------------------------------
//
class Crawlmap::Private
{
  public:
    std::ofstream crawlFile;
    TextStream crawl;
};

Crawlmap::Crawlmap() : p(std::make_unique<Private>()) {}
Crawlmap::~Crawlmap() = default;
Crawlmap::Crawlmap(Crawlmap &&) = default;

void Crawlmap::initialize()
{
  QCString fileName = Config_getString(HTML_OUTPUT) + "/" + crawlFileName;
  addHtmlExtensionIfMissing(fileName);
  p->crawlFile = Portable::openOutputStream(fileName);
  if (!p->crawlFile.is_open())
  {
    term("Could not open file %s for writing\n", qPrint(fileName));
  }
  p->crawl.setStream(&p->crawlFile);
  p->crawl << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"https://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  p->crawl << "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"" + theTranslator->trISOLang() + "\">\n";
  p->crawl << "<head>\n";
  p->crawl << "<title>Validator / crawler helper</title>\n";
  p->crawl << "<meta http-equiv=\"Content-Type\" content=\"text/xhtml;charset=UTF-8\"/>\n";
  p->crawl << "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=11\"/>\n";

  p->crawl << "<meta name=\"generator\" content=\"Doxygen " + getDoxygenVersion() + "\"/>\n";
  p->crawl << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>\n";
  p->crawl << "</head>\n";
  p->crawl << "<body>\n";
}

void Crawlmap::finalize()
{
  p->crawl << "</body>\n";
  p->crawl << "</html>\n";
  p->crawl.flush();
  p->crawlFile.close();
}

void Crawlmap::addIndexFile(const QCString & fileName)
{
  QCString fn = fileName;
  addHtmlExtensionIfMissing(fn);
  p->crawl << "<a href=\"" << fn << "\"/>\n";
}

