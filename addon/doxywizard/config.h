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

class Input;
class QTextStream;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
class QTextCodec;
#else
class QStringEncoder;
#endif

bool parseConfig(
      const QString &fileName,
      const QHash<QString,Input *> &options
    );

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void writeStringValue(QTextStream &t,QTextCodec *codec,const QString &s);
#else
void writeStringValue(QTextStream &t,QStringEncoder *codec,const QString &s);
#endif

// directly copied from ../../src/config.h to be consistent
enum
{
 /*! Maximum length of an option in the config file. Used for
  *  alignment purposes.
  */
  MAX_OPTION_LENGTH = 23
};

#endif
