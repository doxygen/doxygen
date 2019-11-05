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

#include "definition.h"

class DefinitionImpl : virtual public Definition
{
  public:
    /*! Create a new definition */
    DefinitionImpl(
        const char *defFileName,int defLine,int defColumn,
        const char *name,const char *b=0,const char *d=0,
        bool isSymbol=TRUE);
    virtual ~DefinitionImpl();

    virtual bool isAlias() const { return FALSE; }
    virtual QCString name() const;
    virtual bool isAnonymous() const;
    virtual QCString localName() const;
    virtual QCString qualifiedName() const;
    virtual QCString symbolName() const;
    virtual QCString getSourceFileBase() const;
    virtual QCString getSourceAnchor() const;
    virtual QCString documentation() const;
    virtual int docLine() const;
    virtual QCString docFile() const;
    virtual QCString briefDescription(bool abbreviate=FALSE) const;
    virtual QCString briefDescriptionAsTooltip() const;
    virtual int briefLine() const;
    virtual QCString inbodyDocumentation() const;
    virtual QCString inbodyFile() const;
    virtual int inbodyLine() const;
    virtual QCString briefFile() const;
    virtual QCString getDefFileName() const;
    virtual QCString getDefFileExtension() const;
    virtual int getDefLine() const;
    virtual int getDefColumn() const;
    virtual bool hasDocumentation() const;
    virtual bool hasUserDocumentation() const;
    virtual bool isVisibleInProject() const;
    virtual bool isVisible() const;
    virtual bool isHidden() const;
    virtual bool isArtificial() const;
    virtual QCString getReference() const;
    virtual bool isReference() const;
    virtual QCString externalReference(const QCString &relPath) const;
    virtual int getStartBodyLine() const;
    virtual int getEndBodyLine() const;
    virtual FileDef *getBodyDef() const;
    virtual SrcLangExt getLanguage() const;
    virtual GroupList *partOfGroups() const;
    virtual bool isLinkableViaGroup() const;
    virtual const std::vector<ListItemInfo> &xrefListItems() const;
    virtual Definition *findInnerCompound(const char *name) const;
    virtual Definition *getOuterScope() const;
    virtual MemberSDict *getReferencesMembers() const;
    virtual MemberSDict *getReferencedByMembers() const;
    virtual bool hasSections() const;
    virtual bool hasSources() const;
    virtual bool hasBriefDescription() const;
    virtual QCString id() const;
    virtual SectionDict * getSectionDict() const;
    virtual void setName(const char *name);
    virtual void setId(const char *name);
    virtual void setDefFile(const QCString& df,int defLine,int defColumn);
    virtual void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE);
    virtual void setBriefDescription(const char *b,const char *briefFile,int briefLine);
    virtual void setInbodyDocumentation(const char *d,const char *docFile,int docLine);
    virtual void setReference(const char *r);
    virtual void addSectionsToDefinition(const std::vector<const SectionInfo*> &anchorList);
    virtual void setBodySegment(int bls,int ble);
    virtual void setBodyDef(FileDef *fd);
    virtual void addSourceReferencedBy(const MemberDef *d);
    virtual void addSourceReferences(const MemberDef *d);
    virtual void setRefItems(const std::vector<ListItemInfo> &sli);
    virtual void mergeRefItems(Definition *d);
    virtual void addInnerCompound(const Definition *d);
    virtual void setOuterScope(Definition *d);
    virtual void setHidden(bool b);
    virtual void setArtificial(bool b);
    virtual void setLanguage(SrcLangExt lang);
    virtual void writeSourceDef(OutputList &ol,const char *scopeName) const;
    virtual void writeInlineCode(OutputList &ol,const char *scopeName) const;
    virtual void writeSourceRefs(OutputList &ol,const char *scopeName) const;
    virtual void writeSourceReffedBy(OutputList &ol,const char *scopeName) const;
    virtual void makePartOfGroup(GroupDef *gd);
    virtual void writeNavigationPath(OutputList &ol) const;
    virtual QCString navigationPathAsString() const;
    virtual void writeQuickMemberLinks(OutputList &,const MemberDef *) const;
    virtual void writeSummaryLinks(OutputList &) const;
    virtual QCString pathFragment() const;
    virtual void writeDocAnchorsToTagFile(FTextStream &) const;
    virtual void setLocalName(const QCString name);
    virtual void addSectionsToIndex();
    virtual void writeToc(OutputList &ol, const LocalToc &lt) const;
    virtual void setCookie(Cookie *cookie) const;
    virtual Cookie *cookie() const;

  protected:

    DefinitionImpl(const DefinitionImpl &d);

  private: 
    virtual void _setSymbolName(const QCString &name);
    virtual QCString _symbolName() const ;

    int  _getXRefListId(const char *listName) const;
    void _writeSourceRefList(OutputList &ol,const char *scopeName,
                       const QCString &text,MemberSDict *members,bool) const;
    void _setBriefDescription(const char *b,const char *briefFile,int briefLine);
    void _setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace,bool atTop);
    void _setInbodyDocumentation(const char *d,const char *docFile,int docLine);
    bool _docsAlreadyAdded(const QCString &doc,QCString &sigList);

    // PIMPL idiom
    class IMPL;
    IMPL *m_impl; // internal structure holding all private data
};

class DefinitionAliasImpl : virtual public Definition
{
  public:
    DefinitionAliasImpl(const Definition *scope,const Definition *alias);
    virtual ~DefinitionAliasImpl();

    virtual bool isAlias() const { return TRUE; }
    virtual QCString name() const
    { return m_def->name(); }
    virtual bool isAnonymous() const
    { return m_def->isAnonymous(); }
    virtual QCString localName() const
    { return m_def->localName(); }
    virtual QCString qualifiedName() const
    { return m_def->qualifiedName(); }
    virtual QCString symbolName() const
    { return m_def->symbolName(); }
    virtual QCString getSourceFileBase() const
    { return m_def->getSourceFileBase(); }
    virtual QCString getSourceAnchor() const
    { return m_def->getSourceAnchor(); }
    virtual QCString documentation() const
    { return m_def->documentation(); }
    virtual int docLine() const
    { return m_def->docLine(); }
    virtual QCString docFile() const
    { return m_def->docFile(); }
    virtual QCString briefDescription(bool abbreviate=FALSE) const
    { return m_def->briefDescription(abbreviate); }
    virtual QCString briefDescriptionAsTooltip() const
    { return m_def->briefDescriptionAsTooltip(); }
    virtual int briefLine() const
    { return m_def->briefLine(); }
    virtual QCString inbodyDocumentation() const
    { return m_def->inbodyDocumentation(); }
    virtual QCString inbodyFile() const
    { return m_def->inbodyFile(); }
    virtual int inbodyLine() const
    { return m_def->inbodyLine(); }
    virtual QCString briefFile() const
    { return m_def->briefFile(); }
    virtual QCString getDefFileName() const
    { return m_def->getDefFileName(); }
    virtual QCString getDefFileExtension() const
    { return m_def->getDefFileExtension(); }
    virtual int getDefLine() const
    { return m_def->getDefLine(); }
    virtual int getDefColumn() const
    { return m_def->getDefColumn(); }
    virtual bool hasDocumentation() const
    { return m_def->hasDocumentation(); }
    virtual bool hasUserDocumentation() const
    { return m_def->hasUserDocumentation(); }
    virtual bool isVisibleInProject() const
    { return m_def->isVisibleInProject(); }
    virtual bool isVisible() const
    { return m_def->isVisible(); }
    virtual bool isHidden() const
    { return m_def->isHidden(); }
    virtual bool isArtificial() const
    { return m_def->isArtificial(); }
    virtual QCString getReference() const
    { return m_def->getReference(); }
    virtual bool isReference() const
    { return m_def->isReference(); }
    virtual QCString externalReference(const QCString &relPath) const
    { return m_def->externalReference(relPath); }
    virtual int getStartBodyLine() const
    { return m_def->getStartBodyLine(); }
    virtual int getEndBodyLine() const
    { return m_def->getEndBodyLine(); }
    virtual FileDef *getBodyDef() const
    { return m_def->getBodyDef(); }
    virtual SrcLangExt getLanguage() const
    { return m_def->getLanguage(); }
    virtual GroupList *partOfGroups() const
    { return m_def->partOfGroups(); }
    virtual bool isLinkableViaGroup() const
    { return m_def->isLinkableViaGroup(); }
    virtual const std::vector<ListItemInfo> &xrefListItems() const
    { return m_def->xrefListItems(); }
    virtual Definition *findInnerCompound(const char *name) const
    { return m_def->findInnerCompound(name); }
    virtual Definition *getOuterScope() const
    { return const_cast<Definition*>(m_scope); }
    virtual MemberSDict *getReferencesMembers() const
    { return m_def->getReferencesMembers(); }
    virtual MemberSDict *getReferencedByMembers() const
    { return m_def->getReferencedByMembers(); }
    virtual bool hasSections() const
    { return m_def->hasSections(); }
    virtual bool hasSources() const
    { return m_def->hasSources(); }
    virtual bool hasBriefDescription() const
    { return m_def->hasBriefDescription(); }
    virtual QCString id() const
    { return m_def->id(); }
    virtual SectionDict * getSectionDict() const
    { return m_def->getSectionDict(); }
    virtual QCString navigationPathAsString() const 
    { return m_def->navigationPathAsString(); }
    virtual QCString pathFragment() const 
    { return m_def->pathFragment(); }
    virtual void setName(const char *name) { }
    virtual void setId(const char *name) { }
    virtual void setDefFile(const QCString& df,int defLine,int defColumn) {}
    virtual void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE) {}
    virtual void setBriefDescription(const char *b,const char *briefFile,int briefLine) {}
    virtual void setInbodyDocumentation(const char *d,const char *docFile,int docLine) {}
    virtual void setReference(const char *r) {}
    virtual void addSectionsToDefinition(const std::vector<const SectionInfo*> &anchorList) {}
    virtual void setBodySegment(int bls,int ble) {}
    virtual void setBodyDef(FileDef *fd) {}
    virtual void addSourceReferencedBy(const MemberDef *d) {}
    virtual void addSourceReferences(const MemberDef *d) {}
    virtual void setRefItems(const std::vector<ListItemInfo> &sli) {}
    virtual void mergeRefItems(Definition *d) {}
    virtual void addInnerCompound(const Definition *d) {}
    virtual void setOuterScope(Definition *d) {}
    virtual void setHidden(bool b) {}
    virtual void setArtificial(bool b) {}
    virtual void setLanguage(SrcLangExt lang) {}
    virtual void writeSourceDef(OutputList &ol,const char *scopeName) const {}
    virtual void writeInlineCode(OutputList &ol,const char *scopeName) const {}
    virtual void writeSourceRefs(OutputList &ol,const char *scopeName) const {}
    virtual void writeSourceReffedBy(OutputList &ol,const char *scopeName) const {}
    virtual void makePartOfGroup(GroupDef *gd) {}
    virtual void writeNavigationPath(OutputList &ol) const {}
    virtual void writeQuickMemberLinks(OutputList &,const MemberDef *) const {}
    virtual void writeSummaryLinks(OutputList &) const {}
    virtual void writeDocAnchorsToTagFile(FTextStream &) const {}
    virtual void setLocalName(const QCString name) {}
    virtual void addSectionsToIndex() {}
    virtual void writeToc(OutputList &ol, const LocalToc &lt) const {}
    virtual void setCookie(Cookie *cookie) const { delete m_cookie; m_cookie = cookie; }
    virtual Cookie *cookie() const { return m_cookie; }
  protected:
    const Definition *getAlias() const { return m_def; }
    const Definition *getScope() const { return m_scope; }

  private:
    virtual void _setSymbolName(const QCString &name) { m_symbolName = name; }
    virtual QCString _symbolName() const { return m_symbolName; }
    const Definition *m_scope;
    const Definition *m_def;
    mutable Cookie *m_cookie;
    QCString m_symbolName;
};


#endif
