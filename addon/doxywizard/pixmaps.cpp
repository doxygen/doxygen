/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include "pixmaps.h"

static const char * const folder_xpm_data[] =
{
  "16 16 6 1",
  ". c None",
  "b c #ffff00",
  "d c #000000",
  "* c #999999",
  "a c #cccccc",
  "c c #ffffff",
  "................",
  "................",
  "..*****.........",
  ".*ababa*........",
  "*abababa******..",
  "*cccccccccccc*d.",
  "*cbababababab*d.",
  "*cabababababa*d.",
  "*cbababababab*d.",
  "*cabababababa*d.",
  "*cbababababab*d.",
  "*cabababababa*d.",
  "*cbababababab*d.",
  "**************d.",
  ".dddddddddddddd.",
  "................"
};
const char **folder_xpm = (const char **)folder_xpm_data;

static const char* const file_xpm_data[] =
{
  "16 16 5 1",
  ". c #7f7f7f",
  "# c None",
  "c c #000000",
  "b c #bfbfbf",
  "a c #ffffff",
  "################",
  "#..........#####",
  "#.aaaaaaaab.####",
  "#.aaaaaaaaba.###",
  "#.aaaaaaaacccc##",
  "#.aaaaaaaaaabc##",
  "#.aaaaaaaaaabc##",
  "#.aaaaaaaaaabc##",
  "#.aaaaaaaaaabc##",
  "#.aaaaaaaaaabc##",
  "#.aaaaaaaaaabc##",
  "#.aaaaaaaaaabc##",
  "#.aaaaaaaaaabc##",
  "#.bbbbbbbbbbbc##",
  "#ccccccccccccc##",
  "################"
};
const char **file_xpm = (const char **)file_xpm_data;

static const char * const add_xpm_data[] =
{
  "16 16 5 1",
  ". c None",
  "* c #0328f9",
  "# c #354396",
  "a c #353740",
  "c c #999999",
  "................",
  "......###.......",
  "......#*ac......",
  "......#*ac......",
  "......#*ac......",
  "......#*ac......",
  ".######*a#####..",
  ".#***********ac.",
  ".#aaaaa*aaaaaac.",
  "..cccc#*acccccc.",
  "......#*ac......",
  "......#*ac......",
  "......#*ac......",
  "......#aac......",
  ".......ccc......",
  "................"
};
const char **add_xpm = (const char **)add_xpm_data;

static const char * const del_xpm_data[] =
{
  "16 16 5 1",
  ". c None",
  "* c #0328f9",
  "# c #354396",
  "a c #353740",
  "c c #999999",
  "................",
  "................",
  "................",
  "................",
  "................",
  "................",
  ".#############..",
  ".#***********ac.",
  ".aaaaaaaaaaaaac.",
  "..ccccccccccccc.",
  "................",
  "................",
  "................",
  "................",
  "................",
  "................"
};
const char **del_xpm = (const char **)del_xpm_data;

static char *update_xpm_data[] = 
{
  "16 16 5 1",
  /* colors */
  ". c #0328f9",
  "# c #354396",
  "a c #353740",
  "b c None",
  "c c #999999",
  /* pixels */
  "bbbbbbbbbbbbbbbb",
  "bbbbbbbb#####acb",
  "bbbbbbbb#....abb",
  "bbc##cbb#...acbb",
  "bb#..abb#....abb",
  "bc#..abb#.a..acb",
  "b#..acbbaac#..ab",
  "b#..abbbcbb#..ab",
  "b#..abbbbbb#..ab",
  "b#..acbbbbc#..ab",
  "bc#..#cbbc#..acb",
  "bb#...####...acb",
  "bbca........acbb",
  "bbbbaa....aaccbb",
  "bbbbbcaaaaccbbbb",
  "bbbbbbbbbbbbbbbb"
};
const char **update_xpm = (const char **)update_xpm_data;


static const char *fileopen_xpm_data[] = 
{
  "16 13 5 1",
  ". c #040404",
  "# c #808304",
  "a c None",
  "b c #f3f704",
  "c c #f3f7f3",
  "aaaaaaaaa...aaaa",
  "aaaaaaaa.aaa.a.a",
  "aaaaaaaaaaaaa..a",
  "a...aaaaaaaa...a",
  ".bcb.......aaaaa",
  ".cbcbcbcbc.aaaaa",
  ".bcbcbcbcb.aaaaa",
  ".cbcb...........",
  ".bcb.#########.a",
  ".cb.#########.aa",
  ".b.#########.aaa",
  "..#########.aaaa",
  "...........aaaaa"
};
const char **fileopen_xpm = (const char **)fileopen_xpm_data;

/* XPM */
static const char *filesave_xpm_data[] = 
{
  "14 14 4 1",
  ". c #040404",
  "# c #808304",
  "a c #bfc2bf",
  "b c None",
  "..............",
  ".#.aaaaaaaa.a.",
  ".#.aaaaaaaa...",
  ".#.aaaaaaaa.#.",
  ".#.aaaaaaaa.#.",
  ".#.aaaaaaaa.#.",
  ".#.aaaaaaaa.#.",
  ".##........##.",
  ".############.",
  ".##.........#.",
  ".##......aa.#.",
  ".##......aa.#.",
  ".##......aa.#.",
  "b............."
};
const char **filesave_xpm = (const char **)filesave_xpm_data;
/* XPM */
static const char *rundoxygen_xpm_data[] = 
{
 "16 16 11 1",
" 	c None",
".	c #000000",
"+	c #DCDCDC",
"@	c #A0A0A0",
"#	c #C3C3C3",
"$	c #808080",
"%	c #FFA858",
"&	c #FFDCA8",
"*	c #FFFFC0",
"=	c #FFFFFF",
"-	c #585858",
"       ..       ",
"   .. .++. ..   ",
"  .+@.@##@.@+.  ",
"  .@+$@%%@$+@.  ",
"   .$%%&%&%$.   ",
" ..+@%&$$%&@+.. ",
".+#@%&%@@&*%@#+.",
".$@+$&*&&=*$+@$.",
" .--+$&*=&$+--. ",
"  .$#++$$++#$.  ",
" .@=$-$++$-$=@. ",
" .+@-..@@..-@+. ",
"  ... .+=. ...  ",
"      .-$.      ",
"       ..       ",
"                "
};

const char **rundoxygen_xpm = (const char **)rundoxygen_xpm_data;

