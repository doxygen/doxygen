/***************************************************************************
 *
 * $Id: adraw.c 161 2010-10-26 20:17:16Z Michael.McTernan $
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

/***************************************************************************
 * Include Files
 ***************************************************************************/

#include "mscgen_config.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mscgen_adraw_int.h"

/***************************************************************************
 * Functions
 ***************************************************************************/

Boolean ADrawOpen(unsigned int    w,
                  unsigned int    h,
                  const char     *file,
                  const char     *fontName,
                  ADrawOutputType type,
                  struct ADrawTag *outContext)
{
    assert(outContext);

    switch(type)
    {
        case ADRAW_FMT_NULL:
            return NullInit(outContext);

        case ADRAW_FMT_PNG:
#if !defined(REMOVE_PNG_OUTPUT)
            return GdoInit(w, h, file, fontName, outContext);
#else
            fprintf(stderr, "Built with REMOVE_PNG_OUTPUT; PNG output is not supported\n");
            return FALSE;
#endif
        case ADRAW_FMT_EPS:
            return PsInit(w, h, file, outContext);

        case ADRAW_FMT_SVG:
            return SvgInit(w, h, file, outContext);

        default:
            return FALSE;
    }
}


ADrawColour ADrawGetColour(const char *colour)
{
    assert(colour != NULL);

    /* Check if an RGB value has been specified */
    if(*colour == '#')
    {
        unsigned int c = ADRAW_COL_BLACK;

        if(sscanf(&colour[1], "%x", &c) == 1)
        {
            return (ADrawColour)c;
        }
    }
    else /* Check for name matches */
    {
        static const struct
        {
            char       *name;
            ADrawColour col;
        }
        colourMap[] =
        {
            { "WHITE",        ADRAW_COL_WHITE },
            { "BLACK",        ADRAW_COL_BLACK },
            { "RED",          ADRAW_COL_RED },
            { "ORANGE",       ADRAW_COL_ORANGE },
            { "YELLOW",       ADRAW_COL_YELLOW },
            { "GREEN",        ADRAW_COL_GREEN },
            { "BLUE",         ADRAW_COL_BLUE },
            { "INDIGO",       ADRAW_COL_INDIGO },
            { "VIOLET",       ADRAW_COL_VIOLET },
            { "SILVER",       ADRAW_COL_SILVER },
            { "LIME",         ADRAW_COL_LIME },
            { "GRAY",         ADRAW_COL_GRAY },
            { "OLIVE",        ADRAW_COL_OLIVE },
            { "MAROON",       ADRAW_COL_MAROON },
            { "NAVY",         ADRAW_COL_NAVY },
            { "PURPLE",       ADRAW_COL_PURPLE },
            { "TEAL",         ADRAW_COL_TEAL },
            { "FUCHSIA",      ADRAW_COL_FUCHSIA },
            { "AQUA",         ADRAW_COL_AQUA }
        };

        unsigned int t;

        for(t = 0; t < sizeof(colourMap) / sizeof(colourMap[0]); t++)
        {
            if(strcasecmp(colour, colourMap[t].name) == 0)
            {
                return colourMap[t].col;
            }
        }
    }

    /* Default to black if all else failed */
    return ADRAW_COL_BLACK;
}


void ADrawComputeArcPoint(float cx, float cy, float w, float h, float degrees,
                          unsigned int *x, unsigned int *y)
{
    float rad = (float)((degrees * M_PI) / 180.0f);

    *x = (unsigned int)round(cx + ((w / 2.0f) * cos(rad)));
    *y = (unsigned int)round(cy + ((h / 2.0f) * sin(rad)));
}

/* END OF FILE */
