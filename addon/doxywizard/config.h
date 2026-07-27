/******************************************************************************
 *
 * Copyright (C) 1997-2019 by Dimitri van Heesch.
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

#include <QHash>
#include <QString>

#include "adapter.h"

class Input;
class QTextStream;

bool parseConfig(
      const QString &fileName,
      const QHash<QString,Input *> &options
    );

void writeStringValue(QTextStream &t,TextCodecAdapter *codec,const QString &s,bool convert);

// directly copied from ../../src/config.h to be consistent
enum
{
 /*! Maximum length of an option in the config file. Used for
  *  alignment purposes.
  */
  MAX_OPTION_LENGTH = 23
};

#endif
