/******************************************************************************
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

#include "template.h"

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <deque>
#include <cstdio>

#include <qfile.h>
#include <qdir.h>

#include "ftextstream.h"
#include "message.h"
#include "util.h"
#include "resourcemgr.h"
#include "portable.h"
#include "regex.h"

#define ENABLE_TRACING 0

#if ENABLE_TRACING
#define TRACE(x) printf x
#else
#define TRACE(x)
#endif

class TemplateToken;

//-------------------------------------------------------------------

static std::vector<QCString> split(const QCString &str,const QCString &sep,
                                  bool allowEmptyEntries=FALSE,bool cleanup=TRUE)
{
  std::vector<QCString> lst;

  int j = 0;
  int i = str.find( sep, j );

  while (i!=-1)
  {
    if ( str.mid(j,i-j).length() > 0 )
    {
      if (cleanup)
      {
        lst.push_back(str.mid(j,i-j).stripWhiteSpace());
      }
      else
      {
        lst.push_back(str.mid(j,i-j));
      }
    }
    else if (allowEmptyEntries)
    {
      lst.push_back("");
    }
    j = i + sep.length();
    i = str.find(sep,j);
  }

  int l = str.length() - 1;
  if (str.mid(j,l-j+1).length()>0)
  {
    if (cleanup)
    {
      lst.push_back(str.mid(j,l-j+1).stripWhiteSpace());
    }
    else
    {
      lst.push_back(str.mid(j,l-j+1));
    }
  }
  else if (allowEmptyEntries)
  {
    lst.push_back("");
  }

  return lst;
}

//----------------------------------------------------------------------------

/** Strips spaces surrounding `=` from string \a in, so
 *  `foo = 10 bar=5 baz= 'hello'` will become `foo=10 bar=5 baz='hello'`
 */
static QCString removeSpacesAroundEquals(const char *s)
{
  QCString result(s);
  const char *p=result.data();
  char *q = result.rawData();
  char c;
  while ((c=*p++))
  {
    if (c==' ') // found a space, see if there is a = as well
    {
      const char *t = p;
      bool found=FALSE;
      while (*t==' ' || *t=='=') { if (*t++=='=') found=TRUE; }
      if (found)
      {
        c='=';
        p=t; // move p to end of '\s*=\s*' sequence
      }
    }
    *q++=c;
  }
  if (q<p) result.resize(q-result.data()+1);
  return result;
}

//----------------------------------------------------------------------------

#if ENABLE_TRACING
static QCString replace(const char *s,char csrc,char cdst)
{
  QCString result = s;
  for (char *p=result.data();*p;p++)
  {
    if (*p==csrc) *p=cdst;
  }
  return result;
}
#endif

//- TemplateVariant implementation -------------------------------------------


TemplateVariant::TemplateVariant(TemplateStructIntf *s)
  : m_type(Struct), m_strukt(s), m_raw(FALSE)
{
  m_strukt->addRef();
}

TemplateVariant::TemplateVariant(TemplateListIntf *l)
  : m_type(List), m_list(l), m_raw(FALSE)
{
  m_list->addRef();
}

TemplateVariant::~TemplateVariant()
{
  if      (m_type==Struct) m_strukt->release();
  else if (m_type==List)   m_list->release();
}

TemplateVariant::TemplateVariant(const TemplateVariant &v)
  : m_type(v.m_type), m_strukt(0), m_raw(FALSE)
{
  m_raw = v.m_raw;
  switch (m_type)
  {
    case None: break;
    case Bool:     m_boolVal = v.m_boolVal; break;
    case Integer:  m_intVal  = v.m_intVal;  break;
    case String:   m_strVal  = v.m_strVal;  break;
    case Struct:   m_strukt  = v.m_strukt;  m_strukt->addRef(); break;
    case List:     m_list    = v.m_list;    m_list->addRef();   break;
    case Function: m_delegate= v.m_delegate;break;
  }
}

TemplateVariant &TemplateVariant::operator=(const TemplateVariant &v)
{
  // assignment can change the type of the variable, so we have to be
  // careful with reference counted content.
  TemplateStructIntf *tmpStruct = m_type==Struct ? m_strukt : 0;
  TemplateListIntf   *tmpList   = m_type==List   ? m_list   : 0;
  Type tmpType = m_type;

  m_type    = v.m_type;
  m_raw     = v.m_raw;
  switch (m_type)
  {
    case None: break;
    case Bool:     m_boolVal = v.m_boolVal; break;
    case Integer:  m_intVal  = v.m_intVal;  break;
    case String:   m_strVal  = v.m_strVal;  break;
    case Struct:   m_strukt  = v.m_strukt;  m_strukt->addRef(); break;
    case List:     m_list    = v.m_list;    m_list->addRef();   break;
    case Function: m_delegate= v.m_delegate;break;
  }

  // release overwritten reference counted values
  if      (tmpType==Struct && tmpStruct) tmpStruct->release();
  else if (tmpType==List   && tmpList  ) tmpList->release();
  return *this;
}

bool TemplateVariant::toBool() const
{
  switch (m_type)
  {
    case None:     return FALSE;
    case Bool:     return m_boolVal;
    case Integer:  return m_intVal!=0;
    case String:   return !m_strVal.isEmpty();
    case Struct:   return TRUE;
    case List:     return m_list->count()!=0;
    case Function: return FALSE;
  }
  return FALSE;
}

int TemplateVariant::toInt() const
{
  switch (m_type)
  {
    case None:     return 0;
    case Bool:     return m_boolVal ? 1 : 0;
    case Integer:  return m_intVal;
    case String:   return m_strVal.toInt();
    case Struct:   return 0;
    case List:     return m_list->count();
    case Function: return 0;
  }
  return 0;
}

//- Template struct implementation --------------------------------------------


/** @brief Private data of a template struct object */
class TemplateStruct::Private
{
  public:
    Private() : refCount(0) {}
    std::unordered_map<std::string,TemplateVariant> fields;
    int refCount = 0;
};

TemplateStruct::TemplateStruct()
{
  p = new Private;
}

TemplateStruct::~TemplateStruct()
{
  delete p;
}

int TemplateStruct::addRef()
{
  return ++p->refCount;
}

int TemplateStruct::release()
{
  int count = --p->refCount;
  if (count<=0)
  {
    delete this;
  }
  return count;
}

void TemplateStruct::set(const char *name,const TemplateVariant &v)
{
  auto it = p->fields.find(name);
  if (it!=p->fields.end()) // change existing field
  {
    it->second = v;
  }
  else // insert new field
  {
    p->fields.insert(std::make_pair(name,v));
  }
}

TemplateVariant TemplateStruct::get(const char *name) const
{
  auto it = p->fields.find(name);
  return it!=p->fields.end() ? it->second : TemplateVariant();
}

TemplateStruct *TemplateStruct::alloc()
{
  return new TemplateStruct;
}

//- Template list implementation ----------------------------------------------


/** @brief Private data of a template list object */
class TemplateList::Private
{
  public:
    Private() : index(-1), refCount(0) {}
    std::vector<TemplateVariant> elems;
    int index = -1;
    int refCount = 0;
};


TemplateList::TemplateList()
{
  p = new Private;
}

TemplateList::~TemplateList()
{
  delete p;
}

int TemplateList::addRef()
{
  return ++p->refCount;
}

int TemplateList::release()
{
  int count = --p->refCount;
  if (count<=0)
  {
    delete this;
  }
  return count;
}

uint TemplateList::count() const
{
  return static_cast<uint>(p->elems.size());
}

void TemplateList::append(const TemplateVariant &v)
{
  p->elems.push_back(v);
}

// iterator support
class TemplateListConstIterator : public TemplateListIntf::ConstIterator
{
  public:
    TemplateListConstIterator(const TemplateList &l) : m_list(l) { m_index=0; }
    virtual ~TemplateListConstIterator() {}
    virtual void toFirst()
    {
      m_index=0;
    }
    virtual void toLast()
    {
      if (m_list.p->elems.size()>0)
      {
        m_index=m_list.p->elems.size()-1;
      }
      else
      {
        m_index=0;
      }
    }
    virtual void toNext()
    {
      if (m_index<m_list.p->elems.size())
      {
        m_index++;
      }
    }
    virtual void toPrev()
    {
      if (m_index>0)
      {
        --m_index;
      }
    }
    virtual bool current(TemplateVariant &v) const
    {
      if (m_index<m_list.p->elems.size())
      {
        v = m_list.p->elems[m_index];
        return TRUE;
      }
      else
      {
        v = TemplateVariant();
        return FALSE;
      }
    }
  private:
    const TemplateList &m_list;
    size_t m_index = 0;
};

TemplateListIntf::ConstIterator *TemplateList::createIterator() const
{
  return new TemplateListConstIterator(*this);
}

TemplateVariant TemplateList::at(uint index) const
{
  if (index<p->elems.size())
  {
    return p->elems[index];
  }
  else
  {
    return TemplateVariant();
  }
}

TemplateList *TemplateList::alloc()
{
  return new TemplateList;
}

//- Operator types ------------------------------------------------------------

/** @brief Class representing operators that can appear in template expressions */
class Operator
{
  public:
      /* Operator precedence (low to high)
         or
         and
         not
         in
         ==, !=, <, >, <=, >=
         +, -
         *, /, %
         |
         :
         ,
       */
    enum Type
    {
      Or, And, Not, In, Equal, NotEqual, Less, Greater, LessEqual,
      GreaterEqual, Plus, Minus, Multiply, Divide, Modulo, Filter, Colon, Comma,
      LeftParen, RightParen,
      Last
    };

    static const char *toString(Type op)
    {
      switch(op)
      {
        case Or:           return "or";
        case And:          return "and";
        case Not:          return "not";
        case In:           return "in";
        case Equal:        return "==";
        case NotEqual:     return "!=";
        case Less:         return "<";
        case Greater:      return ">";
        case LessEqual:    return "<=";
        case GreaterEqual: return ">=";
        case Plus:         return "+";
        case Minus:        return "-";
        case Multiply:     return "*";
        case Divide:       return "/";
        case Modulo:       return "%";
        case Filter:       return "|";
        case Colon:        return ":";
        case Comma:        return ",";
        case LeftParen:    return "(";
        case RightParen:   return ")";
        case Last:         return "?";
      }
      return "?";
    }
};

//-----------------------------------------------------------------------------

class TemplateNodeBlock;

/** @brief Class holding stacks of blocks available in the context */
class TemplateBlockContext
{
  public:
    TemplateBlockContext();
    TemplateNodeBlock *get(const QCString &name) const;
    TemplateNodeBlock *pop(const QCString &name);
    void add(TemplateNodeBlock *block);
    void add(TemplateBlockContext *ctx);
    void push(TemplateNodeBlock *block);
    void clear();
    using NodeBlockList = std::deque<TemplateNodeBlock*>;
  private:
    std::map< std::string, NodeBlockList > m_blocks;
};

/** @brief A container to store a key-value pair */
struct TemplateKeyValue
{
  TemplateKeyValue() {}
  TemplateKeyValue(const QCString &k,const TemplateVariant &v) : key(k), value(v) {}
  QCString key;
  TemplateVariant value;
};

/** @brief Internal class representing the implementation of a template
 *  context */
class TemplateContextImpl : public TemplateContext
{
  public:
    TemplateContextImpl(const TemplateEngine *e);
    virtual ~TemplateContextImpl();

    // TemplateContext methods
    void push();
    void pop();
    void set(const char *name,const TemplateVariant &v);
    TemplateVariant get(const QCString &name) const;
    const TemplateVariant *getRef(const QCString &name) const;
    void setOutputDirectory(const QCString &dir)
    { m_outputDir = dir; }
    void setEscapeIntf(const QCString &ext,TemplateEscapeIntf *intf)
    {
      int i=(!ext.isEmpty() && ext.at(0)=='.') ? 1 : 0;
      m_escapeIntfMap.insert(std::make_pair(ext.mid(i).str(),intf));
    }
    void selectEscapeIntf(const QCString &ext)
    {
      auto it = m_escapeIntfMap.find(ext.str());
      m_activeEscapeIntf = it!=m_escapeIntfMap.end() ? it->second : 0;
    }
    void setActiveEscapeIntf(TemplateEscapeIntf *intf) { m_activeEscapeIntf = intf; }
    void setSpacelessIntf(TemplateSpacelessIntf *intf) { m_spacelessIntf = intf; }

    // internal methods
    TemplateBlockContext *blockContext();
    TemplateVariant getPrimary(const QCString &name) const;
    void setLocation(const QCString &templateName,int line)
    { m_templateName=templateName; m_line=line; }
    QCString templateName() const                { return m_templateName; }
    int line() const                             { return m_line; }
    QCString outputDirectory() const             { return m_outputDir; }
    TemplateEscapeIntf *escapeIntf() const       { return m_activeEscapeIntf; }
    TemplateSpacelessIntf *spacelessIntf() const { return m_spacelessIntf; }
    void enableSpaceless(bool b)                 { if (b && !m_spacelessEnabled) m_spacelessIntf->reset();
                                                   m_spacelessEnabled=b;
                                                 }
    bool spacelessEnabled() const                { return m_spacelessEnabled && m_spacelessIntf; }
    void enableTabbing(bool b)                   { m_tabbingEnabled=b;
                                                   if (m_activeEscapeIntf) m_activeEscapeIntf->enableTabbing(b);
                                                 }
    bool tabbingEnabled() const                  { return m_tabbingEnabled; }
    bool needsRecoding() const                   { return !m_encoding.isEmpty(); }
    QCString encoding() const                    { return m_encoding; }
    void setEncoding(const QCString &file,int line,const QCString &enc);
    QCString recode(const QCString &s);
    void warn(const char *fileName,int line,const char *fmt,...) const;

    // index related functions
    void openSubIndex(const QCString &indexName);
    void closeSubIndex(const QCString &indexName);
    void addIndexEntry(const QCString &indexName,const std::vector<TemplateKeyValue> &arguments);

  private:
    const TemplateEngine *m_engine = 0;
    QCString m_templateName;
    int m_line = 0;
    QCString m_outputDir;
    std::deque< std::map<std::string,TemplateVariant> > m_contextStack;
    TemplateBlockContext m_blockContext;
    std::unordered_map<std::string, TemplateEscapeIntf*> m_escapeIntfMap;
    TemplateEscapeIntf *m_activeEscapeIntf = 0;
    TemplateSpacelessIntf *m_spacelessIntf = 0;
    bool m_spacelessEnabled = false;
    bool m_tabbingEnabled = false;
    TemplateAutoRef<TemplateStruct> m_indices;
    std::unordered_map< std::string, std::stack<TemplateVariant> > m_indexStacks;
    QCString m_encoding;
    void *m_fromUtf8 = 0;
};

//-----------------------------------------------------------------------------

/** @brief The implementation of the "add" filter */
class FilterAdd
{
  public:
    static int variantIntValue(const TemplateVariant &v,bool &isInt)
    {
      isInt = v.type()==TemplateVariant::Integer;
      if (!isInt && v.type()==TemplateVariant::String)
      {
        return v.toString().toInt(&isInt);
      }
      return isInt ? v.toInt() : 0;
    }
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &arg)
    {
      if (!v.isValid())
      {
        return arg;
      }
      bool lhsIsInt;
      int  lhsValue = variantIntValue(v,lhsIsInt);
      bool rhsIsInt;
      int  rhsValue = variantIntValue(arg,rhsIsInt);
      if (lhsIsInt && rhsIsInt)
      {
        return lhsValue+rhsValue;
      }
      else if (v.type()==TemplateVariant::String && arg.type()==TemplateVariant::String)
      {
        return TemplateVariant(v.toString() + arg.toString());
      }
      else
      {
        return v;
      }
    }
};

//-----------------------------------------------------------------------------

/** @brief The implementation of the "get" filter */
class FilterGet
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &arg)
    {
      if (v.isValid() && v.type()==TemplateVariant::Struct && arg.type()==TemplateVariant::String)
      {
        TemplateVariant result = v.toStruct()->get(arg.toString());
        //printf("\nok[%s]=%d\n",arg.toString().data(),result.type());
        return result;
      }
      else
      {
        //printf("\nnok[%s]\n",arg.toString().data());
        return FALSE;
      }
    }
};

//-----------------------------------------------------------------------------

/** @brief The implementation of the "raw" filter */
class FilterRaw
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (v.isValid() && (v.type()==TemplateVariant::String || v.type()==TemplateVariant::Integer))
      {
        return TemplateVariant(v.toString(),TRUE);
      }
      else
      {
        return v;
      }
    }
};

//-----------------------------------------------------------------------------

/** @brief The implementation of the "list" filter */
class FilterList
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (v.isValid())
      {
        if (v.type()==TemplateVariant::List) // input is already a list
        {
          return v;
        }
        // create a list with v as the only element
        TemplateList *list = TemplateList::alloc();
        list->append(v);
        return list;
      }
      else
      {
        return v;
      }
    }
};

//-----------------------------------------------------------------------------
/** @brief The implementation of the "texlabel" filter */
class FilterTexLabel
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (v.isValid() && (v.type()==TemplateVariant::String))
      {
        return TemplateVariant(latexEscapeLabelName(v.toString()),TRUE);
      }
      else
      {
        return v;
      }
    }
};

//-----------------------------------------------------------------------------

/** @brief The implementation of the "texindex" filter */
class FilterTexIndex
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (v.isValid() && (v.type()==TemplateVariant::String))
      {
        return TemplateVariant(latexEscapeIndexChars(v.toString()),TRUE);
      }
      else
      {
        return v;
      }
    }
};

//-----------------------------------------------------------------------------

/** @brief The implementation of the "append" filter */
class FilterAppend
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &arg)
    {
      if ((v.type()==TemplateVariant::String || v.type()==TemplateVariant::Integer) &&
          (arg.type()==TemplateVariant::String || arg.type()==TemplateVariant::Integer))
      {
        return TemplateVariant(v.toString() + arg.toString());
      }
      else
      {
        return v;
      }
    }
};

//-----------------------------------------------------------------------------

/** @brief The implementation of the "prepend" filter */
class FilterPrepend
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &arg)
    {
      if ((v.type()==TemplateVariant::String || v.type()==TemplateVariant::Integer) &&
          arg.type()==TemplateVariant::String)
      {
        return TemplateVariant(arg.toString() + v.toString());
      }
      else
      {
        return v;
      }
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "length" filter */
class FilterLength
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (!v.isValid())
      {
        return TemplateVariant();
      }
      if (v.type()==TemplateVariant::List)
      {
        return TemplateVariant((int)v.toList()->count());
      }
      else if (v.type()==TemplateVariant::String)
      {
        return TemplateVariant((int)v.toString().length());
      }
      else
      {
        return TemplateVariant();
      }
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "default" filter */
class FilterDefault
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &arg)
    {
      if (!v.isValid())
      {
        return arg;
      }
      else if (v.type()==TemplateVariant::String && v.toString().isEmpty())
      {
        return arg;
      }
      else
      {
        return v;
      }
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "flatten" filter */
class FilterFlatten
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (!v.isValid() || v.type()!=TemplateVariant::List)
      {
        return v;
      }
      else
      {
        TemplateList *list = TemplateList::alloc();
        flatten(v.toList(),list);
        return TemplateVariant(list);
      }
    }

  private:
    static void flatten(TemplateListIntf *tree,TemplateList *list)
    {
      TemplateListIntf::ConstIterator *it = tree->createIterator();
      TemplateVariant item;
      for (it->toFirst();(it->current(item));it->toNext())
      {
        TemplateStructIntf *s = item.toStruct();
        if (s)
        {
          list->append(item);
          // if s has "children" then recurse into the children
          TemplateVariant children = s->get("children");
          if (children.isValid() && children.type()==TemplateVariant::List)
          {
            flatten(children.toList(),list);
          }
        }
        else
        {
          list->append(item);
        }
      }
      delete it;
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "listsort" filter */
class FilterListSort
{
    struct ListElem
    {
      ListElem(const QCString &k,const TemplateVariant &v) : key(k), value(v) {}
      QCString key;
      TemplateVariant value;
    };
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &args)
    {
      if (v.type()==TemplateVariant::List && args.type()==TemplateVariant::String)
      {
        //printf("FilterListSort::apply: v=%s args=%s\n",v.toString().data(),args.toString().data());
        TemplateListIntf::ConstIterator *it = v.toList()->createIterator();

        TemplateVariant item;
        TemplateList *result = TemplateList::alloc();

        // create list of items based on v using the data in args as a sort key
        using SortList = std::vector<ListElem>;
        SortList sortList;
        sortList.reserve(v.toList()->count());
        for (it->toFirst();(it->current(item));it->toNext())
        {
          TemplateStructIntf *s = item.toStruct();
          if (s)
          {
            QCString sortKey = determineSortKey(s,args.toString());
            sortList.emplace_back(sortKey,item);
            //printf("sortKey=%s\n",sortKey.data());
          }
        }
        delete it;

        // sort the list
        std::sort(sortList.begin(),
                  sortList.end(),
                  [](const auto &lhs,const auto &rhs) { return qstrcmp(lhs.key,rhs.key)<0; });

        // add sorted items to the result list
        for (const auto &elem : sortList)
        {
          result->append(elem.value);
        }
        return result;
      }
      return v;
    }

  private:
    static QCString determineSortKey(TemplateStructIntf *s,const QCString &arg)
    {
      int i,p=0;
      QCString result;
      while ((i=arg.find("{{",p))!=-1)
      {
        result+=arg.mid(p,i-p);
        int j=arg.find("}}",i+2);
        if (j!=-1)
        {
          QCString var = arg.mid(i+2,j-i-2);
          TemplateVariant val=s->get(var);
          //printf("found argument %s value=%s\n",var.data(),val.toString().data());
          result+=val.toString();
          p=j+2;
        }
        else
        {
          p=i+1;
        }
      }
      result+=arg.right(arg.length()-p);
      return result;
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "groupBy" filter */
class FilterGroupBy
{
    struct ListElem
    {
      ListElem(const QCString &k,const TemplateVariant &v) : key(k), value(v) {}
      QCString key;
      TemplateVariant value;
    };
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &args)
    {
      if (v.type()==TemplateVariant::List && args.type()==TemplateVariant::String)
      {
        //printf("FilterListSort::apply: v=%s args=%s\n",v.toString().data(),args.toString().data());
        TemplateListIntf::ConstIterator *it = v.toList()->createIterator();

        TemplateVariant item;
        TemplateList *result = TemplateList::alloc();

        // create list of items based on v using the data in args as a sort key
        using SortList = std::vector<ListElem>;
        SortList sortList;
        sortList.reserve(v.toList()->count());
        for (it->toFirst();(it->current(item));it->toNext())
        {
          TemplateStructIntf *s = item.toStruct();
          if (s)
          {
            QCString sortKey = determineSortKey(s,args.toString());
            sortList.emplace_back(sortKey,item);
            //printf("sortKey=%s\n",sortKey.data());
          }
        }
        delete it;

        // sort the list
        std::sort(sortList.begin(),
                  sortList.end(),
                  [](const auto &lhs,const auto &rhs) { return qstrcmp(lhs.key,rhs.key)<0; });

        // add sorted items to the result list
        TemplateList *groupList=0;
        QCString prevKey;
        for (const auto &elem : sortList)
        {
          if (groupList==0 || elem.key!=prevKey)
          {
            groupList = TemplateList::alloc();
            result->append(groupList);
            prevKey = elem.key;
          }
          groupList->append(elem.value);
        }
        return result;
      }
      return v;
    }

  private:
    static QCString determineSortKey(TemplateStructIntf *s,const QCString &attribName)
    {
       TemplateVariant v = s->get(attribName);
       return v.toString();
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "relative" filter */
class FilterRelative
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (v.isValid() && v.type()==TemplateVariant::String && v.toString().left(2)=="..")
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "paginate" filter */
class FilterPaginate
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &args)
    {
      if (v.isValid() && v.type()==TemplateVariant::List &&
          args.isValid() && args.type()==TemplateVariant::Integer)
      {
        int pageSize = args.toInt();
        TemplateListIntf *list   = v.toList();
        TemplateList     *result = TemplateList::alloc();
        TemplateListIntf::ConstIterator *it = list->createIterator();
        TemplateVariant   item;
        TemplateList     *pageList=0;
        int i = 0;
        for (it->toFirst();(it->current(item));it->toNext())
        {
          if (pageList==0)
          {
            pageList = TemplateList::alloc();
            result->append(pageList);
          }
          pageList->append(item);
          i++;
          if (i==pageSize) // page is full start a new one
          {
            pageList=0;
            i=0;
          }
        }
        delete it;
        return result;
      }
      else // wrong arguments
      {
        return v;
      }
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "alphaIndex" filter */
class FilterAlphaIndex
{
  private:
    struct ListElem
    {
      ListElem(uint k,const TemplateVariant &v) : key(k), value(v) {}
      QCString key;
      TemplateVariant value;
    };
    static QCString keyToLabel(const char *startLetter)
    {
      const char *p = startLetter;
      if (startLetter==0 || *startLetter==0) return "";
      char c = *p;
      QCString result;
      if (c<127 && c>31) // printable ASCII character
      {
        result+=c;
      }
      else
      {
        result="0x";
        const char hex[]="0123456789abcdef";
        while ((c=*p++))
        {
          result+=hex[((unsigned char)c)>>4];
          result+=hex[((unsigned char)c)&0xf];
        }
      }
      return result;
    }
    static uint determineSortKey(TemplateStructIntf *s,const QCString &attribName)
    {
       TemplateVariant v = s->get(attribName);
       int index = getPrefixIndex(v.toString());
       return getUtf8CodeToUpper(v.toString(),index);
    }

  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &args)
    {
      if (v.type()==TemplateVariant::List && args.type()==TemplateVariant::String)
      {
        //printf("FilterListSort::apply: v=%s args=%s\n",v.toString().data(),args.toString().data());
        TemplateListIntf::ConstIterator *it = v.toList()->createIterator();

        TemplateVariant item;
        TemplateList *result = TemplateList::alloc();

        // create list of items based on v using the data in args as a sort key
        using SortList = std::vector<ListElem>;
        SortList sortList;
        sortList.reserve(v.toList()->count());
        for (it->toFirst();(it->current(item));it->toNext())
        {
          TemplateStructIntf *s = item.toStruct();
          if (s)
          {
            uint sortKey = determineSortKey(s,args.toString());
            sortList.emplace_back(sortKey,item);
            //printf("sortKey=%s\n",sortKey.data());
          }
        }
        delete it;

        // sort the list
        std::sort(sortList.begin(),
                  sortList.end(),
                  [](const auto &lhs,const auto &rhs) { return qstrcmp(lhs.key,rhs.key)<0; });

        // create an index from the sorted list
        QCString letter;
        TemplateStruct *indexNode = 0;
        TemplateList *indexList = 0;
        for (const auto &elem : sortList)
        {
          if (letter!=elem.key || indexNode==0)
          {
            // create new indexNode
            indexNode = TemplateStruct::alloc();
            indexList = TemplateList::alloc();
            indexNode->set("letter", elem.key);
            indexNode->set("label",  keyToLabel(elem.key));
            indexNode->set("items",indexList);
            result->append(indexNode);
            letter=elem.key;
          }
          indexList->append(elem.value);
        }
        return result;
      }
      return v;
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "default" filter */
class FilterStripPath
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (!v.isValid() || v.type()!=TemplateVariant::String)
      {
        return v;
      }
      QCString result = v.toString();
      int i=result.findRev('/');
      if (i!=-1)
      {
        result=result.mid(i+1);
      }
      i=result.findRev('\\');
      if (i!=-1)
      {
        result=result.mid(i+1);
      }
      return result;
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "default" filter */
class FilterNoWrap
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (!v.isValid() || v.type()!=TemplateVariant::String)
      {
        return v;
      }
      QCString s = v.toString();
      return substitute(s," ","&#160;");
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "divisibleby" filter */
class FilterDivisibleBy
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &n)
    {
      if (!v.isValid() || !n.isValid())
      {
        return TemplateVariant();
      }
      if (v.type()==TemplateVariant::Integer && n.type()==TemplateVariant::Integer)
      {
        int ni = n.toInt();
        if (ni>0)
        {
          return TemplateVariant((v.toInt()%ni)==0);
        }
        else
        {
          return TemplateVariant(FALSE);
        }
      }
      else
      {
        return TemplateVariant();
      }
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "isRelativeURL" filter */
class FilterIsRelativeURL
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (v.isValid() && v.type()==TemplateVariant::String)
      {
        QCString s = v.toString();
        if (!s.isEmpty() && s.at(0)=='!') return TRUE;
      }
      return FALSE;
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "isRelativeURL" filter */
class FilterIsAbsoluteURL
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (v.isValid() && v.type()==TemplateVariant::String)
      {
        QCString s = v.toString();
        if (!s.isEmpty() && s.at(0)=='^') return TRUE;
      }
      return FALSE;
    }
};

//--------------------------------------------------------------------

/** @brief The implementation of the "decodeURL" filter
 *  The leading character is removed from the value in case it is a ^ or !.
 *  - ^ is used to encode a absolute URL
 *  - ! is used to encode a relative URL
 */
class FilterDecodeURL
{
  public:
    static TemplateVariant apply(const TemplateVariant &v,const TemplateVariant &)
    {
      if (v.isValid() && v.type()==TemplateVariant::String)
      {
        QCString s = v.toString();
        if (!s.isEmpty() && (s.at(0)=='^' || s.at(0)=='!'))
        {
          return s.mid(1);
        }
      }
      return v;
    }
};


//--------------------------------------------------------------------

/** @brief Factory singleton for registering and creating filters */
class TemplateFilterFactory
{
  public:
    typedef TemplateVariant (FilterFunction)(const TemplateVariant &v,const TemplateVariant &arg);

    static TemplateFilterFactory *instance()
    {
      static TemplateFilterFactory *instance = 0;
      if (instance==0) instance = new TemplateFilterFactory;
      return instance;
    }

    TemplateVariant apply(const QCString &name,const TemplateVariant &v,const TemplateVariant &arg, bool &ok)
    {
      auto it = m_registry.find(name.str());
      if (it!=m_registry.end())
      {
        ok=TRUE;
        return it->second(v,arg);
      }
      else
      {
        ok=FALSE;
        return v;
      }
    }

    void registerFilter(const QCString &name,FilterFunction *func)
    {
      m_registry.insert(std::make_pair(name.str(),func));
    }

    /** @brief Helper class for registering a filter function */
    template<class T> class AutoRegister
    {
      public:
        AutoRegister<T>(const QCString &key)
        {
          TemplateFilterFactory::instance()->registerFilter(key,&T::apply);
        }
    };

  private:
    std::unordered_map<std::string,FilterFunction*> m_registry;
};

// register a handlers for each filter we support
static TemplateFilterFactory::AutoRegister<FilterAdd>                fAdd("add");
static TemplateFilterFactory::AutoRegister<FilterGet>                fGet("get");
static TemplateFilterFactory::AutoRegister<FilterRaw>                fRaw("raw");
static TemplateFilterFactory::AutoRegister<FilterList>               fList("list");
static TemplateFilterFactory::AutoRegister<FilterAppend>             fAppend("append");
static TemplateFilterFactory::AutoRegister<FilterLength>             fLength("length");
static TemplateFilterFactory::AutoRegister<FilterNoWrap>             fNoWrap("nowrap");
static TemplateFilterFactory::AutoRegister<FilterFlatten>            fFlatten("flatten");
static TemplateFilterFactory::AutoRegister<FilterDefault>            fDefault("default");
static TemplateFilterFactory::AutoRegister<FilterPrepend>            fPrepend("prepend");
static TemplateFilterFactory::AutoRegister<FilterGroupBy>            fGroupBy("groupBy");
static TemplateFilterFactory::AutoRegister<FilterRelative>           fRelative("relative");
static TemplateFilterFactory::AutoRegister<FilterListSort>           fListSort("listsort");
static TemplateFilterFactory::AutoRegister<FilterTexLabel>           fTexLabel("texLabel");
static TemplateFilterFactory::AutoRegister<FilterTexIndex>           fTexIndex("texIndex");
static TemplateFilterFactory::AutoRegister<FilterPaginate>           fPaginate("paginate");
static TemplateFilterFactory::AutoRegister<FilterStripPath>          fStripPath("stripPath");
static TemplateFilterFactory::AutoRegister<FilterDecodeURL>          fDecodeURL("decodeURL");
static TemplateFilterFactory::AutoRegister<FilterAlphaIndex>         fAlphaIndex("alphaIndex");
static TemplateFilterFactory::AutoRegister<FilterDivisibleBy>        fDivisibleBy("divisibleby");
static TemplateFilterFactory::AutoRegister<FilterIsRelativeURL>      fIsRelativeURL("isRelativeURL");
static TemplateFilterFactory::AutoRegister<FilterIsAbsoluteURL>      fIsAbsoluteURL("isAbsoluteURL");

//--------------------------------------------------------------------

/** @brief Base class for all nodes in the abstract syntax tree of an
 *  expression.
 */
class ExprAst
{
  public:
    virtual ~ExprAst() {}
    virtual TemplateVariant resolve(TemplateContext *) { return TemplateVariant(); }
};

using ExprAstList = std::vector< std::unique_ptr<ExprAst> >;

/** @brief Class representing a number in the AST */
class ExprAstNumber : public ExprAst
{
  public:
    ExprAstNumber(int num) : m_number(num)
    { TRACE(("ExprAstNumber(%d)\n",num)); }
    int number() const { return m_number; }
    virtual TemplateVariant resolve(TemplateContext *) { return TemplateVariant(m_number); }
  private:
    int m_number = 0;
};

/** @brief Class representing a variable in the AST */
class ExprAstVariable : public ExprAst
{
  public:
    ExprAstVariable(const char *name) : m_name(name)
    { TRACE(("ExprAstVariable(%s)\n",name)); }
    const QCString &name() const { return m_name; }
    virtual TemplateVariant resolve(TemplateContext *c)
    {
      TemplateVariant v = c->get(m_name);
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (!v.isValid())
      {
        if (ci) ci->warn(ci->templateName(),ci->line(),"undefined variable '%s' in expression",m_name.data());
      }
      return v;
    }
  private:
    QCString m_name;
};

class ExprAstFunctionVariable : public ExprAst
{
  public:
    ExprAstFunctionVariable(ExprAst *var, ExprAstList &&args)
      : m_var(var), m_args(std::move(args))
    { TRACE(("ExprAstFunctionVariable()\n"));
    }
   ~ExprAstFunctionVariable()
    {
      delete m_var;
    }
    virtual TemplateVariant resolve(TemplateContext *c)
    {
      std::vector<TemplateVariant> args;
      for (const auto &exprArg : m_args)
      {
        TemplateVariant v = exprArg->resolve(c);
        args.push_back(v);
      }
      TemplateVariant v = m_var->resolve(c);
      if (v.type()==TemplateVariant::Function)
      {
        v = v.call(args);
      }
      return v;
    }
  private:
    ExprAst *m_var = 0;
    ExprAstList m_args;
};

/** @brief Class representing a filter in the AST */
class ExprAstFilter : public ExprAst
{
  public:
    ExprAstFilter(const char *name,ExprAst *arg) : m_name(name), m_arg(arg)
    { TRACE(("ExprAstFilter(%s)\n",name)); }
   ~ExprAstFilter() { delete m_arg; }
    const QCString &name() const { return m_name; }
    TemplateVariant apply(const TemplateVariant &v,TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return v; // should not happen
      TRACE(("Applying filter '%s' to '%s' (type=%d)\n",m_name.data(),v.toString().data(),v.type()));
      TemplateVariant arg;
      if (m_arg) arg = m_arg->resolve(c);
      bool ok;
      TemplateVariant result = TemplateFilterFactory::instance()->apply(m_name,v,arg,ok);
      if (!ok)
      {
        ci->warn(ci->templateName(),ci->line(),"unknown filter '%s'",m_name.data());
      }
      return result;
    }
  private:
    QCString m_name;
    ExprAst *m_arg = 0;
};

/** @brief Class representing a filter applied to an expression in the AST */
class ExprAstFilterAppl : public ExprAst
{
  public:
    ExprAstFilterAppl(ExprAst *expr,ExprAstFilter *filter)
      : m_expr(expr), m_filter(filter)
    { TRACE(("ExprAstFilterAppl\n")); }
   ~ExprAstFilterAppl() { delete m_expr; delete m_filter; }
    virtual TemplateVariant resolve(TemplateContext *c)
    {
      return m_filter->apply(m_expr->resolve(c),c);
    }
  private:
    ExprAst *m_expr = 0;
    ExprAstFilter *m_filter;
};

/** @brief Class representing a string literal in the AST */
class ExprAstLiteral : public ExprAst
{
  public:
    ExprAstLiteral(const char *lit) : m_literal(lit)
    { TRACE(("ExprAstLiteral(%s)\n",lit)); }
    const QCString &literal() const { return m_literal; }
    virtual TemplateVariant resolve(TemplateContext *) { return TemplateVariant(m_literal); }
  private:
    QCString m_literal;
};

/** @brief Class representing a negation (not) operator in the AST */
class ExprAstNegate : public ExprAst
{
  public:
    ExprAstNegate(ExprAst *expr) : m_expr(expr)
    { TRACE(("ExprAstNegate\n")); }
   ~ExprAstNegate() { delete m_expr; }
    virtual TemplateVariant resolve(TemplateContext *c)
    { return TemplateVariant(!m_expr->resolve(c).toBool()); }
  private:
    ExprAst *m_expr = 0;
};

class ExprAstUnary : public ExprAst
{
  public:
    ExprAstUnary(Operator::Type op,ExprAst *exp) : m_operator(op), m_exp(exp)
    { TRACE(("ExprAstUnary %s\n",Operator::toString(op))); }
   ~ExprAstUnary() { delete m_exp; }
    virtual TemplateVariant resolve(TemplateContext *c)
    {
      TemplateVariant exp = m_exp->resolve(c);
      switch (m_operator)
      {
        case Operator::Minus:
          return -exp.toInt();
        default:
          return TemplateVariant();
      }
    }
  private:
    Operator::Type m_operator = Operator::Or;
    ExprAst *m_exp = 0;
};

/** @brief Class representing a binary operator in the AST */
class ExprAstBinary : public ExprAst
{
  public:
    ExprAstBinary(Operator::Type op,ExprAst *lhs,ExprAst *rhs)
      : m_operator(op), m_lhs(lhs), m_rhs(rhs)
    { TRACE(("ExprAstBinary %s\n",Operator::toString(op))); }
   ~ExprAstBinary() { delete m_lhs; delete m_rhs; }
    virtual TemplateVariant resolve(TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return TemplateVariant(); // should not happen
      TemplateVariant lhs = m_lhs->resolve(c);
      TemplateVariant rhs = m_rhs ? m_rhs->resolve(c) : TemplateVariant();
      switch(m_operator)
      {
        case Operator::Or:
          return TemplateVariant(lhs.toBool() || rhs.toBool());
        case Operator::And:
          return TemplateVariant(lhs.toBool() && rhs.toBool());
        case Operator::Equal:
          return TemplateVariant(lhs == rhs);
        case Operator::NotEqual:
          return TemplateVariant(!(lhs == rhs));
        case Operator::Less:
          if (lhs.type()==TemplateVariant::String && rhs.type()==TemplateVariant::String)
          {
            return lhs.toString()<rhs.toString();
          }
          else
          {
            return lhs.toInt()<rhs.toInt();
          }
        case Operator::Greater:
          if (lhs.type()==TemplateVariant::String && rhs.type()==TemplateVariant::String)
          {
            return !(lhs.toString()<rhs.toString());
          }
          else
          {
            return lhs.toInt()>rhs.toInt();
          }
        case Operator::LessEqual:
          if (lhs.type()==TemplateVariant::String && rhs.type()==TemplateVariant::String)
          {
            return lhs.toString()==rhs.toString() || lhs.toString()<rhs.toString();
          }
          else
          {
            return lhs.toInt()<=rhs.toInt();
          }
        case Operator::GreaterEqual:
          if (lhs.type()==TemplateVariant::String && rhs.type()==TemplateVariant::String)
          {
            return lhs.toString()==rhs.toString() || !(lhs.toString()<rhs.toString());
          }
          else
          {
            return lhs.toInt()>=rhs.toInt();
          }
        case Operator::Plus:
          {
            return TemplateVariant(lhs.toInt() + rhs.toInt());
          }
        case Operator::Minus:
          {
            return TemplateVariant(lhs.toInt() - rhs.toInt());
          }
        case Operator::Multiply:
          {
            return TemplateVariant(lhs.toInt() * rhs.toInt());
          }
        case Operator::Divide:
          {
            int denom = rhs.toInt();
            if (denom!=0)
            {
              return TemplateVariant(lhs.toInt() / denom);
            }
            else // divide by zero
            {
              ci->warn(ci->templateName(),ci->line(),"division by zero while evaluating expression is undefined");
              return 0;
            }
          }
        case Operator::Modulo:
          {
            int denom = rhs.toInt();
            if (denom!=0)
            {
              return TemplateVariant(lhs.toInt() % denom);
            }
            else // module zero
            {
              ci->warn(ci->templateName(),ci->line(),"modulo zero while evaluating expression is undefined");
              return 0;
            }
          }
        default:
          return TemplateVariant();
      }
    }
  private:
    Operator::Type m_operator = Operator::Or;
    ExprAst *m_lhs = 0;
    ExprAst *m_rhs = 0;
};

//----------------------------------------------------------

/** @brief Base class of all nodes in a template's AST */
class TemplateNode
{
  public:
    TemplateNode(TemplateNode *parent) : m_parent(parent) {}
    virtual ~TemplateNode() {}

    virtual void render(FTextStream &ts, TemplateContext *c) = 0;

    TemplateNode *parent() { return m_parent; }

  private:
    TemplateNode *m_parent = 0;
};

//----------------------------------------------------------

/** @brief Class representing a lexical token in a template */
class TemplateToken
{
  public:
    enum Type { Text, Variable, Block };
    TemplateToken(Type t,const char *d,int l) : type(t), data(d), line(l) {}
    Type type = Text;
    QCString data;
    int line = 0;
};

using TemplateTokenPtr = std::unique_ptr<TemplateToken>;
using TemplateTokenStream = std::deque< TemplateTokenPtr >;

//----------------------------------------------------------

/** @brief Class representing a list of AST nodes in a template */
class TemplateNodeList : public std::vector< std::unique_ptr<TemplateNode> >
{
  public:
    void render(FTextStream &ts,TemplateContext *c)
    {
      TRACE(("{TemplateNodeList::render\n"));
      for (const auto &tn : *this)
      {
        tn->render(ts,c);
      }
      TRACE(("}TemplateNodeList::render\n"));
    }
};

//----------------------------------------------------------

/** @brief Parser for templates */
class TemplateParser
{
  public:
    TemplateParser(const TemplateEngine *engine,
                   const QCString &templateName,
                   TemplateTokenStream &tokens);
    void parse(TemplateNode *parent,int line,const StringVector &stopAt,
               TemplateNodeList &nodes);
    bool hasNextToken() const;
    TemplateTokenPtr takeNextToken();
    void removeNextToken();
    void prependToken(TemplateTokenPtr &&token);
    const TemplateToken *currentToken() const;
    QCString templateName() const { return m_templateName; }
    void warn(const char *fileName,int line,const char *fmt,...) const;
  private:
    const TemplateEngine *m_engine = 0;
    QCString m_templateName;
    TemplateTokenStream &m_tokens;
};

//--------------------------------------------------------------------

/** @brief Recursive decent parser for Django style template expressions.
 */
class ExpressionParser
{
  public:
    ExpressionParser(const TemplateParser *parser,int line)
      : m_parser(parser), m_line(line), m_tokenStream(0)
    {
    }
    virtual ~ExpressionParser()
    {
    }

    ExprAst *parse(const char *expr)
    {
      if (expr==0) return 0;
      m_tokenStream = expr;
      getNextToken();
      return parseExpression();
    }

  private:

    /** @brief Class representing a token within an expression. */
    class ExprToken
    {
      public:
        ExprToken() : type(Unknown), num(-1), op(Operator::Or)
        {
        }
        enum Type
        {
          Unknown, Operator, Number, Identifier, Literal
        };

        Type type;
        int num;
        QCString id;
        Operator::Type op;
    };

    ExprAst *parseExpression()
    {
      TRACE(("{parseExpression(%s)\n",m_tokenStream));
      ExprAst *result = parseOrExpression();
      TRACE(("}parseExpression(%s)\n",m_tokenStream));
      return result;
    }

    ExprAst *parseOrExpression()
    {
      TRACE(("{parseOrExpression(%s)\n",m_tokenStream));
      ExprAst *lhs = parseAndExpression();
      if (lhs)
      {
        while (m_curToken.type==ExprToken::Operator &&
            m_curToken.op==Operator::Or)
        {
          getNextToken();
          ExprAst *rhs = parseAndExpression();
          lhs = new ExprAstBinary(Operator::Or,lhs,rhs);
        }
      }
      TRACE(("}parseOrExpression(%s)\n",m_tokenStream));
      return lhs;
    }

    ExprAst *parseAndExpression()
    {
      TRACE(("{parseAndExpression(%s)\n",m_tokenStream));
      ExprAst *lhs = parseNotExpression();
      if (lhs)
      {
        while (m_curToken.type==ExprToken::Operator &&
               m_curToken.op==Operator::And)
        {
          getNextToken();
          ExprAst *rhs = parseNotExpression();
          lhs = new ExprAstBinary(Operator::And,lhs,rhs);
        }
      }
      TRACE(("}parseAndExpression(%s)\n",m_tokenStream));
      return lhs;
    }

    ExprAst *parseNotExpression()
    {
      TRACE(("{parseNotExpression(%s)\n",m_tokenStream));
      ExprAst *result=0;
      if (m_curToken.type==ExprToken::Operator &&
          m_curToken.op==Operator::Not)
      {
        getNextToken();
        ExprAst *expr = parseCompareExpression();
        if (expr==0)
        {
          warn(m_parser->templateName(),m_line,"argument missing for not operator");
          return 0;
        }
        result = new ExprAstNegate(expr);
      }
      else
      {
        result = parseCompareExpression();
      }
      TRACE(("}parseNotExpression(%s)\n",m_tokenStream));
      return result;
    }

    ExprAst *parseCompareExpression()
    {
      TRACE(("{parseCompareExpression(%s)\n",m_tokenStream));
      ExprAst *lhs = parseAdditiveExpression();
      if (lhs)
      {
        Operator::Type op = m_curToken.op;
        if (m_curToken.type==ExprToken::Operator &&
            (op==Operator::Less      ||
             op==Operator::Greater   ||
             op==Operator::Equal     ||
             op==Operator::NotEqual  ||
             op==Operator::LessEqual ||
             op==Operator::GreaterEqual
            )
           )
        {
          getNextToken();
          ExprAst *rhs = parseNotExpression();
          lhs = new ExprAstBinary(op,lhs,rhs);
        }
      }
      TRACE(("}parseCompareExpression(%s)\n",m_tokenStream));
      return lhs;
    }

    ExprAst *parseAdditiveExpression()
    {
      TRACE(("{parseAdditiveExpression(%s)\n",m_tokenStream));
      ExprAst *lhs = parseMultiplicativeExpression();
      if (lhs)
      {
        while (m_curToken.type==ExprToken::Operator &&
               (m_curToken.op==Operator::Plus || m_curToken.op==Operator::Minus))
        {
          Operator::Type op = m_curToken.op;
          getNextToken();
          ExprAst *rhs = parseMultiplicativeExpression();
          lhs = new ExprAstBinary(op,lhs,rhs);
        }
      }
      TRACE(("}parseAdditiveExpression(%s)\n",m_tokenStream));
      return lhs;
    }

    ExprAst *parseMultiplicativeExpression()
    {
      TRACE(("{parseMultiplicativeExpression(%s)\n",m_tokenStream));
      ExprAst *lhs = parseUnaryExpression();
      if (lhs)
      {
        while (m_curToken.type==ExprToken::Operator &&
               (m_curToken.op==Operator::Multiply || m_curToken.op==Operator::Divide || m_curToken.op==Operator::Modulo))
        {
          Operator::Type op = m_curToken.op;
          getNextToken();
          ExprAst *rhs = parseUnaryExpression();
          lhs = new ExprAstBinary(op,lhs,rhs);
        }
      }
      TRACE(("}parseMultiplicativeExpression(%s)\n",m_tokenStream));
      return lhs;
    }

    ExprAst *parseUnaryExpression()
    {
      TRACE(("{parseUnaryExpression(%s)\n",m_tokenStream));
      ExprAst *result=0;
      if (m_curToken.type==ExprToken::Operator)
      {
        if (m_curToken.op==Operator::Plus)
        {
          getNextToken();
          result = parsePrimaryExpression();
        }
        else if (m_curToken.op==Operator::Minus)
        {
          getNextToken();
          ExprAst *rhs = parsePrimaryExpression();
          result = new ExprAstUnary(m_curToken.op,rhs);
        }
        else
        {
          result = parsePrimaryExpression();
        }
      }
      else
      {
        result = parsePrimaryExpression();
      }
      TRACE(("}parseUnaryExpression(%s)\n",m_tokenStream));
      return result;
    }

    ExprAst *parsePrimaryExpression()
    {
      TRACE(("{parsePrimary(%s)\n",m_tokenStream));
      ExprAst *result=0;
      switch (m_curToken.type)
      {
        case ExprToken::Number:
          result = parseNumber();
          break;
        case ExprToken::Identifier:
          result = parseFilteredVariable();
          break;
        case ExprToken::Literal:
          result = parseLiteral();
          break;
        case ExprToken::Operator:
          if (m_curToken.op==Operator::LeftParen)
          {
            getNextToken(); // skip over opening bracket
            result = parseExpression();
            if (m_curToken.type!=ExprToken::Operator ||
                m_curToken.op!=Operator::RightParen)
            {
              warn(m_parser->templateName(),m_line,"missing closing parenthesis");
            }
            else
            {
              getNextToken(); // skip over closing bracket
            }
          }
          else
          {
            warn(m_parser->templateName(),m_line,"unexpected operator '%s' in expression",
                Operator::toString(m_curToken.op));
          }
          break;
        default:
          warn(m_parser->templateName(),m_line,"unexpected token in expression");
      }
      TRACE(("}parsePrimary(%s)\n",m_tokenStream));
      return result;
    }

    ExprAst *parseNumber()
    {
      TRACE(("{parseNumber(%d)\n",m_curToken.num));
      ExprAst *num = new ExprAstNumber(m_curToken.num);
      getNextToken();
      TRACE(("}parseNumber()\n"));
      return num;
    }

    ExprAst *parseIdentifier()
    {
      TRACE(("{parseIdentifier(%s)\n",m_curToken.id.data()));
      ExprAst *id = new ExprAstVariable(m_curToken.id);
      getNextToken();
      TRACE(("}parseIdentifier()\n"));
      return id;
    }

    ExprAst *parseLiteral()
    {
      TRACE(("{parseLiteral(%s)\n",m_curToken.id.data()));
      ExprAst *expr = new ExprAstLiteral(m_curToken.id);
      getNextToken();
      TRACE(("}parseLiteral()\n"));
      return expr;
    }

    ExprAst *parseIdentifierOptionalArgs()
    {
      TRACE(("{parseIdentifierOptionalArgs(%s)\n",m_curToken.id.data()));
      ExprAst *expr = parseIdentifier();
      if (expr)
      {
        if (m_curToken.type==ExprToken::Operator &&
            m_curToken.op==Operator::Colon)
        {
          getNextToken();
          ExprAstList args;
          args.push_back(std::unique_ptr<ExprAst>(parsePrimaryExpression()));
          while (m_curToken.type==ExprToken::Operator &&
                 m_curToken.op==Operator::Comma)
          {
            getNextToken();
            args.push_back(std::unique_ptr<ExprAst>(parsePrimaryExpression()));
          }
          expr = new ExprAstFunctionVariable(expr,std::move(args));
        }
      }
      TRACE(("}parseIdentifierOptionalArgs()\n"));
      return expr;
    }

    ExprAst *parseFilteredVariable()
    {
      TRACE(("{parseFilteredVariable()\n"));
      ExprAst *expr = parseIdentifierOptionalArgs();
      if (expr)
      {
        while (m_curToken.type==ExprToken::Operator &&
               m_curToken.op==Operator::Filter)
        {
          getNextToken();
          ExprAstFilter *filter = parseFilter();
          if (!filter) break;
          expr = new ExprAstFilterAppl(expr,filter);
        }
      }
      TRACE(("}parseFilteredVariable()\n"));
      return expr;
    }

    ExprAstFilter *parseFilter()
    {
      TRACE(("{parseFilter(%s)\n",m_curToken.id.data()));
      QCString filterName = m_curToken.id;
      getNextToken();
      ExprAst *argExpr=0;
      if (m_curToken.type==ExprToken::Operator &&
          m_curToken.op==Operator::Colon)
      {
        getNextToken();
        argExpr = parsePrimaryExpression();
      }
      ExprAstFilter *filter = new ExprAstFilter(filterName,argExpr);
      TRACE(("}parseFilter()\n"));
      return filter;
    }


    bool getNextToken()
    {
      const char *p = m_tokenStream;
      char s[2];
      s[1]=0;
      if (p==0 || *p=='\0') return FALSE;
      while (*p==' ') p++; // skip over spaces
      char c=*p;
      if (*p=='\0') // only spaces...
      {
        m_tokenStream = p;
        return FALSE;
      }
      const char *q = p;
      switch (c)
      {
        case '=':
          if (c=='=' && *(p+1)=='=') // equal
          {
            m_curToken.op = Operator::Equal;
            p+=2;
          }
          break;
        case '!':
          if (c=='!' && *(p+1)=='=') // not equal
          {
            m_curToken.op = Operator::NotEqual;
            p+=2;
          }
          break;
        case '<':
          if (c=='<' && *(p+1)=='=') // less or equal
          {
            m_curToken.op = Operator::LessEqual;
            p+=2;
          }
          else // less
          {
            m_curToken.op = Operator::Less;
            p++;
          }
          break;
        case '>':
          if (c=='>' && *(p+1)=='=') // greater or equal
          {
            m_curToken.op = Operator::GreaterEqual;
            p+=2;
          }
          else // greater
          {
            m_curToken.op = Operator::Greater;
            p++;
          }
          break;
        case '(':
          m_curToken.op = Operator::LeftParen;
          p++;
          break;
        case ')':
          m_curToken.op = Operator::RightParen;
          p++;
          break;
        case '|':
          m_curToken.op = Operator::Filter;
          p++;
          break;
        case '+':
          m_curToken.op = Operator::Plus;
          p++;
          break;
        case '-':
          m_curToken.op = Operator::Minus;
          p++;
          break;
        case '*':
          m_curToken.op = Operator::Multiply;
          p++;
          break;
        case '/':
          m_curToken.op = Operator::Divide;
          p++;
          break;
        case '%':
          m_curToken.op = Operator::Modulo;
          p++;
          break;
        case ':':
          m_curToken.op = Operator::Colon;
          p++;
          break;
        case ',':
          m_curToken.op = Operator::Comma;
          p++;
          break;
        case 'n':
          if (strncmp(p,"not ",4)==0)
          {
            m_curToken.op = Operator::Not;
            p+=4;
          }
          break;
        case 'a':
          if (strncmp(p,"and ",4)==0)
          {
            m_curToken.op = Operator::And;
            p+=4;
          }
          break;
        case 'o':
          if (strncmp(p,"or ",3)==0)
          {
            m_curToken.op = Operator::Or;
            p+=3;
          }
          break;
        default:
          break;
      }
      if (p!=q) // found an operator
      {
        m_curToken.type = ExprToken::Operator;
      }
      else // no token found yet
      {
        if (c>='0' && c<='9') // number?
        {
          m_curToken.type = ExprToken::Number;
          const char *np = p;
          m_curToken.num = 0;
          while (*np>='0' && *np<='9')
          {
            m_curToken.num*=10;
            m_curToken.num+=*np-'0';
            np++;
          }
          p=np;
        }
        else if (c=='_' || (c>='a' && c<='z') || (c>='A' && c<='Z')) // identifier?
        {
          m_curToken.type = ExprToken::Identifier;
          s[0]=c;
          m_curToken.id = s;
          p++;
          while ((c=*p) &&
              (c=='_' || c=='.' ||
               (c>='a' && c<='z') ||
               (c>='A' && c<='Z') ||
               (c>='0' && c<='9'))
              )
          {
            s[0]=c;
            m_curToken.id+=s;
            p++;
          }
          if (m_curToken.id=="True") // treat true literal as numerical 1
          {
            m_curToken.type = ExprToken::Number;
            m_curToken.num = 1;
          }
          else if (m_curToken.id=="False") // treat false literal as numerical 0
          {
            m_curToken.type = ExprToken::Number;
            m_curToken.num = 0;
          }
        }
        else if (c=='"' || c=='\'') // string literal
        {
          m_curToken.type = ExprToken::Literal;
          m_curToken.id.resize(0);
          p++;
          char tokenChar = c;
          char cp=0;
          while ((c=*p) && (c!=tokenChar || (c==tokenChar && cp=='\\')))
          {
            s[0]=c;
            if (c!='\\' || cp=='\\') // don't add escapes
            {
              m_curToken.id+=s;
            }
            cp=c;
            p++;
          }
          if (*p==tokenChar) p++;
        }
      }
      if (p==q) // still no valid token found -> error
      {
        m_curToken.type = ExprToken::Unknown;
        s[0]=c;
        s[1]=0;
        warn(m_parser->templateName(),m_line,"Found unknown token '%s' (%d) while parsing %s",s,c,m_tokenStream);
        m_curToken.id = s;
        p++;
      }
      //TRACE(("token type=%d op=%d num=%d id=%s\n",
      //    m_curToken.type,m_curToken.op,m_curToken.num,m_curToken.id.data()));

      m_tokenStream = p;
      return TRUE;
    }

    const TemplateParser *m_parser = 0;
    ExprToken m_curToken;
    int m_line = 0;
    const char *m_tokenStream;
};

//----------------------------------------------------------

/** @brief Internal class representing the implementation of a template */
class TemplateImpl : public TemplateNode, public Template
{
  public:
    TemplateImpl(TemplateEngine *e,const QCString &name,const QCString &data,
                 const QCString &extension);
   ~TemplateImpl();
    void render(FTextStream &ts, TemplateContext *c);

    TemplateEngine *engine() const { return m_engine; }
    TemplateBlockContext *blockContext() { return &m_blockContext; }

  private:
    TemplateEngine *m_engine = 0;
    QCString m_name;
    TemplateNodeList m_nodes;
    TemplateBlockContext m_blockContext;
};

//----------------------------------------------------------

/** @brief Weak reference wrapper for TemplateStructIntf that provides access to the
 *  wrapped struct without holding a reference.
 */
class TemplateStructWeakRef : public TemplateStructIntf
{
  public:
    TemplateStructWeakRef(TemplateStructIntf *ref) : m_ref(ref), m_refCount(0) {}
    virtual TemplateVariant get(const char *name) const { return m_ref->get(name); }
    virtual int addRef() { return ++m_refCount; }
    virtual int release() { int count=--m_refCount; if (count<=0) { delete this; } return count; }
  private:
    TemplateStructIntf *m_ref = 0;
    int m_refCount = 0;
};

//----------------------------------------------------------

TemplateContextImpl::TemplateContextImpl(const TemplateEngine *e)
  : m_engine(e), m_templateName("<unknown>"), m_line(1), m_activeEscapeIntf(0),
    m_spacelessIntf(0), m_spacelessEnabled(FALSE), m_tabbingEnabled(FALSE), m_indices(TemplateStruct::alloc())
{
  m_fromUtf8 = (void*)(-1);
  push();
  set("index",m_indices.get());
}

TemplateContextImpl::~TemplateContextImpl()
{
  pop();
}

void TemplateContextImpl::setEncoding(const QCString &templateName,int line,const QCString &enc)
{
  if (enc==m_encoding) return; // nothing changed
  if (m_fromUtf8!=(void *)(-1))
  {
    portable_iconv_close(m_fromUtf8);
    m_fromUtf8 = (void*)(-1);
  }
  m_encoding=enc;
  if (!enc.isEmpty())
  {
    m_fromUtf8 = portable_iconv_open(enc,"UTF-8");
    if (m_fromUtf8==(void*)(-1))
    {
      warn(templateName,line,"unsupported character conversion: '%s'->'UTF-8'\n", enc.data());
    }
  }
  //printf("TemplateContextImpl::setEncoding(%s)\n",enc.data());
}

QCString TemplateContextImpl::recode(const QCString &s)
{
  //printf("TemplateContextImpl::recode(%s)\n",s.data());
  int iSize        = s.length();
  int oSize        = iSize*4+1;
  QCString output(oSize);
  size_t iLeft     = iSize;
  size_t oLeft     = oSize;
  char *iPtr       = s.rawData();
  char *oPtr       = output.rawData();
  if (!portable_iconv(m_fromUtf8,&iPtr,&iLeft,&oPtr,&oLeft))
  {
    oSize -= (int)oLeft;
    output.resize(oSize+1);
    output.at(oSize)='\0';
    return output;
  }
  else
  {
    return s;
  }
}

void TemplateContextImpl::set(const char *name,const TemplateVariant &v)
{
  auto &ctx = m_contextStack.front();
  auto it = ctx.find(name);
  if (it!=ctx.end())
  {
    ctx.erase(it);
  }
  ctx.insert(std::make_pair(name,v));
}

TemplateVariant TemplateContextImpl::get(const QCString &name) const
{
  int i=name.find('.');
  if (i==-1) // simple name
  {
    return getPrimary(name);
  }
  else // obj.prop
  {
    QCString objName = name.left(i);
    TemplateVariant v = getPrimary(objName);
    QCString propName = name.mid(i+1);
    while (!propName.isEmpty())
    {
      //printf("getPrimary(%s) type=%d:%s\n",objName.data(),v.type(),v.toString().data());
      if (v.type()==TemplateVariant::Struct)
      {
        i = propName.find(".");
        int l = i==-1 ? propName.length() : i;
        v = v.toStruct()->get(propName.left(l));
        if (!v.isValid())
        {
          warn(m_templateName,m_line,"requesting non-existing property '%s' for object '%s'",propName.left(l).data(),objName.data());
        }
        if (i!=-1)
        {
          objName = propName.left(i);
          propName = propName.mid(i+1);
        }
        else
        {
          propName.resize(0);
        }
      }
      else if (v.type()==TemplateVariant::List)
      {
        i = propName.find(".");
        int l = i==-1 ? propName.length() : i;
        bool b;
        int index = propName.left(l).toInt(&b);
        if (b)
        {
          v = v.toList()->at(index);
        }
        else
        {
          warn(m_templateName,m_line,"list index '%s' is not valid",propName.data());
          break;
        }
        if (i!=-1)
        {
          propName = propName.mid(i+1);
        }
        else
        {
          propName.resize(0);
        }
      }
      else
      {
        warn(m_templateName,m_line,"using . on an object '%s' is not an struct or list",objName.data());
        return TemplateVariant();
      }
    }
    return v;
  }
}

const TemplateVariant *TemplateContextImpl::getRef(const QCString &name) const
{
  for (const auto &ctx : m_contextStack)
  {
    auto it = ctx.find(name.str());
    if (it!=ctx.end())
    {
      return &it->second;
    }
  }
  return 0; // not found
}

TemplateVariant TemplateContextImpl::getPrimary(const QCString &name) const
{
  const TemplateVariant *v = getRef(name);
  return v ? *v : TemplateVariant();
}

void TemplateContextImpl::push()
{
  m_contextStack.push_front(std::map<std::string,TemplateVariant>());
}

void TemplateContextImpl::pop()
{
  if (m_contextStack.empty())
  {
    warn(m_templateName,m_line,"pop() called on empty context stack!\n");
  }
  else
  {
    m_contextStack.pop_front();
  }
}

TemplateBlockContext *TemplateContextImpl::blockContext()
{
  return &m_blockContext;
}

void TemplateContextImpl::warn(const char *fileName,int line,const char *fmt,...) const
{
  va_list args;
  va_start(args,fmt);
  va_warn(fileName,line,fmt,args);
  va_end(args);
  m_engine->printIncludeContext(fileName,line);
}

void TemplateContextImpl::openSubIndex(const QCString &indexName)
{
  //printf("TemplateContextImpl::openSubIndex(%s)\n",indexName.data());
  auto kv = m_indexStacks.find(indexName.str());
  if (kv==m_indexStacks.end() || kv->second.empty() || kv->second.top().type()==TemplateVariant::List) // error: no stack yet or no entry
  {
    warn(m_templateName,m_line,"opensubindex for index %s without preceding indexentry",indexName.data());
    return;
  }
  // get the parent entry to add the list to
  auto &stack = kv->second;
  TemplateStruct *entry = dynamic_cast<TemplateStruct*>(stack.top().toStruct());
  if (entry)
  {
    // add new list to the stack
    TemplateList *list = TemplateList::alloc();
    stack.emplace(list);
    entry->set("children",list);
    entry->set("is_leaf_node",false);
  }
}

void TemplateContextImpl::closeSubIndex(const QCString &indexName)
{
  //printf("TemplateContextImpl::closeSubIndex(%s)\n",indexName.data());
  auto kv = m_indexStacks.find(indexName.str());
  if (kv==m_indexStacks.end() || kv->second.size()<3)
  {
    warn(m_templateName,m_line,"closesubindex for index %s without matching open",indexName.data());
  }
  else
  {
    auto &stack = kv->second; // stack.size()>2
    if (stack.top().type()==TemplateVariant::Struct)
    {
      stack.pop(); // pop struct
      stack.pop(); // pop list
    }
    else // empty list! correct "is_left_node" attribute of the parent entry
    {
      stack.pop(); // pop list
      TemplateStruct *entry = dynamic_cast<TemplateStruct*>(stack.top().toStruct());
      if (entry)
      {
        entry->set("is_leaf_node",true);
      }
    }
  }
  //fprintf(stderr,"TemplateContextImpl::closeSubIndex(%s) end g_count=%d\n\n",indexName.data(),g_count);
}

static void getPathListFunc(TemplateStructIntf *entry,TemplateList *list)
{
  TemplateVariant parent = entry->get("parent");
  if (parent.type()==TemplateVariant::Struct)
  {
    getPathListFunc(parent.toStruct(),list);
  }
  list->append(entry);
}

static TemplateVariant getPathFunc(const void *ctx, const std::vector<TemplateVariant> &)
{
  TemplateStruct *entry = (TemplateStruct*)ctx;
  TemplateList *result = TemplateList::alloc();
  getPathListFunc(entry,result);
  return result;
}

void TemplateContextImpl::addIndexEntry(const QCString &indexName,const std::vector<TemplateKeyValue> &arguments)
{
  auto it = arguments.begin();
  //printf("TemplateContextImpl::addIndexEntry(%s)\n",indexName.data());
  //while (it!=arguments.end())
  //{
  //  printf("  key=%s value=%s\n",(*it).key.data(),(*it).value.toString().data());
  //  ++it;
  //}
  TemplateVariant parent(FALSE);
  auto kv = m_indexStacks.find(indexName.str());
  if (kv==m_indexStacks.end()) // no stack yet, create it!
  {
    kv = m_indexStacks.insert(std::make_pair(indexName.str(),std::stack<TemplateVariant>())).first;
  }
  TemplateList *list  = 0;
  auto &stack = kv->second;
  if (stack.empty()) // first item, create empty list and add it to the index
  {
    list = TemplateList::alloc();
    stack.emplace(list);
    m_indices->set(indexName,list); // make list available under index
  }
  else // stack not empty
  {
    if (stack.top().type()==TemplateVariant::Struct) // already an entry in the list
    {
      // remove current entry from the stack
      stack.pop();
    }
    else // first entry after opensubindex
    {
      ASSERT(stack.top().type()==TemplateVariant::List);
    }
    if (stack.size()>1)
    {
      TemplateVariant tmp = stack.top();
      stack.pop();
      // To prevent a cyclic dependency between parent and child which causes a memory
      // leak, we wrap the parent into a weak reference version.
      parent = new TemplateStructWeakRef(stack.top().toStruct());
      stack.push(tmp);
      ASSERT(parent.type()==TemplateVariant::Struct);
    }
    // get list to add new item
    list = dynamic_cast<TemplateList*>(stack.top().toList());
  }
  TemplateStruct *entry = TemplateStruct::alloc();
  // add user specified fields to the entry
  for (it=arguments.begin();it!=arguments.end();++it)
  {
    entry->set((*it).key,(*it).value);
  }
  if (list->count()>0)
  {
    TemplateStruct *lastEntry = dynamic_cast<TemplateStruct*>(list->at(list->count()-1).toStruct());
    if (lastEntry)
    {
      lastEntry->set("last",false);
    }
  }
  entry->set("is_leaf_node",true);
  entry->set("first",list->count()==0);
  entry->set("index",list->count());
  entry->set("parent",parent);
  entry->set("path",TemplateVariant::Delegate::fromFunction(entry,getPathFunc));
  entry->set("last",true);
  stack.emplace(entry);
  list->append(entry);
}

//----------------------------------------------------------

/** @brief Class representing a piece of plain text in a template */
class TemplateNodeText : public TemplateNode
{
  public:
    TemplateNodeText(TemplateParser *,TemplateNode *parent,int,const QCString &data)
      : TemplateNode(parent), m_data(data)
    {
      TRACE(("TemplateNodeText('%s')\n",replace(data,'\n',' ').data()));
    }

    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      //printf("TemplateNodeText::render(%s) needsRecoding=%d ci=%p\n",m_data.data(),ci->needsRecoding(),ci);
      if (ci->spacelessEnabled())
      {
        if (ci->needsRecoding())
        {
          ts << ci->recode(ci->spacelessIntf()->remove(m_data));
        }
        else
        {
          ts << ci->spacelessIntf()->remove(m_data);
        }
      }
      else
      {
        if (ci->needsRecoding())
        {
          ts << ci->recode(m_data);
        }
        else
        {
          ts << m_data;
        }
      }
    }
  private:
    QCString m_data;
};

//----------------------------------------------------------

/** @brief Class representing a variable in a template */
class TemplateNodeVariable : public TemplateNode
{
  public:
    TemplateNodeVariable(TemplateParser *parser,TemplateNode *parent,int line,const QCString &var)
      : TemplateNode(parent), m_templateName(parser->templateName()), m_line(line)
    {
      TRACE(("TemplateNodeVariable(%s)\n",var.data()));
      ExpressionParser expParser(parser,line);
      m_var = expParser.parse(var);
      if (m_var==0)
      {
        parser->warn(m_templateName,line,"invalid expression '%s' for variable",var.data());
      }
    }
    ~TemplateNodeVariable()
    {
      delete m_var;
    }

    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      if (m_var)
      {
        TemplateVariant v = m_var->resolve(c);
        if (v.type()==TemplateVariant::Function)
        {
          v = v.call(std::vector<TemplateVariant>());
        }
        if (ci->escapeIntf() && !v.raw())
        {
          if (ci->needsRecoding())
          {
            ts << ci->recode(ci->escapeIntf()->escape(v.toString()));
          }
          else
          {
            ts << ci->escapeIntf()->escape(v.toString());
          }
        }
        else
        {
          if (ci->needsRecoding())
          {
            ts << ci->recode(v.toString());
          }
          else
          {
            ts << v.toString();
          }
        }
      }
    }

  private:
    QCString m_templateName;
    int m_line = 0;
    ExprAst *m_var = 0;
};

//----------------------------------------------------------

/** @brief Helper class for creating template AST tag nodes and returning
  * the template for a given node.
 */
template<class T> class TemplateNodeCreator : public TemplateNode
{
  public:
    TemplateNodeCreator(TemplateParser *parser,TemplateNode *parent,int line)
      : TemplateNode(parent), m_templateName(parser->templateName()), m_line(line) {}
    static TemplateNode *createInstance(TemplateParser *parser,
                                        TemplateNode *parent,
                                        int line,
                                        const QCString &data)
    {
      return new T(parser,parent,line,data);
    }
    TemplateImpl *getTemplate()
    {
      TemplateNode *root = this;
      while (root && root->parent())
      {
        root = root->parent();
      }
      return dynamic_cast<TemplateImpl*>(root);
    }
  protected:
    void mkpath(TemplateContextImpl *ci,const QCString &fileName)
    {
      int i=fileName.find('/');
      QCString outputDir = ci->outputDirectory();
      QDir d(outputDir);
      if (!d.exists())
      {
        QDir rootDir;
        rootDir.setPath(QDir::currentDirPath());
        if (!rootDir.mkdir(outputDir))
        {
          err("tag OUTPUT_DIRECTORY: Output directory '%s' does not "
	      "exist and cannot be created\n",outputDir.data());
          return;
        }
        d.setPath(outputDir);
      }
      int j=0;
      while (i!=-1) // fileName contains path part
      {
        if (d.exists())
        {
          bool ok = d.mkdir(fileName.mid(j,i-j));
          if (!ok)
          {
            err("Failed to create directory '%s'\n",(fileName.mid(j,i-j)).data());
            break;
          }
          QCString dirName = outputDir+'/'+fileName.left(i);
          d = QDir(dirName);
          j = i+1;
        }
        i=fileName.find('/',i+1);
      }
    }
    QCString m_templateName;
    int m_line = 0;
};

//----------------------------------------------------------

/** @brief Class representing an 'if' tag in a template */
class TemplateNodeIf : public TemplateNodeCreator<TemplateNodeIf>
{
  public:
    TemplateNodeIf(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data) :
      TemplateNodeCreator<TemplateNodeIf>(parser,parent,line)
    {
      TRACE(("{TemplateNodeIf(%s)\n",data.data()));
      if (data.isEmpty())
      {
        parser->warn(m_templateName,line,"missing argument for if tag");
      }
      StringVector stopAt = { "endif", "elif", "else" };

      // if 'nodes'
      {
        m_ifGuardedNodes.push_back(std::make_unique<GuardedNodes>());
        auto &guardedNodes = m_ifGuardedNodes.back();
        ExpressionParser ex(parser,line);
        guardedNodes->line = line;
        guardedNodes->guardAst = ex.parse(data);
        parser->parse(this,line,stopAt,guardedNodes->trueNodes);
      }
      auto tok = parser->takeNextToken();

      // elif 'nodes'
      while (tok && tok->data.left(5)=="elif ")
      {
        m_ifGuardedNodes.push_back(std::make_unique<GuardedNodes>());
        auto &guardedNodes = m_ifGuardedNodes.back();
        ExpressionParser ex(parser,line);
        guardedNodes->line = tok->line;
        guardedNodes->guardAst = ex.parse(tok->data.mid(5));
        parser->parse(this,tok->line,stopAt,guardedNodes->trueNodes);
        // proceed to the next token
        tok = parser->takeNextToken();
      }

      // else 'nodes'
      if (tok && tok->data=="else")
      {
        stopAt.pop_back(); // remove "else"
        stopAt.pop_back(); // remove "elif"
        parser->parse(this,line,stopAt,m_falseNodes);
        parser->removeNextToken(); // skip over endif
      }
      TRACE(("}TemplateNodeIf(%s)\n",data.data()));
    }
    ~TemplateNodeIf()
    {
    }

    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      //printf("TemplateNodeIf::render #trueNodes=%d #falseNodes=%d\n",m_trueNodes.count(),m_falseNodes.count());
      bool processed=FALSE;
      for (auto &nodes : m_ifGuardedNodes)
      {
        if (nodes->guardAst)
        {
          TemplateVariant guardValue = nodes->guardAst->resolve(c);
          if (guardValue.toBool()) // render nodes for the first guard that evaluated to 'true'
          {
            nodes->trueNodes.render(ts,c);
            processed=TRUE;
            break;
          }
        }
        else
        {
          ci->warn(m_templateName,nodes->line,"invalid expression for if/elif");
        }
      }
      if (!processed)
      {
        // all guards are false, render 'else' nodes
        m_falseNodes.render(ts,c);
      }
    }
  private:
    struct GuardedNodes
    {
      GuardedNodes() : guardAst(0) {}
     ~GuardedNodes() { delete guardAst; }
      int line = 0;
      ExprAst *guardAst = 0;
      TemplateNodeList trueNodes;
    };
    std::vector< std::unique_ptr<GuardedNodes> > m_ifGuardedNodes;
    TemplateNodeList m_falseNodes;
};

//----------------------------------------------------------
/** @brief Class representing a 'for' tag in a template */
class TemplateNodeRepeat : public TemplateNodeCreator<TemplateNodeRepeat>
{
  public:
    TemplateNodeRepeat(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeRepeat>(parser,parent,line)
    {
      TRACE(("{TemplateNodeRepeat(%s)\n",data.data()));
      ExpressionParser expParser(parser,line);
      m_expr = expParser.parse(data);
      StringVector stopAt = { "endrepeat" };
      parser->parse(this,line,stopAt,m_repeatNodes);
      parser->removeNextToken(); // skip over endrepeat
      TRACE(("}TemplateNodeRepeat(%s)\n",data.data()));
    }
    ~TemplateNodeRepeat()
    {
      delete m_expr;
    }
    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      TemplateVariant v;
      if (m_expr && (v=m_expr->resolve(c)).type()==TemplateVariant::Integer)
      {
        int i, n = v.toInt();
        for (i=0;i<n;i++)
        {
          TemplateAutoRef<TemplateStruct> s(TemplateStruct::alloc());
          s->set("counter0",    (int)i);
          s->set("counter",     (int)(i+1));
          s->set("revcounter",  (int)(n-i));
          s->set("revcounter0", (int)(n-i-1));
          s->set("first",i==0);
          s->set("last", i==n-1);
          c->set("repeatloop",s.get());
          // render all items for this iteration of the loop
          m_repeatNodes.render(ts,c);
        }
      }
      else // simple type...
      {
        ci->warn(m_templateName,m_line,"for requires a variable of list type!");
      }
    }
  private:
    TemplateNodeList m_repeatNodes;
    ExprAst *m_expr = 0;
};

//----------------------------------------------------------

/** @brief Class representing a 'range' tag in a template */
class TemplateNodeRange : public TemplateNodeCreator<TemplateNodeRange>
{
  public:
    TemplateNodeRange(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeRange>(parser,parent,line), m_down(FALSE)
    {
      TRACE(("{TemplateNodeRange(%s)\n",data.data()));
      QCString start,end;
      int i1 = data.find(" from ");
      int i2 = data.find(" to ");
      int i3 = data.find(" downto ");
      if (i1==-1)
      {
        if (data.right(5)==" from")
        {
          parser->warn(m_templateName,line,"range missing after 'from' keyword");
        }
        else if (data=="from")
        {
          parser->warn(m_templateName,line,"range needs an iterator variable and a range");
        }
        else
        {
          parser->warn(m_templateName,line,"range is missing 'from' keyword");
        }
      }
      else if (i2==-1 && i3==-1)
      {
        if (data.right(3)==" to")
        {
          parser->warn(m_templateName,line,"range is missing end value after 'to' keyword");
        }
        else if (data.right(7)==" downto")
        {
          parser->warn(m_templateName,line,"range is missing end value after 'downto' keyword");
        }
        else
        {
          parser->warn(m_templateName,line,"range is missing 'to' or 'downto' keyword");
        }
      }
      else
      {
        m_var = data.left(i1).stripWhiteSpace();
        if (m_var.isEmpty())
        {
          parser->warn(m_templateName,line,"range needs an iterator variable");
        }
        start = data.mid(i1+6,i2-i1-6).stripWhiteSpace();
        if (i2!=-1)
        {
          end = data.right(data.length()-i2-4).stripWhiteSpace();
          m_down = FALSE;
        }
        else if (i3!=-1)
        {
          end = data.right(data.length()-i3-8).stripWhiteSpace();
          m_down = TRUE;
        }
      }
      ExpressionParser expParser(parser,line);
      m_startExpr = expParser.parse(start);
      m_endExpr   = expParser.parse(end);

      StringVector stopAt = { "endrange" };
      parser->parse(this,line,stopAt,m_loopNodes);
      parser->removeNextToken(); // skip over endrange
      TRACE(("}TemplateNodeRange(%s)\n",data.data()));
    }

    ~TemplateNodeRange()
    {
      delete m_startExpr;
      delete m_endExpr;
    }

    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      //printf("TemplateNodeRange::render #loopNodes=%d\n",
      //    m_loopNodes.count());
      if (m_startExpr && m_endExpr)
      {
        TemplateVariant vs = m_startExpr->resolve(c);
        TemplateVariant ve = m_endExpr->resolve(c);
        if (vs.type()==TemplateVariant::Integer && ve.type()==TemplateVariant::Integer)
        {
          int s = vs.toInt();
          int e = ve.toInt();
          int l = m_down ? s-e+1 : e-s+1;
          if (l>0)
          {
            c->push();
            //int index = m_reversed ? list.count() : 0;
            const TemplateVariant *parentLoop = c->getRef("forloop");
            uint index = 0;
            int i = m_down ? e : s;
            bool done=false;
            while (!done)
            {
              // set the forloop meta-data variable
              TemplateAutoRef<TemplateStruct> ls(TemplateStruct::alloc());
              ls->set("counter0",    (int)index);
              ls->set("counter",     (int)(index+1));
              ls->set("revcounter",  (int)(l-index));
              ls->set("revcounter0", (int)(l-index-1));
              ls->set("first",index==0);
              ls->set("last", (int)index==l-1);
              ls->set("parentloop",parentLoop ? *parentLoop : TemplateVariant());
              c->set("forloop",ls.get());

              // set the iterator variable
              c->set(m_var,i);

              // render all items for this iteration of the loop
              m_loopNodes.render(ts,c);

              index++;
              if (m_down)
              {
                i--;
                done = i<e;
              }
              else
              {
                i++;
                done = i>e;
              }
            }
            c->pop();
          }
          else
          {
            ci->warn(m_templateName,m_line,"range %d %s %d is empty!",
                s,m_down?"downto":"to",e);
          }
        }
        else if (vs.type()!=TemplateVariant::Integer)
        {
          ci->warn(m_templateName,m_line,"range requires a start value of integer type!");
        }
        else if (ve.type()!=TemplateVariant::Integer)
        {
          ci->warn(m_templateName,m_line,"range requires an end value of integer type!");
        }
      }
      else if (!m_startExpr)
      {
        ci->warn(m_templateName,m_line,"range has empty start value");
      }
      else if (!m_endExpr)
      {
        ci->warn(m_templateName,m_line,"range has empty end value");
      }
    }

  private:
    bool m_down = false;
    ExprAst *m_startExpr = 0;
    ExprAst *m_endExpr = 0;
    QCString m_var;
    TemplateNodeList m_loopNodes;
};

//----------------------------------------------------------

/** @brief Class representing a 'for' tag in a template */
class TemplateNodeFor : public TemplateNodeCreator<TemplateNodeFor>
{
  public:
    TemplateNodeFor(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeFor>(parser,parent,line), m_reversed(FALSE)
    {
      TRACE(("{TemplateNodeFor(%s)\n",data.data()));
      QCString exprStr;
      int i = data.find(" in ");
      if (i==-1)
      {
        if (data.right(3)==" in")
        {
          parser->warn(m_templateName,line,"for is missing container after 'in' keyword");
        }
        else if (data=="in")
        {
          parser->warn(m_templateName,line,"for needs at least one iterator variable");
        }
        else
        {
          parser->warn(m_templateName,line,"for is missing 'in' keyword");
        }
      }
      else
      {
        m_vars = split(data.left(i),",");
        if (m_vars.size()==0)
        {
          parser->warn(m_templateName,line,"for needs at least one iterator variable");
        }

        int j = data.find(" reversed",i);
        m_reversed = (j!=-1);

        if (j==-1) j=data.length();
        if (j>i+4)
        {
          exprStr = data.mid(i+4,j-i-4); // skip over " in " part
        }
        if (exprStr.isEmpty())
        {
          parser->warn(m_templateName,line,"for is missing container after 'in' keyword");
        }
      }
      ExpressionParser expParser(parser,line);
      m_expr = expParser.parse(exprStr);

      StringVector stopAt = { "endfor", "empty" };
      parser->parse(this,line,stopAt,m_loopNodes);
      auto tok = parser->takeNextToken();
      if (tok && tok->data=="empty")
      {
        stopAt.pop_back();
        parser->parse(this,line,stopAt,m_emptyNodes);
        parser->removeNextToken(); // skip over endfor
      }
      TRACE(("}TemplateNodeFor(%s)\n",data.data()));
    }

    ~TemplateNodeFor()
    {
      delete m_expr;
    }

    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      //printf("TemplateNodeFor::render #loopNodes=%d #emptyNodes=%d\n",
      //    m_loopNodes.count(),m_emptyNodes.count());
      if (m_expr)
      {
        TemplateVariant v = m_expr->resolve(c);
        if (v.type()==TemplateVariant::Function)
        {
          v = v.call(std::vector<TemplateVariant>());
        }
        const TemplateListIntf *list = v.toList();
        if (list)
        {
          uint listSize = list->count();
          if (listSize==0) // empty for loop
          {
            m_emptyNodes.render(ts,c);
            return;
          }
          c->push();
          //int index = m_reversed ? list.count() : 0;
          //TemplateVariant v;
          const TemplateVariant *parentLoop = c->getRef("forloop");
          uint index = m_reversed ? listSize-1 : 0;
          TemplateListIntf::ConstIterator *it = list->createIterator();
          TemplateVariant ve;
          for (m_reversed ? it->toLast() : it->toFirst();
              (it->current(ve));
              m_reversed ? it->toPrev() : it->toNext())
          {
            TemplateAutoRef<TemplateStruct> s(TemplateStruct::alloc());
            s->set("counter0",    (int)index);
            s->set("counter",     (int)(index+1));
            s->set("revcounter",  (int)(listSize-index));
            s->set("revcounter0", (int)(listSize-index-1));
            s->set("first",index==0);
            s->set("last", index==listSize-1);
            s->set("parentloop",parentLoop ? *parentLoop : TemplateVariant());
            c->set("forloop",s.get());

            // add variables for this loop to the context
            //obj->addVariableToContext(index,m_vars,c);
            uint vi=0;
            if (m_vars.size()==1) // loop variable represents an item
            {
              c->set(m_vars[vi++],ve);
            }
            else if (m_vars.size()>1 && ve.type()==TemplateVariant::Struct)
              // loop variables represent elements in a list item
            {
              for (uint i=0;i<m_vars.size();i++,vi++)
              {
                c->set(m_vars[vi],ve.toStruct()->get(m_vars[vi]));
              }
            }
            for (;vi<m_vars.size();vi++)
            {
              c->set(m_vars[vi],TemplateVariant());
            }

            // render all items for this iteration of the loop
            m_loopNodes.render(ts,c);

            if (m_reversed) index--; else index++;
          }
          c->pop();
          delete it;
        }
        else // simple type...
        {
          ci->warn(m_templateName,m_line,"for requires a variable of list type, got type '%s'!",v.typeAsString().data());
        }
      }
    }

  private:
    bool m_reversed = false;
    ExprAst *m_expr = 0;
    std::vector<QCString> m_vars;
    TemplateNodeList m_loopNodes;
    TemplateNodeList m_emptyNodes;
};

//----------------------------------------------------------

/** @brief Class representing an 'markers' tag in a template */
class TemplateNodeMsg : public TemplateNodeCreator<TemplateNodeMsg>
{
  public:
    TemplateNodeMsg(TemplateParser *parser,TemplateNode *parent,int line,const QCString &)
      : TemplateNodeCreator<TemplateNodeMsg>(parser,parent,line)
    {
      TRACE(("{TemplateNodeMsg()\n"));
      StringVector stopAt = { "endmsg" };
      parser->parse(this,line,stopAt,m_nodes);
      parser->removeNextToken(); // skip over endmsg
      TRACE(("}TemplateNodeMsg()\n"));
    }
    void render(FTextStream &, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      TemplateEscapeIntf *escIntf = ci->escapeIntf();
      ci->setActiveEscapeIntf(0); // avoid escaping things we send to standard out
      bool enable = ci->spacelessEnabled();
      ci->enableSpaceless(FALSE);
      FTextStream ts(stdout);
      m_nodes.render(ts,c);
      ts << endl;
      ci->setActiveEscapeIntf(escIntf);
      ci->enableSpaceless(enable);
    }
  private:
    TemplateNodeList m_nodes;
};


//----------------------------------------------------------

/** @brief Class representing a 'block' tag in a template */
class TemplateNodeBlock : public TemplateNodeCreator<TemplateNodeBlock>
{
  public:
    TemplateNodeBlock(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeBlock>(parser,parent,line)
    {
      TRACE(("{TemplateNodeBlock(%s)\n",data.data()));
      m_blockName = data;
      if (m_blockName.isEmpty())
      {
        parser->warn(parser->templateName(),line,"block tag without name");
      }
      StringVector stopAt = { "endblock" };
      parser->parse(this,line,stopAt,m_nodes);
      parser->removeNextToken(); // skip over endblock
      TRACE(("}TemplateNodeBlock(%s)\n",data.data()));
    }

    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      TemplateImpl *t = getTemplate();
      if (t)
      {
        // remove block from the context, so block.super can work
        TemplateNodeBlock *nb = ci->blockContext()->pop(m_blockName);
        if (nb) // block is overruled
        {
          ci->push();
          QGString super;
          FTextStream ss(&super);
          // get super block of block nb
          TemplateNodeBlock *sb = ci->blockContext()->get(m_blockName);
          if (sb && sb!=nb && sb!=this) // nb and sb both overrule this block
          {
            sb->render(ss,c); // render parent of nb to string
          }
          else if (nb!=this) // only nb overrules this block
          {
            m_nodes.render(ss,c); // render parent of nb to string
          }
          // add 'block.super' variable to allow access to parent block content
          TemplateAutoRef<TemplateStruct> superBlock(TemplateStruct::alloc());
          superBlock->set("super",TemplateVariant(super.data(),TRUE));
          ci->set("block",superBlock.get());
          // render the overruled block contents
          t->engine()->enterBlock(nb->m_templateName,nb->m_blockName,nb->m_line);
          nb->m_nodes.render(ts,c);
          t->engine()->leaveBlock();
          ci->pop();
          // re-add block to the context
          ci->blockContext()->push(nb);
        }
        else // block has no overrule
        {
          t->engine()->enterBlock(m_templateName,m_blockName,m_line);
          m_nodes.render(ts,c);
          t->engine()->leaveBlock();
        }
      }
    }

    QCString name() const
    {
      return m_blockName;
    }

  private:
    QCString m_blockName;
    TemplateNodeList m_nodes;
};

//----------------------------------------------------------

/** @brief Class representing a 'extend' tag in a template */
class TemplateNodeExtend : public TemplateNodeCreator<TemplateNodeExtend>
{
  public:
    TemplateNodeExtend(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeExtend>(parser,parent,line)
    {
      TRACE(("{TemplateNodeExtend(%s)\n",data.data()));
      ExpressionParser ep(parser,line);
      if (data.isEmpty())
      {
        parser->warn(m_templateName,line,"extend tag is missing template file argument");
      }
      m_extendExpr = ep.parse(data);
      StringVector stopAt;
      parser->parse(this,line,stopAt,m_nodes);
      TRACE(("}TemplateNodeExtend(%s)\n",data.data()));
    }
   ~TemplateNodeExtend()
    {
      delete m_extendExpr;
    }

    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      if (m_extendExpr==0) return;

      QCString extendFile = m_extendExpr->resolve(c).toString();
      if (extendFile.isEmpty())
      {
        ci->warn(m_templateName,m_line,"invalid parameter for extend command");
      }

      // goto root of tree (template node)
      TemplateImpl *t = getTemplate();
      if (t)
      {
        Template *bt = t->engine()->loadByName(extendFile,m_line);
        TemplateImpl *baseTemplate = bt ? dynamic_cast<TemplateImpl*>(bt) : 0;
        if (baseTemplate)
        {
          // fill block context
          TemplateBlockContext *bc = ci->blockContext();

          // add overruling blocks to the context
          for (const auto &n : m_nodes)
          {
            TemplateNodeBlock *nb = dynamic_cast<TemplateNodeBlock*>(n.get());
            if (nb)
            {
              bc->add(nb);
            }
            TemplateNodeMsg *msg = dynamic_cast<TemplateNodeMsg*>(n.get());
            if (msg)
            {
              msg->render(ts,c);
            }
          }

          // render the base template with the given context
          baseTemplate->render(ts,c);

          // clean up
          bc->clear();
          t->engine()->unload(t);
        }
        else
        {
          ci->warn(m_templateName,m_line,"failed to load template %s for extend",extendFile.data());
        }
      }
    }

  private:
    ExprAst *m_extendExpr = 0;
    TemplateNodeList m_nodes;
};

/** @brief Class representing an 'include' tag in a template */
class TemplateNodeInclude : public TemplateNodeCreator<TemplateNodeInclude>
{
  public:
    TemplateNodeInclude(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeInclude>(parser,parent,line)
    {
      TRACE(("TemplateNodeInclude(%s)\n",data.data()));
      ExpressionParser ep(parser,line);
      if (data.isEmpty())
      {
        parser->warn(m_templateName,line,"include tag is missing template file argument");
      }
      m_includeExpr = ep.parse(data);
    }
   ~TemplateNodeInclude()
    {
      delete m_includeExpr;
    }
    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      if (m_includeExpr)
      {
        QCString includeFile = m_includeExpr->resolve(c).toString();
        if (includeFile.isEmpty())
        {
          ci->warn(m_templateName,m_line,"invalid parameter for include command\n");
        }
        else
        {
          TemplateImpl *t = getTemplate();
          if (t)
          {
            Template *it = t->engine()->loadByName(includeFile,m_line);
            TemplateImpl *incTemplate = it ? dynamic_cast<TemplateImpl*>(it) : 0;
            if (incTemplate)
            {
              incTemplate->render(ts,c);
              t->engine()->unload(t);
            }
            else
            {
              ci->warn(m_templateName,m_line,"failed to load template '%s' for include",includeFile.data()?includeFile.data():"");
            }
          }
        }
      }
    }

  private:
    ExprAst *m_includeExpr = 0;
};

//----------------------------------------------------------

static void stripLeadingWhiteSpace(QGString &s)
{
  const char *src = s.data();
  if (src)
  {
    char *dst = s.data();
    char c;
    bool skipSpaces=TRUE;
    while ((c=*src++))
    {
      if (c=='\n') { *dst++=c; skipSpaces=TRUE; }
      else if (c==' ' && skipSpaces) {}
      else { *dst++=c; skipSpaces=FALSE; }
    }
    *dst='\0';
  }
}

/** @brief Class representing an 'create' tag in a template */
class TemplateNodeCreate : public TemplateNodeCreator<TemplateNodeCreate>
{
  public:
    TemplateNodeCreate(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeCreate>(parser,parent,line), m_templateExpr(0), m_fileExpr(0)
    {
      TRACE(("TemplateNodeCreate(%s)\n",data.data()));
      if (data.isEmpty())
      {
        parser->warn(m_templateName,line,"create tag is missing arguments");
      }
      int i = data.find(" from ");
      if (i==-1)
      {
        if (data.right(3)==" from")
        {
          parser->warn(m_templateName,line,"create is missing template name after 'from' keyword");
        }
        else if (data=="from")
        {
          parser->warn(m_templateName,line,"create needs a file name and a template name");
        }
        else
        {
          parser->warn(m_templateName,line,"create is missing 'from' keyword");
        }
      }
      else
      {
        ExpressionParser ep(parser,line);
        m_fileExpr = ep.parse(data.left(i).stripWhiteSpace());
        m_templateExpr = ep.parse(data.mid(i+6).stripWhiteSpace());
      }
    }
   ~TemplateNodeCreate()
    {
      delete m_templateExpr;
      delete m_fileExpr;
    }
    void render(FTextStream &, TemplateContext *c)
    {
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      if (m_templateExpr && m_fileExpr)
      {
        QCString templateFile = m_templateExpr->resolve(c).toString();
        QCString outputFile = m_fileExpr->resolve(c).toString();
        if (templateFile.isEmpty())
        {
          ci->warn(m_templateName,m_line,"empty template name parameter for create command\n");
        }
        else if (outputFile.isEmpty())
        {
          ci->warn(m_templateName,m_line,"empty file name parameter for create command\n");
        }
        else
        {
          TemplateImpl *t = getTemplate();
          if (t)
          {
            QCString extension=outputFile;
            int i=extension.findRev('.');
            if (i!=-1)
            {
              extension=extension.right(extension.length()-i-1);
            }
            t->engine()->setOutputExtension(extension);
            Template *ct = t->engine()->loadByName(templateFile,m_line);
            TemplateImpl *createTemplate = ct ? dynamic_cast<TemplateImpl*>(ct) : 0;
            if (createTemplate)
            {
              mkpath(ci,outputFile);
              if (!ci->outputDirectory().isEmpty())
              {
                outputFile.prepend(ci->outputDirectory()+"/");
              }
              //printf("NoteCreate(%s)\n",outputFile.data());
              QFile f(outputFile);
              if (f.open(IO_WriteOnly))
              {
                TemplateEscapeIntf *escIntf = ci->escapeIntf();
                ci->selectEscapeIntf(extension);
                FTextStream ts(&f);
                QGString out;
                FTextStream os(&out);
                createTemplate->render(os,c);
                stripLeadingWhiteSpace(out);
                ts << out;
                t->engine()->unload(t);
                ci->setActiveEscapeIntf(escIntf);
              }
              else
              {
                ci->warn(m_templateName,m_line,"failed to open output file '%s' for create command",outputFile.data());
              }
            }
            else
            {
              ci->warn(m_templateName,m_line,"failed to load template '%s' for include",templateFile.data());
            }
            t->engine()->setOutputExtension("");
          }
        }
      }
    }

  private:
    ExprAst *m_templateExpr = 0;
    ExprAst *m_fileExpr = 0;
};

//----------------------------------------------------------

/** @brief Class representing an 'tree' tag in a template */
class TemplateNodeTree : public TemplateNodeCreator<TemplateNodeTree>
{
    struct TreeContext
    {
      TreeContext(TemplateNodeTree *o,const TemplateListIntf *l,TemplateContext *c)
        : object(o), list(l), templateCtx(c) {}
      TemplateNodeTree *object;
      const TemplateListIntf *list;
      TemplateContext  *templateCtx;
    };
  public:
    TemplateNodeTree(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeTree>(parser,parent,line)
    {
      TRACE(("{TemplateNodeTree(%s)\n",data.data()));
      ExpressionParser ep(parser,line);
      if (data.isEmpty())
      {
        parser->warn(m_templateName,line,"recursetree tag is missing data argument");
      }
      m_treeExpr = ep.parse(data);
      StringVector stopAt = { "endrecursetree" };
      parser->parse(this,line,stopAt,m_treeNodes);
      parser->removeNextToken(); // skip over endrecursetree
      TRACE(("}TemplateNodeTree(%s)\n",data.data()));
    }
    ~TemplateNodeTree()
    {
      delete m_treeExpr;
    }
    static TemplateVariant renderChildrenStub(const void *ctx, const std::vector<TemplateVariant> &)
    {
      return TemplateVariant(((TreeContext*)ctx)->object->
                             renderChildren((const TreeContext*)ctx),TRUE);
    }
    QCString renderChildren(const TreeContext *ctx)
    {
      //printf("TemplateNodeTree::renderChildren(%d)\n",ctx->list->count());
      // render all children of node to a string and return it
      TemplateContext *c = ctx->templateCtx;
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return QCString(); // should not happen
      QGString result;
      FTextStream ss(&result);
      c->push();
      TemplateVariant node;
      TemplateListIntf::ConstIterator *it = ctx->list->createIterator();
      for (it->toFirst();(it->current(node));it->toNext())
      {
        c->set("node",node);
        bool hasChildren=FALSE;
        const TemplateStructIntf *ns = node.toStruct();
        if (ns) // node is a struct
        {
          TemplateVariant v = ns->get("children");
          if (v.isValid()) // with a field 'children'
          {
            const TemplateListIntf *list = v.toList();
            if (list && list->count()>0) // non-empty list
            {
              TreeContext childCtx(this,list,ctx->templateCtx);
              TemplateVariant children(TemplateVariant::Delegate::fromFunction(&childCtx,renderChildrenStub));
              children.setRaw(TRUE);
              c->set("children",children);
              m_treeNodes.render(ss,c);
              hasChildren=TRUE;
            }
            else if (list==0)
            {
              ci->warn(m_templateName,m_line,"recursetree: children attribute has type '%s' instead of list\n",v.typeAsString().data());
            }
          }
          //else
          //{
          //  ci->warn(m_templateName,m_line,"recursetree: children attribute is not valid");
          //}
        }
        if (!hasChildren)
        {
          c->set("children",TemplateVariant("")); // provide default
          m_treeNodes.render(ss,c);
        }
      }
      c->pop();
      delete it;
      return result.data();
    }
    void render(FTextStream &ts, TemplateContext *c)
    {
      //printf("TemplateNodeTree::render()\n");
      TemplateContextImpl* ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      TemplateVariant v = m_treeExpr->resolve(c);
      const TemplateListIntf *list = v.toList();
      if (list)
      {
        TreeContext ctx(this,list,c);
        ts << renderChildren(&ctx);
      }
      else
      {
        ci->warn(m_templateName,m_line,"recursetree's argument should be a list type");
      }
    }

  private:
    ExprAst         *m_treeExpr = 0;
    TemplateNodeList m_treeNodes;
};

//----------------------------------------------------------

/** @brief Class representing an 'indexentry' tag in a template */
class TemplateNodeIndexEntry : public TemplateNodeCreator<TemplateNodeIndexEntry>
{
    struct Mapping
    {
      Mapping(const QCString &n,std::unique_ptr<ExprAst> &&e) : name(n), value(std::move(e)) {}
      QCString name;
      std::unique_ptr<ExprAst> value = 0;
    };
  public:
    TemplateNodeIndexEntry(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeIndexEntry>(parser,parent,line)
    {
      TRACE(("{TemplateNodeIndexEntry(%s)\n",data.data()));
      ExpressionParser expParser(parser,line);
      std::vector<QCString> args = split(data," ");
      auto it = args.begin();
      if (it==args.end() || (*it).find('=')!=-1)
      {
        parser->warn(parser->templateName(),line,"Missing name for indexentry tag");
      }
      else
      {
        m_name = *it;
        ++it;
        while (it!=args.end())
        {
          QCString arg = *it;
          int j=arg.find('=');
          if (j>0)
          {
            ExprAst *expr = expParser.parse(arg.mid(j+1));
            if (expr)
            {
              m_args.emplace_back(arg.left(j),std::unique_ptr<ExprAst>(expr));
            }
          }
          else
          {
            parser->warn(parser->templateName(),line,"invalid argument '%s' for indexentry tag",arg.data());
          }
          ++it;
        }
      }
      TRACE(("}TemplateNodeIndexEntry(%s)\n",data.data()));
    }
    void render(FTextStream &, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      if (!m_name.isEmpty())
      {
        ci->setLocation(m_templateName,m_line);
        std::vector<TemplateKeyValue> list;
        for (const auto &mapping : m_args)
        {
          list.emplace_back(mapping.name,mapping.value->resolve(c));
        }
        ci->addIndexEntry(m_name,list);
      }
    }
  private:
    QCString m_name;
    std::vector<Mapping> m_args;
};

//----------------------------------------------------------

/** @brief Class representing an 'opensubindex' tag in a template */
class TemplateNodeOpenSubIndex : public TemplateNodeCreator<TemplateNodeOpenSubIndex>
{
  public:
    TemplateNodeOpenSubIndex(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeOpenSubIndex>(parser,parent,line)
    {
      TRACE(("{TemplateNodeOpenSubIndex(%s)\n",data.data()));
      m_name = data.stripWhiteSpace();
      if (m_name.isEmpty())
      {
        parser->warn(parser->templateName(),line,"Missing argument for opensubindex tag");
      }
      else if (m_name.find(' ')!=-1)
      {
        parser->warn(parser->templateName(),line,"Expected single argument for opensubindex tag got '%s'",data.data());
        m_name="";
      }
      TRACE(("}TemplateNodeOpenSubIndex(%s)\n",data.data()));
    }
    void render(FTextStream &, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      if (!m_name.isEmpty())
      {
        ci->setLocation(m_templateName,m_line);
        ci->openSubIndex(m_name);
      }
    }
  private:
    QCString m_name;
};

//----------------------------------------------------------

/** @brief Class representing an 'closesubindex' tag in a template */
class TemplateNodeCloseSubIndex : public TemplateNodeCreator<TemplateNodeCloseSubIndex>
{
  public:
    TemplateNodeCloseSubIndex(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeCloseSubIndex>(parser,parent,line)
    {
      TRACE(("{TemplateNodeCloseSubIndex(%s)\n",data.data()));
      m_name = data.stripWhiteSpace();
      if (m_name.isEmpty())
      {
        parser->warn(parser->templateName(),line,"Missing argument for closesubindex tag");
      }
      else if (m_name.find(' ')!=-1 || m_name.isEmpty())
      {
        parser->warn(parser->templateName(),line,"Expected single argument for closesubindex tag got '%s'",data.data());
        m_name="";
      }
      TRACE(("}TemplateNodeCloseSubIndex(%s)\n",data.data()));
    }
    void render(FTextStream &, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      if (!m_name.isEmpty())
      {
        ci->setLocation(m_templateName,m_line);
        ci->closeSubIndex(m_name);
      }
    }
  private:
    QCString m_name;
};


//----------------------------------------------------------

/** @brief Class representing an 'with' tag in a template */
class TemplateNodeWith : public TemplateNodeCreator<TemplateNodeWith>
{
    struct Mapping
    {
      Mapping(const QCString &n,std::unique_ptr<ExprAst> &&e) : name(n), value(std::move(e)) {}
      QCString name;
      std::unique_ptr<ExprAst> value;
    };
  public:
    TemplateNodeWith(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeWith>(parser,parent,line)
    {
      TRACE(("{TemplateNodeWith(%s)\n",data.data()));
      ExpressionParser expParser(parser,line);
      QCString filteredData = removeSpacesAroundEquals(data);
      std::vector<QCString> args = split(filteredData," ");
      auto it = args.begin();
      while (it!=args.end())
      {
        QCString arg = *it;
        int j=arg.find('=');
        if (j>0)
        {
          ExprAst *expr = expParser.parse(arg.mid(j+1));
          if (expr)
          {
            m_args.emplace_back(arg.left(j),std::unique_ptr<ExprAst>(expr));
          }
        }
        else
        {
          parser->warn(parser->templateName(),line,"invalid argument '%s' for 'with' tag",arg.data());
        }
        ++it;
      }
      StringVector stopAt = { "endwith" };
      parser->parse(this,line,stopAt,m_nodes);
      parser->removeNextToken(); // skip over endwith
      TRACE(("}TemplateNodeWith(%s)\n",data.data()));
    }
    ~TemplateNodeWith()
    {
    }
    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      c->push();
      for (const auto &mapping : m_args)
      {
        TemplateVariant value = mapping.value->resolve(c);
        ci->set(mapping.name,value);
      }
      m_nodes.render(ts,c);
      c->pop();
    }
  private:
    TemplateNodeList m_nodes;
    std::vector<Mapping> m_args;
};

//----------------------------------------------------------

/** @brief Class representing an 'cycle' tag in a template */
class TemplateNodeCycle : public TemplateNodeCreator<TemplateNodeCycle>
{
  public:
    TemplateNodeCycle(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeCycle>(parser,parent,line)
    {
      TRACE(("{TemplateNodeCycle(%s)\n",data.data()));
      m_index=0;
      ExpressionParser expParser(parser,line);
      std::vector<QCString> args = split(data," ");
      auto it = args.begin();
      while (it!=args.end())
      {
        ExprAst *expr = expParser.parse(*it);
        if (expr)
        {
          m_args.push_back(std::unique_ptr<ExprAst>(expr));
        }
        ++it;
      }
      if (m_args.size()<2)
      {
          parser->warn(parser->templateName(),line,"expected at least two arguments for cycle command, got %zu",m_args.size());
      }
      TRACE(("}TemplateNodeCycle(%s)\n",data.data()));
    }
    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      ci->setLocation(m_templateName,m_line);
      if (m_index<m_args.size())
      {
        TemplateVariant v = m_args[m_index]->resolve(c);
        if (v.type()==TemplateVariant::Function)
        {
          v = v.call(std::vector<TemplateVariant>());
        }
        if (ci->escapeIntf() && !v.raw())
        {
          if (ci->needsRecoding())
          {
            ts << ci->recode(ci->escapeIntf()->escape(v.toString()));
          }
          else
          {
            ts << ci->escapeIntf()->escape(v.toString());
          }
        }
        else
        {
          if (ci->needsRecoding())
          {
            ts << ci->recode(v.toString());
          }
          else
          {
            ts << v.toString();
          }
        }
      }
      if (++m_index==m_args.size()) // wrap around
      {
        m_index=0;
      }
    }
  private:
    size_t m_index = 0;
    ExprAstList m_args;
};

//----------------------------------------------------------

/** @brief Class representing an 'set' tag in a template */
class TemplateNodeSet : public TemplateNodeCreator<TemplateNodeSet>
{
    struct Mapping
    {
      Mapping(const QCString &n,ExprAst *e) : name(n), value(e) {}
     ~Mapping() { }
      QCString name;
      ExprAst *value = 0;
    };
  public:
    TemplateNodeSet(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeSet>(parser,parent,line)
    {
      TRACE(("{TemplateNodeSet(%s)\n",data.data()));
      ExpressionParser expParser(parser,line);
      // data format: name=expression
      int j=data.find('=');
      ExprAst *expr = 0;
      if (j>0 && (expr = expParser.parse(data.mid(j+1))))
      {
        m_mapping = std::make_unique<Mapping>(data.left(j),expr);
      }
      TRACE(("}TemplateNodeSet(%s)\n",data.data()));
    }
    ~TemplateNodeSet()
    {
    }
    void render(FTextStream &, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      if (m_mapping)
      {
        TemplateVariant value = m_mapping->value->resolve(c);
        ci->set(m_mapping->name,value);
      }
    }
  private:
    std::unique_ptr<Mapping> m_mapping;
};

//----------------------------------------------------------

/** @brief Class representing an 'spaceless' tag in a template */
class TemplateNodeSpaceless : public TemplateNodeCreator<TemplateNodeSpaceless>
{
  public:
    TemplateNodeSpaceless(TemplateParser *parser,TemplateNode *parent,int line,const QCString &)
      : TemplateNodeCreator<TemplateNodeSpaceless>(parser,parent,line)
    {
      TRACE(("{TemplateNodeSpaceless()\n"));
      StringVector stopAt = { "endspaceless" };
      parser->parse(this,line,stopAt,m_nodes);
      parser->removeNextToken(); // skip over endwith
      TRACE(("}TemplateNodeSpaceless()\n"));
    }
    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      bool wasSpaceless = ci->spacelessEnabled();
      ci->enableSpaceless(TRUE);
      m_nodes.render(ts,c);
      ci->enableSpaceless(wasSpaceless);
    }
  private:
    TemplateNodeList m_nodes;
};

//----------------------------------------------------------

/** @brief Class representing an 'markers' tag in a template */
class TemplateNodeMarkers : public TemplateNodeCreator<TemplateNodeMarkers>
{
  public:
    TemplateNodeMarkers(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeMarkers>(parser,parent,line), m_listExpr(0), m_patternExpr(0)
    {
      TRACE(("{TemplateNodeMarkers(%s)\n",data.data()));
      int i = data.find(" in ");
      int w = data.find(" with ");
      if (i==-1 || w==-1 || w<i)
      {
        parser->warn(m_templateName,line,"markers tag as wrong format. Expected: markers <var> in <list> with <string_with_markers>");
      }
      else
      {
        ExpressionParser expParser(parser,line);
        m_var = data.left(i);
        m_listExpr = expParser.parse(data.mid(i+4,w-i-4));
        m_patternExpr = expParser.parse(data.right(data.length()-w-6));
      }
      StringVector stopAt = { "endmarkers" };
      parser->parse(this,line,stopAt,m_nodes);
      parser->removeNextToken(); // skip over endmarkers
      TRACE(("}TemplateNodeMarkers(%s)\n",data.data()));
    }
   ~TemplateNodeMarkers()
    {
      delete m_listExpr;
      delete m_patternExpr;
    }
    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      if (!m_var.isEmpty() && m_listExpr && m_patternExpr)
      {
        TemplateVariant v = m_listExpr->resolve(c);
        const TemplateListIntf *list = v.toList();
        TemplateVariant patternStr = m_patternExpr->resolve(c);
        if (list)
        {
          if (patternStr.type()==TemplateVariant::String)
          {
            TemplateListIntf::ConstIterator *it = list->createIterator();
            c->push();
            std::string str = patternStr.toString().str();

            static const reg::Ex marker(R"(@\d+)");
            reg::Iterator re_it(str,marker);
            reg::Iterator end;
            size_t index=0;
            for ( ; re_it!=end ; ++re_it)
            {
              const auto &match = *re_it;
              size_t newIndex = match.position();
              size_t matchLen = match.length();
              std::string part = str.substr(index,newIndex-index);
              if (ci->needsRecoding())
              {
                ts << ci->recode(part); // write text before marker
              }
              else
              {
                ts << part; // write text before marker
              }
              unsigned long entryIndex = std::stoul(match.str().substr(1));
              TemplateVariant var;
              size_t i=0;
              // search for list element at position id
              for (it->toFirst(); (it->current(var)) && i<entryIndex; it->toNext(),i++) {}
              if (i==entryIndex) // found element
              {
                TemplateAutoRef<TemplateStruct> s(TemplateStruct::alloc());
                s->set("id",(int)i);
                c->set("markers",s.get());
                c->set(m_var,var); // define local variable to hold element of list type
                bool wasSpaceless = ci->spacelessEnabled();
                ci->enableSpaceless(TRUE);
                m_nodes.render(ts,c);
                ci->enableSpaceless(wasSpaceless);
              }
              else if (i<entryIndex)
              {
                ci->warn(m_templateName,m_line,"markers list does not an element for marker position %d",i);
              }
              index=newIndex+matchLen; // set index just after marker
            }
            if (ci->needsRecoding())
            {
              ts << ci->recode(str.substr(index)); // write text after last marker
            }
            else
            {
              ts << str.substr(index); // write text after last marker
            }
            c->pop();
            delete it;
          }
          else
          {
            ci->warn(m_templateName,m_line,"markers requires a parameter of string type after 'with'!");
          }
        }
        else
        {
          ci->warn(m_templateName,m_line,"markers requires a parameter of list type after 'in'!");
        }
      }
    }
  private:
    TemplateNodeList m_nodes;
    QCString m_var;
    ExprAst *m_listExpr = 0;
    ExprAst *m_patternExpr = 0;
};

//----------------------------------------------------------

/** @brief Class representing an 'tabbing' tag in a template */
class TemplateNodeTabbing : public TemplateNodeCreator<TemplateNodeTabbing>
{
  public:
    TemplateNodeTabbing(TemplateParser *parser,TemplateNode *parent,int line,const QCString &)
      : TemplateNodeCreator<TemplateNodeTabbing>(parser,parent,line)
    {
      TRACE(("{TemplateNodeTabbing()\n"));
      StringVector stopAt = { "endtabbing" };
      parser->parse(this,line,stopAt,m_nodes);
      parser->removeNextToken(); // skip over endtabbing
      TRACE(("}TemplateNodeTabbing()\n"));
    }
    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      bool wasTabbing = ci->tabbingEnabled();
      ci->enableTabbing(TRUE);
      m_nodes.render(ts,c);
      ci->enableTabbing(wasTabbing);
    }
  private:
    TemplateNodeList m_nodes;
};

//----------------------------------------------------------

/** @brief Class representing an 'markers' tag in a template */
class TemplateNodeResource : public TemplateNodeCreator<TemplateNodeResource>
{
  public:
    TemplateNodeResource(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeResource>(parser,parent,line)
    {
      TRACE(("{TemplateNodeResource(%s)\n",data.data()));
      ExpressionParser ep(parser,line);
      int i;
      if (data.isEmpty())
      {
        parser->warn(m_templateName,line,"resource tag is missing resource file argument");
        m_resExpr=0;
        m_asExpr=0;
      }
      else if ((i=data.find(" as "))!=-1) // resource a as b
      {
        m_resExpr = ep.parse(data.left(i));  // part before as
        m_asExpr  = ep.parse(data.mid(i+4)); // part after as
      }
      else // resource a
      {
        m_resExpr = ep.parse(data);
        m_asExpr  = 0;
      }
      TRACE(("}TemplateNodeResource(%s)\n",data.data()));
    }
    ~TemplateNodeResource()
    {
      delete m_resExpr;
      delete m_asExpr;
    }
    void render(FTextStream &, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      if (m_resExpr)
      {
        QCString resourceFile = m_resExpr->resolve(c).toString();
        if (resourceFile.isEmpty())
        {
          ci->warn(m_templateName,m_line,"invalid parameter for resource command\n");
        }
        else
        {
          QCString outputDirectory = ci->outputDirectory();
          if (m_asExpr)
          {
            QCString targetFile = m_asExpr->resolve(c).toString();
            mkpath(ci,targetFile);
            if (targetFile.isEmpty())
            { ci->warn(m_templateName,m_line,"invalid parameter at right side of 'as' for resource command\n");
            }
            else
            {
              ResourceMgr::instance().copyResourceAs(resourceFile,outputDirectory,targetFile);
            }
          }
          else
          {
            ResourceMgr::instance().copyResource(resourceFile,outputDirectory);
          }
        }
      }
    }
  private:
    ExprAst *m_resExpr = 0;
    ExprAst *m_asExpr = 0;
};

//----------------------------------------------------------

/** @brief Class representing the 'encoding' tag in a template */
class TemplateNodeEncoding : public TemplateNodeCreator<TemplateNodeEncoding>
{
  public:
    TemplateNodeEncoding(TemplateParser *parser,TemplateNode *parent,int line,const QCString &data)
      : TemplateNodeCreator<TemplateNodeEncoding>(parser,parent,line)
    {
      TRACE(("{TemplateNodeEncoding(%s)\n",data.data()));
      ExpressionParser ep(parser,line);
      if (data.isEmpty())
      {
        parser->warn(m_templateName,line,"encoding tag is missing encoding argument");
        m_encExpr = 0;
      }
      else
      {
        m_encExpr = ep.parse(data);
      }
      StringVector stopAt = { "endencoding" };
      parser->parse(this,line,stopAt,m_nodes);
      parser->removeNextToken(); // skip over endencoding
      TRACE(("}TemplateNodeEncoding(%s)\n",data.data()));
    }
   ~TemplateNodeEncoding()
    {
      delete m_encExpr;
    }
    void render(FTextStream &ts, TemplateContext *c)
    {
      TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
      if (ci==0) return; // should not happen
      ci->setLocation(m_templateName,m_line);
      QCString encStr;
      if (m_encExpr)
      {
        encStr = m_encExpr->resolve(c).toString();
      }
      QCString oldEncStr = ci->encoding();
      if (!encStr.isEmpty())
      {
        ci->setEncoding(m_templateName,m_line,encStr);
      }
      m_nodes.render(ts,c);
      ci->setEncoding(m_templateName,m_line,oldEncStr);
    }
  private:
    ExprAst *m_encExpr;
    TemplateNodeList m_nodes;
};

//----------------------------------------------------------

/** @brief Factory class for creating tag AST nodes found in a template */
class TemplateNodeFactory
{
  public:
    typedef TemplateNode *(*CreateFunc)(TemplateParser *parser,
                                        TemplateNode *parent,
                                        int line,
                                        const QCString &data);

    static TemplateNodeFactory *instance()
    {
      static TemplateNodeFactory *instance = 0;
      if (instance==0) instance = new TemplateNodeFactory;
      return instance;
    }

    TemplateNode *create(const QCString &name,
                         TemplateParser *parser,
                         TemplateNode *parent,
                         int line,
                         const QCString &data)
    {
      auto it = m_registry.find(name.str());
      if (it==m_registry.end()) return 0;
      return it->second(parser,parent,line,data);
    }

    void registerTemplateNode(const QCString &name,CreateFunc func)
    {
      m_registry.insert(std::make_pair(name.str(),func));
    }

    /** @brief Helper class for registering a template AST node */
    template<class T> class AutoRegister
    {
      public:
        AutoRegister<T>(const QCString &key)
        {
          TemplateNodeFactory::instance()->registerTemplateNode(key,T::createInstance);
        }
    };

  private:
    std::unordered_map<std::string,CreateFunc> m_registry;
};

// register a handler for each start tag we support
static TemplateNodeFactory::AutoRegister<TemplateNodeIf>            autoRefIf("if");
static TemplateNodeFactory::AutoRegister<TemplateNodeFor>           autoRefFor("for");
static TemplateNodeFactory::AutoRegister<TemplateNodeMsg>           autoRefMsg("msg");
static TemplateNodeFactory::AutoRegister<TemplateNodeSet>           autoRefSet("set");
static TemplateNodeFactory::AutoRegister<TemplateNodeTree>          autoRefTree("recursetree");
static TemplateNodeFactory::AutoRegister<TemplateNodeWith>          autoRefWith("with");
static TemplateNodeFactory::AutoRegister<TemplateNodeBlock>         autoRefBlock("block");
static TemplateNodeFactory::AutoRegister<TemplateNodeCycle>         autoRefCycle("cycle");
static TemplateNodeFactory::AutoRegister<TemplateNodeRange>         autoRefRange("range");
static TemplateNodeFactory::AutoRegister<TemplateNodeExtend>        autoRefExtend("extend");
static TemplateNodeFactory::AutoRegister<TemplateNodeCreate>        autoRefCreate("create");
static TemplateNodeFactory::AutoRegister<TemplateNodeRepeat>        autoRefRepeat("repeat");
static TemplateNodeFactory::AutoRegister<TemplateNodeInclude>       autoRefInclude("include");
static TemplateNodeFactory::AutoRegister<TemplateNodeMarkers>       autoRefMarkers("markers");
static TemplateNodeFactory::AutoRegister<TemplateNodeTabbing>       autoRefTabbing("tabbing");
static TemplateNodeFactory::AutoRegister<TemplateNodeResource>      autoRefResource("resource");
static TemplateNodeFactory::AutoRegister<TemplateNodeEncoding>      autoRefEncoding("encoding");
static TemplateNodeFactory::AutoRegister<TemplateNodeSpaceless>     autoRefSpaceless("spaceless");
static TemplateNodeFactory::AutoRegister<TemplateNodeIndexEntry>    autoRefIndexEntry("indexentry");
static TemplateNodeFactory::AutoRegister<TemplateNodeOpenSubIndex>  autoRefOpenSubIndex("opensubindex");
static TemplateNodeFactory::AutoRegister<TemplateNodeCloseSubIndex> autoRefCloseSubIndex("closesubindex");

//----------------------------------------------------------

TemplateBlockContext::TemplateBlockContext()
{
}

TemplateNodeBlock *TemplateBlockContext::get(const QCString &name) const
{
  auto it = m_blocks.find(name.str());
  if (it==m_blocks.end() || it->second.empty())
  {
    return 0;
  }
  else
  {
    return it->second.back();
  }
}

TemplateNodeBlock *TemplateBlockContext::pop(const QCString &name)
{
  auto it = m_blocks.find(name.str());
  if (it==m_blocks.end() || it->second.empty())
  {
    return 0;
  }
  else
  {
    TemplateNodeBlock *bld = it->second.back();
    it->second.pop_back();
    return bld;
  }
}

void TemplateBlockContext::add(TemplateNodeBlock *block)
{
  auto it = m_blocks.find(block->name().str());
  if (it==m_blocks.end())
  {
    it = m_blocks.insert(std::make_pair(block->name().str(),NodeBlockList())).first;
  }
  it->second.push_front(block);
}

void TemplateBlockContext::add(TemplateBlockContext *ctx)
{
  for (auto &kv : ctx->m_blocks)
  {
    for (auto &nb : kv.second)
    {
      add(nb);
    }
  }
}

void TemplateBlockContext::clear()
{
  m_blocks.clear();
}

void TemplateBlockContext::push(TemplateNodeBlock *block)
{
  auto it = m_blocks.find(block->name().str());
  if (it==m_blocks.end())
  {
    it = m_blocks.insert(std::make_pair(block->name().str(),NodeBlockList())).first;
  }
  it->second.push_back(block);
}


//----------------------------------------------------------

/** @brief Lexer class for turning a template into a list of tokens */
class TemplateLexer
{
  public:
    TemplateLexer(const TemplateEngine *engine,const QCString &fileName,const QCString &data);
    void tokenize(TemplateTokenStream &tokens);
    void setOpenCloseCharacters(char openChar,char closeChar)
    { m_openChar=openChar; m_closeChar=closeChar; }
  private:
    void addToken(TemplateTokenStream &tokens,
                  const char *data,int line,int startPos,int endPos,
                  TemplateToken::Type type);
    void reset();
    const TemplateEngine *m_engine = 0;
    QCString m_fileName;
    QCString m_data;
    char m_openChar = 0;
    char m_closeChar = 0;
};

TemplateLexer::TemplateLexer(const TemplateEngine *engine,const QCString &fileName,const QCString &data) :
  m_engine(engine), m_fileName(fileName), m_data(data)
{
  m_openChar='{';
  m_closeChar='}';
}

void TemplateLexer::tokenize(TemplateTokenStream &tokens)
{
  enum LexerStates
  {
    StateText,
    StateBeginTemplate,
    StateTag,
    StateEndTag,
    StateComment,
    StateEndComment,
    StateMaybeVar,
    StateVariable,
    StateEndVariable
  };

  const char *p=m_data.data();
  if (p==0) return;
  int  state=StateText;
  int  pos=0;
  int  lastTokenPos=0;
  int  startLinePos=0;
  bool emptyOutputLine=TRUE;
  int  line=1;
  char c;
  int  markStartPos=-1;
  for (;(c=*p);p++,pos++)
  {
    switch (state)
    {
      case StateText:
        if (c==m_openChar) // {{ or {% or {# or something else
        {
          state=StateBeginTemplate;
        }
        else if (c!=' ' && c!='\t' && c!='\n') // non-whitespace text
        {
          emptyOutputLine=FALSE;
        }
        break;
      case StateBeginTemplate:
        switch (c)
        {
          case '%': // {%
            state=StateTag;
            markStartPos=pos-1;
            break;
          case '#': // {#
            state=StateComment;
            markStartPos=pos-1;
            break;
          case '{': // {{
            if (m_openChar=='{')
            {
              state=StateMaybeVar;
            }
            else
            {
              state=StateVariable;
            }
            markStartPos=pos-1;
            break;
          default:
            state=StateText;
            emptyOutputLine=FALSE;
            break;
        }
        break;
      case StateTag:
        if (c=='\n')
        {
          warn(m_fileName,line,"unexpected new line inside %c%%...%%%c block",m_openChar,m_closeChar);
          m_engine->printIncludeContext(m_fileName,line);
        }
        else if (c=='%') // %} or something else
        {
          state=StateEndTag;
        }
        break;
      case StateEndTag:
        if (c==m_closeChar) // %}
        {
          // found tag!
          state=StateText;
          addToken(tokens,m_data.data(),line,lastTokenPos,
                   emptyOutputLine ? startLinePos : markStartPos,
                   TemplateToken::Text);
          addToken(tokens,m_data.data(),line,markStartPos+2,
                   pos-1,TemplateToken::Block);
          lastTokenPos = pos+1;
        }
        else // something else
        {
          if (c=='\n')
          {
            warn(m_fileName,line,"unexpected new line inside %c%%...%%%c block",m_openChar,m_closeChar);
            m_engine->printIncludeContext(m_fileName,line);
          }
          state=StateTag;
        }
        break;
      case StateComment:
        if (c=='\n')
        {
          warn(m_fileName,line,"unexpected new line inside %c#...#%c block",m_openChar,m_closeChar);
          m_engine->printIncludeContext(m_fileName,line);
        }
        else if (c=='#') // #} or something else
        {
          state=StateEndComment;
        }
        break;
      case StateEndComment:
        if (c==m_closeChar) // #}
        {
          // found comment tag!
          state=StateText;
          addToken(tokens,m_data.data(),line,lastTokenPos,
                   emptyOutputLine ? startLinePos : markStartPos,
                   TemplateToken::Text);
          lastTokenPos = pos+1;
        }
        else // something else
        {
          if (c=='\n')
          {
            warn(m_fileName,line,"unexpected new line inside %c#...#%c block",m_openChar,m_closeChar);
            m_engine->printIncludeContext(m_fileName,line);
          }
          state=StateComment;
        }
        break;
      case StateMaybeVar:
        switch (c)
        {
          case '#': // {{#
            state=StateComment;
            markStartPos=pos-1;
            break;
          case '%': // {{%
            state=StateTag;
            markStartPos=pos-1;
            break;
          default:  // {{
            state=StateVariable;
            break;
        }
        break;
      case StateVariable:
        emptyOutputLine=FALSE; // assume a variable expands to content
        if (c=='\n')
        {
          warn(m_fileName,line,"unexpected new line inside %c{...}%c block",m_openChar,m_closeChar);
          m_engine->printIncludeContext(m_fileName,line);
        }
        else if (c=='}') // }} or something else
        {
          state=StateEndVariable;
        }
        break;
      case StateEndVariable:
        if (c==m_closeChar) // }}
        {
          // found variable tag!
          state=StateText;
          addToken(tokens,m_data.data(),line,lastTokenPos,
                   emptyOutputLine ? startLinePos : markStartPos,
                   TemplateToken::Text);
          addToken(tokens,m_data.data(),line,markStartPos+2,
                   pos-1,TemplateToken::Variable);
          lastTokenPos = pos+1;
        }
        else // something else
        {
          if (c=='\n')
          {
            warn(m_fileName,line,"unexpected new line inside %c{...}%c block",m_openChar,m_closeChar);
            m_engine->printIncludeContext(m_fileName,line);
          }
          state=StateVariable;
        }
        break;
    }
    if (c=='\n') // new line
    {
      state=StateText;
      startLinePos=pos+1;
      // if the current line only contain commands and whitespace,
      // then skip it in the output by moving lastTokenPos
      if (markStartPos!=-1 && emptyOutputLine) lastTokenPos = startLinePos;
      // reset markers
      markStartPos=-1;
      line++;
      emptyOutputLine=TRUE;
    }
  }
  if (lastTokenPos<pos)
  {
    addToken(tokens,m_data.data(),line,
             lastTokenPos,pos,
             TemplateToken::Text);
  }
}

void TemplateLexer::addToken(TemplateTokenStream &tokens,
                             const char *data,int line,
                             int startPos,int endPos,
                             TemplateToken::Type type)
{
  if (startPos<endPos)
  {
    int len = endPos-startPos+1;
    QCString text(len);
    qstrncpy(text.rawData(),data+startPos,len);
    if (type!=TemplateToken::Text) text = text.stripWhiteSpace();
    tokens.push_back(std::make_unique<TemplateToken>(type,text,line));
  }
}

//----------------------------------------------------------

TemplateParser::TemplateParser(const TemplateEngine *engine,
                               const QCString &templateName,
                               TemplateTokenStream &tokens) :
   m_engine(engine), m_templateName(templateName), m_tokens(tokens)
{
}

void TemplateParser::parse(
                     TemplateNode *parent,int line,const StringVector &stopAt,
                     TemplateNodeList &nodes)
{
  TRACE(("{TemplateParser::parse\n"));
  // process the tokens. Build node list
  while (hasNextToken())
  {
    auto tok = takeNextToken();
    //printf("%p:Token type=%d data='%s' line=%d\n",
    //       parent,tok->type,tok->data.data(),tok->line);
    switch(tok->type)
    {
      case TemplateToken::Text:
        nodes.push_back(std::make_unique<TemplateNodeText>(this,parent,tok->line,tok->data));
        break;
      case TemplateToken::Variable: // {{ var }}
        nodes.push_back(std::make_unique<TemplateNodeVariable>(this,parent,tok->line,tok->data));
        break;
      case TemplateToken::Block:    // {% tag %}
        {
          QCString command = tok->data;
          int sep = command.find(' ');
          if (sep!=-1)
          {
            command=command.left(sep);
          }
          TemplateToken *tok_ptr = tok.get();
          if (std::find(stopAt.begin(),stopAt.end(),command)!=stopAt.end())
          {
            prependToken(std::move(tok));
            TRACE(("}TemplateParser::parse: stop\n"));
            return;
          }
          QCString arg;
          if (sep!=-1)
          {
            arg = tok_ptr->data.mid(sep+1);
          }
          std::unique_ptr<TemplateNode> node { TemplateNodeFactory::instance()->
                               create(command,this,parent,tok_ptr->line,arg) };
          if (node)
          {
            nodes.push_back(std::move(node));
          }
          else if (command=="empty"          || command=="else"         ||
                   command=="endif"          || command=="endfor"       ||
                   command=="endblock"       || command=="endwith"      ||
                   command=="endrecursetree" || command=="endspaceless" ||
                   command=="endmarkers"     || command=="endmsg"       ||
                   command=="endrepeat"      || command=="elif"         ||
                   command=="endrange"       || command=="endtabbing"   ||
                   command=="endencoding")
          {
            warn(m_templateName,tok_ptr->line,"Found tag '%s' without matching start tag",command.data());
          }
          else
          {
            warn(m_templateName,tok_ptr->line,"Unknown tag '%s'",command.data());
          }
        }
        break;
    }
  }
  if (!stopAt.empty())
  {
    QCString options;
    for (const auto &s : stopAt)
    {
      if (!options.isEmpty()) options+=", ";
      options+=s.c_str();
    }
    warn(m_templateName,line,"Unclosed tag in template, expected one of: %s",
        options.data());
  }
  TRACE(("}TemplateParser::parse: last token\n"));
}

bool TemplateParser::hasNextToken() const
{
  return !m_tokens.empty();
}

TemplateTokenPtr TemplateParser::takeNextToken()
{
  if (m_tokens.empty()) return TemplateTokenPtr();
  auto tok = std::move(m_tokens.front());
  m_tokens.pop_front();
  return tok;
}

const TemplateToken *TemplateParser::currentToken() const
{
  return m_tokens.front().get();
}

void TemplateParser::removeNextToken()
{
  m_tokens.pop_front();
}

void TemplateParser::prependToken(TemplateTokenPtr &&token)
{
  m_tokens.push_front(std::move(token));
}

void TemplateParser::warn(const char *fileName,int line,const char *fmt,...) const
{
  va_list args;
  va_start(args,fmt);
  va_warn(fileName,line,fmt,args);
  va_end(args);
  m_engine->printIncludeContext(fileName,line);
}



//----------------------------------------------------------


TemplateImpl::TemplateImpl(TemplateEngine *engine,const QCString &name,const QCString &data,
    const QCString &extension)
  : TemplateNode(0)
{
  m_name = name;
  m_engine = engine;
  TemplateLexer lexer(engine,name,data);
  if (extension=="tex")
  {
    lexer.setOpenCloseCharacters('<','>');
  }
  TemplateTokenStream tokens;
  lexer.tokenize(tokens);
  TemplateParser parser(engine,name,tokens);
  parser.parse(this,1,StringVector(),m_nodes);
}

TemplateImpl::~TemplateImpl()
{
  //printf("deleting template %s\n",m_name.data());
}

void TemplateImpl::render(FTextStream &ts, TemplateContext *c)
{
  TemplateContextImpl *ci = dynamic_cast<TemplateContextImpl*>(c);
  if (ci==0) return; // should not happen
  if (!m_nodes.empty())
  {
    TemplateNodeExtend *ne = dynamic_cast<TemplateNodeExtend*>(m_nodes.front().get());
    if (ne==0) // normal template, add blocks to block context
    {
      TemplateBlockContext *bc = ci->blockContext();
      for (const auto &n : m_nodes)
      {
        TemplateNodeBlock *nb = dynamic_cast<TemplateNodeBlock*>(n.get());
        if (nb)
        {
          bc->add(nb);
        }
      }
    }
    m_nodes.render(ts,c);
  }
}

//----------------------------------------------------------

/** @brief Private data of the template engine */
class TemplateEngine::Private
{
    class IncludeEntry
    {
      public:
        enum Type { Template, Block };
        IncludeEntry(Type type,const QCString &fileName,const QCString &blockName,int line)
          : m_type(type), m_fileName(fileName), m_blockName(blockName), m_line(line) {}
        Type type() const { return m_type; }
        QCString fileName() const { return m_fileName; }
        QCString blockName() const { return m_blockName; }
        int line() const { return m_line; }

      private:
        Type m_type = Template;
        QCString m_fileName;
        QCString m_blockName;
        int m_line = 0;
    };
  public:
    Private(TemplateEngine *engine) : m_engine(engine)
    {
    }
    Template *loadByName(const QCString &fileName,int line)
    {
      //for (int i=0;i<m_indent;i++) printf("  ");
      //m_indent++;
      //printf("loadByName(%s,%d) {\n",fileName.data(),line);
      m_includeStack.emplace_back(IncludeEntry::Template,fileName,QCString(),line);
      auto kv = m_templateCache.find(fileName.str());
      if (kv==m_templateCache.end()) // first time template is referenced
      {
        QCString filePath = m_templateDirName+"/"+fileName;
        QFile f(filePath);
        if (f.open(IO_ReadOnly)) // read template from disk
        {
          QFileInfo fi(filePath);
          int size=fi.size();
          QCString data(size+1);
          if (f.readBlock(data.rawData(),size)==size)
          {
            kv = m_templateCache.insert(
                std::make_pair(fileName.str(),
                  std::make_unique<TemplateImpl>(m_engine,filePath,data,m_extension))).first;
          }
        }
        else // fallback to default built-in template
        {
          const QCString data = ResourceMgr::instance().getAsString(fileName);
          if (!data.isEmpty())
          {
            kv = m_templateCache.insert(
                std::make_pair(fileName.str(),
                  std::make_unique<TemplateImpl>(m_engine,fileName,data,m_extension))).first;
          }
          else
          {
            err("Could not open template file %s\n",fileName.data());
          }
        }
      }
      return kv!=m_templateCache.end() ? kv->second.get() : 0;
    }

    void unload(Template * /*t*/)
    {
      //(void)t;
      //m_indent--;
      //for (int i=0;i<m_indent;i++) printf("  ");
      //printf("}\n");
      m_includeStack.pop_back();
    }

    void enterBlock(const QCString &fileName,const QCString &blockName,int line)
    {
      //for (int i=0;i<m_indent;i++) printf("  ");
      //m_indent++;
      //printf("enterBlock(%s,%s,%d) {\n",fileName.data(),blockName.data(),line);
      m_includeStack.emplace_back(IncludeEntry::Block,fileName,blockName,line);
    }

    void leaveBlock()
    {
      //m_indent--;
      //for (int i=0;i<m_indent;i++) printf("  ");
      //printf("}\n");
      m_includeStack.pop_back();
    }

    void printIncludeContext(const char *fileName,int line) const
    {
      auto it = m_includeStack.rbegin();
      while (it!=m_includeStack.rend())
      {
        const IncludeEntry &ie = *it;
        ++it;
        const IncludeEntry *next = it!=m_includeStack.rend() ? &(*it) : 0;
        if (ie.type()==IncludeEntry::Template)
        {
          if (next)
          {
            warn(fileName,line,"  inside template '%s' included from template '%s' at line %d",ie.fileName().data(),next->fileName().data(),ie.line());
          }
        }
        else // ie.type()==IncludeEntry::Block
        {
          warn(fileName,line,"  included by block '%s' inside template '%s' at line %d",ie.blockName().data(),
              ie.fileName().data(),ie.line());
        }
      }
    }

    void setOutputExtension(const char *extension)
    {
      m_extension = extension;
    }

    QCString outputExtension() const
    {
      return m_extension;
    }

    void setTemplateDir(const char *dirName)
    {
      m_templateDirName = dirName;
    }

  private:
    std::unordered_map< std::string, std::unique_ptr<Template> > m_templateCache;
    //mutable int m_indent;
    TemplateEngine *m_engine = 0;
    std::vector<IncludeEntry> m_includeStack;
    QCString m_extension;
    QCString m_templateDirName;
};

TemplateEngine::TemplateEngine()
{
  p = new Private(this);
}

TemplateEngine::~TemplateEngine()
{
  delete p;
}

TemplateContext *TemplateEngine::createContext() const
{
  return new TemplateContextImpl(this);
}

void TemplateEngine::destroyContext(TemplateContext *ctx)
{
  delete ctx;
}

Template *TemplateEngine::loadByName(const QCString &fileName,int line)
{
  return p->loadByName(fileName,line);
}

void TemplateEngine::unload(Template *t)
{
  p->unload(t);
}

void TemplateEngine::enterBlock(const QCString &fileName,const QCString &blockName,int line)
{
  p->enterBlock(fileName,blockName,line);
}

void TemplateEngine::leaveBlock()
{
  p->leaveBlock();
}

void TemplateEngine::printIncludeContext(const char *fileName,int line) const
{
  p->printIncludeContext(fileName,line);
}

void TemplateEngine::setOutputExtension(const char *extension)
{
  p->setOutputExtension(extension);
}

QCString TemplateEngine::outputExtension() const
{
  return p->outputExtension();
}

void TemplateEngine::setTemplateDir(const char *dirName)
{
  p->setTemplateDir(dirName);
}


