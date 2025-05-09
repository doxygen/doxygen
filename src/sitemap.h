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

#ifndef SITEMAP_H
#define SITEMAP_H

#include <memory>

#include "qcstring.h"
#include "construct.h"
#include "indexlist.h"

class Definition;
class MemberDef;

class Sitemap : public IndexIntf
{
  public:
    Sitemap();
   ~Sitemap();
    NON_COPYABLE(Sitemap)

    void initialize();
    void finalize();
    void incContentsDepth(){}
    void decContentsDepth(){}
    void addContentsItem(bool isDir, const QCString &name, const QCString &ref,
                         const QCString &file, const QCString &anchor,
                         bool separateIndex, bool addToNavIndex,
                         const Definition *def, const QCString &nameAsHtml) {}
    void addIndexItem(const Definition *context, const MemberDef *md,
                      const QCString &sectionAnchor, const QCString &title) {}
    void addIndexFile(const QCString & name);
    void addImageFile(const QCString & name) {}
    void addStyleSheetFile(const QCString & name) {}

    static inline const QCString sitemapFileName = "sitemap.xml";

  private:
    class Private;
    std::unique_ptr<Private> p;
};

class Crawlmap : public IndexIntf
{
  public:
    Crawlmap();
   ~Crawlmap();
    NON_COPYABLE(Crawlmap)

    void initialize();
    void finalize();
    void incContentsDepth(){}
    void decContentsDepth(){}
    void addContentsItem(bool isDir, const QCString & name, const QCString & ref,
                         const QCString & file, const QCString & anchor,
                         bool separateIndex,bool addToNavIndex,
                         const Definition *def, const QCString &nameAsHtml);
    void addIndexItem(const Definition *context, const MemberDef *md,
                      const QCString &sectionAnchor, const QCString &title);
    void addIndexFile(const QCString & name);
    void addImageFile(const QCString & name){}
    void addStyleSheetFile(const QCString & name){}

    static inline const QCString crawlFileName = "doxygen_crawl";

  private:
    class Private;
    std::unique_ptr<Private> p;
};


#endif // SITEMAP_H

