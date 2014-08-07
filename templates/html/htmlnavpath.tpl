{# input: navpath which is a list of links #}
{% if navpath %}
  <div id="nav-path" class="navpath">
    <ul>
    {% for obj in navpath %}
      <li class="navelem">
        {% with text=obj.text %}
          {% include 'htmlobjlink.tpl' %}
        {% endwith %}
      </li>
    {% endfor %}
    </ul>
  </div>
{% endif %}
