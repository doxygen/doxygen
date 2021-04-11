/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#ifndef CONCEPTDEF_H
#define CONCEPTDEF_H

#include "definition.h"
#include "filedef.h"

class ConceptDef : public Definition
{
  public:
    virtual DefType definitionType() const = 0;
    virtual QCString getOutputFileBase() const = 0;
    virtual bool hasDetailedDescription() const = 0;
    virtual QCString displayName(bool includeScope=true) const = 0;
    virtual const IncludeInfo *includeInfo() const = 0;
    virtual ArgumentList getTemplateParameterList() const = 0;
    virtual QCString anchor() const = 0;
    virtual bool isLinkableInProject() const = 0;
    virtual bool isLinkable() const = 0;
    virtual QCString initializer() const = 0;
    virtual void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const = 0;
    virtual const NamespaceDef *getNamespaceDef() const = 0;
    virtual const FileDef *getFileDef() const = 0;
};

class ConceptDefMutable : public DefinitionMutable, public ConceptDef
{
  public:
    virtual void setIncludeFile(FileDef *fd,const QCString &incName,bool local,bool force) = 0;
    virtual void setTemplateArguments(const ArgumentList &al) = 0;
    virtual void setNamespace(NamespaceDef *nd) = 0;
    virtual void setFileDef(FileDef *fd) = 0;
    virtual void writeTagFile(TextStream &) = 0;
    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual void setInitializer(const QCString &init) = 0;
    virtual void findSectionsInDocumentation() = 0;
};

ConceptDefMutable *createConceptDef(
    const QCString &fileName,int startLine,int startColumn,const QCString &name,
    const QCString &tagRef=QCString(),const QCString &tagFile=QCString());

ConceptDef *createConceptDefAlias(const Definition *newScope,const ConceptDef *cd);

// ---- Map

class ConceptLinkedMap : public LinkedMap<ConceptDef>
{
};

class ConceptLinkedRefMap : public LinkedRefMap<const ConceptDef>
{
  public:
    bool declVisible() const;
    void writeDeclaration(OutputList &ol,const QCString &header,bool localNames) const;
    void writeDocumentation(OutputList &ol,const Definition * container=0) const;
};

// ---- Cast functions

ConceptDef        *toConceptDef(Definition *d);
ConceptDef        *toConceptDef(DefinitionMutable *d);
const ConceptDef  *toConceptDef(const Definition *d);
ConceptDefMutable *toConceptDefMutable(Definition *d);
ConceptDefMutable *toConceptDefMutable(const Definition *d);

#endif
