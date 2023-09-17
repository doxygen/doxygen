/******************************************************************************
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

#ifndef TYPES_H
#define TYPES_H

#include "qcstring.h"

/** @file
 *  @brief This file contains a number of basic enums and types.
 */

/** Protection level of members */
enum class Protection   { Public, Protected, Private, Package } ;

/** Virtualness of a member. */
enum class Specifier    { Normal, Virtual, Pure } ;

/** Kind of method */
enum class MethodTypes  { Method, Signal, Slot, DCOP, Property, Event };

/** Type of member relation */
enum class RelatesType  { Simple, Duplicate, MemberOf };

/** Kind of member relationship */
enum class Relationship { Member, Related, Foreign };

/** Language as given by extension */
enum SrcLangExt
{
  SrcLangExt_Unknown  = 0x00000,
  SrcLangExt_IDL      = 0x00008,
  SrcLangExt_Java     = 0x00010,
  SrcLangExt_CSharp   = 0x00020,
  SrcLangExt_D        = 0x00040,
  SrcLangExt_PHP      = 0x00080,
  SrcLangExt_ObjC     = 0x00100,
  SrcLangExt_Cpp      = 0x00200,
  SrcLangExt_JS       = 0x00400,
  SrcLangExt_Python   = 0x00800,
  SrcLangExt_Fortran  = 0x01000,
  SrcLangExt_VHDL     = 0x02000,
  SrcLangExt_XML      = 0x04000,
  //SrcLangExt_Tcl      = 0x08000, // no longer supported
  SrcLangExt_Markdown = 0x10000,
  SrcLangExt_SQL      = 0x20000,
  SrcLangExt_Slice    = 0x40000,
  SrcLangExt_Lex      = 0x80000
};

/** Grouping info */
struct Grouping
{
  /** Grouping priority */
  enum GroupPri_t
  {
    GROUPING_LOWEST,
    GROUPING_AUTO_WEAK = GROUPING_LOWEST,     //!< membership in group was defined via \@weakgroup
    GROUPING_AUTO_ADD,     //!< membership in group was defined via \@add[to]group
    GROUPING_AUTO_DEF,     //!< membership in group was defined via \@defgroup
    GROUPING_AUTO_HIGHEST = GROUPING_AUTO_DEF,
    GROUPING_INGROUP,      //!< membership in group was defined by \@ingroup
    GROUPING_HIGHEST = GROUPING_INGROUP
  };

  static const char *getGroupPriName( GroupPri_t priority )
  {
    switch( priority )
    {
      case GROUPING_AUTO_WEAK:
        return "@weakgroup";
      case GROUPING_AUTO_ADD:
        return "@addtogroup";
      case GROUPING_AUTO_DEF:
        return "@defgroup";
      case GROUPING_INGROUP:
        return "@ingroup";
    }
    return "???";
  }

  Grouping( const QCString &gn, GroupPri_t p ) : groupname(gn), pri(p) {}
  QCString groupname;   //!< name of the group
  GroupPri_t pri;       //!< priority of this definition

};

enum MemberListType
{
  MemberListType_privateLists       = 0x00800,
  MemberListType_detailedLists      = 0x01000,
  MemberListType_declarationLists   = 0x02000,
  MemberListType_documentationLists = 0x04000,
  MemberListType_exportedLists      = 0x08000,
  MemberListType_internalLists      = 0x10000,

  MemberListType_undefined               = -1,

  MemberListType_pubMethods              = 0,
  MemberListType_proMethods              = 1,
  MemberListType_pacMethods              = 2,
  MemberListType_priMethods              = 3 + MemberListType_privateLists,
  MemberListType_pubStaticMethods        = 4,
  MemberListType_proStaticMethods        = 5,
  MemberListType_pacStaticMethods        = 6,
  MemberListType_priStaticMethods        = 7 + MemberListType_privateLists,
  MemberListType_pubSlots                = 8,
  MemberListType_proSlots                = 9,
  MemberListType_priSlots                = 10 + MemberListType_privateLists,
  MemberListType_pubAttribs              = 11,
  MemberListType_proAttribs              = 12,
  MemberListType_pacAttribs              = 13,
  MemberListType_priAttribs              = 14 + MemberListType_privateLists,
  MemberListType_pubStaticAttribs        = 15,
  MemberListType_proStaticAttribs        = 16,
  MemberListType_pacStaticAttribs        = 17,
  MemberListType_priStaticAttribs        = 18 + MemberListType_privateLists,
  MemberListType_pubTypes                = 19,
  MemberListType_proTypes                = 20,
  MemberListType_pacTypes                = 21,
  MemberListType_priTypes                = 22 + MemberListType_privateLists,
  MemberListType_related                 = 23,
  MemberListType_signals                 = 24,
  MemberListType_friends                 = 25,
  MemberListType_dcopMethods             = 26,
  MemberListType_properties              = 27,
  MemberListType_events                  = 28,

  MemberListType_typedefMembers          = 29 + MemberListType_detailedLists,
  MemberListType_enumMembers             = 30 + MemberListType_detailedLists,
  MemberListType_enumValMembers          = 31 + MemberListType_detailedLists,
  MemberListType_functionMembers         = 32 + MemberListType_detailedLists,
  MemberListType_relatedMembers          = 33 + MemberListType_detailedLists,
  MemberListType_variableMembers         = 34 + MemberListType_detailedLists,
  MemberListType_propertyMembers         = 35 + MemberListType_detailedLists,
  MemberListType_eventMembers            = 36 + MemberListType_detailedLists,
  MemberListType_constructors            = 37 + MemberListType_detailedLists,

  MemberListType_allMembersList          = 38,

  MemberListType_decDefineMembers        = 39 + MemberListType_declarationLists,
  MemberListType_decProtoMembers         = 40 + MemberListType_declarationLists,
  MemberListType_decTypedefMembers       = 41 + MemberListType_declarationLists,
  MemberListType_decEnumMembers          = 42 + MemberListType_declarationLists,
  MemberListType_decFuncMembers          = 43 + MemberListType_declarationLists,
  MemberListType_decVarMembers           = 44 + MemberListType_declarationLists,
  MemberListType_decEnumValMembers       = 45 + MemberListType_declarationLists,
  MemberListType_decPubSlotMembers       = 46 + MemberListType_declarationLists,
  MemberListType_decProSlotMembers       = 47 + MemberListType_declarationLists,
  MemberListType_decPriSlotMembers       = 48 + MemberListType_declarationLists,
  MemberListType_decSignalMembers        = 49 + MemberListType_declarationLists,
  MemberListType_decEventMembers         = 50 + MemberListType_declarationLists,
  MemberListType_decFriendMembers        = 51 + MemberListType_declarationLists,
  MemberListType_decPropMembers          = 52 + MemberListType_declarationLists,

  MemberListType_docDefineMembers        = 53 + MemberListType_documentationLists,
  MemberListType_docProtoMembers         = 54 + MemberListType_documentationLists,
  MemberListType_docTypedefMembers       = 55 + MemberListType_documentationLists,
  MemberListType_docEnumMembers          = 56 + MemberListType_documentationLists,
  MemberListType_docFuncMembers          = 57 + MemberListType_documentationLists,
  MemberListType_docVarMembers           = 58 + MemberListType_documentationLists,
  MemberListType_docEnumValMembers       = 59 + MemberListType_documentationLists,
  MemberListType_docPubSlotMembers       = 60 + MemberListType_documentationLists,
  MemberListType_docProSlotMembers       = 61 + MemberListType_documentationLists,
  MemberListType_docPriSlotMembers       = 62 + MemberListType_documentationLists,
  MemberListType_docSignalMembers        = 63 + MemberListType_documentationLists,
  MemberListType_docEventMembers         = 64 + MemberListType_documentationLists,
  MemberListType_docFriendMembers        = 65 + MemberListType_documentationLists,
  MemberListType_docPropMembers          = 66 + MemberListType_documentationLists,

  MemberListType_redefinedBy             = 67,
  MemberListType_enumFields              = 68,
  MemberListType_memberGroup             = 69,

  // this one is for the summary section on the class page
  MemberListType_interfaces              = 70,
  // this one is for the detailed section on the class page
  MemberListType_interfaceMembers        = 71 + MemberListType_detailedLists,
  MemberListType_services                = 72,
  MemberListType_serviceMembers          = 73 + MemberListType_detailedLists,

  MemberListType_decSequenceMembers      = 74 + MemberListType_declarationLists,
  MemberListType_docSequenceMembers      = 75 + MemberListType_documentationLists,
  MemberListType_decDictionaryMembers    = 76 + MemberListType_declarationLists,
  MemberListType_docDictionaryMembers    = 77 + MemberListType_documentationLists,
};

enum class MemberListContainer
{
  File,
  Namespace,
  Group,
  Class,
  Module
};

enum class CodeSymbolType
{
  Default,
  // Class types
  Class,
  Struct,
  Union,
  Interface,
  Protocol,
  Category,
  Exception,
  Service,
  Singleton,
  // Other container types
  Concept,
  Namespace,
  Package,
  Module,
  // Member types
  Define,
  Function,
  Variable,
  Typedef,
  EnumValue,
  Enumeration,
  Signal,
  Slot,
  Friend,
  DCOP,
  Property,
  Event,
  Sequence,
  Dictionary
};

constexpr const char *codeSymbolType2Str(CodeSymbolType type)
{
  switch (type)
  {
    case CodeSymbolType::Class:       return "class";
    case CodeSymbolType::Struct:      return "struct";
    case CodeSymbolType::Union:       return "union";
    case CodeSymbolType::Interface:   return "interface";
    case CodeSymbolType::Protocol:    return "protocol";
    case CodeSymbolType::Category:    return "category";
    case CodeSymbolType::Exception:   return "exception";
    case CodeSymbolType::Service:     return "service";
    case CodeSymbolType::Singleton:   return "singleton";
    case CodeSymbolType::Concept:     return "concept";
    case CodeSymbolType::Namespace:   return "namespace";
    case CodeSymbolType::Package:     return "package";
    case CodeSymbolType::Define:      return "define";
    case CodeSymbolType::Function:    return "function";
    case CodeSymbolType::Variable:    return "variable";
    case CodeSymbolType::Typedef:     return "typedef";
    case CodeSymbolType::EnumValue:   return "enumvalue";
    case CodeSymbolType::Enumeration: return "enumeration";
    case CodeSymbolType::Signal:      return "signal";
    case CodeSymbolType::Slot:        return "slot";
    case CodeSymbolType::Friend:      return "friend";
    case CodeSymbolType::DCOP:        return "dcop";
    case CodeSymbolType::Property:    return "property";
    case CodeSymbolType::Event:       return "event";
    case CodeSymbolType::Sequence:    return "sequence";
    case CodeSymbolType::Dictionary:  return "dictionary";
    default:
      return 0;
  }
}


enum MemberType
{
  MemberType_Define,
  MemberType_Function,
  MemberType_Variable,
  MemberType_Typedef,
  MemberType_Enumeration,
  MemberType_EnumValue,
  MemberType_Signal,
  MemberType_Slot,
  MemberType_Friend,
  MemberType_DCOP,
  MemberType_Property,
  MemberType_Event,
  MemberType_Interface,
  MemberType_Service,
  MemberType_Sequence,
  MemberType_Dictionary
};

enum FortranFormat
{
  FortranFormat_Unknown,
  FortranFormat_Free,
  FortranFormat_Fixed
};


class LocalToc
{
  public:
    enum Type {
      None                   = 0, // initial value
      Html                   = 0, // index / also to be used as bit position in mask (1 << Html)
      Latex                  = 1, // ...
      Xml                    = 2, // ...
      Docbook                = 3, // ...
      numTocTypes            = 4  // number of enum values
    };
    LocalToc() : m_mask(None) { memset(m_level,0,sizeof(m_level)); }

    // setters
    void enableHtml(int level)
    {
      m_mask|=(1<<Html);
      m_level[Html]=level;
    }
    void enableLatex(int level)
    {
      m_mask|=(1<<Latex);
      m_level[Latex]=level;
    }
    void enableXml(int level)
    {
      m_mask|=(1<<Xml);
      m_level[Xml]=level;
    }
    void enableDocbook(int level)
    {
      m_mask|=(1<<Docbook);
      m_level[Docbook]=level;
    }

    // getters
    bool isHtmlEnabled()  const { return (m_mask & (1<<Html))!=0;  }
    bool isLatexEnabled() const { return (m_mask & (1<<Latex))!=0; }
    bool isXmlEnabled()   const { return (m_mask & (1<<Xml))!=0;   }
    bool isDocbookEnabled()   const { return (m_mask & (1<<Docbook))!=0;   }
    bool nothingEnabled() const { return m_mask == None; }
    int htmlLevel()       const { return m_level[Html]; }
    int latexLevel()      const { return m_level[Latex]; }
    int xmlLevel()        const { return m_level[Xml]; }
    int docbookLevel()    const { return m_level[Docbook]; }
    int mask()            const { return m_mask; }

  private:
    int m_mask;
    int m_level[numTocTypes];
};

//---------------------------------------------------------------------------------------


#define TYPE_SPECIFIERS                                                                                                         \
/*  0 */ TSPEC(Template)          TSPEC(Generic)         TSPEC(Ref)               TSPEC(Value)        TSPEC(Interface)          \
/*  5 */ TSPEC(Struct)            TSPEC(Union)           TSPEC(Exception)         TSPEC(Protocol)     TSPEC(Category)           \
/* 10 */ TSPEC(SealedClass)       TSPEC(AbstractClass)   TSPEC(Enum)              TSPEC(Service)      TSPEC(Singleton)          \
/* 15 */ TSPEC(ForwardDecl)       TSPEC(Local)           TSPEC(EnumStruct)        TSPEC(ConstExpr)    TSPEC(PrivateGettable)    \
/* 20 */ TSPEC(ProtectedGettable) TSPEC(PrivateSettable) TSPEC(ProtectedSettable) TSPEC(Inline)       TSPEC(Explicit)           \
/* 25 */ TSPEC(Mutable)           TSPEC(Settable)        TSPEC(Gettable)          TSPEC(Readable)     TSPEC(Writable)           \
/* 30 */ TSPEC(Final)             TSPEC(Abstract)        TSPEC(Addable)           TSPEC(Removable)    TSPEC(Raisable)           \
/* 35 */ TSPEC(Override)          TSPEC(New)             TSPEC(Sealed)            TSPEC(Initonly)     TSPEC(Optional)           \
/* 40 */ TSPEC(Required)          TSPEC(NonAtomic)       TSPEC(Copy)              TSPEC(Retain)       TSPEC(Assign)             \
/* 45 */ TSPEC(Strong)            TSPEC(Weak)            TSPEC(Unretained)        TSPEC(Alias)        TSPEC(ConstExp)           \
/* 50 */ TSPEC(Default)           TSPEC(Delete)          TSPEC(NoExcept)          TSPEC(Attribute)    TSPEC(Property)           \
/* 55 */ TSPEC(Readonly)          TSPEC(Bound)           TSPEC(Constrained)       TSPEC(Transient)    TSPEC(MaybeVoid)          \
/* 60 */ TSPEC(MaybeDefault)      TSPEC(MaybeAmbiguous)  TSPEC(Published)

/** Wrapper class for a number of boolean properties.
 *  The properties are packed together, and initialized to false.
 */
class TypeSpecifier
{
  public:
    TypeSpecifier() { reset(); }

    void reset() { std::memset(this, 0, sizeof(*this)); }

    void merge(const TypeSpecifier &other)
    {
#define TSPEC(x) m_is##x = m_is##x || other.is##x();
      TYPE_SPECIFIERS
#undef TSPEC
    }

    friend inline bool operator==(const TypeSpecifier &t1,const TypeSpecifier &t2)
    {
      bool eq = true;
#define TSPEC(x) eq = eq && (t1.m_is##x == t2.m_is##x);
      TYPE_SPECIFIERS
#undef TSPEC
      return eq;
    }

    friend inline bool operator!=(const TypeSpecifier &t1,const TypeSpecifier &t2)
    {
      return !(operator==(t1,t2));
    }


    std::string to_string() const
    {
      std::string result="[";
      bool first=true;
#define TSPEC(x)                                                          \
      if (m_is##x) {                                                      \
        if (!first) result+=",";                                          \
        result+=#x; first=false;                                          \
      }
      TYPE_SPECIFIERS
#undef TSPEC
      result+="]";
      return result;
    }

    // generate getter and setter for each property
#define TSPEC(x)                                                          \
    public:                                                               \
      TypeSpecifier &set##x(bool b) { m_is##x = b; return *this; }        \
      bool is##x() const { return m_is##x; }                              \
    private:                                                              \
      bool m_is##x : 1;
    TYPE_SPECIFIERS
#undef TSPEC

};

enum class VhdlSpecifier
{
  UNKNOWN=0,
  LIBRARY,
  ENTITY,
  PACKAGE_BODY,
  ARCHITECTURE,
  PACKAGE,
  ATTRIBUTE,
  SIGNAL,
  COMPONENT,
  CONSTANT,
  TYPE,
  SUBTYPE,
  FUNCTION,
  RECORD,
  PROCEDURE,
  USE,
  PROCESS,
  PORT,
  UNITS,
  GENERIC,
  INSTANTIATION,
  GROUP,
  VFILE,
  SHAREDVARIABLE,
  CONFIG,
  ALIAS,
  MISCELLANEOUS,
  UCF_CONST
};


//     Type                Categories (or'ed)
#define ENTRY_TYPES                         \
 ETYPE(Empty,              None)            \
 ETYPE(Class,              Compound|Scope)  \
 ETYPE(Namespace,          Scope)           \
 ETYPE(Concept,            None)            \
 ETYPE(ClassDoc,           CompoundDoc|Doc) \
 ETYPE(StructDoc,          CompoundDoc|Doc) \
 ETYPE(UnionDoc,           CompoundDoc|Doc) \
 ETYPE(ExceptionDoc,       CompoundDoc|Doc) \
 ETYPE(InterfaceDoc,       CompoundDoc|Doc) \
 ETYPE(ProtocolDoc,        CompoundDoc|Doc) \
 ETYPE(CategoryDoc,        CompoundDoc|Doc) \
 ETYPE(ServiceDoc,         CompoundDoc|Doc) \
 ETYPE(SingletonDoc,       CompoundDoc|Doc) \
 ETYPE(Source,             File)            \
 ETYPE(Header,             File)            \
 ETYPE(ModuleDoc,          Doc)             \
 ETYPE(ConceptDoc,         Doc)             \
 ETYPE(NamespaceDoc,       Doc)             \
 ETYPE(EnumDoc,            Doc)             \
 ETYPE(PageDoc,            Doc)             \
 ETYPE(MemberDoc,          Doc)             \
 ETYPE(OverloadDoc,        Doc)             \
 ETYPE(Example,            Doc)             \
 ETYPE(VariableDoc,        Doc)             \
 ETYPE(FileDoc,            Doc)             \
 ETYPE(DefineDoc,          Doc)             \
 ETYPE(GroupDoc,           Doc)             \
 ETYPE(MainpageDoc,        Doc)             \
 ETYPE(MemberGrp,          Doc)             \
 ETYPE(PackageDoc,         Doc)             \
 ETYPE(DirDoc,             Doc)             \
 ETYPE(Variable,           None)            \
 ETYPE(Function,           None)            \
 ETYPE(Typedef,            None)            \
 ETYPE(Include,            None)            \
 ETYPE(Enum,               None)            \
 ETYPE(Define,             None)            \
 ETYPE(UsingDir,           None)            \
 ETYPE(UsingDecl,          None)            \
 ETYPE(Package,            None)            \
 ETYPE(ObjcImpl,           None)            \
 ETYPE(ExportedInterface,  None)            \
 ETYPE(IncludedService,    None)            \
 ETYPE(ExampleLineno,      None)            \

/** Wrapper class for the Entry type. Can be set only during construction.
 *  Packs the type together with category flags.
 */
class EntryType
{
  public:
#define ETYPE(x,bits)                                                   \
    static EntryType make##x() { return EntryType(x|bits); }            \
    bool is##x() const { return (m_type&TypeMask)==x; }
    ENTRY_TYPES
#undef ETYPE
    bool isCompound()    const { return (m_type & Compound)!=0;    }
    bool isScope()       const { return (m_type & Scope)!=0;       }
    bool isFile()        const { return (m_type & File)!=0;        }
    bool isCompoundDoc() const { return (m_type & CompoundDoc)!=0; }
    bool isDoc()         const { return (m_type & Doc)!=0;         }
    std::string to_string() const
    {
      switch (type())
      {
#define ETYPE(x,bits)                                                   \
        case x : return "["+std::string(#x)+bits_to_string()+"]";
        ENTRY_TYPES
#undef ETYPE
      }
      return "[unknown]";
    }
    friend inline bool operator==(const EntryType &t1,const EntryType &t2) { return t1.m_type==t2.m_type; }
    friend inline bool operator!=(const EntryType &t1,const EntryType &t2) { return !(operator==(t1,t2)); }

  private:
    enum TypeName
    {
#define ETYPE(x,bits)                                                   \
      x,
      ENTRY_TYPES
#undef ETYPE
    };

    enum CategoryBits
    {
      None        = 0,
      Compound    = (1<<16),
      Scope       = (1<<17),
      File        = (1<<18),
      CompoundDoc = (1<<19),
      Doc         = (1<<20),
      TypeMask      = 0x0000FFFF,
      CategoryMask  = 0xFFFF0000
    };
    explicit EntryType(int t) : m_type(t) {}
    std::string bits_to_string() const
    {
      std::string result;
      if (m_type&Compound) result+=",Compound";
      if (m_type&Scope) result+=",Scope";
      if (m_type&File) result+=",File";
      if (m_type&CompoundDoc) result+=",CompoundDoc";
      return result;
    }
    TypeName type() const { return static_cast<TypeName>(m_type & TypeMask); }
    unsigned int m_type = Empty;
};


#endif
