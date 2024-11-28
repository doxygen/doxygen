/***************************************************************************
 *
 * $Id: utf8.h 93 2009-08-24 20:57:31Z Michael.McTernan $
 *
 * UTF8 decode routine interface.
 * Copyright (C) 2009 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ***************************************************************************/

#ifndef MSCGEN_UTF8_H
#define MSCGEN_UTF8_H

/**************************************************************************
 * Includes
 **************************************************************************/

#include "mscgen_bool.h"

/**************************************************************************
 * Macros
 **************************************************************************/

/**************************************************************************
 * Types
 **************************************************************************/

/**************************************************************************
 * Prototypes
 **************************************************************************/

Boolean Utf8Decode(const char *s, unsigned int *r, unsigned int *bytes);

#endif

/* END OF FILE */
