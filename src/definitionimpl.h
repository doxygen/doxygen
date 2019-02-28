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

    virtual QCString name() const;
    virtual QCString displayName(bool includeScope=TRUE) const = 0;
    virtual QCString localName() const;
    virtual QCString qualifiedName() const;
    virtual QCString symbolName() const;
    virtual QCString getOutputFileBase() const = 0;
    virtual QCString anchor() const = 0;
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
    virtual bool isLinkableInProject() const = 0;
    virtual bool isLinkable() const = 0;
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
    virtual QList<ListItemInfo> *xrefListItems() const;
    virtual Definition *findInnerCompound(const char *name) const;
    virtual Definition *getOuterScope() const;
    virtual MemberSDict *getReferencesMembers() const;
    virtual MemberSDict *getReferencedByMembers() const;
    virtual bool hasSections() const;
    virtual bool hasSources() const;
    virtual bool hasBriefDescription() const;
    virtual QCString id() const;
    virtual SectionDict * getSectionDict(void);
    virtual void setName(const char *name);
    virtual void setId(const char *name);
    virtual void setDefFile(const QCString& df,int defLine,int defColumn);
    virtual void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE);
    virtual void setBriefDescription(const char *b,const char *briefFile,int briefLine);
    virtual void setInbodyDocumentation(const char *d,const char *docFile,int docLine);
    virtual void setReference(const char *r);
    virtual void addSectionsToDefinition(QList<SectionInfo> *anchorList);
    virtual void setBodySegment(int bls,int ble);
    virtual void setBodyDef(FileDef *fd);
    virtual void addSourceReferencedBy(MemberDef *d);
    virtual void addSourceReferences(MemberDef *d);
    virtual void setRefItems(const QList<ListItemInfo> *sli);
    virtual void mergeRefItems(Definition *d);
    virtual void addInnerCompound(Definition *d);
    virtual void setOuterScope(Definition *d);
    virtual void setHidden(bool b);
    virtual void setArtificial(bool b);
    virtual void setLanguage(SrcLangExt lang);
    virtual void writeSourceDef(OutputList &ol,const char *scopeName);
    virtual void writeInlineCode(OutputList &ol,const char *scopeName);
    virtual void writeSourceRefs(OutputList &ol,const char *scopeName);
    virtual void writeSourceReffedBy(OutputList &ol,const char *scopeName);
    virtual void makePartOfGroup(GroupDef *gd);
    virtual void writeNavigationPath(OutputList &ol) const;
    virtual QCString navigationPathAsString() const;
    virtual void writeQuickMemberLinks(OutputList &,MemberDef *) const;
    virtual void writeSummaryLinks(OutputList &);
    virtual QCString pathFragment() const;
    virtual void writeDocAnchorsToTagFile(FTextStream &);
    virtual void setLocalName(const QCString name);
    virtual void addSectionsToIndex();
    virtual void writeToc(OutputList &ol, const LocalToc &lt);
    virtual void setCookie(Cookie *cookie);
    virtual Cookie *cookie() const;

  protected:

    DefinitionImpl(const DefinitionImpl &d);

  private: 
    static void addToMap(const char *name,Definition *d);
    static void removeFromMap(Definition *d);

    virtual void _setSymbolName(const QCString &name);
    virtual QCString _symbolName() const ;

    int  _getXRefListId(const char *listName) const;
    void _writeSourceRefList(OutputList &ol,const char *scopeName,
                       const QCString &text,MemberSDict *members,bool);
    void _setBriefDescription(const char *b,const char *briefFile,int briefLine);
    void _setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace,bool atTop);
    void _setInbodyDocumentation(const char *d,const char *docFile,int docLine);
    bool _docsAlreadyAdded(const QCString &doc,QCString &sigList);

    // PIMPL idiom
    class IMPL;
    IMPL *m_impl; // internal structure holding all private data
};


#endif
