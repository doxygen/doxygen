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
    QString translateDocs(const QString &optionName, const QString &defaultDocs);
    static QString trStatic(const QString &optionName);
    static QString trDocsStatic(const QString &optionName, const QString &defaultDocs);
    static QString translateTags(const QString &text);
    void retranslate();

private:
    OptionTranslations() {}
    Q_DISABLE_COPY(OptionTranslations)
    
    void initTranslations();
    void initDocsTranslations();
    QMap<QString, QString> m_translations;
    QMap<QString, QString> m_docsTranslations;
    bool m_initialized = false;
    bool m_docsInitialized = false;
};

#endif // OPTIONTRANSLATIONS_H
