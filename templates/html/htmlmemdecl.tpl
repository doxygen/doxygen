{# inputs: member, inheritId=<string> anonymousNestingLevel=<int> #}
{% if not member.isEnumValue %}
  {# start member declaration #}
  <tr class="memitem:{{ member.anchor}}{% if inheritId %} inherit {{ inheritId }}{% endif %}">
  {% if member.isEnumeration %}
    {% if anonymousNestingLevel>0 %}
      <td class="memItemLeft">
    {% else %}
      <td class="memItemLeft" align="right" valign="top">
    {% endif %}
    {# write optional anchor #}
      {% if not member.hasDetails %}
        <a class="anchor" id="{% if member.anonymousMember %}{{ member.anonymousMember.anchor}}{% else %}{{ member.anchor }}{% endif %}"></a>
      {% endif %}
    {# write optional indent #}
    {% repeat anonymousNestingLevel %}&#160;&#160;&#160;{% endrepeat %}
    enum&#160;</td><td class="memTemplItemRight" valign="bottom">
    {# write name #}
    {% if not member.isAnonymous %}
      {% with obj=member text=member.nameWithContextFor:compound.compoundKind %}
        {% include 'htmlobjlink.tpl' %}
      {% endwith %}
    {% endif %}
    {% if member.enumBaseType %} : {{ member.enumBaseType }}{% endif %}
    {% if member.enumValues|length>0 and config.ENUM_VALUES_PER_LINE>0 %}
      {
        {% for enumVal in member.enumValues %}
          {% if member.enumValues|length>config.ENUM_VALUES_PER_LINE and forloop.counter0|divisibleby:config.ENUM_VALUES_PER_LINE %}
            <br/>&#160;&#160;
          {% endif %}
          {% spaceless %}
            {% with obj=enumVal text=enumVal.name %}
              {% include 'htmlobjlink.tpl' %}
              {% if enumVal.hasOneLineInitializer %}
                {{ member.initializer }}
              {% endif %}
              {% if not forloop.last %},{% endif %}
            {% endwith %}
          {% endspaceless %}
        {% endfor %}
        {% if member.enumValues|length>config.ENUM_VALUES_PER_LINE %}
        <br/>
        {% endif %}
      }
    {% endif %}
  {% else %}
    {% if anonymousNestingLevel>0 or member.anonymousType %}
      <td class="memItemLeft">
    {% else %}
      {% if member.templateArgs %}
        <td class="memTemplParams" colspan="2">
      {% else %}
        <td class="memItemLeft" align="right" valign="top">
      {% endif %}
    {% endif %}
    {# write optional anchor #}
      {% if not member.hasDetails %}
        <a class="anchor" id="{% if member.anonymousMember %}{{ member.anonymousMember.anchor}}{% else %}{{ member.anchor }}{% endif %}"></a>
      {% endif %}
    {# write optional indent #}
    {% repeat anonymousNestingLevel %}&#160;&#160;&#160;{% endrepeat %}
    {# write template list #}
      {% if member.templateArgs and member.language=='cpp' %}
        {% spaceless %}
          template&lt;
          {% for targ in member.templateArgs %}
            {{ targ.type }} {{ targ.name }}{% if targ.defVal %} = {{ targ.defval }}{% endif %}{% if not forloop.last %}, {% endif %}
          {% endfor %}
        {% endspaceless %} &gt;
        </td></tr><tr class="memitem:{{ member.anchor }}{% if inheritId %} inherit {{ inheritId }}{% endif %}"><td class="memTemplItemLeft" align="right" valign="top">
      {% endif %}
    {# write type #}
      {% if member.anonymousType %}
        {% with ctx=member.anonymousType anonymousNestingLevel=anonymousNestingLevel|add:1 %}
          {{ ctx.compoundType }}
          {% if ctx.bareName %}
            &#160;<b>{{ ctx.bareName }}</b>
          {% endif %}
          {</td></tr>
          {# recursively write members that can appear inside the anonymous class/struct #}
          {% with memberListInfo=ctx.publicTypes %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.publicMethods %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.publicStaticMethods %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.publicAttributes %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.publicStaticAttributes %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.protectedTypes %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.protectedMethods %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.protectedStaticMethods %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.protectedAttributes %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.protectedStaticAttributes %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.privateTypes %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.privateMethods %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.privateStaticMethods %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.privateAttributes %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
          {% with memberListInfo=ctx.privateStaticAttributes %}
            {% include 'htmlmemlist.tpl' %}
          {% endwith %}
        {% endwith %}
        <tr class="memitem:{{ member.anchor }}{% if inheritId %} inherit {{ inheritId }}{% endif %}">
        <td class="memItemLeft" valign="top">{% repeat anonymousNestingLevel %}&#160;&#160;&#160;{% endrepeat %}
        }
      {% else %}
        {{ member.declType }}
      {% endif %}
      {% spaceless %}
        &#160;
        {% if anonymousNestingLevel>0 %}
           &#160;&#160;
        {% else %}
          </td><td class="{% if member.templateArgs %}memTemplItemRight{% else %}memItemRight{% endif %}" valign="bottom">
          {% if member.isObjCMethod %}
            {% if member.isStatic %}+&#160;{% else %}-&#160;{% endif %}
          {% endif %}
        {% endif %}
      {% endspaceless %}
    {# write name #}
      {% if not member.isAnonymous %}
        {% if member.anonymousMember %}
	  {% with obj=member.anonymousMember text=member.anonymousMember.nameWithContextFor:compound.compoundKind %}
            {% include 'htmlobjlink.tpl' %}
          {% endwith %}
        {% else %}
          {% with obj=member text=member.nameWithContextFor:compound.compoundKind %}
            {% include 'htmlobjlink.tpl' %}
          {% endwith %}
        {% endif %}
      {% endif %}
    {# write arguments #}
      {% if not member.isObjCMethod %}
        {{ member.declArgs }}
      {% endif %}
    {# write exceptions #}
      {% if member.exception %}
        {{ member.exception }}
      {% endif %}
    {# write bitfield #}
      {% if member.bitfields %}
        {{ member.bitfields }}
      {% endif %}
    {# write one-line initializer #}
      {% if member.hasOneLineInitializer %}
        {% if member.isDefine %}&#160;&#160;&#160;{% endif %}
        {{ member.initializer }}
      {% endif %}
    {# write template alias #}
      {% if member.templateAlias %}
        {{ member.templateAlias }}
      {% endif %}
    {# write obj-c implementation #}
      {% if member.isObjCMethod or member.isObjCProperty %}
        {% if member.isImplementation %}
          <code> [implementation]</code>
        {% endif %}
      {% endif %}
    {# write getter/setter property #}
      {% if member.isProperty and member.propertyAttrs|length>0 %}
        <code> [
        {% for attr in member.propertyAttrs %}
          {{ attr }}{% if not forloop.last %},{% endif %}
        {% endfor %}
        ]</code>
      {% endif %}
    {# write event methods #}
      {% if member.isEvent and member.eventAttrs|length>0 %}
        <code> [
        {% for attr in member.eventAttrs %}
          {{ attr }}{% if not forloop.last %},{% endif %}
        {% endfor %}
        ]</code>
      {% endif %}
    {# end member declaration #}
  {% endif %} {# member.isEnumeration #}
  </td></tr>
  {# brief description #}
    {% if member.brief %}
      <tr class="memdesc:{{ member.anchor }}{% if inheritId %} inherit {{ inheritId }}{% endif %}"><td class="mdescLeft">&#160;</td><td class="mdescRight">
      {{ member.brief }}
      {% if member.hasDetails %}
        <a href="{{ member.fileName }}{{ config.HTML_FILE_EXTENSION }}#{{ member.anchor }}">{{ tr.more }}</a>
      {% endif %}
      <br/></td></tr>
    {% endif %}
  <tr class="separator:{{ member.anchor }}{% if inheritId %} inherit {{ inheritId }}{% endif %}"><td class="memSeparator" colspan="2">&#160;</td></tr>
{% endif %} {# not member.isEnumValue #}
