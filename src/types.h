/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

#include "qtbc.h"

/** @file
 *  @brief This file contains a number of basic enums and types. 
 */

enum Protection   { Public, Protected, Private, Package } ;
enum Specifier    { Normal, Virtual, Pure } ;
enum MethodTypes  { Method, Signal, Slot, DCOP, Property, Event };
enum RelatesType  { Simple, Duplicate, MemberOf };
enum Relationship { Member, Related, Foreign };
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

struct Grouping 
{
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


#endif
