{# input: obj which should have dynSectionId attribute #}
{% if config.HTML_DYNAMIC_SECTIONS %}
  <div id="dynsection-{{ obj.dynSectionId }}-summary" class="dynsummary" style="display:block;"></div>
  <div class="dyncontent" id="dynsection-{{ obj.dynSectionId }}-content" style="display:none;">
{% else %}
  <div class="dyncontent">
{% endif %}
