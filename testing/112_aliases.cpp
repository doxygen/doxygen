// objective: test multi-line alias expansion in different comment styles
// config: HAVE_DOT = YES
// config: ALIASES += "long_note{1}=@note @parblock \1 @endparblock"
// check: 112__aliases_8cpp.xml
/// @file

    /// Embedded list
    ///
    /// * item1
    /// * item2
    ///
    /// Verbatim section includes slashes
    /// \verbatim
    /// * item1
    /// * item2
    /// \endverbatim
    ///
    /// Code section, does not include slashes, but shows comments
    /// \code
    /// // code comment
    /// int foo();
    /// \endcode
    ///
    /// Dot graph, does not include slashes
    /// \dot
    /// // dot comment
    /// digraph G {
    ///   A -> B
    /// }
    /// \enddot
    ///
    /// @long_note{
    /// Some text
    /// * item1
    /// * item2
    ///
    /// More text
    /// }
    ///
    /// @long_note{
    /// Alias with embedded verbatim section
    /// \verbatim
    /// Some text
    /// \endverbatim
    /// }
    void foo1();

    /*! Embedded list
     *
     *  * item1
     *  * item2
     *
     *  Verbatim section includes slashes
     *  \verbatim
     *  * item1
     *  * item2
     *  \endverbatim
     *
     *  Code section, does not include slashes, but shows comments
     *  \code
     *  // code comment
     *  int foo();
     *  \endcode
     *
     *  Dot graph, does not include slashes
     *  \dot
     *  // dot comment
     *  digraph G {
     *    A -> B
     *  }
     *  \enddot
     *
     *  @long_note{
     *  Some text
     *  * item1
     *  * item2
     *
     *  More text
     *  }
     *
     *  @long_note{
     *  Alias with embedded verbatim section
     *  \verbatim
     *  Some text
     *  \endverbatim
     *  }
     */
    void foo2();
