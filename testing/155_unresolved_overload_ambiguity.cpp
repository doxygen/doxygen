// objective: match uniquely identifiable unresolved argument types but not ambiguous or different types
// check: class_overloads.xml
// check: class_ambiguous.xml
// check: class_resolved.xml
// check: class_qualified.xml
// config: EXTRACT_ALL = YES
// config: WARNINGS = NO

namespace clang
{
class Type;
class Decl;
}

class Overloads
{
public:
  Overloads(int, bool);
  Overloads(int, const clang::Type &);
  Overloads(int, const clang::Decl *);
};

/// Documentation for the Type overload.
Overloads::Overloads(int, const Type &tag) {}

/// Documentation for the Decl overload.
Overloads::Overloads(int, const Decl *decl) {}

namespace A { class Choice; class LocalType; }
namespace B { class Choice; class LocalType; }
class LocalType {};

class Ambiguous
{
public:
  void f(const A::Choice &);
  void f(const B::Choice &);
};

/// This must not be assigned to either overload.
void Ambiguous::f(const Choice &) {}

class Resolved
{
public:
  void f(const A::LocalType &);
  void f(int);
};

/// The global LocalType is not A::LocalType.
void Resolved::f(const LocalType &) {}

class Qualified
{
public:
  void f(const A::LocalType &);
  void f(int);
};

/// B::LocalType is not A::LocalType.
void Qualified::f(const B::LocalType &) {}
