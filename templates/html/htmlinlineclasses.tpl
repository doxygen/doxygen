{# input: classList #}
{% if classList %}
  <h2 class="groupheader">{{ tr.classDocumentation }}</h2>
  {% for class in classList %}
    {# write anchor #}
    <a class="anchor" id="{{ class.anchor }}"></a>
    <div class="memitem">
    <div class="memproto">
    <table class="memname">
      <tr><td class="memname">{{ class.compoundType }} {{ class.name }}</td></tr>
    </table>
    </div>
    <div class="memdoc">
    <div class="textblock">
      {# template specifier #}
      {% if class.language=='cpp' and class.templateDecls %}
        <h3>{% spaceless %}
          {% for targList in class.templateDecls %}
            template&lt;
            {% for targ in targList %}
              {{ targ.type }}{% if targ.name %} {{ targ.name }}{% endif %}{% if targ.defVal %}&#160;= {{ targ.defVal }}{% endif %}{% if not forloop.last %}, {% endif %}
            {% endfor %}
            &gt;<br/>
          {% endfor %}
        {% endspaceless %}
        {{ class.classType }}&#160;{{ class.name }}
        </h3>
      {% endif %}
      {# brief description #}
      {% if class.brief and config.REPEAT_BRIEF %}
        <p>{{ class.brief }}</p>
      {% endif %}
      {# detailed docs #}
      {{ class.details }}
      {# source def #}
      {% if class.sourceDef %}
        {% markers obj in class.sourceDef with tr.definedAtLineInSourceFile %}
          {% with text=obj.text %}
            {% include 'htmlobjlink.tpl' %}
          {% endwith %}
        {% endmarkers %}
      {% endif %}
    </div><!-- textblock -->
    {# table with fields #}
    <table class="fieldtable">
      <tr><th colspan="3">{{ tr.compoundMembers }}</th></tr>
      {% for member in class.members %}
        <tr><td class="fieldtype">
               <a class="anchor" id="{{ member.anchor }}"></a>{{ member.fieldType }}
            </td>
            <td class="fieldname">
               {{ member.name }}
               {% if member.isVariable and member.declArgs %}{{ member.declArgs }}{% endif %}
               {{ member.bitfields }}
            </td>
            <td class="fielddoc">
              {% if member.brief and not member.details %}{# only brief #}
                {{ member.brief }}
              {% else %} {# only details or both #}
                {% if member.brief %}<p>{{ member.brief }}</p>{% endif %}
                {{ member.details }}
              {% endif %}
            </td>
        </tr>
      {% endfor %}
    </table>
    </div><!-- memdoc -->
    </div><!-- memitem -->
  {% endfor %}
{% endif %}
