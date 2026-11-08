// objective: test preprocessor token pasting with empty arguments
// config: EXTRACT_ALL=YES
// config: MACRO_EXPANSION=YES
// check: 148__empty__token__pasting_8cpp.xml
// check-preprocessor: ^[0-9]+ +.*- +> +- +>
// check-preprocessor: ^[0-9]+ +:2: +:22 22:
// check-preprocessor: ^[0-9]+ +EMPTY_PASTE_LEFT: +left\+ +\+right
// check-preprocessor: ^[0-9]+ +EMPTY_PASTE_RIGHT: +left\+ +\+right
// check-preprocessor: ^[0-9]+ +EMPTY_PASTE_CHAIN: +leftright
// check-preprocessor: ^[0-9]+ +EMPTY_PASTE_BOTH: +- +>
// check-preprocessor: ^[0-9]+ +EMPTY_PASTE_RESCAN: +expanded_identifier

/** \file */

#define VALUE expanded_identifier
#define EMPTY_WITH_LEADING_SPACE
#define f(x, y) :x: -y##> -##y>
#define g(x, y) :x: :y##2 2##y:
#define EMPTY_PASTE_LEFT(x) EMPTY_PASTE_LEFT: left+x##+right
#define EMPTY_PASTE_RIGHT(x) EMPTY_PASTE_RIGHT: left+##x+right
#define EMPTY_PASTE_CHAIN(x) EMPTY_PASTE_CHAIN: left##x##right
#define EMPTY_PASTE_BOTH(x, y) EMPTY_PASTE_BOTH: -x##y>
#define EMPTY_PASTE_RESCAN(x) EMPTY_PASTE_RESCAN: x##VALUE

 EMPTY_WITH_LEADING_SPACE
f(:,)
g(2, 2)
EMPTY_PASTE_LEFT()
EMPTY_PASTE_RIGHT()
EMPTY_PASTE_CHAIN()
EMPTY_PASTE_BOTH(,)
EMPTY_PASTE_RESCAN()
