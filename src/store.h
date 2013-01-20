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

#ifndef STORE_H
#define STORE_H

#include <qglobal.h>
#include <stdio.h>

#include "portable.h"

/*! @brief Abstract interface for file based memory storage operations */
class StorageIntf
{
  public:
    /*! Required by gcc */
    virtual ~StorageIntf() {}
    /*! Read \a size bytes from the store into \a buf. */
    virtual int read(char *buf,uint size) = 0;
    /*! Write \a size bytes from \a buf into the store. */
    virtual int write(const char *buf,uint size) = 0;
};

/*! @brief The Store is a file based memory manager.
 *
 *  You can open the store using open(). Then obtain a handle via alloc()
 *  followed by a sequence of write() commands to store information,
 *  and finalize it using end(). 
 *  
 *  Later on you locate the information
 *  with seek() using the handle obtained with alloc(), and then use a
 *  sequence of read() calls to read the information back. 
 *
 *  If no longer needed the storage space can be freed using release().
 *  
 *  The store will dynamically grow the file on disk if needed.
 */
class Store : public StorageIntf
{
  public:
    /*! Creates a store. */
    Store();

    /*! Releases the store object. Will close the underlying file if opened. */
   ~Store();

    /*! Opens the file underlying the store using \a name as the file name. 
     *  Returns 0 upon success, or -1 otherwise.
     */
    int open(const char *name);

    /*! Allocates a handle to write to and read from. */
    portable_off_t alloc();

    /*! Writes \a size bytes in array \a buf to the store. 
     *  First alloc() has to be called.
     *  \note The information can only be read after end() has been called.
     */
    int write(const char *buf,uint size);

    /*! Ends the sequence of writes. 
     *  \note After this call, first alloc() has to be called
     *  before new writes can be done.
     */
    void end();

    /*! Releases the memory corresponding to the handle returned with alloc() */
    void release(portable_off_t handle);

    /*! Closes the store */
    void close();

    /*! Goes to the start of information corresponding to handle \a pos */
    void seek(portable_off_t handle);

    /*! Reads \a size bytes from the store into the array pointed to be \a buf.
     *  \note Before reading seek() has to be called to set the right start of the store.
     */
    int read(char *buf,uint size);

    void printStats();

    portable_off_t pos() const { return m_cur; }

    void dumpBlock(portable_off_t start,portable_off_t end);

  private:
    enum State
    {
      Init,
      Reading,
      Writing
    };
    struct Node
    {
      portable_off_t pos;
      struct Node *next;
    };
    void printFreeList();
    FILE *m_file;
    portable_off_t m_front;
    portable_off_t m_cur;
    Node *m_head;
    State m_state;
    int m_reads;
    int m_writes;
};

#endif
