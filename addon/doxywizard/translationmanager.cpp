#include "translationmanager.h"

#include <QApplication>
#include <QLocale>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
#include <QCoreApplication>
#include <QLibraryInfo>

TranslationManager& TranslationManager::instance()
{
    static TranslationManager instance;
    return instance;
}

TranslationManager::TranslationManager()
    : m_translator(nullptr)
    , m_qtTranslator(nullptr)
    , m_optionsTranslator(nullptr)
    , m_currentLangCode(QString::fromLatin1("en"))
    , m_initialized(false)
{
    loadAvailableLanguages();
}

TranslationManager::~TranslationManager()
{
    unloadTranslation();
}

void TranslationManager::initialize()
{
    if (m_initialized) return;
    
    QSettings settings(QString::fromLatin1("Doxygen.org"), QString::fromLatin1("Doxywizard"));
    QString savedLang = settings.value(QString::fromLatin1("language/code")).toString();
    
    if (!savedLang.isEmpty() && isLanguageAvailable(savedLang))
    {
        switchLanguage(savedLang);
    }
    else
    {
        switchToSystemLanguage();
    }
    
    m_initialized = true;
}

void TranslationManager::loadAvailableLanguages()
{
    LanguageInfo en;
    en.code = QString::fromLatin1("en");
    en.nativeName = QString::fromLatin1("English");
    en.englishName = QString::fromLatin1("English");
    en.tsFile = QString();
    en.qmFile = QString();
    en.optionsQmFile = QString();
    m_languages.insert(en.code, en);

    LanguageInfo zhCN;
    zhCN.code = QString::fromLatin1("zh_CN");
    zhCN.nativeName = QString::fromUtf8("简体中文");
    zhCN.englishName = QString::fromLatin1("Simplified Chinese");
    zhCN.tsFile = QString::fromLatin1("doxywizard_zh_CN.ts");
    zhCN.qmFile = QString::fromLatin1("doxywizard_zh_CN.qm");
    zhCN.optionsQmFile = QString::fromLatin1("doxywizard_options_zh_CN.qm");
    m_languages.insert(zhCN.code, zhCN);

    LanguageInfo zhTW;
    zhTW.code = QString::fromLatin1("zh_TW");
    zhTW.nativeName = QString::fromUtf8("繁體中文");
    zhTW.englishName = QString::fromLatin1("Traditional Chinese");
    zhTW.tsFile = QString::fromLatin1("doxywizard_zh_TW.ts");
    zhTW.qmFile = QString::fromLatin1("doxywizard_zh_TW.qm");
    zhTW.optionsQmFile = QString::fromLatin1("doxywizard_options_zh_TW.qm");
    m_languages.insert(zhTW.code, zhTW);

    LanguageInfo de;
    de.code = QString::fromLatin1("de");
    de.nativeName = QString::fromLatin1("Deutsch");
    de.englishName = QString::fromLatin1("German");
    de.tsFile = QString::fromLatin1("doxywizard_de.ts");
    de.qmFile = QString::fromLatin1("doxywizard_de.qm");
    de.optionsQmFile = QString::fromLatin1("doxywizard_options_de.qm");
    m_languages.insert(de.code, de);

    LanguageInfo fr;
    fr.code = QString::fromLatin1("fr");
    fr.nativeName = QString::fromUtf8("Français");
    fr.englishName = QString::fromLatin1("French");
    fr.tsFile = QString::fromLatin1("doxywizard_fr.ts");
    fr.qmFile = QString::fromLatin1("doxywizard_fr.qm");
    fr.optionsQmFile = QString::fromLatin1("doxywizard_options_fr.qm");
    m_languages.insert(fr.code, fr);

    LanguageInfo ja;
    ja.code = QString::fromLatin1("ja");
    ja.nativeName = QString::fromUtf8("日本語");
    ja.englishName = QString::fromLatin1("Japanese");
    ja.tsFile = QString::fromLatin1("doxywizard_ja.ts");
    ja.qmFile = QString::fromLatin1("doxywizard_ja.qm");
    ja.optionsQmFile = QString::fromLatin1("doxywizard_options_ja.qm");
    m_languages.insert(ja.code, ja);

    LanguageInfo ko;
    ko.code = QString::fromLatin1("ko");
    ko.nativeName = QString::fromUtf8("한국어");
    ko.englishName = QString::fromLatin1("Korean");
    ko.tsFile = QString::fromLatin1("doxywizard_ko.ts");
    ko.qmFile = QString::fromLatin1("doxywizard_ko.qm");
    ko.optionsQmFile = QString::fromLatin1("doxywizard_options_ko.qm");
    m_languages.insert(ko.code, ko);

    LanguageInfo es;
    es.code = QString::fromLatin1("es");
    es.nativeName = QString::fromUtf8("Español");
    es.englishName = QString::fromLatin1("Spanish");
    es.tsFile = QString::fromLatin1("doxywizard_es.ts");
    es.qmFile = QString::fromLatin1("doxywizard_es.qm");
    es.optionsQmFile = QString::fromLatin1("doxywizard_options_es.qm");
    m_languages.insert(es.code, es);

    LanguageInfo ru;
    ru.code = QString::fromLatin1("ru");
    ru.nativeName = QString::fromUtf8("Русский");
    ru.englishName = QString::fromLatin1("Russian");
    ru.tsFile = QString::fromLatin1("doxywizard_ru.ts");
    ru.qmFile = QString::fromLatin1("doxywizard_ru.qm");
    ru.optionsQmFile = QString::fromLatin1("doxywizard_options_ru.qm");
    m_languages.insert(ru.code, ru);
}

QStringList TranslationManager::availableLanguages() const
{
    return m_languages.keys();
}

QString TranslationManager::currentLanguage() const
{
    if (m_languages.contains(m_currentLangCode))
    {
        return m_languages[m_currentLangCode].nativeName;
    }
    return QString::fromLatin1("English");
}

QString TranslationManager::currentLanguageCode() const
{
    return m_currentLangCode;
}

TranslationManager::LanguageInfo TranslationManager::languageInfo(const QString &langCode) const
{
    if (m_languages.contains(langCode))
    {
        return m_languages[langCode];
    }
    return LanguageInfo();
}

bool TranslationManager::isLanguageAvailable(const QString &langCode) const
{
    return m_languages.contains(langCode);
}

QString TranslationManager::systemLanguageCode() const
{
    return detectSystemLanguage();
}

QString TranslationManager::fallbackLanguageCode() const
{
    return QString::fromLatin1("en");
}

QString TranslationManager::detectSystemLanguage() const
{
    QString systemLocale = QLocale::system().name();
    QString langCode = systemLocale.left(2);
    
    if (systemLocale == QString::fromLatin1("zh_CN") || systemLocale == QString::fromLatin1("zh_SG"))
    {
        return QString::fromLatin1("zh_CN");
    }
    else if (systemLocale.startsWith(QString::fromLatin1("zh")))
    {
        return QString::fromLatin1("zh_TW");
    }
    else if (m_languages.contains(langCode))
    {
        return langCode;
    }
    else if (m_languages.contains(systemLocale))
    {
        return systemLocale;
    }
    
    return fallbackLanguageCode();
}

bool TranslationManager::switchLanguage(const QString &langCode)
{
    if (!m_languages.contains(langCode))
    {
        emit languageSwitchFailed(langCode, tr("Language not available"));
        return false;
    }
    
    if (langCode == m_currentLangCode)
    {
        return true;
    }
    
    if (langCode == QString::fromLatin1("en"))
    {
        unloadTranslation();
        m_currentLangCode = langCode;
        
        QSettings settings(QString::fromLatin1("Doxygen.org"), QString::fromLatin1("Doxywizard"));
        settings.setValue(QString::fromLatin1("language/code"), langCode);
        
        emit languageChanged(langCode);
        return true;
    }
    
    QString qmPath = qmFilePath(langCode);
    if (qmPath.isEmpty())
    {
        emit languageSwitchFailed(langCode, tr("Translation file not found"));
        return false;
    }
    
    unloadTranslation();
    
    m_translator = new QTranslator(this);
    if (!m_translator->load(qmPath))
    {
        delete m_translator;
        m_translator = nullptr;
        emit languageSwitchFailed(langCode, tr("Failed to load translation file"));
        return false;
    }
    qApp->installTranslator(m_translator);
    
    QString qmOptionsPath = qmOptionsFilePath(langCode);
    if (!qmOptionsPath.isEmpty())
    {
        m_optionsTranslator = new QTranslator(this);
        if (m_optionsTranslator->load(qmOptionsPath))
        {
            qApp->installTranslator(m_optionsTranslator);
        }
        else
        {
            delete m_optionsTranslator;
            m_optionsTranslator = nullptr;
        }
    }
    
    m_qtTranslator = new QTranslator(this);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QString qtQmPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath) + 
                       QDir::separator() + QString::fromLatin1("qtbase_") + langCode + QString::fromLatin1(".qm");
#else
    QString qtQmPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath) + 
                       QDir::separator() + QString::fromLatin1("qtbase_") + langCode + QString::fromLatin1(".qm");
#endif
    if (QFileInfo::exists(qtQmPath))
    {
        m_qtTranslator->load(qtQmPath);
        qApp->installTranslator(m_qtTranslator);
    }
    else
    {
        delete m_qtTranslator;
        m_qtTranslator = nullptr;
    }
    
    m_currentLangCode = langCode;
    
    QSettings settings(QString::fromLatin1("Doxygen.org"), QString::fromLatin1("Doxywizard"));
    settings.setValue(QString::fromLatin1("language/code"), langCode);
    
    emit languageChanged(langCode);
    return true;
}

void TranslationManager::switchToSystemLanguage()
{
    QString sysLang = detectSystemLanguage();
    switchLanguage(sysLang);
}

void TranslationManager::switchToFallbackLanguage()
{
    switchLanguage(fallbackLanguageCode());
}

bool TranslationManager::loadTranslation(const QString &qmFilePath)
{
    if (!m_translator)
    {
        m_translator = new QTranslator(this);
    }
    
    if (m_translator->load(qmFilePath))
    {
        qApp->installTranslator(m_translator);
        return true;
    }
    
    return false;
}

void TranslationManager::unloadTranslation()
{
    if (m_translator)
    {
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
    
    if (m_optionsTranslator)
    {
        qApp->removeTranslator(m_optionsTranslator);
        delete m_optionsTranslator;
        m_optionsTranslator = nullptr;
    }
    
    if (m_qtTranslator)
    {
        qApp->removeTranslator(m_qtTranslator);
        delete m_qtTranslator;
        m_qtTranslator = nullptr;
    }
}

QString TranslationManager::qmFilePath(const QString &langCode) const
{
    if (!m_languages.contains(langCode))
    {
        return QString();
    }
    
    const LanguageInfo &info = m_languages[langCode];
    if (info.qmFile.isEmpty())
    {
        return QString();
    }
    
    QString resourcePath = QString::fromLatin1(":/translations/") + info.qmFile;
    if (QFileInfo::exists(resourcePath))
    {
        return resourcePath;
    }
    
    QString appDir = QCoreApplication::applicationDirPath();
    QString qmPath = appDir + QDir::separator() + info.qmFile;
    
    if (QFileInfo::exists(qmPath))
    {
        return qmPath;
    }
    
    return QString();
}

QString TranslationManager::tsFilePath(const QString &langCode) const
{
    if (!m_languages.contains(langCode))
    {
        return QString();
    }
    
    return m_languages[langCode].tsFile;
}

QString TranslationManager::qmOptionsFilePath(const QString &langCode) const
{
    if (!m_languages.contains(langCode))
    {
        return QString();
    }
    
    const LanguageInfo &info = m_languages[langCode];
    if (info.optionsQmFile.isEmpty())
    {
        return QString();
    }
    
    QString resourcePath = QString::fromLatin1(":/translations/") + info.optionsQmFile;
    if (QFileInfo::exists(resourcePath))
    {
        return resourcePath;
    }
    
    QString appDir = QCoreApplication::applicationDirPath();
    QString qmPath = appDir + QDir::separator() + info.optionsQmFile;
    
    if (QFileInfo::exists(qmPath))
    {
        return qmPath;
    }
    
    return QString();
}
