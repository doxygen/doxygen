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
enum Protection   { Public, Protected, Private, Package } ;

/** Virtualness of a member. */
enum Specifier    { Normal, Virtual, Pure } ;

/** Kind of method */
enum MethodTypes  { Method, Signal, Slot, DCOP, Property, Event };

/** Type of member relation */
enum RelatesType  { Simple, Duplicate, MemberOf };

/** Kind of member relationship */
enum Relationship { Member, Related, Foreign };

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

/** This struct is used to store the class specifiers */
struct Spec
{
  Spec() : spec(0), specExt(0) {}
  Spec(uint64 n, uint64 e = 0) : spec(n), specExt(e) {}

  bool operator==(Spec rhs) const { return rhs.spec == spec && rhs.specExt == specExt;}

  bool operator!=(Spec rhs) const { return !(rhs==*this);}
  friend bool operator!=(const int lhs, const Spec& rhs) { return !(rhs==lhs);}

  bool operator&&(const Spec& rhs) { return (*this!=0) && (rhs!=0);}
  friend bool operator&&(const bool lhs, const Spec& rhs) { return lhs && (rhs!=0);}

  bool operator||(const Spec& rhs) { return (*this!=0) || (rhs!=0);}
  friend bool operator||(const bool lhs, const Spec& rhs) { return lhs || (rhs!=0);}

  bool operator!() { return !(spec!=0 || specExt!=0);}

  Spec operator~() const { return Spec(~spec,~specExt);}

  Spec operator&(Spec rhs) const { return Spec(rhs.spec & spec, rhs.specExt & specExt);}
  Spec operator&=(const Spec rhs) { spec &= rhs.spec; specExt &= rhs.specExt; return *this;}

  Spec operator|(Spec rhs) const { return Spec(rhs.spec | spec, rhs.specExt | specExt);}
  Spec operator|=(const Spec rhs) { spec |= rhs.spec; specExt |= rhs.specExt; return *this;}

  Spec operator^(Spec rhs) const { return Spec(rhs.spec ^ spec, rhs.specExt ^ specExt);}
  Spec operator^=(const Spec rhs) { spec ^= rhs.spec; specExt ^= rhs.specExt; return *this;}

  uint64 spec;
  uint64 specExt;
};
// class specifiers (add new items to the end)
static const Spec SpecifierTemplate        = (1ULL<<0);
static const Spec SpecifierGeneric         = (1ULL<<1);
static const Spec SpecifierRef             = (1ULL<<2);
static const Spec SpecifierValue           = (1ULL<<3);
static const Spec SpecifierInterface       = (1ULL<<4);
static const Spec SpecifierStruct          = (1ULL<<5);
static const Spec SpecifierUnion           = (1ULL<<6);
static const Spec SpecifierException       = (1ULL<<7);
static const Spec SpecifierProtocol        = (1ULL<<8);
static const Spec SpecifierCategory        = (1ULL<<9);
static const Spec SpecifierSealedClass     = (1ULL<<10);
static const Spec SpecifierAbstractClass   = (1ULL<<11);
static const Spec SpecifierEnum            = (1ULL<<12); // for Java-style enums
static const Spec SpecifierService         = (1ULL<<13); // UNO IDL
static const Spec SpecifierSingleton       = (1ULL<<14); // UNO IDL
static const Spec SpecifierForwardDecl     = (1ULL<<15); // forward declared template classes
static const Spec SpecifierLocal           = (1ULL<<16); // for Slice types

// member specifiers (add new items to the beginning)
static const Spec SpecifierEnumStruct      = (1ULL<<18);
static const Spec SpecifierConstExpr       = (1ULL<<19); // C++11 constexpr
static const Spec SpecifierPrivateGettable     = (1ULL<<20); // C# private getter
static const Spec SpecifierProtectedGettable   = (1ULL<<21); // C# protected getter
static const Spec SpecifierPrivateSettable     = (1ULL<<22); // C# private setter
static const Spec SpecifierProtectedSettable   = (1ULL<<23); // C# protected setter
static const Spec SpecifierInline          = (1ULL<<24);
static const Spec SpecifierExplicit        = (1ULL<<25);
static const Spec SpecifierMutable         = (1ULL<<26);
static const Spec SpecifierSettable        = (1ULL<<27);
static const Spec SpecifierGettable        = (1ULL<<28);
static const Spec SpecifierReadable        = (1ULL<<29);
static const Spec SpecifierWritable        = (1ULL<<30);
static const Spec SpecifierFinal           = (1ULL<<31);
static const Spec SpecifierAbstract        = (1ULL<<32);
static const Spec SpecifierAddable         = (1ULL<<33);
static const Spec SpecifierRemovable       = (1ULL<<34);
static const Spec SpecifierRaisable        = (1ULL<<35);
static const Spec SpecifierOverride        = (1ULL<<36);
static const Spec SpecifierNew             = (1ULL<<37);
static const Spec SpecifierSealed          = (1ULL<<38);
static const Spec SpecifierInitonly        = (1ULL<<39);
static const Spec SpecifierOptional        = (1ULL<<40);
static const Spec SpecifierRequired        = (1ULL<<41);
static const Spec SpecifierNonAtomic       = (1ULL<<42);
static const Spec SpecifierCopy            = (1ULL<<43);
static const Spec SpecifierRetain          = (1ULL<<44);
static const Spec SpecifierAssign          = (1ULL<<45);
static const Spec SpecifierStrong          = (1ULL<<46);
static const Spec SpecifierWeak            = (1ULL<<47);
static const Spec SpecifierUnretained      = (1ULL<<48);
static const Spec SpecifierAlias           = (1ULL<<49);
static const Spec SpecifierConstExp        = (1ULL<<50);
static const Spec SpecifierDefault         = (1ULL<<51);
static const Spec SpecifierDelete          = (1ULL<<52);
static const Spec SpecifierNoExcept        = (1ULL<<53);
static const Spec SpecifierAttribute       = (1ULL<<54); // UNO IDL attribute
static const Spec SpecifierProperty        = (1ULL<<55); // UNO IDL property
static const Spec SpecifierReadonly        = (1ULL<<56); // on UNO IDL attribute or property
static const Spec SpecifierBound           = (1ULL<<57); // on UNO IDL attribute or property
static const Spec SpecifierConstrained     = (1ULL<<58); // on UNO IDL property
static const Spec SpecifierTransient       = (1ULL<<59); // on UNO IDL property
static const Spec SpecifierMaybeVoid       = (1ULL<<60); // on UNO IDL property
static const Spec SpecifierMaybeDefault    = (1ULL<<61); // on UNO IDL property
static const Spec SpecifierMaybeAmbiguous  = (1ULL<<62); // on UNO IDL property
static const Spec SpecifierPublished       = (1ULL<<63); // UNO IDL keyword

// VHDL specifiers
static const Spec SpecifierLibrary         = Spec(0,1ULL<< 1);
static const Spec SpecifierEntity          = Spec(0,1ULL<< 2);
static const Spec SpecifierPackage_body    = Spec(0,1ULL<< 3);
static const Spec SpecifierArchitecture    = Spec(0,1ULL<< 4);
static const Spec SpecifierPackage         = Spec(0,1ULL<< 5);
static const Spec SpecifierAttributeVhdl       = Spec(0,1ULL<< 6);
static const Spec SpecifierSignal          = Spec(0,1ULL<< 7);
static const Spec SpecifierComponent       = Spec(0,1ULL<< 8);
static const Spec SpecifierConstant        = Spec(0,1ULL<< 9);
static const Spec SpecifierType            = Spec(0,1ULL<<10);
static const Spec SpecifierSubtype         = Spec(0,1ULL<<11);
static const Spec SpecifierFunction        = Spec(0,1ULL<<12);
static const Spec SpecifierRecord          = Spec(0,1ULL<<13);
static const Spec SpecifierProcedure       = Spec(0,1ULL<<14);
static const Spec SpecifierUse             = Spec(0,1ULL<<15);
static const Spec SpecifierProcess         = Spec(0,1ULL<<16);
static const Spec SpecifierPort            = Spec(0,1ULL<<17);
static const Spec SpecifierUnits           = Spec(0,1ULL<<18);
static const Spec SpecifierGenericVhdl         = Spec(0,1ULL<<19);
static const Spec SpecifierInstantiation   = Spec(0,1ULL<<20);
static const Spec SpecifierGroup           = Spec(0,1ULL<<21);
static const Spec SpecifierVfile           = Spec(0,1ULL<<22);
static const Spec SpecifierSharedvariable  = Spec(0,1ULL<<23);
static const Spec SpecifierConfig          = Spec(0,1ULL<<24);
static const Spec SpecifierAliasVhdl           = Spec(0,1ULL<<25);
static const Spec SpecifierMiscellaneous   = Spec(0,1ULL<<26);
static const Spec SpecifierUcf_const       = Spec(0,1ULL<<27);

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
  MemberListType_privateLists       = 0x0800,
  MemberListType_detailedLists      = 0x1000,
  MemberListType_declarationLists   = 0x2000,
  MemberListType_documentationLists = 0x4000,

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
  MemberListType_docDictionaryMembers    = 77 + MemberListType_documentationLists
};

enum class MemberListContainer
{
  File,
  Namespace,
  Group,
  Class
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

#endif
