// objective: test \ref command with a variadic template function
// check: 070__ref__variadic__template_8cpp.xml

/** \file
 *
 *  @attention
 *  @parblock
 *  At the time of writing, the part between \<\> is totally ignored:
 *  %func<Args...>(Args... args) is interpreted as %func(Args... args).
 *
 *  Beware that a function parameter with either a \& or \* operator,
 *  e.g. 'const Args&... args', requires \\link and \\ref to specify
 *  such parameter as verbatim, i.e. 'const Args&... args'.  At the
 *  time of writing, the form %func(const Args&...) will fail, unless
 *  the function parameter was declared just as 'const Args&...'.
 *  @endparblock
 *
 *  \ref Test::func(int,Args...)const "variadic template method"
 *
 *  References to the variadic template function overloads:
 *    @li \ref func(int,Args&... args) "First overload"
 *    @li \ref func(int,Args&&... args) "Second overload"
 *    @li \ref func(int,const Args&... args) "Third overload"
 *    @li \ref func(int,const Args&&... args) "Fourth overload"
 *    @li \ref func(int,Args*... args) "Fifth overload"
 *    @li \ref func(int,Args**... args) "Sixth overload"
 *    @li \ref func(int,const Args*... args) "Seventh overload"
 *    @li \ref func(int,const Args**... args) "Eighth overload"
 *    @li \ref func(int,Args...) "Ninth overload"
 *
 *  The following are interpreted the same:
 *    @li \ref func(int,const Args&... args) "without template argument"
 *    @li \ref func<Args...>(int,const Args&... args) "with template argument"
 *
 *  See the \ref Test "test" class.
 */

/** A function
 */
void func(int p);

/** A variadic template function overload
 */
template <typename... Args>
void func(int p, Args&... args);

/** A variadic template function overload
 */
template <typename... Args>
void func(int p, Args&&... args);

/** A variadic template function overload
 */
template <typename... Args>
void func(int p, const Args&... args);

/** A variadic template function overload
 */
template <typename... Args>
void func(int p, const Args&&... args);

/** A variadic template function overload
 */
template <typename... Args>
void func(int p, Args*... args);

/** A variadic template function overload
 */
template <typename... Args>
void func(int p, Args**... args);

/** A variadic template function overload
 */
template <typename... Args>
void func(int p, const Args*... args);

/** A variadic template function overload
 */
template <typename... Args>
void func(int p, const Args**... args);

/** A variadic template function overload
 */
template <typename... Args>
void func(int p, Args... args);

/** A test */
class Test
{
 public:
  /** A variadic template method
   */
  template <typename... Args>
  void func(int p, Args... args) const;
};
