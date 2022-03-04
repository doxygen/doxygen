[OPTIONS]
Compatibility=1.1
Full-text search=Yes
Contents file=index.hhc
Default Window=main
Default topic=index{{ config.HTML_FILE_EXTENSION }}
Index file=index.hhk
Language={{ tr.langString }}
{% if config.BINARY_TOC %}Binary TOC=YES{% endif %}
{% if config.GENERATE_CHI %}Create CHI file=YES{% endif %}
Title={{ config.PROJECT_NAME }}

[WINDOWS]
main="My Project","index.hhc","index.hhk","index{{ config.HTML_FILE_EXTENSION }}","index{{ config.HTML_FILE_EXTENSION }}",,,,,0x23520,,0x{% if config.BINARY_TOC %}7{% else %}1{% endif %}0387e,,,,,,,,0

[FILES]
{% for node in index.nav|flatten|listsort:config.HTML_FILE_EXTENSION|prepend:'{{file}}' %}
{% if not node.isReference and node.file %}{{ node.file }}{{ config.HTML_FILE_EXTENSION }}{% endif %}
{% endfor %}
tab_a.png
tab_b.png
tab_h.png
tab_s.png
nav_h.png
nav_f.png
bc_s.png
doxygen.svg
closed.png
open.png
bdwn.png
sync_on.png
sync_off.png
