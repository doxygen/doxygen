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

 inline bool jj_3R_interface_variable_declaration_1485_49_207()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_iteration_scheme_1526_3_461()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_parameter_specification_1833_1_543()) return true;
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1484_37_206()
 {
    if (jj_done) return true;
    if (jj_3R_mode_1615_1_363()) return true;
    return false;
  }

 inline bool jj_3R_iteration_scheme_1519_1_383()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_iteration_scheme_1519_1_460()) {
    jj_scanpos = xsp;
    if (jj_3R_iteration_scheme_1526_3_461()) return true;
    }
    return false;
  }

 inline bool jj_3R_iteration_scheme_1519_1_460()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHILE_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    return false;
  }

 inline bool jj_3R_interface_list_1476_26_415()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEMI_T)) return true;
    if (jj_3R_interface_element_1462_3_414()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1413_86_424()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1483_3_204()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(116)) {
    jj_scanpos = xsp;
    if (jj_scan_token(100)) {
    jj_scanpos = xsp;
    if (jj_scan_token(31)) {
    jj_scanpos = xsp;
    if (jj_scan_token(101)) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1483_1_96()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_variable_declaration_1483_3_204()) jj_scanpos = xsp;
    if (jj_3R_identifier_list_1354_4_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_interface_variable_declaration_1484_37_206()) jj_scanpos = xsp;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(27)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_variable_declaration_1485_49_207()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_interface_list_1476_3_327()
 {
    if (jj_done) return true;
    if (jj_3R_interface_element_1462_3_414()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_interface_list_1476_26_415()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_ifunc_2910_98_693()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_interface_file_declaration_1467_3_97()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_3R_identifier_list_1354_4_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    return false;
  }

 inline bool jj_3R_interface_element_1462_3_414()
 {
    if (jj_done) return true;
    if (jj_3R_interface_declaration_1437_1_513()) return true;
    return false;
  }

 inline bool jj_3R_ifunc_2910_87_444()
 {
    if (jj_done) return true;
    if (jj_scan_token(IS_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_ifunc_2910_98_693()) {
    jj_scanpos = xsp;
    if (jj_scan_token(145)) return true;
    }
    return false;
  }

 inline bool jj_3_46()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2294_1_98()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1452_2_572()
 {
    if (jj_done) return true;
    if (jj_3R_object_class_1711_1_625()) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1449_1_571()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2294_1_98()) return true;
    return false;
  }

 inline bool jj_3_45()
 {
    if (jj_done) return true;
    if (jj_3R_interface_file_declaration_1467_3_97()) return true;
    return false;
  }

 inline bool jj_3_44()
 {
    if (jj_done) return true;
    if (jj_3R_interface_variable_declaration_1483_1_96()) return true;
    return false;
  }

 inline bool jj_3_43()
 {
    if (jj_done) return true;
    if (jj_3R_interface_package_declaration_2658_2_95()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1437_1_513()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_42()) {
    jj_scanpos = xsp;
    if (jj_3_43()) {
    jj_scanpos = xsp;
    if (jj_3_44()) {
    jj_scanpos = xsp;
    if (jj_3_45()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1449_1_571()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1452_2_572()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3_42()
 {
    if (jj_done) return true;
    if (jj_3R_interface_subprogram_declaration_2895_4_94()) return true;
    return false;
  }

 inline bool jj_3R_index_constraint_1395_42_159()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_discrete_range_964_3_72()) return true;
    return false;
  }

 inline bool jj_3_41()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASIC_IDENTIFIER)) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1378_46_631()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1421_3_427()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1420_3_426()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1419_3_343()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_instantiation_list_1419_3_425()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_list_1420_3_426()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_list_1421_3_427()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_instantiation_list_1419_3_425()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_list_1354_4_205()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1414_3_340()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1413_3_339()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_41()) jj_scanpos = xsp;
    if (jj_3R_name_1633_2_63()) return true;
    xsp = jj_scanpos;
    if (jj_3R_instantiation_unit_1413_86_424()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1412_1_177()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_instantiation_unit_1412_1_338()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_unit_1413_3_339()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_unit_1414_3_340()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_40()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_964_3_72()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1412_1_338()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(28)) jj_scanpos = xsp;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_index_subtype_definition_1407_2_722()
 {
    if (jj_done) return true;
    if (jj_3R_type_mark_2456_3_161()) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_param_2937_5_535()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_interface_list_1476_3_327()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_index_specification_1402_3_628()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3_125()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    return false;
  }

 inline bool jj_3R_param_2936_3_442()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(77)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_param_2937_5_535()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_index_specification_1400_2_627()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_964_3_72()) return true;
    return false;
  }

 inline bool jj_3R_index_specification_1400_2_577()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_index_specification_1400_2_627()) {
    jj_scanpos = xsp;
    if (jj_3R_index_specification_1402_3_628()) return true;
    }
    return false;
  }

 inline bool jj_3R_index_constraint_1395_3_65()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_discrete_range_964_3_72()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_index_constraint_1395_42_159()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_sel_var_list_2871_58_401()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_703_3_90()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(134)) {
    jj_scanpos = xsp;
    if (jj_scan_token(137)) return true;
    }
    return false;
  }

 inline bool jj_3R_incomplete_type_declaration_1387_3_547()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_sel_wave_list_2877_43_546()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_sel_wave_list_2877_4_466()) return true;
    return false;
  }

 inline bool jj_3R_selected_force_assignment_2866_52_316()
 {
    if (jj_done) return true;
    if (jj_3R_inout_stat_2882_3_399()) return true;
    return false;
  }

 inline bool jj_3R_else_stat_2888_28_561()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1374_6_630()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_sequence_of_statement_2128_2_268()) return true;
    return false;
  }

 inline bool jj_3R_ifunc_2910_3_443()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(84)) {
    jj_scanpos = xsp;
    if (jj_scan_token(52)) return true;
    }
    return false;
  }

 inline bool jj_3R_ifunc_2910_2_361()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_ifunc_2910_3_443()) jj_scanpos = xsp;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    if (jj_3R_param_2936_3_442()) return true;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    xsp = jj_scanpos;
    if (jj_3R_ifunc_2910_87_444()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_if_statement_1367_6_629()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSIF_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    if (jj_scan_token(THEN_T)) return true;
    if (jj_3R_sequence_of_statement_2128_2_268()) return true;
    return false;
  }

 inline bool jj_3R_identifier_list_1354_16_362()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_iproc_2901_3_360()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_3R_param_2936_3_442()) return true;
    return false;
  }

 inline bool jj_3R_interface_subprogram_declaration_2896_4_203()
 {
    if (jj_done) return true;
    if (jj_3R_ifunc_2910_2_361()) return true;
    return false;
  }

 inline bool jj_3R_interface_subprogram_declaration_2895_4_202()
 {
    if (jj_done) return true;
    if (jj_3R_iproc_2901_3_360()) return true;
    return false;
  }

 inline bool jj_3R_interface_subprogram_declaration_2895_4_94()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_subprogram_declaration_2895_4_202()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_subprogram_declaration_2896_4_203()) return true;
    }
    return false;
  }

 inline bool jj_3R_if_statement_1359_4_264()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1359_3_125()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_if_statement_1359_4_264()) jj_scanpos = xsp;
    if (jj_scan_token(IF_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    if (jj_scan_token(THEN_T)) return true;
    if (jj_3R_sequence_of_statement_2128_2_268()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_if_statement_1367_6_629()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_if_statement_1374_6_630()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(IF_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_if_statement_1378_46_631()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_identifier_list_1354_4_205()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_identifier_list_1354_16_362()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_else_stat_2888_5_493()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_else_stat_2888_28_561()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_else_stat_2888_4_400()
 {
    if (jj_done) return true;
    Token * xsp;
    if (jj_3R_else_stat_2888_5_493()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_else_stat_2888_5_493()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_identifier_1349_4_171()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASIC_IDENTIFIER)) return true;
    return false;
  }

 inline bool jj_3R_identifier_1348_3_170()
 {
    if (jj_done) return true;
    if (jj_scan_token(EXTENDED_CHARACTER)) return true;
    return false;
  }

 inline bool jj_3R_identifier_1348_3_74()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_identifier_1348_3_170()) {
    jj_scanpos = xsp;
    if (jj_3R_identifier_1349_4_171()) return true;
    }
    return false;
  }

 inline bool jj_3R_group_constituent_list_1322_28_560()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_group_constituent_1316_2_559()) return true;
    return false;
  }

 inline bool jj_3R_inout_stat_2882_3_399()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(53)) {
    jj_scanpos = xsp;
    if (jj_scan_token(75)) return true;
    }
    return false;
  }

 inline bool jj_3R_sel_wave_list_2877_4_466()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_element_2551_2_463()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_703_3_90()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sel_wave_list_2877_43_546()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_selected_waveform_assignment_2860_17_465()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_924_1_420()) return true;
    return false;
  }

 inline bool jj_3R_sel_var_list_2871_3_317()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_703_3_90()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(134)) {
    jj_scanpos = xsp;
    if (jj_scan_token(137)) return true;
    }
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sel_var_list_2871_58_401()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_group_template_declaration_1335_2_68()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_entity_class_entry_list_1038_2_491()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_selected_force_assignment_2865_3_150()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_scan_token(FORCE_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_selected_force_assignment_2866_52_316()) jj_scanpos = xsp;
    if (jj_3R_sel_var_list_2871_3_317()) return true;
    return false;
  }

 inline bool jj_3R_group_declaration_1327_3_398()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_group_constituent_list_1322_3_492()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_124()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_396_3_114()) return true;
    return false;
  }

 inline bool jj_3R_selected_waveform_assignment_2858_3_385()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_selected_waveform_assignment_2860_17_465()) jj_scanpos = xsp;
    if (jj_3R_sel_wave_list_2877_4_466()) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_list_1322_3_492()
 {
    if (jj_done) return true;
    if (jj_3R_group_constituent_1316_2_559()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_group_constituent_list_1322_28_560()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_select_name_2853_3_687()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_1317_4_620()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_682_3_369()) return true;
    return false;
  }

 inline bool jj_3R_select_name_2851_3_686()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_396_3_114()) return true;
    return false;
  }

 inline bool jj_3R_select_name_2851_3_663()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_select_name_2851_3_686()) {
    jj_scanpos = xsp;
    if (jj_3R_select_name_2853_3_687()) return true;
    }
    return false;
  }

 inline bool jj_3_121()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_2786_3_148()) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_1316_2_619()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_1316_2_559()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_group_constituent_1316_2_619()) {
    jj_scanpos = xsp;
    if (jj_3R_group_constituent_1317_4_620()) return true;
    }
    return false;
  }

 inline bool jj_3R_generic_map_aspect_1311_6_73()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_association_list_500_1_169()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_else_wave_list_2823_27_587()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3_123()
 {
    if (jj_done) return true;
    if (jj_3R_selected_force_assignment_2865_3_150()) return true;
    return false;
  }

 inline bool jj_3R_selected_variable_assignment_2844_3_542()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_3R_select_name_2851_3_663()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_sel_var_list_2871_3_317()) return true;
    return false;
  }

 inline bool jj_3R_conditional_force_assignment_2830_15_314()
 {
    if (jj_done) return true;
    if (jj_3R_inout_stat_2882_3_399()) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_list_2797_47_312()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_2786_3_148()) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_force_assignment_2831_13_315()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_3R_else_stat_2888_4_400()) return true;
    return false;
  }

 inline bool jj_3R_generic_list_1306_2_162()
 {
    if (jj_done) return true;
    if (jj_3R_interface_list_1476_3_327()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveform_assignment_2817_26_464()
 {
    if (jj_done) return true;
    if (jj_3R_else_wave_list_2823_3_545()) return true;
    return false;
  }

 inline bool jj_3_39()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_wave_2839_3_280()
 {
    if (jj_done) return true;
    if (jj_3R_selected_waveform_assignment_2858_3_385()) return true;
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_wave_2837_3_132()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_selected_signal_assignment_wave_2837_3_279()) {
    jj_scanpos = xsp;
    if (jj_3R_selected_signal_assignment_wave_2839_3_280()) return true;
    }
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_wave_2837_3_279()
 {
    if (jj_done) return true;
    if (jj_3R_selected_force_assignment_2865_3_150()) return true;
    return false;
  }

 inline bool jj_3R_generic_clause_1301_2_69()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_generic_list_1306_2_162()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_generate_scheme_1296_3_336()
 {
    if (jj_done) return true;
    if (jj_scan_token(IF_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveform_assignment_2816_17_462()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_924_1_420()) return true;
    return false;
  }

 inline bool jj_3R_generate_scheme_1295_1_174()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_generate_scheme_1295_1_335()) {
    jj_scanpos = xsp;
    if (jj_3R_generate_scheme_1296_3_336()) return true;
    }
    return false;
  }

 inline bool jj_3R_generate_scheme_1295_1_335()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_force_assignment_2828_3_149()
 {
    if (jj_done) return true;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_scan_token(FORCE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_conditional_force_assignment_2830_15_314()) jj_scanpos = xsp;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_conditional_force_assignment_2831_13_315()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_else_wave_list_2823_3_545()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_else_wave_list_2823_27_587()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_122()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_force_assignment_2828_3_149()) return true;
    return false;
  }

 inline bool jj_3R_generate_statement_1283_1_78()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_generate_scheme_1295_1_174()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveform_assignment_2815_3_384()
 {
    if (jj_done) return true;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_conditional_waveform_assignment_2816_17_462()) jj_scanpos = xsp;
    if (jj_3R_waveform_element_2551_2_463()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    xsp = jj_scanpos;
    if (jj_3R_conditional_waveform_assignment_2817_26_464()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_function_call_1278_1_110()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_actual_parameter_part_370_4_236()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_wave_2810_3_278()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_waveform_assignment_2815_3_384()) return true;
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_wave_2808_3_131()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_conditional_signal_assignment_wave_2808_3_277()) {
    jj_scanpos = xsp;
    if (jj_3R_conditional_signal_assignment_wave_2810_3_278()) return true;
    }
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_wave_2808_3_277()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_force_assignment_2828_3_149()) return true;
    return false;
  }

 inline bool jj_3R_package_path_name_2803_3_570()
 {
    if (jj_done) return true;
    if (jj_scan_token(AT_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_2786_19_313()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_120()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_list_2797_3_147()) return true;
    return false;
  }

 inline bool jj_3R_file_declaration_1199_67_481()
 {
    if (jj_done) return true;
    if (jj_3R_file_open_information_1214_2_549()) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_list_2797_3_147()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_2786_3_148()) return true;
    if (jj_scan_token(DOT_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_pathname_element_list_2797_47_312()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_relative_pathname_2776_17_624()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_list_2797_3_147()) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_2786_3_148()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_pathname_element_2786_19_313()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_file_open_information_1214_38_592()
 {
    if (jj_done) return true;
    if (jj_3R_inout_stat_2882_3_399()) return true;
    return false;
  }

 inline bool jj_3R_formal_part_1241_11_160()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_formal_designator_1229_5_326()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_neg_list_2781_3_647()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEG_T)) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_full_type_declaration_1246_3_142()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_type_definition_2444_1_588()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_neg_list_2781_2_623()
 {
    if (jj_done) return true;
    Token * xsp;
    if (jj_3R_neg_list_2781_3_647()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_neg_list_2781_3_647()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_119()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_pathname_element_list_2797_3_147()) return true;
    return false;
  }

 inline bool jj_3R_relative_pathname_2776_3_569()
 {
    if (jj_done) return true;
    if (jj_3R_neg_list_2781_2_623()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_relative_pathname_2776_17_624()) jj_scanpos = xsp;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_formal_part_1241_1_66()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_formal_part_1241_11_160()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_absolute_pathname_2771_3_622()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_formal_parameter_list_1236_2_582()
 {
    if (jj_done) return true;
    if (jj_3R_interface_list_1476_3_327()) return true;
    return false;
  }

 inline bool jj_3R_absolute_pathname_2769_2_621()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_pathname_element_list_2797_3_147()) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_absolute_pathname_2769_2_568()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_absolute_pathname_2769_2_621()) {
    jj_scanpos = xsp;
    if (jj_3R_absolute_pathname_2771_3_622()) return true;
    }
    return false;
  }

 inline bool jj_3R_formal_designator_1230_6_413()
 {
    if (jj_done) return true;
    if (jj_scan_token(INTEGER)) return true;
    return false;
  }

 inline bool jj_3R_formal_designator_1229_5_326()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_formal_designator_1229_5_412()) {
    jj_scanpos = xsp;
    if (jj_3R_formal_designator_1230_6_413()) return true;
    }
    return false;
  }

 inline bool jj_3R_formal_designator_1229_5_412()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_2764_4_512()
 {
    if (jj_done) return true;
    if (jj_3R_package_path_name_2803_3_570()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_2763_4_511()
 {
    if (jj_done) return true;
    if (jj_3R_relative_pathname_2776_3_569()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_2762_4_411()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_external_pathname_2762_4_510()) {
    jj_scanpos = xsp;
    if (jj_3R_external_pathname_2763_4_511()) {
    jj_scanpos = xsp;
    if (jj_3R_external_pathname_2764_4_512()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_external_pathname_2762_4_510()
 {
    if (jj_done) return true;
    if (jj_3R_absolute_pathname_2769_2_568()) return true;
    return false;
  }

 inline bool jj_3R_context_declaration_2633_128_189()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_2756_5_509()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_2755_5_508()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_2754_3_410()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sig_stat_2754_3_507()) {
    jj_scanpos = xsp;
    if (jj_3R_sig_stat_2755_5_508()) {
    jj_scanpos = xsp;
    if (jj_3R_sig_stat_2756_5_509()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_sig_stat_2754_3_507()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_file_type_definition_1219_2_659()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_type_mark_2456_3_161()) return true;
    return false;
  }

 inline bool jj_3R_file_open_information_1214_4_591()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_file_open_information_1214_2_549()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_file_open_information_1214_4_591()) jj_scanpos = xsp;
    if (jj_scan_token(IS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_file_open_information_1214_38_592()) jj_scanpos = xsp;
    if (jj_3R_file_logical_name_1209_2_593()) return true;
    return false;
  }

 inline bool jj_3_114()
 {
    if (jj_done) return true;
    if (jj_3R_ttend_2711_3_144()) return true;
    return false;
  }

 inline bool jj_3_118()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_file_logical_name_1209_2_593()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_external_name_2744_2_323()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLSL_T)) return true;
    if (jj_3R_sig_stat_2754_3_410()) return true;
    if (jj_3R_external_pathname_2762_4_411()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    if (jj_scan_token(RSRS_T)) return true;
    return false;
  }

 inline bool jj_3_117()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_instantiation_declaration_2667_73_284()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_2678_4_137()) return true;
    return false;
  }

 inline bool jj_3R_factor_1189_14_467()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOUBLEMULT_T)) return true;
    if (jj_3R_primary_1870_1_386()) return true;
    return false;
  }

 inline bool jj_3R_file_declaration_1199_2_392()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_3R_identifier_list_1354_4_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_file_declaration_1199_67_481()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_116()
 {
    if (jj_done) return true;
    if (jj_3R_block_declarative_item_593_3_146()) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1154_43_273()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_factor_1193_3_295()
 {
    if (jj_done) return true;
    if (jj_3R_logop_1174_3_320()) return true;
    if (jj_3R_primary_1870_1_386()) return true;
    return false;
  }

 inline bool jj_3R_factor_1192_3_294()
 {
    if (jj_done) return true;
    if (jj_scan_token(QQ_T)) return true;
    if (jj_3R_primary_1870_1_386()) return true;
    return false;
  }

 inline bool jj_3R_factor_1191_3_293()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOT_T)) return true;
    if (jj_3R_primary_1870_1_386()) return true;
    return false;
  }

 inline bool jj_3R_factor_1190_3_292()
 {
    if (jj_done) return true;
    if (jj_scan_token(ABS_T)) return true;
    if (jj_3R_primary_1870_1_386()) return true;
    return false;
  }

 inline bool jj_3R_factor_1189_1_141()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_factor_1189_1_291()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1190_3_292()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1191_3_293()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1192_3_294()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1193_3_295()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_factor_1189_1_291()
 {
    if (jj_done) return true;
    if (jj_3R_primary_1870_1_386()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_factor_1189_14_467()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_ttend_2711_12_296()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_enumeration_type_definition_1148_38_251()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_enumeration_literal_1142_2_102()) return true;
    return false;
  }

 inline bool jj_3R_package_instantiation_declaration_2649_71_246()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_2678_4_137()) return true;
    return false;
  }

 inline bool jj_3R_expression_1168_16_152()
 {
    if (jj_done) return true;
    if (jj_3R_logop_1174_3_320()) return true;
    if (jj_3R_relation_2032_3_151()) return true;
    return false;
  }

 inline bool jj_3_115()
 {
    if (jj_done) return true;
    if (jj_3R_label_1537_2_145()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1179_4_409()
 {
    if (jj_done) return true;
    if (jj_scan_token(OR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1178_4_408()
 {
    if (jj_done) return true;
    if (jj_scan_token(XOR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1177_4_407()
 {
    if (jj_done) return true;
    if (jj_scan_token(XNOR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1176_4_406()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1175_4_405()
 {
    if (jj_done) return true;
    if (jj_scan_token(NAND_T)) return true;
    return false;
  }

 inline bool jj_3R_ttend_2711_3_144()
 {
    if (jj_done) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_ttend_2711_12_296()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_package_declaration_2658_55_648()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_2678_4_137()) return true;
    return false;
  }

 inline bool jj_3R_context_declaration_2633_80_188()
 {
    if (jj_done) return true;
    if (jj_3R_libustcont_stats_2642_4_349()) return true;
    return false;
  }

 inline bool jj_3R_logop_1174_3_320()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_logop_1174_3_404()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1175_4_405()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1176_4_406()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1177_4_407()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1178_4_408()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1179_4_409()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_logop_1174_3_404()
 {
    if (jj_done) return true;
    if (jj_scan_token(AND_T)) return true;
    return false;
  }

 inline bool jj_3R_expression_1168_1_61()
 {
    if (jj_done) return true;
    if (jj_3R_relation_2032_3_151()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_expression_1168_16_152()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_exit_statement_1155_3_274()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1154_3_272()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1154_1_129()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_exit_statement_1154_3_272()) jj_scanpos = xsp;
    if (jj_scan_token(EXIT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_exit_statement_1154_43_273()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_exit_statement_1155_3_274()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_gen_interface_list_2683_4_136()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_interface_list_1476_3_327()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_enumeration_type_definition_1148_3_116()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_enumeration_literal_1142_2_102()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_enumeration_type_definition_1148_38_251()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_enumeration_literal_1143_4_218()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_682_3_369()) return true;
    return false;
  }

 inline bool jj_3R_gen_assoc_list_2678_4_137()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_association_list_500_1_169()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_enumeration_literal_1142_2_217()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_enumeration_literal_1142_2_102()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_enumeration_literal_1142_2_217()) {
    jj_scanpos = xsp;
    if (jj_3R_enumeration_literal_1143_4_218()) return true;
    }
    return false;
  }

 inline bool jj_3R_entity_tag_1137_3_685()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_682_3_369()) return true;
    return false;
  }

 inline bool jj_3R_entity_tag_1136_1_684()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_entity_tag_1136_1_661()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_tag_1136_1_684()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_tag_1137_3_685()) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_instantiation_declaration_2667_2_135()
 {
    if (jj_done) return true;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    if (jj_3R_signature_2241_1_245()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_instantiation_declaration_2667_73_284()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1105_28_645()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_entity_designator_1094_1_644()) return true;
    return false;
  }

 inline bool jj_3_38()
 {
    if (jj_done) return true;
    if (jj_3R_process_statement_1952_1_93()) return true;
    return false;
  }

 inline bool jj_3_37()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_assertion_statement_758_1_92()) return true;
    return false;
  }

 inline bool jj_3R_interface_package_declaration_2658_2_95()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_package_declaration_2658_55_648()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_context_ref_2625_31_435()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_package_instantiation_declaration_2649_2_91()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    if (jj_3R_signature_2241_1_245()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_instantiation_declaration_2649_71_246()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_specification_1114_1_488()
 {
    if (jj_done) return true;
    if (jj_3R_entity_name_list_1105_1_554()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_entity_class_1012_1_555()) return true;
    return false;
  }

 inline bool jj_3R_libustcont_stats_2644_6_438()
 {
    if (jj_done) return true;
    if (jj_3R_context_ref_2625_2_348()) return true;
    return false;
  }

 inline bool jj_3R_libustcont_stats_2643_6_437()
 {
    if (jj_done) return true;
    if (jj_3R_library_clause_1542_2_346()) return true;
    return false;
  }

 inline bool jj_3R_entity_designator_1094_18_662()
 {
    if (jj_done) return true;
    if (jj_3R_signature_2241_1_245()) return true;
    return false;
  }

 inline bool jj_3R_libustcont_stats_2642_4_349()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_libustcont_stats_2642_4_436()) {
    jj_scanpos = xsp;
    if (jj_3R_libustcont_stats_2643_6_437()) {
    jj_scanpos = xsp;
    if (jj_3R_libustcont_stats_2644_6_438()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_libustcont_stats_2642_4_436()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2467_2_347()) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1107_3_597()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1106_3_596()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1105_1_595()
 {
    if (jj_done) return true;
    if (jj_3R_entity_designator_1094_1_644()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_entity_name_list_1105_28_645()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_entity_name_list_1105_1_554()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_name_list_1105_1_595()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_name_list_1106_3_596()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_name_list_1107_3_597()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_context_declaration_2633_2_87()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONTEXT_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_context_declaration_2633_80_188()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(32)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_context_declaration_2633_128_189()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declaration_2603_31_683()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_2534_97_263()
 {
    if (jj_done) return true;
    if (jj_3R_timeout_clause_2424_1_382()) return true;
    return false;
  }

 inline bool jj_3R_entity_designator_1094_1_644()
 {
    if (jj_done) return true;
    if (jj_3R_entity_tag_1136_1_661()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_designator_1094_18_662()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_context_ref_2625_2_348()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONTEXT_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_context_ref_2625_31_435()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_part_2616_6_692()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_declarative_item_2609_5_700()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_part_2616_5_682()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_protected_type_declarative_part_2616_6_692()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2611_7_720()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2467_2_347()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2610_7_719()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_526_3_395()) return true;
    return false;
  }

 inline bool jj_3_36()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2649_2_91()) return true;
    return false;
  }

 inline bool jj_3_34()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_505_2_67()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2609_5_718()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_specification_2341_2_364()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2609_5_700()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_declarative_item_2609_5_718()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_declarative_item_2610_7_719()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_declarative_item_2611_7_720()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_35()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1335_2_68()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_entry_list_1038_39_558()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_entity_class_entry_1033_2_557()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_2567_38_681()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_2534_71_262()
 {
    if (jj_done) return true;
    if (jj_3R_condition_clause_814_3_381()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declaration_2599_4_660()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_3R_protected_type_declarative_part_2616_5_682()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_declaration_2603_31_683()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_part_2593_7_691()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_body_declarative_item_2572_5_699()) return true;
    return false;
  }

 inline bool jj_3_112()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_505_2_67()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_part_2593_5_680()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_protected_type_body_declarative_part_2593_7_691()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2587_7_717()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1327_3_398()) return true;
    return false;
  }

 inline bool jj_3_113()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1335_2_68()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2584_7_716()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2467_2_347()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2583_7_715()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_526_3_395()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_entry_1033_21_618()
 {
    if (jj_done) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2581_7_714()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_505_2_67()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2579_7_713()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_401_2_393()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2578_7_712()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1199_2_392()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2577_7_711()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_2503_1_391()) return true;
    return false;
  }

 inline bool jj_3R_entity_declaration_1044_5_366()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2576_7_710()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_877_4_389()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2575_7_709()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_2382_1_388()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2574_7_708()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_2437_1_387()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2573_7_707()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_2271_1_540()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2572_5_706()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2294_1_98()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2572_5_699()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_body_declarative_item_2572_5_706()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2573_7_707()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2574_7_708()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2575_7_709()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2576_7_710()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2577_7_711()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2578_7_712()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2579_7_713()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2581_7_714()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2583_7_715()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2584_7_716()) {
    jj_scanpos = xsp;
    if (jj_3_113()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2587_7_717()) return true;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_wait_statement_2534_43_261()
 {
    if (jj_done) return true;
    if (jj_3R_sensitivity_clause_2112_2_380()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_entry_list_1038_2_491()
 {
    if (jj_done) return true;
    if (jj_3R_entity_class_entry_1033_2_557()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_entity_class_entry_list_1038_39_558()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_entity_class_entry_1033_2_557()
 {
    if (jj_done) return true;
    if (jj_3R_entity_class_1012_1_555()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_class_entry_1033_21_618()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_waveform_element_2551_19_544()
 {
    if (jj_done) return true;
    if (jj_scan_token(AFTER_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_entity_aspect_1005_27_626()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_2563_4_143()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    if (jj_3R_protected_type_body_declarative_part_2593_5_680()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_body_2567_38_681()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_entity_class_1028_3_614()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    return false;
  }

 inline bool jj_3R_waveform_2543_23_519()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_waveform_element_2551_2_463()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1027_3_613()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1026_3_612()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNITS_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1025_3_611()
 {
    if (jj_done) return true;
    if (jj_scan_token(LITERAL_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1024_3_610()
 {
    if (jj_done) return true;
    if (jj_scan_token(LABEL_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1023_3_609()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1022_3_608()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1021_3_607()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1020_3_606()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1019_3_605()
 {
    if (jj_done) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1018_3_604()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1017_3_603()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1016_3_602()
 {
    if (jj_done) return true;
    if (jj_scan_token(FUNCTION_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1015_3_601()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    return false;
  }

 inline bool jj_3R_waveform_element_2551_2_463()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_waveform_element_2551_19_544()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_entity_class_1014_3_600()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1013_3_599()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARCHITECTURE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1012_1_598()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1012_1_555()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_class_1012_1_598()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1013_3_599()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1014_3_600()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1015_3_601()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1016_3_602()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1017_3_603()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1018_3_604()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1019_3_605()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1020_3_606()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1021_3_607()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1022_3_608()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1023_3_609()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1024_3_610()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1025_3_611()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1026_3_612()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1027_3_613()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1028_3_614()) return true;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_waveform_2545_1_422()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNAFFECTED_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_aspect_1007_3_576()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_aspect_1006_3_575()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_waveform_2543_1_332()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_waveform_2543_1_421()) {
    jj_scanpos = xsp;
    if (jj_3R_waveform_2545_1_422()) return true;
    }
    return false;
  }

 inline bool jj_3R_waveform_2543_1_421()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_element_2551_2_463()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_waveform_2543_23_519()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_entity_aspect_1005_1_574()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_aspect_1005_27_626()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_entity_aspect_1005_1_521()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_aspect_1005_1_574()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_aspect_1006_3_575()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_aspect_1007_3_576()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_wait_statement_2534_3_260()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_2534_1_122()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_2534_3_260()) jj_scanpos = xsp;
    if (jj_scan_token(WAIT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_2534_43_261()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_2534_71_262()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_2534_97_263()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_element_declaration_983_3_705()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_list_1354_4_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_33()
 {
    if (jj_done) return true;
    if (jj_3R_choices_703_3_90()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3_32()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    return false;
  }

 inline bool jj_3R_unconstraint_array_definition_2461_53_723()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_index_subtype_definition_1407_2_722()) return true;
    return false;
  }

 inline bool jj_3_31()
 {
    if (jj_done) return true;
    if (jj_3R_range_2008_4_88()) return true;
    return false;
  }

 inline bool jj_3R_element_association_973_2_376()
 {
    if (jj_done) return true;
    if (jj_3R_choices_703_3_90()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_element_association_973_1_247()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_element_association_973_2_376()) jj_scanpos = xsp;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_variable_declaration_2504_3_480()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_discrete_range_967_3_168()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    return false;
  }

 inline bool jj_3R_variable_declaration_2503_1_391()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(101)) jj_scanpos = xsp;
    if (jj_scan_token(VARIABLE_T)) return true;
    if (jj_3R_identifier_list_1354_4_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    xsp = jj_scanpos;
    if (jj_3R_variable_declaration_2504_3_480()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_discrete_range_964_3_167()
 {
    if (jj_done) return true;
    if (jj_3R_range_2008_4_88()) return true;
    return false;
  }

 inline bool jj_3R_discrete_range_964_3_72()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_discrete_range_964_3_167()) {
    jj_scanpos = xsp;
    if (jj_3R_discrete_range_967_3_168()) return true;
    }
    return false;
  }

 inline bool jj_3R_variable_assignment_statement_2498_2_458()
 {
    if (jj_done) return true;
    if (jj_3R_selected_variable_assignment_2844_3_542()) return true;
    return false;
  }

 inline bool jj_3R_guarded_signal_specificatio_959_2_489()
 {
    if (jj_done) return true;
    if (jj_3R_signal_list_2234_2_556()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3_111()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_variable_assignment_statement_2494_1_378()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_variable_assignment_statement_2494_1_457()) {
    jj_scanpos = xsp;
    if (jj_3R_variable_assignment_statement_2498_2_458()) return true;
    }
    return false;
  }

 inline bool jj_3R_variable_assignment_statement_2494_1_457()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_111()) jj_scanpos = xsp;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_use_clause_2467_28_490()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_selected_name_2095_2_434()) return true;
    return false;
  }

 inline bool jj_3R_disconnection_specification_954_1_397()
 {
    if (jj_done) return true;
    if (jj_scan_token(DISCONNECT_T)) return true;
    if (jj_3R_guarded_signal_specificatio_959_2_489()) return true;
    if (jj_scan_token(AFTER_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_direction_949_3_166()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOWNTO_T)) return true;
    return false;
  }

 inline bool jj_3R_constant_declaration_877_74_477()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_direction_948_1_165()
 {
    if (jj_done) return true;
    if (jj_scan_token(TO_T)) return true;
    return false;
  }

 inline bool jj_3R_direction_948_1_71()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_direction_948_1_165()) {
    jj_scanpos = xsp;
    if (jj_3R_direction_949_3_166()) return true;
    }
    return false;
  }

 inline bool jj_3R_designator_943_4_581()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_1721_1_321()) return true;
    return false;
  }

 inline bool jj_3R_designator_942_2_580()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_designator_942_2_536()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_designator_942_2_580()) {
    jj_scanpos = xsp;
    if (jj_3R_designator_943_4_581()) return true;
    }
    return false;
  }

 inline bool jj_3R_use_clause_2467_2_347()
 {
    if (jj_done) return true;
    if (jj_scan_token(USE_T)) return true;
    if (jj_3R_selected_name_2095_2_434()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_use_clause_2467_28_490()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_106()
 {
    if (jj_done) return true;
    if (jj_3R_constraint_901_5_139()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_925_5_573()
 {
    if (jj_done) return true;
    if (jj_scan_token(REJECT_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_925_3_518()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_delay_mechanism_925_5_573()) jj_scanpos = xsp;
    if (jj_scan_token(INERTIAL_T)) return true;
    return false;
  }

 inline bool jj_3R_unconstraint_array_definition_2461_1_704()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_index_subtype_definition_1407_2_722()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_unconstraint_array_definition_2461_53_723()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_924_1_517()
 {
    if (jj_done) return true;
    if (jj_scan_token(TRANSPORT_T)) return true;
    return false;
  }

 inline bool jj_3_30()
 {
    if (jj_done) return true;
    if (jj_3R_context_declaration_2633_2_87()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_924_1_420()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_delay_mechanism_924_1_517()) {
    jj_scanpos = xsp;
    if (jj_3R_delay_mechanism_925_3_518()) return true;
    }
    return false;
  }

 inline bool jj_3R_type_mark_2456_3_161()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_subtype_indication_2398_58_193()
 {
    if (jj_done) return true;
    if (jj_3R_constraint_901_5_139()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2451_3_643()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_declaration_2599_4_660()) return true;
    return false;
  }

 inline bool jj_3_29()
 {
    if (jj_done) return true;
    if (jj_3R_index_constraint_1395_3_65()) return true;
    return false;
  }

 inline bool jj_3R_context_item_913_3_187()
 {
    if (jj_done) return true;
    if (jj_3R_context_ref_2625_2_348()) return true;
    return false;
  }

 inline bool jj_3_28()
 {
    if (jj_done) return true;
    if (jj_3R_range_constraint_2017_1_86()) return true;
    return false;
  }

 inline bool jj_3_110()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_body_2563_4_143()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2447_3_642()
 {
    if (jj_done) return true;
    if (jj_3R_file_type_definition_1219_2_659()) return true;
    return false;
  }

 inline bool jj_3R_context_item_912_3_186()
 {
    if (jj_done) return true;
    if (jj_3R_context_declaration_2633_2_87()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2446_3_641()
 {
    if (jj_done) return true;
    if (jj_3R_access_type_definition_349_3_658()) return true;
    return false;
  }

 inline bool jj_3R_context_item_911_3_185()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2467_2_347()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2445_3_640()
 {
    if (jj_done) return true;
    if (jj_3R_composite_type_definition_752_2_657()) return true;
    return false;
  }

 inline bool jj_3R_context_item_910_2_184()
 {
    if (jj_done) return true;
    if (jj_3R_library_clause_1542_2_346()) return true;
    return false;
  }

 inline bool jj_3R_context_item_910_2_85()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_context_item_910_2_184()) {
    jj_scanpos = xsp;
    if (jj_3R_context_item_911_3_185()) {
    jj_scanpos = xsp;
    if (jj_3R_context_item_912_3_186()) {
    jj_scanpos = xsp;
    if (jj_3R_context_item_913_3_187()) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_type_definition_2444_1_639()
 {
    if (jj_done) return true;
    if (jj_3R_scalar_type_definition_2074_1_656()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2444_1_588()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_type_definition_2444_1_639()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_2445_3_640()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_2446_3_641()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_2447_3_642()) {
    jj_scanpos = xsp;
    if (jj_3_110()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_2451_3_643()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_constraint_904_5_286()
 {
    if (jj_done) return true;
    if (jj_3R_index_constraint_1395_3_65()) return true;
    return false;
  }

 inline bool jj_3R_subtype_indication_2398_45_192()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_type_declaration_2439_3_476()
 {
    if (jj_done) return true;
    if (jj_3R_incomplete_type_declaration_1387_3_547()) return true;
    return false;
  }

 inline bool jj_3R_constraint_901_5_139()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_constraint_901_5_285()) {
    jj_scanpos = xsp;
    if (jj_3R_constraint_904_5_286()) return true;
    }
    return false;
  }

 inline bool jj_3R_constraint_901_5_285()
 {
    if (jj_done) return true;
    if (jj_3R_range_constraint_2017_1_86()) return true;
    return false;
  }

 inline bool jj_3_109()
 {
    if (jj_done) return true;
    if (jj_3R_full_type_declaration_1246_3_142()) return true;
    return false;
  }

 inline bool jj_3R_type_declaration_2437_1_387()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_109()) {
    jj_scanpos = xsp;
    if (jj_3R_type_declaration_2439_3_476()) return true;
    }
    return false;
  }

 inline bool jj_3_27()
 {
    if (jj_done) return true;
    if (jj_3R_context_item_910_2_85()) return true;
    return false;
  }

 inline bool jj_3R_type_conversion_2432_3_111()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_108()
 {
    if (jj_done) return true;
    if (jj_3R_multiplying_operation_1624_1_140()) return true;
    if (jj_3R_factor_1189_1_141()) return true;
    return false;
  }

 inline bool jj_3R_constraint_array_definition_890_1_703()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_3R_index_constraint_1395_3_65()) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    return false;
  }

 inline bool jj_3R_timeout_clause_2424_1_382()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_term_2419_2_134()
 {
    if (jj_done) return true;
    if (jj_3R_factor_1189_1_141()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_108()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_target_2414_3_256()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_396_3_114()) return true;
    return false;
  }

 inline bool jj_3R_constant_declaration_877_4_389()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    if (jj_3R_identifier_list_1354_4_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_constant_declaration_877_74_477()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_target_2413_2_119()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_target_2413_2_255()) {
    jj_scanpos = xsp;
    if (jj_3R_target_2414_3_256()) return true;
    }
    return false;
  }

 inline bool jj_3_107()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_target_2413_2_255()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3_26()
 {
    if (jj_done) return true;
    if (jj_3R_component_configuration_708_5_84()) return true;
    return false;
  }

 inline bool jj_3R_suffix_2408_3_228()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_suffix_2407_3_227()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_1721_1_321()) return true;
    return false;
  }

 inline bool jj_3R_suffix_2406_3_226()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_682_3_369()) return true;
    return false;
  }

 inline bool jj_3R_configuration_specification_872_1_396()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_component_specification_747_1_181()) return true;
    if (jj_3R_binding_indication_567_2_344()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_configuration_item_866_4_579()
 {
    if (jj_done) return true;
    if (jj_3R_block_configuration_585_5_345()) return true;
    return false;
  }

 inline bool jj_3R_suffix_2404_1_105()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_suffix_2404_1_225()) {
    jj_scanpos = xsp;
    if (jj_3R_suffix_2406_3_226()) {
    jj_scanpos = xsp;
    if (jj_3R_suffix_2407_3_227()) {
    jj_scanpos = xsp;
    if (jj_3R_suffix_2408_3_228()) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_suffix_2404_1_225()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3_105()
 {
    if (jj_done) return true;
    if (jj_3R_reslution_indication_2391_3_138()) return true;
    return false;
  }

 inline bool jj_3R_configuration_item_864_2_578()
 {
    if (jj_done) return true;
    if (jj_3R_component_configuration_708_5_84()) return true;
    return false;
  }

 inline bool jj_3R_configuration_item_864_2_523()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_configuration_item_864_2_578()) {
    jj_scanpos = xsp;
    if (jj_3R_configuration_item_866_4_579()) return true;
    }
    return false;
  }

 inline bool jj_3R_subtype_indication_2398_3_89()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_105()) jj_scanpos = xsp;
    if (jj_3R_subtype_indication_2398_45_192()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_subtype_indication_2398_45_192()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_subtype_indication_2398_58_193()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_reslution_indication_2391_3_138()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_subtype_declaration_2382_1_388()
 {
    if (jj_done) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_statement_part_2377_4_637()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2133_5_117()) return true;
    return false;
  }

 inline bool jj_3_25()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_statement_part_2377_3_584()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_subprogram_statement_part_2377_4_637()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_configuration_declaration_838_2_367()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveforms_831_7_334()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveforms_829_7_333()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_waveform_2543_1_332()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2366_3_539()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_formal_parameter_list_1236_2_582()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveforms_828_1_173()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_2543_1_332()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_conditional_waveforms_829_7_333()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_conditional_waveforms_831_7_334()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_823_2_75()
 {
    if (jj_done) return true;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_3R_options_1726_3_172()) return true;
    if (jj_3R_conditional_waveforms_828_1_173()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2355_4_538()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(84)) {
    jj_scanpos = xsp;
    if (jj_scan_token(52)) return true;
    }
    return false;
  }

 inline bool jj_3_104()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_2678_4_137()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2355_2_451()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_2355_4_538()) jj_scanpos = xsp;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_designator_942_2_536()) return true;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_2366_3_539()) jj_scanpos = xsp;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_type_mark_2456_3_161()) return true;
    return false;
  }

 inline bool jj_3_103()
 {
    if (jj_done) return true;
    if (jj_3R_gen_interface_list_2683_4_136()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2349_5_537()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_interface_list_1476_3_327()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_condition_clause_814_3_381()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNTIL_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    return false;
  }

 inline bool jj_3R_condition_809_3_83()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3_24()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_procedure_call_statement_763_1_82()) return true;
    return false;
  }

 inline bool jj_3_23()
 {
    if (jj_done) return true;
    if (jj_3R_component_instantiation_statement_730_2_81()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2341_2_364()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_2341_2_450()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_specification_2355_2_451()) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_specification_2341_2_450()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    if (jj_3R_designator_942_2_536()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_2349_5_537()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3_103()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3_104()) jj_scanpos = xsp;
    if (jj_3R_param_2936_3_442()) return true;
    return false;
  }

 inline bool jj_3_22()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_signal_assignment_statement_768_1_80()) return true;
    return false;
  }

 inline bool jj_3R_null_792_12_79()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_21()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_792_12_79()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(ASSERT_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_kind_2335_2_638()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(46)) {
    jj_scanpos = xsp;
    if (jj_scan_token(80)) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_declarative_part_2330_4_636()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declarative_item_2309_1_655()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_part_2330_3_583()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_subprogram_declarative_part_2330_4_636()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_null_784_12_77()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_19()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_784_12_77()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    return false;
  }

 inline bool jj_3_101()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_505_2_67()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2325_3_675()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1327_3_398()) return true;
    return false;
  }

 inline bool jj_3R_null_781_12_76()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_18()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_781_12_76()) jj_scanpos = xsp;
    if (jj_scan_token(BLOCK_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2321_3_674()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2467_2_347()) return true;
    return false;
  }

 inline bool jj_3_102()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1335_2_68()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2320_3_673()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_526_3_395()) return true;
    return false;
  }

 inline bool jj_3_20()
 {
    if (jj_done) return true;
    if (jj_3R_generate_statement_1283_1_78()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2318_1_672()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_505_2_67()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2316_3_671()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_401_2_393()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2315_3_670()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1199_2_392()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2314_3_669()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_2503_1_391()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_body_2284_33_586()
 {
    if (jj_done) return true;
    if (jj_3R_designator_942_2_536()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2313_3_668()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_877_4_389()) return true;
    return false;
  }

 inline bool jj_3_17()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_823_2_75()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2312_3_667()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_2382_1_388()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2311_4_666()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_2271_1_540()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2310_2_665()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_2437_1_387()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2309_1_664()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2294_1_98()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2309_1_655()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_declarative_item_2309_1_664()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2310_2_665()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2311_4_666()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2312_3_667()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2313_3_668()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2314_3_669()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2315_3_670()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2316_3_671()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2318_1_672()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2320_3_673()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2321_3_674()) {
    jj_scanpos = xsp;
    if (jj_3_102()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2325_3_675()) return true;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_772_3_176()
 {
    if (jj_done) return true;
    if (jj_3R_selected_signal_assignment_2100_3_337()) return true;
    return false;
  }

 inline bool jj_3R_signal_declaration_2218_89_479()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_770_1_175()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_823_2_75()) return true;
    return false;
  }

 inline bool jj_3_16()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_100()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_instantiation_declaration_2667_2_135()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_1_2303_2_365()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_1_2303_2_452()) {
    jj_scanpos = xsp;
    if (jj_scan_token(137)) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_1_2303_2_452()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_2271_1_540()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_768_1_80()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_16()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_concurrent_signal_assignment_statement_770_1_175()) {
    jj_scanpos = xsp;
    if (jj_3R_concurrent_signal_assignment_statement_772_3_176()) return true;
    }
    return false;
  }

 inline bool jj_3_15()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_concurrent_procedure_call_statement_763_1_82()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_15()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_3R_procedure_call_1912_2_180()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declaration_2297_1_209()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_specification_2341_2_364()) return true;
    if (jj_3R_subprogram_1_2303_2_365()) return true;
    return false;
  }

 inline bool jj_3_14()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_13()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1311_6_73()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_body_2284_11_585()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_kind_2335_2_638()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declaration_2294_1_208()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_instantiation_declaration_2667_2_135()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declaration_2294_1_98()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_declaration_2294_1_208()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declaration_2297_1_209()) return true;
    }
    return false;
  }

 inline bool jj_3_99()
 {
    if (jj_done) return true;
    if (jj_3R_adding_operator_389_3_133()) return true;
    if (jj_3R_term_2419_2_134()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_assertion_statement_758_1_92()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_14()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_3R_assertion_475_4_196()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_composite_type_definition_753_3_679()
 {
    if (jj_done) return true;
    if (jj_3R_record_type_definition_2022_5_690()) return true;
    return false;
  }

 inline bool jj_3R_composite_type_definition_752_2_678()
 {
    if (jj_done) return true;
    if (jj_3R_array_type_definition_466_2_689()) return true;
    return false;
  }

 inline bool jj_3R_composite_type_definition_752_2_657()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_composite_type_definition_752_2_678()) {
    jj_scanpos = xsp;
    if (jj_3R_composite_type_definition_753_3_679()) return true;
    }
    return false;
  }

 inline bool jj_3R_component_instantiation_statement_742_11_179()
 {
    if (jj_done) return true;
    if (jj_3R_port_map_aspect_1865_2_341()) return true;
    return false;
  }

 inline bool jj_3R_component_instantiation_statement_741_11_178()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1311_6_73()) return true;
    return false;
  }

 inline bool jj_3R_signal_declaration_2218_68_478()
 {
    if (jj_done) return true;
    if (jj_3R_signal_kind_2228_3_548()) return true;
    return false;
  }

 inline bool jj_3R_component_declaration_723_27_487()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_simple_expression_2250_35_164()
 {
    if (jj_done) return true;
    if (jj_3R_adding_operator_389_3_133()) return true;
    if (jj_3R_term_2419_2_134()) return true;
    return false;
  }

 inline bool jj_3R_component_specification_747_1_181()
 {
    if (jj_done) return true;
    if (jj_3R_instantiation_list_1419_3_343()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_body_2271_1_540()
 {
    if (jj_done) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_subprogram_declarative_part_2330_3_583()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_subprogram_statement_part_2377_3_584()) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_body_2284_11_585()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_body_2284_33_586()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_component_instantiation_statement_730_2_81()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_instantiation_unit_1412_1_177()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_instantiation_statement_741_11_178()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_instantiation_statement_742_11_179()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_string_literal_2265_1_373()
 {
    if (jj_done) return true;
    if (jj_scan_token(STRINGLITERAL)) return true;
    return false;
  }

 inline bool jj_3R_component_declaration_718_5_486()
 {
    if (jj_done) return true;
    if (jj_3R_port_clause_1855_4_553()) return true;
    return false;
  }

 inline bool jj_3R_component_declaration_717_5_485()
 {
    if (jj_done) return true;
    if (jj_3R_generic_clause_1301_2_69()) return true;
    return false;
  }

 inline bool jj_3R_signature_2242_15_453()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_component_configuration_710_11_183()
 {
    if (jj_done) return true;
    if (jj_3R_block_configuration_585_5_345()) return true;
    return false;
  }

 inline bool jj_3R_component_configuration_709_11_182()
 {
    if (jj_done) return true;
    if (jj_3R_binding_indication_567_2_344()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_choices_703_15_195()
 {
    if (jj_done) return true;
    if (jj_scan_token(BAR_T)) return true;
    if (jj_3R_choice_687_4_194()) return true;
    return false;
  }

 inline bool jj_3R_simple_expression_2250_3_163()
 {
    if (jj_done) return true;
    if (jj_3R_sign_2188_2_328()) return true;
    return false;
  }

 inline bool jj_3R_component_declaration_715_2_394()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_declaration_717_5_485()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_declaration_718_5_486()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_component_declaration_723_27_487()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_simple_expression_2250_1_70()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_simple_expression_2250_3_163()) jj_scanpos = xsp;
    if (jj_3R_term_2419_2_134()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_simple_expression_2250_35_164()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_component_configuration_708_5_84()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_component_specification_747_1_181()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_configuration_709_11_182()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_configuration_710_11_183()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_signature_2243_4_375()
 {
    if (jj_done) return true;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_signal_list_2234_12_646()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_signature_2242_4_374()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_signature_2242_15_453()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_12()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_964_3_72()) return true;
    return false;
  }

 inline bool jj_3R_signature_2241_1_245()
 {
    if (jj_done) return true;
    if (jj_scan_token(LBRACKET_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signature_2242_4_374()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_signature_2243_4_375()) jj_scanpos = xsp;
    if (jj_scan_token(RBRACKET_T)) return true;
    return false;
  }

 inline bool jj_3R_choices_703_3_90()
 {
    if (jj_done) return true;
    if (jj_3R_choice_687_4_194()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_choices_703_15_195()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_signal_list_2236_3_617()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3_11()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2250_1_70()) return true;
    return false;
  }

 inline bool jj_3R_signal_list_2235_3_616()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_choice_697_5_354()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_list_2234_2_615()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_signal_list_2234_12_646()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_signal_list_2234_2_556()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_list_2234_2_615()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_list_2235_3_616()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_list_2236_3_617()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_10()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2250_1_70()) return true;
    if (jj_3R_direction_948_1_71()) return true;
    return false;
  }

 inline bool jj_3R_choice_696_4_353()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_signal_kind_2229_5_590()
 {
    if (jj_done) return true;
    if (jj_scan_token(BUS_T)) return true;
    return false;
  }

 inline bool jj_3R_choice_693_4_352()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_964_3_72()) return true;
    return false;
  }

 inline bool jj_3R_signal_kind_2228_3_589()
 {
    if (jj_done) return true;
    if (jj_scan_token(REGISTER_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_kind_2228_3_548()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_kind_2228_3_589()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_kind_2229_5_590()) return true;
    }
    return false;
  }

 inline bool jj_3R_choice_690_4_351()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2250_1_70()) return true;
    return false;
  }

 inline bool jj_3R_choice_687_4_194()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_choice_687_4_350()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_690_4_351()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_693_4_352()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_696_4_353()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_697_5_354()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_choice_687_4_350()
 {
    if (jj_done) return true;
    if (jj_3R_range_2008_4_88()) return true;
    return false;
  }

 inline bool jj_3R_case_statement_662_28_634()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_character_literal_682_3_369()
 {
    if (jj_done) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_signal_declaration_2218_1_390()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    if (jj_3R_identifier_list_1354_4_205()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_declaration_2218_68_478()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_declaration_2218_89_479()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_98()
 {
    if (jj_done) return true;
    if (jj_3R_selected_signal_assignment_wave_2837_3_132()) return true;
    return false;
  }

 inline bool jj_3R_case_statement_alternative_671_3_632()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_703_3_90()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    if (jj_3R_sequence_of_statement_2128_2_268()) return true;
    return false;
  }

 inline bool jj_3_97()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_wave_2808_3_131()) return true;
    return false;
  }

 inline bool jj_3R_case_statement_661_11_633()
 {
    if (jj_done) return true;
    if (jj_3R_case_statement_alternative_671_3_632()) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2203_3_541()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_924_1_420()) return true;
    return false;
  }

 inline bool jj_3_96()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2201_3_456()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_96()) jj_scanpos = xsp;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_signal_assignment_statement_2203_3_541()) jj_scanpos = xsp;
    if (jj_3R_waveform_2543_1_332()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2198_3_455()
 {
    if (jj_done) return true;
    if (jj_3R_selected_signal_assignment_wave_2837_3_132()) return true;
    return false;
  }

 inline bool jj_3R_shift_expression_2174_26_402()
 {
    if (jj_done) return true;
    if (jj_3R_shift_operator_2178_3_494()) return true;
    if (jj_3R_simple_expression_2250_1_70()) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2195_3_377()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_assignment_statement_2195_3_454()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_assignment_statement_2198_3_455()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_assignment_statement_2201_3_456()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2195_3_454()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_wave_2808_3_131()) return true;
    return false;
  }

 inline bool jj_3R_sign_2189_4_417()
 {
    if (jj_done) return true;
    if (jj_scan_token(MINUS_T)) return true;
    return false;
  }

 inline bool jj_3R_case_statement_653_3_265()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_sign_2188_2_328()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sign_2188_2_416()) {
    jj_scanpos = xsp;
    if (jj_3R_sign_2189_4_417()) return true;
    }
    return false;
  }

 inline bool jj_3R_sign_2188_2_416()
 {
    if (jj_done) return true;
    if (jj_scan_token(PLUS_T)) return true;
    return false;
  }

 inline bool jj_3R_case_statement_653_1_126()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_case_statement_653_3_265()) jj_scanpos = xsp;
    if (jj_scan_token(CASE_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_case_statement_alternative_671_3_632()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_case_statement_661_11_633()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(CASE_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_case_statement_662_28_634()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2183_5_567()
 {
    if (jj_done) return true;
    if (jj_scan_token(ROR_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2182_5_566()
 {
    if (jj_done) return true;
    if (jj_scan_token(ROL_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2181_5_565()
 {
    if (jj_done) return true;
    if (jj_scan_token(SRA_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2180_5_564()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLA_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2179_5_563()
 {
    if (jj_done) return true;
    if (jj_scan_token(SRL_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2178_3_562()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLL_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2178_3_494()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_shift_operator_2178_3_562()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2179_5_563()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2180_5_564()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2181_5_565()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2182_5_566()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2183_5_567()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_block_specification_629_12_522()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_index_specification_1400_2_577()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_expression_2174_2_318()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2250_1_70()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_shift_expression_2174_26_402()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_sequential_statement_2169_5_254()
 {
    if (jj_done) return true;
    if (jj_3R_null_statement_1698_1_379()) return true;
    return false;
  }

 inline bool jj_3_95()
 {
    if (jj_done) return true;
    if (jj_3R_return_statement_2068_1_130()) return true;
    return false;
  }

 inline bool jj_3_9()
 {
    if (jj_done) return true;
    if (jj_3R_generic_clause_1301_2_69()) return true;
    return false;
  }

 inline bool jj_3_94()
 {
    if (jj_done) return true;
    if (jj_3R_exit_statement_1154_1_129()) return true;
    return false;
  }

 inline bool jj_3R_block_specification_629_3_431()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_block_specification_629_12_522()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_93()
 {
    if (jj_done) return true;
    if (jj_3R_next_statement_1685_1_128()) return true;
    return false;
  }

 inline bool jj_3R_null_2145_19_123()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_88()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_2145_19_123()) jj_scanpos = xsp;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    return false;
  }

 inline bool jj_3_92()
 {
    if (jj_done) return true;
    if (jj_3R_loop_statement_1586_1_127()) return true;
    return false;
  }

 inline bool jj_3_91()
 {
    if (jj_done) return true;
    if (jj_3R_case_statement_653_1_126()) return true;
    return false;
  }

 inline bool jj_3_90()
 {
    if (jj_done) return true;
    if (jj_3R_if_statement_1359_3_125()) return true;
    return false;
  }

 inline bool jj_3_89()
 {
    if (jj_done) return true;
    if (jj_3R_procedure_call_statement_1918_1_124()) return true;
    return false;
  }

 inline bool jj_3R_null_2133_18_118()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_612_3_311()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1327_3_398()) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_2145_5_253()
 {
    if (jj_done) return true;
    if (jj_3R_variable_assignment_statement_2494_1_378()) return true;
    return false;
  }

 inline bool jj_3_7()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_505_2_67()) return true;
    return false;
  }

 inline bool jj_3_84()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_2133_18_118()) jj_scanpos = xsp;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    return false;
  }

 inline bool jj_3_87()
 {
    if (jj_done) return true;
    if (jj_3R_wait_statement_2534_1_122()) return true;
    return false;
  }

 inline bool jj_3_8()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1335_2_68()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_608_3_310()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2467_2_347()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_607_3_309()
 {
    if (jj_done) return true;
    if (jj_3R_disconnection_specification_954_1_397()) return true;
    return false;
  }

 inline bool jj_3_86()
 {
    if (jj_done) return true;
    if (jj_3R_report_statement_2055_1_121()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_606_3_308()
 {
    if (jj_done) return true;
    if (jj_3R_configuration_specification_872_1_396()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_605_3_307()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_526_3_395()) return true;
    return false;
  }

 inline bool jj_3_85()
 {
    if (jj_done) return true;
    if (jj_3R_assertion_statement_486_3_120()) return true;
    return false;
  }

 inline bool jj_3R_selected_waveforms_2107_32_520()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_waveform_2543_1_332()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_703_3_90()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_601_3_305()
 {
    if (jj_done) return true;
    if (jj_3R_component_declaration_715_2_394()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_603_1_306()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_505_2_67()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_600_3_304()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_401_2_393()) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_2133_5_117()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sequential_statement_2133_5_252()) {
    jj_scanpos = xsp;
    if (jj_3_85()) {
    jj_scanpos = xsp;
    if (jj_3_86()) {
    jj_scanpos = xsp;
    if (jj_3_87()) {
    jj_scanpos = xsp;
    if (jj_3R_sequential_statement_2145_5_253()) {
    jj_scanpos = xsp;
    if (jj_3_89()) {
    jj_scanpos = xsp;
    if (jj_3_90()) {
    jj_scanpos = xsp;
    if (jj_3_91()) {
    jj_scanpos = xsp;
    if (jj_3_92()) {
    jj_scanpos = xsp;
    if (jj_3_93()) {
    jj_scanpos = xsp;
    if (jj_3_94()) {
    jj_scanpos = xsp;
    if (jj_3_95()) {
    jj_scanpos = xsp;
    if (jj_3R_sequential_statement_2169_5_254()) return true;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_sequential_statement_2133_5_252()
 {
    if (jj_done) return true;
    if (jj_3R_signal_assignment_statement_2195_3_377()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_599_3_303()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1199_2_392()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_598_3_302()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_2503_1_391()) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_list_2123_12_524()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_597_3_301()
 {
    if (jj_done) return true;
    if (jj_3R_signal_declaration_2218_1_390()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_596_3_300()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_877_4_389()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_595_3_299()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_2382_1_388()) return true;
    return false;
  }

 inline bool jj_3R_block_configuration_587_11_433()
 {
    if (jj_done) return true;
    if (jj_3R_configuration_item_864_2_523()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_594_3_298()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_2437_1_387()) return true;
    return false;
  }

 inline bool jj_3R_block_configuration_586_11_432()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2467_2_347()) return true;
    return false;
  }

 inline bool jj_3_83()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2133_5_117()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_593_3_146()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_block_declarative_item_593_3_297()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_594_3_298()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_595_3_299()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_596_3_300()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_597_3_301()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_598_3_302()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_599_3_303()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_600_3_304()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_601_3_305()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_603_1_306()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_605_3_307()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_606_3_308()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_607_3_309()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_608_3_310()) {
    jj_scanpos = xsp;
    if (jj_3_8()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_612_3_311()) return true;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_block_declarative_item_593_3_297()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2294_1_98()) return true;
    return false;
  }

 inline bool jj_3R_sequence_of_statement_2128_2_268()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_83()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_block_configuration_585_5_345()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_block_specification_629_3_431()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_block_configuration_586_11_432()) { jj_scanpos = xsp; break; }
    }
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_block_configuration_587_11_433()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_list_2123_2_440()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sensitivity_list_2123_12_524()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_attribute_name_521_68_250()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_list_2121_3_357()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sensitivity_list_2121_3_439()) {
    jj_scanpos = xsp;
    if (jj_3R_sensitivity_list_2123_2_440()) return true;
    }
    return false;
  }

 inline bool jj_3R_sensitivity_list_2121_3_439()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_return_statement_2068_53_276()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_clause_2112_2_380()
 {
    if (jj_done) return true;
    if (jj_scan_token(ON_T)) return true;
    if (jj_3R_sensitivity_list_2121_3_357()) return true;
    return false;
  }

 inline bool jj_3R_bit_string_literal_574_3_100()
 {
    if (jj_done) return true;
    if (jj_scan_token(BIT_STRING_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_selected_waveforms_2107_2_423()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_2543_1_332()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_703_3_90()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_selected_waveforms_2107_32_520()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_binding_indication_569_4_430()
 {
    if (jj_done) return true;
    if (jj_3R_port_map_aspect_1865_2_341()) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_568_4_429()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1311_6_73()) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_567_4_428()
 {
    if (jj_done) return true;
    if (jj_scan_token(USE_T)) return true;
    if (jj_3R_entity_aspect_1005_1_521()) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_567_2_344()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_binding_indication_567_4_428()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_binding_indication_568_4_429()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_binding_indication_569_4_430()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_2100_3_337()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    if (jj_3R_target_2413_2_119()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_3R_options_1726_3_172()) return true;
    if (jj_3R_selected_waveforms_2107_2_423()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2076_26_688()
 {
    if (jj_done) return true;
    if (jj_3R_physical_type_definition_1843_9_694()) return true;
    return false;
  }

 inline bool jj_3R_attribute_name_521_46_249()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_selected_name_2095_2_434()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_suffix_2404_1_105()) return true;
    return false;
  }

 inline bool jj_3R_secondary_unit_declaration_2087_1_721()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(EQU_T)) return true;
    if (jj_3R_physical_literal_1838_2_108()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_82()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_type_definition_1148_3_116()) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2076_3_677()
 {
    if (jj_done) return true;
    if (jj_3R_range_constraint_2017_1_86()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_scalar_type_definition_2076_26_688()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2074_1_676()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_type_definition_1148_3_116()) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2074_1_656()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_scalar_type_definition_2074_1_676()) {
    jj_scanpos = xsp;
    if (jj_3R_scalar_type_definition_2076_3_677()) return true;
    }
    return false;
  }

 inline bool jj_3R_assertion_475_63_356()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEVERITY_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_return_statement_2068_3_275()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_return_statement_2068_1_130()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_return_statement_2068_3_275()) jj_scanpos = xsp;
    if (jj_scan_token(RETURN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_return_statement_2068_53_276()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_report_statement_2057_11_259()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEVERITY_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_attribute_specification_526_3_395()
 {
    if (jj_done) return true;
    if (jj_scan_token(ATTRIBUTE_T)) return true;
    if (jj_3R_attribute_designator_514_3_230()) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_entity_specification_1114_1_488()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_association_list_500_26_330()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_association_element_494_2_329()) return true;
    return false;
  }

 inline bool jj_3R_attribute_name_521_3_115()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(85)) {
    jj_scanpos = xsp;
    if (jj_3R_attribute_name_521_46_249()) return true;
    }
    xsp = jj_scanpos;
    if (jj_3R_attribute_name_521_68_250()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_report_statement_2055_3_258()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_2032_25_319()
 {
    if (jj_done) return true;
    if (jj_3R_relation_operator_2037_2_403()) return true;
    if (jj_3R_shift_expression_2174_2_318()) return true;
    return false;
  }

 inline bool jj_3R_report_statement_2055_1_121()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_report_statement_2055_3_258()) jj_scanpos = xsp;
    if (jj_scan_token(REPORT_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    xsp = jj_scanpos;
    if (jj_3R_report_statement_2057_11_259()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_designator_515_5_372()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_designator_514_3_230()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_attribute_designator_514_3_371()) {
    jj_scanpos = xsp;
    if (jj_3R_attribute_designator_515_5_372()) return true;
    }
    return false;
  }

 inline bool jj_3R_attribute_designator_514_3_371()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2048_3_506()
 {
    if (jj_done) return true;
    if (jj_scan_token(QNEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2047_3_505()
 {
    if (jj_done) return true;
    if (jj_scan_token(QEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2046_3_504()
 {
    if (jj_done) return true;
    if (jj_scan_token(QL_T)) return true;
    return false;
  }

 inline bool jj_3R_record_type_definition_2027_21_698()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2045_3_503()
 {
    if (jj_done) return true;
    if (jj_scan_token(QG_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2044_3_502()
 {
    if (jj_done) return true;
    if (jj_scan_token(QLT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2043_3_501()
 {
    if (jj_done) return true;
    if (jj_scan_token(QGT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2042_3_500()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOTEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2041_3_499()
 {
    if (jj_done) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2040_3_498()
 {
    if (jj_done) return true;
    if (jj_scan_token(GREATERTHAN_T)) return true;
    return false;
  }

 inline bool jj_3_6()
 {
    if (jj_done) return true;
    if (jj_3R_formal_part_1241_1_66()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_declaration_505_2_67()
 {
    if (jj_done) return true;
    if (jj_scan_token(ATTRIBUTE_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_type_mark_2456_3_161()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2039_3_497()
 {
    if (jj_done) return true;
    if (jj_scan_token(EQU_T)) return true;
    return false;
  }

 inline bool jj_3R_assertion_475_31_355()
 {
    if (jj_done) return true;
    if (jj_scan_token(REPORT_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2038_3_496()
 {
    if (jj_done) return true;
    if (jj_scan_token(GT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2037_2_403()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_relation_operator_2037_2_495()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2038_3_496()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2039_3_497()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2040_3_498()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2041_3_499()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2042_3_500()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2043_3_501()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2044_3_502()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2045_3_503()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2046_3_504()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2047_3_505()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2048_3_506()) return true;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_relation_operator_2037_2_495()
 {
    if (jj_done) return true;
    if (jj_scan_token(LT_T)) return true;
    return false;
  }

 inline bool jj_3R_association_list_500_1_169()
 {
    if (jj_done) return true;
    if (jj_3R_association_element_494_2_329()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_association_list_500_26_330()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_relation_2032_3_151()
 {
    if (jj_done) return true;
    if (jj_3R_shift_expression_2174_2_318()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_relation_2032_25_319()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_record_type_definition_2024_9_697()
 {
    if (jj_done) return true;
    if (jj_3R_element_declaration_983_3_705()) return true;
    return false;
  }

 inline bool jj_3R_association_element_494_3_418()
 {
    if (jj_done) return true;
    if (jj_3R_formal_part_1241_1_66()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_association_element_494_2_329()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_association_element_494_3_418()) jj_scanpos = xsp;
    if (jj_3R_actual_part_376_2_419()) return true;
    return false;
  }

 inline bool jj_3R_record_type_definition_2022_5_690()
 {
    if (jj_done) return true;
    if (jj_scan_token(RECORD_T)) return true;
    Token * xsp;
    if (jj_3R_record_type_definition_2024_9_697()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_record_type_definition_2024_9_697()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(RECORD_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_record_type_definition_2027_21_698()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_assertion_statement_486_5_257()
 {
    if (jj_done) return true;
    if (jj_3R_label_1537_2_145()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_assertion_statement_486_3_120()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_assertion_statement_486_5_257()) jj_scanpos = xsp;
    if (jj_3R_assertion_475_4_196()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_80()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2250_1_70()) return true;
    if (jj_3R_direction_948_1_71()) return true;
    if (jj_3R_simple_expression_2250_1_70()) return true;
    return false;
  }

 inline bool jj_3_81()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_name_521_3_115()) return true;
    return false;
  }

 inline bool jj_3R_range_constraint_2017_1_86()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_3R_range_2008_4_88()) return true;
    return false;
  }

 inline bool jj_3R_assertion_475_4_196()
 {
    if (jj_done) return true;
    if (jj_scan_token(ASSERT_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_assertion_475_31_355()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_assertion_475_63_356()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_79()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_396_3_114()) return true;
    return false;
  }

 inline bool jj_3_5()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_3R_index_constraint_1395_3_65()) return true;
    if (jj_scan_token(OF_T)) return true;
    return false;
  }

 inline bool jj_3R_range_2011_2_191()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_name_521_3_115()) return true;
    return false;
  }

 inline bool jj_3R_range_2008_4_190()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2250_1_70()) return true;
    if (jj_3R_direction_948_1_71()) return true;
    if (jj_3R_simple_expression_2250_1_70()) return true;
    return false;
  }

 inline bool jj_3R_range_2008_4_88()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_range_2008_4_190()) {
    jj_scanpos = xsp;
    if (jj_3R_range_2011_2_191()) return true;
    }
    return false;
  }

 inline bool jj_3R_array_type_definition_469_4_696()
 {
    if (jj_done) return true;
    if (jj_3R_unconstraint_array_definition_2461_1_704()) return true;
    return false;
  }

 inline bool jj_3R_qualified_expression_2001_7_238()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_qualified_expression_1999_5_237()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_396_3_114()) return true;
    return false;
  }

 inline bool jj_3R_array_type_definition_466_2_695()
 {
    if (jj_done) return true;
    if (jj_3R_constraint_array_definition_890_1_703()) return true;
    return false;
  }

 inline bool jj_3R_array_type_definition_466_2_689()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_array_type_definition_466_2_695()) {
    jj_scanpos = xsp;
    if (jj_3R_array_type_definition_469_4_696()) return true;
    }
    return false;
  }

 inline bool jj_3R_qualified_expression_1997_3_64()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_qualified_expression_1999_5_237()) {
    jj_scanpos = xsp;
    if (jj_3R_qualified_expression_2001_7_238()) return true;
    }
    return false;
  }

 inline bool jj_3R_process_statement_part_1992_4_359()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2133_5_117()) return true;
    return false;
  }

 inline bool jj_3R_process_statement_part_1992_3_200()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_process_statement_part_1992_4_359()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_process_statement_1970_24_201()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_alias_declaration_403_44_484()
 {
    if (jj_done) return true;
    if (jj_3R_signature_2241_1_245()) return true;
    return false;
  }

 inline bool jj_3R_aggregate_396_39_248()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_element_association_973_1_247()) return true;
    return false;
  }

 inline bool jj_3R_allocator_423_6_244()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    return false;
  }

 inline bool jj_3R_process_statement_1960_4_198()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_sensitivity_list_2121_3_357()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_allocator_421_3_113()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_4()) {
    jj_scanpos = xsp;
    if (jj_3R_allocator_423_6_244()) return true;
    }
    return false;
  }

 inline bool jj_3_4()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_qualified_expression_1997_3_64()) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_416_4_552()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_1721_1_321()) return true;
    return false;
  }

 inline bool jj_3R_process_statement_1952_3_197()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_415_4_551()
 {
    if (jj_done) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_process_statement_1952_1_93()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_process_statement_1952_3_197()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_process_statement_1960_4_198()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    if (jj_3R_process_declarative_part_1947_2_199()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_process_statement_part_1992_3_200()) return true;
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_process_statement_1970_24_201()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_414_2_550()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_414_2_482()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_alias_designator_414_2_550()) {
    jj_scanpos = xsp;
    if (jj_3R_alias_designator_415_4_551()) {
    jj_scanpos = xsp;
    if (jj_3R_alias_designator_416_4_552()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_process_declarative_part_1947_4_358()
 {
    if (jj_done) return true;
    if (jj_3R_process_declarative_item_1926_1_441()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_part_1947_2_199()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_process_declarative_part_1947_4_358()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_process_declarative_item_1942_3_534()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1327_3_398()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1938_3_533()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2467_2_347()) return true;
    return false;
  }

 inline bool jj_3R_alias_declaration_402_4_483()
 {
    if (jj_done) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    return false;
  }

 inline bool jj_3_78()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1335_2_68()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1937_3_532()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_526_3_395()) return true;
    return false;
  }

 inline bool jj_3R_alias_declaration_401_2_393()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALIAS_T)) return true;
    if (jj_3R_alias_designator_414_2_482()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_alias_declaration_402_4_483()) jj_scanpos = xsp;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_name_1633_2_63()) return true;
    xsp = jj_scanpos;
    if (jj_3R_alias_declaration_403_44_484()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1933_3_531()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_401_2_393()) return true;
    return false;
  }

 inline bool jj_3_77()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_505_2_67()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1932_3_530()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1199_2_392()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1931_3_529()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_2503_1_391()) return true;
    return false;
  }

 inline bool jj_3R_aggregate_396_3_114()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_element_association_973_1_247()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_aggregate_396_39_248()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1930_3_528()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_877_4_389()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1929_3_527()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_2382_1_388()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1928_3_526()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_2437_1_387()) return true;
    return false;
  }

 inline bool jj_3R_adding_operator_390_5_282()
 {
    if (jj_done) return true;
    if (jj_scan_token(MINUS_T)) return true;
    return false;
  }

 inline bool jj_3R_adding_operator_391_4_283()
 {
    if (jj_done) return true;
    if (jj_scan_token(AMPERSAND_T)) return true;
    return false;
  }

 inline bool jj_3_3()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1926_1_525()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2294_1_98()) return true;
    return false;
  }

 inline bool jj_3R_adding_operator_389_3_133()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_adding_operator_389_3_281()) {
    jj_scanpos = xsp;
    if (jj_3R_adding_operator_390_5_282()) {
    jj_scanpos = xsp;
    if (jj_3R_adding_operator_391_4_283()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_adding_operator_389_3_281()
 {
    if (jj_done) return true;
    if (jj_scan_token(PLUS_T)) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_1926_1_441()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_process_declarative_item_1926_1_525()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_1928_3_526()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_1929_3_527()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_1930_3_528()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_1931_3_529()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_1932_3_530()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_1933_3_531()) {
    jj_scanpos = xsp;
    if (jj_3_77()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_1937_3_532()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_1938_3_533()) {
    jj_scanpos = xsp;
    if (jj_3_78()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_1942_3_534()) return true;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_procedure_call_1912_14_342()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_actual_parameter_part_370_4_236()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_2()
 {
    if (jj_done) return true;
    if (jj_3R_actual_designator_356_1_62()) return true;
    return false;
  }

 inline bool jj_3_76()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_procedure_call_statement_1918_1_124()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_76()) jj_scanpos = xsp;
    if (jj_3R_procedure_call_1912_2_180()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_part_381_2_516()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_actual_designator_356_1_62()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_part_379_4_515()
 {
    if (jj_done) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_procedure_call_1912_2_180()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_procedure_call_1912_14_342()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_actual_part_376_2_514()
 {
    if (jj_done) return true;
    if (jj_3R_actual_designator_356_1_62()) return true;
    return false;
  }

 inline bool jj_3R_actual_part_376_2_419()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_actual_part_376_2_514()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_part_379_4_515()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_part_381_2_516()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_74()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2649_2_91()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_1906_4_214()
 {
    if (jj_done) return true;
    if (jj_3R_context_declaration_2633_2_87()) return true;
    return false;
  }

 inline bool jj_3R_actual_parameter_part_370_4_236()
 {
    if (jj_done) return true;
    if (jj_3R_association_list_500_1_169()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_1905_3_213()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_1773_2_368()) return true;
    return false;
  }

 inline bool jj_3_1()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3_75()
 {
    if (jj_done) return true;
    if (jj_3R_interface_package_declaration_2658_2_95()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_1900_1_212()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2649_2_91()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_1898_3_211()
 {
    if (jj_done) return true;
    if (jj_3R_configuration_declaration_838_2_367()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_361_3_155()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3_73()
 {
    if (jj_done) return true;
    if (jj_3R_allocator_421_3_113()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_1897_1_210()
 {
    if (jj_done) return true;
    if (jj_3R_entity_declaration_1044_5_366()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_1897_1_99()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_primary_unit_1897_1_210()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_1898_3_211()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_1900_1_212()) {
    jj_scanpos = xsp;
    if (jj_3_75()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_1905_3_213()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_1906_4_214()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3_72()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_358_1_154()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3_71()
 {
    if (jj_done) return true;
    if (jj_3R_literal_1566_2_112()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_356_1_153()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_356_1_62()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_actual_designator_356_1_153()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_designator_358_1_154()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_designator_361_3_155()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_primary_1891_1_475()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_396_3_114()) return true;
    return false;
  }

 inline bool jj_3_70()
 {
    if (jj_done) return true;
    if (jj_3R_type_conversion_2432_3_111()) return true;
    return false;
  }

 inline bool jj_3R_primary_1888_1_474()
 {
    if (jj_done) return true;
    if (jj_3R_allocator_421_3_113()) return true;
    return false;
  }

 inline bool jj_3R_access_type_definition_349_3_658()
 {
    if (jj_done) return true;
    if (jj_scan_token(ACCESS_T)) return true;
    if (jj_3R_subtype_indication_2398_3_89()) return true;
    return false;
  }

 inline bool jj_3_69()
 {
    if (jj_done) return true;
    if (jj_3R_qualified_expression_1997_3_64()) return true;
    return false;
  }

 inline bool jj_3R_primary_1885_1_473()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3_68()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_primary_1882_1_472()
 {
    if (jj_done) return true;
    if (jj_3R_literal_1566_2_112()) return true;
    return false;
  }

 inline bool jj_3R_abstract_literal_343_4_235()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASED_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_physical_type_definition_1848_33_702()
 {
    if (jj_done) return true;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_abstract_literal_342_4_234()
 {
    if (jj_done) return true;
    if (jj_scan_token(INTEGER)) return true;
    return false;
  }

 inline bool jj_3_67()
 {
    if (jj_done) return true;
    if (jj_3R_function_call_1278_1_110()) return true;
    return false;
  }

 inline bool jj_3R_abstract_literal_341_4_109()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_abstract_literal_341_4_233()) {
    jj_scanpos = xsp;
    if (jj_3R_abstract_literal_342_4_234()) {
    jj_scanpos = xsp;
    if (jj_3R_abstract_literal_343_4_235()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_abstract_literal_341_4_233()
 {
    if (jj_done) return true;
    if (jj_scan_token(DECIMAL_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_primary_1879_1_471()
 {
    if (jj_done) return true;
    if (jj_3R_type_conversion_2432_3_111()) return true;
    return false;
  }

 inline bool jj_3R_primary_1876_1_470()
 {
    if (jj_done) return true;
    if (jj_3R_qualified_expression_1997_3_64()) return true;
    return false;
  }

 inline bool jj_3R_primary_1873_1_469()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_primary_1870_1_386()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_primary_1870_1_468()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_1873_1_469()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_1876_1_470()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_1879_1_471()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_1882_1_472()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_1885_1_473()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_1888_1_474()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_1891_1_475()) return true;
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_primary_1870_1_468()
 {
    if (jj_done) return true;
    if (jj_3R_function_call_1278_1_110()) return true;
    return false;
  }

 inline bool jj_3R_port_map_aspect_1865_2_341()
 {
    if (jj_done) return true;
    if (jj_scan_token(PORT_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_association_list_500_1_169()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_port_list_1860_3_594()
 {
    if (jj_done) return true;
    if (jj_3R_interface_list_1476_3_327()) return true;
    return false;
  }

 inline bool jj_3R_physical_type_definition_1846_14_701()
 {
    if (jj_done) return true;
    if (jj_3R_secondary_unit_declaration_2087_1_721()) return true;
    return false;
  }

 inline bool jj_3R_port_clause_1855_4_553()
 {
    if (jj_done) return true;
    if (jj_scan_token(PORT_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_port_list_1860_3_594()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_physical_type_definition_1843_9_694()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNITS_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_physical_type_definition_1846_14_701()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(UNITS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_physical_type_definition_1848_33_702()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_66()
 {
    if (jj_done) return true;
    if (jj_3R_abstract_literal_341_4_109()) return true;
    return false;
  }

 inline bool jj_3R_physical_literal_1838_3_232()
 {
    if (jj_done) return true;
    if (jj_3R_abstract_literal_341_4_109()) return true;
    return false;
  }

 inline bool jj_3R_physical_literal_1838_2_108()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_physical_literal_1838_3_232()) jj_scanpos = xsp;
    if (jj_3R_name_1633_2_63()) return true;
    return false;
  }

 inline bool jj_3R_parameter_specification_1833_1_543()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(IN_T)) return true;
    if (jj_3R_discrete_range_964_3_72()) return true;
    return false;
  }

 inline bool jj_3_65()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2649_2_91()) return true;
    return false;
  }

 inline bool jj_3_63()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_505_2_67()) return true;
    return false;
  }

 inline bool jj_3_64()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1335_2_68()) return true;
    return false;
  }

 inline bool jj_3R_package_declaration_1773_2_368()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3_62()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1335_2_68()) return true;
    return false;
  }

 inline bool jj_3R_options_1726_21_331()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_924_1_420()) return true;
    return false;
  }

 inline bool jj_3R_options_1726_3_172()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(50)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_options_1726_21_331()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_next_statement_1685_43_270()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_operator_symbol_1721_1_321()
 {
    if (jj_done) return true;
    if (jj_scan_token(STRINGLITERAL)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1716_2_654()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1715_2_653()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1714_2_652()
 {
    if (jj_done) return true;
    if (jj_scan_token(SHARED_T)) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3_61()
 {
    if (jj_done) return true;
    if (jj_3R_physical_literal_1838_2_108()) return true;
    return false;
  }

 inline bool jj_3R_object_class_1713_2_651()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1712_2_650()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1711_1_649()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1711_1_625()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_object_class_1711_1_649()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1712_2_650()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1713_2_651()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1714_2_652()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1715_2_653()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1716_2_654()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_name_ext_1664_47_370()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_numeric_literal_1706_4_216()
 {
    if (jj_done) return true;
    if (jj_3R_abstract_literal_341_4_109()) return true;
    return false;
  }

 inline bool jj_3R_numeric_literal_1704_2_215()
 {
    if (jj_done) return true;
    if (jj_3R_physical_literal_1838_2_108()) return true;
    return false;
  }

 inline bool jj_3R_numeric_literal_1704_2_101()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_numeric_literal_1704_2_215()) {
    jj_scanpos = xsp;
    if (jj_3R_numeric_literal_1706_4_216()) return true;
    }
    return false;
  }

 inline bool jj_3R_null_statement_1698_3_459()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_null_statement_1698_1_379()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_statement_1698_3_459()) jj_scanpos = xsp;
    if (jj_scan_token(NULL_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_null_1663_32_107()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    return false;
  }

 inline bool jj_3R_next_statement_1686_3_271()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_809_3_83()) return true;
    return false;
  }

 inline bool jj_3R_next_statement_1685_3_269()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_60()
 {
    if (jj_done) return true;
    if (jj_scan_token(LBRACKET_T)) return true;
    return false;
  }

 inline bool jj_3R_next_statement_1685_1_128()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_next_statement_1685_3_269()) jj_scanpos = xsp;
    if (jj_scan_token(NEXT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_next_statement_1685_43_270()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_next_statement_1686_3_271()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_test_att_name_1674_4_231()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_59()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_null_1663_32_107()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_test_att_name_1672_5_229()
 {
    if (jj_done) return true;
    if (jj_3R_signature_2241_1_245()) return true;
    return false;
  }

 inline bool jj_3R_test_att_name_1672_3_106()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_test_att_name_1672_5_229()) jj_scanpos = xsp;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_3R_attribute_designator_514_3_230()) return true;
    xsp = jj_scanpos;
    if (jj_3R_test_att_name_1674_4_231()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_58()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_discrete_range_964_3_72()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_54()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext_1650_3_104()) return true;
    return false;
  }

 inline bool jj_3_57()
 {
    if (jj_done) return true;
    if (jj_3R_test_att_name_1672_3_106()) return true;
    return false;
  }

 inline bool jj_3_52()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1644_4_103()) return true;
    return false;
  }

 inline bool jj_3_51()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1644_4_103()) return true;
    return false;
  }

 inline bool jj_3_53()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1644_4_103()) return true;
    return false;
  }

 inline bool jj_3_56()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_suffix_2404_1_105()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1663_3_224()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1168_1_61()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_name_ext_1664_47_370()) { jj_scanpos = xsp; break; }
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

 inline bool jj_3R_name_ext1_1644_18_219()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext_1650_3_104()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1660_2_223()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_discrete_range_964_3_72()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1657_3_222()
 {
    if (jj_done) return true;
    if (jj_3R_test_att_name_1672_3_106()) return true;
    return false;
  }

 inline bool jj_3R_name_1634_25_322()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1644_4_103()) return true;
    return false;
  }

 inline bool jj_3R_name_1635_24_324()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1644_4_103()) return true;
    return false;
  }

 inline bool jj_3R_name_1636_22_325()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1644_4_103()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1654_2_221()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_suffix_2404_1_105()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1651_2_220()
 {
    if (jj_done) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1650_3_104()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_ext_1651_2_220()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_1654_2_221()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_1657_3_222()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_1660_2_223()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_1663_3_224()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_name_ext1_1644_4_103()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext_1650_3_104()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_name_ext1_1644_18_219()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_name_1636_6_158()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_1636_22_325()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_1635_5_157()
 {
    if (jj_done) return true;
    if (jj_3R_external_name_2744_2_323()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_1635_24_324()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_1634_4_156()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_1721_1_321()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_1634_25_322()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_1633_2_63()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_1634_4_156()) {
    jj_scanpos = xsp;
    if (jj_3R_name_1635_5_157()) {
    jj_scanpos = xsp;
    if (jj_3R_name_1636_6_158()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_multiplying_operation_1627_3_290()
 {
    if (jj_done) return true;
    if (jj_scan_token(REM_T)) return true;
    return false;
  }

 inline bool jj_3R_multiplying_operation_1626_3_289()
 {
    if (jj_done) return true;
    if (jj_scan_token(MOD_T)) return true;
    return false;
  }

 inline bool jj_3R_multiplying_operation_1625_3_288()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLASH_T)) return true;
    return false;
  }

 inline bool jj_3R_multiplying_operation_1624_1_140()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_multiplying_operation_1624_1_287()) {
    jj_scanpos = xsp;
    if (jj_3R_multiplying_operation_1625_3_288()) {
    jj_scanpos = xsp;
    if (jj_3R_multiplying_operation_1626_3_289()) {
    jj_scanpos = xsp;
    if (jj_3R_multiplying_operation_1627_3_290()) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_multiplying_operation_1624_1_287()
 {
    if (jj_done) return true;
    if (jj_scan_token(MULT_T)) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_1594_28_635()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    return false;
  }

 inline bool jj_3R_mode_1619_3_449()
 {
    if (jj_done) return true;
    if (jj_scan_token(LINKAGE_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_1618_3_448()
 {
    if (jj_done) return true;
    if (jj_scan_token(BUFFER_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_1617_3_447()
 {
    if (jj_done) return true;
    if (jj_scan_token(INOUT_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_1616_3_446()
 {
    if (jj_done) return true;
    if (jj_scan_token(OUT_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_1615_1_363()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_mode_1615_1_445()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_1616_3_446()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_1617_3_447()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_1618_3_448()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_1619_3_449()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_mode_1615_1_445()
 {
    if (jj_done) return true;
    if (jj_scan_token(IN_T)) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_1587_7_267()
 {
    if (jj_done) return true;
    if (jj_3R_iteration_scheme_1519_1_383()) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_1586_3_266()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_1586_1_127()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_loop_statement_1586_3_266()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_loop_statement_1587_7_267()) jj_scanpos = xsp;
    if (jj_scan_token(LOOP_T)) return true;
    if (jj_3R_sequence_of_statement_2128_2_268()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(LOOP_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_loop_statement_1594_28_635()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_50()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_literal_1142_2_102()) return true;
    return false;
  }

 inline bool jj_3_49()
 {
    if (jj_done) return true;
    if (jj_3R_numeric_literal_1704_2_101()) return true;
    return false;
  }

 inline bool jj_3R_literal_1576_3_243()
 {
    if (jj_done) return true;
    if (jj_scan_token(NULL_T)) return true;
    return false;
  }

 inline bool jj_3_48()
 {
    if (jj_done) return true;
    if (jj_3R_bit_string_literal_574_3_100()) return true;
    return false;
  }

 inline bool jj_3R_literal_1575_3_242()
 {
    if (jj_done) return true;
    if (jj_3R_string_literal_2265_1_373()) return true;
    return false;
  }

 inline bool jj_3R_literal_1573_1_241()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_literal_1142_2_102()) return true;
    return false;
  }

 inline bool jj_3R_literal_1569_2_240()
 {
    if (jj_done) return true;
    if (jj_3R_numeric_literal_1704_2_101()) return true;
    return false;
  }

 inline bool jj_3R_literal_1566_2_112()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_literal_1566_2_239()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_1569_2_240()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_1573_1_241()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_1575_3_242()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_1576_3_243()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_literal_1566_2_239()
 {
    if (jj_done) return true;
    if (jj_3R_bit_string_literal_574_3_100()) return true;
    return false;
  }

 inline bool jj_3_47()
 {
    if (jj_done) return true;
    if (jj_3R_primary_unit_1897_1_99()) return true;
    return false;
  }

 inline bool jj_3R_library_clause_1542_2_346()
 {
    if (jj_done) return true;
    if (jj_scan_token(LIBRARY_T)) return true;
    if (jj_3R_identifier_list_1354_4_205()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_label_1537_2_145()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1348_3_74()) return true;
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
  int currP = 0;
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
