\section{<{ tr.classList }>}
<{ tr.classListDescription }>
\begin{DoxyCompactList}
<% for cls in classList %>
\item\contentsline{section}
{<% with obj=cls text=cls.name %><% include 'latexobjlink.tpl' %><% endwith %><% if cls.brief %>\\*<{ cls.brief }><% endif %>}
{\pageref{<{ cls.fileName|raw }>}}{}
<% endfor %>
\end{DoxyCompactList}
