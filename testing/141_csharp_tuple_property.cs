// objective: C# tuple properties are parsed and documented
// check: class_tuple_property.xml
// check: class_derived_tuple_property.xml
// check: class_static_tuple_property.xml

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

/// <summary>A class with a tuple property.</summary>
public class StaticTupleProperty
{
  /// <summary>The static tuple property documentation.</summary>
  public static (int, int) Tuple
  {
    get { return (0, 0); }
  }
}
