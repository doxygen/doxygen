{# inputs: memberListInfo #}
{% if memberListInfo %}
  {% if memberListInfo.members %}
    <h2 class="groupheader">{{ memberListInfo.title }}</h2>
    {% for member in memberListInfo.members %}
      {% if member.detailsVisibleFor:compound.compoundKind %}
        <a class="anchor" id="{{ member.anchor }}"></a>
        <div class="memitem">
        <div class="memproto">
        {# write template declarations #}
        {% if member.language=='cpp' and member.templateDecls %}
          {% for targList in member.templateDecls %}
            {% spaceless %}
              <div class="memtemplate">
              template&lt;
              {% for targ in targList %}
                {{ targ.type }}{% if targ.name %} {{ targ.name }}{% endif %}{% if targ.defVal %}&#160;= {{ targ.defVal }}{% endif %}{% if not forloop.last %}, {% endif %}
              {% endfor %}
              &gt;
              </div>
            {% endspaceless %}
          {% endfor %}
        {% endif %}
        {# start of labels if present #}
        {% if member.labels %}
          <table class="mlabels"><tr><td class="mlabels-left">
        {% endif %}
        <table class="memname">
          <tr><td class="memname">
            {{ member.definition }}
            {# write argument list #}
            {# TODO: TCL #}
            {% if member.hasParameters %}
              {% if member.isObjCMethod %}
                {% if member.parameters %}
                </td><td></td>
                {% endif %}
                {% for arg in member.parameters %}
                  {% if not forloop.first %}
                    <tr><td class="paramkey">{{ arg.namePart }}</td><td></td>
                  {% endif %}
                  <td class="paramtype">({{ arg.type }})&#160;</td><td class="paramname">
                  {% if arg.name or arg.type=='...' %}
                    <em>{% if not arg.name %}{{ arg.type }}{% else %}{{ arg.name }}{% endif %}</em>
                  {% endif %}
		  {% if not forloop.last %}
                    </td></tr>
                  {% endif %}
                {% endfor %}
              {% else %}
                </td><td>(</td>
                {% for arg in member.parameters %}
                  {% if member.isDefine %}
                    {% if not forloop.first %}
                      <tr><td class="paramkey"></td><td></td>
                    {% endif %}
                    <td class="paramtype"></td><td class="paramname">
		    {% spaceless %}
                      {% if arg.type %}
                        <em>{{ arg.type }}</em>
                      {% endif %}
                      {% if not forloop.last %}
                        ,</td></tr>
                      {% endif %}
		    {% endspaceless %}
                  {% else %} {# normal function/method #}
                    {% if forloop.first %}
                      <td class="paramtype">
                    {% endif %}
                    {% if arg.attrib %}{{ arg.attrib }} {% endif %}
                    {% if arg.type!='...' %}
                       {{ arg.type }}
                    {% endif %}
                    &#160;</td><td class="paramname">
                    {% if arg.name or arg.type=='...' %}
                      <em>{% if not arg.name %}{{ arg.type }}{% else %}{{ arg.name }}{% endif %}</em>
                    {% endif %}
                    {{ arg.array }}
                    {% if arg.defVal %} = {{ arg.defVal }}{% endif %}
		    {% if not forloop.last %}
                      ,</td></tr><tr><td class="paramkey"></td><td></td><td class="paramtype">
                    {% endif %}
                  {% endif %}
                {% endfor %}
		{% if member.parameters|length==0 %}
                  <td class="paramname">
                {% endif %}
                {% if member.parameters|length<2 %}
                  </td><td>)</td><td>
                {% else %}
                  &#160;</td></tr>
                  <tr><td></td><td>)</td><td></td><td>
                {% endif %}
                {{ member.extraTypeChars }}
                {% if member.hasConstQualifier %} const {% endif %}
                {% if member.hasVolatileQualifier %} volatile {% endif %}
                {{ member.trailingReturnType }}
              {% endif %}
            {% endif %}
            {# one line initializer #}
            {% if member.hasOneLineInitializer %}
              {% if member.isDefine %}&#160;&#160;&#160;{% endif %}
              {{ member.initializer }}
            {% endif %}
            {# exception list #}
            {% if member.exception %}
              {# TODO: special exception rendering for UNO IDL... #}
              {{ member.exception }}
            {% endif %}
          </td></tr>
        </table>
        {# end of labels if present #}
        {% if member.labels %}
          </td><td class="mlabels-right">{% spaceless %}
          {% for label in member.labels %}
            <span class="mlabel">{{ label }}</span>
          {% endfor %}{% endspaceless %}
          </td></tr></table>
        {% endif %}
        </div>
        <div class="memdoc">
        {# write group include #}
          {% if compound.compoundKind=="module" and config.SHOW_GROUPED_MEMB_INC and member.file %}
            <p><tt>{% spaceless %}
            {% if compound.language=='java' or compound.language=='idl' %}
            import&#160; "
            {% else %}
            #include&#160;&lt;
            {% endif %}
            {% with obj=member.file text=member.file.bareName %}
              {% include 'htmlobjlink.tpl' %}
            {% endwith %}
            {% if compound.language=='java' or compound.language=='idl' %}"
            {% else %}&gt;
            {% endif %}
            {% endspaceless %}</tt></p>
          {% endif %}
        {# multi-line initializer #}
          {% if member.hasMultiLineInitializer %}
            <b>{% if member.isDefine %}{{ tr.defineValue }}{% else %}{{ tr.initialValue }}{% endif %}</b>
            <div class="fragment">{{ member.initializerAsCode }}</div>
          {% endif %}
        {# brief description #}
          {% if member.brief and config.REPEAT_BRIEF and config.BRIEF_MEMBER_DESC %}
            <p>{{ member.brief }}</p>
          {% endif %}
        {# detailed description #}
          {# TODO: VHDL #}
          {{ member.details }}
        {# inbody description #}
          {{ member.inbodyDocs }}
        {# argument list #}
          {{ member.paramDocs }}
        {# enum values #}
          {% if member.isEnumeration and member.enumValues|length>0 %}
            <table class="fieldtable">
            <tr><th colspan="2">{{ tr.enumValues }}</th></tr>
            {% for enumVal in member.enumValues %}
              <tr><td class="fieldname"><em><a class="anchor" id="{{ enumVal.anchor}}"></a>{{ enumVal.name }}</em>&#160;</td>
                  <td class="fielddoc">{{ enumVal.brief }}{{ enumVal.details }}</td>
              </tr>
            {% endfor %}
            </table>
          {% endif %}
        {# reimplements #}
          {% if member.reimplements %}
            <p>
            {% markers mem in member.reimplements with tr.reimplements %}
              {% with obj=mem text=mem.class.name %}
                {% include 'htmlobjlink.tpl' %}
              {% endwith %}
            {% endmarkers %}
            </p>
          {% endif %}
          {% if member.implements %}
            <p>
            {% markers mem in member.implements with tr.implements %}
              {% with obj=mem text=mem.class.name %}
                {% include 'htmlobjlink.tpl' %}
              {% endwith %}
            {% endmarkers %}
            </p>
          {% endif %}
        {# reimplementedBy #}
          {% if member.reimplementedBy %}
            <p>
            {% markers mem in member.reimplementedBy with tr.reimplementedBy:member.reimplementedBy|length %}
              {% with obj=mem text=mem.class.name %}
                {% include 'htmlobjlink.tpl' %}
              {% endwith %}
            {% endmarkers %}
            </p>
          {% endif %}
          {% if member.implementedBy %}
            <p>
            {% markers mem in member.implementedBy with tr.implementedBy:member.implementedBy|length %}
              {% with obj=mem text=mem.class.name %}
                {% include 'htmlobjlink.tpl' %}
              {% endwith %}
            {% endmarkers %}
            </p>
          {% endif %}
        {# category relation #}
          {% if member.class and member.categoryRelation %}
            {% if member.category %}
              <p>
              {% markers mem in member.categoryRelation|list with tr.providedByCategory %}
                {% with obj=mem text=member.category.name %}
                  {% include 'htmlobjlink.tpl' %}
                {% endwith %}
              {% endmarkers %}
              </p>
            {% elif member.class.categoryOf %}
              <p>
              {% markers mem in member.categoryRelation|list with tr.extendsClass %}
                {% with obj=mem text=member.class.categoryOf.name %}
                  {% include 'htmlobjlink.tpl' %}
                {% endwith %}
              {% endmarkers %}
              </p>
            {% endif %}
          {% endif %}
        {# examples #}
          {% if member.examples %}
            <dl><dt><b>{{ tr.examples }}</b><dd>
            {% markers obj in member.examples with tr.exampleList:member.examples|length %}
              {% with text=obj.text %}
                {% include 'htmlobjlink.tpl' %}
              {% endwith %}
            {% endmarkers %}
            </dd></dl>
          {% endif %}
        {# type constraints #}
          {% with obj=member %}
            {% include 'htmltypeconstraints.tpl' %}
          {% endwith %}
        {# source def #}
          {% if member.sourceDef %}
            {% markers obj in member.sourceDef with tr.definedAtLineInSourceFile %}
              {% with text=obj.text %}
                {% include 'htmlobjlink.tpl' %}
              {% endwith %}
            {% endmarkers %}
          {% endif %}
        {# source refs #}
          {% if member.sourceRefs|length>0 %}
            <p>
            {% markers mem in member.sourceRefs with tr.sourceRefs:member.sourceRefs|length %}
              {% if mem.sourceDef and config.REFERENCES_LINK_SOURCE %}
                {% with obj=mem.sourceDef.0 text=mem.name|append:mem.functionQualifier %}
                  {% include 'htmlobjlink.tpl' %}
                {% endwith %}
              {% else %}
                {% with obj=mem text=mem.name|append:mem.functionQualifier %}
                  {% include 'htmlobjlink.tpl' %}
                {% endwith %}
              {% endif %}
            {% endmarkers %}
            </p>
          {% endif %}
        {# source refs by #}
          {% if member.sourceRefBys|length>0%}
            <p>
            {% markers mem in member.sourceRefBys with tr.sourceRefBys:member.sourceRefBys|length %}
              {% if mem.sourceDef and config.REFERENCES_LINK_SOURCE %}
                {% with obj=mem.sourceDef.0 text=mem.name|append:mem.functionQualifier %}
                  {% include 'htmlobjlink.tpl' %}
                {% endwith %}
              {% else %}
                {% with obj=mem text=mem.name|append:mem.functionQualifier %}
                  {% include 'htmlobjlink.tpl' %}
                {% endwith %}
              {% endif %}
            {% endmarkers %}
            </p>
          {% endif %}
        {# inline code #}
          {% if member.hasSources and config.INLINE_SOURCES %}
            <div class="fragment">
            {{ member.sourceCode }}
            </div>
          {% endif %}
        {# call graph #}
          {% if member.hasCallGraph %}
             {% with obj=member %}
               {% include 'htmldynheader.tpl' %}
             {% endwith %}
               {{ tr.callGraph }}
             </div>
             {% with obj=member %}
               {% include 'htmldyncontents.tpl' %}
             {% endwith %}
               {{ member.callGraph }}
             </div>
          {% endif %}
        {# caller graph #}
          {% if member.hasCallerGraph %}
             {% with obj=member %}
               {% include 'htmldynheader.tpl' %}
             {% endwith %}
               {{ tr.callerGraph }}
             </div>
             {% with obj=member %}
               {% include 'htmldyncontents.tpl' %}
             {% endwith %}
               {{ member.callerGraph }}
             </div>
          {% endif %}
        </div>
        </div>
      {% endif %}
    {% endfor %} {# for each member #}
  {% endif %}
{% endif %}

