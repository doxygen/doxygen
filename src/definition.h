/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#ifndef DEFINITION_H
#define DEFINITION_H

#include <qlist.h>
#include <qdict.h>

#include "types.h"

class FileDef;
class OutputList;
class SectionDict;
class MemberSDict;
class MemberDef;
class GroupDef;
class GroupList;
struct ListItemInfo;
struct SectionInfo;
class Definition;
class DefinitionImpl;
  
/** Data associated with a detailed description. */
struct DocInfo
{
    QCString doc;  
    int      line;
    QCString file;
};

/** Data associated with a brief description. */
struct BriefInfo
{
    QCString doc;  
    QCString tooltip;  
    int      line;
    QCString file;
};

/** Data associated with description found in the body. */
struct BodyInfo
{
    int      startLine;   //!< line number of the start of the definition
    int      endLine;     //!< line number of the end of the definition
    FileDef *fileDef;     //!< file definition containing the function body
};
    
/** Abstract interface for a Definition or DefinitionList */
class DefinitionIntf
{
  public:
    DefinitionIntf() {}
    virtual ~DefinitionIntf() {}
    /*! Types of derived classes */
    enum DefType 
    { 
      TypeClass      = 0, 
      TypeFile       = 1, 
      TypeNamespace  = 2, 
      TypeMember     = 3, 
      TypeGroup      = 4, 
      TypePackage    = 5, 
      TypePage       = 6, 
      TypeDir        = 7, 
      TypeSymbolList = 8
    };
    /*! Use this for dynamic inspection of the type of the derived class */
    virtual DefType definitionType() const = 0;
};

/** The common base class of all entity definitions found in the sources. 
 *
 *  This can be a class or a member function, or a file, or a namespace, etc.
 *  Use definitionType() to find which type of definition this is.
 */
class Definition : public DefinitionIntf
{
  public:
    
    /*! Create a new definition */
    Definition(
        const char *defFileName,int defLine,int defColumn,
        const char *name,const char *b=0,const char *d=0,
        bool isSymbol=TRUE);

    /*! Destroys the definition */
    virtual ~Definition();

    //-----------------------------------------------------------------------------------
    // ----  getters -----
    //-----------------------------------------------------------------------------------

    /*! Returns the name of the definition */
    const QCString& name() const { return m_name; }

    /*! Returns the name of the definition as it appears in the output */
    virtual QCString displayName(bool includeScope=TRUE) const = 0;

    /*! Returns the local name without any scope qualifiers. */
    QCString localName() const;

    /*! Returns the fully qualified name of this definition
     */
    virtual QCString qualifiedName() const;

    /*! Returns the name of this definition as it appears in the symbol map.
     */
    QCString symbolName() const;

    /*! Returns the base file name (without extension) of this definition.
     *  as it is referenced to/written to disk.
     */
    virtual QCString getOutputFileBase() const = 0;

    /*! Returns the anchor within a page where this item can be found */
    virtual QCString anchor() const = 0;

    /*! Returns the name of the source listing of this definition. */
    virtual QCString getSourceFileBase() const;

    /*! Returns the anchor of the source listing of this definition. */
    virtual QCString getSourceAnchor() const;

    /*! Returns the detailed description of this definition */
    QCString documentation() const;
    
    /*! Returns the line number at which the detailed documentation was found. */
    int docLine() const;

    /*! Returns the file in which the detailed documentation block was found. 
     *  This can differ from getDefFileName().
     */
    QCString docFile() const;

    /*! Returns the brief description of this definition. This can include commands. */
    QCString briefDescription(bool abbreviate=FALSE) const;

    /*! Returns a plain text version of the brief description suitable for use
     *  as a tool tip. 
     */
    QCString briefDescriptionAsTooltip() const;

    /*! Returns the line number at which the brief description was found. */
    int briefLine() const;

    /*! Returns the documentation found inside the body of a member */
    QCString inbodyDocumentation() const;

    /*! Returns the file in which the in body documentation was found */
    QCString inbodyFile() const;

    /*! Returns the line at which the first in body documentation 
        part was found */
    int inbodyLine() const;

    /*! Returns the file in which the brief description was found. 
     *  This can differ from getDefFileName().
     */
    QCString briefFile() const;

    /*! returns the file in which this definition was found */
    QCString getDefFileName() const;

    /*! returns the extension of the file in which this definition was found */
    QCString getDefFileExtension() const;

    /*! returns the line number at which the definition was found */
    int getDefLine() const { return m_defLine; }

    /*! returns the column number at which the definition was found */
    int getDefColumn() const { return m_defColumn; }

    /*! Returns TRUE iff the definition is documented 
     *  (which could be generated documentation) 
     *  @see hasUserDocumentation()
     */
    virtual bool hasDocumentation() const;

    /*! Returns TRUE iff the definition is documented by the user. */
    virtual bool hasUserDocumentation() const;

    /*! Returns TRUE iff it is possible to link to this item within this
     *  project. 
     */
    virtual bool isLinkableInProject() const = 0;

    /*! Returns TRUE iff it is possible to link to this item. This can
     *  be a link to another project imported via a tag file. 
     */
    virtual bool isLinkable() const = 0;

    /*! Returns TRUE iff the name is part of this project and 
     *  may appear in the output 
     */
    virtual bool isVisibleInProject() const;

    /*! Returns TRUE iff the name may appear in the output */
    virtual bool isVisible() const;

    /*! Returns TRUE iff this item is supposed to be hidden from the output. */
    bool isHidden() const;

    /*! returns TRUE if this entity was artificially introduced, for 
     *  instance because it is used to show a template instantiation relation. 
     */
    bool isArtificial() const;

    /*! If this definition was imported via a tag file, this function
     *  returns the tagfile for the external project. This can be
     *  translated into an external link target via 
     *  Doxygen::tagDestinationDict
     */
    virtual QCString getReference() const;

    /*! Returns TRUE if this definition is imported via a tag file. */
    virtual bool isReference() const;

    /*! Returns the first line of the body of this item (applicable to classes and 
     *  functions).
     */
    int getStartBodyLine() const;

    /*! Returns the last line of the body of this item (applicable to classes and 
     *  functions).
     */
    int getEndBodyLine() const;

    /*! Returns the file in which the body of this item is located or 0 if no
     *  body is available.
     */
    FileDef *getBodyDef();

    /** Returns the programming language this definition was written in. */
    SrcLangExt getLanguage() const;

    GroupList *partOfGroups() const;

    QList<ListItemInfo> *xrefListItems() const;

    virtual Definition *findInnerCompound(const char *name);
    virtual Definition *getOuterScope() const;

    MemberSDict *getReferencesMembers() const;
    MemberSDict *getReferencedByMembers() const;

    bool hasSections() const;

    QCString id() const;

    //-----------------------------------------------------------------------------------
    // ----  setters -----
    //-----------------------------------------------------------------------------------

    /*! Sets a new \a name for the definition */
    void setName(const char *name);

    /*! Sets a unique id for the symbol. Used for libclang integration. */
    void setId(const char *name);

    /*! Sets the documentation of this definition to \a d. */
    virtual void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE);

    /*! Sets the brief description of this definition to \a b.
     *  A dot is added to the sentence if not available.
     */
    virtual void setBriefDescription(const char *b,const char *briefFile,int briefLine);

    /*! Set the documentation that was found inside the body of an item.
     *  If there was already some documentation set, the new documentation
     *  will be appended.
     */
    virtual void setInbodyDocumentation(const char *d,const char *docFile,int docLine);

    /*! Sets the tag file id via which this definition was imported. */
    void setReference(const char *r);

    /*! Add the list of anchors that mark the sections that are found in the 
     * documentation.
     */
    void addSectionsToDefinition(QList<SectionInfo> *anchorList);

    // source references
    void setBodySegment(int bls,int ble);
    void setBodyDef(FileDef *fd);
    void addSourceReferencedBy(MemberDef *d);
    void addSourceReferences(MemberDef *d);

    void setRefItems(const QList<ListItemInfo> *sli);
    void mergeRefItems(Definition *d);
    virtual void addInnerCompound(Definition *d);
    virtual void setOuterScope(Definition *d);

    virtual void setHidden(bool b);

    void setArtificial(bool b);
    void setLanguage(SrcLangExt lang);

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    QCString convertNameToFile(const char *name,bool allowDots=FALSE) const;
    void writeSourceDef(OutputList &ol,const char *scopeName);
    void writeInlineCode(OutputList &ol,const char *scopeName);
    void writeSourceRefs(OutputList &ol,const char *scopeName);
    void writeSourceReffedBy(OutputList &ol,const char *scopeName);
    void makePartOfGroup(GroupDef *gd);
    //void writePathFragment(OutputList &ol) const;
    void writeNavigationPath(OutputList &ol) const;
    QCString navigationPathAsString() const;
    virtual void writeQuickMemberLinks(OutputList &,MemberDef *) const {}
    virtual void writeSummaryLinks(OutputList &) {}
    QCString pathFragment() const;

    /*! Writes the documentation anchors of the definition to 
     *  the Doxygen::tagFile stream.
     */
    void writeDocAnchorsToTagFile();
    void setLocalName(const QCString name);

    void addSectionsToIndex();
    void writeToc(OutputList &ol);

  protected:

    Definition(const Definition &d);

  private: 
    static void addToMap(const char *name,Definition *d);
    static void removeFromMap(Definition *d);

    void _setSymbolName(const QCString &name);

    int  _getXRefListId(const char *listName) const;
    void _writeSourceRefList(OutputList &ol,const char *scopeName,
                       const QCString &text,MemberSDict *members,bool);
    void _setBriefDescription(const char *b,const char *briefFile,int briefLine);
    void _setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace,bool atTop);
    void _setInbodyDocumentation(const char *d,const char *docFile,int docLine);
    bool _docsAlreadyAdded(const QCString &doc,QCString &sigList);
    DefinitionImpl *m_impl; // internal structure holding all private data
    QCString m_name;
    bool m_isSymbol;
    QCString m_symbolName;
    int m_defLine;
    int m_defColumn;
};

/** A list of Definition objects. */
class DefinitionList : public QList<Definition>, public DefinitionIntf
{
  public:
    ~DefinitionList() {}
    DefType definitionType() const { return TypeSymbolList; }
    int compareItems(QCollection::Item item1,QCollection::Item item2)
    {
      return qstricmp(((Definition *)item1)->name(),
                     ((Definition *)item2)->name()
                    );
    }

};

/** An iterator for Definition objects in a DefinitionList. */
class DefinitionListIterator : public QListIterator<Definition>
{
  public:
    DefinitionListIterator(const DefinitionList &l) :
      QListIterator<Definition>(l) {}
    ~DefinitionListIterator() {}
};

/** Reads a fragment from file \a fileName starting with line \a startLine
 *  and ending with line \a endLine. The result is returned as a string 
 *  via \a result. The function returns TRUE if successful and FALSE 
 *  in case of an error.
 */
bool readCodeFragment(const char *fileName, 
                      int &startLine,int &endLine,
                      QCString &result);
#endif
