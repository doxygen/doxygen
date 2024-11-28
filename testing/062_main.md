<!--
// objective: test @include{doc} command with raise option
// check: md_062__main.xml
-->
[TOC]

Some text in main.md

# Section 1
@include{doc,raise=1} sub.md

# Section 2
## Subsection 2.1
@include{doc,raise=2} subsub.md

