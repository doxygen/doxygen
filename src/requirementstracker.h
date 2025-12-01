/****************************************************************************
 *
 * Requirements traceability support for Doxygen.
 *
 * Copyright (C) 2025
 *
 * See the top-level LICENSE file for more details.
 *
 ****************************************************************************/

#ifndef REQUIREMENTSTRACKER_H
#define REQUIREMENTSTRACKER_H

#include "qcstring.h"

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

class Definition;
class Entry;
class MemberDef;

class RequirementsTracker {
public:
  // Individual requirement with its traceability references
  struct Requirement {
    QCString id;                      //!< Requirement ID (anchor from tagfile)
    QCString title;                   //!< Human readable title/summary
    QCString url;                     //!< Full URL to requirement in external docs
    std::vector<QCString> satisfiedBy; //!< Collection of strings referencing what satisfies this
    std::vector<QCString> verifiedBy;  //!< Collection of strings referencing what verifies this
  };

  // Collection of requirements with the same prefix (e.g., "A024_SRS_")
  struct RequirementsCollection {
    QCString prefix;                  //!< Prefix pattern (e.g., "A024_SRS_")
    QCString tagFilePath;             //!< Path to the tag file
    QCString destination;             //!< External documentation base URL or internal page name
    QCString pageFilename;            //!< Filename from tag file (e.g., "index.html") or generated page name
    QCString displayTitle;            //!< Friendly title for this collection
    QCString pageName;                //!< Page name for traceability output
    std::map<QCString, Requirement> requirements; //!< Map of requirement ID -> Requirement (for quick lookup)
  };

  // Stores information about an Entry that has satisfies/verifies annotations
  struct EntryRequirementInfo {
    std::shared_ptr<Entry> entry;     //!< The entry with requirements
    std::vector<QCString> satisfies;  //!< Requirements it satisfies
    std::vector<QCString> verifies;   //!< Requirements it verifies
  };

  static RequirementsTracker &instance();

  void initialize();
  void collectFromEntries(const std::shared_ptr<Entry> &root);
  void addDocumentationSections();  // Add sections to Definition objects
  void generateTraceabilityPages();
  void finalize();
  void reset();

  bool hasRequirements() const;

  // Get all requirements collections
  const std::vector<RequirementsCollection>& collections() const { return m_collections; }

  // Get a requirements collection by prefix (e.g., "A024_SRS" -> finds "A024_SRS_" collection)
  RequirementsCollection* findCollectionByPrefix(const QCString &requirementId);

  // Add a satisfied/verified reference string to a requirement
  void addSatisfiedBy(const QCString &requirementId, const QCString &reference);
  void addVerifiedBy(const QCString &requirementId, const QCString &reference);

private:
  RequirementsTracker() = default;
  RequirementsTracker(const RequirementsTracker &) = delete;
  RequirementsTracker &operator=(const RequirementsTracker &) = delete;

  void parseRequirementsTagFile(RequirementsCollection &collection, const QCString &tagFilePath, const QCString &destination);
  void parseRequirementsFile(RequirementsCollection &collection, const QCString &absFilePath, const QCString &originalPath);
  QCString extractPrefix(const QCString &requirementId) const;
  void generateTraceabilityPage(const RequirementsCollection &collection);
  QCString getQualifiedNameForEntry(const Entry *entry) const;

  bool m_initialized = false;
  std::vector<RequirementsCollection> m_collections; //!< All requirement collections
  std::vector<EntryRequirementInfo> m_entryRequirements; //!< Entries with requirement annotations
};

#endif
