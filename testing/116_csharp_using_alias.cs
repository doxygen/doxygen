// objective: check C# using alias
// config: EXTRACT_ALL=YES
// check: class_test_app_1_1_test_class.xml

namespace N {
  public class A {}
  public class B {}
  public class C {}

  namespace Collections {
    public class Dictionary<TKey, TValue> {}
    public class List<T> {}
  }
}

namespace M {
  public class A {}
}

using Foo = N.A;
using Bar = N.Collections.Dictionary<string, N.Collections.List<N.A>>;

using GlobalFoo = global::N.B;
using GlobalBar = N.Collections.Dictionary<string, N.Collections.List<N.B>>;

namespace TestApp
{
    public class Outer {
        public class Nested {}
    }
}

namespace TestApp
{
    using InnerFoo = N.C;
    using InnerBar = N.Collections.Dictionary<string, N.Collections.List<N.C>>;

    using NestedAlias = TestApp.Outer.Nested;

    public class TestClass : Bar
    {
        public Foo myFoo;
        public Bar myBar;

        public InnerFoo myInnerFoo;
        public InnerBar myInnerBar;

        public GlobalFoo myGlobalFoo;
        public GlobalBar myGlobalBar;

        public NestedAlias myNestedAlias;

        public Foo fooMethod(Bar b) { return null; }
        public InnerFoo innerFooMethod(InnerBar b) { return null; }
        public GlobalFoo globalFooMethod(GlobalBar b) { return null; }
    }
}

using N;
using A = M.A;

interface I : A {}
