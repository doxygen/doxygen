{# all members #}
{% with list=page.all section='' text=tr.all %}
  {% include 'htmljsmenuletterdata.tpl' %}
{% endwith %}
{# functions #}
{% if page.functions %}
  {% with list=page.functions section='_func' text=tr.functions %}
    ,{% include 'htmljsmenuletterdata.tpl' %}
  {% endwith %}
{% endif %}
{# variables #}
{% if page.variables %}
  {% with list=page.variables section='_vars' text=tr.variables %}
    ,{% include 'htmljsmenuletterdata.tpl' %}
  {% endwith %}
{% endif %}
{# typedefs #}
{% if page.typedefs %}
  {% with list=page.typedefs section='_type' text=tr.typedefs %}
    ,{% include 'htmljsmenuletterdata.tpl' %}
  {% endwith %}
{% endif %}
{# enums #}
{% if page.enums %}
  {% with list=page.enums section='_enum' text=tr.enums %}
    ,{% include 'htmljsmenuletterdata.tpl' %}
  {% endwith %}
{% endif %}
{# enumValues #}
{% if page.enumValues %}
  {% with list=page.enumValues section='_eval' text=tr.enumValues %}
    ,{% include 'htmljsmenuletterdata.tpl' %}
  {% endwith %}
{% endif %}
{# macros #}
{% if page.macros %}
  {% with list=page.macros section='_defs' text=tr.macros %}
    ,{% include 'htmljsmenuletterdata.tpl' %}
  {% endwith %}
{% endif %}
{# properties #}
{% if page.properties %}
  {% with list=page.properties section='_prop' text=tr.properties %}
    ,{% include 'htmljsmenuletterdata.tpl' %}
  {% endwith %}
{% endif %}
{# events #}
{% if page.events %}
  {% with list=page.events section='_evnt' text=tr.events %}
    ,{% include 'htmljsmenuletterdata.tpl' %}
  {% endwith %}
{% endif %}
{# related #}
{% if page.related %}
  {% with list=page.related section='_rela' text=tr.related %}
    ,{% include 'htmljsmenuletterdata.tpl' %}
  {% endwith %}
{% endif %}
