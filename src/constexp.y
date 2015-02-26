/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
 */

%{

#include "cppvalue.h"
#include "constexp.h"
#include "message.h"

#if defined(_MSC_VER)
#define MSDOS
#endif

#define YYSTYPE CPPValue

#include <stdio.h>
#include <stdlib.h>

int constexpYYerror(const char *s)
{
  warn(g_constExpFileName,g_constExpLineNr,
       "preprocessing issue while doing constant expression evaluation: %s",s);
  return 0;
}

int constexpYYlex();

%}

%token TOK_QUESTIONMARK
%token TOK_COLON
%token TOK_OR
%token TOK_AND
%token TOK_BITWISEOR
%token TOK_BITWISEXOR
%token TOK_AMPERSAND
%token TOK_NOTEQUAL
%token TOK_EQUAL
%token TOK_LESSTHAN
%token TOK_GREATERTHAN
%token TOK_LESSTHANOREQUALTO
%token TOK_GREATERTHANOREQUALTO
%token TOK_SHIFTLEFT
%token TOK_SHIFTRIGHT
%token TOK_PLUS
%token TOK_MINUS
%token TOK_STAR
%token TOK_DIVIDE
%token TOK_MOD
%token TOK_TILDE
%token TOK_NOT
%token TOK_LPAREN
%token TOK_RPAREN
%token TOK_OCTALINT
%token TOK_DECIMALINT
%token TOK_HEXADECIMALINT
%token TOK_CHARACTER
%token TOK_FLOAT

%%

start: constant_expression
       { g_resultValue = $1; return 0; }
;

constant_expression: logical_or_expression
                     { $$ = $1; }
	           | logical_or_expression 
                     TOK_QUESTIONMARK logical_or_expression 
                     TOK_COLON logical_or_expression
		     {
		       bool c = ($1.isInt() ? ((long)$1 != 0) : ((double)$1 != 0.0));
		       $$ = c ? $3 : $5;
	             }
;

logical_or_expression: logical_and_expression
		       { $$ = $1; }
                     | logical_or_expression TOK_OR logical_and_expression
		       {
			 $$ = CPPValue( (long)((long)$1 || (long)$3) );
		       }
;

logical_and_expression: inclusive_or_expression
			{ $$ = $1; }
		      | logical_and_expression TOK_AND inclusive_or_expression
			{
			  $$ = CPPValue( (long)((long)$1 && (long)$3) );
			}
;

inclusive_or_expression: exclusive_or_expression
			 { $$ = $1; }
		       | inclusive_or_expression TOK_BITWISEOR 
                         exclusive_or_expression
			 { 
			   $$ = CPPValue( (long)$1 | (long)$3 );
			 }
;

exclusive_or_expression: and_expression
			 { $$ = $1; }
		       | exclusive_or_expression TOK_BITWISEXOR and_expression
			 {
			   $$ = CPPValue( (long)$1 ^ (long)$3 );
			 }
;

and_expression:	equality_expression
		{ $$ = $1; }
	      | and_expression TOK_AMPERSAND equality_expression
		{ 
		  $$ = CPPValue( (long)$1 & (long)$3 );
		}
;

equality_expression: relational_expression
		     { $$ = $1; }
		   | equality_expression TOK_EQUAL relational_expression
		     { 
		       $$ = CPPValue( (long)((double)$1 == (double)$3) );
	             }
		   | equality_expression TOK_NOTEQUAL relational_expression
		     {
                       $$ = CPPValue( (long)((double)$1 != (double)$3) );
		     }
;

relational_expression: shift_expression
		       { $$ = $1; }
		     | relational_expression TOK_LESSTHAN shift_expression
		       { 
			 $$ = CPPValue( (long)((double)$1 < (double)$3) );
		       }
		     | relational_expression TOK_GREATERTHAN shift_expression
		       {
                         $$ = CPPValue( (long)((double)$1 > (double)$3) );
		       }
		     | relational_expression TOK_LESSTHANOREQUALTO
		       shift_expression
		       {
		         $$ = CPPValue( (long)((double)$1 <= (double)$3) );
		       }
		     | relational_expression TOK_GREATERTHANOREQUALTO
		       shift_expression
		       {
			 $$ = CPPValue( (long)((double)$1 >= (double)$3) );
		       }
;

shift_expression: additive_expression
		  { $$ = $1; }
		| shift_expression TOK_SHIFTLEFT additive_expression
		  {
		    $$ = CPPValue( (long)$1 << (long)$3 );	
		  }
		| shift_expression TOK_SHIFTRIGHT additive_expression
		  {
		    $$ = CPPValue( (long)$1 >> (long)$3 );
		  }
;

additive_expression: multiplicative_expression
		     { $$ = $1; }
		   | additive_expression TOK_PLUS multiplicative_expression
		     {
		       if (!$1.isInt() || !$3.isInt())
		       {
		         $$ = CPPValue( (double)$1 + (double)$3 );
		       }
		       else	
		       {
		         $$ = CPPValue( (long)$1 + (long)$3 );
		       }
		     }
		   | additive_expression TOK_MINUS multiplicative_expression
		     {
		       if (!$1.isInt() || !$3.isInt())
		       {
		         $$ = CPPValue( (double)$1 - (double)$3 );
		       }
		       else	
		       {
		         $$ = CPPValue( (long)$1 - (long)$3 );
		       }
		     }
;

multiplicative_expression: unary_expression
			   { $$ = $1; }
			 | multiplicative_expression TOK_STAR unary_expression
			   { 
			     if (!$1.isInt() || !$3.isInt())
			     {
			       $$ = CPPValue( (double)$1 * (double)$3 );
			     }
			     else
			     {
			       $$ = CPPValue( (long)$1 * (long)$3 );
			     }
			   }
			 | multiplicative_expression TOK_DIVIDE unary_expression
			   { 
			     if (!$1.isInt() || !$3.isInt())
			     {
			       $$ = CPPValue( (double)$1 / (double)$3 );
			     }
			     else
			     {
			       long value = $3;
			       if (value==0) value=1;
			       $$ = CPPValue( (long)$1 / value );
			     }
			   }
			 | multiplicative_expression TOK_MOD unary_expression
			   { 
			     long value = $3;
			     if (value==0) value=1;
			     $$ = CPPValue( (long)$1 % value );
			   }
;

unary_expression: primary_expression
		  { $$ = $1; }
	        | TOK_PLUS unary_expression
		  { $$ = $1; }
		| TOK_MINUS unary_expression
		  { 
		    if ($2.isInt()) 
                      $$ = CPPValue(-(long)$2);
                    else
		      $$ = CPPValue(-(double)$2);
		  }
		| TOK_TILDE unary_expression
		  {
		    $$ = CPPValue(~(long)$2);
		  }
		| TOK_NOT unary_expression
		  {
		    $$ = CPPValue((long)!(long)$2);
		  }
;

primary_expression: constant
		    { $$ = $1; }
		  | TOK_LPAREN constant_expression TOK_RPAREN
		    { $$ = $2; }
;

constant: TOK_OCTALINT
	  { $$ = parseOctal(); }
	| TOK_DECIMALINT
	  { $$ = parseDecimal(); }
	| TOK_HEXADECIMALINT
	  { $$ = parseHexadecimal(); }
	| TOK_CHARACTER
	  { $$ = parseCharacter(); }
	| TOK_FLOAT
	  { $$ = parseFloat(); }
;

%%
