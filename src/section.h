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

#include "qcstring.h"
#include "linkedmap.h"
#include "construct.h"

class Definition;

class SectionType
{
  public:
    static constexpr int Page            = 0;
    static constexpr int MinLevel        = 1;
    static constexpr int Section         = 1;
    static constexpr int Subsection      = 2;
    static constexpr int Subsubsection   = 3;
    static constexpr int Paragraph       = 4;
    static constexpr int Subparagraph    = 5;
    static constexpr int Subsubparagraph = 6;
    static constexpr int MaxLevel        = 6;
    static constexpr int Anchor          = 7;
    static constexpr int Table           = 8;

    constexpr SectionType() : m_level(0) {}
    constexpr SectionType(int lvl) : m_level(lvl) {}
    constexpr int level() const { return m_level; }
    constexpr bool isSection() const
    {
      return m_level>=SectionType::MinLevel && m_level<=SectionType::MaxLevel;
    }

  private:
    int m_level;
};

//! class that provide information about a section.
class SectionInfo
{
  public:
    SectionInfo(const QCString &label, const QCString &fileName, int lineNr,
                const QCString &title, SectionType type, int level,const QCString &ref) :
        m_label(label), m_title(title), m_type(type), m_ref(ref),
        m_lineNr(lineNr), m_fileName(fileName), m_level(level)
    {
      //printf("SectionInfo(%p) fileName=%s\n",(void*)this,qPrint(fileName));
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
    void setFileName(const QCString &fn) { m_fileName   = fn; }
    void setType(SectionType t)          { m_type       = t;  }
    void setGenerated(bool b)            { m_generated  = b;  }
    void setDefinition(Definition *d)    { m_definition = d;  }
    void setTitle(const QCString &t)     { m_title      = t;  }
    void setLevel(int l)                 { m_level      = l;  }
    void setReference(const QCString &r) { m_ref        = r;  }
    void setLineNr(int l)                { m_lineNr     = l;  }

  private:
    QCString    m_label;
    QCString    m_title;
    SectionType m_type;
    QCString    m_ref;
    int         m_lineNr;
    QCString    m_fileName;
    bool        m_generated = false;
    int         m_level;
    Definition *m_definition = nullptr;
};

//! class that represents a list of constant references to sections.
class SectionRefs
{
    using SectionInfoVec = std::vector<const SectionInfo*>;
  public:
    using const_iterator = SectionInfoVec::const_iterator;

    //! Returns a constant pointer to the section info given a section label or nullptr
    //! if no section with the given label can be found.
    const SectionInfo *find(const QCString &label) const
    {
      auto it = m_lookup.find(label.str());
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    //! Adds a non-owning section reference.
    void add(const SectionInfo *si)
    {
      m_lookup.emplace(toStdString(si->label()),si);
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
      return LinkedMap<SectionInfo>::add(si.label(),si.fileName(),
                      si.lineNr(),si.title(),si.type(),si.level(),si.ref());
    }

    //! Add a new section
    //! Return a non-owning pointer to the newly added section
    SectionInfo *add(const QCString &label, const QCString &fileName, int lineNr,
                     const QCString &title, SectionType type, int level,const QCString &ref=QCString())
    {
      return LinkedMap<SectionInfo>::add(label.data(),fileName,lineNr,title,type,level,ref);
    }

    //! Replace an existing section with a new one
    //! Return a non-owning pointer to the newly added section
    SectionInfo *replace(const QCString &label, const QCString &fileName, int lineNr,
                         const QCString &title, SectionType type, int level,const QCString &ref=QCString())
    {
      SectionInfo *si = LinkedMap<SectionInfo>::find(label.data());
      if (si)
      {
        si->setFileName(fileName);
        si->setLineNr(lineNr);
        si->setTitle(title);
        si->setType(type);
        si->setLevel(level);
        si->setReference(ref);
        return si;
      }
      else
      {
        return LinkedMap<SectionInfo>::add(label.data(),fileName,lineNr,title,type,level,ref);
      }
    }

    //! returns a reference to the singleton
    static SectionManager &instance()
    {
      static SectionManager sm;
      return sm;
    }

  private:
    SectionManager() = default;
   ~SectionManager() = default;
    NON_COPYABLE(SectionManager)
};


#endif
