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

