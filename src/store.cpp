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

#include "store.h"
#include "portable.h"


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define BLOCK_SIZE         512 // should be >8 and a power of 2
#define BLOCK_POINTER_SIZE sizeof(portable_off_t)


#define ASSERTS_ENABLED

#ifdef ASSERTS_ENABLED
#define STORE_ASSERT(x) assert(x)
#else
#define STORE_ASSERT(x)
#endif

// Decide to use ftell or keep track of the current file pointer ourselves.
// Since valgrind shows that calling ftell has the unwanted side-effect of
// writing some uninitialized bytes (!) it might be better (and faster) to keep track
// of the current pointer ourselves.
#define USE_FTELL 0

//------------------------------------------------------------------------------------

Store::Store() 
{
  m_file       = 0;
  m_front      = 0;
  m_cur        = 0;
  m_head       = 0;
  m_state      = Init;
  m_reads      = 0;
  m_writes     = 0;
}

Store::~Store()
{
  if (m_file)   fclose(m_file);

  // clean up free list
  while (m_head)
  {
    Node *node = m_head;
    m_head = node->next;
    delete node;
  }
}

int Store::open(const char *name)
{
  int i;
  STORE_ASSERT(m_state==Init);
  if (m_file) return 0; // already open
  m_file = portable_fopen(name,"w+b");
  if (m_file==0) return -1;

  // first block serves as header, so offset=0 can be used as the end of the list.
  for (i=0;i<BLOCK_SIZE/8;i++)
  {
    fputc('D',m_file);
    fputc('O',m_file);
    fputc('X',m_file);
    fputc('Y',m_file);
    fputc('G',m_file);
    fputc('E',m_file);
    fputc('N',m_file);
    fputc(0,m_file);
  }
  m_front  = BLOCK_SIZE;
  m_cur    = BLOCK_SIZE;
  m_head   = 0;
  m_state  = Reading;
  return 0;
}

void Store::close()
{
  if (m_file) fclose(m_file);
  m_file=0;
  m_state  = Init;
}
     
portable_off_t Store::alloc()
{
  STORE_ASSERT(m_state==Reading);
  m_state=Writing;
  portable_off_t pos;
  if (m_head==0) // allocate new block
  {
    //printf("alloc: new block, pos=%lld\n",(long long)m_front);
    if (portable_fseek(m_file,0,SEEK_END)==-1) // go to end of the file
    {
      fprintf(stderr,"Store::alloc: Error seeking to end of file: %s\n",strerror(errno));
      exit(1);
    }
#if USE_FTELL
    pos = portable_ftell(m_file);
    STORE_ASSERT( (pos & (BLOCK_SIZE-1))==0 );
    m_front = pos + BLOCK_SIZE; // move front to end of this block
#else
    m_cur = m_front;
    pos   = m_cur;
    STORE_ASSERT( (pos & (BLOCK_SIZE-1))==0 );
    m_front = pos + BLOCK_SIZE;
#endif
  }
  else // reuse freed block
  {
    //printf("alloc: reuse block: pos=%lld\n",(long long)m_head->pos);
    Node *node = m_head;
    pos = node->pos;
    // point head to next free item
    m_head = node->next;
    delete node;
    // move to start of the block
    if (portable_fseek(m_file,pos,SEEK_SET)==-1)
    {
      fprintf(stderr,"Store::alloc: Error seeking to position %d: %s\n",
          (int)pos,strerror(errno));
      exit(1);
    }
    m_cur = pos;
    STORE_ASSERT( (pos & (BLOCK_SIZE-1))==0 );
  }
  //printf("%x: Store::alloc\n",(int)pos);
  return pos;
}

int Store::write(const char *buf,uint size)
{
  STORE_ASSERT(m_state==Writing);
  //printf("%x: Store::write\n",(int)portable_ftell(m_file));
  do
  {
#if USE_FTELL
    portable_off_t curPos = portable_ftell(m_file);
#else
    portable_off_t curPos = m_cur;
#endif
    int bytesInBlock = (int)(BLOCK_SIZE - BLOCK_POINTER_SIZE - (curPos & (BLOCK_SIZE-1)));
    int bytesLeft    = bytesInBlock<(int)size ? (int)size-bytesInBlock : 0;
    int numBytes     = size - bytesLeft;
    STORE_ASSERT(bytesInBlock>=0);
    STORE_ASSERT(numBytes<=(int)(BLOCK_SIZE-BLOCK_POINTER_SIZE));
    if (numBytes>0)
    {
      if ((int)fwrite(buf,1,numBytes,m_file)!=numBytes)
      {
        fprintf(stderr,"Error writing: %s\n",strerror(errno));
        exit(1);
      }
      m_cur+=numBytes;
      m_writes++;
    }
    if (bytesLeft>0) // still more bytes to write
    {
#if USE_FTELL
      STORE_ASSERT(((portable_ftell(m_file)+BLOCK_POINTER_SIZE)&(BLOCK_SIZE-1))==0);
#else
      STORE_ASSERT(((m_cur+BLOCK_POINTER_SIZE)&(BLOCK_SIZE-1))==0);
#endif
      // allocate new block
      if (m_head==0) // no free blocks to reuse
      {
        //printf("%x: Store::write: new: pos=%x\n",(int)m_front,(int)portable_ftell(m_file));
        // write pointer to next block
        if (fwrite(&m_front,BLOCK_POINTER_SIZE,1,m_file)!=1)
        {
          fprintf(stderr,"Error writing to store: %s\n",strerror(errno));
          exit(1);
        }
        m_cur+=BLOCK_POINTER_SIZE;
#if USE_FTELL
        STORE_ASSERT(portable_ftell(m_file)==(curPos&~(BLOCK_SIZE-1))+BLOCK_SIZE);
#else
        STORE_ASSERT(m_cur==(curPos&~(BLOCK_SIZE-1))+BLOCK_SIZE);
#endif
        // move to next block
        if (portable_fseek(m_file,0,SEEK_END)==-1) // go to end of the file
        {
          fprintf(stderr,"Store::alloc: Error seeking to end of file: %s\n",strerror(errno));
          exit(1);
        }
        m_cur=m_front;
#if USE_FTELL
        STORE_ASSERT(portable_ftell(m_file)==m_front);
#else
        STORE_ASSERT(m_cur==m_front);
#endif
        // move front to the next of the block
        m_front+=BLOCK_SIZE;
      }
      else // reuse block from the free list
      {
        // write pointer to next block
        if (fwrite(&m_head->pos,BLOCK_POINTER_SIZE,1,m_file)!=1)
        {
          fprintf(stderr,"Error writing to store: %s\n",strerror(errno));
          exit(1);
        }
        Node *node = m_head;
        portable_off_t pos = node->pos;
        // point head to next free item
        m_head = node->next;
        delete node;
        // move to start of the block
        if (portable_fseek(m_file,pos,SEEK_SET)==-1)
        {
          fprintf(stderr,"Store::write: Error seeking to position %d: %s\n",
              (int)pos,strerror(errno));
          exit(1);
        }
        m_cur = pos;
        //printf("%x: Store::write: reuse\n",(int)pos);
      }
    }
    size-=numBytes;
    buf+=numBytes;
  }
  while (size>0);
  return size;
}

void Store::end()
{
  STORE_ASSERT(m_state==Writing);
#if USE_FTELL
  portable_off_t curPos = portable_ftell(m_file);
#else
  portable_off_t curPos = m_cur;
#endif
  int bytesInBlock = (int)(BLOCK_SIZE - (curPos & (BLOCK_SIZE-1)));
  //printf("%x: Store::end erasing %x bytes\n",(int)curPos&~(BLOCK_SIZE-1),bytesInBlock);
  //printf("end: bytesInBlock=%x\n",bytesInBlock);
  // zero out rest of the block
  int i;
  for (i=0;i<bytesInBlock;i++)
  {
    fputc(0,m_file);
  }
  m_state=Reading;
}

void Store::release(portable_off_t pos)
{
  STORE_ASSERT(m_state==Reading);
  //printf("release: block pos=%lld\n",(long long)pos);
  STORE_ASSERT(pos>0 && (pos & (BLOCK_SIZE-1))==0);
  // goto end of the block
  portable_off_t cur = pos, next;
  while (1)
  {
    // add new node to the free list
    Node *node = new Node;
    node->next = m_head;
    node->pos = cur;

    m_head = node;
    // goto the end of cur block
    if (portable_fseek(m_file,cur+BLOCK_SIZE-BLOCK_POINTER_SIZE,SEEK_SET)==-1)
    {
      fprintf(stderr,"Store::release: Error seeking to position %d: %s\n",
          (int)(cur+BLOCK_SIZE-BLOCK_POINTER_SIZE),strerror(errno));
      exit(1);
    }
    // read pointer to next block
    if (fread(&next,BLOCK_POINTER_SIZE,1,m_file)!=1)
    {
      fprintf(stderr,"Store::release: Error reading from store: %s\n",strerror(errno));
      exit(1);
    }
    m_cur = cur+BLOCK_SIZE;
    if (next==0) break; // found end of list -> cur is last element
    STORE_ASSERT((next & (BLOCK_SIZE-1))==0);
    cur = next;
    //printf("%x: Store::release\n",(int)cur);
  }
}

void Store::seek(portable_off_t pos)
{
  STORE_ASSERT(m_state==Reading);
  //printf("%x: Store::seek\n",(int)pos);
  if (portable_fseek(m_file,pos,SEEK_SET)==-1)
  {
    fprintf(stderr,"Store::seek: Error seeking to position %d: %s\n",
        (int)pos,strerror(errno));
    exit(1);
  }
  m_cur = pos;
  STORE_ASSERT((pos&(BLOCK_SIZE-1))==0);
}

int Store::read(char *buf,uint size)
{
  STORE_ASSERT(m_state==Reading);
  //printf("%x: Store::read total=%d\n",(int)portable_ftell(m_file),size);
  do
  {
#if USE_FTELL
    portable_off_t curPos = portable_ftell(m_file);
#else
    portable_off_t curPos = m_cur;
#endif
    int bytesInBlock = (int)(BLOCK_SIZE - BLOCK_POINTER_SIZE - (curPos & (BLOCK_SIZE-1)));
    int bytesLeft    = bytesInBlock<(int)size ? (int)size-bytesInBlock : 0;
    int numBytes     = size - bytesLeft;
    //printf("  Store::read: pos=%x num=%d left=%d\n",(int)curPos,numBytes,bytesLeft);

    if (numBytes>0)
    {
      //printf("%x: Store::read: %d out of %d bytes\n",(int)portable_ftell(m_file),numBytes,size);
      if ((int)fread(buf,1,numBytes,m_file)!=numBytes)
      {
        fprintf(stderr,"Error reading from store: %s\n",strerror(errno));
        exit(1);
      }
      m_cur+=numBytes;
      m_reads++;
    }
    if (bytesLeft>0)
    {
      portable_off_t newPos;
      // read offset of the next block
#if USE_FTELL
      STORE_ASSERT(((portable_ftell(m_file)+BLOCK_POINTER_SIZE)&(BLOCK_SIZE-1))==0);
#else
      STORE_ASSERT(((m_cur+BLOCK_POINTER_SIZE)&(BLOCK_SIZE-1))==0);
#endif
      if (fread((char *)&newPos,BLOCK_POINTER_SIZE,1,m_file)!=1)
      {
        fprintf(stderr,"Error reading from store: %s\n",strerror(errno));
        exit(1);
      }
      //printf("%x: Store::read: continue in next block, %d bytes to go\n",(int)newPos,bytesLeft);
      //printf("  Store::read: next block=%x\n",(int)newPos);
      STORE_ASSERT(newPos!=0);
      STORE_ASSERT((newPos&(BLOCK_SIZE-1))==0);
      curPos = newPos;
      // move to next block
      if (portable_fseek(m_file,curPos,SEEK_SET)==-1)
      {
        fprintf(stderr,"Store::read: Error seeking to position %d: %s\n",
            (int)curPos,strerror(errno));
        exit(1);
      }
      m_cur = curPos;
    }

    size-=numBytes;
    buf+=numBytes;
  }
  while (size>0);
  return size;
}

void Store::printFreeList()
{
  printf("FreeList: ");
  while (m_head)
  {
    portable_off_t pos = m_head->pos;
    printf("%x ",(int)pos);
    m_head = m_head->next;
  }
  printf("\n");
}

void Store::printStats()
{
  printf("ObjStore: block size %d bytes, total size %ld blocks, wrote %d blocks, read %d blocks\n",
      BLOCK_SIZE,(long)(m_front/BLOCK_SIZE),m_reads,m_writes);
}

void Store::dumpBlock(portable_off_t s,portable_off_t e)
{
  portable_fseek(m_file,s,SEEK_SET);
  int size = (int)(e-s);
  uchar *buf = new uchar[size];
  if (fread(buf,size,1,m_file)==(size_t)size)
  {
    int i,j;
    for (i=0;i<size;i+=16)
    {
      printf("%08x: ",(int)s+i);
      for (j=i;j<QMIN(size,i+16);j++)
      {
        printf("%02x ",buf[i+j]);
      }
      printf("  ");
      for (j=i;j<QMIN(size,i+16);j++)
      {
        printf("%c",(buf[i+j]>=32 && buf[i+j]<128)?buf[i+j]:'.');
      }
      printf("\n");
    }
  }
  delete[] buf;
  portable_fseek(m_file,m_cur,SEEK_SET);
}

#ifdef  STORE_TEST

int main()
{
  printf("sizeof(portable_off_t)=%d\n",(int)sizeof(portable_off_t));
  Store s;
  if (s.open("test.db")==0)
  {
    const char *str1 = "This is a test message... ";
    const char *str2 = "Another message. ";

    int i,j;
    for (j=0;j<5;j++)
    {
      char buf[100];

      portable_off_t handle = s.alloc();
      for (i=0;i<1000000000;i++)
      {
        s.write(str1,strlen(str1)+1);
      }
      s.end();
      portable_off_t handle2 = s.alloc();
      for (i=0;i<10;i++)
      {
        s.write(str2,strlen(str2)+1);
      }
      s.end();

      s.seek(handle);
      for (i=0;i<3;i++)
      {
        s.read(buf,strlen(str1)+1);
        printf("i=%d Read: %s\n",i,buf);
      }

      s.release(handle);

      s.seek(handle2);
      for (i=0;i<3;i++)
      {
        s.read(buf,strlen(str2)+1);
        printf("i=%d Read: %s\n",i,buf);
      }

      s.release(handle2);
    }

    s.close();
  }
  else
  {
    printf("Open failed! %s\n",strerror(errno));
  }
}

#endif

