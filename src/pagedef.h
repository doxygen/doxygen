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

/** @brief A model of a page symbol. */
class PageDef : public DefinitionMutable, public Definition
{
  public:
    // setters
    virtual void setFileName(const QCString &name) = 0;
    virtual void setLocalToc(const LocalToc &tl) = 0;
    virtual void setShowLineNo(bool) = 0;
    virtual void setTitle(const QCString &title) = 0;

    // getters
    virtual DefType definitionType() const = 0;
    virtual bool isLinkableInProject() const = 0;
    virtual bool isLinkable() const = 0;
    virtual QCString getOutputFileBase() const = 0;
    virtual QCString anchor() const = 0;
    virtual void findSectionsInDocumentation() = 0;
    virtual QCString title() const = 0;
    virtual const GroupDef *getGroupDef() const = 0;
    virtual const PageLinkedRefMap &getSubPages() const = 0;
    virtual void addInnerCompound(const Definition *) = 0;
    virtual bool visibleInIndex() const = 0;
    virtual bool documentedPage() const = 0;
    virtual bool hasSubPages() const = 0;
    virtual bool hasParentPage() const = 0;
    virtual bool hasTitle() const = 0;
    virtual LocalToc localToc() const = 0;
    virtual void setPageScope(Definition *) = 0;
    virtual Definition *getPageScope() const = 0;
    virtual QCString displayName(bool=TRUE) const = 0;
    virtual bool showLineNo() const = 0;

    virtual void writeDocumentation(OutputList &) = 0;
    virtual void writeTagFile(TextStream &) = 0;
    virtual void setNestingLevel(int) = 0;
    virtual void writePageDocumentation(OutputList &) const = 0;

};

PageDef *createPageDef(const QCString &f,int l,const QCString &n,const QCString &d,const QCString &t);

// --- Cast functions

PageDef            *toPageDef(Definition *d);
const PageDef      *toPageDef(const Definition *d);

// ------------------

class PageLinkedMap : public LinkedMap<PageDef>
{
};

class PageLinkedRefMap : public LinkedRefMap<const PageDef>
{
};

#endif

