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
        const QCString &name,const char *b=0,const char *d=0,
        bool isSymbol=TRUE);
    virtual ~DefinitionImpl();

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
    void addSourceReferencedBy(MemberDef *d);
    void addSourceReferences(MemberDef *d);
    void setRefItems(const RefItemVector &sli);
    void mergeRefItems(Definition *d);
    void mergeReferences(const Definition *other);
    void mergeReferencedBy(const Definition *other);
    void addInnerCompound(Definition *d);
    void setOuterScope(Definition *d);
    void setHidden(bool b);
    void setArtificial(bool b);
    void setLanguage(SrcLangExt lang);
    void writeSourceDef(OutputList &ol,const QCString &scopeName) const;
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
    void addSectionsToIndex();
    void writeToc(OutputList &ol, const LocalToc &lt) const;
    void computeTooltip();
    void _setSymbolName(const QCString &name);
    QCString _symbolName() const;

    DefinitionImpl(const DefinitionImpl &d);

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
        const QCString &name,const char *b=0,const char *d=0,
        bool isSymbol=TRUE) : m_impl(this,defFileName,defLine,defColumn,name,b,d,isSymbol) {}
    virtual ~DefinitionMixin() = default;

    virtual bool isAlias() const { return FALSE; }

    //======== Definition
    virtual const QCString &name() const { return m_impl.name(); }
    virtual bool isAnonymous() const { return m_impl.isAnonymous(); }
    virtual const QCString &localName() const { return m_impl.localName(); }
    virtual QCString qualifiedName() const { return m_impl.qualifiedName(); }
    virtual QCString symbolName() const { return m_impl.symbolName(); }
    virtual QCString getSourceFileBase() const { return m_impl.getSourceFileBase(); }
    virtual QCString getSourceAnchor() const { return m_impl.getSourceAnchor(); }
    virtual QCString documentation() const { return m_impl.documentation(); }
    virtual int docLine() const { return m_impl.docLine(); }
    virtual QCString docFile() const { return m_impl.docFile(); }
    virtual QCString briefDescription(bool abbreviate=FALSE) const { return m_impl.briefDescription(abbreviate); }
    virtual QCString briefDescriptionAsTooltip() const { return m_impl.briefDescriptionAsTooltip(); }
    virtual int briefLine() const { return m_impl.briefLine(); }
    virtual QCString inbodyDocumentation() const { return m_impl.inbodyDocumentation(); }
    virtual QCString inbodyFile() const { return m_impl.inbodyFile(); }
    virtual int inbodyLine() const { return m_impl.inbodyLine(); }
    virtual QCString briefFile() const { return m_impl.briefFile(); }
    virtual QCString getDefFileName() const { return m_impl.getDefFileName(); }
    virtual QCString getDefFileExtension() const { return m_impl.getDefFileExtension(); }
    virtual int getDefLine() const { return m_impl.getDefLine(); }
    virtual int getDefColumn() const { return m_impl.getDefColumn(); }
    virtual bool hasDocumentation() const { return m_impl.hasDocumentation(); }
    virtual bool hasUserDocumentation() const { return m_impl.hasUserDocumentation(); }
    virtual bool isVisibleInProject() const { return m_impl.isVisibleInProject(); }
    virtual bool isVisible() const { return m_impl.isVisible(); }
    virtual bool isHidden() const { return m_impl.isHidden(); }
    virtual bool isArtificial() const { return m_impl.isArtificial(); }
    virtual QCString getReference() const { return m_impl.getReference(); }
    virtual bool isReference() const { return m_impl.isReference(); }
    virtual QCString externalReference(const QCString &relPath) const { return m_impl.externalReference(relPath); }
    virtual int getStartDefLine() const { return m_impl.getStartDefLine(); }
    virtual int getStartBodyLine() const { return m_impl.getStartBodyLine(); }
    virtual int getEndBodyLine() const { return m_impl.getEndBodyLine(); }
    virtual const FileDef *getBodyDef() const { return m_impl.getBodyDef(); }
    virtual SrcLangExt getLanguage() const { return m_impl.getLanguage(); }
    virtual const GroupList &partOfGroups() const { return m_impl.partOfGroups(); }
    virtual bool isLinkableViaGroup() const { return m_impl.isLinkableViaGroup(); }
    virtual const RefItemVector &xrefListItems() const { return m_impl.xrefListItems(); }
    virtual const Definition *findInnerCompound(const QCString &name) const { return m_impl.findInnerCompound(name); }
    virtual Definition *getOuterScope() const { return m_impl.getOuterScope(); }
    virtual const MemberVector &getReferencesMembers() const { return m_impl.getReferencesMembers(); }
    virtual const MemberVector &getReferencedByMembers() const { return m_impl.getReferencedByMembers(); }
    virtual bool hasSections() const { return m_impl.hasSections(); }
    virtual bool hasSources() const { return m_impl.hasSources(); }
    virtual bool hasBriefDescription() const { return m_impl.hasBriefDescription(); }
    virtual QCString id() const { return m_impl.id(); }
    virtual const SectionRefs &getSectionRefs() const { return m_impl.getSectionRefs(); }

    //======== DefinitionMutable
    virtual void setName(const QCString &name) { m_impl.setName(name); }
    virtual void setId(const QCString &name) { m_impl.setId(name); }
    virtual void setDefFile(const QCString& df,int defLine,int defColumn)
    { m_impl.setDefFile(df,defLine,defColumn); }
    virtual void setDocumentation(const QCString &doc,const QCString &docFile,int docLine,bool stripWhiteSpace=TRUE)
    { m_impl.setDocumentation(doc,docFile,docLine,stripWhiteSpace); }
    virtual void setBriefDescription(const QCString &brief,const QCString &briefFile,int briefLine)
    { m_impl.setBriefDescription(brief,briefFile,briefLine); }
    virtual void setInbodyDocumentation(const QCString &doc,const QCString &docFile,int docLine)
    { m_impl.setInbodyDocumentation(doc,docFile,docLine); }
    virtual void setReference(const QCString &r)
    { m_impl.setReference(r); }
    virtual void addSectionsToDefinition(const std::vector<const SectionInfo*> &anchorList)
    { m_impl.addSectionsToDefinition(anchorList); }
    virtual void setBodySegment(int defLine,int bls,int ble)
    { m_impl.setBodySegment(defLine,bls,ble); }
    virtual void setBodyDef(const FileDef *fd)
    { m_impl.setBodyDef(fd); }
    virtual void addSourceReferencedBy(MemberDef *md)
    { m_impl.addSourceReferencedBy(md); }
    virtual void addSourceReferences(MemberDef *md)
    { m_impl.addSourceReferences(md); }
    virtual void setRefItems(const RefItemVector &sli)
    { m_impl.setRefItems(sli); }
    virtual void mergeRefItems(Definition *def)
    { m_impl.mergeRefItems(def); }
    virtual void mergeReferences(const Definition *other)
    { m_impl.mergeReferences(other); }
    virtual void mergeReferencedBy(const Definition *other)
    { m_impl.mergeReferencedBy(other); }
    virtual void addInnerCompound(Definition *def)
    { m_impl.addInnerCompound(def); }
    virtual void setOuterScope(Definition *def)
    { m_impl.setOuterScope(def); }
    virtual void setHidden(bool b)
    { m_impl.setHidden(b); }
    virtual void setArtificial(bool b)
    { m_impl.setArtificial(b); }
    virtual void setLanguage(SrcLangExt lang)
    { m_impl.setLanguage(lang); }
    virtual void writeSourceDef(OutputList &ol,const QCString &scopeName) const
    { m_impl.writeSourceDef(ol,scopeName); }
    virtual void writeInlineCode(OutputList &ol,const QCString &scopeName) const
    { m_impl.writeInlineCode(ol,scopeName); }
    virtual bool hasSourceRefs() const
    { return m_impl.hasSourceRefs(); }
    virtual bool hasSourceReffedBy() const
    { return m_impl.hasSourceReffedBy(); }
    virtual void writeSourceRefs(OutputList &ol,const QCString &scopeName) const
    { m_impl.writeSourceRefs(ol,scopeName); }
    virtual void writeSourceReffedBy(OutputList &ol,const QCString &scopeName) const
    { m_impl.writeSourceReffedBy(ol,scopeName); }
    virtual void makePartOfGroup(GroupDef *gd)
    { m_impl.makePartOfGroup(gd); }
    virtual void writeNavigationPath(OutputList &ol) const
    { m_impl.writeNavigationPath(ol); }
    virtual QCString navigationPathAsString() const
    { return m_impl.navigationPathAsString(); }
    virtual void writeQuickMemberLinks(OutputList &ol,const MemberDef *md) const
    { m_impl.writeQuickMemberLinks(ol,md); }
    virtual void writeSummaryLinks(OutputList &ol) const
    { m_impl.writeSummaryLinks(ol); }
    virtual QCString pathFragment() const
    { return m_impl.pathFragment(); }
    virtual void writeDocAnchorsToTagFile(TextStream &fs) const
    { m_impl.writeDocAnchorsToTagFile(fs); }
    virtual void setLocalName(const QCString &name)
    { m_impl.setLocalName(name); }
    virtual void addSectionsToIndex()
    { m_impl.addSectionsToIndex(); }
    virtual void writeToc(OutputList &ol, const LocalToc &lt) const
    { m_impl.writeToc(ol,lt); }
    virtual void computeTooltip()
    { m_impl.computeTooltip(); }
    virtual void _setSymbolName(const QCString &name)
    { m_impl._setSymbolName(name); }
    virtual QCString _symbolName() const
    { return m_impl._symbolName(); }

  protected:

    DefinitionMixin(const DefinitionMixin &def) = default;

  private:
    virtual Definition *toDefinition_() { return this; }
    virtual DefinitionMutable *toDefinitionMutable_() { return this; }
    virtual const DefinitionImpl *toDefinitionImpl_() const { return &m_impl; }

    DefinitionImpl m_impl;
};

class DefinitionAliasImpl
{
  public:
    DefinitionAliasImpl(Definition *def,const Definition *scope,const Definition *alias);
    virtual ~DefinitionAliasImpl();
    void init();
    void deinit();
    const QCString &name() const;
    QCString qualifiedName() const;
  private:
    void updateQualifiedName() const;
    Definition *m_def;
    const Definition *m_scope;
    QCString m_symbolName;
    mutable QCString m_qualifiedName;
};

template<class Base>
class DefinitionAliasMixin : public Base
{
  public:
    DefinitionAliasMixin(const Definition *scope,const Definition *alias)
      : m_impl(this,scope,alias), m_scope(scope), m_alias(alias) {}

    void init() { m_impl.init(); }
    void deinit() { m_impl.deinit(); }

    virtual ~DefinitionAliasMixin() = default;

    virtual bool isAlias() const { return TRUE; }

    //======== Definition
    virtual const QCString &name() const
    { return m_impl.name(); }
    virtual bool isAnonymous() const
    { return m_alias->isAnonymous(); }
    virtual const QCString &localName() const
    { return m_alias->localName(); }
    virtual QCString qualifiedName() const
    { return m_impl.qualifiedName(); }
    virtual QCString symbolName() const
    { return m_alias->symbolName(); }
    virtual QCString getSourceFileBase() const
    { return m_alias->getSourceFileBase(); }
    virtual QCString getSourceAnchor() const
    { return m_alias->getSourceAnchor(); }
    virtual QCString documentation() const
    { return m_alias->documentation(); }
    virtual int docLine() const
    { return m_alias->docLine(); }
    virtual QCString docFile() const
    { return m_alias->docFile(); }
    virtual QCString briefDescription(bool abbreviate=FALSE) const
    { return m_alias->briefDescription(abbreviate); }
    virtual QCString briefDescriptionAsTooltip() const
    { return m_alias->briefDescriptionAsTooltip(); }
    virtual int briefLine() const
    { return m_alias->briefLine(); }
    virtual QCString inbodyDocumentation() const
    { return m_alias->inbodyDocumentation(); }
    virtual QCString inbodyFile() const
    { return m_alias->inbodyFile(); }
    virtual int inbodyLine() const
    { return m_alias->inbodyLine(); }
    virtual QCString briefFile() const
    { return m_alias->briefFile(); }
    virtual QCString getDefFileName() const
    { return m_alias->getDefFileName(); }
    virtual QCString getDefFileExtension() const
    { return m_alias->getDefFileExtension(); }
    virtual int getDefLine() const
    { return m_alias->getDefLine(); }
    virtual int getDefColumn() const
    { return m_alias->getDefColumn(); }
    virtual bool hasDocumentation() const
    { return m_alias->hasDocumentation(); }
    virtual bool hasUserDocumentation() const
    { return m_alias->hasUserDocumentation(); }
    virtual bool isVisibleInProject() const
    { return m_alias->isVisibleInProject(); }
    virtual bool isVisible() const
    { return m_alias->isVisible(); }
    virtual bool isHidden() const
    { return m_alias->isHidden(); }
    virtual bool isArtificial() const
    { return m_alias->isArtificial(); }
    virtual QCString getReference() const
    { return m_alias->getReference(); }
    virtual bool isReference() const
    { return m_alias->isReference(); }
    virtual QCString externalReference(const QCString &relPath) const
    { return m_alias->externalReference(relPath); }
    virtual int getStartDefLine() const
    { return m_alias->getStartDefLine(); }
    virtual int getStartBodyLine() const
    { return m_alias->getStartBodyLine(); }
    virtual int getEndBodyLine() const
    { return m_alias->getEndBodyLine(); }
    virtual const FileDef *getBodyDef() const
    { return m_alias->getBodyDef(); }
    virtual SrcLangExt getLanguage() const
    { return m_alias->getLanguage(); }
    virtual const GroupList &partOfGroups() const
    { return m_alias->partOfGroups(); }
    virtual bool isLinkableViaGroup() const
    { return m_alias->isLinkableViaGroup(); }
    virtual const RefItemVector &xrefListItems() const
    { return m_alias->xrefListItems(); }
    virtual const Definition *findInnerCompound(const QCString &name) const
    { return m_alias->findInnerCompound(name); }
    virtual Definition *getOuterScope() const
    { return const_cast<Definition*>(m_scope); }
    virtual const MemberVector &getReferencesMembers() const
    { return m_alias->getReferencesMembers(); }
    virtual const MemberVector &getReferencedByMembers() const
    { return m_alias->getReferencedByMembers(); }
    virtual bool hasSections() const
    { return m_alias->hasSections(); }
    virtual bool hasSources() const
    { return m_alias->hasSources(); }
    virtual bool hasBriefDescription() const
    { return m_alias->hasBriefDescription(); }
    virtual QCString id() const
    { return m_alias->id(); }
    virtual const SectionRefs &getSectionRefs() const
    { return m_alias->getSectionRefs(); }
    virtual QCString navigationPathAsString() const
    { return m_alias->navigationPathAsString(); }
    virtual QCString pathFragment() const
    { return m_alias->pathFragment(); }

  protected:
    const Definition *getAlias() const { return m_alias; }
    const Definition *getScope() const { return m_scope; }

  private:
    virtual Definition *toDefinition_() { return this; }
    virtual DefinitionMutable *toDefinitionMutable_() { return 0; }
    virtual const DefinitionImpl *toDefinitionImpl_() const { return 0; }

    virtual void _setSymbolName(const QCString &name) { m_symbolName = name; }
    virtual QCString _symbolName() const { return m_symbolName; }
    DefinitionAliasImpl m_impl;
    const Definition *m_scope;
    const Definition *m_alias;
    QCString m_symbolName;
};

#endif
