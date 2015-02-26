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

#include <qcstring.h>

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
  SrcLangExt_Tcl      = 0x08000,
  SrcLangExt_Markdown = 0x10000
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

  Grouping( const char *gn, GroupPri_t p ) : groupname(gn), pri(p) {}
  Grouping( const Grouping &g ) : groupname(g.groupname), pri(g.pri) {}
  QCString groupname;   //!< name of the group
  GroupPri_t pri;       //!< priority of this definition

};

struct ListItemInfo
{
  QCString type;
  int itemId;
};

enum MemberListType
{
  MemberListType_privateLists       = 0x0800,
  MemberListType_detailedLists      = 0x1000,
  MemberListType_declarationLists   = 0x2000,
  MemberListType_documentationLists = 0x4000,

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
};

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
};

enum FortranFormat
{
  FortranFormat_Unknown,
  FortranFormat_Free,
  FortranFormat_Fixed
};

#endif
