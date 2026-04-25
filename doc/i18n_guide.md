\page i18n_guide Doxywizard Internationalization (i18n)

This document provides detailed instructions on how to manage multi-language support in the Doxywizard project.

\tableofcontents

\section i18n_overview Overview

Doxywizard uses Qt's internationalization (i18n) framework, implementing multi-language support through the `QCoreApplication::translate()` function and Qt Linguist tools. Translation files use XML format `.ts` files, which are compiled into `.qm` files and embedded in the application resources.

\section i18n_file_structure Translation File Structure

Translation files are located in the `translations/` directory:

```
translations/
├── doxywizard_zh_CN.ts          # Main UI translation (Simplified Chinese)
├── doxywizard_zh_TW.ts          # Main UI translation (Traditional Chinese)
├── doxywizard_de.ts             # Main UI translation (German)
├── doxywizard_fr.ts             # Main UI translation (French)
├── doxywizard_ja.ts             # Main UI translation (Japanese)
├── doxywizard_ko.ts             # Main UI translation (Korean)
├── doxywizard_es.ts             # Main UI translation (Spanish)
└── doxywizard_ru.ts             # Main UI translation (Russian)
```

\subsection i18n_file_types File Types

| File Type | Purpose | Translation Contexts |
|-----------|---------|---------------------|
| `doxywizard_xx.ts` | Main UI translation | `DoxygenWizard`, `Expert`, `Messages`, etc. |

\subsection i18n_cmake_config CMakeLists.txt Configuration

```cmake
set(DOXYWIZARD_TRANSLATION_FILES
  ${TRANSLATIONS_DIR}/doxywizard_zh_CN.ts
  ${TRANSLATIONS_DIR}/doxywizard_zh_TW.ts
  ${TRANSLATIONS_DIR}/doxywizard_de.ts
  ${TRANSLATIONS_DIR}/doxywizard_fr.ts
  ${TRANSLATIONS_DIR}/doxywizard_ja.ts
  ${TRANSLATIONS_DIR}/doxywizard_ko.ts
  ${TRANSLATIONS_DIR}/doxywizard_es.ts
  ${TRANSLATIONS_DIR}/doxywizard_ru.ts
)
```

\section i18n_contexts Translation Contexts

\subsection i18n_main_contexts Main Translation Contexts

| Context | File Location | Purpose |
|---------|---------------|---------|
| `DoxygenWizard` | `doxywizard.cpp` | Main wizard interface text |
| `Expert` | `expert.cpp` | Expert mode interface and dynamic content |
| `Messages` | `doxywizard.cpp` | Topic names and format labels |
| `Wizard` | `wizard.cpp` | Wizard step labels |
| `InputBool` | `inputbool.cpp` | Boolean input control |
| `InputInt` | `inputint.cpp` | Integer input control |
| `InputString` | `inputstring.cpp` | String input control |

\subsection i18n_dynamic_content Dynamic Content Translation

In `expert.cpp`, dynamic content is translated using `Expert::tr()`:

```cpp
// Minimum, maximum, default values
docs += Expert::tr("Minimum value: %1, maximum value: %2, default value: %3.")
    .arg(minval).arg(maxval).arg(defval);

// Default value
docs += Expert::tr("The default value is: <code>%1</code>.").arg(defval);

// Dependencies
docs += Expert::tr("This tag requires that the tag %1 is set to <code>YES</code>.")
    .arg(dependsOn);
```

\section i18n_config_options Configuration Option Documentation Multilingual Solution

\subsection i18n_config_architecture Architecture Overview

Configuration option detailed descriptions support two multilingual approaches:

**Approach 1: Localized config.xml files**

```
src/
├── config.xml           # Original English configuration
├── config_zh_CN.xml     # Simplified Chinese configuration
├── config_zh_TW.xml     # Traditional Chinese configuration
├── config_de.xml        # German configuration
└── ...
```

**Approach 2: Translation files (.ts)**

Configuration option descriptions are translated directly in the main translation files using the `OptionDocs` context.

\subsection i18n_config_sync Syncing Localized Config Files

When the original `config.xml` is updated (options added or removed), you need to sync the localized config files. The `-sync` command provides two modes:

**Report Mode (default)**: Only reports differences without modifying files:

```bash
cd src
python configgen.py -sync config.xml /
```

**Auto-Sync Mode**: Automatically adds missing options and removes extra options:

```bash
cd src
python configgen.py -sync config.xml / --auto
```

The `-sync` command will:
1. Read all option IDs from the original `config.xml`
2. Compare with each localized `config_xx.xml` file
3. Report missing options (exist in original but not in localized)
4. Report extra options (exist in localized but not in original)
5. In auto-sync mode: automatically add missing options and remove extra options

\note 
- In auto-sync mode, a backup file (`.bak`) is created before modification
- Added options use the original English documentation; you need to translate them manually
- Existing synchronized options are not modified

\subsection i18n_config_flow Translation Flow

\subsubsection i18n_config_load Step 1: Load Language-Specific Config

The `Expert` class loads the appropriate config file based on current language:

```cpp
void Expert::loadConfigXml()
{
  QString langCode = TranslationManager::instance().currentLanguageCode();
  QString configPath;
  
  if (langCode != QLatin1String("en"))
  {
    configPath = SA(":/config_") + langCode + SA(".xml");
    QFile langFile(configPath);
    if (!langFile.exists())
    {
      configPath = SA(":/config.xml");
    }
  }
  else
  {
    configPath = SA(":/config.xml");
  }
  
  QFile file(configPath);
  // ... parse XML
  m_rootElement = configXml.documentElement();
}
```

\subsubsection i18n_config_dynamic_content Step 2: Add Dynamic Content

Dynamic content (minimum, maximum, default values) is translated using `Expert::tr()`:

```cpp
// int type
if (type == SA("int"))
{
    if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
    docs += SA("<br/>");
    QString minval = child.attribute(SA("minval"));
    QString maxval = child.attribute(SA("maxval"));
    QString defval = child.attribute(SA("defval"));
    docs += Expert::tr("Minimum value: %1, maximum value: %2, default value: %3.")
        .arg(minval).arg(maxval).arg(defval);
}

// bool type
if (type == SA("bool"))
{
    QString defval = child.attribute(SA("defval"));
    QString valStr = (defval == SA("1") ? SA("YES") : SA("NO"));
    docs += Expert::tr("The default value is: <code>%1</code>.").arg(valStr);
}
```

\subsubsection i18n_config_refresh Step 3: Update on Language Switch

When switching languages, `retranslateUi()` reloads the config and rebuilds the UI:

```cpp
void Expert::retranslateUi()
{
    m_treeWidget->setHeaderLabels(QStringList() << DoxygenWizard::msgTopicsHeader());
    m_prev->setText(DoxygenWizard::msgPreviousButton());
    m_next->setText(DoxygenWizard::msgNextButton());
    
    loadConfigXml();
    
    m_topics.clear();
    while (m_topicStack->count() > 0)
    {
        QWidget *w = m_topicStack->widget(0);
        m_topicStack->removeWidget(w);
    }
    m_options.clear();
    m_optionElements.clear();
    
    m_treeWidget->clear();
    
    createTopics(m_rootElement);
    
    m_treeWidget->setCurrentItem(m_treeWidget->invisibleRootItem()->child(0));
}
```

\subsection i18n_config_format Translation File Format

\subsubsection i18n_expert_context Expert Context

Dynamic content uses the `Expert` context with `%1` placeholders:

```xml
<context>
    <name>Expert</name>
    <message>
        <source>Possible values are:</source>
        <translation>Mögliche Werte sind:</translation>
    </message>
    <message>
        <source>and</source>
        <translation>und</translation>
    </message>
    <message>
        <source>Minimum value: %1, maximum value: %2, default value: %3.</source>
        <translation>Mindestwert: %1, Höchstwert: %2, Standardwert: %3.</translation>
    </message>
    <message>
        <source>The default value is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Der Standardwert ist: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
</context>
```

\note In XML files, `<code>` tags need to be escaped as `&lt;code&gt;` and `&lt;/code&gt;`.

\section i18n_message_sharing Message Sharing System

\subsection i18n_sharing_goals Design Goals

The message sharing system aims to:
1. Reduce duplication of translation strings
2. Ensure the same text uses the same translation in different locations
3. Support translation of dynamic content

\subsection i18n_static_translation Static Translation Functions

The `OptionTranslations` class provides static functions for obtaining translations in static contexts:

```cpp
class OptionTranslations : public QObject
{
    Q_OBJECT
public:
    static OptionTranslations& instance();
    QString translate(const QString &optionName);
    static QString trStatic(const QString &optionName);
    void retranslate();
private:
    QMap<QString, QString> m_translations;
};
```

\subsection i18n_language_switch Language Switching Process

When switching languages, `TranslationManager`:
1. Unloads current translations
2. Loads new language translation files
3. Emits `languageChanged` signal
4. Triggers `retranslateUi` to update the interface

```cpp
bool TranslationManager::switchLanguage(const QString &langCode)
{
    unloadTranslation();
    
    QString qmPath = qmFilePath(langCode);
    m_translator = new QTranslator(this);
    if (!m_translator->load(qmPath))
    {
        delete m_translator;
        m_translator = nullptr;
        return false;
    }
    qApp->installTranslator(m_translator);
    
    m_currentLangCode = langCode;
    emit languageChanged(langCode);
    return true;
}
```

\section i18n_adding Adding New Language Support

\subsection i18n_required Required Files

Adding a new language requires creating or modifying the following files:

| File | Description |
|------|-------------|
| `doxywizard_xx.ts` | Translation source file for main UI (required) |
| `translationmanager.cpp` | Register the new language (required) |
| `CMakeLists.txt` | Add translation file reference (required) |

Optionally, you can also create:
| File | Description |
|------|-------------|
| `config_xx.xml` | Localized configuration file (optional) |
| `doxywizard.qrc` | Add config file reference (if using localized config) |

\subsection i18n_steps Detailed Steps

\subsubsection i18n_step1 Step 1: Create Translation Source File

1. Copy existing translation file as template:
   ```bash
   cp translations/doxywizard_zh_CN.ts translations/doxywizard_xx.ts
   ```
   Where `xx` is the language code (e.g., `pt` for Portuguese, `it` for Italian)

2. Modify the language attribute in the file header:
   ```xml
   <TS version="2.1" language="xx">
   ```

3. Translate all text within the `<translation>` tags

\subsubsection i18n_step2 Step 2: Register New Language

Add the new language in `translationmanager.cpp`:

```cpp
static const LanguageData languageTable[] = {
    { "en", "English", "English" },
    { "zh_CN", "简体中文", "Simplified Chinese" },
    // ... existing languages
    { "xx", "Native Name", "English Name" },
    { nullptr, nullptr, nullptr }
};
```

\subsubsection i18n_step3 Step 3: Add to CMakeLists.txt

Add to `CMakeLists.txt`:

```cmake
set(DOXYWIZARD_TRANSLATION_FILES
  # ... existing files
  ${TRANSLATIONS_DIR}/doxywizard_xx.ts
)
```

\subsubsection i18n_step4 Step 4: (Optional) Create Localized Config File

If you want to use localized config files:

1. Copy the original config file:
   ```bash
   cp src/config.xml src/config_xx.xml
   ```

2. Translate the documentation strings within `<docs>` elements

3. Add to `doxywizard.qrc`:
   ```xml
   <file alias="config_xx.xml">../../src/config_xx.xml</file>
   ```

\subsection i18n_codes Language Code Standards

Use ISO 639-1 language codes, and ISO 3166-1 country codes for regional variants.

**Authoritative References:**
- ISO 639-1 (Language Codes): https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
- ISO 3166-1 (Country Codes): https://en.wikipedia.org/wiki/ISO_3166-1

| Code | Language |
|------|----------|
| `zh_CN` | Simplified Chinese |
| `zh_TW` | Traditional Chinese |
| `de` | German |
| `fr` | French |
| `ja` | Japanese |
| `ko` | Korean |
| `es` | Spanish |
| `ru` | Russian |
| `pt` | Portuguese |
| `it` | Italian |

\section i18n_modifying Modifying Individual String Translations

\subsection i18n_structure Translation File Structure

Translation files (`.ts`) use XML format:

```xml
<context>
    <name>ClassName</name>
    <message>
        <source>Original English Text</source>
        <translation>Translated Text</translation>
    </message>
</context>
```

\subsection i18n_edit Direct Edit of `.ts` File

1. Open the corresponding `doxywizard_xx.ts` file
2. Find the `<message>` entry to modify
3. Modify the text within the `<translation>` tag
4. Save the file

\subsection i18n_notes Important Notes

1. **Preserve Placeholders**: If the original text contains `%1`, `%2`, etc. placeholders, they must be preserved in the translation
   ```xml
   <source>Language changed to: %1</source>
   <translation>Language changed to: %1</translation>
   ```

2. **Escape Special Characters**: XML special characters need to be escaped
   | Character | Escape Sequence |
   |-----------|-----------------|
   | \< | `&lt;` |
   | \> | `&gt;` |
   | \& | `&amp;` |
   | '  | `&apos;` |
   | \" | `&quot;` |

3. **HTML Tag Escaping**: In `.ts` files, HTML tags must be escaped:
   | Original | Escaped |
   |----------|---------|
   | `<code>` | `&lt;code&gt;` |
   | `</code>` | `&lt;/code&gt;` |
   | `<br/>` | `&lt;br/&gt;` |

4. **Maintain Consistency**: The same term should use the same translation in different places

5. **Translation Status**: Ensure there is no `type="unfinished"` attribute after translation

\section i18n_adding_strings Adding New Strings in Doxywizard

\subsection i18n_ui_strings Adding UI Strings

\subsubsection i18n_tr Step 1: Use tr() in Code

Use the `tr()` function for strings that need translation:

```cpp
// Before
QLabel *label = new QLabel("Some Text");

// After
QLabel *label = new QLabel(tr("Some Text"));
```

\subsubsection i18n_update Step 2: Update Translation Files

The CMake build system automatically extracts translatable strings during the build process. After adding new `tr()` calls, rebuild the project to update the `.ts` files.

Alternatively, you can manually extract strings using CMake:

```bash
cmake --build build --target update_translations
```

\subsubsection i18n_translate Step 3: Translate New Strings

Translate the newly added strings in each language's `.ts` file.

\subsection i18n_dynamic_refresh Adding Dynamic Refresh Support

If you need to refresh text when switching languages, implement the `retranslateUi()` method:

```cpp
class MyClass : public QWidget
{
    Q_OBJECT
public:
    void retranslateUi();
};

void MyClass::retranslateUi()
{
    m_label->setText(tr("Some Text"));
    m_button->setText(tr("Button Text"));
}
```

Then connect to the `languageChanged` signal:

```cpp
connect(&TranslationManager::instance(), SIGNAL(languageChanged(QString)),
        this, SLOT(retranslateUi()));
```

\section i18n_issues Common Issues and Solutions

\subsection i18n_not_displaying Translations Not Displaying

**Problem**: After switching languages, some text still shows in English

**Causes**:
- `retranslateUi()` not called
- String not wrapped with `tr()` function
- Translation missing in `.ts` file

**Solutions**:
1. Ensure strings are wrapped with `tr()`
2. Check if the translation exists in the `.ts` file
3. Ensure `retranslateUi()` is called when switching languages

\subsection i18n_dynamic_not_displaying Dynamic Content Not Displaying

**Problem**: Minimum value, maximum value, default value not showing in non-English languages

**Causes**:
- Missing `Expert` context translations in main translation file
- HTML tags not properly escaped in translation strings

**Solutions**:
1. Ensure `Expert` context exists in `doxywizard_xx.ts`
2. Escape HTML tags: use `&lt;code&gt;` instead of `<code>`
3. Use `%1` placeholders for dynamic values

\htmlonly
Go to the <a href="perlmod.html">next</a> section or return to the
 <a href="index.html">index</a>.
\endhtmlonly
