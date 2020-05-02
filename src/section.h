/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#ifndef SECTION_H
#define SECTION_H

#include <string>
#include <unordered_map>

#include <qcstring.h>

#include "linkedmap.h"

class Definition;

//! enum representing the various types of sections and entities that can be referred to.
enum class SectionType
{
  Page          = 0,
  Section       = 1,
  Subsection    = 2,
  Subsubsection = 3,
  Paragraph     = 4,
  Anchor        = 5,
  Table         = 6
};

//! return true if type is a section, and false if it is a page, anchor or table.
inline constexpr bool isSection(SectionType type)
{
  return (type==SectionType::Section       ||
          type==SectionType::Subsection    ||
          type==SectionType::Subsubsection ||
          type==SectionType::Paragraph);
}

//! class that provide information about a section.
class SectionInfo
{
  public:
    SectionInfo(const char *label, const char *fileName, int lineNr,
                const char *title, SectionType type, int level,const char *ref) :
        m_label(label), m_title(title), m_type(type), m_ref(ref),
        m_lineNr(lineNr), m_fileName(fileName), m_level(level)
    {
      //printf("SectionInfo(%p)\n",this);
    }
    ~SectionInfo()
    {
      //printf("~SectionInfo(%p)\n",this);
    }

    // getters
    QCString    label()      const { return m_label;      }
    QCString    title()      const { return m_title;      }
    SectionType type()       const { return m_type;       }
    QCString    ref()        const { return m_ref;        }
    int         lineNr()     const { return m_lineNr;     }
    QCString    fileName()   const { return m_fileName;   }
    bool        generated()  const { return m_generated;  }
    int         level()      const { return m_level;      }
    Definition *definition() const { return m_definition; }

    // setters
    void setFileName(const char *fn)  { m_fileName   = fn; }
    void setType(SectionType t)       { m_type       = t;  }
    void setGenerated(bool b)         { m_generated  = b;  }
    void setDefinition(Definition *d) { m_definition = d;  }

  private:
    QCString    m_label;
    QCString    m_title;
    SectionType m_type;
    QCString    m_ref;
    int         m_lineNr;
    QCString    m_fileName;
    bool        m_generated = false;
    int         m_level;
    Definition *m_definition = 0;
};

//! class that represents a list of constant references to sections.
class SectionRefs
{
    using SectionInfoVec = std::vector<const SectionInfo*>;
  public:
    using const_iterator = SectionInfoVec::const_iterator;

    //! Returns a constant pointer to the section info given a section label or nullptr
    //! if no section with the given label can be found.
    const SectionInfo *find(const char *label) const
    {
      auto it = m_lookup.find(label);
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    //! Adds a non-owning section reference.
    void add(const SectionInfo *si)
    {
      m_lookup.insert({toStdString(si->label()),si});
      m_entries.push_back(si);
    }

    const_iterator begin() const { return m_entries.cbegin(); }
    const_iterator end()   const { return m_entries.cend(); }
    bool empty() const { return m_entries.empty(); }
    size_t size() const { return m_entries.size(); }

  private:
    SectionInfoVec m_entries;
    std::unordered_map< std::string, const SectionInfo* > m_lookup;
};

//! singleton class that owns the list of all sections
class SectionManager : public LinkedMap<SectionInfo>
{
  public:
    //! Add a new section given the data of an existing section.
    //! Returns a non-owning pointer to the newly added section.
    SectionInfo *add(const SectionInfo &si)
    {
      return LinkedMap<SectionInfo>::add(si.label(),si.fileName(),si.lineNr(),si.title(),si.type(),si.level(),si.ref());
    }

    //! Add a new section
    //! Return a non-owning pointer to the newly added section
    SectionInfo *add(const char *label, const char *fileName, int lineNr,
                     const char *title, SectionType type, int level,const char *ref=0)
    {
      return LinkedMap<SectionInfo>::add(label,fileName,lineNr,title,type,level,ref);
    }

    //! returns a reference to the singleton
    static SectionManager &instance()
    {
      static SectionManager sm;
      return sm;
    }

  private:
    SectionManager() {}
    SectionManager(const SectionManager &other) = delete;
    SectionManager &operator=(const SectionManager &other) = delete;
};


#endif
