// objective: test the \include and \includelineno commands
// check: indexpage.xml
// check: another_page.xml
// config: STRIP_CODE_COMMENTS=NO

/** \mainpage
 *  Some text.
 *  \include example_test.cpp
 *  More text.
 *  \include{lineno} example_test.cpp
 */

/** \page another_page Example with stipped comments
 *  Here is the same example with stipped comments
 *  \include{lineno,strip} example_test.cpp
 *  End.
 */
