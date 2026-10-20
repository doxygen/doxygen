// objective: C# members of a type nested in a generic type
// check: class_outer-1-g.xml
// check: class_outer-1-g_1_1_inner.xml
/// A generic class
public class Outer<T>
{
    /// Field of the outer class
    public T OuterField;
    /// A nested class
    public class Inner
    {
        /// Field of the nested class
        public int InnerField;
        /// Method of the nested class
        public void InnerMethod() {}
    }
}
