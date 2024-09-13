<!--
// objective: test markdown parsing
// check: md_055__markdown.xml
-->

# Foo

## Bar

[Inline link](http://example.com/inline)

[Reference link][1]

[1]: http://example.com/reference

## Baz

More text

[Upper-cased reference link on last line][U]

[U]: http://example.com/last-line

Dash - NDash -- MDash --- EDash \- ENDash \-- EMDash \--- E3Dash \-\-\-

Dashes in links shouldn't cause out-of-bounds assertions:
[--](https://doxygen.nl) [---](https://doxygen.nl)

## Markdown in HTML

<h3>**Header3** blah _blah_ `blah`</h3>
