<!--
// objective: test @snippet{doc} command with raise option
// check: md_063__main.xml
-->
[TOC]

Some text in main.md

# Section 1
@snippet{doc,raise=1} snip.md SUB

# Section 2
## Subsection 2.1
@snippet{doc,raise=2} snip.md SUBSUB

