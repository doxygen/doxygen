/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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
class SectionList;
class MemberList;
class MemberDict;
class MemberDef;

/*! The common base class of all entity definitions found in the sources. */
class Definition
{
  public:
    /*! create a new definition */
    Definition(
        const char *defFileName,int defLine,
        const char *name,const char *b=0,const char *d=0);
    /*! destroys the definition */
    virtual ~Definition();
    /*! returns the name of the definition */
    const QCString& name() const { return n; }
    /*! returns the base name of the output file that contains this 
     *  definition. 
     */
    virtual QCString getOutputFileBase() const = 0;
    /*! Returns the name of the source listing of this file. */
    const QCString sourceName() const { return getOutputFileBase()+"-source"; }
    /*! returns the detailed description of this definition */
    const QCString& documentation() const { return doc; }
    /*! returns the brief description of this definition */
    const QCString& briefDescription() const { return brief; }
    /*! sets a new \a name for the definition */
    void setName(const char *name) { n=name; }
    /*! sets the documentation of this definition to \a d. */
    void setDocumentation(const char *d,bool stripWhiteSpace=TRUE) 
      { 
        if (stripWhiteSpace)
          doc=((QCString)d).stripWhiteSpace();
        else
          doc=d;
      }
    /*! sets the brief description of this definition to \a b.
     *  A dot is added to the sentence if not available.
     */
    void setBriefDescription(const char *b);
    /*! returns TRUE iff the definition is documented */
    virtual bool hasDocumentation();
    virtual bool isLinkableInProject() = 0;
    virtual bool isLinkable() = 0;

    bool isReference() { return !ref.isEmpty(); }
    void setReference(const char *r) { ref=r; }
    QCString getReference() { return ref; }

    /*! returns the base file name that corresponds with the \a name of this 
     *  definition. This replaces a number of special characters in the
     *  name by string that are more suitable to use in file names.
     *  The function getOutputFileBase() also uses this function in most cases.
     *  \sa setName(),Definition()
     */
    QCString nameToFile(const char *name,bool allowDot=FALSE);

    /*! Add the list of anchors that mark the sections that are found in the 
     * documentation.
     */
    void addSectionsToDefinition(QList<QCString> *anchorList);

    // source references
    void setBodySegment(int bls,int ble) 
    {
      startBodyLine=bls; 
      endBodyLine=ble; 
    }
    void setBodyDef(FileDef *fd)         { bodyDef=fd; }
    int getStartBodyLine() const         { return startBodyLine; }
    int getEndBodyLine() const           { return endBodyLine; }
    FileDef *getBodyDef()                { return bodyDef; }
    void writeSourceDef(OutputList &ol,const char *scopeName);
    void writeInlineCode(OutputList &ol,const char *scopeName);
    void writeSourceRefs(OutputList &ol,const char *scopeName);
    void addSourceReference(MemberDef *d);

    void setRefItems(int todoId,int testId) { m_todoId=todoId; m_testId=testId; }
    int todoId() const { return m_todoId; }
    int testId() const { return m_testId; }

    /*! returns the file in which this definition was found */
    QCString getDefFileName() const { return defFileName; }
    /*! returns the line number at which the definition was found */
    int getDefLine() const { return defLine; }

  protected:
    int      startBodyLine;   // line number of the start of the definition
    int      endBodyLine;     // line number of the end of the definition
    FileDef *bodyDef;         // file definition containing the function body

    // where the item was found
    QCString defFileName;
    int      defLine;

  private: 
    QCString n;     // name of the definition
    QCString brief; // brief description
    QCString doc;   // detailed description
    QCString ref;   // reference to external documentation
    SectionList *sectionList; // list of all sections
    MemberList  *sourceRefList;  // list of entities that refer to this
                                 // entity in their definition 
    MemberDict *sourceRefDict;
    int m_testId;     // id for test case
    int m_todoId;     // id for todo case

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
