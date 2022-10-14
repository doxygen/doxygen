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

/*
The manual and changelog can be found in the header file "lodepng.h"
You are free to name this file lodepng.cpp or lodepng.c depending on your usage.
*/

#include "lodepng.h"

#define USE_BRUTE_FORCE_ENCODING 1

#define VERSION_STRING "20080927"

/* ////////////////////////////////////////////////////////////////////////// */
/* / Tools For C                                                            / */
/* ////////////////////////////////////////////////////////////////////////// */

/*
About these tools (vector, uivector, ucvector and string):
-LodePNG was originally written in C++. The vectors replace the std::vectors that were used in the C++ version.
-The string tools are made to avoid problems with compilers that declare things like strncat as deprecated.
-They're not used in the interface, only internally in this file, so all their functions are made static.
*/

//--------------------------------------------------------------------------------------------


/*LodePNG_chunk functions: These functions need as input a large enough amount of allocated memory.*/

static unsigned LodePNG_chunk_length(const unsigned char* chunk); /*get the length of the data of the chunk. Total chunk length has 12 bytes more.*/

static void LodePNG_chunk_generate_crc(unsigned char* chunk); /*generates the correct CRC from the data and puts it in the last 4 bytes of the chunk*/

/*add chunks to out buffer. It reallocs the buffer to append the data. returns error code*/
static unsigned LodePNG_create_chunk(unsigned char** out, size_t* outlength, unsigned length, const char* type, const unsigned char* data); /*appends new chunk to out. Returns pointer to start of appended chunk, or NULL if error happened; may change memory address of out buffer*/

static void LodePNG_InfoColor_init(LodePNG_InfoColor* info);
static void LodePNG_InfoColor_cleanup(LodePNG_InfoColor* info);
static unsigned LodePNG_InfoColor_copy(LodePNG_InfoColor* dest, const LodePNG_InfoColor* source);

/*Use these functions instead of allocating palette manually*/
static void LodePNG_InfoColor_clearPalette(LodePNG_InfoColor* info);

/*additional color info*/
static unsigned LodePNG_InfoColor_getBpp(const LodePNG_InfoColor* info);      /*bits per pixel*/
static unsigned LodePNG_InfoColor_isGreyscaleType(const LodePNG_InfoColor* info); /*is it a greyscale type? (colorType 0 or 4)*/
static unsigned LodePNG_InfoColor_isAlphaType(const LodePNG_InfoColor* info);     /*has it an alpha channel? (colorType 2 or 6)*/

static void LodePNG_InfoPng_init(LodePNG_InfoPng* info);
static void LodePNG_InfoPng_cleanup(LodePNG_InfoPng* info);
static unsigned LodePNG_InfoPng_copy(LodePNG_InfoPng* dest, const LodePNG_InfoPng* source);

static void LodePNG_InfoRaw_init(LodePNG_InfoRaw* info);
static void LodePNG_InfoRaw_cleanup(LodePNG_InfoRaw* info);
static unsigned LodePNG_InfoRaw_copy(LodePNG_InfoRaw* dest, const LodePNG_InfoRaw* source);

/*
LodePNG_convert: Converts from any color type to 24-bit or 32-bit (later maybe more supported). return value = LodePNG error code
The out buffer must have (w * h * bpp + 7) / 8, where bpp is the bits per pixel of the output color type (LodePNG_InfoColor_getBpp)
*/
static unsigned LodePNG_convert(unsigned char* out, const unsigned char* in, LodePNG_InfoColor* infoOut, LodePNG_InfoColor* infoIn, unsigned w, unsigned h);

static void LodeZlib_DeflateSettings_init(LodeZlib_DeflateSettings* settings);

/* ////////////////////////////////////////////////////////////////////////// */
/* LodeFlate & LodeZlib                                                       */
/* ////////////////////////////////////////////////////////////////////////// */

/*This function reallocates the out buffer and appends the data.
Either, *out must be NULL and *outsize must be 0, or, *out must be a valid buffer and *outsize its size in bytes.*/
//unsigned LodeZlib_compress(unsigned char** out, size_t* outsize, const unsigned char* in, size_t insize, const LodeZlib_DeflateSettings* settings);

//--------------------------------------------------------------------------------------------

typedef struct vector /*this one is used only by the deflate compressor*/
{
  void* data;
  size_t size; /*in groups of bytes depending on type*/
  size_t allocsize; /*in bytes*/
  unsigned typesize; /*sizeof the type you store in data*/
} vector;

static unsigned vector_resize(vector* p, size_t size) /*returns 1 if success, 0 if failure ==> nothing done*/
{
  if(size * p->typesize > p->allocsize)
  {
    size_t newsize = size * p->typesize * 2;
    void* data = realloc(p->data, newsize);
    if(data)
    {
      p->allocsize = newsize;
      p->data = data;
      p->size = size;
    }
    else return 0;
  }
  else p->size = size;
  return 1;
}

static unsigned vector_resized(vector* p, size_t size, void dtor(void*)) /*resize and use destructor on elements if it gets smaller*/
{
  size_t i;
  if(size < p->size) for(i = size; i < p->size; i++) dtor(&((char*)(p->data))[i * p->typesize]);
  return vector_resize(p, size);
}

static void vector_cleanup(void* p)
{
  ((vector*)p)->size = ((vector*)p)->allocsize = 0;
  free(((vector*)p)->data);
  ((vector*)p)->data = NULL;
}

static void vector_cleanupd(vector* p, void dtor(void*)) /*clear and use destructor on elements*/
{
  vector_resized(p, 0, dtor);
  vector_cleanup(p);
}

static void vector_init(vector* p, unsigned typesize)
{
  p->data = NULL;
  p->size = p->allocsize = 0;
  p->typesize = typesize;
}

static void vector_swap(vector* p, vector* q) /*they're supposed to have the same typesize*/
{
  size_t tmp;
  void* tmpp;
  tmp = p->size; p->size = q->size; q->size = tmp;
  tmp = p->allocsize; p->allocsize = q->allocsize; q->allocsize = tmp;
  tmpp = p->data; p->data = q->data; q->data = tmpp;
}

static void* vector_get(vector* p, size_t index)
{
  return &((char*)p->data)[index * p->typesize];
}

/* /////////////////////////////////////////////////////////////////////////// */

typedef struct uivector
{
  unsigned* data;
  size_t size; /*size in number of unsigned longs*/
  size_t allocsize; /*allocated size in bytes*/
} uivector;

static void uivector_cleanup(void* p)
{
  ((uivector*)p)->size = ((uivector*)p)->allocsize = 0;
  free(((uivector*)p)->data);
  ((uivector*)p)->data = NULL;
}

static unsigned uivector_resize(uivector* p, size_t size) /*returns 1 if success, 0 if failure ==> nothing done*/
{
  if(size * sizeof(unsigned) > p->allocsize)
  {
    size_t newsize = size * sizeof(unsigned) * 2;
    void* data = realloc(p->data, newsize);
    if(data)
    {
      p->allocsize = newsize;
      p->data = (unsigned*)data;
      p->size = size;
    }
    else return 0;
  }
  else p->size = size;
  return 1;
}

static unsigned uivector_resizev(uivector* p, size_t size, unsigned value) /*resize and give all new elements the value*/
{
  size_t oldsize = p->size, i;
  if(!uivector_resize(p, size)) return 0;
  for(i = oldsize; i < size; i++) p->data[i] = value;
  return 1;
}

static void uivector_init(uivector* p)
{
  p->data = NULL;
  p->size = p->allocsize = 0;
}

static unsigned uivector_push_back(uivector* p, unsigned c) /*returns 1 if success, 0 if failure ==> nothing done*/
{
  if(!uivector_resize(p, p->size + 1)) return 0;
  p->data[p->size - 1] = c;
  return 1;
}

static unsigned uivector_copy(uivector* p, const uivector* q) /*copy q to p, returns 1 if success, 0 if failure ==> nothing done*/
{
  size_t i;
  if(!uivector_resize(p, q->size)) return 0;
  for(i = 0; i < q->size; i++) p->data[i] = q->data[i];
  return 1;
}

static void uivector_swap(uivector* p, uivector* q)
{
  size_t tmp;
  unsigned* tmpp;
  tmp = p->size; p->size = q->size; q->size = tmp;
  tmp = p->allocsize; p->allocsize = q->allocsize; q->allocsize = tmp;
  tmpp = p->data; p->data = q->data; q->data = tmpp;
}

/* /////////////////////////////////////////////////////////////////////////// */

typedef struct ucvector
{
  unsigned char* data;
  size_t size; /*used size*/
  size_t allocsize; /*allocated size*/
} ucvector;

static void ucvector_cleanup(void* p)
{
  ((ucvector*)p)->size = ((ucvector*)p)->allocsize = 0;
  free(((ucvector*)p)->data);
  ((ucvector*)p)->data = NULL;
}

static unsigned ucvector_resize(ucvector* p, size_t size) /*returns 1 if success, 0 if failure ==> nothing done*/
{
  if(size * sizeof(unsigned) > p->allocsize)
  {
    size_t newsize = size * sizeof(unsigned) * 2;
    void* data = realloc(p->data, newsize);
    if(data)
    {
      p->allocsize = newsize;
      p->data = (unsigned char*)data;
      p->size = size;
    }
    else return 0; /*error: not enough memory*/
  }
  else p->size = size;
  return 1;
}


static void ucvector_init(ucvector* p)
{
  p->data = NULL;
  p->size = p->allocsize = 0;
}

/*you can both convert from vector to buffer&size and vica versa*/
static void ucvector_init_buffer(ucvector* p, unsigned char* buffer, size_t size)
{
  p->data = buffer;
  p->allocsize = p->size = size;
}

static unsigned ucvector_push_back(ucvector* p, unsigned char c) /*returns 1 if success, 0 if failure ==> nothing done*/
{
  if(!ucvector_resize(p, p->size + 1)) return 0;
  p->data[p->size - 1] = c;
  return 1;
}

/* ////////////////////////////////////////////////////////////////////////// */
/* / Reading and writing single bits and bytes from/to stream for Deflate   / */
/* ////////////////////////////////////////////////////////////////////////// */

static void addBitToStream(size_t* bitpointer, ucvector* bitstream, unsigned char bit)
{
  if((*bitpointer) % 8 == 0) ucvector_push_back(bitstream, 0); /*add a new byte at the end*/
  (bitstream->data[bitstream->size - 1]) |= (bit << ((*bitpointer) & 0x7)); /*earlier bit of huffman code is in a lesser significant bit of an earlier byte*/
  (*bitpointer)++;
}

static void addBitsToStream(size_t* bitpointer, ucvector* bitstream, unsigned value, size_t nbits)
{
  size_t i;
  for(i = 0; i < nbits; i++) addBitToStream(bitpointer, bitstream, (unsigned char)((value >> i) & 1));
}

static void addBitsToStreamReversed(size_t* bitpointer, ucvector* bitstream, unsigned value, size_t nbits)
{
  size_t i;
  for(i = 0; i < nbits; i++) addBitToStream(bitpointer, bitstream, (unsigned char)((value >> (nbits - 1 - i)) & 1));
}


/* ////////////////////////////////////////////////////////////////////////// */
/* / Deflate - Huffman                                                      / */
/* ////////////////////////////////////////////////////////////////////////// */

#define FIRST_LENGTH_CODE_INDEX 257
#define LAST_LENGTH_CODE_INDEX 285
#define NUM_DEFLATE_CODE_SYMBOLS 288 /*256 literals, the end code, some length codes, and 2 unused codes*/
#define NUM_DISTANCE_SYMBOLS 32 /*the distance codes have their own symbols, 30 used, 2 unused*/
#define NUM_CODE_LENGTH_CODES 19 /*the code length codes. 0-15: code lengths, 16: copy previous 3-6 times, 17: 3-10 zeros, 18: 11-138 zeros*/

static const unsigned LENGTHBASE[29] /*the base lengths represented by codes 257-285*/
  = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
static const unsigned LENGTHEXTRA[29] /*the extra bits used by codes 257-285 (added to base length)*/
  = {0, 0, 0, 0, 0, 0, 0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,   4,   5,   5,   5,   5,   0};
static const unsigned DISTANCEBASE[30] /*the base backwards distances (the bits of distance codes appear after length codes and use their own huffman tree)*/
  = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
static const unsigned DISTANCEEXTRA[30] /*the extra bits of backwards distances (added to base)*/
  = {0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,   6,   6,   7,   7,   8,   8,    9,    9,   10,   10,   11,   11,   12,    12,    13,    13};
static const unsigned CLCL[NUM_CODE_LENGTH_CODES] /*the order in which "code length alphabet code lengths" are stored, out of this the huffman tree of the dynamic huffman tree lengths is generated*/
  = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

/* /////////////////////////////////////////////////////////////////////////// */

/*terminology used for the package-merge algorithm and the coin collector's problem*/
typedef struct Coin /*a coin can be multiple coins (when they're merged)*/
{
  uivector symbols;
  float weight; /*the sum of all weights in this coin*/
} Coin;

static void Coin_init(Coin* c)
{
  uivector_init(&c->symbols);
}

static void Coin_cleanup(void* c) /*void* so that this dtor can be given as function pointer to the vector resize function*/
{
  uivector_cleanup(&((Coin*)c)->symbols);
}

static void Coin_copy(Coin* c1, const Coin* c2)
{
  c1->weight = c2->weight;
  uivector_copy(&c1->symbols, &c2->symbols);
}

static void addCoins(Coin* c1, const Coin* c2)
{
  unsigned i;
  for(i = 0; i < c2->symbols.size; i++) uivector_push_back(&c1->symbols, c2->symbols.data[i]);
  c1->weight += c2->weight;
}

static void Coin_sort(Coin* data, size_t amount) /*combsort*/
{
  size_t gap = amount;
  unsigned char swapped = 0;
  while(gap > 1 || swapped)
  {
    size_t i;
    gap = (gap * 10) / 13; /*shrink factor 1.3*/
    if(gap == 9 || gap == 10) gap = 11; /*combsort11*/
    if(gap < 1) gap = 1;
    swapped = 0;
    for(i = 0; i < amount - gap; i++)
    {
      size_t j = i + gap;
      if(data[j].weight < data[i].weight)
      {
        float temp = data[j].weight; data[j].weight = data[i].weight; data[i].weight = temp;
        uivector_swap(&data[i].symbols, &data[j].symbols);
        swapped = 1;
      }
    }
  }
}

typedef struct HuffmanTree
{
  uivector tree2d;
  uivector tree1d;
  uivector lengths; /*the lengths of the codes of the 1d-tree*/
  unsigned maxbitlen; /*maximum number of bits a single code can get*/
  unsigned numcodes; /*number of symbols in the alphabet = number of codes*/
} HuffmanTree;

/*function used for debug purposes*/
/*#include <iostream>
static void HuffmanTree_draw(HuffmanTree* tree)
{
  std::cout << "tree. length: " << tree->numcodes << " maxbitlen: " << tree->maxbitlen << std::endl;
  for(size_t i = 0; i < tree->tree1d.size; i++)
  {
    if(tree->lengths.data[i])
      std::cout << i << " " << tree->tree1d.data[i] << " " << tree->lengths.data[i] << std::endl;
  }
  std::cout << std::endl;
}*/

static void HuffmanTree_init(HuffmanTree* tree)
{
  uivector_init(&tree->tree2d);
  uivector_init(&tree->tree1d);
  uivector_init(&tree->lengths);
}

static void HuffmanTree_cleanup(HuffmanTree* tree)
{
  uivector_cleanup(&tree->tree2d);
  uivector_cleanup(&tree->tree1d);
  uivector_cleanup(&tree->lengths);
}

/*the tree representation used by the decoder. return value is error*/
static unsigned HuffmanTree_make2DTree(HuffmanTree* tree)
{
  unsigned nodefilled = 0; /*up to which node it is filled*/
  unsigned treepos = 0; /*position in the tree (1 of the numcodes columns)*/
  unsigned n, i;

  if(!uivector_resize(&tree->tree2d, tree->numcodes * 2)) return 9901; /*if failed return not enough memory error*/
  /*convert tree1d[] to tree2d[][]. In the 2D array, a value of 32767 means uninited, a value >= numcodes is an address to another bit, a value < numcodes is a code. The 2 rows are the 2 possible bit values (0 or 1), there are as many columns as codes - 1
  a good huffman tree has N * 2 - 1 nodes, of which N - 1 are internal nodes. Here, the internal nodes are stored (what their 0 and 1 option point to). There is only memory for such good tree currently, if there are more nodes (due to too long length codes), error 55 will happen*/
  for(n = 0;  n < tree->numcodes * 2; n++) tree->tree2d.data[n] = 32767; /*32767 here means the tree2d isn't filled there yet*/

  for(n = 0; n < tree->numcodes; n++) /*the codes*/
  for(i = 0; i < tree->lengths.data[n]; i++) /*the bits for this code*/
  {
    unsigned char bit = (unsigned char)((tree->tree1d.data[n] >> (tree->lengths.data[n] - i - 1)) & 1);
    if(treepos > tree->numcodes - 2) return 55; /*error 55: oversubscribed; see description in header*/
    if(tree->tree2d.data[2 * treepos + bit] == 32767) /*not yet filled in*/
    {
      if(i + 1 == tree->lengths.data[n]) /*last bit*/
      {
        tree->tree2d.data[2 * treepos + bit] = n; /*put the current code in it*/
        treepos = 0;
      }
      else /*put address of the next step in here, first that address has to be found of course (it's just nodefilled + 1)...*/
      {
        nodefilled++;
        tree->tree2d.data[2 * treepos + bit] = nodefilled + tree->numcodes; /*addresses encoded with numcodes added to it*/
        treepos = nodefilled;
      }
    }
    else treepos = tree->tree2d.data[2 * treepos + bit] - tree->numcodes;
  }
  for(n = 0;  n < tree->numcodes * 2; n++) if(tree->tree2d.data[n] == 32767) tree->tree2d.data[n] = 0; /*remove possible remaining 32767's*/

  return 0;
}

static unsigned HuffmanTree_makeFromLengths2(HuffmanTree* tree) /*given that numcodes, lengths and maxbitlen are already filled in correctly. return value is error.*/
{
  uivector blcount;
  uivector nextcode;
  unsigned bits, n, error = 0;

  uivector_init(&blcount);
  uivector_init(&nextcode);
  if(!uivector_resize(&tree->tree1d, tree->numcodes)
  || !uivector_resizev(&blcount, tree->maxbitlen + 1, 0)
  || !uivector_resizev(&nextcode, tree->maxbitlen + 1, 0))
    error = 9902;

  if(!error)
  {
    /*step 1: count number of instances of each code length*/
    for(bits = 0; bits < tree->numcodes; bits++) blcount.data[tree->lengths.data[bits]]++;
    /*step 2: generate the nextcode values*/
    for(bits = 1; bits <= tree->maxbitlen; bits++) nextcode.data[bits] = (nextcode.data[bits - 1] + blcount.data[bits - 1]) << 1;
    /*step 3: generate all the codes*/
    for(n = 0; n < tree->numcodes; n++) if(tree->lengths.data[n] != 0) tree->tree1d.data[n] = nextcode.data[tree->lengths.data[n]]++;
  }

  uivector_cleanup(&blcount);
  uivector_cleanup(&nextcode);

  if(!error) return HuffmanTree_make2DTree(tree);
  else return error;
}

/*given the code lengths (as stored in the PNG file), generate the tree as defined by Deflate. maxbitlen is the maximum bits that a code in the tree can have. return value is error.*/
static unsigned HuffmanTree_makeFromLengths(HuffmanTree* tree, const unsigned* bitlen, size_t numcodes, unsigned maxbitlen)
{
  unsigned i;
  if(!uivector_resize(&tree->lengths, numcodes)) return 9903;
  for(i = 0; i < numcodes; i++) tree->lengths.data[i] = bitlen[i];
  tree->numcodes = (unsigned)numcodes; /*number of symbols*/
  tree->maxbitlen = maxbitlen;
  return HuffmanTree_makeFromLengths2(tree);
}

static unsigned HuffmanTree_fillInCoins(vector* coins, const unsigned* frequencies, unsigned numcodes, size_t sum)
{
  unsigned i;
  for(i = 0; i < numcodes; i++)
  {
    Coin* coin;
    if(frequencies[i] == 0) continue; /*it's important to exclude symbols that aren't present*/
    if(!vector_resize(coins, coins->size + 1)) { vector_cleanup(coins); return 9904; }
    coin = (Coin*)(vector_get(coins, coins->size - 1));
    Coin_init(coin);
    coin->weight = frequencies[i] / (float)sum;
    uivector_push_back(&coin->symbols, i);
  }
  if(coins->size) Coin_sort((Coin*)coins->data, coins->size);
  return 0;
}

static unsigned HuffmanTree_makeFromFrequencies(HuffmanTree* tree, const unsigned* frequencies, size_t numcodes, unsigned maxbitlen)
{
  unsigned i, j;
  size_t sum = 0, numpresent = 0;
  unsigned error = 0;

  vector prev_row; /*type Coin, the previous row of coins*/
  vector coins; /*type Coin, the coins of the currently calculated row*/

  tree->maxbitlen = maxbitlen;

  for(i = 0; i < numcodes; i++)
  {
    if(frequencies[i] > 0)
    {
      numpresent++;
      sum += frequencies[i];
    }
  }

  if(numcodes == 0) return 80; /*error: a tree of 0 symbols is not supposed to be made*/
  tree->numcodes = (unsigned)numcodes; /*number of symbols*/
  if (!uivector_resize(&tree->lengths, 0)) return 9955;
  if(!uivector_resizev(&tree->lengths, tree->numcodes, 0)) return 9905;

  if(numpresent == 0) /*there are no symbols at all, in that case add one symbol of value 0 to the tree (see RFC 1951 section 3.2.7) */
  {
    tree->lengths.data[0] = 1;
    return HuffmanTree_makeFromLengths2(tree);
  }
  else if(numpresent == 1) /*the package merge algorithm gives wrong results if there's only one symbol (theoretically 0 bits would then suffice, but we need a proper symbol for zlib)*/
  {
    for(i = 0; i < numcodes; i++) if(frequencies[i]) tree->lengths.data[i] = 1;
    return HuffmanTree_makeFromLengths2(tree);
  }

  vector_init(&coins, sizeof(Coin));
  vector_init(&prev_row, sizeof(Coin));

  /*Package-Merge algorithm represented by coin collector's problem
  For every symbol, maxbitlen coins will be created*/

  /*first row, lowest denominator*/
  error = HuffmanTree_fillInCoins(&coins, frequencies, tree->numcodes, sum);
  if(!error)
  {
    for(j = 1; j <= maxbitlen && !error; j++) /*each of the remaining rows*/
    {
      vector_swap(&coins, &prev_row); /*swap instead of copying*/
      if(!vector_resized(&coins, 0, Coin_cleanup)) { error = 9906; break; }

      for(i = 0; i + 1 < prev_row.size; i += 2)
      {
        if(!vector_resize(&coins, coins.size + 1)) { error = 9907; break; }
        Coin_init((Coin*)vector_get(&coins, coins.size - 1));
        Coin_copy((Coin*)vector_get(&coins, coins.size - 1), (Coin*)vector_get(&prev_row, i));
        addCoins((Coin*)vector_get(&coins, coins.size - 1), (Coin*)vector_get(&prev_row, i + 1)); /*merge the coins into packages*/
      }
      if(j < maxbitlen)
      {
        error = HuffmanTree_fillInCoins(&coins, frequencies, tree->numcodes, sum);
      }
    }
  }

  if(!error)
  {
    /*keep the coins with lowest weight, so that they add up to the amount of symbols - 1*/
    vector_resized(&coins, numpresent - 1, Coin_cleanup);

    /*calculate the lengths of each symbol, as the amount of times a coin of each symbol is used*/
    for(i = 0; i < coins.size; i++)
    {
      Coin* coin = (Coin*)vector_get(&coins, i);
      for(j = 0; j < coin->symbols.size; j++) tree->lengths.data[coin->symbols.data[j]]++;
    }

    error = HuffmanTree_makeFromLengths2(tree);
  }

  vector_cleanupd(&coins, Coin_cleanup);
  vector_cleanupd(&prev_row, Coin_cleanup);

  return error;
}

static unsigned HuffmanTree_getCode(const HuffmanTree* tree, unsigned index) { return tree->tree1d.data[index]; }
static unsigned HuffmanTree_getLength(const HuffmanTree* tree, unsigned index) { return tree->lengths.data[index]; }

/*get the tree of a deflated block with fixed tree, as specified in the deflate specification*/
static unsigned generateFixedTree(HuffmanTree* tree)
{
  unsigned i, error = 0;
  uivector bitlen;
  uivector_init(&bitlen);
  if(!uivector_resize(&bitlen, NUM_DEFLATE_CODE_SYMBOLS)) error = 9909;

  if(!error)
  {
    /*288 possible codes: 0-255=literals, 256=endcode, 257-285=lengthcodes, 286-287=unused*/
    for(i =   0; i <= 143; i++) bitlen.data[i] = 8;
    for(i = 144; i <= 255; i++) bitlen.data[i] = 9;
    for(i = 256; i <= 279; i++) bitlen.data[i] = 7;
    for(i = 280; i <= 287; i++) bitlen.data[i] = 8;

    error = HuffmanTree_makeFromLengths(tree, bitlen.data, NUM_DEFLATE_CODE_SYMBOLS, 15);
  }

  uivector_cleanup(&bitlen);
  return error;
}

static unsigned generateDistanceTree(HuffmanTree* tree)
{
  unsigned i, error = 0;
  uivector bitlen;
  uivector_init(&bitlen);
  if(!uivector_resize(&bitlen, NUM_DISTANCE_SYMBOLS)) error = 9910;

  /*there are 32 distance codes, but 30-31 are unused*/
  if(!error)
  {
    for(i = 0; i < NUM_DISTANCE_SYMBOLS; i++) bitlen.data[i] = 5;
    error = HuffmanTree_makeFromLengths(tree, bitlen.data, NUM_DISTANCE_SYMBOLS, 15);
  }
  uivector_cleanup(&bitlen);
  return error;
}

/* ////////////////////////////////////////////////////////////////////////// */
/* / Deflator                                                               / */
/* ////////////////////////////////////////////////////////////////////////// */

static const size_t MAX_SUPPORTED_DEFLATE_LENGTH = 258;

/*bitlen is the size in bits of the code*/
static void addHuffmanSymbol(size_t* bp, ucvector* compressed, unsigned code, unsigned bitlen)
{
  addBitsToStreamReversed(bp, compressed, code, bitlen);
}

/*search the index in the array, that has the largest value smaller than or equal to the given value, given array must be sorted (if no value is smaller, it returns the size of the given array)*/
static size_t searchCodeIndex(const unsigned* array, size_t array_size, size_t value)
{
  /*linear search implementation*/
  /*for(size_t i = 1; i < array_size; i++) if(array[i] > value) return i - 1;
  return array_size - 1;*/

  /*binary search implementation (not that much faster) (precondition: array_size > 0)*/
  size_t left  = 1;
  size_t right = array_size - 1;
  while(left <= right)
  {
    size_t mid = (left + right) / 2;
    if(array[mid] <= value) left = mid + 1; /*the value to find is more to the right*/
    else if(array[mid - 1] > value) right = mid - 1; /*the value to find is more to the left*/
    else return mid - 1;
  }
  return array_size - 1;
}

static void addLengthDistance(uivector* values, size_t length, size_t distance)
{
  /*values in encoded vector are those used by deflate:
  0-255: literal bytes
  256: end
  257-285: length/distance pair (length code, followed by extra length bits, distance code, extra distance bits)
  286-287: invalid*/

  unsigned length_code = (unsigned)searchCodeIndex(LENGTHBASE, 29, length);
  unsigned extra_length = (unsigned)(length - LENGTHBASE[length_code]);
  unsigned dist_code = (unsigned)searchCodeIndex(DISTANCEBASE, 30, distance);
  unsigned extra_distance = (unsigned)(distance - DISTANCEBASE[dist_code]);

  uivector_push_back(values, length_code + FIRST_LENGTH_CODE_INDEX);
  uivector_push_back(values, extra_length);
  uivector_push_back(values, dist_code);
  uivector_push_back(values, extra_distance);
}

#if USE_BRUTE_FORCE_ENCODING
#define encodeLZ77 encodeLZ77_brute
/*the "brute force" version of the encodeLZ7 algorithm, not used anymore, kept here for reference*/
static unsigned encodeLZ77_brute(uivector* out, const unsigned char* in, size_t size, unsigned windowSize)
{
  size_t pos;
  /*using pointer instead of vector for input makes it faster when NOT using optimization when compiling; no influence if optimization is used*/
  for(pos = 0; pos < size; pos++)
  {
    /*Phase 1: doxygen images often have long runs of the same color, try to find them*/
    const int minLength = 4; // Minimum length for a run to make sense

    if(pos < size - minLength * 4)
    {
      size_t p, fp;
      size_t current_length;

      /*RGBA pixel run?*/
      p  = pos;
      fp = pos + 4;
      current_length = 0;

      while(fp < size && in[p] == in[fp] && current_length < MAX_SUPPORTED_DEFLATE_LENGTH)
      {
        ++p;
        ++fp;
        ++current_length;
      }

      if (current_length > (minLength - 1 ) * 4) /*worth using?*/
      {
        uivector_push_back(out, in[pos    ]);
        uivector_push_back(out, in[pos + 1]);
        uivector_push_back(out, in[pos + 2]);
        uivector_push_back(out, in[pos + 3]);
        addLengthDistance(out, current_length, 4);

        pos += current_length + 4 - 1; /*-1 for loop's pos++*/
        continue;
      }

      /*RGB pixel run?*/
      p  = pos;
      fp = pos + 3;
      current_length = 0;

      while(fp < size && in[p] == in[fp] && current_length < MAX_SUPPORTED_DEFLATE_LENGTH)
      {
        ++p;
        ++fp;
        ++current_length;
      }

      if (current_length > (minLength - 1 ) * 3) /*worth using?*/
      {
        uivector_push_back(out, in[pos    ]);
        uivector_push_back(out, in[pos + 1]);
        uivector_push_back(out, in[pos + 2]);
        addLengthDistance(out, current_length, 3);

        pos += current_length + 3 - 1; /*-1 for loop's pos++*/
        continue;
      }
    }

    size_t length = 0, offset = 0; /*the length and offset found for the current position*/
    size_t max_offset = pos < windowSize ? pos : windowSize; /*how far back to test*/
    size_t current_offset;

    /**search for the longest string**/
    for(current_offset = 1; current_offset < max_offset; current_offset++) /*search backwards through all possible distances (=offsets)*/
    {
      size_t backpos = pos - current_offset;
      if(in[backpos] == in[pos])
      {
        /*test the next characters*/
        size_t current_length = 1;
        size_t backtest = backpos + 1;
        size_t foretest = pos + 1;
        while(foretest < size && in[backtest] == in[foretest] && current_length < MAX_SUPPORTED_DEFLATE_LENGTH) /*maximum support length by deflate is max length*/
        {
          if(backpos >= pos) backpos -= current_offset; /*continue as if we work on the decoded bytes after pos by jumping back before pos*/
          current_length++;
          backtest++;
          foretest++;
        }
        if(current_length > length)
        {
          length = current_length; /*the longest length*/
          offset = current_offset; /*the offset that is related to this longest length*/
          if(current_length == MAX_SUPPORTED_DEFLATE_LENGTH) break; /*you can jump out of this for loop once a length of max length is found (gives significant speed gain)*/
        }
      }
    }

    /**encode it as length/distance pair or literal value**/
    if(length < 3) /*only lengths of 3 or higher are supported as length/distance pair*/
    {
      uivector_push_back(out, in[pos]);
    }
    else
    {
      addLengthDistance(out, length, offset);
      pos += (length - 1);
    }
  } /*end of the loop through each character of input*/

  return 0;
}
#endif

/*
static const unsigned HASH_NUM_VALUES = 65536;
static const unsigned HASH_NUM_CHARACTERS = 6;
static const unsigned HASH_SHIFT = 2;
Good and fast values: HASH_NUM_VALUES=65536, HASH_NUM_CHARACTERS=6, HASH_SHIFT=2
making HASH_NUM_CHARACTERS larger (like 8), makes the file size larger but is a bit faster
making HASH_NUM_CHARACTERS smaller (like 3), makes the file size smaller but is slower
*/

#if !defined(USE_BRUTE_FORCE_ENCODING)
static unsigned getHash(const unsigned char* data, size_t size, size_t pos)
{
  unsigned result = 0;
  size_t amount, i;
  if(pos >= size) return 0;
  amount = HASH_NUM_CHARACTERS; if(pos + amount >= size) amount = size - pos;
  for(i = 0; i < amount; i++) result ^= (data[pos + i] << (i * HASH_SHIFT));
  return result % HASH_NUM_VALUES;
}

/*LZ77-encode the data using a hash table technique to let it encode faster. Return value is error code*/
static unsigned encodeLZ77(uivector* out, const unsigned char* in, size_t size, unsigned windowSize)
{
  /**generate hash table**/
  vector table; /*HASH_NUM_VALUES uivectors; this represents what would be an std::vector<std::vector<unsigned> > in C++*/
  uivector tablepos1, tablepos2;
  unsigned pos, i, error = 0;

  vector_init(&table, sizeof(uivector));
  if(!vector_resize(&table, HASH_NUM_VALUES)) return 9917;
  for(i = 0; i < HASH_NUM_VALUES; i++)
  {
    uivector* v = (uivector*)vector_get(&table, i);
    uivector_init(v);
  }

  /*remember start and end positions in the tables to searching in*/
  uivector_init(&tablepos1);
  uivector_init(&tablepos2);
  if(!uivector_resizev(&tablepos1, HASH_NUM_VALUES, 0)) error = 9918;
  if(!uivector_resizev(&tablepos2, HASH_NUM_VALUES, 0)) error = 9919;

  if(!error)
  {
    for(pos = 0; pos < size; pos++)
    {
      unsigned length = 0, offset = 0; /*the length and offset found for the current position*/
      unsigned max_offset = pos < windowSize ? pos : windowSize; /*how far back to test*/
      unsigned tablepos;

      /*/search for the longest string*/
      /*first find out where in the table to start (the first value that is in the range from "pos - max_offset" to "pos")*/
      unsigned hash = getHash(in, size, pos);
      if(!uivector_push_back((uivector*)vector_get(&table, hash), pos))  { error = 9920; break; }

      while(((uivector*)vector_get(&table, hash))->data[tablepos1.data[hash]] < pos - max_offset) tablepos1.data[hash]++; /*it now points to the first value in the table for which the index is larger than or equal to pos - max_offset*/
      while(((uivector*)vector_get(&table, hash))->data[tablepos2.data[hash]] < pos) tablepos2.data[hash]++; /*it now points to the first value in the table for which the index is larger than or equal to pos*/

      for(tablepos = tablepos2.data[hash] - 1; tablepos >= tablepos1.data[hash] && tablepos < tablepos2.data[hash]; tablepos--)
      {
        unsigned backpos = ((uivector*)vector_get(&table, hash))->data[tablepos];
        unsigned current_offset = pos - backpos;

        /*test the next characters*/
        unsigned current_length = 0;
        unsigned backtest = backpos;
        unsigned foretest = pos;
        while(foretest < size && in[backtest] == in[foretest] && current_length < MAX_SUPPORTED_DEFLATE_LENGTH) /*maximum support length by deflate is max length*/
        {
          if(backpos >= pos) backpos -= current_offset; /*continue as if we work on the decoded bytes after pos by jumping back before pos*/
          current_length++;
          backtest++;
          foretest++;
        }
        if(current_length > length)
        {
          length = current_length; /*the longest length*/
          offset = current_offset; /*the offset that is related to this longest length*/
          if(current_length == MAX_SUPPORTED_DEFLATE_LENGTH) break; /*you can jump out of this for loop once a length of max length is found (gives significant speed gain)*/
        }
      }

      /**encode it as length/distance pair or literal value**/
      if(length < 3) /*only lengths of 3 or higher are supported as length/distance pair*/
      {
        if(!uivector_push_back(out, in[pos])) { error = 9921; break; }
      }
      else
      {
        unsigned j;
        addLengthDistance(out, length, offset);
        for(j = 0; j < length - 1; j++)
        {
          pos++;
          if(!uivector_push_back((uivector*)vector_get(&table, getHash(in, size, pos)), pos)) { error = 9922; break; }
        }
      }
    } /*end of the loop through each character of input*/
  } /*end of "if(!error)"*/

  /*cleanup*/
  for(i = 0; i < table.size; i++)
  {
    uivector* v = (uivector*)vector_get(&table, i);
    uivector_cleanup(v);
  }
  vector_cleanup(&table);
  uivector_cleanup(&tablepos1);
  uivector_cleanup(&tablepos2);
  return error;
}
#endif

/* /////////////////////////////////////////////////////////////////////////// */

static unsigned deflateNoCompression(ucvector* out, const unsigned char* data, size_t datasize)
{
  /*non compressed deflate block data: 1 bit BFINAL,2 bits BTYPE,(5 bits): it jumps to start of next byte, 2 bytes LEN, 2 bytes NLEN, LEN bytes literal DATA*/

  size_t i, j, numdeflateblocks = datasize / 65536 + 1;
  unsigned datapos = 0;
  for(i = 0; i < numdeflateblocks; i++)
  {
    unsigned BFINAL, BTYPE, LEN, NLEN;
    unsigned char firstbyte;

    BFINAL = (i == numdeflateblocks - 1);
    BTYPE = 0;

    firstbyte = (unsigned char)(BFINAL + ((BTYPE & 1) << 1) + ((BTYPE & 2) << 1));
    ucvector_push_back(out, firstbyte);

    LEN = 65535;
    if(datasize - datapos < 65535) LEN = (unsigned)datasize - datapos;
    NLEN = 65535 - LEN;

    ucvector_push_back(out, (unsigned char)(LEN % 256));
    ucvector_push_back(out, (unsigned char)(LEN / 256));
    ucvector_push_back(out, (unsigned char)(NLEN % 256));
    ucvector_push_back(out, (unsigned char)(NLEN / 256));

    /*Decompressed data*/
    for(j = 0; j < 65535 && datapos < datasize; j++)
    {
      ucvector_push_back(out, data[datapos++]);
    }
  }

  return 0;
}

/*write the encoded data, using lit/len as well as distance codes*/
static void writeLZ77data(size_t* bp, ucvector* out, const uivector* lz77_encoded, const HuffmanTree* codes, const HuffmanTree* codesD)
{
  size_t i = 0;
  for(i = 0; i < lz77_encoded->size; i++)
  {
    unsigned val = lz77_encoded->data[i];
    addHuffmanSymbol(bp, out, HuffmanTree_getCode(codes, val), HuffmanTree_getLength(codes, val));
    if(val > 256) /*for a length code, 3 more things have to be added*/
    {
      unsigned length_index = val - FIRST_LENGTH_CODE_INDEX;
      unsigned n_length_extra_bits = LENGTHEXTRA[length_index];
      unsigned length_extra_bits = lz77_encoded->data[++i];

      unsigned distance_code = lz77_encoded->data[++i];

      unsigned distance_index = distance_code;
      unsigned n_distance_extra_bits = DISTANCEEXTRA[distance_index];
      unsigned distance_extra_bits = lz77_encoded->data[++i];

      addBitsToStream(bp, out, length_extra_bits, n_length_extra_bits);
      addHuffmanSymbol(bp, out, HuffmanTree_getCode(codesD, distance_code), HuffmanTree_getLength(codesD, distance_code));
      addBitsToStream(bp, out, distance_extra_bits, n_distance_extra_bits);
    }
  }
}

static unsigned deflateDynamic(ucvector* out, const unsigned char* data, size_t datasize, const LodeZlib_DeflateSettings* settings)
{
  /*
  after the BFINAL and BTYPE, the dynamic block consists out of the following:
  - 5 bits HLIT, 5 bits HDIST, 4 bits HCLEN
  - (HCLEN+4)*3 bits code lengths of code length alphabet
  - HLIT + 257 code lengths of lit/length alphabet (encoded using the code length alphabet, + possible repetition codes 16, 17, 18)
  - HDIST + 1 code lengths of distance alphabet (encoded using the code length alphabet, + possible repetition codes 16, 17, 18)
  - compressed data
  - 256 (end code)
  */

  unsigned error = 0;

  uivector lz77_encoded;
  HuffmanTree codes; /*tree for literal values and length codes*/
  HuffmanTree codesD; /*tree for distance codes*/
  HuffmanTree codelengthcodes;
  uivector frequencies;
  uivector frequenciesD;
  uivector amounts; /*the amounts in the "normal" order*/
  uivector lldl;
  uivector lldll; /*lit/len & dist code lengths*/
  uivector clcls;

  unsigned BFINAL = 1; /*make only one block... the first and final one*/
  size_t numcodes, numcodesD, i, bp = 0; /*the bit pointer*/
  unsigned HLIT, HDIST, HCLEN;

  uivector_init(&lz77_encoded);
  HuffmanTree_init(&codes);
  HuffmanTree_init(&codesD);
  HuffmanTree_init(&codelengthcodes);
  uivector_init(&frequencies);
  uivector_init(&frequenciesD);
  uivector_init(&amounts);
  uivector_init(&lldl);
  uivector_init(&lldll);
  uivector_init(&clcls);

  while(!error) /*the goto-avoiding while construct: break out to go to the cleanup phase, a break at the end makes sure the while is never repeated*/
  {
    if(settings->useLZ77)
    {
      error = encodeLZ77(&lz77_encoded, data, datasize, settings->windowSize); /*LZ77 encoded*/
      if(error) break;
    }
    else
    {
      if(!uivector_resize(&lz77_encoded, datasize)) { error = 9923; break; }
      for(i = 0; i < datasize; i++) lz77_encoded.data[i] = data[i]; /*no LZ77, but still will be Huffman compressed*/
    }

    if(!uivector_resizev(&frequencies, 286, 0)) { error = 9924; break; }
    if(!uivector_resizev(&frequenciesD, 30, 0)) { error = 9925; break; }
    for(i = 0; i < lz77_encoded.size; i++)
    {
      unsigned symbol = lz77_encoded.data[i];
      frequencies.data[symbol]++;
      if(symbol > 256)
      {
        unsigned dist = lz77_encoded.data[i + 2];
        frequenciesD.data[dist]++;
        i += 3;
      }
    }
    frequencies.data[256] = 1; /*there will be exactly 1 end code, at the end of the block*/

    error = HuffmanTree_makeFromFrequencies(&codes, frequencies.data, frequencies.size, 15);
    if(error) break;
    error = HuffmanTree_makeFromFrequencies(&codesD, frequenciesD.data, frequenciesD.size, 15);
    if(error) break;

    addBitToStream(&bp, out, BFINAL);
    addBitToStream(&bp, out, 0); /*first bit of BTYPE "dynamic"*/
    addBitToStream(&bp, out, 1); /*second bit of BTYPE "dynamic"*/

    numcodes = codes.numcodes; if(numcodes > 286) numcodes = 286;
    numcodesD = codesD.numcodes; if(numcodesD > 30) numcodesD = 30;
    for(i = 0; i < numcodes; i++) uivector_push_back(&lldll, HuffmanTree_getLength(&codes, (unsigned)i));
    for(i = 0; i < numcodesD; i++) uivector_push_back(&lldll, HuffmanTree_getLength(&codesD, (unsigned)i));

    /*make lldl smaller by using repeat codes 16 (copy length 3-6 times), 17 (3-10 zeros), 18 (11-138 zeros)*/
    for(i = 0; i < (unsigned)lldll.size; i++)
    {
      unsigned j = 0;
      while(i + j + 1 < (unsigned)lldll.size && lldll.data[i + j + 1] == lldll.data[i]) j++;

      if(lldll.data[i] == 0 && j >= 2)
      {
        j++; /*include the first zero*/
        if(j <= 10) { uivector_push_back(&lldl, 17); uivector_push_back(&lldl, j - 3); }
        else
        {
          if(j > 138) j = 138;
          uivector_push_back(&lldl, 18); uivector_push_back(&lldl, j - 11);
        }
        i += (j - 1);
      }
      else if(j >= 3)
      {
        size_t k;
        unsigned num = j / 6, rest = j % 6;
        uivector_push_back(&lldl, lldll.data[i]);
        for(k = 0; k < num; k++) { uivector_push_back(&lldl, 16); uivector_push_back(&lldl,    6 - 3); }
        if(rest >= 3)            { uivector_push_back(&lldl, 16); uivector_push_back(&lldl, rest - 3); }
        else j -= rest;
        i += j;
      }
      else uivector_push_back(&lldl, lldll.data[i]);
    }

    /*generate huffmantree for the length codes of lit/len and dist codes*/
    if(!uivector_resizev(&amounts, 19, 0)) { error = 9926; break; } /*16 possible lengths (0-15) and 3 repeat codes (16, 17 and 18)*/
    for(i = 0; i < lldl.size; i++)
    {
      amounts.data[lldl.data[i]]++;
      if(lldl.data[i] >= 16) i++; /*after a repeat code come the bits that specify the amount, those don't need to be in the amounts calculation*/
    }

    error = HuffmanTree_makeFromFrequencies(&codelengthcodes, amounts.data, amounts.size, 7);
    if(error) break;

    if(!uivector_resize(&clcls, 19)) { error = 9927; break; }
    for(i = 0; i < 19; i++) clcls.data[i] = HuffmanTree_getLength(&codelengthcodes, CLCL[i]); /*lengths of code length tree is in the order as specified by deflate*/
    while(clcls.data[clcls.size - 1] == 0 && clcls.size > 4)
    {
      if(!uivector_resize(&clcls, clcls.size - 1)) { error = 9928; break; } /*remove zeros at the end, but minimum size must be 4*/
    }
    if(error) break;

    /*write the HLIT, HDIST and HCLEN values*/
    HLIT = (unsigned)(numcodes - 257);
    HDIST = (unsigned)(numcodesD - 1);
    HCLEN = (unsigned)clcls.size - 4;
    addBitsToStream(&bp, out, HLIT, 5);
    addBitsToStream(&bp, out, HDIST, 5);
    addBitsToStream(&bp, out, HCLEN, 4);

    /*write the code lengths of the code length alphabet*/
    for(i = 0; i < HCLEN + 4; i++) addBitsToStream(&bp, out, clcls.data[i], 3);

    /*write the lengths of the lit/len AND the dist alphabet*/
    for(i = 0; i < lldl.size; i++)
    {
      addHuffmanSymbol(&bp, out, HuffmanTree_getCode(&codelengthcodes, lldl.data[i]), HuffmanTree_getLength(&codelengthcodes, lldl.data[i]));
      /*extra bits of repeat codes*/
      if(lldl.data[i] == 16) addBitsToStream(&bp, out, lldl.data[++i], 2);
      else if(lldl.data[i] == 17) addBitsToStream(&bp, out, lldl.data[++i], 3);
      else if(lldl.data[i] == 18) addBitsToStream(&bp, out, lldl.data[++i], 7);
    }

    /*write the compressed data symbols*/
    writeLZ77data(&bp, out, &lz77_encoded, &codes, &codesD);
    if(HuffmanTree_getLength(&codes, 256) == 0) { error = 64; break; } /*the length of the end code 256 must be larger than 0*/
    addHuffmanSymbol(&bp, out, HuffmanTree_getCode(&codes, 256), HuffmanTree_getLength(&codes, 256)); /*end code*/

    break; /*end of error-while*/
  }

  /*cleanup*/
  uivector_cleanup(&lz77_encoded);
  HuffmanTree_cleanup(&codes);
  HuffmanTree_cleanup(&codesD);
  HuffmanTree_cleanup(&codelengthcodes);
  uivector_cleanup(&frequencies);
  uivector_cleanup(&frequenciesD);
  uivector_cleanup(&amounts);
  uivector_cleanup(&lldl);
  uivector_cleanup(&lldll);
  uivector_cleanup(&clcls);

  return error;
}

static unsigned deflateFixed(ucvector* out, const unsigned char* data, size_t datasize, const LodeZlib_DeflateSettings* settings)
{
  HuffmanTree codes; /*tree for literal values and length codes*/
  HuffmanTree codesD; /*tree for distance codes*/

  unsigned BFINAL = 1; /*make only one block... the first and final one*/
  unsigned error = 0;
  size_t i, bp = 0; /*the bit pointer*/

  HuffmanTree_init(&codes);
  HuffmanTree_init(&codesD);

  generateFixedTree(&codes);
  generateDistanceTree(&codesD);

  addBitToStream(&bp, out, BFINAL);
  addBitToStream(&bp, out, 1); /*first bit of BTYPE*/
  addBitToStream(&bp, out, 0); /*second bit of BTYPE*/

  if(settings->useLZ77) /*LZ77 encoded*/
  {
    uivector lz77_encoded;
    uivector_init(&lz77_encoded);
    error = encodeLZ77(&lz77_encoded, data, datasize, settings->windowSize);
    if(!error) writeLZ77data(&bp, out, &lz77_encoded, &codes, &codesD);
    uivector_cleanup(&lz77_encoded);
  }
  else /*no LZ77, but still will be Huffman compressed*/
  {
    for(i = 0; i < datasize; i++) addHuffmanSymbol(&bp, out, HuffmanTree_getCode(&codes, data[i]), HuffmanTree_getLength(&codes, data[i]));
  }
  if(!error) addHuffmanSymbol(&bp, out, HuffmanTree_getCode(&codes, 256), HuffmanTree_getLength(&codes, 256)); /*"end" code*/

  /*cleanup*/
  HuffmanTree_cleanup(&codes);
  HuffmanTree_cleanup(&codesD);

  return error;
}

unsigned LodeFlate_deflate(ucvector* out, const unsigned char* data, size_t datasize, const LodeZlib_DeflateSettings* settings)
{
  unsigned error = 0;
  if(settings->btype == 0) error = deflateNoCompression(out, data, datasize);
  else if(settings->btype == 1) error = deflateFixed(out, data, datasize, settings);
  else if(settings->btype == 2) error = deflateDynamic(out, data, datasize, settings);
  else error = 61;
  return error;
}


/* ////////////////////////////////////////////////////////////////////////// */
/* / Adler32                                                                  */
/* ////////////////////////////////////////////////////////////////////////// */

static unsigned update_adler32(unsigned adler, const unsigned char* data, unsigned len)
{
   unsigned s1 = adler & 0xffff;
   unsigned s2 = (adler >> 16) & 0xffff;

  while(len > 0)
  {
    /*at least 5550 sums can be done before the sums overflow, saving us from a lot of module divisions*/
    unsigned amount = len > 5550 ? 5550 : len;
    len -= amount;
    while(amount > 0)
    {
      s1 = (s1 + *data++);
      s2 = (s2 + s1);
      amount--;
    }
    s1 %= 65521;
    s2 %= 65521;
  }

  return (s2 << 16) | s1;
}

/*Return the adler32 of the bytes data[0..len-1]*/
static unsigned adler32(const unsigned char* data, unsigned len)
{
  return update_adler32(1L, data, len);
}

/* ////////////////////////////////////////////////////////////////////////// */
/* / Reading and writing single bits and bytes from/to stream for Zlib      / */
/* ////////////////////////////////////////////////////////////////////////// */

static void LodeZlib_add32bitInt(ucvector* buffer, unsigned value)
{
  ucvector_push_back(buffer, (unsigned char)((value >> 24) & 0xff));
  ucvector_push_back(buffer, (unsigned char)((value >> 16) & 0xff));
  ucvector_push_back(buffer, (unsigned char)((value >>  8) & 0xff));
  ucvector_push_back(buffer, (unsigned char)((value      ) & 0xff));
}

/* ////////////////////////////////////////////////////////////////////////// */
/* / Zlib                                                                   / */
/* ////////////////////////////////////////////////////////////////////////// */

static unsigned LodeZlib_compress(unsigned char** out, size_t* outsize, const unsigned char* in, size_t insize, const LodeZlib_DeflateSettings* settings)
{
  /*initially, *out must be NULL and outsize 0, if you just give some random *out that's pointing to a non allocated buffer, this'll crash*/
  ucvector deflatedata, outv;
  size_t i;
  unsigned error;

  unsigned ADLER32;
  /*zlib data: 1 byte CMF (CM+CINFO), 1 byte FLG, deflate data, 4 byte ADLER32 checksum of the Decompressed data*/
  unsigned CMF = 120; /*0b01111000: CM 8, CINFO 7. With CINFO 7, any window size up to 32768 can be used.*/
  unsigned FLEVEL = 0;
  unsigned FDICT = 0;
  unsigned CMFFLG = 256 * CMF + FDICT * 32 + FLEVEL * 64;
  unsigned FCHECK = 31 - CMFFLG % 31;
  CMFFLG += FCHECK;

  ucvector_init_buffer(&outv, *out, *outsize); /*ucvector-controlled version of the output buffer, for dynamic array*/

  ucvector_push_back(&outv, (unsigned char)(CMFFLG / 256));
  ucvector_push_back(&outv, (unsigned char)(CMFFLG % 256));

  ucvector_init(&deflatedata);
  error = LodeFlate_deflate(&deflatedata, in, insize, settings);

  if(!error)
  {
    ADLER32 = adler32(in, (unsigned)insize);
    for(i = 0; i < deflatedata.size; i++) ucvector_push_back(&outv, deflatedata.data[i]);
    ucvector_cleanup(&deflatedata);
    LodeZlib_add32bitInt(&outv, ADLER32);
  }

  *out = outv.data;
  *outsize = outv.size;

  return error;
}

/* ////////////////////////////////////////////////////////////////////////// */

void LodeZlib_DeflateSettings_init(LodeZlib_DeflateSettings* settings)
{
  settings->btype = 2; /*compress with dynamic huffman tree (not in the mathematical sense, just not the predefined one)*/
  settings->useLZ77 = 1;
  settings->windowSize = 2048; /*this is a good tradeoff between speed and compression ratio*/
}

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* // End of Zlib related code, now comes the PNG related code that uses it// */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */

/*
The two functions below (LodePNG_decompress and LodePNG_compress) directly call the
LodeZlib_decompress and LodeZlib_compress functions. The only purpose of the functions
below, is to provide the ability to let LodePNG use a different Zlib encoder by only
changing the two functions below, instead of changing it inside the various places
in the other LodePNG functions.

*out must be NULL and *outsize must be 0 initially, and after the function is done,
*out must point to the decompressed data, *outsize must be the size of it, and must
be the size of the useful data in bytes, not the alloc size.
*/

static unsigned LodePNG_compress(unsigned char** out, size_t* outsize, const unsigned char* in, size_t insize, const LodeZlib_DeflateSettings* settings)
{
  return LodeZlib_compress(out, outsize, in, insize, settings);
}

/* ////////////////////////////////////////////////////////////////////////// */
/* / CRC32                                                                  / */
/* ////////////////////////////////////////////////////////////////////////// */

static unsigned Crc32_crc_table_computed = 0;
static unsigned Crc32_crc_table[256];

/*Make the table for a fast CRC.*/
static void Crc32_make_crc_table(void)
{
  unsigned int c, k, n;
  for(n = 0; n < 256; n++)
  {
    c = n;
    for(k = 0; k < 8; k++)
    {
      if(c & 1) c = (unsigned int)(0xedb88320L ^ (c >> 1));
      else c = c >> 1;
    }
    Crc32_crc_table[n] = c;
  }
  Crc32_crc_table_computed = 1;
}

/*Update a running CRC with the bytes buf[0..len-1]--the CRC should be
initialized to all 1's, and the transmitted value is the 1's complement of the
final running CRC (see the crc() routine below).*/
static unsigned Crc32_update_crc(const unsigned char* buf, unsigned int crc, size_t len)
{
  unsigned int c = crc;
  size_t n;

  if(!Crc32_crc_table_computed) Crc32_make_crc_table();
  for(n = 0; n < len; n++)
  {
    c = Crc32_crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
  }
  return c;
}

/*Return the CRC of the bytes buf[0..len-1].*/
static unsigned Crc32_crc(const unsigned char* buf, size_t len)
{
  return Crc32_update_crc(buf, 0xffffffffu, len) ^ 0xffffffffu;
}

/* ////////////////////////////////////////////////////////////////////////// */
/* / Reading and writing single bits and bytes from/to stream for LodePNG   / */
/* ////////////////////////////////////////////////////////////////////////// */

static unsigned char readBitFromReversedStream(size_t* bitpointer, const unsigned char* bitstream)
{
  unsigned char result = (unsigned char)((bitstream[(*bitpointer) >> 3] >> (7 - ((*bitpointer) & 0x7))) & 1);
  (*bitpointer)++;
  return result;
}

static unsigned readBitsFromReversedStream(size_t* bitpointer, const unsigned char* bitstream, size_t nbits)
{
  unsigned result = 0;
  size_t i;
  for(i = nbits - 1; i < nbits; i--) result += (unsigned)readBitFromReversedStream(bitpointer, bitstream) << i;
  return result;
}

static void setBitOfReversedStream(size_t* bitpointer, unsigned char* bitstream, unsigned char bit)
{
  /*the current bit in bitstream may be 0 or 1 for this to work*/
  if(bit == 0) bitstream[(*bitpointer) >> 3] &=  (unsigned char)(~(1 << (7 - ((*bitpointer) & 0x7))));
  else bitstream[(*bitpointer) >> 3] |=  (1 << (7 - ((*bitpointer) & 0x7)));
  (*bitpointer)++;
}

static unsigned LodePNG_read32bitInt(const unsigned char* buffer)
{
  return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

static void LodePNG_set32bitInt(unsigned char* buffer, unsigned value) /*buffer must have at least 4 allocated bytes available*/
{
  buffer[0] = (unsigned char)((value >> 24) & 0xff);
  buffer[1] = (unsigned char)((value >> 16) & 0xff);
  buffer[2] = (unsigned char)((value >>  8) & 0xff);
  buffer[3] = (unsigned char)((value      ) & 0xff);
}

static void LodePNG_add32bitInt(ucvector* buffer, unsigned value)
{
  ucvector_resize(buffer, buffer->size + 4);
  LodePNG_set32bitInt(&buffer->data[buffer->size - 4], value);
}

/* ////////////////////////////////////////////////////////////////////////// */
/* / PNG chunks                                                             / */
/* ////////////////////////////////////////////////////////////////////////// */

static unsigned LodePNG_chunk_length(const unsigned char* chunk) /*get the length of the data of the chunk. Total chunk length has 12 bytes more.*/
{
  return LodePNG_read32bitInt(&chunk[0]);
}

static void LodePNG_chunk_generate_crc(unsigned char* chunk) /*generates the correct CRC from the data and puts it in the last 4 bytes of the chunk*/
{
  unsigned length = LodePNG_chunk_length(chunk);
  unsigned CRC = Crc32_crc(&chunk[4], length + 4);
  LodePNG_set32bitInt(chunk + 8 + length, CRC);
}

static unsigned LodePNG_create_chunk(unsigned char** out, size_t* outlength, unsigned length, const char* type, const unsigned char* data) /*appends new chunk to out. Returns error code; may change memory address of out buffer*/
{
  unsigned i;
  unsigned char *chunk, *new_buffer;
  size_t new_length = (*outlength) + length + 12;
  if(new_length < length + 12 || new_length < (*outlength)) return 77; /*integer overflow happened*/
  new_buffer = (unsigned char*)realloc(*out, new_length);
  if(!new_buffer) return 9930;
  (*out) = new_buffer;
  (*outlength) = new_length;
  chunk = &(*out)[(*outlength) - length - 12];

  /*1: length*/
  LodePNG_set32bitInt(chunk, (unsigned)length);

  /*2: chunk name (4 letters)*/
  chunk[4] = type[0];
  chunk[5] = type[1];
  chunk[6] = type[2];
  chunk[7] = type[3];

  /*3: the data*/
  for(i = 0; i < length; i++) chunk[8 + i] = data[i];

  /*4: CRC (of the chunkname characters and the data)*/
  LodePNG_chunk_generate_crc(chunk);

  return 0;
}

/* ////////////////////////////////////////////////////////////////////////// */
/* / Color types and such                                                   / */
/* ////////////////////////////////////////////////////////////////////////// */

/*return type is a LodePNG error code*/
static unsigned checkColorValidity(unsigned colorType, unsigned bd) /*bd = bitDepth*/
{
  switch(colorType)
  {
    case 0: if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8 || bd == 16)) return 37; break; /*grey*/
    case 2: if(!(                                 bd == 8 || bd == 16)) return 37; break; /*RGB*/
    case 3: if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8            )) return 37; break; /*palette*/
    case 4: if(!(                                 bd == 8 || bd == 16)) return 37; break; /*grey + alpha*/
    case 6: if(!(                                 bd == 8 || bd == 16)) return 37; break; /*RGBA*/
    default: return 31;
  }
  return 0; /*allowed color type / bits combination*/
}

static unsigned getNumColorChannels(unsigned colorType)
{
  switch(colorType)
  {
    case 0: return 1; /*grey*/
    case 2: return 3; /*RGB*/
    case 3: return 1; /*palette*/
    case 4: return 2; /*grey + alpha*/
    case 6: return 4; /*RGBA*/
  }
  return 0; /*nonexistent color type*/
}

static unsigned getBpp(unsigned colorType, unsigned bitDepth)
{
  return getNumColorChannels(colorType) * bitDepth; /*bits per pixel is amount of channels * bits per channel*/
}

/* ////////////////////////////////////////////////////////////////////////// */

static void LodePNG_InfoColor_init(LodePNG_InfoColor* info)
{
  info->key_defined = 0;
  info->key_r = info->key_g = info->key_b = 0;
  info->colorType = 6;
  info->bitDepth = 8;
  info->palette = 0;
  info->palettesize = 0;
}

static void LodePNG_InfoColor_cleanup(LodePNG_InfoColor* info)
{
  LodePNG_InfoColor_clearPalette(info);
}

static void LodePNG_InfoColor_clearPalette(LodePNG_InfoColor* info)
{
  if(info->palette) free(info->palette);
  info->palettesize = 0;
}

unsigned LodePNG_InfoColor_addPalette(LodePNG_InfoColor* info, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  unsigned char* data;
  /*the same resize technique as C++ std::vectors is used, and here it's made so that for a palette with the max of 256 colors, it'll have the exact alloc size*/
  if(!(info->palettesize & (info->palettesize - 1))) /*if palettesize is 0 or a power of two*/
  {
    /*allocated data must be at least 4* palettesize (for 4 color bytes)*/
    size_t alloc_size = info->palettesize == 0 ? 4 : info->palettesize * 4 * 2;
    data = (unsigned char*)realloc(info->palette, alloc_size);
    if(!data) return 9931;
    else info->palette = data;
  }
  info->palette[4 * info->palettesize + 0] = r;
  info->palette[4 * info->palettesize + 1] = g;
  info->palette[4 * info->palettesize + 2] = b;
  info->palette[4 * info->palettesize + 3] = a;
  info->palettesize++;
  return 0;
}

static unsigned LodePNG_InfoColor_getBpp(const LodePNG_InfoColor* info) { return getBpp(info->colorType, info->bitDepth); } /*calculate bits per pixel out of colorType and bitDepth*/
static unsigned LodePNG_InfoColor_isGreyscaleType(const LodePNG_InfoColor* info) { return info->colorType == 0 || info->colorType == 4; }
static unsigned LodePNG_InfoColor_isAlphaType(const LodePNG_InfoColor* info) { return (info->colorType & 4) != 0; }

static unsigned LodePNG_InfoColor_equal(const LodePNG_InfoColor* info1, const LodePNG_InfoColor* info2)
{
  return info1->colorType == info2->colorType
      && info1->bitDepth  == info2->bitDepth; /*palette and color key not compared*/
}


static void LodePNG_InfoPng_init(LodePNG_InfoPng* info)
{
  info->width = info->height = 0;
  LodePNG_InfoColor_init(&info->color);
  info->interlaceMethod = 0;
  info->compressionMethod = 0;
  info->filterMethod = 0;
}

static void LodePNG_InfoPng_cleanup(LodePNG_InfoPng* info)
{
  LodePNG_InfoColor_cleanup(&info->color);
}

static unsigned LodePNG_InfoPng_copy(LodePNG_InfoPng* dest, const LodePNG_InfoPng* source)
{
  unsigned error = 0;
  LodePNG_InfoPng_cleanup(dest);
  *dest = *source;
  LodePNG_InfoColor_init(&dest->color);
  error = LodePNG_InfoColor_copy(&dest->color, &source->color); if(error) return error;
  return error;
}

static unsigned LodePNG_InfoColor_copy(LodePNG_InfoColor* dest, const LodePNG_InfoColor* source)
{
  size_t i;
  LodePNG_InfoColor_cleanup(dest);
  *dest = *source;
  dest->palette = (unsigned char*)malloc(source->palettesize * 4);
  if(!dest->palette && source->palettesize) return 9935;
  for(i = 0; i < source->palettesize * 4; i++) dest->palette[i] = source->palette[i];
  return 0;
}

static void LodePNG_InfoRaw_init(LodePNG_InfoRaw* info)
{
  LodePNG_InfoColor_init(&info->color);
}

static void LodePNG_InfoRaw_cleanup(LodePNG_InfoRaw* info)
{
  LodePNG_InfoColor_cleanup(&info->color);
}

static unsigned LodePNG_InfoRaw_copy(LodePNG_InfoRaw* dest, const LodePNG_InfoRaw* source)
{
  unsigned error = 0;
  LodePNG_InfoRaw_cleanup(dest);
  *dest = *source;
  LodePNG_InfoColor_init(&dest->color);
  error = LodePNG_InfoColor_copy(&dest->color, &source->color);
  return error;
}

/* ////////////////////////////////////////////////////////////////////////// */

/*
converts from any color type to 24-bit or 32-bit (later maybe more supported). return value = LodePNG error code
the out buffer must have (w * h * bpp + 7) / 8 bytes, where bpp is the bits per pixel of the output color type (LodePNG_InfoColor_getBpp)
for < 8 bpp images, there may _not_ be padding bits at the end of scanlines.
*/
static unsigned LodePNG_convert(unsigned char* out, const unsigned char* in, LodePNG_InfoColor* infoOut, LodePNG_InfoColor* infoIn, unsigned w, unsigned h)
{
  const size_t numpixels = w * h; /*amount of pixels*/
  const unsigned OUT_BYTES = LodePNG_InfoColor_getBpp(infoOut) / 8; /*bytes per pixel in the output image*/
  const unsigned OUT_ALPHA = LodePNG_InfoColor_isAlphaType(infoOut); /*use 8-bit alpha channel*/
  size_t i, c, bp = 0; /*bitpointer, used by less-than-8-bit color types*/

  /*cases where in and out already have the same format*/
  if(LodePNG_InfoColor_equal(infoIn, infoOut))
  {
    size_t i, size = (w * h * LodePNG_InfoColor_getBpp(infoIn) + 7) / 8;
    for(i = 0; i < size; i++) out[i] = in[i];
    return 0;
  }

  if((infoOut->colorType == 2 || infoOut->colorType == 6) && infoOut->bitDepth == 8)
  {
    if(infoIn->bitDepth == 8)
    {
      switch(infoIn->colorType)
      {
        case 0: /*greyscale color*/
          for(i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out[OUT_BYTES * i + 3] = 255;
            out[OUT_BYTES * i + 0] = out[OUT_BYTES * i + 1] = out[OUT_BYTES * i + 2] = in[i];
            if(OUT_ALPHA && infoIn->key_defined && in[i] == infoIn->key_r) out[OUT_BYTES * i + 3] = 0;
          }
        break;
        case 2: /*RGB color*/
          for(i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out[OUT_BYTES * i + 3] = 255;
            for(c = 0; c < 3; c++) out[OUT_BYTES * i + c] = in[3 * i + c];
            if(OUT_ALPHA && infoIn->key_defined == 1 && in[3 * i + 0] == infoIn->key_r && in[3 * i + 1] == infoIn->key_g && in[3 * i + 2] == infoIn->key_b) out[OUT_BYTES * i + 3] = 0;
          }
        break;
        case 3: /*indexed color (palette)*/
          for(i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out[OUT_BYTES * i + 3] = 255;
            if(in[i] >= infoIn->palettesize) return 46;
            for(c = 0; c < OUT_BYTES; c++) out[OUT_BYTES * i + c] = infoIn->palette[4 * in[i] + c]; /*get rgb colors from the palette*/
          }
        break;
        case 4: /*greyscale with alpha*/
          for(i = 0; i < numpixels; i++)
          {
            out[OUT_BYTES * i + 0] = out[OUT_BYTES * i + 1] = out[OUT_BYTES * i + 2] = in[2 * i + 0];
            if(OUT_ALPHA) out[OUT_BYTES * i + 3] = in[2 * i + 1];
          }
        break;
        case 6: /*RGB with alpha*/
          for(i = 0; i < numpixels; i++)
          {
            for(c = 0; c < OUT_BYTES; c++) out[OUT_BYTES * i + c] = in[4 * i + c];
          }
        break;
        default: break;
      }
    }
    else if(infoIn->bitDepth == 16)
    {
      switch(infoIn->colorType)
      {
        case 0: /*greyscale color*/
          for(i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out[OUT_BYTES * i + 3] = 255;
            out[OUT_BYTES * i + 0] = out[OUT_BYTES * i + 1] = out[OUT_BYTES * i + 2] = in[2 * i];
            if(OUT_ALPHA && infoIn->key_defined && 256U * in[i] + in[i + 1] == infoIn->key_r) out[OUT_BYTES * i + 3] = 0;
          }
        break;
        case 2: /*RGB color*/
          for(i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out[OUT_BYTES * i + 3] = 255;
            for(c = 0; c < 3; c++) out[OUT_BYTES * i + c] = in[6 * i + 2 * c];
            if(OUT_ALPHA && infoIn->key_defined && 256U * in[6 * i + 0] + in[6 * i + 1] == infoIn->key_r && 256U * in[6 * i + 2] + in[6 * i + 3] == infoIn->key_g && 256U * in[6 * i + 4] + in[6 * i + 5] == infoIn->key_b) out[OUT_BYTES * i + 3] = 0;
          }
        break;
        case 4: /*greyscale with alpha*/
          for(i = 0; i < numpixels; i++)
          {
            out[OUT_BYTES * i + 0] = out[OUT_BYTES * i + 1] = out[OUT_BYTES * i + 2] = in[4 * i]; /*most significant byte*/
            if(OUT_ALPHA) out[OUT_BYTES * i + 3] = in[4 * i + 2];
          }
        break;
        case 6: /*RGB with alpha*/
          for(i = 0; i < numpixels; i++)
          {
            for(c = 0; c < OUT_BYTES; c++) out[OUT_BYTES * i + c] = in[8 * i + 2 * c];
          }
          break;
        default: break;
      }
    }
    else /*infoIn->bitDepth is less than 8 bit per channel*/
    {
      switch(infoIn->colorType)
      {
        case 0: /*greyscale color*/
          for(i = 0; i < numpixels; i++)
          {
            unsigned value = readBitsFromReversedStream(&bp, in, infoIn->bitDepth);
            if(OUT_ALPHA) out[OUT_BYTES * i + 3] = 255;
            if(OUT_ALPHA && infoIn->key_defined && value && ((1U << infoIn->bitDepth) - 1U) == infoIn->key_r && ((1U << infoIn->bitDepth) - 1U)) out[OUT_BYTES * i + 3] = 0;
            value = (value * 255) / ((1 << infoIn->bitDepth) - 1); /*scale value from 0 to 255*/
            out[OUT_BYTES * i + 0] = out[OUT_BYTES * i + 1] = out[OUT_BYTES * i + 2] = (unsigned char)(value);
          }
        break;
        case 3: /*indexed color (palette)*/
          for(i = 0; i < numpixels; i++)
          {
            unsigned value = readBitsFromReversedStream(&bp, in, infoIn->bitDepth);
            if(OUT_ALPHA) out[OUT_BYTES * i + 3] = 255;
            if(value >= infoIn->palettesize) return 47;
            for(c = 0; c < OUT_BYTES; c++) out[OUT_BYTES * i + c] = infoIn->palette[4 * value + c]; /*get rgb colors from the palette*/
          }
        break;
        default: break;
      }
    }
  }
  else if(LodePNG_InfoColor_isGreyscaleType(infoOut) && infoOut->bitDepth == 8) /*conversion from greyscale to greyscale*/
  {
    if(!LodePNG_InfoColor_isGreyscaleType(infoIn)) return 62;
    if(infoIn->bitDepth == 8)
    {
      switch(infoIn->colorType)
      {
        case 0: /*greyscale color*/
          for(i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out[OUT_BYTES * i + 1] = 255;
            out[OUT_BYTES * i] = in[i];
            if(OUT_ALPHA && infoIn->key_defined && in[i] == infoIn->key_r) out[OUT_BYTES * i + 1] = 0;
          }
        break;
        case 4: /*greyscale with alpha*/
          for(i = 0; i < numpixels; i++)
          {
            out[OUT_BYTES * i + 0] = in[2 * i + 0];
            if(OUT_ALPHA) out[OUT_BYTES * i + 1] = in[2 * i + 1];
          }
        break;
        default: return 31;
      }
    }
    else if(infoIn->bitDepth == 16)
    {
      switch(infoIn->colorType)
      {
        case 0: /*greyscale color*/
          for(i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out[OUT_BYTES * i + 1] = 255;
            out[OUT_BYTES * i] = in[2 * i];
            if(OUT_ALPHA && infoIn->key_defined && 256U * in[i] + in[i + 1] == infoIn->key_r) out[OUT_BYTES * i + 1] = 0;
          }
        break;
        case 4: /*greyscale with alpha*/
          for(i = 0; i < numpixels; i++)
          {
            out[OUT_BYTES * i] = in[4 * i]; /*most significant byte*/
            if(OUT_ALPHA) out[OUT_BYTES * i + 1] = in[4 * i + 2]; /*most significant byte*/
          }
        break;
        default: return 31;
      }
    }
    else /*infoIn->bitDepth is less than 8 bit per channel*/
    {
      if(infoIn->colorType != 0) return 31; /*colorType 0 is the only greyscale type with < 8 bits per channel*/
      for(i = 0; i < numpixels; i++)
      {
        unsigned value = readBitsFromReversedStream(&bp, in, infoIn->bitDepth);
        if(OUT_ALPHA) out[OUT_BYTES * i + 1] = 255;
        if(OUT_ALPHA && infoIn->key_defined && value && ((1U << infoIn->bitDepth) - 1U) == infoIn->key_r && ((1U << infoIn->bitDepth) - 1U)) out[OUT_BYTES * i + 1] = 0;
        value = (value * 255) / ((1 << infoIn->bitDepth) - 1); /*scale value from 0 to 255*/
        out[OUT_BYTES * i] = (unsigned char)(value);
      }
    }
  }
  else return 59;

  return 0;
}

/*Path predictor, used by PNG filter type 4*/
static int paethPredictor(int a, int b, int c)
{
  int p = a + b - c;
  int pa = p > a ? p - a : a - p;
  int pb = p > b ? p - b : b - p;
  int pc = p > c ? p - c : c - p;

  if(pa <= pb && pa <= pc) return a;
  else if(pb <= pc) return b;
  else return c;
}

/*shared values used by multiple Adam7 related functions*/

static const unsigned ADAM7_IX[7] = { 0, 4, 0, 2, 0, 1, 0 }; /*x start values*/
static const unsigned ADAM7_IY[7] = { 0, 0, 4, 0, 2, 0, 1 }; /*y start values*/
static const unsigned ADAM7_DX[7] = { 8, 8, 4, 4, 2, 2, 1 }; /*x delta values*/
static const unsigned ADAM7_DY[7] = { 8, 8, 8, 4, 4, 2, 2 }; /*y delta values*/

static void Adam7_getpassvalues(unsigned passw[7], unsigned passh[7], size_t filter_passstart[8], size_t padded_passstart[8], size_t passstart[8], unsigned w, unsigned h, unsigned bpp)
{
  /*the passstart values have 8 values: the 8th one actually indicates the byte after the end of the 7th (= last) pass*/
  unsigned i;

  /*calculate width and height in pixels of each pass*/
  for(i = 0; i < 7; i++)
  {
    passw[i] = (w + ADAM7_DX[i] - ADAM7_IX[i] - 1) / ADAM7_DX[i];
    passh[i] = (h + ADAM7_DY[i] - ADAM7_IY[i] - 1) / ADAM7_DY[i];
    if(passw[i] == 0) passh[i] = 0;
    if(passh[i] == 0) passw[i] = 0;
  }

  filter_passstart[0] = padded_passstart[0] = passstart[0] = 0;
  for(i = 0; i < 7; i++)
  {
    filter_passstart[i + 1] = filter_passstart[i] + ((passw[i] && passh[i]) ? passh[i] * (1 + (passw[i] * bpp + 7) / 8) : 0); /*if passw[i] is 0, it's 0 bytes, not 1 (no filtertype-byte)*/
    padded_passstart[i + 1] = padded_passstart[i] + passh[i] * ((passw[i] * bpp + 7) / 8); /*bits padded if needed to fill full byte at end of each scanline*/
    passstart[i + 1] = passstart[i] + (passh[i] * passw[i] * bpp + 7) / 8; /*only padded at end of reduced image*/
  }
}


/* ////////////////////////////////////////////////////////////////////////// */
/* / PNG Encoder                                                            / */
/* ////////////////////////////////////////////////////////////////////////// */

/*chunkName must be string of 4 characters*/
static unsigned addChunk(ucvector* out, const char* chunkName, const unsigned char* data, size_t length)
{
  unsigned error = LodePNG_create_chunk(&out->data, &out->size, (unsigned)length, chunkName, data);
  if(error) return error;
  out->allocsize = out->size; /*fix the allocsize again*/
  return 0;
}

static void writeSignature(ucvector* out)
{
  /*8 bytes PNG signature*/
  ucvector_push_back(out, 137);
  ucvector_push_back(out, 80);
  ucvector_push_back(out, 78);
  ucvector_push_back(out, 71);
  ucvector_push_back(out, 13);
  ucvector_push_back(out, 10);
  ucvector_push_back(out, 26);
  ucvector_push_back(out, 10);
}

static unsigned addChunk_IHDR(ucvector* out, unsigned w, unsigned h, unsigned bitDepth, unsigned colorType, unsigned interlaceMethod)
{
  unsigned error = 0;
  ucvector header;
  ucvector_init(&header);

  LodePNG_add32bitInt(&header, w); /*width*/
  LodePNG_add32bitInt(&header, h); /*height*/
  ucvector_push_back(&header, (unsigned char)bitDepth); /*bit depth*/
  ucvector_push_back(&header, (unsigned char)colorType); /*color type*/
  ucvector_push_back(&header, 0); /*compression method*/
  ucvector_push_back(&header, 0); /*filter method*/
  ucvector_push_back(&header, interlaceMethod); /*interlace method*/

  error = addChunk(out, "IHDR", header.data, header.size);
  ucvector_cleanup(&header);

  return error;
}

static unsigned addChunk_PLTE(ucvector* out, const LodePNG_InfoColor* info)
{
  unsigned error = 0;
  size_t i;
  ucvector PLTE;
  ucvector_init(&PLTE);
  for(i = 0; i < info->palettesize * 4; i++) if(i % 4 != 3) ucvector_push_back(&PLTE, info->palette[i]); /*add all channels except alpha channel*/
  error = addChunk(out, "PLTE", PLTE.data, PLTE.size);
  ucvector_cleanup(&PLTE);

  return error;
}

static unsigned addChunk_tRNS(ucvector* out, const LodePNG_InfoColor* info)
{
  unsigned error = 0;
  size_t i;
  ucvector tRNS;
  ucvector_init(&tRNS);
  if(info->colorType == 3)
  {
    for(i = 0; i < info->palettesize; i++) ucvector_push_back(&tRNS, info->palette[4 * i + 3]); /*add only alpha channel*/
  }
  else if(info->colorType == 0)
  {
    if(info->key_defined)
    {
      ucvector_push_back(&tRNS, (unsigned char)(info->key_r / 256));
      ucvector_push_back(&tRNS, (unsigned char)(info->key_r % 256));
    }
  }
  else if(info->colorType == 2)
  {
    if(info->key_defined)
    {
      ucvector_push_back(&tRNS, (unsigned char)(info->key_r / 256));
      ucvector_push_back(&tRNS, (unsigned char)(info->key_r % 256));
      ucvector_push_back(&tRNS, (unsigned char)(info->key_g / 256));
      ucvector_push_back(&tRNS, (unsigned char)(info->key_g % 256));
      ucvector_push_back(&tRNS, (unsigned char)(info->key_b / 256));
      ucvector_push_back(&tRNS, (unsigned char)(info->key_b % 256));
    }
  }

  error = addChunk(out, "tRNS", tRNS.data, tRNS.size);
  ucvector_cleanup(&tRNS);

  return error;
}

static unsigned addChunk_IDAT(ucvector* out, const unsigned char* data, size_t datasize, LodeZlib_DeflateSettings* zlibsettings)
{
  ucvector zlibdata;
  unsigned error = 0;

  /*compress with the Zlib compressor*/
  ucvector_init(&zlibdata);
  error = LodePNG_compress(&zlibdata.data, &zlibdata.size, data, datasize, zlibsettings);
  if(!error) error = addChunk(out, "IDAT", zlibdata.data, zlibdata.size);
  ucvector_cleanup(&zlibdata);

  return error;
}

static unsigned addChunk_IEND(ucvector* out)
{
  unsigned error = 0;
  error = addChunk(out, "IEND", 0, 0);
  return error;
}

static void filterScanline(unsigned char* out, const unsigned char* scanline, const unsigned char* prevline, size_t length, size_t bytewidth, unsigned char filterType)
{
  size_t i;
  switch(filterType)
  {
    case 0:
      for(i = 0; i < length; i++) out[i] = scanline[i];
      break;
    case 1:
      for(i =         0; i < bytewidth; i++) out[i] = scanline[i];
      for(i = bytewidth; i <    length; i++) out[i] = scanline[i] - scanline[i - bytewidth];
      break;
    case 2:
      if(prevline) for(i = 0; i < length; i++) out[i] = scanline[i] - prevline[i];
      else         for(i = 0; i < length; i++) out[i] = scanline[i];
      break;
    case 3:
      if(prevline)
      {
        for(i =         0; i < bytewidth; i++) out[i] = scanline[i] - prevline[i] / 2;
        for(i = bytewidth; i <    length; i++) out[i] = scanline[i] - ((scanline[i - bytewidth] + prevline[i]) / 2);
      }
      else
      {
        for(i =         0; i < length; i++) out[i] = scanline[i];
        for(i = bytewidth; i < length; i++) out[i] = scanline[i] - scanline[i - bytewidth] / 2;
      }
      break;
    case 4:
      if(prevline)
      {
        for(i =         0; i < bytewidth; i++) out[i] = (unsigned char)(scanline[i] - paethPredictor(0, prevline[i], 0));
        for(i = bytewidth; i <    length; i++) out[i] = (unsigned char)(scanline[i] - paethPredictor(scanline[i - bytewidth], prevline[i], prevline[i - bytewidth]));
      }
      else
      {
        for(i =         0; i < bytewidth; i++) out[i] = scanline[i];
        for(i = bytewidth; i <    length; i++) out[i] = (unsigned char)(scanline[i] - paethPredictor(scanline[i - bytewidth], 0, 0));
      }
      break;
  default: return; /*nonexistent filter type given*/
  }
}

static unsigned filter(unsigned char* out, const unsigned char* in, unsigned w, unsigned h, const LodePNG_InfoColor* info)
{
  /*
  For PNG filter method 0
  out must be a buffer with as size: h + (w * h * bpp + 7) / 8, because there are the scanlines with 1 extra byte per scanline

  There is a nice heuristic described here: http://www.cs.toronto.edu/~cosmin/pngtech/optipng.html. It says:
   *  If the image type is Palette, or the bit depth is smaller than 8, then do not filter the image (i.e. use fixed filtering, with the filter None).
   * (The other case) If the image type is Grayscale or RGB (with or without Alpha), and the bit depth is not smaller than 8, then use adaptive filtering heuristic as follows: independently for each row, apply all five filters and select the filter that produces the smallest sum of absolute values per row.

  Here the above method is used mostly. Note though that it appears to be better to use the adaptive filtering on the plasma 8-bit palette example, but that image isn't the best reference for palette images in general.
  */

  unsigned bpp = LodePNG_InfoColor_getBpp(info);
  size_t linebytes = (w * bpp + 7) / 8; /*the width of a scanline in bytes, not including the filter type*/
  size_t bytewidth = (bpp + 7) / 8; /*bytewidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise*/
  const unsigned char* prevline = 0;
  unsigned x, y;
  unsigned heuristic;
  unsigned error = 0;

  if(bpp == 0) return 31; /*invalid color type*/

  /*choose heuristic as described above*/
  if(info->colorType == 3 || info->bitDepth < 8) heuristic = 0;
  else heuristic = 1;

  if(heuristic == 0) /*None filtertype for everything*/
  {
    for(y = 0; y < h; y++)
    {
      size_t outindex = (1 + linebytes) * y; /*the extra filterbyte added to each row*/
      size_t inindex = linebytes * y;
      const unsigned TYPE = 0;
      out[outindex] = TYPE; /*filter type byte*/
      filterScanline(&out[outindex + 1], &in[inindex], prevline, linebytes, bytewidth, TYPE);
      prevline = &in[inindex];
    }
  }
  else if(heuristic == 1) /*adaptive filtering*/
  {
    size_t sum[5];
    ucvector attempt[5]; /*five filtering attempts, one for each filter type*/
    size_t smallest = 0;
    unsigned type, bestType = 0;

    for(type = 0; type < 5; type++) ucvector_init(&attempt[type]);
    for(type = 0; type < 5; type++)
    {
      if(!ucvector_resize(&attempt[type], linebytes)) { error = 9949; break; }
    }

    if(!error)
    {
      for(y = 0; y < h; y++)
      {
        /*try the 5 filter types*/
        for(type = 0; type < 5; type++)
        {
          filterScanline(attempt[type].data, &in[y * linebytes], prevline, linebytes, bytewidth, type);

          /*calculate the sum of the result*/
          sum[type] = 0;
          for(x = 0; x < attempt[type].size; x+=3) sum[type] += attempt[type].data[x]; /*note that not all pixels are checked to speed this up while still having probably the best choice*/

          /*check if this is smallest sum (or if type == 0 it's the first case so always store the values)*/
          if(type == 0 || sum[type] < smallest)
          {
            bestType = type;
            smallest = sum[type];
          }
        }

        prevline = &in[y * linebytes];

        /*now fill the out values*/
        out[y * (linebytes + 1)] = bestType; /*the first byte of a scanline will be the filter type*/
        for(x = 0; x < linebytes; x++) out[y * (linebytes + 1) + 1 + x] = attempt[bestType].data[x];
      }
    }

    for(type = 0; type < 5; type++) ucvector_cleanup(&attempt[type]);
  }

  return error;
}

static void addPaddingBits(unsigned char* out, const unsigned char* in, size_t olinebits, size_t ilinebits, unsigned h)
{
  /*The opposite of the removePaddingBits function
  olinebits must be >= ilinebits*/
  unsigned y;
  size_t diff = olinebits - ilinebits;
  size_t obp = 0, ibp = 0; /*bit pointers*/
  for(y = 0; y < h; y++)
  {
    size_t x;
    for(x = 0; x < ilinebits; x++)
    {
      unsigned char bit = readBitFromReversedStream(&ibp, in);
      setBitOfReversedStream(&obp, out, bit);
    }
    /*obp += diff; --> no, fill in some value in the padding bits too, to avoid "Use of uninitialised value of size ###" warning from valgrind*/
    for(x = 0; x < diff; x++) setBitOfReversedStream(&obp, out, 0);
  }
}

static void Adam7_interlace(unsigned char* out, const unsigned char* in, unsigned w, unsigned h, unsigned bpp)
{
  /*Note: this function works on image buffers WITHOUT padding bits at end of scanlines with non-multiple-of-8 bit amounts, only between reduced images is padding*/
  unsigned passw[7], passh[7]; size_t filter_passstart[8], padded_passstart[8], passstart[8];
  unsigned i;

  Adam7_getpassvalues(passw, passh, filter_passstart, padded_passstart, passstart, w, h, bpp);

  if(bpp >= 8)
  {
    for(i = 0; i < 7; i++)
    {
      unsigned x, y, b;
      size_t bytewidth = bpp / 8;
      for(y = 0; y < passh[i]; y++)
      for(x = 0; x < passw[i]; x++)
      {
        size_t pixelinstart = ((ADAM7_IY[i] + y * ADAM7_DY[i]) * w + ADAM7_IX[i] + x * ADAM7_DX[i]) * bytewidth;
        size_t pixeloutstart = passstart[i] + (y * passw[i] + x) * bytewidth;
        for(b = 0; b < bytewidth; b++)
        {
          out[pixeloutstart + b] = in[pixelinstart + b];
        }
      }
    }
  }
  else /*bpp < 8: Adam7 with pixels < 8 bit is a bit trickier: with bit pointers*/
  {
    for(i = 0; i < 7; i++)
    {
      unsigned x, y, b;
      unsigned ilinebits = bpp * passw[i];
      unsigned olinebits = bpp * w;
      size_t obp, ibp; /*bit pointers (for out and in buffer)*/
      for(y = 0; y < passh[i]; y++)
      for(x = 0; x < passw[i]; x++)
      {
        ibp = (ADAM7_IY[i] + y * ADAM7_DY[i]) * olinebits + (ADAM7_IX[i] + x * ADAM7_DX[i]) * bpp;
        obp = (8 * passstart[i]) + (y * ilinebits + x * bpp);
        for(b = 0; b < bpp; b++)
        {
          unsigned char bit = readBitFromReversedStream(&ibp, in);
          setBitOfReversedStream(&obp, out, bit);
        }
      }
    }
  }
}

/*out must be buffer big enough to contain uncompressed IDAT chunk data, and in must contain the full image*/
static unsigned preProcessScanlines(unsigned char** out, size_t* outsize, const unsigned char* in, const LodePNG_InfoPng* infoPng) /*return value is error*/
{
  /*
  This function converts the pure 2D image with the PNG's colortype, into filtered-padded-interlaced data. Steps:
  *) if no Adam7: 1) add padding bits (= possible extra bits per scanline if bpp < 8) 2) filter
  *) if adam7: 1) Adam7_interlace 2) 7x add padding bits 3) 7x filter
  */
  unsigned bpp = LodePNG_InfoColor_getBpp(&infoPng->color);
  unsigned w = infoPng->width;
  unsigned h = infoPng->height;
  unsigned error = 0;

  if(infoPng->interlaceMethod == 0)
  {
    *outsize = h + (h * ((w * bpp + 7) / 8)); /*image size plus an extra byte per scanline + possible padding bits*/
    *out = (unsigned char*)malloc(*outsize);
    if(!(*out) && (*outsize)) error = 9950;

    if(!error)
    {
      if(bpp < 8 && w * bpp != ((w * bpp + 7) / 8) * 8) /*non multiple of 8 bits per scanline, padding bits needed per scanline*/
      {
        ucvector padded;
        ucvector_init(&padded);
        if(!ucvector_resize(&padded, h * ((w * bpp + 7) / 8))) error = 9951;
        if(!error)
        {
          addPaddingBits(padded.data, in, ((w * bpp + 7) / 8) * 8, w * bpp, h);
          error = filter(*out, padded.data, w, h, &infoPng->color);
        }
        ucvector_cleanup(&padded);
      }
      else error = filter(*out, in, w, h, &infoPng->color); /*we can immediately filter into the out buffer, no other steps needed*/
    }
  }
  else /*interlaceMethod is 1 (Adam7)*/
  {
    unsigned char* adam7 = (unsigned char*)malloc((h * w * bpp + 7) / 8);
    if(!adam7 && ((h * w * bpp + 7) / 8)) error = 9952; /*malloc failed*/

    while(!error) /*not a real while loop, used to break out to cleanup to avoid a goto*/
    {
      unsigned passw[7], passh[7]; size_t filter_passstart[8], padded_passstart[8], passstart[8];
      unsigned i;

      Adam7_getpassvalues(passw, passh, filter_passstart, padded_passstart, passstart, w, h, bpp);

      *outsize = filter_passstart[7]; /*image size plus an extra byte per scanline + possible padding bits*/
      *out = (unsigned char*)malloc(*outsize);
      if(!(*out) && (*outsize)) { error = 9953; break; }

      Adam7_interlace(adam7, in, w, h, bpp);

      for(i = 0; i < 7; i++)
      {
        if(bpp < 8)
        {
          ucvector padded;
          ucvector_init(&padded);
          if(!ucvector_resize(&padded, h * ((w * bpp + 7) / 8))) error = 9954;
          if(!error)
          {
            addPaddingBits(&padded.data[padded_passstart[i]], &adam7[passstart[i]], ((passw[i] * bpp + 7) / 8) * 8, passw[i] * bpp, passh[i]);
            error = filter(&(*out)[filter_passstart[i]], &padded.data[padded_passstart[i]], passw[i], passh[i], &infoPng->color);
          }

          ucvector_cleanup(&padded);
        }
        else
        {
          error = filter(&(*out)[filter_passstart[i]], &adam7[padded_passstart[i]], passw[i], passh[i], &infoPng->color);
        }
      }

      break;
    }

    free(adam7);
  }

  return error;
}

/*palette must have 4 * palettesize bytes allocated*/
static unsigned isPaletteFullyOpaque(const unsigned char* palette, size_t palettesize) /*palette given in format RGBARGBARGBARGBA...*/
{
  size_t i;
  for(i = 0; i < palettesize; i++)
  {
    if(palette[4 * i + 3] != 255) return 0;
  }
  return 1;
}

/*this function checks if the input image given by the user has no transparent pixels*/
static unsigned isFullyOpaque(const unsigned char* image, unsigned w, unsigned h, const LodePNG_InfoColor* info)
{
  /*TODO: When the user specified a color key for the input image, then this function must also check for pixels that are the same as the color key and treat those as transparent.*/

  unsigned i, numpixels = w * h;
  if(info->colorType == 6)
  {
    if(info->bitDepth == 8)
    {
      for(i = 0; i < numpixels; i++) if(image[i * 4 + 3] != 255) return 0;
    }
    else
    {
      for(i = 0; i < numpixels; i++) if(image[i * 8 + 6] != 255 || image[i * 8 + 7] != 255) return 0;
    }
    return 1; /*no single pixel with alpha channel other than 255 found*/
  }
  else if(info->colorType == 4)
  {
    if(info->bitDepth == 8)
    {
      for(i = 0; i < numpixels; i++) if(image[i * 2 + 1] != 255) return 0;
    }
    else
    {
      for(i = 0; i < numpixels; i++) if(image[i * 4 + 2] != 255 || image[i * 4 + 3] != 255) return 0;
    }
    return 1; /*no single pixel with alpha channel other than 255 found*/
  }
  else if(info->colorType == 3)
  {
    /*when there's a palette, we could check every pixel for translucency, but much quicker is to just check the palette*/
    return(isPaletteFullyOpaque(info->palette, info->palettesize));
  }

  return 0; /*color type that isn't supported by this function yet, so assume there is transparency to be safe*/
}

void LodePNG_encode(LodePNG_Encoder* encoder, unsigned char** out, size_t* outsize, const unsigned char* image, unsigned w, unsigned h)
{
  LodePNG_InfoPng info;
  ucvector outv;
  unsigned char* data = 0; /*uncompressed version of the IDAT chunk data*/
  size_t datasize = 0;

  /*provide some proper output values if error will happen*/
  *out = 0;
  *outsize = 0;
  encoder->error = 0;

  info = encoder->infoPng; /*UNSAFE copy to avoid having to cleanup! but we will only change primitive parameters, and not invoke the cleanup function nor touch the palette's buffer so we use it safely*/
  info.width = w;
  info.height = h;

  if(encoder->settings.autoLeaveOutAlphaChannel && isFullyOpaque(image, w, h, &encoder->infoRaw.color))
  {
    /*go to a color type without alpha channel*/
    if(info.color.colorType == 6) info.color.colorType = 2;
    else if(info.color.colorType == 4) info.color.colorType = 0;
  }

  if(encoder->settings.zlibsettings.windowSize > 32768) { encoder->error = 60; return; } /*error: windowsize larger than allowed*/
  if(encoder->settings.zlibsettings.btype > 2) { encoder->error = 61; return; } /*error: nonexistent btype*/
  if(encoder->infoPng.interlaceMethod > 1) { encoder->error = 71; return; } /*error: nonexistent interlace mode*/
  if((encoder->error = checkColorValidity(info.color.colorType, info.color.bitDepth))) return; /*error: nonexistent color type given*/
  if((encoder->error = checkColorValidity(encoder->infoRaw.color.colorType, encoder->infoRaw.color.bitDepth))) return; /*error: nonexistent color type given*/

  if(!LodePNG_InfoColor_equal(&encoder->infoRaw.color, &info.color))
  {
    unsigned char* converted;
    size_t size = (w * h * LodePNG_InfoColor_getBpp(&info.color) + 7) / 8;

    if((info.color.colorType != 6 && info.color.colorType != 2) || (info.color.bitDepth != 8)) { encoder->error = 59; return; } /*for the output image, only these types are supported*/
    converted = (unsigned char*)malloc(size);
    if(!converted && size) encoder->error = 9955; /*error: malloc failed*/
    if(!encoder->error) encoder->error = LodePNG_convert(converted, image, &info.color, &encoder->infoRaw.color, w, h);
    if(!encoder->error) preProcessScanlines(&data, &datasize, converted, &info);/*filter(data.data, converted.data, w, h, LodePNG_InfoColor_getBpp(&info.color));*/
    free(converted);
  }
  else preProcessScanlines(&data, &datasize, image, &info);/*filter(data.data, image, w, h, LodePNG_InfoColor_getBpp(&info.color));*/

  ucvector_init(&outv);
  while(!encoder->error) /*not really a while loop, this is only used to break out if an error happens to avoid goto's to do the ucvector cleanup*/
  {
    /*write signature and chunks*/
    writeSignature(&outv);
    /*IHDR*/
    addChunk_IHDR(&outv, w, h, info.color.bitDepth, info.color.colorType, info.interlaceMethod);
    /*PLTE*/
    if(info.color.colorType == 3)
    {
      if(info.color.palettesize == 0 || info.color.palettesize > 256) { encoder->error = 68; break; }
      addChunk_PLTE(&outv, &info.color);
    }
    if(encoder->settings.force_palette && (info.color.colorType == 2 || info.color.colorType == 6))
    {
      if(info.color.palettesize == 0 || info.color.palettesize > 256) { encoder->error = 68; break; }
      addChunk_PLTE(&outv, &info.color);
    }
    /*tRNS*/
    if(info.color.colorType == 3 && !isPaletteFullyOpaque(info.color.palette, info.color.palettesize)) addChunk_tRNS(&outv, &info.color);
    if((info.color.colorType == 0 || info.color.colorType == 2) && info.color.key_defined) addChunk_tRNS(&outv, &info.color);
    /*IDAT (multiple IDAT chunks must be consecutive)*/
    encoder->error = addChunk_IDAT(&outv, data, datasize, &encoder->settings.zlibsettings);
    if(encoder->error) break;
    /*IEND*/
    addChunk_IEND(&outv);

    break; /*this isn't really a while loop; no error happened so break out now!*/
  }

  free(data);
  /*instead of cleaning the vector up, give it to the output*/
  *out = outv.data;
  *outsize = outv.size;
}

void LodePNG_EncodeSettings_init(LodePNG_EncodeSettings* settings)
{
  LodeZlib_DeflateSettings_init(&settings->zlibsettings);
  settings->autoLeaveOutAlphaChannel = 1;
  settings->force_palette = 0;
}

void LodePNG_Encoder_init(LodePNG_Encoder* encoder)
{
  LodePNG_EncodeSettings_init(&encoder->settings);
  LodePNG_InfoPng_init(&encoder->infoPng);
  LodePNG_InfoRaw_init(&encoder->infoRaw);
  encoder->error = 1;
}

void LodePNG_Encoder_cleanup(LodePNG_Encoder* encoder)
{
  LodePNG_InfoPng_cleanup(&encoder->infoPng);
  LodePNG_InfoRaw_cleanup(&encoder->infoRaw);
}

void LodePNG_Encoder_copy(LodePNG_Encoder* dest, const LodePNG_Encoder* source)
{
  LodePNG_Encoder_cleanup(dest);
  *dest = *source;
  LodePNG_InfoPng_init(&dest->infoPng);
  LodePNG_InfoRaw_init(&dest->infoRaw);
  dest->error = LodePNG_InfoPng_copy(&dest->infoPng, &source->infoPng); if(dest->error) return;
  dest->error = LodePNG_InfoRaw_copy(&dest->infoRaw, &source->infoRaw); if(dest->error) return;
}

/* ////////////////////////////////////////////////////////////////////////// */
/* / File IO                                                                / */
/* ////////////////////////////////////////////////////////////////////////// */

/*write given buffer to the file, overwriting the file, it doesn't append to it.*/
unsigned LodePNG_saveFile(const unsigned char* buffer, size_t buffersize, const char* filename)
{
  FILE* file = fopen(filename,"wb");
  if(!file) return 79;
  fwrite((char*)buffer , 1 , buffersize, file);
  fclose(file);
  return 0;
}

