/* GLOBAL.H - RSAREF types and constants */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
   Inc., created 1991. All rights reserved.
 */

#ifndef _MD5LOC_H
#define _MD5LOC_H

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

#ifndef NULL_PTR
#define NULL_PTR ((POINTER)0)
#endif

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) x = *(&x);
#endif

#endif /* end _GLOBAL_H_ */
