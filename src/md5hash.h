/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef MD5HASH_H
#define MD5HASH_H

#include <array>
#include <string_view>
#include <cstdint>
#include <md5.h>

#include "dstring.h"

inline std::array<uint8_t,16> md5hash(const std::string_view &str)
{
  std::array<uint8_t,16> result;
  MD5Buffer(str.data(),str.length(),result.data());
  return result;
}

inline DString md5str(const std::string_view &str)
{
  char sigStr[33];
  MD5SigToString(md5hash(str).data(),sigStr);
  return sigStr;
}

#endif // MD5HASH_H

