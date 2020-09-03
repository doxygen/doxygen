/***************************************************************************
 *
 * $Id: main.c 75 2009-07-26 14:45:59Z Michael.McTernan $
 *
 * This file is part of mscgen, a message sequence chart renderer.
 * Copyright (C) 2010 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 **************************************************************************/

/***************************************************************************
 * Include Files
 ***************************************************************************/

#include "mscgen_config.h"
#include <stdio.h>
#include "mscgen_usage.h"

/***************************************************************************
 * Types
 ***************************************************************************/

/***************************************************************************
 * Local Variables.
 ***************************************************************************/

/***************************************************************************
 * Functions
 ***************************************************************************/

/** Print program usage and return.
 */
void Usage(void)
{
    printf(
"Usage: mscgen -T <type> [-o <file>] [-i] <infile>\n"
"       mscgen -l\n"
"\n"
"Where:\n"
" -T <type>   Specifies the output file type, which maybe one of 'png', 'eps',\n"
"             'svg' or 'ismap'\n"
" -i <infile> The file from which to read input.  If omitted or specified as\n"
"              '-', input will be read from stdin.  The '-i' flag maybe\n"
"              omitted if <infile> is specified as the last option on the\n"
"              command line.\n"
" -o <file>   Write output to the named file.  This option must be specified if \n"
"              input is taken from stdin, otherwise the output filename\n"
"              defaults to <infile>.<type>.  This may also be specified as '-'\n"
"              to write output directly to stdout.\n"
#ifdef USE_FREETYPE
" -F <font>   Use specified font for PNG output.  This must be a font specifier\n"
"              compatible with fontconfig (see 'fc-list'), and overrides the\n"
"              MSCGEN_FONT environment variable if also set.\n"
#endif
" -p          Print parsed msc output (for parser debug).\n"
" -l          Display program licence and exit.\n"
"\n"
"Mscgen version %s, Copyright (C) 2010 Michael C McTernan,\n"
"                                        Michael.McTernan.2001@cs.bris.ac.uk\n"
"Mscgen comes with ABSOLUTELY NO WARRANTY.  This is free software, and you are\n"
"welcome to redistribute it under certain conditions; type `mscgen -l' for\n"
"details.\n"
"\n"
"PNG rendering by libgd, www.libgd.org\n"
"\n",
PACKAGE_VERSION);
}


/** Print program licence and return.
 */
void Licence(void)
{
    printf(
"Mscgen, a message sequence chart renderer.\n"
"Copyright (C) 2010 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk\n"
"\n"
"TTPCom Ltd., hereby disclaims all copyright interest in the program `mscgen'\n"
"(which renders message sequence charts) written by Michael McTernan.\n"
"\n"
"Rob Meades of TTPCom Ltd, 1 August 2005\n"
"Rob Meades, director of Software\n"
"\n"
"This program is free software; you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation; either version 2 of the License, or\n"
"(at your option) any later version.\n"
"\n"
"This program is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.\n"
"\n"
"You should have received a copy of the GNU General Public License\n"
"along with this program; if not, write to the Free Software\n"
"Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA\n");
}

/* END OF FILE */
