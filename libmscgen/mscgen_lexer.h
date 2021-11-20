/***************************************************************************
 *
 * $Id: lexer.h 152 2010-10-10 14:17:37Z Michael.McTernan $
 *
 * Extra lexer/scanner API functions.
 * Copyright (C) 2010 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
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
 ***************************************************************************/

#ifndef MSCGEN_LEXER_H
#define MSCGEN_LEXER_H

/*****************************************************************************
 * Header Files
 *****************************************************************************/

#include "mscgen_bool.h"

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
Boolean        lex_getutf8(void);
#ifdef __cplusplus
}
#endif

unsigned long  lex_getlinenum(void);
char          *lex_getline(void);
void           lex_destroy(void);
void           lex_resetparser(void);

#endif /* MSCGEN_LEXER_H */

/* END OF FILE */
