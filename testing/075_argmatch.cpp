// objective: test argument matching in particular for unnamed types
// check: struct_foo.xml
/** @brief Foo */
struct Foo {
    void foo(float value);
    void foo(unsigned value);
    void foo(unsigned long);
    void foo(signed long);
    void foo(const struct Foo);
    void foo(const char * const, const double param2);
    void foo(void(*fptr)(int x,int));
};

/** @brief Float */
void Foo::foo(float) {}

/** @brief Unsigned int */
void Foo::foo(unsigned value) {}

/** @brief Unnamed unsigned long */
void Foo::foo(unsigned long) {}

/** @brief Unnamed signed long */
void Foo::foo(signed long) {}

/** @brief Unnamed struct foo */
void Foo::foo(const struct Foo) {}

/** @brief Unnamed const pointer */
void Foo::foo(const char * const, const double param2) {}

/** @brief Function pointer */
void Foo::foo(void(*fptr)(int y,int z)) {}
