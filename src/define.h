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

#ifndef DEFINE_H
#define DEFINE_H

#include <map>
#include <string>
#include <memory>

#include <qcstring.h>

class FileDef;

/** A class representing a macro definition. */
class Define
{
  public:
    Define();
    Define(const Define &d);
   ~Define();
    bool hasDocumentation();
    QCString name;
    QCString definition;
    QCString fileName;
    QCString doc;
    QCString brief;
    QCString args;
    QCString anchor;
    FileDef *fileDef;
    int lineNr;
    int columnNr;
    int nargs;
    bool undef;
    bool varArgs;
    bool isPredefined;
    bool nonRecursive;
};

/** A dictionary of references to Define objects. */
typedef std::map< std::string,Define* > DefineMapRef;

/** A dictionary of managed Define objects. */
typedef std::map< std::string,std::unique_ptr<Define> > DefineMapOwning;

#endif
