/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

class FileDef;
class OutputList;
class SectionDict;
class MemberList;
class MemberDict;
class MemberDef;

/*! The common base class of all entity definitions found in the sources. */
class Definition
{
  public:
    /*! Types of derived classes */
    enum DefType 
    { 
      TypeClass, TypeMember, TypeFile, TypeGroup, TypeNamespace, TypePackage 
    };
    /*! Use this for dynamic inspection of the derived class */
    virtual DefType definitionType() = 0;
    
    /*! Create a new definition */
    Definition(
        const char *defFileName,int defLine,
        const char *name,const char *b=0,const char *d=0);
    /*! Destroys the definition */
    virtual ~Definition();
    /*! Returns the name of the definition */
    const QCString& name() const { return m_name; }
    /*! Returns the base name of the output file that contains this 
     *  definition. 
     */
    QCString qualifiedName() const;
    QCString localName() const;
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
    void setDocumentation(const char *d,bool stripWhiteSpace=TRUE) 
      { 
        if (stripWhiteSpace)
          m_doc=((QCString)d).stripWhiteSpace();
        else
          m_doc=d;
      }
    /*! Sets the brief description of this definition to \a b.
     *  A dot is added to the sentence if not available.
     */
    void setBriefDescription(const char *b);
    /*! Returns TRUE iff the definition is documented */
    virtual bool hasDocumentation() const;
    virtual bool isLinkableInProject() const = 0;
    virtual bool isLinkable() const = 0;
    virtual QCString getReference() const { return m_ref; }

    virtual bool isReference() const { return !m_ref.isEmpty(); }
    void setReference(const char *r) { m_ref=r; }

    /*! Add the list of anchors that mark the sections that are found in the 
     * documentation.
     */
    void addSectionsToDefinition(QList<QCString> *anchorList);
    void writeDocAnchorsToTagFile();

    // source references
    void setBodySegment(int bls,int ble) 
    {
      m_startBodyLine=bls; 
      m_endBodyLine=ble; 
    }
    void setBodyDef(FileDef *fd)         { m_bodyDef=fd; }
    int getStartBodyLine() const         { return m_startBodyLine; }
    int getEndBodyLine() const           { return m_endBodyLine; }
    FileDef *getBodyDef()                { return m_bodyDef; }
    void writeSourceDef(OutputList &ol,const char *scopeName);
    void writeInlineCode(OutputList &ol,const char *scopeName);
    void writeSourceRefs(OutputList &ol,const char *scopeName);
    void addSourceReference(MemberDef *d);

    void setRefItems(int todoId,int testId,int bugId) 
    { 
      if (todoId!=0) m_todoId=todoId; 
      if (testId!=0) m_testId=testId; 
      if (bugId!=0)  m_bugId=bugId; 
    }
    int todoId() const { return m_todoId; }
    int testId() const { return m_testId; }
    int bugId() const { return m_bugId; }

    /*! returns the file in which this definition was found */
    QCString getDefFileName() const { return m_defFileName; }
    /*! returns the line number at which the definition was found */
    int getDefLine() const { return m_defLine; }

    virtual Definition *findInnerCompound(const char *name);
    virtual Definition *getOuterScope() const { return m_outerScope; }
    virtual void addInnerCompound(Definition *d);
    virtual void setOuterScope(Definition *d) { m_outerScope = d; }

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

  private: 
    //QCString m_qualifiedName;  // name of the definition
    QCString m_brief; // brief description
    QCString m_doc;   // detailed description
    QCString m_ref;   // reference to external documentation
    SectionDict *m_sectionDict;  // dictionary of all sections
    MemberList  *m_sourceRefList;  // list of entities that refer to this
                                 // entity in their definition 
    MemberDict *m_sourceRefDict;
    int m_testId;     // id for test list item
    int m_todoId;     // id for todo list item
    int m_bugId;      // id for bug list item

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
