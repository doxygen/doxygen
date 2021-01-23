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
 */

#ifndef _HTMLATTRIB_H
#define _HTMLATTRIB_H

#include <vector>
#include <qcstring.h>

/*! \brief Class representing a HTML attribute. */
struct HtmlAttrib
{
  QCString name;
  QCString value;
};

/*! \brief Class representing a list of HTML attributes. */
class HtmlAttribList : public std::vector<HtmlAttrib>
{
};

#endif

