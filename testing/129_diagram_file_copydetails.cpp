// objective: test that \dotfile, \mscfile, and \mermaidfile do not drop a later \copydetails in the same comment, see issue #12390
// check: 129__diagram__file__copydetails_8cpp.xml
// config: HAVE_DOT = YES
// config: DOTFILE_DIRS = $INPUTDIR/more_129
// config: MSCFILE_DIRS = $INPUTDIR/more_129
// config: MERMAIDFILE_DIRS = $INPUTDIR/more_129
// config: MERMAID_RENDER_MODE = CLIENT_SIDE
/** @file */

/** @brief The function whose details are copied.
 *
 *  Copied details.
 */
void source();

/** @brief Has a \\dotfile before a \\copydetails.
 *
 *  @dotfile graph.dot
 *
 *  @copydetails source()
 */
void dotFileFirst();

/** @brief Has a \\mscfile before a \\copydetails.
 *
 *  @mscfile chart.msc
 *
 *  @copydetails source()
 */
void mscFileFirst();

/** @brief Has a \\mermaidfile before a \\copydetails.
 *
 *  @mermaidfile chart.mmd
 *
 *  @copydetails source()
 */
void mermaidFileFirst();
