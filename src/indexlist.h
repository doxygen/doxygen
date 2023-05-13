/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#ifndef INDEXLIST_H
#define INDEXLIST_H

#include <utility>
#include <vector>
#include <memory>
#include <mutex>
#include <variant>

#include "qcstring.h"
#include "dispatcher.h"

#include "docsets.h"
#include "eclipsehelp.h"
#include "ftvhelp.h"
#include "htmlhelp.h"
#include "qhp.h"
#include "sitemap.h"

class Definition;
class MemberDef;
class OutputList;

/** Namespace containing typed wrappers to refer to member functions for each method of the indices using a fixed `method` static variable.
 *  Based on this idea https://stackoverflow.com/a/71357544/784672
 */
namespace IndexIntf
{
  template <class T> struct initialize        { static constexpr auto method = &T::initialize;        };
  template <class T> struct finalize          { static constexpr auto method = &T::finalize;          };
  template <class T> struct incContentsDepth  { static constexpr auto method = &T::incContentsDepth;  };
  template <class T> struct decContentsDepth  { static constexpr auto method = &T::decContentsDepth;  };
  template <class T> struct addContentsItem   { static constexpr auto method = &T::addContentsItem;   };
  template <class T> struct addIndexItem      { static constexpr auto method = &T::addIndexItem;      };
  template <class T> struct addIndexFile      { static constexpr auto method = &T::addIndexFile;      };
  template <class T> struct addImageFile      { static constexpr auto method = &T::addImageFile;      };
  template <class T> struct addStyleSheetFile { static constexpr auto method = &T::addStyleSheetFile; };
}

/** \brief A list of index interfaces.
 *
 *  This class itself implements all methods of IndexIntf and
 *  just forwards the calls to all items in the list (composite design pattern).
 */
class IndexList
{
    using IndexVariant = std::variant<DocSets, EclipseHelp, FTVHelp, HtmlHelp, Qhp, Sitemap>;

    template<template <class> class IndexT, class... As>
    void foreach(As&&... args)
    {
      for (auto &v : m_indices)
      {
        dispatch_call<IndexT>(v,std::forward<As>(args)...);
      }
    }
    template<template <class> class IndexT, class... As>
    void foreach_locked(As&&... args)
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto &v : m_indices)
      {
        dispatch_call<IndexT>(v,std::forward<As>(args)...);
      }
    }

  public:
    /** disable the indices */
    void disable()
    { m_enabled = FALSE; }

    /** enable the indices */
    void enable()
    { m_enabled = TRUE; }

    /** returns true iff the indices are enabled */
    bool isEnabled() const
    { return m_enabled; }

    /** Add an index generator to the list, using a syntax similar to std::make_unique<T>() */
    template<class T,class... As>
    void addIndex(As&&... args)
    { m_indices.push_back(std::move(T{std::forward<As>(args)...})); }

    void initialize()
    { foreach<IndexIntf::initialize>(); }

    void finalize()
    { foreach<IndexIntf::finalize>(); }

    void incContentsDepth()
    { if (m_enabled) foreach_locked<IndexIntf::incContentsDepth>(); }

    void decContentsDepth()
    { if (m_enabled) foreach_locked<IndexIntf::decContentsDepth>(); }

    void addContentsItem(bool isDir, const QCString &name, const QCString &ref,
                         const QCString &file, const QCString &anchor,bool separateIndex=FALSE,bool addToNavIndex=FALSE,
                         const Definition *def=0)
    { if (m_enabled) foreach_locked<IndexIntf::addContentsItem>(isDir,name,ref,file,anchor,separateIndex,addToNavIndex,def); }

    void addIndexItem(const Definition *context,const MemberDef *md,const QCString &sectionAnchor=QCString(),const QCString &title=QCString())
    { if (m_enabled) foreach_locked<IndexIntf::addIndexItem>(context,md,sectionAnchor,title); }

    void addIndexFile(const QCString &name)
    { if (m_enabled) foreach_locked<IndexIntf::addIndexFile>(name); }

    void addImageFile(const QCString &name)
    { if (m_enabled) foreach_locked<IndexIntf::addImageFile>(name); }

    void addStyleSheetFile(const QCString &name)
    { if (m_enabled) foreach_locked<IndexIntf::addStyleSheetFile>(name); }

  private:
    bool m_enabled = true;
    std::mutex m_mutex;
    std::vector<IndexVariant> m_indices;
};

#endif // INDEXLIST_H
