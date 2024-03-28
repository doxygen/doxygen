/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "qcstring.h"
#include "containers.h"

class FileDef;

/** A class representing a macro definition. */
class Define
{
  public:
    QCString name;
    QCString definition;
    QCString fileName;
    QCString args;
    FileDef *fileDef = nullptr;
    int lineNr = 1;
    int columnNr = 1;
    int nargs = -1;
    bool undef = FALSE;
    bool varArgs = FALSE;
    bool isPredefined = FALSE;
    bool nonRecursive = FALSE;
    bool expandAsDefined = FALSE;
};

/** List of all macro definitions */
using DefineList = std::vector<Define>;
using DefinesPerFileList = std::unordered_map< std::string, DefineList >;

#endif
