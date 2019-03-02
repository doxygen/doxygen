/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#if defined(_WIN32) && !defined(__CYGWIN__)
// To disable 'inherits via dominance' warnings.
// See also https://stackoverflow.com/a/14487243/784672
#pragma warning( disable: 4250 )
#endif

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
class FTextStream;
  
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
    struct Cookie
    {
      virtual ~Cookie() {}
    };

    //-----------------------------------------------------------------------------------
    // ----  getters -----
    //-----------------------------------------------------------------------------------

    /*! Returns the name of the definition */
    virtual QCString name() const = 0;

    /*! Returns the name of the definition as it appears in the output */
    virtual QCString displayName(bool includeScope=TRUE) const = 0;

    /*! Returns the local name without any scope qualifiers. */
    virtual QCString localName() const = 0;

    /*! Returns the fully qualified name of this definition
     */
    virtual QCString qualifiedName() const = 0;

    /*! Returns the name of this definition as it appears in the symbol map.
     */
    virtual QCString symbolName() const = 0;

    /*! Returns the base file name (without extension) of this definition.
     *  as it is referenced to/written to disk.
     */
    virtual QCString getOutputFileBase() const = 0;

    /*! Returns the anchor within a page where this item can be found */
    virtual QCString anchor() const = 0;

    /*! Returns the name of the source listing of this definition. */
    virtual QCString getSourceFileBase() const = 0;

    /*! Returns the anchor of the source listing of this definition. */
    virtual QCString getSourceAnchor() const = 0;

    /*! Returns the detailed description of this definition */
    virtual QCString documentation() const = 0;

    /*! Returns the line number at which the detailed documentation was found. */
    virtual int docLine() const = 0;

    /*! Returns the file in which the detailed documentation block was found.
     *  This can differ from getDefFileName().
     */
    virtual QCString docFile() const = 0;

    /*! Returns the brief description of this definition. This can include commands. */
    virtual QCString briefDescription(bool abbreviate=FALSE) const = 0;

    /*! Returns a plain text version of the brief description suitable for use
     *  as a tool tip. 
     */
    virtual QCString briefDescriptionAsTooltip() const = 0;

    /*! Returns the line number at which the brief description was found. */
    virtual int briefLine() const = 0;

    /*! Returns the documentation found inside the body of a member */
    virtual QCString inbodyDocumentation() const = 0;

    /*! Returns the file in which the in body documentation was found */
    virtual QCString inbodyFile() const = 0;

    /*! Returns the line at which the first in body documentation 
        part was found */
    virtual int inbodyLine() const = 0;

    /*! Returns the file in which the brief description was found. 
     *  This can differ from getDefFileName().
     */
    virtual QCString briefFile() const = 0;

    /*! returns the file in which this definition was found */
    virtual QCString getDefFileName() const = 0;

    /*! returns the extension of the file in which this definition was found */
    virtual QCString getDefFileExtension() const = 0;

    /*! returns the line number at which the definition was found */
    virtual int getDefLine() const = 0;

    /*! returns the column number at which the definition was found */
    virtual int getDefColumn() const = 0;

    /*! Returns TRUE iff the definition is documented 
     *  (which could be generated documentation) 
     *  @see hasUserDocumentation()
     */
    virtual bool hasDocumentation() const = 0;

    /*! Returns TRUE iff the definition is documented by the user. */
    virtual bool hasUserDocumentation() const = 0;

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
    virtual bool isVisibleInProject() const = 0;

    /*! Returns TRUE iff the name may appear in the output */
    virtual bool isVisible() const = 0;

    /*! Returns TRUE iff this item is supposed to be hidden from the output. */
    virtual bool isHidden() const = 0;

    /*! returns TRUE if this entity was artificially introduced, for 
     *  instance because it is used to show a template instantiation relation. 
     */
    virtual bool isArtificial() const = 0;

    /*! If this definition was imported via a tag file, this function
     *  returns the tagfile for the external project. This can be
     *  translated into an external link target via 
     *  Doxygen::tagDestinationDict
     */
    virtual QCString getReference() const = 0;

    /*! Returns TRUE if this definition is imported via a tag file. */
    virtual bool isReference() const = 0;

    /*! Convenience method to return a resolved external link */
    virtual QCString externalReference(const QCString &relPath) const = 0;

    /*! Returns the first line of the body of this item (applicable to classes and 
     *  functions).
     */
    virtual int getStartBodyLine() const = 0;

    /*! Returns the last line of the body of this item (applicable to classes and 
     *  functions).
     */
    virtual int getEndBodyLine() const = 0;

    /*! Returns the file in which the body of this item is located or 0 if no
     *  body is available.
     */
    virtual FileDef *getBodyDef() const = 0;

    /** Returns the programming language this definition was written in. */
    virtual SrcLangExt getLanguage() const = 0;

    virtual GroupList *partOfGroups() const = 0;
    virtual bool isLinkableViaGroup() const = 0;

    virtual QList<ListItemInfo> *xrefListItems() const = 0;

    virtual Definition *findInnerCompound(const char *name) const = 0;
    virtual Definition *getOuterScope() const = 0;

    virtual MemberSDict *getReferencesMembers() const = 0;
    virtual MemberSDict *getReferencedByMembers() const = 0;

    virtual bool hasSections() const = 0;
    virtual bool hasSources() const = 0;

    /** returns TRUE if this class has a brief description */
    virtual bool hasBriefDescription() const = 0;

    virtual QCString id() const = 0;

    /** returns the section dictionary, only of importance for pagedef */
    virtual SectionDict * getSectionDict(void) = 0;

    //-----------------------------------------------------------------------------------
    // ----  setters -----
    //-----------------------------------------------------------------------------------

    /*! Sets a new \a name for the definition */
    virtual void setName(const char *name) = 0;

    /*! Sets a unique id for the symbol. Used for libclang integration. */
    virtual void setId(const char *name) = 0;

    /*! Set a new file name and position */
    virtual void setDefFile(const QCString& df,int defLine,int defColumn) = 0;

    /*! Sets the documentation of this definition to \a d. */
    virtual void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE) = 0;

    /*! Sets the brief description of this definition to \a b.
     *  A dot is added to the sentence if not available.
     */
    virtual void setBriefDescription(const char *b,const char *briefFile,int briefLine) = 0;

    /*! Set the documentation that was found inside the body of an item.
     *  If there was already some documentation set, the new documentation
     *  will be appended.
     */
    virtual void setInbodyDocumentation(const char *d,const char *docFile,int docLine) = 0;

    /*! Sets the tag file id via which this definition was imported. */
    virtual void setReference(const char *r) = 0;

    /*! Add the list of anchors that mark the sections that are found in the 
     * documentation.
     */
    virtual void addSectionsToDefinition(QList<SectionInfo> *anchorList) = 0;

    // source references
    virtual void setBodySegment(int bls,int ble) = 0;
    virtual void setBodyDef(FileDef *fd) = 0;
    virtual void addSourceReferencedBy(MemberDef *d) = 0;
    virtual void addSourceReferences(MemberDef *d) = 0;

    virtual void setRefItems(const QList<ListItemInfo> *sli) = 0;
    virtual void mergeRefItems(Definition *d) = 0;
    virtual void addInnerCompound(Definition *d) = 0;
    virtual void setOuterScope(Definition *d) = 0;

    virtual void setHidden(bool b) = 0;

    virtual void setArtificial(bool b) = 0;
    virtual void setLanguage(SrcLangExt lang) = 0;

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    virtual void writeSourceDef(OutputList &ol,const char *scopeName) = 0;
    virtual void writeInlineCode(OutputList &ol,const char *scopeName) = 0;
    virtual void writeSourceRefs(OutputList &ol,const char *scopeName) = 0;
    virtual void writeSourceReffedBy(OutputList &ol,const char *scopeName) = 0;
    virtual void makePartOfGroup(GroupDef *gd) = 0;
    virtual void writeNavigationPath(OutputList &ol) const = 0;
    virtual QCString navigationPathAsString() const = 0;
    virtual void writeQuickMemberLinks(OutputList &,MemberDef *) const = 0;
    virtual void writeSummaryLinks(OutputList &) = 0;
    virtual QCString pathFragment() const = 0;

    /*! Writes the documentation anchors of the definition to 
     *  the Doxygen::tagFile stream.
     */
    virtual void writeDocAnchorsToTagFile(FTextStream &) = 0;
    virtual void setLocalName(const QCString name) = 0;

    virtual void addSectionsToIndex() = 0;
    virtual void writeToc(OutputList &ol, const LocalToc &lt) = 0;

    virtual void setCookie(Cookie *cookie) = 0;
    virtual Cookie *cookie() const = 0;

    virtual void _setSymbolName(const QCString &name) = 0;
    virtual QCString _symbolName() const = 0;
};

/** A list of Definition objects. */
class DefinitionList : public QList<Definition>, public DefinitionIntf
{
  public:
    ~DefinitionList() {}
    DefType definitionType() const { return TypeSymbolList; }
    int compareValues(const Definition *item1,const Definition *item2) const
    {
      return qstricmp(item1->name(),item2->name());
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
