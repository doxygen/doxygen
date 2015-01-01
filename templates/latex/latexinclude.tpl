<# input: ii with attributes (file,name,isImport,isLocal), compound with attribute language #>
<% spaceless %>
<% if ii.file or ii.name %>
{\ttfamily<{ space }>
    <% if compound.language=='java' or compound.language=='idl' %>
      import
    <% else %>
      <% if ii.isImport %>
      \#import
      <% else %>
      \#include
      <% endif %>
    <% endif %>
    <{ space }>
    <% if ii.isLocal %>
      \char`\"{}
    <% else %>
      $<$
    <% endif %>
    <% if ii.name %>
      <{ ii.name }>
    <% else %>
      <{ ii.file.name }>
    <% endif %>
    <% if ii.isLocal %>
      \char`\"{}
    <% else %>
      $>$
    <% endif %>
}
<% endif %>
<% endspaceless %>
