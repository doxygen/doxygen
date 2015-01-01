{% msg %}----- Start generating LaTeX output for {{ config.PROJECT_NAME }} from template ----{% endmsg %}

{% create 'refman.tex' from 'latexrefman.tpl' %}
{% create 'Makefile' from 'latexmakefile.tpl' %}

{# module index #}
{% if moduleTree.tree %}
  {% create 'modules.tex' from 'latexmodules.tpl' %}
{% endif %}

{# namespace index #}
{% if namespaceTree.tree %}
  {% create 'namespaces.tex' from 'latexnamespaces.tpl' %}
{% endif %}

{# class index #}
{% if classTree.tree %}
  {% create 'annotated.tex' from 'latexannotated.tpl' %}
{% endif %}

{# file index #}
{% if fileTree.tree %}
  {% create 'files.tex' from 'latexfiles.tpl' %}
{% endif %}

{# TODO: pages #}
{# TODO: examples #}
{# TODO: directories #}

{# write class documentation pages #}
{% for compound in classList %}
  {% create compound.fileName|append:'.tex' from 'latexclass.tpl' %}
{% endfor %}

{% msg %}----- End generating LaTeX output for {{ config.PROJECT_NAME }} from template ----{% endmsg %}
