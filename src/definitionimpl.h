/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2019 by Dimitri van Heesch.
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
#include <unordered_map>
#include <string>

#include "definition.h"

class DefinitionImpl
{
  public:
    DefinitionImpl(
        Definition *def,
        const QCString &defFileName,int defLine,int defColumn,
        const QCString &name,const char *b=nullptr,const char *d=nullptr,
        bool isSymbol=TRUE);
    ~DefinitionImpl();
    DefinitionImpl(const DefinitionImpl &d);
    DefinitionImpl &operator=(const DefinitionImpl &d);
    DefinitionImpl(DefinitionImpl &&d) = delete;
    DefinitionImpl &operator=(DefinitionImpl &&d) = delete;

    const QCString &name() const;
    bool isAnonymous() const;
    const QCString &localName() const;
    QCString qualifiedName() const;
    QCString symbolName() const;
    QCString getSourceFileBase() const;
    QCString getSourceAnchor() const;
    QCString documentation() const;
    int docLine() const;
    QCString docFile() const;
    QCString briefDescription(bool abbreviate=FALSE) const;
    QCString briefDescriptionAsTooltip() const;
    int briefLine() const;
    QCString inbodyDocumentation() const;
    QCString inbodyFile() const;
    int inbodyLine() const;
    QCString briefFile() const;
    QCString getDefFileName() const;
    QCString getDefFileExtension() const;
    int getDefLine() const;
    int getDefColumn() const;
    bool hasDocumentation() const;
    bool hasUserDocumentation() const;
    bool isVisibleInProject() const;
    bool isVisible() const;
    bool isHidden() const;
    bool isArtificial() const;
    bool isExported() const;
    QCString getReference() const;
    bool isReference() const;
    QCString externalReference(const QCString &relPath) const;
    int getStartDefLine() const;
    int getStartBodyLine() const;
    int getEndBodyLine() const;
    const FileDef *getBodyDef() const;
    SrcLangExt getLanguage() const;
    const GroupList &partOfGroups() const;
    bool isLinkableViaGroup() const;
    const RefItemVector &xrefListItems() const;
    const Definition *findInnerCompound(const QCString &name) const;
    Definition *getOuterScope() const;
    const MemberVector &getReferencesMembers() const;
    const MemberVector &getReferencedByMembers() const;
    bool hasSections() const;
    bool hasSources() const;
    bool hasBriefDescription() const;
    QCString id() const;
    const SectionRefs &getSectionRefs() const;
    void setName(const QCString &name);
    void setId(const QCString &name);
    void setDefFile(const QCString& df,int defLine,int defColumn);
    void setDocumentation(const QCString &d,const QCString &docFile,int docLine,bool stripWhiteSpace=TRUE);
    void setBriefDescription(const QCString &b,const QCString &briefFile,int briefLine);
    void setInbodyDocumentation(const QCString &d,const QCString &docFile,int docLine);
    void setReference(const QCString &r);
    void addSectionsToDefinition(const std::vector<const SectionInfo*> &anchorList);
    void setBodySegment(int defLine,int bls,int ble);
    void setBodyDef(const FileDef *fd);
    void addSourceReferencedBy(MemberDef *d,const QCString &sourceRefName);
    void addSourceReferences(MemberDef *d,const QCString &sourceRefName);
    void setRefItems(const RefItemVector &sli);
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
    void writeInlineCode(OutputList &ol,const QCString &scopeName) const;
    bool hasSourceRefs() const;
    bool hasSourceReffedBy() const;
    void writeSourceRefs(OutputList &ol,const QCString &scopeName) const;
    void writeSourceReffedBy(OutputList &ol,const QCString &scopeName) const;
    void makePartOfGroup(GroupDef *gd);
    void writeNavigationPath(OutputList &ol) const;
    QCString navigationPathAsString() const;
    void writeQuickMemberLinks(OutputList &,const MemberDef *) const;
    void writeSummaryLinks(OutputList &) const;
    QCString pathFragment() const;
    void writeDocAnchorsToTagFile(TextStream &) const;
    void setLocalName(const QCString &name);
    void writeToc(OutputList &ol, const LocalToc &lt) const;
    void computeTooltip();
    void _setSymbolName(const QCString &name);
    QCString _symbolName() const;


  private:

    int  _getXRefListId(const QCString &listName) const;
    void _writeSourceRefList(OutputList &ol,const QCString &scopeName,const QCString &text,
                       const std::unordered_map<std::string,MemberDef *> &members,bool) const;
    void _setBriefDescription(const QCString &b,const QCString &briefFile,int briefLine);
    void _setDocumentation(const QCString &d,const QCString &docFile,int docLine,bool stripWhiteSpace,bool atTop);
    void _setInbodyDocumentation(const QCString &d,const QCString &docFile,int docLine);
    bool _docsAlreadyAdded(const QCString &doc,QCString &sigList);

    // PIMPL idiom
    class IMPL;
    std::unique_ptr<IMPL> m_impl; // internal structure holding all private data
};

template<class Base>
class DefinitionMixin : public Base
{
  public:
    /*! Create a new definition */
    DefinitionMixin(
        const QCString &defFileName,int defLine,int defColumn,
        const QCString &name,const char *b=nullptr,const char *d=nullptr,
        bool isSymbol=TRUE) : m_impl(this,defFileName,defLine,defColumn,name,b,d,isSymbol) {}
    DefinitionMixin(const DefinitionMixin &other) : Base(other), m_impl(other.m_impl) {}
    DefinitionMixin &operator=(const DefinitionMixin &other) { if (this!=&other) { m_impl = other.m_impl; }; return *this; }
    DefinitionMixin(DefinitionMixin &&) = delete;
    DefinitionMixin &operator=(DefinitionMixin &&) = delete;
   ~DefinitionMixin() override = default;

    bool isAlias() const override { return FALSE; }

    //======== Definition
    const QCString &name() const override { return m_impl.name(); }
    bool isAnonymous() const override { return m_impl.isAnonymous(); }
    const QCString &localName() const override { return m_impl.localName(); }
    QCString qualifiedName() const override { return m_impl.qualifiedName(); }
    QCString symbolName() const override { return m_impl.symbolName(); }
    QCString getSourceFileBase() const override { return m_impl.getSourceFileBase(); }
    QCString getSourceAnchor() const override { return m_impl.getSourceAnchor(); }
    QCString documentation() const override { return m_impl.documentation(); }
    int docLine() const override { return m_impl.docLine(); }
    QCString docFile() const override { return m_impl.docFile(); }
    QCString briefDescription(bool abbreviate=FALSE) const override { return m_impl.briefDescription(abbreviate); }
    QCString briefDescriptionAsTooltip() const override { return m_impl.briefDescriptionAsTooltip(); }
    int briefLine() const override { return m_impl.briefLine(); }
    QCString inbodyDocumentation() const override { return m_impl.inbodyDocumentation(); }
    QCString inbodyFile() const override { return m_impl.inbodyFile(); }
    int inbodyLine() const override { return m_impl.inbodyLine(); }
    QCString briefFile() const override { return m_impl.briefFile(); }
    QCString getDefFileName() const override { return m_impl.getDefFileName(); }
    QCString getDefFileExtension() const override { return m_impl.getDefFileExtension(); }
    int getDefLine() const override { return m_impl.getDefLine(); }
    int getDefColumn() const override { return m_impl.getDefColumn(); }
    bool hasDocumentation() const override { return m_impl.hasDocumentation(); }
    bool hasUserDocumentation() const override { return m_impl.hasUserDocumentation(); }
    bool isVisibleInProject() const override { return m_impl.isVisibleInProject(); }
    bool isVisible() const override { return m_impl.isVisible(); }
    bool isHidden() const override { return m_impl.isHidden(); }
    bool isArtificial() const override { return m_impl.isArtificial(); }
    bool isExported() const override { return m_impl.isExported(); }
    QCString getReference() const override { return m_impl.getReference(); }
    bool isReference() const override { return m_impl.isReference(); }
    QCString externalReference(const QCString &relPath) const override { return m_impl.externalReference(relPath); }
    int getStartDefLine() const override { return m_impl.getStartDefLine(); }
    int getStartBodyLine() const override { return m_impl.getStartBodyLine(); }
    int getEndBodyLine() const override { return m_impl.getEndBodyLine(); }
    const FileDef *getBodyDef() const override { return m_impl.getBodyDef(); }
    SrcLangExt getLanguage() const override { return m_impl.getLanguage(); }
    const GroupList &partOfGroups() const override { return m_impl.partOfGroups(); }
    bool isLinkableViaGroup() const override { return m_impl.isLinkableViaGroup(); }
    const RefItemVector &xrefListItems() const override { return m_impl.xrefListItems(); }
    const Definition *findInnerCompound(const QCString &name) const override { return m_impl.findInnerCompound(name); }
    Definition *getOuterScope() const override { return m_impl.getOuterScope(); }
    const MemberVector &getReferencesMembers() const override { return m_impl.getReferencesMembers(); }
    const MemberVector &getReferencedByMembers() const override { return m_impl.getReferencedByMembers(); }
    bool hasSections() const override { return m_impl.hasSections(); }
    bool hasSources() const override { return m_impl.hasSources(); }
    bool hasBriefDescription() const override { return m_impl.hasBriefDescription(); }
    QCString id() const override { return m_impl.id(); }
    const SectionRefs &getSectionRefs() const override { return m_impl.getSectionRefs(); }

    //======== DefinitionMutable
    void setName(const QCString &name) override { m_impl.setName(name); }
    void setId(const QCString &name) override { m_impl.setId(name); }
    void setDefFile(const QCString& df,int defLine,int defColumn) override
    { m_impl.setDefFile(df,defLine,defColumn); }
    void setDocumentation(const QCString &doc,const QCString &docFile,int docLine,bool stripWhiteSpace=TRUE) override
    { m_impl.setDocumentation(doc,docFile,docLine,stripWhiteSpace); }
    void setBriefDescription(const QCString &brief,const QCString &briefFile,int briefLine) override
    { m_impl.setBriefDescription(brief,briefFile,briefLine); }
    void setInbodyDocumentation(const QCString &doc,const QCString &docFile,int docLine) override
    { m_impl.setInbodyDocumentation(doc,docFile,docLine); }
    void setReference(const QCString &r) override
    { m_impl.setReference(r); }
    void addSectionsToDefinition(const std::vector<const SectionInfo*> &anchorList) override
    { m_impl.addSectionsToDefinition(anchorList); }
    void setBodySegment(int defLine,int bls,int ble) override
    { m_impl.setBodySegment(defLine,bls,ble); }
    void setBodyDef(const FileDef *fd) override
    { m_impl.setBodyDef(fd); }
    void addSourceReferencedBy(MemberDef *md,const QCString &sourceRefName) override
    { m_impl.addSourceReferencedBy(md,sourceRefName); }
    void addSourceReferences(MemberDef *md,const QCString &sourceRefName) override
    { m_impl.addSourceReferences(md,sourceRefName); }
    void setRefItems(const RefItemVector &sli) override
    { m_impl.setRefItems(sli); }
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
    void writeInlineCode(OutputList &ol,const QCString &scopeName) const override
    { m_impl.writeInlineCode(ol,scopeName); }
    bool hasSourceRefs() const override
    { return m_impl.hasSourceRefs(); }
    bool hasSourceReffedBy() const override
    { return m_impl.hasSourceReffedBy(); }
    void writeSourceRefs(OutputList &ol,const QCString &scopeName) const override
    { m_impl.writeSourceRefs(ol,scopeName); }
    void writeSourceReffedBy(OutputList &ol,const QCString &scopeName) const override
    { m_impl.writeSourceReffedBy(ol,scopeName); }
    void makePartOfGroup(GroupDef *gd) override
    { m_impl.makePartOfGroup(gd); }
    void writeNavigationPath(OutputList &ol) const override
    { m_impl.writeNavigationPath(ol); }
    QCString navigationPathAsString() const override
    { return m_impl.navigationPathAsString(); }
    void writeQuickMemberLinks(OutputList &ol,const MemberDef *md) const override
    { m_impl.writeQuickMemberLinks(ol,md); }
    void writeSummaryLinks(OutputList &ol) const override
    { m_impl.writeSummaryLinks(ol); }
    QCString pathFragment() const override
    { return m_impl.pathFragment(); }
    void writeDocAnchorsToTagFile(TextStream &fs) const override
    { m_impl.writeDocAnchorsToTagFile(fs); }
    void setLocalName(const QCString &name) override
    { m_impl.setLocalName(name); }
    void writeToc(OutputList &ol, const LocalToc &lt) const override
    { m_impl.writeToc(ol,lt); }
    void computeTooltip() override
    { m_impl.computeTooltip(); }
    void _setSymbolName(const QCString &name) override
    { m_impl._setSymbolName(name); }
    QCString _symbolName() const override
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
    const QCString &name() const;
    QCString qualifiedName() const;
  private:
    Definition *m_def;
    const Definition *m_scope;
    QCString m_symbolName;
    QCString m_qualifiedName;
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


    bool isAlias() const override { return TRUE; }

    //======== Definition
    const QCString &name() const override
    { return m_impl.name(); }
    bool isAnonymous() const override
    { return m_alias->isAnonymous(); }
    const QCString &localName() const override
    { return m_alias->localName(); }
    QCString qualifiedName() const override
    { return m_impl.qualifiedName(); }
    QCString symbolName() const override
    { return m_alias->symbolName(); }
    QCString getSourceFileBase() const override
    { return m_alias->getSourceFileBase(); }
    QCString getSourceAnchor() const override
    { return m_alias->getSourceAnchor(); }
    QCString documentation() const override
    { return m_alias->documentation(); }
    int docLine() const override
    { return m_alias->docLine(); }
    QCString docFile() const override
    { return m_alias->docFile(); }
    QCString briefDescription(bool abbreviate=FALSE) const override
    { return m_alias->briefDescription(abbreviate); }
    QCString briefDescriptionAsTooltip() const override
    { return m_alias->briefDescriptionAsTooltip(); }
    int briefLine() const override
    { return m_alias->briefLine(); }
    QCString inbodyDocumentation() const override
    { return m_alias->inbodyDocumentation(); }
    QCString inbodyFile() const override
    { return m_alias->inbodyFile(); }
    int inbodyLine() const override
    { return m_alias->inbodyLine(); }
    QCString briefFile() const override
    { return m_alias->briefFile(); }
    QCString getDefFileName() const override
    { return m_alias->getDefFileName(); }
    QCString getDefFileExtension() const override
    { return m_alias->getDefFileExtension(); }
    int getDefLine() const override
    { return m_alias->getDefLine(); }
    int getDefColumn() const override
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
    QCString getReference() const override
    { return m_alias->getReference(); }
    bool isReference() const override
    { return m_alias->isReference(); }
    QCString externalReference(const QCString &relPath) const override
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
    const Definition *findInnerCompound(const QCString &name) const override
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
    QCString id() const override
    { return m_alias->id(); }
    const SectionRefs &getSectionRefs() const override
    { return m_alias->getSectionRefs(); }
    QCString navigationPathAsString() const override
    { return m_alias->navigationPathAsString(); }
    QCString pathFragment() const override
    { return m_alias->pathFragment(); }

  protected:
    const Definition *getAlias() const { return m_alias; }
    const Definition *getScope() const { return m_scope; }

  private:
    virtual Definition *toDefinition_() { return this; }
    DefinitionMutable *toDefinitionMutable_() override { return nullptr; }
    const DefinitionImpl *toDefinitionImpl_() const override { return nullptr; }

    void _setSymbolName(const QCString &name) override { m_symbolName = name; }
    QCString _symbolName() const override { return m_symbolName; }
    DefinitionAliasImpl m_impl;
    const Definition *m_scope;
    const Definition *m_alias;
    QCString m_symbolName;
};

#endif
