{# input: info (with .id .inheritedFrom and .members) #}
<tr class="inherit_header {{ info.id }}">
<td colspan="2" onclick="javascript:toggleInherit('{{ info.id }}')">
<img src="{{ page.relPath }}closed.png" alt="-"/>&#160;
  {% markers mark in info.inheritedFrom with tr.inheritedFrom %}
    {% if markers.id==0 %} {# the title mark #}
      {{ mark }}
    {% endif %}
    {% if markers.id==1 %} {# the class link mark #}
      {% with obj=mark text=mark.name %}
        {% include 'htmlobjlink.tpl' %}
      {% endwith %}
    {% endif %}
  {% endmarkers %}
</td></tr>
{% with inheritId=info.id anonymousNestingLevel=0 %}
  {% for member in info.members %}
    {% include 'htmlmemdecl.tpl' %}
  {% endfor %}
{% endwith %}
