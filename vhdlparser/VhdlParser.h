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
QCString absolute_pathname();
QCString abstract_literal();
QCString access_type_definition();
QCString access_incomplete_type_definition();
QCString actual_designator();
QCString actual_parameter_part();
QCString actual_part();
QCString adding_operator();
QCString aggregate();
QCString alias_declaration();
QCString alias_designator();
void allocator();
void architecture_body();
void anonymous_type_indication();
void architecture_declarative_part();
void architecture_statement_part();
void array_constraint();
void array_element_constraint();
void array_element_resolution();
void array_incomplete_type_definition();
void array_index_incomplete_type();
void array_index_incomplete_type_list();
QCString array_mode_view_indication();
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
void case_scheme();
void when_stats();
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
void concurrent_conditional_signal_assignment();
void concurrent_simple_signal_assignment();
void concurrent_selected_signal_assignment();
void concurrent_signal_association_statement();
void concurrent_simple_signal_association();
void concurrent_statement();
QCString condition();
QCString condition_operator();
QCString condition_clause();
void conditional_signal_assignment_wave();
void conditional_waveform_assignment();
void else_wave_list();
void conditional_force_assignment();
void else_stat();
QCString conditional_expression();
QCString conditional_or_unaffected_expression();
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
QCString context_ref();
void context_declaration();
QCString decimal_literal();
QCString delay_mechanism();
void design_file();
void design_unit();
QCString designator();
QCString direction();
QCString discrete_incomplete_type_definition();
void disconnection_specification();
void guarded_signal_specificatio();
QCString discrete_range();
QCString element_array_mode_view_indication();
QCString element_association();
QCString element_constraint();
QCString element_declaration();
QCString element_resolution();
QCString element_mode_indication();
QCString element_mode_view_indication();
QCString element_record_mode_view_indication();
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
QCString expression_or_unaffected();
QCString external_pathname();
QCString external_name();
QCString sig_stat();
QCString logop();
QCString extended_identifier();
QCString factor();
QCString file_declaration();
QCString file_logical_name();
QCString file_open_information();
QCString file_type_definition();
QCString file_incomplete_type_definition();
QCString floating_incomplete_type_definition();
QCString floating_type_definition();
QCString formal_designator();
QCString formal_parameter_list();
QCString formal_part();
QCString full_type_declaration();
QCString function_call();
void geninter();
void gen_assoc_list();
void gen_interface_list();
void generate_statement_body();
void generate_statement_body1();
void generate_scheme_1();
void generate_scheme_2();
void generate_scheme_3();
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
QCString incomplete_subtype_indication();
QCString incomplete_type_definition();
QCString incomplete_type_mark();
QCString integer_incomplete_type_definition();
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
QCString interface_package_declaration();
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
QCString mode_indication();
QCString mode_view_declaration();
QCString mode_view_element_definition();
QCString mode_view_indication();
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
void package_declarative_item();
void package_declarative_part();
void package_instantiation_declaration();
QCString parameter_specification();
QCString pathname_element();
QCString pathname_element_list();
QCString package_path_name();
QCString physical_literal();
QCString physical_type_definition();
void physical_incomplete_type_definition();
void port_clause();
QCString port_list();
void port_map_aspect();
QCString primary();
void primary_unit();
QCString private_variable_declaration();
QCString private_incomplete_type_definition();
QCString parameter_map_aspect();
QCString procedure_call();
QCString procedure_call_statement();
QCString procedure_specifiction();
QCString process_declarative_item();
QCString process_declarative_part();
void process_statement();
void process_statement_part();
QCString protected_type_body();
void protected_type_body_declarative_item();
void protected_type_body_declarative_part();
QCString protected_type_declaration();
void protected_type_header();
void protected_type_declarative_item();
void protected_type_declarative_part();
QCString qualified_expression();
QCString range();
QCString range_constraint();
void record_type_definition();
void record_constraint();
void record_element_constraint();
void record_element_list();
void record_element_resolution();
void record_resolution();
void record_mode_view_indication();
QCString relation();
QCString relative_pathname();
QCString neg_list();
QCString relation_operator();
QCString report_statement();
QCString resolution_indication();
QCString return_statement();
QCString plain_return_statement();
QCString value_return_statement();
QCString scalar_type_definition();
void scalar_incomplete_type_definition();
void secondary_unit();
QCString secondary_unit_declaration();
QCString selected_name();
void selected_signal_assignment();
void selected_waveforms();
QCString sensitivity_clause();
void selected_signal_assignment_wave();
void selected_variable_assignment();
void select_name();
void selected_waveform_assignment();
void selected_force_assignment();
void sel_var_list();
void sel_wave_list();
QCString sensitivity_list();
QCString sequence_of_statements();
QCString sequential_statement();
QCString sequential_bock_statement();
QCString sequential_block_declarative_part();
QCString sequential_block_statement_part();
QCString sequential_statement_body();
QCString sequential_statement_declarative_part();
QCString shift_expression();
QCString shift_operator();
QCString sign();
QCString signal_assignment_statement();
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
QCString subtype_indication();
QCString suffix();
QCString target();
QCString simpleTerm();
QCString timeout_clause();
QCString type_conversion();
QCString type_declaration();
QCString type_definition();
void protected_type_instantiation_definition();
QCString type_mark();
QCString unconstraint_array_definition();
QCString use_clause();
QCString variable_assignment_statement();
QCString variable_declaration();
QCString wait_statement();
QCString waveform();
QCString waveform_element();
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

 inline bool jj_2_136(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_136() || jj_done);
 { jj_save(135, xla); }
  }

 inline bool jj_2_137(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_137() || jj_done);
 { jj_save(136, xla); }
  }

 inline bool jj_2_138(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_138() || jj_done);
 { jj_save(137, xla); }
  }

 inline bool jj_2_139(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_139() || jj_done);
 { jj_save(138, xla); }
  }

 inline bool jj_2_140(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_140() || jj_done);
 { jj_save(139, xla); }
  }

 inline bool jj_2_141(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_141() || jj_done);
 { jj_save(140, xla); }
  }

 inline bool jj_2_142(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_142() || jj_done);
 { jj_save(141, xla); }
  }

 inline bool jj_2_143(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_143() || jj_done);
 { jj_save(142, xla); }
  }

 inline bool jj_2_144(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_144() || jj_done);
 { jj_save(143, xla); }
  }

 inline bool jj_2_145(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_145() || jj_done);
 { jj_save(144, xla); }
  }

 inline bool jj_2_146(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_146() || jj_done);
 { jj_save(145, xla); }
  }

 inline bool jj_2_147(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_147() || jj_done);
 { jj_save(146, xla); }
  }

 inline bool jj_2_148(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_148() || jj_done);
 { jj_save(147, xla); }
  }

 inline bool jj_2_149(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_149() || jj_done);
 { jj_save(148, xla); }
  }

 inline bool jj_2_150(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_150() || jj_done);
 { jj_save(149, xla); }
  }

 inline bool jj_2_151(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_151() || jj_done);
 { jj_save(150, xla); }
  }

 inline bool jj_2_152(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_152() || jj_done);
 { jj_save(151, xla); }
  }

 inline bool jj_2_153(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_153() || jj_done);
 { jj_save(152, xla); }
  }

 inline bool jj_2_154(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_154() || jj_done);
 { jj_save(153, xla); }
  }

 inline bool jj_2_155(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_155() || jj_done);
 { jj_save(154, xla); }
  }

 inline bool jj_2_156(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_156() || jj_done);
 { jj_save(155, xla); }
  }

 inline bool jj_2_157(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_157() || jj_done);
 { jj_save(156, xla); }
  }

 inline bool jj_2_158(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_158() || jj_done);
 { jj_save(157, xla); }
  }

 inline bool jj_2_159(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_159() || jj_done);
 { jj_save(158, xla); }
  }

 inline bool jj_2_160(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_160() || jj_done);
 { jj_save(159, xla); }
  }

 inline bool jj_2_161(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_161() || jj_done);
 { jj_save(160, xla); }
  }

 inline bool jj_2_162(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_162() || jj_done);
 { jj_save(161, xla); }
  }

 inline bool jj_2_163(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_163() || jj_done);
 { jj_save(162, xla); }
  }

 inline bool jj_2_164(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_164() || jj_done);
 { jj_save(163, xla); }
  }

 inline bool jj_2_165(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_165() || jj_done);
 { jj_save(164, xla); }
  }

 inline bool jj_2_166(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_166() || jj_done);
 { jj_save(165, xla); }
  }

 inline bool jj_2_167(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_167() || jj_done);
 { jj_save(166, xla); }
  }

 inline bool jj_2_168(int xla)
 {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    jj_done = false;
    return (!jj_3_168() || jj_done);
 { jj_save(167, xla); }
  }

 inline bool jj_3R_simple_expression_3063_3_199()
 {
    if (jj_done) return true;
    if (jj_3R_sign_2996_2_378()) return true;
    return false;
  }

 inline bool jj_3R_simple_expression_3063_1_85()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_simple_expression_3063_3_199()) jj_scanpos = xsp;
    if (jj_3R_simpleTerm_3246_2_169()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_simple_expression_3063_41_200()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_signature_3056_4_568()
 {
    if (jj_done) return true;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_signature_3055_4_567()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_signature_3055_15_674()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_signature_3054_1_472()
 {
    if (jj_done) return true;
    if (jj_scan_token(LBRACKET_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signature_3055_4_567()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_signature_3056_4_568()) jj_scanpos = xsp;
    if (jj_scan_token(RBRACKET_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_list_3039_12_783()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_file_declaration_1443_67_522()
 {
    if (jj_done) return true;
    if (jj_3R_file_open_information_1459_2_639()) return true;
    return false;
  }

 inline bool jj_3R_signal_list_3041_3_723()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_list_3040_3_722()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_formal_part_1496_11_191()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_formal_designator_1484_5_374()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_list_3039_2_721()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_signal_list_3039_12_783()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_signal_list_3039_2_644()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_list_3039_2_721()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_list_3040_3_722()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_list_3041_3_723()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_signal_kind_3034_5_695()
 {
    if (jj_done) return true;
    if (jj_scan_token(BUS_T)) return true;
    return false;
  }

 inline bool jj_3R_full_type_declaration_1501_3_176()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_type_definition_3271_1_691()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_kind_3033_3_694()
 {
    if (jj_done) return true;
    if (jj_scan_token(REGISTER_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_kind_3033_3_638()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_kind_3033_3_694()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_kind_3034_5_695()) return true;
    }
    return false;
  }

 inline bool jj_3R_file_open_information_1459_39_697()
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

 inline bool jj_3R_formal_part_1496_1_76()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_formal_part_1496_11_191()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_formal_parameter_list_1491_2_739()
 {
    if (jj_done) return true;
    if (jj_3R_interface_list_1836_3_377()) return true;
    return false;
  }

 inline bool jj_3R_formal_designator_1485_6_485()
 {
    if (jj_done) return true;
    if (jj_scan_token(INTEGER)) return true;
    return false;
  }

 inline bool jj_3R_formal_designator_1484_5_484()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_formal_designator_1484_5_374()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_formal_designator_1484_5_484()) {
    jj_scanpos = xsp;
    if (jj_3R_formal_designator_1485_6_485()) return true;
    }
    return false;
  }

 inline bool jj_3R_signal_declaration_3022_1_419()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_declaration_3022_68_518()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_declaration_3022_89_519()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_153()
 {
    if (jj_done) return true;
    if (jj_3R_selected_signal_assignment_wave_2849_3_167()) return true;
    return false;
  }

 inline bool jj_3_152()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_wave_926_3_166()) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_3011_3_665()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_1092_1_381()) return true;
    return false;
  }

 inline bool jj_3R_floating_incomplete_type_definition_1474_4_114()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3_151()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_3009_3_553()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_151()) jj_scanpos = xsp;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_signal_assignment_statement_3011_3_665()) jj_scanpos = xsp;
    if (jj_3R_waveform_3377_1_382()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_3006_3_552()
 {
    if (jj_done) return true;
    if (jj_3R_selected_signal_assignment_wave_2849_3_167()) return true;
    return false;
  }

 inline bool jj_3R_shift_expression_2982_26_474()
 {
    if (jj_done) return true;
    if (jj_3R_shift_operator_2986_3_577()) return true;
    if (jj_3R_simple_expression_3063_1_85()) return true;
    return false;
  }

 inline bool jj_3R_file_incomplete_type_definition_1469_2_790()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_type_mark_3291_3_195()) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_3003_3_551()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_signal_assignment_wave_926_3_166()) return true;
    return false;
  }

 inline bool jj_3R_signal_assignment_statement_3003_3_460()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_signal_assignment_statement_3003_3_551()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_assignment_statement_3006_3_552()) {
    jj_scanpos = xsp;
    if (jj_3R_signal_assignment_statement_3009_3_553()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_file_type_definition_1464_2_801()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_type_mark_3291_3_195()) return true;
    return false;
  }

 inline bool jj_3R_sign_2997_4_489()
 {
    if (jj_done) return true;
    if (jj_scan_token(MINUS_T)) return true;
    return false;
  }

 inline bool jj_3R_file_open_information_1459_4_696()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_sign_2996_2_488()
 {
    if (jj_done) return true;
    if (jj_scan_token(PLUS_T)) return true;
    return false;
  }

 inline bool jj_3R_sign_2996_2_378()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sign_2996_2_488()) {
    jj_scanpos = xsp;
    if (jj_3R_sign_2997_4_489()) return true;
    }
    return false;
  }

 inline bool jj_3R_shift_operator_2991_5_680()
 {
    if (jj_done) return true;
    if (jj_scan_token(ROR_T)) return true;
    return false;
  }

 inline bool jj_3R_file_open_information_1459_2_639()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_file_open_information_1459_4_696()) jj_scanpos = xsp;
    if (jj_scan_token(IS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_file_open_information_1459_39_697()) jj_scanpos = xsp;
    if (jj_3R_file_logical_name_1454_2_698()) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2990_5_679()
 {
    if (jj_done) return true;
    if (jj_scan_token(ROL_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2989_5_678()
 {
    if (jj_done) return true;
    if (jj_scan_token(SRA_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2988_5_677()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLA_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2987_5_676()
 {
    if (jj_done) return true;
    if (jj_scan_token(SRL_T)) return true;
    return false;
  }

 inline bool jj_3R_file_logical_name_1454_2_698()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2986_3_675()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLL_T)) return true;
    return false;
  }

 inline bool jj_3R_shift_operator_2986_3_577()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_shift_operator_2986_3_675()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2987_5_676()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2988_5_677()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2989_5_678()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2990_5_679()) {
    jj_scanpos = xsp;
    if (jj_3R_shift_operator_2991_5_680()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_shift_expression_2982_2_365()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_3063_1_85()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_shift_expression_2982_26_474()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_factor_1433_14_566()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOUBLEMULT_T)) return true;
    if (jj_3R_primary_2429_1_473()) return true;
    return false;
  }

 inline bool jj_3R_file_declaration_1443_2_421()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_file_declaration_1443_67_522()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_declarative_part_2974_3_854()
 {
    if (jj_done) return true;
    if (jj_3R_process_declarative_item_2507_1_513()) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_declarative_part_2974_2_843()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sequential_statement_declarative_part_2974_3_854()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_factor_1437_3_364()
 {
    if (jj_done) return true;
    if (jj_3R_logop_1418_3_367()) return true;
    if (jj_3R_primary_2429_1_473()) return true;
    return false;
  }

 inline bool jj_3R_factor_1436_3_363()
 {
    if (jj_done) return true;
    if (jj_scan_token(QQ_T)) return true;
    if (jj_3R_primary_2429_1_473()) return true;
    return false;
  }

 inline bool jj_3R_factor_1435_3_362()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOT_T)) return true;
    if (jj_3R_primary_2429_1_473()) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_body_2969_3_811()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_declarative_part_2974_2_843()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    return false;
  }

 inline bool jj_3R_factor_1434_3_361()
 {
    if (jj_done) return true;
    if (jj_scan_token(ABS_T)) return true;
    if (jj_3R_primary_2429_1_473()) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_body_2969_2_797()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sequential_statement_body_2969_3_811()) jj_scanpos = xsp;
    if (jj_3R_sequence_of_statements_2902_2_334()) return true;
    return false;
  }

 inline bool jj_3R_factor_1433_1_175()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_factor_1433_1_360()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1434_3_361()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1435_3_362()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1436_3_363()) {
    jj_scanpos = xsp;
    if (jj_3R_factor_1437_3_364()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_factor_1433_1_360()
 {
    if (jj_done) return true;
    if (jj_3R_primary_2429_1_473()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_factor_1433_14_566()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_sequential_bock_statement_2952_16_163()
 {
    if (jj_done) return true;
    if (jj_3R_label_2022_2_84()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_sequential_block_statement_part_2964_3_344()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2907_5_148()) return true;
    return false;
  }

 inline bool jj_3R_sequential_block_statement_part_2964_2_164()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sequential_block_statement_part_2964_3_344()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_logop_1423_4_481()
 {
    if (jj_done) return true;
    if (jj_scan_token(OR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1422_4_480()
 {
    if (jj_done) return true;
    if (jj_scan_token(XOR_T)) return true;
    return false;
  }

 inline bool jj_3R_sequential_block_declarative_part_2959_2_469()
 {
    if (jj_done) return true;
    if (jj_3R_process_declarative_part_2532_2_236()) return true;
    return false;
  }

 inline bool jj_3R_logop_1421_4_479()
 {
    if (jj_done) return true;
    if (jj_scan_token(XNOR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1420_4_478()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOR_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1419_4_477()
 {
    if (jj_done) return true;
    if (jj_scan_token(NAND_T)) return true;
    return false;
  }

 inline bool jj_3R_sequential_bock_statement_2954_4_468()
 {
    if (jj_done) return true;
    if (jj_3R_label_2022_2_84()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1418_3_476()
 {
    if (jj_done) return true;
    if (jj_scan_token(AND_T)) return true;
    return false;
  }

 inline bool jj_3R_logop_1418_3_367()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_logop_1418_3_476()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1419_4_477()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1420_4_478()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1421_4_479()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1422_4_480()) {
    jj_scanpos = xsp;
    if (jj_3R_logop_1423_4_481()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_sequential_bock_statement_2954_2_343()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sequential_bock_statement_2954_4_468()) jj_scanpos = xsp;
    if (jj_scan_token(BLOCK_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    if (jj_3R_sequential_block_declarative_part_2959_2_469()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_sequential_block_statement_part_2964_2_164()) return true;
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(24)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_sequential_bock_statement_2954_159_795()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_sequential_bock_statement_2952_1_162()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_150()) {
    jj_scanpos = xsp;
    if (jj_3R_sequential_bock_statement_2954_2_343()) return true;
    }
    return false;
  }

 inline bool jj_3_150()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sequential_bock_statement_2952_16_163()) jj_scanpos = xsp;
    if (jj_scan_token(BLOCK_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_sequential_block_statement_part_2964_2_164()) return true;
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(24)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_sequential_bock_statement_2952_124_165()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_1412_5_592()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_1411_5_591()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3_135()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_2946_5_321()
 {
    if (jj_done) return true;
    if (jj_3R_null_statement_2209_1_462()) return true;
    return false;
  }

 inline bool jj_3R_expression_1380_35_180()
 {
    if (jj_done) return true;
    if (jj_3R_logop_1418_3_367()) return true;
    if (jj_3R_relation_2723_3_179()) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_1410_3_590()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_sig_stat_1410_3_482()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sig_stat_1410_3_590()) {
    jj_scanpos = xsp;
    if (jj_3R_sig_stat_1411_5_591()) {
    jj_scanpos = xsp;
    if (jj_3R_sig_stat_1412_5_592()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_149()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_bock_statement_2952_1_162()) return true;
    return false;
  }

 inline bool jj_3_148()
 {
    if (jj_done) return true;
    if (jj_3R_return_statement_2776_5_161()) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1366_43_339()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3_147()
 {
    if (jj_done) return true;
    if (jj_3R_exit_statement_1366_1_160()) return true;
    return false;
  }

 inline bool jj_3R_sel_var_list_2883_58_459()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_775_3_106()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(136)) {
    jj_scanpos = xsp;
    if (jj_scan_token(139)) return true;
    }
    return false;
  }

 inline bool jj_3_146()
 {
    if (jj_done) return true;
    if (jj_3R_next_statement_2196_1_159()) return true;
    return false;
  }

 inline bool jj_3R_null_2919_19_154()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_external_name_1400_2_371()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLSL_T)) return true;
    if (jj_3R_sig_stat_1410_3_482()) return true;
    if (jj_3R_external_pathname_1393_4_483()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    if (jj_scan_token(RSRS_T)) return true;
    return false;
  }

 inline bool jj_3_145()
 {
    if (jj_done) return true;
    if (jj_3R_loop_statement_2072_1_158()) return true;
    return false;
  }

 inline bool jj_3_141()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_2919_19_154()) jj_scanpos = xsp;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    return false;
  }

 inline bool jj_3R_expression_1380_20_70()
 {
    if (jj_done) return true;
    if (jj_3R_relation_2723_3_179()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_expression_1380_35_180()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_external_pathname_1395_4_595()
 {
    if (jj_done) return true;
    if (jj_3R_package_path_name_2384_3_683()) return true;
    return false;
  }

 inline bool jj_3_144()
 {
    if (jj_done) return true;
    if (jj_3R_case_statement_712_1_157()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_1394_4_594()
 {
    if (jj_done) return true;
    if (jj_3R_relative_pathname_2728_3_682()) return true;
    return false;
  }

 inline bool jj_3R_enumeration_type_definition_1360_38_316()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_enumeration_literal_1354_2_126()) return true;
    return false;
  }

 inline bool jj_3R_sel_wave_list_2889_43_673()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_sel_wave_list_2889_4_565()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_1393_4_593()
 {
    if (jj_done) return true;
    if (jj_3R_absolute_pathname_329_2_681()) return true;
    return false;
  }

 inline bool jj_3R_external_pathname_1393_4_483()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_external_pathname_1393_4_593()) {
    jj_scanpos = xsp;
    if (jj_3R_external_pathname_1394_4_594()) {
    jj_scanpos = xsp;
    if (jj_3R_external_pathname_1395_4_595()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_143()
 {
    if (jj_done) return true;
    if (jj_3R_if_statement_1668_3_156()) return true;
    return false;
  }

 inline bool jj_3R_selected_force_assignment_2878_53_317()
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

 inline bool jj_3R_expression_or_unaffected_1388_6_745()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNAFFECTED_T)) return true;
    return false;
  }

 inline bool jj_3_142()
 {
    if (jj_done) return true;
    if (jj_3R_procedure_call_statement_2493_1_155()) return true;
    return false;
  }

 inline bool jj_3R_expression_or_unaffected_1387_4_744()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_expression_or_unaffected_1387_4_668()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_expression_or_unaffected_1387_4_744()) {
    jj_scanpos = xsp;
    if (jj_3R_expression_or_unaffected_1388_6_745()) return true;
    }
    return false;
  }

 inline bool jj_3R_null_2907_18_149()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_2919_5_320()
 {
    if (jj_done) return true;
    if (jj_3R_variable_assignment_statement_3329_1_461()) return true;
    return false;
  }

 inline bool jj_3_137()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_2907_18_149()) jj_scanpos = xsp;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    return false;
  }

 inline bool jj_3_140()
 {
    if (jj_done) return true;
    if (jj_3R_wait_statement_3368_1_153()) return true;
    return false;
  }

 inline bool jj_3_139()
 {
    if (jj_done) return true;
    if (jj_3R_report_statement_2756_1_152()) return true;
    return false;
  }

 inline bool jj_3_138()
 {
    if (jj_done) return true;
    if (jj_3R_assertion_statement_539_3_151()) return true;
    return false;
  }

 inline bool jj_3R_sequential_statement_2907_5_148()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sequential_statement_2907_5_319()) {
    jj_scanpos = xsp;
    if (jj_3_138()) {
    jj_scanpos = xsp;
    if (jj_3_139()) {
    jj_scanpos = xsp;
    if (jj_3_140()) {
    jj_scanpos = xsp;
    if (jj_3R_sequential_statement_2919_5_320()) {
    jj_scanpos = xsp;
    if (jj_3_142()) {
    jj_scanpos = xsp;
    if (jj_3_143()) {
    jj_scanpos = xsp;
    if (jj_3_144()) {
    jj_scanpos = xsp;
    if (jj_3_145()) {
    jj_scanpos = xsp;
    if (jj_3_146()) {
    jj_scanpos = xsp;
    if (jj_3_147()) {
    jj_scanpos = xsp;
    if (jj_3_148()) {
    jj_scanpos = xsp;
    if (jj_3_149()) {
    jj_scanpos = xsp;
    if (jj_3R_sequential_statement_2946_5_321()) return true;
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

 inline bool jj_3R_sequential_statement_2907_5_319()
 {
    if (jj_done) return true;
    if (jj_3R_signal_assignment_statement_3003_3_460()) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_list_2897_12_622()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3_136()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2907_5_148()) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1367_3_340()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    return false;
  }

 inline bool jj_3R_exit_statement_1366_3_338()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_sequence_of_statements_2902_2_334()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_136()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_exit_statement_1366_1_160()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_exit_statement_1366_3_338()) jj_scanpos = xsp;
    if (jj_scan_token(EXIT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_exit_statement_1366_43_339()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_exit_statement_1367_3_340()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_list_2897_2_512()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sensitivity_list_2897_12_622()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_sensitivity_list_2895_3_511()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_enumeration_type_definition_1360_3_146()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_enumeration_literal_1354_2_126()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_enumeration_type_definition_1360_38_316()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_sensitivity_list_2895_3_412()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_sensitivity_list_2895_3_511()) {
    jj_scanpos = xsp;
    if (jj_3R_sensitivity_list_2897_2_512()) return true;
    }
    return false;
  }

 inline bool jj_3R_enumeration_literal_1355_4_280()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_754_3_445()) return true;
    return false;
  }

 inline bool jj_3R_sel_wave_list_2889_4_565()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_element_3385_2_562()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_775_3_106()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sel_wave_list_2889_43_673()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_enumeration_literal_1354_2_126()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_enumeration_literal_1354_2_279()) {
    jj_scanpos = xsp;
    if (jj_3R_enumeration_literal_1355_4_280()) return true;
    }
    return false;
  }

 inline bool jj_3R_enumeration_literal_1354_2_279()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_selected_waveform_assignment_2872_17_564()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_1092_1_381()) return true;
    return false;
  }

 inline bool jj_3R_entity_tag_1349_3_824()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_754_3_445()) return true;
    return false;
  }

 inline bool jj_3R_sel_var_list_2883_3_318()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_775_3_106()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(136)) {
    jj_scanpos = xsp;
    if (jj_scan_token(139)) return true;
    }
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_sel_var_list_2883_58_459()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_entity_tag_1348_1_823()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_entity_tag_1348_1_805()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_tag_1348_1_823()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_tag_1349_3_824()) return true;
    }
    return false;
  }

 inline bool jj_3R_entity_name_list_1317_28_782()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_entity_designator_1306_1_781()) return true;
    return false;
  }

 inline bool jj_3_57()
 {
    if (jj_done) return true;
    if (jj_3R_process_statement_2537_1_112()) return true;
    return false;
  }

 inline bool jj_3R_selected_force_assignment_2877_3_147()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(158)) jj_scanpos = xsp;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_scan_token(FORCE_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_selected_force_assignment_2878_53_317()) jj_scanpos = xsp;
    if (jj_3R_sel_var_list_2883_3_318()) return true;
    return false;
  }

 inline bool jj_3_56()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_assertion_statement_831_1_111()) return true;
    return false;
  }

 inline bool jj_3_134()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_400_3_141()) return true;
    return false;
  }

 inline bool jj_3R_selected_waveform_assignment_2870_3_471()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(158)) jj_scanpos = xsp;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_selected_waveform_assignment_2872_17_564()) jj_scanpos = xsp;
    if (jj_3R_sel_wave_list_2889_4_565()) return true;
    return false;
  }

 inline bool jj_3R_select_name_2865_3_842()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_selected_waveforms_2835_32_385()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_waveform_3377_1_382()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_775_3_106()) return true;
    return false;
  }

 inline bool jj_3R_select_name_2863_3_841()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_400_3_141()) return true;
    return false;
  }

 inline bool jj_3R_select_name_2863_3_810()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_select_name_2863_3_841()) {
    jj_scanpos = xsp;
    if (jj_3R_select_name_2865_3_842()) return true;
    }
    return false;
  }

 inline bool jj_3R_entity_specification_1326_1_530()
 {
    if (jj_done) return true;
    if (jj_3R_entity_name_list_1317_1_642()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_entity_class_1219_1_643()) return true;
    return false;
  }

 inline bool jj_3_133()
 {
    if (jj_done) return true;
    if (jj_3R_selected_force_assignment_2877_3_147()) return true;
    return false;
  }

 inline bool jj_3R_entity_designator_1306_18_806()
 {
    if (jj_done) return true;
    if (jj_3R_signature_3054_1_472()) return true;
    return false;
  }

 inline bool jj_3R_selected_variable_assignment_2856_3_666()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(158)) jj_scanpos = xsp;
    if (jj_3R_select_name_2863_3_810()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_sel_var_list_2883_3_318()) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1319_3_703()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1318_3_702()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_wave_2851_3_348()
 {
    if (jj_done) return true;
    if (jj_3R_selected_waveform_assignment_2870_3_471()) return true;
    return false;
  }

 inline bool jj_3R_entity_name_list_1317_1_701()
 {
    if (jj_done) return true;
    if (jj_3R_entity_designator_1306_1_781()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_entity_name_list_1317_28_782()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_entity_name_list_1317_1_642()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_name_list_1317_1_701()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_name_list_1318_3_702()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_name_list_1319_3_703()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_wave_2849_3_167()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_selected_signal_assignment_wave_2849_3_347()) {
    jj_scanpos = xsp;
    if (jj_3R_selected_signal_assignment_wave_2851_3_348()) return true;
    }
    return false;
  }

 inline bool jj_3R_selected_signal_assignment_wave_2849_3_347()
 {
    if (jj_done) return true;
    if (jj_3R_selected_force_assignment_2877_3_147()) return true;
    return false;
  }

 inline bool jj_3R_entity_designator_1306_1_781()
 {
    if (jj_done) return true;
    if (jj_3R_entity_tag_1348_1_805()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_designator_1306_18_806()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_sensitivity_clause_2840_2_463()
 {
    if (jj_done) return true;
    if (jj_scan_token(ON_T)) return true;
    if (jj_3R_sensitivity_list_2895_3_412()) return true;
    return false;
  }

 inline bool jj_3R_selected_waveforms_2835_2_209()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_3377_1_382()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_775_3_106()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_selected_waveforms_2835_32_385()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_55()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3_53()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3R_selected_name_2823_2_508()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_suffix_3230_1_130()) return true;
    return false;
  }

 inline bool jj_3_54()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1644_2_80()) return true;
    return false;
  }

 inline bool jj_3R_plain_return_statement_2782_42_315()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2796_26_844()
 {
    if (jj_done) return true;
    if (jj_3R_physical_type_definition_2394_9_855()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_entry_list_1245_39_646()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_entity_class_entry_1240_2_645()) return true;
    return false;
  }

 inline bool jj_3_52()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3R_secondary_unit_declaration_2813_1_888()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(EQU_T)) return true;
    if (jj_3R_physical_literal_2389_2_133()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_51()
 {
    if (jj_done) return true;
    if (jj_3R_package_body_2242_1_77()) return true;
    return false;
  }

 inline bool jj_3_50()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3_132()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_type_definition_1360_3_146()) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2796_3_813()
 {
    if (jj_done) return true;
    if (jj_3R_range_constraint_2678_1_103()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_scalar_type_definition_2796_26_844()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_entity_class_entry_1240_21_724()
 {
    if (jj_done) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2794_1_812()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_type_definition_1360_3_146()) return true;
    return false;
  }

 inline bool jj_3R_scalar_type_definition_2794_1_798()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_scalar_type_definition_2794_1_812()) {
    jj_scanpos = xsp;
    if (jj_3R_scalar_type_definition_2796_3_813()) return true;
    }
    return false;
  }

 inline bool jj_3R_entity_declaration_1251_5_442()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3_131()
 {
    if (jj_done) return true;
    if (jj_3R_plain_return_statement_2782_1_145()) return true;
    return false;
  }

 inline bool jj_3R_value_return_statement_2787_3_559()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_value_return_statement_2787_1_467()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_value_return_statement_2787_3_559()) jj_scanpos = xsp;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_conditional_or_unaffected_expression_965_2_560()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_plain_return_statement_2782_3_314()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_return_statement_2777_7_342()
 {
    if (jj_done) return true;
    if (jj_3R_value_return_statement_2787_1_467()) return true;
    return false;
  }

 inline bool jj_3R_plain_return_statement_2782_1_145()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_plain_return_statement_2782_3_314()) jj_scanpos = xsp;
    if (jj_scan_token(RETURN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_plain_return_statement_2782_42_315()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_entry_list_1245_2_533()
 {
    if (jj_done) return true;
    if (jj_3R_entity_class_entry_1240_2_645()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_entity_class_entry_list_1245_39_646()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_return_statement_2776_5_161()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_return_statement_2776_5_341()) {
    jj_scanpos = xsp;
    if (jj_3R_return_statement_2777_7_342()) return true;
    }
    return false;
  }

 inline bool jj_3R_return_statement_2776_5_341()
 {
    if (jj_done) return true;
    if (jj_3R_plain_return_statement_2782_1_145()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_entry_1240_2_645()
 {
    if (jj_done) return true;
    if (jj_3R_entity_class_1219_1_643()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_class_entry_1240_21_724()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_resolution_indication_2770_5_409()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_aspect_1212_27_755()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1235_3_720()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    return false;
  }

 inline bool jj_3R_resolution_indication_2769_4_232()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_resolution_indication_2769_4_408()) {
    jj_scanpos = xsp;
    if (jj_3R_resolution_indication_2770_5_409()) return true;
    }
    return false;
  }

 inline bool jj_3R_resolution_indication_2769_4_408()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1234_3_719()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1233_3_718()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNITS_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1232_3_717()
 {
    if (jj_done) return true;
    if (jj_scan_token(LITERAL_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1231_3_716()
 {
    if (jj_done) return true;
    if (jj_scan_token(LABEL_T)) return true;
    return false;
  }

 inline bool jj_3R_report_statement_2758_11_326()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEVERITY_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1230_3_715()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1229_3_714()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1228_3_713()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1227_3_712()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1226_3_711()
 {
    if (jj_done) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1225_3_710()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1224_3_709()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1223_3_708()
 {
    if (jj_done) return true;
    if (jj_scan_token(FUNCTION_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1222_3_707()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1221_3_706()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    return false;
  }

 inline bool jj_3R_report_statement_2756_3_325()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1220_3_705()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARCHITECTURE_T)) return true;
    return false;
  }

 inline bool jj_3R_report_statement_2756_1_152()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_report_statement_2756_3_325()) jj_scanpos = xsp;
    if (jj_scan_token(REPORT_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    xsp = jj_scanpos;
    if (jj_3R_report_statement_2758_11_326()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_130()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_list_2378_3_69()) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1219_1_704()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_class_1219_1_643()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_class_1219_1_704()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1220_3_705()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1221_3_706()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1222_3_707()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1223_3_708()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1224_3_709()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1225_3_710()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1226_3_711()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1227_3_712()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1228_3_713()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1229_3_714()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1230_3_715()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1231_3_716()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1232_3_717()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1233_3_718()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1234_3_719()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_class_1235_3_720()) return true;
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

 inline bool jj_3R_entity_aspect_1214_3_686()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2749_3_589()
 {
    if (jj_done) return true;
    if (jj_scan_token(QNEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_aspect_1213_3_685()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2748_3_588()
 {
    if (jj_done) return true;
    if (jj_scan_token(QEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2747_3_587()
 {
    if (jj_done) return true;
    if (jj_scan_token(QL_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2746_3_586()
 {
    if (jj_done) return true;
    if (jj_scan_token(QG_T)) return true;
    return false;
  }

 inline bool jj_3R_entity_aspect_1212_1_684()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_aspect_1212_27_755()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_entity_aspect_1212_1_618()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_entity_aspect_1212_1_684()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_aspect_1213_3_685()) {
    jj_scanpos = xsp;
    if (jj_3R_entity_aspect_1214_3_686()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_relation_operator_2745_3_585()
 {
    if (jj_done) return true;
    if (jj_scan_token(QLT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_2723_25_366()
 {
    if (jj_done) return true;
    if (jj_3R_relation_operator_2738_2_475()) return true;
    if (jj_3R_shift_expression_2982_2_365()) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2744_3_584()
 {
    if (jj_done) return true;
    if (jj_scan_token(QGT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2743_3_583()
 {
    if (jj_done) return true;
    if (jj_scan_token(NOTEQU_T)) return true;
    return false;
  }

 inline bool jj_3R_relative_pathname_2728_17_754()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_list_2378_3_69()) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2742_3_582()
 {
    if (jj_done) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2741_3_581()
 {
    if (jj_done) return true;
    if (jj_scan_token(GREATERTHAN_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2740_3_580()
 {
    if (jj_done) return true;
    if (jj_scan_token(EQU_T)) return true;
    return false;
  }

 inline bool jj_3R_element_record_mode_view_indication_1206_2_109()
 {
    if (jj_done) return true;
    if (jj_scan_token(VIEW_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2739_3_579()
 {
    if (jj_done) return true;
    if (jj_scan_token(GT_T)) return true;
    return false;
  }

 inline bool jj_3R_element_mode_view_indication_1201_5_822()
 {
    if (jj_done) return true;
    if (jj_3R_element_array_mode_view_indication_1145_2_850()) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2738_2_578()
 {
    if (jj_done) return true;
    if (jj_scan_token(LT_T)) return true;
    return false;
  }

 inline bool jj_3R_relation_operator_2738_2_475()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_relation_operator_2738_2_578()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2739_3_579()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2740_3_580()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2741_3_581()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2742_3_582()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2743_3_583()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2744_3_584()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2745_3_585()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2746_3_586()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2747_3_587()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2748_3_588()) {
    jj_scanpos = xsp;
    if (jj_3R_relation_operator_2749_3_589()) return true;
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

 inline bool jj_3R_element_mode_view_indication_1200_3_804()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_49()) {
    jj_scanpos = xsp;
    if (jj_3R_element_mode_view_indication_1201_5_822()) return true;
    }
    return false;
  }

 inline bool jj_3R_record_mode_view_indication_2717_21_791()
 {
    if (jj_done) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3_49()
 {
    if (jj_done) return true;
    if (jj_3R_element_record_mode_view_indication_1206_2_109()) return true;
    return false;
  }

 inline bool jj_3R_neg_list_2733_3_796()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEG_T)) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3_48()
 {
    if (jj_done) return true;
    if (jj_3R_array_element_resolution_487_3_108()) return true;
    return false;
  }

 inline bool jj_3R_element_mode_indication_1195_5_769()
 {
    if (jj_done) return true;
    if (jj_3R_element_mode_view_indication_1200_3_804()) return true;
    return false;
  }

 inline bool jj_3R_neg_list_2733_2_753()
 {
    if (jj_done) return true;
    Token * xsp;
    if (jj_3R_neg_list_2733_3_796()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_neg_list_2733_3_796()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_element_mode_indication_1194_3_768()
 {
    if (jj_done) return true;
    if (jj_3R_mode_2101_1_428()) return true;
    return false;
  }

 inline bool jj_3R_element_mode_indication_1194_3_693()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_element_mode_indication_1194_3_768()) {
    jj_scanpos = xsp;
    if (jj_3R_element_mode_indication_1195_5_769()) return true;
    }
    return false;
  }

 inline bool jj_3R_relative_pathname_2728_3_682()
 {
    if (jj_done) return true;
    if (jj_3R_neg_list_2733_2_753()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_relative_pathname_2728_17_754()) jj_scanpos = xsp;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_relation_2723_3_179()
 {
    if (jj_done) return true;
    if (jj_3R_shift_expression_2982_2_365()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_relation_2723_25_366()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_record_element_list_2702_20_767()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_record_mode_view_indication_2717_4_735()
 {
    if (jj_done) return true;
    if (jj_scan_token(VIEW_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_record_mode_view_indication_2717_21_791()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_record_type_definition_2686_21_859()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_record_element_list_2702_5_692()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_record_element_list_2702_20_767()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_element_declaration_1167_3_868()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_record_element_constraint_2697_5_231()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_3R_element_constraint_1161_3_407()) return true;
    return false;
  }

 inline bool jj_3R_element_constraint_1162_4_510()
 {
    if (jj_done) return true;
    if (jj_3R_array_constraint_476_3_621()) return true;
    return false;
  }

 inline bool jj_3_47()
 {
    if (jj_done) return true;
    if (jj_3R_record_constraint_2691_6_107()) return true;
    return false;
  }

 inline bool jj_3R_element_constraint_1161_3_407()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_47()) {
    jj_scanpos = xsp;
    if (jj_3R_element_constraint_1162_4_510()) return true;
    }
    return false;
  }

 inline bool jj_3_46()
 {
    if (jj_done) return true;
    if (jj_3R_choices_775_3_106()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_record_constraint_2691_6_107()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_record_element_constraint_2697_5_231()) return true;
    return false;
  }

 inline bool jj_3R_record_type_definition_2684_9_858()
 {
    if (jj_done) return true;
    if (jj_3R_element_declaration_1167_3_868()) return true;
    return false;
  }

 inline bool jj_3R_record_type_definition_2683_8_846()
 {
    if (jj_done) return true;
    if (jj_scan_token(RECORD_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_record_type_definition_2684_9_858()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(RECORD_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_record_type_definition_2686_21_859()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_element_association_1151_2_453()
 {
    if (jj_done) return true;
    if (jj_3R_choices_775_3_106()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3_45()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3R_element_association_1151_1_305()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_element_association_1151_2_453()) jj_scanpos = xsp;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3_44()
 {
    if (jj_done) return true;
    if (jj_3R_range_2669_4_105()) return true;
    return false;
  }

 inline bool jj_3_128()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_3063_1_85()) return true;
    if (jj_3R_direction_1115_1_86()) return true;
    if (jj_3R_simple_expression_3063_1_85()) return true;
    return false;
  }

 inline bool jj_3_129()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_name_575_3_144()) return true;
    return false;
  }

 inline bool jj_3R_element_array_mode_view_indication_1145_2_850()
 {
    if (jj_done) return true;
    if (jj_scan_token(VIEW_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_range_constraint_2678_1_103()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_3R_range_2669_4_105()) return true;
    return false;
  }

 inline bool jj_3R_discrete_range_1139_3_204()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3_126()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_400_3_141()) return true;
    return false;
  }

 inline bool jj_3R_discrete_range_1136_3_87()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_discrete_range_1136_3_203()) {
    jj_scanpos = xsp;
    if (jj_3R_discrete_range_1139_3_204()) return true;
    }
    return false;
  }

 inline bool jj_3R_discrete_range_1136_3_203()
 {
    if (jj_done) return true;
    if (jj_3R_range_2669_4_105()) return true;
    return false;
  }

 inline bool jj_3R_range_2672_2_228()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_name_575_3_144()) return true;
    return false;
  }

 inline bool jj_3R_range_2669_4_105()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_range_2669_4_227()) {
    jj_scanpos = xsp;
    if (jj_3R_range_2672_2_228()) return true;
    }
    return false;
  }

 inline bool jj_3R_range_2669_4_227()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_3063_1_85()) return true;
    if (jj_3R_direction_1115_1_86()) return true;
    if (jj_3R_simple_expression_3063_1_85()) return true;
    return false;
  }

 inline bool jj_3R_qualified_expression_2662_7_451()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_guarded_signal_specificatio_1131_2_531()
 {
    if (jj_done) return true;
    if (jj_3R_signal_list_3039_2_644()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_qualified_expression_2660_5_450()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_400_3_141()) return true;
    return false;
  }

 inline bool jj_3R_disconnection_specification_1126_1_425()
 {
    if (jj_done) return true;
    if (jj_scan_token(DISCONNECT_T)) return true;
    if (jj_3R_guarded_signal_specificatio_1131_2_531()) return true;
    if (jj_scan_token(AFTER_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_qualified_expression_2659_3_189()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_qualified_expression_2660_5_450()) {
    jj_scanpos = xsp;
    if (jj_3R_qualified_expression_2662_7_451()) return true;
    }
    return false;
  }

 inline bool jj_3R_protected_type_header_2634_26_861()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1620_6_88()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declaration_2629_31_821()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_qualified_expression_2657_3_74()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_127()) {
    jj_scanpos = xsp;
    if (jj_3R_qualified_expression_2659_3_189()) return true;
    }
    return false;
  }

 inline bool jj_3_127()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    if (jj_scan_token(LETTER_OR_DIGIT)) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_discrete_incomplete_type_definition_1121_2_785()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_part_2650_6_849()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_declarative_item_2639_5_862()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_part_2650_5_820()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_protected_type_declarative_part_2650_6_849()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_direction_1116_3_202()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOWNTO_T)) return true;
    return false;
  }

 inline bool jj_3_124()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_3108_1_142()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2644_7_887()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_3302_2_400()) return true;
    return false;
  }

 inline bool jj_3R_direction_1115_1_86()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_direction_1115_1_201()) {
    jj_scanpos = xsp;
    if (jj_3R_direction_1116_3_202()) return true;
    }
    return false;
  }

 inline bool jj_3R_direction_1115_1_201()
 {
    if (jj_done) return true;
    if (jj_scan_token(TO_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2643_7_886()
 {
    if (jj_done) return true;
    if (jj_3R_private_variable_declaration_2471_3_890()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2642_7_885()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_580_3_423()) return true;
    return false;
  }

 inline bool jj_3R_designator_1110_4_737()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_2232_1_369()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2641_7_884()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_instantiation_declaration_3202_2_170()) return true;
    return false;
  }

 inline bool jj_3_125()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_405_2_143()) return true;
    return false;
  }

 inline bool jj_3R_designator_1109_2_736()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_designator_1109_2_659()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_designator_1109_2_736()) {
    jj_scanpos = xsp;
    if (jj_3R_designator_1110_4_737()) return true;
    }
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2639_5_883()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_3108_1_142()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declarative_item_2639_5_862()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_declarative_item_2639_5_883()) {
    jj_scanpos = xsp;
    if (jj_3_125()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_declarative_item_2641_7_884()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_declarative_item_2642_7_885()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_declarative_item_2643_7_886()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_declarative_item_2644_7_887()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3_43()
 {
    if (jj_done) return true;
    if (jj_3R_context_clause_1046_3_104()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_header_2634_7_848()
 {
    if (jj_done) return true;
    if (jj_3R_generic_clause_1610_2_81()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_header_2634_26_861()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_protected_type_header_2634_5_819()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_header_2634_7_848()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_constant_declaration_1026_74_517()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_conditional_expression_960_3_137()) return true;
    return false;
  }

 inline bool jj_3R_context_ref_1068_31_509()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_or_unaffected_expression_965_133_670()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_1093_5_615()
 {
    if (jj_done) return true;
    if (jj_scan_token(REJECT_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_1093_3_493()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_delay_mechanism_1093_5_615()) jj_scanpos = xsp;
    if (jj_scan_token(INERTIAL_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_declaration_2624_4_803()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_3R_protected_type_header_2634_5_819()) return true;
    if (jj_3R_protected_type_declarative_part_2650_5_820()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_declaration_2629_31_821()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_delay_mechanism_1092_1_492()
 {
    if (jj_done) return true;
    if (jj_scan_token(TRANSPORT_T)) return true;
    return false;
  }

 inline bool jj_3R_delay_mechanism_1092_1_381()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_delay_mechanism_1092_1_492()) {
    jj_scanpos = xsp;
    if (jj_3R_delay_mechanism_1093_3_493()) return true;
    }
    return false;
  }

 inline bool jj_3R_protected_type_body_2588_38_818()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_part_2618_7_847()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_body_declarative_item_2593_5_860()) return true;
    return false;
  }

 inline bool jj_3_122()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_part_2618_5_817()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_protected_type_body_declarative_part_2618_7_847()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2612_7_882()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1636_3_426()) return true;
    return false;
  }

 inline bool jj_3_123()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1644_2_80()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2609_7_881()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_3302_2_400()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2608_7_880()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_580_3_423()) return true;
    return false;
  }

 inline bool jj_3_121()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2606_7_879()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3R_context_declaration_1076_2_444()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONTEXT_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2604_7_878()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_405_2_143()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2603_7_877()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1443_2_421()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2602_7_876()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_3338_1_420()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2601_7_875()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_1026_4_418()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2600_7_874()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_3213_1_416()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2599_7_873()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_3264_1_415()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2598_7_872()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3R_context_ref_1068_2_401()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONTEXT_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_context_ref_1068_31_509()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2597_7_871()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3_42()
 {
    if (jj_done) return true;
    if (jj_3R_index_constraint_1735_3_75()) return true;
    return false;
  }

 inline bool jj_3_120()
 {
    if (jj_done) return true;
    if (jj_3R_package_body_2242_1_77()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2594_7_870()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_3085_1_664()) return true;
    return false;
  }

 inline bool jj_3_41()
 {
    if (jj_done) return true;
    if (jj_3R_range_constraint_2678_1_103()) return true;
    return false;
  }

 inline bool jj_3R_context_item_1063_3_226()
 {
    if (jj_done) return true;
    if (jj_3R_context_ref_1068_2_401()) return true;
    return false;
  }

 inline bool jj_3R_context_item_1061_3_225()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_3302_2_400()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2593_5_869()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_3108_1_142()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_declarative_item_2593_5_860()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_body_declarative_item_2593_5_869()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2594_7_870()) {
    jj_scanpos = xsp;
    if (jj_3_120()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2597_7_871()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2598_7_872()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2599_7_873()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2600_7_874()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2601_7_875()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2602_7_876()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2603_7_877()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2604_7_878()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2606_7_879()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2608_7_880()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2609_7_881()) {
    jj_scanpos = xsp;
    if (jj_3_123()) {
    jj_scanpos = xsp;
    if (jj_3R_protected_type_body_declarative_item_2612_7_882()) return true;
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

 inline bool jj_3R_context_item_1060_2_102()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_context_item_1060_2_224()) {
    jj_scanpos = xsp;
    if (jj_3R_context_item_1061_3_225()) {
    jj_scanpos = xsp;
    if (jj_3R_context_item_1063_3_226()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_context_item_1060_2_224()
 {
    if (jj_done) return true;
    if (jj_3R_library_clause_2027_2_399()) return true;
    return false;
  }

 inline bool jj_3R_constraint_1054_5_355()
 {
    if (jj_done) return true;
    if (jj_3R_index_constraint_1735_3_75()) return true;
    return false;
  }

 inline bool jj_3R_constraint_1051_5_173()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_constraint_1051_5_354()) {
    jj_scanpos = xsp;
    if (jj_3R_constraint_1054_5_355()) return true;
    }
    return false;
  }

 inline bool jj_3R_constraint_1051_5_354()
 {
    if (jj_done) return true;
    if (jj_3R_range_constraint_2678_1_103()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_body_2584_4_177()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    if (jj_3R_protected_type_body_declarative_part_2618_5_817()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(PROTECTED_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_body_2588_38_818()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_40()
 {
    if (jj_done) return true;
    if (jj_3R_context_item_1060_2_102()) return true;
    return false;
  }

 inline bool jj_3R_context_clause_1046_3_104()
 {
    if (jj_done) return true;
    Token * xsp;
    if (jj_3_40()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_40()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_process_statement_part_2577_4_414()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2907_5_148()) return true;
    return false;
  }

 inline bool jj_3R_process_statement_part_2577_3_237()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_process_statement_part_2577_4_414()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_process_statement_2555_24_238()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_constraint_array_definition_1040_1_866()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_3R_index_constraint_1735_3_75()) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3R_constant_declaration_1026_4_418()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_constant_declaration_1026_74_517()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_39()
 {
    if (jj_done) return true;
    if (jj_3R_component_configuration_780_5_101()) return true;
    return false;
  }

 inline bool jj_3R_configuration_specification_1021_1_424()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_component_specification_820_1_221()) return true;
    if (jj_3R_binding_indication_622_2_397()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_configuration_item_1015_4_689()
 {
    if (jj_done) return true;
    if (jj_3R_block_configuration_640_5_398()) return true;
    return false;
  }

 inline bool jj_3R_configuration_item_1013_2_688()
 {
    if (jj_done) return true;
    if (jj_3R_component_configuration_780_5_101()) return true;
    return false;
  }

 inline bool jj_3R_configuration_item_1013_2_620()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_configuration_item_1013_2_688()) {
    jj_scanpos = xsp;
    if (jj_3R_configuration_item_1015_4_689()) return true;
    }
    return false;
  }

 inline bool jj_3R_process_statement_2545_4_235()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_sensitivity_list_2895_3_412()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_37()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    return false;
  }

 inline bool jj_3R_process_statement_2537_3_234()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_process_statement_2537_1_112()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_process_statement_2537_3_234()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_process_statement_2545_4_235()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    if (jj_3R_process_declarative_part_2532_2_236()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_process_statement_part_2577_3_237()) return true;
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_process_statement_2555_24_238()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_part_2532_4_413()
 {
    if (jj_done) return true;
    if (jj_3R_process_declarative_item_2507_1_513()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_part_2532_2_236()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_process_declarative_part_2532_4_413()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_conditional_or_unaffected_expression_965_32_669()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_expression_or_unaffected_1387_4_668()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2527_3_635()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1636_3_426()) return true;
    return false;
  }

 inline bool jj_3_38()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2523_3_634()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_3302_2_400()) return true;
    return false;
  }

 inline bool jj_3_119()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1644_2_80()) return true;
    return false;
  }

 inline bool jj_3R_procedure_call_2488_38_394()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_actual_parameter_part_371_4_297()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2522_3_633()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_580_3_423()) return true;
    return false;
  }

 inline bool jj_3_117()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3R_configuration_declaration_986_2_443()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2518_3_632()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_405_2_143()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveforms_979_7_384()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    return false;
  }

 inline bool jj_3_118()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2517_3_631()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1443_2_421()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2516_3_630()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_3338_1_420()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveforms_977_7_383()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_waveform_3377_1_382()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2515_3_629()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_1026_4_418()) return true;
    return false;
  }

 inline bool jj_3R_else_stat_955_28_617()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2514_3_628()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_3264_1_415()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2513_3_627()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_3213_1_416()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2512_3_626()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3R_conditional_expression_960_20_298()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2511_3_625()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3_116()
 {
    if (jj_done) return true;
    if (jj_3R_package_body_2242_1_77()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveforms_976_1_207()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_3377_1_382()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_conditional_waveforms_977_7_383()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_conditional_waveforms_979_7_384()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2508_3_624()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_3085_1_664()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2507_1_623()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_3108_1_142()) return true;
    return false;
  }

 inline bool jj_3R_process_declarative_item_2507_1_513()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_process_declarative_item_2507_1_623()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2508_3_624()) {
    jj_scanpos = xsp;
    if (jj_3_116()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2511_3_625()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2512_3_626()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2513_3_627()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2514_3_628()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2515_3_629()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2516_3_630()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2517_3_631()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2518_3_632()) {
    jj_scanpos = xsp;
    if (jj_3_118()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2522_3_633()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2523_3_634()) {
    jj_scanpos = xsp;
    if (jj_3_119()) {
    jj_scanpos = xsp;
    if (jj_3R_process_declarative_item_2527_3_635()) return true;
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

 inline bool jj_3R_else_wave_list_941_27_750()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_conditional_or_unaffected_expression_965_2_560()
 {
    if (jj_done) return true;
    if (jj_3R_expression_or_unaffected_1387_4_668()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_conditional_or_unaffected_expression_965_32_669()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_conditional_or_unaffected_expression_965_133_670()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_procedure_call_2488_12_393()
 {
    if (jj_done) return true;
    if (jj_scan_token(PARAMETER_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    return false;
  }

 inline bool jj_3R_conditional_force_assignment_948_16_219()
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

 inline bool jj_3R_conditional_expression_960_3_137()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_conditional_expression_960_20_298()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_conditional_force_assignment_949_13_220()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_3R_else_stat_955_4_395()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveform_assignment_935_26_563()
 {
    if (jj_done) return true;
    if (jj_3R_else_wave_list_941_3_672()) return true;
    return false;
  }

 inline bool jj_3R_else_stat_955_5_498()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_else_stat_955_28_617()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_115()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_else_stat_955_4_395()
 {
    if (jj_done) return true;
    Token * xsp;
    if (jj_3R_else_stat_955_5_498()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_else_stat_955_5_498()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_procedure_call_statement_2493_1_155()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_115()) jj_scanpos = xsp;
    if (jj_3R_procedure_call_2488_3_218()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_procedure_call_2488_3_218()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_procedure_call_2488_12_393()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_procedure_call_2488_38_394()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_conditional_waveform_assignment_934_17_561()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_1092_1_381()) return true;
    return false;
  }

 inline bool jj_3R_conditional_force_assignment_946_3_99()
 {
    if (jj_done) return true;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    if (jj_scan_token(FORCE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_conditional_force_assignment_948_16_219()) jj_scanpos = xsp;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_conditional_force_assignment_949_13_220()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_private_incomplete_type_definition_2476_3_784()
 {
    if (jj_done) return true;
    if (jj_scan_token(PRIVATE_T)) return true;
    return false;
  }

 inline bool jj_3R_else_wave_list_941_3_672()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_else_wave_list_941_27_750()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_concurrent_selected_signal_assignment_861_79_208()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_1092_1_381()) return true;
    return false;
  }

 inline bool jj_3R_private_variable_declaration_2471_3_890()
 {
    if (jj_done) return true;
    if (jj_scan_token(PRIVATE_T)) return true;
    if (jj_3R_variable_declaration_3338_1_420()) return true;
    return false;
  }

 inline bool jj_3_36()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_force_assignment_946_3_99()) return true;
    return false;
  }

 inline bool jj_3_114()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3R_conditional_waveform_assignment_933_3_470()
 {
    if (jj_done) return true;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_conditional_waveform_assignment_934_17_561()) jj_scanpos = xsp;
    if (jj_3R_waveform_element_3385_2_562()) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    xsp = jj_scanpos;
    if (jj_3R_conditional_waveform_assignment_935_26_563()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_2465_4_276()
 {
    if (jj_done) return true;
    if (jj_3R_context_declaration_1076_2_444()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_2464_3_275()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_wave_928_3_346()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_waveform_assignment_933_3_470()) return true;
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_wave_926_3_166()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_conditional_signal_assignment_wave_926_3_345()) {
    jj_scanpos = xsp;
    if (jj_3R_conditional_signal_assignment_wave_928_3_346()) return true;
    }
    return false;
  }

 inline bool jj_3R_conditional_signal_assignment_wave_926_3_345()
 {
    if (jj_done) return true;
    if (jj_3R_conditional_force_assignment_946_3_99()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_2459_3_273()
 {
    if (jj_done) return true;
    if (jj_3R_configuration_declaration_986_2_443()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_2461_1_274()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3_113()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_400_3_141()) return true;
    return false;
  }

 inline bool jj_3R_primary_unit_2458_1_123()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_primary_unit_2458_1_272()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_2459_3_273()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_2461_1_274()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_2464_3_275()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_unit_2465_4_276()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_primary_unit_2458_1_272()
 {
    if (jj_done) return true;
    if (jj_3R_entity_declaration_1251_5_442()) return true;
    return false;
  }

 inline bool jj_3_112()
 {
    if (jj_done) return true;
    if (jj_3R_allocator_426_3_140()) return true;
    return false;
  }

 inline bool jj_3R_condition_clause_917_3_464()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNTIL_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    return false;
  }

 inline bool jj_3_111()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3_110()
 {
    if (jj_done) return true;
    if (jj_3R_literal_2052_2_139()) return true;
    return false;
  }

 inline bool jj_3R_primary_2450_1_576()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_400_3_141()) return true;
    return false;
  }

 inline bool jj_3_109()
 {
    if (jj_done) return true;
    if (jj_3R_type_conversion_3259_3_138()) return true;
    return false;
  }

 inline bool jj_3R_primary_2447_1_575()
 {
    if (jj_done) return true;
    if (jj_3R_allocator_426_3_140()) return true;
    return false;
  }

 inline bool jj_3_108()
 {
    if (jj_done) return true;
    if (jj_3R_qualified_expression_2657_3_74()) return true;
    return false;
  }

 inline bool jj_3R_primary_2444_1_574()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_condition_906_3_100()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3_35()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_procedure_call_statement_836_1_98()) return true;
    return false;
  }

 inline bool jj_3_107()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_conditional_expression_960_3_137()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_primary_2441_1_573()
 {
    if (jj_done) return true;
    if (jj_3R_literal_2052_2_139()) return true;
    return false;
  }

 inline bool jj_3_34()
 {
    if (jj_done) return true;
    if (jj_3R_component_instantiation_statement_803_2_97()) return true;
    return false;
  }

 inline bool jj_3_106()
 {
    if (jj_done) return true;
    if (jj_3R_function_call_1534_1_136()) return true;
    return false;
  }

 inline bool jj_3R_primary_2438_1_572()
 {
    if (jj_done) return true;
    if (jj_3R_type_conversion_3259_3_138()) return true;
    return false;
  }

 inline bool jj_3_33()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_signal_association_statement_866_2_96()) return true;
    return false;
  }

 inline bool jj_3R_primary_2435_1_571()
 {
    if (jj_done) return true;
    if (jj_3R_qualified_expression_2657_3_74()) return true;
    return false;
  }

 inline bool jj_3_32()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_signal_assignment_statement_841_1_95()) return true;
    return false;
  }

 inline bool jj_3_104()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_2367_3_134()) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_physical_type_definition_2401_33_865()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_primary_2432_1_570()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_conditional_expression_960_3_137()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_null_886_12_94()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_31()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_886_12_94()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(ASSERT_T)) return true;
    return false;
  }

 inline bool jj_3R_primary_2429_1_569()
 {
    if (jj_done) return true;
    if (jj_3R_function_call_1534_1_136()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_simple_signal_assignment_856_39_496()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_1092_1_381()) return true;
    return false;
  }

 inline bool jj_3R_primary_2429_1_473()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_primary_2429_1_569()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2432_1_570()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2435_1_571()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2438_1_572()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2441_1_573()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2444_1_574()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2447_1_575()) {
    jj_scanpos = xsp;
    if (jj_3R_primary_2450_1_576()) return true;
    }
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_null_880_12_92()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3_29()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_880_12_92()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_scan_token(PROCESS_T)) return true;
    return false;
  }

 inline bool jj_3R_port_map_aspect_2424_2_392()
 {
    if (jj_done) return true;
    if (jj_scan_token(PORT_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_association_list_553_1_205()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_list_2378_47_178()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_2367_3_134()) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_null_877_12_91()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_concurrent_conditional_signal_assignment_851_38_206()
 {
    if (jj_done) return true;
    if (jj_3R_delay_mechanism_1092_1_381()) return true;
    return false;
  }

 inline bool jj_3_28()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_877_12_91()) jj_scanpos = xsp;
    if (jj_scan_token(BLOCK_T)) return true;
    return false;
  }

 inline bool jj_3R_port_list_2419_3_700()
 {
    if (jj_done) return true;
    if (jj_3R_interface_list_1836_3_377()) return true;
    return false;
  }

 inline bool jj_3_30()
 {
    if (jj_done) return true;
    if (jj_3R_generate_statement_1592_1_93()) return true;
    return false;
  }

 inline bool jj_3R_port_clause_2414_4_641()
 {
    if (jj_done) return true;
    if (jj_scan_token(PORT_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_port_list_2419_3_700()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_physical_incomplete_type_definition_2409_5_786()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNITS_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_physical_type_definition_2399_14_864()
 {
    if (jj_done) return true;
    if (jj_3R_secondary_unit_declaration_2813_1_888()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_simple_signal_association_871_2_214()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(LGT_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_package_instantiation_declaration_2350_57_307()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_1544_4_172()) return true;
    return false;
  }

 inline bool jj_3_27()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_physical_type_definition_2394_9_855()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNITS_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_physical_type_definition_2399_14_864()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(UNITS_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_physical_type_definition_2401_33_865()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_concurrent_signal_association_statement_866_2_96()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_27()) jj_scanpos = xsp;
    if (jj_3R_concurrent_simple_signal_association_871_2_214()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_selected_signal_assignment_861_6_90()
 {
    if (jj_done) return true;
    if (jj_scan_token(WITH_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(SELECT_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(158)) jj_scanpos = xsp;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(50)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_concurrent_selected_signal_assignment_861_79_208()) jj_scanpos = xsp;
    if (jj_3R_selected_waveforms_2835_2_209()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_105()
 {
    if (jj_done) return true;
    if (jj_3R_abstract_literal_338_4_135()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_simple_signal_assignment_856_7_388()
 {
    if (jj_done) return true;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(50)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_concurrent_simple_signal_assignment_856_39_496()) jj_scanpos = xsp;
    if (jj_3R_waveform_3377_1_382()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_concurrent_conditional_signal_assignment_851_6_89()
 {
    if (jj_done) return true;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(LESSTHAN_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(50)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_concurrent_conditional_signal_assignment_851_38_206()) jj_scanpos = xsp;
    if (jj_3R_conditional_waveforms_976_1_207()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_26()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_selected_signal_assignment_861_6_90()) return true;
    return false;
  }

 inline bool jj_3R_physical_literal_2389_3_292()
 {
    if (jj_done) return true;
    if (jj_3R_abstract_literal_338_4_135()) return true;
    return false;
  }

 inline bool jj_3R_physical_literal_2389_2_133()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_physical_literal_2389_3_292()) jj_scanpos = xsp;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3_25()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_conditional_signal_assignment_851_6_89()) return true;
    return false;
  }

 inline bool jj_3R_package_path_name_2384_3_683()
 {
    if (jj_done) return true;
    if (jj_scan_token(AT_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_2367_19_293()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_845_3_213()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_simple_signal_assignment_856_7_388()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_844_3_212()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_selected_signal_assignment_861_6_90()) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_list_2378_3_69()
 {
    if (jj_done) return true;
    if (jj_3R_pathname_element_2367_3_134()) return true;
    if (jj_scan_token(DOT_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_pathname_element_list_2378_47_178()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_24()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_843_1_211()
 {
    if (jj_done) return true;
    if (jj_3R_concurrent_conditional_signal_assignment_851_6_89()) return true;
    return false;
  }

 inline bool jj_3R_concurrent_signal_assignment_statement_841_1_95()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_24()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_concurrent_signal_assignment_statement_843_1_211()) {
    jj_scanpos = xsp;
    if (jj_3R_concurrent_signal_assignment_statement_844_3_212()) {
    jj_scanpos = xsp;
    if (jj_3R_concurrent_signal_assignment_statement_845_3_213()) return true;
    }
    }
    return false;
  }

 inline bool jj_3_23()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_concurrent_procedure_call_statement_836_1_98()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_23()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_3R_procedure_call_2488_3_218()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_21()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1620_6_88()) return true;
    return false;
  }

 inline bool jj_3_22()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_pathname_element_2367_3_134()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_pathname_element_2367_19_293()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_concurrent_assertion_statement_831_1_111()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_22()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(79)) jj_scanpos = xsp;
    if (jj_3R_assertion_528_4_233()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_composite_type_definition_826_3_815()
 {
    if (jj_done) return true;
    if (jj_3R_record_type_definition_2683_8_846()) return true;
    return false;
  }

 inline bool jj_3R_composite_type_definition_825_2_814()
 {
    if (jj_done) return true;
    if (jj_3R_array_type_definition_519_2_845()) return true;
    return false;
  }

 inline bool jj_3R_composite_type_definition_825_2_799()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_composite_type_definition_825_2_814()) {
    jj_scanpos = xsp;
    if (jj_3R_composite_type_definition_826_3_815()) return true;
    }
    return false;
  }

 inline bool jj_3R_component_declaration_796_31_529()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_parameter_specification_2360_1_667()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(IN_T)) return true;
    if (jj_3R_discrete_range_1136_3_87()) return true;
    return false;
  }

 inline bool jj_3R_component_instantiation_statement_815_11_217()
 {
    if (jj_done) return true;
    if (jj_3R_port_map_aspect_2424_2_392()) return true;
    return false;
  }

 inline bool jj_3R_component_instantiation_statement_814_11_216()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1620_6_88()) return true;
    return false;
  }

 inline bool jj_3R_component_specification_820_1_221()
 {
    if (jj_done) return true;
    if (jj_3R_instantiation_list_1761_3_396()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_package_instantiation_declaration_2350_2_110()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_instantiation_declaration_2350_57_307()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_part_2345_3_376()
 {
    if (jj_done) return true;
    if (jj_3R_package_declarative_item_2314_1_487()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_part_2345_2_193()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_package_declarative_part_2345_3_376()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_package_declarative_item_2339_2_611()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3R_component_instantiation_statement_803_2_97()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_instantiation_unit_1754_1_215()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_instantiation_statement_814_11_216()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_instantiation_statement_815_11_217()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_103()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2335_3_610()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1636_3_426()) return true;
    return false;
  }

 inline bool jj_3R_case_scheme_730_73_83()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3_101()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2331_3_609()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_3302_2_400()) return true;
    return false;
  }

 inline bool jj_3_102()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1644_2_80()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2330_3_608()
 {
    if (jj_done) return true;
    if (jj_3R_disconnection_specification_1126_1_425()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2329_3_607()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_580_3_423()) return true;
    return false;
  }

 inline bool jj_3R_package_declaration_2305_26_194()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_component_declaration_790_5_528()
 {
    if (jj_done) return true;
    if (jj_3R_port_clause_2414_4_641()) return true;
    return false;
  }

 inline bool jj_3R_component_declaration_789_5_527()
 {
    if (jj_done) return true;
    if (jj_3R_generic_clause_1610_2_81()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2327_1_606()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2325_3_605()
 {
    if (jj_done) return true;
    if (jj_3R_mode_view_declaration_2116_3_417()) return true;
    return false;
  }

 inline bool jj_3R_component_configuration_782_11_223()
 {
    if (jj_done) return true;
    if (jj_3R_block_configuration_640_5_398()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2324_3_604()
 {
    if (jj_done) return true;
    if (jj_3R_component_declaration_787_2_422()) return true;
    return false;
  }

 inline bool jj_3R_component_configuration_781_11_222()
 {
    if (jj_done) return true;
    if (jj_3R_binding_indication_622_2_397()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2323_3_603()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_405_2_143()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2322_3_602()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1443_2_421()) return true;
    return false;
  }

 inline bool jj_3R_choices_775_15_230()
 {
    if (jj_done) return true;
    if (jj_scan_token(BAR_T)) return true;
    if (jj_3R_choice_759_4_229()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2321_3_601()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_3338_1_420()) return true;
    return false;
  }

 inline bool jj_3R_component_declaration_787_2_422()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMPONENT_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(56)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_declaration_789_5_527()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_declaration_790_5_528()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(28)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_declaration_796_31_529()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2320_3_600()
 {
    if (jj_done) return true;
    if (jj_3R_signal_declaration_3022_1_419()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2319_3_599()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_1026_4_418()) return true;
    return false;
  }

 inline bool jj_3_100()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3R_component_configuration_780_5_101()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_component_specification_820_1_221()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_configuration_781_11_222()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_component_configuration_782_11_223()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_99()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2316_3_598()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_3213_1_416()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2315_3_597()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_3264_1_415()) return true;
    return false;
  }

 inline bool jj_3_16()
 {
    if (jj_done) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_case_scheme_730_73_83()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2314_1_596()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_3108_1_142()) return true;
    return false;
  }

 inline bool jj_3R_package_declarative_item_2314_1_487()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_declarative_item_2314_1_596()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2315_3_597()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2316_3_598()) {
    jj_scanpos = xsp;
    if (jj_3_99()) {
    jj_scanpos = xsp;
    if (jj_3_100()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2319_3_599()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2320_3_600()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2321_3_601()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2322_3_602()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2323_3_603()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2324_3_604()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2325_3_605()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2327_1_606()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2329_3_607()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2330_3_608()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2331_3_609()) {
    jj_scanpos = xsp;
    if (jj_3_102()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2335_3_610()) {
    jj_scanpos = xsp;
    if (jj_3_103()) {
    jj_scanpos = xsp;
    if (jj_3R_package_declarative_item_2339_2_611()) return true;
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
    }
    }
    }
    return false;
  }

 inline bool jj_3_20()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_1136_3_87()) return true;
    return false;
  }

 inline bool jj_3R_choices_775_3_106()
 {
    if (jj_done) return true;
    if (jj_3R_choice_759_4_229()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_choices_775_15_230()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3_19()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_3063_1_85()) return true;
    return false;
  }

 inline bool jj_3R_choice_769_5_406()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3_18()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_3063_1_85()) return true;
    if (jj_3R_direction_1115_1_86()) return true;
    return false;
  }

 inline bool jj_3R_package_header_2282_25_486()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1620_6_88()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_choice_768_4_405()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_choice_765_4_404()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_1136_3_87()) return true;
    return false;
  }

 inline bool jj_3R_choice_762_4_403()
 {
    if (jj_done) return true;
    if (jj_3R_simple_expression_3063_1_85()) return true;
    return false;
  }

 inline bool jj_3R_choice_759_4_229()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_choice_759_4_402()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_762_4_403()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_765_4_404()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_768_4_405()) {
    jj_scanpos = xsp;
    if (jj_3R_choice_769_5_406()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_choice_759_4_402()
 {
    if (jj_done) return true;
    if (jj_3R_range_2669_4_105()) return true;
    return false;
  }

 inline bool jj_3R_case_statement_721_38_760()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_character_literal_754_3_445()
 {
    if (jj_done) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_package_declaration_2289_2_78()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_package_header_2282_4_192()) return true;
    if (jj_3R_package_declarative_part_2345_2_193()) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(76)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_declaration_2305_26_194()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_package_header_2282_6_375()
 {
    if (jj_done) return true;
    if (jj_3R_generic_clause_1610_2_81()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_header_2282_25_486()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_package_body_2251_36_526()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3_17()
 {
    if (jj_done) return true;
    if (jj_3R_label_2022_2_84()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_package_header_2282_4_192()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_header_2282_6_375()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_case_statement_alternative_743_3_748()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_choices_775_3_106()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    if (jj_3R_sequential_statement_body_2969_2_797()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_part_2277_2_640()
 {
    if (jj_done) return true;
    if (jj_3R_package_body_declarative_item_2256_1_699()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_part_2277_1_524()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_package_body_declarative_part_2277_2_640()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2272_3_780()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1636_3_426()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2268_3_779()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_3302_2_400()) return true;
    return false;
  }

 inline bool jj_3_98()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1644_2_80()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2267_3_778()
 {
    if (jj_done) return true;
    if (jj_3R_mode_view_declaration_2116_3_417()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2266_3_777()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_405_2_143()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2265_3_776()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1443_2_421()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2264_4_775()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3R_case_statement_720_11_759()
 {
    if (jj_done) return true;
    if (jj_3R_case_statement_alternative_743_3_748()) return true;
    return false;
  }

 inline bool jj_3_97()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3_96()
 {
    if (jj_done) return true;
    if (jj_3R_package_body_2242_1_77()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2261_3_774()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_3338_1_420()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2260_3_773()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_1026_4_418()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2259_3_772()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_3213_1_416()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2258_3_771()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_3264_1_415()) return true;
    return false;
  }

 inline bool jj_3R_package_body_2251_10_525()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2256_1_770()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_3108_1_142()) return true;
    return false;
  }

 inline bool jj_3R_package_body_declarative_item_2256_1_699()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_body_declarative_item_2256_1_770()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2258_3_771()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2259_3_772()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2260_3_773()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2261_3_774()) {
    jj_scanpos = xsp;
    if (jj_3_96()) {
    jj_scanpos = xsp;
    if (jj_3_97()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2264_4_775()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2265_3_776()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2266_3_777()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2267_3_778()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2268_3_779()) {
    jj_scanpos = xsp;
    if (jj_3_98()) {
    jj_scanpos = xsp;
    if (jj_3R_package_body_declarative_item_2272_3_780()) return true;
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

 inline bool jj_3_15()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_case_statement_712_1_157()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_15()) jj_scanpos = xsp;
    if (jj_scan_token(CASE_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(158)) jj_scanpos = xsp;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_case_statement_alternative_743_3_748()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_case_statement_720_11_759()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(CASE_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(158)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_case_statement_721_38_760()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_package_body_2242_1_77()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_scan_token(BODY_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_package_body_declarative_part_2277_1_524()) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_body_2251_10_525()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_package_body_2251_36_526()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_next_statement_2196_43_336()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_block_specification_688_12_619()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_index_specification_1740_2_687()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_operator_symbol_2232_1_369()
 {
    if (jj_done) return true;
    if (jj_scan_token(STRINGLITERAL)) return true;
    return false;
  }

 inline bool jj_3R_object_class_2227_2_658()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    return false;
  }

 inline bool jj_3_14()
 {
    if (jj_done) return true;
    if (jj_3R_generic_clause_1610_2_81()) return true;
    return false;
  }

 inline bool jj_3R_object_class_2226_2_657()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_view_declaration_2116_112_516()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_object_class_2225_2_656()
 {
    if (jj_done) return true;
    if (jj_scan_token(SHARED_T)) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3_95()
 {
    if (jj_done) return true;
    if (jj_3R_physical_literal_2389_2_133()) return true;
    return false;
  }

 inline bool jj_3R_object_class_2224_2_655()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARIABLE_T)) return true;
    return false;
  }

 inline bool jj_3R_block_specification_688_3_505()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_block_specification_688_12_619()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_object_class_2223_2_654()
 {
    if (jj_done) return true;
    if (jj_scan_token(SIGNAL_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_2222_1_653()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONSTANT_T)) return true;
    return false;
  }

 inline bool jj_3R_object_class_2222_1_545()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_object_class_2222_1_653()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_2223_2_654()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_2224_2_655()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_2225_2_656()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_2226_2_657()) {
    jj_scanpos = xsp;
    if (jj_3R_object_class_2227_2_658()) return true;
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_numeric_literal_2217_4_278()
 {
    if (jj_done) return true;
    if (jj_3R_abstract_literal_338_4_135()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_2173_47_446()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_numeric_literal_2215_2_125()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_numeric_literal_2215_2_277()) {
    jj_scanpos = xsp;
    if (jj_3R_numeric_literal_2217_4_278()) return true;
    }
    return false;
  }

 inline bool jj_3R_numeric_literal_2215_2_277()
 {
    if (jj_done) return true;
    if (jj_3R_physical_literal_2389_2_133()) return true;
    return false;
  }

 inline bool jj_3R_null_statement_2209_3_556()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_null_statement_2209_1_462()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_null_statement_2209_3_556()) jj_scanpos = xsp;
    if (jj_scan_token(NULL_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_671_3_256()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1636_3_426()) return true;
    return false;
  }

 inline bool jj_3_12()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3_11()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3_13()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1644_2_80()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_667_3_255()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_3302_2_400()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_666_3_254()
 {
    if (jj_done) return true;
    if (jj_3R_disconnection_specification_1126_1_425()) return true;
    return false;
  }

 inline bool jj_3R_null_2172_32_132()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_665_3_253()
 {
    if (jj_done) return true;
    if (jj_3R_configuration_specification_1021_1_424()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_664_3_252()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_580_3_423()) return true;
    return false;
  }

 inline bool jj_3R_next_statement_2197_3_337()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHEN_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    return false;
  }

 inline bool jj_3R_next_statement_2196_3_335()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_660_3_250()
 {
    if (jj_done) return true;
    if (jj_3R_component_declaration_787_2_422()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_662_1_251()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3_93()
 {
    if (jj_done) return true;
    if (jj_scan_token(LBRACKET_T)) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_659_3_249()
 {
    if (jj_done) return true;
    if (jj_3R_package_instantiation_declaration_2350_2_110()) return true;
    return false;
  }

 inline bool jj_3R_next_statement_2196_1_159()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_next_statement_2196_3_335()) jj_scanpos = xsp;
    if (jj_scan_token(NEXT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_next_statement_2196_43_336()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_next_statement_2197_3_337()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_658_3_248()
 {
    if (jj_done) return true;
    if (jj_3R_package_declaration_2289_2_78()) return true;
    return false;
  }

 inline bool jj_3_10()
 {
    if (jj_done) return true;
    if (jj_3R_package_body_2242_1_77()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_656_3_247()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_405_2_143()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_655_3_246()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1443_2_421()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_654_3_245()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_3338_1_420()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_653_3_244()
 {
    if (jj_done) return true;
    if (jj_3R_signal_declaration_3022_1_419()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_652_3_243()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_1026_4_418()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_651_3_242()
 {
    if (jj_done) return true;
    if (jj_3R_mode_view_declaration_2116_3_417()) return true;
    return false;
  }

 inline bool jj_3R_test_att_name_2185_4_449()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_block_configuration_642_11_507()
 {
    if (jj_done) return true;
    if (jj_3R_configuration_item_1013_2_620()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_650_3_241()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_3213_1_416()) return true;
    return false;
  }

 inline bool jj_3R_test_att_name_2183_5_447()
 {
    if (jj_done) return true;
    if (jj_3R_signature_3054_1_472()) return true;
    return false;
  }

 inline bool jj_3R_block_configuration_641_11_506()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_3302_2_400()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_649_3_240()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_3264_1_415()) return true;
    return false;
  }

 inline bool jj_3R_block_declarative_item_648_3_113()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_block_declarative_item_648_3_239()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_649_3_240()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_650_3_241()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_651_3_242()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_652_3_243()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_653_3_244()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_654_3_245()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_655_3_246()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_656_3_247()) {
    jj_scanpos = xsp;
    if (jj_3_10()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_658_3_248()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_659_3_249()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_660_3_250()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_662_1_251()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_664_3_252()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_665_3_253()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_666_3_254()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_667_3_255()) {
    jj_scanpos = xsp;
    if (jj_3_13()) {
    jj_scanpos = xsp;
    if (jj_3R_block_declarative_item_671_3_256()) return true;
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
    }
    }
    }
    return false;
  }

 inline bool jj_3R_block_declarative_item_648_3_239()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_3108_1_142()) return true;
    return false;
  }

 inline bool jj_3_92()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_null_2172_32_132()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_test_att_name_2183_3_291()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_test_att_name_2183_5_447()) jj_scanpos = xsp;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_3R_attribute_designator_568_3_448()) return true;
    xsp = jj_scanpos;
    if (jj_3R_test_att_name_2185_4_449()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_test_att_name_2181_4_131()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_94()) {
    jj_scanpos = xsp;
    if (jj_3R_test_att_name_2183_3_291()) return true;
    }
    return false;
  }

 inline bool jj_3_94()
 {
    if (jj_done) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    if (jj_scan_token(INTEGER)) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_91()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_discrete_range_1136_3_87()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_87()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext_2159_3_129()) return true;
    return false;
  }

 inline bool jj_3R_block_configuration_640_5_398()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_block_specification_688_3_505()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_block_configuration_641_11_506()) { jj_scanpos = xsp; break; }
    }
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_block_configuration_642_11_507()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_90()
 {
    if (jj_done) return true;
    if (jj_3R_test_att_name_2181_4_131()) return true;
    return false;
  }

 inline bool jj_3R_attribute_name_575_68_313()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_84()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_2153_4_128()) return true;
    return false;
  }

 inline bool jj_3_85()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_2153_4_128()) return true;
    return false;
  }

 inline bool jj_3R_mode_view_declaration_2116_61_515()
 {
    if (jj_done) return true;
    if (jj_3R_mode_view_element_definition_2121_3_637()) return true;
    return false;
  }

 inline bool jj_3_86()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_2153_4_128()) return true;
    return false;
  }

 inline bool jj_3_89()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_suffix_3230_1_130()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_2172_3_286()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_name_ext_2173_47_446()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_88()
 {
    if (jj_done) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_name_ext1_2153_18_281()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext_2159_3_129()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_2169_2_285()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_discrete_range_1136_3_87()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_name_ext_2166_3_284()
 {
    if (jj_done) return true;
    if (jj_3R_test_att_name_2181_4_131()) return true;
    return false;
  }

 inline bool jj_3R_name_2143_25_370()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_2153_4_128()) return true;
    return false;
  }

 inline bool jj_3R_name_2144_24_372()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_2153_4_128()) return true;
    return false;
  }

 inline bool jj_3R_bit_string_literal_629_3_124()
 {
    if (jj_done) return true;
    if (jj_scan_token(BIT_STRING_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_name_2145_22_373()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext1_2153_4_128()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_2163_2_283()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_suffix_3230_1_130()) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_624_4_504()
 {
    if (jj_done) return true;
    if (jj_3R_port_map_aspect_2424_2_392()) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_623_4_503()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1620_6_88()) return true;
    return false;
  }

 inline bool jj_3R_name_ext_2159_3_129()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_ext_2160_2_282()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_2163_2_283()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_2166_3_284()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_2169_2_285()) {
    jj_scanpos = xsp;
    if (jj_3R_name_ext_2172_3_286()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_name_ext_2160_2_282()
 {
    if (jj_done) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_622_4_502()
 {
    if (jj_done) return true;
    if (jj_scan_token(USE_T)) return true;
    if (jj_3R_entity_aspect_1212_1_618()) return true;
    return false;
  }

 inline bool jj_3R_binding_indication_622_2_397()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_binding_indication_622_4_502()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_binding_indication_623_4_503()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_binding_indication_624_4_504()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_ext1_2153_4_128()
 {
    if (jj_done) return true;
    if (jj_3R_name_ext_2159_3_129()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_name_ext1_2153_18_281()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_attribute_name_575_46_312()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_name_2145_6_188()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_2145_22_373()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_2144_5_187()
 {
    if (jj_done) return true;
    if (jj_3R_external_name_1400_2_371()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_2144_24_372()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_2143_4_186()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_2232_1_369()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_2143_25_370()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_name_2142_2_73()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_name_2143_4_186()) {
    jj_scanpos = xsp;
    if (jj_3R_name_2144_5_187()) {
    jj_scanpos = xsp;
    if (jj_3R_name_2145_6_188()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_multiplying_operation_2136_3_359()
 {
    if (jj_done) return true;
    if (jj_scan_token(REM_T)) return true;
    return false;
  }

 inline bool jj_3R_multiplying_operation_2135_3_358()
 {
    if (jj_done) return true;
    if (jj_scan_token(MOD_T)) return true;
    return false;
  }

 inline bool jj_3R_multiplying_operation_2134_3_357()
 {
    if (jj_done) return true;
    if (jj_scan_token(SLASH_T)) return true;
    return false;
  }

 inline bool jj_3R_multiplying_operation_2133_1_174()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_multiplying_operation_2133_1_356()) {
    jj_scanpos = xsp;
    if (jj_3R_multiplying_operation_2134_3_357()) {
    jj_scanpos = xsp;
    if (jj_3R_multiplying_operation_2135_3_358()) {
    jj_scanpos = xsp;
    if (jj_3R_multiplying_operation_2136_3_359()) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_multiplying_operation_2133_1_356()
 {
    if (jj_done) return true;
    if (jj_scan_token(MULT_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_view_indication_2127_5_652()
 {
    if (jj_done) return true;
    if (jj_3R_record_mode_view_indication_2717_4_735()) return true;
    return false;
  }

 inline bool jj_3R_mode_view_indication_2126_3_544()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_83()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_view_indication_2127_5_652()) return true;
    }
    return false;
  }

 inline bool jj_3_83()
 {
    if (jj_done) return true;
    if (jj_3R_array_mode_view_indication_512_1_127()) return true;
    return false;
  }

 inline bool jj_3R_assertion_528_63_411()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEVERITY_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_mode_view_element_definition_2121_3_637()
 {
    if (jj_done) return true;
    if (jj_3R_record_element_list_2702_5_692()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_element_mode_indication_1194_3_693()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_view_declaration_2116_3_417()
 {
    if (jj_done) return true;
    if (jj_scan_token(VIEW_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    if (jj_scan_token(IS_T)) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_mode_view_declaration_2116_61_515()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(VIEW_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_mode_view_declaration_2116_112_516()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_specification_580_3_423()
 {
    if (jj_done) return true;
    if (jj_scan_token(ATTRIBUTE_T)) return true;
    if (jj_3R_attribute_designator_568_3_448()) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_entity_specification_1326_1_530()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_conditional_expression_960_3_137()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_association_list_553_26_380()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_association_element_547_2_379()) return true;
    return false;
  }

 inline bool jj_3R_attribute_name_575_3_144()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(APOSTROPHE_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(86)) {
    jj_scanpos = xsp;
    if (jj_3R_attribute_name_575_46_312()) return true;
    }
    xsp = jj_scanpos;
    if (jj_3R_attribute_name_575_68_313()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_attribute_designator_569_5_547()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_2080_28_749()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_mode_2105_3_539()
 {
    if (jj_done) return true;
    if (jj_scan_token(LINKAGE_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_2104_3_538()
 {
    if (jj_done) return true;
    if (jj_scan_token(BUFFER_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_designator_568_3_546()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_mode_2103_3_537()
 {
    if (jj_done) return true;
    if (jj_scan_token(INOUT_T)) return true;
    return false;
  }

 inline bool jj_3R_attribute_designator_568_3_448()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_attribute_designator_568_3_546()) {
    jj_scanpos = xsp;
    if (jj_3R_attribute_designator_569_5_547()) return true;
    }
    return false;
  }

 inline bool jj_3R_mode_2102_3_536()
 {
    if (jj_done) return true;
    if (jj_scan_token(OUT_T)) return true;
    return false;
  }

 inline bool jj_3_78()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(OF_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_2101_1_535()
 {
    if (jj_done) return true;
    if (jj_scan_token(IN_T)) return true;
    return false;
  }

 inline bool jj_3R_mode_2101_1_428()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_mode_2101_1_535()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_2102_3_536()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_2103_3_537()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_2104_3_538()) {
    jj_scanpos = xsp;
    if (jj_3R_mode_2105_3_539()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_attribute_declaration_558_2_79()
 {
    if (jj_done) return true;
    if (jj_scan_token(ATTRIBUTE_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_type_mark_3291_3_195()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_9()
 {
    if (jj_done) return true;
    if (jj_3R_formal_part_1496_1_76()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_assertion_528_31_410()
 {
    if (jj_done) return true;
    if (jj_scan_token(REPORT_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_association_list_553_1_205()
 {
    if (jj_done) return true;
    if (jj_3R_association_element_547_2_379()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_association_list_553_26_380()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_association_element_547_3_490()
 {
    if (jj_done) return true;
    if (jj_3R_formal_part_1496_1_76()) return true;
    if (jj_scan_token(ARROW_T)) return true;
    return false;
  }

 inline bool jj_3R_association_element_547_2_379()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_association_element_547_3_490()) jj_scanpos = xsp;
    if (jj_3R_actual_part_380_2_491()) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_2073_7_333()
 {
    if (jj_done) return true;
    if (jj_3R_iteration_scheme_2004_1_466()) return true;
    return false;
  }

 inline bool jj_3R_assertion_statement_539_5_324()
 {
    if (jj_done) return true;
    if (jj_3R_label_2022_2_84()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_assertion_statement_539_3_151()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_assertion_statement_539_5_324()) jj_scanpos = xsp;
    if (jj_3R_assertion_528_4_233()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_2072_3_332()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_array_index_incomplete_type_list_505_33_826()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_array_index_incomplete_type_497_2_825()) return true;
    return false;
  }

 inline bool jj_3R_loop_statement_2072_1_158()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_loop_statement_2072_3_332()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_loop_statement_2073_7_333()) jj_scanpos = xsp;
    if (jj_scan_token(LOOP_T)) return true;
    if (jj_3R_sequence_of_statements_2902_2_334()) return true;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(LOOP_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_loop_statement_2080_28_749()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_82()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_literal_1354_2_126()) return true;
    return false;
  }

 inline bool jj_3R_assertion_528_4_233()
 {
    if (jj_done) return true;
    if (jj_scan_token(ASSERT_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_assertion_528_31_410()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_assertion_528_63_411()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_81()
 {
    if (jj_done) return true;
    if (jj_3R_numeric_literal_2215_2_125()) return true;
    return false;
  }

 inline bool jj_3_8()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_3R_index_constraint_1735_3_75()) return true;
    if (jj_scan_token(OF_T)) return true;
    return false;
  }

 inline bool jj_3R_literal_2062_3_303()
 {
    if (jj_done) return true;
    if (jj_scan_token(NULL_T)) return true;
    return false;
  }

 inline bool jj_3_80()
 {
    if (jj_done) return true;
    if (jj_3R_bit_string_literal_629_3_124()) return true;
    return false;
  }

 inline bool jj_3R_literal_2061_3_302()
 {
    if (jj_done) return true;
    if (jj_3R_string_literal_3078_1_452()) return true;
    return false;
  }

 inline bool jj_3R_array_type_definition_522_4_857()
 {
    if (jj_done) return true;
    if (jj_3R_unconstraint_array_definition_3296_1_867()) return true;
    return false;
  }

 inline bool jj_3R_literal_2059_1_301()
 {
    if (jj_done) return true;
    if (jj_3R_enumeration_literal_1354_2_126()) return true;
    return false;
  }

 inline bool jj_3R_literal_2055_2_300()
 {
    if (jj_done) return true;
    if (jj_3R_numeric_literal_2215_2_125()) return true;
    return false;
  }

 inline bool jj_3R_array_type_definition_519_2_856()
 {
    if (jj_done) return true;
    if (jj_3R_constraint_array_definition_1040_1_866()) return true;
    return false;
  }

 inline bool jj_3R_array_type_definition_519_2_845()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_array_type_definition_519_2_856()) {
    jj_scanpos = xsp;
    if (jj_3R_array_type_definition_522_4_857()) return true;
    }
    return false;
  }

 inline bool jj_3R_literal_2052_2_139()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_literal_2052_2_299()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_2055_2_300()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_2059_1_301()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_2061_3_302()) {
    jj_scanpos = xsp;
    if (jj_3R_literal_2062_3_303()) return true;
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_literal_2052_2_299()
 {
    if (jj_done) return true;
    if (jj_3R_bit_string_literal_629_3_124()) return true;
    return false;
  }

 inline bool jj_3R_array_mode_view_indication_512_1_127()
 {
    if (jj_done) return true;
    if (jj_scan_token(VIEW_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3_79()
 {
    if (jj_done) return true;
    if (jj_3R_primary_unit_2458_1_123()) return true;
    return false;
  }

 inline bool jj_3R_array_index_incomplete_type_list_505_2_807()
 {
    if (jj_done) return true;
    if (jj_3R_array_index_incomplete_type_497_2_825()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_array_index_incomplete_type_list_505_33_826()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_array_index_incomplete_type_499_4_853()
 {
    if (jj_done) return true;
    if (jj_3R_anonymous_type_indication_459_2_543()) return true;
    return false;
  }

 inline bool jj_3R_array_index_incomplete_type_498_4_852()
 {
    if (jj_done) return true;
    if (jj_3R_index_subtype_definition_1749_2_863()) return true;
    return false;
  }

 inline bool jj_3R_array_index_incomplete_type_497_2_851()
 {
    if (jj_done) return true;
    if (jj_3R_index_constraint_1735_3_75()) return true;
    return false;
  }

 inline bool jj_3R_array_index_incomplete_type_497_2_825()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_array_index_incomplete_type_497_2_851()) {
    jj_scanpos = xsp;
    if (jj_3R_array_index_incomplete_type_498_4_852()) {
    jj_scanpos = xsp;
    if (jj_3R_array_index_incomplete_type_499_4_853()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_array_incomplete_type_definition_492_3_788()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_array_index_incomplete_type_list_505_2_807()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_incomplete_subtype_indication_1696_3_808()) return true;
    return false;
  }

 inline bool jj_3R_library_clause_2027_2_399()
 {
    if (jj_done) return true;
    if (jj_scan_token(LIBRARY_T)) return true;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_array_element_resolution_487_3_108()
 {
    if (jj_done) return true;
    if (jj_3R_resolution_indication_2769_4_232()) return true;
    return false;
  }

 inline bool jj_3R_label_2022_2_84()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1973_49_260()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_array_constraint_477_5_690()
 {
    if (jj_done) return true;
    if (jj_3R_index_constraint_1735_3_75()) return true;
    return false;
  }

 inline bool jj_3R_array_constraint_476_3_621()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_7()) {
    jj_scanpos = xsp;
    if (jj_3R_array_constraint_477_5_690()) return true;
    }
    return false;
  }

 inline bool jj_3R_iteration_scheme_2011_3_558()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_parameter_specification_2360_1_667()) return true;
    return false;
  }

 inline bool jj_3_7()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_scan_token(OPEN_T)) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1972_37_259()
 {
    if (jj_done) return true;
    if (jj_3R_mode_2101_1_428()) return true;
    return false;
  }

 inline bool jj_3R_iteration_scheme_2004_1_557()
 {
    if (jj_done) return true;
    if (jj_scan_token(WHILE_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    return false;
  }

 inline bool jj_3R_iteration_scheme_2004_1_466()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_iteration_scheme_2004_1_557()) {
    jj_scanpos = xsp;
    if (jj_3R_iteration_scheme_2011_3_558()) return true;
    }
    return false;
  }

 inline bool jj_3R_anonymous_type_indication_459_2_543()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_incomplete_type_definition_1702_2_651()) return true;
    return false;
  }

 inline bool jj_3R_alias_declaration_407_44_523()
 {
    if (jj_done) return true;
    if (jj_3R_signature_3054_1_472()) return true;
    return false;
  }

 inline bool jj_3R_ifunc_1879_98_650()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_aggregate_400_39_306()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_element_association_1151_1_305()) return true;
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1971_3_257()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(117)) {
    jj_scanpos = xsp;
    if (jj_scan_token(101)) {
    jj_scanpos = xsp;
    if (jj_scan_token(31)) {
    jj_scanpos = xsp;
    if (jj_scan_token(102)) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_interface_type_indication_1965_8_435()
 {
    if (jj_done) return true;
    if (jj_3R_mode_view_indication_2126_3_544()) return true;
    return false;
  }

 inline bool jj_3R_interface_variable_declaration_1971_1_115()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_variable_declaration_1971_3_257()) jj_scanpos = xsp;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(COLON_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_interface_variable_declaration_1972_37_259()) jj_scanpos = xsp;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(27)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_variable_declaration_1973_49_260()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_interface_type_indication_1964_8_434()
 {
    if (jj_done) return true;
    if (jj_3R_anonymous_type_indication_459_2_543()) return true;
    return false;
  }

 inline bool jj_3R_allocator_428_6_304()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3R_interface_type_indication_1963_6_268()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_type_indication_1963_6_433()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_type_indication_1964_8_434()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_type_indication_1965_8_435()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_interface_type_indication_1963_6_433()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3R_ifunc_1879_87_542()
 {
    if (jj_done) return true;
    if (jj_scan_token(IS_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_ifunc_1879_98_650()) {
    jj_scanpos = xsp;
    if (jj_scan_token(148)) return true;
    }
    return false;
  }

 inline bool jj_3R_allocator_426_3_140()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_6()) {
    jj_scanpos = xsp;
    if (jj_3R_allocator_428_6_304()) return true;
    }
    return false;
  }

 inline bool jj_3_6()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_qualified_expression_2657_3_74()) return true;
    return false;
  }

 inline bool jj_3R_interface_signal_declaration_1859_104_270()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_conditional_expression_960_3_137()) return true;
    return false;
  }

 inline bool jj_3R_interface_type_declaration_1958_3_119()
 {
    if (jj_done) return true;
    if (jj_3R_interface_incomplete_type_declaration_1937_3_267()) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_421_4_458()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_2232_1_369()) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_420_4_457()
 {
    if (jj_done) return true;
    if (jj_scan_token(CHARACTER_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_alias_designator_419_2_310()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_alias_designator_419_2_456()) {
    jj_scanpos = xsp;
    if (jj_3R_alias_designator_420_4_457()) {
    jj_scanpos = xsp;
    if (jj_3R_alias_designator_421_4_458()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_alias_designator_419_2_456()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_alias_declaration_406_4_311()
 {
    if (jj_done) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3R_alias_declaration_405_2_143()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALIAS_T)) return true;
    if (jj_3R_alias_designator_419_2_310()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_alias_declaration_406_4_311()) jj_scanpos = xsp;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    xsp = jj_scanpos;
    if (jj_3R_alias_declaration_407_44_523()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_incomplete_type_declaration_1937_3_267()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_360_44_368()
 {
    if (jj_done) return true;
    if (jj_scan_token(INERTIAL_T)) return true;
    return false;
  }

 inline bool jj_3R_aggregate_400_3_141()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_element_association_1151_1_305()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_aggregate_400_39_306()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_adding_operator_394_5_350()
 {
    if (jj_done) return true;
    if (jj_scan_token(MINUS_T)) return true;
    return false;
  }

 inline bool jj_3R_adding_operator_395_4_351()
 {
    if (jj_done) return true;
    if (jj_scan_token(AMPERSAND_T)) return true;
    return false;
  }

 inline bool jj_3_5()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_adding_operator_393_3_168()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_adding_operator_393_3_349()) {
    jj_scanpos = xsp;
    if (jj_3R_adding_operator_394_5_350()) {
    jj_scanpos = xsp;
    if (jj_3R_adding_operator_395_4_351()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_adding_operator_393_3_349()
 {
    if (jj_done) return true;
    if (jj_scan_token(PLUS_T)) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_3368_97_330()
 {
    if (jj_done) return true;
    if (jj_3R_timeout_clause_3251_1_465()) return true;
    return false;
  }

 inline bool jj_3_75()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_4()
 {
    if (jj_done) return true;
    if (jj_3R_actual_designator_358_1_72()) return true;
    return false;
  }

 inline bool jj_3R_actual_part_385_2_614()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_actual_designator_358_1_72()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_part_383_4_613()
 {
    if (jj_done) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_part_380_2_612()
 {
    if (jj_done) return true;
    if (jj_3R_actual_designator_358_1_72()) return true;
    return false;
  }

 inline bool jj_3R_actual_part_380_2_491()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_actual_part_380_2_612()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_part_383_4_613()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_part_385_2_614()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_param_1907_5_649()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_interface_list_1836_3_377()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_parameter_part_371_4_297()
 {
    if (jj_done) return true;
    if (jj_3R_association_list_553_1_205()) return true;
    return false;
  }

 inline bool jj_3R_param_1906_3_540()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(77)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_param_1907_5_649()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_2()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(54)) jj_scanpos = xsp;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_interface_constant_declaration_1816_89_269()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_conditional_expression_960_3_137()) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_3368_71_329()
 {
    if (jj_done) return true;
    if (jj_3R_condition_clause_917_3_464()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_364_2_185()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3_3()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_360_1_184()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_actual_designator_360_44_368()) jj_scanpos = xsp;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_358_1_183()
 {
    if (jj_done) return true;
    if (jj_scan_token(OPEN_T)) return true;
    return false;
  }

 inline bool jj_3R_actual_designator_358_1_72()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_actual_designator_358_1_183()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_designator_360_1_184()) {
    jj_scanpos = xsp;
    if (jj_3_3()) {
    jj_scanpos = xsp;
    if (jj_3R_actual_designator_364_2_185()) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_variable_declaration_3338_90_520()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1620_6_88()) return true;
    return false;
  }

 inline bool jj_3R_access_incomplete_type_definition_352_3_789()
 {
    if (jj_done) return true;
    if (jj_scan_token(ACCESS_T)) return true;
    if (jj_3R_incomplete_subtype_indication_1696_3_808()) return true;
    return false;
  }

 inline bool jj_3R_access_type_definition_346_3_800()
 {
    if (jj_done) return true;
    if (jj_scan_token(ACCESS_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3R_ifunc_1879_3_541()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(85)) {
    jj_scanpos = xsp;
    if (jj_scan_token(52)) return true;
    }
    return false;
  }

 inline bool jj_3R_ifunc_1879_2_431()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_ifunc_1879_3_541()) jj_scanpos = xsp;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    if (jj_3R_param_1906_3_540()) return true;
    if (jj_scan_token(RETURN_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    xsp = jj_scanpos;
    if (jj_3R_ifunc_1879_87_542()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_abstract_literal_340_4_296()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASED_LITERAL)) return true;
    return false;
  }

 inline bool jj_3R_abstract_literal_339_4_295()
 {
    if (jj_done) return true;
    if (jj_scan_token(INTEGER)) return true;
    return false;
  }

 inline bool jj_3R_abstract_literal_338_4_135()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_abstract_literal_338_4_294()) {
    jj_scanpos = xsp;
    if (jj_3R_abstract_literal_339_4_295()) {
    jj_scanpos = xsp;
    if (jj_3R_abstract_literal_340_4_296()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_abstract_literal_338_4_294()
 {
    if (jj_done) return true;
    if (jj_scan_token(DECIMAL_LITERAL)) return true;
    return false;
  }

 inline bool jj_3_1()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_pathname_element_list_2378_3_69()) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_3368_43_328()
 {
    if (jj_done) return true;
    if (jj_3R_sensitivity_clause_2840_2_463()) return true;
    return false;
  }

 inline bool jj_3R_iproc_1870_3_430()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_3R_param_1906_3_540()) return true;
    return false;
  }

 inline bool jj_3R_absolute_pathname_331_3_752()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_waveform_element_3385_19_671()
 {
    if (jj_done) return true;
    if (jj_scan_token(AFTER_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_interface_subprogram_declaration_1865_4_265()
 {
    if (jj_done) return true;
    if (jj_3R_ifunc_1879_2_431()) return true;
    return false;
  }

 inline bool jj_3R_interface_subprogram_declaration_1864_4_117()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_subprogram_declaration_1864_4_264()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_subprogram_declaration_1865_4_265()) return true;
    }
    return false;
  }

 inline bool jj_3R_interface_subprogram_declaration_1864_4_264()
 {
    if (jj_done) return true;
    if (jj_3R_iproc_1870_3_430()) return true;
    return false;
  }

 inline bool jj_3R_absolute_pathname_329_2_751()
 {
    if (jj_done) return true;
    if (jj_scan_token(DOT_T)) return true;
    if (jj_3R_pathname_element_list_2378_3_69()) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_absolute_pathname_329_2_681()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_absolute_pathname_329_2_751()) {
    jj_scanpos = xsp;
    if (jj_3R_absolute_pathname_331_3_752()) return true;
    }
    return false;
  }

 inline bool jj_3R_waveform_3377_23_616()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_waveform_element_3385_2_562()) return true;
    return false;
  }

 inline bool jj_3_74()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEMI_T)) return true;
    if (jj_3R_interface_element_1821_3_122()) return true;
    return false;
  }

 inline bool jj_3R_interface_signal_declaration_1859_2_121()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(101)) jj_scanpos = xsp;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_interface_type_indication_1963_6_268()) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(27)) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_signal_declaration_1859_104_270()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_interface_package_generic_map_aspect_1849_5_432()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1620_6_88()) return true;
    return false;
  }

 inline bool jj_3_77()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_scan_token(DEFAULT_T)) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_waveform_element_3385_2_562()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_waveform_element_3385_19_671()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_76()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_package_generic_map_aspect_1847_3_266()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_76()) {
    jj_scanpos = xsp;
    if (jj_3_77()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_package_generic_map_aspect_1849_5_432()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_waveform_3379_1_495()
 {
    if (jj_done) return true;
    if (jj_scan_token(UNAFFECTED_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_package_declaration_1841_3_118()
 {
    if (jj_done) return true;
    if (jj_scan_token(PACKAGE_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    if (jj_3R_interface_package_generic_map_aspect_1847_3_266()) return true;
    return false;
  }

 inline bool jj_3R_waveform_3377_1_494()
 {
    if (jj_done) return true;
    if (jj_3R_waveform_element_3385_2_562()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_waveform_3377_23_616()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_waveform_3377_1_382()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_waveform_3377_1_494()) {
    jj_scanpos = xsp;
    if (jj_3R_waveform_3379_1_495()) return true;
    }
    return false;
  }

 inline bool jj_3R_instantiation_unit_1755_86_497()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_list_1836_3_377()
 {
    if (jj_done) return true;
    if (jj_3R_interface_element_1821_3_122()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_74()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3_75()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_wait_statement_3368_3_327()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_wait_statement_3368_1_153()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_3368_3_327()) jj_scanpos = xsp;
    if (jj_scan_token(WAIT_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_3368_43_328()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_3368_71_329()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_wait_statement_3368_97_330()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_interface_file_declaration_1826_3_429()
 {
    if (jj_done) return true;
    if (jj_scan_token(FILE_T)) return true;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3_73()
 {
    if (jj_done) return true;
    if (jj_3R_interface_signal_declaration_1859_2_121()) return true;
    return false;
  }

 inline bool jj_3_72()
 {
    if (jj_done) return true;
    if (jj_3R_interface_constant_declaration_1816_3_120()) return true;
    return false;
  }

 inline bool jj_3R_interface_element_1821_3_122()
 {
    if (jj_done) return true;
    if (jj_3R_interface_declaration_1778_1_271()) return true;
    return false;
  }

 inline bool jj_3R_interface_object_declaration_1811_9_263()
 {
    if (jj_done) return true;
    if (jj_3R_interface_file_declaration_1826_3_429()) return true;
    return false;
  }

 inline bool jj_3R_interface_constant_declaration_1816_3_120()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(31)) jj_scanpos = xsp;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(COLON_T)) return true;
    xsp = jj_scanpos;
    if (jj_scan_token(53)) jj_scanpos = xsp;
    if (jj_3R_interface_type_indication_1963_6_268()) return true;
    xsp = jj_scanpos;
    if (jj_3R_interface_constant_declaration_1816_89_269()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_interface_object_declaration_1809_9_262()
 {
    if (jj_done) return true;
    if (jj_3R_interface_signal_declaration_1859_2_121()) return true;
    return false;
  }

 inline bool jj_3R_interface_object_declaration_1808_7_116()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_object_declaration_1808_7_261()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_object_declaration_1809_9_262()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_object_declaration_1811_9_263()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_interface_object_declaration_1808_7_261()
 {
    if (jj_done) return true;
    if (jj_3R_interface_constant_declaration_1816_3_120()) return true;
    return false;
  }

 inline bool jj_3R_unconstraint_array_definition_3296_53_889()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_index_subtype_definition_1749_2_863()) return true;
    return false;
  }

 inline bool jj_3R_variable_declaration_3340_3_521()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_conditional_expression_960_3_137()) return true;
    return false;
  }

 inline bool jj_3_71()
 {
    if (jj_done) return true;
    if (jj_3R_interface_subprogram_declaration_1864_4_117()) return true;
    return false;
  }

 inline bool jj_3R_variable_declaration_3338_1_420()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(102)) jj_scanpos = xsp;
    if (jj_scan_token(VARIABLE_T)) return true;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    xsp = jj_scanpos;
    if (jj_3R_variable_declaration_3338_90_520()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_variable_declaration_3340_3_521()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_variable_assignment_statement_3333_2_555()
 {
    if (jj_done) return true;
    if (jj_3R_selected_variable_assignment_2856_3_666()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1797_2_441()
 {
    if (jj_done) return true;
    if (jj_3R_object_class_2222_1_545()) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3_69()
 {
    if (jj_done) return true;
    if (jj_3R_interface_package_declaration_1841_3_118()) return true;
    return false;
  }

 inline bool jj_3_168()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_variable_assignment_statement_3329_1_554()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_168()) jj_scanpos = xsp;
    if (jj_3R_target_3239_2_150()) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_conditional_or_unaffected_expression_965_2_560()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_use_clause_3302_28_532()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_selected_name_2823_2_508()) return true;
    return false;
  }

 inline bool jj_3_68()
 {
    if (jj_done) return true;
    if (jj_3R_interface_subprogram_declaration_1864_4_117()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1794_1_440()
 {
    if (jj_done) return true;
    if (jj_3R_interface_subprogram_declaration_1864_4_117()) return true;
    return false;
  }

 inline bool jj_3R_variable_assignment_statement_3329_1_461()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_variable_assignment_statement_3329_1_554()) {
    jj_scanpos = xsp;
    if (jj_3R_variable_assignment_statement_3333_2_555()) return true;
    }
    return false;
  }

 inline bool jj_3_67()
 {
    if (jj_done) return true;
    if (jj_3R_interface_object_declaration_1808_7_116()) return true;
    return false;
  }

 inline bool jj_3_70()
 {
    if (jj_done) return true;
    if (jj_3R_interface_type_declaration_1958_3_119()) return true;
    return false;
  }

 inline bool jj_3_66()
 {
    if (jj_done) return true;
    if (jj_3R_interface_variable_declaration_1971_1_115()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1787_1_439()
 {
    if (jj_done) return true;
    if (jj_3R_interface_package_declaration_1841_3_118()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1784_1_438()
 {
    if (jj_done) return true;
    if (jj_3R_interface_subprogram_declaration_1864_4_117()) return true;
    return false;
  }

 inline bool jj_3_163()
 {
    if (jj_done) return true;
    if (jj_3R_constraint_1051_5_173()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1781_2_437()
 {
    if (jj_done) return true;
    if (jj_3R_interface_object_declaration_1808_7_116()) return true;
    return false;
  }

 inline bool jj_3R_interface_declaration_1778_1_271()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_interface_declaration_1778_1_436()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1781_2_437()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1784_1_438()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1787_1_439()) {
    jj_scanpos = xsp;
    if (jj_3_70()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1794_1_440()) {
    jj_scanpos = xsp;
    if (jj_3R_interface_declaration_1797_2_441()) return true;
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_interface_declaration_1778_1_436()
 {
    if (jj_done) return true;
    if (jj_3R_interface_variable_declaration_1971_1_115()) return true;
    return false;
  }

 inline bool jj_3R_index_constraint_1735_42_190()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_discrete_range_1136_3_87()) return true;
    return false;
  }

 inline bool jj_3_158()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1620_6_88()) return true;
    return false;
  }

 inline bool jj_3R_subtype_indication_3224_85_182()
 {
    if (jj_done) return true;
    if (jj_3R_constraint_1051_5_173()) return true;
    return false;
  }

 inline bool jj_3_65()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASIC_IDENTIFIER)) return true;
    if (jj_scan_token(DOT_T)) return true;
    return false;
  }

 inline bool jj_3R_protected_type_instantiation_definition_3285_20_816()
 {
    if (jj_done) return true;
    if (jj_3R_generic_map_aspect_1620_6_88()) return true;
    return false;
  }

 inline bool jj_3R_use_clause_3302_2_400()
 {
    if (jj_done) return true;
    if (jj_scan_token(USE_T)) return true;
    if (jj_3R_selected_name_2823_2_508()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_use_clause_3302_28_532()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1763_3_501()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1762_3_500()
 {
    if (jj_done) return true;
    if (jj_scan_token(OTHER_T)) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1761_3_499()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_list_1663_4_258()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_list_1761_3_396()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_instantiation_list_1761_3_499()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_list_1762_3_500()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_list_1763_3_501()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_unconstraint_array_definition_3296_1_867()
 {
    if (jj_done) return true;
    if (jj_scan_token(ARRAY_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_index_subtype_definition_1749_2_863()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_unconstraint_array_definition_3296_53_889()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(OF_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1756_3_391()
 {
    if (jj_done) return true;
    if (jj_scan_token(CONFIGURATION_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_type_mark_3291_3_195()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1755_3_390()
 {
    if (jj_done) return true;
    if (jj_scan_token(ENTITY_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_65()) jj_scanpos = xsp;
    if (jj_3R_name_2142_2_73()) return true;
    xsp = jj_scanpos;
    if (jj_3R_instantiation_unit_1755_86_497()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_subprogram_specification_3174_118_662()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_header_3155_6_738()) return true;
    return false;
  }

 inline bool jj_3R_instantiation_unit_1754_1_215()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_instantiation_unit_1754_1_389()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_unit_1755_3_390()) {
    jj_scanpos = xsp;
    if (jj_3R_instantiation_unit_1756_3_391()) return true;
    }
    }
    return false;
  }

 inline bool jj_3R_instantiation_unit_1754_1_389()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(28)) jj_scanpos = xsp;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_protected_type_instantiation_definition_3285_3_802()
 {
    if (jj_done) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_protected_type_instantiation_definition_3285_20_816()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3_64()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_1136_3_87()) return true;
    return false;
  }

 inline bool jj_3R_index_subtype_definition_1749_2_863()
 {
    if (jj_done) return true;
    if (jj_3R_type_mark_3291_3_195()) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3_162()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_3279_3_766()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_declaration_2624_4_803()) return true;
    return false;
  }

 inline bool jj_3R_index_specification_1742_3_757()
 {
    if (jj_done) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3_167()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_body_2584_4_177()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_3275_3_765()
 {
    if (jj_done) return true;
    if (jj_3R_protected_type_instantiation_definition_3285_3_802()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_3274_3_764()
 {
    if (jj_done) return true;
    if (jj_3R_file_type_definition_1464_2_801()) return true;
    return false;
  }

 inline bool jj_3R_index_specification_1740_2_756()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_range_1136_3_87()) return true;
    return false;
  }

 inline bool jj_3R_index_specification_1740_2_687()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_index_specification_1740_2_756()) {
    jj_scanpos = xsp;
    if (jj_3R_index_specification_1742_3_757()) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_instantiation_declaration_3202_75_353()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_1544_4_172()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_3273_3_763()
 {
    if (jj_done) return true;
    if (jj_3R_access_type_definition_346_3_800()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_3272_3_762()
 {
    if (jj_done) return true;
    if (jj_3R_composite_type_definition_825_2_799()) return true;
    return false;
  }

 inline bool jj_3R_index_constraint_1735_3_75()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_discrete_range_1136_3_87()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_index_constraint_1735_42_190()) { jj_scanpos = xsp; break; }
    }
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_type_definition_3271_1_761()
 {
    if (jj_done) return true;
    if (jj_3R_scalar_type_definition_2794_1_798()) return true;
    return false;
  }

 inline bool jj_3R_type_definition_3271_1_691()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_type_definition_3271_1_761()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_3272_3_762()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_3273_3_763()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_3274_3_764()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_3275_3_765()) {
    jj_scanpos = xsp;
    if (jj_3_167()) {
    jj_scanpos = xsp;
    if (jj_3R_type_definition_3279_3_766()) return true;
    }
    }
    }
    }
    }
    }
    return false;
  }

 inline bool jj_3R_if_statement_1687_47_758()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    return false;
  }

 inline bool jj_3R_type_declaration_3266_3_514()
 {
    if (jj_done) return true;
    if (jj_3R_incomplete_type_declaration_1727_3_636()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_declaration_1727_3_636()
 {
    if (jj_done) return true;
    if (jj_scan_token(TYPE_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_166()
 {
    if (jj_done) return true;
    if (jj_3R_full_type_declaration_1501_3_176()) return true;
    return false;
  }

 inline bool jj_3R_type_declaration_3264_1_415()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_166()) {
    jj_scanpos = xsp;
    if (jj_3R_type_declaration_3266_3_514()) return true;
    }
    return false;
  }

 inline bool jj_3_62()
 {
    if (jj_done) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_type_conversion_3259_3_138()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3_165()
 {
    if (jj_done) return true;
    if (jj_3R_multiplying_operation_2133_1_174()) return true;
    if (jj_3R_factor_1433_1_175()) return true;
    return false;
  }

 inline bool jj_3R_integer_incomplete_type_definition_1722_3_787()
 {
    if (jj_done) return true;
    if (jj_scan_token(RANGE_T)) return true;
    if (jj_scan_token(BOX_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_instantiation_declaration_3202_58_352()
 {
    if (jj_done) return true;
    if (jj_3R_signature_3054_1_472()) return true;
    return false;
  }

 inline bool jj_3R_timeout_clause_3251_1_465()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_definition_1710_5_734()
 {
    if (jj_done) return true;
    if (jj_3R_file_incomplete_type_definition_1469_2_790()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_definition_1709_5_733()
 {
    if (jj_done) return true;
    if (jj_3R_access_incomplete_type_definition_352_3_789()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_definition_1708_5_732()
 {
    if (jj_done) return true;
    if (jj_3R_array_incomplete_type_definition_492_3_788()) return true;
    return false;
  }

 inline bool jj_3R_simpleTerm_3246_2_169()
 {
    if (jj_done) return true;
    if (jj_3R_factor_1433_1_175()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_165()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_incomplete_type_definition_1707_5_731()
 {
    if (jj_done) return true;
    if (jj_3R_integer_incomplete_type_definition_1722_3_787()) return true;
    return false;
  }

 inline bool jj_3_63()
 {
    if (jj_done) return true;
    if (jj_3R_floating_incomplete_type_definition_1474_4_114()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_definition_1705_5_730()
 {
    if (jj_done) return true;
    if (jj_3R_physical_incomplete_type_definition_2409_5_786()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_definition_1704_5_729()
 {
    if (jj_done) return true;
    if (jj_3R_discrete_incomplete_type_definition_1121_2_785()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_definition_1703_5_728()
 {
    if (jj_done) return true;
    if (jj_scan_token(148)) return true;
    return false;
  }

 inline bool jj_3R_target_3240_3_323()
 {
    if (jj_done) return true;
    if (jj_3R_aggregate_400_3_141()) return true;
    return false;
  }

 inline bool jj_3R_target_3239_2_150()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_target_3239_2_322()) {
    jj_scanpos = xsp;
    if (jj_3R_target_3240_3_323()) return true;
    }
    return false;
  }

 inline bool jj_3_164()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_target_3239_2_322()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_definition_1702_2_727()
 {
    if (jj_done) return true;
    if (jj_3R_private_incomplete_type_definition_2476_3_784()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_type_definition_1702_2_651()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_incomplete_type_definition_1702_2_727()) {
    jj_scanpos = xsp;
    if (jj_3R_incomplete_type_definition_1703_5_728()) {
    jj_scanpos = xsp;
    if (jj_3R_incomplete_type_definition_1704_5_729()) {
    jj_scanpos = xsp;
    if (jj_3R_incomplete_type_definition_1705_5_730()) {
    jj_scanpos = xsp;
    if (jj_3_63()) {
    jj_scanpos = xsp;
    if (jj_3R_incomplete_type_definition_1707_5_731()) {
    jj_scanpos = xsp;
    if (jj_3R_incomplete_type_definition_1708_5_732()) {
    jj_scanpos = xsp;
    if (jj_3R_incomplete_type_definition_1709_5_733()) {
    jj_scanpos = xsp;
    if (jj_3R_incomplete_type_definition_1710_5_734()) return true;
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

 inline bool jj_3R_incomplete_subtype_indication_1697_5_828()
 {
    if (jj_done) return true;
    if (jj_3R_anonymous_type_indication_459_2_543()) return true;
    return false;
  }

 inline bool jj_3R_suffix_3234_3_290()
 {
    if (jj_done) return true;
    if (jj_scan_token(ALL_T)) return true;
    return false;
  }

 inline bool jj_3R_suffix_3233_3_289()
 {
    if (jj_done) return true;
    if (jj_3R_operator_symbol_2232_1_369()) return true;
    return false;
  }

 inline bool jj_3R_suffix_3232_3_288()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_754_3_445()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_subtype_indication_1696_3_827()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    return false;
  }

 inline bool jj_3R_incomplete_subtype_indication_1696_3_808()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_incomplete_subtype_indication_1696_3_827()) {
    jj_scanpos = xsp;
    if (jj_3R_incomplete_subtype_indication_1697_5_828()) return true;
    }
    return false;
  }

 inline bool jj_3R_suffix_3230_1_130()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_suffix_3230_1_287()) {
    jj_scanpos = xsp;
    if (jj_3R_suffix_3232_3_288()) {
    jj_scanpos = xsp;
    if (jj_3R_suffix_3233_3_289()) {
    jj_scanpos = xsp;
    if (jj_3R_suffix_3234_3_290()) return true;
    }
    }
    }
    return false;
  }

 inline bool jj_3R_suffix_3230_1_287()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_subtype_indication_3224_4_181()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_subtype_indication_3224_3_71()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subtype_indication_3224_4_181()) jj_scanpos = xsp;
    if (jj_3_162()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3_162()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_subtype_indication_3224_85_182()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_if_statement_1683_6_747()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSE_T)) return true;
    if (jj_3R_sequence_of_statements_2902_2_334()) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1676_6_746()
 {
    if (jj_done) return true;
    if (jj_scan_token(ELSIF_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    if (jj_scan_token(THEN_T)) return true;
    if (jj_3R_sequence_of_statements_2902_2_334()) return true;
    return false;
  }

 inline bool jj_3R_identifier_list_1663_16_427()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_subtype_declaration_3213_1_416()
 {
    if (jj_done) return true;
    if (jj_scan_token(SUBTYPE_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_subtype_indication_3224_3_71()) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1668_4_331()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_if_statement_1668_3_156()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_if_statement_1668_4_331()) jj_scanpos = xsp;
    if (jj_scan_token(IF_T)) return true;
    if (jj_3R_condition_906_3_100()) return true;
    if (jj_scan_token(THEN_T)) return true;
    if (jj_3R_sequence_of_statements_2902_2_334()) return true;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_if_statement_1676_6_746()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3R_if_statement_1683_6_747()) jj_scanpos = xsp;
    if (jj_scan_token(END_T)) return true;
    if (jj_scan_token(IF_T)) return true;
    xsp = jj_scanpos;
    if (jj_3R_if_statement_1687_47_758()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_161()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(OF_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_instantiation_declaration_3202_2_170()
 {
    if (jj_done) return true;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(NEW_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_instantiation_declaration_3202_58_352()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_instantiation_declaration_3202_75_353()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_identifier_list_1663_4_258()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_identifier_list_1663_16_427()) { jj_scanpos = xsp; break; }
    }
    xsp = jj_scanpos;
    if (jj_3_62()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_subprogram_statement_part_3197_4_793()
 {
    if (jj_done) return true;
    if (jj_3R_sequential_statement_2907_5_148()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_statement_part_3197_3_741()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_subprogram_statement_part_3197_4_793()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_identifier_1658_4_198()
 {
    if (jj_done) return true;
    if (jj_scan_token(BASIC_IDENTIFIER)) return true;
    return false;
  }

 inline bool jj_3R_identifier_1657_3_82()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_identifier_1657_3_197()) {
    jj_scanpos = xsp;
    if (jj_3R_identifier_1658_4_198()) return true;
    }
    return false;
  }

 inline bool jj_3R_identifier_1657_3_197()
 {
    if (jj_done) return true;
    if (jj_scan_token(EXTENDED_CHARACTER)) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_list_1631_28_648()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_group_constituent_1625_2_647()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_3186_3_663()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(77)) jj_scanpos = xsp;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_formal_parameter_list_1491_2_739()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_group_template_declaration_1644_2_80()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_entity_class_entry_list_1245_2_533()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_3174_4_661()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(85)) {
    jj_scanpos = xsp;
    if (jj_scan_token(52)) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_specification_3174_2_549()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_3174_4_661()) jj_scanpos = xsp;
    if (jj_scan_token(FUNCTION_T)) return true;
    if (jj_3R_designator_1109_2_659()) return true;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_3174_118_662()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_3186_3_663()) jj_scanpos = xsp;
    if (jj_scan_token(RETURN_T)) return true;
    xsp = jj_scanpos;
    if (jj_3_161()) jj_scanpos = xsp;
    if (jj_3R_type_mark_3291_3_195()) return true;
    return false;
  }

 inline bool jj_3_160()
 {
    if (jj_done) return true;
    if (jj_3R_gen_assoc_list_1544_4_172()) return true;
    return false;
  }

 inline bool jj_3R_group_declaration_1636_3_426()
 {
    if (jj_done) return true;
    if (jj_scan_token(GROUP_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_group_constituent_list_1631_3_534()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3_159()
 {
    if (jj_done) return true;
    if (jj_3R_gen_interface_list_1549_4_171()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_3168_5_660()
 {
    if (jj_done) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_interface_list_1836_3_377()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_list_1631_3_534()
 {
    if (jj_done) return true;
    if (jj_3R_group_constituent_1625_2_647()) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_group_constituent_list_1631_28_648()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_group_constituent_1626_4_726()
 {
    if (jj_done) return true;
    if (jj_3R_character_literal_754_3_445()) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_1625_2_725()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_group_constituent_1625_2_647()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_group_constituent_1625_2_725()) {
    jj_scanpos = xsp;
    if (jj_3R_group_constituent_1626_4_726()) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_specification_3160_2_548()
 {
    if (jj_done) return true;
    if (jj_scan_token(PROCEDURE_T)) return true;
    if (jj_3R_designator_1109_2_659()) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_3168_5_660()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3_159()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3_160()) jj_scanpos = xsp;
    if (jj_3R_param_1906_3_540()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_specification_3160_2_454()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_specification_3160_2_548()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_specification_3174_2_549()) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_header_3155_6_738()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_generic_list_1615_2_196()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3_158()) jj_scanpos = xsp;
    return false;
  }

 inline bool jj_3R_generic_map_aspect_1620_6_88()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_association_list_553_1_205()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_generic_list_1615_2_196()
 {
    if (jj_done) return true;
    if (jj_3R_interface_list_1836_3_377()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_kind_3149_2_794()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_scan_token(46)) {
    jj_scanpos = xsp;
    if (jj_scan_token(81)) return true;
    }
    return false;
  }

 inline bool jj_3_61()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_part_3144_4_792()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declarative_item_3123_1_809()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_part_3144_3_740()
 {
    if (jj_done) return true;
    Token * xsp;
    while (true) {
      xsp = jj_scanpos;
      if (jj_3R_subprogram_declarative_part_3144_4_792()) { jj_scanpos = xsp; break; }
    }
    return false;
  }

 inline bool jj_3R_generic_clause_1610_2_81()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_generic_list_1615_2_196()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_generate_scheme_1605_3_387()
 {
    if (jj_done) return true;
    if (jj_scan_token(IF_T)) return true;
    return false;
  }

 inline bool jj_3_156()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3139_3_840()
 {
    if (jj_done) return true;
    if (jj_3R_group_declaration_1636_3_426()) return true;
    return false;
  }

 inline bool jj_3R_generate_scheme_1604_1_210()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_generate_scheme_1604_1_386()) {
    jj_scanpos = xsp;
    if (jj_3R_generate_scheme_1605_3_387()) return true;
    }
    return false;
  }

 inline bool jj_3R_generate_scheme_1604_1_386()
 {
    if (jj_done) return true;
    if (jj_scan_token(FOR_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3135_3_839()
 {
    if (jj_done) return true;
    if (jj_3R_use_clause_3302_2_400()) return true;
    return false;
  }

 inline bool jj_3_157()
 {
    if (jj_done) return true;
    if (jj_3R_group_template_declaration_1644_2_80()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3134_3_838()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_specification_580_3_423()) return true;
    return false;
  }

 inline bool jj_3_60()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3132_1_837()
 {
    if (jj_done) return true;
    if (jj_3R_attribute_declaration_558_2_79()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3130_3_836()
 {
    if (jj_done) return true;
    if (jj_3R_alias_declaration_405_2_143()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3129_3_835()
 {
    if (jj_done) return true;
    if (jj_3R_file_declaration_1443_2_421()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3128_3_834()
 {
    if (jj_done) return true;
    if (jj_3R_variable_declaration_3338_1_420()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_body_3098_33_743()
 {
    if (jj_done) return true;
    if (jj_3R_designator_1109_2_659()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3127_3_833()
 {
    if (jj_done) return true;
    if (jj_3R_constant_declaration_1026_4_418()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3126_3_832()
 {
    if (jj_done) return true;
    if (jj_3R_subtype_declaration_3213_1_416()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3125_4_831()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_3085_1_664()) return true;
    return false;
  }

 inline bool jj_3R_generate_statement_1592_1_93()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    if (jj_3R_generate_scheme_1604_1_210()) return true;
    return false;
  }

 inline bool jj_3_59()
 {
    if (jj_done) return true;
    if (jj_3R_identifier_1657_3_82()) return true;
    if (jj_scan_token(COLON_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3124_2_830()
 {
    if (jj_done) return true;
    if (jj_3R_type_declaration_3264_1_415()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3123_1_829()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_declaration_3108_1_142()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declarative_item_3123_1_809()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_declarative_item_3123_1_829()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3124_2_830()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3125_4_831()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3126_3_832()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3127_3_833()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3128_3_834()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3129_3_835()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3130_3_836()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3132_1_837()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3134_3_838()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3135_3_839()) {
    jj_scanpos = xsp;
    if (jj_3_157()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declarative_item_3139_3_840()) return true;
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

 inline bool jj_3_58()
 {
    if (jj_done) return true;
    if (jj_3R_block_declarative_item_648_3_113()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_1_3117_2_550()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_body_3085_1_664()) return true;
    return false;
  }

 inline bool jj_3_155()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_instantiation_declaration_3202_2_170()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_1_3117_2_455()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_1_3117_2_550()) {
    jj_scanpos = xsp;
    if (jj_scan_token(139)) return true;
    }
    return false;
  }

 inline bool jj_3_154()
 {
    if (jj_done) return true;
    if (jj_3R_adding_operator_393_3_168()) return true;
    if (jj_3R_simpleTerm_3246_2_169()) return true;
    return false;
  }

 inline bool jj_3R_sequential_bock_statement_2954_159_795()
 {
    if (jj_done) return true;
    if (jj_3R_label_2022_2_84()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declaration_3111_1_309()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_specification_3160_2_454()) return true;
    if (jj_3R_subprogram_1_3117_2_455()) return true;
    return false;
  }

 inline bool jj_3R_signal_declaration_3022_89_519()
 {
    if (jj_done) return true;
    if (jj_scan_token(VARASSIGN_T)) return true;
    if (jj_3R_expression_1380_20_70()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_body_3098_11_742()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_kind_3149_2_794()) return true;
    return false;
  }

 inline bool jj_3R_subprogram_declaration_3108_1_142()
 {
    if (jj_done) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_declaration_3108_1_308()) {
    jj_scanpos = xsp;
    if (jj_3R_subprogram_declaration_3111_1_309()) return true;
    }
    return false;
  }

 inline bool jj_3R_subprogram_declaration_3108_1_308()
 {
    if (jj_done) return true;
    if (jj_3R_subprogram_instantiation_declaration_3202_2_170()) return true;
    return false;
  }

 inline bool jj_3R_simple_expression_3063_41_200()
 {
    if (jj_done) return true;
    if (jj_3R_adding_operator_393_3_168()) return true;
    if (jj_3R_simpleTerm_3246_2_169()) return true;
    return false;
  }

 inline bool jj_3R_signal_declaration_3022_68_518()
 {
    if (jj_done) return true;
    if (jj_3R_signal_kind_3033_3_638()) return true;
    return false;
  }

 inline bool jj_3R_gen_interface_list_1549_4_171()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_interface_list_1836_3_377()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_subprogram_body_3085_1_664()
 {
    if (jj_done) return true;
    if (jj_scan_token(IS_T)) return true;
    if (jj_3R_subprogram_declarative_part_3144_3_740()) return true;
    if (jj_scan_token(BEGIN_T)) return true;
    if (jj_3R_subprogram_statement_part_3197_3_741()) return true;
    if (jj_scan_token(END_T)) return true;
    Token * xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_body_3098_11_742()) jj_scanpos = xsp;
    xsp = jj_scanpos;
    if (jj_3R_subprogram_body_3098_33_743()) jj_scanpos = xsp;
    if (jj_scan_token(SEMI_T)) return true;
    return false;
  }

 inline bool jj_3R_gen_assoc_list_1544_4_172()
 {
    if (jj_done) return true;
    if (jj_scan_token(GENERIC_T)) return true;
    if (jj_scan_token(MAP_T)) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_association_list_553_1_205()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
    return false;
  }

 inline bool jj_3R_string_literal_3078_1_452()
 {
    if (jj_done) return true;
    if (jj_scan_token(STRINGLITERAL)) return true;
    return false;
  }

 inline bool jj_3R_sequential_bock_statement_2952_124_165()
 {
    if (jj_done) return true;
    if (jj_3R_label_2022_2_84()) return true;
    return false;
  }

 inline bool jj_3R_signature_3055_15_674()
 {
    if (jj_done) return true;
    if (jj_scan_token(COMMA_T)) return true;
    if (jj_3R_name_2142_2_73()) return true;
    return false;
  }

 inline bool jj_3R_function_call_1534_1_136()
 {
    if (jj_done) return true;
    if (jj_3R_name_2142_2_73()) return true;
    if (jj_scan_token(LPAREN_T)) return true;
    if (jj_3R_actual_parameter_part_371_4_297()) return true;
    if (jj_scan_token(RPAREN_T)) return true;
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
  JJCalls       jj_2_rtns[169];
  bool          jj_rescan;
  int           jj_gc;
  Token        *jj_scanpos, *jj_lastpos;
  int           jj_la;
  /** Whether we are looking ahead. */
  bool          jj_lookingAhead;
  bool          jj_semLA;
  int           jj_gen;
  int           jj_la1[388];
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
  VhdlSection param_sec = VhdlSection::UNKNOWN;
  VhdlSection parse_sec = VhdlSection::UNKNOWN;
  VhdlSpecifier currP = VhdlSpecifier::UNKNOWN;
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
