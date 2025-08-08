<!--
// objective: test markdown strict setting
// config: MARKDOWN_STRICT = YES
// check: md_113__markdown__strict.xml
-->

Single quote inside backtick fragment should not end the verbatim section:

Declare `x` like this `var x='c'`.

Single quote inside double backtick fragment should not end the verbatim section:

Declare `y` like this ``var y='d'``.

Double quotes characters do not need to be escaped in a verbatim fragment:

Unescaped `"`quotes`"`.
