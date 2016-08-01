// objective: test inheritance over namespace works the same in C++ and Fortran (see test 095)
// check: structm1_1_1t1.xml
// check: structm1_1_1t2.xml
// check: structm1_1_1t3.xml
// check: structm2_1_1t3.xml
// check: structm3_1_1t4.xml
// check: namespacem1.xml
// check: namespacem2.xml
// check: namespacem3.xml

namespace m1
{
  struct t1
  {
  };

  struct t2: public t1
  {
  };

  struct t3: public t2
  {
  };
}

namespace m2
{
  using namespace m1;

  struct t3: public t2
  {
  };
}

namespace m3
{
  using m2::t1;

  struct t4: public t1
  {
  };
}
