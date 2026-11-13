// objective: test macro argument expansion with stringification and token pasting
// config: EXTRACT_ALL=YES
// config: MACRO_EXPANSION=YES
// config: PREDEFINED=__USER_LABEL_PREFIX__=_
// check: 147__macro__argument__expansion_8cpp.xml

#define __REDIRECT(name, proto, alias) name proto __asm__ (__ASMNAME (#alias))
#define __ASMNAME(cname) __ASMNAME2 (__USER_LABEL_PREFIX__, cname)
#define __ASMNAME2(prefix, cname) __STRING (prefix) cname
#define __STRING(x) #x

int __REDIRECT (redirected, (void), target);
const char *nested = __ASMNAME("target");
const char *direct = __ASMNAME2 (__USER_LABEL_PREFIX__, "harumph");

#undef __USER_LABEL_PREFIX__
const char *undefined_prefix = __ASMNAME2 (__USER_LABEL_PREFIX__, "harumph");
#define __USER_LABEL_PREFIX__ _
const char *source_prefix = __ASMNAME2 (__USER_LABEL_PREFIX__, "harumph");

#define VALUE expanded_identifier
#define STRING(x) #x
#define STRING_EXPANDED(x) STRING(x)
#define MIX(x) #x, x
#define PASTE(x, y) x##y
#define PASTE_EXPANDED(x, y) PASTE(x, y)
#define STRING_ARGS(...) #__VA_ARGS__
#define STRING_ARGS_EXPANDED(...) STRING_ARGS(__VA_ARGS__)
#define EMPTY
#define OPTIONAL(...) "before" __VA_OPT__("present") "after"
#define SELF SELF

const char *expanded_identifier = "value";
const char *raw = STRING(VALUE);
const char *expanded = STRING_EXPANDED(VALUE);
const char *mixed[] = { MIX(VALUE) };
int PASTE(VALUE, _suffix) = 1;
int PASTE_EXPANDED(VALUE, _suffix) = 2;
const char *variadic_raw = STRING_ARGS(VALUE, tail);
const char *variadic_expanded = STRING_ARGS_EXPANDED(VALUE, tail);
const char *quoted = STRING("a\\b");
const char *quoted_at = STRING("a@b");
const char *optional_empty = OPTIONAL(EMPTY);
const char *optional_present = OPTIONAL(VALUE);
const char *selfref = STRING_EXPANDED(SELF);
