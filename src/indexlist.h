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

#include "qcstring.h"
#include "construct.h"

class Definition;
class MemberDef;
class OutputList;

/** Abstract interface for index generators. */
class IndexIntf
{
  public:
    ABSTRACT_BASE_CLASS(IndexIntf)

    virtual void initialize() = 0;
    virtual void finalize() = 0;
    virtual void incContentsDepth() = 0;
    virtual void decContentsDepth() = 0;
    virtual void addContentsItem(bool isDir,
                         const QCString &name,
                         const QCString &ref,
                         const QCString &file,
                         const QCString &anchor,
                         bool separateIndex,
                         bool addToNavIndex,
                         const Definition *def,
                         const QCString &nameAsHtml
                        ) = 0;
    virtual void addIndexItem(const Definition *context,const MemberDef *md,
                      const QCString &sectionAnchor,const QCString &title) = 0;
    virtual void addIndexFile(const QCString &name) = 0;
    virtual void addImageFile(const QCString &) = 0;
    virtual void addStyleSheetFile(const QCString &) = 0;
};

/** \brief A list of index interfaces.
 *
 *  This class itself implements all methods of IndexIntf and
 *  just forwards the calls to all items in the list (composite design pattern).
 */
class IndexList
{
    using IndexPtr = std::unique_ptr<IndexIntf>;

    template<class... Ts,class... As>
    void foreach(void (IndexIntf::*methodPtr)(Ts...),As&&... args)
    {
      for (const auto &intf : m_indices)
      {
        (intf.get()->*methodPtr)(std::forward<As>(args)...);
      }
    }

    template<class... Ts,class... As>
    void foreach_locked(void (IndexIntf::*methodPtr)(Ts...),As&&... args)
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (const auto &intf : m_indices)
      {
        (intf.get()->*methodPtr)(std::forward<As>(args)...);
      }
    }

  public:
    /** disable the indices */
    void disable()
    { m_enabled = FALSE; }

    /** enable the indices */
    void enable()
    { m_enabled = TRUE; }

    /** Add an index generator to the list, using a syntax similar to std::make_unique<T>() */
    template<class T,class... As>
    void addIndex(As&&... args)
    { m_indices.push_back(std::make_unique<T>(std::forward<As>(args)...)); }

    void initialize()
    { foreach(&IndexIntf::initialize); }

    void finalize()
    { foreach(&IndexIntf::finalize); }

    void incContentsDepth()
    { if (m_enabled) foreach_locked(&IndexIntf::incContentsDepth); }

    void decContentsDepth()
    { if (m_enabled) foreach_locked(&IndexIntf::decContentsDepth); }

    void addContentsItem(bool isDir, const QCString &name, const QCString &ref,
                         const QCString &file, const QCString &anchor,bool separateIndex=FALSE,bool addToNavIndex=FALSE,
                         const Definition *def=nullptr, const QCString &nameAsHtml = QCString())
    { if (m_enabled) foreach_locked(&IndexIntf::addContentsItem,isDir,name,ref,file,anchor,separateIndex,addToNavIndex,def,nameAsHtml); }

    void addIndexItem(const Definition *context,const MemberDef *md,const QCString &sectionAnchor=QCString(),const QCString &title=QCString())
    { if (m_enabled) foreach_locked(&IndexIntf::addIndexItem,context,md,sectionAnchor,title); }

    void addIndexFile(const QCString &name)
    { if (m_enabled) foreach_locked(&IndexIntf::addIndexFile,name); }

    void addImageFile(const QCString &name)
    { if (m_enabled) foreach_locked(&IndexIntf::addImageFile,name); }

    void addStyleSheetFile(const QCString &name)
    { if (m_enabled) foreach_locked(&IndexIntf::addStyleSheetFile,name); }

  private:
    bool m_enabled = true;
    std::mutex m_mutex;
    std::vector<IndexPtr> m_indices;
};

#endif // INDEXLIST_H
