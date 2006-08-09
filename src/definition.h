/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
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

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>

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

struct ReachableDefinition
{
  ReachableDefinition(Definition *d,int dist) : def(d), distance(dist) {}
  Definition *def;
  int distance;
};

struct DocInfo
{
    QCString doc;  
    int      line;
    QCString file;
};

struct BodyInfo
{
    int      startLine;   // line number of the start of the definition
    int      endLine;     // line number of the end of the definition
    FileDef *fileDef;     // file definition containing the function body
};
    
/*! Abstract interface for a Definition or DefinitionList */
class DefinitionIntf
{
  public:
    DefinitionIntf() {}
    virtual ~DefinitionIntf() {}
    /*! Types of derived classes */
    enum DefType 
    { 
      TypeClass, TypeMember, TypeFile, TypeGroup, 
      TypeNamespace, TypePackage, TypePage, TypeDir, 
      TypeSymbolList
    };
    /*! Use this for dynamic inspection of the type of the derived class */
    virtual DefType definitionType() const = 0;
};

/*! The common base class of all entity definitions found in the sources. */
class Definition : public DefinitionIntf
{
  public:
    
    /*! Create a new definition */
    Definition(
        const char *defFileName,int defLine,
        const char *name,const char *b=0,const char *d=0,
        bool isSymbol=TRUE);

    /*! Destroys the definition */
    virtual ~Definition();

    //-----------------------------------------------------------------------------------
    // ----  getters -----
    //-----------------------------------------------------------------------------------

    /*! Returns the name of the definition */
    const QCString& name() const { return m_name; }

    /*! Returns the local name without any scope qualifiers. */
    QCString localName() const;

    /*! Returns the base name of the output file that contains this 
     *  definition. 
     */
    virtual QCString qualifiedName();

    /*! Returns the name of this definition as it appears in the symbol map.
     */
    QCString symbolName() const { return m_symbolName; }

    /*! Returns the base file name (without extension) of this definition.
     *  as it is referenced to/written to disk.
     */
    virtual QCString getOutputFileBase() const = 0;

    /*! Returns the name of the source listing of this file. */
    virtual QCString getSourceFileBase() const { ASSERT(0); return "NULL"; }

    /*! Returns the detailed description of this definition */
    QCString documentation() const { return m_details ? m_details->doc : QCString(""); }
    
    /*! Returns the line number at which the detailed documentation was found. */
    int docLine() const { return m_details ? m_details->line : 1; }

    /*! Returns the file in which the detailed documentation block was found. 
     *  This can differ from getDefFileName().
     */
    QCString docFile() const { return m_details ? m_details->file : QCString("<"+m_name+">"); }

    /*! Returns the brief description of this definition */
    QCString briefDescription() const { return m_brief ? m_brief->doc : QCString(""); }

    /*! Returns the line number at which the brief description was found. */
    int briefLine() const { return m_brief ? m_brief->line : 1; }

    /*! Returns the file in which the brief description was found. 
     *  This can differ from getDefFileName().
     */
    QCString briefFile() const { return m_brief ? m_brief->file : QCString("<"+m_name+">"); }

    /*! returns the file in which this definition was found */
    QCString getDefFileName() const { return m_defFileName; }

    /*! returns the file in which this definition was found */
    QCString getDefFileExtension() const { return m_defFileExt; }

    /*! returns the line number at which the definition was found */
    int getDefLine() const { return m_defLine; }

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
    virtual bool isVisibleInProject() const 
    { return m_hidden || isLinkableInProject(); }

    /*! Returns TRUE iff the name may appear in the output */
    virtual bool isVisible() const
    { return m_hidden || isLinkable(); }

    bool isHidden() const { return m_hidden; }

    /*! If this definition was imported via a tag file, this function
     *  returns the tagfile for the external project. This can be
     *  translated into an external link target via 
     *  Doxygen::tagDestinationDict
     */
    virtual QCString getReference() const { return m_ref; }

    /*! Returns TRUE if this definition is imported via a tag file. */
    virtual bool isReference() const { return !m_ref.isEmpty(); }

    int getStartBodyLine() const         { return m_body ? m_body->startLine : -1; }
    int getEndBodyLine() const           { return m_body ? m_body->endLine : -1; }
    FileDef *getBodyDef()                { return m_body ? m_body->fileDef : 0; }

    GroupList *partOfGroups() const { return m_partOfGroups; }

    const QList<ListItemInfo> *xrefListItems() const;

    virtual Definition *findInnerCompound(const char *name);
    virtual Definition *getOuterScope() const { return m_outerScope; }

    MemberSDict *getReferencesMembers() const { return m_sourceRefsDict; }
    MemberSDict *getReferencedByMembers() const { return m_sourceRefByDict; }

    //-----------------------------------------------------------------------------------
    // ----  setters -----
    //-----------------------------------------------------------------------------------

    /*! Sets a new \a name for the definition */
    void setName(const char *name) { m_name=name; }

    /*! Sets the documentation of this definition to \a d. */
    void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE);

    /*! Sets the brief description of this definition to \a b.
     *  A dot is added to the sentence if not available.
     */
    void setBriefDescription(const char *b,const char *briefFile,int briefLine);

    /*! Sets the tag file id via which this definition was imported. */
    void setReference(const char *r) { m_ref=r; }

    /*! Sets the name of this definition as it should appear in the symbol map.
     */
    void setSymbolName(const QCString &name) { m_symbolName=name; }

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

    void setHidden(bool b) { m_hidden = b; }

    //-----------------------------------------------------------------------------------
    // --- actions ----
    //-----------------------------------------------------------------------------------

    QCString convertNameToFile(const char *name,bool allowDots=FALSE) const;
    void writeSourceDef(OutputList &ol,const char *scopeName);
    void writeInlineCode(OutputList &ol,const char *scopeName);
    void writeSourceRefs(OutputList &ol,const char *scopeName);
    void writeSourceReffedBy(OutputList &ol,const char *scopeName);
    void makePartOfGroup(GroupDef *gd);
    void writePathFragment(OutputList &ol) const;
    void writeNavigationPath(OutputList &ol) const;
    virtual void writeQuickMemberLinks(OutputList &,MemberDef *) const {}

    /*! Writes the documentation anchors of the definition to 
     *  the Doxygen::tagFile stream.
     */
    void writeDocAnchorsToTagFile();

  protected:
    void setLocalName(const QCString name) { m_localName=name; }
    
  private: 
    int getXRefListId(const char *listName) const;
    void writeSourceRefList(OutputList &ol,const char *scopeName,
                       const QCString &text,MemberSDict *members,bool);

    //-----------------------------------------------------------------------------------
    // --- member variables
    //-----------------------------------------------------------------------------------

    SectionDict *m_sectionDict;  // dictionary of all sections

    MemberSDict *m_sourceRefByDict;
    MemberSDict *m_sourceRefsDict;

    DocInfo  *m_details;
    DocInfo  *m_brief;
    BodyInfo *m_body;

    /*! The class, namespace in which this class is located 
     */ 
    QCString m_name;           // name of the definition
    QCString m_localName;      // local (unqualified) name of the definition
                               // in the future m_name should become m_localName
    QCString m_symbolName;
    QCString m_qualifiedName;
    QCString m_ref;   // reference to external documentation

    QList<ListItemInfo> *m_xrefListItems;
    bool m_isSymbol;
    bool m_hidden;

    Definition *m_outerScope;
    GroupList *m_partOfGroups;

    // where the item was found
    QCString m_defFileName;
    int      m_defLine;
    QCString m_defFileExt;
};

class DefinitionList : public QList<Definition>, public DefinitionIntf
{
  public:
    ~DefinitionList() {}
    DefType definitionType() const { return TypeSymbolList; }
    int compareItems(GCI item1,GCI item2)
    {
      return stricmp(((Definition *)item1)->name(),
                     ((Definition *)item2)->name()
                    );
    }

};

class DefinitionListIterator : public QListIterator<Definition>
{
  public:
    DefinitionListIterator(const DefinitionList &l) :
      QListIterator<Definition>(l) {}
    ~DefinitionListIterator() {}
};

#endif
