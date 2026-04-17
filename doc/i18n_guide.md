\page i18n_guide Doxywizard Internationalization (i18n)

This document provides detailed instructions on how to manage multi-language support in the Doxywizard project.

\tableofcontents

\section i18n_overview Overview

Doxywizard uses Qt's internationalization (i18n) framework, implementing multi-language support through the `QCoreApplication::translate()` function and Qt Linguist tools. Translation files use XML format `.ts` files, which are compiled into `.qm` files and embedded in the application resources.

\section i18n_file_structure Translation File Structure

Translation files are located in the `translations/` directory. Each language consists of two files:

```
translations/
├── doxywizard_zh_CN.ts          # Main UI translation (Simplified Chinese)
├── doxywizard_options_zh_CN.ts  # Configuration option translation (Simplified Chinese)
├── doxywizard_zh_TW.ts          # Main UI translation (Traditional Chinese)
├── doxywizard_options_zh_TW.ts  # Configuration option translation (Traditional Chinese)
├── doxywizard_de.ts             # Main UI translation (German)
├── doxywizard_options_de.ts     # Configuration option translation (German)
├── ...                          # Other languages
└── doxywizard_options_ru.ts     # Configuration option translation (Russian)
```

\subsection i18n_file_types File Types

| File Type | Purpose | Translation Contexts |
|-----------|---------|---------------------|
| `doxywizard_xx.ts` | Main UI translation | `DoxygenWizard`, `Expert`, etc. |
| `doxywizard_options_xx.ts` | Configuration option detailed descriptions | `OptionDocs`, `Expert`, `OptionValue` |

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

set(DOXYWIZARD_OPTIONS_TRANSLATION_FILES
  ${TRANSLATIONS_DIR}/doxywizard_options_zh_CN.ts
  ${TRANSLATIONS_DIR}/doxywizard_options_zh_TW.ts
  ${TRANSLATIONS_DIR}/doxywizard_options_de.ts
  ${TRANSLATIONS_DIR}/doxywizard_options_fr.ts
  ${TRANSLATIONS_DIR}/doxywizard_options_ja.ts
  ${TRANSLATIONS_DIR}/doxywizard_options_ko.ts
  ${TRANSLATIONS_DIR}/doxywizard_options_es.ts
  ${TRANSLATIONS_DIR}/doxywizard_options_ru.ts
)
```

\section i18n_contexts Translation Contexts

\subsection i18n_main_contexts Main Translation Contexts

| Context | File Location | Purpose |
|---------|---------------|---------|
| `DoxygenWizard` | `doxywizard.cpp` | Main wizard interface text |
| `Expert` | `expert.cpp` | Expert mode interface and dynamic content |
| `OptionDocs` | `doxywizard_options_xx.ts` | Configuration option detailed descriptions |
| `OptionValue` | `doxywizard_options_xx.ts` | Enum value descriptions |
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
    .arg(SA("\\ref cfg_") + dependsOn.toLower() + SA(" \"") + dependsOn.toUpper() + SA("\""));
```

\section i18n_message_sharing Message Sharing System

\subsection i18n_sharing_goals Design Goals

The message sharing system aims to:
1. Reduce duplication of translation strings
2. Ensure the same text uses the same translation in different locations
3. Support translation of dynamic content

\subsection i18n_static_translation Static Translation Functions

The `OptionTranslations` class provides static functions for obtaining translations in static contexts:

```cpp
// optiontranslations.h
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
    QMap<QString, QString> m_translations;
    QMap<QString, QString> m_docsTranslations;
};
```

\subsection i18n_translation_tags Translation Tag System

For cases where dynamic translation content needs to be embedded in translation strings, use `%{tr:context:text}` tags:

```cpp
// Use tags in translation strings
QString docs = SA("%{tr:Expert:Possible values are:}");

// The translateTags function processes these tags
QString OptionTranslations::translateTags(const QString &text)
{
    QRegularExpression trPattern(QLatin1String("%\\{tr:([^:]+):([^}]+)\\}"));
    // ... process tags and translate
}
```

\subsection i18n_language_switch Language Switching Process

When switching languages, `TranslationManager`:
1. Unloads current translations
2. Loads new language translation files (main and options files)
3. Emits `languageChanged` signal
4. Triggers `retranslateUi` to update the interface

```cpp
// translationmanager.cpp
bool TranslationManager::switchLanguage(const QString &langCode)
{
    // Unload current translations
    unloadTranslation();
    
    // Load main translation file
    QString mainQmPath = QString::fromLatin1(":/translations/doxywizard_%1.qm").arg(langCode);
    m_mainTranslator->load(mainQmPath);
    QCoreApplication::installTranslator(m_mainTranslator);
    
    // Load options translation file
    QString optionsQmPath = QString::fromLatin1(":/translations/doxywizard_options_%1.qm").arg(langCode);
    m_optionsTranslator->load(optionsQmPath);
    QCoreApplication::installTranslator(m_optionsTranslator);
    
    m_currentLangCode = langCode;
    emit languageChanged(langCode);
    return true;
}
```

\section i18n_config_options Configuration Option Documentation Multilingual Solution

\subsection i18n_config_architecture Architecture Overview

Configuration option detailed descriptions come from the `config.xml` file and require special multilingual handling:

```
config.xml → getDocsForNode() → Translation Processing → Display
```

\subsection i18n_config_flow Translation Flow

\subsubsection i18n_config_get Step 1: Get Original Documentation

The `getDocsForNode()` function reads original English documentation from XML:

```cpp
static QString getDocsForNode(const QDomElement &child)
{
    QString docs = SA("");
    QDomElement docsVal = child.firstChildElement();
    while (!docsVal.isNull())
    {
        if (docsVal.tagName() == SA("docs") &&
            docsVal.attribute(SA("doxywizard")) != SA("0"))
        {
            for (QDomNode n = docsVal.firstChild(); !n.isNull(); n = n.nextSibling())
            {
                QDomText t = n.toText();
                if (!t.isNull()) docs += t.data();
            }
        }
        docsVal = docsVal.nextSiblingElement();
    }
    // ...
}
```

\subsubsection i18n_config_translate Step 2: Translate Configuration Option Description

Use `OptionTranslations::trDocsStatic()` to translate configuration option descriptions:

```cpp
bool needTranslation = (TranslationManager::instance().currentLanguageCode() != QLatin1String("en"));

QString baseDocs = docs;
if (needTranslation)
{
    QString translatedDocs = OptionTranslations::trDocsStatic(id, docs);
    if (!translatedDocs.isEmpty() && translatedDocs != id)
    {
        baseDocs = translatedDocs;
    }
}
```

\subsubsection i18n_config_dynamic Step 3: Add Dynamic Content

Dynamic content (minimum, maximum, default values) is translated using `Expert::tr()`:

```cpp
// int type
if (type == SA("int"))
{
    docs = baseDocs;
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

\subsubsection i18n_config_refresh Step 4: Update on Language Switch

When switching languages, `retranslateUi()` regenerates documentation for all options:

```cpp
void Expert::retranslateUi()
{
    // ... update interface text
    
    QHashIterator<QString, Input*> i(m_options);
    while (i.hasNext())
    {
        i.next();
        if (i.value())
        {
            QString id = i.key();
            if (m_optionElements.contains(id))
            {
                QString docs = getDocsForNode(m_optionElements[id]);
                i.value()->setDocs(docs);
            }
            i.value()->retranslate();
        }
    }
}
```

\subsection i18n_config_format Translation File Format

\subsubsection i18n_optiondocs_context OptionDocs Context

Configuration option descriptions use the `OptionDocs` context:

```xml
<context>
    <name>OptionDocs</name>
    <message>
        <source>DOXYFILE_ENCODING</source>
        <translation>This tag specifies the encoding used for all characters...</translation>
    </message>
</context>
```

\subsubsection i18n_expert_context Expert Context

Dynamic content uses the `Expert` context with `%1` placeholders:

```xml
<context>
    <name>Expert</name>
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

\subsubsection i18n_optionvalue_context OptionValue Context

Enum value descriptions use the `OptionValue` context:

```xml
<context>
    <name>OptionValue</name>
    <message>
        <source>Generate HTML output</source>
        <translation>Generar salida HTML</translation>
    </message>
</context>
```

\section i18n_adding Adding New Language Support

\subsection i18n_required Required Files

Adding a new language requires creating or modifying the following files:

| File | Description |
|------|-------------|
| `doxywizard_xx.ts` | Translation source file for main UI (required) |
| `doxywizard_options_xx.ts` | Translation source file for configuration options (required) |
| `translationmanager.cpp` | Register the new language (required) |
| `CMakeLists.txt` | Add translation file reference (required) |

\subsection i18n_steps Detailed Steps

\subsubsection i18n_step1 Step 1: Create Translation Source Files

1. Copy existing translation files as templates:
   ```bash
   cp translations/doxywizard_zh_CN.ts translations/doxywizard_xx.ts
   cp translations/doxywizard_options_zh_CN.ts translations/doxywizard_options_xx.ts
   ```
   Where `xx` is the language code (e.g., `pt` for Portuguese, `it` for Italian)

2. Modify the language attribute in the file headers:
   ```xml
   <TS version="2.1" language="xx">
   ```

3. Translate all text within the `<translation>` tags

\subsubsection i18n_step2 Step 2: Register New Language

Add the new language in `translationmanager.cpp`:

```cpp
void TranslationManager::initLanguages()
{
    m_languages.clear();
    m_languages.insert(QString::fromLatin1("en"), tr("English"));
    // ... existing languages
    m_languages.insert(QString::fromLatin1("xx"), tr("Language Name"));
}
```

\subsubsection i18n_step3 Step 3: Add to CMakeLists.txt

Add to `CMakeLists.txt`:

```cmake
set(DOXYWIZARD_TRANSLATION_FILES
  # ... existing files
  ${TRANSLATIONS_DIR}/doxywizard_xx.ts
)

set(DOXYWIZARD_OPTIONS_TRANSLATION_FILES
  # ... existing files
  ${TRANSLATIONS_DIR}/doxywizard_options_xx.ts
)
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

1. Open the corresponding `doxywizard_xx.ts` or `doxywizard_options_xx.ts` file
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

\subsection i18n_config_strings Adding Configuration Option Strings

Configuration option translations are stored in `doxywizard_options_xx.ts` files.

\subsubsection i18n_config_add Step 1: Add to Translation File

Add to `doxywizard_options_xx.ts`:

```xml
<context>
    <name>OptionDocs</name>
    <message>
        <source>NEW_OPTION</source>
        <translation>New Option Description</translation>
    </message>
</context>
```

\subsubsection i18n_config_dynamic Add Dynamic Content Translations

For dynamic content like minimum/maximum values, add to the `Expert` context:

```xml
<context>
    <name>Expert</name>
    <message>
        <source>Minimum value: %1, maximum value: %2, default value: %3.</source>
        <translation>Min: %1, Max: %2, Default: %3.</translation>
    </message>
</context>
```

\subsection i18n_dynamic Adding Dynamic Refresh Support

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

Then call it in `MainWindow::retranslateUi()`:

```cpp
void MainWindow::retranslateUi()
{
    // ... other refresh code
    myClass->retranslateUi();
}
```

\section i18n_building Building and Verification

\subsection i18n_build Building the Project

\subsubsection i18n_build_windows Windows (using Ninja)

```cmd
cmake -S E:\path\to\doxygen -B build -Dbuild_wizard=ON -G Ninja
cmake --build build --target doxywizard
```

\subsubsection i18n_build_nmake Windows (using NMake)

```cmd
REM Open "x64 Native Tools Command Prompt for VS 2022"
cmake -S E:\path\to\doxygen -B build -Dbuild_wizard=ON -G "NMake Makefiles"
cmake --build build --target doxywizard
```

\subsubsection i18n_build_linux Linux/macOS

```bash
mkdir build && cd build
cmake -Dbuild_wizard=ON ..
make doxywizard
```

\subsection i18n_verify Verifying Translations

1. Run `doxywizard.exe`
2. Select Settings -> Language from the menu
3. Select the target language
4. Check that all interface text displays correctly

\subsection i18n_verify_resource Verifying Resource Embedding

Check if `.qm` files are correctly embedded in the executable:

```bash
# Windows
rsrc -list doxywizard.exe | findstr translations

# Linux
strings doxywizard | grep translations
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
- Missing `Expert` context translations in `doxywizard_options_xx.ts`
- HTML tags not properly escaped in translation strings

**Solutions**:
1. Ensure `Expert` context exists in `doxywizard_options_xx.ts`
2. Escape HTML tags: use `&lt;code&gt;` instead of `<code>`
3. Use `%1` placeholders for dynamic values

\htmlonly
Go to the <a href="perlmod.html">next</a> section or return to the
 <a href="index.html">index</a>.
\endhtmlonly
