{% with page=namespaceMembersIndex %}
  {# all members #}
  {% with list=namespaceMembersIndex.all section='' template='htmlclmembers.tpl' %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
  {# functions #}
  {% with list=namespaceMembersIndex.functions section='_func' template='htmlclmembers.tpl' %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
  {# variables #}
  {% with list=namespaceMembersIndex.variables section='_vars' template='htmlclmembers.tpl' %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
  {# typedefs #}
  {% with list=namespaceMembersIndex.typedefs section='_type' template='htmlclmembers.tpl' %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
  {# enums #}
  {% with list=namespaceMembersIndex.enums section='_enum' template='htmlclmembers.tpl' %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
  {# enumValues #}
  {% with list=namespaceMembersIndex.enumValues section='_eval' template='htmlclmembers.tpl' %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endwith %}
