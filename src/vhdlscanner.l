/************** VHDL scanner in LEX format **********
 *
 * Version 0.2 Wed Aug 11, 1993
 *
 * This scanner is derived from a scanner of the ALLIANCE CAD toolset,
 * release 1.1. That toolset was written from:
 *   MASI/CAO-VLSI CAD Team  
 *   Laboratoire MASI/CAO-VLSI
 *   Tour 55-65, 2eme etage, Porte 13
 *   Universite Pierre et Marie Curie (PARIS VI)
 *   4, place Jussieu 75252 PARIS Cedex 05, FRANCE
 * The ALLIANCE CAD Toolset can be obtained from ftp site : ftp-masi.ibp.fr    
 *
 * This scanner is avail at: ftp.cs.utwente.nl in pub/src/VHDL/Grammar
 * A corresponding Yacc grammar is available at the same site
 *
 * author of this derived scanner version:
 * Thomas Dettmer
 * Dortmund University
 * Dept. of Computer Scienc, LS1
 * PB 500 500
 * D-44221 Dortmund (Germany)
 * Phone: +49-231-755-6464
 * e-mail: dettmer@ls1.informatik.uni-dortmund.de
 *
 *
 ****************************************************************
 * 
 * This file is intended not to be used for commercial purposes
 * without permission of the University of Dortmund
 *   
 * NOTE THAT THERE IS NO WARRANTY FOR CORRECTNES, COMPLETENESS, SUPPORT
 * OR ANYTHING ELSE.
 *******************************************************/
/******************************************************************************
 * modified for doxygen by M. Kreis
 * extended to VHDL 93/2002/2008 
 ******************************************************************************/
%{

#ifndef YYSTYPE
	typedef int YYSTYPE;
#endif

#include <ctype.h>
#include <search.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "commentscan.h"
#include "vhdlparser.h"
#include "vhdlscanner.h"

#include "doxygen.h"
#include "searchindex.h"
#include <ctype.h>
#include "scanner.h"
#include "vhdldocgen.h"
#include "util.h"
#include "bufstr.h"
#include "message.h"
#include "vhdlcode.h"
#include <qmap.h>
#include "entry.h"

#define YY_NO_INPUT 1

extern void vhdlParse(); // defined in vhdlparser.y

static bool          g_lexInit = FALSE;
static ParserInterface *g_thisParser;
static VhdlContainer* yycont=NULL;
static Entry* current_root;
static Entry   gBlock;

static int yyLineNr =1;
static int   g_lastCommentContext;
static int              inputPosition;
static int startComment;
static QCString inputVhdlString;
static QFile            inputFile;
static QCString         yyFileName;
static QList<QCString> qlist;
static QCString lastLetter;

static bool doxComment=FALSE; // doxygen comment ? 
static QCString strComment;
static int iDocLine=-1;
static int* lineIndex=NULL;
static int num_chars;
static  int prevToken;
static int iCodeLen;

static QMap<QCString, int> keyMap;
static QList<Entry>  lineEntry;
static bool  checkMultiComment(QCString& qcs,int line);
static void handleCommentBlock(const QCString &doc,bool brief);
static void mapLibPackage(const Entry* ce);
static QList<Entry>* getEntryAtLine(const Entry* ce,int line);
static bool addLibUseClause(const QCString &type);
static Entry* oldEntry;
static bool varr=FALSE;
static QCString varName;

static struct 
{
  QCString doc;
  bool brief;
  bool pending;
  int iDocLine;
} str_doc;

#define YY_NEVER_INTERACTIVE 1
#define YY_USER_ACTION num_chars += vhdlScanYYleng;

#define MAX_KEYWORD_LEN 20

typedef struct 
{
  char nom[MAX_KEYWORD_LEN];
  int kval;
} el_mc;

static el_mc tab_mc []=
{
    { "abs",                t_ABS                },
    { "access",             t_ACCESS             },
    { "after",              t_AFTER              },
    { "alias",              t_ALIAS              },
    { "all",                t_ALL                },
    { "and",                t_AND                },
    { "architecture",       t_ARCHITECTURE       },
    { "array",              t_ARRAY              },
    { "assert",             t_ASSERT             },
    { "assume",             t_ASSUME             },
    { "assume_guarantee",   t_ASSUME_GUARANTEE   },
    { "attribute",          t_ATTRIBUTE          },

    { "begin",              t_BEGIN              },
    { "block",              t_BLOCK              },
    { "body",               t_BODY               },
    { "buffer",             t_BUFFER             },
    { "bus",                t_BUS                },

    { "case",               t_CASE               },
    { "component",          t_COMPONENT          },
    { "configuration",      t_CONFIGURATION      },
    { "constant",           t_CONSTANT           },
    { "context",            t_CONTEXT            },
    { "cover",              t_COVER              },

    { "default",            t_DEFAULT            },
    { "disconnect",         t_DISCONNECT         },
    { "downto",             t_DOWNTO             },

    { "else",               t_ELSE               },
    { "elsif",              t_ELSIF              },
    { "end",                t_END                },
    { "entity",             t_ENTITY             },
    { "exit",               t_EXIT               },

    { "fairness",           t_FAIRNESS           },
    { "file",               t_FILE               },
    { "for",                t_FOR                },
    { "force",              t_FORCE              },
    { "function",           t_FUNCTION           },

    { "generate",           t_GENERATE           },
    { "generic",            t_GENERIC            },
    { "group",              t_GROUP              },
    { "guarded",            t_GUARDED            },

    { "if",                 t_IF                 },
    { "impure",             t_IMPURE             },
    { "in",                 t_IN                 },
    { "inertial",           t_INERTIAL           },
    { "inout",              t_INOUT              },
    { "is",                 t_IS                 },
  
    { "label",              t_LABEL              },
    { "library",            t_LIBRARY            },
    { "linkage",            t_LINKAGE            },
    { "literal",            t_LITERAL            },
    { "loop",               t_LOOP               },

    { "map",                t_MAP                },
    { "mod",                t_MOD                },

    { "nand",               t_NAND               },
    { "new",                t_NEW                },
    { "next",               t_NEXT               },
    { "nor",                t_NOR                },
    { "not",                t_NOT                },
    { "null",               t_NULL               },

    { "of",                 t_OF                 },
    { "on",                 t_ON                 },
    { "open",               t_OPEN               },
    { "or",                 t_OR                 },
    { "others",             t_OTHERS             },
    { "out",                t_OUT                },

    { "package",            t_PACKAGE            },
    { "parameter",          t_PARAMETER          },
    { "port",               t_PORT               },
    { "postponed",          t_POSTPONED          },
    { "procedure",          t_PROCEDURE          },
    { "process",            t_PROCESS            },
    { "property",           t_PROPERTY           },
    { "protected",          t_PROTECTED          },
    { "pure",               t_PURE               },
  
    { "range",              t_RANGE              },
    { "record",             t_RECORD             },
    { "register",           t_REGISTER           },
    { "reject",             t_REJECT             },
    { "release",            t_RELEASE            },
    { "restrict",           t_RESTRICT           },
    { "restrict_guarantee", t_RESTRICT_GUARANTEE },
    { "rem",                t_REM                },
    { "report",             t_REPORT             },
    { "rol",                t_ROL                },
    { "ror",                t_ROR                },
    { "return",             t_RETURN             },

    { "select",             t_SELECT             },
    { "sequence",           t_SEQUENCE           },
    { "severity",           t_SEVERITY           },
    { "signal",             t_SIGNAL             },
    { "shared",             t_SHARED             },
    { "sla",                t_SLA                },
    { "sll",                t_SLL                },
    { "sra",                t_SRA                },
    { "srl",                t_SRL                },
    { "strong",             t_STRONG             },
    { "subtype",            t_SUBTYPE            },

    { "then",               t_THEN               },
    { "to",                 t_TO                 },
    { "transport",          t_TRANSPORT          },
    { "type",               t_TYPE               },

    { "unaffected",         t_UNAFFECTED         },
    { "units",              t_UNITS              },
    { "until",              t_UNTIL              },
    { "use",                t_USE                },

    { "variable",           t_VARIABLE           },
    { "vmode",              t_VMODE              },
    { "vprop",              t_VPROP              },
    { "vunit",              t_VUNIT              },

    { "wait",               t_WAIT               },
    { "when",               t_WHEN               },
    { "while",              t_WHILE              },
    { "with",               t_WITH               },

    { "xor",                t_XOR                },
    { "xnor",               t_XNOR               },
    { "zz",                 -1                   } // list end
};

   
static int find_keyword(char *s)
{
  QCString word(s);
  // keyword ?
  if (word.length() >  MAX_KEYWORD_LEN) 
	return -1;

  word=word.lower();
  QMap<QCString, int>::Iterator it = keyMap.find(word);
  if (it.key())
	return it.data();

  return -1;
}

// update current line
static void lineCount()
{
  for (const char* c=vhdlScanYYtext ; *c ; ++c )
  {
    yyLineNr += (*c == '\n') ;
  }
}


static void startCodeBlock(int index){
  int ll=strComment.length();
  iCodeLen=inputVhdlString.findRev(strComment.data())+ll;
  // fprintf(stderr,"\n startin code..%d %d %d\n",iCodeLen,num_chars,ll);
  //assert(false);
  gBlock.reset();
  int len=strComment.length();
  QCString name=strComment.right(len-index);// 
  name=VhdlDocGen::getIndexWord(name.data(),1);
  if (!name)
    gBlock.name="misc"+ VhdlDocGen::getRecordNumber(); 
  else
    gBlock.name=name;
  strComment=strComment.left(index);
  gBlock.startLine=yyLineNr+1;
  gBlock.bodyLine=yyLineNr+1;
  VhdlDocGen::prepareComment(strComment);
  gBlock.brief+=strComment;
}
static void makeInlineDoc(int endCode)
{
  int len=endCode-iCodeLen;
  QCString par=inputVhdlString.mid(iCodeLen,len);
  gBlock.doc=par;
  gBlock.inbodyDocs=par;
  gBlock.section=Entry::VARIABLE_SEC;
  gBlock.spec=VhdlDocGen::MISCELLANEOUS;
  gBlock.fileName = yyFileName;
  gBlock.endBodyLine=yyLineNr-1;
  gBlock.lang=SrcLangExt_VHDL;
  Entry *temp=new Entry(gBlock);

  Entry* compound=getVhdlCompound();

  if (compound)
  {
    compound->addSubEntry(temp);
  }
  else
  {
    temp->type="misc"; // global code like library ieee...
    current_root->addSubEntry(temp);
  }
  strComment.resize(0);
  gBlock.reset();


}// makeInlineDoc

static bool isConstraintFile(const QCString &fileName,const QCString &ext)
{
  return fileName.right(ext.length())==ext;
}

//static void resetScanner(const char* s,MyParserVhdl* parse);

#undef  YY_INPUT
#define	YY_INPUT(buf,result,max_size) result=vhdlScanYYread(buf,max_size);


static int vhdlScanYYread(char *buf,int max_size)
{
  int c=0;
  while ( c < max_size && inputVhdlString.at(inputPosition) )
  {
	*buf = inputVhdlString.at(inputPosition++) ;
	c++; buf++;
  }
  return c;
}

%}

upper_case_letter          [A-Z]
digit                      [0-9]
special_character          [\#\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\_\|]
space_character            [ \t]
format_effector            [\t\v\r\l\f]
end_of_line                \n
lower_case_letter          [a-z]
other_special_character    [\!\$\@\?\[\\\]\^\`\{\}\~]

graphic_character          ({basic_graphic_character}|{lower_case_letter}|{other_special_character})
basic_graphic_character    ({upper_case_letter}|{digit}|{special_character}|{space_character})
letter                     ({upper_case_letter}|{lower_case_letter})
letter_or_digit            ({letter}|{digit})
decimal_literal            {integer}(\.{integer})?({exponent})?
integer                    {digit}(_?{digit})*
exponent                   ([eE][-+]?{integer})
base                       {integer}
based_integer              {extended_digit}(_?{extended_digit})*
extended_digit             ({digit}|[a-fA-F])

extended_character         [\\]{graphic_character}*[\\] 

base_specifier             {digit}*(B|b|D|O|o|X|x|"UB"|"UO"|"UX"|"SB"|"SO"|"SX")

vhdl2008tooldir            `{graphic_character}+

B                          [ \t]
BR                         [ \t\n\r]


%option noyywrap
%x Comment
%x Vhdl2008Comment
%x EndVhdl2008Comment
%%

{space_character}  { /* nothing */ }
\&                 { return(t_Ampersand); }
\'                 { return(t_Apostrophe); }
\(                 { return(t_LeftParen); }
\)                 { return(t_RightParen); }
"**"               { return(t_DoubleStar); }
\*                 { return(t_Star); }
\+                 { return(t_Plus); }
\,                 { return(t_Comma); }
\-                 { return(t_Minus); }
":="               { return(t_VarAsgn); }
\:                 { return(t_Colon); }
\;                 { return(t_Semicolon); }
"<="               { return(t_LESym); }
">="               { return(t_GESym); }
\<                 { return(t_LTSym); }
\>                 { return(t_GTSym); }
\=                 { return(t_EQSym); }
\/=                { return(t_NESym); }
"=>"               { return(t_Arrow); }
"<>"               { return(t_Box); }
"<<"               { return(t_SLSL); }
">>"               { return(t_SRSR); }
"??"               { return(t_QQ); }
"?>="              { return(t_QGT); }
"?<="              { return(t_QLT); }
"?>"               { return(t_QG); }
"?<"               { return(t_QL); }
"?="               { return(t_QEQU); }
"?/="              { return(t_QNEQU); }
\?                 { return(t_Q); }
\|                 { return(t_Bar); }
\.                 { return(t_Dot); }
\/                 { return(t_Slash); }
\@                 { return(t_At); }
\^                 { return(t_Neg); }
\[                 { return(t_LEFTBR); }
\]                 { return(t_RIGHTBR); }


{letter}(_?{letter_or_digit})*|{extended_character} 	{
          int itoken=find_keyword(vhdlScanYYtext);
        
          //	fprintf(stderr,"\n <<<< search tok:  %s %d %d>>>\n",vhdlScanYYtext,itoken,yyLineNr);
        
          // tokens in vhdlparser.hpp 258..412
	  if (itoken>200  && itoken<500 && prevToken!=t_END)
	  {
	    //  	printf("\n <<<< insert tok:  %s %d %d>>>\n",vhdlScanYYtext,itoken,yyLineNr);
	    lineIndex[itoken]=yyLineNr;
	  }
	 
	 // global members
	 if (( itoken==t_ARCHITECTURE )  || 
             ( itoken==t_ENTITY)         || 
             ( itoken==t_PACKAGE )       || 
             ( itoken==t_LIBRARY )       ||  
             ( itoken==t_USE )           ||  
             ( itoken==t_CONFIGURATION ) ||  
             ( itoken==t_CONTEXT ) )
         {
	   lineIndex[itoken]=yyLineNr;
         }
	 
	  prevToken=itoken;
        
          yycont->qstr=vhdlScanYYtext;
          yycont->yyLineNr=yyLineNr;
          if (itoken== -1)  
          {
            yycont->iLine=yyLineNr;
            return ( t_LETTER );
          }
          else 
          {
            return ( itoken );
          }
        }

({decimal_literal})|({base}#{based_integer}(\.{based_integer})?#({exponent})?)|({base}:{based_integer}(\.{based_integer})?:({exponent})?) {
          yycont->qstr=vhdlScanYYtext;
          return ( t_ABSTRLIST );
        }

'({graphic_character}|\"|\%)' 	{
          QCString q(vhdlScanYYtext);	       
          yycont->qstr=vhdlScanYYtext; 
        
          if (q=="'('") // std_logic'('1') ?
          {
            char c=yy_hold_char;
            if (isalpha(c) || isdigit(c))
            {
              unput('\'');
              unput('(');
              return(t_Apostrophe);
            }
            else
	    {
              return ( t_CHARLIST );
	    }
          }
          return ( t_CHARLIST );
        }

(\"({graphic_character}|(\"\")|\%)*\")|(\%({graphic_character}|(\%\%)|\")*\%) {
          yycont->qstr=vhdlScanYYtext;
          yycont->iLine=yyLineNr;
	  return ( t_STRING );
        } 

{base_specifier}(\"{extended_digit}(_?{extended_digit})*\"|\%{extended_digit}(_?{extended_digit})*\%) 							{
	  yycont->qstr=vhdlScanYYtext;
	  yycont->iLine=yyLineNr;
	  return ( t_DIGIT );
        }

{vhdl2008tooldir} {
	  yycont->qstr=vhdlScanYYtext;
	  yycont->iLine=yyLineNr;
	  return(t_ToolDir);
        }	

\n	{
	  yyLineNr++;
          yycont->yyLineNr=yyLineNr;
        }

<*>"--"[^\n]* {
          /* comment */
	  QCString qcs(vhdlScanYYtext);
	  //  vhdl comment ?
	  if (qcs.stripPrefix("--!"))
	  {
	    REJECT;
	  }
        }               
.	{ /* unknown characters */ }
									

<*>{BR}*"--!"[^{}\n][^\n]*\n/{B}*"--!" { // multi line comment
  if (iDocLine==-1) iDocLine=yyLineNr;
  QCString qc(vhdlScanYYtext);
  int len=qc.contains('\n')+yyLineNr-1;
  if (YY_START!=Comment) // Start of the comment block
  {
    startComment=yyLineNr;
    g_lastCommentContext=YY_START;
  }
  
   if(!checkMultiComment(qc,len))
  {
    strComment+=vhdlScanYYtext;
  }
  lineCount();
  BEGIN(Comment);
}

<Comment>^{B}*"--!"[^\n]* {
  if (iDocLine==-1) iDocLine=yyLineNr;
  strComment+=vhdlScanYYtext;
  int index=strComment.find("\\code");
  if (index>0)
  {
    startCodeBlock(index);
    doxComment=TRUE;
  }
  lineCount();
  BEGIN(Comment);
}

<Comment>.|\n {
          // found end of comment block
          
          int index =strComment.find("\\code");
          if (index>0)
          {
            startCodeBlock(index);
          }

          VhdlDocGen::prepareComment(strComment);


          if (index==-1 && !doxComment) 
          {
            handleCommentBlock(strComment,FALSE);
          }
          strComment.resize(0);;
          unput(*vhdlScanYYtext);
          doxComment=FALSE;
          BEGIN(g_lastCommentContext);
        }
        
<*>"--!"[^\n]* { // one line comment
          if (iDocLine==-1) iDocLine=yyLineNr;
          QCString qcs(vhdlScanYYtext);

	  bool isEndCode=qcs.contains("\\endcode");

	  int index = qcs.find("\\code");
	  if (isEndCode)
	  {
	    int end=inputVhdlString.find(qcs.data(),iCodeLen);
	    makeInlineDoc(end);
	  }
	  else if (index > 0 ) 
	  {
	    // assert(false);
	    strComment=qcs;
	    startCodeBlock(index);
	    strComment.resize(0);
	  }

	  //printf("--> handleCommentBlock line %d\n",yyLineNr);
	  if (!isEndCode && index==-1)
          {
            int j=qcs.find("--!");
            qcs=qcs.right(qcs.length()-3-j);

            if(!checkMultiComment(qcs,yyLineNr))
            {
              handleCommentBlock(qcs,TRUE);
            }
          }//endcode
       }

<*>"/*" {
	  strComment+=vhdlScanYYtext;
	  if (yy_hold_char=='!') // found  comment starting with "/*!"
  	  {
	    doxComment=TRUE;
	  }
	  BEGIN(Vhdl2008Comment); 
        }

<Vhdl2008Comment>[^*]*[*]+    {
    	  QCString tt(vhdlScanYYtext);
	  int len=tt.length();
	  yyLineNr+=tt.contains('\n');

	  // delete * from comments 
	  // /*! 
	  // *   see vim !
	  // */

	  if (yytext[len-1]=='*' && tt.contains('\n'))
	  {
	    QCString ss=tt;
	    VhdlDocGen::deleteAllChars(ss,' ');
	    VhdlDocGen::deleteAllChars(ss,'\t');
	    if (ss.data()  && ss.at(ss.length()-2)=='\n')
	    {
	      tt=tt.left(len-1);
	      len--;
	    }
	  }

	  //	   fprintf(stderr,"\n << %s  >>",tt.data());
	  strComment+=tt;
	  char c=yy_hold_char;
	  if (c =='/')
	  {
	    unput('*');
	    BEGIN(EndVhdl2008Comment);
	  }
	  else 
	  {
	    BEGIN(Vhdl2008Comment);
	  }
        }

<EndVhdl2008Comment>"*/" {	 
          if (doxComment)
          {  
            strComment.stripPrefix("/*!");
            strComment= strComment.left(  strComment.length()-1);
            handleCommentBlock( strComment,TRUE);
          }
          doxComment=FALSE;
          strComment.resize(0);
          BEGIN(INITIAL);
        }


%%

static void parserInit()
{
  num_chars=0;
  lineIndex=(int*)malloc(500*sizeof(int));

  if (!g_lexInit) 
  {
    VhdlDocGen::init();
    el_mc oop;
    int p=0;
    while ((oop=tab_mc[p++]).kval!=-1)
    {
      QCString q(&oop.nom[0]);
      keyMap.insert(q,oop.kval);
    }
  }
}

void vhdlscanFreeScanner()
{
#if defined(YY_FLEX_SUBMINOR_VERSION)
  if (g_lexInit)
  {
    vhdlScanYYlex_destroy();
  }
#endif
}

void VHDLLanguageScanner::resetCodeParserState()
{
}

bool VHDLLanguageScanner::needsPreprocessing(const QCString & /*extension*/)
{ 
  return TRUE; 
}

void VHDLLanguageScanner::parsePrototype(const char *text)
{ 
  varName=text;
  varr=TRUE;
}

// do parsing
//int VhdlParser::doLex()
//{
//  int token=vhdlScanYYlex();
//  //fprintf(stderr,"\ntoken: %d at line: %d",token,yyLineNr);
//  return token;
//}

void VHDLLanguageScanner::parseInput(const char *fileName,const char *fileBuf,Entry *root)
{
  yyFileName=QCString(fileName);

  bool xilinx_ucf=isConstraintFile(yyFileName,".ucf");
  bool altera_qsf=isConstraintFile(yyFileName,".qsf");

  // support XILINX(ucf) and ALTERA (qsf) file

  if (xilinx_ucf) 
  { 
    VhdlDocGen::parseUCF(fileBuf,root,yyFileName,FALSE);  
    return; 
  }
  if (altera_qsf) 
  { 
    VhdlDocGen::parseUCF(fileBuf,root,yyFileName,TRUE); 
    return; 
  }

  ::parserInit();
  yycont=getVhdlCont();
  yycont->root=root;
  yycont->fileName=fileName;
  initVhdlParser();
  QCString pPuffer(" ");
  pPuffer+=fileBuf;

  inputFile.setName(fileName);
  if (g_lexInit)
  {
    vhdlScanYYrestart( vhdlScanYYin );
  }
  g_lexInit=TRUE;
  g_thisParser=this;
  inputPosition=0;
  inputVhdlString=fileBuf;
  yyLineNr=1;
  current_root=root;
  groupEnterFile(fileName,yyLineNr);
  vhdlParse();
  Entry* curr=getCurrentVhdlEntry(); // delete last current
  delete curr;
  curr=0;
  free(lineIndex);
  inputFile.close();
  mapLibPackage(root);
}

void VHDLLanguageScanner::parseCode(CodeOutputInterface &codeOutIntf,
    const char *scopeName,
    const QCString &input,
    bool isExampleBlock,
    const char *exampleName,
    FileDef *fileDef,
    int startLine,
    int endLine,
    bool inlineFragment,
    MemberDef *memberDef,
    bool showLineNumbers,
    Definition *searchCtx
    )
{
  ::parseVhdlCode(codeOutIntf,scopeName,input,isExampleBlock,exampleName,
                  fileDef,startLine,endLine,inlineFragment,memberDef,
                  showLineNumbers,searchCtx);
}

/*
 * adds the library|use statements to the next class (entity|package|architecture|package body
 * library ieee
 * entity xxx
 * .....
 * library
 * package
 * enity zzz
 * .....
 * and so on..
 */
static void mapLibPackage(const Entry* ce)
{
  Entry *lastComp=0;
  while (TRUE)
  {
    bool found = FALSE;
    Entry *rt=0;
    //const QList<Entry> *epp=ce->children();
    EntryListIterator eli(*ce->children());
    EntryListIterator eli1=eli;
    for (;(rt=eli.current()),eli1=eli;++eli)
    {
      if (rt->spec==VhdlDocGen::LIBRARY || rt->spec==VhdlDocGen::USE)
        // top level library or use statement
      {
        Entry *temp=0;
        if(!addLibUseClause(rt->name))
        {
          rt->spec=-1;
          rt->section=0;
          //  continue;
        }

        for (;(temp=eli1.current());++eli1) // find next entity
        {
          if (temp->spec==VhdlDocGen::ENTITY || 
              temp->spec==VhdlDocGen::PACKAGE || 
              temp->spec==VhdlDocGen::ARCHITECTURE || 
              temp->spec==VhdlDocGen::PACKAGE_BODY)
          {
            Entry *ee=new Entry(*rt); //append a copy to entries sublist
            temp->addSubEntry(ee);
            found=TRUE;
            rt->spec=-1; //nullify entry
            rt->section=0;
            lastComp=temp;
            break;
          }
        }//for
        if (lastComp && rt->spec!=-1)
        {
          Entry *ee=new Entry(*rt); //append a copy to entries sublist
          lastComp->addSubEntry(ee);
          found=TRUE;
          rt->spec=-1; //nullify entry
          rt->section=0;
        }
      }//if
    }//for
    if (!found) // nothing left to do
    {
      return; 
    }
  }//while
}//MapLib

static bool addLibUseClause(const QCString &type)
{
  static bool show=Config_getBool("SHOW_INCLUDE_FILES");
  static bool showIEEESTD=Config_getBool("FORCE_LOCAL_INCLUDES");

  if (!show)  // all libraries and included packages will not  be shown
  {
    return FALSE;
  }

  if (!showIEEESTD) // all standard packages and libraries will not be shown
  {  
    if (type.lower().stripPrefix("ieee")) return FALSE;
    if (type.lower().stripPrefix("std")) return FALSE;
  }  
  return TRUE;
}

static void handleCommentBlock(const QCString &doc,bool brief)
{
  int position=0;

  // empty comment  --!
  if (doc.isEmpty()) return;

  bool needsEntry=FALSE;
  Protection protection=Public;
  int lineNr = iDocLine;

  Entry* current=getCurrentVhdlEntry();

  if (oldEntry==current)
  {
    //printf("\n find pending message  < %s > at line: %d \n ",doc.data(),iDocLine);
    str_doc.doc=doc;
    str_doc.iDocLine=iDocLine;
    str_doc.brief=brief;
    str_doc.pending=TRUE;
    return;
  }

  oldEntry=current;

  if (brief) 
  {
    current->briefLine = iDocLine; 
  }
  else 
  {
    current->docLine = iDocLine;
  }
  
//  printf("parseCommentBlock file<%s>\n [%s]\n",yyFileName.data(),doc.data());
  while (parseCommentBlock(
	g_thisParser,
	current,
	doc,        // text
	yyFileName, // file
	lineNr,     // line of block start
	brief, 
	0,
	FALSE,
	protection,
        position,
        needsEntry
        )
     ) 
  {
 //printf("parseCommentBlock position=%d [%s]\n",position,doc.data()+position);
    if (needsEntry) newVhdlEntry();
  }
  if (needsEntry)
  {
    if (varr)
    {
      varr=FALSE;
      current->name=varName;
      current->section=Entry::VARIABLEDOC_SEC;
      varName="";
      strComment.resize(0);
    }
   
    newVhdlEntry();
  }
  iDocLine=-1;
  strComment.resize(0);
}

// returns the vhdl parsed types at line xxx
QList<Entry>* getEntryAtLine(const Entry* ce,int line)
{
  EntryListIterator eli(*ce->children());
  Entry *rt;
  for (;(rt=eli.current());++eli)
  {
    if (rt->bodyLine==line)
    {
      lineEntry.insert(0,rt);
    } // if

    getEntryAtLine(rt,line);
  }
  return &lineEntry;
}

// token index in vhdlparser.hpp 258..416
int getParsedLine(int object)
{
  //assert(object>254 && object <416);
  return lineIndex [object];
}

void isVhdlDocPending()
{
  if (!str_doc.pending)
    return;
  str_doc.pending=FALSE;
  oldEntry=0; // prevents endless recursion
  iDocLine=str_doc.iDocLine;
  handleCommentBlock(str_doc.doc,str_doc.brief);
  iDocLine=-1;
}

static bool  checkMultiComment(QCString& qcs,int line)
{
  QList<Entry> *pTemp=getEntryAtLine(current_root,line);

  if (pTemp->isEmpty()) return false;

  //int ii=pTemp->count();
  qcs.stripPrefix("--!");
  while (!pTemp->isEmpty())
  {
    Entry *e=(Entry*)pTemp->first();
    e->briefLine=line;
    e->brief+=qcs;
    iDocLine=-1;
    pTemp->removeFirst();
    //ii=pTemp->count();
  }
  return true; 
}
 
 
