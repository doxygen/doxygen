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

#endif
