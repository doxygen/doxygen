/******************************************************************************
 *
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

#ifndef CONFIG_H
#define CONFIG_H

#ifndef DOXYWIZARD
#include "qtbc.h"
#endif
#include <qstrlist.h>
#include <qfile.h>

extern void parseConfig(const QCString &config,const char *fn);
extern void writeTemplateConfig(QFile *f,bool shortList);
extern void checkConfig();
extern void configStrToVal();
extern void substituteEnvironmentVars();

struct Config
{
  static void init();

#CONFIG Header
};

#endif
