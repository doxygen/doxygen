#ifndef _DOXMLINTF_H
#define _DOXMLINTF_H

#include <qstring.h>

class IMember;
class IDocIterator;
class ICompound;
class ISection;

class ILinkedText
{
  public:
    enum Kind { Kind_Text, Kind_Ref };
    virtual Kind kind() const = 0;
    virtual ~ILinkedText() {}
};

class ILT_Text : public ILinkedText
{
  public:
    virtual QString text() const = 0;
};

class ILT_Ref : public ILinkedText
{
  public:
    enum TargetKind { Member, Compound };
    virtual QString id() const = 0;
    virtual TargetKind targetKind() const = 0;
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
    virtual ~IParam() {}
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
    virtual ~IMemberReference() {}
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
    virtual ~IEnumValue() {}
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
      Invalid = 0,        //  0
      Para = 1,           //  1 -> IDocPara
      Text = 2,           //  2 -> IDocText
      MarkupModifier,     //  3 -> IDocMarkupModifier
      ItemizedList,       //  4 -> IDocItemizedList
      OrderedList,        //  5 -> IDocOrderedList
      ListItem,           //  6 -> IDocListItem
      ParameterList,      //  7 -> IDocParameterList
      Parameter,          //  8 -> IDocParameter
      SimpleSect,         //  9 -> IDocSimpleSect
      Title,              // 10 -> IDocTitle
      Ref,                // 11 -> IDocRef
      VariableList,       // 12 -> IDocVariableList
      VariableListEntry,  // 13 -> IDocVariableListEntry
      HRuler,             // 14 -> IDocHRuler
      LineBreak,          // 15 -> IDocLineBreak
      ULink,              // 16 -> IDocULink
      EMail,              // 17 -> IDocEMail
      Link,               // 18 -> IDocLink
      ProgramListing,     // 19 -> IDocProgramListing
      CodeLine,           // 20 -> IDocCodeLine
      Highlight,          // 21 -> IDocHighlight
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
    virtual ~IDoc() {}
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
  public:
    virtual bool enabled() const = 0;
    virtual int markup() const = 0;
};

class IDocItemizedList : public IDoc
{
  public:
    virtual IDocIterator *elements() const = 0;
};

class IDocOrderedList : public IDoc
{
  public:
    virtual IDocIterator *elements() const = 0;
};

class IDocListItem : public IDoc
{
  public:
    virtual IDocIterator *contents() const = 0;
};

class IDocParameterList : public IDoc
{
  public:
    enum Types { Param, RetVal, Exception };
    virtual Types listType() const = 0;
    virtual IDocIterator *params() const = 0;
};

class IDocParameter : public IDoc
{
  public:
    virtual QString name() const = 0;
    virtual IDocPara *description() const = 0;
};

class IDocTitle : public IDoc
{
  public:
    virtual IDocIterator *title() const = 0;
};

class IDocSimpleSect : public IDoc
{
  public:
    enum Types { Invalid = 0,
                 See, Return, Author, Version, 
                 Since, Date, Bug, Note,
                 Warning, Par, Deprecated, Pre, 
                 Post, Invar, Remark, Attention,
                 Todo, Test, RCS, EnumValues, 
                 Examples
    };
    virtual Types sectionType() const = 0;
    virtual IDocTitle *title() const = 0;
    virtual IDocPara *description() const = 0;
};

class IDocRef : public IDoc
{
  public:
    enum TargetKind { Member, Compound };
    virtual QString refId() const = 0;
    virtual TargetKind targetKind() const = 0;
    virtual QString external() const = 0;
    virtual QString text() const = 0;
};

class IDocVariableList : public IDoc
{
  public:
    virtual IDocIterator *entries() const = 0;
};

class IDocVariableListEntry : public IDoc
{
  public:
    virtual QString term() const = 0;
    virtual IDocPara *description() const = 0;
};

class IDocHRuler : public IDoc
{
};

class IDocLineBreak : public IDoc
{
};

class IDocULink : public IDoc
{
  public:
    virtual QString url() const = 0;
    virtual QString text() const = 0;
};

class IDocEMail : public IDoc
{
  public:
    virtual QString address() const = 0;
};

class IDocLink : public IDoc
{
  public:
    virtual QString refId() const = 0;
    virtual QString text() const = 0;
};

class IDocProgramListing : public IDoc
{
  public:
    virtual IDocIterator *codeLines() const = 0;
};

class IDocCodeLine : public IDoc
{
  public:
    virtual int lineNumber() const = 0;
    virtual QString refId() const = 0;
    virtual IDocIterator *codeElements() const = 0;
};

class IDocHighlight : public IDoc
{
  public:
    enum HighlightKind 
    { Invalid=0,
      Comment, Keyword, 
      KeywordType, KeywordFlow, CharLiteral, 
      StringLiteral, Preprocessor
    };
    virtual HighlightKind highlightKind() const = 0;
    virtual IDocIterator *codeElements() const = 0;
};

class IDocFormula : public IDoc
{
  public:
    virtual QString id() const = 0;
    virtual QString text() const = 0;
};

class IDocImage : public IDoc
{
  public:
    virtual QString name() const = 0;
    virtual QString caption() const = 0;
};

class IDocDotFile : public IDoc
{
  public:
    virtual QString name() const = 0;
    virtual QString caption() const = 0;
};

class IDocIndexEntry : public IDoc
{
  public:
    virtual QString primary() const = 0;
    virtual QString secondary() const = 0;
};

class IDocTable : public IDoc
{
  public:
    virtual IDocIterator *rows() const = 0;
    virtual int numColumns() const = 0;
    virtual QString caption() const = 0;
};

class IDocRow : public IDoc
{
  public:
    virtual IDocIterator *entries() const = 0;
};

class IDocEntry : public IDoc
{
  public:
    virtual IDocIterator *contents() const = 0;
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

class IEdgeLabel
{
  public:
    virtual QString label() = 0;
};

class IEdgeLabelIterator 
{
  public:
    virtual IEdgeLabel *toFirst() = 0;
    virtual IEdgeLabel *toLast() = 0;
    virtual IEdgeLabel *toNext() = 0;
    virtual IEdgeLabel *toPrev() = 0;
    virtual IEdgeLabel *current() const = 0;
    virtual void release() = 0;
};

class IChildNode
{
  public:
    enum NodeRelation { PublicInheritance, ProtectedInheritance,
                        PrivateInheritance, Usage, TemplateInstace
                      };
    virtual QString id() const = 0;
    virtual NodeRelation relation() const = 0;
    virtual IEdgeLabelIterator *edgeLabels() const = 0;
};

class IChildNodeIterator 
{
  public:
    virtual IChildNode *toFirst() = 0;
    virtual IChildNode *toLast() = 0;
    virtual IChildNode *toNext() = 0;
    virtual IChildNode *toPrev() = 0;
    virtual IChildNode *current() const = 0;
    virtual void release() = 0;
};

class INode
{
  public:
    virtual QString id() const = 0;
    virtual QString label() const = 0;
    virtual QString linkId() const = 0;
    virtual IChildNodeIterator *children() const = 0;
};

class INodeIterator 
{
  public:
    virtual INode *toFirst() = 0;
    virtual INode *toLast() = 0;
    virtual INode *toNext() = 0;
    virtual INode *toPrev() = 0;
    virtual INode *current() const = 0;
    virtual void release() = 0;
};

class IGraph
{
  public:
    virtual INodeIterator *nodes() const = 0;
    virtual ~IGraph() {}
};

class IMember 
{
  public:
    enum MemberKind { Invalid=0,
                      Define, Property, Variable, Typedef, Enum,
                      Function, Signal, Prototype, Friend, DCOP, Slot
                    };
    virtual ICompound *compound() const = 0;
    virtual ISection *section() const = 0;
    virtual MemberKind kind() const = 0;
    virtual QString kindString() const = 0;
    virtual QString id() const = 0;
    virtual QString protection() const = 0;
    virtual QString virtualness() const = 0;
    virtual ILinkedTextIterator *type() const = 0;
    virtual QString typeString() const = 0;
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
    virtual ~IMember() {}
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
    enum SectionKind { Invalid=0,
                       UserDefined,
                       PubTypes, PubFuncs, PubAttribs, PubSlots,
                       Signals, DCOPFuncs, Properties,
                       PubStatFuncs, PubStatAttribs,
                       ProTypes, ProFuncs, ProAttribs, ProSlots,
                       ProStatFuncs, ProStatAttribs,
                       PriTypes, PriFuncs, PriAttribs, PriSlots,
                       PriStatFuncs, PriStatAttribs, 
                       Friend, Related, Defines, Prototypes, Typedefs,
                       Enums, Functions, Variables
                     };
    virtual QString kindString() const = 0;
    virtual SectionKind kind() const = 0;
    virtual IMemberIterator *members() const = 0;
    virtual bool isStatic() const = 0;
    virtual bool isPublic() const = 0;
    virtual bool isPrivate() const = 0;
    virtual bool isProtected() const = 0;
    virtual ~ISection() {}
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
    enum CompoundKind { Invalid=0,
                        Class, Struct, Union, Interface, Exception,
                        Namespace, File, Group, Page, Package 
                      };
    virtual QString name() const = 0;
    virtual QString id()   const = 0;
    virtual CompoundKind kind() const = 0;
    virtual QString kindString() const = 0;
    virtual ISectionIterator *sections() const = 0;
    virtual IDocRoot *briefDescription() const = 0;
    virtual IDocRoot *detailedDescription() const = 0;

    /*! Returns an interface to a member given its id. 
     *  @param id The member id.
     */
    virtual IMember *memberById(const QString &id) const = 0;

    /*! Returns a list of all members within the compound having a certain 
     *  name. Overloading is the reason why there can be more than one member. 
     *  @param name The name of the member.
     */
    virtual IMemberIterator *memberByName(const QString &name) const = 0;

    /*! Decreases the reference counter for this compound. If it reaches
     *  zero, the memory for the compound will be released.
     */
    virtual void release() = 0;

    // TODO:
    // class:
    //  IRelatedCompoundIterator *baseClasses()
    //  IRelatedCompoundIterator *derivedClasses()
    //  ICompoundIterator *innerClasses()
    //  ITemplateParamListIterator *templateParamLists()
    //  listOfAllMembers()
    //  IDotGraph *inheritanceGraph()
    //  IDotGraph *collaborationGraph()
    //  locationFile()
    //  locationLine()
    //  locationBodyStartLine()
    //  locationBodyEndLine()
    // namespace:
    //  ICompound *innerNamespaces()
    // file:
    //  includes()
    //  includedBy()
    //  IDotGraph *includeDependencyGraph()
    //  IDotGraph *includedByDependencyGraph()
    //  IDocProgramListing *source()
    // group:
    //  Title()
    //  innerFile()
    //  innerPage()
    // page:
};

class ICompoundIterator 
{
  public:
    virtual void toFirst() = 0;
    virtual void toLast() = 0;
    virtual void toNext() = 0;
    virtual void toPrev() = 0;
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

    /*! Returns an interface to a compound containing a member given it the
     *  member's id. Given the ICompound interface one can use the same id
     *  to obtain the IMember interface.
     *  @param id The member id.
     */
    virtual ICompound *memberById(const QString &id) const = 0;

    /*! Returns a list of all compounds containing at least one members 
     *  with a certain name. Each compound can be asked to return the
     *  list of members with that name.
     *  @param name The name of the member.
     */
    virtual ICompoundIterator *memberByName(const QString &name) const = 0;

    /*! Releases the memory for the object hierarchy obtained by 
     *  createdObjecModelFromXML(). First release all iterators before calling
     *  this function.
     */
    virtual void release() = 0;

    /*! Sets the debug level.
     *  - 0 all debugging messages are disabled (the default).
     *  - 1 display important messages only
     *  - 2 display any messages.
     */
    virtual void setDebugLevel(int level) = 0;

    /*! Reads an XML directory produced by doxygen and builds up a data 
     *  structure representing the contents of the XML files in the directory. 
     */
    virtual bool readXMLDir(const char *xmlDirName) = 0;
};

/*! Factory method that creates an empty object model for a doxygen generated XML file.
 *  Use the readXMLDir() method to build the model from an XML output 
 *  directory containing doxygen output.
 */
IDoxygen *createObjectModel();

#endif
