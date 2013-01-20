/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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

#ifndef _REFLIST_H
#define _REFLIST_H

#include <qintdict.h>
#include <qlist.h>
#include "sortdict.h"

/** This struct represents an item in the list of references. */
struct RefItem
{
  RefItem() /*: written(FALSE)*/ {}
  QCString text;           //!< text of the item.
  QCString listAnchor;     //!< anchor in the list

  QCString prefix;         //!< type prefix for the name
  QCString name;           //!< name of the entity containing the reference
  QCString title;          //!< display name of the entity
  QCString args;           //!< optional arguments for the entity (if function)
  //bool written;
  QList<RefItem> extraItems; //!< more items belonging to the same entity
};

/** List of items sorted by title */
class SortedRefItems : public SDict<RefItem>
{
  public:
    SortedRefItems(int size=17) : SDict<RefItem>(size) {}
    virtual ~SortedRefItems() {}
    int compareItems(QCollection::Item item1,QCollection::Item item2)
    {
      RefItem *r1 = (RefItem*)item1;
      RefItem *r2 = (RefItem*)item2;
      return qstricmp(r1->title,r2->title);
    }
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
    int      addRefItem();
    RefItem *getRefItem(int todoItemId);
    RefItem *getFirstRefItem();
    RefItem *getNextRefItem();
    QCString listName() const;
    QCString pageTitle() const;
    QCString sectionTitle() const;

    RefList(const char *listName,
            const char *pageTitle,const char *secTitle
           );
   ~RefList();
    void insertIntoList(const char *key,RefItem *item);
    void generatePage();

  private:
    int m_id;
    QCString m_listName;
    QCString m_pageTitle;
    QCString m_secTitle;
    SortedRefItems *m_itemList;
    QIntDict<RefItem> *m_dict;
    QIntDictIterator<RefItem> *m_dictIterator;
};

#endif
