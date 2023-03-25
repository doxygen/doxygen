/******************************************************************************
 *
 * Copyright (C) 1997-2019 by Dimitri van Heesch.
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

#include "stlsupport.h"
#include "entry.h"
#include "config.h"

/** A struct contained the data for an STL class */
struct STLInfo
{
  const char *className;
  const char *baseClass1;
  const char *baseClass2;
  const char *templType1;
  const char *templName1;
  const char *templType2;
  const char *templName2;
  bool virtualInheritance;
  bool iterators;
};

static STLInfo g_stlinfo[] =
{
  // className              baseClass1                      baseClass2             templType1     templName1     templType2    templName2     virtInheritance  // iterators
  { "allocator",            0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "auto_ptr",             0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // deprecated
  { "smart_ptr",            0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++11
  { "unique_ptr",           0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++11
  { "shared_ptr",           0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++14
  { "weak_ptr",             0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++11
  { "atomic",               0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++11
  { "atomic_ref",           0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++20
  { "lock_guard",           0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++11
  { "unique_lock",          0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++11
  { "shared_lock",          0,                              0,                     "T",           "ptr",         0,            0,             FALSE,              FALSE }, // C++14
  { "ios_base",             0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "error_code",           0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "error_category",       0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "system_error",         0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "error_condition",      0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "thread",               0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "jthread",              0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "mutex",                0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "timed_mutex",          0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "recursive_mutex",      0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "recursive_timed_mutex",0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++11
  { "shared_mutex",         0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++17
  { "shared_timed_mutex",   0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }, // C++14
  { "basic_ios",            "ios_base",                     0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_istream",        "basic_ios<Char>",              0,                     "Char",        0,             0,            0,             TRUE,               FALSE },
  { "basic_ostream",        "basic_ios<Char>",              0,                     "Char",        0,             0,            0,             TRUE,               FALSE },
  { "basic_iostream",       "basic_istream<Char>",          "basic_ostream<Char>", "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_ifstream",       "basic_istream<Char>",          0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_ofstream",       "basic_ostream<Char>",          0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_fstream",        "basic_iostream<Char>",         0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_istringstream",  "basic_istream<Char>",          0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_ostringstream",  "basic_ostream<Char>",          0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "basic_stringstream",   "basic_iostream<Char>",         0,                     "Char",        0,             0,            0,             FALSE,              FALSE },
  { "ios",                  "basic_ios<char>",              0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wios",                 "basic_ios<wchar_t>",           0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "istream",              "basic_istream<char>",          0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wistream",             "basic_istream<wchar_t>",       0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ostream",              "basic_ostream<char>",          0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wostream",             "basic_ostream<wchar_t>",       0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ifstream",             "basic_ifstream<char>",         0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wifstream",            "basic_ifstream<wchar_t>",      0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ofstream",             "basic_ofstream<char>",         0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wofstream",            "basic_ofstream<wchar_t>",      0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "fstream",              "basic_fstream<char>",          0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wfstream",             "basic_fstream<wchar_t>",       0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "istringstream",        "basic_istringstream<char>",    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wistringstream",       "basic_istringstream<wchar_t>", 0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ostringstream",        "basic_ostringstream<char>",    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wostringstream",       "basic_ostringstream<wchar_t>", 0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "stringstream",         "basic_stringstream<char>",     0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "wstringstream",        "basic_stringstream<wchar_t>",  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "basic_string",         0,                              0,                     "Char",        0,             0,            0,             FALSE,              TRUE  },
  { "string",               "basic_string<char>",           0,                     0,             0,             0,            0,             FALSE,              TRUE  },
  { "wstring",              "basic_string<wchar_t>",        0,                     0,             0,             0,            0,             FALSE,              TRUE  },
  { "u8string",             "basic_string<char8_t>",        0,                     0,             0,             0,            0,             FALSE,              TRUE  }, // C++20
  { "u16string",            "basic_string<char16_t>",       0,                     0,             0,             0,            0,             FALSE,              TRUE  }, // C++11
  { "u32string",            "basic_string<char32_t>",       0,                     0,             0,             0,            0,             FALSE,              TRUE  }, // C++11
  { "basic_string_view",    0,                              0,                     "Char",        0,             0,            0,             FALSE,              TRUE  },
  { "string_view",          "basic_string_view<char>",      0,                     0,             0,             0,            0,             FALSE,              TRUE  }, // C++17
  { "wstring_view",         "basic_string_view<wchar_t>",   0,                     0,             0,             0,            0,             FALSE,              TRUE  }, // C++17
  { "u8string_view",        "basic_string_view<char8_t>",   0,                     0,             0,             0,            0,             FALSE,              TRUE  }, // C++20
  { "u16string_view",       "basic_string_view<char16_t>",  0,                     0,             0,             0,            0,             FALSE,              TRUE  }, // C++17
  { "u32string_view",       "basic_string_view<char32_t>",  0,                     0,             0,             0,            0,             FALSE,              TRUE  }, // C++17
  { "complex",              0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bitset",               0,                              0,                     "Bits",        0,             0,            0,             FALSE,              FALSE },
  { "deque",                0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  },
  { "list",                 0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  },
  { "forward_list",         0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  }, // C++11
  { "map",                  0,                              0,                     "K",           "keys",        "T",          "elements",    FALSE,              TRUE  },
  { "unordered_map",        0,                              0,                     "K",           "keys",        "T",          "elements",    FALSE,              TRUE  }, // C++11
  { "multimap",             0,                              0,                     "K",           "keys",        "T",          "elements",    FALSE,              TRUE  },
  { "unordered_multimap",   0,                              0,                     "K",           "keys",        "T",          "elements",    FALSE,              TRUE  }, // C++11
  { "set",                  0,                              0,                     "K",           "keys",        0,            0,             FALSE,              TRUE  },
  { "unordered_set",        0,                              0,                     "K",           "keys",        0,            0,             FALSE,              TRUE  }, // C++11
  { "multiset",             0,                              0,                     "K",           "keys",        0,            0,             FALSE,              TRUE  },
  { "unordered_multiset",   0,                              0,                     "K",           "keys",        0,            0,             FALSE,              TRUE  }, // C++11
  { "array",                0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  }, // C++11
  { "vector",               0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  },
  { "span",                 0,                              0,                     "T",           "elements",    0,            0,             FALSE,              TRUE  }, // C++20
  { "queue",                0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "priority_queue",       0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "stack",                0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "valarray",             0,                              0,                     "T",           "elements",    0,            0,             FALSE,              FALSE },
  { "exception",            0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bad_alloc",            "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bad_cast",             "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bad_typeid",           "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "logic_error",          "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "ios_base::failure",    "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "runtime_error",        "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "bad_exception",        "exception",                    0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "domain_error",         "logic_error",                  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "invalid_argument",     "logic_error",                  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "length_error",         "logic_error",                  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "out_of_range",         "logic_error",                  0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "range_error",          "runtime_error",                0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "overflow_error",       "runtime_error",                0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { "underflow_error",      "runtime_error",                0,                     0,             0,             0,            0,             FALSE,              FALSE },
  { 0,                      0,                              0,                     0,             0,             0,            0,             FALSE,              FALSE }
};

static void addSTLMember(const std::shared_ptr<Entry> &root,const char *type,const char *name)
{
  std::shared_ptr<Entry> memEntry = std::make_shared<Entry>();
  memEntry->name       = name;
  memEntry->type       = type;
  memEntry->protection = Protection::Public;
  memEntry->section    = Entry::VARIABLE_SEC;
  memEntry->brief      = "STL member";
  memEntry->hidden     = FALSE;
  memEntry->artificial = TRUE;
  root->moveToSubEntryAndKeep(memEntry);
}

static void addSTLIterator(const std::shared_ptr<Entry> &classEntry,const QCString &name)
{
  std::shared_ptr<Entry> iteratorClassEntry = std::make_shared<Entry>();
  iteratorClassEntry->fileName  = "[STL]";
  iteratorClassEntry->startLine = 1;
  iteratorClassEntry->name      = name;
  iteratorClassEntry->section   = Entry::CLASS_SEC;
  iteratorClassEntry->brief     = "STL iterator class";
  iteratorClassEntry->hidden    = FALSE;
  iteratorClassEntry->artificial= TRUE;
  classEntry->moveToSubEntryAndKeep(iteratorClassEntry);
}

static void addSTLClass(const std::shared_ptr<Entry> &root,const STLInfo *info)
{
  //printf("Adding STL class %s\n",info->className);
  QCString fullName = info->className;
  fullName.prepend("std::");

  // add fake Entry for the class
  std::shared_ptr<Entry> classEntry = std::make_shared<Entry>();
  classEntry->fileName  = "[STL]";
  classEntry->startLine = 1;
  classEntry->name      = fullName;
  classEntry->section   = Entry::CLASS_SEC;
  classEntry->brief     = "STL class";
  classEntry->hidden    = FALSE;
  classEntry->artificial= TRUE;

  // add template arguments to class
  if (info->templType1)
  {
    ArgumentList al;
    Argument a;
    a.type="typename";
    a.name=info->templType1;
    al.push_back(a);
    if (info->templType2) // another template argument
    {
      a.type="typename";
      a.name=info->templType2;
      al.push_back(a);
    }
    classEntry->tArgLists.push_back(al);
  }
  // add member variables
  if (info->templName1)
  {
    addSTLMember(classEntry,info->templType1,info->templName1);
  }
  if (info->templName2)
  {
    addSTLMember(classEntry,info->templType2,info->templName2);
  }
  if (fullName=="std::auto_ptr" ||
      fullName=="std::smart_ptr" ||
      fullName=="std::shared_ptr" ||
      fullName=="std::weak_ptr" ||
      fullName=="std::unique_ptr")
  {
    std::shared_ptr<Entry> memEntry = std::make_shared<Entry>();
    memEntry->name       = "operator->";
    memEntry->args       = "()";
    memEntry->type       = "T*";
    memEntry->protection = Protection::Public;
    memEntry->section    = Entry::FUNCTION_SEC;
    memEntry->brief      = "STL member";
    memEntry->hidden     = FALSE;
    memEntry->artificial = FALSE;
    classEntry->moveToSubEntryAndKeep(memEntry);
  }
  Specifier virt = info->virtualInheritance ? Specifier::Virtual : Specifier::Normal;
  if (info->baseClass1)
  {
    classEntry->extends.push_back(BaseInfo(info->baseClass1, Protection::Public, virt));
  }
  if (info->baseClass2)
  {
    classEntry->extends.push_back(BaseInfo(info->baseClass2, Protection::Public, virt));
  }
  if (info->iterators)
  {
    // add iterator class
    addSTLIterator(classEntry,fullName+"::iterator");
    addSTLIterator(classEntry,fullName+"::const_iterator");
    addSTLIterator(classEntry,fullName+"::reverse_iterator");
    addSTLIterator(classEntry,fullName+"::const_reverse_iterator");
  }
  root->moveToSubEntryAndKeep(classEntry);
}


static void addSTLClasses(const std::shared_ptr<Entry> &root)
{
  std::shared_ptr<Entry> namespaceEntry = std::make_shared<Entry>();
  namespaceEntry->fileName  = "[STL]";
  namespaceEntry->startLine = 1;
  namespaceEntry->name      = "std";
  namespaceEntry->section   = Entry::NAMESPACE_SEC;
  namespaceEntry->brief     = "STL namespace";
  namespaceEntry->hidden    = FALSE;
  namespaceEntry->artificial= TRUE;

  STLInfo *info = g_stlinfo;
  while (info->className)
  {
    addSTLClass(namespaceEntry,info);
    info++;
  }

  root->moveToSubEntryAndKeep(namespaceEntry);
}

void addSTLSupport(std::shared_ptr<Entry> &root)
{
  if (Config_getBool(BUILTIN_STL_SUPPORT))
  {
    addSTLClasses(root);
  }
}
