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

    [[ nodiscard, deprecated ( "Use write() instead" ) ]]
    void log() {}

    /*! @deprecated Use write() instead */
    [[deprecated]]
    void log_old() {}

    /*! new function */
    void write() {}
};
