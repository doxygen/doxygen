/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
{% recursetree index.nav %}
  {% set varName=node.file %}
  {% if node.anchor %}
    {% update varName=varName|append:'_'|append:node.anchor %}
  {% endif %}
  {% if node.parent %}
    {% if node.parent.file==node.file %}
      {% update varName=varName|append:'_dup' %}
    {% endif %}
  {% endif %}
[ "{{ node.name }}",{% if node.file %}"{% if node.isReference %}{{ node.externalReference }}{% endif %}{{ node.file|decodeURL }}{{ config.HTML_FILE_EXTENSION }}{% if node.anchor %}#{{ node.anchor }}{% endif %}"{% else %}null{% endif %},{% if not node.is_leaf_node %}{% if node.separateIndex %}"{{ varName }}"{% else %} [
     {{ children }}
    ]{% endif %}{% else %} null{% endif %} ]{% if not node.last %},{% endif %}
  {% if node.separateIndex %}
     {% create varName|append:'.js' from 'htmljsnavpage.tpl' %}
  {% endif %}
{% endrecursetree %}
];

var NAVTREEINDEX =
[
{% with navlist=index.nav|flatten|listsort:config.HTML_FILE_EXTENSION|prepend:'{{file}}'|append:'#{{anchor}}' navpages=navlist|paginate:250 %}
  {% for page in navpages %}
    "{{ page.0.file }}{{ config.HTML_FILE_EXTENSION }}{% if page.0.anchor %}#{{ page.0.anchor }}{% endif %}"{% if not forloop.last %},{%endif %}
    {% with idx=forloop.counter0 entries=page %}
      {% create forloop.counter0|prepend:'navtreeindex'|append:'.js' from 'htmljsnavindex.tpl' %}
    {% endwith %}
  {% endfor %}
{% endwith %}
];

var SYNCONMSG  = '{{ tr.panelSyncOn }}';
var SYNCOFFMSG = '{{ tr.panelSyncOff }}';
