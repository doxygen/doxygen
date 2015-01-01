{# obj should be a class or member #}
{% if obj.typeConstraints %}
  <div class="typecontraint">
  <dl><dt><b>{{ tr.typeConstraints }}</b></dt>
  <dd><table border="0" cellspacing="2" cellpadding="0">
  {% for arg in obj.typeConstraints %}
    <tr><td valign="top"><em>{{ arg.name }}</em></td>
        <td>&#160;</td><td valign="top"><em>{{ arg.type }}</em></td>
        <td>&#160;</td><td>{{ arg.docs }}</td>
    </tr>
  {% endfor %}
  </table></dl></div>
{% endif %}
