// objective: test that enum values in anonymous namespaces produce no warning
// check: namespace_a_namespace_1_1_0d130315250316227335362355006121037335073327177063.xml

namespace ANamespace { namespace {

enum class Boolean {
    False,
    True,
    FileNotFound
};

}}
