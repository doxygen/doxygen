// objective: test inline namespaces
// check: namespacelibrary.xml
// check: namespacelibrary_1_1v1.xml
// check: namespacelibrary_1_1v2.xml

/// the main namespace
namespace library
{
    /// the first namespace
    namespace v1
    {
        /// the class
        class foo {
          public:
            /// member of the class
            void member();
        };
        /// a method
        void func();
        /// a namespace
        namespace NS {}
    }
    /// the inline namespace
    inline namespace v2
    {
        /// the class
        class foo {
          public:
            /// member of the class
            void member();
        };
        /// a method
        void func();
        /// a namespace
        namespace NS {}
    }
}
