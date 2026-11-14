// objective: template and normal parameter names (in the documentation and in signatures) must not link to classes or namespaces with the same name
// check: 151__template__parameter__links_8cpp.xml
// check: class_holder.xml
// config: WARN_IF_UNDOCUMENTED = NO

/// A class with the same name as a template parameter.
class cls1 {};

/// A class with the same name as a function parameter.
class cls2 {};

/// A namespace with the same name as a parameter.
namespace nsp {}

/// A class used as an exception and in descriptions.
class Error {};

/// A function.
/// \tparam cls1 the template parameter; see Error
/// \param cl1 the first parameter
/// \param cls2 the second parameter
/// \param nsp the third parameter
/// \exception Error an exception
/// \retval Error a named result
template <typename cls1>
int fie1(const cls1 cl1, int cls2, int nsp);

/// A class template.
/// \tparam cls1 the template parameter
template <typename cls1>
class Holder
{
  public:
    /// A member using the class template parameter.
    /// \param a the first parameter
    /// \param nsp the second parameter
    void mem(cls1 a, int nsp);
    /// A member template.
    /// \tparam cls2 the member template parameter
    /// \param a the first parameter
    /// \param b the second parameter
    template <class cls2> cls1 mem2(cls1 a, cls2 b);
    /// A variable of the template parameter type.
    cls1 var;
};

/// A function with a non-type, a template template and a namespace named template parameter.
/// \tparam nsp the template parameter
/// \tparam cls2 the non-type parameter
/// \param x the parameter
template <typename nsp, int cls2>
void fie3(nsp x);

/// <summary>Another function.</summary>
/// <typeparam name="cls1">The template parameter.</typeparam>
/// <param name="cls2">The parameter.</param>
/// <exception cref="Error">An exception.</exception>
template <typename cls1>
void fie2(int cls2);
