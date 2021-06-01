{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
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
  {% indexentry nav name=tr.classIndex file=page.fileName anchor='' isReference=False separateIndex=False %}
  {# multi column index #}
  <div class="classindex">
  {% for section in index %}
    <dl class="classindex {% cycle 'even' 'odd' %}">
    {% for cls in section.items %}
      {% if forloop.first %}
      <dt class="alphachar"><a id="letter_{{ section.label }}" name="letter_{{ section.label }}">{{ section.letter }}</a></dt>
      {% endif %}
      <dd>
      {% with obj=cls text=cls.name %}
        {% include 'htmlobjlink.tpl' %}
      {% endwith %}
      </dd>
    {% endfor %}
    </dl>
  {% endfor %}
  </div>
{% endwith %}
</div><!-- contents -->
{% endblock %}

