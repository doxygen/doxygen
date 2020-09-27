/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 */


#include <unordered_map>

#include "parserintf.h"
#include "docvisitor.h"
#include "util.h"
#include "types.h"
#include "doxygen.h"

struct DocVisitor::Private
{
  int id;
  std::unordered_map< std::string, std::unique_ptr<CodeParserInterface> > parserFactoryMap;
};

DocVisitor::DocVisitor(int id) : p(std::make_unique<Private>())
{
  p->id = id;
}

DocVisitor::~DocVisitor()
{
}

CodeParserInterface &DocVisitor::getCodeParser(const char *extension)
{
  std::string ext(extension?extension:"");
  // for each extension we create a code parser once per visitor, so that
  // the context of the same parser object is reused thoughout multiple passes for instance
  // for code fragments shown via dontinclude.
  auto it = p->parserFactoryMap.find(ext);
  if (it==p->parserFactoryMap.end())
  {
    auto factory = Doxygen::parserManager->getCodeParserFactory(extension);
    auto result = p->parserFactoryMap.insert(std::make_pair(ext,factory()));
    it = result.first;
  }
  return *it->second.get();
}

int DocVisitor::id() const
{
  return p->id;
}
