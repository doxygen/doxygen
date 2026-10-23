// objective: C# tuple properties are parsed and documented
// check: class_tuple_property.xml
// check: class_derived_tuple_property.xml

/// <summary>A class containing a tuple property.</summary>
public class TupleProperty
{
  /// <summary>The tuple property documentation.</summary>
  public (int, int) Tuple { get; }
}

/// <summary>A class hiding a tuple property.</summary>
public class DerivedTupleProperty : TupleProperty
{
  /// <summary>The hidden tuple property documentation.</summary>
  public new (int, int) Tuple
  {
    get { return base.Tuple; }
  }
}
