/******************************************************************************
 * ftvhelp.cpp,v 1.0 2000/09/06 16:09:00
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
 * Original version contributed by Kenney Wong <kwong@ea.com>
 * Modified by Dimitri van Heesch
 *
 * Folder Tree View for offline help on browsers that do not support HTML Help.
 */

#include <stdio.h>
#include <stdlib.h>
#include <qlist.h>
#include <qdict.h>
#include <qfileinfo.h>

#include "ftvhelp.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "language.h"
#include "htmlgen.h"
#include "layout.h"
#include "pagedef.h"

#define MAX_INDENT 1024


static const char navtree_script[]=
#include "navtree_js.h"
;

static const char resize_script[]=
#include "resize_js.h"
;

static const char navtree_css[]=
#include "navtree_css.h"
;

static unsigned char blank_png[352] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static unsigned char folderopen_png[528] =
{
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,228,195,193,190,187,218,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,195,215,221,225,225,178,176,176,175,176,178,180,255,255,255,255,255,255,
  255,255,255,255,255,255,189,206,215,219,226,220,214,212,207,204,200,176,255,255,255,255,255,255,
  255,255,255,255,168,154,153,153,152,152,151,149,150,150,149,147,146,145,145,167,255,255,255,255,
  255,255,255,255,146,187,187,188,187,187,185,183,183,182,179,178,175,173,174,145,255,255,255,255,
  255,255,255,255,146,180,182,182,181,181,179,178,176,174,173,171,169,170,168,144,255,255,255,255,
  255,255,255,255,144,173,176,176,177,175,175,174,171,170,168,168,166,166,164,143,255,255,255,255,
  255,255,255,255,142,168,170,171,170,170,169,168,166,166,165,163,163,164,162,142,255,255,255,255,
  255,255,255,255,141,162,166,164,164,165,163,163,161,161,161,161,161,160,159,141,255,255,255,255,
  255,255,255,255,138,157,159,159,158,158,158,157,157,157,157,156,157,157,155,138,255,255,255,255,
  255,255,255,255,137,154,153,154,154,153,154,154,154,153,154,154,154,154,154,137,255,255,255,255,
  255,255,255,255,137,154,154,154,154,154,154,154,153,154,154,153,153,153,154,137,255,255,255,255,
  255,255,255,255,137,125,125,125,125,124,125,124,124,125,124,124,125,124,125,138,255,255,255,255,
  255,255,255,255,212,209,204,199,193,190,186,183,180,181,185,188,192,197,202,203,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};

static unsigned char folderopen_a_png[528] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static unsigned char folderclosed_png[528] =
{
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,197,155,155,155,155,196,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,155,191,191,191,192,155,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,168,144,180,180,181,180,145,145,146,145,146,146,146,146,145,167,255,255,255,255,
  255,255,255,255,147,225,226,226,225,226,225,221,221,219,215,214,212,211,213,145,255,255,255,255,
  255,255,255,255,147,212,211,211,210,211,210,205,206,205,201,201,199,196,201,145,255,255,255,255,
  255,255,255,255,146,204,203,204,203,203,202,200,200,197,197,196,195,194,196,145,255,255,255,255,
  255,255,255,255,146,202,200,201,201,200,199,198,198,195,194,194,193,192,194,145,255,255,255,255,
  255,255,255,255,145,200,196,196,196,195,195,193,192,192,190,189,189,189,191,143,255,255,255,255,
  255,255,255,255,143,192,191,190,190,189,189,188,186,187,186,185,185,185,187,142,255,255,255,255,
  255,255,255,255,142,186,184,183,182,183,182,183,180,181,181,181,181,181,182,141,255,255,255,255,
  255,255,255,255,138,177,175,176,176,177,177,176,175,174,175,175,175,174,176,138,255,255,255,255,
  255,255,255,255,138,173,169,170,168,170,169,170,170,169,171,171,171,171,174,137,255,255,255,255,
  255,255,255,255,138,166,163,163,162,162,162,162,162,162,164,163,163,163,166,137,255,255,255,255,
  255,255,255,255,137,124,124,124,125,124,124,124,125,125,124,124,125,124,125,138,255,255,255,255,
  255,255,255,255,231,231,228,225,222,220,218,216,214,215,217,219,221,224,227,226,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};

static unsigned char folderclosed_a_png[528] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,
    0,  0,  0,  0,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static unsigned char doc_png[528] =
{
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,218,214,208,208,204,191,179,190,197,209,231,255,255,255,255,255,255,255,255,
  255,255,255,255,255,195,224,226,226,222,214,204,181,203,229,188,225,255,255,255,255,255,255,255,
  255,255,255,255,255,198,226,228,227,227,224,215,203,180,252,229,184,224,255,255,255,255,255,255,
  255,255,255,255,255,198,229,230,229,229,228,224,214,154,252,252,229,187,235,255,255,255,255,255,
  255,255,255,255,255,198,232,233,233,232,231,230,223,176,154,144,165,177,216,255,255,255,255,255,
  255,255,255,255,255,198,236,236,216,226,238,219,232,225,209,190,189,166,193,255,255,255,255,255,
  255,255,255,255,255,198,239,240,178,177,230,175,169,184,188,219,208,189,187,255,255,255,255,255,
  255,255,255,255,255,198,241,242,240,218,237,236,240,235,241,244,221,208,182,255,255,255,255,255,
  255,255,255,255,255,198,243,243,188,154,183,158,166,140,185,198,231,219,177,255,255,255,255,255,
  255,255,255,255,255,198,243,245,248,228,241,241,226,249,237,227,239,232,177,255,255,255,255,255,
  255,255,255,255,255,198,244,246,213,172,163,149,171,200,167,149,242,239,177,255,255,255,255,255,
  255,255,255,255,255,198,249,248,240,218,237,236,240,235,241,244,244,242,177,255,255,255,255,255,
  255,255,255,255,255,198,249,251,188,155,184,158,166,140,185,198,246,244,177,255,255,255,255,255,
  255,255,255,255,255,198,251,253,248,228,241,241,226,249,237,227,249,246,177,255,255,255,255,255,
  255,255,255,255,255,196,253,252,252,252,252,251,251,250,250,249,249,248,175,255,255,255,255,255,
  255,255,255,255,255,194, 64, 30, 37, 37, 37, 37, 37, 37, 37, 37, 30, 64,188,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};

static unsigned char doc_a_png[528] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static unsigned char arrow_right_png[352] =
{
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,152,152,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,152,152,152,152,255,255,255,255,255,255,255,255,255,
  255,255,255,152,152,152,152,152,255,255,255,255,255,255,255,255,
  255,255,255,152,152,152,152,152,152,152,255,255,255,255,255,255,
  255,255,255,152,152,152,152,152,152,152,152,255,255,255,255,255,
  255,255,255,152,152,152,152,152,152,152,255,255,255,255,255,255,
  255,255,255,152,152,152,152,152,255,255,255,255,255,255,255,255,
  255,255,255,152,152,152,152,255,255,255,255,255,255,255,255,255,
  255,255,255,152,152,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};

static unsigned char arrow_right_a_png[352] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,223, 75,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,255,255,176, 33,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,255,255,255,248,117,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,255,255,255,255,255,211, 60,  0,  0,  0,  0,  0,  0,
    0,  0,  0,255,255,255,255,255,255,255, 77,  0,  0,  0,  0,  0,
    0,  0,  0,255,255,255,255,255,211, 60,  0,  0,  0,  0,  0,  0,
    0,  0,  0,255,255,255,248,117,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,255,255,176, 33,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,223, 75,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static unsigned char arrow_down_png[352] =
{
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,152,152,152,152,152,152,152,152,152,255,255,255,255,
  255,255,255,152,152,152,152,152,152,152,152,152,255,255,255,255,
  255,255,255,255,152,152,152,152,152,152,152,255,255,255,255,255,
  255,255,255,255,152,152,152,152,152,152,152,255,255,255,255,255,
  255,255,255,255,255,152,152,152,152,152,255,255,255,255,255,255,
  255,255,255,255,255,255,152,152,152,255,255,255,255,255,255,255,
  255,255,255,255,255,255,152,152,152,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,152,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};

static unsigned char arrow_down_a_png[352] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,231,255,255,255,255,255,255,255,216,  0,  0,  0,  0,
    0,  0,  0, 87,255,255,255,255,255,255,255, 65,  0,  0,  0,  0,
    0,  0,  0,  0,186,255,255,255,255,255,164,  0,  0,  0,  0,  0,
    0,  0,  0,  0, 38,251,255,255,255,241, 25,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,127,255,255,255,107,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,221,255,204,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0, 72,253, 52,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0, 77,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

#define SPLITBAR_LINE 170,242,224,202,183,170
#define SPLITBAR_BLOCK2  SPLITBAR_LINE    , SPLITBAR_LINE 
#define SPLITBAR_BLOCK4  SPLITBAR_BLOCK2  , SPLITBAR_BLOCK2 
#define SPLITBAR_BLOCK8  SPLITBAR_BLOCK4  , SPLITBAR_BLOCK4 
#define SPLITBAR_BLOCK16 SPLITBAR_BLOCK8  , SPLITBAR_BLOCK8 
#define SPLITBAR_BLOCK32 SPLITBAR_BLOCK16 , SPLITBAR_BLOCK16

#define SPLITBAR_ALTLINE1 170,242,170,202,170,170
#define SPLITBAR_ALTLINE2 170,243,224,255,183,255
#define SPLITBAR_ALTBLOCK2 SPLITBAR_ALTLINE1  , SPLITBAR_ALTLINE2 
#define SPLITBAR_ALTBLOCK4 SPLITBAR_ALTBLOCK2 , SPLITBAR_ALTBLOCK2 
#define SPLITBAR_ALTBLOCK8 SPLITBAR_ALTBLOCK4 , SPLITBAR_ALTBLOCK4

static unsigned char splitbar_png[32*32*6] =
{
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK8,
  SPLITBAR_BLOCK8,
  SPLITBAR_ALTBLOCK8,
  SPLITBAR_BLOCK8,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32,
  SPLITBAR_BLOCK32
};

struct FTVImageInfo
{
  const char *alt;
  const char *name;
  const unsigned char *data;
  unsigned int len;
  unsigned short width, height;
};

//extern FTVImageInfo image_info[];

#define FTVIMG_blank        0
#define FTVIMG_doc          1
#define FTVIMG_folderclosed 2
#define FTVIMG_folderopen   3
#define FTVIMG_lastnode     4
#define FTVIMG_link         5
#define FTVIMG_mlastnode    6
#define FTVIMG_mnode        7
#define FTVIMG_node         8
#define FTVIMG_plastnode    9
#define FTVIMG_pnode       10
#define FTVIMG_vertline    11

#define FTV_S(name) #name
#define FTV_ICON_FILE(name) "ftv2" FTV_S(name) ".png"
#define FTVIMG_INDEX(name) FTVIMG_ ## name
#define FTV_INFO(name) ( image_info[FTVIMG_INDEX(name)] )
#define FTV_IMGATTRIBS(name) \
    "src=\"" FTV_ICON_FILE(name) "\" " \
    "alt=\"" << FTV_INFO(name).alt << "\" " \
    "width=\"" << FTV_INFO(name).width << "\" " \
    "height=\"" << FTV_INFO(name).height << "\" "


static FTVImageInfo image_info[] =
{
  { "&#160;", "ftv2blank.png",    0 /*ftv2blank_png*/        ,174,16,22 },
  { "*",  "ftv2doc.png",          0 /*ftv2doc_png*/          ,255,24,22 },
  { "+",  "ftv2folderclosed.png", 0 /*ftv2folderclosed_png*/ ,259,24,22 },
  { "-",  "ftv2folderopen.png",   0 /*ftv2folderopen_png*/   ,261,24,22 },
  { "\\", "ftv2lastnode.png",     0 /*ftv2lastnode_png*/     ,233,16,22 },
  { "-",  "ftv2link.png",         0 /*ftv2link_png*/         ,358,24,22 },
  { "\\", "ftv2mlastnode.png",    0 /*ftv2mlastnode_png*/    ,160,16,22 },
  { "o",  "ftv2mnode.png",        0 /*ftv2mnode_png*/        ,194,16,22 },
  { "o",  "ftv2node.png",         0 /*ftv2node_png*/         ,235,16,22 },
  { "\\", "ftv2plastnode.png",    0 /*ftv2plastnode_png*/    ,165,16,22 },
  { "o",  "ftv2pnode.png",        0 /*ftv2pnode_png*/        ,200,16,22 },
  { "|",  "ftv2vertline.png",     0 /*ftv2vertline_png*/     ,229,16,22 },
  {   0,  0,                      0,                            0, 0, 0 }
};

static ColoredImgDataItem ftv_image_data[] =
{
  { "ftv2blank.png",        16,  22, blank_png,        blank_png          },
  { "ftv2doc.png",          24,  22, doc_png,          doc_a_png          },
  { "ftv2folderclosed.png", 24,  22, folderclosed_png, folderclosed_a_png },
  { "ftv2folderopen.png",   24,  22, folderopen_png,   folderopen_a_png   },
  { "ftv2lastnode.png",     16,  22, blank_png,        blank_png          },
  { "ftv2link.png",         24,  22, doc_png,          doc_a_png          },
  { "ftv2mlastnode.png",    16,  22, arrow_down_png,   arrow_down_a_png   },
  { "ftv2mnode.png",        16,  22, arrow_down_png,   arrow_down_a_png   },
  { "ftv2node.png",         16,  22, blank_png,        blank_png          },
  { "ftv2plastnode.png",    16,  22, arrow_right_png,  arrow_right_a_png  },
  { "ftv2pnode.png",        16,  22, arrow_right_png,  arrow_right_a_png  },
  { "ftv2vertline.png",     16,  22, blank_png,        blank_png          },
  { "ftv2splitbar.png",      6,1024, splitbar_png,     0                  },
  { 0,                       0,   0, 0,                0                  }
};

static int folderId=1;

struct FTVNode
{
  FTVNode(bool dir,const char *r,const char *f,const char *a,const char *n,bool sepIndex,bool navIndex)
    : isLast(TRUE), isDir(dir),ref(r),file(f),anchor(a),name(n), index(0),
      parent(0), separateIndex(sepIndex), addToNavIndex(navIndex) { children.setAutoDelete(TRUE); }
  bool isLast;
  bool isDir;
  QCString ref;
  QCString file;
  QCString anchor;
  QCString name;
  int index;
  QList<FTVNode> children;
  FTVNode *parent;
  bool separateIndex;
  bool addToNavIndex;
};


//----------------------------------------------------------------------------

/*! Constructs an ftv help object. 
 *  The object has to be \link initialize() initialized\endlink before it can 
 *  be used.
 */
FTVHelp::FTVHelp(bool TLI) 
{
  /* initial depth */
  m_indentNodes = new QList<FTVNode>[MAX_INDENT];
  m_indentNodes[0].setAutoDelete(TRUE);
  m_indent=0;
  m_topLevelIndex = TLI;
}

/*! Destroys the ftv help object. */
FTVHelp::~FTVHelp()
{
  delete[] m_indentNodes;
}

/*! This will create a folder tree view table of contents file (tree.js).
 *  \sa finalize()
 */
void FTVHelp::initialize()
{
}

/*! Finalizes the FTV help. This will finish and close the
 *  contents file (index.js).
 *  \sa initialize()
 */
void FTVHelp::finalize()
{
  generateTreeView();
}

/*! Increase the level of the contents hierarchy. 
 *  This will start a new sublist in contents file.
 *  \sa decContentsDepth()
 */
void FTVHelp::incContentsDepth()
{
  //printf("incContentsDepth() indent=%d\n",m_indent);
  m_indent++;
  ASSERT(m_indent<MAX_INDENT);
}

/*! Decrease the level of the contents hierarchy.
 *  This will end the current sublist.
 *  \sa incContentsDepth()
 */
void FTVHelp::decContentsDepth()
{
  //printf("decContentsDepth() indent=%d\n",m_indent);
  ASSERT(m_indent>0);
  if (m_indent>0)
  {
    m_indent--;
    QList<FTVNode> *nl = &m_indentNodes[m_indent];
    FTVNode *parent = nl->getLast();
    if (parent)
    {
      QList<FTVNode> *children = &m_indentNodes[m_indent+1];
      while (!children->isEmpty())
      {
        parent->children.append(children->take(0));
      }
    }
  }
}

/*! Add a list item to the contents file.
 *  \param isDir TRUE if the item is a directory, FALSE if it is a text
 *  \param ref  the URL of to the item.
 *  \param file the file containing the definition of the item
 *  \param anchor the anchor within the file.
 *  \param name the name of the item.
 *  \param separateIndex put the entries in a separate index file
 *  \param addToNavIndex add this entry to the quick navigation index
 */
void FTVHelp::addContentsItem(bool isDir,
                              const char *name,
                              const char *ref,
                              const char *file,
                              const char *anchor,
                              bool separateIndex,
                              bool addToNavIndex
                              )
{
  //printf("addContentsItem(%s,%s,%s,%s)\n",name,ref,file,anchor);
  QList<FTVNode> *nl = &m_indentNodes[m_indent];
  FTVNode *newNode = new FTVNode(isDir,ref,file,anchor,name,separateIndex,addToNavIndex);
  if (!nl->isEmpty())
  {
    nl->getLast()->isLast=FALSE;
  }
  nl->append(newNode);
  newNode->index = nl->count()-1;
  if (m_indent>0)
  {
    QList<FTVNode> *pnl = &m_indentNodes[m_indent-1];
    newNode->parent = pnl->getLast();
  }
  
}

static QCString node2URL(FTVNode *n)
{
  QCString url = n->file;
  if (!url.isEmpty() && url.at(0)=='!')  // relative URL
  {
    // remove leading !
    url = url.mid(1);
  }
  else if (!url.isEmpty() && url.at(0)=='^') // absolute URL
  {
    // skip, keep ^ in the output
  }
  else // local file (with optional anchor)
  {
    url+=Doxygen::htmlFileExtension;
    if (!n->anchor.isEmpty()) url+="#"+n->anchor;
  }
  return url;
}


void FTVHelp::generateIndent(FTextStream &t, FTVNode *n,int level)
{
  if (n->parent)
  {
    generateIndent(t,n->parent,level+1);
  }
  // from the root up to node n do...
  if (level==0) // item before a dir or document
  {
    if (n->isLast)
    {
      if (n->isDir)
      {
        t << "<img " << FTV_IMGATTRIBS(plastnode) << "onclick=\"toggleFolder('folder" << folderId << "', this)\"/>";
      }
      else
      {
        t << "<img " << FTV_IMGATTRIBS(lastnode) << "/>";
      }
    }
    else
    {
      if (n->isDir)
      {
        t << "<img " << FTV_IMGATTRIBS(pnode) << "onclick=\"toggleFolder('folder" << folderId << "', this)\"/>";
      }
      else
      {
        t << "<img " << FTV_IMGATTRIBS(node) << "/>";
      }
    }
  }
  else // item at another level
  {
    if (n->isLast)
    {
      t << "<img " << FTV_IMGATTRIBS(blank) << "/>";
    }
    else
    {
      t << "<img " << FTV_IMGATTRIBS(vertline) << "/>";
    }
  }
}

void FTVHelp::generateLink(FTextStream &t,FTVNode *n)
{
  //printf("FTVHelp::generateLink(ref=%s,file=%s,anchor=%s\n",
  //    n->ref.data(),n->file.data(),n->anchor.data());
  if (n->file.isEmpty()) // no link
  {
    t << "<b>" << convertToHtml(n->name) << "</b>";
  }
  else // link into other frame
  {
    if (!n->ref.isEmpty()) // link to entity imported via tag file
    {
      t << "<a class=\"elRef\" ";
      t << externalLinkTarget() << externalRef("",n->ref,FALSE);
    }
    else // local link
    {
      t << "<a class=\"el\" ";
    }
    t << "href=\"";
    t << externalRef("",n->ref,TRUE);
    t << node2URL(n);
    if (m_topLevelIndex)
      t << "\" target=\"basefrm\">";
    else
      t << "\" target=\"_self\">";
    t << convertToHtml(n->name);
    t << "</a>";
    if (!n->ref.isEmpty())
    {
      t << "&#160;[external]";
    }
  }
}

void FTVHelp::generateJSLink(FTextStream &t,FTVNode *n)
{
  if (n->file.isEmpty()) // no link
  {
    t << "\"" << convertToJSString(n->name) << "\", null, ";
  }
  else // link into other page
  {
    // TODO: use m_topLevelIndex
    t << "\"" << convertToJSString(n->name) << "\", \"";
    t << externalRef("",n->ref,TRUE);
    t << node2URL(n);
    t << "\", ";
  }
}

void FTVHelp::generateTree(FTextStream &t, const QList<FTVNode> &nl,int level)
{
  QCString spaces;
  spaces.fill(' ',level*2+8);
  QListIterator<FTVNode> nli(nl);
  FTVNode *n;
  for (nli.toFirst();(n=nli.current());++nli)
  {
    t << spaces << "<p>";
    generateIndent(t,n,0);
    if (n->isDir)
    {
      t << "<img " << FTV_IMGATTRIBS(folderclosed) << "onclick=\"toggleFolder('folder" << folderId << "', this)\"/>";
      generateLink(t,n);
      t << "</p>\n";
      t << spaces << "<div id=\"folder" << folderId << "\">\n";
      folderId++;
      generateTree(t,n->children,level+1);
      t << spaces << "</div>\n";
    }
    else
    {
      t << "<img " << FTV_IMGATTRIBS(doc) << "/>";
      generateLink(t,n);
      t << "</p>\n";
    }
  }
}

static void writePathToNode(FTextStream &tidx,FTVNode *leaf,FTVNode *n)
{
  if (n->parent)
  {
    writePathToNode(tidx,leaf,n->parent);
  }
  tidx << n->index;
  if (leaf!=n) tidx << ",";
}

bool childOfHierarchy(const FTVNode *n)
{
  if (n==0) return FALSE;
  if (n->file=="hierarchy") 
    return TRUE;
  else 
    return childOfHierarchy(n->parent);
}

bool dupOfParent(const FTVNode *n)
{
  if (n->parent==0) return FALSE;
  if (n->file==n->parent->file) return TRUE;
  return FALSE;
}

bool FTVHelp::generateJSTree(FTextStream &tidx,FTextStream &t, const QList<FTVNode> &nl,int level,bool &first)
{
  QCString indentStr;
  indentStr.fill(' ',level*2);
  bool found=FALSE;
  QListIterator<FTVNode> nli(nl);
  FTVNode *n;
  for (nli.toFirst();(n=nli.current());++nli)
  {
    // terminate previous entry
    if (!first) t << "," << endl;
    first=FALSE;

    // start entry
    if (!found)
    {
      t << "[" << endl;
    }
    found=TRUE;

    //if (!n->file.isEmpty() && !childOfHierarchy(n->parent))
    if (n->addToNavIndex)
    {
      tidx << "," << endl << "\"" << node2URL(n) << "\":[";
      writePathToNode(tidx,n,n);
      tidx << "]";
    }

    if (n->separateIndex) // store items in a separate file for dynamic loading
    {
      bool firstChild=TRUE;
      t << indentStr << "  [ ";
      generateJSLink(t,n);
      if (n->children.count()>0) // write children to separate file for dynamic loading
      {
        QCString fileId = n->file;
        if (dupOfParent(n)) fileId+="_dup";
        QFile f(Config_getString("HTML_OUTPUT")+"/"+fileId+".js");
        if (f.open(IO_WriteOnly))
        {
          FTextStream tt(&f);
          QCString varId = fileId;
          int i=fileId.findRev('/');
          if (i>=0) varId = varId.mid(i+1);
          tt << "var " << varId << " =" << endl;
          generateJSTree(tidx,tt,n->children,1,firstChild);
          tt << endl << "];"; 
        }
        // write file name without extension as marker
        t << "\"" << fileId << "\" ]";
        //if (n->file!="hierarchy") addFilesToIndex(tidx,n);
      }
      else // no children
      {
        t << "null ]";
      }
    }
    else // show items in this file
    {
      bool firstChild=TRUE;
      t << indentStr << "  [ ";
      generateJSLink(t,n);
      bool emptySection = !generateJSTree(tidx,t,n->children,level+1,firstChild);
      if (emptySection)
        t << "null ]";
      else
        t << endl << indentStr << "  ] ]"; 
    }
  }
  return found;
}

// new style images
void FTVHelp::generateTreeViewImages()
{
  QCString dname=Config_getString("HTML_OUTPUT");
  writeColoredImgData(dname,ftv_image_data);
}

// new style scripts
void FTVHelp::generateTreeViewScripts()
{
  // generate navtree.js
  {
    QCString htmlOutput = Config_getString("HTML_OUTPUT");
    QFile f(htmlOutput+"/navtree.js");
    QFile fidx(htmlOutput+"/navtreeindex.js");
    if (f.open(IO_WriteOnly) && fidx.open(IO_WriteOnly))
    {
      FTextStream tidx(&fidx);
      tidx << "var NAVTREEINDEX =" << endl;
      tidx << "{" << endl;
      FTextStream t(&f);
      t << "var NAVTREE =" << endl;
      t << "[" << endl;
      t << "  [ ";
      QCString &projName = Config_getString("PROJECT_NAME");
      if (projName.isEmpty())
      {
        if (Doxygen::mainPage && !Doxygen::mainPage->title().isEmpty()) // Use title of main page as root
        {
          t << "\"" << convertToJSString(Doxygen::mainPage->title()) << "\", ";
        }
        else // Use default section title as root
        {
          LayoutNavEntry *lne = LayoutDocManager::instance().rootNavEntry()->find(LayoutNavEntry::MainPage);
          t << "\"" << convertToJSString(lne->title()) << "\", ";
        }
      }
      else // use PROJECT_NAME as root tree element
      {
        t << "\"" << convertToJSString(projName) << "\", ";
      }
      t << "\"index" << Doxygen::htmlFileExtension << "\", ";

      tidx << "\"index" << Doxygen::htmlFileExtension << "\":[]";

      bool first=TRUE;
      generateJSTree(tidx,t,m_indentNodes[0],1,first);

      if (first) 
        t << "]" << endl;
      else 
        t << endl << "  ] ]" << endl;
      t << "];" << endl;
      t << endl << navtree_script;

      tidx << endl << "};" << endl;
    }
  }
  // generate resize.js
  {
    QFile f(Config_getString("HTML_OUTPUT")+"/resize.js");
    if (f.open(IO_WriteOnly))
    {
      FTextStream t(&f);
      t << resize_script;
    }
  }
  // generate navtree.css
  {
    QFile f(Config_getString("HTML_OUTPUT")+"/navtree.css");
    if (f.open(IO_WriteOnly))
    {
      FTextStream t(&f);
      t << replaceColorMarkers(navtree_css);
    }
  }
}

// old style script (used for inline trees)
void FTVHelp::generateScript(FTextStream &t)
{
  t << "    <script type=\"text/javascript\">\n";
  t << "    <!-- // Hide script from old browsers\n";
  t << "    \n";

  /* User has clicked on a node (folder or +/-) in the tree */
  t << "    function toggleFolder(id, imageNode) \n";
  t << "    {\n";
  t << "      var folder = document.getElementById(id);\n";
  t << "      var l = imageNode.src.length;\n";
  /* If the user clicks on the book icon, we move left one image so 
   * the code (below) will also adjust the '+' icon. 
   */
  t << "      if (imageNode.src.substring(l-20,l)==\"" FTV_ICON_FILE(folderclosed) "\" || \n";
  t << "          imageNode.src.substring(l-18,l)==\"" FTV_ICON_FILE(folderopen)  "\")\n";
  t << "      {\n";
  t << "        imageNode = imageNode.previousSibling;\n";
  t << "        l = imageNode.src.length;\n";
  t << "      }\n";
  t << "      if (folder == null) \n";
  t << "      {\n";
  t << "      } \n";
  /* Node controls a open section, we need to close it */
  t << "      else if (folder.style.display == \"block\") \n";
  t << "      {\n";
  t << "        if (imageNode != null) \n";
  t << "        {\n";
  t << "          imageNode.nextSibling.src = \"" FTV_ICON_FILE(folderclosed) "\";\n";
  t << "          if (imageNode.src.substring(l-13,l) == \"" FTV_ICON_FILE(mnode) "\")\n";
  t << "          {\n";
  t << "            imageNode.src = \"" FTV_ICON_FILE(pnode) "\";\n";
  t << "          }\n";
  t << "          else if (imageNode.src.substring(l-17,l) == \"" FTV_ICON_FILE(mlastnode) "\")\n";
  t << "          {\n";
  t << "            imageNode.src = \"" FTV_ICON_FILE(plastnode) "\";\n";
  t << "          }\n";
  t << "        }\n";
  t << "        folder.style.display = \"none\";\n";
  t << "      } \n";
  t << "      else \n"; /* section is closed, we need to open it */
  t << "      {\n";
  t << "        if (imageNode != null) \n";
  t << "        {\n";
  t << "          imageNode.nextSibling.src = \"" FTV_ICON_FILE(folderopen) "\";\n";
  t << "          if (imageNode.src.substring(l-13,l) == \"" FTV_ICON_FILE(pnode) "\")\n";
  t << "          {\n";
  t << "            imageNode.src = \"" FTV_ICON_FILE(mnode) "\";\n";
  t << "          }\n";
  t << "          else if (imageNode.src.substring(l-17,l) == \"" FTV_ICON_FILE(plastnode) "\")\n";
  t << "          {\n";
  t << "            imageNode.src = \"" FTV_ICON_FILE(mlastnode) "\";\n";
  t << "          }\n";
  t << "        }\n";
  t << "        folder.style.display = \"block\";\n";
  t << "      }\n";
  t << "    }\n";
  t << "\n";
  t << "    // End script hiding -->        \n";
  t << "    </script>\n";
}

// write tree inside page
void FTVHelp::generateTreeViewInline(FTextStream &t)
{
  generateScript(t);
  t << "    <div class=\"directory-alt\">\n";
  t << "      <br/>\n";
  t << "      <div style=\"display: block;\">\n";

  generateTree(t,m_indentNodes[0],0);

  t << "      </div>\n";
  t << "    </div>\n";
}

// write old style index.html and tree.html
void FTVHelp::generateTreeView()
{
  generateTreeViewImages();
  generateTreeViewScripts();
}

