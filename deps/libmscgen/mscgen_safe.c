/***************************************************************************
 *
 * $Id: safe.c 152 2010-10-10 14:17:37Z Michael.McTernan $
 *
 * This file is part of timgen, a timing diagram renderer.
 * Copyright (C) 2010 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
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

#define FILE_NAME SAFE

/*****************************************************************************
 * Header Files
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mscgen_config.h"
#include "mscgen_safe.h"

/*****************************************************************************
 * Preprocessor Macros & Constants
 *****************************************************************************/

/*****************************************************************************
 * Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Local Variable Definitions
 *****************************************************************************/

/*****************************************************************************
 * Global Variable Definitions
 *****************************************************************************/

/*****************************************************************************
 * Local Function Definitions
 *****************************************************************************/

static void checkNotNull(void *p, const char *message)
{
    if(!p)
    {
        fprintf(stderr, "Fatal error: %s\n", message);
        exit(EXIT_FAILURE);
    }
}

/*****************************************************************************
 * Global Function Definitions
 *****************************************************************************/

void *realloc_s(void *ptr, size_t size)
{
    void *r = realloc(ptr, size);

    checkNotNull(r, "realloc() failed");

    return r;
}

void *malloc_s(size_t size)
{
    void *r = malloc(size);

    checkNotNull(r, "malloc() failed");

    return r;
}

void *zalloc_s(size_t size)
{
    void *r = malloc(size);

    checkNotNull(r, "malloc() failed");
    memset(r, 0, size);

    return r;
}

char *strdup_s(const char *s)
{
    char *r = strdup(s);

    checkNotNull(r, "strdup() failed");

    return r;
}

const char *mscgen_getenv_s(const char *name)
{
    char *r = getenv(name);

    if(r == NULL) r = "";

    return r;
}

/*****************************************************************************
 * Unit Test Support
 *****************************************************************************/


/* END OF FILE */
