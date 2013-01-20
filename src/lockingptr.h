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

#ifndef LOCKINGPTR_H
#define LOCKINGPTR_H

/*! @brief Abstract interface for lockable objects.
 *
 *  By implementing this interface, a smart pointer can be created which
 *  will lock this object. This is used to prevent that an internal pointer
 *  owned by a lockable object would become invalid when the object is removed from 
 *  memory, leaving the client with an invalid pointer. By letting the client use 
 *  a smart pointer instead of the real object the object will be locked into
 *  memory until the pointer is no longer used, at which point the owner object will be
 *  unlock and can be removed from memory.
 */
class LockableObj
{
  public:
    LockableObj() : m_lockCount(0) {}
    virtual ~LockableObj() {}

    /*! Returns TRUE if the object is currently locked. */
    bool isLocked() const { return m_lockCount>0; }

//VC++6.0 workaround
//  protected:
    /*! Called when the object is locked. */
    virtual void lock() const = 0;

    /*! Called when the object is unlocked. */
    virtual void unlock() const = 0;

//VC++6.0 workaround
//  private:
//    template<class T> friend class LockingPtr;
    int  m_lockCount;
};

/*! @brief Smart pointer which keeps a lock on the owner of the pointer.
 *
 *  With the pointer an owner object derived from LockableObj is associated.
 *  As long as the smart object exists it will keep a lock on the obj by calling 
 *  LockableObj::lock(). Smart pointers can be copied and passed by value. As 
 *  soon as there or no more smart pointer references to the object, 
 *  LockableObj::unlock() will be called automatically.
 */
template<class T> class LockingPtr
{
    LockableObj *m_owner;
    const T *m_ptr;

  public:
    /*! Creates a smart pointer for pointer \a p owned by object \a o.
     */
    LockingPtr(const LockableObj *o,const T* p) 
    { 
      if (o->m_lockCount==0) o->lock(); 
      m_owner = (LockableObj *)o;
      m_owner->m_lockCount++;
      m_ptr = p;
    }
    
    /*! Copies the smart pointer \a lp 
     */
    LockingPtr(const LockingPtr &lp)
    {
      m_ptr   = lp.m_ptr;
      m_owner = lp.m_owner;
      m_owner->m_lockCount++;
    }

    /*! Assigns the smart pointer \a lp 
     */
    LockingPtr &operator=(const LockingPtr &lp)
    {
      m_owner->m_lockCount--;
      if (m_owner->m_lockCount==0) // no more references
      {
        m_owner->unlock(); 
      }
      m_ptr   = lp.m_ptr;
      m_owner = lp.m_owner;
      m_owner->m_lockCount++;
      return *this;
    }

    /*! Destroys the smart pointer, will unlock the owner.
     */
    ~LockingPtr() 
    { 
      m_owner->m_lockCount--;
      if (m_owner->m_lockCount==0) // no more references
      {
        m_owner->unlock(); 
      }
    }

    bool isNull() const
    {
      return m_ptr==0;
    }

    bool operator!() const
    {
      return !m_ptr;
    }

    bool operator==(T *p) const
    {
      return m_ptr==p;
    }

    bool operator==(const LockingPtr &lp) const
    {
      return m_ptr==lp.m_ptr;
    }

    bool operator!=(T *p) const
    {
      return m_ptr!=p;
    }

    bool operator!=(const LockingPtr &lp) const
    {
      return m_ptr!=lp.m_ptr;
    }

    /*! Dereference operator */
    const T& operator* () const
    { 
      return *m_ptr; 
    }

    T* pointer() const
    {
      return (T*)m_ptr;
    }
    
    /*! Pointer operator */
    T* operator-> () const
    { 
      return (T*)m_ptr; 
    }
};

#endif // LOCKINGPTR_H

