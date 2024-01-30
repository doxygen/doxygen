/***************************************************************************
 *
 * $Id: safe.h 152 2010-10-10 14:17:37Z Michael.McTernan $
 *
 * This file is part of mscgen, a message sequence chart renderer.
 * Copyright (C) 2005 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 **************************************************************************/

#ifndef MSCGEN_SAFE_H
#define MSCGEN_SAFE_H

/*****************************************************************************
 * Header Files
 *****************************************************************************/

/*****************************************************************************
 * Preprocessor Macros & Constants
 *****************************************************************************/

/*****************************************************************************
 * Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Global Variable Declarations
 *****************************************************************************/

/*****************************************************************************
 * Global Function Declarations
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
void *realloc_s(void *ptr, size_t size);
void *malloc_s(size_t size);
void *zalloc_s(size_t size);
char *strdup_s(const char *s);
const char *mscgen_getenv_s(const char *name);
#ifdef __cplusplus
}
#endif


#endif /* MSCGEN_SAFE_H */
