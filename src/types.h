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
enum class SrcLangExt
{
  Unknown  = 0x00000,
  IDL      = 0x00008,
  Java     = 0x00010,
  CSharp   = 0x00020,
  D        = 0x00040,
  PHP      = 0x00080,
  ObjC     = 0x00100,
  Cpp      = 0x00200,
  JS       = 0x00400,
  Python   = 0x00800,
  Fortran  = 0x01000,
  VHDL     = 0x02000,
  XML      = 0x04000,
  //Tcl      = 0x08000, // no longer supported
  Markdown = 0x10000,
  SQL      = 0x20000,
  Slice    = 0x40000,
  Lex      = 0x80000
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

//       enum name             category      to-public          to-protected     html-label                xml-label
#define ML_TYPES \
  ML_TYPE(PubMethods,          Public,       Invalid,           Invalid,          "pub-methods",           "public-func"             ) \
  ML_TYPE(ProMethods,          Protected,    PubMethods,        Invalid,          "pro-methods",           "protected-func"          ) \
  ML_TYPE(PacMethods,          Package,      Invalid,           Invalid,          "pac-methods",           "package-func"            ) \
  ML_TYPE(PriMethods,          Private,      PubMethods,        ProMethods,       "pri-methods",           "private-func"            ) \
  ML_TYPE(PubStaticMethods,    Public,       Invalid,           Invalid,          "pub-static-methods",    "public-static-func"      ) \
  ML_TYPE(ProStaticMethods,    Protected,    PubStaticMethods,  Invalid,          "pro-static-methods",    "protected-static-func"   ) \
  ML_TYPE(PacStaticMethods,    Package,      Invalid,           Invalid,          "pac-static-methods",    "package-static-func"     ) \
  ML_TYPE(PriStaticMethods,    Private,      PubStaticMethods,  ProStaticMethods, "pri-static-methods",    "private-static-func"     ) \
  ML_TYPE(PubSlots,            Public,       Invalid,           Invalid,          "pub-slots",             "public-slot"             ) \
  ML_TYPE(ProSlots,            Protected,    PubSlots,          Invalid,          "pro-slots",             "protected-slot"          ) \
  ML_TYPE(PriSlots,            Private,      PubSlots,          ProSlots,         "pri-slots",             "private-slot"            ) \
  ML_TYPE(PubAttribs,          Public,       Invalid,           Invalid,          "pub-attribs",           "public-attrib"           ) \
  ML_TYPE(ProAttribs,          Protected,    PubAttribs,        Invalid,          "pro-attribs",           "protected-attrib"        ) \
  ML_TYPE(PacAttribs,          Package,      Invalid,           Invalid,          "pac-attribs",           "package-attrib"          ) \
  ML_TYPE(PriAttribs,          Private,      PubAttribs,        ProAttribs,       "pri-attribs",           "private-attrib"          ) \
  ML_TYPE(PubStaticAttribs,    Public,       Invalid,           Invalid,          "pub-static-attribs",    "public-static-attrib"    ) \
  ML_TYPE(ProStaticAttribs,    Protected,    PubStaticAttribs,  Invalid,          "pro-static-attribs",    "protected-static-attrib" ) \
  ML_TYPE(PacStaticAttribs,    Package,      Invalid,           Invalid,          "pac-static-attribs",    "package-static-attrib"   ) \
  ML_TYPE(PriStaticAttribs,    Private,      PubStaticAttribs,  ProStaticAttribs, "pri-static-attribs",    "private-static-attrib"   ) \
  ML_TYPE(PubTypes,            Public,       Invalid,           Invalid,          "pub-types",             "public-type"             ) \
  ML_TYPE(ProTypes,            Protected,    PubTypes,          Invalid,          "pro-types",             "protected-type"          ) \
  ML_TYPE(PacTypes,            Package,      Invalid,           Invalid,          "pac-types",             "package-type"            ) \
  ML_TYPE(PriTypes,            Private,      PubTypes,          ProTypes,         "pri-types",             "private-type"            ) \
  ML_TYPE(Related,             OnlyPublic,   Invalid,           Invalid,          "related",               "related"                 ) \
  ML_TYPE(Signals,             OnlyPublic,   Invalid,           Invalid,          "signals",               "signal"                  ) \
  ML_TYPE(Friends,             OnlyPublic,   Invalid,           Invalid,          "friends",               "friend"                  ) \
  ML_TYPE(DcopMethods,         OnlyPublic,   Invalid,           Invalid,          "dcop-methods",          "dcop-func"               ) \
  ML_TYPE(Properties,          OnlyPublic,   Invalid,           Invalid,          "properties",            "property"                ) \
  ML_TYPE(Events,              OnlyPublic,   Invalid,           Invalid,          "events",                "event"                   ) \
  ML_TYPE(AllMembersList,      OnlyPublic,   Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(EnumFields,          OnlyPublic,   Invalid,           Invalid,          "enum-fields",           ""                        ) \
  ML_TYPE(MemberGroup,         OnlyPublic,   Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(Interfaces,          OnlyPublic,   Invalid,           Invalid,          "interfaces",            "interfaces"              ) \
  ML_TYPE(Services,            OnlyPublic,   Invalid,           Invalid,          "services",              "services"                ) \
  ML_TYPE(DecDefineMembers,    Declaration,  Invalid,           Invalid,          "define-members",        "define"                  ) \
  ML_TYPE(DecProtoMembers,     Declaration,  Invalid,           Invalid,          "proto-members",         "prototype"               ) \
  ML_TYPE(DecTypedefMembers,   Declaration,  Invalid,           Invalid,          "typedef-members",       "typedef"                 ) \
  ML_TYPE(DecEnumMembers,      Declaration,  Invalid,           Invalid,          "enum-members",          "enum"                    ) \
  ML_TYPE(DecFuncMembers,      Declaration,  Invalid,           Invalid,          "func-members",          "func"                    ) \
  ML_TYPE(DecVarMembers,       Declaration,  Invalid,           Invalid,          "var-members",           "var"                     ) \
  ML_TYPE(DecEnumValMembers,   Declaration,  Invalid,           Invalid,          "enumval-members",       ""                        ) \
  ML_TYPE(DecPubSlotMembers,   Declaration,  Invalid,           Invalid,          "pub-slot-members",      ""                        ) \
  ML_TYPE(DecProSlotMembers,   Declaration,  Invalid,           Invalid,          "pro-slot-members",      ""                        ) \
  ML_TYPE(DecPriSlotMembers,   Declaration,  Invalid,           Invalid,          "pri-slot-members",      ""                        ) \
  ML_TYPE(DecSignalMembers,    Declaration,  Invalid,           Invalid,          "signal-members",        ""                        ) \
  ML_TYPE(DecEventMembers,     Declaration,  Invalid,           Invalid,          "event-members",         ""                        ) \
  ML_TYPE(DecFriendMembers,    Declaration,  Invalid,           Invalid,          "friend-members",        ""                        ) \
  ML_TYPE(DecPropMembers,      Declaration,  Invalid,           Invalid,          "prop-members",          ""                        ) \
  ML_TYPE(DecSequenceMembers,  Declaration,  Invalid,           Invalid,          "sequence-members",      "sequence"                ) \
  ML_TYPE(DecDictionaryMembers,Declaration,  Invalid,           Invalid,          "dictionary-members",    "dictionary"              ) \
  ML_TYPE(TypedefMembers,      Detailed,     Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(EnumMembers,         Detailed,     Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(EnumValMembers,      Detailed,     Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(FunctionMembers,     Detailed,     Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(RelatedMembers,      Detailed,     Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(VariableMembers,     Detailed,     Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(PropertyMembers,     Detailed,     Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(EventMembers,        Detailed,     Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(Constructors,        Detailed,     Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(InterfaceMembers,    Detailed,     Invalid,           Invalid,          "interface-members",     ""                        ) \
  ML_TYPE(ServiceMembers,      Detailed,     Invalid,           Invalid,          "service-members",       ""                        ) \
  ML_TYPE(DocDefineMembers,    Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocProtoMembers,     Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocTypedefMembers,   Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocEnumMembers,      Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocFuncMembers,      Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocVarMembers,       Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocEnumValMembers,   Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocPubSlotMembers,   Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocProSlotMembers,   Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocPriSlotMembers,   Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocSignalMembers,    Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocEventMembers,     Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocFriendMembers,    Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocPropMembers,      Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocSequenceMembers,  Documentation,Invalid,           Invalid,          "",                      ""                        ) \
  ML_TYPE(DocDictionaryMembers,Documentation,Invalid,           Invalid,          "",                      ""                        ) \

/** Wrapper class for the MemberListType type. Can be set only during construction.
 *  Packs the type together with category flags.
 */
class MemberListType
{
    enum CategoryBits
    {
      Public        = (1<<16),
      Protected     = (1<<17),
      Package       = (1<<18),
      Private       = (1<<19),
      OnlyPublic    = (1<<20),
      Detailed      = (1<<21),
      Declaration   = (1<<22),
      Documentation = (1<<23),
      TypeMask      = 0x0000FFFF,
      CategoryMask  = 0xFFFF0000
    };

    enum TypeName
    {
      Invalid_ = -1,
#define ML_TYPE(x,bits,to_pub,to_prot,label,xml_str) \
      x##_,
      ML_TYPES
#undef ML_TYPE
    };

  public:
    static MemberListType Invalid() { return MemberListType(Invalid_); }
    constexpr bool isInvalid() const { return m_type==Invalid_; }
#define ML_TYPE(x,bits,to_pub,to_prot,label,xml_str)                                                                   \
    static MemberListType x() { return MemberListType(static_cast<int>(x##_)|static_cast<int>(bits)); } \
    bool is##x() const { return (m_type&TypeMask)==x##_; }
    ML_TYPES
#undef ML_TYPE
    constexpr bool isPublic()         const { return (m_type & Public)!=0;        }
    constexpr bool isOnlyPublic()     const { return (m_type & OnlyPublic)!=0;    }
    constexpr bool isProtected()      const { return (m_type & Protected)!=0;     }
    constexpr bool isPackage()        const { return (m_type & Package)!=0;       }
    constexpr bool isPrivate()        const { return (m_type & Private)!=0;       }
    constexpr bool isDetailed()       const { return (m_type & Detailed)!=0;      }
    constexpr bool isDeclaration()    const { return (m_type & Declaration)!=0;   }
    constexpr bool isDocumentation()  const { return (m_type & Documentation)!=0; }
    std::string to_string() const
    {
      switch (type())
      {
        case Invalid_: return "[Invalid]";
#define ML_TYPE(x,bits,to_pub,to_prot,label,xml_str) \
        case x##_ : return "["+std::string(#x)+bits_to_string()+"]";
        ML_TYPES
#undef ML_TYPE
      }
      return "[unknown]";
    }
    int to_int() const
    {
      return m_type!=Invalid_ ? m_type&TypeMask : -1;
    }
    constexpr const char *toLabel() const
    {
      switch (type())
      {
        case Invalid_: return "";
#define ML_TYPE(x,bits,to_pub,to_prot,label,xml_str) \
        case x##_ : return label;
        ML_TYPES
#undef ML_TYPE
      }
      return "";
    }
    constexpr const char *toXML() const
    {
      switch (type())
      {
        case Invalid_: return "";
#define ML_TYPE(x,bits,to_pub,to_prot,label,xml_str) \
        case x##_ : return xml_str;
        ML_TYPES
#undef ML_TYPE
      }
      return "";
    }
    MemberListType toPublic() const
    {
      switch (type())
      {
        case Invalid_: return Invalid();
#define ML_TYPE(x,bits,to_pub,to_prot,label,xml_str) \
        case x##_ : return to_pub();
        ML_TYPES
#undef ML_TYPE
      }
      return Invalid();
    }
    MemberListType toProtected() const
    {
      switch (type())
      {
        case Invalid_: return Invalid();
#define ML_TYPE(x,bits,to_pub,to_prot,label,xml_str) \
        case x##_ : return to_prot();
        ML_TYPES
#undef ML_TYPE
      }
      return Invalid();
    }
    friend inline bool operator==(const MemberListType &t1,const MemberListType &t2) { return t1.m_type==t2.m_type; }
    friend inline bool operator!=(const MemberListType &t1,const MemberListType &t2) { return !(operator==(t1,t2)); }

  private:
    explicit MemberListType(int t) : m_type(t) {}
    std::string bits_to_string() const
    {
      std::string result;
      if (m_type&Public) result+=",Public";
      if (m_type&Protected) result+=",Protected";
      if (m_type&Package) result+=",Package";
      if (m_type&Private) result+=",Private";
      if (m_type&OnlyPublic) result+=",OnlyPublic";
      if (m_type&Detailed) result+=",Detailed";
      if (m_type&Documentation) result+=",Documentation";
      return result;
    }
    constexpr TypeName type() const { return static_cast<TypeName>(m_type & TypeMask); }
    int m_type = static_cast<int>(Invalid_);
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
      return nullptr;
  }
}


enum class MemberType
{
  Define,
  Function,
  Variable,
  Typedef,
  Enumeration,
  EnumValue,
  Signal,
  Slot,
  Friend,
  DCOP,
  Property,
  Event,
  Interface,
  Service,
  Sequence,
  Dictionary
};

enum class FortranFormat
{
  Unknown,
  Free,
  Fixed
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
/* 60 */ TSPEC(MaybeDefault)      TSPEC(MaybeAmbiguous)  TSPEC(Published)         TSPEC(ConstEval)    TSPEC(ConstInit)          \
/* 65 */ TSPEC(NoDiscard)

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
#define ETYPE(x,bits)                                                                            \
    static EntryType make##x() { return EntryType(static_cast<int>(x)|static_cast<int>(bits)); } \
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
