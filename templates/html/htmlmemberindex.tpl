{# input: list #}
{% set singleList=(list|length<=maxItemsForFlatList) or (list|length>maxItemsForMultiPageList) %}
{% if singleList %}
<ul>
{% endif %}
{% with index=list|alphaIndex:'name' %}
  {% for section in index %}
    {% if not singleList or letter=='' or section.letter==letter %}
      {% if not singleList %}
      <a class="anchor" id="{{ section.label }}"></a><h3>- {{ section.letter }} -</h3>
      <ul>
      {% endif %}
      {% for nameList in section.items|groupBy:'name' %}
        {% spaceless %}
          {% for item in nameList|listsort:'{{item.file.name}}' %}
            {% if forloop.first %}
              <li>{{ item.name }}{% if (item.isFunction or item.isSignal or item.isSlot) and not item.isObjCMethod %}(){% endif %}&#160;:&#160;
            {% endif %}
            {% with obj=item scope=item|get:scope text=scope.name %}
               {% include 'htmlobjlink.tpl' %}
            {% endwith %}
            {% if not forloop.last %},&#160;
            {% else %}
              </li>
            {% endif %}
          {% endfor %}
        {% endspaceless %}
      {% endfor %}
      {% if not singleList %}
      </ul>
      {% endif %}
    {% endif %}
  {% endfor %}
{% endwith %}
{% if singleList %}
</ul>
{% endif %}
