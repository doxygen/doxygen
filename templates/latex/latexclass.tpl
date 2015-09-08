<# input: compound #>
<% msg %>Generating LaTeX output for class <{ compound.name }><% endmsg %>
\hypertarget{<{ compound.fileName|raw }>}{}\section{<{ compound.title }>}
\label{<{ compound.fileName|raw }>}\index{<{ compound.name|texLabel }>@{<{ compound.name|texIndex }>}}
<# brief description #>
<% if compound.brief %>
  <{ compound.brief }>

<% endif %>
<# compound includes #>
<% if compound.includeInfo %>
    <% with ii=compound.includeInfo %>
      <% include 'latexinclude.tpl' %>
    <% endwith %>


<% endif %>
<# inheritance graph #>
  <% if compound.hasInheritanceDiagram %>
    <{ tr.inheritanceDiagramFor:compound.name }>
    <{ compound.inheritanceDiagram }>
  <% else %>
    <# textual inheritance list #>
    <% if compound.inherits|length>0 %>
      <% markers c in compound.inherits with tr.inheritsList:compound.inherits|length %>
        <% with obj=c.class text=c.name %>
          <% include 'latexobjlink.tpl' %>
        <% endwith %>
      <% endmarkers %>


    <% endif %>
    <% if compound.inheritedBy|length>0 %>
      <% markers c in compound.inheritedBy with tr.inheritedByList:compound.inheritedBy|length %>
        <% with obj=c.class text=c.name %>
          <% include 'latexobjlink.tpl' %>
        <% endwith %>
      <% endmarkers %>


    <% endif %>
  <% endif %>
<# collaboration graph #>
  <% if compound.hasCollaborationDiagram %>
      <{ tr.collaborationDiagramFor:compound.name }>
      <{ compound.collaborationDiagram }>


  <% endif %>
<# member declarations #>
<% if compound.hasDetails %>
  <% if compound.anchor %>
    \label{<{ compound.anchor|raw }>}
    <% if config.PDF_HYPERLINKS and config.USE_PDFLATEX %>
    \hypertarget{<% if compound.fileName %><{ compound.fileName|raw }>_<% endif %><{ compound.anchor|raw }>}{}
    <% endif %>
  <% endif %>
  <% if config.COMPACT_LATEX %>\subsubsection<% else %>\subsection<% endif %>{<{ tr.detailedDesc }>}
  <# template specifier #>
  <% if compound.language=='cpp' and compound.templateDecls %>
    <% spaceless %>
    \subsubsection*{
      <% for targList in compound.templateDecls %>
        template$<$
        <% for targ in targList %>
          <{ targ.type }><% if targ.name %><{ space }><{ targ.name }><% endif %><% if targ.defVal %><{ space }>= <{ targ.defVal }><% endif %><% if not forloop.last %>, <% endif %>
        <% endfor %>
        $>$\\*
      <% endfor %>
    <{ compound.compoundType }><{ space }><{ compound.name }>
    }
    <% endspaceless %>


  <% endif %>
  <% if compound.brief and config.REPEAT_BRIEF %>
  <{ compound.brief }>


  <% endif %>
  <{ compound.details }>


  <# type constraints #>
  <% with obj=compound %>
    <% include 'latextypeconstraints.tpl' %>
  <% endwith %>


<% endif %>
<% msg %>
  <# examples #>
    <% if compound.examples %>
      <dl><dt><b><{ tr.examples }></b><dd>
      <% markers obj in compound.examples with tr.exampleList:compound.examples|length %>
        <% with page=compound text=obj.text %>
          <% include 'htmlobjlink.tpl' %>
        <% endwith %>
      <% endmarkers %>
      </dd></dl>
    <% endif %>
  <# source definition #>
    <% if compound.sourceDef %>
      <% markers obj in compound.sourceDef with tr.definedAtLineInSourceFile %>
        <% with page=compound text=obj.text %>
          <% include 'htmlobjlink.tpl' %>
        <% endwith %>
      <% endmarkers %>
    <% endif %>
<% endmsg %>
<# detailed description #>
<# member definitions #>
<# used files #>
<# separate member pages #>
