/***************************************************************************
 *
 * $Id: ps_out.c 186 2011-03-01 21:18:19Z Michael.McTernan $
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
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mscgen_adraw_int.h"
#include "mscgen_utf8.h"
#include "mscgen_safe.h"

/***************************************************************************
 * Manifest Constants
 ***************************************************************************/

/** Overall scaling of the Postscript output.
 */
#define PS_OUT_SCALE  0.7f

/***************************************************************************
 * Local types
 ***************************************************************************/

typedef struct PsContextTag
{
    /** Output file. */
    FILE        *of;

    /** Point size of the current font. */
    int          fontPoints;

    /** Current pen colour. */
    ADrawColour  penColour;

    /** Background colour for the pen. */
    ADrawColour  penBgColour;
}
PsContext;

typedef struct
{
    int capheight, xheight, ascender, descender;
    int widths[256];
}
PsCharMetric;

/** Helvetica character widths.
 * This gives the width of each character is 1/1000ths of a point.
 * The values are taken from the Adobe Font Metric file for Hevletica.
 */
static const PsCharMetric PsHelvetica =
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
static PsContext *getPsCtx(struct ADrawTag *ctx)
{
    return (PsContext *)ctx->internal;
}

/** Get the context pointer from an ADraw structure.
 */
static FILE *getPsFile(struct ADrawTag *ctx)
{
    return getPsCtx(ctx)->of;
}

/** Given a font metric measurement, return device dependent units.
 * Font metric data is stored as 1/1000th of a point, and therefore
 * needs to be multiplied by the font point size and divided by
 * 1000 to give a value in device dependent units.
 */
static int getSpace(struct ADrawTag *ctx, long thousanths)
{
    return ((thousanths * getPsCtx(ctx)->fontPoints) + 500) / 1000;
}

/** Write out a line of text, escaping special characters.
 */
static void writeEscaped(struct ADrawTag *ctx, const char *string)
{
    FILE *f = getPsFile(ctx);

    while(*string != '\0')
    {
        unsigned int code, bytes;

        switch(*string)
        {
            case '(': fprintf(f, "\\("); break;
            case ')': fprintf(f, "\\)"); break;
            default:
                if(Utf8Decode(string, &code, &bytes))
                {
                    fprintf(f, "\\%o", code);
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

static void setColour(struct ADrawTag *ctx,
                      ADrawColour      col)
{
    float r, g, b;

    /* Extract RGB values */
    r = (float)((col & 0xff0000) >> 16);
    g = (float)((col & 0x00ff00) >>  8);
    b = (float)((col & 0x0000ff) >>  0);

    /* Normalise */
    r /= 255.0f;
    g /= 255.0f;
    b /= 255.0f;

    /* Generate output command */
    fprintf(getPsFile(ctx), "%f %f %f setrgbcolor\n", r ,g ,b);
}

/***************************************************************************
 * API Functions
 ***************************************************************************/

unsigned int PsTextWidth(struct ADrawTag *ctx,
                          const char *string)
{
    unsigned long width = 0;

    while(*string != '\0')
    {
        int           i = *string & 0xff;
        unsigned long w = PsHelvetica.widths[i];

        /* Ignore undefined characters */
        width += w > 0 ? w : 0;

        string++;
    }

    return getSpace(ctx, width);
}


int PsTextHeight(struct ADrawTag *ctx)
{
    return getSpace(ctx, PsHelvetica.ascender - PsHelvetica.descender);
}


void PsLine(struct ADrawTag *ctx,
             unsigned int     x1,
             unsigned int     y1,
             unsigned int     x2,
             unsigned int     y2)
{
    fprintf(getPsFile(ctx),
            "newpath %d %d moveto %d %d lineto stroke\n",
            x1, 0-y1, x2, 0-y2);

}


void PsDottedLine(struct ADrawTag *ctx,
                   unsigned int     x1,
                   unsigned int     y1,
                   unsigned int     x2,
                   unsigned int     y2)
{
    fprintf(getPsFile(ctx), "[2] 0 setdash\n");
    PsLine(ctx, x1, y1, x2, y2);
    fprintf(getPsFile(ctx), "[] 0 setdash\n");
}


void PsFilledRectangle(struct ADrawTag *ctx,
                       unsigned int     x1,
                       unsigned int     y1,
                       unsigned int     x2,
                       unsigned int     y2)
{
    fprintf(getPsFile(ctx),
            "newpath "
            "%d %d moveto "
            "%d %d lineto "
            "%d %d lineto "
            "%d %d lineto "
            "closepath "
            "fill\n",
            x1, 0-y1,
            x2, 0-y1,
            x2, 0-y2,
            x1, 0-y2);
}


void PsTextR(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    const PsContext *context = getPsCtx(ctx);

    /* Push the string and get its width */
    fprintf(getPsFile(ctx), "(");
    writeEscaped(ctx, string);
    fprintf(getPsFile(ctx), ") dup stringwidth\n");

    /* Draw the background box */
    setColour(ctx, context->penBgColour);
    fprintf(getPsFile(ctx), "pop "                /* Ignore y-value */
                            "dup "                /* Duplicate string width */
                            "newpath "
                            "%d %d moveto "       /* Bottom left of the box */
                            "0 rlineto "          /* Move to bottom right of the box */
                            "0 %d rlineto "       /* To top right */
                            "neg 0 rlineto "      /* Back to bottom left */
                            "closepath fill\n",   /* Done */
                            x, 0-y - getSpace(ctx, PsHelvetica.descender),
                            getSpace(ctx, PsHelvetica.ascender));

    /* Restore pen and show the string */
    setColour(ctx, context->penColour);
    fprintf(getPsFile(ctx), "%d %d moveto show\n",
                             x, 0-y - getSpace(ctx, PsHelvetica.descender));
}


void PsTextL(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    const PsContext *context = getPsCtx(ctx);

    /* Draw the background box */
    setColour(ctx, context->penBgColour);
    PsFilledRectangle(ctx, x, 0-y, x + 10, 0-y + 10);
    setColour(ctx, context->penColour);

    fprintf(getPsFile(ctx),
            "%d %d moveto "
            "(",
            x, 0-y - getSpace(ctx, PsHelvetica.descender));
    writeEscaped(ctx, string);
    fprintf(getPsFile(ctx),
            ") dup stringwidth "
            "pop "  /* Ignore y value */
            "neg "  /* Invert x value */
            "0 "
            "rmoveto "
            "show\n");
}


void PsTextC(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    const PsContext *context = getPsCtx(ctx);

    /* Push the string and get its width */
    fprintf(getPsFile(ctx), "(");
    writeEscaped(ctx, string);
    fprintf(getPsFile(ctx), ") dup stringwidth\n");

    /* Draw the background box */
    setColour(ctx, context->penBgColour);
    fprintf(getPsFile(ctx), "pop "                     /* Ignore y-value */
                            "dup dup "                 /* Duplicate string width twice */
                            "newpath "
                            "%d %d moveto "            /* Starting point, centre bottom of box */
                            "2 div neg 0 rmoveto "     /* Move to bottom left */
                            "0 rlineto "               /* Move to bottom right of the box */
                            "0 %d rlineto "            /* To top right */
                            "neg 0 rlineto "           /* Back to bottom left */
                            "closepath fill\n",        /* Done */
                            x, 0-y,
                            getSpace(ctx, PsHelvetica.ascender));

    /* Restore pen and show the string */
    setColour(ctx, context->penColour);
    fprintf(getPsFile(ctx), "%d %d moveto dup stringwidth pop 2 div neg 0 rmoveto show\n",
                             x, 0-y - getSpace(ctx, PsHelvetica.descender));
}


void PsFilledTriangle(struct ADrawTag *ctx,
                       unsigned int x1,
                       unsigned int y1,
                       unsigned int x2,
                       unsigned int y2,
                       unsigned int x3,
                       unsigned int y3)
{
    fprintf(getPsFile(ctx),
            "newpath "
            "%d %d moveto "
            "%d %d lineto "
            "%d %d lineto "
            "closepath "
            "fill\n",
            x1, 0-y1,
            x2, 0-y2,
            x3, 0-y3);
}


void PsFilledCircle(struct ADrawTag *ctx,
                     unsigned int x,
                     unsigned int y,
                     unsigned int r)
{
    fprintf(getPsFile(ctx),
            "newpath "
            "%d %d %d 0 360 arc "
            "closepath "
            "fill\n",
            x, 0-y, r);
}


void PsArc(struct ADrawTag *ctx,
            unsigned int cx,
            unsigned int cy,
            unsigned int w,
            unsigned int h,
            unsigned int s,
            unsigned int e)
{
    fprintf(getPsFile(ctx),
            "newpath "
            "%d %d %d %d %d %d ellipse "
            "stroke\n",
            cx, 0-cy, w, h, s, e);
}


void PsDottedArc(struct ADrawTag *ctx,
                  unsigned int cx,
                  unsigned int cy,
                  unsigned int w,
                  unsigned int h,
                  unsigned int s,
                  unsigned int e)
{
    fprintf(getPsFile(ctx), "[2] 0 setdash\n");
    PsArc(ctx, cx, cy, w, h, s, e);
    fprintf(getPsFile(ctx), "[] 0 setdash\n");
}


void PsSetPen(struct ADrawTag *ctx,
              ADrawColour      col)
{
    PsContext *context = getPsCtx(ctx);

    assert(col != ADRAW_COL_INVALID);

    /* Check if the pen colour has changed */
    if(context->penColour != col)
    {
        setColour(ctx, col);

        /* Store the pen colour */
        context->penColour = col;
    }
}


void PsSetBgPen(struct ADrawTag *ctx,
                ADrawColour      col)
{
    PsContext *context = getPsCtx(ctx);

    context->penBgColour = col;
}


void PsSetFontSize(struct ADrawTag *ctx,
                   ADrawFontSize    size)
{
    PsContext *context = getPsCtx(ctx);

    switch(size)
    {
        case ADRAW_FONT_TINY:
            getPsCtx(ctx)->fontPoints = 8;
            break;

        case ADRAW_FONT_SMALL:
            getPsCtx(ctx)->fontPoints = 12;
            break;

        default:
            assert(0);
    }

    fprintf(context->of, "/Helvetica findfont\n");
    fprintf(context->of, "%d scalefont\n", getPsCtx(ctx)->fontPoints);
    fprintf(context->of, "setfont\n");
}


Boolean PsClose(struct ADrawTag *ctx)
{
    PsContext *context = getPsCtx(ctx);

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



Boolean PsInit(unsigned int     w,
               unsigned int     h,
               const char      *file,
               struct ADrawTag *outContext)
{
    PsContext *context;

    /* Create context */
    context = outContext->internal = malloc_s(sizeof(PsContext));
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
            fprintf(stderr, "PsInit: Failed to open output file '%s': %s\n", file, strerror(errno));
            return FALSE;
        }
    }

    /* Write the header */
    fprintf(context->of, "%%!PS-Adobe-3.0 EPSF-2.0\n"
                         "%%%%BoundingBox: 0 0 %.0f %.0f\n", w * PS_OUT_SCALE, h * PS_OUT_SCALE);
    fprintf(context->of, "%%%%Creator: mscgen %s\n", PACKAGE_VERSION);
    fprintf(context->of, "%%%%EndComments\n");

    /* Shrink everything by 70% */
    fprintf(context->of, "%f %f scale\n", PS_OUT_SCALE, PS_OUT_SCALE);

    /* Create clipping rectangle to constrain dimensions */
    fprintf(context->of, "0 0 moveto\n");
    fprintf(context->of, "0 %u lineto\n", h);
    fprintf(context->of, "%u %u lineto\n", w, h);
    fprintf(context->of, "%u 0 lineto\n", w);
    fprintf(context->of, "closepath\n");
    fprintf(context->of, "clip\n");
    fprintf(context->of, "%%PageTrailer\n");
    fprintf(context->of, "%%Page: 1 1\n");

    /* Set default font */
    fprintf(context->of, "/Helvetica findfont\n");
    fprintf(context->of, "10 scalefont\n");
    fprintf(context->of, "setfont\n");

    /* Get the default font size */
    PsSetFontSize(outContext, ADRAW_FONT_SMALL);

    /* Translate up by the height, y-axis will be inverted */
    fprintf(context->of, "0 %d translate\n", h);

    /* Arc drawing function */
    fprintf(context->of, "/mtrx matrix def\n"
                         "/ellipse\n"
                         "  { /endangle exch def\n"
                         "    /startangle exch def\n"
                         "    /ydia exch def\n"
                         "    /xdia exch def\n"
                         "    /y exch def\n"
                         "    /x exch def\n"
                         "    /savematrix mtrx currentmatrix def\n"
                         "    x y translate\n"
                         "    xdia 2 div ydia 2 div scale\n"
                         "    1 -1 scale\n"
                         "    0 0 1 startangle endangle arc\n"
                         "    savematrix setmatrix\n"
                         "} def\n");

    /* Set the current pen colours */
    context->penColour = ADRAW_COL_BLACK;
    context->penBgColour = ADRAW_COL_WHITE;

    /* Now fill in the function pointers */
    outContext->line            = PsLine;
    outContext->dottedLine      = PsDottedLine;
    outContext->textL           = PsTextL;
    outContext->textC           = PsTextC;
    outContext->textR           = PsTextR;
    outContext->textWidth       = PsTextWidth;
    outContext->textHeight      = PsTextHeight;
    outContext->filledRectangle = PsFilledRectangle;
    outContext->filledTriangle  = PsFilledTriangle;
    outContext->filledCircle    = PsFilledCircle;
    outContext->arc             = PsArc;
    outContext->dottedArc       = PsDottedArc;
    outContext->setPen          = PsSetPen;
    outContext->setBgPen        = PsSetBgPen;
    outContext->setFontSize     = PsSetFontSize;
    outContext->close           = PsClose;

    return TRUE;
}

/* END OF FILE */
