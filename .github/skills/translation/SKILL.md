---
name: translation
description: >
  Keeps all Doxygen and Doxywizard translations up to date across three mechanisms:
  translator C++ classes (src/translator_xx.h), Qt .ts locale files for the Doxywizard
  GUI (addon/doxywizard/i18n/), and config-option XML files (src/i18n/config_xx.xml).
  Use this skill when updating or adding a language translation, improving translation
  quality, handling new or changed English source strings, adding a new language, or
  synchronising translations after a Doxygen release.
compatibility: >
  Requires Python 3 (for doc/translator.py and src/configgen.py) and Git.
  Targets the doxygen/doxygen repository.
---

# Doxygen Translation Agent Skill

This document instructs a translation agent on how to keep all Doxygen and Doxywizard
translations up to date, improve translation quality, and handle changes to English source
strings.

## Overview of Translation Mechanisms

Doxygen uses **three separate, independent translation mechanisms**:

| Mechanism          | Source Files                             | Target Files                       | Purpose                                                                    |
|--------------------|------------------------------------------|------------------------------------|----------------------------------------------------------------------------|
| Translator classes | `src/translator_en.h`                    | `src/translator_xx.h`              | All text strings emitted by doxygen itself into generated documentation    |
| Qt `.ts` files     | `addon/doxywizard/i18n/doxywizard_xx.ts` | generated `doxywizard_xx.qm` files | Doxywizard GUI strings                                                     |
| Config XML files   | `src/config.xml`                         | `src/i18n/config_xx.xml`           | Documentation for configuration options shown in the Doxywizard Expert tab |

Each mechanism must be maintained independently. A change in the English text of a source file need to be translated to each of the corresponding Target Files.

---

## Mechanism 1 — Translator Classes (`src/translator_xx.h`)

### How it works

- `src/translator.h` defines the abstract `Translator` base class with pure virtual methods
  such as `trRelatedFunctions()`, `trClassHierarchy()`, etc.
- `src/translator_en.h` contains `TranslatorEnglish`, which is always fully up to date and
  is the authoritative source for English strings and their inline documentation comments.
- Each supported language has its own `src/translator_xx.h` file (e.g. `translator_de.h` for
  German, `translator_fr.h` for French) containing a class such as `TranslatorGerman` that
  overrides every method.
- `src/translator_adapter.h` defines adapter classes (`TranslatorAdapter_X_Y_Z`) that provide
  English fallback implementations for methods introduced in a given release. A language
  translator that is not fully up to date inherits from the appropriate adapter instead of
  directly from `Translator`.
- `src/language.cpp` registers all translators and `doc/maintainers.txt` lists the human maintainer
  for each language.

### Identifying what needs translation

Run the `doc/translator.py` script from the repository root to get a full report:

```bash
python doc/translator.py
```

This generates `translator_report.txt` and `language.dox`. The report shows:
- Which translators are fully up to date (inherit directly from `Translator`)
- Which translators are outdated (inherit from an adapter class) and which methods they are
  missing
- The percentage of implemented methods per language

To check a specific language only (e.g., German):

```bash
python doc/translator.py de
```

### Finding new or changed English strings

When commits touch `src/translator_en.h`:
1. Use `git log --follow -p src/translator_en.h` to see all changes.
2. New methods added to `TranslatorEnglish` are pure virtual in `Translator`; they must be
   implemented in every language file or the build will fail.
3. New methods are also added to `TranslatorAdapter_X_Y_Z` in `translator_adapter.h` as
   English fallbacks. A language translator can temporarily inherit from the new adapter;
   eventually it should provide a real translation.
4. Changed method signatures (parameter lists, return types) must be reflected in all language
   files that override the method.
5. Removed methods mean the corresponding overrides in language files can be deleted.

### How to update a language translator

1. **Locate the file**: `src/translator_xx.h`
2. **Check the base class**: If the class inherits from `TranslatorAdapter_X_Y_Z`, all
   methods listed in that adapter (and newer adapters) are missing and fall back to English.
3. **Add missing methods**: Copy the method signature from `TranslatorEnglish` and provide a
   translation. Use and copy the inline comments in `translator_en.h` to understand the context.
4. **Change the base class**: Once all methods are implemented, change the base class from
   `TranslatorAdapter_X_Y_Z` to `Translator` directly.
5. **Update the adapter**: If a new adapter class was added to `translator_adapter.h` for the
   current release, it must include the English fallback for each new method.

### Quality guidelines for translator classes

- Use domain-appropriate software-development terminology, not generic dictionary translations.
  For example, "class", "namespace", "template", "typedef", "enum" are technical terms often
  left in their English form or translated according to the established convention in the
  target language.
- Use correct accents, diacritics, and ligatures for the target language (e.g., German umlauts
  ä/ö/ü/ß, French accents é/è/ê/ë, etc.). Encode the file as UTF-8.
- Check that plural forms are grammatically correct; many methods have boolean parameters to
  select singular vs. plural.
- Check that sentences with embedded variable content (parameter substitution via `QCString`
  concatenation or format arguments) remain grammatically correct for all cases.
- Some methods vary their output based on config booleans such as `OPTIMIZE_OUTPUT_FOR_C`,
  `OPTIMIZE_OUTPUT_VHDL`, `OPTIMIZE_OUTPUT_SLICE`. Translate all branches.
- Keep formatting identical to `translator_en.h` (same indentation, same brace style).
- Add a comment at the top of the file with your name, date, and a brief changelog entry for
  the update.

### Adding a brand-new language

1. Copy `src/translator_en.h` to `src/translator_xx.h`.
2. Rename the class from `TranslatorEnglish` to `TranslatorYourLanguage`.
3. Change `idLanguage()` to return the language name in lowercase English (e.g., `"portuguese"`).
4. Change `trISOLang()` to return the BCP 47 / ISO language tag (e.g., `"pt-PT"`).
5. Change `getLanguageString()` to return the correct HTML-help language code.
6. Translate all return values.
7. Include the new file in `src/language.cpp` and add a `case` entry in `setTranslator()`.
8. Add an entry in `doc/maintainers.txt` with your name and e-mail.

---

## Mechanism 2 — Doxywizard Qt Translations (`addon/doxywizard/i18n/doxywizard_xx.ts`)

### How it works

Doxywizard uses Qt's internationalization framework. All user-visible strings in the
Doxywizard GUI are wrapped with `tr()` calls in the C++ source. The Qt Linguist tool extracts
these strings into `.ts` (Translation Source) XML files. At build time the `.ts` files are
compiled into binary `.qm` files and embedded in the application resources.

Supported languages and their files:

| Language            | File                                        |
|---------------------|---------------------------------------------|
| German              | `addon/doxywizard/i18n/doxywizard_de.ts`    |
| Spanish             | `addon/doxywizard/i18n/doxywizard_es.ts`    |
| French              | `addon/doxywizard/i18n/doxywizard_fr.ts`    |
| Japanese            | `addon/doxywizard/i18n/doxywizard_ja.ts`    |
| Korean              | `addon/doxywizard/i18n/doxywizard_ko.ts`    |
| Russian             | `addon/doxywizard/i18n/doxywizard_ru.ts`    |
| Simplified Chinese  | `addon/doxywizard/i18n/doxywizard_zh_CN.ts` |
| Traditional Chinese | `addon/doxywizard/i18n/doxywizard_zh_TW.ts` |

### Translation file format

Each `.ts` file is XML with this structure:

```xml
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de">
<context>
    <name>MainWindow</name>
    <message>
        <source>Original English text</source>
        <translation>Translated text</translation>
    </message>
    ...
</context>
</TS>
```

Translation contexts and their source files:

| Context           | Source file        | Purpose                                   |
|-------------------|--------------------|-------------------------------------------|
| `MainWindow`      | `doxywizard.cpp`   | Main wizard interface                     |
| `Expert`          | `expert.cpp`       | Expert mode interface and dynamic content |
| `Messages`        | `doxywizard.cpp`   | Topic names and format labels             |
| `Wizard`          | `wizard.cpp`       | Wizard step labels                        |
| `Step1`–`Step4`   | `wizard.cpp`       | Step dialog labels                        |
| `TuneColorDialog` | `wizard.cpp`       | Color tuning dialog                       |
| `InputString`     | `inputstring.cpp`  | String input control                      |
| `InputStrList`    | `inputstrlist.cpp` | String list input control                 |
| `HelpLabel`       | `helplabel.h`      | Label with context menu                   |

### Identifying new or changed GUI strings

When commits touch any of the Doxywizard `.cpp` or `.h` source files, check whether new
`tr("...")` calls have been added or existing ones have changed.

A quick way to scan for changes to translatable strings between two commits:

```bash
git log commit1..commit2 --follow -p addon/doxywizard | grep '^[+-].*\btr("'
```

New or changed `tr()` calls require updates to every `.ts` file.

### Updating a `.ts` file

Edit the file directly in any text editor or use Qt Linguist:

1. **New string**: Add a `<message>` block inside the appropriate `<context>` with the
   `<source>` text matching exactly what the C++ code passes to `tr()`, and provide a
   `<translation>`.
2. **Changed source string**: Find the existing `<message>` by its old `<source>` text, update
   `<source>` to the new English text, and update `<translation>` accordingly.
3. **Removed string**: Delete the entire `<message>` block.
4. **Unfinished translation**: Qt marks untranslated strings with `type="unfinished"` on the
   `<translation>` element. Remove this attribute once the translation is complete.

### Quality guidelines for `.ts` files

- **Preserve placeholders**: If the source contains `%1`, `%2`, … positional markers, they
  must appear in the translation (order may differ if the target language requires it).
- **Escape XML special characters**: Use `&lt;` for `<`, `&gt;` for `>`, `&amp;` for `&`,
  `&apos;` for `'`, `&quot;` for `"`.
- **Escape HTML tags**: Inside `.ts` files, HTML tags embedded in strings must also be escaped:
  write `&lt;code&gt;` instead of `<code>`.
- **GUI terminology**: Use the established software localisation conventions for the target
  language, e.g. the official Qt or KDE glossary for German or French. Menu items, button
  labels, and dialog titles follow platform conventions.
- **Consistent terminology**: Use the same translation for the same English term throughout
  the file and across different `.ts` files for the same language.
- **File encoding**: Files must be UTF-8.

### Adding a new language to Doxywizard

1. Copy an existing `.ts` file (e.g., `doxywizard_de.ts`) to `doxywizard_xx.ts`.
2. Change the `language` attribute in the `<TS>` root element to the BCP 47 tag (e.g., `pt`).
3. Translate all `<translation>` strings.
4. Verify that the CMake build picks up the new `.ts` file (it typically uses a glob; check
   `addon/doxywizard/CMakeLists.txt`).

---

## Mechanism 3 — Config Option Translations (`src/i18n/config_xx.xml`)

### How it works

The configuration options shown in the Doxywizard Expert tab come from `src/config.xml`.
Each option has a `<docs>` or `<desc>` element with an English description. For non-English Doxywizard
sessions, a language-specific `src/i18n/config_xx.xml` file provides translated `<docs>` or `<desc>`
elements. The file mirrors the structure of `config.xml` but contains only the translated
documentation text for each option. Note that when an option in `config.xml` has multiple `<docs>` tags
only the ones where the `doxywizard` attribute is absent or set to `1` need to translated. Also 
`<docs>` elements for a single option are combined in the translation into a single `<docs>` element.

Supported languages and their files:

| Language            | File                        |
|---------------------|-----------------------------|
| German              | `src/i18n/config_de.xml`    |
| Spanish             | `src/i18n/config_es.xml`    |
| French              | `src/i18n/config_fr.xml`    |
| Japanese            | `src/i18n/config_ja.xml`    |
| Korean              | `src/i18n/config_ko.xml`    |
| Russian             | `src/i18n/config_ru.xml`    |
| Simplified Chinese  | `src/i18n/config_zh_CN.xml` |
| Traditional Chinese | `src/i18n/config_zh_TW.xml` |

### Identifying changes to `config.xml`

When a commit touches `src/config.xml`, options may have been added, removed, or their
documentation may have changed. Use the built-in sync tool to find differences:

**Report mode** (read-only, shows what is out of sync):

```bash
cd src
python3 configgen.py -sync config.xml i18n
```

**Auto-sync mode** (automatically adds missing options with English text, removes obsolete ones):

```bash
cd src
python3 configgen.py -sync config.xml i18n --auto
```

After auto-sync, newly added options will have their English `<docs>` text copied verbatim —
these must then be translated manually.

### Config XML file format

```xml
<?xml version="1.0" encoding="utf-8"?>
<doxygenconfig lang="Deutsch">
  <group name="Project" trname="Translated group name"
         docs="Translated group description">
    <option type="string" id="OPTION_NAME" ...>
      <docs>
<![CDATA[Translated documentation for this option. Use \c for code spans.]]>
      </docs>
    </option>
    ...
  </group>
</doxygenconfig>
```

Key points:
- The `lang` attribute on `<doxygenconfig>` is the human-readable language name in that
  language (e.g., `"Deutsch"` for German, `"Français"` for French).
- Each `<group>` has a `trname` (translated group name) and a `docs` attribute (translated
  group description).
- Each `<option>` element must carry the same `id` as in `config.xml`.
- The `<docs>` content uses `CDATA` sections. Doxygen markup such as `\c`, `\note`, and
  HTML entities are supported inside `<docs>`.
- Enum `<value>` child elements under an `<option>` may have a `trname` attribute for a
  translated value label; if omitted the English name is used.
- Do **not** include obsolete options (options that exist in a translated file but not in
  `config.xml`); the sync tool will flag and remove these.

### Quality guidelines for config XML files

- Translate the meaning of each option accurately. Configuration option descriptions are
  technical; use proper software-development terminology.
- Preserve all `\c`, `\note`, `\deprecated`, and similar Doxygen markers exactly.
- Preserve HTML entity references (`&lt;`, `&gt;`, `&amp;`) and hyperlinks.
- Keep the description concise but complete — Doxywizard displays these directly to the user.
- Use correct typographic conventions for the target language (quotation marks, spacing before
  punctuation, etc.).
- Files must be UTF-8 encoded.

---

## Monitoring Commits for Translation-Impacting Changes

When reviewing commits to the repository, check the following files and act accordingly:

| Changed file                     | Translation work required                                                          |
|----------------------------------|------------------------------------------------------------------------------------|
| `src/translator_en.h`            | Update all `src/translator_xx.h` files                                             |
| `src/translator_adapter.h`       | Review: new adapter = new methods needed in all languages                          |
| `src/translator.h`               | Review: new pure virtuals = mandatory new methods in all languages                 |
| `addon/doxywizard/*.cpp`, `*.h`  | Update all `addon/doxywizard/i18n/doxywizard_xx.ts` files                          |
| `src/config.xml`                 | Run sync tool; translate newly added options in all `src/i18n/config_xx.xml` files |

Useful git commands for change detection:

```bash
# Show changes to the English translator since a given tag
git log <old-commit>..<new-commit> --follow -p src/translator_en.h

# Show only added/removed method lines (replace <old-commit> and <new-commit>
# with actual commit hashes, branch names, or tags, e.g. v1.9.0..HEAD)
git diff <old-commit>..<new-commit> src/translator_en.h | grep '^[+-].*QCString tr'

# Show changes to doxywizard GUI strings
git log <old-commit>..<new-commnt> --follow -p addon/doxywizard | grep '^[+-].*\btr("'

# Show changes to config.xml option documentation
git log <old-commit>..<new-commit> --follow -p src/config.xml
```

---

## Quality Improvement Guidelines

When reviewing existing translations for quality:

1. **Technical terminology**: Verify that software-development terms (class, namespace, struct,
   typedef, enum, template, interface, module, concept, etc.) follow the established
   convention for the target language. In many languages these English terms are kept as-is;
   in others (e.g., French) there are official equivalents.

2. **Accents and diacritics**: Check that all diacritics are present and correct. Common
   mistakes include missing accents in French (é/è/ê), missing umlauts in German (ä/ö/ü/ß),
   and missing cedillas or tildes in Spanish/Portuguese.

3. **Grammar and plural agreement**: Methods with `bool singular` parameters must produce
   grammatically correct singular and plural forms. Methods with `bool first_capital` must
   capitalise the first letter when the parameter is `true`.

4. **Consistency**: The same English term must map to the same translated term throughout all
   three mechanisms for a given language.

5. **Outdated adapters**: If a language translator inherits from a very old adapter (e.g.,
   `TranslatorAdapter_1_8_0`), many methods fall back to English. Prioritise bringing these
   up to date.

6. **Encoding**: All files must be UTF-8 without BOM. Do not use HTML entities like `&auml;`
   in translator `.h` files; use the actual UTF-8 character `ä` directly. Note that this
   is the opposite of the rule for `.ts` files (Mechanism 2), where XML special characters
   *must* be escaped using entities — the difference arises because `.ts` files are XML
   while `.h` files are plain C++ source.

---

## Summary Workflow for Each Doxygen Release

1. **Run `doc/translator.py`** to see which language translators are behind and which methods
   are missing. Translate missing methods in each affected `src/translator_xx.h`.

2. **Inspect `src/translator_adapter.h`** to see if a new adapter class was added for this
   release. The new adapter lists all new methods — these are the ones that need translating.

3. **Run `python3 src/configgen.py -sync src/config.xml src/i18n --auto`** to sync the
   localized config XML files. Translate the newly copied English `<docs>` in each
   `src/i18n/config_xx.xml`.

4. **Inspect commits to Doxywizard source** for new or changed `tr()` calls. Update every
   `addon/doxywizard/i18n/doxywizard_xx.ts` accordingly.

5. **Verify no `type="unfinished"`** attributes remain in any `.ts` file.

6. **Proofread** the updated translations for grammar, terminology, and accents.

7. **Submit a pull request** with all three sets of changes grouped together for review.
