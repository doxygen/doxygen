/*
 @licstart  The following is the entire license notice for the
 JavaScript code in this file.

 Copyright (C) 1997-2017 by Dimitri van Heesch

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 @licend  The above is the entire license notice
 for the JavaScript code in this file
 */
var menudata={children:[
{text:'{{ tr.mainPage }}',url:'index{{ config.HTML_FILE_EXTENSION }}'}
{% if pageTree.tree %}
,{text:'{{ tr.pages }}',url:'pages{{ config.HTML_FILE_EXTENSION }}'}
{% endif %}
{% if moduleTree.tree %}
,{text:'{{ tr.modules }}',url:'modules{{ config.HTML_FILE_EXTENSION }}'}
{% endif %}
{% if namespaceList %}
,{text:'{{ tr.namespaces }}',url:'namespaces{{ config.HTML_FILE_EXTENSION }}',children:[
 {text:'{{ tr.namespaceList }}',url:'namespaces{{ config.HTML_FILE_EXTENSION }}'}
{% if namespaceMembersIndex.all %}
,{text:'{{ tr.namespaceMembers }}',url:'namespacemembers{{ config.HTML_FILE_EXTENSION }}',children:[
  {% with page=namespaceMembersIndex %}
    {% include 'htmljsmenumembersdata.tpl' %}
  {% endwith %}
 ]}
{% endif %}
]}
{% endif %}
{% if classList %}
,{text:'{{ tr.classes }}',url:'annotated{{ config.HTML_FILE_EXTENSION }}',children:[
 {text:'{{ tr.classList }}',url:'annotated{{ config.HTML_FILE_EXTENSION }}'}
{% if classHierarchy.tree %}
,{text:'{{ tr.classHierarchy }}',url:'hierarchy{{ config.HTML_FILE_EXTENSION }}'}
{% endif %}
{% if classMembersIndex.all %}
,{text:'{{ tr.classMembers }}',url:'functions{{ config.HTML_FILE_EXTENSION }}',children:[
  {% with page=classMembersIndex %}
    {% include 'htmljsmenumembersdata.tpl' %}
  {% endwith %}
]}
{% endif %}
]}
{% endif %}
{% if fileList %}
,{text:'{{ tr.files }}',url:'files{{ config.HTML_FILE_EXTENSION }}',children[
 {text:'{{ tr.fileList }}',url:'files{{ config.HTML_FILE_EXTENSION }}'}
{% if globalsIndex.all %}
,{text:'{{ tr.fileMembers }}',url'globals{{ config.HTML_FILE_EXTENSION }}',children:[
  {% with page=globalsIndex %}
    {% include 'htmljsmenumembersdata.tpl' %}
  {% endwith %}
]}
{% endif %}
]}
{% endif %}
{% if exampleTree.tree %}
,{text:'{{ tr.examples }}',url:'examples{{ config.HTML_FILE_EXTENSION }}'}
{% endif %}
]}
