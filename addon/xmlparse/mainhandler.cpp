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
 */

#include "mainhandler.h"

void MainHandler::startCompound(const QXmlAttributes& attrib)
{
  CompoundHandler *compHandler = new CompoundHandler(this);
  compHandler->startCompound(attrib);
  m_compounds.append(compHandler);
}

MainHandler::MainHandler()
{
  m_compounds.setAutoDelete(TRUE);
  addStartHandler("doxygen"); 
  addStartHandler("compounddef",this,&MainHandler::startCompound);
  addEndHandler("doxygen");
  addEndHandler("compounddef");
}

MainHandler::~MainHandler()
{
}


