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
    struct LanguageData {
        const char *code;
        const char *nativeName;
        const char *englishName;
    };
    
    static const LanguageData languageTable[] = {
        { "en", "English", "English" },
        { "zh_CN", "简体中文", "Simplified Chinese" },
        { "zh_TW", "繁體中文", "Traditional Chinese" },
        { "de", "Deutsch", "German" },
        { "fr", "Français", "French" },
        { "ja", "日本語", "Japanese" },
        { "ko", "한국어", "Korean" },
        { "es", "Español", "Spanish" },
        { "ru", "Русский", "Russian" },
        { nullptr, nullptr, nullptr }
    };
    
    for (int i = 0; languageTable[i].code != nullptr; ++i)
    {
        LanguageInfo info;
        info.code = QString::fromLatin1(languageTable[i].code);
        info.nativeName = QString::fromUtf8(languageTable[i].nativeName);
        info.englishName = QString::fromLatin1(languageTable[i].englishName);
        
        if (info.code == QLatin1String("en"))
        {
            info.tsFile = QString();
            info.qmFile = QString();
            info.optionsQmFile = QString();
        }
        else
        {
            info.tsFile = QString::fromLatin1("doxywizard_%1.ts").arg(info.code);
            info.qmFile = QString::fromLatin1("doxywizard_%1.qm").arg(info.code);
            info.optionsQmFile = QString::fromLatin1("doxywizard_options_%1.qm").arg(info.code);
        }
        
        m_languages.insert(info.code, info);
    }
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
    
    // Note: Options translations are now handled via localized config_xx.xml files
    // The doxywizard_options_xx.qm files are no longer loaded here
    
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
    
    // Note: m_optionsTranslator is no longer used
    // Options translations are handled via localized config_xx.xml files
    
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
