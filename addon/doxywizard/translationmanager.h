#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>
#include <QTranslator>
#include <QStringList>
#include <QMap>

class TranslationManager : public QObject
{
    Q_OBJECT

public:
    struct LanguageInfo
    {
        QString code;
        QString nativeName;
        QString englishName;
        QString tsFile;
        QString qmFile;
    };

    static TranslationManager& instance();
    
    void initialize();
    QStringList availableLanguages() const;
    QString currentLanguage() const;
    QString currentLanguageCode() const;
    LanguageInfo languageInfo(const QString &langCode) const;
    bool isLanguageAvailable(const QString &langCode) const;
    
    QString systemLanguageCode() const;
    QString fallbackLanguageCode() const;

public slots:
    bool switchLanguage(const QString &langCode);
    void switchToSystemLanguage();
    void switchToFallbackLanguage();

signals:
    void languageChanged(const QString &langCode);
    void languageSwitchFailed(const QString &langCode, const QString &error);

private:
    TranslationManager();
    ~TranslationManager();
    Q_DISABLE_COPY(TranslationManager)
    
    void loadAvailableLanguages();
    QString detectSystemLanguage() const;
    bool loadTranslation(const QString &qmFilePath);
    void unloadTranslation();
    QString qmFilePath(const QString &langCode) const;
    QString tsFilePath(const QString &langCode) const;

    QTranslator *m_translator;
    QTranslator *m_qtTranslator;
    QString m_currentLangCode;
    QMap<QString, LanguageInfo> m_languages;
    bool m_initialized;
};

#endif // TRANSLATIONMANAGER_H
