/*
LodePNG version 20080927

Copyright (c) 2005-2008 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

/** Minified version of LodePNG, with only the encoder code */

#ifndef LODEPNG_H
#define LODEPNG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ////////////////////////////////////////////////////////////////////////// */
/* LodeFlate & LodeZlib Setting structs                                       */
/* ////////////////////////////////////////////////////////////////////////// */


typedef struct LodeZlib_DeflateSettings /*deflate = compress*/
{
  /*LZ77 related settings*/
  unsigned btype; /*the block type for LZ*/
  unsigned useLZ77; /*whether or not to use LZ77*/
  unsigned windowSize; /*the maximum is 32768*/
} LodeZlib_DeflateSettings;


/* ////////////////////////////////////////////////////////////////////////// */
/* LodePNG                                                                    */
/* ////////////////////////////////////////////////////////////////////////// */

typedef struct LodePNG_InfoColor /*info about the color type of an image*/
{
  /*header (IHDR)*/
  unsigned colorType; /*color type*/
  unsigned bitDepth;  /*bits per sample*/

  /*palette (PLTE)*/
  unsigned char* palette; /*palette in RGBARGBA... order*/
  size_t palettesize; /*palette size in number of colors (amount of bytes is 4 * palettesize)*/

  /*transparent color key (tRNS)*/
  unsigned key_defined; /*is a transparent color key given?*/
  unsigned key_r;       /*red component of color key*/
  unsigned key_g;       /*green component of color key*/
  unsigned key_b;       /*blue component of color key*/
} LodePNG_InfoColor;

typedef struct LodePNG_InfoPng /*information about the PNG image, except pixels and sometimes except width and height*/
{
  /*header (IHDR), palette (PLTE) and transparency (tRNS)*/
  unsigned width;             /*width of the image in pixels (ignored by encoder, but filled in by decoder)*/
  unsigned height;            /*height of the image in pixels (ignored by encoder, but filled in by decoder)*/
  unsigned compressionMethod; /*compression method of the original file*/
  unsigned filterMethod;      /*filter method of the original file*/
  unsigned interlaceMethod;   /*interlace method of the original file*/
  LodePNG_InfoColor color;    /*color type and bits, palette, transparency*/
} LodePNG_InfoPng;

typedef struct LodePNG_InfoRaw /*contains user-chosen information about the raw image data, which is independent of the PNG image*/
{
  LodePNG_InfoColor color;
} LodePNG_InfoRaw;

unsigned LodePNG_InfoColor_addPalette(LodePNG_InfoColor* info, unsigned char r, unsigned char g, unsigned char b, unsigned char a); /*add 1 color to the palette*/

typedef struct LodePNG_EncodeSettings
{
  LodeZlib_DeflateSettings zlibsettings; /*settings for the zlib encoder, such as window size, ...*/

  unsigned autoLeaveOutAlphaChannel; /*automatically use color type without alpha instead of given one, if given image is opaque*/
  unsigned force_palette; /*force creating a PLTE chunk if colortype is 2 or 6 (= a suggested palette). If colortype is 3, PLTE is _always_ created.*/
} LodePNG_EncodeSettings;

void LodePNG_EncodeSettings_init(LodePNG_EncodeSettings* settings);

typedef struct LodePNG_Encoder
{
  LodePNG_EncodeSettings settings;
  LodePNG_InfoPng infoPng; /*the info specified by the user may not be changed by the encoder. The encoder will try to generate a PNG close to the given info.*/
  LodePNG_InfoRaw infoRaw; /*put the properties of the input raw image in here*/
  unsigned error;
} LodePNG_Encoder;

void LodePNG_Encoder_init(LodePNG_Encoder* encoder);
void LodePNG_Encoder_cleanup(LodePNG_Encoder* encoder);
void LodePNG_Encoder_copy(LodePNG_Encoder* dest, const LodePNG_Encoder* source);

/*This function allocates the out buffer and stores the size in *outsize.*/
void LodePNG_encode(LodePNG_Encoder* encoder, unsigned char** out, size_t* outsize, const unsigned char* image, unsigned w, unsigned h);

/*free functions allowing to load and save a file from/to harddisk*/
/*This function allocates the out buffer and stores the size in *outsize.*/
//unsigned LodePNG_loadFile(unsigned char** out, size_t* outsize, const char* filename);
unsigned LodePNG_saveFile(const unsigned char* buffer, size_t buffersize, const char* filename);

#ifdef __cplusplus
}
#endif

#endif

