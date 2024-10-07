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

#ifndef PRE_H
#define PRE_H

#include <memory>
#include <string>
#include "construct.h"

class QCString;

class Preprocessor
{
  public:
    Preprocessor();
   ~Preprocessor();
    NON_COPYABLE(Preprocessor)

    void processFile(const QCString &fileName,const std::string &input,std::string &output);
    void addSearchDir(const QCString &dir);
 private:
   struct Private;
   std::unique_ptr<Private> p;
};

#endif
