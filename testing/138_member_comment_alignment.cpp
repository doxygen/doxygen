// objective: test that a misaligned line of a comment block after a member does not take the brief description (see issue #5266)
// check: class_after_member.xml
// check: class_still_separate.xml
// check: class_unchanged.xml
// config: EXTRACT_ALL = YES

/** \file
 *  The lines of a comment block written after a member should be aligned, and
 *  a line that is not starts a new block. When that line also has a marker
 *  ending in '<', the new block still documents the same member: it must come
 *  after the lines before it, not become the brief description ahead of them.
 */

/** Blocks written after the member. */
class AfterMember
{
  public:
    int twoAligned;      //!< Line 1.
                         //!< Line 2.

    int twoMisaligned;   //!< Line 1.
        //!< Line 2.

    int threeLastOut;    //!< Line 1.
                         //!< Line 2.
        //!< Line 3.

    int jdAligned;       ///< Line 1.
                         ///< Line 2.

    int jdMisaligned;    ///< Line 1.
        ///< Line 2.

    /* a misaligned line indented with a tab */
    int tabMisaligned;   //!< Line 1.
	//!< Line 2.

    /* shifted to the right, the line does not become a Markdown code block */
    int rightShifted;    ///< Line 1.
                                 ///< Line 2.

    int listMisaligned;  ///< Line 1
        ///< - item

    enum Values
    {
      Aligned,           //!< Line 1.
                         //!< Line 2.
      Misaligned         //!< Line 1.
          //!< Line 2.
    };
};

/** A line without '<' that is not aligned still starts a block of its own. */
class StillSeparate
{
  public:
    /* it documents the next member */
    int a; //!< doc for a
           //!< more doc for a
//! doc for b
    int b;

    /* also when the block before it is on a line of its own */
    int c;
        //!< doc for c
        //!< more doc for c
//! doc for d
    int d;
};

/** Cases that must not change. */
class Unchanged
{
  public:
    /* Unchanged by this fix: a misaligned line still ends the block before it,
       so a brief paragraph ends there. */
    int b1; //!< \brief Brief description of b1.
        //!< Detailed description of b1.

    /* an indented member in a group keeps its own brief description */
    //! \name Accessors
    //! @{
        //! Brief description of getA.
        int getA();
    //! @}
};
