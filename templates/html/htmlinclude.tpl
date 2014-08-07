{# input: ii with attributes (file,name,isImport,isLocal), compound with attribute language #}
{% spaceless %}
  {% if ii.file or ii.name %}
    <tt>
    {% if compound.language=='java' or compound.language=='idl' %}
      import&#160;
    {%else %}
      {% if ii.isImport %}
      #import&#160;
      {% else %}
      #include&#160;
      {% endif %}
    {%endif %}
    {% if ii.isLocal %}"{% else %}&lt;{% endif %}
      {% if ii.name %}
        {% if ii.file %}
          <a class="el" href="{{ ii.file.sourceFileName }}{{ config.HTML_FILE_EXTENSION }}">{{ ii.name }}</a>
        {% else %}
          {{ ii.name }}
        {% endif %}
      {% else %}
       <a class="el" href="{{ ii.file.sourceFileName }}{{ config.HTML_FILE_EXTENSION }}">{{ ii.file.name }}</a>
      {% endif %}
    {% if ii.isLocal %}"{% else %}&gt;{% endif %}
    </tt>
  {% endif %}
{% endspaceless %}
