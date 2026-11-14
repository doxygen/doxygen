// objective: inherit documentation for an override whose argument names the base class without its namespace, but not for a class or a specialization that has the name of its base class (see issue #12367)
// check: class_b_1_1_base.xml
// check: class_b_1_1_derived.xml
// check: class_c_1_1_base_3_01int_01_4.xml
// config: INHERIT_DOCS = YES
// config: EXTRACT_ALL = YES

namespace A
{
/// Base class.
class Base
{
  public:
    /// Documented in base class.
    virtual void attach(Base *other) = 0;
};
}

namespace B
{
/// Class with the name of its base class: inside it, Base names this class.
class Base : public A::Base
{
  public:
    void attach(Base *other);
};

/// Derived class in another namespace, with an undocumented override.
class Derived : public A::Base
{
  public:
    void attach(Base *other) override;
};
}

namespace C
{
/// Primary template with the name of A::Base.
template<class T> class Base;

/// Specialization with the name of its base class: inside it, Base names this specialization.
template<> class Base<int> : public A::Base
{
  public:
    void attach(Base *other);
};
}
