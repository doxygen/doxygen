/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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

/*! The common base class of all entity definitions found in the sources. */
class Definition
{
  public:
    /*! Types of derived classes */
    enum DefType 
    { 
      TypeClass, TypeMember, TypeFile, TypeGroup, 
      TypeNamespace, TypePackage, TypePage
    };
    /*! Use this for dynamic inspection of the type of the derived class */
    virtual DefType definitionType() = 0;
    
    /*! Create a new definition */
    Definition(
        const char *defFileName,int defLine,
        const char *name,const char *b=0,const char *d=0,
        bool isSymbol=TRUE);

    /*! Destroys the definition */
    virtual ~Definition();

    /*! Returns the name of the definition */
    const QCString& name() const { return m_name; }

    /*! Returns the base name of the output file that contains this 
     *  definition. 
     */
    virtual QCString qualifiedName();

    /*! Returns the local name without any scope qualifiers. */
    QCString localName() const;

    /*! Returns the base file name (without extension) of this definition.
     *  as it is referenced to/written to disk.
     */
    virtual QCString getOutputFileBase() const = 0;

    /*! Returns the name of the source listing of this file. */
    const QCString getSourceFileBase() const { ASSERT(0); return "NULL"; }

    /*! Returns the detailed description of this definition */
    const QCString& documentation() const { return m_doc; }
    
    /*! Returns the brief description of this definition */
    const QCString& briefDescription() const { return m_brief; }

    /*! Sets a new \a name for the definition */
    void setName(const char *name) { m_name=name; }

    /*! Sets the documentation of this definition to \a d. */
    void setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace=TRUE);

    /*! Returns the line number at which the detailed documentation was found. */
    int docLine() const { return m_docLine; }

    /*! Returns the file in which the detailed documentation block was found. 
     *  This can differ from getDefFileName().
     */
    QCString docFile() const { return m_docFile; }

    /*! Sets the brief description of this definition to \a b.
     *  A dot is added to the sentence if not available.
     */
    void setBriefDescription(const char *b,const char *briefFile,int briefLine);

    /*! Returns the line number at which the brief description was found. */
    int briefLine() const { return m_briefLine; }

    /*! Returns the file in which the brief description was found. 
     *  This can differ from getDefFileName().
     */
    QCString briefFile() const { return m_briefFile; }

    /*! returns the file in which this definition was found */
    QCString getDefFileName() const { return m_defFileName; }

    /*! returns the line number at which the definition was found */
    int getDefLine() const { return m_defLine; }

    /*! Returns TRUE iff the definition is documented */
    virtual bool hasDocumentation() const;

    /*! Returns TRUE iff it is possible to link to this item within this
     *  project. 
     */
    virtual bool isLinkableInProject() const = 0;

    /*! Returns TRUE iff it is possible to link to this item. This can
     *  be a link to another project imported via a tag file. 
     */
    virtual bool isLinkable() const = 0;

    /*! If this definition was imported via a tag file, this function
     *  returns the tagfile for the external project. This can be
     *  translated into an external link target via 
     *  Doxygen::tagDestinationDict
     */
    virtual QCString getReference() const { return m_ref; }

    /*! Returns TRUE if this definition is imported via a tag file. */
    virtual bool isReference() const { return !m_ref.isEmpty(); }

    /*! Sets the tag file id via which this definition was imported. */
    void setReference(const char *r) { m_ref=r; }

    /*! Returns the name of this definition as it appears in the symbol map.
     */
    QCString symbolName() const { return m_symbolName; }

    /*! Sets the name of this definition as it should appear in the symbol map.
     */
    void setSymbolName(const QCString &name) { m_symbolName=name; }

    /*! Add the list of anchors that mark the sections that are found in the 
     * documentation.
     */
    void addSectionsToDefinition(QList<SectionInfo> *anchorList);

    /*! Writes the documentation anchors of the definition to 
     *  the Doxygen::tagFile stream.
     */
    void writeDocAnchorsToTagFile();

    // source references
    void setBodySegment(int bls,int ble);
    void setBodyDef(FileDef *fd)         { m_bodyDef=fd; }
    int getStartBodyLine() const         { return m_startBodyLine; }
    int getEndBodyLine() const           { return m_endBodyLine; }
    FileDef *getBodyDef()                { return m_bodyDef; }
    void writeSourceDef(OutputList &ol,const char *scopeName);
    void writeInlineCode(OutputList &ol,const char *scopeName);
    void writeSourceRefs(OutputList &ol,const char *scopeName);
    void writeSourceReffedBy(OutputList &ol,const char *scopeName);
    void addSourceReferencedBy(MemberDef *d);
    void addSourceReferences(MemberDef *d);

    void setRefItems(const QList<ListItemInfo> *sli);
    void mergeRefItems(Definition *d);
    const QList<ListItemInfo> *xrefListItems() const;

    virtual Definition *findInnerCompound(const char *name);
    virtual Definition *getOuterScope() const { return m_outerScope; }
    virtual void addInnerCompound(Definition *d);
    virtual void setOuterScope(Definition *d) { m_outerScope = d; }

    MemberSDict *getReferencesMembers() const { return m_sourceRefsDict; }
    MemberSDict *getReferencedByMembers() const { return m_sourceRefByDict; }

    void makePartOfGroup(GroupDef *gd);
    GroupList *partOfGroups() const { return m_partOfGroups; }
    QCString convertNameToFile(const char *name,bool allowDots=FALSE) const;

  protected:
    int      m_startBodyLine;   // line number of the start of the definition
    int      m_endBodyLine;     // line number of the end of the definition
    FileDef *m_bodyDef;         // file definition containing the function body

    // where the item was found
    QCString m_defFileName;
    int      m_defLine;

    /*! The class, namespace in which this class is located 
     */ 
    Definition *m_outerScope;
    QCString m_name;     // name of the definition
    QCString m_localName;      // local (unqualified) name of the definition
                               // in the future m_name should become m_localName
    /*! List of groups this definition is part of */
    GroupList *m_partOfGroups;

  private: 
    int getXRefListId(const char *listName) const;
    void writeSourceRefList(OutputList &ol,const char *scopeName,
                       const QCString &text,MemberSDict *members,bool);
    //QCString m_qualifiedName;  // name of the definition
    QCString m_brief; // brief description
    QCString m_doc;   // detailed description
    QCString m_ref;   // reference to external documentation
    SectionDict *m_sectionDict;  // dictionary of all sections
    MemberSDict *m_sourceRefByDict;
    MemberSDict *m_sourceRefsDict;
    int m_testId;       // id for test list item
    int m_todoId;       // id for todo list item
    int m_bugId;        // id for bug list item
    int m_deprecatedId; // id for deprecated list item
    int m_docLine;
    QCString m_docFile;
    int m_briefLine;
    QCString m_briefFile;
    QList<ListItemInfo> *m_xrefListItems;
    QCString m_symbolName;
    bool m_isSymbol;

    
    QCString m_qualifiedName;
};

class DefinitionList : public QList<Definition>
{
  public:
    ~DefinitionList() {}
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
