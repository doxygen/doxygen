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

/*! Create a list of items that are cross referenced with documentation blocks
 *  @param listName String representing the name of the list.
 *  @param optionName String representation of the option enabling the list.
 *  @param pageTitle String representing the title of the list page.
 *  @param secTitle String representing the title of the section.
 *  @param type Type of the section.
 */
RefList::RefList(const char *listName,const char *optionName,const char *pageTitle,
                 const char *secTitle,BaseOutputDocInterface::SectionTypes type)
{ 
  m_dict = 0;
  m_dictIterator = 0;
  m_id = 0;
  m_listName = listName;
  m_optionName = optionName;
  m_pageTitle = pageTitle;
  m_secTitle = secTitle;
  m_type = type;
}

/*! Destroy the todo list. Currently not called! */
RefList::~RefList() 
{ 
  delete m_dictIterator; 
  delete m_dict; 
}

/*! Adds a new item to the list.
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

/*! Returns the name of the list as set in the constructor. */
QCString RefList::listName() const
{
  return m_listName;
}

QCString RefList::optionName() const
{
  return m_optionName;
}

QCString RefList::pageTitle() const
{
  return m_pageTitle;
}

QCString RefList::sectionTitle() const
{
  return m_secTitle;
}

BaseOutputDocInterface::SectionTypes RefList::sectionType() const
{
  return m_type;
}

