/***************************************************************************
 *
 * $Id: cmdparse.c 79 2009-08-02 12:11:45Z Michael.McTernan $
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

/***************************************************************************
 * Include Files
 **************************************************************************/

#include <stdio.h>
#include <string.h>
#include "mscgen_cmdparse.h"
#include "mscgen_bool.h"

/***************************************************************************
 * Local Functions
 **************************************************************************/

/***************************************************************************
 *
 * Function:     findSwitch
 *
 * Parameters:   opts   Array of option switches to search.
 *               nOpts  Number of elements in supplied array.
 *               swt    The switch string to match.
 *
 * Returns:      NULL if the switch was not found, otherwise pointer to
 *                the element in opts[] which matched.
 *
 * Description:  Search an array of switch options for some matching switch
 *                and return a pointer to the matching element if found.
 *
 ***************************************************************************/
static const CmdSwitch *findSwitch(const CmdSwitch opts[],
                                   const int       nOpts,
                                   const char     *swt)
{
    int t;

    for(t = 0; t < nOpts; t++)
    {
        if(strncmp(opts[t].switchString, swt, strlen(opts[t].switchString)) == 0)
        {
            return &opts[t];
        }
    }

    return NULL;
}


/***************************************************************************
 * Global Functions
 **************************************************************************/

/***************************************************************************
 *
 * Function:     CmdParse
 *
 * Parameters:   opts        Options list to use to parse command options.
 *               nOpts       Length of opts array.
 *               argc        Count of arguments to parse.
 *               argv        Arguments to parse.
 *               inputSwitch Special switch to assume incase the last input
 *                            option is unmatched.  In such a case, the
 *                            switch table for this option is used.
 *
 * Returns:      TRUE if parsing suceeded.
 *
 * Description:  Parse some list of options according to the CmdSwitch
 *                array.
 *
 ***************************************************************************/
Boolean CmdParse(const CmdSwitch opts[],
                 const int       nOpts,
                 const int       argc,
                 const char     *argv[],
                 const char     *inputSwitch)
{
    Boolean lastOpt = FALSE;
    int     t;

    /* Parse supplied options in turn */
    for(t = 0; t < argc; t++)
    {
        /* Attempt to match the option */
        const CmdSwitch *swt = findSwitch(opts, nOpts, argv[t]);

        /* Assume last parameter is the trailing input filename */
        if(swt == NULL && t == argc - 1)
        {
            swt = findSwitch(opts, nOpts, inputSwitch);
            lastOpt = TRUE;
        }

        if(swt == NULL)
        {
            fprintf(stderr, "Unrecognised option '%s'\n", argv[t]);
            return FALSE;
        }
        /* Check if the option was a prefixed switch or a distinct argument */
        else if(strcmp(swt->switchString, argv[t]) == 0)
        {
            /* Indicate that the flag is present */
            *swt->presentFlag = TRUE;

            /* Check if an option needs parsing */
            if(swt->parseString != NULL)
            {
                /* Skip the switch */
                t++;

                /* Check that another parameter is available to parse */
                if(t >= argc)
                {
                    fprintf(stderr, "Switch '%s' requires a parameter\n", swt->switchString);
                    return FALSE;
                }

                /* Attempt a parse */
                if(sscanf(argv[t], swt->parseString, swt->parseResult) != 1)
                {
                    fprintf(stderr,
                            "Invalid or unparsable parameter to option '%s'\n",
                            swt->switchString);
                    return FALSE;
                }
            }
        }
        /* Option was a prefix */
        else
        {
            /* Indicate that the flag is present */
            *swt->presentFlag = TRUE;

            /* Check if an option needs parsing */
            if(swt->parseString != NULL)
            {
                const char *opt = argv[t];

                if(!lastOpt)
                    opt += strlen(swt->switchString);

                /* Attempt a parse */
                if(sscanf(opt, swt->parseString, swt->parseResult) != 1)
                {
                    fprintf(stderr,
                            "Invalid or unparsable parameter to option '%s'\n",
                            argv[t]);
                    return FALSE;
                }
            }
        }

    }

    return TRUE;
}


/* END OF FILE */
