// objective: test if #include inside a class body will include the results as-is
// check: class_test.xml

/// Test clasx
class Test
{
#include "members.ci"
};
