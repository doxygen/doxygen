<# inputs: obj, text #>
<% if config.PDF_HYPERLINKS %>
\hyperlink{<{ obj.fileName|raw }><% if obj.anchor %>_<{ obj.anchor }><% endif %>}{<{ text }>}
<% else %>
{\bf <{ text }>}
<% endif %>
