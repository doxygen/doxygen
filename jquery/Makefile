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
RESULTS = jquery_p1.js jquery_p2.js jquery_p3.js \
          jquery_ui.js jquery_fx.js jquery_pt.js

SCRIPTS_MIN = $(SCRIPTS:%.js=%-min.js)

all: $(RESULTS)

install: $(RESULTS)
	cp $(RESULTS) ../src/

jquery_ui.js: scripts
	cat jquery.ui-$(JQUERY_UI_VERSION).core-min.js \
            jquery.ui-$(JQUERY_UI_VERSION).widget-min.js \
            jquery.ui-$(JQUERY_UI_VERSION).mouse-min.js \
            jquery.ui-$(JQUERY_UI_VERSION).resizable-min.js \
            jquery.ba-$(HASHCHANGE_VERSION)-hashchange-min.js > jquery_ui.js

jquery_pt.js: scripts
	cat jquery.powertip-$(POWERTIP_VERSION)-min.js > jquery_pt.js

jquery_fx.js: scripts
	cat jquery.scrollTo-$(SCROLL_VERSION)-min.js > jquery_fx.js

jquery_p1.js jquery_p2.js jquery_p3.js: scripts
	perl split_jquery.pl jquery-$(JQUERY_VERSION)-min.js $@

scripts: $(SCRIPTS_MIN)

clean:
	rm -f $(SCRIPTS_MIN) $(RESULTS)

%-min.js: %.js
	java -jar $(MINIFIER).jar --line-break 13000 $^ > $@

