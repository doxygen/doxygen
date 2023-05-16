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
void interface_object_declaration();
QCString interface_constant_declaration();
QCString interface_element();
QCString interface_file_declaration();
QCString interface_list();
QCString interface_package_generic_map_aspect();
QCString interface_procedure_specification();
QCString interface_signal_declaration();
QCString interface_subprogram_declaration();
QCString iproc();
QCString ifunc();
QCString param();
QCString interface_subprogram_default();
QCString interface_subprogram_specification();
QCString interface_function_specification();
QCString interface_incomplete_type_declaration();
QCString interface_type_declaration();
QCString interface_type_indication();
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
QCString simple_mode_indication();
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
void subprogram_header();
void subprogram_specification();
void subprogram_statement_part();
QCString subprogram_instantiation_declaration();
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

 inline bool jj_2_126(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_126() || jj_done);
 { jj_save(125, xla); }
  }

 inline bool jj_2_127(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_127() || jj_done);
 { jj_save(126, xla); }
  }

 inline bool jj_2_128(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_128() || jj_done);
 { jj_save(127, xla); }
  }

 inline bool jj_2_129(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_129() || jj_done);
 { jj_save(128, xla); }
  }

 inline bool jj_2_130(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_130() || jj_done);
 { jj_save(129, xla); }
  }

 inline bool jj_2_131(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_131() || jj_done);
 { jj_save(130, xla); }
  }

 inline bool jj_2_132(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_132() || jj_done);
 { jj_save(131, xla); }
  }

 inline bool jj_2_133(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_133() || jj_done);
 { jj_save(132, xla); }
  }

 inline bool jj_2_134(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_134() || jj_done);
 { jj_save(133, xla); }
  }

 inline bool jj_2_135(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_135() || jj_done);
 { jj_save(134, xla); }
  }

 inline bool jj_3_50()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEMI_T)) return true;
    if (jj_3R_interface_element_1493_3_101()) return true;
    return false;
  }

 inline bool jj_3R_interface_signal_declaration_1527_2_100()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(100)) jj_scanpos = xsp;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_interface_type_indication_1629_6_216()) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(27)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_signal_declaration_1527_87_218()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_else_stat_3053_5_508()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_else_stat_3053_28_579()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_else_stat_3053_4_417()
 {
    if (jj_done) return true;
    Token * xsp;
    if (jj_3R_else_stat_3053_5_508()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_else_stat_3053_5_508()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_53()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_scan_token(DEFAULT_T)) return true;
    return false;
  }

 inline bool jj_3_52()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_inout_stat_3047_3_416()
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

 inline bool jj_3R_instantiation_unit_1428_86_439()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_sel_wave_list_3042_4_479()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_element_2724_2_476()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_718_3_90()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sel_wave_list_3042_43_562()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_list_1507_3_338()
 {
    if (jj_done) return true;
    if (jj_3R_interface_element_1493_3_101()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_50()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3_51()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_selected_waveform_assignment_3025_17_478()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_939_1_435()) return true;
    return false;
  }

 inline bool jj_3R_sel_var_list_3036_3_328()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_718_3_90()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(134)) {
    jj_scanpos = xsp;
    if (jj_scan_token(137)) return true;
    }
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sel_var_list_3036_58_418()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_interface_file_declaration_1498_3_373()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    return false;
  }

 inline bool jj_3_49()
 {
    if (jj_done) return true;
    if (jj_3R_interface_signal_declaration_1527_2_100()) return true;
    return false;
  }

 inline bool jj_3R_selected_force_assignment_3030_3_153()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_scan_token(FORCE_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_selected_force_assignment_3031_52_327()) jj_scanpos = xsp;
    if (jj_3R_sel_var_list_3036_3_328()) return true;
    return false;
  }

 inline bool jj_3_48()
 {
    if (jj_done) return true;
    if (jj_3R_interface_constant_declaration_1488_3_99()) return true;
    return false;
  }

 inline bool jj_3R_interface_element_1493_3_101()
 {
    if (jj_done) return true;
    if (jj_3R_interface_declaration_1451_1_219()) return true;
    return false;
  }

 inline bool jj_3_134()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_411_3_117()) return true;
    return false;
  }

 inline bool jj_3R_interface_object_declaration_1483_9_211()
 {
    if (jj_done) return true;
    if (jj_3R_interface_file_declaration_1498_3_373()) return true;
    return false;
  }

 inline bool jj_3R_interface_constant_declaration_1488_3_99()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(31)) jj_scanpos = xsp;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(COLON_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(53)) jj_scanpos = xsp;
    if (jj_3R_interface_type_indication_1629_6_216()) return true;
    xsp = jj_scanpos;
    if (jj_3R_interface_constant_declaration_1488_89_217()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_selected_waveform_assignment_3023_3_401()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_selected_waveform_assignment_3025_17_478()) jj_scanpos = xsp;
    if (jj_3R_sel_wave_list_3042_4_479()) return true;
    return false;
  }

 inline bool jj_3R_interface_object_declaration_1481_9_210()
 {
    if (jj_done) return true;
    if (jj_3R_interface_signal_declaration_1527_2_100()) return true;
    return false;
  }

 inline bool jj_3R_interface_object_declaration_1480_7_209()
 {
    if (jj_done) return true;
    if (jj_3R_interface_constant_declaration_1488_3_99()) return true;
    return false;
  }

 inline bool jj_3R_interface_object_declaration_1480_7_95()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_object_declaration_1480_7_209()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_object_declaration_1481_9_210()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_object_declaration_1483_9_211()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_select_name_3018_3_692()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_select_name_3016_3_691()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_411_3_117()) return true;
    return false;
  }

 inline bool jj_3R_select_name_3016_3_677()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_select_name_3016_3_691()) {
    jj_scanpos = xsp;
    if (jj_3R_select_name_3018_3_692()) return true;
    }
    return false;
  }

 inline bool jj_3_131()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_2951_3_151()) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_else_wave_list_2988_27_596()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3_133()
 {
    if (jj_done) return true;
    if (jj_3R_selected_force_assignment_3030_3_153()) return true;
    return false;
  }

 inline bool jj_3_47()
 {
    if (jj_done) return true;
    if (jj_3R_interface_subprogram_declaration_1532_4_96()) return true;
    return false;
  }

 inline bool jj_3R_selected_variable_assignment_3009_3_558()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_3R_select_name_3016_3_677()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_sel_var_list_3036_3_328()) return true;
    return false;
  }

 inline bool jj_3R_conditional_force_assignment_2995_15_325()
 {
    if (jj_done) return true;
    if (jj_3R_inout_stat_3047_3_416()) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_list_2962_47_323()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_2951_3_151()) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_force_assignment_2996_13_326()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_3R_else_stat_3053_4_417()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveform_assignment_2982_26_477()
 {
    if (jj_done) return true;
    if (jj_3R_else_wave_list_2988_3_561()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1470_2_381()
 {
    if (jj_done) return true;
    if (jj_3R_object_class_1861_1_465()) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_wave_3004_3_290()
 {
    if (jj_done) return true;
    if (jj_3R_selected_waveform_assignment_3023_3_401()) return true;
    return false;
  }

 inline bool jj_3_45()
 {
    if (jj_done) return true;
    if (jj_3R_interface_package_declaration_2831_2_97()) return true;
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_wave_3002_3_135()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_selected_signal_assignment_wave_3002_3_289()) {
    jj_scanpos = xsp;
    if (jj_3R_selected_signal_assignment_wave_3004_3_290()) return true;
    }
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_wave_3002_3_289()
 {
    if (jj_done) return true;
    if (jj_3R_selected_force_assignment_3030_3_153()) return true;
    return false;
  }

 inline bool jj_3_44()
 {
    if (jj_done) return true;
    if (jj_3R_interface_subprogram_declaration_1532_4_96()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1467_1_380()
 {
    if (jj_done) return true;
    if (jj_3R_interface_subprogram_declaration_1532_4_96()) return true;
    return false;
  }

 inline bool jj_3_43()
 {
    if (jj_done) return true;
    if (jj_3R_interface_object_declaration_1480_7_95()) return true;
    return false;
  }

 inline bool jj_3_46()
 {
    if (jj_done) return true;
    if (jj_3R_interface_type_declaration_1624_3_98()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveform_assignment_2981_17_475()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_939_1_435()) return true;
    return false;
  }

 inline bool jj_3_42()
 {
    if (jj_done) return true;
    if (jj_3R_interface_variable_declaration_1637_1_94()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1460_1_379()
 {
    if (jj_done) return true;
    if (jj_3R_interface_package_declaration_2831_2_97()) return true;
    return false;
  }

 inline bool jj_3R_conditional_force_assignment_2993_3_152()
 {
    if (jj_done) return true;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_scan_token(FORCE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_conditional_force_assignment_2995_15_325()) jj_scanpos = xsp;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_conditional_force_assignment_2996_13_326()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1457_1_378()
 {
    if (jj_done) return true;
    if (jj_3R_interface_subprogram_declaration_1532_4_96()) return true;
    return false;
  }

 inline bool jj_3R_else_wave_list_2988_3_561()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_else_wave_list_2988_27_596()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_interface_declaration_1454_2_377()
 {
    if (jj_done) return true;
    if (jj_3R_interface_object_declaration_1480_7_95()) return true;
    return false;
  }

 inline bool jj_3R_index_constraint_1410_42_162()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_discrete_range_979_3_72()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1451_1_219()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_declaration_1451_1_376()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1454_2_377()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1457_1_378()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1460_1_379()) {
    jj_scanpos = xsp;
    if (jj_3_46()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1467_1_380()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1470_2_381()) return true;
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_interface_declaration_1451_1_376()
 {
    if (jj_done) return true;
    if (jj_3R_interface_variable_declaration_1637_1_94()) return true;
    return false;
  }

 inline bool jj_3_132()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_force_assignment_2993_3_152()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveform_assignment_2980_3_400()
 {
    if (jj_done) return true;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_conditional_waveform_assignment_2981_17_475()) jj_scanpos = xsp;
    if (jj_3R_waveform_element_2724_2_476()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    xsp = jj_scanpos;
    if (jj_3R_conditional_waveform_assignment_2982_26_477()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_41()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASIC_IDENTIFIER)) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_wave_2975_3_288()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_waveform_assignment_2980_3_400()) return true;
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_wave_2973_3_134()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_conditional_signal_assignment_wave_2973_3_287()) {
    jj_scanpos = xsp;
    if (jj_3R_conditional_signal_assignment_wave_2975_3_288()) return true;
    }
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_wave_2973_3_287()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_force_assignment_2993_3_152()) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1393_46_642()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1436_3_442()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1435_3_441()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1434_3_354()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_instantiation_list_1434_3_440()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_list_1435_3_441()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_list_1436_3_442()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_instantiation_list_1434_3_440()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    return false;
  }

 inline bool jj_3R_package_path_name_2968_3_588()
 {
    if (jj_done) return true;
    if (jj_scan_token(AT_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_2951_19_324()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_130()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_list_2962_3_150()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1429_3_351()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1428_3_350()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_41()) jj_scanpos = xsp;
    if (jj_3R_name_1783_2_63()) return true;
    xsp = jj_scanpos;
    if (jj_3R_instantiation_unit_1428_86_439()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_pathname_element_list_2962_3_150()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_2951_3_151()) return true;
    if (jj_scan_token(DOT_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_pathname_element_list_2962_47_323()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_instantiation_unit_1427_1_180()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_instantiation_unit_1427_1_349()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_unit_1428_3_350()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_unit_1429_3_351()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_40()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_979_3_72()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1427_1_349()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(28)) jj_scanpos = xsp;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_index_subtype_definition_1422_2_739()
 {
    if (jj_done) return true;
    if (jj_3R_type_mark_2629_3_164()) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_relative_pathname_2941_17_636()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_list_2962_3_150()) return true;
    return false;
  }

 inline bool jj_3R_index_specification_1417_3_639()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_2951_3_151()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_pathname_element_2951_19_324()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_index_specification_1415_2_638()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_979_3_72()) return true;
    return false;
  }

 inline bool jj_3R_index_specification_1415_2_593()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_index_specification_1415_2_638()) {
    jj_scanpos = xsp;
    if (jj_3R_index_specification_1417_3_639()) return true;
    }
    return false;
  }

 inline bool jj_3R_neg_list_2946_3_661()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEG_T)) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_neg_list_2946_2_635()
 {
    if (jj_done) return true;
    Token * xsp;
    if (jj_3R_neg_list_2946_3_661()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_neg_list_2946_3_661()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_index_constraint_1410_3_65()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_discrete_range_979_3_72()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_index_constraint_1410_42_162()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_129()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_pathname_element_list_2962_3_150()) return true;
    return false;
  }

 inline bool jj_3R_relative_pathname_2941_3_587()
 {
    if (jj_done) return true;
    if (jj_3R_neg_list_2946_2_635()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_relative_pathname_2941_17_636()) jj_scanpos = xsp;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_declaration_1402_3_565()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_absolute_pathname_2936_3_634()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_absolute_pathname_2934_2_633()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_pathname_element_list_2962_3_150()) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_absolute_pathname_2934_2_586()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_absolute_pathname_2934_2_633()) {
    jj_scanpos = xsp;
    if (jj_3R_absolute_pathname_2936_3_634()) return true;
    }
    return false;
  }

 inline bool jj_3R_context_declaration_2806_128_192()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_2929_4_527()
 {
    if (jj_done) return true;
    if (jj_3R_package_path_name_2968_3_588()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_2928_4_526()
 {
    if (jj_done) return true;
    if (jj_3R_relative_pathname_2941_3_587()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_2927_4_525()
 {
    if (jj_done) return true;
    if (jj_3R_absolute_pathname_2934_2_586()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_2927_4_428()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_external_pathname_2927_4_525()) {
    jj_scanpos = xsp;
    if (jj_3R_external_pathname_2928_4_526()) {
    jj_scanpos = xsp;
    if (jj_3R_external_pathname_2929_4_527()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_if_statement_1389_6_641()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_sequence_of_statement_2276_2_278()) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_2921_5_524()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_2920_5_523()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1382_6_640()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSIF_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    if (jj_scan_token(THEN_T)) return true;
    if (jj_3R_sequence_of_statement_2276_2_278()) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_2919_3_522()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_2919_3_427()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sig_stat_2919_3_522()) {
    jj_scanpos = xsp;
    if (jj_3R_sig_stat_2920_5_523()) {
    jj_scanpos = xsp;
    if (jj_3R_sig_stat_2921_5_524()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_identifier_list_1369_16_371()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3_124()
 {
    if (jj_done) return true;
    if (jj_3R_ttend_2876_3_147()) return true;
    return false;
  }

 inline bool jj_3_128()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1374_4_274()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1374_3_128()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_if_statement_1374_4_274()) jj_scanpos = xsp;
    if (jj_scan_token(IF_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    if (jj_scan_token(THEN_T)) return true;
    if (jj_3R_sequence_of_statement_2276_2_278()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_if_statement_1382_6_640()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_if_statement_1389_6_641()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(IF_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_if_statement_1393_46_642()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_external_name_2909_2_334()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLSL_T)) return true;
    if (jj_3R_sig_stat_2919_3_427()) return true;
    if (jj_3R_external_pathname_2927_4_428()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    if (jj_scan_token(RSRS_T)) return true;
    return false;
  }

 inline bool jj_3R_identifier_list_1369_4_206()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_identifier_list_1369_16_371()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_127()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_identifier_1364_4_174()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASIC_IDENTIFIER)) return true;
    return false;
  }

 inline bool jj_3R_identifier_1363_3_173()
 {
    if (jj_done) return true;
    if (jj_scan_token(EXTENDED_CHARACTER)) return true;
    return false;
  }

 inline bool jj_3R_identifier_1363_3_74()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_identifier_1363_3_173()) {
    jj_scanpos = xsp;
    if (jj_3R_identifier_1364_4_174()) return true;
    }
    return false;
  }

 inline bool jj_3_126()
 {
    if (jj_done) return true;
    if (jj_3R_block_declarative_item_608_3_149()) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_list_1337_28_578()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_group_constituent_1331_2_577()) return true;
    return false;
  }

 inline bool jj_3R_package_instantiation_declaration_2822_71_256()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_2843_4_140()) return true;
    return false;
  }

 inline bool jj_3R_ttend_2876_12_307()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_group_template_declaration_1350_2_68()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_entity_class_entry_list_1053_2_506()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_context_declaration_2806_80_191()
 {
    if (jj_done) return true;
    if (jj_3R_libustcont_stats_2815_4_360()) return true;
    return false;
  }

 inline bool jj_3R_interface_package_declaration_2831_55_214()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_2843_4_140()) return true;
    return false;
  }

 inline bool jj_3_125()
 {
    if (jj_done) return true;
    if (jj_3R_label_1687_2_148()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_group_declaration_1342_3_415()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_group_constituent_list_1337_3_507()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_ttend_2876_3_147()
 {
    if (jj_done) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_ttend_2876_12_307()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_list_1337_3_507()
 {
    if (jj_done) return true;
    if (jj_3R_group_constituent_1331_2_577()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_group_constituent_list_1337_28_578()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_group_constituent_1332_4_632()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_697_3_385()) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_1331_2_631()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_1331_2_577()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_group_constituent_1331_2_631()) {
    jj_scanpos = xsp;
    if (jj_3R_group_constituent_1332_4_632()) return true;
    }
    return false;
  }

 inline bool jj_3R_generic_map_aspect_1326_6_73()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_association_list_515_1_172()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_generic_list_1321_2_165()
 {
    if (jj_done) return true;
    if (jj_3R_interface_list_1507_3_338()) return true;
    return false;
  }

 inline bool jj_3_39()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_generic_clause_1316_2_69()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_generic_list_1321_2_165()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_gen_interface_list_2848_4_139()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_interface_list_1507_3_338()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_generate_scheme_1311_3_347()
 {
    if (jj_done) return true;
    if (jj_scan_token(IF_T)) return true;
    return false;
  }

 inline bool jj_3R_gen_assoc_list_2843_4_140()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_association_list_515_1_172()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_generate_scheme_1310_1_177()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_generate_scheme_1310_1_346()) {
    jj_scanpos = xsp;
    if (jj_3R_generate_scheme_1311_3_347()) return true;
    }
    return false;
  }

 inline bool jj_3R_generate_scheme_1310_1_346()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    return false;
  }

 inline bool jj_3R_generate_statement_1298_1_78()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_generate_scheme_1310_1_177()) return true;
    return false;
  }

 inline bool jj_3R_interface_package_declaration_2831_2_97()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_package_declaration_2831_55_214()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_function_call_1293_1_113()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_actual_parameter_part_385_4_246()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_context_ref_2798_31_450()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_package_instantiation_declaration_2822_2_91()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    if (jj_3R_signature_2397_1_255()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_instantiation_declaration_2822_71_256()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_libustcont_stats_2817_6_453()
 {
    if (jj_done) return true;
    if (jj_3R_context_ref_2798_2_359()) return true;
    return false;
  }

 inline bool jj_3R_libustcont_stats_2816_6_452()
 {
    if (jj_done) return true;
    if (jj_3R_library_clause_1692_2_357()) return true;
    return false;
  }

 inline bool jj_3R_libustcont_stats_2815_4_360()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_libustcont_stats_2815_4_451()) {
    jj_scanpos = xsp;
    if (jj_3R_libustcont_stats_2816_6_452()) {
    jj_scanpos = xsp;
    if (jj_3R_libustcont_stats_2817_6_453()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_libustcont_stats_2815_4_451()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2640_2_358()) return true;
    return false;
  }

 inline bool jj_3R_file_declaration_1214_67_496()
 {
    if (jj_done) return true;
    if (jj_3R_file_open_information_1229_2_567()) return true;
    return false;
  }

 inline bool jj_3R_context_declaration_2806_2_87()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONTEXT_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_context_declaration_2806_80_191()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(32)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_context_declaration_2806_128_192()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declaration_2776_31_688()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_2707_97_273()
 {
    if (jj_done) return true;
    if (jj_3R_timeout_clause_2597_1_398()) return true;
    return false;
  }

 inline bool jj_3R_file_open_information_1229_38_604()
 {
    if (jj_done) return true;
    if (jj_3R_inout_stat_3047_3_416()) return true;
    return false;
  }

 inline bool jj_3R_formal_part_1256_11_163()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_formal_designator_1244_5_337()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_context_ref_2798_2_359()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONTEXT_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_context_ref_2798_31_450()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_full_type_declaration_1261_3_145()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_type_definition_2617_1_600()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_part_2789_6_698()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_declarative_item_2782_5_717()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_part_2789_5_687()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_protected_type_declarative_part_2789_6_698()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_formal_part_1256_1_66()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_formal_part_1256_11_163()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2784_7_737()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2640_2_358()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2783_7_736()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_541_3_412()) return true;
    return false;
  }

 inline bool jj_3R_formal_parameter_list_1251_2_665()
 {
    if (jj_done) return true;
    if (jj_3R_interface_list_1507_3_338()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2782_5_735()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_specification_2503_2_563()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2782_5_717()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_declarative_item_2782_5_735()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_declarative_item_2783_7_736()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_declarative_item_2784_7_737()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_formal_designator_1245_6_430()
 {
    if (jj_done) return true;
    if (jj_scan_token(INTEGER)) return true;
    return false;
  }

 inline bool jj_3R_formal_designator_1244_5_337()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_formal_designator_1244_5_429()) {
    jj_scanpos = xsp;
    if (jj_3R_formal_designator_1245_6_430()) return true;
    }
    return false;
  }

 inline bool jj_3R_formal_designator_1244_5_429()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_2740_38_686()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_2707_71_272()
 {
    if (jj_done) return true;
    if (jj_3R_condition_clause_829_3_397()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declaration_2772_4_674()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_3R_protected_type_declarative_part_2789_5_687()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_declaration_2776_31_688()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_part_2766_7_697()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_body_declarative_item_2745_5_716()) return true;
    return false;
  }

 inline bool jj_3_122()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_520_2_67()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_part_2766_5_685()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_protected_type_body_declarative_part_2766_7_697()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_file_type_definition_1234_2_673()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_type_mark_2629_3_164()) return true;
    return false;
  }

 inline bool jj_3R_file_open_information_1229_4_603()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2760_7_734()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1342_3_415()) return true;
    return false;
  }

 inline bool jj_3R_file_open_information_1229_2_567()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_file_open_information_1229_4_603()) jj_scanpos = xsp;
    if (jj_scan_token(IS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_file_open_information_1229_38_604()) jj_scanpos = xsp;
    if (jj_3R_file_logical_name_1224_2_605()) return true;
    return false;
  }

 inline bool jj_3_123()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1350_2_68()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2757_7_733()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2640_2_358()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2756_7_732()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_541_3_412()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2754_7_731()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_520_2_67()) return true;
    return false;
  }

 inline bool jj_3R_file_logical_name_1224_2_605()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2752_7_730()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_416_2_410()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2751_7_729()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1214_2_409()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2750_7_728()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_2676_1_408()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2749_7_727()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_892_4_406()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2748_7_726()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_2555_1_405()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2747_7_725()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_2610_1_404()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2746_7_724()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_2428_1_652()) return true;
    return false;
  }

 inline bool jj_3R_factor_1204_14_480()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOUBLEMULT_T)) return true;
    if (jj_3R_primary_2020_1_402()) return true;
    return false;
  }

 inline bool jj_3R_file_declaration_1214_2_409()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_file_declaration_1214_67_496()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2745_5_723()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2451_1_403()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2745_5_716()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_body_declarative_item_2745_5_723()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2746_7_724()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2747_7_725()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2748_7_726()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2749_7_727()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2750_7_728()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2751_7_729()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2752_7_730()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2754_7_731()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2756_7_732()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2757_7_733()) {
    jj_scanpos = xsp;
    if (jj_3_123()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2760_7_734()) return true;
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

 inline bool jj_3R_wait_statement_2707_43_271()
 {
    if (jj_done) return true;
    if (jj_3R_sensitivity_clause_2260_2_396()) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1169_43_283()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_factor_1208_3_306()
 {
    if (jj_done) return true;
    if (jj_3R_logop_1189_3_331()) return true;
    if (jj_3R_primary_2020_1_402()) return true;
    return false;
  }

 inline bool jj_3R_factor_1207_3_305()
 {
    if (jj_done) return true;
    if (jj_scan_token(QQ_T)) return true;
    if (jj_3R_primary_2020_1_402()) return true;
    return false;
  }

 inline bool jj_3R_factor_1206_3_304()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOT_T)) return true;
    if (jj_3R_primary_2020_1_402()) return true;
    return false;
  }

 inline bool jj_3R_waveform_element_2724_19_560()
 {
    if (jj_done) return true;
    if (jj_scan_token(AFTER_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_factor_1205_3_303()
 {
    if (jj_done) return true;
    if (jj_scan_token(ABS_T)) return true;
    if (jj_3R_primary_2020_1_402()) return true;
    return false;
  }

 inline bool jj_3R_factor_1204_1_144()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_factor_1204_1_302()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1205_3_303()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1206_3_304()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1207_3_305()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1208_3_306()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_factor_1204_1_302()
 {
    if (jj_done) return true;
    if (jj_3R_primary_2020_1_402()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_factor_1204_14_480()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_protected_type_body_2736_4_146()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    if (jj_3R_protected_type_body_declarative_part_2766_5_685()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_body_2740_38_686()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_waveform_2716_23_533()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_waveform_element_2724_2_476()) return true;
    return false;
  }

 inline bool jj_3R_enumeration_type_definition_1163_38_261()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_enumeration_literal_1157_2_105()) return true;
    return false;
  }

 inline bool jj_3R_expression_1183_16_155()
 {
    if (jj_done) return true;
    if (jj_3R_logop_1189_3_331()) return true;
    if (jj_3R_relation_2180_3_154()) return true;
    return false;
  }

 inline bool jj_3R_logop_1194_4_426()
 {
    if (jj_done) return true;
    if (jj_scan_token(OR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1193_4_425()
 {
    if (jj_done) return true;
    if (jj_scan_token(XOR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1192_4_424()
 {
    if (jj_done) return true;
    if (jj_scan_token(XNOR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1191_4_423()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1190_4_422()
 {
    if (jj_done) return true;
    if (jj_scan_token(NAND_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1189_3_331()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_logop_1189_3_421()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1190_4_422()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1191_4_423()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1192_4_424()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1193_4_425()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1194_4_426()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_logop_1189_3_421()
 {
    if (jj_done) return true;
    if (jj_scan_token(AND_T)) return true;
    return false;
  }

 inline bool jj_3R_waveform_element_2724_2_476()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_waveform_element_2724_19_560()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_expression_1183_1_61()
 {
    if (jj_done) return true;
    if (jj_3R_relation_2180_3_154()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_expression_1183_16_155()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_waveform_2718_1_437()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNAFFECTED_T)) return true;
    return false;
  }

 inline bool jj_3R_waveform_2716_1_343()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_waveform_2716_1_436()) {
    jj_scanpos = xsp;
    if (jj_3R_waveform_2718_1_437()) return true;
    }
    return false;
  }

 inline bool jj_3R_waveform_2716_1_436()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_element_2724_2_476()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_waveform_2716_23_533()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_wait_statement_2707_3_270()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1170_3_284()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_2707_1_125()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_2707_3_270()) jj_scanpos = xsp;
    if (jj_scan_token(WAIT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_2707_43_271()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_2707_71_272()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_2707_97_273()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1169_3_282()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1169_1_132()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_exit_statement_1169_3_282()) jj_scanpos = xsp;
    if (jj_scan_token(EXIT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_exit_statement_1169_43_283()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_exit_statement_1170_3_284()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_enumeration_type_definition_1163_3_119()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_enumeration_literal_1157_2_105()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_enumeration_type_definition_1163_38_261()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_enumeration_literal_1158_4_228()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_697_3_385()) return true;
    return false;
  }

 inline bool jj_3R_enumeration_literal_1157_2_105()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_enumeration_literal_1157_2_227()) {
    jj_scanpos = xsp;
    if (jj_3R_enumeration_literal_1158_4_228()) return true;
    }
    return false;
  }

 inline bool jj_3R_enumeration_literal_1157_2_227()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_entity_tag_1152_3_690()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_697_3_385()) return true;
    return false;
  }

 inline bool jj_3R_unconstraint_array_definition_2634_53_740()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_index_subtype_definition_1422_2_739()) return true;
    return false;
  }

 inline bool jj_3R_entity_tag_1151_1_689()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_entity_tag_1151_1_675()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_tag_1151_1_689()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_tag_1152_3_690()) return true;
    }
    return false;
  }

 inline bool jj_3R_entity_name_list_1120_28_659()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_entity_designator_1109_1_658()) return true;
    return false;
  }

 inline bool jj_3_38()
 {
    if (jj_done) return true;
    if (jj_3R_process_statement_2102_1_93()) return true;
    return false;
  }

 inline bool jj_3_37()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_assertion_statement_773_1_92()) return true;
    return false;
  }

 inline bool jj_3R_variable_declaration_2677_3_495()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_variable_declaration_2676_1_408()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(101)) jj_scanpos = xsp;
    if (jj_scan_token(VARIABLE_T)) return true;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    xsp = jj_scanpos;
    if (jj_3R_variable_declaration_2677_3_495()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_variable_assignment_statement_2671_2_471()
 {
    if (jj_done) return true;
    if (jj_3R_selected_variable_assignment_3009_3_558()) return true;
    return false;
  }

 inline bool jj_3_121()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_use_clause_2640_28_505()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_selected_name_2243_2_449()) return true;
    return false;
  }

 inline bool jj_3R_variable_assignment_statement_2667_1_394()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_variable_assignment_statement_2667_1_470()) {
    jj_scanpos = xsp;
    if (jj_3R_variable_assignment_statement_2671_2_471()) return true;
    }
    return false;
  }

 inline bool jj_3R_variable_assignment_statement_2667_1_470()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_121()) jj_scanpos = xsp;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_specification_1129_1_503()
 {
    if (jj_done) return true;
    if (jj_3R_entity_name_list_1120_1_572()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_entity_class_1027_1_573()) return true;
    return false;
  }

 inline bool jj_3R_entity_designator_1109_18_676()
 {
    if (jj_done) return true;
    if (jj_3R_signature_2397_1_255()) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1122_3_609()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1121_3_608()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1120_1_607()
 {
    if (jj_done) return true;
    if (jj_3R_entity_designator_1109_1_658()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_entity_name_list_1120_28_659()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_entity_name_list_1120_1_572()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_name_list_1120_1_607()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_name_list_1121_3_608()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_name_list_1122_3_609()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_116()
 {
    if (jj_done) return true;
    if (jj_3R_constraint_916_5_142()) return true;
    return false;
  }

 inline bool jj_3R_subtype_indication_2571_75_196()
 {
    if (jj_done) return true;
    if (jj_3R_constraint_916_5_142()) return true;
    return false;
  }

 inline bool jj_3R_entity_designator_1109_1_658()
 {
    if (jj_done) return true;
    if (jj_3R_entity_tag_1151_1_675()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_designator_1109_18_676()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_use_clause_2640_2_358()
 {
    if (jj_done) return true;
    if (jj_scan_token(USE_T)) return true;
    if (jj_3R_selected_name_2243_2_449()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_use_clause_2640_28_505()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_unconstraint_array_definition_2634_1_721()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_index_subtype_definition_1422_2_739()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_unconstraint_array_definition_2634_53_740()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2517_115_650()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_header_2498_6_664()) return true;
    return false;
  }

 inline bool jj_3_36()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2822_2_91()) return true;
    return false;
  }

 inline bool jj_3_34()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_520_2_67()) return true;
    return false;
  }

 inline bool jj_3R_type_mark_2629_3_164()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3_35()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1350_2_68()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2624_3_657()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_declaration_2772_4_674()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_entry_list_1053_39_576()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_entity_class_entry_1048_2_575()) return true;
    return false;
  }

 inline bool jj_3_115()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3_120()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_body_2736_4_146()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2620_3_656()
 {
    if (jj_done) return true;
    if (jj_3R_file_type_definition_1234_2_673()) return true;
    return false;
  }

 inline bool jj_3_110()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1326_6_73()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2619_3_655()
 {
    if (jj_done) return true;
    if (jj_3R_access_type_definition_364_3_672()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2618_3_654()
 {
    if (jj_done) return true;
    if (jj_3R_composite_type_definition_767_2_671()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_instantiation_declaration_2545_75_295()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_2843_4_140()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2617_1_653()
 {
    if (jj_done) return true;
    if (jj_3R_scalar_type_definition_2222_1_670()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_2617_1_600()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_type_definition_2617_1_653()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_2618_3_654()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_2619_3_655()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_2620_3_656()) {
    jj_scanpos = xsp;
    if (jj_3_120()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_2624_3_657()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_subtype_indication_2571_44_195()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_type_declaration_2612_3_491()
 {
    if (jj_done) return true;
    if (jj_3R_incomplete_type_declaration_1402_3_565()) return true;
    return false;
  }

 inline bool jj_3_119()
 {
    if (jj_done) return true;
    if (jj_3R_full_type_declaration_1261_3_145()) return true;
    return false;
  }

 inline bool jj_3R_type_declaration_2610_1_404()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_119()) {
    jj_scanpos = xsp;
    if (jj_3R_type_declaration_2612_3_491()) return true;
    }
    return false;
  }

 inline bool jj_3R_type_conversion_2605_3_114()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_118()
 {
    if (jj_done) return true;
    if (jj_3R_multiplying_operation_1774_1_143()) return true;
    if (jj_3R_factor_1204_1_144()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_entry_1048_21_630()
 {
    if (jj_done) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_instantiation_declaration_2545_58_294()
 {
    if (jj_done) return true;
    if (jj_3R_signature_2397_1_255()) return true;
    return false;
  }

 inline bool jj_3R_entity_declaration_1059_5_382()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_timeout_clause_2597_1_398()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_term_2592_2_137()
 {
    if (jj_done) return true;
    if (jj_3R_factor_1204_1_144()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_118()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_entity_class_entry_list_1053_2_506()
 {
    if (jj_done) return true;
    if (jj_3R_entity_class_entry_1048_2_575()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_entity_class_entry_list_1053_39_576()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_target_2587_3_266()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_411_3_117()) return true;
    return false;
  }

 inline bool jj_3R_target_2586_2_122()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_target_2586_2_265()) {
    jj_scanpos = xsp;
    if (jj_3R_target_2587_3_266()) return true;
    }
    return false;
  }

 inline bool jj_3_117()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_target_2586_2_265()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_entry_1048_2_575()
 {
    if (jj_done) return true;
    if (jj_3R_entity_class_1027_1_573()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_class_entry_1048_21_630()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_suffix_2581_3_238()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_suffix_2580_3_237()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_1871_1_332()) return true;
    return false;
  }

 inline bool jj_3R_entity_aspect_1020_27_637()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_suffix_2579_3_236()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_697_3_385()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1043_3_626()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1042_3_625()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1041_3_624()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNITS_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1040_3_623()
 {
    if (jj_done) return true;
    if (jj_scan_token(LITERAL_T)) return true;
    return false;
  }

 inline bool jj_3R_suffix_2577_1_108()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_suffix_2577_1_235()) {
    jj_scanpos = xsp;
    if (jj_3R_suffix_2579_3_236()) {
    jj_scanpos = xsp;
    if (jj_3R_suffix_2580_3_237()) {
    jj_scanpos = xsp;
    if (jj_3R_suffix_2581_3_238()) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_suffix_2577_1_235()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1039_3_622()
 {
    if (jj_done) return true;
    if (jj_scan_token(LABEL_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1038_3_621()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1037_3_620()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3_114()
 {
    if (jj_done) return true;
    if (jj_3R_reslution_indication_2564_3_141()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1036_3_619()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_subtype_indication_2571_3_89()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_114()) jj_scanpos = xsp;
    if (jj_3R_subtype_indication_2571_44_195()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_subtype_indication_2571_44_195()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_subtype_indication_2571_75_196()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_entity_class_1035_3_618()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1034_3_617()
 {
    if (jj_done) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1033_3_616()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1032_3_615()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1031_3_614()
 {
    if (jj_done) return true;
    if (jj_scan_token(FUNCTION_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1030_3_613()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1029_3_612()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    return false;
  }

 inline bool jj_3R_reslution_indication_2564_3_141()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1028_3_611()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARCHITECTURE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1027_1_610()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1027_1_573()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_class_1027_1_610()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1028_3_611()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1029_3_612()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1030_3_613()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1031_3_614()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1032_3_615()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1033_3_616()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1034_3_617()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1035_3_618()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1036_3_619()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1037_3_620()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1038_3_621()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1039_3_622()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1040_3_623()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1041_3_624()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1042_3_625()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1043_3_626()) return true;
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

 inline bool jj_3R_entity_aspect_1022_3_592()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_aspect_1021_3_591()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_entity_aspect_1020_1_590()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_aspect_1020_27_637()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_entity_aspect_1020_1_535()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_aspect_1020_1_590()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_aspect_1021_3_591()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_aspect_1022_3_592()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_subtype_declaration_2555_1_405()
 {
    if (jj_done) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_113()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(OF_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_instantiation_declaration_2545_2_138()
 {
    if (jj_done) return true;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_instantiation_declaration_2545_58_294()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_instantiation_declaration_2545_75_295()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_statement_part_2540_4_679()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2281_5_120()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_statement_part_2540_3_667()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_subprogram_statement_part_2540_4_679()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_element_declaration_998_3_722()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_33()
 {
    if (jj_done) return true;
    if (jj_3R_choices_718_3_90()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2529_3_651()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(77)) jj_scanpos = xsp;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_formal_parameter_list_1251_2_665()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_32()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    return false;
  }

 inline bool jj_3_31()
 {
    if (jj_done) return true;
    if (jj_3R_range_2158_4_88()) return true;
    return false;
  }

 inline bool jj_3R_element_association_988_2_392()
 {
    if (jj_done) return true;
    if (jj_3R_choices_718_3_90()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_element_association_988_1_257()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_element_association_988_2_392()) jj_scanpos = xsp;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2517_4_649()
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

 inline bool jj_3R_discrete_range_982_3_171()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2517_2_598()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_2517_4_649()) jj_scanpos = xsp;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_designator_957_2_647()) return true;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_2517_115_650()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_2529_3_651()) jj_scanpos = xsp;
    if (jj_scan_token(RETURN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3_113()) jj_scanpos = xsp;
    if (jj_3R_type_mark_2629_3_164()) return true;
    return false;
  }

 inline bool jj_3_112()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_2843_4_140()) return true;
    return false;
  }

 inline bool jj_3R_discrete_range_979_3_170()
 {
    if (jj_done) return true;
    if (jj_3R_range_2158_4_88()) return true;
    return false;
  }

 inline bool jj_3R_discrete_range_979_3_72()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_discrete_range_979_3_170()) {
    jj_scanpos = xsp;
    if (jj_3R_discrete_range_982_3_171()) return true;
    }
    return false;
  }

 inline bool jj_3_111()
 {
    if (jj_done) return true;
    if (jj_3R_gen_interface_list_2848_4_139()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2511_5_648()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_interface_list_1507_3_338()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_guarded_signal_specificatio_974_2_504()
 {
    if (jj_done) return true;
    if (jj_3R_signal_list_2382_2_574()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2503_2_597()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    if (jj_3R_designator_957_2_647()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_2511_5_648()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3_111()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3_112()) jj_scanpos = xsp;
    if (jj_3R_param_1573_3_462()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_2503_2_563()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_2503_2_597()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_specification_2517_2_598()) return true;
    }
    return false;
  }

 inline bool jj_3R_disconnection_specification_969_1_414()
 {
    if (jj_done) return true;
    if (jj_scan_token(DISCONNECT_T)) return true;
    if (jj_3R_guarded_signal_specificatio_974_2_504()) return true;
    if (jj_scan_token(AFTER_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_header_2498_6_664()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_generic_list_1321_2_165()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_110()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_direction_964_3_169()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOWNTO_T)) return true;
    return false;
  }

 inline bool jj_3R_constant_declaration_892_74_492()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_direction_963_1_168()
 {
    if (jj_done) return true;
    if (jj_scan_token(TO_T)) return true;
    return false;
  }

 inline bool jj_3R_direction_963_1_71()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_direction_963_1_168()) {
    jj_scanpos = xsp;
    if (jj_3R_direction_964_3_169()) return true;
    }
    return false;
  }

 inline bool jj_3R_designator_958_4_663()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_1871_1_332()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_kind_2492_2_680()
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

 inline bool jj_3R_designator_957_2_662()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_designator_957_2_647()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_designator_957_2_662()) {
    jj_scanpos = xsp;
    if (jj_3R_designator_958_4_663()) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_declarative_part_2487_4_678()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declarative_item_2466_1_693()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_part_2487_3_666()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_subprogram_declarative_part_2487_4_678()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_108()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_520_2_67()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2482_3_710()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1342_3_415()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2478_3_709()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2640_2_358()) return true;
    return false;
  }

 inline bool jj_3_109()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1350_2_68()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2477_3_708()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_541_3_412()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_940_5_589()
 {
    if (jj_done) return true;
    if (jj_scan_token(REJECT_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_940_3_532()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_delay_mechanism_940_5_589()) jj_scanpos = xsp;
    if (jj_scan_token(INERTIAL_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2475_1_707()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_520_2_67()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2473_3_706()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_416_2_410()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2472_3_705()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1214_2_409()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_939_1_531()
 {
    if (jj_done) return true;
    if (jj_scan_token(TRANSPORT_T)) return true;
    return false;
  }

 inline bool jj_3_30()
 {
    if (jj_done) return true;
    if (jj_3R_context_declaration_2806_2_87()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_939_1_435()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_delay_mechanism_939_1_531()) {
    jj_scanpos = xsp;
    if (jj_3R_delay_mechanism_940_3_532()) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2471_3_704()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_2676_1_408()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_body_2441_33_669()
 {
    if (jj_done) return true;
    if (jj_3R_designator_957_2_647()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2470_3_703()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_892_4_406()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2469_3_702()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_2555_1_405()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2468_4_701()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_2428_1_652()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2467_2_700()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_2610_1_404()) return true;
    return false;
  }

 inline bool jj_3_29()
 {
    if (jj_done) return true;
    if (jj_3R_index_constraint_1410_3_65()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2466_1_699()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2451_1_403()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_2466_1_693()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_declarative_item_2466_1_699()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2467_2_700()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2468_4_701()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2469_3_702()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2470_3_703()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2471_3_704()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2472_3_705()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2473_3_706()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2475_1_707()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2477_3_708()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2478_3_709()) {
    jj_scanpos = xsp;
    if (jj_3_109()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_2482_3_710()) return true;
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

 inline bool jj_3R_context_item_928_3_190()
 {
    if (jj_done) return true;
    if (jj_3R_context_ref_2798_2_359()) return true;
    return false;
  }

 inline bool jj_3_28()
 {
    if (jj_done) return true;
    if (jj_3R_range_constraint_2167_1_86()) return true;
    return false;
  }

 inline bool jj_3R_context_item_927_3_189()
 {
    if (jj_done) return true;
    if (jj_3R_context_declaration_2806_2_87()) return true;
    return false;
  }

 inline bool jj_3R_context_item_926_3_188()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2640_2_358()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_1_2460_2_599()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_2428_1_652()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_1_2460_2_564()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_1_2460_2_599()) {
    jj_scanpos = xsp;
    if (jj_scan_token(137)) return true;
    }
    return false;
  }

 inline bool jj_3R_context_item_925_2_187()
 {
    if (jj_done) return true;
    if (jj_3R_library_clause_1692_2_357()) return true;
    return false;
  }

 inline bool jj_3R_context_item_925_2_85()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_context_item_925_2_187()) {
    jj_scanpos = xsp;
    if (jj_3R_context_item_926_3_188()) {
    jj_scanpos = xsp;
    if (jj_3R_context_item_927_3_189()) {
    jj_scanpos = xsp;
    if (jj_3R_context_item_928_3_190()) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3_107()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_instantiation_declaration_2545_2_138()) return true;
    return false;
  }

 inline bool jj_3R_constraint_919_5_297()
 {
    if (jj_done) return true;
    if (jj_3R_index_constraint_1410_3_65()) return true;
    return false;
  }

 inline bool jj_3R_constraint_916_5_142()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_constraint_916_5_296()) {
    jj_scanpos = xsp;
    if (jj_3R_constraint_919_5_297()) return true;
    }
    return false;
  }

 inline bool jj_3R_constraint_916_5_296()
 {
    if (jj_done) return true;
    if (jj_3R_range_constraint_2167_1_86()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declaration_2454_1_490()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_specification_2503_2_563()) return true;
    if (jj_3R_subprogram_1_2460_2_564()) return true;
    return false;
  }

 inline bool jj_3R_signal_declaration_2366_89_494()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_body_2441_11_668()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_kind_2492_2_680()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declaration_2451_1_403()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_declaration_2451_1_489()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declaration_2454_1_490()) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_declaration_2451_1_489()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_instantiation_declaration_2545_2_138()) return true;
    return false;
  }

 inline bool jj_3_106()
 {
    if (jj_done) return true;
    if (jj_3R_adding_operator_404_3_136()) return true;
    if (jj_3R_term_2592_2_137()) return true;
    return false;
  }

 inline bool jj_3_27()
 {
    if (jj_done) return true;
    if (jj_3R_context_item_925_2_85()) return true;
    return false;
  }

 inline bool jj_3R_constraint_array_definition_905_1_720()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_3R_index_constraint_1410_3_65()) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    return false;
  }

 inline bool jj_3R_simple_expression_2406_35_167()
 {
    if (jj_done) return true;
    if (jj_3R_adding_operator_404_3_136()) return true;
    if (jj_3R_term_2592_2_137()) return true;
    return false;
  }

 inline bool jj_3R_signal_declaration_2366_68_493()
 {
    if (jj_done) return true;
    if (jj_3R_signal_kind_2376_3_566()) return true;
    return false;
  }

 inline bool jj_3R_constant_declaration_892_4_406()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_constant_declaration_892_74_492()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_body_2428_1_652()
 {
    if (jj_done) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_subprogram_declarative_part_2487_3_666()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_subprogram_statement_part_2540_3_667()) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_body_2441_11_668()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_body_2441_33_669()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_26()
 {
    if (jj_done) return true;
    if (jj_3R_component_configuration_723_5_84()) return true;
    return false;
  }

 inline bool jj_3R_configuration_specification_887_1_413()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_component_specification_762_1_184()) return true;
    if (jj_3R_binding_indication_582_2_355()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_string_literal_2421_1_389()
 {
    if (jj_done) return true;
    if (jj_scan_token(STRINGLITERAL)) return true;
    return false;
  }

 inline bool jj_3R_configuration_item_881_4_595()
 {
    if (jj_done) return true;
    if (jj_3R_block_configuration_600_5_356()) return true;
    return false;
  }

 inline bool jj_3R_configuration_item_879_2_594()
 {
    if (jj_done) return true;
    if (jj_3R_component_configuration_723_5_84()) return true;
    return false;
  }

 inline bool jj_3R_configuration_item_879_2_537()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_configuration_item_879_2_594()) {
    jj_scanpos = xsp;
    if (jj_3R_configuration_item_881_4_595()) return true;
    }
    return false;
  }

 inline bool jj_3R_signature_2398_15_466()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_simple_expression_2406_3_166()
 {
    if (jj_done) return true;
    if (jj_3R_sign_2336_2_339()) return true;
    return false;
  }

 inline bool jj_3R_simple_expression_2406_1_70()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_simple_expression_2406_3_166()) jj_scanpos = xsp;
    if (jj_3R_term_2592_2_137()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_simple_expression_2406_35_167()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_signature_2399_4_391()
 {
    if (jj_done) return true;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_signature_2398_4_390()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_signature_2398_15_466()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_signature_2397_1_255()
 {
    if (jj_done) return true;
    if (jj_scan_token(LBRACKET_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signature_2398_4_390()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_signature_2399_4_391()) jj_scanpos = xsp;
    if (jj_scan_token(RBRACKET_T)) return true;
    return false;
  }

 inline bool jj_3_25()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_list_2382_12_660()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_configuration_declaration_853_2_383()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveforms_846_7_345()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    return false;
  }

 inline bool jj_3R_signal_list_2384_3_629()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_list_2383_3_628()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveforms_844_7_344()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_waveform_2716_1_343()) return true;
    return false;
  }

 inline bool jj_3R_signal_list_2382_2_627()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_signal_list_2382_12_660()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_signal_list_2382_2_574()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_list_2382_2_627()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_list_2383_3_628()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_list_2384_3_629()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_signal_kind_2377_5_602()
 {
    if (jj_done) return true;
    if (jj_scan_token(BUS_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_kind_2376_3_601()
 {
    if (jj_done) return true;
    if (jj_scan_token(REGISTER_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_kind_2376_3_566()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_kind_2376_3_601()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_kind_2377_5_602()) return true;
    }
    return false;
  }

 inline bool jj_3R_conditional_waveforms_843_1_176()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_2716_1_343()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_conditional_waveforms_844_7_344()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_conditional_waveforms_846_7_345()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_838_2_75()
 {
    if (jj_done) return true;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_3R_options_1876_3_175()) return true;
    if (jj_3R_conditional_waveforms_843_1_176()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_condition_clause_829_3_397()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNTIL_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    return false;
  }

 inline bool jj_3R_signal_declaration_2366_1_407()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_declaration_2366_68_493()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_declaration_2366_89_494()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_condition_824_3_83()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3_24()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_procedure_call_statement_778_1_82()) return true;
    return false;
  }

 inline bool jj_3_23()
 {
    if (jj_done) return true;
    if (jj_3R_component_instantiation_statement_745_2_81()) return true;
    return false;
  }

 inline bool jj_3_105()
 {
    if (jj_done) return true;
    if (jj_3R_selected_signal_assignment_wave_3002_3_135()) return true;
    return false;
  }

 inline bool jj_3_22()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_signal_assignment_statement_783_1_80()) return true;
    return false;
  }

 inline bool jj_3_104()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_wave_2973_3_134()) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2351_3_557()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_939_1_435()) return true;
    return false;
  }

 inline bool jj_3R_null_807_12_79()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_21()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_807_12_79()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(ASSERT_T)) return true;
    return false;
  }

 inline bool jj_3_103()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2349_3_469()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_103()) jj_scanpos = xsp;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_signal_assignment_statement_2351_3_557()) jj_scanpos = xsp;
    if (jj_3R_waveform_2716_1_343()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2346_3_468()
 {
    if (jj_done) return true;
    if (jj_3R_selected_signal_assignment_wave_3002_3_135()) return true;
    return false;
  }

 inline bool jj_3R_shift_expression_2322_26_419()
 {
    if (jj_done) return true;
    if (jj_3R_shift_operator_2326_3_509()) return true;
    if (jj_3R_simple_expression_2406_1_70()) return true;
    return false;
  }

 inline bool jj_3R_null_799_12_77()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2343_3_393()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_assignment_statement_2343_3_467()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_assignment_statement_2346_3_468()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_assignment_statement_2349_3_469()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_2343_3_467()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_wave_2973_3_134()) return true;
    return false;
  }

 inline bool jj_3_19()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_799_12_77()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    return false;
  }

 inline bool jj_3R_null_796_12_76()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_18()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_796_12_76()) jj_scanpos = xsp;
    if (jj_scan_token(BLOCK_T)) return true;
    return false;
  }

 inline bool jj_3R_sign_2337_4_432()
 {
    if (jj_done) return true;
    if (jj_scan_token(MINUS_T)) return true;
    return false;
  }

 inline bool jj_3_20()
 {
    if (jj_done) return true;
    if (jj_3R_generate_statement_1298_1_78()) return true;
    return false;
  }

 inline bool jj_3R_sign_2336_2_339()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sign_2336_2_431()) {
    jj_scanpos = xsp;
    if (jj_3R_sign_2337_4_432()) return true;
    }
    return false;
  }

 inline bool jj_3R_sign_2336_2_431()
 {
    if (jj_done) return true;
    if (jj_scan_token(PLUS_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2331_5_585()
 {
    if (jj_done) return true;
    if (jj_scan_token(ROR_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2330_5_584()
 {
    if (jj_done) return true;
    if (jj_scan_token(ROL_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2329_5_583()
 {
    if (jj_done) return true;
    if (jj_scan_token(SRA_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2328_5_582()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLA_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2327_5_581()
 {
    if (jj_done) return true;
    if (jj_scan_token(SRL_T)) return true;
    return false;
  }

 inline bool jj_3_17()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_838_2_75()) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2326_3_580()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLL_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2326_3_509()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_shift_operator_2326_3_580()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2327_5_581()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2328_5_582()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2329_5_583()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2330_5_584()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2331_5_585()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_787_3_179()
 {
    if (jj_done) return true;
    if (jj_3R_selected_signal_assignment_2248_3_348()) return true;
    return false;
  }

 inline bool jj_3R_shift_expression_2322_2_329()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2406_1_70()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_shift_expression_2322_26_419()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_sequential_statement_2317_5_264()
 {
    if (jj_done) return true;
    if (jj_3R_null_statement_1848_1_395()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_785_1_178()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_838_2_75()) return true;
    return false;
  }

 inline bool jj_3_16()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_783_1_80()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_16()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_concurrent_signal_assignment_statement_785_1_178()) {
    jj_scanpos = xsp;
    if (jj_3R_concurrent_signal_assignment_statement_787_3_179()) return true;
    }
    return false;
  }

 inline bool jj_3_102()
 {
    if (jj_done) return true;
    if (jj_3R_return_statement_2216_1_133()) return true;
    return false;
  }

 inline bool jj_3_15()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_101()
 {
    if (jj_done) return true;
    if (jj_3R_exit_statement_1169_1_132()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_procedure_call_statement_778_1_82()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_15()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_3R_procedure_call_2062_2_183()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_100()
 {
    if (jj_done) return true;
    if (jj_3R_next_statement_1835_1_131()) return true;
    return false;
  }

 inline bool jj_3_14()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_13()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1326_6_73()) return true;
    return false;
  }

 inline bool jj_3R_null_2293_19_126()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_95()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_2293_19_126()) jj_scanpos = xsp;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    return false;
  }

 inline bool jj_3_99()
 {
    if (jj_done) return true;
    if (jj_3R_loop_statement_1736_1_130()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_assertion_statement_773_1_92()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_14()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_3R_assertion_490_4_199()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_98()
 {
    if (jj_done) return true;
    if (jj_3R_case_statement_668_1_129()) return true;
    return false;
  }

 inline bool jj_3R_composite_type_definition_768_3_684()
 {
    if (jj_done) return true;
    if (jj_3R_record_type_definition_2172_8_696()) return true;
    return false;
  }

 inline bool jj_3_97()
 {
    if (jj_done) return true;
    if (jj_3R_if_statement_1374_3_128()) return true;
    return false;
  }

 inline bool jj_3R_composite_type_definition_767_2_683()
 {
    if (jj_done) return true;
    if (jj_3R_array_type_definition_481_2_695()) return true;
    return false;
  }

 inline bool jj_3R_composite_type_definition_767_2_671()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_composite_type_definition_767_2_683()) {
    jj_scanpos = xsp;
    if (jj_3R_composite_type_definition_768_3_684()) return true;
    }
    return false;
  }

 inline bool jj_3R_component_instantiation_statement_757_11_182()
 {
    if (jj_done) return true;
    if (jj_3R_port_map_aspect_2015_2_352()) return true;
    return false;
  }

 inline bool jj_3R_component_instantiation_statement_756_11_181()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1326_6_73()) return true;
    return false;
  }

 inline bool jj_3_96()
 {
    if (jj_done) return true;
    if (jj_3R_procedure_call_statement_2068_1_127()) return true;
    return false;
  }

 inline bool jj_3R_component_declaration_738_27_502()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_null_2281_18_121()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_component_specification_762_1_184()
 {
    if (jj_done) return true;
    if (jj_3R_instantiation_list_1434_3_354()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_2293_5_263()
 {
    if (jj_done) return true;
    if (jj_3R_variable_assignment_statement_2667_1_394()) return true;
    return false;
  }

 inline bool jj_3_91()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_2281_18_121()) jj_scanpos = xsp;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    return false;
  }

 inline bool jj_3_94()
 {
    if (jj_done) return true;
    if (jj_3R_wait_statement_2707_1_125()) return true;
    return false;
  }

 inline bool jj_3_93()
 {
    if (jj_done) return true;
    if (jj_3R_report_statement_2203_1_124()) return true;
    return false;
  }

 inline bool jj_3_92()
 {
    if (jj_done) return true;
    if (jj_3R_assertion_statement_501_3_123()) return true;
    return false;
  }

 inline bool jj_3R_selected_waveforms_2255_32_534()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_waveform_2716_1_343()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_718_3_90()) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_2281_5_120()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sequential_statement_2281_5_262()) {
    jj_scanpos = xsp;
    if (jj_3_92()) {
    jj_scanpos = xsp;
    if (jj_3_93()) {
    jj_scanpos = xsp;
    if (jj_3_94()) {
    jj_scanpos = xsp;
    if (jj_3R_sequential_statement_2293_5_263()) {
    jj_scanpos = xsp;
    if (jj_3_96()) {
    jj_scanpos = xsp;
    if (jj_3_97()) {
    jj_scanpos = xsp;
    if (jj_3_98()) {
    jj_scanpos = xsp;
    if (jj_3_99()) {
    jj_scanpos = xsp;
    if (jj_3_100()) {
    jj_scanpos = xsp;
    if (jj_3_101()) {
    jj_scanpos = xsp;
    if (jj_3_102()) {
    jj_scanpos = xsp;
    if (jj_3R_sequential_statement_2317_5_264()) return true;
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

 inline bool jj_3R_sequential_statement_2281_5_262()
 {
    if (jj_done) return true;
    if (jj_3R_signal_assignment_statement_2343_3_393()) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_list_2271_12_538()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_component_instantiation_statement_745_2_81()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_instantiation_unit_1427_1_180()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_instantiation_statement_756_11_181()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_instantiation_statement_757_11_182()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_90()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2281_5_120()) return true;
    return false;
  }

 inline bool jj_3R_sequence_of_statement_2276_2_278()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_90()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_component_declaration_733_5_501()
 {
    if (jj_done) return true;
    if (jj_3R_port_clause_2005_4_571()) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_list_2271_2_455()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sensitivity_list_2271_12_538()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_component_declaration_732_5_500()
 {
    if (jj_done) return true;
    if (jj_3R_generic_clause_1316_2_69()) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_list_2269_3_368()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sensitivity_list_2269_3_454()) {
    jj_scanpos = xsp;
    if (jj_3R_sensitivity_list_2271_2_455()) return true;
    }
    return false;
  }

 inline bool jj_3R_sensitivity_list_2269_3_454()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_component_configuration_725_11_186()
 {
    if (jj_done) return true;
    if (jj_3R_block_configuration_600_5_356()) return true;
    return false;
  }

 inline bool jj_3R_component_configuration_724_11_185()
 {
    if (jj_done) return true;
    if (jj_3R_binding_indication_582_2_355()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_return_statement_2216_53_286()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_choices_718_15_198()
 {
    if (jj_done) return true;
    if (jj_scan_token(BAR_T)) return true;
    if (jj_3R_choice_702_4_197()) return true;
    return false;
  }

 inline bool jj_3R_component_declaration_730_2_411()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_declaration_732_5_500()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_declaration_733_5_501()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_component_declaration_738_27_502()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_component_configuration_723_5_84()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_component_specification_762_1_184()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_configuration_724_11_185()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_configuration_725_11_186()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_clause_2260_2_396()
 {
    if (jj_done) return true;
    if (jj_scan_token(ON_T)) return true;
    if (jj_3R_sensitivity_list_2269_3_368()) return true;
    return false;
  }

 inline bool jj_3_12()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_979_3_72()) return true;
    return false;
  }

 inline bool jj_3R_selected_waveforms_2255_2_438()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_2716_1_343()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_718_3_90()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_selected_waveforms_2255_32_534()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_choices_718_3_90()
 {
    if (jj_done) return true;
    if (jj_3R_choice_702_4_197()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_choices_718_15_198()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_11()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2406_1_70()) return true;
    return false;
  }

 inline bool jj_3R_choice_712_5_365()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3_10()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2406_1_70()) return true;
    if (jj_3R_direction_963_1_71()) return true;
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_2248_3_348()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    if (jj_3R_target_2586_2_122()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_3R_options_1876_3_175()) return true;
    if (jj_3R_selected_waveforms_2255_2_438()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2224_26_694()
 {
    if (jj_done) return true;
    if (jj_3R_physical_type_definition_1993_9_711()) return true;
    return false;
  }

 inline bool jj_3R_choice_711_4_364()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_choice_708_4_363()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_979_3_72()) return true;
    return false;
  }

 inline bool jj_3R_selected_name_2243_2_449()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_suffix_2577_1_108()) return true;
    return false;
  }

 inline bool jj_3R_choice_705_4_362()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2406_1_70()) return true;
    return false;
  }

 inline bool jj_3R_choice_702_4_197()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_choice_702_4_361()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_705_4_362()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_708_4_363()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_711_4_364()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_712_5_365()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_choice_702_4_361()
 {
    if (jj_done) return true;
    if (jj_3R_range_2158_4_88()) return true;
    return false;
  }

 inline bool jj_3R_case_statement_677_28_645()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_secondary_unit_declaration_2235_1_738()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(EQU_T)) return true;
    if (jj_3R_physical_literal_1988_2_111()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_character_literal_697_3_385()
 {
    if (jj_done) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    return false;
  }

 inline bool jj_3_89()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_type_definition_1163_3_119()) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2224_3_682()
 {
    if (jj_done) return true;
    if (jj_3R_range_constraint_2167_1_86()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_scalar_type_definition_2224_26_694()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_case_statement_alternative_686_3_643()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_718_3_90()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    if (jj_3R_sequence_of_statement_2276_2_278()) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2222_1_681()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_type_definition_1163_3_119()) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2222_1_670()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_scalar_type_definition_2222_1_681()) {
    jj_scanpos = xsp;
    if (jj_3R_scalar_type_definition_2224_3_682()) return true;
    }
    return false;
  }

 inline bool jj_3R_case_statement_676_11_644()
 {
    if (jj_done) return true;
    if (jj_3R_case_statement_alternative_686_3_643()) return true;
    return false;
  }

 inline bool jj_3R_return_statement_2216_3_285()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_return_statement_2216_1_133()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_return_statement_2216_3_285()) jj_scanpos = xsp;
    if (jj_scan_token(RETURN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_return_statement_2216_53_286()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_report_statement_2205_11_269()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEVERITY_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_case_statement_668_3_275()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(155)) jj_scanpos = xsp;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_report_statement_2203_3_268()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_2180_25_330()
 {
    if (jj_done) return true;
    if (jj_3R_relation_operator_2185_2_420()) return true;
    if (jj_3R_shift_expression_2322_2_329()) return true;
    return false;
  }

 inline bool jj_3R_case_statement_668_1_129()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_case_statement_668_3_275()) jj_scanpos = xsp;
    if (jj_scan_token(CASE_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_case_statement_alternative_686_3_643()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_case_statement_676_11_644()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(CASE_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_case_statement_677_28_645()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_report_statement_2203_1_124()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_report_statement_2203_3_268()) jj_scanpos = xsp;
    if (jj_scan_token(REPORT_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    xsp = jj_scanpos;
    if (jj_3R_report_statement_2205_11_269()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2196_3_521()
 {
    if (jj_done) return true;
    if (jj_scan_token(QNEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2195_3_520()
 {
    if (jj_done) return true;
    if (jj_scan_token(QEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2194_3_519()
 {
    if (jj_done) return true;
    if (jj_scan_token(QL_T)) return true;
    return false;
  }

 inline bool jj_3R_record_type_definition_2175_21_715()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2193_3_518()
 {
    if (jj_done) return true;
    if (jj_scan_token(QG_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2192_3_517()
 {
    if (jj_done) return true;
    if (jj_scan_token(QLT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2191_3_516()
 {
    if (jj_done) return true;
    if (jj_scan_token(QGT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2190_3_515()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOTEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2189_3_514()
 {
    if (jj_done) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    return false;
  }

 inline bool jj_3R_block_specification_644_12_536()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_index_specification_1415_2_593()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2188_3_513()
 {
    if (jj_done) return true;
    if (jj_scan_token(GREATERTHAN_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2187_3_512()
 {
    if (jj_done) return true;
    if (jj_scan_token(EQU_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2186_3_511()
 {
    if (jj_done) return true;
    if (jj_scan_token(GT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2185_2_510()
 {
    if (jj_done) return true;
    if (jj_scan_token(LT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2185_2_420()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_relation_operator_2185_2_510()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2186_3_511()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2187_3_512()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2188_3_513()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2189_3_514()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2190_3_515()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2191_3_516()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2192_3_517()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2193_3_518()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2194_3_519()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2195_3_520()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2196_3_521()) return true;
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

 inline bool jj_3_9()
 {
    if (jj_done) return true;
    if (jj_3R_generic_clause_1316_2_69()) return true;
    return false;
  }

 inline bool jj_3R_relation_2180_3_154()
 {
    if (jj_done) return true;
    if (jj_3R_shift_expression_2322_2_329()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_relation_2180_25_330()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_record_type_definition_2173_9_714()
 {
    if (jj_done) return true;
    if (jj_3R_element_declaration_998_3_722()) return true;
    return false;
  }

 inline bool jj_3R_block_specification_644_3_446()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_block_specification_644_12_536()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_record_type_definition_2172_8_696()
 {
    if (jj_done) return true;
    if (jj_scan_token(RECORD_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_record_type_definition_2173_9_714()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(RECORD_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_record_type_definition_2175_21_715()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_87()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2406_1_70()) return true;
    if (jj_3R_direction_963_1_71()) return true;
    if (jj_3R_simple_expression_2406_1_70()) return true;
    return false;
  }

 inline bool jj_3_88()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_name_536_3_118()) return true;
    return false;
  }

 inline bool jj_3R_range_constraint_2167_1_86()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_3R_range_2158_4_88()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_627_3_322()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1342_3_415()) return true;
    return false;
  }

 inline bool jj_3_7()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_520_2_67()) return true;
    return false;
  }

 inline bool jj_3_86()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_411_3_117()) return true;
    return false;
  }

 inline bool jj_3R_range_2161_2_194()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_name_536_3_118()) return true;
    return false;
  }

 inline bool jj_3R_range_2158_4_193()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_2406_1_70()) return true;
    if (jj_3R_direction_963_1_71()) return true;
    if (jj_3R_simple_expression_2406_1_70()) return true;
    return false;
  }

 inline bool jj_3R_range_2158_4_88()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_range_2158_4_193()) {
    jj_scanpos = xsp;
    if (jj_3R_range_2161_2_194()) return true;
    }
    return false;
  }

 inline bool jj_3_8()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1350_2_68()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_623_3_321()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2640_2_358()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_622_3_320()
 {
    if (jj_done) return true;
    if (jj_3R_disconnection_specification_969_1_414()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_621_3_319()
 {
    if (jj_done) return true;
    if (jj_3R_configuration_specification_887_1_413()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_620_3_318()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_541_3_412()) return true;
    return false;
  }

 inline bool jj_3R_qualified_expression_2151_7_248()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_616_3_316()
 {
    if (jj_done) return true;
    if (jj_3R_component_declaration_730_2_411()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_618_1_317()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_520_2_67()) return true;
    return false;
  }

 inline bool jj_3R_qualified_expression_2149_5_247()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_411_3_117()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_615_3_315()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_416_2_410()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_614_3_314()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1214_2_409()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_613_3_313()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_2676_1_408()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_612_3_312()
 {
    if (jj_done) return true;
    if (jj_3R_signal_declaration_2366_1_407()) return true;
    return false;
  }

 inline bool jj_3R_qualified_expression_2147_3_64()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_qualified_expression_2149_5_247()) {
    jj_scanpos = xsp;
    if (jj_3R_qualified_expression_2151_7_248()) return true;
    }
    return false;
  }

 inline bool jj_3R_block_declarative_item_611_3_311()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_892_4_406()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_610_3_310()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_2555_1_405()) return true;
    return false;
  }

 inline bool jj_3R_block_configuration_602_11_448()
 {
    if (jj_done) return true;
    if (jj_3R_configuration_item_879_2_537()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_609_3_309()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_2610_1_404()) return true;
    return false;
  }

 inline bool jj_3R_block_configuration_601_11_447()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2640_2_358()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_608_3_149()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_block_declarative_item_608_3_308()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_609_3_309()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_610_3_310()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_611_3_311()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_612_3_312()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_613_3_313()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_614_3_314()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_615_3_315()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_616_3_316()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_618_1_317()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_620_3_318()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_621_3_319()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_622_3_320()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_623_3_321()) {
    jj_scanpos = xsp;
    if (jj_3_8()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_627_3_322()) return true;
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

 inline bool jj_3R_block_declarative_item_608_3_308()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2451_1_403()) return true;
    return false;
  }

 inline bool jj_3R_process_statement_part_2142_4_370()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2281_5_120()) return true;
    return false;
  }

 inline bool jj_3R_process_statement_part_2142_3_203()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_process_statement_part_2142_4_370()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_process_statement_2120_24_204()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_block_configuration_600_5_356()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_block_specification_644_3_446()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_block_configuration_601_11_447()) { jj_scanpos = xsp; break; }
    }
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_block_configuration_602_11_448()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_name_536_68_260()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_bit_string_literal_589_3_103()
 {
    if (jj_done) return true;
    if (jj_scan_token(BIT_STRING_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_584_4_445()
 {
    if (jj_done) return true;
    if (jj_3R_port_map_aspect_2015_2_352()) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_583_4_444()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1326_6_73()) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_582_4_443()
 {
    if (jj_done) return true;
    if (jj_scan_token(USE_T)) return true;
    if (jj_3R_entity_aspect_1020_1_535()) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_582_2_355()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_binding_indication_582_4_443()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_binding_indication_583_4_444()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_binding_indication_584_4_445()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_attribute_name_536_46_259()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_process_statement_2110_4_201()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_sensitivity_list_2269_3_368()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_process_statement_2102_3_200()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_process_statement_2102_1_93()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_process_statement_2102_3_200()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_process_statement_2110_4_201()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    if (jj_3R_process_declarative_part_2097_2_202()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_process_statement_part_2142_3_203()) return true;
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_process_statement_2120_24_204()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_part_2097_4_369()
 {
    if (jj_done) return true;
    if (jj_3R_process_declarative_item_2076_1_456()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_part_2097_2_202()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_process_declarative_part_2097_4_369()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_process_declarative_item_2092_3_548()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1342_3_415()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2088_3_547()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_2640_2_358()) return true;
    return false;
  }

 inline bool jj_3_85()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1350_2_68()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2087_3_546()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_541_3_412()) return true;
    return false;
  }

 inline bool jj_3R_assertion_490_63_367()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEVERITY_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2083_3_545()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_416_2_410()) return true;
    return false;
  }

 inline bool jj_3_84()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_520_2_67()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2082_3_544()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1214_2_409()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2081_3_543()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_2676_1_408()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2080_3_542()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_892_4_406()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2079_3_541()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_2555_1_405()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2078_3_540()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_2610_1_404()) return true;
    return false;
  }

 inline bool jj_3R_attribute_specification_541_3_412()
 {
    if (jj_done) return true;
    if (jj_scan_token(ATTRIBUTE_T)) return true;
    if (jj_3R_attribute_designator_529_3_240()) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_entity_specification_1129_1_503()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2076_1_539()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_2451_1_403()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2076_1_456()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_process_declarative_item_2076_1_539()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2078_3_540()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2079_3_541()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2080_3_542()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2081_3_543()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2082_3_544()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2083_3_545()) {
    jj_scanpos = xsp;
    if (jj_3_84()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2087_3_546()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2088_3_547()) {
    jj_scanpos = xsp;
    if (jj_3_85()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2092_3_548()) return true;
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

 inline bool jj_3R_association_list_515_26_341()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_association_element_509_2_340()) return true;
    return false;
  }

 inline bool jj_3R_procedure_call_2062_14_353()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_actual_parameter_part_385_4_246()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_name_536_3_118()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(85)) {
    jj_scanpos = xsp;
    if (jj_3R_attribute_name_536_46_259()) return true;
    }
    xsp = jj_scanpos;
    if (jj_3R_attribute_name_536_68_260()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_attribute_designator_530_5_388()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    return false;
  }

 inline bool jj_3_83()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_procedure_call_statement_2068_1_127()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_83()) jj_scanpos = xsp;
    if (jj_3R_procedure_call_2062_2_183()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_designator_529_3_240()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_attribute_designator_529_3_387()) {
    jj_scanpos = xsp;
    if (jj_3R_attribute_designator_530_5_388()) return true;
    }
    return false;
  }

 inline bool jj_3R_attribute_designator_529_3_387()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_procedure_call_2062_2_183()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_procedure_call_2062_14_353()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_81()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2822_2_91()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_2056_4_224()
 {
    if (jj_done) return true;
    if (jj_3R_context_declaration_2806_2_87()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_2055_3_223()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_1923_2_384()) return true;
    return false;
  }

 inline bool jj_3_6()
 {
    if (jj_done) return true;
    if (jj_3R_formal_part_1256_1_66()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_declaration_520_2_67()
 {
    if (jj_done) return true;
    if (jj_scan_token(ATTRIBUTE_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_type_mark_2629_3_164()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_assertion_490_31_366()
 {
    if (jj_done) return true;
    if (jj_scan_token(REPORT_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3_82()
 {
    if (jj_done) return true;
    if (jj_3R_interface_package_declaration_2831_2_97()) return true;
    return false;
  }

 inline bool jj_3R_association_list_515_1_172()
 {
    if (jj_done) return true;
    if (jj_3R_association_element_509_2_340()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_association_list_515_26_341()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_primary_unit_2048_3_221()
 {
    if (jj_done) return true;
    if (jj_3R_configuration_declaration_853_2_383()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_2050_1_222()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2822_2_91()) return true;
    return false;
  }

 inline bool jj_3_80()
 {
    if (jj_done) return true;
    if (jj_3R_allocator_436_3_116()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_2047_1_102()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_primary_unit_2047_1_220()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_2048_3_221()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_2050_1_222()) {
    jj_scanpos = xsp;
    if (jj_3_82()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_2055_3_223()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_2056_4_224()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_primary_unit_2047_1_220()
 {
    if (jj_done) return true;
    if (jj_3R_entity_declaration_1059_5_382()) return true;
    return false;
  }

 inline bool jj_3R_association_element_509_3_433()
 {
    if (jj_done) return true;
    if (jj_3R_formal_part_1256_1_66()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_association_element_509_2_340()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_association_element_509_3_433()) jj_scanpos = xsp;
    if (jj_3R_actual_part_391_2_434()) return true;
    return false;
  }

 inline bool jj_3_79()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3_78()
 {
    if (jj_done) return true;
    if (jj_3R_literal_1716_2_115()) return true;
    return false;
  }

 inline bool jj_3R_primary_2041_1_488()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_411_3_117()) return true;
    return false;
  }

 inline bool jj_3R_assertion_statement_501_5_267()
 {
    if (jj_done) return true;
    if (jj_3R_label_1687_2_148()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_77()
 {
    if (jj_done) return true;
    if (jj_3R_type_conversion_2605_3_114()) return true;
    return false;
  }

 inline bool jj_3R_assertion_statement_501_3_123()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_assertion_statement_501_5_267()) jj_scanpos = xsp;
    if (jj_3R_assertion_490_4_199()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_primary_2038_1_487()
 {
    if (jj_done) return true;
    if (jj_3R_allocator_436_3_116()) return true;
    return false;
  }

 inline bool jj_3_76()
 {
    if (jj_done) return true;
    if (jj_3R_qualified_expression_2147_3_64()) return true;
    return false;
  }

 inline bool jj_3R_primary_2035_1_486()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3_75()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_primary_2032_1_485()
 {
    if (jj_done) return true;
    if (jj_3R_literal_1716_2_115()) return true;
    return false;
  }

 inline bool jj_3R_physical_type_definition_1998_33_719()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3_74()
 {
    if (jj_done) return true;
    if (jj_3R_function_call_1293_1_113()) return true;
    return false;
  }

 inline bool jj_3R_primary_2029_1_484()
 {
    if (jj_done) return true;
    if (jj_3R_type_conversion_2605_3_114()) return true;
    return false;
  }

 inline bool jj_3R_assertion_490_4_199()
 {
    if (jj_done) return true;
    if (jj_scan_token(ASSERT_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_assertion_490_31_366()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_assertion_490_63_367()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_5()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_3R_index_constraint_1410_3_65()) return true;
    if (jj_scan_token(OF_T)) return true;
    return false;
  }

 inline bool jj_3R_primary_2026_1_483()
 {
    if (jj_done) return true;
    if (jj_3R_qualified_expression_2147_3_64()) return true;
    return false;
  }

 inline bool jj_3R_primary_2023_1_482()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_array_type_definition_484_4_713()
 {
    if (jj_done) return true;
    if (jj_3R_unconstraint_array_definition_2634_1_721()) return true;
    return false;
  }

 inline bool jj_3R_primary_2020_1_402()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_primary_2020_1_481()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2023_1_482()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2026_1_483()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2029_1_484()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2032_1_485()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2035_1_486()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2038_1_487()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2041_1_488()) return true;
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_primary_2020_1_481()
 {
    if (jj_done) return true;
    if (jj_3R_function_call_1293_1_113()) return true;
    return false;
  }

 inline bool jj_3R_array_type_definition_481_2_712()
 {
    if (jj_done) return true;
    if (jj_3R_constraint_array_definition_905_1_720()) return true;
    return false;
  }

 inline bool jj_3R_array_type_definition_481_2_695()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_array_type_definition_481_2_712()) {
    jj_scanpos = xsp;
    if (jj_3R_array_type_definition_484_4_713()) return true;
    }
    return false;
  }

 inline bool jj_3R_port_map_aspect_2015_2_352()
 {
    if (jj_done) return true;
    if (jj_scan_token(PORT_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_association_list_515_1_172()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_port_list_2010_3_606()
 {
    if (jj_done) return true;
    if (jj_3R_interface_list_1507_3_338()) return true;
    return false;
  }

 inline bool jj_3R_physical_type_definition_1996_14_718()
 {
    if (jj_done) return true;
    if (jj_3R_secondary_unit_declaration_2235_1_738()) return true;
    return false;
  }

 inline bool jj_3R_port_clause_2005_4_571()
 {
    if (jj_done) return true;
    if (jj_scan_token(PORT_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_port_list_2010_3_606()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_physical_type_definition_1993_9_711()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNITS_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_physical_type_definition_1996_14_718()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(UNITS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_physical_type_definition_1998_33_719()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_73()
 {
    if (jj_done) return true;
    if (jj_3R_abstract_literal_356_4_112()) return true;
    return false;
  }

 inline bool jj_3R_alias_declaration_418_44_499()
 {
    if (jj_done) return true;
    if (jj_3R_signature_2397_1_255()) return true;
    return false;
  }

 inline bool jj_3R_physical_literal_1988_3_242()
 {
    if (jj_done) return true;
    if (jj_3R_abstract_literal_356_4_112()) return true;
    return false;
  }

 inline bool jj_3R_physical_literal_1988_2_111()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_physical_literal_1988_3_242()) jj_scanpos = xsp;
    if (jj_3R_name_1783_2_63()) return true;
    return false;
  }

 inline bool jj_3R_aggregate_411_39_258()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_element_association_988_1_257()) return true;
    return false;
  }

 inline bool jj_3R_parameter_specification_1983_1_559()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(IN_T)) return true;
    if (jj_3R_discrete_range_979_3_72()) return true;
    return false;
  }

 inline bool jj_3R_allocator_438_6_254()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    return false;
  }

 inline bool jj_3R_allocator_436_3_116()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_4()) {
    jj_scanpos = xsp;
    if (jj_3R_allocator_438_6_254()) return true;
    }
    return false;
  }

 inline bool jj_3_4()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_qualified_expression_2147_3_64()) return true;
    return false;
  }

 inline bool jj_3_72()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2822_2_91()) return true;
    return false;
  }

 inline bool jj_3_70()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_520_2_67()) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_431_4_570()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_1871_1_332()) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_430_4_569()
 {
    if (jj_done) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    return false;
  }

 inline bool jj_3_71()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1350_2_68()) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_429_2_568()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_429_2_497()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_alias_designator_429_2_568()) {
    jj_scanpos = xsp;
    if (jj_3R_alias_designator_430_4_569()) {
    jj_scanpos = xsp;
    if (jj_3R_alias_designator_431_4_570()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_alias_declaration_417_4_498()
 {
    if (jj_done) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    return false;
  }

 inline bool jj_3R_alias_declaration_416_2_410()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALIAS_T)) return true;
    if (jj_3R_alias_designator_429_2_497()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_alias_declaration_417_4_498()) jj_scanpos = xsp;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    xsp = jj_scanpos;
    if (jj_3R_alias_declaration_418_44_499()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_aggregate_411_3_117()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_element_association_988_1_257()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_aggregate_411_39_258()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_adding_operator_405_5_292()
 {
    if (jj_done) return true;
    if (jj_scan_token(MINUS_T)) return true;
    return false;
  }

 inline bool jj_3R_adding_operator_406_4_293()
 {
    if (jj_done) return true;
    if (jj_scan_token(AMPERSAND_T)) return true;
    return false;
  }

 inline bool jj_3_3()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_adding_operator_404_3_136()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_adding_operator_404_3_291()) {
    jj_scanpos = xsp;
    if (jj_3R_adding_operator_405_5_292()) {
    jj_scanpos = xsp;
    if (jj_3R_adding_operator_406_4_293()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_adding_operator_404_3_291()
 {
    if (jj_done) return true;
    if (jj_scan_token(PLUS_T)) return true;
    return false;
  }

 inline bool jj_3_2()
 {
    if (jj_done) return true;
    if (jj_3R_actual_designator_371_1_62()) return true;
    return false;
  }

 inline bool jj_3R_actual_part_396_2_530()
 {
    if (jj_done) return true;
    if (jj_3R_name_1783_2_63()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_actual_designator_371_1_62()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_part_394_4_529()
 {
    if (jj_done) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_part_391_2_528()
 {
    if (jj_done) return true;
    if (jj_3R_actual_designator_371_1_62()) return true;
    return false;
  }

 inline bool jj_3R_actual_part_391_2_434()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_actual_part_391_2_528()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_part_394_4_529()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_part_396_2_530()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_package_declaration_1923_2_384()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_actual_parameter_part_385_4_246()
 {
    if (jj_done) return true;
    if (jj_3R_association_list_515_1_172()) return true;
    return false;
  }

 inline bool jj_3_1()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_376_3_158()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_373_1_157()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_371_1_156()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_371_1_62()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_actual_designator_371_1_156()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_designator_373_1_157()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_designator_376_3_158()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_69()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1350_2_68()) return true;
    return false;
  }

 inline bool jj_3R_access_type_definition_364_3_672()
 {
    if (jj_done) return true;
    if (jj_scan_token(ACCESS_T)) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    return false;
  }

 inline bool jj_3R_abstract_literal_358_4_245()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASED_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_options_1876_21_342()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_939_1_435()) return true;
    return false;
  }

 inline bool jj_3R_abstract_literal_357_4_244()
 {
    if (jj_done) return true;
    if (jj_scan_token(INTEGER)) return true;
    return false;
  }

 inline bool jj_3R_abstract_literal_356_4_112()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_abstract_literal_356_4_243()) {
    jj_scanpos = xsp;
    if (jj_3R_abstract_literal_357_4_244()) {
    jj_scanpos = xsp;
    if (jj_3R_abstract_literal_358_4_245()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_abstract_literal_356_4_243()
 {
    if (jj_done) return true;
    if (jj_scan_token(DECIMAL_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_options_1876_3_175()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(50)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_options_1876_21_342()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_next_statement_1835_43_280()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_operator_symbol_1871_1_332()
 {
    if (jj_done) return true;
    if (jj_scan_token(STRINGLITERAL)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1866_2_556()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1865_2_555()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1864_2_554()
 {
    if (jj_done) return true;
    if (jj_scan_token(SHARED_T)) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3_68()
 {
    if (jj_done) return true;
    if (jj_3R_physical_literal_1988_2_111()) return true;
    return false;
  }

 inline bool jj_3R_object_class_1863_2_553()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1862_2_552()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1861_1_551()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_1861_1_465()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_object_class_1861_1_551()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1862_2_552()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1863_2_553()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1864_2_554()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1865_2_555()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_1866_2_556()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_name_ext_1814_47_386()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_numeric_literal_1856_4_226()
 {
    if (jj_done) return true;
    if (jj_3R_abstract_literal_356_4_112()) return true;
    return false;
  }

 inline bool jj_3R_numeric_literal_1854_2_104()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_numeric_literal_1854_2_225()) {
    jj_scanpos = xsp;
    if (jj_3R_numeric_literal_1856_4_226()) return true;
    }
    return false;
  }

 inline bool jj_3R_numeric_literal_1854_2_225()
 {
    if (jj_done) return true;
    if (jj_3R_physical_literal_1988_2_111()) return true;
    return false;
  }

 inline bool jj_3R_null_statement_1848_3_472()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_null_statement_1848_1_395()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_statement_1848_3_472()) jj_scanpos = xsp;
    if (jj_scan_token(NULL_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_null_1813_32_110()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_next_statement_1836_3_281()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    return false;
  }

 inline bool jj_3R_next_statement_1835_3_279()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_67()
 {
    if (jj_done) return true;
    if (jj_scan_token(LBRACKET_T)) return true;
    return false;
  }

 inline bool jj_3R_next_statement_1835_1_131()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_next_statement_1835_3_279()) jj_scanpos = xsp;
    if (jj_scan_token(NEXT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_next_statement_1835_43_280()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_next_statement_1836_3_281()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_test_att_name_1824_4_241()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_66()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_null_1813_32_110()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_test_att_name_1822_5_239()
 {
    if (jj_done) return true;
    if (jj_3R_signature_2397_1_255()) return true;
    return false;
  }

 inline bool jj_3R_test_att_name_1822_3_109()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_test_att_name_1822_5_239()) jj_scanpos = xsp;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_3R_attribute_designator_529_3_240()) return true;
    xsp = jj_scanpos;
    if (jj_3R_test_att_name_1824_4_241()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_65()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_discrete_range_979_3_72()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_61()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext_1800_3_107()) return true;
    return false;
  }

 inline bool jj_3_64()
 {
    if (jj_done) return true;
    if (jj_3R_test_att_name_1822_3_109()) return true;
    return false;
  }

 inline bool jj_3_59()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1794_4_106()) return true;
    return false;
  }

 inline bool jj_3_58()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1794_4_106()) return true;
    return false;
  }

 inline bool jj_3_60()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1794_4_106()) return true;
    return false;
  }

 inline bool jj_3_63()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_suffix_2577_1_108()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1813_3_234()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_name_ext_1814_47_386()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_62()
 {
    if (jj_done) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_name_ext1_1794_18_229()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext_1800_3_107()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1810_2_233()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_discrete_range_979_3_72()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1807_3_232()
 {
    if (jj_done) return true;
    if (jj_3R_test_att_name_1822_3_109()) return true;
    return false;
  }

 inline bool jj_3R_name_1784_25_333()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1794_4_106()) return true;
    return false;
  }

 inline bool jj_3R_name_1785_24_335()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1794_4_106()) return true;
    return false;
  }

 inline bool jj_3R_name_1786_22_336()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_1794_4_106()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1804_2_231()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_suffix_2577_1_108()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_1800_3_107()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_ext_1801_2_230()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_1804_2_231()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_1807_3_232()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_1810_2_233()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_1813_3_234()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_name_ext_1801_2_230()
 {
    if (jj_done) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_name_ext1_1794_4_106()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext_1800_3_107()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_name_ext1_1794_18_229()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_name_1786_6_161()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_1786_22_336()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_1785_5_160()
 {
    if (jj_done) return true;
    if (jj_3R_external_name_2909_2_334()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_1785_24_335()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_1784_4_159()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_1871_1_332()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_1784_25_333()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_1783_2_63()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_1784_4_159()) {
    jj_scanpos = xsp;
    if (jj_3R_name_1785_5_160()) {
    jj_scanpos = xsp;
    if (jj_3R_name_1786_6_161()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_multiplying_operation_1777_3_301()
 {
    if (jj_done) return true;
    if (jj_scan_token(REM_T)) return true;
    return false;
  }

 inline bool jj_3R_multiplying_operation_1776_3_300()
 {
    if (jj_done) return true;
    if (jj_scan_token(MOD_T)) return true;
    return false;
  }

 inline bool jj_3R_multiplying_operation_1775_3_299()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLASH_T)) return true;
    return false;
  }

 inline bool jj_3R_multiplying_operation_1774_1_143()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_multiplying_operation_1774_1_298()) {
    jj_scanpos = xsp;
    if (jj_3R_multiplying_operation_1775_3_299()) {
    jj_scanpos = xsp;
    if (jj_3R_multiplying_operation_1776_3_300()) {
    jj_scanpos = xsp;
    if (jj_3R_multiplying_operation_1777_3_301()) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_multiplying_operation_1774_1_298()
 {
    if (jj_done) return true;
    if (jj_scan_token(MULT_T)) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_1744_28_646()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_mode_1769_3_461()
 {
    if (jj_done) return true;
    if (jj_scan_token(LINKAGE_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_1768_3_460()
 {
    if (jj_done) return true;
    if (jj_scan_token(BUFFER_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_1767_3_459()
 {
    if (jj_done) return true;
    if (jj_scan_token(INOUT_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_1766_3_458()
 {
    if (jj_done) return true;
    if (jj_scan_token(OUT_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_1765_1_372()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_mode_1765_1_457()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_1766_3_458()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_1767_3_459()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_1768_3_460()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_1769_3_461()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_mode_1765_1_457()
 {
    if (jj_done) return true;
    if (jj_scan_token(IN_T)) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_1737_7_277()
 {
    if (jj_done) return true;
    if (jj_3R_iteration_scheme_1669_1_399()) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_1736_3_276()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_1736_1_130()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_loop_statement_1736_3_276()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_loop_statement_1737_7_277()) jj_scanpos = xsp;
    if (jj_scan_token(LOOP_T)) return true;
    if (jj_3R_sequence_of_statement_2276_2_278()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(LOOP_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_loop_statement_1744_28_646()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_57()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_literal_1157_2_105()) return true;
    return false;
  }

 inline bool jj_3_56()
 {
    if (jj_done) return true;
    if (jj_3R_numeric_literal_1854_2_104()) return true;
    return false;
  }

 inline bool jj_3R_literal_1726_3_253()
 {
    if (jj_done) return true;
    if (jj_scan_token(NULL_T)) return true;
    return false;
  }

 inline bool jj_3_55()
 {
    if (jj_done) return true;
    if (jj_3R_bit_string_literal_589_3_103()) return true;
    return false;
  }

 inline bool jj_3R_literal_1725_3_252()
 {
    if (jj_done) return true;
    if (jj_3R_string_literal_2421_1_389()) return true;
    return false;
  }

 inline bool jj_3R_literal_1723_1_251()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_literal_1157_2_105()) return true;
    return false;
  }

 inline bool jj_3R_literal_1719_2_250()
 {
    if (jj_done) return true;
    if (jj_3R_numeric_literal_1854_2_104()) return true;
    return false;
  }

 inline bool jj_3R_literal_1716_2_115()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_literal_1716_2_249()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_1719_2_250()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_1723_1_251()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_1725_3_252()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_1726_3_253()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_literal_1716_2_249()
 {
    if (jj_done) return true;
    if (jj_3R_bit_string_literal_589_3_103()) return true;
    return false;
  }

 inline bool jj_3_54()
 {
    if (jj_done) return true;
    if (jj_3R_primary_unit_2047_1_102()) return true;
    return false;
  }

 inline bool jj_3R_library_clause_1692_2_357()
 {
    if (jj_done) return true;
    if (jj_scan_token(LIBRARY_T)) return true;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_label_1687_2_148()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1639_49_208()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_iteration_scheme_1676_3_474()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_parameter_specification_1983_1_559()) return true;
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1638_37_207()
 {
    if (jj_done) return true;
    if (jj_3R_mode_1765_1_372()) return true;
    return false;
  }

 inline bool jj_3R_iteration_scheme_1669_1_399()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_iteration_scheme_1669_1_473()) {
    jj_scanpos = xsp;
    if (jj_3R_iteration_scheme_1676_3_474()) return true;
    }
    return false;
  }

 inline bool jj_3R_iteration_scheme_1669_1_473()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHILE_T)) return true;
    if (jj_3R_condition_824_3_83()) return true;
    return false;
  }

 inline bool jj_3R_ifunc_1547_98_550()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1637_3_205()
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

 inline bool jj_3R_interface_variable_declaration_1637_1_94()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_variable_declaration_1637_3_205()) jj_scanpos = xsp;
    if (jj_3R_identifier_list_1369_4_206()) return true;
    if (jj_scan_token(COLON_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_interface_variable_declaration_1638_37_207()) jj_scanpos = xsp;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(27)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_variable_declaration_1639_49_208()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_interface_type_indication_1629_6_216()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_2571_3_89()) return true;
    return false;
  }

 inline bool jj_3R_ifunc_1547_87_464()
 {
    if (jj_done) return true;
    if (jj_scan_token(IS_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_ifunc_1547_98_550()) {
    jj_scanpos = xsp;
    if (jj_scan_token(145)) return true;
    }
    return false;
  }

 inline bool jj_3R_interface_type_declaration_1624_3_98()
 {
    if (jj_done) return true;
    if (jj_3R_interface_incomplete_type_declaration_1604_3_215()) return true;
    return false;
  }

 inline bool jj_3R_interface_signal_declaration_1527_87_218()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_interface_incomplete_type_declaration_1604_3_215()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    return false;
  }

 inline bool jj_3_51()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_param_1574_5_549()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_interface_list_1507_3_338()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_constant_declaration_1488_89_217()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_param_1573_3_462()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(77)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_param_1574_5_549()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_135()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    return false;
  }

 inline bool jj_3R_sel_var_list_3036_58_418()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_718_3_90()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(134)) {
    jj_scanpos = xsp;
    if (jj_scan_token(137)) return true;
    }
    return false;
  }

 inline bool jj_3R_sel_wave_list_3042_43_562()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_sel_wave_list_3042_4_479()) return true;
    return false;
  }

 inline bool jj_3R_ifunc_1547_3_463()
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

 inline bool jj_3R_ifunc_1547_2_375()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_ifunc_1547_3_463()) jj_scanpos = xsp;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    if (jj_3R_param_1573_3_462()) return true;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_name_1783_2_63()) return true;
    xsp = jj_scanpos;
    if (jj_3R_ifunc_1547_87_464()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_selected_force_assignment_3031_52_327()
 {
    if (jj_done) return true;
    if (jj_3R_inout_stat_3047_3_416()) return true;
    return false;
  }

 inline bool jj_3R_else_stat_3053_28_579()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_expression_1183_1_61()) return true;
    return false;
  }

 inline bool jj_3R_iproc_1538_3_374()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    if (jj_3R_identifier_1363_3_74()) return true;
    if (jj_3R_param_1573_3_462()) return true;
    return false;
  }

 inline bool jj_3R_interface_subprogram_declaration_1533_4_213()
 {
    if (jj_done) return true;
    if (jj_3R_ifunc_1547_2_375()) return true;
    return false;
  }

 inline bool jj_3R_interface_subprogram_declaration_1532_4_212()
 {
    if (jj_done) return true;
    if (jj_3R_iproc_1538_3_374()) return true;
    return false;
  }

 inline bool jj_3R_interface_subprogram_declaration_1532_4_96()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_subprogram_declaration_1532_4_212()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_subprogram_declaration_1533_4_213()) return true;
    }
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
  JJCalls       jj_2_rtns[136];
  bool          jj_rescan;
  int           jj_gc;
  Token        *jj_scanpos, *jj_lastpos;
  int           jj_la;
  /** Whether we are looking ahead. */
  bool          jj_lookingAhead;
  bool          jj_semLA;
  int           jj_gen;
  int           jj_la1[324];
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
  bool interf_sec;
  bool generic_sec;
};

VHDLOutlineParser *m_outlineParser;
SharedState *m_sharedState;

void setOutlineParser(VHDLOutlineParser* p) { m_outlineParser=p; }
VHDLOutlineParser *outlineParser() const { return m_outlineParser; }
void setSharedState(SharedState *s) { m_sharedState=s; }
void clearError() { hasError = false; }

bool checkListTok()
{
    return  (getToken(1)->kind==SEMI_T) && (getToken(2)->kind==RPAREN_T);
}

bool checkNextTok(int kind)
{
    return  getToken(1)->kind==kind;
}

Token* _getPrevTok()
{
    return getToken(0);
}

private:
  bool jj_done;
};
}
}
#endif
