/****************************************************************************
 *
 * Requirements traceability support for Doxygen.
 *
 ****************************************************************************/

#include "requirementstracker.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <fstream>
#include <iomanip>
#include <cerrno>

#include "config.h"
#include "debug.h"
#include "doxygen.h"
#include "filedef.h"
#include "filename.h"
#include "fileinfo.h"
#include "message.h"
#include "pagedef.h"
#include "parserintf.h"
#include "util.h"
#include "xml.h"
#include "outputlist.h"
#include "index.h"
#include "definition.h"
#include "memberdef.h"
#include "doxygen.h"
#include "htmlgen.h"
#include "latexgen.h"
#include "rtfgen.h"

namespace {
// Context structure for XML parsing
struct XMLContext {
  bool insideCompound = false;
  bool collectingName = false;
  bool collectingTitle = false;
  bool collectingFilename = false;
  bool collectingAnchor = false;
  QCString currentText;
  QCString currentFile;
  QCString pageFilename;  // The <filename> element
};

QCString stripQuotes(const QCString &input) {
  if (input.length() >= 2) {
    char first = input.at(0);
    char last = input.at(input.length() - 1);
    if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
      return input.mid(1, input.length() - 2).stripWhiteSpace();
    }
  }
  return input;
}
} // namespace

RequirementsTracker &RequirementsTracker::instance() {
  static RequirementsTracker inst;
  return inst;
}

void RequirementsTracker::initialize() {
  if (m_initialized) {
    return;
  }

  // Don't clear collections if they already exist and have references
  // This preserves @satisfies/@verifies collected during document parsing
  bool hasExistingData = false;
  for (const auto &collection : m_collections) {
    for (const auto &[reqId, req] : collection.requirements) {
      if (!req.satisfiedBy.empty() || !req.verifiedBy.empty()) {
        hasExistingData = true;
        break;
      }
    }
    if (hasExistingData) break;
  }

  if (!hasExistingData) {
    m_collections.clear();
  }

  // First, parse REQUIREMENTS_FILES (direct source files)
  const StringVector &files = Config_getList(REQUIREMENTS_FILES);
  if (!files.empty()) {
    std::unordered_set<std::string> processedFilePaths;
    for (const auto &file : files) {
      if (file.empty()) {
        continue;
      }

      if (!processedFilePaths.insert(file).second) {
        continue; // avoid parsing the same file twice
      }

      QCString filePath(file.c_str());
      filePath = filePath.stripWhiteSpace();

      if (filePath.isEmpty()) {
        continue;
      }

      FileInfo fi(filePath.str());
      if (!fi.exists() || !fi.isFile()) {
        warn(filePath, 1, "requirements file '{}' could not be located", filePath);
        continue;
      }

      // Create a new collection and parse the file
      RequirementsCollection collection;
      collection.tagFilePath = QCString(fi.absFilePath().c_str());

      // Pass the original (possibly relative) path for page name generation
      parseRequirementsFile(collection, collection.tagFilePath, filePath);

      if (!collection.requirements.empty()) {
        msg("Loaded {} requirements from '{}'", collection.requirements.size(), collection.tagFilePath);
        m_collections.push_back(std::move(collection));
      }
    }
  }

  // Then, parse REQUIREMENTS_TAGFILES (external tag files)
  const StringVector &entries = Config_getList(REQUIREMENTS_TAGFILES);
  if (!entries.empty()) {
    std::unordered_set<std::string> processedPaths;
    for (const auto &entry : entries) {
      if (entry.empty()) {
        continue;
      }

      QCString spec(entry.c_str());

      // Only process tag files that contain "requirements" in their name
      if (!spec.contains("requirements")) {
        continue;
      }
      if (!processedPaths.insert(entry).second) {
        continue; // avoid parsing the same specification twice
      }

      QCString tagPath;
      QCString destination;
      int eqPos = spec.find('=');
      if (eqPos != -1) {
        tagPath = spec.left(eqPos).stripWhiteSpace();
        destination = spec.mid(eqPos + 1).stripWhiteSpace();
      } else {
        tagPath = spec.stripWhiteSpace();
      }

      if (tagPath.isEmpty()) {
        continue;
      }

      FileInfo fi(tagPath.str());
      if (!fi.exists() || !fi.isFile()) {
        warn(tagPath, 1, "requirements tag file '{}' could not be located",
             tagPath);
        continue;
      }

      // Create a new collection and parse the tag file
      RequirementsCollection collection;
      collection.tagFilePath = QCString(fi.absFilePath().c_str());
      collection.destination = stripQuotes(destination);

      parseRequirementsTagFile(collection, collection.tagFilePath, collection.destination);

      if (!collection.requirements.empty()) {
        msg("Loaded {} requirements from '{}'", collection.requirements.size(), collection.tagFilePath);
        m_collections.push_back(std::move(collection));
      }
    }
  }

  m_initialized = true;
}

bool RequirementsTracker::hasRequirements() const {
  return !m_collections.empty();
}

void RequirementsTracker::reset() {
  m_initialized = false;
  m_collections.clear();
}

// Extract prefix from requirement ID (e.g., "A024_SRS_123" -> "A024_SRS_")
QCString RequirementsTracker::extractPrefix(const QCString &requirementId) const {
  // Match pattern: (PREFIX)(NUMBER) where PREFIX can be zero or more characters
  // Examples: SRS_123 -> "A0123_SRS_", REQ123 -> "REQ", ABC-456 -> "ABC-", "123" -> ""
  // Use non-greedy match (.*?) to get minimal prefix followed by digits at the end (\\d+$)
  std::regex pattern("^(.*?)(\\d+)$");
  std::string id_str = requirementId.str();
  std::smatch match;

  if (std::regex_match(id_str, match, pattern)) {
    return QCString(match[1].str().c_str());
  }

  return QCString();
}

// Find collection by requirement prefix
RequirementsTracker::RequirementsCollection* RequirementsTracker::findCollectionByPrefix(const QCString &requirementId) {
  QCString prefix = extractPrefix(requirementId);

  for (auto &collection : m_collections) {
    if (collection.prefix == prefix) {
      return &collection;
    }
  }
  return nullptr;
}

void RequirementsTracker::addSatisfiedBy(const QCString &requirementId, const QCString &reference) {
  RequirementsCollection *collection = findCollectionByPrefix(requirementId);
  if (!collection) {
    return;
  }

  auto it = collection->requirements.find(requirementId);
  if (it != collection->requirements.end()) {
    // Check if this reference already exists to avoid duplicates
    auto &satisfiedBy = it->second.satisfiedBy;
    if (std::find(satisfiedBy.begin(), satisfiedBy.end(), reference) == satisfiedBy.end()) {
      satisfiedBy.push_back(reference);
    }
  }
}

void RequirementsTracker::addVerifiedBy(const QCString &requirementId, const QCString &reference) {
  RequirementsCollection *collection = findCollectionByPrefix(requirementId);
  if (!collection) {
    return;
  }

  auto it = collection->requirements.find(requirementId);
  if (it != collection->requirements.end()) {
    // Check if this reference already exists to avoid duplicates
    auto &verifiedBy = it->second.verifiedBy;
    if (std::find(verifiedBy.begin(), verifiedBy.end(), reference) == verifiedBy.end()) {
      verifiedBy.push_back(reference);
    }
  }
}

// Parse a requirements tag file and populate the collection
void RequirementsTracker::parseRequirementsTagFile(RequirementsCollection &collection,
                                                     const QCString &tagFilePath,
                                                     const QCString &destination) {
  // Read the tag file
  QCString raw = fileToString(tagFilePath);
  if (raw.isEmpty()) {
    warn(tagFilePath, 1, "requirements tag file '{}' is empty", tagFilePath);
    return;
  }

  XMLHandlers handlers;
  XMLContext context;

  // Handler for <compound kind="page">
  handlers.startElement = [&](const std::string &name, const XMLHandlers::Attributes &attrs) {
    if (name == "compound") {
      auto kind_it = attrs.find("kind");
      if (kind_it != attrs.end() && kind_it->second == "page") {
        context.insideCompound = true;
      }
    } else if (context.insideCompound && name == "name") {
      context.collectingName = true;
      context.currentText.clear();
    } else if (context.insideCompound && name == "title") {
      context.collectingTitle = true;
      context.currentText.clear();
    } else if (context.insideCompound && name == "filename") {
      context.collectingFilename = true;
      context.currentText.clear();
    } else if (context.insideCompound && name == "docanchor") {
      context.collectingAnchor = true;
      context.currentText.clear();

      // Get the file attribute if present
      auto file_it = attrs.find("file");
      if (file_it != attrs.end()) {
        context.currentFile = QCString(file_it->second.c_str());
      }
    }
    return true;
  };

  handlers.endElement = [&](const std::string &name) {
    if (name == "compound") {
      context.insideCompound = false;
    } else if (name == "name" && context.collectingName) {
      context.collectingName = false;
      // Store the name but prefer title for display
    } else if (name == "title" && context.collectingTitle) {
      context.collectingTitle = false;
      // Use the title as the display title
      collection.displayTitle = context.currentText.stripWhiteSpace();
    } else if (name == "filename" && context.collectingFilename) {
      context.collectingFilename = false;
      context.pageFilename = context.currentText.stripWhiteSpace();
    } else if (name == "docanchor" && context.collectingAnchor) {
      context.collectingAnchor = false;

      // This is a requirement ID (anchor)
      QCString reqId = context.currentText.stripWhiteSpace();
      if (!reqId.isEmpty()) {
        // If this is the first requirement, establish the collection prefix
        if (collection.prefix.isEmpty()) {
          QCString prefix = extractPrefix(reqId);

          // Skip if it doesn't match the requirement pattern (e.g., "mainpage")
          if (prefix.isEmpty()) {
            context.currentFile.clear();
            return true;
          }

          // Store the prefix for this collection
          collection.prefix = prefix;
        }

        // Check if this requirement matches the collection's requirement pattern
        QCString prefix = extractPrefix(reqId);

        // Only add requirements that match the pattern AND have the same prefix
        if (!prefix.isEmpty() && prefix == collection.prefix) {
          Requirement req;
          req.id = reqId;

          // Build URL using the file from the docanchor attribute or the page filename
          QCString fileToUse = context.currentFile.isEmpty() ? context.pageFilename : context.currentFile;
          if (!destination.isEmpty() && !fileToUse.isEmpty()) {
            req.url = destination + "/" + fileToUse + "#" + reqId;
          }

          collection.requirements[reqId] = req;
        }
      }

      context.currentFile.clear();
    }
    return true;
  };

  handlers.characters = [&](const std::string &chars) {
    if (context.collectingName || context.collectingTitle || context.collectingFilename || context.collectingAnchor) {
      context.currentText += QCString(chars.c_str());
    }
  };

  handlers.error = [&](const std::string &fileName, int lineNr, const std::string &msg) {
    warn(fileName.c_str(), lineNr, "{}", msg);
  };

  XMLParser parser(handlers);
  parser.parse(tagFilePath.data(), raw.data(), Debug::isFlagSet(Debug::Lex_xml),
               []() {}, []() {});

  // Generate page name from display title or file name
  if (collection.displayTitle.isEmpty()) {
    FileInfo fi(tagFilePath.str());
    collection.displayTitle = QCString(fi.fileName().c_str());
  }

  // Store the page filename from the tag file
  collection.pageFilename = context.pageFilename;

  // Create page name like "traceability_srs_a024_017322"
  QCString pageName = "traceability_" + collection.displayTitle.lower();
  // Sanitize: replace spaces and problematic characters with underscores
  pageName = substitute(pageName, ' ', '_');
  pageName = substitute(pageName, '-', '_');
  collection.pageName = pageName;
}

// Parse a requirements file directly (markdown, doxygen, source files)
// Parse a requirements file and populate the collection
void RequirementsTracker::parseRequirementsFile(RequirementsCollection &collection,
                                                 const QCString &absFilePath,
                                                 const QCString &originalPath) {
  // Read the file
  QCString raw = fileToString(absFilePath);
  if (raw.isEmpty()) {
    warn(absFilePath, 1, "requirements file '{}' is empty", absFilePath);
    return;
  }

  // Generate the page name that Doxygen will create for this markdown file
  // This must match the logic in markdown.cpp's markdownFileNameToId()
  // which does: absFilePath -> stripFromPath -> remove extension -> escapeCharsInString

  // Use absolute path, then strip from path (matches STRIP_FROM_PATH config)
  std::string absFilePathStr = FileInfo(originalPath.str()).absFilePath();
  QCString baseFn = stripFromPath(absFilePathStr.c_str());

  // Remove extension
  int i = baseFn.findRev('.');
  if (i != -1) {
    baseFn = baseFn.left(i);
  }

  // Apply the same escaping that Doxygen uses (allowDots=false, allowUnderscore=false)
  QCString baseName = escapeCharsInString(baseFn, false, false);

  // Generate page filename with "md_" prefix
  QCString pageFileName = "md_" + baseName;

  // Pattern to match @anchor followed by requirement ID
  // Examples: @anchor A024_SRS_123, @anchor REQ_456
  std::regex anchorPattern(R"(@anchor\s+([A-Za-z0-9_-]+))");

  // Extract the page title from markdown content
  // Look for either ATX-style (# Title) or Setext-style (Title\n===)
  std::regex atxTitlePattern(R"(^\s*#\s+(.+)$)", std::regex::multiline);
  std::regex setextTitlePattern(R"(^(.+)\n=+\s*$)", std::regex::multiline);

  QCString pageTitle;
  std::string content = raw.str();

  // Try Setext-style first (Title\n===) as it's used in the requirements file
  std::smatch titleMatch;
  if (std::regex_search(content, titleMatch, setextTitlePattern)) {
    pageTitle = QCString(titleMatch[1].str().c_str()).stripWhiteSpace();
  }
  // Fallback to ATX-style (# Title)
  else if (std::regex_search(content, titleMatch, atxTitlePattern)) {
    pageTitle = QCString(titleMatch[1].str().c_str()).stripWhiteSpace();
  }

  // Strip Doxygen {#labelid} Header Id Attributes from the title
  if (!pageTitle.isEmpty()) {
    std::regex idTagPattern(R"(\s*\{#[^}]+\})");
    std::string titleStr = pageTitle.str();
    titleStr = std::regex_replace(titleStr, idTagPattern, "");
    pageTitle = QCString(titleStr.c_str()).stripWhiteSpace();
  }

  std::sregex_iterator it(content.begin(), content.end(), anchorPattern);
  std::sregex_iterator end;

  for (; it != end; ++it) {
    std::smatch match = *it;
    QCString reqId = QCString(match[1].str().c_str());

    // If this is the first requirement, establish the collection prefix
    if (collection.prefix.isEmpty()) {
      QCString prefix = extractPrefix(reqId);

      // Skip if it doesn't match a requirement pattern (must have digits at end)
      if (prefix.isEmpty()) {
        continue;
      }

      // Store the prefix for this collection
      collection.prefix = prefix;

      // Use the extracted page title if available, otherwise generate from prefix
      if (!pageTitle.isEmpty()) {
        collection.displayTitle = pageTitle;
      } else {
        // Generate display title from the prefix (remove trailing underscore/dash)
        collection.displayTitle = prefix;
        if (collection.displayTitle.endsWith("_") || collection.displayTitle.endsWith("-")) {
          collection.displayTitle = collection.displayTitle.left(collection.displayTitle.length() - 1);
        }
      }

      // Create page name for traceability - must match generateTraceabilityPage() logic
      QCString pageName = "traceability_" + collection.displayTitle.lower();
      pageName = substitute(pageName, ' ', '_');
      pageName = substitute(pageName, '-', '_');
      collection.pageName = pageName;

      // Store the page filename (the generated .html file)
      collection.pageFilename = pageFileName + ".html";

      // For REQUIREMENTS_FILES, destination is empty (internal page)
      collection.destination = "";
    }

    // Check if this requirement matches the collection's requirement pattern
    QCString prefix = extractPrefix(reqId);

    // Only add requirements that match the pattern AND have the same prefix
    if (!prefix.isEmpty() && prefix == collection.prefix) {
      Requirement req;
      req.id = reqId;

      // URL points to the generated markdown page with the anchor
      req.url = pageFileName + ".html#" + reqId;

      collection.requirements[reqId] = req;
    }
  }

  // If no requirements found, warn the user
  if (collection.requirements.empty()) {
    warn(absFilePath, 1, "no requirement anchors found in file '{}' (expected @anchor PATTERN)", absFilePath);
  }
}

void RequirementsTracker::generateTraceabilityPages() {
  initialize();

  if (m_collections.empty()) {
    return;
  }

  msg("Generating {} traceability pages...", m_collections.size());

  for (const auto &collection : m_collections) {
    msg("  Collection '{}': {} requirements", collection.displayTitle, collection.requirements.size());

    // Count requirements with references
    int withReferences = 0;
    for (const auto &[reqId, req] : collection.requirements) {
      if (!req.satisfiedBy.empty() || !req.verifiedBy.empty()) {
        withReferences++;
      }
    }
    msg("    {} requirements have satisfied/verified references", withReferences);

    // Generate the page
    generateTraceabilityPage(collection);
  }
}

void RequirementsTracker::generateTraceabilityPage(const RequirementsCollection &collection) {
  // Generate page name based on collection
  QCString fileName = "traceability_" + collection.displayTitle.lower();
  fileName = substitute(fileName, " ", "_");
  fileName = substitute(fileName, "-", "_");

  // Create .dox file in current directory where Doxygen can find it
  QCString doxPath = fileName + ".dox";

  std::ofstream doxFile(doxPath.str());
  if (!doxFile.is_open()) {
    printf("ERROR: Could not create .dox file: %s\n", qPrint(doxPath));
    return;
  }

  // Count requirements with references
  int withSatisfiedBy = 0;
  int withVerifiedBy = 0;
  for (const auto &[reqId, req] : collection.requirements) {
    if (!req.satisfiedBy.empty()) withSatisfiedBy++;
    if (!req.verifiedBy.empty()) withVerifiedBy++;
  }

  // Write Doxygen document syntax
  doxFile << "/**\n";
  doxFile << " * \\page " << fileName << " Traceability: " << collection.displayTitle << "\n";
  doxFile << " *\n";

  // Write summary statistics
  doxFile << " * " << collection.requirements.size() << " requirements<br/>\n";
  doxFile << " * " << withSatisfiedBy << " Satisfied (";
  doxFile << std::fixed << std::setprecision(1)
          << (100.0 * withSatisfiedBy / collection.requirements.size()) << "%)<br/>\n";
  doxFile << " * " << withVerifiedBy << " Verified (";
  doxFile << std::fixed << std::setprecision(1)
          << (100.0 * withVerifiedBy / collection.requirements.size()) << "%)\n";
  doxFile << " *\n";

  // Create table using Doxygen markdown syntax
  doxFile << " * | Requirement ID | Satisfied By | Verified By |\n";
  doxFile << " * |----------------|--------------|-------------|\n";

  // Write each requirement row
  for (const auto &[reqId, req] : collection.requirements) {
    doxFile << " * | ";

    // Requirement ID with link to external documentation
    if (!req.url.isEmpty()) {
      QCString url = req.url;
      doxFile << "[" << reqId << "](" << url << ")";
    } else {
      doxFile << reqId;
    }
    doxFile << " | ";

    // Satisfied By column with \\ref links
    if (!req.satisfiedBy.empty()) {
      for (size_t i = 0; i < req.satisfiedBy.size(); ++i) {
        if (i > 0) doxFile << "<br/>";
        doxFile << "\\ref " << req.satisfiedBy[i];
      }
    }
    doxFile << " | ";

    // Verified By column with \\ref links
    if (!req.verifiedBy.empty()) {
      for (size_t i = 0; i < req.verifiedBy.size(); ++i) {
        if (i > 0) doxFile << "<br/>";
        doxFile << "\\ref " << req.verifiedBy[i];
      }
    }
    doxFile << " |\n";
  }

  doxFile << " */\n";
  doxFile.close();

  // Store the .dox file path and page name to be processed later
  m_generatedDoxFiles.push_back(doxPath.str());
  m_generatedPageNames.push_back(fileName.str());
}

void RequirementsTracker::processGeneratedDoxFiles() {
  if (m_generatedDoxFiles.empty()) {
    return;
  }

  for (const auto& doxFile : m_generatedDoxFiles) {
    // Create a FileDef for the .dox file
    QCString doxPath(doxFile.c_str());
    bool ambig = false;
    FileDef *fd = findFileDef(Doxygen::inputNameLinkedMap, doxPath, ambig);
    if (!fd) {
      // Create a new FileDef
      auto fdPtr = createFileDef("", doxPath, "", doxPath);
      fd = fdPtr.get();
      if (!fd) {
        continue;
      }
    }

    // Get the parser for .dox files using the public wrapper
    auto parser = getParserForFileExt(doxPath);
    if (!parser) {
      continue;
    }

    // Parse the .dox file using the same mechanism as normal file processing
    auto fileRoot = parseFileWithParser(*parser.get(), fd, doxPath, nullptr, true);
    if (fileRoot) {
      // Call buildPageList to integrate the parsed pages into Doxygen's page list
      buildPageListFromEntry(fileRoot.get());
    }
  }

  // Now generate documentation for the newly added pages
  std::vector<PageDef*> newPages;
  for (const auto& pageName : m_generatedPageNames) {
    PageDef *pd = Doxygen::pageLinkedMap->find(QCString(pageName.c_str()));
    if (pd) {
      newPages.push_back(pd);
    }
  }

  if (!newPages.empty()) {
    generateDocsForPages(newPages);
  }

  // Don't clear m_generatedDoxFiles here - they will be cleaned up in finalize()
  m_generatedPageNames.clear();
}

void RequirementsTracker::finalize() {
  // Clean up temporary .dox files before doxygen exits
  for (const auto& doxFile : m_generatedDoxFiles) {
    std::remove(doxFile.c_str());
  }
  m_generatedDoxFiles.clear();
  m_generatedPageNames.clear();
}
