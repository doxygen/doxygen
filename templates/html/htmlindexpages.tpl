{# inputs: list, section #}
{% with letter='' %}
  {# create full index page #}
  {% create page.fileName|append:section|append:config.HTML_FILE_EXTENSION from template %}
{% endwith %}
{% if list|length>maxItemsForMultiPageList %}
  {% opensubindex nav %}
  {% with index=list|alphaIndex:'name' %}
    {% for sect in index %}
      {% with letter=sect.letter %}
         {% set page_postfix=section|append:'_'|append:sect.label %}
         {% indexentry nav name=letter file=page.fileName|append:page_postfix anchor='' isReference=False %}
         {# create index pages for all globals starting with a specific letter #}
         {% create page.fileName|append:page_postfix|append:config.HTML_FILE_EXTENSION from template %}
      {% endwith %}
    {% endfor %}
  {% endwith %}
  {% closesubindex nav %}
{% endif %}
