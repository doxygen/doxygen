/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#ifndef _SORTDICT_H
#define _SORTDICT_H

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>

template<class T> class SDict;

/*! internal wrapper class that redirects compareItems() to the 
 *  dictionary 
 */
template<class T>
class SList : public QList<T>
{
  public:
    SList(SDict<T> *owner) : m_owner(owner) {}
    ~SList() {}
    int compareItems(GCI item1,GCI item2)
    {
      return m_owner->compareItems(item1,item2);
    }
  private:
    SDict<T> *m_owner;  
};

/*! Ordered dictionary of elements of type T. 
 *  Internally uses a QList<T> and a QDict<T>.
 */
template<class T>
class SDict 
{
  private:
    SList<T> *m_list;
    QDict<T> *m_dict;
    
  public:
    /*! Create an ordered dictionary.
     *  \param size The size of the dictionary. Should be a prime number for
     *              best distribution of elements.
     */
    SDict(int size) 
    {
      m_list = new SList<T>(this);
      m_dict = new QDict<T>(size);
    }
    /*! Destroys the dictionary */
    virtual ~SDict() 
    {
      delete m_list;
      delete m_dict;
    }
    /*! Appends a compound to the dictionary. The element is owned by the
     *  dictionary.
     *  \param key The unique key to use to quicky find the item later on.
     *  \param d The compound to add.
     *  \sa find()
     */
    void append(const char *key,const T *d)
    {
      m_list->append(d);
      m_dict->insert(key,d);
    }
    /*! Inserts a compound into the dictionary in a sorted way.
     *  \param key The unique key to use to quicky find the item later on.
     *  \param d The compound to add.
     *  \sa find()
     */
    void inSort(const char *key,const T *d)
    {
      m_list->inSort(d);
      m_dict->insert(key,d);
    }
    /*! Indicates whether or not the dictionary owns its elements */
    void setAutoDelete(bool val)
    {
      m_list->setAutoDelete(val);
    }
    /*! Looks up a compound given its key. 
     *  \param key The key to identify this element.
     *  \return The requested compound or zero if it cannot be found.
     *  \sa append() 
     */
    T *find(const char *key)
    {
      return m_dict->find(key);
    }
    /*! Function that is used to compare two items when sorting.
     *  Overload this to properly sort items.
     *  \sa inSort()
     */
    virtual int compareItems(GCI item1,GCI item2)
    {
      return item1!=item2;
    }
    /*! Clears the dictionary. Will delete items if setAutoDelete() was
     *  set to \c TRUE.
     *  \sa setAutoDelete
     */
    void clear()
    {
      m_list->clear();
      m_dict->clear();
    }
    /*! Returns the number of items stored in the dictionary
     */
    int count()
    {
      return m_list->count();
    }

    class Iterator;         // first forward declare
    friend class Iterator;  // then make it a friend
    /*! Simple iterator for SDict. It iterates in the order in which the
     *  elements are stored.
     */
    class Iterator
    {
      public:
        /*! Create an iterator given the dictionary. */
        Iterator(const SDict<T> &dict)
        {
          m_li = new QListIterator<T>(*dict.m_list);
        }
        /*! Destroys the dictionary */
        virtual ~Iterator()
        {
          delete m_li;
        }
        /*! Set the iterator to the first element in the list. 
         *  \return The first compound, or zero if the list was empty. 
         */
        T *toFirst() const
        {
          return m_li->toFirst();
        }
        /*! Returns the current compound */
        T *current() const
        {
          return m_li->current();
        }
        /*! Moves the iterator to the next element.
         *  \return the new "current" element, or zero if the iterator was
         *          already pointing at the last element.
         */
        T *operator++()
        {
          return m_li->operator++();
        }

      private:
        QListIterator<T> *m_li;
    };

};

#endif
