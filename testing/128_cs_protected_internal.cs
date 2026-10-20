// objective: C# protected internal members are protected
// check: class_c.xml
// config: EXTRACT_PACKAGE = YES
/// A class
public class C
{
    /// Protected internal field
    protected internal int ProtectedInternal;
    /// Internal protected field
    internal protected int InternalProtected;
    /// Internal field
    internal int Internal;
}
