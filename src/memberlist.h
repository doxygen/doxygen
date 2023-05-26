/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#ifndef MEMBERLIST_H
#define MEMBERLIST_H

#include <vector>
#include <algorithm>

#include "memberdef.h"
#include "linkedmap.h"
#include "types.h"
#include "membergroup.h"

class GroupDef;

int genericCompareMembers(const MemberDef *c1,const MemberDef *c2);

/** A vector of MemberDef object */
class MemberVector
{
  public:
    using T                = MemberDef*;
    using Vec              = std::vector<T>;
    using value_type       = Vec::value_type;
    using allocator_type   = Vec::allocator_type;
    using size_type        = Vec::size_type;
    using difference_type  = Vec::difference_type;
    using reference        = Vec::reference;
    using const_reference  = Vec::const_reference;
    using iterator         = Vec::iterator;
    using const_iterator   = Vec::const_iterator;

    void push_back( const T& value )        { m_members.push_back(value); }
    void push_back( T&& value )             { m_members.push_back(std::move(value)); }

    iterator erase( iterator pos )          { return m_members.erase(pos); }
    iterator erase( const_iterator pos )    { return m_members.erase(pos); }

          iterator begin()       noexcept   { return m_members.begin(); }
    const_iterator begin() const noexcept   { return m_members.begin(); }
          iterator end()         noexcept   { return m_members.end();   }
    const_iterator end()   const noexcept   { return m_members.end();   }

    size_type size() const noexcept         { return m_members.size();  }
    bool empty()     const noexcept         { return m_members.empty(); }

          reference front()                 { return m_members.front(); }
    const_reference front() const           { return m_members.front(); }

          reference back()                  { return m_members.back();  }
    const_reference back() const            { return m_members.back();  }

          reference operator[]( size_type pos )       { return m_members.operator[](pos); }
    const_reference operator[]( size_type pos ) const { return m_members.operator[](pos); }


    static bool lessThan(const MemberDef *md1,const MemberDef *md2)
    {
      return genericCompareMembers(md1,md2)<0;
    }
    void sort()
    {
      std::sort(m_members.begin(),m_members.end(),lessThan);
    }
    void inSort(MemberDef *md)
    {
      m_members.insert( std::upper_bound( m_members.begin(), m_members.end(), md, lessThan), md);
    }
    void remove(const MemberDef *md)
    {
      auto it = std::find(m_members.begin(),m_members.end(),md);
      if (it!=m_members.end()) erase(it);
    }
    bool contains(const MemberDef *md) const
    {
      return std::find(m_members.begin(),m_members.end(),md)!=m_members.end();
    }
    MemberDef *find(const QCString &name)
    {
      auto it = std::find_if(m_members.begin(),m_members.end(),[name=name](auto &el) { return el->name()==name; });
      if (it != m_members.end())
      {
        return *it;
      }

      return nullptr;
    }
  protected:
    Vec m_members;
};

/** A list of MemberDef objects as shown in documentation sections. */
class MemberList : public MemberVector
{
  public:
    MemberList(MemberListType lt,MemberListContainer container);
   ~MemberList();
    MemberListType listType() const { return m_listType; }
    static QCString listTypeAsString(MemberListType type);
    MemberListContainer container() const { return m_container; }

    int numDecMembers() const    { ASSERT(m_numDecMembers!=-1); return m_numDecMembers; }
    int numDecEnumValues() const { return m_numDecEnumValues; }
    int numDocMembers() const    { ASSERT(m_numDocMembers!=-1); return m_numDocMembers; }
    int numDocEnumValues() const { return m_numDocEnumValues; }
    bool needsSorting() const    { return m_needsSorting; }
    void countDecMembers();
    void countDocMembers();
    int countInheritableMembers(const ClassDef *inheritedFrom) const;
    void writePlainDeclarations(OutputList &ol,bool inGroup,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd, const GroupDef *gd,
               int indentLevel,const ClassDef *inheritedFrom,const QCString &inheritId) const;
    void writeDeclarations(OutputList &ol,
               const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,
               const QCString &title,const QCString &subtitle,
               bool showEnumValues=FALSE,bool showInline=FALSE,
               const ClassDef *inheritedFrom=0,MemberListType lt=MemberListType_pubMethods) const;
    void writeDocumentation(OutputList &ol,const QCString &scopeName,
               const Definition *container,const QCString &title,
               bool showEnumValues=FALSE,bool showInline=FALSE) const;
    void writeSimpleDocumentation(OutputList &ol,const Definition *container) const;
    void writeDocumentationPage(OutputList &ol,
               const QCString &scopeName, const DefinitionMutable *container, int hierarchyLevel=0) const;
    void writeTagFile(TextStream &,bool useQualifiedName=false);
    bool declVisible() const;
    void addMemberGroup(MemberGroup *mg);
    void addListReferences(Definition *def);
    void findSectionsInDocumentation(const Definition *d);
    void setNeedsSorting(bool b);
    const MemberGroupRefList &getMemberGroupList() const { return m_memberGroupRefList; }
    void setAnonymousEnumType();
    void setAnchors();

  private:
    int countEnumValues(const MemberDef *md) const;
    int m_numDecMembers; // number of members in the brief part of the memberlist
    int m_numDecEnumValues;
    int m_numDocMembers; // number of members in the detailed part of the memberlist
    int m_numDocEnumValues;
    MemberGroupRefList m_memberGroupRefList;
    MemberListContainer m_container;
    MemberListType m_listType;
    bool m_needsSorting;
};

class MemberLinkedRefMap : public LinkedRefMap<const MemberDef>
{
};

class MemberLists : public std::vector< std::unique_ptr<MemberList> >
{
  public:
    MemberLists() = default;
    const std::unique_ptr<MemberList> &get(MemberListType lt,MemberListContainer con)
    {
      // find the list with the given type
      auto it = std::find_if(begin(),end(),[&lt](const auto &ml) { return ml->listType()==lt; });
      if (it!=end()) return *it;
      // or create a new list if it is not found
      emplace_back(std::make_unique<MemberList>(lt,con));
      return back();
    }

  private:
    MemberLists(const MemberLists &) = delete;
    MemberLists &operator=(const MemberLists &) = delete;
};


#endif
