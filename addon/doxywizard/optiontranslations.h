#ifndef OPTIONTRANSLATIONS_H
#define OPTIONTRANSLATIONS_H

#include <QString>
#include <QMap>
#include <QObject>

class OptionTranslations : public QObject
{
    Q_OBJECT

public:
    static OptionTranslations& instance();
    QString translate(const QString &optionName);
    static QString trStatic(const QString &optionName);
    void retranslate();

private:
    OptionTranslations() {}
    Q_DISABLE_COPY(OptionTranslations)
    
    void initTranslations();
    QMap<QString, QString> m_translations;
    bool m_initialized = false;
};

#endif // OPTIONTRANSLATIONS_H
