/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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
#include "constexp_p.h"
#include "message.h"

#include <stdio.h>
#include <stdlib.h>

int constexpYYerror(yyscan_t yyscanner, const char *s)
{
  struct constexpYY_state* yyextra = constexpYYget_extra(yyscanner);
  warn(yyextra->constExpFileName, yyextra->constExpLineNr,
       "preprocessing issue while doing constant expression evaluation: {}:\n    input='{}'\n    doxygen interpretation '{}'",
       s,yyextra->orgString,yyextra->inputString);
  return 0;
}

%}

%define api.prefix {constexpYY}
%define api.pure full
%lex-param {yyscan_t yyscanner}
%parse-param {yyscan_t yyscanner}

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
%token TOK_BINARYINT
%token TOK_CHARACTER
%token TOK_FLOAT

%%

start: constant_expression
       {
         struct constexpYY_state* yyextra = constexpYYget_extra(yyscanner);
         /* dummy statement to silence a 'set but not used' compiler warning */ (void)yynerrs;
         yyextra->resultValue = $1; return 0;
       }
;

constant_expression: logical_or_expression
                     { $$ = $1; }
	           | logical_or_expression
                     TOK_QUESTIONMARK logical_or_expression
                     TOK_COLON logical_or_expression
		     {
		       bool c = ($1.type()==CPPValue::Type::Int ? ((long)$1 != 0) : ((double)$1 != 0.0));
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
		       if ($1.type()!=CPPValue::Type::Int || $3.type()!=CPPValue::Type::Int)
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
		       if ($1.type()!=CPPValue::Type::Int || $3.type()!=CPPValue::Type::Int)
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
			     if ($1.type()!=CPPValue::Type::Int || $3.type()!=CPPValue::Type::Int)
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
			     if ($1.type()!=CPPValue::Type::Int || $3.type()!=CPPValue::Type::Int)
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
		    if ($2.type()==CPPValue::Type::Int)
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
	  {
	    struct constexpYY_state* yyextra = constexpYYget_extra(yyscanner);
	    $$ = CPPValue::parseOctal(yyextra->strToken);
	  }
	| TOK_DECIMALINT
	  {
	    struct constexpYY_state* yyextra = constexpYYget_extra(yyscanner);
	    $$ = CPPValue::parseDecimal(yyextra->strToken);
	  }
	| TOK_HEXADECIMALINT
	  {
	    struct constexpYY_state* yyextra = constexpYYget_extra(yyscanner);
	    $$ = CPPValue::parseHexadecimal(yyextra->strToken);
	  }
	| TOK_BINARYINT
	  {
	    struct constexpYY_state* yyextra = constexpYYget_extra(yyscanner);
	    $$ = CPPValue::parseBinary(yyextra->strToken);
	  }
	| TOK_CHARACTER
	  {
	    struct constexpYY_state* yyextra = constexpYYget_extra(yyscanner);
	    $$ = CPPValue::parseCharacter(yyextra->strToken);
	  }
	| TOK_FLOAT
	  {
	    struct constexpYY_state* yyextra = constexpYYget_extra(yyscanner);
	    $$ = CPPValue::parseFloat(yyextra->strToken);
	  }
;

%%
