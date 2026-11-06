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

#ifndef REFLIST_H
#define REFLIST_H

#include <vector>
#include <unordered_map>
#include <memory>

#include "dstring.h"
#include "linkedmap.h"
#include "construct.h"

class Definition;
class RefList;

/** This struct represents an item in the list of references. */
class RefItem
{
  public:
    RefItem(int id,RefList *list) : m_id(id), m_list(list) {}

    void setText  (const DString &text)   { m_text   = text;   }
    void setAnchor(const DString &anchor) { m_anchor = anchor; }
    void setPrefix(const DString &prefix) { m_prefix = prefix; }
    void setName  (const DString &name)   { m_name   = name;   }
    void setTitle (const DString &title)  { m_title  = title;  }
    void setArgs  (const DString &args)   { m_args   = args;   }
    void setGroup (const DString &group)  { m_group  = group;  }
    void setScope (const Definition *scope) { m_scope  = scope;  }

    DString text()     const { return m_text;   }
    DString anchor()   const { return m_anchor; }
    DString prefix()   const { return m_prefix; }
    DString name()     const { return m_name;   }
    DString title()    const { return m_title;  }
    DString args()     const { return m_args;   }
    DString group()    const { return m_group;  }
    int id()            const { return m_id;     }
    RefList *list()     const { return m_list;   }
    const Definition *scope() const { return m_scope;  }

  private:
    int m_id = 0;              //!< unique identifier for this item within its list
    RefList *m_list;           //!< list owning this item
    DString m_text;           //!< text of the item.
    DString m_anchor;         //!< anchor in the list
    DString m_prefix;         //!< type prefix for the name
    DString m_name;           //!< name of the entity containing the reference
    DString m_title;          //!< display name of the entity
    DString m_args;           //!< optional arguments for the entity (if function)
    DString m_group;          //!< group id used to combine item under a single header
    const Definition *m_scope = nullptr;   //!< scope to use for references.
};

/** List of cross-referenced items
 *
 *  This class represents a list of items that are put
 *  at a certain point in the documentation by some special command
 *  and are collected in a list. The items cross-reference the
 *  documentation and the list.
 *
 *  Examples are the todo list, the test list and the bug list,
 *  introduced by the \\todo, \\test, and \\bug commands respectively.
 */
class RefList
{
  public:
    /*! Create a list of items that are cross referenced with documentation blocks
     *  @param listName String representing the name of the list.
     *  @param pageTitle String representing the title of the list page.
     *  @param secTitle String representing the title of the section.
     */
    RefList(const DString &listName, const DString &pageTitle, const DString &secTitle);
    bool isEnabled() const;

    /*! Adds a new item to the list.
     *  @returns A unique id for this item.
     */
    RefItem *add();

    /*! Returns an item given it's id that is obtained with addRefItem()
     *  @param itemId item's identifier.
     *  @returns A pointer to the todo item's structure.
     */
    RefItem *find(int itemId);

    DString listName() const      { return m_listName;  }
    DString fileName() const      { return m_fileName;  }
    DString pageTitle() const     { return m_pageTitle; }
    DString sectionTitle() const  { return m_secTitle;  }

    void generatePage();

  private:
    int m_id = 0;
    DString m_listName;
    DString m_fileName;
    DString m_pageTitle;
    DString m_secTitle;
    std::vector< std::unique_ptr< RefItem > > m_entries;
    std::unordered_map< int, RefItem* > m_lookup;
};

class RefListManager final : public LinkedMap<RefList>
{
  public:
    static RefListManager &instance()
    {
      static RefListManager rlm;
      return rlm;
    }

  private:
    RefListManager() = default;
   ~RefListManager() = default;
    NON_COPYABLE(RefListManager)
};

using RefItemVector = std::vector<RefItem*>;

#endif
