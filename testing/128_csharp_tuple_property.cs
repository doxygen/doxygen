// objective: C# tuple properties are parsed and documented
// check: class_tuple_property.xml

/// <summary>A class containing a tuple property.</summary>
public class TupleProperty
{
  /// <summary>The tuple property documentation.</summary>
  public (int, int) Tuple { get; }
}
