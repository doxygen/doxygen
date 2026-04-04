# Doxywizard Internationalization (i18n) Development Guide

This document provides detailed instructions on how to manage multi-language support in the Doxywizard project.

## Table of Contents

1. [Adding New Language Support](#1-adding-new-language-support)
2. [Modifying Individual String Translations](#2-modifying-individual-string-translations)
3. [Adding New Strings in Doxywizard](#3-adding-new-strings-in-doxywizard)
4. [Building and Verification](#4-building-and-verification)
5. [Common Issues and Solutions](#5-common-issues-and-solutions)

---

## 1. Adding New Language Support

### 1.1 Required Files

Adding a new language requires creating or modifying the following files:

| File | Description |
|------|-------------|
| `doxywizard_xx.ts` | Translation source file for the new language (required) |
| `translationmanager.cpp` | Register the new language (required) |
| `translations.qrc` | Add resource reference (required) |
| `CMakeLists.txt` | Add translation file reference (required) |

### 1.2 Detailed Steps

#### Step 1: Create Translation Source File

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

#### Step 2: Register New Language

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

#### Step 3: Add to Resource File

Add to `translations.qrc`:
```xml
<file>doxywizard_xx.qm</file>
```

#### Step 4: Add to CMakeLists.txt

Add to `DOXYWIZARD_TRANSLATION_FILES` in `CMakeLists.txt`:
```cmake
set(DOXYWIZARD_TRANSLATION_FILES
  ...
  doxywizard_xx.ts
)
```

### 1.3 Language Code Standards

Use ISO 639-1 language codes, and ISO 3166-1 country codes for regional variants:

| Code | Language |
|------|----------|
| zh_CN | Simplified Chinese |
| zh_TW | Traditional Chinese |
| de | German |
| fr | French |
| ja | Japanese |
| ko | Korean |
| es | Spanish |
| ru | Russian |
| pt | Portuguese |
| it | Italian |

---

## 2. Modifying Individual String Translations

### 2.1 Translation File Structure

Translation files (.ts) use XML format:

```xml
<context>
    <name>ClassName</name>
    <message>
        <source>Original English Text</source>
        <translation>Translated Text</translation>
    </message>
</context>
```

### 2.2 Modification Steps

#### Method 1: Direct Edit of .ts File

1. Open the corresponding `doxywizard_xx.ts` file
2. Find the `<message>` entry to modify
3. Modify the text within the `<translation>` tag
4. Save the file

#### Method 2: Using Qt Linguist

1. Open Qt Linguist
2. Load the `doxywizard_xx.ts` file
3. Select context and message on the left side
4. Enter the new translation in the translation area
5. Save the file

### 2.3 Important Notes

1. **Preserve Placeholders**: If the original text contains `%1`, `%2`, etc. placeholders, they must be preserved in the translation
   ```xml
   <source>Language changed to: %1</source>
   <translation>Language changed to: %1</translation>
   ```

2. **Escape Special Characters**: XML special characters need to be escaped
   | Character | Escape Sequence |
   |-----------|-----------------|
   | `<` | `&lt;` |
   | `>` | `&gt;` |
   | `&` | `&amp;` |
   | `'` | `&apos;` |
   | `"` | `&quot;` |

3. **Maintain Consistency**: The same term should use the same translation in different places

4. **Translation Status**: Ensure there is no `type="unfinished"` attribute after translation

---

## 3. Adding New Strings in Doxywizard

### 3.1 Adding UI Strings

#### Step 1: Use tr() in Code

Use the `tr()` function for strings that need translation:

```cpp
// Before
QLabel *label = new QLabel("Some Text");

// After
QLabel *label = new QLabel(tr("Some Text"));
```

#### Step 2: Update Translation Files

Run lupdate to extract new strings:

```bash
lupdate addon/doxywizard/*.cpp addon/doxywizard/*.h -ts addon/doxywizard/doxywizard_zh_CN.ts
```

Or use CMake build, which will update automatically.

#### Step 3: Translate New Strings

Translate the newly added strings in each language's .ts file.

### 3.2 Adding Configuration Option Strings

Configuration option translations are defined in `optiontranslations.cpp`.

#### Step 1: Add to Translation Table

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

#### Step 2: Update Translation Files

After running lupdate, the new string will appear in the OptionTranslations context.

### 3.3 Adding Dynamic Refresh Support

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

---

## 4. Building and Verification

### 4.1 Building the Project

#### Using Qt Creator

1. Open Qt Creator
2. Open the project file `CMakeLists.txt`
3. Configure the project with Qt 6 (or Qt 5)
4. Add `-Dbuild_wizard=ON` to the CMake configuration
5. Build the project (Ctrl+B)

#### Using Command Line (Windows)

```cmd
REM Open "x64 Native Tools Command Prompt for VS 2022"
cmake -S E:\path\to\doxygen -B build -Dbuild_wizard=ON -G Ninja
cmake --build build --target doxywizard
```

#### Using Command Line (Linux/macOS)

```bash
mkdir build && cd build
cmake -Dbuild_wizard=ON ..
make doxywizard
```

### 4.2 Verifying Translations

1. Run doxywizard.exe
2. Select Settings -> Language from the menu
3. Select the target language
4. Check that all interface text displays correctly

### 4.3 Verifying Resource Embedding

Check if .qm files are correctly embedded in the exe:

```bash
# Windows
rsrc -list doxywizard.exe | findstr translations

# Linux
strings doxywizard | grep translations
```

---

## 5. Common Issues and Solutions

### 5.1 Translations Not Displaying

**Problem**: After switching languages, some text still shows in English

**Causes**:
- `retranslateUi()` not called
- String not wrapped with `tr()` function
- Translation missing in .ts file

**Solutions**:
1. Ensure strings are wrapped with `tr()`
2. Check if the translation exists in the .ts file
3. Ensure `retranslateUi()` is called when switching languages


---

## Appendix: File Structure

```
addon/doxywizard/
├── CMakeLists.txt          # Build configuration
├── translations.qrc        # Translation resource file
├── translationmanager.h    # Translation manager header
├── translationmanager.cpp  # Translation manager implementation
├── optiontranslations.h    # Configuration option translation header
├── optiontranslations.cpp  # Configuration option translation implementation
├── doxywizard_zh_CN.ts     # Simplified Chinese translation
├── doxywizard_zh_TW.ts     # Traditional Chinese translation
├── doxywizard_de.ts        # German translation
├── doxywizard_fr.ts        # French translation
├── doxywizard_ja.ts        # Japanese translation
├── doxywizard_ko.ts        # Korean translation
├── doxywizard_es.ts        # Spanish translation
└── doxywizard_ru.ts        # Russian translation
```
