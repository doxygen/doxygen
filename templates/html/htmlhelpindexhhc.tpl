<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML><HEAD></HEAD><BODY>
<OBJECT type="text/site properties">
<param name="FrameName" value="right">
</OBJECT>
<UL>
{% recursetree index.nav %}
<LI><OBJECT type="text/sitemap"><param name="Name" value="{{ node.name }}">
    <param name="{% if node.file|isAbsoluteURL %}URL{% else %}Local{% endif %}" value="{{ node.file|decodeURL }}{{ config.HTML_FILE_EXTENSION }}{% if node.anchor %}#{{ node.anchor }}{% endif %}">
    <param name="ImageNumber" value="1{% if node.is_leaf_node %}1{% endif %}">
    </OBJECT>
  {% if not node.is_leaf_node %}
  <UL>{{ children }}</UL>
  {% endif %}
{% endrecursetree %}
</UL>
</BODY>
</HTML>
