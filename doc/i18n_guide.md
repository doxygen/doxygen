\page i18n_guide Doxywizard Internationalization (i18n)

This document provides detailed instructions on how to manage multi-language support in the Doxywizard project.

\tableofcontents

\section i18n_adding Adding New Language Support

\subsection i18n_required Required Files

Adding a new language requires creating or modifying the following files:

| File | Description |
|------|-------------|
| `doxywizard_xx.ts` | Translation source file for the new language (required) |
| `translationmanager.cpp` | Register the new language (required) |
| `translations.qrc` | Add resource reference (required) |
| `CMakeLists.txt` | Add translation file reference (required) |

\subsection i18n_steps Detailed Steps

\subsubsection i18n_step1 Step 1: Create Translation Source File

1. Copy an existing translation file as a template:
   ```bash
   cp doxywizard_zh_CN.ts doxywizard_xx.ts
   ```
   Where `xx` is the language code (e.g., `pt` for Portuguese, `it` for Italian)

2. Modify the language attribute in the file header:
   ```xml
   <TS version="2.1" language="xx">
   ```

3. Translate all text within the `<translation>` tags

\subsubsection i18n_step2 Step 2: Register New Language

Add the new language in the `loadAvailableLanguages()` method in `translationmanager.cpp`:

```cpp
// Add in the loadAvailableLanguages() method
LanguageInfo xx;
xx.code = QString::fromLatin1("xx");
xx.nativeName = QString::fromLatin1("Language Native Name");
xx.englishName = QString::fromLatin1("Language English Name");
xx.tsFile = QString::fromLatin1("doxywizard_xx.ts");
xx.qmFile = QString::fromLatin1("doxywizard_xx.qm");
m_languages.insert(xx.code, xx);
```

\subsubsection i18n_step3 Step 3: Add to Resource File

Add to `translations.qrc`:
```xml
<file>doxywizard_xx.qm</file>
```

\subsubsection i18n_step4 Step 4: Add to CMakeLists.txt

Add to `DOXYWIZARD_TRANSLATION_FILES` in `CMakeLists.txt`:
```cmake
set(DOXYWIZARD_TRANSLATION_FILES
  ...
  doxywizard_xx.ts
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

3. **Maintain Consistency**: The same term should use the same translation in different places

4. **Translation Status**: Ensure there is no `type="unfinished"` attribute after translation

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

Configuration option translations are defined in `optiontranslations.cpp`.

\subsubsection i18n_config_table Step 1: Add to Translation Table

Add to the `s_optionTranslations` array in `optiontranslations.cpp`:

```cpp
static const struct {
    const char *key;
    const char *text;
} s_optionTranslations[] = {
    // ... existing entries
    { "NEW_OPTION", QT_TR_NOOP("New Option Description") },
    // ...
};
```

\subsubsection i18n_config_update Step 2: Update Translation Files

Rebuild the project to update the `.ts` files with the new strings.

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

\htmlonly
Go to the <a href="perlmod.html">next</a> section or return to the
 <a href="index.html">index</a>.
\endhtmlonly
