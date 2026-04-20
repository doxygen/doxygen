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

#include "qcstring.h"

struct DotJob
{
  DotJob(const QCString &f, const QCString &ap, const QCString &rdn, const QCString &m,
      const QCString &s, int l)
    : format(f), absPath(ap), relDotName(rdn), md5Hash(m), srcFile(s), srcLine(l) {}
  QCString format;
  QCString absPath;
  QCString relDotName;
  QCString md5Hash;
  QCString srcFile;
  int srcLine;
};

#endif // DOTJOB_H
