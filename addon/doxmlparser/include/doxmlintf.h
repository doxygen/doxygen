#ifndef _DOXMLINTF_H
#define _DOXMLINTF_H

#include <qstring.h>

class IMember;
class IDocIterator;

class ILinkedText
{
  public:
    enum Kind { Kind_Text, Kind_Ref };
    virtual Kind kind() const = 0;
};

class ILT_Text : public ILinkedText
{
  public:
    virtual QString text() const = 0;
};

class ILT_Ref : public ILinkedText
{
  public:
    virtual QString id() const = 0;
    virtual QString external() const = 0;
    virtual QString text() const = 0;
};

class ILinkedTextIterator 
{
  public:
    virtual ILinkedText *toFirst() = 0;
    virtual ILinkedText *toLast() = 0;
    virtual ILinkedText *toNext() = 0;
    virtual ILinkedText *toPrev() = 0;
    virtual ILinkedText *current() const = 0;
    virtual void release() = 0;
};

class IParam 
{
  public:
    virtual ILinkedTextIterator *type() const = 0;
    virtual QString declarationName() const = 0;
    virtual QString definitionName() const = 0;
    virtual QString attrib() const = 0;
    virtual QString arraySpecifier() const = 0;
    virtual ILinkedTextIterator *defaultValue() const = 0;
};

class IParamIterator
{
  public:
    virtual IParam *toFirst() = 0;
    virtual IParam *toLast() = 0;
    virtual IParam *toNext() = 0;
    virtual IParam *toPrev() = 0;
    virtual IParam *current() const = 0;
    virtual void release() = 0;
};

class IMemberReference 
{
  public:
    virtual IMember *member() const = 0;
    virtual QString memberName() const = 0;
};

class IMemberReferenceIterator 
{
  public:
    virtual IMemberReference *toFirst() = 0;
    virtual IMemberReference *toLast() = 0;
    virtual IMemberReference *toNext() = 0;
    virtual IMemberReference *toPrev() = 0;
    virtual IMemberReference *current() const = 0;
    virtual void release() = 0;
};

class IEnumValue
{
  public:
    virtual QString name() const = 0;
    virtual QString initializer() const = 0;
};

class IEnumValueIterator 
{
  public:
    virtual IEnumValue *toFirst() = 0;
    virtual IEnumValue *toLast() = 0;
    virtual IEnumValue *toNext() = 0;
    virtual IEnumValue *toPrev() = 0;
    virtual IEnumValue *current() const = 0;
    virtual void release() = 0;
};

class IDoc
{
  public:
    enum Kind 
    { 
      Para,               //  0 -> IDocPara
      Text,               //  1 -> IDocText
      MarkupModifier,     //  2 -> IDocMarkupModifier
      ItemizedList,       //  3 -> IDocList
      OrderedList,        //  4 -> IDocList
      ListItem,           //  5 -> IDocListItem
      ParameterList,      //  6 -> IDocParameterList
      Parameter,          //  7 -> IDocParameter
      SimpleSect,         //  8 -> IDocSimpleSect
      Title,              //  9 -> IDocTitle
      Ref,                // 10 -> IDocRef
      VariableList,       // 11 -> IDocVariableList
      VariableListEntry,  // 12 -> IDocVariableListEntry
      HRuler,             // 13 -> IDocHRuler
      LineBreak,          // 14 -> IDocLineBreak
      ULink,              // 15 -> IDocULink
      EMail,              // 16 -> IDocEMail
      Link,               // 17 -> IDocLink
      ProgramListing,     // 18 -> IDocProgramListing
      CodeLine,           // 19 -> IDocCodeLine
      Highlight,          // 20 -> IDocHighlight
      Anchor,             // 21 -> IDocAnchor
      Formula,            // 22 -> IDocFormula
      Image,              // 23 -> IDocImage
      DotFile,            // 24 -> IDocDotFile
      IndexEntry,         // 25 -> IDocIndexEntry
      Table,              // 26 -> IDocTable
      Row,                // 27 -> IDocRow
      Entry,              // 28 -> IDocEntry
      Section,            // 29 -> IDocSection
      Root                // 30 -> IDocRoot
    };
    virtual Kind kind() const = 0;
};

class IDocMarkup : public IDoc
{
  public:
    enum Markup 
    { 
      Normal         = 0x00,
      Bold           = 0x01,
      Emphasis       = 0x02,
      ComputerOutput = 0x04,
      Subscript      = 0x08,
      Superscript    = 0x10,
      SmallFont      = 0x20,
      Center         = 0x40
    };

};

class IDocPara : public IDoc
{
  public:
    virtual IDocIterator *contents() const = 0; 
};

class IDocText : public IDocMarkup
{
  public:
    virtual QString text() const = 0;
    virtual int markup() const = 0;
};

class IDocMarkupModifier : public IDoc
{
};

class IDocList : public IDoc
{
};

class IDocListItem : public IDoc
{
};

class IDocParameterList : public IDoc
{
};

class IDocParameter : public IDoc
{
};

class IDocSimpleSect : public IDoc
{
};

class IDocTitle : public IDoc
{
  public:
    virtual IDocIterator *title() const = 0;
};

class IDocRef : public IDoc
{
};

class IDocVariableList : public IDoc
{
};

class IDocVariableListEntry : public IDoc
{
};

class IDocHRuler : public IDoc
{
};

class IDocLineBreak : public IDoc
{
};

class IDocULink : public IDoc
{
};

class IDocEMail : public IDoc
{
};

class IDocLink : public IDoc
{
};

class IDocProgramListing : public IDoc
{
};

class IDocCodeLine : public IDoc
{
};

class IDocHighlight : public IDoc
{
};

class IDocAnchor : public IDoc
{
};

class IDocFormula : public IDoc
{
};

class IDocImage : public IDoc
{
};

class IDocDotFile : public IDoc
{
};

class IDocIndexEntry : public IDoc
{
};

class IDocTable : public IDoc
{
};

class IDocRow : public IDoc
{
};

class IDocEntry : public IDoc
{
};

class IDocSection : public IDoc
{
  public:
    virtual QString id() const = 0; 
    virtual int level() const = 0;
    virtual IDocIterator *title() const = 0;
};

class IDocRoot : public IDoc
{
  public:
    virtual IDocIterator *contents() const = 0; 
};

class IDocIterator 
{
  public:
    virtual IDoc *toFirst() = 0;
    virtual IDoc *toLast() = 0;
    virtual IDoc *toNext() = 0;
    virtual IDoc *toPrev() = 0;
    virtual IDoc *current() const = 0;
    virtual void release() = 0;
};

class IMember 
{
  public:
    virtual QString kind() const = 0;
    virtual QString id() const = 0;
    virtual QString protection() const = 0;
    virtual QString virtualness() const = 0;
    virtual ILinkedTextIterator *type() const = 0;
    virtual QString name() const = 0;
    virtual bool isConst() const = 0;
    virtual bool isVolatile() const = 0;
    virtual IParamIterator *params() const = 0;
    virtual ILinkedTextIterator *initializer() const = 0;
    virtual ILinkedTextIterator *exceptions() const = 0;
    virtual IMemberReferenceIterator *references() const = 0; 
    virtual IMemberReferenceIterator *referencedBy() const = 0;
    virtual int bodyStart() const = 0;
    virtual int bodyEnd() const = 0;
    virtual QString definitionFile() const = 0;
    virtual int definitionLine() const = 0;
    virtual IMemberReference *reimplements() const = 0;
    virtual IMemberReferenceIterator *reimplementedBy() const = 0;
    virtual IEnumValueIterator *enumValues() const = 0;
    virtual IDocRoot *briefDescription() const = 0;
    virtual IDocRoot *detailedDescription() const = 0;
};

class IMemberIterator 
{
  public:
    virtual IMember *toFirst() = 0;
    virtual IMember *toLast() = 0;
    virtual IMember *toNext() = 0;
    virtual IMember *toPrev() = 0;
    virtual IMember *current() const = 0;
    virtual void release() = 0;
};

class ISection 
{
  public:
    virtual QString kind() const = 0;
    virtual IMemberIterator *members() const = 0;
};

class ISectionIterator 
{
  public:
    virtual ISection *toFirst() = 0;
    virtual ISection *toLast() = 0;
    virtual ISection *toNext() = 0;
    virtual ISection *toPrev() = 0;
    virtual ISection *current() const = 0;
    virtual void release() = 0;
};

class ICompound 
{
  public:
    virtual QString name() const = 0;
    virtual QString id()   const = 0;
    virtual QString kind() const = 0;
    virtual ISectionIterator *sections() const = 0;
    virtual IDocRoot *briefDescription() const = 0;
    virtual IDocRoot *detailedDescription() const = 0;
};

class ICompoundIterator 
{
  public:
    virtual ICompound *toFirst() = 0;
    virtual ICompound *toLast() = 0;
    virtual ICompound *toNext() = 0;
    virtual ICompound *toPrev() = 0;
    virtual ICompound *current() const = 0;
    virtual void release() = 0;
};

/*! Root node of the object model. */
class IDoxygen 
{
  public:
    /*! Returns an iterator that can be used to iterate over the list
     *  of compounds found in the project.
     */
    virtual ICompoundIterator *compounds() const = 0;

    /*! Returns a compound given its unique \a id. If you have a
     *  compound id this function is much more efficient than iterating
     *  over the compound list. Returns 0 if the id is not valid.
     */
    virtual ICompound *compoundById(const QString &id) const = 0;

    /*! Returns a compound given its name (including the scope). 
     *  Returns 0 if the name is not found in the project.
     */
    virtual ICompound *compoundByName(const QString &name) const = 0;

    /*! Returns an interface to a member given its id. 
     *  @param id The member id.
     */
    virtual IMember *memberById(const QString &id) const = 0;

    /*! Returns a list of all members with a certain name. 
     *  @param name The name of the member.
     */
    virtual IMemberIterator *memberByName(const QString &name) const = 0;
    
    /*! Releases the memory for the object hierarchy obtained by 
     *  createdObjecModelFromXML(). First release all iterators before calling
     *  this function.
     */
    virtual void release() = 0;
};

/*! Factory method that creates an object model given an XML file generated
 *  by doxygen.
 *  @param xmlFileName The name of the XML to parse.
 *  @returns An iterface to the object model.
 */
IDoxygen *createObjectModelFromXML(const char *xmlFileName);

#endif
