/******************************************************************************
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

#ifndef OBJCACHE_H
#define OBJCACHE_H

//#define CACHE_TEST
//#define CACHE_DEBUG
#define CACHE_STATS

/** @brief Cache for objects.
 *
 *  This cache is used to decide which objects should remain in
 *  memory. It uses a least recently used policy (LRU) to decide
 *  which object should make room for a new object when the cache
 *  is full. An object should be added using add(), and then use()
 *  should be called when the object is used.
 */
class ObjCache
{
  private:
    struct CacheNode
    {
      CacheNode() : next(-1), prev(-1), obj(0) {}
      int next;
      int prev;
      void *obj;
    };
    struct HashNode
    {
      HashNode() : head(-1), nextHash(-1), index(-1), obj(0) {}
      int head;
      int nextHash; 
      int index;
      void *obj;
    };

  public:
    /*! Creates the cache. The number of elements in the cache is 2 to 
     *  the power of \a logSize. 
     */
    ObjCache(unsigned int logSize);

    /*! Deletes the cache and free all internal data-structures used. */
   ~ObjCache();

    /*! Adds \a obj to the cache. When victim is not null, this object is
     *  removed from the cache to make room for \a obj. 
     *  Returns a handle to the object, which can be used by the use()
     *  function, each time the object is used.
     */
    int add(void *obj,void **victim);

    /*! Indicates that this object is used. This will move the object
     *  to the front of the internal LRU list to make sure it is removed last.
     *  The parameter \a handle is returned when called add().
     */
    void use(int handle)
    {
      if (handle==m_lastHandle) return;
      m_lastHandle = handle;
      m_hits++;
      moveToFront(handle);
    }

    /*! Removes the item identified by \a handle from the cache.
     *  @see add()
     */
    void del(int handle);

    /*! Debug function. Prints the LRU list */
    void printLRU();
    /*! Print miss/hits statistics */
    void printStats();

    /*! total size of the cache */
    int size() const { return m_size; }

    /*! number of elements in the cache */
    int count() const { return m_count; }

    int hits() const 
    { 
      return m_hits; 
    }
    int misses() const 
    { 
      return m_misses; 
    }


  private:
    void moveToFront(int index);
    unsigned int hash(void *addr);
    HashNode *hashFind(void *obj);
    HashNode *hashInsert(void *obj);
    void hashRemove(void *obj);

    CacheNode *m_cache;
    HashNode  *m_hash;
    int        m_head;
    int        m_tail;
    int        m_size;
    int        m_count;
    int        m_freeHashNodes;
    int        m_freeCacheNodes;
    int        m_lastHandle;
    int        m_misses;
    int        m_hits;
};

#endif // OBJCACHE_H

