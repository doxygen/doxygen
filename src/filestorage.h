/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

#include <qfile.h>
#include <assert.h>
#include "store.h"


#ifndef FILESTORAGE_H
#define FILESTORAGE_H

/** @brief Store implementation based on a file. 
    Writing is linear, after that the file is re-opened for reading.
    Reading is random (seek+read).
 */
class FileStorage : public StorageIntf
{
  public:
    FileStorage() : m_readOnly(FALSE) {}
    FileStorage( const QString &name) : 
                    m_readOnly(FALSE)    { m_file.setName(name); }
    int read(char *buf,uint size)        { return m_file.readBlock(buf,size); }
    int write(const char *buf,uint size) { assert(m_readOnly==FALSE); return m_file.writeBlock(buf,size); }
    bool open( int m )                   { m_readOnly = m==IO_ReadOnly; return m_file.open(m); }
    bool seek(int64 pos)                 { return m_file.seek(pos); }
    int64 pos() const                    { return m_file.pos(); }
    void close()                         { m_file.close(); }
    void setName( const char *name )     { m_file.setName(name); }
  private:
    bool m_readOnly;
    QFile m_file;
};

#if 0 // experimental version using mmap after opening the file as read only.
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

class FileStorage : public StorageIntf
{
  public:
    FileStorage() : m_readOnly(FALSE), m_map(0), m_off(0), m_size(0) {}
    FileStorage( const QString &name) : 
                    m_readOnly(FALSE)    { m_file.setName(name); }
    void setName( const char *name )     { m_file.setName(name); }
    bool open( int m )                   
    { 
      if (m==IO_ReadOnly)
      {
        m_readOnly=TRUE;
        QString name = m_file.name();
        m_file.close();
        m_fd = ::open(name.data(),O_RDONLY);
        struct stat stat;
        fstat(m_fd,&stat);
        m_size = stat.st_size;
        m_map = mmap(NULL,m_size,PROT_READ,MAP_SHARED,m_fd,0);
        if (m_map==MAP_FAILED) perror("mmap failed");
        assert(m_map!=MAP_FAILED);
        m_off = 0;
        return TRUE;
      }
      else
      {
        m_readOnly = FALSE;
        return m_file.open(m); 
      }
    }
    int write(const char *buf,uint size) 
    { 
      assert(m_map==0);
      return m_file.writeBlock(buf,size); 
    }
    int read(char *buf,uint size)        
    { 
      assert(m_map!=0);
      memcpy(buf,((char *)m_map)+m_off,size); 
      m_off+=size; 
      return size; 
    }
    bool seek(int64 pos)                 
    { 
      m_off=pos; 
      return TRUE; 
    }
    int64 pos() const                    
    { 
      if (m_readOnly)
      {
        return m_off; 
      }
      else
      {
        return m_file.pos();
      }
    }
    void close()                         
    { 
      if (m_readOnly)
      {
        munmap(m_map,m_size); 
        ::close(m_fd);
        exit(1);
      }
      else
      {
        m_file.close();
      }
    }
  private:
    bool m_readOnly;
    QFile m_file;
    int m_fd;
    void *m_map;
    off_t m_off;
    off_t m_size;
};
#endif

#endif
