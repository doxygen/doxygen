/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
 * ---------------------------------------------------------------------------
 *
 * The Portable Network Graphic format is an ISO Standard.
 * Most of the code below was donated by Bernhard Ristow.
 */

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

#include <png.h>
#include <stdio.h>
#include <malloc.h>
#include "pngenc.h"
#include "message.h"

static void user_error_fn(png_structp, png_const_charp error_msg)
{
    err("%s\n", error_msg);
}

static void user_warning_fn(png_structp, png_const_charp warning_msg)
{
    err("%s\n", warning_msg);
}

PngEncoder::PngEncoder(Byte *rawBytes, Color *p, int w, int h, Byte d, int t) : 
     data(rawBytes), palette(p), width(w), height(h), depth(d), transIndex(t)
{
   numPixels = w*h;
   dataPtr   = data;
}

PngEncoder::~PngEncoder()
{
}

void PngEncoder::write(const char *name)
{
   FILE           * file               = NULL;
   unsigned char ** rows               = 0;
   unsigned char  * cmap               = 0;
   short            numOfColors        = (1<<depth);
   short            bit_depth          = 4;
   long             i                  = 0;
   long             j                  = 0;
   png_structp      png_ptr;
   png_infop        info_ptr;
   char             user_error_ptr[]   = "PngEncoder";
   png_colorp       png_palette;
   png_byte         ti[1];

   png_ptr = png_create_write_struct
             ( PNG_LIBPNG_VER_STRING, (png_voidp)user_error_ptr,
               user_error_fn, user_warning_fn);
   if (!png_ptr) 
   {
      err("Can not allocate writing structure!\n");
      return;
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) 
   {
      png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
      err("Can not allocate writing structure!\n");
      return;
   }

   if (setjmp(png_jmpbuf(png_ptr))) 
   {
     png_destroy_write_struct(&png_ptr, &info_ptr);
     return;
   }
   else
   {
     png_palette = (png_colorp) png_malloc(png_ptr,
         PNG_MAX_PALETTE_LENGTH*sizeof(png_color));
     memset(png_palette,0,PNG_MAX_PALETTE_LENGTH*sizeof(png_color));
     for (i=0; i<numOfColors; i++) 
     {
       png_palette[i].red   = palette[i].red;
       png_palette[i].green = palette[i].green;
       png_palette[i].blue  = palette[i].blue;
     }
     png_set_PLTE(png_ptr, info_ptr, png_palette, PNG_MAX_PALETTE_LENGTH);
     png_set_IHDR( png_ptr, info_ptr, width, height, bit_depth,
         PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE
                 );
     ti[0] = transIndex;
     png_set_tRNS(png_ptr,info_ptr,ti,1,NULL);
     rows    = (unsigned char **) calloc(sizeof(unsigned char*),height);
     rows[0] = (unsigned char  *) calloc(sizeof(unsigned char),height*width);
     for (i=1; i<height; i++) 
     {
       rows[i] = rows[i-1] + width;
     }
     for (i=0, dataPtr=data; i<height; i++) 
     {
       for (j=0; j<width; j++) 
       {
         if (j%2)
         {
           rows[i][j/2] = ( rows[i][j/2] | *dataPtr );
         }
         else
         {
           rows[i][j/2] = (*dataPtr) << 4;
         }
         dataPtr++;
       }
     }
     png_set_rows(png_ptr,info_ptr,rows);

     file = fopen(name,"wb");
     png_init_io(png_ptr,file);
     png_write_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,NULL);
   }

   png_destroy_write_struct(&png_ptr, &info_ptr);

   if (file)
   {
      fclose (file);
   }
   if (cmap)
   {
      free(cmap);
   }
   if (rows) 
   {
      if (rows[0])
      {
         free(rows[0]);
      }
      free(rows);
   }
   return;
}

