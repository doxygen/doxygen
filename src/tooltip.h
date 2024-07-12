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
 *
 */
#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <memory>
#include "construct.h"

class Definition;
class OutputCodeList;

//! Class that manages the tooltips for a source file
class TooltipManager
{
  public:
    TooltipManager();
   ~TooltipManager();
    NON_COPYABLE(TooltipManager)

    /** add a tooltip for a given symbol definition */
    void addTooltip(const Definition *d);

    /** write the list of all collected tooltip to the given outputs */
    void writeTooltips(OutputCodeList &ol);

  private:
    class Private;
    std::unique_ptr<Private> p;
};

#endif
