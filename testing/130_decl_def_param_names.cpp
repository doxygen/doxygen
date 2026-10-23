// objective: test that the documentation of a function definition keeps its own parameter names when the declaration uses other names, see issue #12120
// input: more_130_b.h
// config: JAVADOC_AUTOBRIEF = YES
// check: 130__decl__def__param__names_8cpp.xml
/** @file */

#include "more_130_b.h"

/**
 * Test func.
 * @param a a param.
 * @param b another param.
 */
void test1(int a, int b)
{
    ++a;
}
