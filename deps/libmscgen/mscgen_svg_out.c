/***************************************************************************
 *
 * $Id: svg_out.c 186 2011-03-01 21:18:19Z Michael.McTernan $
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

#include "mscgen_config.h"
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mscgen_adraw_int.h"
#include "mscgen_safe.h"
#include "mscgen_utf8.h"

/***************************************************************************
 * Local types
 ***************************************************************************/

typedef struct SvgContextTag
{
    /** Output file. */
    FILE        *of;

    /** Current pen colour name. */
    const char  *penColName;

    /** Current background pen colour name. */
    const char  *penBgColName;

    int          fontPoints;
}
SvgContext;

typedef struct
{
    int capheight, xheight, ascender, descender;
    int widths[256];
}
SvgCharMetric;

/** Helvetica character widths.
 * This gives the width of each character is 1/1000ths of a point.
 * The values are taken from the Adobe Font Metric file for Hevletica.
 */
static const SvgCharMetric SvgHelvetica =
{
    718, 523, 718, -207,
    {
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      278,  278,  355,  556,  556,  889,  667,  222,
      333,  333,  389,  584,  278,  333,  278,  278,
      556,  556,  556,  556,  556,  556,  556,  556,
      556,  556,  278,  278,  584,  584,  584,  556,
     1015,  667,  667,  722,  722,  667,  611,  778,
      722,  278,  500,  667,  556,  833,  722,  778,
      667,  778,  722,  667,  611,  722,  667,  944,
      667,  667,  611,  278,  278,  278,  469,  556,
      222,  556,  556,  500,  556,  556,  278,  556,
      556,  222,  222,  500,  222,  833,  556,  556,
      556,  556,  333,  500,  278,  556,  500,  722,
      500,  500,  500,  334,  260,  334,  584,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,  333,  556,  556,  167,  556,  556,  556,
      556,  191,  333,  556,  333,  333,  500,  500,
       -1,  556,  556,  556,  278,   -1,  537,  350,
      222,  333,  333,  556, 1000, 1000,   -1,  611,
       -1,  333,  333,  333,  333,  333,  333,  333,
      333,   -1,  333,  333,   -1,  333,  333,  333,
     1000,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1, 1000,   -1,  370,   -1,   -1,   -1,   -1,
      556,  778, 1000,  365,   -1,   -1,   -1,   -1,
       -1,  889,   -1,   -1,   -1,  278,   -1,   -1,
      222,  611,  944,  611,   -1,   -1,   -1,   -1
    }
};

/***************************************************************************
 * Helper functions
 ***************************************************************************/


/** Get the context pointer from an ADraw structure.
 */
static SvgContext *getSvgCtx(struct ADrawTag *ctx)
{
    return (SvgContext *)ctx->internal;
}

/** Get the context pointer from an ADraw structure.
 */
static FILE *getSvgFile(struct ADrawTag *ctx)
{
    return getSvgCtx(ctx)->of;
}

static const char *getSvgPen(struct ADrawTag *ctx)
{
    return getSvgCtx(ctx)->penColName;
}

static const char *getSvgBgPen(struct ADrawTag *ctx)
{
    return getSvgCtx(ctx)->penBgColName;
}

/** Given a font metric measurement, return device dependent units.
 * Font metric data is stored as 1/1000th of a point, and therefore
 * needs to be multiplied by the font point size and divided by
 * 1000 to give a value in device dependent units.
 */
static int getSpace(struct ADrawTag *ctx, long thousanths)
{
    return ((thousanths * getSvgCtx(ctx)->fontPoints) + 500) / 1000;
}


/** Compute a point on an ellipse.
 * This computes the point on an ellipse.
 *
 * \param[in] cx,cy   Center of the ellipse.
 * \param[in] w,h     Ellipse width and height.
 * \param[in] a       Angle in degrees.
 * \param[in,out] x,y Pointer to be populated with result co-ordinates.
 */
static void arcPoint(float         cx,
                     float         cy,
                     float         w,
                     float         h,
                     float         a,
                     unsigned int *x,
                     unsigned int *y)
{
    float rad = (float)((a * M_PI) / 180.0f);

    /* Compute point, noting this is for SVG co-ordinate system */
    *x = (unsigned int)round(cx + ((w / 2.0f) * cos(rad)));
    *y = (unsigned int)round(cy + ((h / 2.0f) * sin(rad)));
}


/** Write out a line of text, escaping special characters.
 */
static void writeEscaped(struct ADrawTag *ctx, const char *string)
{
    FILE *f = getSvgFile(ctx);

    while(*string != '\0')
    {
        unsigned int code, bytes;

        switch(*string)
        {
            case '<': fprintf(f, "&lt;"); break;
            case '>': fprintf(f, "&gt;"); break;
            case '"': fprintf(f, "&quot;"); break;
            case '&': fprintf(f, "&amp;"); break;
            default:
                if(Utf8Decode(string, &code, &bytes))
                {
                    fprintf(f, "&#x%x;", code);
                    string += bytes - 1;
                }
                else
                {
                    fputc(*string, f);
                }
                break;

        }

        string++;
    }
}



static void svgRect(struct ADrawTag *ctx,
                    const char      *colour,
                    unsigned int     x1,
                    unsigned int     y1,
                    unsigned int     x2,
                    unsigned int     y2)
{
    fprintf(getSvgFile(ctx),
            "<polygon fill=\"%s\" points=\"%u,%u %u,%u %u,%u %u,%u\"/>\n",
            colour,
            x1, y1,
            x2, y1,
            x2, y2,
            x1, y2);
}

static const char *svgColour(ADrawColour col)
{
    switch(col)
    {
        case ADRAW_COL_WHITE:
            return "white";

        case ADRAW_COL_BLACK:
            return "black";

        case ADRAW_COL_BLUE:
            return "blue";

        case ADRAW_COL_RED:
            return "red";

        case ADRAW_COL_GREEN:
            return "green";

        default:
            return NULL;
    }
}


/***************************************************************************
 * API Functions
 ***************************************************************************/

unsigned int SvgTextWidth(struct ADrawTag *ctx,
                          const char *string)
{
    unsigned long width = 0;

    while(*string != '\0')
    {
        int           i = *string & 0xff;
        unsigned long w = SvgHelvetica.widths[i];

        /* Ignore undefined characters */
        width += w > 0 ? w : 0;

        string++;
    }

    return getSpace(ctx, width);
}


int SvgTextHeight(struct ADrawTag *ctx)
{
    return getSpace(ctx, SvgHelvetica.ascender - SvgHelvetica.descender);
}


void SvgLine(struct ADrawTag *ctx,
             unsigned int     x1,
             unsigned int     y1,
             unsigned int     x2,
             unsigned int     y2)
{
    fprintf(getSvgFile(ctx),
            "<line x1=\"%u\" y1=\"%u\" x2=\"%u\" y2=\"%u\" stroke=\"%s\"/>\n",
            x1, y1, x2, y2, getSvgPen(ctx));

}


void SvgDottedLine(struct ADrawTag *ctx,
                   unsigned int     x1,
                   unsigned int     y1,
                   unsigned int     x2,
                   unsigned int     y2)
{
    fprintf(getSvgFile(ctx),
            "<line x1=\"%u\" y1=\"%u\" x2=\"%u\" y2=\"%u\" stroke=\"%s\" stroke-dasharray=\"2,2\"/>\n",
            x1, y1, x2, y2, getSvgPen(ctx));
}


void SvgTextR(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    const SvgContext *context = getSvgCtx(ctx);

    svgRect(ctx, getSvgBgPen(ctx), x - 2, y - SvgTextHeight(ctx) + 1, x + SvgTextWidth(ctx, string), y - 1);

    y += getSpace(ctx, SvgHelvetica.descender);

    fprintf(getSvgFile(ctx),
            "<text x=\"%u\" y=\"%u\" textLength=\"%u\" font-family=\"Helvetica\" font-size=\"%u\" fill=\"%s\">\n",
            x - 1, y, SvgTextWidth(ctx, string), context->fontPoints, context->penColName);
    writeEscaped(ctx, string);
    fprintf(getSvgFile(ctx), "\n</text>\n");
}


void SvgTextL(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    const SvgContext *context = getSvgCtx(ctx);

    svgRect(ctx, getSvgBgPen(ctx), x - (SvgTextWidth(ctx, string) + 2), y - SvgTextHeight(ctx) + 1, x, y - 1);

    y += getSpace(ctx, SvgHelvetica.descender);

    fprintf(getSvgFile(ctx),
            "<text x=\"%u\" y=\"%u\" textLength=\"%u\" font-family=\"Helvetica\" font-size=\"%u\" fill=\"%s\" text-anchor=\"end\">\n",
            x, y, SvgTextWidth(ctx, string), context->fontPoints, context->penColName);
    writeEscaped(ctx, string);
    fprintf(getSvgFile(ctx), "\n</text>\n");


}


void SvgTextC(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    const SvgContext  *context = getSvgCtx(ctx);
    unsigned int hw = SvgTextWidth(ctx, string) / 2;

    svgRect(ctx, getSvgBgPen(ctx), x - (hw + 2), y - SvgTextHeight(ctx) + 1, x + hw, y - 1);

    y += getSpace(ctx, SvgHelvetica.descender);

    fprintf(getSvgFile(ctx),
            "<text x=\"%u\" y=\"%u\" textLength=\"%u\" font-family=\"Helvetica\" font-size=\"%u\" fill=\"%s\" text-anchor=\"middle\">\n\n",
            x, y, SvgTextWidth(ctx, string), context->fontPoints, context->penColName);
    writeEscaped(ctx, string);
    fprintf(getSvgFile(ctx), "\n</text>\n");
}


void SvgFilledTriangle(struct ADrawTag *ctx,
                       unsigned int x1,
                       unsigned int y1,
                       unsigned int x2,
                       unsigned int y2,
                       unsigned int x3,
                       unsigned int y3)
{

    fprintf(getSvgFile(ctx),
            "<polygon fill=\"%s\" points=\"%u,%u %u,%u %u,%u\"/>\n",
            getSvgPen(ctx),
            x1, y1,
            x2, y2,
            x3, y3);
}


void SvgFilledCircle(struct ADrawTag *ctx,
                     unsigned int x,
                     unsigned int y,
                     unsigned int r)
{
    fprintf(getSvgFile(ctx),
            "<circle fill=\"%s\" cx=\"%u\" cy=\"%u\" r=\"%u\"/>\n",
            getSvgPen(ctx),
            x, y, r);
}


void SvgFilledRectangle(struct ADrawTag *ctx,
                        unsigned int x1,
                        unsigned int y1,
                        unsigned int x2,
                        unsigned int y2)
{
    svgRect(ctx, getSvgPen(ctx), x1, y1, x2, y2);
}


void SvgArc(struct ADrawTag *ctx,
            unsigned int cx,
            unsigned int cy,
            unsigned int w,
            unsigned int h,
            unsigned int s,
            unsigned int e)
{
    unsigned int sx, sy, ex, ey;

    /* Get start and end x,y */
    arcPoint((float)cx, (float)cy, (float)w, (float)h, (float)s, &sx, &sy);
    arcPoint((float)cx, (float)cy, (float)w, (float)h, (float)e, &ex, &ey);

    fprintf(getSvgFile(ctx),
            "<path d=\"M %u %u A%u,%u 0 0,1 %u,%u\" stroke=\"%s\" fill=\"none\"/>",
            sx, sy, w / 2, h / 2,  ex, ey, getSvgPen(ctx));
}


void SvgDottedArc(struct ADrawTag *ctx,
                  unsigned int cx,
                  unsigned int cy,
                  unsigned int w,
                  unsigned int h,
                  unsigned int s,
                  unsigned int e)
{
    unsigned int sx, sy, ex, ey;

    /* Get start and end x,y */
    arcPoint((float)cx, (float)cy, (float)w, (float)h, (float)s, &sx, &sy);
    arcPoint((float)cx, (float)cy, (float)w, (float)h, (float)e, &ex, &ey);


    fprintf(getSvgFile(ctx),
            "<path d=\"M %u %u A%u,%u 0 0,1 %u,%u\" stroke=\"%s\" fill=\"none\" stroke-dasharray=\"2,2\"/>",
            sx, sy, w / 2, h / 2,  ex, ey, getSvgPen(ctx));
}


void SvgSetPen(struct ADrawTag *ctx,
               ADrawColour      col)
{
    static char colCmd[10];

    getSvgCtx(ctx)->penColName = svgColour(col);
    if(getSvgCtx(ctx)->penColName == NULL)
    {
        /* Print the RGB value into the local storage */
        sprintf(colCmd, "#%06X", col);

        /* Now set the colour name to the local store */
        getSvgCtx(ctx)->penColName = colCmd;
    }
}


void SvgSetBgPen(struct ADrawTag *ctx,
                 ADrawColour      col)
{
    static char colCmd[10];

    getSvgCtx(ctx)->penBgColName = svgColour(col);
    if(getSvgCtx(ctx)->penBgColName == NULL)
    {
        /* Print the RGB value into the local storage */
        sprintf(colCmd, "#%06X", col);

        /* Now set the colour name to the local store */
        getSvgCtx(ctx)->penBgColName = colCmd;
    }
}


void SvgSetFontSize(struct ADrawTag *ctx,
                    ADrawFontSize    size)
{
    SvgContext *context = getSvgCtx(ctx);

    switch(size)
    {
        case ADRAW_FONT_TINY:
            context->fontPoints = 8;
            break;

        case ADRAW_FONT_SMALL:
            context->fontPoints = 12;
            break;

        default:
            assert(0);
    }

}


Boolean SvgClose(struct ADrawTag *ctx)
{
    SvgContext *context = getSvgCtx(ctx);

    /* Close the SVG */
    fprintf(context->of, "</svg>\n");

    /* Close the output file */
    if(context->of != stdout)
    {
        fclose(context->of);
    }

    /* Free and destroy context */
    free(context);
    ctx->internal = NULL;

    return TRUE;
}



Boolean SvgInit(unsigned int     w,
                unsigned int     h,
                const char      *file,
                struct ADrawTag *outContext)
{
    SvgContext *context;

    /* Create context */
    context = outContext->internal = malloc_s(sizeof(SvgContext));
    if(context == NULL)
    {
        return FALSE;
    }

    /* Open the output file */
    if(strcmp(file, "-") == 0)
    {
        context->of = stdout;
    }
    else
    {
        context->of = fopen(file, "wb");
        if(!context->of)
        {
            fprintf(stderr, "SvgInit: Failed to open output file '%s': %s\n", file, strerror(errno));
            return FALSE;
        }
    }

    /* Set the initial pen state */
    SvgSetPen(outContext, ADRAW_COL_BLACK);
    SvgSetBgPen(outContext, ADRAW_COL_WHITE);

    /* Default to small font */
    SvgSetFontSize(outContext, ADRAW_FONT_SMALL);

    fprintf(context->of, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n"
                         " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");

    fprintf(context->of, "<svg version=\"1.1\"\n"
                         " width=\"%upx\" height=\"%upx\"\n"
                         " viewBox=\"0 0 %u %u\"\n"
                         " xmlns=\"http://www.w3.org/2000/svg\" shape-rendering=\"crispEdges\"\n"
                         " stroke-width=\"1\" text-rendering=\"geometricPrecision\">\n",
                         w, h, w, h);

    /* Now fill in the function pointers */
    outContext->line            = SvgLine;
    outContext->dottedLine      = SvgDottedLine;
    outContext->textL           = SvgTextL;
    outContext->textC           = SvgTextC;
    outContext->textR           = SvgTextR;
    outContext->textWidth       = SvgTextWidth;
    outContext->textHeight      = SvgTextHeight;
    outContext->filledRectangle = SvgFilledRectangle;
    outContext->filledTriangle  = SvgFilledTriangle;
    outContext->filledCircle    = SvgFilledCircle;
    outContext->arc             = SvgArc;
    outContext->dottedArc       = SvgDottedArc;
    outContext->setPen          = SvgSetPen;
    outContext->setBgPen        = SvgSetBgPen;
    outContext->setFontSize     = SvgSetFontSize;
    outContext->close           = SvgClose;

    return TRUE;
}

/* END OF FILE */
