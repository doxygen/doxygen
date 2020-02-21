/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include <memory>
#include <vector>
#include <map>
#include <string>

#include <qcstring.h>

class Definition;

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

inline constexpr bool isSection(SectionType type)
{
  return (type==SectionType::Section       ||
          type==SectionType::Subsection    ||
          type==SectionType::Subsubsection ||
          type==SectionType::Paragraph);
}


class SectionInfo
{
  public:
    SectionInfo(const char *fileName, int lineNr, const char *label,
                const char *title, SectionType type, int level, const char *ref):
        m_label(label), m_title(title), m_type(type), m_ref(ref),
        m_lineNr(lineNr), m_fileName(fileName), m_level(level)
    {
      //printf("SectionInfo(%p)\n",this);
    }
    ~SectionInfo()
    {
      //printf("~SectionInfo(%p)\n",this);
    }


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

class SectionRefs
{
    using SectionInfoVec = std::vector<const SectionInfo*>;
  public:
    using iterator = SectionInfoVec::const_iterator;

    const SectionInfo *find(const char *label) const
    {
      auto it = m_lookup.find(label);
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    void add(const SectionInfo *si)
    {
      m_lookup.insert({std::string(si->label()),si});
      m_entries.push_back(si);
    }

    iterator begin() const
    {
      return m_entries.cbegin();
    }

    iterator end() const
    {
      return m_entries.cend();
    }

    bool empty() const
    {
      return m_lookup.empty();
    }

  private:
    SectionInfoVec m_entries;
    std::map< std::string, const SectionInfo* > m_lookup;
};

class SectionManager
{
    using SectionInfoPtr = std::unique_ptr<SectionInfo>;
    using SectionInfoVec = std::vector<SectionInfoPtr>;

  public:
    using iterator = SectionInfoVec::iterator;

    SectionInfo *find(const char *label)
    {
      auto it = m_lookup.find(label);
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    const SectionInfo *find(const char *label) const
    {
      auto it = m_lookup.find(label);
      return it!=m_lookup.end() ? it->second : nullptr;
    }

    SectionInfo *add(const char *fileName,
                     int lineNr,
                     const char *label,
                     const char *title,
                     SectionType type,
                     int level,
                     const char *ref=0)
    {
      SectionInfoPtr si = std::make_unique<SectionInfo>(
          fileName,lineNr,label,title,type,level,ref);
      SectionInfo *result = si.get();
      m_lookup.insert({std::string(label),result});
      m_entries.push_back(std::move(si));
      return result;
    }

    SectionInfo *add(const SectionInfo &si)
    {
      add(si.fileName(),si.lineNr(),si.label(),si.title(),si.type(),si.level(),si.ref());
      return find(si.label());
    }
    iterator begin()
    {
      return m_entries.begin();
    }
    iterator end()
    {
      return m_entries.end();
    }
    bool empty() const
    {
      return m_entries.empty();
    }
    void clear()
    {
      m_entries.clear();
      m_lookup.clear();
    }

    static SectionManager &instance()
    {
      static SectionManager sm;
      return sm;
    }

  private:
    SectionManager() {}
    SectionManager(const SectionManager &other) = delete;
    SectionManager &operator=(const SectionManager &other) = delete;

    SectionInfoVec m_entries;
    std::map< std::string, SectionInfo* > m_lookup;
};


#endif
