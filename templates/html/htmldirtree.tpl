{# input tree with maxDepth, preferredDepth, and nodes #}
<div class="directory">
{# level selection #}
{% if tree.maxDepth > 1 %}
  <div class="levels">[{{ tr.detailLevel }}
  {% range i from 1 to tree.maxDepth %}
  <span onclick="javascript:toggleLevel({{ i }});">{{ i }}</span>
  {% endrange %}
  ]</div>
{% endif %}
{# the table with entries #}
<table class="directory">
{% recursetree tree.tree %}
  {% indexentry nav name=node.name file=node.fileName anchor=node.anchor isReference=node.isReference externalReference=node.externalReference %}
  {% spaceless %}
  <tr id="row_{{ node.id }}" class="{% cycle 'even' 'odd' %}"{%if node.level>tree.preferredDepth %} style="display:none;"{% endif %}>
    <td class="entry">
  {% if node.is_leaf_node %}
    <span style="width:{{ (node.level+1)*16 }}px;display:inline-block;">&#160;</span>
  {% else %}
    <span style="width:{{ (node.level)*16 }}px;display:inline-block;">&#160;</span>
    <span id="arr_{{ node.id }}" class="arrow" onclick="toggleFolder('{{ node.id}}')">
       {%if node.level+1<tree.preferredDepth %}&#9660;{% else %}&#9658;{% endif %}
    </span>
  {% endif %}
  {% if node.namespace %}
    <span class="icona"><span class="icon">N</span></span>
  {% elif node.class %}
    <span class="icona"><span class="icon">C</span></span>
  {% elif node.dir %}
    <span id="img_{{ node.id }}" class="iconf{%if node.level+1<tree.preferredDepth %}open{% else %}closed{% endif %}" onclick="toggleFolder('{{ node.id }}')">&#160;</span>
  {% elif node.file %}
    {% if node.file.hasSourceFile %}
      <a href="{{ node.file.sourceFileName }}{{ config.HTML_FILE_EXTENSION }}">
    {% endif %}
    <span class="icondoc"></span>
    {% if node.file.hasSourceFile %}
      </a>
    {% endif %}
  {% endif %}
  {% with obj=node text=node.name %}
    {% include 'htmlobjlink.tpl' %}
  {% endwith %}
    </td><td class="desc">{{ node.brief }}</td>
  </tr>
  {% endspaceless %}
  {% opensubindex nav %}
  {{ children }}
  {% closesubindex nav %}
{% endrecursetree %}
</table>
</div><!-- directory -->
