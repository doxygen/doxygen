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

class Sitemap::Private
{
  public:
    std::ofstream docFile;
    TextStream doc;
    TextStream index;
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
