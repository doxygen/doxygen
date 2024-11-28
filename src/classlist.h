/******************************************************************************
 *
 *
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

#ifndef CLASSLIST_H
#define CLASSLIST_H

#include "linkedmap.h"
#include "classdef.h"

class Definition;

class ClassLinkedMap : public LinkedMap<ClassDef>
{
};

class ClassLinkedRefMap : public LinkedRefMap<ClassDef>
{
  public:
    bool declVisible(const ClassDef::CompoundType *filter=nullptr) const;
    void writeDeclaration(OutputList &ol,const ClassDef::CompoundType *filter,const QCString &header,bool localNames) const;
    void writeDocumentation(OutputList &ol,const Definition * container=nullptr) const;
};

#endif
