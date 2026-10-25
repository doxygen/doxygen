// objective: test that with JAVADOC_AUTOBRIEF set a misaligned line of a comment block after a member does not take the brief description (see issue #5266)
// check: class_after_member_auto.xml
// config: EXTRACT_ALL = YES
// config: JAVADOC_AUTOBRIEF = YES

/** \file
 *  JAVADOC_AUTOBRIEF makes the first sentence of a Javadoc style comment such
 *  as /// the brief description, and does not apply to Qt style comments such
 *  as //!. A misaligned line must not take the brief description in either
 *  style, also when the lines before it do not start with a sentence.
 */

/** Blocks written after the member. */
class AfterMemberAuto
{
  public:
    int qtMisaligned;    //!< Line 1.
        //!< Line 2.

    int jdDetailsFirst;  ///< \details Line 1.
        ///< Line 2. More.

    int jdListFirst;     ///< - item
        ///< Line 2. More.

    int jdMisaligned;    ///< Line 1.
        ///< Line 2.

    /* Unchanged by this fix: a misaligned line still ends the block before it,
       so the first sentence ends there. */
    int jdNoPeriod;      ///< Brief without a period
        ///< more text
};
