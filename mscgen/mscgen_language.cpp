
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "language.y"

/***************************************************************************
 *
 * $Id: language.y 175 2011-02-06 21:07:43Z Michael.McTernan $
 *
 * Grammar and parser for the mscgen language.
 * Copyright (C) 2009 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This file is part of msclib.
 *
 * Msc is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * Msclib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Foobar; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mscgen_lexer.h"
#include "mscgen_safe.h"
#include "mscgen_msc.h"

/* Lexer prototypes to prevent compiler warnings */
int yylex(void);
int yylex_destroy(void);

/* Use verbose error reporting such that the expected token names are dumped */
#define YYERROR_VERBOSE

/* Name of parameter that is passed to yyparse() */
#define YYPARSE_PARAM yyparse_result

#define YYMALLOC malloc_s

/* yyerror
 *  Error handling function.  The TOK_XXX names are substituted for more
 *  understandable values that make more sense to the user.
 */
void yyerror(const char *str)
{
    static const char *tokNames[] = { "TOK_OCBRACKET",          "TOK_CCBRACKET",
                                      "TOK_OSBRACKET",          "TOK_CSBRACKET",
                                      "TOK_REL_DOUBLE_TO",      "TOK_REL_DOUBLE_FROM",
                                      "TOK_REL_SIG_TO",         "TOK_REL_SIG_FROM",
                                      "TOK_REL_METHOD_TO",      "TOK_REL_METHOD_FROM",
                                      "TOK_REL_RETVAL_TO",      "TOK_REL_RETVAL_FROM",
                                      "TOK_REL_CALLBACK_TO",    "TOK_REL_CALLBACK_FROM",
                                      "TOK_REL_SIG",            "TOK_REL_METHOD",
                                      "TOK_REL_RETVAL",         "TOK_REL_DOUBLE",
                                      "TOK_EQUAL",              "TOK_COMMA",
                                      "TOK_SEMICOLON",          "TOK_MSC",
                                      "TOK_ATTR_LABEL",         "TOK_ATTR_URL",
                                      "TOK_ATTR_IDURL",         "TOK_ATTR_ID",
                                      "TOK_ATTR_LINE_COLOUR",   "TOK_ATTR_TEXT_COLOUR",
                                      "TOK_SPECIAL_ARC",        "TOK_UNKNOWN",
                                      "TOK_STRING",             "TOK_QSTRING",
                                      "TOK_OPT_HSCALE",         "TOK_ASTERISK",
                                      "TOK_OPT_WIDTH",          "TOK_ARC_BOX",
                                      "TOK_ARC_ABOX",           "TOK_ARC_RBOX",
                                      "TOK_ATTR_TEXT_BGCOLOUR", "TOK_ATTR_ARC_TEXT_BGCOLOUR",
                                      "TOK_REL_LOSS_TO",        "TOK_REL_LOSS_FROM",
                                      "TOK_OPT_ARCGRADIENT",    "TOK_ATTR_ARC_SKIP",
                                      "TOK_OPT_WORDWRAPARCS",   "TOK_REL_NOTE" };

    static const char *tokRepl[] =  { "'{'",             "'}'",
                                      "'['",             "']'",
                                      "':>'",            "'<:'",
                                      "'->'",            "'<-'",
                                      "'=>'",            "'<='",
                                      "'>>'",            "'<<'",
                                      "'=>>'",           "'<<='",
                                      "'--'",            "'=='",
                                      "'..'",            "'::'",
                                      "'='",             "','",
                                      "';'",             "'msc'",
                                      "'label'",         "'url'",
                                      "'idurl'",         "'id'",
                                      "'linecolour'",    "'textcolour'",
                                      "'...', '---'",    "characters",
                                      "'string'",        "'quoted string'",
                                      "'hscale'",        "'*'",
                                      "'width'",         "'box'",
                                      "'abox'",          "'rbox'",
                                      "'textbgcolour'",  "'arctextbgcolor'",
                                      "'-x'",            "'x-'",
                                      "'arcgradient'",   "'arcskip'",
                                      "'wordwraparcs'",  "'note'" };

    static const int tokArrayLen = sizeof(tokNames) / sizeof(char *);

    char *s, *line;
    int   t;

    /* Print standard message part */
    fprintf(stderr, "Error detected at line %lu: ", lex_getlinenum());

    /* Search for TOK */
    s = (char *)strstr(str, "TOK_");
    while(s != NULL)
    {
        int found = 0;

        /* Print out message until start of the token is found */
        while(str < s)
        {
            fprintf(stderr, "%c", *str);
            str++;
        }

        /* Look for the token name */
        for(t = 0; t < tokArrayLen && !found; t++)
        {
            if(strncmp(tokNames[t], str, strlen(tokNames[t])) == 0)
            {
                /* Dump the replacement string */
                fprintf(stderr, "%s", tokRepl[t]);

                /* Skip the token name */
                str += strlen(tokNames[t]);

                /* Exit the loop */
                found = 1;
            }
        }

        /* Check if a replacement was found */
        if(!found)
        {
            /* Dump the next char and skip it so that TOK doesn't match again */
            fprintf(stderr, "%c", *str);
            str++;
        }

        s = (char *)strstr(str, "TOK_");
    }

    fprintf(stderr, "%s.\n", str);

    line = lex_getline();
    if(line != NULL)
    {
        fprintf(stderr, "> %s\n", line);

        /* If the input line contains a 'lost arc', print a helpful note since
         *  without whitespace, this can confuse the lexer.
         */
        if(strstr(line, "x-") != NULL)
        {
            fprintf(stderr, "\nNote: This input line contains 'x-' which has special meaning as a \n"
                            "      'lost message' arc, but may not have been recognised as such if it\n"
                            "      is preceded by other letters or numbers.  Please use double-quoted\n"
                            "      strings for tokens before 'x-', or insert a preceding whitespace if\n"
                            "      this is what you intend.\n");
        }
    }
    else
    {
        fprintf(stderr, ".\n");
    }
}

int yywrap()
{
    return 1;
}


char *removeEscapes(char *in)
{
    const uint16_t l = strlen(in);
    char          *r = (char *)malloc_s(l + 1);
    uint16_t       t, u;

    for(t = u = 0; t < l; t++)
    {
        r[u] = in[t];
        if(in[t] != '\\' || in[t + 1] != '\"')
        {
            u++;
        }
    }

    r[u] = '\0';

    free(in);

    return r;
}

extern FILE *yyin;
extern int   yyparse (void *YYPARSE_PARAM);


Msc MscParse(FILE *in)
{
    Msc m;

    yyin = in;

    /* Parse, and check that no errors are found */
    if(yyparse((void *)&m) != 0)
    {
        m = NULL;
    }

    lex_destroy();
    yylex_destroy();

    return m;
}




/* Line 189 of yacc.c  */
#line 300 "language.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_STRING = 258,
     TOK_QSTRING = 259,
     TOK_EQUAL = 260,
     TOK_COMMA = 261,
     TOK_SEMICOLON = 262,
     TOK_OCBRACKET = 263,
     TOK_CCBRACKET = 264,
     TOK_OSBRACKET = 265,
     TOK_CSBRACKET = 266,
     TOK_MSC = 267,
     TOK_ATTR_LABEL = 268,
     TOK_ATTR_URL = 269,
     TOK_ATTR_ID = 270,
     TOK_ATTR_IDURL = 271,
     TOK_ATTR_LINE_COLOUR = 272,
     TOK_ATTR_TEXT_COLOUR = 273,
     TOK_ATTR_TEXT_BGCOLOUR = 274,
     TOK_ATTR_ARC_LINE_COLOUR = 275,
     TOK_ATTR_ARC_TEXT_COLOUR = 276,
     TOK_ATTR_ARC_TEXT_BGCOLOUR = 277,
     TOK_REL_LOSS_TO = 278,
     TOK_REL_LOSS_FROM = 279,
     TOK_REL_SIG_BI = 280,
     TOK_REL_SIG_TO = 281,
     TOK_REL_SIG_FROM = 282,
     TOK_REL_METHOD_BI = 283,
     TOK_REL_METHOD_TO = 284,
     TOK_REL_METHOD_FROM = 285,
     TOK_REL_RETVAL_BI = 286,
     TOK_REL_RETVAL_TO = 287,
     TOK_REL_RETVAL_FROM = 288,
     TOK_REL_DOUBLE_BI = 289,
     TOK_REL_DOUBLE_TO = 290,
     TOK_REL_DOUBLE_FROM = 291,
     TOK_REL_CALLBACK_BI = 292,
     TOK_REL_CALLBACK_TO = 293,
     TOK_REL_CALLBACK_FROM = 294,
     TOK_REL_BOX = 295,
     TOK_REL_ABOX = 296,
     TOK_REL_RBOX = 297,
     TOK_REL_NOTE = 298,
     TOK_SPECIAL_ARC = 299,
     TOK_OPT_HSCALE = 300,
     TOK_OPT_WIDTH = 301,
     TOK_OPT_ARCGRADIENT = 302,
     TOK_OPT_WORDWRAPARCS = 303,
     TOK_ASTERISK = 304,
     TOK_UNKNOWN = 305,
     TOK_REL_SIG = 306,
     TOK_REL_METHOD = 307,
     TOK_REL_RETVAL = 308,
     TOK_REL_DOUBLE = 309,
     TOK_ATTR_ARC_SKIP = 310
   };
#endif
/* Tokens.  */
#define TOK_STRING 258
#define TOK_QSTRING 259
#define TOK_EQUAL 260
#define TOK_COMMA 261
#define TOK_SEMICOLON 262
#define TOK_OCBRACKET 263
#define TOK_CCBRACKET 264
#define TOK_OSBRACKET 265
#define TOK_CSBRACKET 266
#define TOK_MSC 267
#define TOK_ATTR_LABEL 268
#define TOK_ATTR_URL 269
#define TOK_ATTR_ID 270
#define TOK_ATTR_IDURL 271
#define TOK_ATTR_LINE_COLOUR 272
#define TOK_ATTR_TEXT_COLOUR 273
#define TOK_ATTR_TEXT_BGCOLOUR 274
#define TOK_ATTR_ARC_LINE_COLOUR 275
#define TOK_ATTR_ARC_TEXT_COLOUR 276
#define TOK_ATTR_ARC_TEXT_BGCOLOUR 277
#define TOK_REL_LOSS_TO 278
#define TOK_REL_LOSS_FROM 279
#define TOK_REL_SIG_BI 280
#define TOK_REL_SIG_TO 281
#define TOK_REL_SIG_FROM 282
#define TOK_REL_METHOD_BI 283
#define TOK_REL_METHOD_TO 284
#define TOK_REL_METHOD_FROM 285
#define TOK_REL_RETVAL_BI 286
#define TOK_REL_RETVAL_TO 287
#define TOK_REL_RETVAL_FROM 288
#define TOK_REL_DOUBLE_BI 289
#define TOK_REL_DOUBLE_TO 290
#define TOK_REL_DOUBLE_FROM 291
#define TOK_REL_CALLBACK_BI 292
#define TOK_REL_CALLBACK_TO 293
#define TOK_REL_CALLBACK_FROM 294
#define TOK_REL_BOX 295
#define TOK_REL_ABOX 296
#define TOK_REL_RBOX 297
#define TOK_REL_NOTE 298
#define TOK_SPECIAL_ARC 299
#define TOK_OPT_HSCALE 300
#define TOK_OPT_WIDTH 301
#define TOK_OPT_ARCGRADIENT 302
#define TOK_OPT_WORDWRAPARCS 303
#define TOK_ASTERISK 304
#define TOK_UNKNOWN 305
#define TOK_REL_SIG 306
#define TOK_REL_METHOD 307
#define TOK_REL_RETVAL 308
#define TOK_REL_DOUBLE 309
#define TOK_ATTR_ARC_SKIP 310




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 248 "language.y"

    char         *string;
    Msc           msc;
    MscOpt        opt;
    MscOptType    optType;
    MscArc        arc;
    MscArcList    arclist;
    MscArcType    arctype;
    MscEntity     entity;
    MscEntityList entitylist;
    MscAttrib     attrib;
    MscAttribType attribType;



/* Line 214 of yacc.c  */
#line 462 "language.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 474 "language.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   129

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  56
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  68
/* YYNRULES -- Number of states.  */
#define YYNSTATES  102

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   310

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,    13,    21,    23,    27,    31,    33,    35,
      37,    39,    41,    45,    47,    52,    54,    58,    62,    67,
      69,    71,    75,    79,    83,    87,    91,    95,    99,   101,
     103,   105,   107,   109,   111,   113,   115,   117,   119,   121,
     123,   125,   127,   129,   131,   133,   135,   137,   139,   141,
     143,   145,   147,   149,   151,   155,   159,   161,   163,   165,
     167,   169,   171,   173,   175,   177,   179,   181,   183
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      57,     0,    -1,    12,     8,    58,     7,    61,     7,    63,
       7,     9,    -1,    12,     8,    61,     7,    63,     7,     9,
      -1,    59,    -1,    58,     6,    59,    -1,    60,     5,    74,
      -1,    45,    -1,    46,    -1,    47,    -1,    48,    -1,    62,
      -1,    61,     6,    62,    -1,    74,    -1,    62,    10,    71,
      11,    -1,    64,    -1,    63,     7,    64,    -1,    63,     6,
      64,    -1,    65,    10,    71,    11,    -1,    65,    -1,    44,
      -1,    74,    66,    74,    -1,    74,    68,    74,    -1,    74,
      69,    74,    -1,    74,    67,    74,    -1,    74,    70,    74,
      -1,    74,    69,    49,    -1,    49,    70,    74,    -1,    40,
      -1,    41,    -1,    42,    -1,    43,    -1,    51,    -1,    52,
      -1,    53,    -1,    54,    -1,    25,    -1,    28,    -1,    31,
      -1,    37,    -1,    34,    -1,    26,    -1,    29,    -1,    32,
      -1,    38,    -1,    35,    -1,    23,    -1,    27,    -1,    30,
      -1,    33,    -1,    39,    -1,    36,    -1,    24,    -1,    72,
      -1,    71,     6,    72,    -1,    73,     5,    74,    -1,    13,
      -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,    18,
      -1,    19,    -1,    20,    -1,    21,    -1,    22,    -1,    55,
      -1,     4,    -1,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   287,   287,   293,   300,   301,   306,   311,   311,   311,
     311,   313,   317,   324,   328,   334,   338,   342,   351,   355,
     357,   361,   365,   371,   375,   381,   385,   389,   394,   394,
     394,   394,   395,   395,   395,   395,   396,   396,   396,   396,
     396,   397,   397,   397,   397,   397,   397,   398,   398,   398,
     398,   398,   398,   400,   401,   406,   411,   411,   411,   411,
     412,   412,   412,   413,   413,   413,   414,   417,   421
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_STRING", "TOK_QSTRING", "TOK_EQUAL",
  "TOK_COMMA", "TOK_SEMICOLON", "TOK_OCBRACKET", "TOK_CCBRACKET",
  "TOK_OSBRACKET", "TOK_CSBRACKET", "TOK_MSC", "TOK_ATTR_LABEL",
  "TOK_ATTR_URL", "TOK_ATTR_ID", "TOK_ATTR_IDURL", "TOK_ATTR_LINE_COLOUR",
  "TOK_ATTR_TEXT_COLOUR", "TOK_ATTR_TEXT_BGCOLOUR",
  "TOK_ATTR_ARC_LINE_COLOUR", "TOK_ATTR_ARC_TEXT_COLOUR",
  "TOK_ATTR_ARC_TEXT_BGCOLOUR", "TOK_REL_LOSS_TO", "TOK_REL_LOSS_FROM",
  "TOK_REL_SIG_BI", "TOK_REL_SIG_TO", "TOK_REL_SIG_FROM",
  "TOK_REL_METHOD_BI", "TOK_REL_METHOD_TO", "TOK_REL_METHOD_FROM",
  "TOK_REL_RETVAL_BI", "TOK_REL_RETVAL_TO", "TOK_REL_RETVAL_FROM",
  "TOK_REL_DOUBLE_BI", "TOK_REL_DOUBLE_TO", "TOK_REL_DOUBLE_FROM",
  "TOK_REL_CALLBACK_BI", "TOK_REL_CALLBACK_TO", "TOK_REL_CALLBACK_FROM",
  "TOK_REL_BOX", "TOK_REL_ABOX", "TOK_REL_RBOX", "TOK_REL_NOTE",
  "TOK_SPECIAL_ARC", "TOK_OPT_HSCALE", "TOK_OPT_WIDTH",
  "TOK_OPT_ARCGRADIENT", "TOK_OPT_WORDWRAPARCS", "TOK_ASTERISK",
  "TOK_UNKNOWN", "TOK_REL_SIG", "TOK_REL_METHOD", "TOK_REL_RETVAL",
  "TOK_REL_DOUBLE", "TOK_ATTR_ARC_SKIP", "$accept", "msc", "optlist",
  "opt", "optval", "entitylist", "entity", "arclist", "arc", "arcrel",
  "relation_box", "relation_line", "relation_bi", "relation_to",
  "relation_from", "attrlist", "attr", "attrval", "string", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    56,    57,    57,    58,    58,    59,    60,    60,    60,
      60,    61,    61,    62,    62,    63,    63,    63,    64,    64,
      65,    65,    65,    65,    65,    65,    65,    65,    66,    66,
      66,    66,    67,    67,    67,    67,    68,    68,    68,    68,
      68,    69,    69,    69,    69,    69,    69,    70,    70,    70,
      70,    70,    70,    71,    71,    72,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    74,    74
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     9,     7,     1,     3,     3,     1,     1,     1,
       1,     1,     3,     1,     4,     1,     3,     3,     4,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     1,    68,    67,     7,     8,     9,
      10,     0,     4,     0,     0,    11,    13,     0,     0,     0,
       0,     0,     0,     5,     0,     6,    12,    20,     0,     0,
      15,    19,     0,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,     0,    53,     0,     0,    52,    47,
      48,    49,    51,    50,     0,     0,     0,     0,    46,    36,
      41,    37,    42,    38,    43,    40,    45,    39,    44,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,     0,
       0,     0,     0,    14,     0,     0,    27,    17,     3,    16,
       0,    21,    24,    22,    26,    23,    25,    54,    55,     0,
      18,     2
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,    11,    12,    13,    14,    15,    29,    30,    31,
      77,    78,    79,    80,    54,    44,    45,    46,    32
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -36
static const yytype_int8 yypact[] =
{
      -6,    35,    48,    15,   -36,   -36,   -36,   -36,   -36,   -36,
     -36,    16,   -36,     3,    34,    40,   -36,   -10,    42,    42,
      42,     8,    11,   -36,    47,   -36,    40,   -36,    90,    49,
     -36,    55,    59,   -36,   -36,   -36,   -36,   -36,   -36,   -36,
     -36,   -36,   -36,   -36,    -1,   -36,    53,     8,   -36,   -36,
     -36,   -36,   -36,   -36,    42,     8,    -2,    11,   -36,   -36,
     -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,
     -36,   -36,   -36,   -36,   -36,   -36,   -36,    42,    42,    42,
      10,    42,    11,   -36,    42,    61,   -36,   -36,   -36,   -36,
      28,   -36,   -36,   -36,   -36,   -36,   -36,   -36,   -36,     0,
     -36,   -36
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -36,   -36,   -36,    52,   -36,    54,    50,    24,   -35,   -36,
     -36,   -36,   -36,   -36,    41,    22,    21,   -36,    -3
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      16,     5,     6,     5,     6,    82,     1,    88,    19,   101,
      83,     5,     6,     5,     6,    16,    25,    16,     5,     6,
      87,    89,    17,    18,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    82,     7,     8,     9,    10,   100,
      20,    21,    27,     3,    27,     5,     6,    28,     4,    28,
      22,    86,    27,    20,    47,    55,    56,    28,    84,    94,
       7,     8,     9,    10,    89,    57,    43,    55,    99,    23,
      26,    85,    24,    81,    91,    92,    93,    95,    96,    90,
       0,    98,    58,    48,    59,    60,    49,    61,    62,    50,
      63,    64,    51,    65,    66,    52,    67,    68,    53,    69,
      70,    71,    72,    97,     0,     0,     0,     0,     0,     0,
      73,    74,    75,    76,    48,     0,     0,    49,     0,     0,
      50,     0,     0,    51,     0,     0,    52,     0,     0,    53
};

static const yytype_int8 yycheck[] =
{
       3,     3,     4,     3,     4,     6,    12,     9,     5,     9,
      11,     3,     4,     3,     4,    18,    19,    20,     3,     4,
      55,    56,     6,     7,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,     6,    45,    46,    47,    48,    11,
       6,     7,    44,     8,    44,     3,     4,    49,     0,    49,
      10,    54,    44,     6,     7,     6,     7,    49,     5,    49,
      45,    46,    47,    48,    99,    10,    55,     6,     7,    17,
      20,    47,    18,    32,    77,    78,    79,    80,    81,    57,
      -1,    84,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      51,    52,    53,    54,    24,    -1,    -1,    27,    -1,    -1,
      30,    -1,    -1,    33,    -1,    -1,    36,    -1,    -1,    39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    12,    57,     8,     0,     3,     4,    45,    46,    47,
      48,    58,    59,    60,    61,    62,    74,     6,     7,     5,
       6,     7,    10,    59,    61,    74,    62,    44,    49,    63,
      64,    65,    74,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    55,    71,    72,    73,     7,    24,    27,
      30,    33,    36,    39,    70,     6,     7,    10,    23,    25,
      26,    28,    29,    31,    32,    34,    35,    37,    38,    40,
      41,    42,    43,    51,    52,    53,    54,    66,    67,    68,
      69,    70,     6,    11,     5,    63,    74,    64,     9,    64,
      71,    74,    74,    74,    49,    74,    74,    72,    74,     7,
      11,     9
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 288 "language.y"
    {
    (yyval.msc) = MscAlloc((yyvsp[(3) - (9)].opt), (yyvsp[(5) - (9)].entitylist), (yyvsp[(7) - (9)].arclist));
    *(Msc *)yyparse_result = (yyval.msc);

}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 294 "language.y"
    {
    (yyval.msc) = MscAlloc(NULL, (yyvsp[(3) - (7)].entitylist), (yyvsp[(5) - (7)].arclist));
    *(Msc *)yyparse_result = (yyval.msc);

}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 302 "language.y"
    {
    (yyval.opt) = MscLinkOpt((yyvsp[(1) - (3)].opt), (yyvsp[(3) - (3)].opt));
}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 307 "language.y"
    {
    (yyval.opt) = MscAllocOpt((yyvsp[(1) - (3)].optType), (yyvsp[(3) - (3)].string));
}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 314 "language.y"
    {
    (yyval.entitylist) = MscLinkEntity(NULL, (yyvsp[(1) - (1)].entity));   /* Create new list */
}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 318 "language.y"
    {
    (yyval.entitylist) = MscLinkEntity((yyvsp[(1) - (3)].entitylist), (yyvsp[(3) - (3)].entity));     /* Add to existing list */
}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 325 "language.y"
    {
    (yyval.entity) = MscAllocEntity((yyvsp[(1) - (1)].string));
}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 329 "language.y"
    {
    MscEntityLinkAttrib((yyvsp[(1) - (4)].entity), (yyvsp[(3) - (4)].attrib));
}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 335 "language.y"
    {
    (yyval.arclist) = MscLinkArc(NULL, (yyvsp[(1) - (1)].arc));      /* Create new list */
}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 339 "language.y"
    {
    (yyval.arclist) = MscLinkArc((yyvsp[(1) - (3)].arclist), (yyvsp[(3) - (3)].arc));     /* Add to existing list */
}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 343 "language.y"
    {
    /* Add a special 'parallel' arc */
    (yyval.arclist) = MscLinkArc(MscLinkArc((yyvsp[(1) - (3)].arclist), MscAllocArc(NULL, NULL, MSC_ARC_PARALLEL, lex_getlinenum())), (yyvsp[(3) - (3)].arc));
}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 352 "language.y"
    {
    MscArcLinkAttrib((yyvsp[(1) - (4)].arc), (yyvsp[(3) - (4)].attrib));
}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 358 "language.y"
    {
    (yyval.arc) = MscAllocArc(NULL, NULL, (yyvsp[(1) - (1)].arctype), lex_getlinenum());
}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 362 "language.y"
    {
    (yyval.arc) = MscAllocArc((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string), (yyvsp[(2) - (3)].arctype), lex_getlinenum());
}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 366 "language.y"
    {
    MscArc arc = MscAllocArc((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string), (yyvsp[(2) - (3)].arctype), lex_getlinenum());
    MscArcLinkAttrib(arc, MscAllocAttrib(MSC_ATTR_BI_ARROWS, strdup_s("true")));
    (yyval.arc) = arc;
}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 372 "language.y"
    {
    (yyval.arc) = MscAllocArc((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string), (yyvsp[(2) - (3)].arctype), lex_getlinenum());
}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 376 "language.y"
    {
    MscArc arc = MscAllocArc((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string), (yyvsp[(2) - (3)].arctype), lex_getlinenum());
    MscArcLinkAttrib(arc, MscAllocAttrib(MSC_ATTR_NO_ARROWS, strdup_s("true")));
    (yyval.arc) = arc;
}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 382 "language.y"
    {
    (yyval.arc) = MscAllocArc((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string), (yyvsp[(2) - (3)].arctype), lex_getlinenum());
}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 386 "language.y"
    {
    (yyval.arc) = MscAllocArc((yyvsp[(1) - (3)].string), strdup_s("*"), (yyvsp[(2) - (3)].arctype), lex_getlinenum());
}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 390 "language.y"
    {
    (yyval.arc) = MscAllocArc((yyvsp[(3) - (3)].string), strdup_s("*"), (yyvsp[(2) - (3)].arctype), lex_getlinenum());
}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 402 "language.y"
    {
    (yyval.attrib) = MscLinkAttrib((yyvsp[(1) - (3)].attrib), (yyvsp[(3) - (3)].attrib));
}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 407 "language.y"
    {
    (yyval.attrib) = MscAllocAttrib((yyvsp[(1) - (3)].attribType), (yyvsp[(3) - (3)].string));
}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 418 "language.y"
    {
    (yyval.string) = removeEscapes((yyvsp[(1) - (1)].string));
}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 422 "language.y"
    {
    (yyval.string) = (yyvsp[(1) - (1)].string);
}
    break;



/* Line 1455 of yacc.c  */
#line 2003 "language.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 425 "language.y"



/* END OF FILE */

