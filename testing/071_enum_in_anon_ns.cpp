// objective: test that enum values in anonymous namespaces produce no warning
// check: namespace_a_namespace_1_1_0D0.xml

namespace ANamespace { namespace {

enum class Boolean {
    False,
    True,
    FileNotFound
};

}}
