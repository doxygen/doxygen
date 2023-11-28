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
#include "version.h"
#include "language.h"
#include "portable.h"

class Sitemap::Private
{
  public:
    bool site;
    std::ofstream docFile;
    TextStream doc;
    //TextStream index;
    std::ofstream crawlFile;
    TextStream crawl;
    
};

Sitemap::Sitemap(const bool site) : p(std::make_unique<Private>()) {p->site = site;}
Sitemap::~Sitemap() = default;
Sitemap::Sitemap(Sitemap &&) = default;

void Sitemap::initialize()
{
  QCString fileName = Config_getString(HTML_OUTPUT) + "/" + sitemapFileName;
  QCString fileNameCrawl = Config_getString(HTML_OUTPUT) + "/" + crawlFileName;
  addHtmlExtensionIfMissing(fileNameCrawl);

  if (p->site)
  {
    p->docFile = Portable::openOutputStream(fileName);
    if (!p->docFile.is_open())
    {
      term("Could not open file %s for writing\n", fileName.data());
    }
    p->doc.setStream(&p->docFile);

    p->doc << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    p->doc << "<urlset\n";
    p->doc << "  xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\"\n";
    p->doc << "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    p->doc << "  xsi:schemaLocation=\"http://www.sitemaps.org/schemas/sitemap/0.9\n";
    p->doc << "      http://www.sitemaps.org/schemas/sitemap/0.9/sitemap.xsd\">\n";
  }
  p->crawlFile = Portable::openOutputStream(fileNameCrawl);
  if (!p->crawlFile.is_open())
  {
    term("Could not open file %s for writing\n", fileNameCrawl.data());
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

void Sitemap::finalize()
{
  if (p->site)
  {
    p->doc << "</urlset>\n";
    p->doc.flush();
    p->docFile.close();
  }
  p->crawl << "</body>\n";
  p->crawl << "</html>\n";
  p->crawl.flush();
  p->crawlFile.close();
}

void Sitemap::addIndexFile(const QCString & fileName)
{
  QCString fn = fileName;
  addHtmlExtensionIfMissing(fn);
  if (p->site)
  {
    QCString sidemapUrl = Config_getString(SITEMAP_URL);
    p->doc << "  <url>\n";
    p->doc << "    <loc>" << convertToXML(sidemapUrl + fn) << "</loc>\n";
    p->doc << "  </url>\n";
  }
  p->crawl << "    <a href=\"" << fn << "\"/>\n";
}
