#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gdhelpers.h"
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <ctype.h>

/* TBB: gd_strtok_r is not portable; provide an implementation */

#define SEP_TEST (separators[*((unsigned char *) s)])

char *
gd_strtok_r (char *s, char *sep, char **state)
{
	char separators[256];
	char *result = 0;
	memset (separators, 0, sizeof (separators));
	while (*sep) {
		separators[*((unsigned char *) sep)] = 1;
		sep++;
	}
	if (!s) {
		/* Pick up where we left off */
		s = *state;
	}
	/* 1. EOS */
	if (!(*s)) {
		*state = s;
		return 0;
	}
	/* 2. Leading separators, if any */
	if (SEP_TEST) {
		do {
			s++;
		} while (SEP_TEST);
		/* 2a. EOS after separators only */
		if (!(*s)) {
			*state = s;
			return 0;
		}
	}
	/* 3. A token */
	result = s;
	do {
		/* 3a. Token at end of string */
		if (!(*s)) {
			*state = s;
			return result;
		}
		s++;
	} while (!SEP_TEST);
	/* 4. Terminate token and skip trailing separators */
	*s = '\0';
	do {
		s++;
	} while (SEP_TEST);
	/* 5. Return token */
	*state = s;
	return result;
}

void * gdCalloc (size_t nmemb, size_t size)
{
	return calloc (nmemb, size);
}

void *
gdMalloc (size_t size)
{
	return malloc (size);
}

void *
gdRealloc (void *ptr, size_t size)
{
	return realloc (ptr, size);
}

void *
gdReallocEx (void *ptr, size_t size)
{
	void *newPtr = gdRealloc (ptr, size);
	if (!newPtr && ptr)
		gdFree(ptr);
	return newPtr;
}

/*
  Function: gdFree

    Frees memory that has been allocated by libgd functions.

	Unless more specialized functions exists (for instance, <gdImageDestroy>),
	all memory that has been allocated by public libgd functions has to be
	freed by calling <gdFree>, and not by free(3), because libgd internally
	doesn't use alloc(3) and friends but rather its own allocation functions,
	which are, however, not publicly available.

  Parameters:

	ptr - Pointer to the memory space to free. If it is NULL, no operation is
		  performed.

  Returns:

	Nothing.
*/
BGD_DECLARE(void) gdFree (void *ptr)
{
	free (ptr);
}


