/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include <memory>
#include <vector>

#include "construct.h"
#include "dstring.h"

class Definition;
class Entry;
class MemberDef;
class OutputList;
class PageDef;
class TextStream;

/// Types are references to a requirement
enum class RequirementRefType { Satisfies, Verifies };

/// Class to hold requirement reference information
class RequirementRef
{
  public:
    RequirementRef(RequirementRefType type,const DString &reqId,const DString &title,const DString &file,int line)
      : m_type(type), m_reqId(reqId), m_title(title), m_file(file), m_line(line) {}
    RequirementRefType  type()  const { return m_type;  }
    DString            reqId() const { return m_reqId; }
    DString            title() const { return m_title; }
    DString            file()  const { return m_file;  }
    int                 line()  const { return m_line;  }

  private:
    RequirementRefType m_type;
    DString           m_reqId;
    DString           m_title;
    DString           m_file;
    int                m_line;
};

/// List of requirement references
using RequirementRefs = std::vector<RequirementRef>;

void splitRequirementRefs(const RequirementRefs &input,RequirementRefs &satisfiesRefs,RequirementRefs &verifiesRefs);

class RequirementIntf
{
  public:
    using DefinitionVector = std::vector<const Definition *>;
    ABSTRACT_BASE_CLASS(RequirementIntf)
    virtual DString id()                const = 0;
    virtual DString file()              const = 0;
    virtual int      line()              const = 0;
    virtual DString title()             const = 0;
    virtual DString doc()               const = 0;
    virtual DString getTagFile()        const = 0;
    virtual DString getExtPage()        const = 0;
    virtual DString getOutputFileBase() const = 0;
    virtual const DefinitionVector &satisfiedBy() const = 0;
    virtual const DefinitionVector &verifiedBy()  const = 0;
};

using RequirementIntfList = std::vector<const RequirementIntf *>;

class RequirementManager
{
  public:
    static RequirementManager &instance();
    void addRequirement(Entry *e);
    void generatePage();
    void writeTagFile(TextStream &tagFile);
    void addRequirementRefsForSymbol(const Definition *symbol);
    const RequirementIntf *find(const DString &reqId) const;
    void writeRef(OutputList &ol,const RequirementRef &ref);
    bool hasRequirements() const;
    RequirementIntfList requirements() const;
    const PageDef *requirementsPage() const;
  private:
    RequirementManager();
    class Private;
    std::unique_ptr<Private> p;
};

#endif // REQUIREMENT_H
