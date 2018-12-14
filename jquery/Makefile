JQUERY_VERSION = 1.7.2
JQUERY_UI_VERSION = 1.8.18
HASHCHANGE_VERSION = 1.3
SCROLL_VERSION = 1.4.2
POWERTIP_VERSION = 1.2.0
TOUCHPUNCH_VERSION = 0.2.3
SMARTMENUS_VERSION = 1.0.0

MINIFIER ?= /usr/local/bin/yuicompressor-2.4.7
SCRIPTS = jquery-$(JQUERY_VERSION).js \
          jquery.ui-$(JQUERY_UI_VERSION).core.js \
          jquery.ui-$(JQUERY_UI_VERSION).widget.js \
          jquery.ui-$(JQUERY_UI_VERSION).mouse.js \
          jquery.ui-$(JQUERY_UI_VERSION).resizable.js \
          jquery.ba-$(HASHCHANGE_VERSION)-hashchange.js \
          jquery.scrollTo-$(SCROLL_VERSION).js \
	  jquery.powertip-$(POWERTIP_VERSION).js \
	  jquery.ui-$(TOUCHPUNCH_VERSION).touch-punch.js \
	  jquery.smartmenus-$(SMARTMENUS_VERSION).js
RESULTS = jquery.js doxmenu-min.css

SCRIPTS_MIN = $(SCRIPTS:%.js=%-min.js)

all: $(RESULTS)

install: $(RESULTS)
	cp jquery.js ../templates/html/jquery.js
	cp doxmenu-min.css ../templates/html/tabs.css

jquery.js: $(SCRIPTS_MIN)
	cat $(SCRIPTS_MIN) > jquery.js

doxmenu-min.css: sm-core-css.css \
                 sass/sm-dox.scss \
                 sass/_round-corners-last-item.scss \
                 sass/_sm-dox.scss \
                 sass/_sub-items-indentation.scss
	compass compile --css-dir . --force sass/sm-dox.scss
	cat sm-core-css.css sm-dox.css > doxmenu.css
	java -jar $(MINIFIER).jar doxmenu.css > doxmenu-min.css
	rm -f sm-dox.css doxmenu.css

scripts: $(SCRIPTS_MIN)

clean:
	rm -rf $(SCRIPTS_MIN) $(RESULTS) doxmenu.css .sass-cache jquery.js

%-min.js: %.js
	java -jar $(MINIFIER).jar $^ > $@

