/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef _GIFENC_H
#define _GIFENC_H
#include <qfile.h>

typedef unsigned char  Byte;
typedef unsigned short Word;

struct Color
{
  Byte red;
  Byte green;
  Byte blue;
};

const int hashTableSize=5003;     // size of the hash table 

class GifEncoder
{
  public:
    GifEncoder(Byte *rawBytes,Color *p,int w,int h,Byte d,int t);
   ~GifEncoder();
    void writeGIF(QFile &fp);
   
  protected:
    void putWord( Word w, QFile &fp );
    void putByte( Byte b, QFile &fp );
    void compress(int csize, QFile &fp);
    void writeCode(int code, QFile &fp);
    void writeChar( Byte c, QFile &fp );
    void writePacket(QFile &fp);
    int  nextPixel()
    {
      if ( --numPixels < 0) return EOF;
      return *dataPtr++;
    }
    void clearHashTable();
    
  private:
    const int colorResolution;      // 8 bit for Red, Green and Blue;
    const int globalPaletteFlag;
    const int bits;
    const int maxMaxCode; 

    // image variables
    int width;             // image width 
    int height;            // image height 
    Byte depth;            // bits per CLUT entry 
    int transIndex;        // index of the transparant color; -1 = none 
    Color *palette;        // pointer to the color palette 
    int numPixels;         // total number of pixel (i.e. width * height)
    Byte *data;            // pointer to the image data (one byte per pixel) 
    Byte *dataPtr;         // pointer located at the current pixel.
    
    // compression variables
    bool clearFlag;        // clear hash table flag
    int  freeEntry;        // first free entry in the hash table
    unsigned int curAccum; // encoded bit accumulator
    int curBits;           // number of bits in curAccum
    Byte accum[256];       // Buffer for a packet
    int aCount;            // Number of characters so far in this 'packet'
    int nBits;             // number of bits/code
    int maxCode;           // maximum code, given n_bits
    int initBits;          // initial number of bits
    int EOFCode;           // code for signaling the end of the file
    int ClearCode;         // the clear code for the decompressor
    
    // tables
    int htab[hashTableSize];       // the hash table
    Word codetab[hashTableSize];   // the code table
};

#endif
