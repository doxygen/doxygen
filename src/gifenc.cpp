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
 * The GIF compression code below is based on the file ppmtogif.c of the
 * netpbm package. The original copyright message follows:
 *
 * ---------------------------------------------------------------------------
 * 
 * Copyright (C) 1989 by Jef Poskanzer.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "as is" without express or
 * implied warranty.
 *
 * ---------------------------------------------------------------------------
 *
 * The Graphics Interchange Format(c) is the Copyright property of
 * CompuServe Incorporated.  GIF(sm) is a Service Mark property of
 * CompuServe Incorporated.
 */

#include "gifenc.h"

static const unsigned int masks[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
                                      0x001F, 0x003F, 0x007F, 0x00FF,
                                      0x01FF, 0x03FF, 0x07FF, 0x0FFF,
                                      0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };

GifEncoder::GifEncoder(Byte *rawBytes,Color *p,int w,int h, Byte d,
                       int t)
  : colorResolution(8),globalPaletteFlag(0x80),bits(12),
    maxMaxCode(1<<bits)
{
  width = w;
  height = h;
  depth = d;
  transIndex = t;
  palette = p;
  data = rawBytes;
  dataPtr = data;
}

GifEncoder::~GifEncoder()
{
}

void GifEncoder::writeGIF(QFile &fp)
{
  // Write the Magic header
  fp.writeBlock( transIndex < 0 ? "GIF87a" : "GIF89a", 6 );

  // Write the logical screen descriptor
  putWord( width,   fp );
  putWord( height,  fp );
  Byte pack = globalPaletteFlag | ((colorResolution-1)<<4) | (depth-1);
  putByte( pack,    fp );
  putByte( 0,       fp ); // the background color
  putByte( 0,       fp ); // no aspect ration defined

  // Write global color table
  int i; for ( i=0 ; i< (1<<depth) ; i++)
  {
    putByte(palette[i].red,  fp);
    putByte(palette[i].green,fp);
    putByte(palette[i].blue, fp);
  }
  
  if ( transIndex >= 0)
  {
    // Write graphic control extension (needed for GIF transparancy)
    putByte( 0x21, fp); // extension introducer
    putByte( 0xf9, fp); // graphic control label
    putByte(    4, fp); // block size
    putByte(    1, fp); // announce transparacy value
    putWord(    0, fp); // zero delay time
    putByte( transIndex, fp); // write transparant index
    putByte(    0, fp); // end block
  }
  
  // Write the image descriptor
  putByte(   0x2c, fp); // image separator
  putWord(      0, fp); // image left position
  putWord(      0, fp); // image top position
  putWord(  width, fp); // image width
  putWord( height, fp); // image height
  putByte(      0, fp); // no local color table, no interlacing
 
  // Write table based image data
  Byte initCodeSize = depth<=1 ? 2 : depth;
  putByte( initCodeSize, fp); // LZW Minimum Code Size
  compress( initCodeSize+1, fp);
  putByte( 0, fp); // end of blocks
 
  // Write GIF Trailer
  putByte( 0x3b, fp);
}

void GifEncoder::compress( int ibits, QFile &outfile )
{
  int i;
  int entry;

  initBits  = ibits;
  numPixels = width*height;
  dataPtr   = data;
  clearFlag = FALSE;
  nBits     = initBits;
  maxCode   = (1<<nBits) -1;
  ClearCode = (1 << (initBits - 1));
  EOFCode   = ClearCode + 1;
  freeEntry = ClearCode + 2;
  aCount    = 0;
  curAccum  = 0;
  curBits   = 0;

  entry = nextPixel();

  int hshift = 0;
  int fcode;
  for ( fcode = hashTableSize;  fcode < 65536L; fcode *= 2L ) ++hshift;
  hshift = 8 - hshift;                /* set hash code range bound */

  clearHashTable();                   /* clear hash table */

  writeCode( ClearCode,outfile );

  int c;
  while ( (c = nextPixel()) != EOF ) 
  {  
    fcode = (c << bits) + entry;
    i = (c << hshift) ^ entry;    /* xor hashing */

    bool found=FALSE;
    if (htab[i]==fcode) 
    {
      entry = codetab[i];
      found=TRUE;
    } 
    else if (htab[i]>=0)
    {
      int disp = hashTableSize - i;
      if (i==0) disp=1;
      do
      {
        if ((i-=disp)<0) i+=hashTableSize;
        if (htab[i]==fcode)
        {
          entry=codetab[i];
          found=TRUE;
        }
      } while (htab[i]>0 && !found);
    }
    if (!found)
    {
      writeCode( entry, outfile );
      entry = c;
      if ( freeEntry < maxMaxCode ) 
      {
        codetab[i] = freeEntry++; /* code -> hashtable */
        htab[i]    = fcode;
      } 
      else
      {
        clearHashTable();
        freeEntry = ClearCode + 2;
        clearFlag = TRUE;
        writeCode( ClearCode, outfile );
      }
    }
  }
  writeCode( entry, outfile );
  writeCode( EOFCode, outfile );
}

void GifEncoder::putWord( Word w, QFile &fp )
{
  fp.putch( w & 0xff );
  fp.putch( (w>>8) & 0xff );
}

void GifEncoder::putByte( Byte b, QFile &fp )
{
  fp.putch( b );
}

void GifEncoder::writeCode( int code, QFile &outfile )
{
  curAccum &= masks[ curBits ];

  if ( curBits > 0 ) 
  {
    curAccum |= (code << curBits);
  }
  else
  {
    curAccum = code;
  }

  curBits += nBits;

  while( curBits >= 8 ) 
  {
    writeChar( (Byte)(curAccum & 0xff),outfile );
    curAccum >>= 8;
    curBits -= 8;
  }

  /*
   * If the next entry is going to be too big for the code size,
   * then increase it, if possible.
   */
  if ( freeEntry > maxCode || clearFlag ) 
  {
    if( clearFlag ) 
    {
      nBits = initBits;
      maxCode = (1<<nBits)-1;
      clearFlag = FALSE;
    } 
    else 
    {
      ++nBits;
      if ( nBits == bits )
        maxCode = maxMaxCode;
      else
        maxCode = (1<<nBits)-1;
    }
  }

  if ( code == EOFCode ) 
  {
    /* At EOF, write the rest of the buffer.  */
    while( curBits > 0 ) 
    {
      writeChar( (Byte)(curAccum & 0xff), outfile );
      curAccum >>= 8;
      curBits -= 8;
    }

    writePacket(outfile);
  }
}

/*
 * Add a character to the end of the current packet, and if it is 254
 * characters, flush the packet to disk.
 */
void GifEncoder::writeChar( Byte c, QFile &fp )
{
  accum[ aCount++ ] = c;
  if( aCount >= 254 ) writePacket(fp);
}

/*
 * Flush the packet to disk, and reset the accumulator
 */
void GifEncoder::writePacket(QFile &fp)
{
  if ( aCount > 0 ) 
  {
    fp.putch( aCount );
    fp.writeBlock( (const char *)accum, aCount );
    aCount = 0;
  }
}

void GifEncoder::clearHashTable()          /* reset code table */
{
  int *htab_p = htab;
  int i; for (i=0;i<hashTableSize;i++) *htab_p++ = -1;
}

