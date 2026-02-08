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

#include <memory>

#include "definition.h"
#include "filedef.h"

class ModuleDef;

class ConceptDef : public Definition
{
  public:
    ABSTRACT_BASE_CLASS(ConceptDef)

    enum class PartType { Code, Doc };
    struct Part
    {
      Part(PartType t,const QCString &s,int ln,int col) : type(t), content(s), lineNr(ln), colNr(col) {}
      PartType type;
      QCString content;
      int lineNr;
      int colNr;
    };
    using Parts = std::vector<Part>;

    virtual bool hasDetailedDescription() const = 0;
    virtual const IncludeInfo *includeInfo() const = 0;
    virtual ArgumentList getTemplateParameterList() const = 0;
    virtual QCString initializer() const = 0;
    virtual void writeDeclarationLink(OutputList &ol,bool &found,
                              const QCString &header,bool localNames) const = 0;
    virtual const NamespaceDef *getNamespaceDef() const = 0;
    virtual const FileDef *getFileDef() const = 0;
    virtual const ModuleDef *getModuleDef() const = 0;
    virtual QCString title() const = 0;
    virtual int groupId() const = 0;
    virtual Parts conceptParts() const = 0;
};

class ConceptDefMutable : public DefinitionMutable, public ConceptDef
{
  public:
    ABSTRACT_BASE_CLASS(ConceptDefMutable)

    virtual void setIncludeFile(FileDef *fd,const QCString &incName,bool local,bool force) = 0;
    virtual void setTemplateArguments(const ArgumentList &al) = 0;
    virtual void setNamespace(NamespaceDef *nd) = 0;
    virtual void setFileDef(FileDef *fd) = 0;
    virtual void writeTagFile(TextStream &) = 0;
    virtual void writeDocumentation(OutputList &ol) = 0;
    virtual void setInitializer(const QCString &init) = 0;
    virtual void findSectionsInDocumentation() = 0;
    virtual void setGroupId(int id) = 0;
    virtual void setModuleDef(ModuleDef *mod) = 0;
    virtual void addListReferences() = 0;
    virtual void addRequirementReferences() = 0;
    virtual void addDocPart(const QCString &doc,int lineNr,int colNr) = 0;
    virtual void addCodePart(const QCString &code,int lineNr,int colNr) = 0;
};

std::unique_ptr<ConceptDef> createConceptDef(
    const QCString &fileName,int startLine,int startColumn,const QCString &name,
    const QCString &tagRef=QCString(),const QCString &tagFile=QCString());

std::unique_ptr<ConceptDef> createConceptDefAlias(const Definition *newScope,const ConceptDef *cd);

// ---- Map

class ConceptLinkedMap : public LinkedMap<ConceptDef>
{
};

class ConceptLinkedRefMap : public LinkedRefMap<ConceptDef>
{
  public:
    bool declVisible() const;
    void writeDeclaration(OutputList &ol,const QCString &header,bool localNames) const;
};

// ---- Cast functions

ConceptDef        *toConceptDef(Definition *d);
ConceptDef        *toConceptDef(DefinitionMutable *d);
const ConceptDef  *toConceptDef(const Definition *d);
ConceptDefMutable *toConceptDefMutable(Definition *d);

// --- Helpers

ConceptDef *getConcept(const QCString &key);
inline ConceptDefMutable *getConceptMutable(const QCString &key)
{
  return toConceptDefMutable(getConcept(key));
}
ConceptDef *getResolvedConcept(const Definition *scope,const QCString &name);


#endif
