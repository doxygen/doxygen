// objective: test the \include and \includelineno commands
// check: indexpage.xml
// config: STRIP_CODE_COMMENTS=NO

/** \mainpage
 *  Some text.
 *  \include example_test.cpp
 *  More text.
 *  \include{lineno} example_test.cpp
 *  Same example with stipped comments
 *  \include{lineno,strip} example_test.cpp
 *  End.
 */
