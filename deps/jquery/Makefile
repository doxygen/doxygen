JQUERY_VERSION = 3.6.0
JQUERY_UI_VERSION = 1.13.2
SCROLL_VERSION = 2.1.2
POWERTIP_VERSION = 1.3.1
TOUCHPUNCH_VERSION = 0.2.3
SMARTMENUS_VERSION = 1.1.0

UGLIFYCSS ?= /usr/local/bin/uglifycss
UGLIFYJS ?= /usr/local/bin/uglifyjs
SCRIPTS = jquery-$(JQUERY_VERSION).min.js \
          jquery.ui-$(JQUERY_UI_VERSION).min.js \
          jquery.scrollTo-$(SCROLL_VERSION).min.js \
	  jquery.powertip-$(POWERTIP_VERSION).mod.min.js \
	  jquery.ui.touch-punch-$(TOUCHPUNCH_VERSION).min.js \
	  jquery.smartmenus-$(SMARTMENUS_VERSION).min.js
RESULTS = jquery.js doxmenu-min.css

all: $(RESULTS)

install: $(RESULTS)
	cp jquery.js ../../templates/html/jquery.js
	cp doxmenu-min.css ../../templates/html/tabs.css

jquery.js: $(SCRIPTS)
	cat $(SCRIPTS) > jquery.js

doxmenu-min.css: sm-core-css.css \
                 sass/sm-dox.scss \
                 sass/_round-corners-last-item.scss \
                 sass/_sm-dox.scss \
                 sass/_sub-items-indentation.scss
	compass compile --css-dir . --force sass/sm-dox.scss
	cat sm-core-css.css sm-dox.css > doxmenu.css
	$(UGLIFYCSS) doxmenu.css > doxmenu-min.css
	$(UGLIFYJS) -O max_line_len=1024,comments=true jquery.js > jquery-min.js
	mv jquery-min.js jquery.js
	rm -f sm-dox.css doxmenu.css

clean:
	rm -rf $(RESULTS) doxmenu.css .sass-cache

