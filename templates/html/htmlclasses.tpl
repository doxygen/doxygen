{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{% indexentry nav name=tr.classIndex file=page.fileName anchor='' %}
</div>
<div class="classindex" style="column-count:{{ config.COLS_IN_ALPHA_INDEX }};-moz-column-count:{{ config.COLS_IN_ALPHA_INDEX }};-webkit-column-count:{{ config.COLS_IN_ALPHA_INDEX}}">
<ul>
{% with index=classIndex.list|alphaIndex:'name' %}
  {% for section in index %}
    <div class="ah">&#160;&#160;{{ section.letter }}&#160;&#160;</div>
    {% for cls in section.items %}
      <li>{{ cls.name }}</li>
    {% endfor %}
  {% endfor %}
{% endwith %}
</ul>
</div><!-- classindex -->
</div><!-- contents -->
{% endblock %}

