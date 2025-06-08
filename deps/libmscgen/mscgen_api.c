#include "mscgen_api.h"
#include "mscgen_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef  HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include "mscgen_lexer.h"
#include "mscgen_adraw.h"
#include "mscgen_msc.h"
#include "mscgen_safe.h"

/***************************************************************************
 * Macro definitions
 ***************************************************************************/

#define M_Max(a, b) (((a) > (b)) ? (a) : (b))
#define M_Min(a, b) (((a) < (b)) ? (a) : (b))

/***************************************************************************
 * Types
 ***************************************************************************/

/** Structure for holding global options.
 * This structure groups all the options that affect the text output into
 * one structure.
 */
typedef struct GlobalOptionsTag
{
    /** Ideal width of output canvas.
     * If this value allows the entitySpacing to be increased, then
     * entitySpacing will be set to the larger value of it's original
     * value and idealCanvasWidth / number of entities.
     */
    unsigned int idealCanvasWidth;

    /** Horizontal spacing between entities. */
    unsigned int entitySpacing;

    /** Gap at the top of the page. */
    unsigned int entityHeadGap;

    /** Vertical spacing between arcs. */
    unsigned int arcSpacing;

    /** Arc gradient.
     * Y offset of arc head, relative to tail, in pixels.
     */
    int          arcGradient;

    /** Gap between adjacent boxes. */
    unsigned int boxSpacing;

    /** Minimum distance between box edges and text. */
    unsigned int boxInternalBorder;

    /** Radius of rounded box corner arcs. */
    unsigned int rboxArc;

    /** Size of 'corner' added to note boxes. */
    unsigned int noteCorner;

    /** Angular box slope in pixels. */
    unsigned int aboxSlope;

    /** If TRUE, wrap arc text as well as box contents. */
    Boolean      wordWrapArcLabels;

    /** Horizontal width of the arrow heads. */
    unsigned int arrowWidth;

    /** Vertical depth of the arrow heads. */
    unsigned int arrowHeight;

    /** Height of an arc which loops back to itself. */
    unsigned int loopArcHeight;

    /** Horizontal gap between text and horizontal lines. */
    unsigned int textHGapPre;

    /** Horizontal gap between text and horizontal lines. */
    unsigned int textHGapPost;
}
GlobalOptions;

/** Information about each out row.
 */
typedef struct
{
    /** Minimum Y value. */
    unsigned int ymin;

    /** Y position of the arc on the row. */
    unsigned int arcliney;

    /** Maximum Y value. */
    unsigned int ymax;

    /** Maximum lines of text on the row. */
    unsigned int maxTextLines;
}
RowInfo;


typedef struct ContextTag
{
   GlobalOptions opts;
   ADraw drw;
} Context;


/***************************************************************************
 * Local Variables.
 ***************************************************************************/

static const GlobalOptions gDefaultOpts =
{
    600,    /* idealCanvasWidth */

    80,     /* entitySpacing */
    20,     /* entityHeadGap */
    6,      /* arcSpacing */
    0,      /* arcGradient */
    8,      /* boxSpacing */
    4,      /* boxInternalBorder */
    6,      /* rboxArc */
    12,     /* noteCorner */
    6,      /* aboxSlope */
    FALSE,  /* wordWrapArcLabels */

    /* Arrow options */
    10, 6,

    /* loopArcHeight */
    12,

    /* textHGapPre, textHGapPost */
    2, 2
};


/***************************************************************************
 * Functions
 ***************************************************************************/

/** Count the number of lines in some string.
 * This counts line breaks that are written as a literal '\n' in the line to
 * determine how many lines of output are needed.
 *
 * \param[in] l  Pointer to the input string to inspect.
 * \returns      The count of '\n' characters appearing in the input string + 1.
 */
static unsigned int countLines(const char *l)
{
    unsigned int c = 1;

    do
    {
        c++;

        l = strstr(l, "\\n");
        if (l) l += 2;
    } while (l != NULL);

    return c;
}

/** Check if some arc type indicates a box.
 */
static Boolean isBoxArc(const MscArcType a)
{
    return a == MSC_ARC_BOX || a == MSC_ARC_RBOX  ||
           a == MSC_ARC_ABOX || a== MSC_ARC_NOTE;
}

/** Get the skip value in pixels for some the current arc in the Msc.
 */
static int getArcGradient(Context *ctx, Msc m, const RowInfo *rowInfo, unsigned int row)
{
    const char   *s = MscGetCurrentArcAttrib(m, MSC_ATTR_ARC_SKIP);
    unsigned int  v = ctx->opts.arcGradient;

    if (s != NULL && rowInfo != NULL)
    {
        const unsigned int rowCount = MscGetNumArcs(m) - MscGetNumParallelArcs(m);
        unsigned int       skip;

        if (sscanf(s, "%u", &skip) == 1)
        {
            unsigned int ystart = rowInfo[row].arcliney;
            unsigned int yend   = rowInfo[M_Min(rowCount - 1, row + skip)].arcliney;

            v += yend - ystart;
        }
        else
        {
            fprintf(stderr, "Warning: Non-integer arcskip value: %s\n", s);
        }
    }

    return v;
}


/** Check if some arc name indicates a broadcast entity.
 */
static Boolean isBroadcastArc(const char *entity)
{
    return entity != NULL && (strcmp(entity, "*") == 0);
}

/** Free memory allocated for the label lines.
 */
static void freeLabelLines(unsigned int n, char **lines)
{
    while (n > 0)
    {
        n--;
        free(lines[n]);
    }

    free(lines);
}


/** Word wrap a line of text until the first line is less than \a width wide.
 * This removes words from the input line and builds them into a 2nd new
 * string until the input line is shorter than the supplied width.  The
 * input string is directly truncated, while the remaining characters are
 * returned in a new memory allocation.  On return, the input line of text
 * will be shorter than \a width, while the newly returned string will contain
 * all the remaining characters.
 *
 * If the input line is already shorter than \a width, the function returns
 * NULL and does not modify the input line of text.
 *
 * \param[in,out] l     Input line of text which maybe modified if needed.
 * \param[in]     width Maximum allowable text line width.
 * \returns       NULL if \a l was already less then \a width long,
 *                 otherwise a new string giving the remained of the string.
 */
static char *splitStringToWidth(Context *ctx, char *l, unsigned int width)
{
    char *p = l + strlen(l);
    char *orig = NULL;

    if (ctx->drw.textWidth(&ctx->drw, l) > width)
    {
        /* Duplicate the original string */
        orig = strdup_s(l);

        /* Now remove words from the line until it fits the available width */
        do
        {
            /* Skip back 1 word */
            while (!isspace(*p) && p > l)
            {
                p--;
            }

            if (p > l)
            {
                *p = '\0';
            }
        }
        while (ctx->drw.textWidth(&ctx->drw, l) > width && p > l);

        /* Check if the first word is bigger than the available space;
         *  we need to hyphenate in this case.
         */
        if (p == l)
        {
            const unsigned int hyphenWidth = ctx->drw.textWidth(&ctx->drw, "-");

            /* Find the end of the first word */
            while (!isspace(*p) && *p != '\0')
            {
                p++;
            }

            /* Start removing characters from the word */
            do
            {
                *p = '\0';
                p--;
            }
            while (ctx->drw.textWidth(&ctx->drw, l) + hyphenWidth > width && p > l);

            /* Add a hyphen */
            *p = '-';
        }

        /* Copy the remaining line to the start of the string */
        int m = 0;
        int n = (int)(p - l);

        while (isspace(orig[n]) && orig[n] != '\0')
        {
            n++;
        }

        do
        {
            orig[m++] = orig[n++];
        }
        while (orig[m - 1] != '\0');
    }

    return orig;
}


/** Split an input arc label into lines, word-wrapping if needed.
 * This takes the literal label supplied from the input and splits it into an
 * array of char * text lines.  Splitting is first done according to literal
 * '\n' character sequences added by the user, then according to word wrapping
 * to fit available space, if appropriate.
 *
 * \param[in]     m         The MSC for which the lines are to be split.
 * \param[in]     arcType   The type of the arc being labelled.
 * \param[in,out] lines     Pointer to be filled with output line array.
 * \param[in]     label     Original arc label from input file.
 * \param[in]     startCol  Column in which the arc starts.
 * \param[in]     endCol    Column in which the arc ends, or -1 for broadcast arcs.
 *
 * \note The returned strings and array must be free()'d.  freeLabelLines() can
 *        be used for this purpose.
 */
static unsigned int computeLabelLines(Context          *ctx,
                                      Msc               m,
                                      const MscArcType  arcType,
                                      char           ***lines,
                                      const char       *label,
                                      int               startCol,
                                      int               endCol)
{
    unsigned int  width;
    unsigned int  nAllocLines = 8;
    char        **retLines = malloc_s(sizeof(char *) * nAllocLines);
    unsigned int  c = 0;

    assert(startCol >= 0 && startCol < (signed)MscGetNumEntities(m));
    assert(startCol >= -1 && startCol < (signed)MscGetNumEntities(m));

    /* Compute available width for text */
    if (isBoxArc(arcType) || ctx->opts.wordWrapArcLabels)
    {
        if (endCol == -1)
        {
            /* This is a special case for a broadcast arc */
            width = ctx->opts.entitySpacing * MscGetNumEntities(m);
        }
        else if (startCol < endCol)
        {
            width = ctx->opts.entitySpacing * (1 + (endCol - startCol));
        }
        else
        {
            width = ctx->opts.entitySpacing * (1 + (startCol - endCol));
        }

        /* Reduce the width due to the box borders */
        if (isBoxArc(arcType))
        {
            width -= (ctx->opts.boxSpacing + ctx->opts.boxInternalBorder) * 2;
        }

        if (arcType == MSC_ARC_NOTE)
        {
            width -= ctx->opts.noteCorner;
        }
    }
    else
    {
        width = UINT_MAX;
    }

    /* Split the input label into lines */
    while (label != NULL)
    {
        /* First split around user specified lines with literal '\n' */
        char *nextLine = strstr(label, "\\n");
        if (nextLine)
        {
            const int lineLen = (int)(nextLine - label);

            /* Allocate storage and duplicate the line */
            retLines[c] = malloc_s(lineLen + 1);
            memcpy(retLines[c], label, lineLen);
            retLines[c][lineLen] = '\0';

            /* Advance the label */
            label = nextLine + 2;
        }
        else
        {
            /* Duplicate the final line */
            retLines[c] = strdup_s(label);
            label = NULL;
        }

        /* Now split the line as required to wrap into the space available */
        do
        {
            /* Check if more storage maybe needed */
            if (c + 2 >= nAllocLines)
            {
                nAllocLines += 8;
                retLines = realloc_s(retLines, sizeof(char *) * nAllocLines);
            }

            retLines[c + 1] = splitStringToWidth(ctx, retLines[c], width);
            c++;
        }
        while (retLines[c] != NULL);
    }

    /* Return the array of lines and the count */
    *lines = retLines;

    return c;
}



/** Compute the output canvas size required for some MSC.
 * This computes the dimensions for the canvas as well as the height for each
 * row.
 *
 * \param[in]     m    The MSC to analyse.
 * \param[in,out] w    Pointer to be filled with the output width.
 * \param[in,out] h    Pointer to be filled with the output height.
 * \returns  An array giving the height of each row.
 */
static RowInfo *computeCanvasSize(Context      *ctx,
                                  Msc           m,
                                  unsigned int *w,
                                  unsigned int *h)
{
    const unsigned int rowCount = MscGetNumArcs(m) - MscGetNumParallelArcs(m);
    const unsigned int textHeight = ctx->drw.textHeight(&ctx->drw);
    RowInfo      *rowHeight;
    unsigned int  nextYmin, ymin, ymax, yskipmax, row;

    /* Allocate storage for the height of each row */
    rowHeight = zalloc_s(sizeof(RowInfo) * rowCount);
    row = 0;

    nextYmin = ymin = ctx->opts.entityHeadGap;
    yskipmax = 0;

    ymax  = 0;
    MscResetArcIterator(m);
    do
    {
        const MscArcType   arcType           = MscGetCurrentArcType(m);
        const int          arcGradient       = isBoxArc(arcType) ? 0 : getArcGradient(ctx, m, NULL, 0);
        char             **arcLabelLines     = NULL;
        int                startCol = -1, endCol = -1;

        if (arcType == MSC_ARC_PARALLEL)
        {
            assert(row > 0);

            row--;

            ymin = rowHeight[row].ymin;
            nextYmin = rowHeight[row].ymax;
        }
        else
        {
            /* Get the entity indices */
            if (arcType != MSC_ARC_DISCO && arcType != MSC_ARC_DIVIDER && arcType != MSC_ARC_SPACE)
            {
                startCol = MscGetEntityIndex(m, MscGetCurrentArcSource(m));
                endCol   = MscGetEntityIndex(m, MscGetCurrentArcDest(m));
            }
            else
            {
                /* Discontinuity or parallel arc spans whole chart */
                startCol = 0;
                endCol   = MscGetNumEntities(m) - 1;
            }

            /* Work out how the label fits the gap between entities */
            unsigned int arcLabelLineCount = computeLabelLines(ctx, m, arcType, &arcLabelLines,
                                                  MscGetCurrentArcAttrib(m, MSC_ATTR_LABEL),
                                                  startCol, endCol);

            assert(row < rowCount);

            /* Update the max line count for the row */
            if (arcLabelLineCount > rowHeight[row].maxTextLines)
            {
                rowHeight[row].maxTextLines = arcLabelLineCount;
            }

            freeLabelLines(arcLabelLineCount, arcLabelLines);

            /* Compute the height of this arc */
            if (arcType != MSC_ARC_DISCO && arcType != MSC_ARC_DIVIDER && arcType != MSC_ARC_SPACE)
            {
                ymax = ymin + ctx->opts.arcSpacing;
                ymax += (M_Max(rowHeight[row].maxTextLines, 2) * textHeight);
            }
            else
            {
                ymax = ymin + ctx->opts.arcSpacing;
                ymax += (M_Max(rowHeight[row].maxTextLines, 1) * textHeight);
            }

            /* Update next potential row start */
            if (ymax > nextYmin)
            {
                nextYmin = ymax;
            }

            /* Compute the dimensions for the completed row */
            rowHeight[row].ymin     = ymin;
            rowHeight[row].ymax     = nextYmin - ctx->opts.arcSpacing;
            rowHeight[row].arcliney = rowHeight[row].ymin + (rowHeight[row].ymax - rowHeight[row].ymin) / 2;
            row++;

            /* Start new row */
            ymin = nextYmin;
        }

        /* Keep a track of where the gradient may cause the graph to end */
        if (ymax + arcGradient > ymax)
        {
            yskipmax = ymax + arcGradient;
        }

    }
    while (MscNextArc(m));

    if (ymax < yskipmax)
        ymax = yskipmax;

    /* Set the return values */
    *w = MscGetNumEntities(m) * ctx->opts.entitySpacing;
    *h = ymax;

    return rowHeight;
}

/** Add a point to the output imagemap.
 * If \a ismap and \a url are non-NULL, this function will add a rectangle
 * to the imagemap according to the parameters passed.
 *
 * \param ismap  The file to which the imagemap should be rendered.
 * \param url    The URL to which the imagemap area should link.
 * \param x1     The x coordinate for the upper left point.
 * \param y2     The y coordinate for the upper left point.
 * \param x2     The x coordinate for the lower right point.
 * \param y2     The y coordinate for the lower right point.
 */
static void ismapRect(FILE        *ismap,
                      const char  *url,
                      unsigned int x1,
                      unsigned int y1,
                      unsigned int x2,
                      unsigned int y2)
{
    if(ismap && url)
    {
        assert(x1 <= x2); assert(y1 <= y2);

        fprintf(ismap,
                "rect %s %d,%d %d,%d\n",
                url,
                x1, y1,
                x2, y2);
    }
#if 0
    /* For debug render a cross onto the output */
    drw.line(&drw, x1, y1, x2, y2);
    drw.line(&drw, x2, y1, x1, y2);
#endif
}


/** Get some line from a string containing '\n' delimiters.
 * Given a string that contains literal '\n' delimiters, return a subset in
 * a passed buffer that gives the nth line.
 *
 * \param[in] string  The string to parse.
 * \param[in] line    The line number to return from the string, which should
 *                     count from 0.
 * \param[in] out     Pointer to a buffer to fill with line data.
 * \param[in] outLen  The length of the buffer pointed to by \a out, in bytes.
 * \returns  A pointer to \a out.
 */
static char *getLine(const char        *string,
                     unsigned int       line,
                     char *const        out,
                     const unsigned int outLen)
{
    const char  *lineStart, *lineEnd;
    unsigned int lineLen;

    /* Setup for the loop */
    lineEnd = NULL;
    line++;

    do
    {
        /* Check if this is the first or a repeat iteration */
        if(lineEnd)
        {
            lineStart = lineEnd + 2;
        }
        else
        {
            lineStart = string;
        }

        /* Search for next delimited */
        lineEnd = strstr(lineStart, "\\n");

        line--;
    }
    while (line > 0 && lineEnd != NULL);

    /* Determine the length of the line */
    if(lineEnd != NULL)
    {
        lineLen = (unsigned int)(lineEnd - lineStart);
    }
    else
    {
        lineLen = (unsigned int)(strlen(string) - (lineStart - string));
    }

    /* Clamp the length to the buffer */
    if(lineLen > outLen - 1)
    {
        lineLen = outLen - 1;
    }

    /* Copy desired characters */
    memcpy(out, lineStart, lineLen);

    /* NULL terminate */
    out[lineLen] = '\0';

    return out;
}


/** Render some entity text.
 * Draw the text for some entity.
 * \param  ismap       If not \a NULL, write an ismap description here.
 * \param  x           The x position at which the entity text should be centered.
 * \param  y           The y position where the text should be placed.
 * \param  entLabel    The label to render, which maybe \a NULL in which case
 *                       no output is produced.
 * \param  entUrl      The URL for rendering the label as a hyperlink.  This
 *                       maybe \a NULL if not required.
 * \param  entId       The text identifier for the arc.
 * \param  entIdUrl    The URL for rendering the test identifier as a hyperlink.
 *                       This maybe \a NULL if not required.
 * \param  entColour   The text colour name or specification for the entity text.
 *                      If NULL, use default colouring scheme.
 * \param  entBgColour The text background colour name or specification for the
 *                      entity text. If NULL, use default colouring scheme.
 */
static void entityText(Context          *ctx,
                       FILE             *ismap,
                       unsigned int      x,
                       unsigned int      y,
                       const char       *entLabel,
                       const char       *entUrl,
                       const char       *entId,
                       const char       *entIdUrl,
                       const char       *entColour,
                       const char       *entBgColour)
{
    if(entLabel)
    {
        const unsigned int lines = countLines(entLabel);
        unsigned int       l;
        char               lineBuffer[1024];

        /* Adjust y to be above the writing line */
        y -= ctx->drw.textHeight(&ctx->drw) * (lines - 1);

        for (l = 0; l < lines - 1; l++)
        {
            char         *lineLabel = getLine(entLabel, l, lineBuffer, sizeof(lineBuffer));
            unsigned int  width     = ctx->drw.textWidth(&ctx->drw, lineLabel);

            /* Push text down one line */
            y += ctx->drw.textHeight(&ctx->drw);

            /* Check if a URL is associated */
            if(entUrl)
            {
                /* If no explicit colour has been set, make URLS blue */
                ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLUE);

                /* Image map output */
                ismapRect(ismap,
                          entUrl,
                          x - (width / 2), y - ctx->drw.textHeight(&ctx->drw),
                          x + (width / 2), y);
            }

            /* Set to the explicit colours if directed */
            if(entColour != NULL)
            {
                ctx->drw.setPen(&ctx->drw, ADrawGetColour(entColour));
            }

            if(entBgColour != NULL)
            {
                ctx->drw.setBgPen(&ctx->drw, ADrawGetColour(entBgColour));
            }

            /* Render text and restore pen */
            ctx->drw.textC (&ctx->drw, x, y, lineLabel);
            ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLACK);
            ctx->drw.setBgPen(&ctx->drw, ADRAW_COL_WHITE);

            /* Render the Id of the title, if specified and for first line only */
            if(entId && l == 0)
            {
                unsigned int idwidth;
                int          idx, idy;

                idy = y - ctx->drw.textHeight(&ctx->drw);
                idx = x + (width / 2);

                ctx->drw.setFontSize(&ctx->drw, ADRAW_FONT_TINY);

                idwidth = ctx->drw.textWidth(&ctx->drw, entId);
                idy    += (ctx->drw.textHeight(&ctx->drw) + 1) / 2;

                if(entIdUrl)
                {
                    ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLUE);
                    ctx->drw.textR (&ctx->drw, idx, idy, entId);
                    ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLACK);

                    /* Image map output */
                    ismapRect(ismap,
                              entIdUrl,
                              idx, idy - ctx->drw.textHeight(&ctx->drw),
                              idx + idwidth, idy);
                }
                else
                {
                    ctx->drw.textR(&ctx->drw, idx, idy, entId);
                }

                ctx->drw.setFontSize(&ctx->drw, ADRAW_FONT_SMALL);
            }
        }
    }
}

/** Draw vertical lines stemming from entities.
 * This function will draw a single segment of the vertical line that
 * drops from an entity.
 *
 * \param m          The \a Msc for which the lines are drawn
 * \param ymin       Top of the row.
 * \param ymax       Bottom of the row.
 * \param dotted     If #TRUE, produce a dotted line, otherwise solid.
 * \param colourRefs Colour references for each entity.
 */
static void entityLines(Context           *ctx,
                        Msc                m,
                        const unsigned int ymin,
                        const unsigned int ymax,
                        Boolean            dotted,
                        const ADrawColour *colourRefs)
{
    unsigned int t;

    for (t = 0; t < MscGetNumEntities(m); t++)
    {
        unsigned int x = (ctx->opts.entitySpacing / 2) + (ctx->opts.entitySpacing * t);

        ctx->drw.setPen(&ctx->drw, colourRefs[t]);

        if(dotted)
        {
            ctx->drw.dottedLine(&ctx->drw, x, ymin, x, ymax);
        }
        else
        {
            ctx->drw.line(&ctx->drw, x, ymin, x, ymax);
        }
    }

    ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLACK);

}

/** Draw an arrow pointing to the right.
 * \param x     The x co-ordinate for the end point for the arrow head.
 * \param y     The y co-ordinate for the end point for the arrow head.
 * \param type  The arc type, which controls the format of the arrow head.
 */
static void arrowR(Context *ctx,
                   unsigned int x,
                   unsigned int y,
                   MscArcType   type)
{
    switch(type)
    {
        case MSC_ARC_SIGNAL: /* Unfilled half */
            ctx->drw.line(&ctx->drw,
                     x, y,
                     x - ctx->opts.arrowWidth, y + ctx->opts.arrowHeight);
            break;

        case MSC_ARC_DOUBLE:
        case MSC_ARC_METHOD: /* Filled */
        case MSC_ARC_RETVAL: /* Filled, dotted arc (not rendered here) */
            ctx->drw.filledTriangle(&ctx->drw,
                               x, y,
                               x - ctx->opts.arrowWidth, y + ctx->opts.arrowHeight,
                               x - ctx->opts.arrowWidth, y - ctx->opts.arrowHeight);
            break;

        case MSC_ARC_CALLBACK: /* Non-filled */
            ctx->drw.line(&ctx->drw,
                     x, y,
                     x - ctx->opts.arrowWidth, y + ctx->opts.arrowHeight);
            ctx->drw.line(&ctx->drw,
                     x - ctx->opts.arrowWidth, y - ctx->opts.arrowHeight,
                     x, y);
            break;

        default:
            assert(0);
            break;
    }
}


/** Draw an arrow pointing to the left.
 * \param x     The x co-ordinate for the end point for the arrow head.
 * \param y     The y co-ordinate for the end point for the arrow head.
 * \param type  The arc type, which controls the format of the arrow head.
 */
static void arrowL(Context *ctx,
                   unsigned int x,
                   unsigned int y,
                   MscArcType   type)
{
    switch(type)
    {
        case MSC_ARC_SIGNAL: /* Unfilled half */
            ctx->drw.line(&ctx->drw,
                     x, y,
                     x + ctx->opts.arrowWidth, y + ctx->opts.arrowHeight);
            break;

        case MSC_ARC_DOUBLE:
        case MSC_ARC_METHOD: /* Filled */
        case MSC_ARC_RETVAL: /* Filled, dotted arc (not rendered here) */
            ctx->drw.filledTriangle(&ctx->drw,
                               x, y,
                               x + ctx->opts.arrowWidth, y + ctx->opts.arrowHeight,
                               x + ctx->opts.arrowWidth, y - ctx->opts.arrowHeight);
            break;

        case MSC_ARC_CALLBACK: /* Non-filled */
            ctx->drw.line(&ctx->drw,
                     x, y,
                     x + ctx->opts.arrowWidth, y + ctx->opts.arrowHeight);
            ctx->drw.line(&ctx->drw,
                     x, y,
                     x + ctx->opts.arrowWidth, y - ctx->opts.arrowHeight);
            break;

        default:
            assert(0);
            break;
    }
}

/** Draw vertical lines and boxes stemming from entities.
 * \param ymin          Top of the row.
 * \param ymax          Bottom of the row.
 * \param boxStart      Column in which the box starts.
 * \param boxEnd        Column in which the box ends.
 * \param boxType       The type of box to draw, MSC_ARC_BOX, MSC_ARC_RBOX etc.
 * \param lineColour    Colour of the lines to use for rendering the box.
 * \param bgColour      Background colour for rendering the box.
 */
static void arcBox(Context           *ctx,
                   unsigned int       ymin,
                   unsigned int       ymax,
                   unsigned int       boxStart,
                   unsigned int       boxEnd,
                   MscArcType         boxType,
                   const char        *lineColour,
                   const char        *bgColour)
{

    /* Ensure the start is less than or equal to the end */
    if(boxStart > boxEnd)
    {
        unsigned int t = boxEnd;
        boxEnd = boxStart;
        boxStart = t;
    }

    /* Now draw the box */
    unsigned int x1 = (ctx->opts.entitySpacing * boxStart) + ctx->opts.boxSpacing;
    unsigned int x2 = ctx->opts.entitySpacing * (boxEnd + 1) - ctx->opts.boxSpacing;
    unsigned int ymid = (ymin + ymax) / 2;

    /* Set colour for the background area */
    if(bgColour != NULL)
    {
        ctx->drw.setPen(&ctx->drw, ADrawGetColour(bgColour));
    }
    else
    {
        ctx->drw.setPen(&ctx->drw, ADRAW_COL_WHITE);
    }

    /* Draw the background to overwrite the entity lines */
    switch(boxType)
    {
        case MSC_ARC_BOX:
            ctx->drw.filledRectangle(&ctx->drw, x1, ymin, x2, ymax);
            break;

        case MSC_ARC_RBOX:
            ctx->drw.filledRectangle(&ctx->drw, x1 + ctx->opts.rboxArc, ymin, x2 - ctx->opts.rboxArc, ymax);
            ctx->drw.filledRectangle(&ctx->drw, x1, ymin + ctx->opts.rboxArc, x2, ymax - ctx->opts.rboxArc);
            ctx->drw.filledCircle(&ctx->drw, x1 + ctx->opts.rboxArc, ymin + ctx->opts.rboxArc, ctx->opts.rboxArc);
            ctx->drw.filledCircle(&ctx->drw, x2 - ctx->opts.rboxArc, ymin + ctx->opts.rboxArc, ctx->opts.rboxArc);
            ctx->drw.filledCircle(&ctx->drw, x1 + ctx->opts.rboxArc, ymax - ctx->opts.rboxArc, ctx->opts.rboxArc);
            ctx->drw.filledCircle(&ctx->drw, x2 - ctx->opts.rboxArc, ymax - ctx->opts.rboxArc, ctx->opts.rboxArc);
            break;

        case MSC_ARC_NOTE:
            ctx->drw.filledRectangle(&ctx->drw, x1, ymin, x2 - ctx->opts.noteCorner, ymax);
            ctx->drw.filledRectangle(&ctx->drw, x1, ymin + ctx->opts.noteCorner, x2, ymax);
            ctx->drw.filledTriangle(&ctx->drw, x2 - ctx->opts.noteCorner, ymin,
                                     x2, ymin + ctx->opts.noteCorner,
                                     x2 - ctx->opts.noteCorner, ymin + ctx->opts.noteCorner);
            break;

        case MSC_ARC_ABOX:
            ctx->drw.filledRectangle(&ctx->drw, x1 + ctx->opts.aboxSlope, ymin, x2 - ctx->opts.aboxSlope, ymax);
            ctx->drw.filledTriangle(&ctx->drw, x1 + ctx->opts.aboxSlope, ymin,
                                     x1 + ctx->opts.aboxSlope, ymax,
                                     x1, ymid);
            ctx->drw.filledTriangle(&ctx->drw, x2 - ctx->opts.aboxSlope, ymin,
                                     x2 - ctx->opts.aboxSlope, ymax,
                                     x2, ymid);
            break;

        default:
            assert(0);
    }

    /* Setup the colour for rendering the boxes */
    if(lineColour)
    {
        ctx->drw.setPen(&ctx->drw, ADrawGetColour(lineColour));
    }
    else
    {
        ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLACK);
    }

    /* Draw the outline */
    switch(boxType)
    {
        case MSC_ARC_BOX:
            ctx->drw.line(&ctx->drw, x1, ymin, x2, ymin);
            ctx->drw.line(&ctx->drw, x1, ymax, x2, ymax);
            ctx->drw.line(&ctx->drw, x1, ymin, x1, ymax);
            ctx->drw.line(&ctx->drw, x2, ymin, x2, ymax);
            break;

        case MSC_ARC_NOTE:
            ctx->drw.line(&ctx->drw, x1, ymin, x2 - ctx->opts.noteCorner, ymin);
            ctx->drw.line(&ctx->drw, x1, ymax, x2, ymax);
            ctx->drw.line(&ctx->drw, x1, ymin, x1, ymax);
            ctx->drw.line(&ctx->drw, x2, ymin + ctx->opts.noteCorner, x2, ymax);
            ctx->drw.line(&ctx->drw, x2 - ctx->opts.noteCorner, ymin,
                           x2, ymin + ctx->opts.noteCorner);
            ctx->drw.line(&ctx->drw, x2 - ctx->opts.noteCorner, ymin,
                           x2 - ctx->opts.noteCorner, ymin + ctx->opts.noteCorner);
            ctx->drw.line(&ctx->drw, x2, ymin + ctx->opts.noteCorner,
                           x2 - ctx->opts.noteCorner, ymin + ctx->opts.noteCorner);
            break;

        case MSC_ARC_RBOX:
            ctx->drw.line(&ctx->drw, x1 + ctx->opts.rboxArc, ymin, x2 - ctx->opts.rboxArc, ymin);
            ctx->drw.line(&ctx->drw, x1 + ctx->opts.rboxArc, ymax, x2 - ctx->opts.rboxArc, ymax);
            ctx->drw.line(&ctx->drw, x1, ymin + ctx->opts.rboxArc, x1, ymax - ctx->opts.rboxArc);
            ctx->drw.line(&ctx->drw, x2, ymin + ctx->opts.rboxArc, x2, ymax - ctx->opts.rboxArc);

            ctx->drw.arc(&ctx->drw, x1 + ctx->opts.rboxArc,
                    ymin + ctx->opts.rboxArc, ctx->opts.rboxArc * 2, ctx->opts.rboxArc * 2,
                    180, 270);
            ctx->drw.arc(&ctx->drw, x2 - ctx->opts.rboxArc,
                    ymin + ctx->opts.rboxArc, ctx->opts.rboxArc * 2, ctx->opts.rboxArc * 2,
                    270, 0);
            ctx->drw.arc(&ctx->drw, x2 - ctx->opts.rboxArc,
                    ymax - ctx->opts.rboxArc, ctx->opts.rboxArc * 2, ctx->opts.rboxArc * 2,
                    0, 90);
            ctx->drw.arc(&ctx->drw, x1 + ctx->opts.rboxArc,
                    ymax - ctx->opts.rboxArc, ctx->opts.rboxArc * 2, ctx->opts.rboxArc * 2,
                    90, 180);
            break;

        case MSC_ARC_ABOX:
            ctx->drw.line(&ctx->drw, x1 + ctx->opts.aboxSlope, ymin, x2 - ctx->opts.aboxSlope, ymin);
            ctx->drw.line(&ctx->drw, x1 + ctx->opts.aboxSlope, ymax, x2 - ctx->opts.aboxSlope, ymax);
            ctx->drw.line(&ctx->drw, x1 + ctx->opts.aboxSlope, ymin, x1, ymid);
            ctx->drw.line(&ctx->drw, x1, ymid, x1 + ctx->opts.aboxSlope, ymax);
            ctx->drw.line(&ctx->drw, x2 - ctx->opts.aboxSlope, ymin, x2, ymid);
            ctx->drw.line(&ctx->drw, x2, ymid, x2 - ctx->opts.aboxSlope, ymax);
            break;

        default:
            assert(0);
    }

    /* Restore the pen colour if needed */
    if(lineColour)
    {
        ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLACK);
    }
}


/** Render text on an arc.
 * Draw the text on some arc.
 * \param m              The Msc for which the text is being rendered.
 * \param ismap          If not \a NULL, write an ismap description here.
 * \param outwidth       Width of the output image.
 * \param ymid           Co-ordinate of the row on which the text should be aligned.
 * \param startCol       The column at which the arc being labelled starts.
 * \param endCol         The column at which the arc being labelled ends.
 * \param arcLabelLineCount  Count of lines of text in arcLabelLines.
 * \param arcLabelLines  Array of lines of text from 0 to arcLabelLineCount - 1.
 * \param arcUrl         The URL for rendering the label as a hyperlink.  This
 *                        maybe \a NULL if not required.
 * \param arcId          The text identifier for the arc.
 * \param arcIdUrl       The URL for rendering the test identifier as a hyperlink.
 *                        This maybe \a NULL if not required.
 * \param arcTextColour  Colour for the arc text, or NULL to use default.
 * \param arcTextColour  Colour for the arc text background, or NULL to use default.
 * \param arcType        The type of arc, used to control output semantics.
 */
static void arcText(Context           *ctx,
                    Msc                m,
                    FILE              *ismap,
                    unsigned int       outwidth,
                    unsigned int       ymid,
                    int                ygradient,
                    unsigned int       startCol,
                    unsigned int       endCol,
                    const unsigned int arcLabelLineCount,
                    char             **arcLabelLines,
                    const char        *arcUrl,
                    const char        *arcId,
                    const char        *arcIdUrl,
                    const char        *arcTextColour,
                    const char        *arcTextBgColour,
                    const MscArcType   arcType)
{
    unsigned int l;
    unsigned int y;

    /* A single line of normal text is above the midline */
    if(arcLabelLineCount == 1 && !isBoxArc(arcType) &&
       arcType != MSC_ARC_DISCO && arcType != MSC_ARC_DIVIDER &&
       arcType != MSC_ARC_SPACE)
    {
        y = ymid + (ygradient / 2) - ctx->drw.textHeight(&ctx->drw);
    }
    else /* Text is vertically centered on the midline */
    {
        int yoff = ygradient - (ctx->drw.textHeight(&ctx->drw) * arcLabelLineCount);
        y = ymid + (yoff / 2);
    }

    for (l = 0; l < arcLabelLineCount; l++)
    {
        const char *lineLabel = arcLabelLines[l];
        unsigned int width = ctx->drw.textWidth(&ctx->drw, lineLabel);
        int x = ((startCol + endCol + 1) * ctx->opts.entitySpacing) / 2;

        y += ctx->drw.textHeight(&ctx->drw);

        if(startCol != endCol || isBoxArc(arcType))
        {
            /* Produce central aligned text */
            x -= width / 2;
        }
        else if(startCol < (MscGetNumEntities(m) / 2))
        {
            /* Form text to the right */
            x += ctx->opts.textHGapPre;
        }
        else
        {
            /* Form text to the left */
            x -= width + ctx->opts.textHGapPost;
        }

        /* Clip against edges of image */
        if(x + width > outwidth)
        {
            x = outwidth - width;
        }

        if(x < 0)
        {
            x = 0;
        }

        /* Check if a URL is associated */
        if(arcUrl)
        {
            /* Default to blue */
            ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLUE);

            /* Image map output */
            ismapRect(ismap,
                      arcUrl,
                      x, y - ctx->drw.textHeight(&ctx->drw),
                      x + width, y);
        }


        /* Set to the explicit colours if directed */
        if(arcTextColour != NULL)
        {
            ctx->drw.setPen(&ctx->drw, ADrawGetColour(arcTextColour));
        }

        if(arcTextBgColour != NULL)
        {
            ctx->drw.setBgPen(&ctx->drw, ADrawGetColour(arcTextBgColour));
        }

        /* Render text and restore pen */
        ctx->drw.textR (&ctx->drw, x, y, lineLabel);
        ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLACK);
        ctx->drw.setBgPen(&ctx->drw, ADRAW_COL_WHITE);

        /* Render the Id of the arc, if specified and for the first line*/
        if(arcId && l == 0)
        {
            unsigned int idwidth;
            int          idx, idy;

            idy = y - ctx->drw.textHeight(&ctx->drw);
            idx = x + width;

            ctx->drw.setFontSize(&ctx->drw, ADRAW_FONT_TINY);

            idwidth = ctx->drw.textWidth(&ctx->drw, arcId);
            idy    += (ctx->drw.textHeight(&ctx->drw) + 1) / 2;

            if(arcIdUrl)
            {
                ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLUE);

                /* Image map output */
                ismapRect(ismap,
                          arcIdUrl,
                          idx, idy - ctx->drw.textHeight(&ctx->drw),
                          idx + idwidth, idy);
            }

            /* Render text and restore pen and font */
            ctx->drw.textR (&ctx->drw, idx, idy, arcId);
            ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLACK);
            ctx->drw.setFontSize(&ctx->drw, ADRAW_FONT_SMALL);
        }
    }
}


/** Render the line and arrow head for some arc.
 * This will draw the arc line and arrow head between two columns,
 * noting that if the start and end column are the same, an arc is
 * rendered.
 * \param  m           The Msc for which the text is being rendered.
 * \param  ymin        Top of row.
 * \param  ymax        Bottom of row.
 * \param  ygradient   The gradient of the arc which alters the y position a
 *                      the ending column.
 * \param  startCol    Starting column for the arc.
 * \param  endCol      Column at which the arc terminates.
 * \param  hasArrows   If true, draw arc arrows, otherwise omit them.
 * \param  hasBiArrows If true, has arrows in both directions.
 * \param  arcType     The type of the arc, which dictates its rendered style.
 */
static void arcLine(Context          *ctx,
                    Msc               m,
                    unsigned int      y,
                    unsigned int      ygradient,
                    unsigned int      startCol,
                    unsigned int      endCol,
                    const char       *arcLineCol,
                    Boolean           hasArrows,
                    const int         hasBiArrows,
                    const MscArcType  arcType)
{
    const unsigned int sx = (startCol * ctx->opts.entitySpacing) +
                             (ctx->opts.entitySpacing / 2);
    const unsigned int dx = (endCol * ctx->opts.entitySpacing) +
                             (ctx->opts.entitySpacing / 2);

    /* Check if an explicit line colour is requested */
    if(arcLineCol != NULL)
    {
        ctx->drw.setPen(&ctx->drw, ADrawGetColour(arcLineCol));
    }

    if(startCol != endCol)
    {
        /* Draw the line */
        if(arcType == MSC_ARC_RETVAL)
        {
            ctx->drw.dottedLine(&ctx->drw, sx, y, dx, y + ygradient);
        }
        else if(arcType == MSC_ARC_DOUBLE)
        {
            ctx->drw.line(&ctx->drw, sx, y - 1, dx, y - 1 + ygradient);
            ctx->drw.line(&ctx->drw, sx, y + 1, dx, y + 1 + ygradient);
        }
        else if(arcType == MSC_ARC_LOSS)
        {
            signed int   span = dx - sx;
            unsigned int mx = sx + (span / 4) * 3;

            ctx->drw.line(&ctx->drw, sx, y, mx, y + ygradient);
            hasArrows = 0;

            ctx->drw.line(&ctx->drw, mx - 4, y + ygradient - 4, mx + 4, y + ygradient + 4);
            ctx->drw.line(&ctx->drw, mx + 4, y + ygradient - 4, mx - 4, y + ygradient + 4);
        }
        else
        {
            ctx->drw.line(&ctx->drw, sx, y, dx, y + ygradient);
        }

        /* Now the arrow heads */
        if(hasArrows)
        {
            if(startCol < endCol)
            {
                arrowR(ctx, dx, y + ygradient, arcType);
            }
            else
            {
                arrowL(ctx, dx, y + ygradient, arcType);
            }

            if(hasBiArrows)
            {
                if(startCol < endCol)
                {
                    arrowL(ctx, sx, y + ygradient, arcType);
                }
                else
                {
                    arrowR(ctx, sx, y + ygradient, arcType);
                }
            }
        }
    }
    else if(startCol < (MscGetNumEntities(m) / 2))
    {
        /* Arc looping to the left */
        if(arcType == MSC_ARC_RETVAL)
        {
            ctx->drw.dottedArc(&ctx->drw,
                          sx, y,
                          ctx->opts.entitySpacing,
                          ctx->opts.loopArcHeight,
                          90,
                          270);
        }
        else if(arcType == MSC_ARC_DOUBLE)
        {
            ctx->drw.arc(&ctx->drw,
                    sx, y - 1,
                    ctx->opts.entitySpacing,
                    ctx->opts.loopArcHeight,
                    90,
                    270);
            ctx->drw.arc(&ctx->drw,
                    sx, y + 1,
                    ctx->opts.entitySpacing,
                    ctx->opts.loopArcHeight,
                    90,
                    270);
        }
        else if(arcType == MSC_ARC_LOSS)
        {
            unsigned int px, py;

            ctx->drw.arc(&ctx->drw,
                    sx, y - 1,
                    ctx->opts.entitySpacing - 8,
                    ctx->opts.loopArcHeight,
                    180 - 45,
                    270);

            hasArrows = FALSE;

            /* Get co-ordinates of the arc end-point */
            ADrawComputeArcPoint((float)sx, (float)(y - 1), (float)(ctx->opts.entitySpacing - 8),
                                 (float)ctx->opts.loopArcHeight, 180.0f - 45.0f,
                                 &px, &py);

            /* Draw a cross */
            ctx->drw.line(&ctx->drw, px - 4, py - 4, px + 4, py + 4);
            ctx->drw.line(&ctx->drw, px + 4, py - 4, px - 4, py + 4);
        }
        else
        {
            ctx->drw.arc(&ctx->drw,
                    sx, y,
                    ctx->opts.entitySpacing - 4,
                    ctx->opts.loopArcHeight,
                    90,
                    270);
        }

        if(hasArrows)
        {
            arrowR(ctx, dx, y + (ctx->opts.loopArcHeight / 2), arcType);
        }
    }
    else
    {
        /* Arc looping to right */
        if(arcType == MSC_ARC_RETVAL)
        {
            ctx->drw.dottedArc(&ctx->drw,
                          sx, y,
                          ctx->opts.entitySpacing,
                          ctx->opts.loopArcHeight,
                          270,
                          90);
        }
        else if(arcType == MSC_ARC_DOUBLE)
        {
            ctx->drw.arc(&ctx->drw,
                    sx, y - 1,
                    ctx->opts.entitySpacing,
                    ctx->opts.loopArcHeight,
                    270,
                    90);
            ctx->drw.arc(&ctx->drw,
                    sx, y + 1,
                    ctx->opts.entitySpacing,
                    ctx->opts.loopArcHeight,
                    270,
                    90);
        }
        else if(arcType == MSC_ARC_LOSS)
        {
            unsigned int px, py;

            ctx->drw.arc(&ctx->drw,
                    sx, y - 1,
                    ctx->opts.entitySpacing - 8,
                    ctx->opts.loopArcHeight,
                    270,
                    45);

            hasArrows = FALSE;

            /* Get co-ordinates of the arc end-point */
            ADrawComputeArcPoint((float)sx, (float)(y - 1),
                                 (float)(ctx->opts.entitySpacing - 8),
                                 (float)ctx->opts.loopArcHeight, 45.0f,
                                 &px, &py);

            /* Draw a cross */
            ctx->drw.line(&ctx->drw, px - 4, py - 4, px + 4, py + 4);
            ctx->drw.line(&ctx->drw, px + 4, py - 4, px - 4, py + 4);
        }
        else
        {
            ctx->drw.arc(&ctx->drw,
                    sx, y,
                    ctx->opts.entitySpacing,
                    ctx->opts.loopArcHeight,
                    270,
                    90);
        }

        if(hasArrows)
        {
            arrowL(ctx, dx, y + (ctx->opts.loopArcHeight / 2), arcType);
        }
    }

    /* Restore pen if needed */
    if(arcLineCol != NULL)
    {
        ctx->drw.setPen(&ctx->drw, ADRAW_COL_BLACK);
    }
}



/* Perform post-parsing validation of the MSC.
 *  This checks the passed MSC for various rules which can't easily be tested
 *  at parse time.
 */
static Boolean checkMsc(Msc m)
{
    /* Check all arc entities are known */
    MscResetArcIterator(m);
    do
    {
        const MscArcType arcType  = MscGetCurrentArcType(m);

        if (arcType != MSC_ARC_PARALLEL && arcType != MSC_ARC_DISCO &&
            arcType != MSC_ARC_DIVIDER  && arcType != MSC_ARC_SPACE)
        {
            const char *src      = MscGetCurrentArcSource(m);
            const char *dst      = MscGetCurrentArcDest(m);
            const int   startCol = MscGetEntityIndex(m, src);
            const int   endCol   = MscGetEntityIndex(m, dst);

            /* Check the start column is valid */
            if (startCol == -1)
            {
                fprintf(stderr, "Error detected at line %u: Unknown source entity '%s'.\n",
                        MscGetCurrentArcInputLine(m), src);
                return FALSE;
            }

            if (endCol == -1 && !isBroadcastArc(dst))
            {
                fprintf(stderr, "Error detected at line %u: Unknown destination entity '%s'.\n",
                        MscGetCurrentArcInputLine(m), dst);
                return FALSE;
            }
        }
    }
    while (MscNextArc(m));

    return TRUE;
}


int mscgen_generate(const char *inputFile,
                    const char *outputFile,
                    mscgen_format_t format
                   )
{
  //printf("mscgen_generate(in=%s,out=%s,format=%d)\n",inputFile,outputFile,format);
  ADrawOutputType  outType;
  ADrawColour     *entColourRef = NULL;
  const char      *outImage = outputFile;
  Msc              m;
  float            f;
  unsigned int     w, h, row, col;
  RowInfo         *rowInfo = NULL;
  Boolean          addLines;
  FILE            *ismap = NULL;
#if defined(_WIN32) && !defined(__CYGWIN__)
  const char      *nullFile = "nul";
#else
  const char      *nullFile = "/dev/null";
#endif
  Context ctx;
  ctx.opts = gDefaultOpts;

  switch (format)
  {
    case mscgen_format_png:
      outType = ADRAW_FMT_PNG;
      break;
    case mscgen_format_eps:
      outType = ADRAW_FMT_EPS;
      break;
    case mscgen_format_svg:
      outType = ADRAW_FMT_SVG;
      break;
    case mscgen_format_pngmap:
      outType = ADRAW_FMT_PNG;
      outImage = nullFile;
      break;
    case mscgen_format_svgmap:
      outType = ADRAW_FMT_SVG;
      outImage = nullFile;
      break;
  }
  /* open file */
  FILE *in = fopen(inputFile, "r");
  if (!in)
  {
    fprintf(stderr, "Failed to open input file '%s'\n", inputFile);
    return MSCGEN_FILE_ERROR;
  }
  /* parse file */
  m = MscParse(in);
  fclose(in);
  if (!m || !checkMsc(m))
  {
    fprintf(stderr, "Input format error for '%s'\n", inputFile);
    return MSCGEN_INPUT_FORMAT_ERROR;
  }

  /* Check if an ismap file should also be generated */
  if (format==mscgen_format_pngmap || format==mscgen_format_svgmap)
  {
      ismap = fopen(outputFile, "w");
      if (!ismap)
      {
        fprintf(stderr, "Failed to open output file '%s': %s\n", outputFile, strerror(errno));
        MscFree(m);
        return MSCGEN_FILE_ERROR;
      }
  }

  /* Open the drawing context with dummy dimensions */
  if (!ADrawOpen(10, 10, nullFile, "", outType, &ctx.drw))
  {
    if (ismap)
    {
      fclose(ismap);
    }
    MscFree(m);
    fprintf(stderr, "Failed to open temporary drawing context\n");
    return MSCGEN_OUTPUT_CONTEXT_ERROR;
  }

  /* Now compute ideal canvas size, which may use text metrics */
  if (MscGetOptAsFloat(m, MSC_OPT_WIDTH, &f))
  {
     ctx.opts.idealCanvasWidth = (unsigned int)f;
  }
  else if (MscGetOptAsFloat(m, MSC_OPT_HSCALE, &f))
  {
     ctx.opts.idealCanvasWidth *= (unsigned int)f;
  }

  /* Set the arc gradient if needed */
  if (MscGetOptAsFloat(m, MSC_OPT_ARCGRADIENT, &f))
  {
     ctx.opts.arcGradient = (int)f;
     ctx.opts.arcSpacing += ctx.opts.arcGradient;
  }

  /* Check if word wrapping on arcs other than boxes should be used */
  MscGetOptAsBoolean(m, MSC_OPT_WORDWRAPARCS, &ctx.opts.wordWrapArcLabels);

  /* Work out the entitySpacing */
  if (ctx.opts.idealCanvasWidth / MscGetNumEntities(m) > ctx.opts.entitySpacing)
  {
    ctx.opts.entitySpacing = ctx.opts.idealCanvasWidth / MscGetNumEntities(m);
  }

  /* Work out the entityHeadGap */
  MscResetEntityIterator(m);
  for (col = 0; col < MscGetNumEntities(m); col++)
  {
    unsigned int lines = countLines(MscGetCurrentEntAttrib(m, MSC_ATTR_LABEL));
    unsigned int gap;

    /* Get the required gap */
    gap = lines * ctx.drw.textHeight(&ctx.drw);
    if (gap > ctx.opts.entityHeadGap)
    {
      ctx.opts.entityHeadGap = gap;
    }

    MscNextEntity(m);
  }

  //printf("opts: idealCanvasWidth=%d entitySpacing=%d entityHeadGap=%d "
  //             "arcSpacing=%d arcGradient=%d boxSpacing=%d boxInternalBorder=%d "
  //             "rboxArc=%d noteCorner=%d aboxSlope=%d wordWrapArcLabels=%d arrowWidth=%d "
  //             "arrowHeight=%d loopArcHeight=%d textHGapPre=%d textHGapPost=%d\n",
  //              ctx.opts.idealCanvasWidth,ctx.opts.entitySpacing,ctx.opts.entityHeadGap,
  //              ctx.opts.arcSpacing,ctx.opts.arcGradient,ctx.opts.boxSpacing,ctx.opts.boxInternalBorder,
  //              ctx.opts.rboxArc,ctx.opts.noteCorner,ctx.opts.aboxSlope,ctx.opts.wordWrapArcLabels,ctx.opts.arrowWidth,
  //              ctx.opts.arrowHeight,ctx.opts.loopArcHeight,ctx.opts.textHGapPre,ctx.opts.textHGapPost);

  /* Work out the width and height of the canvas */
  rowInfo = computeCanvasSize(&ctx, m, &w , &h);

  /* Close the temporary output file */
  ctx.drw.close(&ctx.drw);

  //printf("opening canvas %d x %d for %s type=%d\n",w,h,outImage,outType);

  /* Open the output */
  if (!ADrawOpen(w, h, outImage, "", outType, &ctx.drw))
  {
    free(rowInfo);
    if (ismap)
    {
      fclose(ismap);
    }
    MscFree(m);
    return MSCGEN_OUTPUT_CONTEXT_ERROR;
  }

  /* Allocate storage for entity heading colours */
  entColourRef = malloc_s(MscGetNumEntities(m) * sizeof(ADrawColour));

  /* Draw the entity headings */
  MscResetEntityIterator(m);
  for (col = 0; col < MscGetNumEntities(m); col++)
  {
    unsigned int x = (ctx.opts.entitySpacing / 2) + (ctx.opts.entitySpacing * col);
    const char  *line;

    /* Titles */
    entityText(&ctx, ismap,
        x,
        ctx.opts.entityHeadGap - (ctx.drw.textHeight(&ctx.drw) / 2),
        MscGetCurrentEntAttrib(m, MSC_ATTR_LABEL),
        MscGetCurrentEntAttrib(m, MSC_ATTR_URL),
        MscGetCurrentEntAttrib(m, MSC_ATTR_ID),
        MscGetCurrentEntAttrib(m, MSC_ATTR_IDURL),
        MscGetCurrentEntAttrib(m, MSC_ATTR_TEXT_COLOUR),
        MscGetCurrentEntAttrib(m, MSC_ATTR_TEXT_BGCOLOUR));

    /* Get the colours */
    line = MscGetCurrentEntAttrib(m, MSC_ATTR_LINE_COLOUR);
    if (line != NULL)
    {
      entColourRef[col] = ADrawGetColour(line);
    }
    else
    {
      entColourRef[col] = ADRAW_COL_BLACK;
    }

    MscNextEntity(m);
  }

  /* Draw the arcs */
  addLines = TRUE;
  row = 0;

  MscResetArcIterator(m);
  do
  {
    const MscArcType   arcType           = MscGetCurrentArcType(m);
    const char        *arcUrl            = MscGetCurrentArcAttrib(m, MSC_ATTR_URL);
    const char        *arcId             = MscGetCurrentArcAttrib(m, MSC_ATTR_ID);
    const char        *arcIdUrl          = MscGetCurrentArcAttrib(m, MSC_ATTR_IDURL);
    const char        *arcTextColour     = MscGetCurrentArcAttrib(m, MSC_ATTR_TEXT_COLOUR);
    const char        *arcTextBgColour   = MscGetCurrentArcAttrib(m, MSC_ATTR_TEXT_BGCOLOUR);
    const char        *arcLineColour     = MscGetCurrentArcAttrib(m, MSC_ATTR_LINE_COLOUR);
    const int          arcGradient       = isBoxArc(arcType) ? 0 : getArcGradient(&ctx, m, rowInfo, row);
    const int          arcHasArrows      = MscGetCurrentArcAttrib(m, MSC_ATTR_NO_ARROWS) == NULL;
    const int          arcHasBiArrows    = MscGetCurrentArcAttrib(m, MSC_ATTR_BI_ARROWS) != NULL;
    char             **arcLabelLines     = NULL;
    int                startCol = -1, endCol = -1;

    if (arcType == MSC_ARC_PARALLEL)
    {
      addLines = FALSE;

      /* Rewind the row */
      assert(row > 0);
      row--;
    }
    else
    {
      const unsigned int ymin = rowInfo[row].ymin;
      const unsigned int ymid = rowInfo[row].arcliney;
      const unsigned int ymax = rowInfo[row].ymax;

#if 0
      /* For debug, mark the row spacing */
      ctx.drw.line(&ctx.drw, 0, ymin, 10, ymin);
      ctx.drw.line(&ctx.drw, 0, ymid, 5, ymid);
      ctx.drw.line(&ctx.drw, 0, ymax, 10, ymax);
#endif
      /* Get the entity indices */
      if (arcType != MSC_ARC_DISCO && arcType != MSC_ARC_DIVIDER && arcType != MSC_ARC_SPACE)
      {
        startCol = MscGetEntityIndex(m, MscGetCurrentArcSource(m));
        endCol   = MscGetEntityIndex(m, MscGetCurrentArcDest(m));

        /* Check that the start column is known and the end column is
         *  known, or that it's a broadcast arc
         */
        assert(startCol != -1);
        assert(endCol != -1 || isBroadcastArc(MscGetCurrentArcDest(m)));

        /* Check for entity colouring if not set explicitly on the arc */
        if (arcTextColour == NULL)
        {
          arcTextColour = MscGetEntAttrib(m, startCol, MSC_ATTR_ARC_TEXT_COLOUR);
        }

        if (arcTextBgColour == NULL)
        {
          arcTextBgColour = MscGetEntAttrib(m, startCol, MSC_ATTR_ARC_TEXT_BGCOLOUR);
        }

        if (arcLineColour == NULL)
        {
          arcLineColour = MscGetEntAttrib(m, startCol, MSC_ATTR_ARC_LINE_COLOUR);
        }

      }
      else
      {
        /* Discontinuity or parallel arc spans whole chart */
        startCol = 0;
        endCol   = MscGetNumEntities(m) - 1;
      }

      /* Work out how the label fits the gap between entities */
      unsigned int arcLabelLineCount = computeLabelLines(&ctx, m, arcType, &arcLabelLines,
          MscGetCurrentArcAttrib(m, MSC_ATTR_LABEL),
          startCol, endCol);

      /* Check if this is a broadcast message */
      if (isBroadcastArc(MscGetCurrentArcDest(m)))
      {
        unsigned int t;

        /* Add in the entity lines */
        if (addLines)
        {
          entityLines(&ctx, m, ymin, ymax + ctx.opts.arcSpacing, FALSE, entColourRef);
        }

        /* Draw arcs to each entity */
        for (t = 0; t < MscGetNumEntities(m); t++)
        {
          if ((signed)t != startCol)
          {
            arcLine(&ctx, m, ymid, arcGradient, startCol,
                t, arcLineColour, arcHasArrows,
                arcHasBiArrows, arcType);
          }
        }

        /* Fix up the start/end columns to span chart */
        startCol = 0;
        endCol   = MscGetNumEntities(m) - 1;
      }
      else
      {
        /* Check if it is a box, discontinuity arc etc... */
        if (isBoxArc(arcType))
        {
          if (addLines)
          {
            entityLines(&ctx, m, ymin, ymax + ctx.opts.arcSpacing, FALSE, entColourRef);
          }
          arcBox(&ctx, ymin, ymax, startCol, endCol, arcType, arcLineColour, arcTextBgColour);
        }
        else if (arcType == MSC_ARC_DISCO)
        {
          if (addLines)
          {
            entityLines(&ctx, m, ymin, ymax + ctx.opts.arcSpacing, TRUE /* dotted */, entColourRef);
          }
        }
        else if (arcType == MSC_ARC_DIVIDER || arcType == MSC_ARC_SPACE)
        {
          if (addLines)
          {
            entityLines(&ctx, m, ymin, ymax + ctx.opts.arcSpacing, FALSE, entColourRef);
          }

          /* Dividers also have a horizontal line at the middle */
          if (arcType == MSC_ARC_DIVIDER)
          {
            const unsigned int margin = ctx.opts.entitySpacing / 4;

            if (arcLineColour != NULL)
            {
              ctx.drw.setPen(&ctx.drw, ADrawGetColour(arcLineColour));
            }

            /* Draw line through middle of text */
            ctx.drw.dottedLine(&ctx.drw,
                margin, ymid,
                (MscGetNumEntities(m) * ctx.opts.entitySpacing) - margin,  ymid);

            if (arcLineColour != NULL)
            {
              ctx.drw.setPen(&ctx.drw, ADRAW_COL_BLACK);
            }
          }
        }
        else
        {
          if (addLines)
          {
            entityLines(&ctx, m, ymin, ymax + ctx.opts.arcSpacing, FALSE, entColourRef);
          }
          arcLine(&ctx, m, ymid, arcGradient, startCol, endCol, arcLineColour,
              arcHasArrows, arcHasBiArrows, arcType);
        }
      }

      /* All may have text */
      if (arcLabelLineCount > 0)
      {
        arcText(&ctx, m, ismap, w, ymid, arcGradient,
            startCol, endCol,
            arcLabelLineCount, arcLabelLines,
            arcUrl, arcId, arcIdUrl,
            arcTextColour, arcTextBgColour, arcType);
      }

      freeLabelLines(arcLabelLineCount, arcLabelLines);

      /* Advance the row */
      row++;
      addLines = TRUE;
    }
  } while (MscNextArc(m));

  /* Skip arcs may require the entity lines to be extended */
  entityLines(&ctx, m,
      rowInfo[(MscGetNumArcs(m) - MscGetNumParallelArcs(m)) - 1].ymax,
      h, FALSE, entColourRef);

  if (ismap)
  {
    fclose(ismap);
  }

  free(entColourRef);
  free(rowInfo);
  MscFree(m);
  /* Close the context */
  ctx.drw.close(&ctx.drw);

  return MSCGEN_OK;
}


const char *mscgen_error2str(int code)
{
  switch (code)
  {
    case MSCGEN_OK: return "OK";
    case MSCGEN_FILE_ERROR: return "FILE ERROR";
    case MSCGEN_INPUT_FORMAT_ERROR: return "INPUT FORMAT ERROR";
    case MSCGEN_OUTPUT_CONTEXT_ERROR: return "OUTPUT CONTEXT ERROR";
    default: return "UNKNOWN_ERROR";
  }
}

