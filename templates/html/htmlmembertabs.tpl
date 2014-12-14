{# inputs page, list #}
{% if not config.DISABLE_INDEX %}
{# third row of tabs #}
<div id="navrow3" class="tabs2">
  <ul class="tablist">
    <li {% if section=='' %}class="current"{% endif %}><a href="{{ page.fileName }}{{ config.HTML_FILE_EXTENSION }}">{{ tr.all }}</a></li>
{% if page.functions %}
    <li {% if section=='_func' %}class="current"{% endif %}><a href="{{page.fileName}}_func{{ config.HTML_FILE_EXTENSION }}">{{ tr.functions }}</a></li>
{% endif %}
{% if page.variables %}
    <li {% if section=='_vars' %}class="current"{% endif %}><a href="{{page.fileName}}_vars{{ config.HTML_FILE_EXTENSION }}">{{ tr.variables }}</a></li>
{% endif %}
{% if page.typedefs %}
    <li {% if section=='_type' %}class="current"{% endif %}><a href="{{page.fileName}}_type{{ config.HTML_FILE_EXTENSION }}">{{ tr.typedefs }}</a></li>
{% endif %}
{% if page.enums %}
    <li {% if section=='_enum' %}class="current"{% endif %}><a href="{{page.fileName}}_enum{{ config.HTML_FILE_EXTENSION }}">{{ tr.enums }}</a></li>
{% endif %}
{% if page.enumValues %}
    <li {% if section=='_eval' %}class="current"{% endif %}><a href="{{page.fileName}}_eval{{ config.HTML_FILE_EXTENSION }}">{{ tr.enumValues }}</a></li>
{% endif %}
{% if page.macros %}
    <li {% if section=='_defs' %}class="current"{% endif %}><a href="{{page.fileName}}_defs{{ config.HTML_FILE_EXTENSION }}">{{ tr.macros }}</a></li>
{% endif %}
{% if page.properties %}
    <li {% if section=='_prop' %}class="current"{% endif %}><a href="{{page.fileName}}_prop{{ config.HTML_FILE_EXTENSION }}">{{ tr.properties }}</a></li>
{% endif %}
{% if page.events %}
    <li {% if section=='_evnt' %}class="current"{% endif %}><a href="{{page.fileName}}_evnt{{ config.HTML_FILE_EXTENSION }}">{{ tr.events }}</a></li>
{% endif %}
{% if page.related %}
    <li {% if section=='_rela' %}class="current"{% endif %}><a href="{{page.fileName}}_rela{{ config.HTML_FILE_EXTENSION }}">{{ tr.related }}</a></li>
{% endif %}
  </ul>
</div>
{# forth row of tabs #}
{% if list|length>maxItemsForMultiPageList %}
<div id="navrow4" class="tabs3">
  <ul class="tablist">
    {% with index=list|alphaIndex:'name' %}
      {% for sect in index %}
        <li {% if sect.letter==letter %}class="current"{% endif %}><a href="{{page.fileName}}{{section}}_{{sect.label}}{{ config.HTML_FILE_EXTENSION }}">{{ sect.letter }}</a></li>
      {% endfor %}
    {% endwith %}
  </ul>
</div>
{% endif %}
{% endif %}
