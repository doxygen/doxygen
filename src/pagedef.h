/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#ifndef PAGEDEF_H
#define PAGEDEF_H

#include "definition.h"

class PageLinkedRefMap;
class OutputList;
struct TagInfo;

/** @brief A model of a page symbol. */
class PageDef : public DefinitionMutable, public Definition
{
  public:
    ABSTRACT_BASE_CLASS(PageDef)

    // setters
    virtual void setFileName(const DString &name) = 0;
    virtual void setLocalToc(const LocalToc &tl) = 0;
    virtual void setShowLineNo(bool) = 0;
    virtual void setTitle(const DString &title) = 0;

    // getters
    virtual void findSectionsInDocumentation() = 0;
    virtual DString title() const = 0;
    virtual DString titleAsText() const = 0;
    virtual const GroupDef *getGroupDef() const = 0;
    virtual const PageLinkedRefMap &getSubPages() const = 0;
    virtual bool visibleInIndex() const = 0;
    virtual bool documentedPage() const = 0;
    virtual bool hasSubPages() const = 0;
    virtual bool hasParentPage() const = 0;
    virtual bool hasTitle() const = 0;
    virtual LocalToc localToc() const = 0;
    virtual void setPageScope(Definition *) = 0;
    virtual Definition *getPageScope() const = 0;
    virtual bool showLineNo() const = 0;

    virtual void writeDocumentation(OutputList &) = 0;
    virtual void writeTagFile(TextStream &) = 0;
    virtual void setNestingLevel(int) = 0;
    virtual void writePageDocumentation(OutputList &) const = 0;
    virtual void addSectionsToIndex() = 0;
    virtual void addListReferences() = 0;
    virtual void addRequirementReferences() = 0;

};

std::unique_ptr<PageDef> createPageDef(const DString &f,int l,const DString &n,const DString &d,const DString &t);

// ----------------------

[[maybe_unused]] PageDef *addRelatedPage(
                        const DString &name,
                        const DString &ptitle,
                        const DString &doc,
                        const DString &fileName,
                        int docLine,
                        int startLine,
                        const RefItemVector &sli = RefItemVector(),
                        GroupDef *gd=nullptr,
                        const TagInfo *tagInfo=nullptr,
                        bool xref=false,
                        SrcLangExt lang=SrcLangExt::Unknown
                       );

// --- Cast functions

PageDef            *toPageDef(Definition *d);
const PageDef      *toPageDef(const Definition *d);

// ------------------

class PageLinkedMap final : public LinkedMap<PageDef>
{
};

class PageLinkedRefMap final : public LinkedRefMap<PageDef>
{
};

#endif

