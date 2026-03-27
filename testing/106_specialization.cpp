// objective: test implicit and explicit template specializations
// config: EXTRACT_ALL=YES
// check: index.xml
// check: indexpage.xml
// check: class_container_3_01_elem5_01_4.xml
// check: struct_container_3_01_elem5_01_4_1_1_nested.xml
template<class T> class Container { };
class Elem1 {};
class Elem2 {};
class Elem3 {};
class Elem4 {};
class Elem5 {};

class Derived : public Container<Elem1>
{
  public:
    typedef Container<Elem2> Type;
    void func(const Container<Elem3> &c);
    Container<Elem4> var;
};

void Derived::func(const Container<Elem3> &c) {}

template<>
class Container<Elem5>
{
  public:
    void specialization()
    {
      int a;
    }
    struct Nested {};
};

/** @mainpage
 *  See Container<Elem5>::Nested for more info.
 *  See Container<Elem5>::specialization() for more info.
 */
