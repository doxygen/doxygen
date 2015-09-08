{# input: page #}
{% opensubindex nav %}
{# all members #}
{% with list=page.all section='' %}
  {% indexentry nav name=tr.all file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% include 'htmlindexpages.tpl' %}
{% endwith %}
{# functions #}
{% if page.functions %}
  {% set page_postfix='_func' %}
  {% indexentry nav name=tr.functions file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% with list=page.functions section=page_postfix %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endif %}
{# variables #}
{% if page.variables %}
  {% set page_postfix='_vars' %}
  {% indexentry nav name=tr.variables file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% with list=page.variables section=page_postfix %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endif %}
{# typedefs #}
{% if page.typedefs %}
  {% set page_postfix='_type' %}
  {% indexentry nav name=tr.typedefs file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% with list=page.typedefs section=page_postfix %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endif %}
{# enums #}
{% if page.enums %}
  {% set page_postfix='_enum' %}
  {% indexentry nav name=tr.enums file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% with list=page.enums section=page_postfix %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endif %}
{# enumValues #}
{% if page.enumValues %}
  {% set page_postfix='_eval' %}
  {% indexentry nav name=tr.enumValues file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% with list=page.enumValues section=page_postfix %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endif %}
{# macros #}
{% if page.macros %}
  {% set page_postfix='_defs' %}
  {% indexentry nav name=tr.macros file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% with list=page.macros section=page_postfix %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endif %}
{# properties #}
{% if page.properties %}
  {% set page_postfix='_prop' %}
  {% indexentry nav name=tr.properties file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% with list=page.properties section=page_postfix %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endif %}
{# events #}
{% if page.events %}
  {% set page_postfix='_evnt' %}
  {% indexentry nav name=tr.events file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% with list=page.events section=page_postfix %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endif %}
{# related #}
{% if page.related %}
  {% set page_postfix='_rela' %}
  {% indexentry nav name=tr.related file=page.fileName|append:page_postfix anchor='' isReference=False %}
  {% with list=page.related section=page_postfix %}
    {% include 'htmlindexpages.tpl' %}
  {% endwith %}
{% endif %}
{% set page_postfix='' %}
{% closesubindex nav %}
