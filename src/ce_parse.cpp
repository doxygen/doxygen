
/*  A Bison parser, made from constexp.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse cppExpYYparse
#define yylex cppExpYYlex
#define yyerror cppExpYYerror
#define yylval cppExpYYlval
#define yychar cppExpYYchar
#define yydebug cppExpYYdebug
#define yynerrs cppExpYYnerrs
#define	TOK_QUESTIONMARK	258
#define	TOK_COLON	259
#define	TOK_OR	260
#define	TOK_AND	261
#define	TOK_BITWISEOR	262
#define	TOK_BITWISEXOR	263
#define	TOK_AMPERSAND	264
#define	TOK_NOTEQUAL	265
#define	TOK_EQUAL	266
#define	TOK_LESSTHAN	267
#define	TOK_GREATERTHAN	268
#define	TOK_LESSTHANOREQUALTO	269
#define	TOK_GREATERTHANOREQUALTO	270
#define	TOK_SHIFTLEFT	271
#define	TOK_SHIFTRIGHT	272
#define	TOK_PLUS	273
#define	TOK_MINUS	274
#define	TOK_STAR	275
#define	TOK_DIVIDE	276
#define	TOK_MOD	277
#define	TOK_TILDE	278
#define	TOK_NOT	279
#define	TOK_LPAREN	280
#define	TOK_RPAREN	281
#define	TOK_OCTALINT	282
#define	TOK_DECIMALINT	283
#define	TOK_HEXADECIMALINT	284
#define	TOK_CHARACTER	285
#define	TOK_FLOAT	286



#include "cppvalue.h"
#include "constexp.h"

#if defined(_MSC_VER)
#define MSDOS
#endif

#define YYSTYPE CPPValue

#include <stdio.h>
#include <stdlib.h>

int cppExpYYerror(const char *s)
{
  printf("ERROR: %s\n",s);
  return 0;
}

int cppExpYYlex();

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		73
#define	YYFLAG		-32768
#define	YYNTBASE	32

#define YYTRANSLATE(x) ((unsigned)(x) <= 286 ? yytranslate[x] : 47)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,    10,    12,    16,    18,    22,    24,    28,
    30,    34,    36,    40,    42,    46,    50,    52,    56,    60,
    64,    68,    70,    74,    78,    80,    84,    88,    90,    94,
    98,   102,   104,   107,   110,   113,   116,   118,   122,   124,
   126,   128,   130
};

static const short yyrhs[] = {    33,
     0,    34,     0,    34,     3,    34,     4,    34,     0,    35,
     0,    34,     5,    35,     0,    36,     0,    35,     6,    36,
     0,    37,     0,    36,     7,    37,     0,    38,     0,    37,
     8,    38,     0,    39,     0,    38,     9,    39,     0,    40,
     0,    39,    11,    40,     0,    39,    10,    40,     0,    41,
     0,    40,    12,    41,     0,    40,    13,    41,     0,    40,
    14,    41,     0,    40,    15,    41,     0,    42,     0,    41,
    16,    42,     0,    41,    17,    42,     0,    43,     0,    42,
    18,    43,     0,    42,    19,    43,     0,    44,     0,    43,
    20,    44,     0,    43,    21,    44,     0,    43,    22,    44,
     0,    45,     0,    18,    44,     0,    19,    44,     0,    23,
    44,     0,    24,    44,     0,    46,     0,    25,    33,    26,
     0,    27,     0,    28,     0,    29,     0,    30,     0,    31,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    74,    78,    80,    89,    91,    97,    99,   105,   107,   114,
   116,   122,   124,   130,   132,   136,   142,   144,   148,   152,
   157,   164,   166,   170,   176,   178,   189,   202,   204,   215,
   228,   236,   238,   240,   247,   251,   257,   259,   263,   265,
   267,   269,   271
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","TOK_QUESTIONMARK",
"TOK_COLON","TOK_OR","TOK_AND","TOK_BITWISEOR","TOK_BITWISEXOR","TOK_AMPERSAND",
"TOK_NOTEQUAL","TOK_EQUAL","TOK_LESSTHAN","TOK_GREATERTHAN","TOK_LESSTHANOREQUALTO",
"TOK_GREATERTHANOREQUALTO","TOK_SHIFTLEFT","TOK_SHIFTRIGHT","TOK_PLUS","TOK_MINUS",
"TOK_STAR","TOK_DIVIDE","TOK_MOD","TOK_TILDE","TOK_NOT","TOK_LPAREN","TOK_RPAREN",
"TOK_OCTALINT","TOK_DECIMALINT","TOK_HEXADECIMALINT","TOK_CHARACTER","TOK_FLOAT",
"start","constant_expression","logical_or_expression","logical_and_expression",
"inclusive_or_expression","exclusive_or_expression","and_expression","equality_expression",
"relational_expression","shift_expression","additive_expression","multiplicative_expression",
"unary_expression","primary_expression","constant", NULL
};
#endif

static const short yyr1[] = {     0,
    32,    33,    33,    34,    34,    35,    35,    36,    36,    37,
    37,    38,    38,    39,    39,    39,    40,    40,    40,    40,
    40,    41,    41,    41,    42,    42,    42,    43,    43,    43,
    43,    44,    44,    44,    44,    44,    45,    45,    46,    46,
    46,    46,    46
};

static const short yyr2[] = {     0,
     1,     1,     5,     1,     3,     1,     3,     1,     3,     1,
     3,     1,     3,     1,     3,     3,     1,     3,     3,     3,
     3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
     3,     1,     2,     2,     2,     2,     1,     3,     1,     1,
     1,     1,     1
};

static const short yydefact[] = {     0,
     0,     0,     0,     0,     0,    39,    40,    41,    42,    43,
     1,     2,     4,     6,     8,    10,    12,    14,    17,    22,
    25,    28,    32,    37,    33,    34,    35,    36,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    38,     0,
     5,     7,     9,    11,    13,    16,    15,    18,    19,    20,
    21,    23,    24,    26,    27,    29,    30,    31,     0,     3,
     0,     0,     0
};

static const short yydefgoto[] = {    71,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24
};

static const short yypact[] = {   -13,
   -13,   -13,   -13,   -13,   -13,-32768,-32768,-32768,-32768,-32768,
-32768,     4,     2,    20,    27,    31,    21,     7,    17,    18,
     8,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    24,   -13,
   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,
   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,-32768,    34,
     2,    20,    27,    31,    21,     7,     7,    17,    17,    17,
    17,    18,    18,     8,     8,-32768,-32768,-32768,   -13,    39,
    13,    51,-32768
};

static const short yypgoto[] = {-32768,
    47,   -26,    25,    23,    26,    28,    22,     5,   -15,     6,
     9,    -1,-32768,-32768
};


#define	YYLAST		62


static const short yytable[] = {    25,
    26,    27,    28,    50,     1,     2,    30,    32,    31,     3,
     4,     5,    72,     6,     7,     8,     9,    10,    38,    39,
    40,    41,    58,    59,    60,    61,    33,    46,    47,    48,
    36,    37,    42,    43,    34,    44,    45,    69,    31,    35,
    56,    57,    70,    31,    66,    67,    68,    62,    63,    49,
    73,    29,    64,    65,    52,    51,    55,     0,    53,     0,
     0,    54
};

static const short yycheck[] = {     1,
     2,     3,     4,    30,    18,    19,     3,     6,     5,    23,
    24,    25,     0,    27,    28,    29,    30,    31,    12,    13,
    14,    15,    38,    39,    40,    41,     7,    20,    21,    22,
    10,    11,    16,    17,     8,    18,    19,     4,     5,     9,
    36,    37,    69,     5,    46,    47,    48,    42,    43,    26,
     0,     5,    44,    45,    32,    31,    35,    -1,    33,    -1,
    -1,    34
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */


/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

#ifndef YYPARSE_RETURN_TYPE
#define YYPARSE_RETURN_TYPE int
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
YYPARSE_RETURN_TYPE yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

YYPARSE_RETURN_TYPE
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
{ resultValue = yyvsp[0]; return 0; ;
    break;}
case 2:
{ yyval = yyvsp[0]; ;
    break;}
case 3:
{
		       bool c = (yyvsp[-4].isInt() ? ((long)yyvsp[-4] != 0) : ((double)yyvsp[-4] != 0.0));
		       yyval = c ? yyvsp[-2] : yyvsp[0];
	             ;
    break;}
case 4:
{ yyval = yyvsp[0]; ;
    break;}
case 5:
{
			 yyval = CPPValue( (long)((long)yyvsp[-2] || (long)yyvsp[0]) );
		       ;
    break;}
case 6:
{ yyval = yyvsp[0]; ;
    break;}
case 7:
{
			  yyval = CPPValue( (long)((long)yyvsp[-2] && (long)yyvsp[0]) );
			;
    break;}
case 8:
{ yyval = yyvsp[0]; ;
    break;}
case 9:
{ 
			   yyval = CPPValue( (long)yyvsp[-2] | (long)yyvsp[0] );
			 ;
    break;}
case 10:
{ yyval = yyvsp[0]; ;
    break;}
case 11:
{
			   yyval = CPPValue( (long)yyvsp[-2] ^ (long)yyvsp[0] );
			 ;
    break;}
case 12:
{ yyval = yyvsp[0]; ;
    break;}
case 13:
{ 
		  yyval = CPPValue( (long)yyvsp[-2] & (long)yyvsp[0] );
		;
    break;}
case 14:
{ yyval = yyvsp[0]; ;
    break;}
case 15:
{ 
		       yyval = CPPValue( (long)((double)yyvsp[-2] == (double)yyvsp[0]) );
	             ;
    break;}
case 16:
{
                       yyval = CPPValue( (long)((double)yyvsp[-2] != (double)yyvsp[0]) );
		     ;
    break;}
case 17:
{ yyval = yyvsp[0]; ;
    break;}
case 18:
{ 
			 yyval = CPPValue( (long)((double)yyvsp[-2] < (double)yyvsp[0]) );
		       ;
    break;}
case 19:
{
                         yyval = CPPValue( (long)((double)yyvsp[-2] > (double)yyvsp[0]) );
		       ;
    break;}
case 20:
{
		         yyval = CPPValue( (long)((double)yyvsp[-2] <= (double)yyvsp[0]) );
		       ;
    break;}
case 21:
{
			 yyval = CPPValue( (long)((double)yyvsp[-2] >= (double)yyvsp[0]) );
		       ;
    break;}
case 22:
{ yyval = yyvsp[0]; ;
    break;}
case 23:
{
		    yyval = CPPValue( (long)yyvsp[-2] << (long)yyvsp[0] );	
		  ;
    break;}
case 24:
{
		    yyval = CPPValue( (long)yyvsp[-2] >> (long)yyvsp[0] );
		  ;
    break;}
case 25:
{ yyval = yyvsp[0]; ;
    break;}
case 26:
{
		       if (!yyvsp[-2].isInt() || !yyvsp[0].isInt())
		       {
		         yyval = CPPValue( (double)yyvsp[-2] + (double)yyvsp[0] );
		       }
		       else	
		       {
		         yyval = CPPValue( (long)yyvsp[-2] + (long)yyvsp[0] );
		       }
		     ;
    break;}
case 27:
{
		       if (!yyvsp[-2].isInt() || !yyvsp[0].isInt())
		       {
		         yyval = CPPValue( (double)yyvsp[-2] - (double)yyvsp[0] );
		       }
		       else	
		       {
		         yyval = CPPValue( (long)yyvsp[-2] - (long)yyvsp[0] );
		       }
		     ;
    break;}
case 28:
{ yyval = yyvsp[0]; ;
    break;}
case 29:
{ 
			     if (!yyvsp[-2].isInt() || !yyvsp[0].isInt())
			     {
			       yyval = CPPValue( (double)yyvsp[-2] * (double)yyvsp[0] );
			     }
			     else
			     {
			       yyval = CPPValue( (long)yyvsp[-2] * (long)yyvsp[0] );
			     }
			   ;
    break;}
case 30:
{ 
			     if (!yyvsp[-2].isInt() || !yyvsp[0].isInt())
			     {
			       yyval = CPPValue( (double)yyvsp[-2] / (double)yyvsp[0] );
			     }
			     else
			     {
			       long value = yyvsp[0];
			       if (value==0) value=1;
			       yyval = CPPValue( (long)yyvsp[-2] / value );
			     }
			   ;
    break;}
case 31:
{ 
			     long value = yyvsp[0];
			     if (value==0) value=1;
			     yyval = CPPValue( (long)yyvsp[-2] % value );
			   ;
    break;}
case 32:
{ yyval = yyvsp[0]; ;
    break;}
case 33:
{ yyval = yyvsp[-1]; ;
    break;}
case 34:
{ 
		    if (yyvsp[0].isInt()) 
                      yyval = CPPValue(-(long)yyvsp[0]);
                    else
		      yyval = CPPValue(-(double)yyvsp[0]);
		  ;
    break;}
case 35:
{
		    yyval = CPPValue(~(long)yyvsp[0]);
		  ;
    break;}
case 36:
{
		    yyval = CPPValue((long)!(long)yyvsp[0]);
		  ;
    break;}
case 37:
{ yyval = yyvsp[0]; ;
    break;}
case 38:
{ yyval = yyvsp[-1]; ;
    break;}
case 39:
{ yyval = parseOctal(); ;
    break;}
case 40:
{ yyval = parseDecimal(); ;
    break;}
case 41:
{ yyval = parseHexadecimal(); ;
    break;}
case 42:
{ yyval = parseCharacter(); ;
    break;}
case 43:
{ yyval = parseFloat(); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */


  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}

