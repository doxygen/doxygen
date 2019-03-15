/***************************************************************************
 *
 * $Id: null_out.c 112 2010-08-18 12:59:54Z Michael.McTernan $
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


#include <stdio.h>
#include <assert.h>

#include "mscgen_adraw_int.h"

/***************************************************************************
 * API Functions
 ***************************************************************************/

static unsigned int NullTextWidth(struct ADrawTag *ctx UNUSED,
                                  const char *string UNUSED)
{
    return 0;
}


static int NullTextHeight(struct ADrawTag *ctx UNUSED)
{
    return 0;
}


static void NullLine(struct ADrawTag *ctx UNUSED,
                     unsigned int     x1 UNUSED,
                     unsigned int     y1 UNUSED,
                     unsigned int     x2 UNUSED,
                     unsigned int     y2 UNUSED)
{
}


static void NullDottedLine(struct ADrawTag *ctx UNUSED,
                           unsigned int     x1 UNUSED,
                           unsigned int     y1 UNUSED,
                           unsigned int     x2 UNUSED,
                           unsigned int     y2 UNUSED)
{
}


static void NullTextR(struct ADrawTag *ctx UNUSED,
                      unsigned int     x UNUSED,
                      unsigned int     y UNUSED,
                      const char      *string UNUSED)
{
}


static void NullTextL(struct ADrawTag *ctx UNUSED,
                      unsigned int     x UNUSED,
                      unsigned int     y UNUSED,
                      const char      *string UNUSED)
{
}


static void NullTextC(struct ADrawTag *ctx UNUSED,
                      unsigned int     x UNUSED,
                      unsigned int     y UNUSED,
                      const char      *string UNUSED)
{
}


static void NullFilledRectangle(struct ADrawTag *ctx UNUSED,
                               unsigned int x1 UNUSED,
                               unsigned int y1 UNUSED,
                               unsigned int x2 UNUSED,
                               unsigned int y2 UNUSED)
{
}


static void NullFilledTriangle(struct ADrawTag *ctx UNUSED,
                               unsigned int x1 UNUSED,
                               unsigned int y1 UNUSED,
                               unsigned int x2 UNUSED,
                               unsigned int y2 UNUSED,
                               unsigned int x3 UNUSED,
                               unsigned int y3 UNUSED)
{
}


static void NullArc(struct ADrawTag *ctx UNUSED,
                    unsigned int cx UNUSED,
                    unsigned int cy UNUSED,
                    unsigned int w UNUSED,
                    unsigned int h UNUSED,
                    unsigned int s UNUSED,
                    unsigned int e UNUSED)
{
}


static void NullDottedArc(struct ADrawTag *ctx UNUSED,
                          unsigned int cx UNUSED,
                          unsigned int cy UNUSED,
                          unsigned int w UNUSED,
                          unsigned int h UNUSED,
                          unsigned int s UNUSED,
                          unsigned int e UNUSED)
{
}


static void NullSetPen (struct ADrawTag *ctx UNUSED,
                        ADrawColour      col UNUSED)
{
}


static void NullSetFontSize(struct ADrawTag *ctx UNUSED,
                            ADrawFontSize    size UNUSED)
{
}


static Boolean NullClose(struct ADrawTag *ctx UNUSED)
{
    return TRUE;
}


Boolean NullInit(struct ADrawTag *outContext)
{
    /* Fill in the function pointers */
    outContext->line            = NullLine;
    outContext->dottedLine      = NullDottedLine;
    outContext->textL           = NullTextL;
    outContext->textC           = NullTextC;
    outContext->textR           = NullTextR;
    outContext->textWidth       = NullTextWidth;
    outContext->textHeight      = NullTextHeight;
    outContext->filledRectangle = NullFilledRectangle;
    outContext->filledTriangle  = NullFilledTriangle;
    outContext->arc             = NullArc;
    outContext->dottedArc       = NullDottedArc;
    outContext->setPen          = NullSetPen;
    outContext->setBgPen        = NullSetPen;
    outContext->setFontSize     = NullSetFontSize;
    outContext->close           = NullClose;

    return TRUE;
}

/* END OF FILE */
