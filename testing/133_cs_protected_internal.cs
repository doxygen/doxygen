// objective: C# protected internal members are protected and private protected members are package
// check: class_c.xml
// config: EXTRACT_PACKAGE = YES
// config: EXTRACT_PRIVATE = YES
/// A class
public class C
{
    /// Protected internal field
    protected internal int ProtectedInternal;
    /// Internal protected field
    internal protected int InternalProtected;
    /// Internal field
    internal int Internal;
    /// Private protected field
    private protected int PrivateProtected;
    /// Protected private field
    protected private int ProtectedPrivate;
    /// Protected field
    protected int Protected;
    /// Private field
    private int Private;
}
