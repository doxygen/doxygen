// objective: test inheritance in C++ over namespaces
// check: class_m1_1_1_t1.xml
// check: class_m1_1_1_t2.xml
// check: class_m1_1_1_t3.xml
// check: class_m2_1_1_t3.xml
// check: namespace_m1.xml
// check: namespace_m2.xml

namespace M1
{
  class T1
  {
  };

  class T2: public T1
  {
  };

  class T3: public T2
  {
  };
}

namespace M2
{
  using M1::T2;

  class T3: public T2
  {
  };
}
