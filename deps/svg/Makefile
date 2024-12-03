SVGDOTJS_VERSION = 3.1.2
SVGPAN_VERSION = 1.2mod

MINIFIER ?= uglifyjs
RESULTS = svg.min.js svgpan.min.js

all: $(RESULTS)

install: $(RESULTS)
	cat svg.min.js svgpan.min.js > ../../templates/html/svg.min.js

svg.min.js: svg-$(SVGDOTJS_VERSION).js
	$(MINIFIER) --comments /^\!/ svg-$(SVGDOTJS_VERSION).js > svg.min.js

svgpan.min.js: svgpan-$(SVGPAN_VERSION).js
	$(MINIFIER) --comments /^\!/ svgpan-$(SVGPAN_VERSION).js > svgpan.min.js

clean:
	rm -rf $(RESULTS)

