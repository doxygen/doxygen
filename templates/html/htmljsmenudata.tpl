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
var menudata={children:[
{text:"{{ tr.mainPage }}",url:"index{{ config.HTML_FILE_EXTENSION }}"}
{% if pageTree.tree %}
,{text:"{{ tr.pages }}",url:"pages{{ config.HTML_FILE_EXTENSION }}"}
{% endif %}
{% if moduleTree.tree %}
,{text:"{{ tr.modules }}",url:"modules{{ config.HTML_FILE_EXTENSION }}"}
{% endif %}
{% if namespaceList %}
,{text:"{{ tr.namespaces }}",url:'namespaces{{ config.HTML_FILE_EXTENSION }}",children:[
 {text:"{{ tr.namespaceList }}",url:'namespaces{{ config.HTML_FILE_EXTENSION }}"}
{% if namespaceMembersIndex.all %}
,{text:"{{ tr.namespaceMembers }}",url:'namespacemembers{{ config.HTML_FILE_EXTENSION }}",children:[
  {% with page=namespaceMembersIndex %}
    {% include 'htmljsmenumembersdata.tpl' %}
  {% endwith %}
 ]}
{% endif %}
]}
{% endif %}
{% if classList %}
,{text:"{{ tr.classes }}",url:"annotated{{ config.HTML_FILE_EXTENSION }}",children:[
 {text:"{{ tr.classList }}",url:"annotated{{ config.HTML_FILE_EXTENSION }}"}
,{text:"{{ tr.classIndex }}",url:"classes{{ config.HTML_FILE_EXTENSION }}"}
{% if classHierarchy.tree %}
,{text:"{{ tr.classHierarchy }}",url:"hierarchy{{ config.HTML_FILE_EXTENSION }}"}
{% endif %}
{% if classMembersIndex.all %}
,{text:"{{ tr.classMembers }}",url:"functions{{ config.HTML_FILE_EXTENSION }}",children:[
  {% with page=classMembersIndex %}
    {% include 'htmljsmenumembersdata.tpl' %}
  {% endwith %}
]}
{% endif %}
]}
{% endif %}
{% if fileList %}
,{text:"{{ tr.files }}",url:"files{{ config.HTML_FILE_EXTENSION }}",children:[
 {text:"{{ tr.fileList }}",url:"files{{ config.HTML_FILE_EXTENSION }}"}
{% if globalsIndex.all %}
,{text:"{{ tr.fileMembers }}",url:"globals{{ config.HTML_FILE_EXTENSION }}",children:[
  {% with page=globalsIndex %}
    {% include 'htmljsmenumembersdata.tpl' %}
  {% endwith %}
]}
{% endif %}
]}
{% endif %}
{% if exampleTree.tree %}
,{text:"{{ tr.examples }}",url:"examples{{ config.HTML_FILE_EXTENSION }}"}
{% endif %}
]}
