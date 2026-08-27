// objective: test that function pointers are split correctly into type and args
// check: 123__function__pointer_8c.xml
// check: struct_my_iface.xml

/** \file */

/** \brief A function pointer typedef. */
typedef int (*MyCallback)(int a, int b);

/** \brief A function pointer array typedef. */
typedef int (*MyCallbackArray[10])(int a, int b);

/** \brief A struct with a function pointer member. */
struct MyIface
{
  /** \brief a member function pointer */
  int (*do_it)(struct MyIface *self, int x);
};
