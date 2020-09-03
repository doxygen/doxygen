// objective: Test termination of brief description with lists
// check: 081__brief__lists_8h.xml
/// \file

/// @brief Just with minus
/// - Item 1
/// - Item 2
void c_subr1(void);

/// @brief With minus and hash
/// -# Item 3
/// -# Item 4
void c_subr2(void);

/// @brief With numbers
/// 1. Item 5
/// 2. Item 6
void c_subr3(void);

/// @brief With asterisk
/// * Item 7
/// * Item 8
void c_subr4(void);
