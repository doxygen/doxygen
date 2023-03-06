/***************************************************************************
 *
 * $Id: adraw.h 161 2010-10-26 20:17:16Z Michael.McTernan $
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

#ifndef MSCGEN_ADRAW_H
#define MSCGEN_ADRAW_H

#include "mscgen_bool.h"

/***************************************************************************
 * Types
 ***************************************************************************/

/** Output format types.
 * Enumerated types for output formats.
 */
typedef enum
{
    /** Null output format.
     * This allows all the graphics commands to be called, but does nothing.
     */
    ADRAW_FMT_NULL = 0,

    /** Generate a PNG. */
    ADRAW_FMT_PNG,

    /** Generate Encapsulated Postscript. */
    ADRAW_FMT_EPS,

    /** Scalable Vector Graphics. */
    ADRAW_FMT_SVG
}
ADrawOutputType;


/** Supported colours.
 * Enumerated type to describe colours.
 *
 * The 16 basic HTML colours at http://www.w3.org/TR/html4/types.html#h-6.5
 * are defined as well as a few other common colour names.
 */
typedef enum
{
    ADRAW_COL_WHITE   = 0x00ffffff,
    ADRAW_COL_BLACK   = 0x00000000,
    ADRAW_COL_RED     = 0x00ff0000,
    ADRAW_COL_ORANGE  = 0x00ffb000,
    ADRAW_COL_YELLOW  = 0x00ffff00,
    ADRAW_COL_GREEN   = 0x0000ff00,
    ADRAW_COL_BLUE    = 0x000000ff,
    ADRAW_COL_INDIGO  = 0x00440088,
    ADRAW_COL_VIOLET  = 0x00d02090,
    ADRAW_COL_SILVER  = 0x00C0C0C0,
    ADRAW_COL_LIME    = 0x0000FF00,
    ADRAW_COL_GRAY    = 0x00808080,
    ADRAW_COL_OLIVE   = 0x00808000,
    ADRAW_COL_MAROON  = 0x00800000,
    ADRAW_COL_NAVY    = 0x00000080,
    ADRAW_COL_PURPLE  = 0x00800080,
    ADRAW_COL_TEAL    = 0x00008080,
    ADRAW_COL_FUCHSIA = 0x00FF00FF,
    ADRAW_COL_AQUA    = 0x0000FFFF,

    ADRAW_COL_INVALID = 0xff000000,
}
ADrawColour;


/** Basic font sizes.
 * Enumerated type for different font sizes.
 */
typedef enum
{
    ADRAW_FONT_TINY = 0,
    ADRAW_FONT_SMALL
}
ADrawFontSize;


/** An ADraw context.
 * This is the main structure used for accessing ADraw functions.
 * ADrawOpen() returns an instance of this structure that can then be used
 * to render to some device.  Once drawing is complete, ADrawClose() should
 * be called to reclaim resources and finalise any output.
 *
 * All the functions assume that 0,0 is in the top-left corner of the image,
 * and that the dimensions grow towards w,y at the bottom-right corner.
 */
typedef struct ADrawTag
{
    /** Draw a line.
     * Draw a solid straight line between two points.
     * \param ctx    The drawing context.
     * \param x1     The first x co-ordinate,
     * \param y1     The first y co-ordinate,
     * \param x2     The second x co-ordinate,
     * \param y2     The second y co-ordinate,
     */
    void         (*line)          (struct ADrawTag *ctx,
                                   unsigned int x1,
                                   unsigned int y1,
                                   unsigned int x2,
                                   unsigned int y2);

    /** Draw a dotted line.
     * Draw a dotted straight line between two points.
     * \param ctx    The drawing context.
     * \param x1     The first x co-ordinate,
     * \param y1     The first y co-ordinate,
     * \param x2     The second x co-ordinate,
     * \param y2     The second y co-ordinate,
     */
    void         (*dottedLine)    (struct ADrawTag *ctx,
                                   unsigned int x1,
                                   unsigned int y1,
                                   unsigned int x2,
                                   unsigned int y2);

    /** Left aligned text.
     * Write a single line of text that will end at some co-ordinates.
     * \param ctx    The drawing context.
     * \param x      The position at which the text should terminate.
     * \param y      The position above which the text will lie.
     * \param string The string to write.
     */
    void         (*textL)         (struct ADrawTag *ctx,
                                   unsigned int x,
                                   unsigned int y,
                                   const char *string);

    /** Center aligned text.
     * Write a single line of text that will center on some co-ordinates.
     * \param ctx    The drawing context.
     * \param x      The position at which the text should be centered.
     * \param y      The position above which the text will lie.
     * \param string The string to write.
     */
    void         (*textC)         (struct ADrawTag *ctx,
                                   unsigned int x,
                                   unsigned int y,
                                   const char *string);

    /** Right aligned text.
     * Write a single line of text that will start at some co-ordinates.
     * \param ctx    The drawing context.
     * \param x      The position at which the text should start.
     * \param y      The position above which the text will lie.
     * \param string The string to write.
     */
    void         (*textR)         (struct ADrawTag *ctx,
                                   unsigned int x,
                                   unsigned int y,
                                   const char *string);

    /** Determine the width of some string.
     * \param ctx    The drawing context.
     * \param string The string for which the width should be determined.
     * \returns      The width of the passed string as it would be rendered
     *                by the current drawing context.
     */
    unsigned int (*textWidth)     (struct ADrawTag *ctx,
                                   const char *string);

    /** Determine the height of text in the current font.
     * \param ctx    The drawing context.
     *
     */
    int          (*textHeight)    (struct ADrawTag *ctx);

    void         (*filledRectangle)(struct ADrawTag *ctx,
                                    unsigned int x1,
                                    unsigned int y1,
                                    unsigned int x2,
                                    unsigned int y2);

    void         (*filledTriangle)(struct ADrawTag *ctx,
                                   unsigned int x1,
                                   unsigned int y1,
                                   unsigned int x2,
                                   unsigned int y2,
                                   unsigned int x3,
                                   unsigned int y3);

    void           (*filledCircle)(struct ADrawTag *ctx,
                                   unsigned int x,
                                   unsigned int y,
                                   unsigned int r);

    /** Draw an arc.
     * This draws an arc centred at (cx,cy) which fits in a box of \a w by \a h.
     * The arc is drawn from \a s degrees to \a e degrees.
     * \note 0 degrees points east on the page and 90 degrees is south.
     *
     */
    void         (*arc)           (struct ADrawTag *ctx,
                                   unsigned int cx,
                                   unsigned int cy,
                                   unsigned int w,
                                   unsigned int h,
                                   unsigned int s,
                                   unsigned int e);

    void         (*dottedArc)     (struct ADrawTag *ctx,
                                   unsigned int cx,
                                   unsigned int cy,
                                   unsigned int w,
                                   unsigned int h,
                                   unsigned int s,
                                   unsigned int e);

    void         (*setPen)        (struct ADrawTag *ctx,
                                   ADrawColour col);

    void         (*setBgPen)      (struct ADrawTag *ctx,
                                   ADrawColour col);

    void         (*setFontSize)   (struct ADrawTag *ctx,
                                   ADrawFontSize size);

    Boolean      (*close)         (struct ADrawTag *context);

    /* Internal context, not accessible by the user */
    void *internal;
}
ADraw;

/***************************************************************************
 * Functions
 ***************************************************************************/

/** Create a new drawing context.
 * This will create a drawing context with some dimensions, and some format.
 * After this has been called, the function pointers in the returned structure
 * can be called together with a pointer to the structure itself to cause
 * image functions to be executed.
 *
 * \param[in] w                The width of the output image.
 * \param[in] h                The height of the output image.
 * \param[in] file             The file to which the image should be written.
 * \param[in] fontName         The name of the font to use for rendering.
 * \param[in] type             The output type to generate.
 * \param[in, out] *outContext Pointer to an \a ADraw structure to populate
 *                              with values.
 * \retval Boolean             On error, #FALSE will be returned.
 */
Boolean ADrawOpen(unsigned int     w,
                  unsigned int     h,
                  const char      *file,
                  const char      *fontName,
                  ADrawOutputType  type,
                  struct ADrawTag *outContext);

/** Given a string name for a colour, return the corresponding ADrawColour.
 *
 * \param[in] colour  The string representation of the colour that is sought.
 */
ADrawColour ADrawGetColour(const char *colour);

/** Compute the position of a point on an arc.
 * This allows co-ordinates on arc drawn using arc() or dottedArc() to be
 * computed.  The centre and bounding box for the arc are supplied as well
 * as an angular offset.
 *
 * \param[in] cx        Center of the arc.
 * \param[in] cy        Center of the arc.
 * \param[in] w         Arc x-diameter (width).
 * \param[in] h         Arc y-diameter (height).
 * \param[in] degrees   Position on the arc to be returned.
 * \param[in,out] x     Pointer to fill with x coordinate.
 * \param[in,out] y     Pointer to fill with y coordinate.
 */
void ADrawComputeArcPoint(float cx,
                          float cy,
                          float w,
                          float h,
                          float degrees,
                          unsigned int *x,
                          unsigned int *y);

#endif /* MSCGEN_ADRAW_H */

/* END OF FILE */
