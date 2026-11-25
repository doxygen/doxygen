// objective: test the \include and \includelineno commands
// check: indexpage.xml
// config: STRIP_CODE_COMMENTS=NO

/** \mainpage
 *  Some text.
 *  \include{godbolt} example_test.cpp
 *  More text.
 *  \include{lineno,godbolt=(link;simple_godbolt)} example_test.cpp
 *  Here is the same example with stripped comments
 *  \include{godbolt=(link;simple_godbolt),lineno,strip} example_test.cpp
 *  End.
 */
