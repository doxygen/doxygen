// objective: test use of define inside enums
// config: EXTRACT_ALL=YES
// config: MACRO_EXPANSION=YES
// check: 107__define__in__enums_8cpp.xml

enum A {
#define ID(X) X,
  ID(A1)
  ID(A2)
};

enum class B {
#define ID(X) X,
  ID(B1)
  ID(B2)
};
