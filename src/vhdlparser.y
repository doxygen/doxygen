
/*********************************************************************************************
 * The original was a VHDL parser description to be used with GMD Compiler
 * Tool Box
 * written from:
 * Jan den Ouden, Jaap Hofstede
 * Department of Computer Science (INF/SPA)
 * University of Twente
 * PB 217, 7500 AE ENSCHEDE, The Netherlands
 * Tel: +31 53 893788
 * Email: hofstede@cs.utwente.nl
 * avail at: ftp.cs.utwente.nl in pub/src/VHDL/Grammar
 *
 * author of YACC transformation:
 * Thomas Dettmer
 * Dortmund University
 * Dept. of Computer Scienc, LS1
 * PB 500 500
 * D-44221 Dortmund (Germany)
 * Phone: +49-231-755-6464
 * e-mail: dettmer@ls1.informatik.uni-dortmund.de
 *****************************************************************
 *
 * This file is intended not to be used for commercial purposes
 * without permission of the University of Twente and permission
 * of the University of Dortmund
 *
 * NOTE THAT THERE IS NO WARRANTY FOR CORRECTNES, COMPLETENESS, SUPPORT
 * OR ANYTHING ELSE.
 *
 *******************************************************/
/******************************************************************************
 * modified for doxygen by M. Kreis
 * extended to VHDL 93/2002/2008
 ******************************************************************************/


%{
#include <stdio.h>
#include <qcstring.h>
#include <qstringlist.h>

#ifndef YYSTYPE
typedef int YYSTYPE;
#endif

struct  YYMM
{
  int itype;
  QCString qstr;
};

// define struct instead of union
#define YYSTYPE YYMM

#include "membergroup.h"
#include "vhdldocgen.h"
#include "doxygen.h"
#include "searchindex.h"
#include "vhdlscanner.h"
#include "commentscan.h"
#include "entry.h"
#include "arguments.h"

//-----------------------------variables ---------------------------------------------------------------------------
//static VhdlParser* myconv=0;

static VhdlContainer s_str;
static QList<Entry>instFiles;
static int yyLineNr;
static Entry* lastCompound;
static Entry* currentCompound;
static Entry* lastEntity;
static Entry* current;
static Entry* tempEntry;
static Entry* current_root;
static QCString compSpec;
static QCString currName;
static int levelCounter;
static QCString confName;
static QCString genLabels;

static QList<VhdlConfNode> configL;
static VhdlConfNode* currNode;

static int currP=0;

enum  { GEN_SEC=0x1, PARAM_SEC,CONTEXT_SEC,PROTECTED_SEC } ;

static int param_sec = 0;
static int parse_sec=0;


//---------------------------- function --------------------------------------------------------------------------------

int vhdlScanYYlex ();
void vhdlScanYYerror (char const *);

static void addVhdlType(const QCString &name,int startLine,
                        int section,int spec,
			const char* args,const char* type,
			Protection prot=Public);
static void addCompInst(char *n, char* instName,char* comp,int line);

static void newEntry();
static void initEntry(Entry *e);
static bool isFuncProcProced();
static void popConfig();
static void pushLabel(const QCString &label);
static void popLabel();
static void addConfigureNode(const char* a,const char*b,
                         bool isRoot,bool isLeave,bool inlineConf=FALSE);
//static bool addLibUseClause(const QCString &type);
static bool isFuncProcProced();
static void initEntry(Entry *e);
static void addProto(const char *s1,const char *s2,const char *s3,
                     const char *s4,const char *s5,const char *s6);
static void createFunction(const QCString &impure,int spec,
                           const QCString &fname);

void newVhdlEntry()
{
  newEntry();
}

Entry* getCurrentVhdlEntry()
{
  return current;
}

void initVhdlParser()
{
  lastCompound=0;
  lastEntity=0;
  currentCompound=0;
  lastEntity=0;
  current_root=s_str.root;
  current=new Entry();
  initEntry(current);
}

QList<Entry> & getVhdlInstList()
{
  return instFiles;
}


%}

%token
t_ABSTRLIST
t_CHARLIST
t_DIGIT
t_STRING
t_LETTER
t_ACCESS
t_AFTER
t_ALIAS
t_ALL
t_AND
t_ARCHITECTURE
t_ARRAY
t_ASSERT
t_ATTRIBUTE
t_BEGIN
t_BLOCK
t_BODY
t_BUFFER
t_BUS
t_CASE
t_COMPONENT
t_CONFIGURATION
t_CONSTANT
t_DISCONNECT
t_DOWNTO
t_ELSE
t_ELSIF
t_END
t_ENTITY
t_EXIT
t_FILE
t_FOR
t_FUNCTION
t_GENERATE
t_GENERIC
t_GUARDED
t_IF
t_IN
t_INOUT
t_IS
t_LABEL
t_LIBRARY
t_LINKAGE
t_LOOP
t_MAP
t_NAND
t_NEW
t_NEXT
t_NOR
t_NULL
t_OF
t_ON
t_OPEN
t_OR
t_OTHERS
t_OUT
t_PACKAGE
t_PORT
t_PROCEDURE
t_PROCESS
t_RANGE
t_RECORD
t_REGISTER
t_REPORT
t_RETURN
t_SELECT
t_SEVERITY
t_SIGNAL
t_SUBTYPE
t_THEN
t_TO
t_TRANSPORT
t_TYPE
t_UNITS
t_UNTIL
t_USE
t_VARIABLE
t_WAIT
t_WHEN
t_WHILE
t_WITH
t_XOR

/* new keywords */
t_IMPURE
t_PURE
t_GROUP
t_POSTPONED
t_SHARED
t_XNOR
t_SLL
t_SRA
t_SLA
t_SRL
t_ROR
t_ROL
t_UNAFFECTED
/*============== VHDL 2008 keywords   ======================= */

t_ASSUME_GUARANTEE
t_ASSUME
t_CONTEXT
t_COVER
t_DEFAULT
t_FAIRNESS
t_FORCE
t_INERTIAL
t_LITERAL
t_PARAMETER
t_PROTECTED
t_PROPERTY
t_REJECT
t_RELEASE
t_RESTRICT
t_RESTRICT_GUARANTEE
t_SEQUENCE
t_STRONG
t_VMODE
t_VPROP
t_VUNIT


/*============== VHDL binary operators 2008   ======================= */

t_SLSL
t_SRSR
t_QQ
t_QGT
t_QLT
t_QG
t_QL
t_QEQU
t_QNEQU

%nonassoc t_EQSym t_NESym t_LTSym t_LESym t_GTSym t_GESym t_QNEQU t_QEQU t_QL t_QG t_QLT  t_QGT
%left t_Plus t_Minus t_Ampersand
%left MED_PRECEDENCE
%left t_Star t_Slash t_MOD t_REM
%nonassoc t_DoubleStar t_ABS t_NOT MAX_PRECEDENCE

/* misc syms */
/*t_Space */

%token
t_Apostrophe
t_LeftParen
t_RightParen
t_Comma
t_VarAsgn
t_Colon
t_Semicolon
t_Arrow
t_Box
t_Bar
t_Dot
t_Q
t_At
t_Neg
t_LEFTBR
t_RIGHTBR
t_ToolDir

%type<qstr> designator literal enumeration_literal physical_literal physical_literal_no_default physical_literal_1
%type<qstr> lib_clause use_clause sel_list entity_decl entity_start entity_decl_2 entity_decl_1 arch_body arch_start arch_body_1
%type<qstr> config_decl config_start config_decl_2 config_decl_1 config_decl_3 package_decl package_start package_body pack_body_start package_body_2 package_body_1 common_decltve_item
%type<qstr> config_decltve_item subprog_decl subprog_body interf_list interf_element interf_element_4 interf_element_3 interf_element_2 interf_element_1 mode
%type<qstr> association_list association_list_1 association_list_2 gen_association_list gen_association_list_1 gen_association_list_2 association_element gen_association_element formal_part actual_part mark expr and_relation relation primary name name2 sel_name suffix ifts_name
%type<qstr> attribute_name aggregate element_association_list2 qualified_expr element_association choices choices_1 choices_2 choice type_decl type_decl_1 type_definition enumeration_type_definition enumeration_type_definition_1 enumeration_type_definition_2 physical_type_definition physical_type_definition_1 physical_type_definition_2 base_unit_decl secondary_unit_decl unconstrained_array_definition unconstrained_array_definition_1 unconstrained_array_definition_2 index_subtype_definition constrained_array_definition record_type_definition record_type_definition_1 record_type_definition_2 element_decl
%type<qstr> access_type_definition file_type_definition subtype_decl subtype_indic subtype_indic_1 subtype_indic1 subtype_indic1_1 range_constraint index_constraint index_constraint_1 index_constraint_2 discrete_range discrete_range1 range_spec direction constant_decl constant_decl_1 signal_decl signal_decl_2 signal_decl_1 variable_decl variable_decl_1 object_class signal_kind alias_decl file_decl file_decl_1 attribute_decl attribute_spec entity_spec entity_name_list entity_name_list_1
%type<qstr> entity_name_list_2 entity_class block_stat
%type<qstr> generate_stat generate_stat_1 procs_stat procs_stat1 procs_stat1_1 sensitivity_list sensitivity_list_1 sensitivity_list_2
%type<qstr> procedure_call_stat comp_decl comp_decl_2 comp_decl_1 block_config block_config_2 block_config_3 block_config_1 block_config_4 block_spec config_item comp_config comp_config_2 comp_config_1 config_spec binding_indic comp_spec
%type<qstr> inst_list entity_aspect idf_list procs_stat1_6
%type<qstr> t_Identifier t_StringLit t_BitStringLit t_AbstractLit t_CharacterLit tbox group_name record_type_simple_name
%type<qstr> entity_class_entry_list entity_class_entry group_constituent_list group_constituent group_declaration group_template_declaration
%type<qstr> procs_stat1_5 comp_1 mark_comp dot_name fi_dec multiplying_operator factor term adding_op
%type<qstr> simple_exp alias_spec sigma signature1 mark_stats mark_stats_1 signature
%type<qstr> protected_type_body protected_type_declaration alias_name_stat vcomp_stat comp_spec_stat
%type<qstr> sig_stat external_name absolute_pathname relative_pathname package_path_name external_pathname pathname_element_list neg_list pathname_element

%type<qstr> func_name return_is param func_prec iproc ifunc interface_subprogram_decl interface_package_decl package_instantiation_decl
%type<qstr> subprogram_instantiation_decl
%type<qstr>  context_ref  libustcont_stats   libustcont_stat  context_decl


%debug

// for debugging set yydebug=1
%initial-action { yydebug=0; }

%expect 2

// minimum bison version
//%require "2.2"

%%
start: design_file


design_file     : design_unit_list

design_unit_list: design_unit
                | design_unit_list design_unit
                ;

designator      : t_Identifier                      { $$=$1; }
                | t_StringLit                       { $$=$1; }
                ;

literal         : t_AbstractLit                     { $$=$1; }
                | t_CharacterLit                    { $$=$1; }
                | t_BitStringLit                    { $$=$1; }
                | physical_literal_no_default       { $$=$1; }
                | t_NULL                            { $$="null"; }
                ;

enumeration_literal  : t_CharacterLit               { $$=$1; }
                     | t_Identifier                 { $$=$1; }

physical_literal     : physical_literal_1 t_Identifier  { $$=$1+" "+$2; }

physical_literal_1   : /* empty */        { $$=""; }
                     | t_AbstractLit      { $$=$1; }

physical_literal_no_default : t_AbstractLit t_Identifier   { $$=$1+" "+$2; }

idf_list : t_Identifier { $$=$1; }
         | idf_list t_Comma t_Identifier { $$=$1+","+$3}
         ;

/*------------------------------------------
--  Desing Unit
--------------------------------------------*/

design_unit      : context_list lib_unit

context_list     :  /* empty */
                 | context_list context_item

lib_unit         : entity_decl
                 | config_decl
                 | package_decl
                 | arch_body
                 | package_body
                 | context_decl
                 | package_instantiation_decl
                 ;

context_item     : lib_clause
                 | use_clause
                 ;

lib_clause       : t_LIBRARY idf_list t_Semicolon
                 {
                   if ( parse_sec == 0)
                   {
                    addVhdlType($2,getParsedLine(t_LIBRARY),Entry::VARIABLE_SEC,VhdlDocGen::LIBRARY,$2.data(),"_library_");
                   }
                   $$="library "+$2;
                 }

use_clause : t_USE sel_list t_Semicolon
                 {
                   QStringList ql1=QStringList::split(",",$2,FALSE);
                   for (uint j=0;j<ql1.count();j++)
                   {
                     QStringList ql=QStringList::split(".",ql1[j],FALSE);
                     QCString it=ql[1].utf8();
                     if ( parse_sec == 0 )
                     {
                       addVhdlType(it,getParsedLine(t_USE),Entry::VARIABLE_SEC,VhdlDocGen::USE,it.data(),"_use_");
                     }
                   }
                   $$="use "+$2;
                 }

sel_list    : sel_name { $$=$1; }
            | sel_list t_Comma sel_name { $$=$1+","+$3; }
            ;
/*------------------------------------------
--  Library Units
--------------------------------------------*/

entity_decl : entity_start error comp_end_dec  t_Semicolon

entity_decl : entity_start entity_decl_1 entity_decl_2
                     entity_decl_3 entity_decl_4   comp_end_dec  t_Semicolon

entity_start: t_ENTITY t_Identifier t_IS
              {
                $$=$2;
                lastEntity=current;
                lastCompound=0;
                /*int k=*/ getParsedLine(t_ENTITY);
                addVhdlType($$,getParsedLine(t_ENTITY),Entry::CLASS_SEC,VhdlDocGen::ENTITY,0,0,Public);
                //fprintf(stderr,"\n entiy %s : at line %d",$$.data(),s_str.yyLineNr);
              }
              ;

entity_decl_5 :   /* empty */
              |  t_Identifier
              ;
entity_decl_4 :  /* empty */
              | t_BEGIN concurrent_stats
              ;
entity_decl_3 :  /* empty */
              | entity_decl_3 entity_decl_6
              ;

entity_decl_6 : entity_decltve_item

entity_decl_2 : /* empty */  { $$=""; }
              | t_PORT { currP=VhdlDocGen::PORT; }  interf_list t_Semicolon  { currP=0; }
              ;
entity_decl_1 :  /* empty */  { $$=""; }
              | t_GENERIC { currP=VhdlDocGen::GENERIC;parse_sec=GEN_SEC} interf_list t_Semicolon{ currP=0;parse_sec=0; }
              | t_GENERIC error t_Semicolon{ currP=0; }
              ;


arch_body     : arch_start arch_body_1 t_BEGIN concurrent_stats t_END arch_body_2 t_Semicolon
arch_body     : arch_start error t_END arch_body_2 t_Semicolon

arch_start    : t_ARCHITECTURE t_Identifier t_OF t_Identifier t_IS
                {
                  $$=$4;
                  $$+="::";
                  $$+=$2;
                  pushLabel($2);
                  lastCompound=current;
                  addVhdlType($$,getParsedLine(t_ARCHITECTURE),Entry::CLASS_SEC,VhdlDocGen::ARCHITECTURE,0,0,Private);
                }
arch_body_2   : /* empty */
arch_body_2   : t_Identifier
arch_body_2   : t_ARCHITECTURE  t_Identifier
arch_body_2   : t_ARCHITECTURE

arch_body_1   : /* empty */  { $$=""; }
arch_body_1   : arch_body_1 arch_body_3

arch_body_3   :  block_decltve_item

config_decl   : config_start error  t_END config_decl_2 t_Semicolon { genLabels.resize(0); }
config_decl   : config_start config_decl_1 block_config t_END config_decl_2 t_Semicolon { genLabels.resize(0); }
                {
                  confName="";
                }


config_start  : t_CONFIGURATION t_Identifier t_OF t_Identifier t_IS
                {
                  confName=$2+"::"+$4;
                  addVhdlType($2.data(),getParsedLine(t_CONFIGURATION),Entry::VARIABLE_SEC,VhdlDocGen::CONFIG,"configuration",$4.data());
                }

config_decl_2 : /* empty */     { $$=""; }
config_decl_2 : t_Identifier    { $$=$1; }
config_decl_2 : t_CONFIGURATION { $$="configuration"; }
config_decl_2 : t_CONFIGURATION t_Identifier  { $$=$2; }
config_decl_1 :  /* empty */    { $$=""; }
config_decl_1 : config_decl_1 config_decl_3   { $$=$1+" "+$2; }
config_decl_3 : config_decltve_item           { $$=$1; }

package_decl  : package_start error t_END package_decl_2 t_Semicolon
package_decl  : package_start package_decl_1 t_END package_decl_2 t_Semicolon
package_start : t_PACKAGE t_Identifier t_IS
                           {
                          lastCompound=current;


         Entry *clone=new Entry(*current);
         clone->section=Entry::NAMESPACE_SEC;
         clone->spec=VhdlDocGen::PACKAGE;
           clone->name=$2;
           int line=s_str.iLine;
         clone->startLine=line;
           clone->bodyLine=line;

         clone->protection=Package;
             current_root->addSubEntry(clone);


                        addVhdlType($2,line,Entry::CLASS_SEC,VhdlDocGen::PACKAGE,0,0,Package);
                        //fprintf(stderr,"\n entiy %s : at line %d",$$.data(),s_str.yyLineNr);
                       }

package_decl_2     :  /* empty */
package_decl_2     : t_Identifier  { lastCompound=0; }
package_decl_2     : t_PACKAGE t_Identifier { lastCompound=0; }
package_decl_2     : t_PACKAGE { lastCompound=0; }

package_decl_1     :  /* empty */
package_decl_1    : package_decl_22
package_decl_1     : package_decl_1 package_decl_3
package_decl_3     : package_decltve_item

package_decl_22: gen_interface_list t_Semicolon
package_decl_22: gen_interface_list
package_decl_22: gen_interface_list gen_assoc_list
package_decl_22: gen_interface_list gen_assoc_list t_Semicolon

package_body    : pack_body_start error t_END package_body_2 t_Semicolon
package_body    : pack_body_start package_body_1 t_END package_body_2 t_Semicolon
pack_body_start : t_PACKAGE t_BODY t_Identifier t_IS
                      {
                        $$=$3;
                        lastCompound=current;
                        $$.prepend("_");
                        addVhdlType($$,getParsedLine(t_PACKAGE) ,Entry::CLASS_SEC,VhdlDocGen::PACKAGE_BODY,0,0,Protected);
                      }
package_body_2  :  /* empty */  { $$="";lastCompound=0; }
package_body_2  : t_Identifier                                            { lastCompound=0; }
package_body_2  : t_PACKAGE t_BODY                         { lastCompound=0; }
package_body_2  : t_PACKAGE t_BODY t_Identifier      { lastCompound=0; }


package_body_1  :  /* empty */  { $$=""; }
package_body_1  : package_body_1 package_body_3
package_body_3  : package_body_decltve_item



/*------------------------------------------
--  Declarative Item
--------------------------------------------*/

common_decltve_item_1: package_decl
common_decltve_item_1: package_instantiation_decl
common_decltve_item_1: package_body
common_decltve_item_1: subprogram_instantiation_decl


common_decltve_item: type_decl
common_decltve_item: subtype_decl
common_decltve_item: constant_decl
common_decltve_item: file_decl
common_decltve_item: alias_decl
common_decltve_item: subprog_decl
common_decltve_item: use_clause

entity_decltve_item: common_decltve_item
entity_decltve_item: subprog_body
entity_decltve_item: attribute_decl
entity_decltve_item: attribute_spec
entity_decltve_item: disconnection_spec
entity_decltve_item: signal_decl
entity_decltve_item: variable_decl
entity_decltve_item: group_template_declaration
entity_decltve_item: group_declaration
entity_decltve_item: common_decltve_item_1


block_decltve_item: common_decltve_item
block_decltve_item: subprog_body
block_decltve_item: comp_decl
block_decltve_item: attribute_decl
block_decltve_item: attribute_spec
block_decltve_item: config_spec
block_decltve_item: disconnection_spec
block_decltve_item: signal_decl
block_decltve_item: variable_decl
block_decltve_item: group_template_declaration
block_decltve_item: group_declaration
block_decltve_item: common_decltve_item_1
block_decltve_item: tool_directive

block_declarative_part: block_decltve_item
                      | block_declarative_part  block_decltve_item


package_decltve_item: common_decltve_item
package_decltve_item: comp_decl
package_decltve_item: attribute_decl
package_decltve_item: attribute_spec
package_decltve_item: disconnection_spec
package_decltve_item: signal_decl
package_decltve_item: variable_decl
package_decltve_item: group_template_declaration
package_decltve_item: group_declaration
package_decltve_item: package_decl
package_decltve_item: package_instantiation_decl
package_decltve_item: subprogram_instantiation_decl

package_body_decltve_item: common_decltve_item
package_body_decltve_item: subprog_body
package_body_decltve_item: variable_decl
package_body_decltve_item: group_template_declaration
package_body_decltve_item: group_declaration
package_body_decltve_item: attribute_decl
package_body_decltve_item: attribute_spec
package_body_decltve_item: common_decltve_item_1

subprog_decltve_item: common_decltve_item
subprog_decltve_item: subprog_body
subprog_decltve_item: attribute_decl
subprog_decltve_item: attribute_spec
subprog_decltve_item: variable_decl
subprog_decltve_item: group_template_declaration
subprog_decltve_item: group_declaration
subprog_decltve_item: common_decltve_item_1

procs_decltve_item: common_decltve_item
procs_decltve_item: subprog_body
procs_decltve_item: attribute_decl
procs_decltve_item: attribute_spec
procs_decltve_item: variable_decl
procs_decltve_item: group_template_declaration
procs_decltve_item: group_declaration
procs_decltve_item: common_decltve_item_1

config_decltve_item: attribute_spec    { $$=$1; }
config_decltve_item: use_clause        { $$=$1; }
config_decltve_item: group_declaration { $$=$1; }
config_decltve_item: t_USE t_VUNIT idf_list t_Semicolon { $$=$3; }

/*------------------------------------------
--  Subprograms
--------------------------------------------*/
func_prec: t_PURE { $$="pure"; }
func_prec: t_IMPURE { $$="impure"; }

subprog_decl: subprog_spec t_Semicolon { currP=0; }

subprog_spec: t_PROCEDURE t_Identifier
              { currP=VhdlDocGen::PROCEDURE; createFunction($2,currP,0); }
              subprog_spec_1 {  newEntry(); }
subprog_spec: func_prec t_FUNCTION designator
              {
                currP=VhdlDocGen::FUNCTION;
                createFunction($1,currP,$3.data());
              }
              subprog_spec_2 t_RETURN mark
              {
                tempEntry=current;
                current->type=$7;
                newEntry();
              }

subprog_spec  :  t_FUNCTION designator
              {
                currP=VhdlDocGen::FUNCTION;
                createFunction(0,currP,$2.data());
              }
              subprog_spec_2 t_RETURN mark
              {
                tempEntry=current;
                current->type=$6;
                newEntry();
              }

subprog_spec_22: gen_interface_list
subprog_spec_22: gen_interface_list gen_assoc_list

subprog_spec_33: t_PARAMETER
                 { param_sec=PARAM_SEC; }
                 interf_list
                 { param_sec= 0; }
subprog_spec_33: { param_sec=PARAM_SEC; }
                 interf_list
                 { param_sec= 0; }

subprog_spec_2:  /* empty */
subprog_spec_2: subprog_spec_22 subprog_spec_33
subprog_spec_2: subprog_spec_33
subprog_spec_2: subprog_spec_22

subprog_spec_1: subprog_spec_2

subprog_body: subprog_spec t_IS  subprog_body_1 t_BEGIN   seq_stats t_END subprog_body_2 t_Semicolon
    {
      currP=0;
    }
subprog_body: subprog_spec t_IS error  t_END subprog_body_2 t_Semicolon
    {
      currP=0;
    }
subprog_body_2:  /* empty */
subprog_body_2: designator
subprog_body_2: t_FUNCTION
subprog_body_2: t_PROCEDURE
subprog_body_2: t_PROCEDURE t_Identifier
subprog_body_2: t_FUNCTION t_Identifier
subprog_body_2: t_FUNCTION t_STRING


subprog_body_1:  /* empty */
//subprog_body_1    :  subprogram_instantiation_decl
subprog_body_1: subprog_body_1 subprog_body_3
subprog_body_3: subprog_decltve_item

/*--------------------------------------------------
--  Interface Lists and Associaton Lists
----------------------------------------------------*/

interf_list:   t_LeftParen interf_element interf_list_1 t_RightParen   { $$=""; }
interf_list:   t_LeftParen error t_RightParen   { $$=""; }
interf_list_1:  /* empty */
interf_list_1: interf_list_1 interf_list_2
interf_list_2: t_Semicolon interf_element

interf_element: interface_package_decl
                  {
                    // adding generic :  [ package foo  is new bar]
                    if (parse_sec==GEN_SEC)
                    {
                      QCString n=current->name;
                      addVhdlType(n.data(),getParsedLine(t_PACKAGE),Entry::VARIABLE_SEC,VhdlDocGen::GENERIC,$1.data(),0);
                    }
                  }
interf_element: interface_subprogram_decl
                  {
                    if (parse_sec==GEN_SEC)
                    {
                      int a=getParsedLine(t_FUNCTION);
                      int b=getParsedLine(t_PROCEDURE);

                      if (a>b) b=a;

                      QCString n=current->name;
                      addVhdlType(n.data(),b,Entry::VARIABLE_SEC,VhdlDocGen::GENERIC,$1.data(),0);
                    }
                  }
interf_element: interf_element_1 t_Identifier
                  {
                    if (parse_sec==GEN_SEC)
                    {
                      QCString v= $1;
                      addVhdlType($2,s_str.iLine,Entry::VARIABLE_SEC,currP,$1.data(),0);
                    }
                  }
interf_element: interf_element_1 idf_list t_Colon interf_element_2 subtype_indic interf_element_3 interf_element_4
                  {
                    QCString b=$2;
                    $$=$2+":"+$4+$5+$6+$7;
                    if (currP!=VhdlDocGen::COMPONENT)
                    {
                      if (currP==VhdlDocGen::FUNCTION || currP==VhdlDocGen::PROCEDURE)
                      {
                        addProto($1.data(),$2.data(),$4.data(),$5.data(),$6.data(),$7.data());
                      }
                      else
                      {
                        QCString i=$5+$6+$7;
                        if (currP==VhdlDocGen::GENERIC)
                          addVhdlType($2,s_str.iLine,Entry::VARIABLE_SEC,currP,i.data(),$4.data());
                        else if(parse_sec != GEN_SEC)
                          addVhdlType($2,s_str.iLine,Entry::VARIABLE_SEC,currP,i.data(),$4.data());
                      }
                      //   fprintf(stderr,"\n\n <<port  %s  >>\n",$$.data());
                    } // if component
                  }
interf_element_4: /* empty :=*/   { $$=""; }
interf_element_4: t_VarAsgn expr  { $$=":="+$2; }
interf_element_3: /* empty */     { $$=""; }
interf_element_3: t_BUFFER        { $$="buffer"; }
interf_element_3: t_BUS           { $$="bus"; }
interf_element_2: /* empty */     { $$=""; }
interf_element_2: mode            { $$=$1; }
interf_element_1: /* empty */     { $$=""; }
interf_element_1: object_class    { $$=$1; }

mode: t_IN                        { $$="in"; }
mode: t_OUT                       { $$="out"; }
mode: t_INOUT                     { $$="inout"; }
mode: t_BUFFER                    { $$="buffer"; }
mode: t_LINKAGE                   { $$="link"; }

association_list:   t_LeftParen association_element association_list_1 t_RightParen  { $$="("+$2+")"; }
association_list_1:  /* empty */                           { $$=""; }
association_list_1: association_list_1 association_list_2  { $$=$1+" "+$2; }
association_list_2: t_Comma association_element            { $$=", "+$2; }

gen_association_list : t_LeftParen gen_association_element gen_association_list_1 t_RightParen
    {
      QCString str="( "+$2;
      str.append(" )");
      $$=str;
    }
gen_association_list: t_LeftParen  error t_RightParen { $$=""; }
gen_association_list: t_LeftParen t_OPEN t_RightParen { $$=" ( open ) "; }

gen_association_list_1:  /* empty */  { $$=""; }
gen_association_list_1: gen_association_list_1   gen_association_list_2  { $$=$1+"?? "+$2; }
gen_association_list_2: t_Comma gen_association_element { $$=","+$2; }

association_element: formal_part t_Arrow actual_part { $$=$1+"=>"+$3; }
association_element: actual_part { $$=$1; }
association_element: t_Box       { $$="<>"; }
association_element: t_DEFAULT   { $$="default"; }

/* changed ;gen_association_element   : association_element  */
gen_association_element: expr            { $$=$1; }
gen_association_element: choice t_Arrow expr
gen_association_element: discrete_range1 {  $$=$1 ; }

formal_part: name  { $$=$1; }

actual_part: expr      { $$=$1; }
actual_part: t_OPEN    { $$="open"; }
actual_part: t_INERTIAL expr    { $$="inertial"; }

/*--------------------------------------------------
--  Names and Expressions
----------------------------------------------------*/



expr: and_relation  { $$=$1; }
expr: relation  { $$=$1; }

shift_op: t_SLL
        | t_SRA
        | t_SLA
        | t_SRL
        | t_ROR
        | t_ROL
        ;
and_relation: relation shift_op relation
and_relation: relation t_AND    relation
and_relation: relation t_XOR    relation
and_relation: relation t_OR     relation
and_relation: relation t_NOR    relation
and_relation: relation t_XNOR   relation
and_relation: relation t_NAND   relation
and_relation: and_relation t_NAND relation
and_relation: and_relation t_NOR  relation
and_relation: and_relation t_XNOR relation
and_relation: and_relation t_AND  relation
and_relation: and_relation t_OR   relation
and_relation: and_relation t_XOR  relation

/* ;relation   : unary_operator primary   */

relation: t_QQ primary                         { $$=" ?? "+$2; }
relation: primary                              { $$=$1; }
relation: t_Plus primary  %prec MED_PRECEDENCE { $$="+"+$2; }
relation: t_Minus primary %prec MED_PRECEDENCE { $$="-"+$2; }
relation: t_ABS primary                        { $$="abs"+$2; }
relation: t_NOT primary                        { $$="not "+$2; }
relation: primary t_DoubleStar primary         { $$=$1+" ** "+$3; }
relation: t_Minus primary t_DoubleStar primary { $$=$2+" ** "+$4; }

/* ;    relation : relation binary_operator primary */

relation: relation t_MOD   relation       { $$=$1+" mod  "+$3; }
relation: relation t_REM   relation       { $$=$1+" rem "+$3;  }
relation: relation t_Ampersand relation   { $$=$1+" & "+$3;    }
relation: relation t_Star  relation       { $$=$1+" * "+$3;    }
relation: relation t_Plus  relation       { $$=$1+" + "+$3;    }
relation: relation t_Minus relation       { $$=$1+" -  "+$3;   }
relation: relation t_LESym relation       { $$=$1+" <= "+$3;   }
relation: relation t_GESym relation       { $$=$1+" >= "+$3;   }
relation: relation t_LTSym relation       { $$=$1+" <  "+$3;   }
relation: relation t_GTSym relation       { $$=$1+" >  "+$3;   }
relation: relation t_EQSym relation       { $$=$1+" ==  "+$3;  }
relation: relation t_NESym relation       { $$=$1+" != "+$3;   }
relation: relation t_Slash relation       { $$=$1+" /"+$3;     }
relation: relation t_QNEQU relation       { $$=$1+" ?/="+$3;   }
relation: relation t_QEQU  relation       { $$=$1+" ?="+$3;    }
relation: relation t_QL    relation       { $$=$1+" ?<"+$3;    }
relation: relation t_QG    relation       { $$=$1+" ?>"+$3;    }
relation: relation t_QLT   relation       { $$=$1+" ?<="+$3;   }
relation: relation t_QGT   relation       { $$=$1+" ?>="+$3;   }

simple_exp: t_Minus  term  { $$ = "-"+$2; }
          | t_Plus term    { $$ = "+"+$2; }
          | term           { $$ = $1; }
          | simple_exp
            adding_op term { $$ = $1+" "+$2+" "+$3; }

adding_op: t_Ampersand { $$ = "&"; }
         | t_Minus     { $$ = "-"; }
         | t_Plus      { $$ = "+"; }
         ;

term: factor { $$=$1; }
    | factor multiplying_operator factor { $$ = $1+" "+$2+" "+$3; }
    ;

multiplying_operator: t_Star  { $$ = "*";   }
                    | t_REM   { $$ = "rem"; }
                    | t_MOD   { $$ = "mod"; }
                    | t_Slash { $$ = "/";   }

factor: primary                        { $$=$1; }
      | t_ABS  primary                 { $$="abs "+$2; }
      | t_NOT  primary                 { $$="not  "+$2; }
      |  primary t_DoubleStar  primary { $$ = $1+" ** "+$3; }

primary: name                          { $$=$1; }
primary: literal                       { $$=$1; }
primary: aggregate                     { $$=$1; }
primary: qualified_expr                { $$=$1; }
primary: allocator                     { $$=""; }
primary: t_LeftParen expr t_RightParen { $$="("+$2+")"; }

name:  mark           { $$=$1; }
name:  name2          { $$=$1; }
name:  external_name  { $$=$1; }
name2: t_StringLit    { $$=$1; }
name2: attribute_name { $$=$1; }
name2: ifts_name      { $$=$1; }

mark: t_Identifier    { $$=$1; }
mark: sel_name        { $$=$1; }

sel_name: name t_Dot suffix   { $$=$1+"."+$3; }

suffix: designator     { $$=$1; }
suffix: t_CharacterLit { $$=$1; }
suffix: t_ALL          { $$="all"; }

ifts_name: mark  gen_association_list { $$=$1+" "+$2; }
ifts_name: name2 gen_association_list { $$=$1+" "+$2; }

sigma:  t_Apostrophe { $$="'"; }
//sigma :  t_LEFTBR signature1 t_RIGHTBR  t_Apostrophe { $$="("+$2;;$$.append(")");$$.append("'"); }

attribute_name: mark  sigma t_Identifier   { $$=$1+"' "+$3; }
attribute_name: attribute_name t_LeftParen expr t_RightParen
attribute_name: name2 sigma t_Identifier   { $$=$1+" '"+$3; }
attribute_name: mark  sigma t_RANGE        { $$=$1+"' range "; }
attribute_name: name2 sigma t_RANGE        { $$=$1+"' range "; }

aggregate  : element_association_list2 t_RightParen  { $$=$1+" ) "; }
aggregate  : t_LeftParen choices t_Arrow expr t_RightParen  { $$="( "+$2+ "=>"+$4+" ) "; }

element_association_list2 : t_LeftParen element_association t_Comma element_association  { $$=" ( "+$2+","+$4; }
element_association_list2 : element_association_list2 t_Comma element_association   { $$=$1+","+$3; }

qualified_expr : mark t_Apostrophe t_LeftParen expr  t_RightParen { $$=$1+"'("+$4+" ) "; }
qualified_expr : mark t_Apostrophe aggregate  { $$=$1+"'"+$3; }

allocator: t_NEW mark mark allocator_1
allocator: t_NEW mark allocator_2
allocator: t_NEW qualified_expr
allocator_2:  /* empty */
allocator_2: gen_association_list
allocator_1:  /* empty */
allocator_1: gen_association_list


/*--------------------------------------------------
--  Element Association and Choices
----------------------------------------------------*/

element_association: choices t_Arrow expr { $$=$1+"=> "+$3; }
element_association: expr                 { $$=$1; }

choices: choice choices_1      { $$=$1+" "+$2; }
choices_1:  /* empty */        { $$="";        }
choices_1: choices_1 choices_2 { $$=$1+" "+$2; }
choices_2: t_Bar choice        { $$=" | "+$2;  }

choice: expr                   { $$=$1; }
choice: discrete_range1        { $$=$1; }
choice: t_OTHERS               { $$="others"; }

/*--------------------------------------------------
--	Type Declarations
----------------------------------------------------*/
type_decl: t_TYPE t_Identifier error t_Semicolon  { $$=""; }
type_decl: t_TYPE t_Identifier type_decl_1 t_Semicolon
           {
             addVhdlType($2,getParsedLine(t_TYPE),Entry::VARIABLE_SEC,VhdlDocGen::TYPE,0,$3.data());
           }
type_decl: t_TYPE error t_Semicolon { $$=""; }

type_decl_1:  /* empty */           { $$=""; }
type_decl_1: t_IS type_definition   { $$="is "+$2; }

type_definition: enumeration_type_definition    { $$=$1; }
type_definition: range_constraint               { $$=$1; }
type_definition: physical_type_definition       { $$=$1; }
type_definition: unconstrained_array_definition { $$=$1; }
type_definition: constrained_array_definition   { $$=$1; }
type_definition: record_type_definition         { $$=$1; }
type_definition: access_type_definition         { $$=$1; }
type_definition: file_type_definition           { $$=$1; }
type_definition: protected_type_declaration     { $$=$1; }
type_definition: protected_type_body            { $$=$1; }


enumeration_type_definition:   t_LeftParen enumeration_literal enumeration_type_definition_1 t_RightParen { $$="( "+$2+" "+$3+" )"; }
enumeration_type_definition_1: { $$=""; }
enumeration_type_definition_1: enumeration_type_definition_1 enumeration_type_definition_2 { $$=$1+" "+$2; }
enumeration_type_definition_2: t_Comma enumeration_literal { $$=","+$2; }

physical_type_definition : range_constraint  t_UNITS base_unit_decl
                           physical_type_definition_1 t_END unit_stat
                           {
                             $$=$1;
                             current->args=$3+"#"+$4;
                             current->args.prepend("units");
                             current->spec=VhdlDocGen::UNITS;
                           }

unit_stat: t_UNITS t_Identifier
unit_stat: t_UNITS

physical_type_definition_1: /* empty */ { $$=""; }
physical_type_definition_1: physical_type_definition_1 physical_type_definition_2 { $$=$1+" "+$2; }
physical_type_definition_2: secondary_unit_decl  { $$=$1+"#"; }

base_unit_decl: t_Identifier t_Semicolon { $$=$1; }

secondary_unit_decl: t_Identifier t_EQSym physical_literal t_Semicolon { $$=$1+"="+$3 }

unconstrained_array_definition: t_ARRAY t_LeftParen
                                index_subtype_definition unconstrained_array_definition_1 t_RightParen t_OF
                                subtype_indic
    {
      QCString sr1=" array ( "+$3+" "+$4;
      QCString sr2=" ) of "+$7;
      $$=sr1+sr2;
    }

unconstrained_array_definition_1: { $$=""; }
unconstrained_array_definition_1: unconstrained_array_definition_1 unconstrained_array_definition_2 { $$=$1+"  "+$2 }
unconstrained_array_definition_2: t_Comma index_subtype_definition { $$=", "+$2 }

index_subtype_definition: mark t_RANGE t_Box { $$=$1+" range<> "; }

constrained_array_definition: t_ARRAY index_constraint t_OF subtype_indic { $$=" array "+$2+" of "+$4; }

record_type_simple_name:/*empty*/     { $$=""; }
                       | t_Identifier { $$=$1; }

record_type_definition: t_RECORD element_decl record_type_definition_1 t_END
                        t_RECORD record_type_simple_name
    {
      QRegExp reg("[\\s]");
      QCString oo=$2+" "+$3;
      current->spec=VhdlDocGen::RECORD;
      current->args=oo;
      current->args.replace(reg,"%");
      current->args.prepend("record");
      $$=$2+" "+$3;
    }

record_type_definition_1: /*empty*/ { $$=""; }
record_type_definition_1: record_type_definition_1 record_type_definition_2
    {
      $$=$1+" "+$2;
    }
record_type_definition_2: element_decl { $$=$1; }

element_decl: idf_list t_Colon subtype_indic t_Semicolon { $$=$1+":"+$3+"#"; }

access_type_definition: t_ACCESS subtype_indic { $$="access "+$2; }

file_type_definition: t_FILE t_OF mark  { $$="file of "+$3; }

/*--------------------------------------------------
--  Subtypes and Constraints
----------------------------------------------------*/

subtype_decl: t_SUBTYPE t_Identifier t_IS subtype_indic t_Semicolon
    {
      addVhdlType($2,getParsedLine(t_SUBTYPE),Entry::VARIABLE_SEC,VhdlDocGen::SUBTYPE,0,$4.data());
    }
subtype_decl:     t_SUBTYPE error  t_Semicolon  { $$=""; }
subtype_indic:    mark subtype_indic_1          { $$=$1+" "+$2; }
subtype_indic:    subtype_indic1                { $$=$1; }
subtype_indic_1:  /* empty */                   { $$=""; }
subtype_indic_1:  gen_association_list          { $$=$1; }

subtype_indic1:   mark mark range_constraint    { $$=$1+" "+$2+" "+$3; }
subtype_indic1:   mark range_constraint         { $$=$1+" "+$2; }
subtype_indic1:   mark mark subtype_indic1_1    { $$=$1+" "+$2+" "+$3; }
subtype_indic1_1:  /* empty */                  { $$=""; }
subtype_indic1_1: gen_association_list          { $$=$1; }

range_constraint: t_RANGE range_spec            { $$="range "+$2; }
//range_constraint        : array_constraint

index_constraint: t_LeftParen discrete_range
                  index_constraint_1
                  t_RightParen                  { $$="("+$2+" "+$3+")"; }
index_constraint_1:  /* empty */                { $$=""; }
index_constraint_1: index_constraint_1
                    index_constraint_2          { $$=$1+" "+$2; }
index_constraint_2: t_Comma discrete_range      { $$=","+$2; }

discrete_range: subtype_indic                   { $$=$1; }
discrete_range: range_spec                      { $$=$1; }

discrete_range1 : subtype_indic1                { $$=$1; }
discrete_range1 : expr direction expr           { $$=$1+"  "+$2+"  "+$3; }

range_spec  : attribute_name                    { $$=$1; }
range_spec  : simple_exp direction simple_exp   { $$=$1+"  "+$2+"  "+$3; }

direction  : t_TO     { $$=" to "; }
direction  : t_DOWNTO { $$=" downto "; }

/*--------------------------------------------------
--  Objects, Aliases, Files, Disconnections
----------------------------------------------------*/

constant_decl: t_CONSTANT idf_list t_Colon subtype_indic constant_decl_1 t_Semicolon
                                  {
                                    QCString it=$4+" "+$5;
                                    //  fprintf(stderr,"\n currP %d \n",currP);
                                    addVhdlType($2,getParsedLine(t_CONSTANT),Entry::VARIABLE_SEC,VhdlDocGen::CONSTANT,0,it.data());
                                  }
constant_decl_1:  /* empty */     { $$="";      }
constant_decl_1: t_VarAsgn expr   { $$=":="+$2; }

signal_decl: t_SIGNAL idf_list t_Colon subtype_indic signal_decl_1 signal_decl_2 t_Semicolon
                                  {
                                    QCString s=$4+" "+$6;
                                    addVhdlType($2,getParsedLine(t_SIGNAL),Entry::VARIABLE_SEC,VhdlDocGen::SIGNAL,0,s.data());
                                  }
signal_decl_2:  /* empty */       { $$=""; }
signal_decl_2: t_VarAsgn expr     { $$=":="+$2; }
signal_decl_1:  /* empty */       { $$=""; }
signal_decl_1: signal_kind        { $$=$1; }

variable_decl: t_VARIABLE idf_list t_Colon subtype_indic variable_decl_1 t_Semicolon
                                  {
                                    $$=$2+":"+$4+" "+$5;
                                  }
variable_decl: t_SHARED t_VARIABLE idf_list t_Colon subtype_indic variable_decl_1 t_Semicolon
                                  {
                                    $$=$5+" "+$6;
                                    addVhdlType($3,getParsedLine(t_VARIABLE),Entry::VARIABLE_SEC,VhdlDocGen::SHAREDVARIABLE,0,$$.data());
                                  }
variable_decl_1:  /* empty */     { $$=""; }
variable_decl_1: t_VarAsgn expr   { $$=":="+$2; }

object_class: t_CONSTANT          { $$="constant"; }
object_class: t_SIGNAL            { $$="signal"; }
object_class: t_VARIABLE          { $$="variable"; }
object_class: t_SHARED t_VARIABLE { $$="shared"; }
object_class: t_FILE              { $$="file"; }
object_class: t_TYPE              { $$="type"; }

signal_kind: t_BUS                { $$="bus"; }
signal_kind: t_REGISTER           { $$="register"; }

alias_decl: t_ALIAS alias_name_stat alias_spec t_IS name signature t_Semicolon
                                  {
                                    QCString s=$3+" is "+$5+$6;
                                    addVhdlType($2,getParsedLine(t_ALIAS),Entry::VARIABLE_SEC,VhdlDocGen::ALIAS,0,s.data());
                                  }
alias_decl: t_ALIAS alias_name_stat alias_spec t_IS error t_Semicolon { $$=""; }

alias_name_stat: t_Identifier     { $$=$1; }
alias_name_stat: t_StringLit      { $$=$1; }

alias_spec :/*empty*/              { $$=""; }
           | t_Colon subtype_indic { $$=","+$2; }
           ;

file_decl: t_FILE idf_list t_Colon subtype_indic t_IS file_decl_1 expr t_Semicolon
           {
             addVhdlType($2,getParsedLine(t_FILE),Entry::VARIABLE_SEC,VhdlDocGen::VFILE,0,$4.data());
           }

file_decl: t_FILE idf_list t_Colon t_Identifier fi_dec t_Semicolon
           {
             QCString s=$4+" "+$5;
             addVhdlType($2,getParsedLine(t_FILE),Entry::VARIABLE_SEC,VhdlDocGen::VFILE,0,s.data());
           }

fi_dec: /*empty*/              { $$=""; }
      |  t_OPEN expr t_IS expr { $$="open "+$2+" is "+s_str.qstr; }


file_decl_1:  /* empty */   { $$=""; }
file_decl_1: mode           { $$=$1; }

disconnection_spec: t_DISCONNECT signal_list t_Colon mark t_AFTER expr t_Semicolon

signal_list:   name signal_list_1
signal_list:   t_OTHERS
signal_list:   t_ALL
signal_list_1:  /* empty */
signal_list_1: signal_list_1 signal_list_2
signal_list_2: t_Comma name

/*--------------------------------------------------
--  Attribute Declarations and Specifications
----------------------------------------------------*/

attribute_decl: t_ATTRIBUTE t_Identifier t_Colon mark t_Semicolon
                {
                  addVhdlType($2,getParsedLine(t_ATTRIBUTE),Entry::VARIABLE_SEC,VhdlDocGen::ATTRIBUTE,0,$4.data());
                }

attribute_spec: t_ATTRIBUTE t_Identifier t_OF entity_spec t_IS expr t_Semicolon
                {
                  QCString oo=$4+" is "+$6;
                  addVhdlType($2,getParsedLine(t_ATTRIBUTE),Entry::VARIABLE_SEC,VhdlDocGen::ATTRIBUTE,0,oo.data());
                }

entity_spec : entity_name_list signature  t_Colon entity_class { $$=$1+$2+":"+$4;}	

entity_name_list:   designator entity_name_list_1         { $$=$1+" "+$2; }
entity_name_list:   t_OTHERS                              { $$="others";  }
entity_name_list:   t_ALL                                 { $$="all";     }
entity_name_list_1: /* empty */                           { $$="";        }
entity_name_list_1: entity_name_list_1 entity_name_list_2 { $$=$1+" "+$2; }
entity_name_list_2: t_Comma designator                    { $$=","+$2;    }

entity_class: t_ENTITY        { $$="entity";        }
entity_class: t_ARCHITECTURE  { $$="architecture";  }
entity_class: t_PACKAGE       { $$="package";       }
entity_class: t_CONFIGURATION { $$="configuration"; }
entity_class: t_COMPONENT     { $$="component";     }
entity_class: t_LABEL         { $$="label";         }
entity_class: t_TYPE          { $$="type";          }
entity_class: t_SUBTYPE       { $$="subtype";       }
entity_class: t_PROCEDURE     { $$="procedure";     }
entity_class: t_FUNCTION      { $$="function";      }
entity_class: t_SIGNAL        { $$="signal";        }
entity_class: t_VARIABLE      { $$="variable";      }
entity_class: t_CONSTANT      { $$="constant";      }
entity_class: t_GROUP         { $$="group";         }
entity_class: t_FILE          { $$="file";          }
entity_class: t_UNITS         { $$="units";         }
entity_class: t_LITERAL       { $$="literal";       }
entity_class: t_SEQUENCE      { $$="sequence";      }
entity_class: t_PROPERTY      { $$="property";      }


/*--------------------------------------------------
--  Schemes

--------------------------------------------------------------------------*/

if_generation_scheme: if_scheme

if_scheme: t_IF expr t_GENERATE  generate_statement_body  if_scheme_1 if_scheme_2
if_scheme: t_IF lable expr t_GENERATE  generate_statement_body  if_scheme_1 if_scheme_2

if_scheme_2: /* empty */
if_scheme_2: t_ELSE t_GENERATE  generate_statement_body
if_scheme_2: t_ELSE lable t_GENERATE  generate_statement_body
if_scheme_1: /* empty */
if_scheme_1: if_scheme_1 if_scheme_3
if_scheme_3: t_ELSIF expr t_GENERATE generate_statement_body
if_scheme_3: t_ELSIF lable expr t_GENERATE generate_statement_body

generation_scheme: for_scheme

iteration_scheme: for_scheme
iteration_scheme: while_scheme

for_scheme: t_FOR t_Identifier t_IN discrete_range
for_scheme: t_FOR lable t_Identifier t_IN discrete_range

while_scheme: t_WHILE expr

/*--------------------------------------------------
--  Concurrent Statements
----------------------------------------------------*/

concurrent_stats:   concurrent_stats_1
concurrent_stats_1: /* empty */
concurrent_stats_1: concurrent_stats_1 concurrent_stats_2
concurrent_stats_2: concurrent_stat

concurrent_stat : block_stat
                | concurrent_assertion_stat
                | concurrent_procedure_call
                | concurrent_signal_assign_stat
                | comp_inst_stat
                | generate_stat
                | procs_stat

block_stat: t_Identifier t_Colon t_BLOCK block_stat_0 block_stat_1 block_stat_2
            block_stat_3 block_stat_4 t_BEGIN concurrent_stats t_END t_BLOCK block_stat_5
            t_Semicolon
block_stat_5: /* empty */
block_stat_5: t_Identifier
block_stat_4: /* empty */
block_stat_4: block_stat_4 block_stat_6
block_stat_6: block_decltve_item
block_stat_3: /* empty */
block_stat_3: t_PORT interf_list t_Semicolon block_stat_7
//block_sta_7:  /* empty */
block_stat_7: t_PORT t_MAP association_list t_Semicolon
block_stat_2: /* empty */
block_stat_2: t_GENERIC interf_list t_Semicolon block_stat_8
block_stat_8: /* empty */
block_stat_8: t_GENERIC t_MAP association_list t_Semicolon
block_stat_1: /* empty */
block_stat_1: t_LeftParen expr t_RightParen  block_stat_0
block_stat_0: /* empty */
block_stat_0: t_IS

dot_name: t_Identifier                 { $$=$1; }
        | dot_name  t_Dot t_Identifier { $$=$1+"."+$3; }
        ;

mark_comp: dot_name  comp_1      { $$=$1+" "+$2; }
mark_comp: dot_name              { $$=$1; }

comp_1: t_LeftParen t_Identifier  t_RightParen  { $$="("+$2+")"; }

vcomp_stat: t_CONFIGURATION      { $$="configurátion";yyLineNr=s_str.iLine; }
vcomp_stat: t_ENTITY             { $$="entity";yyLineNr=s_str.iLine; }
vcomp_stat: t_COMPONENT          { $$="component";yyLineNr=s_str.iLine; }

comp_inst_stat:  t_Identifier   t_Colon  name   { yyLineNr=s_str.iLine; }     t_GENERIC t_MAP association_list comp_inst_stat_1 t_Semicolon
                             {
                                addCompInst($1.data(),$3.data(),0,yyLineNr);
                               }
comp_inst_stat:  t_Identifier   t_Colon     name   { yyLineNr=s_str.iLine; }    t_PORT t_MAP association_list t_Semicolon
                              {
                               addCompInst($1.data(),$3.data(),0,yyLineNr);
                             }

comp_inst_stat:  t_Identifier  t_Colon   vcomp_stat  mark_comp    t_PORT t_MAP association_list t_Semicolon
                             {
                                  addCompInst($1.data(),$4.data(),$3.data(),yyLineNr);
                              }
comp_inst_stat:  t_Identifier  t_Colon  vcomp_stat   mark_comp        t_GENERIC t_MAP association_list comp_inst_stat_1 t_Semicolon
                              {
                                addCompInst($1.data(),$4.data(),$3.data(),yyLineNr);
                              }
comp_inst_stat_1:  /* empty */
comp_inst_stat_1: t_PORT t_MAP association_list

concurrent_assertion_stat:  t_Identifier t_Colon  assertion_stat
concurrent_assertion_stat:  assertion_stat

concurrent_assertion_stat:  t_Identifier t_Colon  t_POSTPONED  assertion_stat
concurrent_assertion_stat:   t_POSTPONED assertion_stat

concurrent_procedure_call: t_Identifier t_Colon procedure_call_stat
concurrent_procedure_call: procedure_call_stat

concurrent_procedure_call: t_Identifier t_Colon t_POSTPONED procedure_call_stat
concurrent_procedure_call: t_POSTPONED procedure_call_stat

concurrent_signal_assign_stat: t_Identifier t_Colon condal_signal_assign
concurrent_signal_assign_stat: condal_signal_assign

concurrent_signal_assign_stat: t_Identifier t_Colon  t_POSTPONED  condal_signal_assign
concurrent_signal_assign_stat: t_POSTPONED  condal_signal_assign

concurrent_signal_assign_stat: t_Identifier t_Colon t_POSTPONED sel_signal_assign
concurrent_signal_assign_stat: t_POSTPONED sel_signal_assign

concurrent_signal_assign_stat: t_Identifier t_Colon sel_signal_assign
concurrent_signal_assign_stat: sel_signal_assign

condal_signal_assign: target t_LESym opts   condal_wavefrms t_Semicolon

condal_wavefrms: wavefrm
condal_wavefrms: wavefrm t_WHEN expr
condal_wavefrms: wavefrm t_WHEN expr t_ELSE condal_wavefrms

wavefrm:   wavefrm_element wavefrm_1
wavefrm:   t_UNAFFECTED
wavefrm_1: /* empty */
wavefrm_1: wavefrm_1 wavefrm_2
wavefrm_2: t_Comma wavefrm_element

wavefrm_element:   expr wavefrm_element_1
wavefrm_element_1: /* empty */
wavefrm_element_1: t_AFTER expr
wavefrm_element_1: t_NULL wavefrm_element_2
wavefrm_element_1: t_NULL
wavefrm_element_2: t_AFTER expr

target: name
target: aggregate

opts: opts_1 opts_2

opts_2: /* empty */
opts_2: t_TRANSPORT
opts_2: t_REJECT expr t_INERTIAL
opts_2: t_INERTIAL

opts_1: /* empty */
opts_1: t_GUARDED

sel_signal_assign: t_WITH expr t_SELECT target t_LESym opts sel_wavefrms t_Semicolon

sel_wavefrms:   sel_wavefrms_1 wavefrm t_WHEN choices
sel_wavefrms_1: /* empty */
sel_wavefrms_1: sel_wavefrms_1 sel_wavefrms_2
sel_wavefrms_2: wavefrm t_WHEN choices t_Comma

gen_stat1: /* empty */
        |  block_declarative_part  t_BEGIN
        | t_BEGIN

 // problem with double end
 // end;
 // end generate;

generate_statement_body:  gen_stat1 concurrent_stats

generate_stat : t_Identifier  t_Colon
                { pushLabel($1); }
                generation_scheme t_GENERATE
                gen_stat1 concurrent_stats  opstat

// stems from VHDL 2008 generate_statement_body
opstat: end_stats t_END generate_stat_1 t_Semicolon { popLabel(); }
opstat: t_END generate_stat_1 t_Semicolon           { popLabel(); }

generate_stat: t_Identifier  t_Colon
               { pushLabel($1); }
               if_generation_scheme opstat //    t_END   generate_stat_1 t_Semicolon { popLabel(); }
generate_stat: t_Identifier  t_Colon case_scheme

generate_stat_1: t_GENERATE              { $$=""; }
generate_stat_1: t_GENERATE t_Identifier { $$=$2; }

//end_stats :
end_stats: t_END t_Semicolon
end_stats: t_END t_Identifier t_Semicolon

procs_stat: t_Identifier t_Colon procs_stat1
               {
                 current->name=$1;
                 current->endBodyLine=s_str.yyLineNr;
                 newEntry();
               }

procs_stat: procs_stat1
               {
                 current->name=VhdlDocGen::getProcessNumber();
                 current->endBodyLine=s_str.yyLineNr;
                 newEntry();
               }

procs_stat1: procs_stat1_5
               { currP=VhdlDocGen::PROCESS; }
               t_PROCESS procs_stat1_1 procs_stat1_2 t_BEGIN seq_stats t_END
               procs_stat1_3 t_Semicolon
               { currP=0;
                 createFunction(currName,VhdlDocGen::PROCESS,$4.data());
               }
procs_stat1: error t_END procs_stat1_3  t_Semicolon { currP=0; }

procs_stat1_3:  /* empty */
procs_stat1_3: procs_stat1_5  t_PROCESS  procs_stat1_6

procs_stat1_5:  /* empty */ { $$=""; }
procs_stat1_5: t_POSTPONED  { $$="postponed"; }

procs_stat1_6:  /* empty */ { $$=""; }
procs_stat1_6: t_Identifier { $$=$1; }

procs_stat1_2:  /* empty */
procs_stat1_2:  t_IS
procs_stat1_2: procs_stat1_2 procs_stat1_4
procs_stat1_4: procs_decltve_item
procs_stat1_1:  /* empty */                                    { $$=""; }
procs_stat1_1: t_LeftParen t_ALL t_RightParen                  { $$="all"; }
procs_stat1_1: t_LeftParen sensitivity_list t_RightParen       { $$=$2; }

sensitivity_list:   name sensitivity_list_1                    { $$=$1+" "+$2; }
sensitivity_list_1: /* empty */                                { $$=""; }
sensitivity_list_1: sensitivity_list_1 sensitivity_list_2      { $$=$1+" "+$2; }
sensitivity_list_2: t_Comma name                               { $$=","+$2; }

/*--------------------------------------------------
--  Sequential Statements
----------------------------------------------------*/

seq_stats:   seq_stats_1
seq_stats_1: /* empty */
seq_stats_1: seq_stats_1 seq_stats_2
seq_stats_2: seq_stat

seq_stat: assertion_stat
seq_stat: lable assertion_stat
seq_stat: case_stat
seq_stat: exit_stat
seq_stat: if_stat
seq_stat: loop_stat
seq_stat: next_stat
seq_stat: null_stat
seq_stat: procedure_call_stat
seq_stat: return_stat
seq_stat: lable signal_assign_stat
seq_stat: signal_assign_stat
seq_stat: variable_assign_stat
seq_stat: wait_stat
seq_stat: lable wait_stat
seq_stat: report_statement

report_statement: loop_stat_1 t_REPORT expr assertion_stat_2  t_Semicolon

assertion_stat: t_ASSERT expr assertion_stat_1 assertion_stat_2 t_Semicolon
assertion_stat_2:  /* empty */
assertion_stat_2     : t_SEVERITY expr
assertion_stat_1     :  /* empty */
assertion_stat_1     : t_REPORT expr

choice_stat :  /* empty */
choice_stat :  t_Q

choice_stat_1:  /* empty */
choice_stat_1 :  t_Q
choice_stat_1 : t_Identifier

case_stat  : t_CASE choice_stat expr t_IS case_stat_alternative case_stat_1 t_END t_CASE choice_stat_1  t_Semicolon
case_stat  : lable t_CASE choice_stat expr t_IS case_stat_alternative case_stat_1 t_END t_CASE choice_stat_1  t_Semicolon

case_stat  : t_CASE error t_END t_CASE choice_stat_1  t_Semicolon
case_stat_1     :  /* empty */
case_stat_1     : case_stat_1 case_stat_2
case_stat_2     : case_stat_alternative

case_stat_alternative     : t_WHEN choices t_Arrow seq_stats

exit_stat  : t_EXIT exit_stat_1 exit_stat_2 t_Semicolon
exit_stat_2     :  /* empty */
exit_stat_2     : t_WHEN expr
exit_stat_1     :  /* empty */
exit_stat_1     : t_Identifier

if_stat    : t_IF expr t_THEN seq_stats if_stat_1 if_stat_2 t_END t_IF t_Semicolon
if_stat  :  t_IF expr t_THEN  error  t_END t_IF  t_Semicolon
if_stat_2  :  /* empty */
if_stat_2  : t_ELSE seq_stats
if_stat_1  :  /* empty */
if_stat_1  : if_stat_1 if_stat_3
if_stat_3  : t_ELSIF expr t_THEN seq_stats

loop_stat:   loop_stat_1 loop_stat_2 t_LOOP seq_stats t_END t_LOOP loop_stat_3 t_Semicolon
loop_stat_3: /* empty */
loop_stat_3: t_Identifier
loop_stat_2: /* epty */
loop_stat_2: iteration_scheme
loop_stat_1: /* empty */
loop_stat_1: t_Identifier t_Colon

next_stat:   t_NEXT next_stat_1 next_stat_2 t_Semicolon
next_stat_2:  /* empty */
next_stat_2: t_WHEN expr
next_stat_1:  /* empty */
next_stat_1: t_Identifier

null_stat: t_NULL t_Semicolon

procedure_call_stat: name t_Semicolon

return_stat:   t_RETURN return_stat_1 t_Semicolon
return_stat_1: /* empty */
return_stat_1: expr

signal_assign_stat: target t_LESym wavefrm t_Semicolon
                  | target t_LESym delay_mechanism wavefrm t_Semicolon
                  | target t_LESym t_FORCE inout_stat  expr t_Semicolon
                  | target t_LESym t_RELEASE inout_stat t_Semicolon
                  | selected_signal_assignment
                  | conditional_signal_assignment
                  ;

variable_assign_stat: variable_assign_stat_1 t_Semicolon
                    | conditional_variable_assignment
                    | lable selected_variable_assignment
                    | selected_variable_assignment

lable: t_Identifier t_Colon
variable_assign_stat_1: target t_VarAsgn expr
variable_assign_stat_1: lable target t_VarAsgn expr

wait_stat:   t_WAIT wait_stat_1 wait_stat_2 wait_stat_3 t_Semicolon
wait_stat_3: /* empty */
wait_stat_3: t_FOR expr
wait_stat_2: /* empty */
wait_stat_2: t_UNTIL expr
wait_stat_1: /* empty */
wait_stat_1: t_ON sensitivity_list


/*--------------------------------------------------
--  Components and Configurations
----------------------------------------------------*/
comp_end_dec : t_END                              { lastEntity=0; lastCompound=0; genLabels.resize(0); }
             | t_END t_COMPONENT entity_decl_5
             | t_END t_ARCHITECTURE entity_decl_5 { lastCompound=0; genLabels.resize(0); }
             | t_END t_ENTITY entity_decl_5       { lastEntity=0; genLabels.resize(0); }
             | t_END t_Identifier                 { lastEntity=0; lastCompound=0; genLabels.resize(0); }

iss :/*empty*/ { currP=VhdlDocGen::COMPONENT; }
    |  t_IS    { currP=VhdlDocGen::COMPONENT; }

comp_decl: t_COMPONENT t_Identifier iss comp_decl_1 comp_decl_2 comp_end_dec  t_Semicolon
           {
             addVhdlType($2,getParsedLine(t_COMPONENT),Entry::VARIABLE_SEC,VhdlDocGen::COMPONENT,0,0);
             currP=0;
           }
comp_decl_2:  /* empty */                      { $$=""; }
comp_decl_2: t_PORT interf_list t_Semicolon    { $$=$2; }
comp_decl_1:  /* empty */                      { $$=""; }
comp_decl_1: t_GENERIC interf_list t_Semicolon { $$=$2; }

block_config: t_FOR block_spec block_config_1 block_config_2 t_END t_FOR t_Semicolon
          {
           popConfig();
          }

block_config:   t_FOR error t_END t_FOR t_Semicolon { $$=""; }
block_config_2: /* empty */                         { $$=""; }
block_config_2: block_config_2 block_config_3       { $$=$1+"  "; }
block_config_3: config_item                         { $$=$1; }
block_config_1: /* empty */                         { $$=""; }
block_config_1: block_config_1 block_config_4       { $$=$1; }
block_config_4: use_clause                          { $$=$1; }

block_spec: name
    {
      $$=$1;

      if (levelCounter==0)
        addConfigureNode($1.data(),NULL,TRUE,FALSE);
      else
        addConfigureNode($1.data(),NULL,FALSE,FALSE);
    }

config_item: block_config { $$=$1; }
config_item: comp_config  { $$=$1; }

comp_config: t_FOR comp_spec comp_config_1 comp_config_2 t_END t_FOR t_Semicolon
             {
               $$=$2+" "+$3+" "+$4;
               popConfig();
             }
comp_config_2:  /* empty */   { $$=""; }
comp_config_2: block_config   { $$=$1; }
comp_config_1:  /*empty*/     { $$=""; }

comp_config_1: binding_indic_1  binding_indic_2  t_Semicolon   { $$=""; }
comp_config_1: t_USE t_VUNIT idf_list  t_Semicolon             { $$=""; }
comp_config_1: t_USE binding_indic t_Semicolon
             {
               addConfigureNode(compSpec.data(),$2.data(),FALSE,FALSE);
             }

config_spec: t_FOR comp_spec comp_spec_stat t_Semicolon                         
             { 
               addConfigureNode($2.data(),$3.data(),TRUE,FALSE,TRUE);currNode->confVhdl=lastCompound->name; 
             }
config_spec: t_FOR comp_spec comp_spec_stat t_Semicolon t_END t_FOR t_Semicolon 
             { 
               addConfigureNode($2.data(),$3.data(),TRUE,FALSE,TRUE);currNode->confVhdl=lastCompound->name; 
             }

comp_spec_stat: t_USE  binding_indic               { $$=$2; }
comp_spec_stat: t_USE t_VUNIT idf_list t_Semicolon { $$=""; }
comp_spec_stat: binding_indic_1  binding_indic_2   { $$=""; }

comp_spec: inst_list t_Colon expr
             {
               $$=$1+":"+$3;
               compSpec=$$;
             }

inst_list: idf_list  { $$=$1; }
inst_list: t_ALL     { $$="all"; }
inst_list: t_OTHERS  { $$="others"; }

binding_indic   : entity_aspect binding_indic_1 binding_indic_2 { $$=$1; }

binding_indic_2:
binding_indic_2: t_PORT t_MAP association_list

binding_indic_1:
binding_indic_1: t_GENERIC t_MAP association_list


entity_aspect: t_ENTITY name { $$="entity "+$2; }
entity_aspect: t_CONFIGURATION mark { $$="configuration "+ $2; }
entity_aspect: t_OPEN { $$="open "; }
             ;

group_constituent: t_Identifier    { $$=$1; }
                 | t_CharacterLit { $$=$1; }
                 ;

group_constituent_list: group_constituent                                { $$=$1; }
                      | group_constituent_list t_Comma group_constituent { $$=$1+","+$3; }
                      ;

group_declaration : t_GROUP t_Identifier t_Colon group_name   t_LeftParen  group_constituent_list  t_RightParen t_Semicolon
                    {
                      // $$=$2+":"+$4+$6;
                      $$="("+$4+$6+")";
                      addVhdlType($2,getParsedLine(t_GROUP),Entry::VARIABLE_SEC,VhdlDocGen::GROUP,$$.data(),0);
                    }

group_template_declaration :  t_GROUP  t_Identifier t_IS  t_LeftParen  entity_class_entry_list  t_RightParen t_Semicolon
                    {
                      $$=$2+":"+$5;
                      addVhdlType($2,getParsedLine(t_GROUP),Entry::VARIABLE_SEC,VhdlDocGen::GROUP,$5.data(),0);
                    }

group_template_declaration: t_GROUP  t_Identifier t_IS  t_LeftParen error t_Semicolon  t_RightParen { $$=""; }

entity_class_entry : entity_class tbox

tbox :  /* empty */ { $$="";   }
tbox :  t_Box       { $$="<>"; }

entity_class_entry_list: entity_class_entry         { $$=$1; }
                       | entity_class_entry_list
                         t_Comma entity_class_entry { $$=$1+","+$3; }
                       ;

group_name: t_Identifier { $$=$1; }
group_name: t_StringLit  { $$=$1; }

t_Identifier: t_LETTER
    {
      $$=s_str.qstr;
    }

t_BitStringLit: t_DIGIT
    {
      $$=s_str.qstr;
    }

t_StringLit: t_STRING
    {
      $$=s_str.qstr;
    }

t_AbstractLit: t_ABSTRLIST
    {
      $$=s_str.qstr;
    }

t_CharacterLit: t_CHARLIST
    {
      $$=s_str.qstr;
    }


/*--------------------------------------------------
--  VHDL 2002 extensions
-- to do: must be added
-----------------------------------------------------*/
protected_type_declaration:t_PROTECTED protected_stats t_END protected_stat_1 { $$=""; }
protected_type_declaration:t_PROTECTED error  t_END protected_stat_1 { $$=""; }

protected_stats:       /* empty */
protected_stats:       protected_stats   protected_stat_decl_1
protected_stat_decl_1: protected_type_declaration_item
protected_stat_1:      t_PROTECTED
protected_stat_1:      t_PROTECTED t_Identifier

protected_type_declaration_item: use_clause
protected_type_declaration_item: attribute_spec
protected_type_declaration_item: subprog_decl
protected_type_declaration_item:  subprogram_instantiation_decl

protected_type_body: t_PROTECTED t_BODY protected_body_stats t_END protected_body_stat_1 { $$=""; }
protected_type_body: t_PROTECTED t_BODY error t_END protected_body_stat_1 { $$=""; }

protected_body_stats: /* empty */
protected_body_stats:  protected_body_stats   protected_body_stat_decl_1
protected_body_stat_decl_1: protected_type_body_declaration_item

protected_body_stat_1: t_PROTECTED t_BODY
protected_body_stat_1: t_PROTECTED t_BODY t_Identifier

protected_type_body_declaration_item: subprog_decltve_item // same as subprog

/*--------------------------------------------------
--  VHDL 2008 extensions
-- to do: must be added
-----------------------------------------------------*/
context_ref: t_CONTEXT  sel_list t_Semicolon { $$="context "+$2; }

context_decl: t_CONTEXT t_Identifier t_IS { parse_sec=CONTEXT_SEC; }  libustcont_stats t_END context_stat_1 t_Semicolon
                        {
                          parse_sec=0;
                          QCString v=$5;
                          addVhdlType($2,getParsedLine(t_LIBRARY),Entry::VARIABLE_SEC,VhdlDocGen::LIBRARY,"context",$5.data());
                        }
context_decl: t_CONTEXT t_Identifier t_IS  t_END context_stat_1 t_Semicolon
                        {
                          addVhdlType($2,getParsedLine(t_LIBRARY),Entry::VARIABLE_SEC,VhdlDocGen::LIBRARY,"context",0);
                        }

context_stat_1: t_CONTEXT
context_stat_1: t_CONTEXT t_Identifier

libustcont_stats: libustcont_stat                     { $$ = $1; }
libustcont_stats: libustcont_stats  libustcont_stat   { $$ = $1+"#"+$2; }

libustcont_stat: use_clause  { $$ = $1; }
libustcont_stat: lib_clause  { $$ = $1; }
libustcont_stat: context_ref { $$ = $1; }

package_instantiation_decl: t_PACKAGE t_Identifier t_IS t_NEW dot_name signature t_Semicolon
    {
      $$=" is new "+$5+$6;
      //Entry * pp=lastCompound;
      //Entry * pps=lastEntity  ;
      //assert(false);
      addVhdlType($2,getParsedLine(t_PACKAGE),Entry::VARIABLE_SEC,VhdlDocGen::INSTANTIATION,"package",$$.data());
    }
package_instantiation_decl: t_PACKAGE t_Identifier t_IS t_NEW dot_name signature  gen_assoc_list t_Semicolon
    {
      $$=" is new "+$5+$6;
      addVhdlType($2,getParsedLine(t_PACKAGE),Entry::VARIABLE_SEC,VhdlDocGen::INSTANTIATION,"package",$$.data());
    }
package_instantiation_decl: t_PACKAGE error  t_Identifier t_IS t_NEW t_Semicolon  { $$=""; }

subprogram_instantiation_decl: t_FUNCTION  t_Identifier t_IS   t_NEW dot_name  signature  t_Semicolon
    {
      $$= " is new "+$5+$6;
      addVhdlType($2,getParsedLine(t_FUNCTION),Entry::VARIABLE_SEC,VhdlDocGen::INSTANTIATION,"function ",$$.data());
    }
subprogram_instantiation_decl: t_FUNCTION  t_Identifier t_IS t_NEW dot_name  signature gen_assoc_list t_Semicolon
    {
      $$=" is new "+$5+$6;
      addVhdlType($2,getParsedLine(t_FUNCTION),Entry::VARIABLE_SEC,VhdlDocGen::INSTANTIATION,"function ",$$.data());
    }
subprogram_instantiation_decl: t_FUNCTION   t_Identifier t_IS   t_NEW error t_Semicolon { $$=""; }

signature:/*empty*/                  { $$=""; }
signature: t_LEFTBR signature1 
           t_RIGHTBR                 { $$="["+$2+" ]"; }
signature: t_LEFTBR t_RIGHTBR        { $$="[ ]"; }

signature1: t_RETURN mark            { $$="return "+$2; }
signature1: mark_stats               { $$=$1; }
signature1: mark_stats t_RETURN mark { $$=$1+" return "+$3; }

mark_stats: mark                     { $$=$1; }
mark_stats: mark_stats mark_stats_1  { $$=$1+" "+$2; }
mark_stats_1: t_Comma mark           { $$=" , "+$2; }

case_scheme:  t_CASE expr   t_GENERATE when_stats ttend  t_END t_GENERATE generate_stat_1  t_Semicolon
case_scheme:  t_CASE expr   t_GENERATE when_stats        t_END t_GENERATE generate_stat_1  t_Semicolon
case_scheme:  t_CASE error  t_GENERATE error             t_END t_GENERATE generate_stat_1  t_Semicolon

when_stats_1: t_WHEN lable choices t_Arrow generate_statement_body
when_stats_1: t_WHEN choices t_Arrow generate_statement_body
when_stats:   when_stats  when_stats_1
when_stats:   when_stats_1

ttend: t_END t_Semicolon
ttend: t_END t_Identifier t_Semicolon

conditional_signal_assignment: conditional_waveform_assignment
conditional_signal_assignment: conditional_force_assignment

conditional_waveform_assignment: target t_LESym wavefrm_element t_WHEN expr else_wave_list t_Semicolon
conditional_waveform_assignment: target t_LESym delay_mechanism wavefrm_element t_WHEN expr else_wave_list t_Semicolon
conditional_waveform_assignment: target t_LESym wavefrm_element t_WHEN expr t_Semicolon
conditional_waveform_assignment: target t_LESym delay_mechanism wavefrm_element t_WHEN expr t_Semicolon
conditional_waveform_assignment: target t_LESym error t_Semicolon

else_wave_list: t_ELSE expr t_WHEN expr
else_wave_list: t_ELSE expr

conditional_force_assignment:  target t_LESym t_FORCE  inout_stat expr t_WHEN expr else_stat t_Semicolon
conditional_force_assignment:  target t_LESym t_FORCE  inout_stat expr t_WHEN expr t_Semicolon

selected_signal_assignment : selected_waveform_assignment
selected_signal_assignment : selected_force_assignment

selected_waveform_assignment: t_WITH expr t_SELECT choice_stat
                              target t_LESym delay_stat sel_wave_list

delay_stat:
delay_stat: delay_mechanism

sel_wave_list: wavefrm_element t_WHEN choices t_Comma  sel_wave_list
sel_wave_list: sel_wave_list_1

sel_wave_list_1: wavefrm_element  t_WHEN choices t_Semicolon

selected_force_assignment: t_WITH expr t_SELECT choice_stat target t_LESym t_FORCE
                                                  inout_stat sel_var_list

inout_stat:
inout_stat: t_IN
inout_stat: t_OUT

delay_mechanism : t_TRANSPORT
                | t_REJECT expr t_INERTIAL
                |  t_INERTIAL

conditional_variable_assignment : variable_assign_stat_1 t_WHEN expr else_stat t_Semicolon
conditional_variable_assignment : variable_assign_stat_1 t_WHEN expr t_Semicolon

else_stat: t_ELSE expr t_WHEN expr
else_stat: else_stat t_ELSE expr t_WHEN expr
else_stat: t_ELSE expr

selected_variable_assignment: t_WITH expr t_SELECT choice_stat select_name t_VarAsgn sel_var_list

sel_var_list: expr t_WHEN choices t_Comma  sel_var_list
sel_var_list: sel_var_list_1

sel_var_list_1: expr t_WHEN choices t_Semicolon

select_name: name
           | aggregate

interface_subprogram_decl: iproc { $$ = $1; }
                         | ifunc { $$=$1; }
                         ;
iproc: t_PROCEDURE t_Identifier param { $$ = "procedure "+$2+$3; current->name=$2; }

ifunc: t_FUNCTION   func_name param   t_RETURN mark return_is
    {
      QCString s=$6;
      if (!s.isEmpty())
      {
        s.prepend(" is ");
      }
      $$=" function "+$2+$3+$5+s;
      current->name=$2;
    }
ifunc: func_prec t_FUNCTION  func_name param t_RETURN mark return_is
    {
      QCString s=$7;
      if (!s.isEmpty())
      {
        s.prepend(" is ");
      }
      $$=$1+" function "+$3+$4+" return "+$6+s;
      current->name=$3;
    }

func_name: t_Identifier   { $$=$1; }
         | t_StringLit    { $$=$1; }  // "?<"
         ;

return_is:  /* empty */       { $$="";   }
         | t_IS  t_Identifier { $$=$2;   }
         | t_IS t_Box         { $$="<>"; }

param: /* empty */ { $$=""; }
param: t_PARAMETER { $$="parameter "; }
param: t_PARAMETER { parse_sec=PARAM_SEC; }
                   t_LeftParen interf_element interf_list_1 t_RightParen
                   { parse_sec=0; }

param: t_LeftParen  interf_element interf_list_1 t_RightParen { $$="("+$2+")"; }

interface_package_decl: t_PACKAGE t_Identifier t_IS t_NEW dot_name
                        {
                          $$="package "+$2+" is new "+$5;
                          current->name=$2;
                        }
interface_package_decl: t_PACKAGE t_Identifier t_IS t_NEW dot_name gen_assoc_list 
                        { 
                          $$="package "+$2+" is new "+$5+"( ... )" ; 
                          current->name=$2; 
                        }

gen_assoc_list:         t_GENERIC t_MAP   association_list

gen_interface_list :    t_GENERIC
                        {
                          //int u=s_str.iLine;
                          parse_sec=GEN_SEC;
                        }
                        interf_list
                        {
                          QCString vo=$3;
                          parse_sec=0;
                        }

external_name: t_SLSL sig_stat external_pathname t_Colon  subtype_indic  t_SRSR
                        {
                          QCString s="<<"+$2;
                          QCString s1=$3+":"+$5+">>";
                          $$=s+s1;
                        }

sig_stat:  t_CONSTANT   { $$="constant "; }
sig_stat:  t_SIGNAL     { $$="signal ";   }
sig_stat:  t_VARIABLE   { $$="variable "; }

external_pathname: absolute_pathname  { $$=$1; }
                 | relative_pathname  { $$=$1; }
                 | package_path_name  { $$=$1; }
                 ;

absolute_pathname:  t_Dot  pathname_element_list t_Identifier  { $$="."+$2+$3; }
absolute_pathname:  t_Dot  t_Identifier  { $$="."+$2; }

relative_pathname: neg_list pathname_element_list t_Identifier { $$=$1+$2+$3; }
relative_pathname: neg_list t_Identifier { $$=$1+$2; }

neg_list:  t_Neg t_Dot          { $$="^."; }
neg_list: neg_list  t_Neg t_Dot { $$=$1+"^."; }

pathname_element: t_Identifier  { $$=$1; }
                | t_Identifier t_LeftParen expr  t_RightParen  { $$=$1+"("+$3+")"; }
                ;

pathname_element_list: pathname_element t_Dot     { $$=$1+"."; }
                     | pathname_element_list pathname_element t_Dot   { $$=$1+$2+"."; }

package_path_name: t_At dot_name { $$="@"+$2; }

tool_directive: t_ToolDir
{
// fprintf(stderr,"\n  tooldir %s",s_str.qstr.data() );
}


%%
extern FILE* yyout;
extern YYSTYPE vhdlScanYYlval;

void vhdlScanYYerror(const char* /*str*/)
{
//  fprintf(stderr,"\n<---error at line %d  : [ %s]   in file : %s ---->",s_str.yyLineNr,s_str.qstr.data(),s_str.fileName);
 // exit(0);
}

void vhdlParse()
{
//  //myconv=conv;
  vhdlScanYYparse();
}

//int lex(void)
//{
//  return myconv->doLex();
//}

VhdlContainer*  getVhdlCont()
{
  return &s_str;
}

Entry* getVhdlCompound()
{
  if (lastEntity) return lastEntity;
  if (lastCompound) return lastCompound;
  return NULL;
}

QList<VhdlConfNode>& getVhdlConfiguration() { return  configL; }

static void addCompInst(char *n, char* instName, char* comp,int iLine)
{

  current->spec=VhdlDocGen::INSTANTIATION;
  current->section=Entry::VARIABLE_SEC;
  current->startLine=iLine;
  current->bodyLine=iLine;
  current->type=instName;                       // foo:instname e.g proto or work. proto(ttt)
  current->exception=genLabels;                 // |arch|label1:label2...
  current->name=n;                              // foo
  current->args=lastCompound->name;             // architecture name
  current->includeName=comp;                    // component/enity/configuration

  //printf(" \n genlable: [%s]  inst: [%s]  name: [%s] \n",genLabels.data(),instName,n);

  if (lastCompound)
  {
    current->args=lastCompound->name;
    if (true) // !findInstant(current->type))
    {
      initEntry(current);
      instFiles.append(new Entry(*current));
    }
  
    Entry *temp=current;  // hold  current pointer  (temp=oldEntry)
    current=new Entry;     // (oldEntry != current)
    delete  temp;
   
  }
  else
  {
    newEntry();
  }
}

static void pushLabel(const QCString &label)
{
  genLabels+="|"+label;
}

static void popLabel()
{
  int u=genLabels.findRev("|");
  if (u<0) return;
  genLabels=genLabels.left(u);
}

static void popConfig()
{
  assert(currNode);
  currNode=currNode->prevNode;
  // printf("\n pop arch %s ",currNode->arch.data());
}

static void addConfigureNode(const char* a,const char*b, bool isRoot,bool isLeave,bool inlineConf)
{
  VhdlConfNode* co;
  QCString ent,arch,lab;
  ent=a;
  lab =  VhdlDocGen::parseForConfig(ent,arch);

  if (b)
  {
    ent=b;
    lab=VhdlDocGen::parseForBinding(ent,arch);
  }

  co=new VhdlConfNode(a,b,confName.data());
  if (inlineConf)
  {
    co->isInlineConf=TRUE;
  }

  if (isRoot)
  {
    co->isRoot=TRUE;
    configL.append(co);
    currNode=co;
    currNode->prevNode=currNode;
  }
  else if (!isLeave)
  {
    currNode->addNode(co);
    co->prevNode=currNode;
    currNode=co;
  }
  else
  {
    assert(0);
    co=new VhdlConfNode(a,b,confName.data());
    currNode->addNode(co);
  }
}// addConfigure

//------------------------------------------------------------------------------------------------------------

static bool isFuncProcProced()
{
  if (currP==VhdlDocGen::FUNCTION ||
      currP==VhdlDocGen::PROCEDURE ||
      currP==VhdlDocGen::PROCESS
     )
  {
    return TRUE;
  }
  return FALSE;
}

static void initEntry(Entry *e)
{
  e->fileName = s_str.fileName;
  e->lang=SrcLangExt_VHDL;
  isVhdlDocPending();
  initGroupInfo(e);
}

static void addProto(const char *s1,const char *s2,const char *s3,
                     const char *s4,const char *s5,const char *s6)
{
 // (void)s3; // avoid unused warning
  (void)s5; // avoid unused warning
  static QRegExp reg("[\\s]");
  QCString name=s2;
  QStringList ql=QStringList::split(",",name,FALSE);

  for (uint u=0;u<ql.count();u++)
  {
    Argument *arg=new Argument;
    arg->name=ql[u].utf8();
    if (s3)
    {
      arg->type=s3;
    }
    arg->type+=" ";
    arg->type+=s4;
    if (s6)
    {
      arg->type+=s6;
    }
    if (parse_sec==GEN_SEC && param_sec==0)
    {
      arg->defval="gen!";
    }

    if (parse_sec==PARAM_SEC)
    {
      assert(false);
    }

    arg->defval+=s1;
    arg->attrib="";//s6;

    current->argList->append(arg);
    current->args+=s2;
    current->args+=",";
  }
}

static void createFunction(const QCString &impure,int spec,
                           const QCString &fname)
{
  int it=0;
  current->bodyLine=getParsedLine(spec);
  current->spec=spec;
  current->section=Entry::FUNCTION_SEC;
  current->exception=impure;

  if (parse_sec==GEN_SEC)
  {
    current->spec= VhdlDocGen::GENERIC;
    current->section=Entry::FUNCTION_SEC;
  }

  if (currP==VhdlDocGen::PROCEDURE)
  {
    current->name=impure;
    current->exception="";
    it=t_PROCEDURE;
  }
  else
  {
    current->name=fname;
    it=t_FUNCTION;
  }

  if (spec==VhdlDocGen::PROCESS)
  {
    it=t_PROCESS;
    current->args=fname;
    current->name=impure;
    if (!fname.isEmpty())
    {
      QStringList q1=QStringList::split(',',fname);
      for (uint ii=0;ii<q1.count();ii++)
      {
        Argument *arg=new Argument;
        arg->name=q1[ii].utf8();
        current->argList->append(arg);
      }
    }
  }

  current->startLine=getParsedLine(it);
  current->bodyLine=getParsedLine(it);
}

static void addVhdlType(const QCString &name,int startLine,int section,int spec,
                        const char* args,const char* type,Protection prot)
{
  static QRegExp reg("[\\s]");

  //int startLine=getParsedLine(spec);

  if (isFuncProcProced())
  {
    return;
  }

  if (parse_sec==GEN_SEC)
  {
    spec= VhdlDocGen::GENERIC;
  }

  // more than one name   ?
  QStringList ql=QStringList::split(",",name,FALSE);

  for (uint u=0;u<ql.count();u++)
  {
    current->name=ql[u].utf8();
 //   if (section==Entry::VARIABLE_SEC &&  !(spec == VhdlDocGen::USE || spec == VhdlDocGen::LIBRARY) )
 //   {
 //     current->name.prepend(VhdlDocGen::getRecordNumber());
 //   }
   
    current->startLine=startLine;
    current->bodyLine=startLine;
    current->section=section;
    current->spec=spec;
    current->fileName=s_str.fileName;
    if (current->args.isEmpty())
    {
      current->args=args;
      current->args.replace(reg,"%"); // insert dummy chars because wihte spaces are removed
    }
    current->type=type;
    current->type.replace(reg,"%"); // insert dummy chars because white spaces are removed
    current->protection=prot;
    newEntry();
  }
}

static void newEntry()
{
  if (current->spec==VhdlDocGen::ENTITY       ||
      current->spec==VhdlDocGen::PACKAGE      ||
      current->spec==VhdlDocGen::ARCHITECTURE ||
      current->spec==VhdlDocGen::PACKAGE_BODY
     )
  {
    current_root->addSubEntry(current);
  }
  else
  {
    if (lastCompound)
    {
      lastCompound->addSubEntry(current);
    }
    else
    {
      if (lastEntity)
      {
        lastEntity->addSubEntry(current);
      }
      else
      {
        current_root->addSubEntry(current);
      }
    }
  }
  current = new Entry ;
  initEntry(current);
}

