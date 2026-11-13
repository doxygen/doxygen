// objective: C# ref struct
// check: struct_parser.xml
// check: struct_plain.xml
/// A ref struct
public ref struct Parser
{
    /// A field
    public int Position;
}
/// A plain struct
public struct Plain
{
    /// A field
    public int Position;
}
