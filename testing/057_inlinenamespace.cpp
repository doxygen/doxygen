// objective: test inline namespaces
// check: namespacelibrary.xml
// check: namespacelibrary_1_1v1.xml
// check: namespacelibrary_1_1v2.xml
namespace library
{
    namespace v1
    {
        class foo { public: void member(); };
        void func();
        namespace NS {}
    }
    inline namespace v2
    {
        class foo { public: void member(); };
        void func();
        namespace NS {}
    }
}
