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

#include "qcstring.h"
#include "linkedmap.h"
#include "construct.h"

class Definition;
class RefList;

/** This struct represents an item in the list of references. */
class RefItem
{
  public:
    RefItem(int id,RefList *list) : m_id(id), m_list(list) {}

    void setText  (const QCString &text)   { m_text   = text;   }
    void setAnchor(const QCString &anchor) { m_anchor = anchor; }
    void setPrefix(const QCString &prefix) { m_prefix = prefix; }
    void setName  (const QCString &name)   { m_name   = name;   }
    void setTitle (const QCString &title)  { m_title  = title;  }
    void setArgs  (const QCString &args)   { m_args   = args;   }
    void setGroup (const QCString &group)  { m_group  = group;  }
    void setScope (const Definition *scope) { m_scope  = scope;  }

    QCString text()     const { return m_text;   }
    QCString anchor()   const { return m_anchor; }
    QCString prefix()   const { return m_prefix; }
    QCString name()     const { return m_name;   }
    QCString title()    const { return m_title;  }
    QCString args()     const { return m_args;   }
    QCString group()    const { return m_group;  }
    int id()            const { return m_id;     }
    RefList *list()     const { return m_list;   }
    const Definition *scope() const { return m_scope;  }

  private:
    int m_id = 0;              //!< unique identifier for this item within its list
    RefList *m_list;           //!< list owning this item
    QCString m_text;           //!< text of the item.
    QCString m_anchor;         //!< anchor in the list
    QCString m_prefix;         //!< type prefix for the name
    QCString m_name;           //!< name of the entity containing the reference
    QCString m_title;          //!< display name of the entity
    QCString m_args;           //!< optional arguments for the entity (if function)
    QCString m_group;          //!< group id used to combine item under a single header
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
    RefList(const QCString &listName, const QCString &pageTitle, const QCString &secTitle);
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

    QCString listName() const      { return m_listName;  }
    QCString fileName() const      { return m_fileName;  }
    QCString pageTitle() const     { return m_pageTitle; }
    QCString sectionTitle() const  { return m_secTitle;  }

    void generatePage();

  private:
    int m_id = 0;
    QCString m_listName;
    QCString m_fileName;
    QCString m_pageTitle;
    QCString m_secTitle;
    std::vector< std::unique_ptr< RefItem > > m_entries;
    std::unordered_map< int, RefItem* > m_lookup;
};

class RefListManager : public LinkedMap<RefList>
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
