// objective: test that with QT_AUTOBRIEF set a misaligned line of a comment block after a member does not take the brief description (see issue #5266)
// check: class_after_member_qt_auto.xml
// config: EXTRACT_ALL = YES
// config: QT_AUTOBRIEF = YES

/** \file
 *  QT_AUTOBRIEF makes the first sentence of a Qt style comment such as //!
 *  the brief description. A misaligned line must not take the brief
 *  description, also when the lines before it do not start with a sentence.
 */

/** Blocks written after the member. */
class AfterMemberQtAuto
{
  public:
    int qtDetailsFirst;  //!< \details Line 1.
        //!< Line 2. More.

    int qtMisaligned;    //!< Line 1.
        //!< Line 2.
};
