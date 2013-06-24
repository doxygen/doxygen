/******************************************************************************
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

class Definition;
class CodeOutputInterface;

class TooltipManager
{
  public:
    static TooltipManager *instance();
    void clearTooltips();
    void addTooltip(Definition *d);
    void writeTooltips(CodeOutputInterface &ol);

  private:
    class Private;
    Private *p;
    TooltipManager();
   ~TooltipManager();
    static TooltipManager *s_theInstance;
};

#endif
