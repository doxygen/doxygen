/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

#include "reflist.h"

/*! The one and only todo list */
RefList todoList;
/*! The test criteria list */
RefList testList;
/*! The bug list */
RefList bugList;

/*! Create a todo list */
RefList::RefList() 
{ 
  m_dict = 0;
  m_dictIterator = 0;
  m_id = 0;
}

/*! Destroy the todo list. Currently not called! */
RefList::~RefList() 
{ 
  delete m_dictIterator; 
  delete m_dict; 
}

/*! Adds a new item to the list.
 *  \param text The item text.
 *  \returns A unique id for this item.
 */
int RefList::addRefItem()
{
  if (m_dict==0)
  {
    m_dict = new QIntDict<RefItem>(1009); 
    m_dict->setAutoDelete(TRUE);
    m_dictIterator = new QIntDictIterator<RefItem>(*m_dict);
  }
  RefItem *item = new RefItem;
  m_id++;
  m_dict->insert(m_id,item);
  return m_id;
}

/*! Returns an item given it's id that is obtained with addRefItem()
 *  \param itemId item's identifier.
 *  \returns A pointer to the todo item's structure.
 */
RefItem *RefList::getRefItem(int itemId)
{
  return m_dict ? m_dict->find(itemId) : 0;
}

/*! Returns the first item in the dictionary or 0 if 
 *  non is available.
 *  Items are not sorted.
 */
RefItem *RefList::getFirstRefItem()
{
  return m_dictIterator ? m_dictIterator->toFirst() : 0;
}

/*! Returns the next item in the dictionary or 0 if
 *  we are at the end of the list.
 *  Items are not sorted.
 */
RefItem *RefList::getNextRefItem()
{
  return m_dictIterator ? m_dictIterator->operator++() : 0;
}

