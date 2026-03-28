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

#ifndef ANCHOR_H
#define ANCHOR_H

#include <memory>
#include <string>

#include "construct.h"

/** Singleton class used to generate anchors for Markdown headers */
class AnchorGenerator
{
  public:
    /** Returns the singleton instance */
    static AnchorGenerator &instance();

    /** generates an anchor for a section with \a title.
     *  Returns the anchor.
     */
    std::string generate(const std::string &title);

    /** Returns true iff \a anchor is one of the generated anchors */
    bool isGenerated(const std::string &anchor) const;

    /** Reserves a non-generated anchor */
    int reserve(const std::string &anchor);

    /** Returns true if \a anchor is a potentially generated anchor.
     *  Note this is a much weaker check than isGenerated() and may not always work.
     */
    static bool looksGenerated(const std::string &anchor);

    static std::string addPrefixIfNeeded(const std::string &anchor);

  private:
    AnchorGenerator();
   ~AnchorGenerator();
    NON_COPYABLE(AnchorGenerator)
    struct Private;
    std::unique_ptr<Private> p;
};

#endif
