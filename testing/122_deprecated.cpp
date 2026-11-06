// objective: test the [[deprecated]] attribute
// check: struct_old_config.xml
// check: struct_logger.xml
// check: deprecated.xml

//! Deprecating an entire struct
[[deprecated("Use NewConfig instead")]]
struct OldConfig {
    //! an x
    int x;
};

//! Deprecating a constructor
struct Logger {
    [[deprecated("Use Logger(int level) instead")]]
    Logger() {}

    /*! new logger constructor */
    Logger(int level) {}

    // ----- various ways to deprecate a function
    //
    // A. attribute present with documentation
    // B. attribute present without documentation
    // C. @deprecated present
    //
    // Different combinations of the above (A and B are mutually exclusive)
    // 1. A
    // 2. B
    // 3. C
    // 4. C + B
    // 5. B + C
    // 6. C + A
    // 7. A + C

    [[ nodiscard, deprecated ( "\e Use `write()` *instead*" ) ]]
    bool log_old1() {}

    [[deprecated]]
    void log_old2() {}

    /*! @deprecated Use `write()` instead */
    void log_old3() {}

    /*! @deprecated Use `write()` instead */
    [[deprecated]]
    void log_old4() {}

    [[deprecated]]
    /*! @deprecated Use `write()` instead */
    void log_old5() {}

    /*! @deprecated Use `write()` instead */
    [[deprecated("Second deprecation message")]]
    void log_old6() {}

    [[deprecated("Use `write()` instead")]]
    /*! @deprecated Second deprecation message */
    void log_old7() {}

    // ------

    /*! new function */
    void write() {}
};
