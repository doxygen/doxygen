
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

#ifndef STLSUPPORT_H
#define STLSUPPORT_H

#include <memory>

class Entry;

/** Add stub entries for the most used classes in the standard template library
 *  @param root Root of the entry tree to add the entries to.
 */
void addSTLSupport(std::shared_ptr<Entry> &root);

#endif
