/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef _PNGENC_H
#define _PNGENC_H
#include <qfile.h>

typedef unsigned char  Byte;

/*! \brief Entry in the color palette of a PNG image */
struct Color
{
  Byte red;
  Byte green;
  Byte blue;
};

/*! \brief PNG image encoder
 *
 *  This class allows to encode a raw image to a compressed PNG.
 *  Use write() to save the result to disk.
 */
class PngEncoder
{
  public:
    PngEncoder(Byte *rawBytes,Color *p,int w,int h,Byte d,int t);
   ~PngEncoder();
    void write(const char *fileName);
   
  private:
    // image variables
    Byte *data;            // pointer to the image data (one byte per pixel) 
    Color *palette;        // pointer to the color palette 
    int width;             // image width 
    int height;            // image height 
    Byte depth;            // bits per CLUT entry 
    int transIndex;        // index of the transparant color; -1 = none 
    int numPixels;         // total number of pixel (i.e. width * height)
    Byte *dataPtr;         // pointer located at the current pixel.
    
};

#endif
