// objective: test use of define inside enums
// config: EXTRACT_ALL=YES
// config: MACRO_EXPANSION=YES
// check: 107__define__in__enums_8cpp.xml

enum A {
#define ID_A(X) X,
  ID_A(A1)
  ID_A(A2)
};

enum class B {
#define ID_B(X) X,
  ID_B(B1)
  ID_B(B2)
};
