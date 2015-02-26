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

#ifndef _SORTDICT_H
#define _SORTDICT_H

#include <qlist.h>
#include <qdict.h>
#include <qintdict.h>

#define AUTORESIZE 1

#if AUTORESIZE
const uint SDict_primes[] = 
{
  17,
  29,
  47,
  71,
  113,
  179,
  293,
  457,
  733,
  1171,
  1871,
  2999,
  4787,
  7669,
  12251,
  19603,
  31379,
  50177,
  80287,
  128449,
  205519,
  328829,
  526139,
  841801,
  1346881,
  2155007,
  3448033,
  5516827,
  8826919,
  14123059,
  23538433,
  39230771,
  65384537,
  108974231,
  181623707,
  302706181,
  504510283,
  840850487,
  0xffffffff
};
#endif

template<class T> class SDict;
template<class T> class SIntDict;

/** internal wrapper class that redirects compareValues() to the 
 *  dictionary 
 */
template<class T>
class SList : public QList<T>
{
  public:
    SList(SDict<T> *owner) : m_owner(owner) {}
    virtual ~SList() {}
    int compareValues(const T *item1,const T *item2) const
    {
      return m_owner->compareValues(item1,item2);
    }
  private:
    SDict<T> *m_owner;  
};

/** Ordered dictionary of elements of type T. 
 *  Internally uses a QList<T> and a QDict<T>.
 */
template<class T>
class SDict 
{
  private:
    SList<T> *m_list;
    QDict<T> *m_dict;
    int m_sizeIndex;
    
  public:
    /*! Create an ordered dictionary.
     *  \param size The size of the dictionary. Should be a prime number for
     *              best distribution of elements.
     *  \param caseSensitive indicated whether the keys should be sorted
     *         in a case sensitive way.
     */
    SDict(int size=17,bool caseSensitive=TRUE) : m_sizeIndex(0)
    {
      m_list = new SList<T>(this);
#if AUTORESIZE
      while ((uint)size>SDict_primes[m_sizeIndex]) m_sizeIndex++;
      m_dict = new QDict<T>(SDict_primes[m_sizeIndex],caseSensitive);
#else
      m_dict = new QDict<T>(size,caseSensitive);
#endif
    }

    /*! Destroys the dictionary */
    virtual ~SDict() 
    {
      delete m_list;
      delete m_dict;
    }

    /*! Appends an element to the dictionary. The element is owned by the
     *  dictionary.
     *  \param key The unique key to use to quicky find the item later on.
     *  \param d The compound to add.
     *  \sa find()
     */
    void append(const char *key,const T *d)
    {
      m_list->append(d);
      m_dict->insert(key,d);
#if AUTORESIZE
      if (m_dict->size()>SDict_primes[m_sizeIndex])
      {
        m_dict->resize(SDict_primes[++m_sizeIndex]);
      }
#endif
    }

    /*! Prepends an element to the dictionary. The element is owned by the
     *  dictionary.
     *  \param key The unique key to use to quicky find the item later on.
     *  \param d The compound to add.
     *  \sa find()
     */
    void prepend(const char *key,const T *d)
    {
      m_list->prepend(d);
      m_dict->insert(key,d);
#if AUTORESIZE
      if (m_dict->size()>SDict_primes[m_sizeIndex])
      {
        m_dict->resize(SDict_primes[++m_sizeIndex]);
      }
#endif
    }

    /*! Remove an item from the dictionary */
    bool remove(const char *key)
    {
      T *item = m_dict->take(key);
      return item ? m_list->remove(item) : FALSE;
    }

    /*! Take an item out of the dictionary without deleting it */
    T *take(const char *key)
    {
      T *item = m_dict->take(key);
      if (item)
      {
        int i = m_list->find(item);
        m_list->take(i);
      }
      return item;
    }

    /*! Sorts the members of the dictionary. First appending a number
     *  of members and then sorting them is faster (O(NlogN) than using 
     *  inSort() for each member (O(N^2)).
     */
    void sort()
    {
      m_list->sort();
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
#if AUTORESIZE
      if (m_dict->size()>SDict_primes[m_sizeIndex])
      {
        m_dict->resize(SDict_primes[++m_sizeIndex]);
      }
#endif
    }

    void insertAt(int i,const char *key,const T *d)
    {
      m_list->insert(i,d);
      m_dict->insert(key,d);
#if AUTORESIZE
      if (m_dict->size()>SDict_primes[m_sizeIndex])
      {
        m_dict->resize(SDict_primes[++m_sizeIndex]);
      }
#endif
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
    T *find(const QCString &key)
    {
      return m_dict->find(key);
    }
    T *find(const QString &key)
    {
      return m_dict->find(key);
    }
    int findAt(const QCString &key)
    {
      T *item = find(key);
      if (item==0) return -1;
      return m_list->find(item);
    }

    /*! Equavalent to find(). */
    T *operator[](const char *key) const
    {
      return m_dict->find(key);
    }

    /*! Returns the item at position \a i in the sorted dictionary */
    T *at(uint i)
    {
      return m_list->at(i);
    }

    /*! Function that is used to compare two items when sorting.
     *  Overload this to properly sort items.
     *  \sa inSort()
     */
    virtual int compareValues(const T *item1,const T *item2) const
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
    int count() const
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

        /*! Set the iterator to the last element in the list. 
         *  \return The first compound, or zero if the list was empty. 
         */
        T *toLast() const
        {
          return m_li->toLast();
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

        /*! Moves the iterator to the previous element.
         *  \return the new "current" element, or zero if the iterator was
         *          already pointing at the first element.
         */
        T *operator--()
        {
          return m_li->operator--();
        }

      private:
        QListIterator<T> *m_li;
    };

    class IteratorDict;         // first forward declare
    friend class IteratorDict;  // then make it a friend
    /*! Simple iterator for SDict. It iterates over the dictionary elements
     *  in an unsorted way, but does provide information about the element's key.
     */
    class IteratorDict
    {
      public:
        /*! Create an iterator given the dictionary. */
        IteratorDict(const SDict<T> &dict)
        {
          m_di = new QDictIterator<T>(*dict.m_dict);
        }

        /*! Destroys the dictionary */
        virtual ~IteratorDict()
        {
          delete m_di;
        }

        /*! Set the iterator to the first element in the list. 
         *  \return The first compound, or zero if the list was empty. 
         */
        T *toFirst() const
        {
          return m_di->toFirst();
        }

        /*! Set the iterator to the last element in the list. 
         *  \return The first compound, or zero if the list was empty. 
         */
        T *toLast() const
        {
          return m_di->toLast();
        }

        /*! Returns the current compound */
        T *current() const
        {
          return m_di->current();
        }
        
        /*! Returns the current key */
        QCString currentKey() const
        {
          return m_di->currentKey();
        }
        
        /*! Moves the iterator to the next element.
         *  \return the new "current" element, or zero if the iterator was
         *          already pointing at the last element.
         */
        T *operator++()
        {
          return m_di->operator++();
        }

        /*! Moves the iterator to the previous element.
         *  \return the new "current" element, or zero if the iterator was
         *          already pointing at the first element.
         */
        T *operator--()
        {
          return m_di->operator--();
        }

      private:
        QDictIterator<T> *m_di;
    };
};

/** internal wrapper class that redirects compareValues() to the 
 *  dictionary 
 */
template<class T>
class SIntList : public QList<T>
{
  public:
    SIntList(SIntDict<T> *owner) : m_owner(owner) {}
    virtual ~SIntList() {}
  private:
    int compareValues(const T *item1,const T *item2) const
    {
      return m_owner->compareValues(item1,item2);
    }
    SIntDict<T> *m_owner;  
};

/** Ordered dictionary of elements of type T. 
 *  Internally uses a QList<T> and a QIntDict<T>.
 */
template<class T>
class SIntDict 
{
  private:
    SIntList<T> *m_list;
    QIntDict<T> *m_dict;
    int m_sizeIndex;
    
  public:
    /*! Create an ordered dictionary.
     *  \param size The size of the dictionary. Should be a prime number for
     *              best distribution of elements.
     */
    SIntDict(int size=17) : m_sizeIndex(0)
    {
      m_list = new SIntList<T>(this);
#if AUTORESIZE
      while ((uint)size>SDict_primes[m_sizeIndex]) m_sizeIndex++;
      m_dict = new QIntDict<T>(SDict_primes[m_sizeIndex]);
#else
      m_dict = new QIntDict<T>(size);
#endif
    }

    /*! Destroys the dictionary */
    virtual ~SIntDict() 
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
    void append(int key,const T *d)
    {
      m_list->append(d);
      m_dict->insert(key,d);
#if AUTORESIZE
      if (m_dict->size()>SDict_primes[m_sizeIndex])
      {
        m_dict->resize(SDict_primes[++m_sizeIndex]);
      }
#endif
    }

    /*! Prepend a compound to the dictionary. The element is owned by the
     *  dictionary.
     *  \param key The unique key to use to quicky find the item later on.
     *  \param d The compound to add.
     *  \sa find()
     */
    void prepend(int key,const T *d)
    {
      m_list->prepend(d);
      m_dict->insert(key,d);
#if AUTORESIZE
      if (m_dict->size()>SDict_primes[m_sizeIndex])
      {
        m_dict->resize(SDict_primes[++m_sizeIndex]);
      }
#endif
    }

    /*! Remove an item from the dictionary */
    bool remove(int key)
    {
      T *item = m_dict->take(key);
      return item ? m_list->remove(item) : FALSE;
    }

    /*! Sorts the members of the dictionary. First appending a number
     *  of members and then sorting them is faster (O(NlogN) than using 
     *  inSort() for each member (O(N^2)).
     */
    void sort()
    {
      m_list->sort();
    }

    /*! Inserts a compound into the dictionary in a sorted way.
     *  \param key The unique key to use to quicky find the item later on.
     *  \param d The compound to add.
     *  \sa find()
     */
    void inSort(int key,const T *d)
    {
      m_list->inSort(d);
      m_dict->insert(key,d);
#if AUTORESIZE
      if (m_dict->size()>SDict_primes[m_sizeIndex])
      {
        m_dict->resize(SDict_primes[++m_sizeIndex]);
      }
#endif
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
    T *find(int key)
    {
      return m_dict->find(key);
    }

    /*! Equavalent to find(). */
    T *operator[](int key) const
    {
      return m_dict->find(key);
    }

    /*! Returns the item at position \a i in the sorted dictionary */
    T *at(uint i)
    {
      return m_list->at(i);
    }

    /*! Function that is used to compare two items when sorting.
     *  Overload this to properly sort items.
     *  \sa inSort()
     */
    virtual int compareValues(const T *item1,const T *item2) const
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
        Iterator(const SIntDict<T> &dict)
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
        
        /*! Set the iterator to the last element in the list. 
         *  \return The first compound, or zero if the list was empty. 
         */
        T *toLast() const
        {
          return m_li->toLast();
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
        
        /*! Moves the iterator to the previous element.
         *  \return the new "current" element, or zero if the iterator was
         *          already pointing at the first element.
         */
        T *operator--()
        {
          return m_li->operator--();
        }

      private:
        QListIterator<T> *m_li;
    };

    class IteratorDict;         // first forward declare
    friend class IteratorDict;  // then make it a friend
    /*! Simple iterator for SDict. It iterates over the dictionary elements
     *  in an unsorted way, but does provide information about the element's key.
     */
    class IteratorDict
    {
      public:
        /*! Create an iterator given the dictionary. */
        IteratorDict(const SIntDict<T> &dict)
        {
          m_di = new QIntDictIterator<T>(*dict.m_dict);
        }

        /*! Destroys the dictionary */
        virtual ~IteratorDict()
        {
          delete m_di;
        }

        /*! Set the iterator to the first element in the list. 
         *  \return The first compound, or zero if the list was empty. 
         */
        T *toFirst() const
        {
          return m_di->toFirst();
        }

        /*! Set the iterator to the last element in the list. 
         *  \return The first compound, or zero if the list was empty. 
         */
        T *toLast() const
        {
          return m_di->toLast();
        }

        /*! Returns the current compound */
        T *current() const
        {
          return m_di->current();
        }
        
        /*! Returns the current key */
        int currentKey() const
        {
          return m_di->currentKey();
        }
        
        /*! Moves the iterator to the next element.
         *  \return the new "current" element, or zero if the iterator was
         *          already pointing at the last element.
         */
        T *operator++()
        {
          return m_di->operator++();
        }

        /*! Moves the iterator to the previous element.
         *  \return the new "current" element, or zero if the iterator was
         *          already pointing at the first element.
         */
        T *operator--()
        {
          return m_di->operator--();
        }

      private:
        QDictIterator<T> *m_di;
    };

};

#endif
