// objective: test the <details> and <summary> commands
// check: class_details_c_s_style.xml
// check: class_details_nested.xml
// check: class_details_summary_at_top.xml
// check: class_details_summary_at_bottom.xml
// check: class_details_without_summary.xml
// check: class_details_multi_summary.xml
/**
 * Before details.
 * <details>
 * <summary>Summary on top</summary>
 * Details body
 * </details>
 * After details.
 */
class DetailsSummaryAtTop {};

/**
 * Before details.
 * <details>
 * Details body without summary
 * </details>
 * After details.
 */
class DetailsWithoutSummary {};

/**
 * Before details.
 * <details>
 * Details body
 * <summary>Summary at the bottom</summary>
 * </details>
 * After details.
 */
class DetailsSummaryAtBottom {};

/**
 * <summary>Summary C# style</summary>
 * Details below
 */
class DetailsCSStyle {};

/**
 * Nested details
 * <details>
 * <summary>Summary at top</summary>
 *   Some text
 *   <details>
 *     More details.
 *     <summary>Nested summary at bottom</summary>
 *   </details>
 * </details>
 */
class DetailsNested {};

/**
 * Multiple summary sections
 * <details>
 * <summary>First summary</summary>
 * More details.
 * <summary>Second summary</summary>
 * More details.
 * <summary>Third summary</summary>
 * Last part.
 * </details>
 */
class DetailsMultiSummary {};

