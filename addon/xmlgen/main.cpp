/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include "doxygen.h"
#include "message.h"
#include "xml.h"

/*! \file
 *  \brief main entry point for doxygen's XML generator
 *
 *  This file contains main()
 */

/*! Main that generate XML output
 *  
 */
int main(int argc,char **argv)
{
  initDoxygen();
  readConfiguration(argc,argv);
  parseInput();
  msg("Generating XML output\n");
  generateXML();
  return 0;
}
