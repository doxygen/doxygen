// objective: test macro expansion with deferred parentheses and nested calls
// config: EXTRACT_ALL=YES
// config: MACRO_EXPANSION=YES
// check: 149__macro__rescan_8cpp.xml
// check: structmy__class.xml
// check: structattribute__class.xml
// check-preprocessor: ^[0-9]+ +constexpr\s+static\s+auto\s+__static_transform\s*\(\s*Self\s*&&\s*self\s*,
// check-preprocessor: ^[0-9]+ +UnaryOp\s*&&\s*unary_op\s*\)
// check-preprocessor: ^[0-9]+ +RESCAN_DEFERRED:\s*\[\s*value\s*\]
// check-preprocessor: ^[0-9]+ +RESCAN_BOUNDARY:\s*\[\s*7\s*\]
// check-preprocessor: ^[0-9]+ +RESCAN_NESTED:\s*\[\s*\[\s*9\s*\]\s*\]
// check-preprocessor: ^[0-9]+ +RESCAN_ARGUMENT_BOUNDARY:\s*\[\s*11\s*\]
// check-preprocessor: ^[0-9]+ +RESCAN_NOT_CALLED:\s*RESCAN_CALL\s+unchanged
// check-preprocessor: ^[0-9]+ +RESCAN_OPERATOR:\s*__static__call
// check-preprocessor: ^[0-9]+ +template\s*<class\.\.\.\s*_Ts>\s*\[\[always_inline\]\]\s*\[\[host\]\]\s*\[\[device\]\].*constexpr\s+auto\s+transform.*template\s*<class\.\.\.\s*_Ts>\s*\[\[always_inline\]\]\s*\[\[host\]\]\s*\[\[device\]\].*constexpr\s+auto\s+transform.*template\s*<class\.\.\.\s*_Ts>\s*\[\[always_inline\]\]\s*\[\[host\]\]\s*\[\[device\]\].*constexpr\s+auto\s+transform
// check-preprocessor: ^[0-9]+ +\[\[first\]\]\s*\[\[second\]\]\s*\[\[third\]\]\s*\[\[fourth\]\]\s*\[\[fifth\]\]\s*int\s+attribute_test
// check-preprocessor: ^[0-9]+ +RESCAN_RECURSIVE:\s*RESCAN_RECURSE\s*\(\s*3\s*\)
// check-preprocessor: ^[0-9]+ +RESCAN_MUTUAL:\s*RESCAN_MUTUAL_A\s*\(\s*4\s*\)

#define STDEXEC_PP_LPAREN() (
#define STDEXEC_PP_RPAREN() )
#define STDEXEC_PP_PARENS() ()
#define STDEXEC_PP_COMMA() ,
#define STDEXEC_PP_CAT_I(_XP, ...)   _XP##__VA_ARGS__
#define STDEXEC_PP_CAT(_XP, ...)     STDEXEC_PP_CAT_I(_XP, __VA_ARGS__)
#define STDEXEC_PP_EXPAND(...)       __VA_ARGS__
#define STDEXEC_PP_EVAL(_MACRO, ...) _MACRO(__VA_ARGS__)
#define STDEXEC_PP_IIF_0(_YP, ...)       __VA_ARGS__
#define STDEXEC_PP_IIF_1(_YP, ...)       _YP
#define STDEXEC_PP_IIF_EVAL(_MACRO, ...) _MACRO(__VA_ARGS__)
#define STDEXEC_PP_IIF_CAT(_XP, ...)     _XP##__VA_ARGS__
#define STDEXEC_PP_IIF(_XP, _YP, ...) \
  STDEXEC_PP_IIF_EVAL(STDEXEC_PP_IIF_CAT(STDEXEC_PP_IIF_, _XP), _YP, __VA_ARGS__)
#define STDEXEC_PP_CHECK_EXPAND(...)      __VA_ARGS__
#define STDEXEC_PP_CHECK_I(_XP, _NP, ...) _NP
#define STDEXEC_PP_CHECK(...)             STDEXEC_PP_CHECK_EXPAND(STDEXEC_PP_CHECK_I(__VA_ARGS__, 0, ))
#define STDEXEC_PP_PROBE_I(_XP, _NP, ...) _XP, _NP,
#define STDEXEC_PP_PROBE(...)             STDEXEC_PP_PROBE_I(__VA_ARGS__, 1)
#define STDEXEC_PP_FRONT_I(_A1, ...) _A1
#define STDEXEC_PP_FRONT(...) __VA_OPT__(STDEXEC_PP_FRONT_I(__VA_ARGS__))
#define STDEXEC_PP_CASE_LABEL_(_PREFIX, ...) \
  STDEXEC_PP_EVAL(STDEXEC_PP_CHECK, \
                  STDEXEC_PP_CAT(_PREFIX##_SWITCH_, STDEXEC_PP_FRONT(__VA_ARGS__)), \
                  DEFAULT(STDEXEC_PP_FRONT(__VA_ARGS__)), )
#define STDEXEC_PP_CASE(_ARG) STDEXEC_PP_PROBE(~, _ARG)
#define STDEXEC_PP_SWITCH(_PREFIX, ...) \
  STDEXEC_PP_CAT(_PREFIX##_CASE_, STDEXEC_PP_CASE_LABEL_(_PREFIX, __VA_ARGS__))
#define STDEXEC_PP_EXPAND_R3(...) \
  STDEXEC_PP_EXPAND(STDEXEC_PP_EXPAND(STDEXEC_PP_EXPAND(STDEXEC_PP_EXPAND(__VA_ARGS__))))
#define STDEXEC_PP_EXPAND_R2(...) \
  STDEXEC_PP_EXPAND_R3(STDEXEC_PP_EXPAND_R3(STDEXEC_PP_EXPAND_R3(STDEXEC_PP_EXPAND_R3(__VA_ARGS__))))
#define STDEXEC_PP_EXPAND_R1(...) \
  STDEXEC_PP_EXPAND_R2(STDEXEC_PP_EXPAND_R2(STDEXEC_PP_EXPAND_R2(STDEXEC_PP_EXPAND_R2(__VA_ARGS__))))
#define STDEXEC_PP_EXPAND_R(...) \
  STDEXEC_PP_EXPAND_R1(STDEXEC_PP_EXPAND_R1(STDEXEC_PP_EXPAND_R1(STDEXEC_PP_EXPAND_R1(__VA_ARGS__))))
#define STDEXEC_PP_FOR_EACH_AGAIN() STDEXEC_PP_FOR_EACH_HELPER
#define STDEXEC_PP_FOR_EACH_HELPER(_MACRO, _A1, ...) \
  _MACRO(_A1) __VA_OPT__(STDEXEC_PP_FOR_EACH_AGAIN STDEXEC_PP_PARENS()(_MACRO, __VA_ARGS__)) /**/
#define STDEXEC_PP_FOR_EACH(_MACRO, ...) \
  __VA_OPT__(STDEXEC_PP_EXPAND_R(STDEXEC_PP_FOR_EACH_HELPER(_MACRO, __VA_ARGS__)))
#define STDEXEC_ATTRIBUTE_CASE_DEFAULT(...) [[__VA_ARGS__]]
#define STDEXEC_ATTRIBUTE_I(_ATTR) STDEXEC_PP_SWITCH(STDEXEC_ATTRIBUTE, _ATTR)
#define STDEXEC_ATTRIBUTE(...) STDEXEC_PP_FOR_EACH(STDEXEC_ATTRIBUTE_I, __VA_ARGS__)

#define STDEXEC_EXPLICIT_THIS_CALL_OPERATOR_PROBE_operator() STDEXEC_PP_PROBE(~, 1)
#define STDEXEC_EXPLICIT_THIS_CALL_OPERATOR_PROBE(_NAME) \
    STDEXEC_PP_CHECK(STDEXEC_PP_CAT(STDEXEC_EXPLICIT_THIS_CALL_OPERATOR_PROBE_, _NAME))
#define STDEXEC_EXPLICIT_THIS_MANGLE(_NAME) \
    STDEXEC_PP_CAT(__static_, \
                   STDEXEC_PP_IIF(STDEXEC_EXPLICIT_THIS_CALL_OPERATOR_PROBE(_NAME), \
                                  _call, \
                                  _NAME))
#define STDEXEC_EXPLICIT_THIS_EAT_this
#define STDEXEC_EXPLICIT_THIS_MANGLE_auto auto STDEXEC_EXPLICIT_THIS_MANGLE STDEXEC_PP_LPAREN()
#define STDEXEC_EXPLICIT_THIS_ARGS(...) \
    STDEXEC_PP_CAT(STDEXEC_EXPLICIT_THIS_EAT_, __VA_ARGS__) STDEXEC_PP_RPAREN()
#define STDEXEC_EXPLICIT_THIS_BEGIN(...) \
    static STDEXEC_PP_EXPAND(STDEXEC_PP_CAT(STDEXEC_EXPLICIT_THIS_MANGLE_, __VA_ARGS__) \
                             STDEXEC_PP_RPAREN()) STDEXEC_PP_LPAREN() STDEXEC_EXPLICIT_THIS_ARGS
#define STDEXEC_AUTO_RETURN(...) \
  noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__) { return __VA_ARGS__; }
#define STDEXEC_EXPLICIT_THIS_END(_NAME) \
  template <class... _Ts> \
  STDEXEC_ATTRIBUTE(always_inline, host, device) \
  constexpr auto _NAME(_Ts&&... __args) && STDEXEC_AUTO_RETURN \
    (STDEXEC_EXPLICIT_THIS_MANGLE(_NAME)(std::move(*this), static_cast<_Ts&&>(__args)...)) \
  template <class... _Ts> \
  STDEXEC_ATTRIBUTE(always_inline, host, device) \
  constexpr auto _NAME(_Ts&&... __args) & STDEXEC_AUTO_RETURN \
    (STDEXEC_EXPLICIT_THIS_MANGLE(_NAME)(*this, static_cast<_Ts&&>(__args)...)) \
  template <class... _Ts> \
  STDEXEC_ATTRIBUTE(always_inline, host, device) \
  constexpr auto _NAME(_Ts&&... __args) const & STDEXEC_AUTO_RETURN \
    (STDEXEC_EXPLICIT_THIS_MANGLE(_NAME)(*this, static_cast<_Ts&&>(__args)...))

template <typename T>
struct my_class {
    T m_value;
    template <typename Self, typename UnaryOp>
    constexpr STDEXEC_EXPLICIT_THIS_BEGIN(auto transform)(
        this Self&& self,
        UnaryOp&& unary_op) {
            using new_my_class_t = my_class<std::remove_cvref_t<
            std::invoke_result_t<UnaryOp, decltype(stdexec::__forward_like<Self>(self.m_value))>
        >>;
        return new_my_class_t{
            std::forward<UnaryOp>(unary_op)(stdexec::__forward_like<Self>(self).m_value)};
    }
    STDEXEC_EXPLICIT_THIS_END(transform)
};

template <typename T>
struct attribute_class {
    STDEXEC_ATTRIBUTE(first, second, third, fourth, fifth) int attribute_test;
};

#define RESCAN_LPAREN() (
#define RESCAN_RPAREN() )
#define RESCAN_CALL(x) [x]
#define RESCAN_ALIAS RESCAN_CALL
#define RESCAN_DEFER() RESCAN_CALL RESCAN_LPAREN()
#define RESCAN_EXPAND(...) __VA_ARGS__
#define RESCAN_RECURSE(x) RESCAN_RECURSE /* comment */ (x)
#define RESCAN_MUTUAL_A(x) RESCAN_MUTUAL_B(x)
#define RESCAN_MUTUAL_B(x) RESCAN_MUTUAL_A(x)

RESCAN_DEFERRED: RESCAN_EXPAND(RESCAN_DEFER() value RESCAN_RPAREN())
RESCAN_BOUNDARY: RESCAN_ALIAS(7)
RESCAN_NESTED: RESCAN_CALL(RESCAN_CALL(9))
RESCAN_ARGUMENT_BOUNDARY: RESCAN_EXPAND(RESCAN_CALL)(11)
RESCAN_NOT_CALLED: RESCAN_EXPAND(RESCAN_CALL) unchanged
RESCAN_OPERATOR: STDEXEC_EXPLICIT_THIS_MANGLE(operator())
RESCAN_RECURSIVE: RESCAN_RECURSE(3)
RESCAN_MUTUAL: RESCAN_MUTUAL_A(4)
