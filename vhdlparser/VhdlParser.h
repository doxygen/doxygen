#ifndef VHDLPARSER_H
#define VHDLPARSER_H
#include "JavaCC.h"
#include "CharStream.h"
#include "Token.h"
#include "TokenManager.h"
#include "vhdljjparser.h"
#include "VhdlParserConstants.h"
#include "ErrorHandler.h"
namespace vhdl {
namespace parser {
  struct JJCalls {
    int        gen;
    int        arg;
    JJCalls*   next;
    Token*     first;
    ~JJCalls() { if (next) delete next; }
     JJCalls() { next = nullptr; arg = 0; gen = -1; first = nullptr; }
  };

class VhdlParser {
public:
QCString abstract_literal();
QCString access_type_definition();
QCString actual_designator();
QCString actual_parameter_part();
QCString actual_part();
QCString adding_operator();
QCString aggregate();
QCString alias_declaration();
QCString alias_designator();
void allocator();
void architecture_body();
void architecture_declarative_part();
void architecture_statement_part();
QCString array_type_definition();
QCString assertion();
QCString assertion_statement();
QCString association_element();
QCString association_list();
QCString attribute_declaration();
QCString attribute_designator();
QCString attribute_name();
QCString attribute_specification();
QCString base();
QCString base_specifier();
QCString base_unit_declaration();
QCString based_integer();
QCString based_literal();
QCString basic_identifier();
void binding_indication();
QCString bit_string_literal();
QCString bit_value();
void block_configuration();
void block_declarative_item();
void block_declarative_part();
void block_header();
void block_specification();
void block_statement();
void block_statement_part();
void case_statement();
void case_statement_alternative();
QCString character_literal();
QCString choice();
QCString choices();
void component_configuration();
void component_declaration();
void component_instantiation_statement();
void component_specification();
QCString composite_type_definition();
void concurrent_assertion_statement();
void concurrent_procedure_call_statement();
void concurrent_signal_assignment_statement();
void concurrent_statement();
QCString condition();
QCString condition_clause();
void conditional_signal_assignment();
void conditional_waveforms();
void configuration_declaration();
void configuration_declarative_item();
void configuration_declarative_part();
void configuration_item();
void configuration_specification();
QCString constant_declaration();
QCString constraint_array_definition();
void context_clause();
QCString constraint();
void context_item();
QCString decimal_literal();
QCString delay_mechanism();
void design_file();
void design_unit();
QCString designator();
QCString direction();
void disconnection_specification();
void guarded_signal_specificatio();
QCString discrete_range();
QCString element_association();
QCString element_declaration();
QCString entity_aspect();
QCString entity_class();
QCString entity_class_entry();
QCString entity_class_entry_list();
void entity_declaration();
void entity_declarative_item();
void entity_declarative_part();
QCString entity_designator();
void entity_header();
QCString entity_name_list();
QCString entity_specification();
void entity_statement();
void entity_statement_part();
QCString entity_tag();
QCString enumeration_literal();
QCString enumeration_type_definition();
QCString exit_statement();
QCString expression();
QCString logop();
QCString extended_identifier();
QCString factor();
QCString file_declaration();
QCString file_logical_name();
QCString file_open_information();
QCString file_type_definition();
QCString floating_type_definition();
QCString formal_designator();
QCString formal_parameter_list();
QCString formal_part();
QCString full_type_declaration();
QCString function_call();
void generate_statement();
void generate_scheme();
void generic_clause();
QCString generic_list();
void generic_map_aspect();
QCString group_constituent();
QCString group_constituent_list();
QCString group_declaration();
QCString group_template_declaration();
void guarded_signal_specification();
QCString identifier();
QCString identifier_list();
void if_statement();
QCString incomplete_type_declaration();
QCString index_constraint();
QCString index_specification();
QCString index_subtype_definition();
QCString instantiation_unit();
QCString instantiation_list();
QCString integer();
QCString integer_type_definition();
QCString interface_declaration();
QCString interface_element();
QCString interface_file_declaration();
QCString interface_list();
QCString interface_variable_declaration();
QCString iteration_scheme();
QCString label();
QCString library_clause();
QCString library_unit();
QCString literal();
QCString logical_operator();
QCString loop_statement();
QCString miscellaneous_operator();
QCString mode();
QCString multiplying_operation();
QCString name();
QCString name_ext1();
QCString name_ext();
QCString test_att_name();
QCString indexed_name();
QCString next_statement();
QCString null_statement();
QCString numeric_literal();
QCString object_class();
QCString operator_symbol();
void options();
void package_body();
void package_body_declarative_item();
void package_body_declarative_part();
void package_header();
void package_declaration();
void geninter();
void package_declarative_item();
void package_declarative_part();
QCString parameter_specification();
QCString physical_literal();
QCString physical_type_definition();
void port_clause();
QCString port_list();
void port_map_aspect();
QCString primary();
void primary_unit();
QCString procedure_call();
QCString procedure_call_statement();
QCString process_declarative_item();
QCString process_declarative_part();
void process_statement();
void process_statement_part();
QCString qualified_expression();
QCString range();
QCString range_constraint();
void record_type_definition();
QCString relation();
QCString relation_operator();
QCString report_statement();
QCString return_statement();
QCString scalar_type_definition();
void secondary_unit();
QCString secondary_unit_declaration();
QCString selected_name();
void selected_signal_assignment();
void selected_waveforms();
QCString sensitivity_clause();
QCString sensitivity_list();
QCString sequence_of_statement();
QCString sequential_statement();
QCString shift_expression();
QCString shift_operator();
QCString sign();
QCString signal_assignment_statement();
void semi();
void signal_declaration();
QCString signal_kind();
QCString signal_list();
QCString signature();
QCString simple_expression();
void simple_name();
QCString slice_name();
QCString string_literal();
void subprogram_body();
void subprogram_declaration();
void subprogram_1();
QCString subprogram_declarative_item();
QCString subprogram_declarative_part();
void subprogram_kind();
void subprogram_specification();
void subprogram_statement_part();
QCString subtype_declaration();
QCString reslution_indication();
QCString subtype_indication();
QCString suffix();
QCString target();
QCString term();
QCString timeout_clause();
QCString type_conversion();
QCString type_declaration();
QCString type_definition();
QCString type_mark();
QCString unconstraint_array_definition();
QCString use_clause();
QCString variable_assignment_statement();
QCString variable_declaration();
QCString wait_statement();
QCString waveform();
QCString waveform_element();
QCString protected_type_body();
void protected_type_body_declarative_item();
void protected_type_body_declarative_part();
QCString protected_type_declaration();
void protected_type_declarative_item();
void protected_type_declarative_part();
QCString context_ref();
void context_declaration();
QCString libustcont_stats();
void package_instantiation_declaration();
QCString interface_package_declaration();
QCString subprogram_instantiation_declaration();
void gen_assoc_list();
void gen_interface_list();
void case_scheme();
void when_stats();
void ttend();
void generate_statement_body();
void generate_statement_body1();
void generate_scheme_1();
void generate_scheme_2();
void generate_scheme_3();
QCString external_name();
QCString sig_stat();
QCString external_pathname();
QCString absolute_pathname();
QCString relative_pathname();
QCString neg_list();
QCString pathname_element();
QCString pathname_element_list();
QCString package_path_name();
void conditional_signal_assignment_wave();
void conditional_waveform_assignment();
void else_wave_list();
void conditional_force_assignment();
void selected_signal_assignment_wave();
void selected_variable_assignment();
void select_name();
void selected_waveform_assignment();
void selected_force_assignment();
void sel_var_list();
void sel_wave_list();
void inout_stat();
void else_stat();
QCString interface_subprogram_declaration();
QCString iproc();
QCString ifunc();
QCString param();
void parseInline();
 inline bool jj_2_1(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_1() || jj_done);
 { jj_save(0, xla); }
  }

 inline bool jj_2_2(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_2() || jj_done);
 { jj_save(1, xla); }
  }

 inline bool jj_2_3(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_3() || jj_done);
 { jj_save(2, xla); }
  }

 inline bool jj_2_4(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_4() || jj_done);
 { jj_save(3, xla); }
  }

 inline bool jj_2_5(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_5() || jj_done);
 { jj_save(4, xla); }
  }

 inline bool jj_2_6(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_6() || jj_done);
 { jj_save(5, xla); }
  }

 inline bool jj_2_7(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_7() || jj_done);
 { jj_save(6, xla); }
  }

 inline bool jj_2_8(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_8() || jj_done);
 { jj_save(7, xla); }
  }

 inline bool jj_2_9(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_9() || jj_done);
 { jj_save(8, xla); }
  }

 inline bool jj_2_10(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_10() || jj_done);
 { jj_save(9, xla); }
  }

 inline bool jj_2_11(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_11() || jj_done);
 { jj_save(10, xla); }
  }

 inline bool jj_2_12(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_12() || jj_done);
 { jj_save(11, xla); }
  }

 inline bool jj_2_13(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_13() || jj_done);
 { jj_save(12, xla); }
  }

 inline bool jj_2_14(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_14() || jj_done);
 { jj_save(13, xla); }
  }

 inline bool jj_2_15(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_15() || jj_done);
 { jj_save(14, xla); }
  }

 inline bool jj_2_16(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_16() || jj_done);
 { jj_save(15, xla); }
  }

 inline bool jj_2_17(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_17() || jj_done);
 { jj_save(16, xla); }
  }

 inline bool jj_2_18(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_18() || jj_done);
 { jj_save(17, xla); }
  }

 inline bool jj_2_19(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_19() || jj_done);
 { jj_save(18, xla); }
  }

 inline bool jj_2_20(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_20() || jj_done);
 { jj_save(19, xla); }
  }

 inline bool jj_2_21(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_21() || jj_done);
 { jj_save(20, xla); }
  }

 inline bool jj_2_22(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_22() || jj_done);
 { jj_save(21, xla); }
  }

 inline bool jj_2_23(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_23() || jj_done);
 { jj_save(22, xla); }
  }

 inline bool jj_2_24(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_24() || jj_done);
 { jj_save(23, xla); }
  }

 inline bool jj_2_25(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_25() || jj_done);
 { jj_save(24, xla); }
  }

 inline bool jj_2_26(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_26() || jj_done);
 { jj_save(25, xla); }
  }

 inline bool jj_2_27(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_27() || jj_done);
 { jj_save(26, xla); }
  }

 inline bool jj_2_28(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_28() || jj_done);
 { jj_save(27, xla); }
  }

 inline bool jj_2_29(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_29() || jj_done);
 { jj_save(28, xla); }
  }

 inline bool jj_2_30(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_30() || jj_done);
 { jj_save(29, xla); }
  }

 inline bool jj_2_31(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_31() || jj_done);
 { jj_save(30, xla); }
  }

 inline bool jj_2_32(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_32() || jj_done);
 { jj_save(31, xla); }
  }

 inline bool jj_2_33(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_33() || jj_done);
 { jj_save(32, xla); }
  }

 inline bool jj_2_34(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_34() || jj_done);
 { jj_save(33, xla); }
  }

 inline bool jj_2_35(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_35() || jj_done);
 { jj_save(34, xla); }
  }

 inline bool jj_2_36(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_36() || jj_done);
 { jj_save(35, xla); }
  }

 inline bool jj_2_37(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_37() || jj_done);
 { jj_save(36, xla); }
  }

 inline bool jj_2_38(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_38() || jj_done);
 { jj_save(37, xla); }
  }

 inline bool jj_2_39(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_39() || jj_done);
 { jj_save(38, xla); }
  }

 inline bool jj_2_40(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_40() || jj_done);
 { jj_save(39, xla); }
  }

 inline bool jj_2_41(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_41() || jj_done);
 { jj_save(40, xla); }
  }

 inline bool jj_2_42(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_42() || jj_done);
 { jj_save(41, xla); }
  }

 inline bool jj_2_43(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_43() || jj_done);
 { jj_save(42, xla); }
  }

 inline bool jj_2_44(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_44() || jj_done);
 { jj_save(43, xla); }
  }

 inline bool jj_2_45(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_45() || jj_done);
 { jj_save(44, xla); }
  }

 inline bool jj_2_46(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_46() || jj_done);
 { jj_save(45, xla); }
  }

 inline bool jj_2_47(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_47() || jj_done);
 { jj_save(46, xla); }
  }

 inline bool jj_2_48(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_48() || jj_done);
 { jj_save(47, xla); }
  }

 inline bool jj_2_49(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_49() || jj_done);
 { jj_save(48, xla); }
  }

 inline bool jj_2_50(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_50() || jj_done);
 { jj_save(49, xla); }
  }

 inline bool jj_2_51(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_51() || jj_done);
 { jj_save(50, xla); }
  }

 inline bool jj_2_52(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_52() || jj_done);
 { jj_save(51, xla); }
  }

 inline bool jj_2_53(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_53() || jj_done);
 { jj_save(52, xla); }
  }

 inline bool jj_2_54(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_54() || jj_done);
 { jj_save(53, xla); }
  }

 inline bool jj_2_55(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_55() || jj_done);
 { jj_save(54, xla); }
  }

 inline bool jj_2_56(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_56() || jj_done);
 { jj_save(55, xla); }
  }

 inline bool jj_2_57(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_57() || jj_done);
 { jj_save(56, xla); }
  }

 inline bool jj_2_58(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_58() || jj_done);
 { jj_save(57, xla); }
  }

 inline bool jj_2_59(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_59() || jj_done);
 { jj_save(58, xla); }
  }

 inline bool jj_2_60(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_60() || jj_done);
 { jj_save(59, xla); }
  }

 inline bool jj_2_61(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_61() || jj_done);
 { jj_save(60, xla); }
  }

 inline bool jj_2_62(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_62() || jj_done);
 { jj_save(61, xla); }
  }

 inline bool jj_2_63(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_63() || jj_done);
 { jj_save(62, xla); }
  }

 inline bool jj_2_64(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_64() || jj_done);
 { jj_save(63, xla); }
  }

 inline bool jj_2_65(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_65() || jj_done);
 { jj_save(64, xla); }
  }

 inline bool jj_2_66(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_66() || jj_done);
 { jj_save(65, xla); }
  }

 inline bool jj_2_67(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_67() || jj_done);
 { jj_save(66, xla); }
  }

 inline bool jj_2_68(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_68() || jj_done);
 { jj_save(67, xla); }
  }

 inline bool jj_2_69(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_69() || jj_done);
 { jj_save(68, xla); }
  }

 inline bool jj_2_70(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_70() || jj_done);
 { jj_save(69, xla); }
  }

 inline bool jj_2_71(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_71() || jj_done);
 { jj_save(70, xla); }
  }

 inline bool jj_2_72(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_72() || jj_done);
 { jj_save(71, xla); }
  }

 inline bool jj_2_73(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_73() || jj_done);
 { jj_save(72, xla); }
  }

 inline bool jj_2_74(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_74() || jj_done);
 { jj_save(73, xla); }
  }

 inline bool jj_2_75(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_75() || jj_done);
 { jj_save(74, xla); }
  }

 inline bool jj_2_76(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_76() || jj_done);
 { jj_save(75, xla); }
  }

 inline bool jj_2_77(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_77() || jj_done);
 { jj_save(76, xla); }
  }

 inline bool jj_2_78(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_78() || jj_done);
 { jj_save(77, xla); }
  }

 inline bool jj_2_79(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_79() || jj_done);
 { jj_save(78, xla); }
  }

 inline bool jj_2_80(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_80() || jj_done);
 { jj_save(79, xla); }
  }

 inline bool jj_2_81(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_81() || jj_done);
 { jj_save(80, xla); }
  }

 inline bool jj_2_82(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_82() || jj_done);
 { jj_save(81, xla); }
  }

 inline bool jj_2_83(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_83() || jj_done);
 { jj_save(82, xla); }
  }

 inline bool jj_2_84(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_84() || jj_done);
 { jj_save(83, xla); }
  }

 inline bool jj_2_85(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_85() || jj_done);
 { jj_save(84, xla); }
  }

 inline bool jj_2_86(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_86() || jj_done);
 { jj_save(85, xla); }
  }

 inline bool jj_2_87(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_87() || jj_done);
 { jj_save(86, xla); }
  }

 inline bool jj_2_88(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_88() || jj_done);
 { jj_save(87, xla); }
  }

 inline bool jj_2_89(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_89() || jj_done);
 { jj_save(88, xla); }
  }

 inline bool jj_2_90(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_90() || jj_done);
 { jj_save(89, xla); }
  }

 inline bool jj_2_91(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_91() || jj_done);
 { jj_save(90, xla); }
  }

 inline bool jj_2_92(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_92() || jj_done);
 { jj_save(91, xla); }
  }

 inline bool jj_2_93(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_93() || jj_done);
 { jj_save(92, xla); }
  }

 inline bool jj_2_94(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_94() || jj_done);
 { jj_save(93, xla); }
  }

 inline bool jj_2_95(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_95() || jj_done);
 { jj_save(94, xla); }
  }

 inline bool jj_2_96(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_96() || jj_done);
 { jj_save(95, xla); }
  }

 inline bool jj_2_97(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_97() || jj_done);
 { jj_save(96, xla); }
  }

 inline bool jj_2_98(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_98() || jj_done);
 { jj_save(97, xla); }
  }

 inline bool jj_2_99(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_99() || jj_done);
 { jj_save(98, xla); }
  }

 inline bool jj_2_100(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_100() || jj_done);
 { jj_save(99, xla); }
  }

 inline bool jj_2_101(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_101() || jj_done);
 { jj_save(100, xla); }
  }

 inline bool jj_2_102(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_102() || jj_done);
 { jj_save(101, xla); }
  }

 inline bool jj_2_103(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_103() || jj_done);
 { jj_save(102, xla); }
  }

 inline bool jj_2_104(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_104() || jj_done);
 { jj_save(103, xla); }
  }

 inline bool jj_2_105(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_105() || jj_done);
 { jj_save(104, xla); }
  }

 inline bool jj_2_106(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_106() || jj_done);
 { jj_save(105, xla); }
  }

 inline bool jj_2_107(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_107() || jj_done);
 { jj_save(106, xla); }
  }

 inline bool jj_2_108(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_108() || jj_done);
 { jj_save(107, xla); }
  }

 inline bool jj_2_109(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_109() || jj_done);
 { jj_save(108, xla); }
  }

 inline bool jj_2_110(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_110() || jj_done);
 { jj_save(109, xla); }
  }

 inline bool jj_2_111(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_111() || jj_done);
 { jj_save(110, xla); }
  }

 inline bool jj_2_112(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_112() || jj_done);
 { jj_save(111, xla); }
  }

 inline bool jj_2_113(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_113() || jj_done);
 { jj_save(112, xla); }
  }

 inline bool jj_2_114(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_114() || jj_done);
 { jj_save(113, xla); }
  }

 inline bool jj_2_115(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_115() || jj_done);
 { jj_save(114, xla); }
  }

 inline bool jj_2_116(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_116() || jj_done);
 { jj_save(115, xla); }
  }

 inline bool jj_2_117(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_117() || jj_done);
 { jj_save(116, xla); }
  }

 inline bool jj_2_118(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_118() || jj_done);
 { jj_save(117, xla); }
  }

 inline bool jj_2_119(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_119() || jj_done);
 { jj_save(118, xla); }
  }

 inline bool jj_2_120(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_120() || jj_done);
 { jj_save(119, xla); }
  }

 inline bool jj_2_121(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_121() || jj_done);
 { jj_save(120, xla); }
  }

 inline bool jj_2_122(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_122() || jj_done);
 { jj_save(121, xla); }
  }

 inline bool jj_2_123(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_123() || jj_done);
 { jj_save(122, xla); }
  }

 inline bool jj_2_124(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_124() || jj_done);
 { jj_save(123, xla); }
  }

 inline bool jj_2_125(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_125() || jj_done);
 { jj_save(124, xla); }
  }

 inline bool jj_3R_207()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_461()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_543()) return true;
    return false;
  }

 inline bool jj_3R_206()
 {
    if (jj_done) return true;
    if (jj_3R_363()) return true;
    return false;
  }

 inline bool jj_3R_383()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_460()) return false;
    jj_scanpos = xsp;
    if (jj_3R_461()) return true;
    return false;
  }

 inline bool jj_3R_460()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHILE_T)) return true;
    if (jj_3R_83()) return true;
    return false;
  }

 inline bool jj_3R_415()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEMI_T)) return true;
    if (jj_3R_414()) return true;
    return false;
  }

 inline bool jj_3R_424()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_204()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_scan_token(116)) return false;
    jj_scanpos = xsp;
    if (!jj_scan_token(100)) return false;
    jj_scanpos = xsp;
    if (!jj_scan_token(31)) return false;
    jj_scanpos = xsp;
    if (jj_scan_token(101)) return true;
    return false;
  }

 inline bool jj_3R_96()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_204()) jj_scanpos = xsp;
    if (jj_3R_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_206()) jj_scanpos = xsp;
    if (jj_3R_89()) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(27)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_207()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_327()
 {
    if (jj_done) return true;
    if (jj_3R_414()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_415()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_693()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_97()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_3R_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_89()) return true;
    return false;
  }

 inline bool jj_3R_414()
 {
    if (jj_done) return true;
    if (jj_3R_513()) return true;
    return false;
  }

 inline bool jj_3R_444()
 {
    if (jj_done) return true;
    if (jj_scan_token(IS_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_693()) return false;
    jj_scanpos = xsp;
    if (jj_scan_token(145)) return true;
    return false;
  }

 inline bool jj_3_46()
 {
    if (jj_done) return true;
    if (jj_3R_98()) return true;
    return false;
  }

 inline bool jj_3R_572()
 {
    if (jj_done) return true;
    if (jj_3R_625()) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_571()
 {
    if (jj_done) return true;
    if (jj_3R_98()) return true;
    return false;
  }

 inline bool jj_3_45()
 {
    if (jj_done) return true;
    if (jj_3R_97()) return true;
    return false;
  }

 inline bool jj_3_44()
 {
    if (jj_done) return true;
    if (jj_3R_96()) return true;
    return false;
  }

 inline bool jj_3_43()
 {
    if (jj_done) return true;
    if (jj_3R_95()) return true;
    return false;
  }

 inline bool jj_3R_513()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3_42()) return false;
    jj_scanpos = xsp;
    if (!jj_3_43()) return false;
    jj_scanpos = xsp;
    if (!jj_3_44()) return false;
    jj_scanpos = xsp;
    if (!jj_3_45()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_571()) return false;
    jj_scanpos = xsp;
    if (jj_3R_572()) return true;
    return false;
  }

 inline bool jj_3_42()
 {
    if (jj_done) return true;
    if (jj_3R_94()) return true;
    return false;
  }

 inline bool jj_3R_159()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_72()) return true;
    return false;
  }

 inline bool jj_3_41()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASIC_IDENTIFIER)) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_631()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_427()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_426()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_343()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_425()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_426()) return false;
    jj_scanpos = xsp;
    if (jj_3R_427()) return true;
    return false;
  }

 inline bool jj_3R_425()
 {
    if (jj_done) return true;
    if (jj_3R_205()) return true;
    return false;
  }

 inline bool jj_3R_340()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_339()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_41()) jj_scanpos = xsp;
    if (jj_3R_63()) return true;
    xsp = jj_scanpos;
    if (jj_3R_424()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_177()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_338()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_339()) return false;
    jj_scanpos = xsp;
    if (jj_3R_340()) return true;
    return false;
  }

 inline bool jj_3_40()
 {
    if (jj_done) return true;
    if (jj_3R_72()) return true;
    return false;
  }

 inline bool jj_3R_338()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(28)) jj_scanpos = xsp;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_722()
 {
    if (jj_done) return true;
    if (jj_3R_161()) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_535()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_327()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_628()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3_125()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    return false;
  }

 inline bool jj_3R_442()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(77)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_535()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_627()
 {
    if (jj_done) return true;
    if (jj_3R_72()) return true;
    return false;
  }

 inline bool jj_3R_577()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_627()) return false;
    jj_scanpos = xsp;
    if (jj_3R_628()) return true;
    return false;
  }

 inline bool jj_3R_65()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_72()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_159()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_401()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_90()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_scan_token(134)) return false;
    jj_scanpos = xsp;
    if (jj_scan_token(137)) return true;
    return false;
  }

 inline bool jj_3R_547()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_546()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_466()) return true;
    return false;
  }

 inline bool jj_3R_316()
 {
    if (jj_done) return true;
    if (jj_3R_399()) return true;
    return false;
  }

 inline bool jj_3R_561()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_630()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_268()) return true;
    return false;
  }

 inline bool jj_3R_443()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_scan_token(84)) return false;
    jj_scanpos = xsp;
    if (jj_scan_token(52)) return true;
    return false;
  }

 inline bool jj_3R_361()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_443()) jj_scanpos = xsp;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_63()) return true;
    if (jj_3R_442()) return true;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_63()) return true;
    xsp = jj_scanpos;
    if (jj_3R_444()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_629()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSIF_T)) return true;
    if (jj_3R_83()) return true;
    if (jj_scan_token(THEN_T)) return true;
    if (jj_3R_268()) return true;
    return false;
  }

 inline bool jj_3R_362()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_360()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_3R_442()) return true;
    return false;
  }

 inline bool jj_3R_203()
 {
    if (jj_done) return true;
    if (jj_3R_361()) return true;
    return false;
  }

 inline bool jj_3R_202()
 {
    if (jj_done) return true;
    if (jj_3R_360()) return true;
    return false;
  }

 inline bool jj_3R_94()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_202()) return false;
    jj_scanpos = xsp;
    if (jj_3R_203()) return true;
    return false;
  }

 inline bool jj_3R_264()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_125()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_264()) jj_scanpos = xsp;
    if (jj_scan_token(IF_T)) return true;
    if (jj_3R_83()) return true;
    if (jj_scan_token(THEN_T)) return true;
    if (jj_3R_268()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_629()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_630()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(IF_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_631()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_205()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_362()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_493()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_61()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_561()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_400()
 {
    if (jj_done) return true;
    Token * xsp;
    if (jj_3R_493()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_493()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_171()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASIC_IDENTIFIER)) return true;
    return false;
  }

 inline bool jj_3R_170()
 {
    if (jj_done) return true;
    if (jj_scan_token(EXTENDED_CHARACTER)) return true;
    return false;
  }

 inline bool jj_3R_74()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_170()) return false;
    jj_scanpos = xsp;
    if (jj_3R_171()) return true;
    return false;
  }

 inline bool jj_3R_560()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_559()) return true;
    return false;
  }

 inline bool jj_3R_399()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_scan_token(53)) return false;
    jj_scanpos = xsp;
    if (jj_scan_token(75)) return true;
    return false;
  }

 inline bool jj_3R_466()
 {
    if (jj_done) return true;
    if (jj_3R_463()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_90()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_546()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_465()
 {
    if (jj_done) return true;
    if (jj_3R_420()) return true;
    return false;
  }

 inline bool jj_3R_317()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_90()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_scan_token(134)) return false;
    jj_scanpos = xsp;
    if (jj_scan_token(137)) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_401()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_68()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_491()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_150()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_3R_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_scan_token(FORCE_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_316()) jj_scanpos = xsp;
    if (jj_3R_317()) return true;
    return false;
  }

 inline bool jj_3R_398()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_492()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_124()
 {
    if (jj_done) return true;
    if (jj_3R_114()) return true;
    return false;
  }

 inline bool jj_3R_385()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_3R_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_465()) jj_scanpos = xsp;
    if (jj_3R_466()) return true;
    return false;
  }

 inline bool jj_3R_492()
 {
    if (jj_done) return true;
    if (jj_3R_559()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_560()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_687()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_620()
 {
    if (jj_done) return true;
    if (jj_3R_369()) return true;
    return false;
  }

 inline bool jj_3R_686()
 {
    if (jj_done) return true;
    if (jj_3R_114()) return true;
    return false;
  }

 inline bool jj_3R_663()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_686()) return false;
    jj_scanpos = xsp;
    if (jj_3R_687()) return true;
    return false;
  }

 inline bool jj_3_121()
 {
    if (jj_done) return true;
    if (jj_3R_148()) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_619()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_559()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_619()) return false;
    jj_scanpos = xsp;
    if (jj_3R_620()) return true;
    return false;
  }

 inline bool jj_3R_73()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_169()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_587()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3_123()
 {
    if (jj_done) return true;
    if (jj_3R_150()) return true;
    return false;
  }

 inline bool jj_3R_542()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_3R_663()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_317()) return true;
    return false;
  }

 inline bool jj_3R_314()
 {
    if (jj_done) return true;
    if (jj_3R_399()) return true;
    return false;
  }

 inline bool jj_3R_312()
 {
    if (jj_done) return true;
    if (jj_3R_148()) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_315()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    if (jj_3R_400()) return true;
    return false;
  }

 inline bool jj_3R_162()
 {
    if (jj_done) return true;
    if (jj_3R_327()) return true;
    return false;
  }

 inline bool jj_3R_464()
 {
    if (jj_done) return true;
    if (jj_3R_545()) return true;
    return false;
  }

 inline bool jj_3_39()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_280()
 {
    if (jj_done) return true;
    if (jj_3R_385()) return true;
    return false;
  }

 inline bool jj_3R_132()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_279()) return false;
    jj_scanpos = xsp;
    if (jj_3R_280()) return true;
    return false;
  }

 inline bool jj_3R_279()
 {
    if (jj_done) return true;
    if (jj_3R_150()) return true;
    return false;
  }

 inline bool jj_3R_69()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_162()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_336()
 {
    if (jj_done) return true;
    if (jj_scan_token(IF_T)) return true;
    return false;
  }

 inline bool jj_3R_462()
 {
    if (jj_done) return true;
    if (jj_3R_420()) return true;
    return false;
  }

 inline bool jj_3R_174()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_335()) return false;
    jj_scanpos = xsp;
    if (jj_3R_336()) return true;
    return false;
  }

 inline bool jj_3R_335()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    return false;
  }

 inline bool jj_3R_149()
 {
    if (jj_done) return true;
    if (jj_3R_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_scan_token(FORCE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_314()) jj_scanpos = xsp;
    if (jj_3R_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_315()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_545()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_61()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_587()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_122()
 {
    if (jj_done) return true;
    if (jj_3R_149()) return true;
    return false;
  }

 inline bool jj_3R_78()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_174()) return true;
    return false;
  }

 inline bool jj_3R_384()
 {
    if (jj_done) return true;
    if (jj_3R_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_462()) jj_scanpos = xsp;
    if (jj_3R_463()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_61()) return true;
    xsp = jj_scanpos;
    if (jj_3R_464()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_110()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_236()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_278()
 {
    if (jj_done) return true;
    if (jj_3R_384()) return true;
    return false;
  }

 inline bool jj_3R_131()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_277()) return false;
    jj_scanpos = xsp;
    if (jj_3R_278()) return true;
    return false;
  }

 inline bool jj_3R_277()
 {
    if (jj_done) return true;
    if (jj_3R_149()) return true;
    return false;
  }

 inline bool jj_3R_570()
 {
    if (jj_done) return true;
    if (jj_scan_token(AT_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_313()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_120()
 {
    if (jj_done) return true;
    if (jj_3R_147()) return true;
    return false;
  }

 inline bool jj_3R_481()
 {
    if (jj_done) return true;
    if (jj_3R_549()) return true;
    return false;
  }

 inline bool jj_3R_147()
 {
    if (jj_done) return true;
    if (jj_3R_148()) return true;
    if (jj_scan_token(DOT_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_312()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_624()
 {
    if (jj_done) return true;
    if (jj_3R_147()) return true;
    return false;
  }

 inline bool jj_3R_148()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_313()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_592()
 {
    if (jj_done) return true;
    if (jj_3R_399()) return true;
    return false;
  }

 inline bool jj_3R_160()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_326()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_647()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEG_T)) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_142()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_588()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_623()
 {
    if (jj_done) return true;
    Token * xsp;
    if (jj_3R_647()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_647()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_119()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_147()) return true;
    return false;
  }

 inline bool jj_3R_569()
 {
    if (jj_done) return true;
    if (jj_3R_623()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_624()) jj_scanpos = xsp;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_66()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_160()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_622()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_582()
 {
    if (jj_done) return true;
    if (jj_3R_327()) return true;
    return false;
  }

 inline bool jj_3R_621()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_147()) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_568()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_621()) return false;
    jj_scanpos = xsp;
    if (jj_3R_622()) return true;
    return false;
  }

 inline bool jj_3R_413()
 {
    if (jj_done) return true;
    if (jj_scan_token(INTEGER)) return true;
    return false;
  }

 inline bool jj_3R_326()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_412()) return false;
    jj_scanpos = xsp;
    if (jj_3R_413()) return true;
    return false;
  }

 inline bool jj_3R_412()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_512()
 {
    if (jj_done) return true;
    if (jj_3R_570()) return true;
    return false;
  }

 inline bool jj_3R_511()
 {
    if (jj_done) return true;
    if (jj_3R_569()) return true;
    return false;
  }

 inline bool jj_3R_411()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_510()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_511()) return false;
    jj_scanpos = xsp;
    if (jj_3R_512()) return true;
    return false;
  }

 inline bool jj_3R_510()
 {
    if (jj_done) return true;
    if (jj_3R_568()) return true;
    return false;
  }

 inline bool jj_3R_189()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_509()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_508()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_410()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_507()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_508()) return false;
    jj_scanpos = xsp;
    if (jj_3R_509()) return true;
    return false;
  }

 inline bool jj_3R_507()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_659()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_161()) return true;
    return false;
  }

 inline bool jj_3R_591()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_549()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_591()) jj_scanpos = xsp;
    if (jj_scan_token(IS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_592()) jj_scanpos = xsp;
    if (jj_3R_593()) return true;
    return false;
  }

 inline bool jj_3_114()
 {
    if (jj_done) return true;
    if (jj_3R_144()) return true;
    return false;
  }

 inline bool jj_3_118()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_593()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_323()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLSL_T)) return true;
    if (jj_3R_410()) return true;
    if (jj_3R_411()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_89()) return true;
    if (jj_scan_token(RSRS_T)) return true;
    return false;
  }

 inline bool jj_3_117()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_284()
 {
    if (jj_done) return true;
    if (jj_3R_137()) return true;
    return false;
  }

 inline bool jj_3R_467()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOUBLEMULT_T)) return true;
    if (jj_3R_386()) return true;
    return false;
  }

 inline bool jj_3R_392()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_3R_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_89()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_481()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_116()
 {
    if (jj_done) return true;
    if (jj_3R_146()) return true;
    return false;
  }

 inline bool jj_3R_273()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_295()
 {
    if (jj_done) return true;
    if (jj_3R_320()) return true;
    if (jj_3R_386()) return true;
    return false;
  }

 inline bool jj_3R_294()
 {
    if (jj_done) return true;
    if (jj_scan_token(QQ_T)) return true;
    if (jj_3R_386()) return true;
    return false;
  }

 inline bool jj_3R_293()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOT_T)) return true;
    if (jj_3R_386()) return true;
    return false;
  }

 inline bool jj_3R_292()
 {
    if (jj_done) return true;
    if (jj_scan_token(ABS_T)) return true;
    if (jj_3R_386()) return true;
    return false;
  }

 inline bool jj_3R_141()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_291()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_292()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_293()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_294()) return false;
    jj_scanpos = xsp;
    if (jj_3R_295()) return true;
    return false;
  }

 inline bool jj_3R_291()
 {
    if (jj_done) return true;
    if (jj_3R_386()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_467()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_296()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_251()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_102()) return true;
    return false;
  }

 inline bool jj_3R_246()
 {
    if (jj_done) return true;
    if (jj_3R_137()) return true;
    return false;
  }

 inline bool jj_3R_152()
 {
    if (jj_done) return true;
    if (jj_3R_320()) return true;
    if (jj_3R_151()) return true;
    return false;
  }

 inline bool jj_3_115()
 {
    if (jj_done) return true;
    if (jj_3R_145()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_409()
 {
    if (jj_done) return true;
    if (jj_scan_token(OR_T)) return true;
    return false;
  }

 inline bool jj_3R_408()
 {
    if (jj_done) return true;
    if (jj_scan_token(XOR_T)) return true;
    return false;
  }

 inline bool jj_3R_407()
 {
    if (jj_done) return true;
    if (jj_scan_token(XNOR_T)) return true;
    return false;
  }

 inline bool jj_3R_406()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOR_T)) return true;
    return false;
  }

 inline bool jj_3R_405()
 {
    if (jj_done) return true;
    if (jj_scan_token(NAND_T)) return true;
    return false;
  }

 inline bool jj_3R_144()
 {
    if (jj_done) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_296()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_648()
 {
    if (jj_done) return true;
    if (jj_3R_137()) return true;
    return false;
  }

 inline bool jj_3R_188()
 {
    if (jj_done) return true;
    if (jj_3R_349()) return true;
    return false;
  }

 inline bool jj_3R_320()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_404()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_405()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_406()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_407()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_408()) return false;
    jj_scanpos = xsp;
    if (jj_3R_409()) return true;
    return false;
  }

 inline bool jj_3R_404()
 {
    if (jj_done) return true;
    if (jj_scan_token(AND_T)) return true;
    return false;
  }

 inline bool jj_3R_61()
 {
    if (jj_done) return true;
    if (jj_3R_151()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_152()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_274()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_83()) return true;
    return false;
  }

 inline bool jj_3R_272()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_129()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_272()) jj_scanpos = xsp;
    if (jj_scan_token(EXIT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_273()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_274()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_136()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_327()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_116()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_102()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_251()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_218()
 {
    if (jj_done) return true;
    if (jj_3R_369()) return true;
    return false;
  }

 inline bool jj_3R_137()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_169()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_217()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_102()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_217()) return false;
    jj_scanpos = xsp;
    if (jj_3R_218()) return true;
    return false;
  }

 inline bool jj_3R_685()
 {
    if (jj_done) return true;
    if (jj_3R_369()) return true;
    return false;
  }

 inline bool jj_3R_684()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_661()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_684()) return false;
    jj_scanpos = xsp;
    if (jj_3R_685()) return true;
    return false;
  }

 inline bool jj_3R_135()
 {
    if (jj_done) return true;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_63()) return true;
    if (jj_3R_245()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_284()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_645()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_644()) return true;
    return false;
  }

 inline bool jj_3_38()
 {
    if (jj_done) return true;
    if (jj_3R_93()) return true;
    return false;
  }

 inline bool jj_3_37()
 {
    if (jj_done) return true;
    if (jj_3R_92()) return true;
    return false;
  }

 inline bool jj_3R_95()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_648()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_435()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_91()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_63()) return true;
    if (jj_3R_245()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_246()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_488()
 {
    if (jj_done) return true;
    if (jj_3R_554()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_555()) return true;
    return false;
  }

 inline bool jj_3R_438()
 {
    if (jj_done) return true;
    if (jj_3R_348()) return true;
    return false;
  }

 inline bool jj_3R_437()
 {
    if (jj_done) return true;
    if (jj_3R_346()) return true;
    return false;
  }

 inline bool jj_3R_662()
 {
    if (jj_done) return true;
    if (jj_3R_245()) return true;
    return false;
  }

 inline bool jj_3R_349()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_436()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_437()) return false;
    jj_scanpos = xsp;
    if (jj_3R_438()) return true;
    return false;
  }

 inline bool jj_3R_436()
 {
    if (jj_done) return true;
    if (jj_3R_347()) return true;
    return false;
  }

 inline bool jj_3R_597()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_596()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_595()
 {
    if (jj_done) return true;
    if (jj_3R_644()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_645()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_554()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_595()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_596()) return false;
    jj_scanpos = xsp;
    if (jj_3R_597()) return true;
    return false;
  }

 inline bool jj_3R_87()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONTEXT_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_188()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(32)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_189()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_683()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_263()
 {
    if (jj_done) return true;
    if (jj_3R_382()) return true;
    return false;
  }

 inline bool jj_3R_644()
 {
    if (jj_done) return true;
    if (jj_3R_661()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_662()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_348()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONTEXT_T)) return true;
    if (jj_3R_74()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_435()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_692()
 {
    if (jj_done) return true;
    if (jj_3R_700()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_682()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_692()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_720()
 {
    if (jj_done) return true;
    if (jj_3R_347()) return true;
    return false;
  }

 inline bool jj_3R_719()
 {
    if (jj_done) return true;
    if (jj_3R_395()) return true;
    return false;
  }

 inline bool jj_3_36()
 {
    if (jj_done) return true;
    if (jj_3R_91()) return true;
    return false;
  }

 inline bool jj_3_34()
 {
    if (jj_done) return true;
    if (jj_3R_67()) return true;
    return false;
  }

 inline bool jj_3R_718()
 {
    if (jj_done) return true;
    if (jj_3R_364()) return true;
    return false;
  }

 inline bool jj_3R_700()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_718()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_719()) return false;
    jj_scanpos = xsp;
    if (jj_3R_720()) return true;
    return false;
  }

 inline bool jj_3_35()
 {
    if (jj_done) return true;
    if (jj_3R_68()) return true;
    return false;
  }

 inline bool jj_3R_558()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_557()) return true;
    return false;
  }

 inline bool jj_3R_681()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_262()
 {
    if (jj_done) return true;
    if (jj_3R_381()) return true;
    return false;
  }

 inline bool jj_3R_660()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_3R_682()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_683()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_691()
 {
    if (jj_done) return true;
    if (jj_3R_699()) return true;
    return false;
  }

 inline bool jj_3_112()
 {
    if (jj_done) return true;
    if (jj_3R_67()) return true;
    return false;
  }

 inline bool jj_3R_680()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_691()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_717()
 {
    if (jj_done) return true;
    if (jj_3R_398()) return true;
    return false;
  }

 inline bool jj_3_113()
 {
    if (jj_done) return true;
    if (jj_3R_68()) return true;
    return false;
  }

 inline bool jj_3R_716()
 {
    if (jj_done) return true;
    if (jj_3R_347()) return true;
    return false;
  }

 inline bool jj_3R_715()
 {
    if (jj_done) return true;
    if (jj_3R_395()) return true;
    return false;
  }

 inline bool jj_3R_618()
 {
    if (jj_done) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_714()
 {
    if (jj_done) return true;
    if (jj_3R_67()) return true;
    return false;
  }

 inline bool jj_3R_713()
 {
    if (jj_done) return true;
    if (jj_3R_393()) return true;
    return false;
  }

 inline bool jj_3R_712()
 {
    if (jj_done) return true;
    if (jj_3R_392()) return true;
    return false;
  }

 inline bool jj_3R_711()
 {
    if (jj_done) return true;
    if (jj_3R_391()) return true;
    return false;
  }

 inline bool jj_3R_366()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_710()
 {
    if (jj_done) return true;
    if (jj_3R_389()) return true;
    return false;
  }

 inline bool jj_3R_709()
 {
    if (jj_done) return true;
    if (jj_3R_388()) return true;
    return false;
  }

 inline bool jj_3R_708()
 {
    if (jj_done) return true;
    if (jj_3R_387()) return true;
    return false;
  }

 inline bool jj_3R_707()
 {
    if (jj_done) return true;
    if (jj_3R_540()) return true;
    return false;
  }

 inline bool jj_3R_706()
 {
    if (jj_done) return true;
    if (jj_3R_98()) return true;
    return false;
  }

 inline bool jj_3R_699()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_706()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_707()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_708()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_709()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_710()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_711()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_712()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_713()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_714()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_715()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_716()) return false;
    jj_scanpos = xsp;
    if (!jj_3_113()) return false;
    jj_scanpos = xsp;
    if (jj_3R_717()) return true;
    return false;
  }

 inline bool jj_3R_261()
 {
    if (jj_done) return true;
    if (jj_3R_380()) return true;
    return false;
  }

 inline bool jj_3R_491()
 {
    if (jj_done) return true;
    if (jj_3R_557()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_558()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_557()
 {
    if (jj_done) return true;
    if (jj_3R_555()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_618()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_544()
 {
    if (jj_done) return true;
    if (jj_scan_token(AFTER_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_626()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_143()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    if (jj_3R_680()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_681()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_614()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    return false;
  }

 inline bool jj_3R_519()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_463()) return true;
    return false;
  }

 inline bool jj_3R_613()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    return false;
  }

 inline bool jj_3R_612()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNITS_T)) return true;
    return false;
  }

 inline bool jj_3R_611()
 {
    if (jj_done) return true;
    if (jj_scan_token(LITERAL_T)) return true;
    return false;
  }

 inline bool jj_3R_610()
 {
    if (jj_done) return true;
    if (jj_scan_token(LABEL_T)) return true;
    return false;
  }

 inline bool jj_3R_609()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    return false;
  }

 inline bool jj_3R_608()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_607()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_606()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_605()
 {
    if (jj_done) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_604()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_603()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    return false;
  }

 inline bool jj_3R_602()
 {
    if (jj_done) return true;
    if (jj_scan_token(FUNCTION_T)) return true;
    return false;
  }

 inline bool jj_3R_601()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    return false;
  }

 inline bool jj_3R_463()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_544()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_600()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    return false;
  }

 inline bool jj_3R_599()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARCHITECTURE_T)) return true;
    return false;
  }

 inline bool jj_3R_598()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    return false;
  }

 inline bool jj_3R_555()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_598()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_599()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_600()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_601()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_602()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_603()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_604()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_605()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_606()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_607()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_608()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_609()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_610()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_611()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_612()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_613()) return false;
    jj_scanpos = xsp;
    if (jj_3R_614()) return true;
    return false;
  }

 inline bool jj_3R_422()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNAFFECTED_T)) return true;
    return false;
  }

 inline bool jj_3R_576()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    return false;
  }

 inline bool jj_3R_575()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_332()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_421()) return false;
    jj_scanpos = xsp;
    if (jj_3R_422()) return true;
    return false;
  }

 inline bool jj_3R_421()
 {
    if (jj_done) return true;
    if (jj_3R_463()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_519()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_574()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    if (jj_3R_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_626()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_521()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_574()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_575()) return false;
    jj_scanpos = xsp;
    if (jj_3R_576()) return true;
    return false;
  }

 inline bool jj_3R_260()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_122()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_260()) jj_scanpos = xsp;
    if (jj_scan_token(WAIT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_261()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_262()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_263()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_705()
 {
    if (jj_done) return true;
    if (jj_3R_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_89()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_33()
 {
    if (jj_done) return true;
    if (jj_3R_90()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3_32()
 {
    if (jj_done) return true;
    if (jj_3R_89()) return true;
    return false;
  }

 inline bool jj_3R_723()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_722()) return true;
    return false;
  }

 inline bool jj_3_31()
 {
    if (jj_done) return true;
    if (jj_3R_88()) return true;
    return false;
  }

 inline bool jj_3R_376()
 {
    if (jj_done) return true;
    if (jj_3R_90()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_247()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_376()) jj_scanpos = xsp;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_480()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_168()
 {
    if (jj_done) return true;
    if (jj_3R_89()) return true;
    return false;
  }

 inline bool jj_3R_391()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(101)) jj_scanpos = xsp;
    if (jj_scan_token(VARIABLE_T)) return true;
    if (jj_3R_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_89()) return true;
    xsp = jj_scanpos;
    if (jj_3R_480()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_167()
 {
    if (jj_done) return true;
    if (jj_3R_88()) return true;
    return false;
  }

 inline bool jj_3R_72()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_167()) return false;
    jj_scanpos = xsp;
    if (jj_3R_168()) return true;
    return false;
  }

 inline bool jj_3R_458()
 {
    if (jj_done) return true;
    if (jj_3R_542()) return true;
    return false;
  }

 inline bool jj_3R_489()
 {
    if (jj_done) return true;
    if (jj_3R_556()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3_111()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_378()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_457()) return false;
    jj_scanpos = xsp;
    if (jj_3R_458()) return true;
    return false;
  }

 inline bool jj_3R_457()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_111()) jj_scanpos = xsp;
    if (jj_3R_119()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_490()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_434()) return true;
    return false;
  }

 inline bool jj_3R_397()
 {
    if (jj_done) return true;
    if (jj_scan_token(DISCONNECT_T)) return true;
    if (jj_3R_489()) return true;
    if (jj_scan_token(AFTER_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_166()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOWNTO_T)) return true;
    return false;
  }

 inline bool jj_3R_477()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_165()
 {
    if (jj_done) return true;
    if (jj_scan_token(TO_T)) return true;
    return false;
  }

 inline bool jj_3R_71()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_165()) return false;
    jj_scanpos = xsp;
    if (jj_3R_166()) return true;
    return false;
  }

 inline bool jj_3R_581()
 {
    if (jj_done) return true;
    if (jj_3R_321()) return true;
    return false;
  }

 inline bool jj_3R_580()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_536()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_580()) return false;
    jj_scanpos = xsp;
    if (jj_3R_581()) return true;
    return false;
  }

 inline bool jj_3R_347()
 {
    if (jj_done) return true;
    if (jj_scan_token(USE_T)) return true;
    if (jj_3R_434()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_490()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_106()
 {
    if (jj_done) return true;
    if (jj_3R_139()) return true;
    return false;
  }

 inline bool jj_3R_573()
 {
    if (jj_done) return true;
    if (jj_scan_token(REJECT_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_518()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_573()) jj_scanpos = xsp;
    if (jj_scan_token(INERTIAL_T)) return true;
    return false;
  }

 inline bool jj_3R_704()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_722()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_723()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_89()) return true;
    return false;
  }

 inline bool jj_3R_517()
 {
    if (jj_done) return true;
    if (jj_scan_token(TRANSPORT_T)) return true;
    return false;
  }

 inline bool jj_3_30()
 {
    if (jj_done) return true;
    if (jj_3R_87()) return true;
    return false;
  }

 inline bool jj_3R_420()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_517()) return false;
    jj_scanpos = xsp;
    if (jj_3R_518()) return true;
    return false;
  }

 inline bool jj_3R_161()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_193()
 {
    if (jj_done) return true;
    if (jj_3R_139()) return true;
    return false;
  }

 inline bool jj_3R_643()
 {
    if (jj_done) return true;
    if (jj_3R_660()) return true;
    return false;
  }

 inline bool jj_3_29()
 {
    if (jj_done) return true;
    if (jj_3R_65()) return true;
    return false;
  }

 inline bool jj_3R_187()
 {
    if (jj_done) return true;
    if (jj_3R_348()) return true;
    return false;
  }

 inline bool jj_3_28()
 {
    if (jj_done) return true;
    if (jj_3R_86()) return true;
    return false;
  }

 inline bool jj_3_110()
 {
    if (jj_done) return true;
    if (jj_3R_143()) return true;
    return false;
  }

 inline bool jj_3R_642()
 {
    if (jj_done) return true;
    if (jj_3R_659()) return true;
    return false;
  }

 inline bool jj_3R_186()
 {
    if (jj_done) return true;
    if (jj_3R_87()) return true;
    return false;
  }

 inline bool jj_3R_641()
 {
    if (jj_done) return true;
    if (jj_3R_658()) return true;
    return false;
  }

 inline bool jj_3R_185()
 {
    if (jj_done) return true;
    if (jj_3R_347()) return true;
    return false;
  }

 inline bool jj_3R_640()
 {
    if (jj_done) return true;
    if (jj_3R_657()) return true;
    return false;
  }

 inline bool jj_3R_184()
 {
    if (jj_done) return true;
    if (jj_3R_346()) return true;
    return false;
  }

 inline bool jj_3R_85()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_184()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_185()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_186()) return false;
    jj_scanpos = xsp;
    if (jj_3R_187()) return true;
    return false;
  }

 inline bool jj_3R_639()
 {
    if (jj_done) return true;
    if (jj_3R_656()) return true;
    return false;
  }

 inline bool jj_3R_588()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_639()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_640()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_641()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_642()) return false;
    jj_scanpos = xsp;
    if (!jj_3_110()) return false;
    jj_scanpos = xsp;
    if (jj_3R_643()) return true;
    return false;
  }

 inline bool jj_3R_286()
 {
    if (jj_done) return true;
    if (jj_3R_65()) return true;
    return false;
  }

 inline bool jj_3R_192()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_476()
 {
    if (jj_done) return true;
    if (jj_3R_547()) return true;
    return false;
  }

 inline bool jj_3R_139()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_285()) return false;
    jj_scanpos = xsp;
    if (jj_3R_286()) return true;
    return false;
  }

 inline bool jj_3R_285()
 {
    if (jj_done) return true;
    if (jj_3R_86()) return true;
    return false;
  }

 inline bool jj_3_109()
 {
    if (jj_done) return true;
    if (jj_3R_142()) return true;
    return false;
  }

 inline bool jj_3R_387()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3_109()) return false;
    jj_scanpos = xsp;
    if (jj_3R_476()) return true;
    return false;
  }

 inline bool jj_3_27()
 {
    if (jj_done) return true;
    if (jj_3R_85()) return true;
    return false;
  }

 inline bool jj_3R_111()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_108()
 {
    if (jj_done) return true;
    if (jj_3R_140()) return true;
    if (jj_3R_141()) return true;
    return false;
  }

 inline bool jj_3R_703()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_3R_65()) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_89()) return true;
    return false;
  }

 inline bool jj_3R_382()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_134()
 {
    if (jj_done) return true;
    if (jj_3R_141()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_108()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_256()
 {
    if (jj_done) return true;
    if (jj_3R_114()) return true;
    return false;
  }

 inline bool jj_3R_389()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    if (jj_3R_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_89()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_477()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_119()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_255()) return false;
    jj_scanpos = xsp;
    if (jj_3R_256()) return true;
    return false;
  }

 inline bool jj_3_107()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_255()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3_26()
 {
    if (jj_done) return true;
    if (jj_3R_84()) return true;
    return false;
  }

 inline bool jj_3R_228()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_227()
 {
    if (jj_done) return true;
    if (jj_3R_321()) return true;
    return false;
  }

 inline bool jj_3R_226()
 {
    if (jj_done) return true;
    if (jj_3R_369()) return true;
    return false;
  }

 inline bool jj_3R_396()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_181()) return true;
    if (jj_3R_344()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_579()
 {
    if (jj_done) return true;
    if (jj_3R_345()) return true;
    return false;
  }

 inline bool jj_3R_105()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_225()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_226()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_227()) return false;
    jj_scanpos = xsp;
    if (jj_3R_228()) return true;
    return false;
  }

 inline bool jj_3R_225()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3_105()
 {
    if (jj_done) return true;
    if (jj_3R_138()) return true;
    return false;
  }

 inline bool jj_3R_578()
 {
    if (jj_done) return true;
    if (jj_3R_84()) return true;
    return false;
  }

 inline bool jj_3R_523()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_578()) return false;
    jj_scanpos = xsp;
    if (jj_3R_579()) return true;
    return false;
  }

 inline bool jj_3R_89()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_105()) jj_scanpos = xsp;
    if (jj_3R_192()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_192()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_193()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_138()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_388()
 {
    if (jj_done) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_89()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_637()
 {
    if (jj_done) return true;
    if (jj_3R_117()) return true;
    return false;
  }

 inline bool jj_3_25()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_83()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    return false;
  }

 inline bool jj_3R_584()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_637()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_367()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_334()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_83()) return true;
    return false;
  }

 inline bool jj_3R_333()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_83()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_332()) return true;
    return false;
  }

 inline bool jj_3R_539()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_582()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_173()
 {
    if (jj_done) return true;
    if (jj_3R_332()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_333()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_334()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_75()
 {
    if (jj_done) return true;
    if (jj_3R_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_3R_172()) return true;
    if (jj_3R_173()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_538()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_scan_token(84)) return false;
    jj_scanpos = xsp;
    if (jj_scan_token(52)) return true;
    return false;
  }

 inline bool jj_3_104()
 {
    if (jj_done) return true;
    if (jj_3R_137()) return true;
    return false;
  }

 inline bool jj_3R_451()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_538()) jj_scanpos = xsp;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_536()) return true;
    xsp = jj_scanpos;
    if (jj_3R_539()) jj_scanpos = xsp;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_161()) return true;
    return false;
  }

 inline bool jj_3_103()
 {
    if (jj_done) return true;
    if (jj_3R_136()) return true;
    return false;
  }

 inline bool jj_3R_537()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_327()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_381()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNTIL_T)) return true;
    if (jj_3R_83()) return true;
    return false;
  }

 inline bool jj_3R_83()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3_24()
 {
    if (jj_done) return true;
    if (jj_3R_82()) return true;
    return false;
  }

 inline bool jj_3_23()
 {
    if (jj_done) return true;
    if (jj_3R_81()) return true;
    return false;
  }

 inline bool jj_3R_364()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_450()) return false;
    jj_scanpos = xsp;
    if (jj_3R_451()) return true;
    return false;
  }

 inline bool jj_3R_450()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    if (jj_3R_536()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_537()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3_103()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3_104()) jj_scanpos = xsp;
    if (jj_3R_442()) return true;
    return false;
  }

 inline bool jj_3_22()
 {
    if (jj_done) return true;
    if (jj_3R_80()) return true;
    return false;
  }

 inline bool jj_3R_79()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_21()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_79()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(ASSERT_T)) return true;
    return false;
  }

 inline bool jj_3R_638()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_scan_token(46)) return false;
    jj_scanpos = xsp;
    if (jj_scan_token(80)) return true;
    return false;
  }

 inline bool jj_3R_636()
 {
    if (jj_done) return true;
    if (jj_3R_655()) return true;
    return false;
  }

 inline bool jj_3R_583()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_636()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_77()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_19()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_77()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    return false;
  }

 inline bool jj_3_101()
 {
    if (jj_done) return true;
    if (jj_3R_67()) return true;
    return false;
  }

 inline bool jj_3R_675()
 {
    if (jj_done) return true;
    if (jj_3R_398()) return true;
    return false;
  }

 inline bool jj_3R_76()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_18()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_76()) jj_scanpos = xsp;
    if (jj_scan_token(BLOCK_T)) return true;
    return false;
  }

 inline bool jj_3R_674()
 {
    if (jj_done) return true;
    if (jj_3R_347()) return true;
    return false;
  }

 inline bool jj_3_102()
 {
    if (jj_done) return true;
    if (jj_3R_68()) return true;
    return false;
  }

 inline bool jj_3R_673()
 {
    if (jj_done) return true;
    if (jj_3R_395()) return true;
    return false;
  }

 inline bool jj_3_20()
 {
    if (jj_done) return true;
    if (jj_3R_78()) return true;
    return false;
  }

 inline bool jj_3R_672()
 {
    if (jj_done) return true;
    if (jj_3R_67()) return true;
    return false;
  }

 inline bool jj_3R_671()
 {
    if (jj_done) return true;
    if (jj_3R_393()) return true;
    return false;
  }

 inline bool jj_3R_670()
 {
    if (jj_done) return true;
    if (jj_3R_392()) return true;
    return false;
  }

 inline bool jj_3R_669()
 {
    if (jj_done) return true;
    if (jj_3R_391()) return true;
    return false;
  }

 inline bool jj_3R_586()
 {
    if (jj_done) return true;
    if (jj_3R_536()) return true;
    return false;
  }

 inline bool jj_3R_668()
 {
    if (jj_done) return true;
    if (jj_3R_389()) return true;
    return false;
  }

 inline bool jj_3_17()
 {
    if (jj_done) return true;
    if (jj_3R_75()) return true;
    return false;
  }

 inline bool jj_3R_667()
 {
    if (jj_done) return true;
    if (jj_3R_388()) return true;
    return false;
  }

 inline bool jj_3R_666()
 {
    if (jj_done) return true;
    if (jj_3R_540()) return true;
    return false;
  }

 inline bool jj_3R_665()
 {
    if (jj_done) return true;
    if (jj_3R_387()) return true;
    return false;
  }

 inline bool jj_3R_664()
 {
    if (jj_done) return true;
    if (jj_3R_98()) return true;
    return false;
  }

 inline bool jj_3R_655()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_664()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_665()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_666()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_667()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_668()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_669()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_670()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_671()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_672()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_673()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_674()) return false;
    jj_scanpos = xsp;
    if (!jj_3_102()) return false;
    jj_scanpos = xsp;
    if (jj_3R_675()) return true;
    return false;
  }

 inline bool jj_3R_176()
 {
    if (jj_done) return true;
    if (jj_3R_337()) return true;
    return false;
  }

 inline bool jj_3R_479()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_175()
 {
    if (jj_done) return true;
    if (jj_3R_75()) return true;
    return false;
  }

 inline bool jj_3_16()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_100()
 {
    if (jj_done) return true;
    if (jj_3R_135()) return true;
    return false;
  }

 inline bool jj_3R_365()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_452()) return false;
    jj_scanpos = xsp;
    if (jj_scan_token(137)) return true;
    return false;
  }

 inline bool jj_3R_452()
 {
    if (jj_done) return true;
    if (jj_3R_540()) return true;
    return false;
  }

 inline bool jj_3R_80()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_16()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (!jj_3R_175()) return false;
    jj_scanpos = xsp;
    if (jj_3R_176()) return true;
    return false;
  }

 inline bool jj_3_15()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_82()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_15()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_3R_180()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_209()
 {
    if (jj_done) return true;
    if (jj_3R_364()) return true;
    if (jj_3R_365()) return true;
    return false;
  }

 inline bool jj_3_14()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_13()
 {
    if (jj_done) return true;
    if (jj_3R_73()) return true;
    return false;
  }

 inline bool jj_3R_585()
 {
    if (jj_done) return true;
    if (jj_3R_638()) return true;
    return false;
  }

 inline bool jj_3R_208()
 {
    if (jj_done) return true;
    if (jj_3R_135()) return true;
    return false;
  }

 inline bool jj_3R_98()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_208()) return false;
    jj_scanpos = xsp;
    if (jj_3R_209()) return true;
    return false;
  }

 inline bool jj_3_99()
 {
    if (jj_done) return true;
    if (jj_3R_133()) return true;
    if (jj_3R_134()) return true;
    return false;
  }

 inline bool jj_3R_92()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_14()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_3R_196()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_679()
 {
    if (jj_done) return true;
    if (jj_3R_690()) return true;
    return false;
  }

 inline bool jj_3R_678()
 {
    if (jj_done) return true;
    if (jj_3R_689()) return true;
    return false;
  }

 inline bool jj_3R_657()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_678()) return false;
    jj_scanpos = xsp;
    if (jj_3R_679()) return true;
    return false;
  }

 inline bool jj_3R_179()
 {
    if (jj_done) return true;
    if (jj_3R_341()) return true;
    return false;
  }

 inline bool jj_3R_178()
 {
    if (jj_done) return true;
    if (jj_3R_73()) return true;
    return false;
  }

 inline bool jj_3R_478()
 {
    if (jj_done) return true;
    if (jj_3R_548()) return true;
    return false;
  }

 inline bool jj_3R_487()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_164()
 {
    if (jj_done) return true;
    if (jj_3R_133()) return true;
    if (jj_3R_134()) return true;
    return false;
  }

 inline bool jj_3R_181()
 {
    if (jj_done) return true;
    if (jj_3R_343()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_540()
 {
    if (jj_done) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_583()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_584()) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_585()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_586()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_81()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_177()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_178()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_179()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_373()
 {
    if (jj_done) return true;
    if (jj_scan_token(STRINGLITERAL)) return true;
    return false;
  }

 inline bool jj_3R_486()
 {
    if (jj_done) return true;
    if (jj_3R_553()) return true;
    return false;
  }

 inline bool jj_3R_485()
 {
    if (jj_done) return true;
    if (jj_3R_69()) return true;
    return false;
  }

 inline bool jj_3R_453()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_183()
 {
    if (jj_done) return true;
    if (jj_3R_345()) return true;
    return false;
  }

 inline bool jj_3R_182()
 {
    if (jj_done) return true;
    if (jj_3R_344()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_195()
 {
    if (jj_done) return true;
    if (jj_scan_token(BAR_T)) return true;
    if (jj_3R_194()) return true;
    return false;
  }

 inline bool jj_3R_163()
 {
    if (jj_done) return true;
    if (jj_3R_328()) return true;
    return false;
  }

 inline bool jj_3R_394()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    if (jj_3R_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_485()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_486()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_487()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_70()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_163()) jj_scanpos = xsp;
    if (jj_3R_134()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_164()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_84()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_181()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_182()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_183()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_375()
 {
    if (jj_done) return true;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_646()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_374()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_453()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_12()
 {
    if (jj_done) return true;
    if (jj_3R_72()) return true;
    return false;
  }

 inline bool jj_3R_245()
 {
    if (jj_done) return true;
    if (jj_scan_token(LBRACKET_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_374()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_375()) jj_scanpos = xsp;
    if (jj_scan_token(RBRACKET_T)) return true;
    return false;
  }

 inline bool jj_3R_90()
 {
    if (jj_done) return true;
    if (jj_3R_194()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_195()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_617()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3_11()
 {
    if (jj_done) return true;
    if (jj_3R_70()) return true;
    return false;
  }

 inline bool jj_3R_616()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_354()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_615()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_646()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_556()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_615()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_616()) return false;
    jj_scanpos = xsp;
    if (jj_3R_617()) return true;
    return false;
  }

 inline bool jj_3_10()
 {
    if (jj_done) return true;
    if (jj_3R_70()) return true;
    if (jj_3R_71()) return true;
    return false;
  }

 inline bool jj_3R_353()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_590()
 {
    if (jj_done) return true;
    if (jj_scan_token(BUS_T)) return true;
    return false;
  }

 inline bool jj_3R_352()
 {
    if (jj_done) return true;
    if (jj_3R_72()) return true;
    return false;
  }

 inline bool jj_3R_589()
 {
    if (jj_done) return true;
    if (jj_scan_token(REGISTER_T)) return true;
    return false;
  }

 inline bool jj_3R_548()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_589()) return false;
    jj_scanpos = xsp;
    if (jj_3R_590()) return true;
    return false;
  }

 inline bool jj_3R_351()
 {
    if (jj_done) return true;
    if (jj_3R_70()) return true;
    return false;
  }

 inline bool jj_3R_194()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_350()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_351()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_352()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_353()) return false;
    jj_scanpos = xsp;
    if (jj_3R_354()) return true;
    return false;
  }

 inline bool jj_3R_350()
 {
    if (jj_done) return true;
    if (jj_3R_88()) return true;
    return false;
  }

 inline bool jj_3R_634()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_369()
 {
    if (jj_done) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_390()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    if (jj_3R_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_89()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_478()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_479()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_98()
 {
    if (jj_done) return true;
    if (jj_3R_132()) return true;
    return false;
  }

 inline bool jj_3R_632()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_90()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    if (jj_3R_268()) return true;
    return false;
  }

 inline bool jj_3_97()
 {
    if (jj_done) return true;
    if (jj_3R_131()) return true;
    return false;
  }

 inline bool jj_3R_633()
 {
    if (jj_done) return true;
    if (jj_3R_632()) return true;
    return false;
  }

 inline bool jj_3R_541()
 {
    if (jj_done) return true;
    if (jj_3R_420()) return true;
    return false;
  }

 inline bool jj_3_96()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_456()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_96()) jj_scanpos = xsp;
    if (jj_3R_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_541()) jj_scanpos = xsp;
    if (jj_3R_332()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_455()
 {
    if (jj_done) return true;
    if (jj_3R_132()) return true;
    return false;
  }

 inline bool jj_3R_402()
 {
    if (jj_done) return true;
    if (jj_3R_494()) return true;
    if (jj_3R_70()) return true;
    return false;
  }

 inline bool jj_3R_377()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_454()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_455()) return false;
    jj_scanpos = xsp;
    if (jj_3R_456()) return true;
    return false;
  }

 inline bool jj_3R_454()
 {
    if (jj_done) return true;
    if (jj_3R_131()) return true;
    return false;
  }

 inline bool jj_3R_417()
 {
    if (jj_done) return true;
    if (jj_scan_token(MINUS_T)) return true;
    return false;
  }

 inline bool jj_3R_265()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_328()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_416()) return false;
    jj_scanpos = xsp;
    if (jj_3R_417()) return true;
    return false;
  }

 inline bool jj_3R_416()
 {
    if (jj_done) return true;
    if (jj_scan_token(PLUS_T)) return true;
    return false;
  }

 inline bool jj_3R_126()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_265()) jj_scanpos = xsp;
    if (jj_scan_token(CASE_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_632()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_633()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(CASE_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_634()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_567()
 {
    if (jj_done) return true;
    if (jj_scan_token(ROR_T)) return true;
    return false;
  }

 inline bool jj_3R_566()
 {
    if (jj_done) return true;
    if (jj_scan_token(ROL_T)) return true;
    return false;
  }

 inline bool jj_3R_565()
 {
    if (jj_done) return true;
    if (jj_scan_token(SRA_T)) return true;
    return false;
  }

 inline bool jj_3R_564()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLA_T)) return true;
    return false;
  }

 inline bool jj_3R_563()
 {
    if (jj_done) return true;
    if (jj_scan_token(SRL_T)) return true;
    return false;
  }

 inline bool jj_3R_562()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLL_T)) return true;
    return false;
  }

 inline bool jj_3R_494()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_562()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_563()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_564()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_565()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_566()) return false;
    jj_scanpos = xsp;
    if (jj_3R_567()) return true;
    return false;
  }

 inline bool jj_3R_522()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_577()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_318()
 {
    if (jj_done) return true;
    if (jj_3R_70()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_402()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_254()
 {
    if (jj_done) return true;
    if (jj_3R_379()) return true;
    return false;
  }

 inline bool jj_3_95()
 {
    if (jj_done) return true;
    if (jj_3R_130()) return true;
    return false;
  }

 inline bool jj_3_9()
 {
    if (jj_done) return true;
    if (jj_3R_69()) return true;
    return false;
  }

 inline bool jj_3_94()
 {
    if (jj_done) return true;
    if (jj_3R_129()) return true;
    return false;
  }

 inline bool jj_3R_431()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_522()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_93()
 {
    if (jj_done) return true;
    if (jj_3R_128()) return true;
    return false;
  }

 inline bool jj_3R_123()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_88()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_123()) jj_scanpos = xsp;
    if (jj_3R_119()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    return false;
  }

 inline bool jj_3_92()
 {
    if (jj_done) return true;
    if (jj_3R_127()) return true;
    return false;
  }

 inline bool jj_3_91()
 {
    if (jj_done) return true;
    if (jj_3R_126()) return true;
    return false;
  }

 inline bool jj_3_90()
 {
    if (jj_done) return true;
    if (jj_3R_125()) return true;
    return false;
  }

 inline bool jj_3_89()
 {
    if (jj_done) return true;
    if (jj_3R_124()) return true;
    return false;
  }

 inline bool jj_3R_118()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_311()
 {
    if (jj_done) return true;
    if (jj_3R_398()) return true;
    return false;
  }

 inline bool jj_3R_253()
 {
    if (jj_done) return true;
    if (jj_3R_378()) return true;
    return false;
  }

 inline bool jj_3_7()
 {
    if (jj_done) return true;
    if (jj_3R_67()) return true;
    return false;
  }

 inline bool jj_3_84()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_118()) jj_scanpos = xsp;
    if (jj_3R_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    return false;
  }

 inline bool jj_3_87()
 {
    if (jj_done) return true;
    if (jj_3R_122()) return true;
    return false;
  }

 inline bool jj_3_8()
 {
    if (jj_done) return true;
    if (jj_3R_68()) return true;
    return false;
  }

 inline bool jj_3R_310()
 {
    if (jj_done) return true;
    if (jj_3R_347()) return true;
    return false;
  }

 inline bool jj_3R_309()
 {
    if (jj_done) return true;
    if (jj_3R_397()) return true;
    return false;
  }

 inline bool jj_3_86()
 {
    if (jj_done) return true;
    if (jj_3R_121()) return true;
    return false;
  }

 inline bool jj_3R_308()
 {
    if (jj_done) return true;
    if (jj_3R_396()) return true;
    return false;
  }

 inline bool jj_3R_307()
 {
    if (jj_done) return true;
    if (jj_3R_395()) return true;
    return false;
  }

 inline bool jj_3_85()
 {
    if (jj_done) return true;
    if (jj_3R_120()) return true;
    return false;
  }

 inline bool jj_3R_520()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_332()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_90()) return true;
    return false;
  }

 inline bool jj_3R_305()
 {
    if (jj_done) return true;
    if (jj_3R_394()) return true;
    return false;
  }

 inline bool jj_3R_306()
 {
    if (jj_done) return true;
    if (jj_3R_67()) return true;
    return false;
  }

 inline bool jj_3R_304()
 {
    if (jj_done) return true;
    if (jj_3R_393()) return true;
    return false;
  }

 inline bool jj_3R_117()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_252()) return false;
    jj_scanpos = xsp;
    if (!jj_3_85()) return false;
    jj_scanpos = xsp;
    if (!jj_3_86()) return false;
    jj_scanpos = xsp;
    if (!jj_3_87()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_253()) return false;
    jj_scanpos = xsp;
    if (!jj_3_89()) return false;
    jj_scanpos = xsp;
    if (!jj_3_90()) return false;
    jj_scanpos = xsp;
    if (!jj_3_91()) return false;
    jj_scanpos = xsp;
    if (!jj_3_92()) return false;
    jj_scanpos = xsp;
    if (!jj_3_93()) return false;
    jj_scanpos = xsp;
    if (!jj_3_94()) return false;
    jj_scanpos = xsp;
    if (!jj_3_95()) return false;
    jj_scanpos = xsp;
    if (jj_3R_254()) return true;
    return false;
  }

 inline bool jj_3R_252()
 {
    if (jj_done) return true;
    if (jj_3R_377()) return true;
    return false;
  }

 inline bool jj_3R_303()
 {
    if (jj_done) return true;
    if (jj_3R_392()) return true;
    return false;
  }

 inline bool jj_3R_302()
 {
    if (jj_done) return true;
    if (jj_3R_391()) return true;
    return false;
  }

 inline bool jj_3R_524()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_301()
 {
    if (jj_done) return true;
    if (jj_3R_390()) return true;
    return false;
  }

 inline bool jj_3R_300()
 {
    if (jj_done) return true;
    if (jj_3R_389()) return true;
    return false;
  }

 inline bool jj_3R_299()
 {
    if (jj_done) return true;
    if (jj_3R_388()) return true;
    return false;
  }

 inline bool jj_3R_433()
 {
    if (jj_done) return true;
    if (jj_3R_523()) return true;
    return false;
  }

 inline bool jj_3R_298()
 {
    if (jj_done) return true;
    if (jj_3R_387()) return true;
    return false;
  }

 inline bool jj_3R_432()
 {
    if (jj_done) return true;
    if (jj_3R_347()) return true;
    return false;
  }

 inline bool jj_3_83()
 {
    if (jj_done) return true;
    if (jj_3R_117()) return true;
    return false;
  }

 inline bool jj_3R_146()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_297()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_298()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_299()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_300()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_301()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_302()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_303()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_304()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_305()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_306()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_307()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_308()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_309()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_310()) return false;
    jj_scanpos = xsp;
    if (!jj_3_8()) return false;
    jj_scanpos = xsp;
    if (jj_3R_311()) return true;
    return false;
  }

 inline bool jj_3R_297()
 {
    if (jj_done) return true;
    if (jj_3R_98()) return true;
    return false;
  }

 inline bool jj_3R_268()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_83()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_345()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_431()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_432()) { jj_scanpos = xsp; break; }
    }
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_433()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_440()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_524()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_250()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_357()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_439()) return false;
    jj_scanpos = xsp;
    if (jj_3R_440()) return true;
    return false;
  }

 inline bool jj_3R_439()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_276()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_380()
 {
    if (jj_done) return true;
    if (jj_scan_token(ON_T)) return true;
    if (jj_3R_357()) return true;
    return false;
  }

 inline bool jj_3R_100()
 {
    if (jj_done) return true;
    if (jj_scan_token(BIT_STRING_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_423()
 {
    if (jj_done) return true;
    if (jj_3R_332()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_90()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_520()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_430()
 {
    if (jj_done) return true;
    if (jj_3R_341()) return true;
    return false;
  }

 inline bool jj_3R_429()
 {
    if (jj_done) return true;
    if (jj_3R_73()) return true;
    return false;
  }

 inline bool jj_3R_428()
 {
    if (jj_done) return true;
    if (jj_scan_token(USE_T)) return true;
    if (jj_3R_521()) return true;
    return false;
  }

 inline bool jj_3R_344()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_428()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_429()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_430()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_337()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    if (jj_3R_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_3R_172()) return true;
    if (jj_3R_423()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_688()
 {
    if (jj_done) return true;
    if (jj_3R_694()) return true;
    return false;
  }

 inline bool jj_3R_249()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_434()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_105()) return true;
    return false;
  }

 inline bool jj_3R_721()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(EQU_T)) return true;
    if (jj_3R_108()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_82()
 {
    if (jj_done) return true;
    if (jj_3R_116()) return true;
    return false;
  }

 inline bool jj_3R_677()
 {
    if (jj_done) return true;
    if (jj_3R_86()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_688()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_676()
 {
    if (jj_done) return true;
    if (jj_3R_116()) return true;
    return false;
  }

 inline bool jj_3R_656()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_676()) return false;
    jj_scanpos = xsp;
    if (jj_3R_677()) return true;
    return false;
  }

 inline bool jj_3R_356()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEVERITY_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_275()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_130()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_275()) jj_scanpos = xsp;
    if (jj_scan_token(RETURN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_276()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_259()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEVERITY_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_395()
 {
    if (jj_done) return true;
    if (jj_scan_token(ATTRIBUTE_T)) return true;
    if (jj_3R_230()) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_488()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_330()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_329()) return true;
    return false;
  }

 inline bool jj_3R_115()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_scan_token(85)) return false;
    jj_scanpos = xsp;
    if (jj_3R_249()) return true;
    xsp = jj_scanpos;
    if (jj_3R_250()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_258()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_319()
 {
    if (jj_done) return true;
    if (jj_3R_403()) return true;
    if (jj_3R_318()) return true;
    return false;
  }

 inline bool jj_3R_121()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_258()) jj_scanpos = xsp;
    if (jj_scan_token(REPORT_T)) return true;
    if (jj_3R_61()) return true;
    xsp = jj_scanpos;
    if (jj_3R_259()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_372()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    return false;
  }

 inline bool jj_3R_230()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_371()) return false;
    jj_scanpos = xsp;
    if (jj_3R_372()) return true;
    return false;
  }

 inline bool jj_3R_371()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_506()
 {
    if (jj_done) return true;
    if (jj_scan_token(QNEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_505()
 {
    if (jj_done) return true;
    if (jj_scan_token(QEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_504()
 {
    if (jj_done) return true;
    if (jj_scan_token(QL_T)) return true;
    return false;
  }

 inline bool jj_3R_698()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_503()
 {
    if (jj_done) return true;
    if (jj_scan_token(QG_T)) return true;
    return false;
  }

 inline bool jj_3R_502()
 {
    if (jj_done) return true;
    if (jj_scan_token(QLT_T)) return true;
    return false;
  }

 inline bool jj_3R_501()
 {
    if (jj_done) return true;
    if (jj_scan_token(QGT_T)) return true;
    return false;
  }

 inline bool jj_3R_500()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOTEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_499()
 {
    if (jj_done) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    return false;
  }

 inline bool jj_3R_498()
 {
    if (jj_done) return true;
    if (jj_scan_token(GREATERTHAN_T)) return true;
    return false;
  }

 inline bool jj_3_6()
 {
    if (jj_done) return true;
    if (jj_3R_66()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_67()
 {
    if (jj_done) return true;
    if (jj_scan_token(ATTRIBUTE_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_161()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_497()
 {
    if (jj_done) return true;
    if (jj_scan_token(EQU_T)) return true;
    return false;
  }

 inline bool jj_3R_355()
 {
    if (jj_done) return true;
    if (jj_scan_token(REPORT_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_496()
 {
    if (jj_done) return true;
    if (jj_scan_token(GT_T)) return true;
    return false;
  }

 inline bool jj_3R_403()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_495()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_496()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_497()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_498()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_499()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_500()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_501()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_502()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_503()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_504()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_505()) return false;
    jj_scanpos = xsp;
    if (jj_3R_506()) return true;
    return false;
  }

 inline bool jj_3R_495()
 {
    if (jj_done) return true;
    if (jj_scan_token(LT_T)) return true;
    return false;
  }

 inline bool jj_3R_169()
 {
    if (jj_done) return true;
    if (jj_3R_329()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_330()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_151()
 {
    if (jj_done) return true;
    if (jj_3R_318()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_319()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_697()
 {
    if (jj_done) return true;
    if (jj_3R_705()) return true;
    return false;
  }

 inline bool jj_3R_418()
 {
    if (jj_done) return true;
    if (jj_3R_66()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_329()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_418()) jj_scanpos = xsp;
    if (jj_3R_419()) return true;
    return false;
  }

 inline bool jj_3R_690()
 {
    if (jj_done) return true;
    if (jj_scan_token(RECORD_T)) return true;
    Token * xsp;
    if (jj_3R_697()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_697()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(RECORD_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_698()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_257()
 {
    if (jj_done) return true;
    if (jj_3R_145()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_120()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_257()) jj_scanpos = xsp;
    if (jj_3R_196()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_80()
 {
    if (jj_done) return true;
    if (jj_3R_70()) return true;
    if (jj_3R_71()) return true;
    if (jj_3R_70()) return true;
    return false;
  }

 inline bool jj_3_81()
 {
    if (jj_done) return true;
    if (jj_3R_115()) return true;
    return false;
  }

 inline bool jj_3R_86()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_3R_88()) return true;
    return false;
  }

 inline bool jj_3R_196()
 {
    if (jj_done) return true;
    if (jj_scan_token(ASSERT_T)) return true;
    if (jj_3R_83()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_355()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_356()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_79()
 {
    if (jj_done) return true;
    if (jj_3R_114()) return true;
    return false;
  }

 inline bool jj_3_5()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_3R_65()) return true;
    if (jj_scan_token(OF_T)) return true;
    return false;
  }

 inline bool jj_3R_191()
 {
    if (jj_done) return true;
    if (jj_3R_115()) return true;
    return false;
  }

 inline bool jj_3R_190()
 {
    if (jj_done) return true;
    if (jj_3R_70()) return true;
    if (jj_3R_71()) return true;
    if (jj_3R_70()) return true;
    return false;
  }

 inline bool jj_3R_88()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_190()) return false;
    jj_scanpos = xsp;
    if (jj_3R_191()) return true;
    return false;
  }

 inline bool jj_3R_696()
 {
    if (jj_done) return true;
    if (jj_3R_704()) return true;
    return false;
  }

 inline bool jj_3R_238()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_237()
 {
    if (jj_done) return true;
    if (jj_3R_114()) return true;
    return false;
  }

 inline bool jj_3R_695()
 {
    if (jj_done) return true;
    if (jj_3R_703()) return true;
    return false;
  }

 inline bool jj_3R_689()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_695()) return false;
    jj_scanpos = xsp;
    if (jj_3R_696()) return true;
    return false;
  }

 inline bool jj_3R_64()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_237()) return false;
    jj_scanpos = xsp;
    if (jj_3R_238()) return true;
    return false;
  }

 inline bool jj_3R_359()
 {
    if (jj_done) return true;
    if (jj_3R_117()) return true;
    return false;
  }

 inline bool jj_3R_200()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_359()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_201()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_484()
 {
    if (jj_done) return true;
    if (jj_3R_245()) return true;
    return false;
  }

 inline bool jj_3R_248()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_247()) return true;
    return false;
  }

 inline bool jj_3R_244()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_89()) return true;
    return false;
  }

 inline bool jj_3R_198()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_357()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_113()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3_4()) return false;
    jj_scanpos = xsp;
    if (jj_3R_244()) return true;
    return false;
  }

 inline bool jj_3_4()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_64()) return true;
    return false;
  }

 inline bool jj_3R_552()
 {
    if (jj_done) return true;
    if (jj_3R_321()) return true;
    return false;
  }

 inline bool jj_3R_197()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_551()
 {
    if (jj_done) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_93()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_197()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_198()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    if (jj_3R_199()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_200()) return true;
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_201()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_550()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_482()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_550()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_551()) return false;
    jj_scanpos = xsp;
    if (jj_3R_552()) return true;
    return false;
  }

 inline bool jj_3R_358()
 {
    if (jj_done) return true;
    if (jj_3R_441()) return true;
    return false;
  }

 inline bool jj_3R_199()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_358()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_534()
 {
    if (jj_done) return true;
    if (jj_3R_398()) return true;
    return false;
  }

 inline bool jj_3R_533()
 {
    if (jj_done) return true;
    if (jj_3R_347()) return true;
    return false;
  }

 inline bool jj_3R_483()
 {
    if (jj_done) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_89()) return true;
    return false;
  }

 inline bool jj_3_78()
 {
    if (jj_done) return true;
    if (jj_3R_68()) return true;
    return false;
  }

 inline bool jj_3R_532()
 {
    if (jj_done) return true;
    if (jj_3R_395()) return true;
    return false;
  }

 inline bool jj_3R_393()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALIAS_T)) return true;
    if (jj_3R_482()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_483()) jj_scanpos = xsp;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_63()) return true;
    xsp = jj_scanpos;
    if (jj_3R_484()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_531()
 {
    if (jj_done) return true;
    if (jj_3R_393()) return true;
    return false;
  }

 inline bool jj_3_77()
 {
    if (jj_done) return true;
    if (jj_3R_67()) return true;
    return false;
  }

 inline bool jj_3R_530()
 {
    if (jj_done) return true;
    if (jj_3R_392()) return true;
    return false;
  }

 inline bool jj_3R_529()
 {
    if (jj_done) return true;
    if (jj_3R_391()) return true;
    return false;
  }

 inline bool jj_3R_114()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_247()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_248()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_528()
 {
    if (jj_done) return true;
    if (jj_3R_389()) return true;
    return false;
  }

 inline bool jj_3R_527()
 {
    if (jj_done) return true;
    if (jj_3R_388()) return true;
    return false;
  }

 inline bool jj_3R_526()
 {
    if (jj_done) return true;
    if (jj_3R_387()) return true;
    return false;
  }

 inline bool jj_3R_282()
 {
    if (jj_done) return true;
    if (jj_scan_token(MINUS_T)) return true;
    return false;
  }

 inline bool jj_3R_283()
 {
    if (jj_done) return true;
    if (jj_scan_token(AMPERSAND_T)) return true;
    return false;
  }

 inline bool jj_3_3()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_525()
 {
    if (jj_done) return true;
    if (jj_3R_98()) return true;
    return false;
  }

 inline bool jj_3R_133()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_281()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_282()) return false;
    jj_scanpos = xsp;
    if (jj_3R_283()) return true;
    return false;
  }

 inline bool jj_3R_281()
 {
    if (jj_done) return true;
    if (jj_scan_token(PLUS_T)) return true;
    return false;
  }

 inline bool jj_3R_441()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_525()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_526()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_527()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_528()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_529()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_530()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_531()) return false;
    jj_scanpos = xsp;
    if (!jj_3_77()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_532()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_533()) return false;
    jj_scanpos = xsp;
    if (!jj_3_78()) return false;
    jj_scanpos = xsp;
    if (jj_3R_534()) return true;
    return false;
  }

 inline bool jj_3R_342()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_236()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_2()
 {
    if (jj_done) return true;
    if (jj_3R_62()) return true;
    return false;
  }

 inline bool jj_3_76()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_124()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_76()) jj_scanpos = xsp;
    if (jj_3R_180()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_516()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_62()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_515()
 {
    if (jj_done) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_180()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_342()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_514()
 {
    if (jj_done) return true;
    if (jj_3R_62()) return true;
    return false;
  }

 inline bool jj_3R_419()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_514()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_515()) return false;
    jj_scanpos = xsp;
    if (jj_3R_516()) return true;
    return false;
  }

 inline bool jj_3_74()
 {
    if (jj_done) return true;
    if (jj_3R_91()) return true;
    return false;
  }

 inline bool jj_3R_214()
 {
    if (jj_done) return true;
    if (jj_3R_87()) return true;
    return false;
  }

 inline bool jj_3R_236()
 {
    if (jj_done) return true;
    if (jj_3R_169()) return true;
    return false;
  }

 inline bool jj_3R_213()
 {
    if (jj_done) return true;
    if (jj_3R_368()) return true;
    return false;
  }

 inline bool jj_3_1()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3_75()
 {
    if (jj_done) return true;
    if (jj_3R_95()) return true;
    return false;
  }

 inline bool jj_3R_212()
 {
    if (jj_done) return true;
    if (jj_3R_91()) return true;
    return false;
  }

 inline bool jj_3R_211()
 {
    if (jj_done) return true;
    if (jj_3R_367()) return true;
    return false;
  }

 inline bool jj_3R_155()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3_73()
 {
    if (jj_done) return true;
    if (jj_3R_113()) return true;
    return false;
  }

 inline bool jj_3R_210()
 {
    if (jj_done) return true;
    if (jj_3R_366()) return true;
    return false;
  }

 inline bool jj_3R_99()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_210()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_211()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_212()) return false;
    jj_scanpos = xsp;
    if (!jj_3_75()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_213()) return false;
    jj_scanpos = xsp;
    if (jj_3R_214()) return true;
    return false;
  }

 inline bool jj_3_72()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_154()
 {
    if (jj_done) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3_71()
 {
    if (jj_done) return true;
    if (jj_3R_112()) return true;
    return false;
  }

 inline bool jj_3R_153()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    return false;
  }

 inline bool jj_3R_62()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_153()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_154()) return false;
    jj_scanpos = xsp;
    if (jj_3R_155()) return true;
    return false;
  }

 inline bool jj_3R_475()
 {
    if (jj_done) return true;
    if (jj_3R_114()) return true;
    return false;
  }

 inline bool jj_3_70()
 {
    if (jj_done) return true;
    if (jj_3R_111()) return true;
    return false;
  }

 inline bool jj_3R_474()
 {
    if (jj_done) return true;
    if (jj_3R_113()) return true;
    return false;
  }

 inline bool jj_3R_658()
 {
    if (jj_done) return true;
    if (jj_scan_token(ACCESS_T)) return true;
    if (jj_3R_89()) return true;
    return false;
  }

 inline bool jj_3_69()
 {
    if (jj_done) return true;
    if (jj_3R_64()) return true;
    return false;
  }

 inline bool jj_3R_473()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3_68()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_472()
 {
    if (jj_done) return true;
    if (jj_3R_112()) return true;
    return false;
  }

 inline bool jj_3R_235()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASED_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_702()
 {
    if (jj_done) return true;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_234()
 {
    if (jj_done) return true;
    if (jj_scan_token(INTEGER)) return true;
    return false;
  }

 inline bool jj_3_67()
 {
    if (jj_done) return true;
    if (jj_3R_110()) return true;
    return false;
  }

 inline bool jj_3R_109()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_233()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_234()) return false;
    jj_scanpos = xsp;
    if (jj_3R_235()) return true;
    return false;
  }

 inline bool jj_3R_233()
 {
    if (jj_done) return true;
    if (jj_scan_token(DECIMAL_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_471()
 {
    if (jj_done) return true;
    if (jj_3R_111()) return true;
    return false;
  }

 inline bool jj_3R_470()
 {
    if (jj_done) return true;
    if (jj_3R_64()) return true;
    return false;
  }

 inline bool jj_3R_469()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_386()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_468()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_469()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_470()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_471()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_472()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_473()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_474()) return false;
    jj_scanpos = xsp;
    if (jj_3R_475()) return true;
    return false;
  }

 inline bool jj_3R_468()
 {
    if (jj_done) return true;
    if (jj_3R_110()) return true;
    return false;
  }

 inline bool jj_3R_341()
 {
    if (jj_done) return true;
    if (jj_scan_token(PORT_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_169()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_594()
 {
    if (jj_done) return true;
    if (jj_3R_327()) return true;
    return false;
  }

 inline bool jj_3R_701()
 {
    if (jj_done) return true;
    if (jj_3R_721()) return true;
    return false;
  }

 inline bool jj_3R_553()
 {
    if (jj_done) return true;
    if (jj_scan_token(PORT_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_594()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_694()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNITS_T)) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_701()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(UNITS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_702()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_66()
 {
    if (jj_done) return true;
    if (jj_3R_109()) return true;
    return false;
  }

 inline bool jj_3R_232()
 {
    if (jj_done) return true;
    if (jj_3R_109()) return true;
    return false;
  }

 inline bool jj_3R_108()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_232()) jj_scanpos = xsp;
    if (jj_3R_63()) return true;
    return false;
  }

 inline bool jj_3R_543()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(IN_T)) return true;
    if (jj_3R_72()) return true;
    return false;
  }

 inline bool jj_3_65()
 {
    if (jj_done) return true;
    if (jj_3R_91()) return true;
    return false;
  }

 inline bool jj_3_63()
 {
    if (jj_done) return true;
    if (jj_3R_67()) return true;
    return false;
  }

 inline bool jj_3_64()
 {
    if (jj_done) return true;
    if (jj_3R_68()) return true;
    return false;
  }

 inline bool jj_3R_368()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3_62()
 {
    if (jj_done) return true;
    if (jj_3R_68()) return true;
    return false;
  }

 inline bool jj_3R_331()
 {
    if (jj_done) return true;
    if (jj_3R_420()) return true;
    return false;
  }

 inline bool jj_3R_172()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(50)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_331()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_270()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_321()
 {
    if (jj_done) return true;
    if (jj_scan_token(STRINGLITERAL)) return true;
    return false;
  }

 inline bool jj_3R_654()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_653()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    return false;
  }

 inline bool jj_3R_652()
 {
    if (jj_done) return true;
    if (jj_scan_token(SHARED_T)) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3_61()
 {
    if (jj_done) return true;
    if (jj_3R_108()) return true;
    return false;
  }

 inline bool jj_3R_651()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_650()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_649()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_625()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_649()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_650()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_651()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_652()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_653()) return false;
    jj_scanpos = xsp;
    if (jj_3R_654()) return true;
    return false;
  }

 inline bool jj_3R_370()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_216()
 {
    if (jj_done) return true;
    if (jj_3R_109()) return true;
    return false;
  }

 inline bool jj_3R_215()
 {
    if (jj_done) return true;
    if (jj_3R_108()) return true;
    return false;
  }

 inline bool jj_3R_101()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_215()) return false;
    jj_scanpos = xsp;
    if (jj_3R_216()) return true;
    return false;
  }

 inline bool jj_3R_459()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_379()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_459()) jj_scanpos = xsp;
    if (jj_scan_token(NULL_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_107()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_61()) return true;
    return false;
  }

 inline bool jj_3R_271()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_83()) return true;
    return false;
  }

 inline bool jj_3R_269()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_60()
 {
    if (jj_done) return true;
    if (jj_scan_token(LBRACKET_T)) return true;
    return false;
  }

 inline bool jj_3R_128()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_269()) jj_scanpos = xsp;
    if (jj_scan_token(NEXT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_270()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_271()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_231()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_59()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_107()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_229()
 {
    if (jj_done) return true;
    if (jj_3R_245()) return true;
    return false;
  }

 inline bool jj_3R_106()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_229()) jj_scanpos = xsp;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_3R_230()) return true;
    xsp = jj_scanpos;
    if (jj_3R_231()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_58()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_72()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_54()
 {
    if (jj_done) return true;
    if (jj_3R_104()) return true;
    return false;
  }

 inline bool jj_3_57()
 {
    if (jj_done) return true;
    if (jj_3R_106()) return true;
    return false;
  }

 inline bool jj_3_52()
 {
    if (jj_done) return true;
    if (jj_3R_103()) return true;
    return false;
  }

 inline bool jj_3_51()
 {
    if (jj_done) return true;
    if (jj_3R_103()) return true;
    return false;
  }

 inline bool jj_3_53()
 {
    if (jj_done) return true;
    if (jj_3R_103()) return true;
    return false;
  }

 inline bool jj_3_56()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_105()) return true;
    return false;
  }

 inline bool jj_3R_224()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_61()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_370()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_55()
 {
    if (jj_done) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_219()
 {
    if (jj_done) return true;
    if (jj_3R_104()) return true;
    return false;
  }

 inline bool jj_3R_223()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_72()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_222()
 {
    if (jj_done) return true;
    if (jj_3R_106()) return true;
    return false;
  }

 inline bool jj_3R_322()
 {
    if (jj_done) return true;
    if (jj_3R_103()) return true;
    return false;
  }

 inline bool jj_3R_324()
 {
    if (jj_done) return true;
    if (jj_3R_103()) return true;
    return false;
  }

 inline bool jj_3R_325()
 {
    if (jj_done) return true;
    if (jj_3R_103()) return true;
    return false;
  }

 inline bool jj_3R_221()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_105()) return true;
    return false;
  }

 inline bool jj_3R_220()
 {
    if (jj_done) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_104()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_220()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_221()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_222()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_223()) return false;
    jj_scanpos = xsp;
    if (jj_3R_224()) return true;
    return false;
  }

 inline bool jj_3R_103()
 {
    if (jj_done) return true;
    if (jj_3R_104()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_219()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_158()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_325()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_157()
 {
    if (jj_done) return true;
    if (jj_3R_323()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_324()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_156()
 {
    if (jj_done) return true;
    if (jj_3R_321()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_322()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_63()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_156()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_157()) return false;
    jj_scanpos = xsp;
    if (jj_3R_158()) return true;
    return false;
  }

 inline bool jj_3R_290()
 {
    if (jj_done) return true;
    if (jj_scan_token(REM_T)) return true;
    return false;
  }

 inline bool jj_3R_289()
 {
    if (jj_done) return true;
    if (jj_scan_token(MOD_T)) return true;
    return false;
  }

 inline bool jj_3R_288()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLASH_T)) return true;
    return false;
  }

 inline bool jj_3R_140()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_287()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_288()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_289()) return false;
    jj_scanpos = xsp;
    if (jj_3R_290()) return true;
    return false;
  }

 inline bool jj_3R_287()
 {
    if (jj_done) return true;
    if (jj_scan_token(MULT_T)) return true;
    return false;
  }

 inline bool jj_3R_635()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }

 inline bool jj_3R_449()
 {
    if (jj_done) return true;
    if (jj_scan_token(LINKAGE_T)) return true;
    return false;
  }

 inline bool jj_3R_448()
 {
    if (jj_done) return true;
    if (jj_scan_token(BUFFER_T)) return true;
    return false;
  }

 inline bool jj_3R_447()
 {
    if (jj_done) return true;
    if (jj_scan_token(INOUT_T)) return true;
    return false;
  }

 inline bool jj_3R_446()
 {
    if (jj_done) return true;
    if (jj_scan_token(OUT_T)) return true;
    return false;
  }

 inline bool jj_3R_363()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_445()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_446()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_447()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_448()) return false;
    jj_scanpos = xsp;
    if (jj_3R_449()) return true;
    return false;
  }

 inline bool jj_3R_445()
 {
    if (jj_done) return true;
    if (jj_scan_token(IN_T)) return true;
    return false;
  }

 inline bool jj_3R_267()
 {
    if (jj_done) return true;
    if (jj_3R_383()) return true;
    return false;
  }

 inline bool jj_3R_266()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_127()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_266()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_267()) jj_scanpos = xsp;
    if (jj_scan_token(LOOP_T)) return true;
    if (jj_3R_268()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(LOOP_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_635()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_50()
 {
    if (jj_done) return true;
    if (jj_3R_102()) return true;
    return false;
  }

 inline bool jj_3_49()
 {
    if (jj_done) return true;
    if (jj_3R_101()) return true;
    return false;
  }

 inline bool jj_3R_243()
 {
    if (jj_done) return true;
    if (jj_scan_token(NULL_T)) return true;
    return false;
  }

 inline bool jj_3_48()
 {
    if (jj_done) return true;
    if (jj_3R_100()) return true;
    return false;
  }

 inline bool jj_3R_242()
 {
    if (jj_done) return true;
    if (jj_3R_373()) return true;
    return false;
  }

 inline bool jj_3R_241()
 {
    if (jj_done) return true;
    if (jj_3R_102()) return true;
    return false;
  }

 inline bool jj_3R_240()
 {
    if (jj_done) return true;
    if (jj_3R_101()) return true;
    return false;
  }

 inline bool jj_3R_112()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (!jj_3R_239()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_240()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_241()) return false;
    jj_scanpos = xsp;
    if (!jj_3R_242()) return false;
    jj_scanpos = xsp;
    if (jj_3R_243()) return true;
    return false;
  }

 inline bool jj_3R_239()
 {
    if (jj_done) return true;
    if (jj_3R_100()) return true;
    return false;
  }

 inline bool jj_3_47()
 {
    if (jj_done) return true;
    if (jj_3R_99()) return true;
    return false;
  }

 inline bool jj_3R_346()
 {
    if (jj_done) return true;
    if (jj_scan_token(LIBRARY_T)) return true;
    if (jj_3R_205()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_145()
 {
    if (jj_done) return true;
    if (jj_3R_74()) return true;
    return false;
  }


public: 
  void setErrorHandler(ErrorHandler *eh) {
    if (errorHandler) delete errorHandler;
    errorHandler = eh;
  }

  TokenManager *token_source = nullptr;
  CharStream   *jj_input_stream = nullptr;
  /** Current token. */
  Token        *token = nullptr;
  /** Next token. */
  Token        *jj_nt = nullptr;

private: 
  int           jj_ntk;
  JJCalls       jj_2_rtns[126];
  bool          jj_rescan;
  int           jj_gc;
  Token        *jj_scanpos, *jj_lastpos;
  int           jj_la;
  /** Whether we are looking ahead. */
  bool          jj_lookingAhead;
  bool          jj_semLA;
  int           jj_gen;
  int           jj_la1[304];
  ErrorHandler *errorHandler = nullptr;

protected: 
  bool          hasError;

  Token *head; 
public: 
  VhdlParser(TokenManager *tokenManager);
  virtual ~VhdlParser();
void ReInit(TokenManager* tokenManager);
void clear();
Token * jj_consume_token(int kind);
bool  jj_scan_token(int kind);
Token * getNextToken();
Token * getToken(int index);
int jj_ntk_f();
private:
  int jj_kind;
  int **jj_expentries;
  int *jj_expentry;
  void jj_add_error_token(int kind, int pos);
protected:
  /** Generate ParseException. */
  virtual void  parseError();
private:
  int  indent;	// trace indentation
  bool trace = false; // trace enabled if true

public:
  bool trace_enabled();
  void enable_tracing();
  void disable_tracing();
  void jj_rescan_token();
  void jj_save(int index, int xla);
struct SharedState
{
  std::shared_ptr<Entry> current_root;
  std::shared_ptr<Entry> tempEntry;
  std::shared_ptr<Entry> lastEntity;
  std::shared_ptr<Entry> lastCompound;
  std::shared_ptr<Entry> current;
  QCString compSpec;
  QCString currName;
  int levelCounter = 0;
  QCString confName;
  QCString genLabels;
  QCString lab;
  int param_sec = 0;
  int parse_sec = 0;
  Spec currP = 0;
};

VHDLOutlineParser *m_outlineParser;
SharedState *m_sharedState;

void setOutlineParser(VHDLOutlineParser* p) { m_outlineParser=p; }
VHDLOutlineParser *outlineParser() const { return m_outlineParser; }
void setSharedState(SharedState *s) { m_sharedState=s; }
void clearError() { hasError = false; }

private:
  bool jj_done;
};
}
}
#endif
