{# input: obj which should have dynSectionId and relPath attributes #}
{% if config.HTML_DYNAMIC_SECTIONS %}
<div id="dynsection-{{ obj.dynSectionId }}" onclick="return toggleVisibility(this)" class="dynheader closed" style="cursor:pointer;"><img
     id="dynsection-{{ obj.dynSectionId }}-trigger" src="{{ obj.relPath }}closed.png" alt="+"/>
{% else %}
<div class="dynheader">
{% endif %}
