{% extend 'htmlbase.tpl' %}

{% block title %}
  <div class="headertitle"><div class="title">{{ compound.name }} {{ tr.memberList }}</div></div>
{% endblock %}

{% block content %}
<div class="contents">
<p>{{ tr.theListOfAllMembers }} <a class="el" href="{{ compound.fileName }}{{ config.HTML_FILE_EXTENSION }}">{{ compound.name }}</a>{{ tr.incInheritedMembers }}</p>
<table class="directory">
{% for mi in compound.allMembersList %}
  <tr class="{% cycle 'even' 'odd' %}">
  {% spaceless %}
  {% with member=mi.member %}
  {% if member.language=='objc' %}
    <td class="entry">
    {% if member.isObjCMethod %}
      {% if member.isStatic %}+&#160;{% else %}-&#160;{% endif %}
    {% endif %}
    </td>
  {% endif %}
  {% if member.isObjCMethod %}
    <td class="entry">
      {% with obj=member text=member.name %}
        {% include 'htmlobjlink.tpl' %}
      {% endwith %}
    </td>
  {%else %}
    {# name #}
    <td class="entry">
      {% with obj=member text=mi.ambiguityScope|append:member.name %}
        {% include 'htmlobjlink.tpl' %}
      {% endwith %}
      {% if member.isEnumeration %}
      &#160;{{ tr.enumName }}
      {% elif member.isEnumValue %}
      &#160;{{ tr.enumValue }}
      {% elif member.isTypedef %}
      &#160;typedef
      {% elif member.isFriend and member.type=='friend class' %}
      &#160;class
      {% elif member.isFunction or member.isSignal or member.isSlot or (member.isFriend and member.hasParameters) %}
      {{ member.declArgs }}
      {% endif %}
    </td>
  {% endif %}
  {# class link #}
  <td class="entry">
  {% if member.category %}
    {% with obj=member.category text=member.category.name %}
      {% include 'htmlobjlink.tpl' %}
    {% endwith %}
  {% else %}
    {% with obj=member.class text=member.class.name %}
      {% include 'htmlobjlink.tpl' %}
    {% endwith %}
  {% endif %}
  </td>
  {# labels #}
  {% if member.labels %}
     <td class="entry">
     {% for label in member.labels %}
     <span class="mlabel">{{ label }}</span>
     {% endfor %}
     </td>
  {% endif %}
  {% endwith %}
  {% endspaceless %}
  </tr>
{% endfor %}
</table>
</div>
{% endblock %}
