/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _DOXMLINTF_H
#define _DOXMLINTF_H

/*! \file
 *  \brief The interface to the object model provided by the XML parser 
 *         library.
 *
 *  To start using this library one calls createObjectModel() and then
 *  uses the returned IDoxygen interface to read doxygen generated
 *  XML output and navigate through the information contained in it.
 *
 *  @see createObjectModel()
 */

class IMember;
class IDocIterator;
class ICompound;
class ISection;
class INode;

/*! \brief Read only interface to a string. 
 */
class IString
{
  public:
    /*! Returns a latin1 character representation of the string. */
    virtual const char *latin1() const = 0;
    /*! Returns a 16-bit unicode character representation of the character at 
     *  position \a index in the string. The first character is at index 0.
     */
    virtual unsigned short unicodeCharAt(int index) const = 0;
    /*! Returns true if this string is empty or false otherwise */
    virtual bool isEmpty() const = 0;
    /*! Returns the number of characters in the string. */
    virtual int length() const = 0;
};

class ILinkedText
{
  public:
    enum Kind { Kind_Text, Kind_Ref };
    virtual Kind kind() const = 0;
};

class ILT_Text : public ILinkedText
{
  public:
    virtual const IString *text() const = 0;
};

class ILT_Ref : public ILinkedText
{
  public:
    enum TargetKind { Member, Compound };
    virtual const IString *id() const = 0;
    virtual TargetKind targetKind() const = 0;
    virtual const IString *external() const = 0;
    virtual const IString *text() const = 0;
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
    virtual const IString * declarationName() const = 0;
    virtual const IString * definitionName() const = 0;
    virtual const IString * attrib() const = 0;
    virtual const IString * arraySpecifier() const = 0;
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
    virtual const IString * memberName() const = 0;
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

class IDoc
{
  public:
    enum Kind 
    { 
      Invalid = 0,        //  0
      Para,               //  1 -> IDocPara
      Text,               //  2 -> IDocText
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
      Verbatim,           // 30 -> IDocVerbatim
      Copy,               // 31 -> IDocCopy
      TocList,            // 32 -> IDocTocList
      TocItem,            // 33 -> IDocTocItem
      Anchor,             // 34 -> IDocAnchor
      Symbol,             // 35 -> IDocSymbol
      Root                // 36 -> IDocRoot
    };
    virtual Kind kind() const = 0;
};

class IDocMarkup : public IDoc
{
  public:
    enum Markup 
    { 
      Normal         = 0x000,
      Bold           = 0x001,
      Emphasis       = 0x002,
      ComputerOutput = 0x004,
      Subscript      = 0x008,
      Superscript    = 0x010,
      SmallFont      = 0x020,
      Center         = 0x040,
      Preformatted   = 0x080,
      Heading        = 0x100
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
    virtual const IString * text() const = 0;
    virtual int markup() const = 0;
    virtual int headingLevel() const = 0;
};

class IDocMarkupModifier : public IDoc
{
  public:
    virtual bool enabled() const = 0;
    virtual int markup() const = 0;
    virtual int headingLevel() const = 0;
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
    virtual const IString * name() const = 0;
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
    virtual Types type() const = 0;
    virtual const IString * typeString() const = 0;
    virtual IDocTitle *title() const = 0;
    virtual IDocPara *description() const = 0;
};

class IDocRef : public IDoc
{
  public:
    enum TargetKind { Member, Compound };
    virtual const IString * refId() const = 0;
    virtual TargetKind targetKind() const = 0;
    virtual const IString * external() const = 0;
    virtual const IString * text() const = 0;
};

class IDocVariableList : public IDoc
{
  public:
    virtual IDocIterator *entries() const = 0;
};

class IDocVariableListEntry : public IDoc
{
  public:
    virtual ILinkedTextIterator * term() const = 0;
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
    virtual const IString * url() const = 0;
    virtual const IString * text() const = 0;
};

class IDocEMail : public IDoc
{
  public:
    virtual const IString * address() const = 0;
};

class IDocLink : public IDoc
{
  public:
    virtual const IString * refId() const = 0;
    virtual const IString * text() const = 0;
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
    virtual const IString * refId() const = 0;
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
    virtual const IString * id() const = 0;
    virtual const IString * text() const = 0;
};

class IDocImage : public IDoc
{
  public:
    virtual const IString * name() const = 0;
    virtual const IString * caption() const = 0;
};

class IDocDotFile : public IDoc
{
  public:
    virtual const IString * name() const = 0;
    virtual const IString * caption() const = 0;
};

class IDocIndexEntry : public IDoc
{
  public:
    virtual const IString * primary() const = 0;
    virtual const IString * secondary() const = 0;
};

class IDocTable : public IDoc
{
  public:
    virtual IDocIterator *rows() const = 0;
    virtual int numColumns() const = 0;
    virtual const IString * caption() const = 0;
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
    virtual const IString * id() const = 0; 
    virtual int level() const = 0;
    virtual IDocIterator *title() const = 0;
};

class IDocTocList : public IDoc
{
  public:
    virtual IDocIterator *elements() const = 0;
};

class IDocTocItem : public IDoc
{
  public:
    virtual const IString *id() const = 0; 
    virtual const IString *title() const = 0;
};

class IDocCopy : public IDoc
{
  public:
    virtual IDocIterator *contents() const = 0; 
};

class IDocVerbatim : public IDoc
{
  public:
    enum Types { Invalid = 0, HtmlOnly, LatexOnly, Verbatim };
    virtual const IString *text() const = 0; 
    virtual Types type() const = 0;
};

class IDocAnchor : public IDoc
{
  public:
    virtual const IString *id() const = 0; 
};

class IDocSymbol : public IDoc
{
  public:
    enum Types 
    { Invalid = 0,
      Umlaut, Acute, Grave, Circ, Tilde, Szlig, Cedil, Ring, Nbsp, Copy
    };
    virtual Types type() const = 0;
    virtual const IString * typeString() const = 0;
    virtual char letter() const = 0;
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
    virtual const IString * label() const = 0;
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
                        PrivateInheritance, Usage, TemplateInstance
                      };
    virtual INode * node() const = 0;
    virtual NodeRelation relation() const = 0;
    virtual const IString * relationString() const = 0;
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
    virtual const IString * id() const = 0;
    virtual const IString * label() const = 0;
    virtual const IString * linkId() const = 0;
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
};

class IMember 
{
  public:
    enum MemberKind { Invalid=0,
                      Define, Property, Variable, Typedef, Enum,
                      Function, Signal, Prototype, Friend, DCOP, Slot, 
                      EnumValue
                    };
    virtual ICompound *compound() const = 0;
    virtual ISection *section() const = 0;
    virtual MemberKind kind() const = 0;
    virtual const IString * kindString() const = 0;
    virtual const IString * id() const = 0;
    virtual const IString * protection() const = 0;
    virtual const IString * virtualness() const = 0;
    virtual ILinkedTextIterator *type() const = 0;
    virtual const IString * typeString() const = 0;
    virtual const IString * name() const = 0;
    virtual bool isConst() const = 0;
    virtual bool isVolatile() const = 0;
    virtual IParamIterator *parameters() const = 0;
    virtual IParamIterator *templateParameters() const = 0;
    virtual ILinkedTextIterator *initializer() const = 0;
    virtual ILinkedTextIterator *exceptions() const = 0;
    virtual IMemberReferenceIterator *references() const = 0; 
    virtual IMemberReferenceIterator *referencedBy() const = 0;
    virtual int bodyStart() const = 0;
    virtual int bodyEnd() const = 0;
    virtual const IString * definitionFile() const = 0;
    virtual int definitionLine() const = 0;
    virtual IMemberReference *reimplements() const = 0;
    virtual IMemberReferenceIterator *reimplementedBy() const = 0;
    virtual IDocRoot *briefDescription() const = 0;
    virtual IDocRoot *detailedDescription() const = 0;
};

class IDefine : public IMember
{
  public:
};

class IProperty : public IMember
{
  public:
};

class IVariable : public IMember
{
  public:
};

class ITypedef : public IMember
{
  public:
};

class IFunction : public IMember
{
  public:
};

class ISignal : public IMember
{
  public:
};

class IPrototype : public IMember
{
  public:
};

class IFriend : public IMember
{
  public:
};

class IDCOP : public IMember
{
  public:
};

class ISlot : public IMember
{
  public:
};

class IEnumValue : public IMember
{
  public:
    virtual const IString * name() const = 0;
};

#if 0
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
#endif


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

class IEnum : public IMember
{
  public:
    virtual IMemberIterator *enumValues() const = 0;
};

/*! \brief The interface to a section in the object model. 
 *
 *  A compound can have a number of sections, where each
 *  section contains a set of members with the properties implied by
 *  the section kind. The kind() method returns the kind of the section.
 *  The members of the section can be accessed via members(). Apart
 *  from using kind(), some of the individual properties of the section can 
 *  also be inspected via isStatic(), isPublic(), isProtected() and 
 *  isPrivate().
 */
class ISection 
{
  public:
    /*! Possible section types */
    enum SectionKind 
    { Invalid=0,
      UserDefined,         //!< A user defined member group 
      PubTypes,            //!< Public member typedefs
      PubFuncs,            //!< Public member functions
      PubAttribs,          //!< Public member attributes 
      PubSlots,            //!< Public Qt Slots
      Signals,             //!< Qt Signals
      DCOPFuncs,           //!< KDE-DCOP interface functions
      Properties,          //!< IDL properties
      PubStatFuncs,        //!< Public static member functions
      PubStatAttribs,      //!< Public static attributes
      ProTypes,            //!< Protected member typedefs
      ProFuncs,            //!< Protected member functions
      ProAttribs,          //!< Protected member attributes
      ProSlots,            //!< Protected slots
      ProStatFuncs,        //!< Protected static member functions
      ProStatAttribs,      //!< Protected static member attributes
      PriTypes,            //!< Private member typedefs
      PriFuncs,            //!< Private member functions
      PriAttribs,          //!< Private member attributes
      PriSlots,            //!< Private Qt slots
      PriStatFuncs,        //!< Private static member functions
      PriStatAttribs,      //!< Private static member attributes
      Friend,              //!< Friends
      Related,             //!< Function marked as related
      Defines,             //!< Preprocessor defines
      Prototypes,          //!< Global function prototypes
      Typedefs,            //!< Global typedefs
      Enums,               //!< Enumerations
      Functions,           //!< Global functions
      Variables            //!< Global variables
    };
    
    /*! Returns a string representation of the value returned by kind() */
    virtual const IString * kindString() const = 0;
    
    /*! Returns what kind of section this is */
    virtual SectionKind kind() const = 0;
    
    /*! Returns an iterator for the members of this section */
    virtual IMemberIterator *members() const = 0;
    
    /*! Returns \c true if this section contains statics */
    virtual bool isStatic() const = 0;
    
    /*! Returns \c true if this section belongs to a 
     *  public section of a class 
     */
    virtual bool isPublic() const = 0;
    
    /*! Returns \c true if this section belongs to a 
     *  private section of a class 
     */
    virtual bool isPrivate() const = 0;
    
    /*! Returns \c true if this section belongs to a 
     *  protected section of a class 
     * */
    virtual bool isProtected() const = 0;
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

/*! \brief The interface to a compound in the object model. 
 *
 *  A compound has a name which can be obtained via the name() method 
 *  and a unique id, which is return via the id() method.
 *  A compound consists zero or more members which are grouped into sections. 
 *  The sections() method can be used to access the individual sections. 
 *  Alternatively, members can be obtained by name or id. There are 
 *  different types of compounds. The kind() method returns what kind of 
 *  compound this is. Depending on the return value one can dynamically 
 *  cast an interface pointer to an more specialised interface that provides 
 *  additional methods.
 *  Example:
 *  \code
 *  ICompound *comp=...;
 *  if (comp->kind()==ICompound::Class)
 *  {
 *    IClass *cls = dynamic_cast<IClass*>(comp);
 *    // use methods of IClass
 *  }
 *  \endcode
 *  The documentation that is provided by a compound is available via
 *  the briefDescription() and detailedDescription() methods.
 *  To avoid excessive memory usage, release() should be called (once) on each 
 *  compound interface pointer that is no longer needed.
 */
class ICompound 
{
  public:
    /*! Represents the kind of compounds recognised by doxygen. */
    enum CompoundKind { Invalid=0,
                        Class, Struct, Union, Interface, Exception,
                        Namespace, File, Group, Page
                      };
    /*! Returns the name of this compound */
    virtual const IString * name() const = 0;

    /*! Returns the id of this compound. The id is a
     *  unique string representing a specific compound object.
     */
    virtual const IString * id()   const = 0;

    /*! Returns the kind of compound. See #CompoundKind for possible
     *  values.
     */
    virtual CompoundKind kind() const = 0;

    /*! Returns a string representation of the compound kind.
     *  @see kind()
     */
    virtual const IString * kindString() const = 0;

    /*! Returns an iterator for the different member sections in this
     *  compound.
     */
    virtual ISectionIterator *sections() const = 0;

    /*! Returns a tree-structured representation of the brief
     *  description that is attached to this compound.
     */
    virtual IDocRoot *briefDescription() const = 0;

    /*! Returns a tree-structured representation of the detailed
     *  description that is attached to this compound.
     */
    virtual IDocRoot *detailedDescription() const = 0;

    /*! Returns an interface to a member given its id. 
     *  @param id The member id.
     */
    virtual IMember *memberById(const char * id) const = 0;

    /*! Returns a list of all members within the compound having a certain 
     *  name. Member overloading is the reason why there can be more than 
     *  one member. 
     *  @param name The name of the member.
     */
    virtual IMemberIterator *memberByName(const char * name) const = 0;

    /*! Decreases the reference counter for this compound. If it reaches
     *  zero, the memory for the compound will be released.
     */
    virtual void release() = 0;
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

class IRelatedCompound
{
  public:
    enum Protection { Public, Protected, Private };
    enum Kind { Normal, Virtual };
    virtual ICompound *compound() const = 0;
    virtual Protection protection() const = 0;
    virtual Kind kind() const = 0;

};

class IRelatedCompoundIterator 
{
  public:
    virtual IRelatedCompound *toFirst() = 0;
    virtual IRelatedCompound *toLast() = 0;
    virtual IRelatedCompound *toNext() = 0;
    virtual IRelatedCompound *toPrev() = 0;
    virtual IRelatedCompound *current() const = 0;
    virtual void release() = 0;
};

/*! \brief The interface to a class in the object model. 
 */
class IClass : public ICompound
{
  public:
    virtual IGraph *inheritanceGraph() const = 0;
    virtual IGraph *collaborationGraph() const = 0;
    virtual IRelatedCompoundIterator *baseCompounds() const = 0;
    virtual IRelatedCompoundIterator *derivedCompounds() const = 0;
    virtual ICompoundIterator *nestedCompounds() const = 0;
    virtual IParamIterator *templateParameters() const = 0;
    virtual const IString *locationFile() const = 0;
    virtual int locationLine() const = 0;
    virtual int locationBodyStartLine() const = 0;
    virtual int locationBodyEndLine() const = 0;

    // TODO:
    // class:
    //  listOfAllMembers()
    //  protection()
    //  isAbstract()
};

/*! \brief The interface to a struct in the object model. 
 */
class IStruct : public ICompound
{
  public:
    virtual ICompoundIterator *nestedCompounds() const = 0;
    virtual IRelatedCompoundIterator *baseCompounds() const = 0;
    virtual IRelatedCompoundIterator *derivedCompounds() const = 0;
    virtual const IString *locationFile() const = 0;
    virtual int locationLine() const = 0;
    virtual int locationBodyStartLine() const = 0;
    virtual int locationBodyEndLine() const = 0;
};

/*! \brief The interface to a union in the object model. 
 */
class IUnion : public ICompound
{
  public:
    virtual ICompoundIterator *nestedCompounds() const = 0;
};

/*! \brief The interface to a Java/IDL interface in the object model. 
 */
class IInterface : public ICompound
{
  public:
    virtual IRelatedCompoundIterator *baseCompounds() const = 0;
    virtual IRelatedCompoundIterator *derivedCompounds() const = 0;
};

/*! \brief The interface to a Java/IDL exception in the object model. 
 */
class IException : public ICompound
{
};

/*! \brief The interface to a namespace in the object model. 
 */
class INamespace : public ICompound
{
  public:
    virtual ICompoundIterator *nestedCompounds() const = 0;
};

/*! \brief The interface to a file in the object model. 
 */
class IFile : public ICompound
{
  public:
    virtual IGraph *includeDependencyGraph() const = 0;
    virtual IGraph *includedByDependencyGraph() const = 0;
    virtual IDocProgramListing *source() const = 0;
    virtual ICompoundIterator *nestedCompounds() const = 0;

    // file:
    //  includes()
    //  includedBy()
    //  ICompound *innerNamespaces()
    //  ICompoundIterator *innerClasses()
};

/*! \brief The interface to a group in the object model. 
 */
class IGroup : public ICompound
{
  public:
    virtual ICompoundIterator *nestedCompounds() const = 0;
    // group:
    //  Title()
    //  innerFile()
    //  innerPage()
};

/*! \brief The interface to a page in the object model. 
 */
class IPage : public ICompound
{
  public:
    virtual const IDocTitle *title() const = 0;
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
    virtual ICompound *compoundById(const char * id) const = 0;

    /*! Returns a compound given its name (including the scope). 
     *  Returns 0 if the name is not found in the project.
     */
    virtual ICompound *compoundByName(const char * name) const = 0;

    /*! Returns an interface to a compound containing a member given it the
     *  member's id. Given the ICompound interface one can use the same id
     *  to obtain the IMember interface.
     *  @param id The member id.
     */
    virtual ICompound *memberById(const char * id) const = 0;

    /*! Returns a list of all compounds containing at least one members 
     *  with a certain name. Each compound can be asked to return the
     *  list of members with that name.
     *  @param name The name of the member.
     */
    virtual ICompoundIterator *memberByName(const char * name) const = 0;

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
