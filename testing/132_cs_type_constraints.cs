// objective: C# generic type constraints
// check: class_box-2-g.xml
/// A generic class
public class Box<T, U> where T : class, new() where U : System.IComparable<U>
{
}
