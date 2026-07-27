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

#include <mutex>
#include <algorithm>
#include <string>
#include <unordered_map>

#include "anchor.h"
#include "containers.h"
#include "utf8.h"
#include "config.h"

struct AnchorGenerator::Private
{
  StringUnorderedSet anchorsUsed;
  int anchorCount=0;
  std::mutex mutex;
  std::unordered_map<std::string,int> idCount;
};

AnchorGenerator::AnchorGenerator() : p(std::make_unique<Private>()) {}

AnchorGenerator::~AnchorGenerator() = default;

AnchorGenerator &AnchorGenerator::instance()
{
  static AnchorGenerator am;
  return am;
}

constexpr auto prefix = "autotoc_md";

std::string AnchorGenerator::addPrefixIfNeeded(const std::string &anchor)
{
  return (Config_getEnum(MARKDOWN_ID_STYLE) == MARKDOWN_ID_STYLE_t::GITHUB &&
         (anchor.empty() || anchor.front() == '-' || std::isdigit(anchor.front())))
    ? prefix + anchor : anchor;
}

std::string AnchorGenerator::generate(const std::string &label)
{
  std::lock_guard lock(p->mutex);

  std::string result;

  auto createDoxygenStyleAnchor = [&]()
  {
    // overwrite result with the doxygen style anchor
    result = prefix+std::to_string(p->anchorCount++);
  };

  auto createGitHubStyleAnchor = [&]()
  {
    result.clear();
    size_t pos=0;
    while (pos<label.length())
    {
      uint8_t bytes       = getUTF8CharNumBytes(label[pos]);
      std::string charStr = getUTF8CharAt(label,pos);
      uint32_t cUnicode   = getUnicodeForUTF8CharAt(label,pos);
      char c = charStr[0];
      if (qisspace(c) || c=='-')
      {
        result+='-';
      }
      else if (c!='_' && isUTF8PunctuationCharacter(cUnicode))
      {
        // skip punctuation characters
      }
      else // normal UTF8 character
      {
        result+=convertUTF8ToLower(charStr);
      }
      pos+=bytes;
    }
    //printf("label='%s' result='%s'\n",qPrint(label),qPrint(result));
    if (result.empty()) // fallback use doxygen style anchor
    {
      createDoxygenStyleAnchor();
    }
    else
    {
      result = addPrefixIfNeeded(result);
      int &count = p->idCount[result];
      // Add end digits if an identical header already exists
      if (count>0)
      {
        result+="-"+std::to_string(count);
      }
      count++;
    }
  };

  switch (Config_getEnum(MARKDOWN_ID_STYLE))
  {
    case MARKDOWN_ID_STYLE_t::DOXYGEN:
      createDoxygenStyleAnchor();
      break;
    case MARKDOWN_ID_STYLE_t::GITHUB:
      createGitHubStyleAnchor();
      break;
  }

  p->anchorsUsed.insert(result);

  return result;
}

bool AnchorGenerator::isGenerated(const std::string &anchor) const
{
  std::lock_guard lock(p->mutex);
  return p->anchorsUsed.find(anchor)!=p->anchorsUsed.end();
}

int AnchorGenerator::reserve(const std::string &anchor)
{
  std::lock_guard lock(p->mutex);
  return p->idCount[anchor]++;
}

bool AnchorGenerator::looksGenerated(const std::string &anchor)
{
  return Config_getEnum(MARKDOWN_ID_STYLE)==MARKDOWN_ID_STYLE_t::DOXYGEN &&
         QCString(anchor).startsWith(prefix);
}
