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

#include "types.h"
#include "util.h"

EntryType EntryType::guessSection(const DString &name)
{
  DString n=name.lower();
  static const std::unordered_set<std::string> sourceExt = {
     "c","cc","cxx","cpp","c++","cppm","ccm","cxxm","c++m",   // C/C++
     "java",                       // Java
     "cs",                         // C#
     "m","mm",                     // Objective-C
     "ii","ixx","ipp","i++","inl", // C/C++ inline
     "xml","lex","sql"             // others
  };
  static const std::unordered_set<std::string> headerExt = {
     "h", "hh", "hxx", "hpp", "h++", "ixx", // C/C++ header
     "idl", "ddl", "pidl", "ice"    // IDL like
  };
  size_t lastDot = n.rfind('.');
  if (lastDot!=DString::npos)
  {
    DString extension = n.mid(lastDot+1); // part after the last dot
    if (sourceExt.find(extension.str())!=sourceExt.end())
    {
      return EntryType::makeSource();
    }
    if (headerExt.find(extension.str())!=headerExt.end())
    {
      return EntryType::makeHeader();
    }
  }
  else
  {
    if (getLanguageFromFileName(name,SrcLangExt::Unknown) == SrcLangExt::Cpp) return EntryType::makeHeader();
  }
  return EntryType::makeEmpty();
}

