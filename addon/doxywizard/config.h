#ifndef CONFIG_H
#define CONFIG_H

#include <QHash>
#include <QString>

class Input;
class QTextStream;
class QTextCodec;

bool parseConfig(
      const QString &fileName,
      const QHash<QString,Input *> &options
    );

void writeStringValue(QTextStream &t,QTextCodec *codec,const QString &s);

// directly copied from ../../src/config.h to be consistent
enum
{
 /*! Maximum length of an option in the config file. Used for
  *  alignment purposes.
  */
  MAX_OPTION_LENGTH = 23
};

#endif
