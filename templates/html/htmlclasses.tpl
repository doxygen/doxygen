{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{% indexentry nav name=tr.classIndex file=page.fileName anchor='' isReference=False %}
</div>
{% with index=classIndex.list|alphaIndex:'name' %}
  {# quick index at top #}
  <div class="qindex">
  {% for section in index %}
    <a class="qindex" href="#letter_{{ section.label }}">{{ section.letter }}</a>
    {% if not forloop.last %}
    &#160;|&#160;
    {% endif %}
  {% endfor %}
  </div>
  {# multi column index #}
  <div class="classindex" style="column-count:{{ config.COLS_IN_ALPHA_INDEX }};-moz-column-count:{{ config.COLS_IN_ALPHA_INDEX }};-webkit-column-count:{{ config.COLS_IN_ALPHA_INDEX}}">
  {% for section in index %}
    <ul>
    {% for cls in section.items %}
      <li>
        <span class="ai">
        {% if forloop.first %}
        <a name="letter_{{ section.label }}"></a>
        <span class="ah">&#160;&#160;{{ section.letter }}&#160;&#160;</span><br/>
        {% endif %}
        {% with obj=cls text=cls.name %}
          {% include 'htmlobjlink.tpl' %}
        {% endwith %}
        </span>
      </li>
    {% endfor %}
    </ul>
  {% endfor %}
  </div><!-- classindex -->
  {# quick index at bottom #}
  <div class="qindex">
  {% for section in index %}
    <a class="qindex" href="#letter_{{ section.label }}">{{ section.letter }}</a>
    {% if not forloop.last %}
    &#160;|&#160;
    {% endif %}
  {% endfor %}
  </div>
{% endwith %}
</div><!-- contents -->
{% endblock %}

