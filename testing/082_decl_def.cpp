// objective: test for declaration and definition order independence: decl first
// check: namespace_n.xml
// config: INPUT = $INPUTDIR/decl_def.h $INPUTDIR/082_decl_def.cpp
#include "test.h"

/** Namespace */
namespace N
{

/** Detailed docs. */
int var;

/** Detailed docs. */
void foo(int param)
{
}

}
