/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef DEFINITIONIMPL_H
#define DEFINITIONIMPL_H

#include <memory>
#include <string>
#include <unordered_map>

#include "definition.h"

class DefinitionImpl
{
  public:
    DefinitionImpl(
        Definition *def,
        const DString &defFileName,int defLine,size_t defColumn,
        const DString &name,const char *b=nullptr,const char *d=nullptr,
        bool isSymbol=true);
    ~DefinitionImpl();
    DefinitionImpl(const DefinitionImpl &d);
    DefinitionImpl &operator=(const DefinitionImpl &d);
    DefinitionImpl(DefinitionImpl &&d) = delete;
    DefinitionImpl &operator=(DefinitionImpl &&d) = delete;

    const DString &name() const;
    bool isAnonymous() const;
    const DString &localName() const;
    DString qualifiedName() const;
    DString symbolName() const;
    DString getSourceFileBase() const;
    DString getSourceAnchor() const;
    DString documentation() const;
    int docLine() const;
    DString docFile() const;
    DString briefDescription(bool abbreviate=false) const;
    DString briefDescriptionAsTooltip() const;
    int briefLine() const;
    DString inbodyDocumentation() const;
    DString inbodyFile() const;
    int inbodyLine() const;
    DString briefFile() const;
    DString getDefFileName() const;
    DString getDefFileExtension() const;
    int getDefLine() const;
    size_t getDefColumn() const;
    bool hasDocumentation() const;
    bool hasUserDocumentation() const;
    bool isVisibleInProject() const;
    bool isVisible() const;
    bool isHidden() const;
    bool isArtificial() const;
    bool isExported() const;
    DString getReference() const;
    bool isReference() const;
    DString externalReference(const DString &relPath) const;
    int getStartDefLine() const;
    int getStartBodyLine() const;
    int getEndBodyLine() const;
    const FileDef *getBodyDef() const;
    SrcLangExt getLanguage() const;
    const GroupList &partOfGroups() const;
    bool isLinkableViaGroup() const;
    const RefItemVector &xrefListItems() const;
    const RequirementRefs &requirementReferences() const;
    const Definition *findInnerCompound(const DString &name) const;
    Definition *getOuterScope() const;
    const MemberVector &getReferencesMembers() const;
    const MemberVector &getReferencedByMembers() const;
    bool hasSections() const;
    bool hasSources() const;
    bool hasBriefDescription() const;
    DString id() const;
    const SectionRefs &getSectionRefs() const;
    void setName(const DString &name);
    void setId(const DString &name);
    void setDefFile(const DString& df,int defLine,size_t defColumn);
    void setDocumentation(const DString &d,const DString &docFile,int docLine,bool stripWhiteSpace=true);
    void setBriefDescription(const DString &b,const DString &briefFile,int briefLine);
    void setInbodyDocumentation(const DString &d,const DString &docFile,int docLine);
    void setReference(const DString &r);
    void addSectionsToDefinition(const std::vector<const SectionInfo*> &anchorList);
    void setBodySegment(int defLine,int bls,int ble);
    void setBodyDef(const FileDef *fd);
    void addSourceReferencedBy(MemberDef *d,const DString &sourceRefName);
    void addSourceReferences(MemberDef *d,const DString &sourceRefName);
    void setRefItems(const RefItemVector &sli);
    void setRequirementReferences(const RequirementRefs &rqli);
    void mergeRefItems(Definition *d);
    void mergeReferences(const Definition *other);
    void mergeReferencedBy(const Definition *other);
    void addInnerCompound(Definition *d);
    void setOuterScope(Definition *d);
    void setHidden(bool b);
    void setArtificial(bool b);
    void setExported(bool b);
    void setLanguage(SrcLangExt lang);
    void writeSourceDef(OutputList &ol) const;
    void writeInlineCode(OutputList &ol,const DString &scopeName) const;
    bool hasSourceRefs() const;
    bool hasSourceReffedBy() const;
    void writeSourceRefs(OutputList &ol,const DString &scopeName) const;
    void writeSourceReffedBy(OutputList &ol,const DString &scopeName) const;
    bool hasRequirementRefs() const;
    void writeRequirementRefs(OutputList &ol) const;
    void makePartOfGroup(GroupDef *gd);
    void writeNavigationPath(OutputList &ol) const;
    DString navigationPathAsString() const;
    void writeQuickMemberLinks(OutputList &,const MemberDef *) const;
    void writeSummaryLinks(OutputList &) const;
    void writePageNavigation(OutputList &ol) const;
    DString pathFragment() const;
    void writeDocAnchorsToTagFile(TextStream &) const;
    void setLocalName(const DString &name);
    void writeToc(OutputList &ol, const LocalToc &lt) const;
    void computeTooltip();
    void _setSymbolName(const DString &name);
    DString _symbolName() const;


  private:

    int  _getXRefListId(const DString &listName) const;
    void _writeSourceRefList(OutputList &ol,const DString &scopeName,const DString &text,
                       const std::unordered_map<std::string,MemberDef *> &members,bool) const;
    void _setBriefDescription(const DString &b,const DString &briefFile,int briefLine);
    void _setDocumentation(const DString &d,const DString &docFile,int docLine,bool stripWhiteSpace,bool atTop);
    void _setInbodyDocumentation(const DString &d,const DString &docFile,int docLine);
    bool _docsAlreadyAdded(const DString &doc,DString &sigList);

    // PIMPL idiom
    class Private;
    std::unique_ptr<Private> p; // internal structure holding all private data
};

template<class Base>
class DefinitionMixin : public Base
{
  public:
    /*! Create a new definition */
    DefinitionMixin(
        const DString &defFileName,int defLine,size_t defColumn,
        const DString &name,const char *b=nullptr,const char *d=nullptr,
        bool isSymbol=true) : m_impl(this,defFileName,defLine,defColumn,name,b,d,isSymbol) {}
    DefinitionMixin(const DefinitionMixin &other) : Base(other), m_impl(other.m_impl) {}
    DefinitionMixin &operator=(const DefinitionMixin &other) { if (this!=&other) { m_impl = other.m_impl; }; return *this; }
    DefinitionMixin(DefinitionMixin &&) = delete;
    DefinitionMixin &operator=(DefinitionMixin &&) = delete;
   ~DefinitionMixin() override = default;

    bool isAlias() const override { return false; }

    //======== Definition
    const DString &name() const override { return m_impl.name(); }
    bool isAnonymous() const override { return m_impl.isAnonymous(); }
    const DString &localName() const override { return m_impl.localName(); }
    DString qualifiedName() const override { return m_impl.qualifiedName(); }
    DString symbolName() const override { return m_impl.symbolName(); }
    DString getSourceFileBase() const override { return m_impl.getSourceFileBase(); }
    DString getSourceAnchor() const override { return m_impl.getSourceAnchor(); }
    DString documentation() const override { return m_impl.documentation(); }
    int docLine() const override { return m_impl.docLine(); }
    DString docFile() const override { return m_impl.docFile(); }
    DString briefDescription(bool abbreviate=false) const override { return m_impl.briefDescription(abbreviate); }
    DString briefDescriptionAsTooltip() const override { return m_impl.briefDescriptionAsTooltip(); }
    int briefLine() const override { return m_impl.briefLine(); }
    DString inbodyDocumentation() const override { return m_impl.inbodyDocumentation(); }
    DString inbodyFile() const override { return m_impl.inbodyFile(); }
    int inbodyLine() const override { return m_impl.inbodyLine(); }
    DString briefFile() const override { return m_impl.briefFile(); }
    DString getDefFileName() const override { return m_impl.getDefFileName(); }
    DString getDefFileExtension() const override { return m_impl.getDefFileExtension(); }
    int getDefLine() const override { return m_impl.getDefLine(); }
    size_t getDefColumn() const override { return m_impl.getDefColumn(); }
    bool hasDocumentation() const override { return m_impl.hasDocumentation(); }
    bool hasUserDocumentation() const override { return m_impl.hasUserDocumentation(); }
    bool isVisibleInProject() const override { return m_impl.isVisibleInProject(); }
    bool isVisible() const override { return m_impl.isVisible(); }
    bool isHidden() const override { return m_impl.isHidden(); }
    bool isArtificial() const override { return m_impl.isArtificial(); }
    bool isExported() const override { return m_impl.isExported(); }
    DString getReference() const override { return m_impl.getReference(); }
    bool isReference() const override { return m_impl.isReference(); }
    DString externalReference(const DString &relPath) const override { return m_impl.externalReference(relPath); }
    int getStartDefLine() const override { return m_impl.getStartDefLine(); }
    int getStartBodyLine() const override { return m_impl.getStartBodyLine(); }
    int getEndBodyLine() const override { return m_impl.getEndBodyLine(); }
    const FileDef *getBodyDef() const override { return m_impl.getBodyDef(); }
    SrcLangExt getLanguage() const override { return m_impl.getLanguage(); }
    const GroupList &partOfGroups() const override { return m_impl.partOfGroups(); }
    bool isLinkableViaGroup() const override { return m_impl.isLinkableViaGroup(); }
    const RefItemVector &xrefListItems() const override { return m_impl.xrefListItems(); }
    const RequirementRefs &requirementReferences() const override { return m_impl.requirementReferences(); }
    const Definition *findInnerCompound(const DString &name) const override { return m_impl.findInnerCompound(name); }
    Definition *getOuterScope() const override { return m_impl.getOuterScope(); }
    const MemberVector &getReferencesMembers() const override { return m_impl.getReferencesMembers(); }
    const MemberVector &getReferencedByMembers() const override { return m_impl.getReferencedByMembers(); }
    bool hasSections() const override { return m_impl.hasSections(); }
    bool hasSources() const override { return m_impl.hasSources(); }
    bool hasBriefDescription() const override { return m_impl.hasBriefDescription(); }
    DString id() const override { return m_impl.id(); }
    const SectionRefs &getSectionRefs() const override { return m_impl.getSectionRefs(); }

    //======== DefinitionMutable
    void setName(const DString &name) override { m_impl.setName(name); }
    void setId(const DString &name) override { m_impl.setId(name); }
    void setDefFile(const DString& df,int defLine,size_t defColumn) override
    { m_impl.setDefFile(df,defLine,defColumn); }
    void setDocumentation(const DString &doc,const DString &docFile,int docLine,bool stripWhiteSpace=true) override
    { m_impl.setDocumentation(doc,docFile,docLine,stripWhiteSpace); }
    void setBriefDescription(const DString &brief,const DString &briefFile,int briefLine) override
    { m_impl.setBriefDescription(brief,briefFile,briefLine); }
    void setInbodyDocumentation(const DString &doc,const DString &docFile,int docLine) override
    { m_impl.setInbodyDocumentation(doc,docFile,docLine); }
    void setReference(const DString &r) override
    { m_impl.setReference(r); }
    void addSectionsToDefinition(const std::vector<const SectionInfo*> &anchorList) override
    { m_impl.addSectionsToDefinition(anchorList); }
    void setBodySegment(int defLine,int bls,int ble) override
    { m_impl.setBodySegment(defLine,bls,ble); }
    void setBodyDef(const FileDef *fd) override
    { m_impl.setBodyDef(fd); }
    void addSourceReferencedBy(MemberDef *md,const DString &sourceRefName) override
    { m_impl.addSourceReferencedBy(md,sourceRefName); }
    void addSourceReferences(MemberDef *md,const DString &sourceRefName) override
    { m_impl.addSourceReferences(md,sourceRefName); }
    void setRefItems(const RefItemVector &sli) override
    { m_impl.setRefItems(sli); }
    void setRequirementReferences(const RequirementRefs &rqli) override
    { m_impl.setRequirementReferences(rqli); }
    void mergeRefItems(Definition *def) override
    { m_impl.mergeRefItems(def); }
    void mergeReferences(const Definition *other) override
    { m_impl.mergeReferences(other); }
    void mergeReferencedBy(const Definition *other) override
    { m_impl.mergeReferencedBy(other); }
    void addInnerCompound(Definition *def) override
    { m_impl.addInnerCompound(def); }
    void setOuterScope(Definition *def) override
    { m_impl.setOuterScope(def); }
    void setHidden(bool b) override
    { m_impl.setHidden(b); }
    void setArtificial(bool b) override
    { m_impl.setArtificial(b); }
    void setExported(bool b) override
    { m_impl.setExported(b); }
    void setLanguage(SrcLangExt lang) override
    { m_impl.setLanguage(lang); }
    void writeSourceDef(OutputList &ol) const override
    { m_impl.writeSourceDef(ol); }
    void writeInlineCode(OutputList &ol,const DString &scopeName) const override
    { m_impl.writeInlineCode(ol,scopeName); }
    bool hasSourceRefs() const override
    { return m_impl.hasSourceRefs(); }
    bool hasSourceReffedBy() const override
    { return m_impl.hasSourceReffedBy(); }
    void writeSourceRefs(OutputList &ol,const DString &scopeName) const override
    { m_impl.writeSourceRefs(ol,scopeName); }
    void writeSourceReffedBy(OutputList &ol,const DString &scopeName) const override
    { m_impl.writeSourceReffedBy(ol,scopeName); }
    bool hasRequirementRefs() const override
    { return m_impl.hasRequirementRefs(); }
    void writeRequirementRefs(OutputList &ol) const override
    { m_impl.writeRequirementRefs(ol); }
    void makePartOfGroup(GroupDef *gd) override
    { m_impl.makePartOfGroup(gd); }
    void writeNavigationPath(OutputList &ol) const override
    { m_impl.writeNavigationPath(ol); }
    DString navigationPathAsString() const override
    { return m_impl.navigationPathAsString(); }
    void writeQuickMemberLinks(OutputList &ol,const MemberDef *md) const override
    { m_impl.writeQuickMemberLinks(ol,md); }
    void writeSummaryLinks(OutputList &ol) const override
    { m_impl.writeSummaryLinks(ol); }
    void writePageNavigation(OutputList &ol) const override
    { m_impl.writePageNavigation(ol); }
    DString pathFragment() const override
    { return m_impl.pathFragment(); }
    void writeDocAnchorsToTagFile(TextStream &fs) const override
    { m_impl.writeDocAnchorsToTagFile(fs); }
    void setLocalName(const DString &name) override
    { m_impl.setLocalName(name); }
    void writeToc(OutputList &ol, const LocalToc &lt) const override
    { m_impl.writeToc(ol,lt); }
    void computeTooltip() override
    { m_impl.computeTooltip(); }
    void _setSymbolName(const DString &name) override
    { m_impl._setSymbolName(name); }
    DString _symbolName() const override
    { return m_impl._symbolName(); }

  private:
    Definition *toDefinition_() override { return this; }
    DefinitionMutable *toDefinitionMutable_() override { return this; }
    const DefinitionImpl *toDefinitionImpl_() const override { return &m_impl; }

    DefinitionImpl m_impl;
};

class DefinitionAliasImpl
{
  public:
    DefinitionAliasImpl(Definition *def,const Definition *scope,const Definition *alias);
    virtual ~DefinitionAliasImpl();
    NON_COPYABLE(DefinitionAliasImpl)

    void init();
    void deinit();
    const DString &name() const;
    DString qualifiedName() const;
  private:
    Definition *m_def;
    const Definition *m_scope;
    DString m_symbolName;
    DString m_qualifiedName;
};

template<class Base>
class DefinitionAliasMixin : public Base
{
  public:
    DefinitionAliasMixin(const Definition *scope,const Definition *alias)
      : m_impl(this,scope,alias), m_scope(scope), m_alias(alias) {}
   ~DefinitionAliasMixin() override = default;
    NON_COPYABLE(DefinitionAliasMixin)

    void init() { m_impl.init(); }
    void deinit() { m_impl.deinit(); }


    bool isAlias() const override { return true; }

    //======== Definition
    const DString &name() const override
    { return m_impl.name(); }
    bool isAnonymous() const override
    { return m_alias->isAnonymous(); }
    const DString &localName() const override
    { return m_alias->localName(); }
    DString qualifiedName() const override
    { return m_impl.qualifiedName(); }
    DString symbolName() const override
    { return m_alias->symbolName(); }
    DString getSourceFileBase() const override
    { return m_alias->getSourceFileBase(); }
    DString getSourceAnchor() const override
    { return m_alias->getSourceAnchor(); }
    DString documentation() const override
    { return m_alias->documentation(); }
    int docLine() const override
    { return m_alias->docLine(); }
    DString docFile() const override
    { return m_alias->docFile(); }
    DString briefDescription(bool abbreviate=false) const override
    { return m_alias->briefDescription(abbreviate); }
    DString briefDescriptionAsTooltip() const override
    { return m_alias->briefDescriptionAsTooltip(); }
    int briefLine() const override
    { return m_alias->briefLine(); }
    DString inbodyDocumentation() const override
    { return m_alias->inbodyDocumentation(); }
    DString inbodyFile() const override
    { return m_alias->inbodyFile(); }
    int inbodyLine() const override
    { return m_alias->inbodyLine(); }
    DString briefFile() const override
    { return m_alias->briefFile(); }
    DString getDefFileName() const override
    { return m_alias->getDefFileName(); }
    DString getDefFileExtension() const override
    { return m_alias->getDefFileExtension(); }
    int getDefLine() const override
    { return m_alias->getDefLine(); }
    size_t getDefColumn() const override
    { return m_alias->getDefColumn(); }
    bool hasDocumentation() const override
    { return m_alias->hasDocumentation(); }
    bool hasUserDocumentation() const override
    { return m_alias->hasUserDocumentation(); }
    bool isVisibleInProject() const override
    { return m_alias->isVisibleInProject(); }
    bool isVisible() const override
    { return m_alias->isVisible(); }
    bool isHidden() const override
    { return m_alias->isHidden(); }
    bool isArtificial() const override
    { return m_alias->isArtificial(); }
    bool isExported() const override
    { return m_alias->isExported(); }
    DString getReference() const override
    { return m_alias->getReference(); }
    bool isReference() const override
    { return m_alias->isReference(); }
    DString externalReference(const DString &relPath) const override
    { return m_alias->externalReference(relPath); }
    int getStartDefLine() const override
    { return m_alias->getStartDefLine(); }
    int getStartBodyLine() const override
    { return m_alias->getStartBodyLine(); }
    int getEndBodyLine() const override
    { return m_alias->getEndBodyLine(); }
    const FileDef *getBodyDef() const override
    { return m_alias->getBodyDef(); }
    SrcLangExt getLanguage() const override
    { return m_alias->getLanguage(); }
    const GroupList &partOfGroups() const override
    { return m_alias->partOfGroups(); }
    bool isLinkableViaGroup() const override
    { return m_alias->isLinkableViaGroup(); }
    const RefItemVector &xrefListItems() const override
    { return m_alias->xrefListItems(); }
    const RequirementRefs &requirementReferences() const override
    { return m_alias->requirementReferences(); }
    const Definition *findInnerCompound(const DString &name) const override
    { return m_alias->findInnerCompound(name); }
    Definition *getOuterScope() const override
    { return const_cast<Definition*>(m_scope); }
    const MemberVector &getReferencesMembers() const override
    { return m_alias->getReferencesMembers(); }
    const MemberVector &getReferencedByMembers() const override
    { return m_alias->getReferencedByMembers(); }
    bool hasSections() const override
    { return m_alias->hasSections(); }
    bool hasSources() const override
    { return m_alias->hasSources(); }
    bool hasBriefDescription() const override
    { return m_alias->hasBriefDescription(); }
    DString id() const override
    { return m_alias->id(); }
    const SectionRefs &getSectionRefs() const override
    { return m_alias->getSectionRefs(); }
    DString navigationPathAsString() const override
    { return m_alias->navigationPathAsString(); }
    DString pathFragment() const override
    { return m_alias->pathFragment(); }

  protected:
    const Definition *getAlias() const { return m_alias; }
    const Definition *getScope() const { return m_scope; }

  private:
    virtual Definition *toDefinition_() { return this; }
    DefinitionMutable *toDefinitionMutable_() override { return nullptr; }
    const DefinitionImpl *toDefinitionImpl_() const override { return nullptr; }

    void _setSymbolName(const DString &name) override { m_symbolName = name; }
    DString _symbolName() const override { return m_symbolName; }
    DefinitionAliasImpl m_impl;
    const Definition *m_scope;
    const Definition *m_alias;
    DString m_symbolName;
};

#endif
