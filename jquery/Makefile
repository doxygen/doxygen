SCRIPTS = ../node_modules/jquery/dist/jquery.slim.min.js \
          ../node_modules/jquery-ui-dist/jquery-ui.min.js \
          ../node_modules/jquery.scrollto/jquery.scrollTo.min.js \
          ../node_modules/tippy.js/dist/tippy-bundle.iife.js \
          ../node_modules/popper.js/dist/umd/popper.min.js \
          ../node_modules/jquery-ui-touch-punch/jquery.ui.touch-punch.min.js \
          ../node_modules/smartmenus/dist/jquery.smartmenus.min.js
RESULTS = bundle.js tabs.css

all: $(RESULTS)

install: $(RESULTS)
	cp bundle.js ../templates/html/.
	cp tabs.css ../templates/html/.

../node_modules:
	yarnpkg install

bundle.js: ../node_modules $(SCRIPTS)
	cat $(SCRIPTS) > bundle.js



tabs.css: sm-core-css.css \
                 sass/sm-dox.scss \
                 sass/_round-corners-last-item.scss \
                 sass/_sm-dox.scss \
                 sass/_sub-items-indentation.scss
	sassc sass/sm-dox.scss > sm-dox.css
	cat sm-core-css.css sm-dox.css > tabs.css
	rm -f sm-dox.css

clean:
	rm -rf $(RESULTS) .sass-cache ../node_modules
