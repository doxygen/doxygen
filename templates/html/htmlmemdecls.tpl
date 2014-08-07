{# inputs: memberListInfo or memberGroupInfo #}
{% if memberListInfo %}
  {% if memberListInfo.members|length>0 or memberListInfo.memberGroups|length>0 %}
    <table class="memberdecls">
    {# section header #}
      <tr class="heading"><td colspan="2"><h2 class="groupheader">{{ memberListInfo.title }}<a name="{{ memberListInfo.anchor }}"></a></h2></td></tr>
    {% if memberListInfo.subtitle %}
      <tr><td class="ititle" colspan="2">{{ memberListInfo.subtitle }}</td></tr>
    {% endif %}
    {# normal members #}
    {% with inheritId='' anonymousNestingLevel=0 %}
      {% for member in memberListInfo.members %}
        {% include 'htmlmemdecl.tpl' %}
      {% endfor %}
    {% endwith %}
    {# grouped members #}
    {% for memgroup in memberListInfo.memberGroups %}
      {% with memberListInfo=memgroup inheritId='' anonymousNestingLevel=0 %}
        {% if memberListInfo.title!='[NOHEADER]' %}
          <tr><td colspan="2"><div class="groupHeader">{{ memberListInfo.title }}</div></td></tr>
          {% if memberListInfo.docs %}
            <tr><td colspan="2"><div class="groupText">{{ memberListInfo.docs }}</div></td></tr>
          {% endif %}
        {% endif %}
        {% for member in memberListInfo.members %}
          {% include 'htmlmemdecl.tpl' %}
        {% endfor %}
      {% endwith %}
    {% endfor %}
    {# inherited members #}
    {% if memberListInfo.inherited %}
      {% for info in memberListInfo.inherited %}
        {% include 'htmlmeminherit.tpl' %}
      {% endfor %}
    {% endif %}
    </table>
  {% endif %}
{% endif %}
