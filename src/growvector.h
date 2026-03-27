/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#ifndef GROWVECTOR_H
#define GROWVECTOR_H

#include <vector>
#include <memory>
#include <iterator>

#include "construct.h"

/** @brief std::vector like container optimized for pushing elements to the back.
 *
 *  It differs from std::vector in that it can grow without invalidating
 *  pointers to its members just like std::deque and std::list.
 *
 *  It differs from std::deque in that the value can be incomplete
 *  just like std::vector.
 *
 *  It differs from std::list in that it does not need to allocate each node
 *  separately and provides random access to its members.
 *
 *  It is implemented as a vector of chunks where each chunk is a fixed capacity vector of T.
 */
template<class T>
class GrowVector
{
  private:
    static const size_t chunkBits = 4; // a chunk holds 2^bits elements
    static const size_t chunkSize = 1 << chunkBits;
    static const size_t chunkMask = chunkSize-1;
    struct Chunk
    {
      Chunk() { data.reserve(chunkSize); }
      std::vector<T> data;
    };
    using ChunkPtr = std::unique_ptr<Chunk>;

  public:
    /// @brief bidirectional iterator
    template<class C,class I>
    class Iterator
    {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = I;
        using pointer           = I*;
        using reference         = I&;

        Iterator(C &vec,size_t pos) : m_vec(&vec), m_pos(pos) {}
        DEFAULT_COPYABLE(Iterator)
        reference operator*() const { return m_vec->at(m_pos); }
        pointer operator->()        { return &m_vec->at(m_pos); }
        Iterator& operator++()      { m_pos++; return *this; }
        Iterator operator++(int)    { Iterator tmp = *this; ++(*this); return tmp; }
        Iterator& operator--()      { m_pos--; return *this; }
        Iterator operator--(int)    { Iterator tmp = *this; --(*this); return tmp; }
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_pos == b.m_pos; }
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_pos != b.m_pos; }

      private:
        C     *m_vec;
        size_t m_pos;
    };
    using iterator       = Iterator<GrowVector,T>;
    using const_iterator = Iterator<const GrowVector,const T>;

    /// returns an iterator to the beginning
    iterator begin()             { return iterator(*this,0); }
    /// returns an iterator to the beginning
    const_iterator begin() const { return const_iterator(*this,0); }

    /// returns an iterator to the end
    iterator end()               { return iterator(*this,size()); }
    /// returns an iterator to the end
    const_iterator end()   const { return const_iterator(*this,size()); }

    /// returns the number of elements
    size_t size() const
    {
      return m_chunks.empty() ? 0 : (m_chunks.size()-1)*chunkSize +
                                     m_chunks.back()->data.size();
    }

    /// adds an element to the end
    void push_back(T &&t)
    {
      make_room();
      m_chunks.back()->data.push_back(std::move(t));
    }

    /// constructs an element in-place at the end
    template<class...Args>
    void emplace_back(Args&&...args)
    {
      make_room();
      m_chunks.back()->data.emplace_back(std::forward<Args>(args)...);
    }

    /// removes the last element
    void pop_back()
    {
      m_chunks.back()->data.pop_back();
      if (m_chunks.back()->data.size()==0) // remove chunk if empty
      {
        m_chunks.pop_back();
      }
    }

    /// access specified element
          T &at(size_t i)       { return m_chunks.at(i>>chunkBits)->data.at(i&chunkMask); }
    /// access specified element
    const T &at(size_t i) const { return m_chunks.at(i>>chunkBits)->data.at(i&chunkMask); }

    /// access the first element
          T &front()            { return m_chunks.front()->data.front(); }
    /// access the first element
    const T &front() const      { return m_chunks.front()->data.front(); }

    /// access the last element
          T &back()             { return m_chunks.back()->data.back(); }
    /// access the last element
    const T &back() const       { return m_chunks.back()->data.back(); }

    /// checks whether the container is empty
    bool empty() const          { return m_chunks.empty(); }

    /// clears the contents
    void clear()                { m_chunks.clear(); }

  private:
    void make_room()
    {
      if (m_chunks.empty() ||
          m_chunks.back()->data.size()==chunkSize) // add new chuck if needed
      {
        m_chunks.push_back(std::make_unique<Chunk>());
      }
    }
    std::vector<ChunkPtr> m_chunks;
};

#endif
