\page i18n_guide Doxywizard Internationalization (i18n)

This document provides detailed instructions on how to manage multi-language support in the Doxywizard application.

\tableofcontents

\section i18n_overview Overview

Doxywizard uses Qt's internationalization (i18n) framework, 
implementing multi-language support through the `QCoreApplication::translate()` function and Qt Linguist tools. 
Translation files use XML format `.ts` files, which are compiled into `.qm` files and embedded in the application resources.

\section i18n_file_structure Translation File Structure

Translation files are located in the `translations/` directory:

```
addon/doxywizard/translations/
    doxywizard_zh_CN.ts          # Main UI translation (Simplified Chinese)
    doxywizard_zh_TW.ts          # Main UI translation (Traditional Chinese)
    doxywizard_de.ts             # Main UI translation (German)
    doxywizard_fr.ts             # Main UI translation (French)
    doxywizard_ja.ts             # Main UI translation (Japanese)
    doxywizard_ko.ts             # Main UI translation (Korean)
    doxywizard_es.ts             # Main UI translation (Spanish)
    doxywizard_ru.ts             # Main UI translation (Russian)
```

\section i18n_contexts Translation Contexts

There are several sections in the translation files, here is an overview of the purpose of each section.

| Context           | File Location      | Purpose                                   |
|-------------------|--------------------|-------------------------------------------|
| `MainWindow`      | `doxywizard.cpp`   | Main wizard interface text                |
| `Expert`          | `expert.cpp`       | Expert mode interface and dynamic content |
| `Messages`        | `doxywizard.cpp`   | Topic names and format labels             |
| `Wizard`          | `wizard.cpp`       | Wizard step labels                        |
| `Step1`           | `wizard.cpp`       | Step1 dialog labels                       |
| `Step2`           | `wizard.cpp`       | Step2 dialog labels                       |
| `Step3`           | `wizard.cpp`       | Step3 dialog labels                       |
| `Step4`           | `wizard.cpp`       | Step4 dialog labels                       |
| `TuneColorDialog` | `wizard.cpp`       | Color tuning dialog labels                |
| `InputString`     | `inputstring.cpp`  | String input control                      |
| `InputStrList`    | `inputstrlist.cpp` | String list input control                 |
| `HelpLabel`       |  `helplabel.h`     | Label with context menu                   |

\section i18n_config_options Configuration Option Translations

Configuration option are translated via a language specific version of `config.xml`

```
src/
    config.xml           # Original English configuration
src/translations/
    config_zh_CN.xml     # Simplified Chinese configuration
    config_zh_TW.xml     # Traditional Chinese configuration
    config_de.xml        # German configuration
    ...
```

These config files are directly included by Doxywizard and used to show the translated documentation for each option.

Note that obsolete options and documentation in `config.xml` that is meant for the manual or 
Doxyfile are not included in the `config_xx.xml` translation files.

\subsection i18n_config_sync Syncing Localized Config Files

When the original `config.xml` is updated (options added or removed), you need to sync the localized config files. 
The `configgen.py` with option `-sync` provides two modes:

**Report Mode (default)**: Only reports differences without modifying files:

```bash
cd src
python3 configgen.py -sync config.xml translations
```

**Auto-Sync Mode**: Automatically adds missing options and removes extra options:

```bash
cd src
python3 configgen.py -sync config.xml translations --auto
```

The `-sync` command will:
1. Read all option IDs from the original `config.xml`
2. Compare with each localized `translatos/config_xx.xml` file
3. Report missing options (exist in original but not in localized)
4. Report extra options (exist in localized but not in original)
5. In auto-sync mode: automatically add missing options and remove extra options

\note
- In auto-sync mode, a backup file (`.bak`) is created before modification
- Added options use the original English documentation; you need to translate them manually
- Existing synchronized options are not modified

\subsection i18n_config_format Translation File Format

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

\section i18n_adding Adding New Language Support

\subsection i18n_required Required Files

Adding a new language requires creating or modifying the following files:

| File                                             | Description                                   |
|--------------------------------------------------|-----------------------------------------------|
| `addon/doxywizard/translationmanager.cpp`        | Register the new language in languageTable    |
| `addon/doxywizard/translations/doxywizard_xx.ts` | Translation source file for user interface    |
| `src/translations/config_xx.xml`                 | Localized configuration option documentation  |

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
static const std::array languageTable = {
    { "en", "English", "English" },
    // ... existing languages
    { "xx", "Native Name", "English Name" }
};
```

\subsubsection i18n_step4 Step 3: Create Localized Config File

If you want to use localized config files:

1. Copy an existing config file:
   ```bash
   cp src/translations/config_de.xml src/translations/config_xx.xml
   ```

2. Translate the documentation strings within `<docs>` elements

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

**Step 1**: Use tr() in Code

Use the `tr()` function for strings that need translation:

```cpp
// Before
QLabel *label = new QLabel("Some Text");

// After
QLabel *label = new QLabel(tr("Some Text"));
```

**Step 2**: Update Translation Files

The CMake build system automatically extracts translatable strings during the build process. After adding new `tr()` calls, rebuild the project to update the `.ts` files.

**Step 3**: Translate New Strings

Translate the newly added strings in each language's `.ts` file.

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
