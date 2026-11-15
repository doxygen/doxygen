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

#ifndef FSWRAPPER_H
#define FSWRAPPER_H

/** @file
 *  @brief Selects the filesystem implementation used by doxygen.
 *
 *  The standard library's std::filesystem is used when it is available and
 *  usable without extra link libraries. Otherwise the bundled
 *  ghc::filesystem (deps/filesystem) is used as a fallback, in particular for:
 *  - GCC 8 / libstdc++ 8 (e.g. RHEL 8), where std::filesystem needs -lstdc++fs
 *  - LLVM libc++ before version 9, where it needs -lc++fs
 *  - macOS deployment targets before 10.15, which lack std::filesystem
 *
 *  Defining DOXYGEN_USE_GHC_FILESYSTEM forces the use of ghc::filesystem.
 *
 *  Doxygen uses UTF-8 encoded std::string paths everywhere. ghc::filesystem
 *  treats narrow strings as UTF-8 on all platforms, but std::filesystem uses
 *  the active (ANSI) code page on Windows. Always convert between strings and
 *  paths using toFsPath() and fromFsPath(), so non-ASCII file names keep
 *  working on Windows.
 */

#include <cstddef> // defines _GLIBCXX_RELEASE / _LIBCPP_VERSION
#include <string>

#if !defined(DOXYGEN_USE_GHC_FILESYSTEM) && defined(__has_include)
#  if __has_include(<filesystem>)
#    if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE < 9
       // libstdc++ 8 (GCC 8): std::filesystem requires linking with -lstdc++fs
#    elif defined(_LIBCPP_VERSION) && _LIBCPP_VERSION < 9000
       // libc++ < 9: std::filesystem requires linking with -lc++fs
#    elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101500
       // std::filesystem is only available from macOS 10.15 onwards
#    else
#      define DOXYGEN_USE_STD_FILESYSTEM 1
#    endif
#  endif
#endif

#if DOXYGEN_USE_STD_FILESYSTEM
#  include <filesystem>
namespace fs = std::filesystem;
#else
#  include "filesystem.hpp"
namespace fs = ghc::filesystem;
#endif

/** Converts a UTF-8 encoded string into a path. */
inline fs::path toFsPath(const std::string &s)
{
#if DOXYGEN_USE_STD_FILESYSTEM && defined(_WIN32)
#  if defined(__cpp_char8_t)
  return fs::path(std::u8string(s.begin(), s.end()));
#  else
  return fs::u8path(s);
#  endif
#else
  return fs::path(s);
#endif
}

/** Converts a path into a UTF-8 encoded string. */
inline std::string fromFsPath(const fs::path &p)
{
#if DOXYGEN_USE_STD_FILESYSTEM && defined(_WIN32)
#  if defined(__cpp_char8_t)
  const std::u8string s = p.u8string();
  return std::string(s.begin(), s.end());
#  else
  return p.u8string();
#  endif
#else
  return p.string();
#endif
}

#endif // FSWRAPPER_H
