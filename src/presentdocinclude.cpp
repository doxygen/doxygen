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

// own header
#include "presentdocinclude.h"

// other includes
#include "godbolt.h"
#include "outputlist.h"
#include "textstream.h"

void presentDocInclude(
  TextStream                    &t,
  OutputCodeList                &ci,
  const DocInclude              &inc,
  const DocIncludeParseFunction &parseCode)
{
  if (auto godbolt = inc.godboltOptions())
  {
    auto clientState = godbolt->clientState;
    if (godbolt->bridge == Godbolt::Bridge::iframe && Godbolt::CanEmbedd(clientState))
    {
      OutputCodeList dummy;
      parseCode(dummy, inc);
      t << "<iframe width=\"100%\" height=\"350px\" src=\"" + Godbolt::EmbeddUrlFor(clientState, inc.text()) + "\"></iframe>";
    }
    else
    {
      ci.startCodeFragment("Godbolt");
      parseCode(ci, inc);
      t << Godbolt::LinkHtml(clientState, inc.text());
      ci.endCodeFragment("Godbolt");
    }
  }
  else
  {
    ci.startCodeFragment("DoxyCode");
    parseCode(ci, inc);
    ci.endCodeFragment("DoxyCode");
  }
}
