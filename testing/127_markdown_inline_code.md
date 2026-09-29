<!--
// objective: test inline code and html spans adjacent to text
// config: MARKDOWN_STRICT = NO
// check: md_127__markdown__inline__code.xml
-->
# Adjacent inline code

Test: one`.h`and one`.c`file.

- t1 test<tt>.h</tt>  test<TT>.c</TT>
- t2 test<em>.h</em>  test<em>.c</em>
- t3 test<b>.h</b> and x<code>y</code>z
- t4 std::vector<int> stays a template
