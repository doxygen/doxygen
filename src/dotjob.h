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
#include "qcstring.h"

struct DotJob
{
  DotJob(const QCString &ap, const QCString &rdn, const QCString &f, const QCString &m,
      const QCString &s, size_t sz, bool genMap = false)
    : absPath(ap), relDotName(rdn), format(f), md5Hash(m), srcFile(s), size(sz), generateImageMap(genMap) {}
  QCString absPath;
  QCString relDotName;
  QCString format;
  QCString md5Hash;
  QCString srcFile;
  size_t size;
  bool generateImageMap;
};

using DotJobs = std::vector<DotJob>;

#endif // DOTJOB_H
