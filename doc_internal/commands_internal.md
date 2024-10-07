\page pg_cmds_int Overview of introduction of internal special commands

The following table gives an overview of the doxygen internal special commands
and the version in which they were introduced.

\secreflist
\refitem cmdialias \\ialias
\refitem cmdendicode \\endicode
\refitem cmdendiliteral \\endiliteral
\refitem cmdendiskip \\endiskip
\refitem cmdendiverbatim \\endiverbatim
\refitem cmdianchor \\ianchor
\refitem cmdicode \\icode
\refitem cmdifile \\ifile
\refitem cmdiline \\iline
\refitem cmdilinebr \\ilinebr
\refitem cmdiliteral \\iliteral
\refitem cmdiprefix \\iprefix
\refitem cmdiraise \\iraise
\refitem cmdiskip \\iskip
\refitem cmdiverbatim \\iverbatim
\endsecreflist

<hr>
\section cmdialias \\ialias{name}

 \addindex \\ialias
 This command is used to prevent endless recursive expansion of aliases. For an alias `name` the command `\ialias{name}` is 
 inserted after its expansion, and then the expanded string is reparsed, but until the `\ialias` is processed the `name` 
 is not considered for further alias expansion.

\since doxygen version 1.11.0

<hr>
\section cmdianchor \\ianchor{title} anchor

  \addindex \\ianchor
  This command has a similar syntax and function as the command `\anchor`, but is internally used
  for the `@page` command in markdown files. Unlike `\anchor` it has an additional `title` option that
  is used as the link text in references to the page.

\since doxygen version 1.9.7

<hr>
\section cmdilinebr \\ilinebr

  \addindex \\ilinebr
  Internal doxygen command to simulate an end of line, but without advancing the
  line counter. In this way it is possible to have multiple commands on one line
  that read till the end of line.
  This command is internally used by doxygen and for replacement of the `^^` in
  `ALIASES` settings.
  This command sees to it that e.g. warning messages stay correct when a command
  is replaced internally by multiple commands.

  Temporarily also as `@_ilinebr` (doxygen version 1.8.14) and `\_ilinebr`
  (doxygen version 1.8.15 till 1.8.18)

\since doxygen version 1.8.19

<hr>
\section cmdifile \\ifile \<filename\>
  \addindex \\ifile

  Internal doxygen command to reset the current filename in a documentation block
  so that doxygen can give a better warning about the original source of a problem
  when a documentation block is constructed from multiple files.

\since doxygen version 1.9.5

<hr>
\section cmdiline \\iline \<linenr\>
  \addindex \\iline

  Internal doxygen command to reset the current line counter in a documentation block
  so that doxygen can give a better warning about the original source of a problem when
  a documentation block is constructed from multiple files or blocks from one file.

\since doxygen version 1.9.2

<hr>
\section cmdicode \\icode['{'\<word\>'}']

  \addindex \\icode
  This command has a similar syntax and function as the command `\code`, but is internally used
  for markdown fenced code blocks (i.e. <code>\`\`\`</code> and `~~~` type of blocks)
  to replace these markers.
  The `\code` command cannot be used as in that case the block cannot contain
  a `\endcode` as this would terminate the `\code` block.

\since doxygen version 1.9.5

<hr>
\section cmdendicode \\endicode

  \addindex \\endicode
  Ends a block of text that was started with a \ref cmdicode "\\icode" command.

\since doxygen version 1.9.5

<hr>
\section cmdiliteral \\iliteral['{'\<option\>'}']
  \addindex \\iliteral

  This command is to replace the Java documentation commands `{@literal .... }` and
  `{@code ...}`. 
  The text in the blocks will not be interpreted by doxygen in any way.
  The text in the `{@literal` will put as is text in the output.
  The text in the `{@code` will be replaced by a code block with class `JavaDocCode`
  i.e. `<code class="JavaDocCode">...</code>`.

\since doxygen version 1.9.3

<hr>
\section cmdendiliteral \\endiliteral

  \addindex \\endiliteral
  Ends a block of text that was started with a \ref cmdiliteral "\\iliteral" command.

\since doxygen version 1.9.3

<hr>
\section cmdiverbatim \\iverbatim

  \addindex \\iverbatim
  This command has a similar function as the command `\verbatim`, but is internally used
  for markdown code blocks (i.e. blocks of text indented with at least 4 extra spaces compared to
  the previous block) and python unformatted docstrings (i.e. <code>'''</code> type of blocks)
  to replace these markers.
  The `\verbatim` command cannot be used as in that case the block cannot contain
  a `\endverbatim` as this would terminate the `\verbatim` block.

\since doxygen version 1.9.5

<hr>
\section cmdiraise \\iraise \<amount\>
  \addindex \\iraise

  Internal doxygen command to increase the section level by a given `amount`.
  After processing `\iraise 1` for instance, a `\section s1` will be treated as a `\subsection s1`.
  Inserted when processing `\include{doc}` with the `raise` option.

\since doxygen version 1.11.0

<hr>
\section cmdiskip \\iskip
  \addindex \\iskip

  Internal doxygen command to suppress evaluation of whitespace to determine the indentation
  of a comment block. Evaluation will continue as normal when the matching 
  \ref cmdendiskip "\\endiskip" command is found.
  Inserted when processing commands that contain literal text like `\startuml`, `\verbatim`, `<code>` etc.

\since doxygen version 1.12.0

<hr>
\section cmdiprefix \\iprefix "<label>"
  \addindex \\iprefix

  Internal doxygen command to prefix section labels references for \c \\ref and \c \\link commands.
  After processing `\iprefix "pf_"` for instance, a <code>\\ref s1</code> will be treated as 
  if <code>\\ref pf_s1</code> was written.
  Inserted internally when processing `\include{doc}` with the `prefix` option.

\since doxygen version 1.11.0

<hr>
\section cmdendiskip \\endiskip

  \addindex \\endiskip
  Ends a block of text that was started with a \ref cmdiskip "\\iskip" command.

\since doxygen version 1.12.0

<hr>
\section cmdendiverbatim \\endiverbatim

  \addindex \\endiverbatim
  Ends a block of text that was started with a \ref cmdiverbatim "\\iverbatim" command.

\since doxygen version 1.9.5

<hr>
