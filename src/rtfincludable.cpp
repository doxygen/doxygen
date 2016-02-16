/******************************************************************************
*
*
*
* Copyright(C) 2016 by Andreas Grimme
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

#include "rtfincludable.h"
#include "OutputList.h"
#include "config.h"
#include "docparser.h"
#include <qdir.h>
#include <qregexp.h>

bool isIncludableRtf()
{
  return Config_getBool(INCLUDABLE_RTF);
}

bool isTableOfContentEntriesEnabled()
{
  return !isIncludableRtf();
}
