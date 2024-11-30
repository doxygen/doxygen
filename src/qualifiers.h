/******************************************************************************
 *
 * Copyright (C) 1997-2024 by Dimitri van Heesch.
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

#ifndef QUALIFIERS_H
#define QUALIFIERS_H

#include <vector>
#include <string>

#include "containers.h"

struct QualifierInfo
{
  std::string label;
  std::vector<std::string> classes;
};
using QualifierInfoVector = std::vector<QualifierInfo *>;

void mergeQualifier(QualifierInfoVector &into, std::string qual, StringVector classes);
void mergeQualifier(QualifierInfoVector &into, QualifierInfoVector from);
void insertQualifier(QualifierInfoVector &into, std::string qual, std::string cls = ""); 
#endif
