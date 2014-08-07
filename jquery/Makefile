JQUERY_VERSION = 1.7.1
JQUERY_UI_VERSION = 1.8.18
HASHCHANGE_VERSION = 1.3
SCROLL_VERSION = 1.4.2
POWERTIP_VERSION = 1.2.0

MINIFIER ?= /usr/local/bin/yuicompressor-2.4.7
SCRIPTS = jquery-$(JQUERY_VERSION).js \
          jquery.ui-$(JQUERY_UI_VERSION).core.js \
          jquery.ui-$(JQUERY_UI_VERSION).widget.js \
          jquery.ui-$(JQUERY_UI_VERSION).mouse.js \
          jquery.ui-$(JQUERY_UI_VERSION).resizable.js \
          jquery.ba-$(HASHCHANGE_VERSION)-hashchange.js \
          jquery.scrollTo-$(SCROLL_VERSION).js \
	  jquery.powertip-$(POWERTIP_VERSION).js
RESULTS = jquery.js

SCRIPTS_MIN = $(SCRIPTS:%.js=%-min.js)

all: $(RESULTS)

install: $(RESULTS)
	cp $(RESULTS) ../templates/html/

jquery.js: scripts
	cat $(SCRIPTS_MIN) > jquery.js

scripts: $(SCRIPTS_MIN)

clean:
	rm -f $(SCRIPTS_MIN) $(RESULTS)

%-min.js: %.js
	java -jar $(MINIFIER).jar $^ > $@

