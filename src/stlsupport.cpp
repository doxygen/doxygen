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
  { "allocator",            nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              false },
  { "auto_ptr",             nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // deprecated
  { "smart_ptr",            nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // C++11
  { "unique_ptr",           nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // C++11
  { "shared_ptr",           nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // C++14
  { "weak_ptr",             nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // C++11
  { "atomic",               nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // C++11
  { "atomic_ref",           nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // C++20
  { "lock_guard",           nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // C++11
  { "unique_lock",          nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // C++11
  { "shared_lock",          nullptr,                        nullptr,               "T",           "ptr",         nullptr,      nullptr,       false,              false }, // C++14
  { "ios_base",             nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "error_code",           nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "error_category",       nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "system_error",         nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "error_condition",      nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "thread",               nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "jthread",              nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "mutex",                nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "timed_mutex",          nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "recursive_mutex",      nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "recursive_timed_mutex",nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++11
  { "shared_mutex",         nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++17
  { "shared_timed_mutex",   nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }, // C++14
  { "basic_ios",            "ios_base",                     nullptr,               "Char",        nullptr,       nullptr,      nullptr,       false,              false },
  { "basic_istream",        "basic_ios<Char>",              nullptr,               "Char",        nullptr,       nullptr,      nullptr,       true,               false },
  { "basic_ostream",        "basic_ios<Char>",              nullptr,               "Char",        nullptr,       nullptr,      nullptr,       true,               false },
  { "basic_iostream",       "basic_istream<Char>",          "basic_ostream<Char>", "Char",        nullptr,       nullptr,      nullptr,       false,              false },
  { "basic_ifstream",       "basic_istream<Char>",          nullptr,               "Char",        nullptr,       nullptr,      nullptr,       false,              false },
  { "basic_ofstream",       "basic_ostream<Char>",          nullptr,               "Char",        nullptr,       nullptr,      nullptr,       false,              false },
  { "basic_fstream",        "basic_iostream<Char>",         nullptr,               "Char",        nullptr,       nullptr,      nullptr,       false,              false },
  { "basic_istringstream",  "basic_istream<Char>",          nullptr,               "Char",        nullptr,       nullptr,      nullptr,       false,              false },
  { "basic_ostringstream",  "basic_ostream<Char>",          nullptr,               "Char",        nullptr,       nullptr,      nullptr,       false,              false },
  { "basic_stringstream",   "basic_iostream<Char>",         nullptr,               "Char",        nullptr,       nullptr,      nullptr,       false,              false },
  { "ios",                  "basic_ios<char>",              nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "wios",                 "basic_ios<wchar_t>",           nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "istream",              "basic_istream<char>",          nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "wistream",             "basic_istream<wchar_t>",       nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "ostream",              "basic_ostream<char>",          nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "wostream",             "basic_ostream<wchar_t>",       nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "ifstream",             "basic_ifstream<char>",         nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "wifstream",            "basic_ifstream<wchar_t>",      nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "ofstream",             "basic_ofstream<char>",         nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "wofstream",            "basic_ofstream<wchar_t>",      nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "fstream",              "basic_fstream<char>",          nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "wfstream",             "basic_fstream<wchar_t>",       nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "istringstream",        "basic_istringstream<char>",    nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "wistringstream",       "basic_istringstream<wchar_t>", nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "ostringstream",        "basic_ostringstream<char>",    nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "wostringstream",       "basic_ostringstream<wchar_t>", nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "stringstream",         "basic_stringstream<char>",     nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "wstringstream",        "basic_stringstream<wchar_t>",  nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "basic_string",         nullptr,                        nullptr,               "Char",        nullptr,       nullptr,      nullptr,       false,              true  },
  { "string",               "basic_string<char>",           nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  },
  { "wstring",              "basic_string<wchar_t>",        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  },
  { "u8string",             "basic_string<char8_t>",        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  }, // C++20
  { "u16string",            "basic_string<char16_t>",       nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  }, // C++11
  { "u32string",            "basic_string<char32_t>",       nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  }, // C++11
  { "basic_string_view",    nullptr,                        nullptr,               "Char",        nullptr,       nullptr,      nullptr,       false,              true  },
  { "string_view",          "basic_string_view<char>",      nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  }, // C++17
  { "wstring_view",         "basic_string_view<wchar_t>",   nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  }, // C++17
  { "u8string_view",        "basic_string_view<char8_t>",   nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  }, // C++20
  { "u16string_view",       "basic_string_view<char16_t>",  nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  }, // C++17
  { "u32string_view",       "basic_string_view<char32_t>",  nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              true  }, // C++17
  { "complex",              nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "bitset",               nullptr,                        nullptr,               "Bits",        nullptr,       nullptr,      nullptr,       false,              false },
  { "deque",                nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              true  },
  { "list",                 nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              true  },
  { "forward_list",         nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              true  }, // C++11
  { "pair",                 nullptr,                        nullptr,               "T1",          "first_type",  "T2",         "second_type", false,              false },
  { "map",                  nullptr,                        nullptr,               "K",           "keys",        "T",          "elements",    false,              true  },
  { "unordered_map",        nullptr,                        nullptr,               "K",           "keys",        "T",          "elements",    false,              true  }, // C++11
  { "multimap",             nullptr,                        nullptr,               "K",           "keys",        "T",          "elements",    false,              true  },
  { "unordered_multimap",   nullptr,                        nullptr,               "K",           "keys",        "T",          "elements",    false,              true  }, // C++11
  { "set",                  nullptr,                        nullptr,               "K",           "keys",        nullptr,      nullptr,       false,              true  },
  { "unordered_set",        nullptr,                        nullptr,               "K",           "keys",        nullptr,      nullptr,       false,              true  }, // C++11
  { "multiset",             nullptr,                        nullptr,               "K",           "keys",        nullptr,      nullptr,       false,              true  },
  { "unordered_multiset",   nullptr,                        nullptr,               "K",           "keys",        nullptr,      nullptr,       false,              true  }, // C++11
  { "array",                nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              true  }, // C++11
  { "vector",               nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              true  },
  { "span",                 nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              true  }, // C++20
  { "queue",                nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              false },
  { "priority_queue",       nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              false },
  { "stack",                nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              false },
  { "valarray",             nullptr,                        nullptr,               "T",           "elements",    nullptr,      nullptr,       false,              false },
  { "exception",            nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "bad_alloc",            "exception",                    nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "bad_cast",             "exception",                    nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "bad_typeid",           "exception",                    nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "logic_error",          "exception",                    nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "ios_base::failure",    "exception",                    nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "runtime_error",        "exception",                    nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "bad_exception",        "exception",                    nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "domain_error",         "logic_error",                  nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "invalid_argument",     "logic_error",                  nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "length_error",         "logic_error",                  nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "out_of_range",         "logic_error",                  nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "range_error",          "runtime_error",                nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "overflow_error",       "runtime_error",                nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { "underflow_error",      "runtime_error",                nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false },
  { nullptr,                nullptr,                        nullptr,               nullptr,       nullptr,       nullptr,      nullptr,       false,              false }
};

static void addSTLMember(const std::shared_ptr<Entry> &root,const char *type,const char *name)
{
  std::shared_ptr<Entry> memEntry = std::make_shared<Entry>();
  memEntry->name       = name;
  memEntry->type       = type;
  memEntry->protection = Protection::Public;
  memEntry->section    = EntryType::makeVariable();
  memEntry->brief      = "STL member";
  memEntry->hidden     = false;
  memEntry->artificial = true;
  root->moveToSubEntryAndKeep(memEntry);
}

static void addSTLIterator(const std::shared_ptr<Entry> &classEntry,const QCString &name)
{
  std::shared_ptr<Entry> iteratorClassEntry = std::make_shared<Entry>();
  iteratorClassEntry->fileName  = "[STL]";
  iteratorClassEntry->startLine = 1;
  iteratorClassEntry->name      = name;
  iteratorClassEntry->section   = EntryType::makeClass();
  iteratorClassEntry->brief     = "STL iterator class";
  iteratorClassEntry->hidden    = false;
  iteratorClassEntry->artificial= true;
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
  classEntry->section = EntryType::makeClass();
  classEntry->brief     = "STL class";
  classEntry->hidden    = false;
  classEntry->artificial= true;

  // add template arguments to class
  if (info->templType1 != nullptr)
  {
    ArgumentList al;
    Argument a;
    a.type="typename";
    a.name=info->templType1;
    al.push_back(a);
    if (info->templType2 != nullptr) // another template argument
    {
      a.type="typename";
      a.name=info->templType2;
      al.push_back(a);
    }
    classEntry->tArgLists.push_back(al);
  }
  // add member variables
  if (info->templName1 != nullptr)
  {
    addSTLMember(classEntry,info->templType1,info->templName1);
  }
  if (info->templName2 != nullptr)
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
    memEntry->section = EntryType::makeFunction();
    memEntry->brief      = "STL member";
    memEntry->hidden     = false;
    memEntry->artificial = false;
    classEntry->moveToSubEntryAndKeep(memEntry);
  }
  Specifier virt = info->virtualInheritance ? Specifier::Virtual : Specifier::Normal;
  if (info->baseClass1 != nullptr)
  {
    classEntry->extends.emplace_back(info->baseClass1, Protection::Public, virt);
  }
  if (info->baseClass2 != nullptr)
  {
    classEntry->extends.emplace_back(info->baseClass2, Protection::Public, virt);
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
  namespaceEntry->section = EntryType::makeNamespace();
  namespaceEntry->brief     = "STL namespace";
  namespaceEntry->hidden    = false;
  namespaceEntry->artificial= true;

  STLInfo *info = g_stlinfo;
  while (info->className != nullptr)
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
