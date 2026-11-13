// objective: test that members of an anonymous union in a class inside an inline namespace are not duplicated
// check: classfmt_1_1v12_1_1basic__format__args.xml
// config: EXTRACT_PRIVATE = YES
// config: EXTRACT_ALL = YES
// config: MACRO_EXPANSION = YES
// config: WARN_IF_UNDOCUMENTED = NO

namespace fmt {
  inline namespace v12 {
    class basic_format_args {
      private:
        union {
          const int values_;
          const int args_;
        };
    };
  }
}
