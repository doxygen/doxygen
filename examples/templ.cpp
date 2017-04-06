
/*! A template class */
template<class T,int i=100> class Test
{
  public:
    Test();
    Test(const Test &);
};

/*! complete specialization */
template<> class Test<void *,200>
{
  public:
    Test();
};

/*! A partial template specialization */
template<class T> class Test<T *> : public Test<void *,200>
{
  public:
    Test();
};

/*! The constructor of the template class*/
template<class T,int i> Test<T,i>::Test() {}

/*! The copy constructor */
template<class T,int i> Test<T,i>::Test(const Test &t) {}

/*! The constructor of the partial specialization */
template<class T> Test<T *>::Test() {}

/*! The constructor of the specialization */
template<> Test<void *,200>::Test() {}

