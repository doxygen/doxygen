// objective: C# init accessors
// check: class_c.xml
/// A class
public class C
{
    private int initialValue;
    private int offset;
    /// \brief Property with an init accessor.
    ///
    /// The detailed description shows the init accessor as a label.
    public int Init { get; init; }
    /// Read-only property that reads a field named like an accessor
    public int Initial { get => initialValue; }
    /// Read-only property that reads a field that contains "set"
    public int Offset { get => offset; }
}
