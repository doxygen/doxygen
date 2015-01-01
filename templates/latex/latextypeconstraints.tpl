<# obj should be a class or member #>
<% msg %>type constraints = <{ obj.typeConstraints|length }><% endmsg %>
<% if obj.typeConstraints %>
\begin{Desc}
\item[<{ tr.typeConstraints }>]
\begin{description}
<% for arg in obj.typeConstraints %>
  \item[{\em <{ arg.name }>} : {\em <{ arg.type }>}] <{ arg.docs }>
<% endfor %>
\end{description}
\end{Desc}
<% endif %>
