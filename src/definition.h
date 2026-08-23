/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#include <vector>

#include "construct.h"
#include "reflist.h"
#include "requirement.h"
#include "types.h"

#ifdef _MSC_VER
// To disable 'inherits via dominance' warnings with MSVC.
// See also https://stackoverflow.com/a/14487243/784672
#pragma warning( disable: 4250 )
#endif

class Definition;
class DefinitionImpl;
class DefinitionMutable;
class FileDef;
class GroupDef;
class GroupList;
class MemberDef;
class MemberVector;
class OutputList;
class SectionInfo;
class SectionRefs;
class TextStream;

/** Data associated with a detailed description. */
struct DocInfo
{
    DString doc;
    int      line = -1;
    DString file;
};

/** Data associated with a brief description. */
struct BriefInfo
{
    DString doc;
    DString tooltip;
    int      line = -1;
    DString file;
};

/** Data associated with description found in the body. */
struct BodyInfo
{
    int      defLine = -1;     //!< line number of the start of the definition
    int      startLine = -1;   //!< line number of the start of the definition's body
    int      endLine = -1;     //!< line number of the end of the definition's body
    const FileDef *fileDef = nullptr;      //!< file definition containing the function body
};

/** The common base class of all entity definitions found in the sources.
 *
 *  This can be a class or a member function, or a file, or a namespace, etc.
 *  Use definitionType() to find which type of definition this is.
 */
class Definition
{
  public:
    ABSTRACT_BASE_CLASS(Definition)

    /*! Types of derived classes */
    enum DefType
    {
      TypeClass      = 0,
      TypeFile       = 1,
      TypeNamespace  = 2,
      TypeModule     = 3,
      TypeMember     = 4,
      TypeGroup      = 5,
      TypePackage    = 6,
      TypePage       = 7,
      TypeDir        = 8,
      TypeConcept    = 9,
    };


    //-----------------------------------------------------------------------------------
    // ----  getters -----
    //-----------------------------------------------------------------------------------

    /*! Use this for dynamic inspection of the type of the derived class */
    virtual DefType definitionType() const = 0;

    /*! Used for syntax highlighting symbol class */
    virtual CodeSymbolType codeSymbolType() const = 0;

    /*! Returns true if this is an alias of another definition */
    virtual bool isAlias() const = 0;

    /*! Returns the name of the definition */
    virtual const DString &name() const = 0;

    /*! Returns true iff this definition has an artificially generated name
     * (typically starting with a @) that is used for nameless definitions
     */
    virtual bool isAnonymous() const = 0;

    /*! Returns the name of the definition as it appears in the output */
    virtual DString displayName(bool includeScope=true) const = 0;

    /*! Returns the local name without any scope qualifiers. */
    virtual const DString &localName() const = 0;

    /*! Returns the fully qualified name of this definition
     */
    virtual DString qualifiedName() const = 0;

    /*! Returns the name of this definition as it appears in the symbol map.
     */
    virtual DString symbolName() const = 0;

    /*! Returns the base file name (without extension) of this definition.
     *  as it is referenced to/written to disk.
     */
    virtual DString getOutputFileBase() const = 0;

    /*! Returns the anchor within a page where this item can be found */
    virtual DString anchor() const = 0;

    /*! Returns the name of the source listing of this definition. */
    virtual DString getSourceFileBase() const = 0;

    /*! Returns the anchor of the source listing of this definition. */
    virtual DString getSourceAnchor() const = 0;

    /*! Returns the detailed description of this definition */
    virtual DString documentation() const = 0;

    /*! Returns the line number at which the detailed documentation was found. */
    virtual int docLine() const = 0;

    /*! Returns the file in which the detailed documentation block was found.
     *  This can differ from getDefFileName().
     */
    virtual DString docFile() const = 0;

    /*! Returns the brief description of this definition. This can include commands. */
    virtual DString briefDescription(bool abbreviate=false) const = 0;

    /*! Returns a plain text version of the brief description suitable for use
     *  as a tool tip.
     */
    virtual DString briefDescriptionAsTooltip() const = 0;

    /*! Returns the line number at which the brief description was found. */
    virtual int briefLine() const = 0;

    /*! Returns the file in which the brief description was found.
     *  This can differ from getDefFileName().
     */
    virtual DString briefFile() const = 0;

    /*! Returns the documentation found inside the body of a member */
    virtual DString inbodyDocumentation() const = 0;

    /*! Returns the file in which the in body documentation was found */
    virtual DString inbodyFile() const = 0;

    /*! Returns the line at which the first in body documentation
        part was found */
    virtual int inbodyLine() const = 0;

    /*! returns the file in which this definition was found */
    virtual DString getDefFileName() const = 0;

    /*! returns the extension of the file in which this definition was found */
    virtual DString getDefFileExtension() const = 0;

    /*! returns the line number at which the definition was found (can be the declaration) */
    virtual int getDefLine() const = 0;

    /*! returns the column number at which the definition was found */
    virtual size_t getDefColumn() const = 0;

    /*! Returns true iff the definition is documented
     *  (which could be generated documentation)
     *  @see hasUserDocumentation()
     */
    virtual bool hasDocumentation() const = 0;

    /*! Returns true iff the definition is documented by the user. */
    virtual bool hasUserDocumentation() const = 0;

    /*! Returns true iff it is possible to link to this item within this
     *  project.
     */
    virtual bool isLinkableInProject() const = 0;

    /*! Returns true iff it is possible to link to this item. This can
     *  be a link to another project imported via a tag file.
     */
    virtual bool isLinkable() const = 0;

    /*! Returns true iff the name is part of this project and
     *  may appear in the output
     */
    virtual bool isVisibleInProject() const = 0;

    /*! Returns true iff the name may appear in the output */
    virtual bool isVisible() const = 0;

    /*! Returns true iff this item is supposed to be hidden from the output. */
    virtual bool isHidden() const = 0;

    /*! Returns true if this entity was artificially introduced, for
     *  instance because it is used to show a template instantiation relation.
     */
    virtual bool isArtificial() const = 0;

    /*! Returns true iff this entity was exported from a C++20 module. */
    virtual bool isExported() const = 0;

    /*! If this definition was imported via a tag file, this function
     *  returns the tagfile for the external project. This can be
     *  translated into an external link target via
     *  Doxygen::tagDestinationDict
     */
    virtual DString getReference() const = 0;

    /*! Returns true if this definition is imported via a tag file. */
    virtual bool isReference() const = 0;

    /*! Convenience method to return a resolved external link */
    virtual DString externalReference(const DString &relPath) const = 0;

    /*! Returns the first line of the implementation of this item. See also getDefLine() */
    virtual int getStartDefLine() const = 0;

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
    virtual const FileDef *getBodyDef() const = 0;

    /** Returns the programming language this definition was written in. */
    virtual SrcLangExt getLanguage() const = 0;

    virtual const GroupList &partOfGroups() const = 0;
    virtual bool isLinkableViaGroup() const = 0;

    virtual const RefItemVector &xrefListItems() const = 0;

    virtual const RequirementRefs &requirementReferences() const = 0;

    virtual const Definition *findInnerCompound(const DString &name) const = 0;
    virtual Definition *getOuterScope() const = 0;

    virtual const MemberVector &getReferencesMembers() const = 0;
    virtual const MemberVector &getReferencedByMembers() const = 0;

    virtual bool hasSections() const = 0;
    virtual bool hasSources() const = 0;


    /** returns true if this class has a brief description */
    virtual bool hasBriefDescription() const = 0;

    virtual DString id() const = 0;

    /** returns the section dictionary, only of importance for pagedef */
    virtual const SectionRefs &getSectionRefs() const = 0;

    virtual DString navigationPathAsString() const = 0;
    virtual DString pathFragment() const = 0;

    //-----------------------------------------------------------------------------------
    // --- symbol name ----
    //-----------------------------------------------------------------------------------
    virtual void _setSymbolName(const DString &name) = 0;
    virtual DString _symbolName() const = 0;

  private:
    friend class DefinitionImpl;
    friend DefinitionMutable* toDefinitionMutable(Definition *);
    virtual DefinitionMutable *toDefinitionMutable_() = 0;
    virtual const DefinitionImpl *toDefinitionImpl_() const = 0;
};

class DefinitionMutable
{
  public:
    ABSTRACT_BASE_CLASS(DefinitionMutable)

    //-----------------------------------------------------------------------------------
    // ----  setters -----
    //-----------------------------------------------------------------------------------

    /*! Sets a new \a name for the definition */
    virtual void setName(const DString &name) = 0;

    /*! Sets a unique id for the symbol. Used for libclang integration. */
    virtual void setId(const DString &name) = 0;

    /*! Set a new file name and position */
    virtual void setDefFile(const DString& df,int defLine,size_t defColumn) = 0;

    /*! Sets the documentation of this definition to \a d. */
    virtual void setDocumentation(const DString &d,const DString &docFile,int docLine,bool stripWhiteSpace=true) = 0;

    /*! Sets the brief description of this definition to \a b.
     *  A dot is added to the sentence if not available.
     */
    virtual void setBriefDescription(const DString &b,const DString &briefFile,int briefLine) = 0;

    /*! Set the documentation that was found inside the body of an item.
     *  If there was already some documentation set, the new documentation
     *  will be appended.
     */
    virtual void setInbodyDocumentation(const DString &d,const DString &docFile,int docLine) = 0;

    /*! Sets the tag file id via which this definition was imported. */
    virtual void setReference(const DString &r) = 0;

    // source references
    virtual void setBodySegment(int defLine, int bls,int ble) = 0;
    virtual void setBodyDef(const FileDef *fd) = 0;

    virtual void setRefItems(const RefItemVector &sli) = 0;
    virtual void setRequirementReferences(const RequirementRefs &rqli) = 0;
    virtual void setOuterScope(Definition *d) = 0;

    virtual void setHidden(bool b) = 0;

    virtual void setArtificial(bool b) = 0;
    virtual void setExported(bool b) = 0;
    virtual void setLanguage(SrcLangExt lang) = 0;
    virtual void setLocalName(const DString &name) = 0;

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    virtual void makePartOfGroup(GroupDef *gd) = 0;

    /*! Add the list of anchors that mark the sections that are found in the
     * documentation.
     */
    virtual void addSectionsToDefinition(const std::vector<const SectionInfo*> &anchorList) = 0;
    virtual void addSourceReferencedBy(MemberDef *d,const DString &sourceRefName) = 0;
    virtual void addSourceReferences(MemberDef *d,const DString &sourceRefName) = 0;
    virtual void mergeRefItems(Definition *d) = 0;
    virtual void addInnerCompound(Definition *d) = 0;
    virtual void mergeReferences(const Definition *other) = 0;
    virtual void mergeReferencedBy(const Definition *other) = 0;
    virtual void computeTooltip() = 0;

    //-----------------------------------------------------------------------------------
    // --- writing output ----
    //-----------------------------------------------------------------------------------
    virtual void writeSourceDef(OutputList &ol) const = 0;
    virtual void writeInlineCode(OutputList &ol,const DString &scopeName) const = 0;
    virtual bool hasSourceRefs() const = 0;
    virtual bool hasSourceReffedBy() const = 0;
    virtual void writeSourceRefs(OutputList &ol,const DString &scopeName) const = 0;
    virtual void writeSourceReffedBy(OutputList &ol,const DString &scopeName) const = 0;
    virtual bool hasRequirementRefs() const = 0;
    virtual void writeRequirementRefs(OutputList &ol) const = 0;
    virtual void writeNavigationPath(OutputList &ol) const = 0;
    virtual void writeQuickMemberLinks(OutputList &,const MemberDef *) const = 0;
    virtual void writeSummaryLinks(OutputList &) const = 0;
    virtual void writePageNavigation(OutputList &) const = 0;
    virtual void writeDocAnchorsToTagFile(TextStream &) const = 0;
    virtual void writeToc(OutputList &ol, const LocalToc &lt) const = 0;

  private:
    friend Definition* toDefinition(DefinitionMutable *);
    virtual Definition *toDefinition_() = 0;
};

Definition          *toDefinition(DefinitionMutable *dm);
DefinitionMutable   *toDefinitionMutable(Definition *d);

/** Reads a fragment from file \a fileName starting with line \a startLine
 *  and ending with line \a endLine. The result is returned as a string
 *  via \a result. The function returns true if successful and false
 *  in case of an error.
 */
bool readCodeFragment(const DString &fileName,bool isMacro,
                      int &startLine,int &endLine,
                      DString &result);
#endif
