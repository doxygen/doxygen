# Copilot Instructions for Doxygen

## Project Overview

Doxygen is the de facto standard tool for generating documentation from annotated source code. It supports C++, C, Objective-C, C#, PHP, Java, Python, IDL, Fortran, VHDL, and more. The project is written primarily in C++ (C++17) and uses Flex (`.l` files) for lexical scanning, Python scripts for code generation, and CMake as its build system.

Current version: **1.17.0**

## Repository Structure

```
/
├── src/               # Main source code (C++, Flex .l files, Python scripts)
├── addon/             # Optional components
│   ├── doxywizard/    # Qt-based GUI frontend
│   ├── doxmlparser/   # XML output parser library
│   ├── doxyapp/       # Example embedding doxygen
│   ├── doxyparse/     # Source dependency parser
│   └── doxysearch/    # External search tools (doxyindexer, doxysearch)
├── deps/              # Bundled third-party libraries (spdlog, fmt, sqlite3, etc.)
├── cmake/             # CMake modules and helpers
├── testing/           # Regression tests (numbered NNN_name.ext pattern)
├── doc/               # User manual source
├── doc_internal/      # Developer/internal documentation (architecture overview)
├── templates/         # HTML/other output templates
├── vhdlparser/        # VHDL-specific parser
└── libxml/            # Bundled XML library
```

## Build System

Doxygen uses **CMake** (minimum 3.14). Always build out-of-source:

```bash
mkdir build && cd build
cmake -G "Unix Makefiles" ..
make
```

### Key CMake Options

| Option | Description |
|---|---|
| `build_wizard=ON` | Build doxywizard (Qt GUI), requires Qt5 or Qt6 |
| `build_app=ON` | Build doxyapp example |
| `build_parse=ON` | Build doxyparse |
| `build_search=ON` | Build doxysearch/doxyindexer |
| `build_doc=ON` | Build user manual (requires LaTeX) |
| `use_libclang=ON` | Add libclang-based parsing support |
| `use_sys_spdlog=ON` | Use system spdlog instead of bundled |
| `use_sys_fmt=ON` | Use system fmt instead of bundled |
| `use_sys_sqlite3=ON` | Use system sqlite3 instead of bundled |
| `static_libclang=ON` | Link statically against libclang |
| `CMAKE_BUILD_TYPE=Debug` | Debug build (enables debug/tracing capabilities) |
| `enable_tracing=ON` | Enable tracing in release builds |
| `enable_lex_debug=ON` | Enable lex debug output in release builds |
| `enable_coverage=ON` | Enable coverage reporting (requires separate build dir) |

### Running Tests

```bash
# Build first, then from the build directory:
make tests         # Run all regression tests sequentially
ctest              # Run via CTest (supports parallel: ctest -j4)
```

Tests are Python-driven via `testing/runtests.py`. Each test is a numbered file in `testing/` matching `NNN_*.ext`. Tests run doxygen and compare XML output.

To run a single test by number:
```bash
python testing/runtests.py --id 001 --doxygen ./bin/doxygen \
  --inputdir ../testing --outputdir ./testing
```

## Code Style

- **Language**: C++17
- **Formatter**: clang-format (see `.clang-format`). Run `clang-format -i <file>` to format.
- **Style highlights**:
  - Allman brace style (braces on new lines for functions/classes/control flow)
  - 2-space indentation, no tabs
  - `ColumnLimit: 0` (no automatic line wrapping)
  - Pointers/references right-aligned: `Type *ptr`, `Type &ref`
  - Short functions/if/loops allowed on single line
- **Static analysis**: clang-tidy (see `.clang-tidy`). Enable with `-DENABLE_CLANG_TIDY=ON`.

## Architecture Overview (Key Concepts)

### Configuration System
- Defined in `src/config.xml` (XML schema)
- Python script `src/configgen.py` generates `configoptions.cpp`, `configvalues.h`, `configvalues.cpp` at build time
- Access config values with macros: `Config_getString()`, `Config_getInt()`, `Config_getList()`, `Config_getEnum()`, `Config_getBool()`
- Config singleton class: `Config`

### Processing Pipeline
1. **Config parsing** – `Config::parse()`
2. **Input file discovery** – `searchInputFiles()`
3. **Tag file reading** – `readTagFile()`
4. **Preprocessing** – `Preprocessor::processFile()` (C/C++ only)
5. **Comment conversion** – `convertCppComments()` (C++ → C style + alias resolution)
6. **Language parsing** – `OutlineParserInterface::parseInput()` → tree of `Entry` objects
7. **Comment parsing** – `CommentScanner::parseCommentBlock()` + `Markdown::process()`
8. **Symbol resolution** – Multiple tree walks over `Entry` nodes → `Definition` objects
9. **Output generation** – Format-specific generators

### Key Class Hierarchy
- `Definition` (abstract base) → `ClassDef`, `NamespaceDef`, `FileDef`, `DirDef`, `GroupDef`, `PageDef`, `ConceptDef`
- `MemberDef` – for functions, variables, enums, etc.
- `ParserManager` – singleton factory for language parsers
- `OutlineParserInterface` / `CodeParserInterface` – parser interfaces

### Lexical Scanners (Flex `.l` files in `src/`)
Key scanners: `commentscan.l`, `commentcnv.l`, `code.l`, `configimpl.l`, `constexp.l`, `declinfo.l`, `defargs.l`, `doctokenizer.l`, `fortranscanner.l`, `pre.l`, etc.

Python scripts pre/post-process the `.l` files: `pre_lex.py`, `post_lex.py`, `scan_states.py`.

### Output Generators
Each output format has a generator class and a doc visitor:
- HTML: `HtmlGenerator` + `HtmlDocVisitor`
- LaTeX: `LatexGenerator` + `LatexDocVisitor`
- RTF: `RTFGenerator` + `RTFDocVisitor`
- Man: `ManGenerator` + `ManDocVisitor`
- XML: `generateXML()` + `XmlDocVisitor`
- DocBook, perlmod also supported

## Debugging Doxygen

Use `-d <option>` on the command line for debug output:

| Option | Description |
|---|---|
| `-d preprocessor` | Preprocessing results |
| `-d commentcnv` | Comment conversion results |
| `-d commentscan` | Comment scanning before/after |
| `-d printtree` | Entry tree dump |
| `-d time` | Timing of stages |
| `-d markdown` | Markdown processing |
| `-d lex` | Lexer start/stop info |
| `-d lex:lexername` | Specific lexer debug output |
| `-d entries` | Entry tree dump |

Use `-t [tracefile]` for full tracing output (very verbose; useful for experts).

## Common Workflows

### Adding a New Configuration Option
1. Edit `src/config.xml` to add the option definition
2. Rebuild – `configgen.py` auto-generates the C++ files
3. Use the appropriate `Config_get*()` macro in source code

### Adding a New Doxygen Command
1. Add the command to `src/cmdmapper.cpp` and `src/cmdmapper.h`
2. Handle it in `src/commentscan.l` (or relevant lexer)
3. Add documentation handling in `src/docparser.cpp`

### Adding a Regression Test
1. Create `testing/NNN_description.ext` with the input source
2. Add comment headers: `// objective: ...` and `// check: filename.xml`
3. Create `testing/NNN/` directory structure with expected XML output
4. Run test: `python testing/runtests.py --id NNN --doxygen ./bin/doxygen --inputdir ../testing --outputdir ./testing`
5. Use `--noreg` flag to generate reference output on first run

### Modifying a Flex Scanner
1. Edit the `.l` file in `src/`
2. The build system automatically runs flex and applies pre/post processing scripts
3. Do not edit the generated `.cpp` files directly

## Generated Files (Do Not Edit Manually)
- `<build>/generated_src/configvalues.h` – from `config.xml` via `configgen.py`
- `<build>/generated_src/configvalues.cpp` – from `config.xml` via `configgen.py`
- `<build>/generated_src/configoptions.cpp` – from `config.xml` via `configgen.py`
- `<build>/generated_src/settings.h` – from CMake configuration
- Scanner `.cpp` files – generated from `.l` files by Flex

## CI/CD

GitHub Actions workflow: `.github/workflows/build_cmake.yml`

Tests run on: Ubuntu (GCC & Clang, Intel & ARM), macOS (Intel & Apple Silicon), Windows (MSVC). Both Debug and Release build types are tested.

## Dependencies

### Bundled (in `deps/`)
- `spdlog` – logging
- `fmt` – string formatting
- `sqlite3` – search index database
- `TinyDeflate` – compression
- `liblodepng` – PNG generation
- `libmscgen` – MSC diagram rendering
- `libmd5` – MD5 hashing

### External (optional)
- Qt 5 or Qt 6 – for doxywizard GUI
- LLVM/libclang – for enhanced C/C++ parsing (`use_libclang=ON`)
- Graphviz dot – for diagram generation at runtime
- LaTeX – for PDF output at runtime
- Python 3 – required for build (code generation scripts)

## Known Issues / Workarounds

- **In-place builds not supported with `enable_coverage=ON`**: Always use a separate build directory.
- **Windows**: libiconv must be downloaded separately (the CI fetches it from `pffang/libiconv-for-Windows`).
- **Flex buffer size**: Controlled by `enlarge_lex_buffers` CMake variable (default 262144). Increase if parsing very large files.
- **CMake < 3.21**: `DEPFILE` support is disabled automatically; no action needed.
- **Snap conflicts on Ubuntu CI**: The workflow removes `firefox` and `snapd` snaps before installing LaTeX to prevent hangs.

## Useful References

- Internal architecture: `doc_internal/doxygen.md`
- User manual: https://www.doxygen.nl/manual/
- Internal docs (generated): https://doxygen.github.io/doxygen-docs/
- Issue tracker: https://github.com/doxygen/doxygen/issues
