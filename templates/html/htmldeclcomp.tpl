{# inputs: list, label, title, local #}
{% if list %}
  <table class="memberdecls"><tr class="heading"><td colspan="2">
  <h2 class="groupheader"><a name="{{ label }}"></a>{{ title }}</h2></td></tr>
  {% for nc in list %}
    <tr class="memitem:{{ nc.anchor }}">
    <td class="memItemLeft" align="right" valign="top">{% if nc.compoundType %}{{ nc.compoundType }}&#160;{% endif %}</td>
    <td class="memItemRight" valign="bottom">
    {% if local %}
      {% with obj=nc text=nc.bareName %}
        {% include 'htmlobjlink.tpl' %}
      {% endwith %}
    {% else %}
      {% with obj=nc text=nc.name %}
        {% include 'htmlobjlink.tpl' %}
      {% endwith %}
    {% endif %}
    </td></tr>
    {# brief description #}
      {% if nc.brief %}
        <tr class="memdesc:{{ nc.anchor }}"><td class="mdescLeft">&#160;</td><td class="mdescRight">
        {{ nc.brief }}
        {% if nc.hasDetails %}
          <a href="{{ page.relPath }}{{ nc.fileName }}{{ config.HTML_FILE_EXTENSION}}{% if nc.anchor %}#{{ nc.anchor }}{% endif %}">{{ tr.more }}</a>
        {% endif %}
        <br/></td></tr>
      {% endif %}
    <tr class="separator:{{ nc.anchor}}"><td class="memSeparator" colspan="2">&#160;</td></tr>
  {% endfor %}
  </table>
{% endif %}
