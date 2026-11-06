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

#ifndef DOTJOB_H
#define DOTJOB_H

#include <vector>
#include "dstring.h"

struct DotJob
{
  DotJob(const DString &ap, const DString &rdn, const DString &f, const DString &m,
      const DString &s, size_t sz, bool genMap = false)
    : absPath(ap), relDotName(rdn), format(f), md5Hash(m), srcFile(s), size(sz), generateImageMap(genMap) {}
  DString absPath;
  DString relDotName;
  DString format;
  DString md5Hash;
  DString srcFile;
  size_t size;
  bool generateImageMap;
};

using DotJobs = std::vector<DotJob>;

#endif // DOTJOB_H
