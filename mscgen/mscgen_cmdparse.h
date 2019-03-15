/***************************************************************************
 *
 * $Id: cmdparse.h 59 2009-07-18 17:31:50Z Michael.McTernan $
 *
 * This file is part of mscgen, a message sequence chart renderer.
 * Copyright (C) 2005 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
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

#ifndef CMDPARSE_HEADER
#define CMDPARSE_HEADER

/***************************************************************************
 * Include Files
 **************************************************************************/

#include "bool.h"

/***************************************************************************
 * Type Definitions
 **************************************************************************/

/** Command switch description
 * This structure defines a single switch that maybe present on the command
 * line, as well as how the switch should be handled.
 */
typedef struct CmdSwitchTag
{
    char    *switchString;  /**< String that identifies the switch */
    Boolean *presentFlag;   /**< If the switch is found, this is set TRUE */
    char    *parseString;   /**< If non-NULL, used to parse switch option */
    void    *parseResult;   /**< Destination of parse result */
}
CmdSwitch;

/***************************************************************************
 * Global Functions
 **************************************************************************/

/** Parse command line options.
 * When supplied with command line options and a suitable array of
 * \a CmdSwitch instructions, this will attempt to parse the command line
 * and return \a TRUE if no errors were found.
 * \param opts   Array of \a CmdSwitch types describing possible options
 *                and how to parse them.
 * \param nOpts  The number of elements in \a opts.
 * \param argc   The number of elements in \a argv.
 * \param argv   The array of command options.
 * \param inputSwitch  Special switch to assume incase the last input
 *                      option is unmatched.  In such a case, the
 *                      switch table for this option is used.
 * \retval Boolean  \a TRUE if all options were successfully parsed.
 */
Boolean CmdParse(const CmdSwitch opts[],
                 const int       nOpts,
                 const int       argc,
                 const char      *argv[],
                 const char      *inputSwitch);

#endif

/* END OF FILE */
