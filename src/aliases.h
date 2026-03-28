/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#ifndef ALIASES_H
#define ALIASES_H

#include <string>
#include <string_view>

std::string resolveAliasCmd(std::string_view aliasCmd);
bool isAliasCmd(std::string_view aliasCmd);
void readAliases();

#endif
