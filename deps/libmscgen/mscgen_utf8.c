/***************************************************************************
 *
 * $Id: utf8.c 93 2009-08-24 20:57:31Z Michael.McTernan $
 *
 * UTF8 decode routine.
 * Copyright (C) 2008 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
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
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 **************************************************************************/

/**************************************************************************
 * Includes
 **************************************************************************/

#include "mscgen_utf8.h"

/**************************************************************************
 * Manifest Constants
 **************************************************************************/

/**************************************************************************
 * Macros
 **************************************************************************/

/**************************************************************************
 * Types
 **************************************************************************/

/**************************************************************************
 * Local Variables
 **************************************************************************/

/**************************************************************************
 * Local Functions
 **************************************************************************/

/** Count leading ones from the MSB.
 */
static unsigned int clo(char c)
{
    unsigned int t = 0;

    while((c & (0x80 >> t)) != 0 && t < 8)
    {
        t++;
    }

    return t;
}

/**************************************************************************
 * Global Functions
 **************************************************************************/

Boolean Utf8Decode(const char *s, unsigned int *r, unsigned int *bytes)
{
    if((*s & 0x80) == 0)
    {
        return FALSE;
    }
    else
    {
        unsigned int t;

        /* Set default return values */
        *bytes = clo(*s);
        *r = 0;

        /* Loop through string, checking for null termination */
        for(t = 0; t < *bytes && s[t] != '\0'; t++)
        {
            /* Shift up previous bits */
            *r <<= 6;

            /* Add the required bits */
            if(t == 0)
            {
                *r |= s[t] & (0xff >> (*bytes + 1));
            }
            else
            {
                *r |= s[t] & 0x3f;
            }
        }

        /* Success if no NULL was encountered */
        return t == *bytes;
    }
}

/* END OF FILE */
