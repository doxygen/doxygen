/******************************************************************************
*
* Copyright (C) 1997-2019 by Dimitri van Heesch.
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

#ifndef DOTLEGENDGRAPH_H
#define DOTLEGENDGRAPH_H

#include "dotgraph.h"
#include "dstring.h"

/** Representation of a legend explaining the meaning of boxes, arrows, and colors */
class DotLegendGraph final : public DotGraph
{
  public:
    void writeGraph(const DString &path);

  private:
    DString getBaseName() const override;
    void computeTheGraph() override;
    DString getMapLabel() const override;

};

#endif

