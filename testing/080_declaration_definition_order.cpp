// objective: test that declaration/definition location is parsed properly independently of order in which they are seen
// check: namespace_foo.xml
// check: struct_foo_1_1_struct.xml

/* the code below should be in 080_declaration_definition_order.cpp but I
   haven't found a way to have two input files for a test, so this has to
   suffice */

namespace Foo {

void foo() {}
Struct::foo() {}
void bar();

int Struct::Var = 42;
int Var = 1337;
extern int War;

}

/* the code below should be in 080_declaration_definition_order.h but I
   haven't found a way to have two input files for a test, so this has to
   suffice */

/** @brief A namespace */
namespace Foo {

/**
 * A foo, declared at 080_declaration_definition_order.cpp:32 and defined at
 * 080_declaration_definition_order.cpp:11
 */
void foo();

/**
 * A bar, declared at 080_declaration_definition_order.cpp:13 and defined at
 * 080_declaration_definition_order.cpp:38
 */
inline void bar() {}

/**
 * A var, declared at 080_declaration_definition_order.cpp:44 and defined at
 * 080_declaration_definition_order.cpp:16
 */
extern int Var;

/**
 * A war, declared at 080_declaration_definition_order.cpp:17 and defined at
 * 080_declaration_definition_order.cpp:50
 */
int War = 0;

/** @brief A struct */
struct Struct {
  /**
   * A foo, declared at 080_declaration_definition_order.cpp:58 and defined at
   * 080_declaration_definition_order.cpp:12
   */
  void foo();

  /**
   * A var, declared at 080_declaration_definition_order.cpp:64 and defined at
   * 080_declaration_definition_order.cpp:15
   */
  static int Var;
};

}
