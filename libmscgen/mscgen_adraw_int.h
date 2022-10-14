/***************************************************************************
 *
 * $Id: adraw_int.h 115 2010-08-19 09:58:45Z Michael.McTernan $
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

#ifndef MSCGEN_ADRAW_INT_H
#define MSCGEN_ADRAW_INT_H

#include "mscgen_adraw.h"

/***************************************************************************
 * Preprocessor Macros
 ***************************************************************************/

/* Define macro to suppress unused parameter warnings */
#ifndef UNUSED
# ifdef __GNUC__
#  define UNUSED __attribute__((unused))
# else
#  define UNUSED
# endif
#endif

/***************************************************************************
 * Functions
 ***************************************************************************/

Boolean NullInit(struct ADrawTag *outContext);

Boolean GdoInit(unsigned int     w,
                unsigned int     h,
                const char      *file,
                const char      *fontName,
                struct ADrawTag *outContext);

Boolean PsInit(unsigned int     w,
               unsigned int     h,
               const char      *file,
               struct ADrawTag *outContext);

Boolean SvgInit(unsigned int     w,
                unsigned int     h,
                const char      *file,
                struct ADrawTag *outContext);

#endif /* MSCGEN_ADRAW_INT_H */

/* END OF FILE */
