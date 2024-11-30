/******************************************************************************
 *
 * Copyright (C) 1997-2024 by Dimitri van Heesch.
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

#include <string>
#include "containers.h"
#include "qualifiers.h"
#include "qcstring.h"

void mergeQualifier(QualifierInfoVector &into, std::string qual, StringVector classes) 
{
  QualifierInfo *search = nullptr;
  for (auto & qi : into)
  {
    if (qi->label == qual)
    {
      search = qi;
      break;
    }
  }
  if (search!=nullptr)
  {
    for (auto & cls : classes)
    {
      if (std::find(search->classes.begin(), search->classes.end(), cls) == search->classes.end())
      {
        search->classes.push_back(cls);
      }
    }
  }
  else
  {
    QualifierInfo *nw = new QualifierInfo;
    nw->label = qual;
    nw->classes = classes;
    into.push_back(nw);
  }
}

void mergeQualifier(QualifierInfoVector &into, QualifierInfoVector from)
{
  for (auto & m_qi : from)
  {
    mergeQualifier(into, m_qi->label, m_qi->classes);
  }
}

void insertQualifier(QualifierInfoVector &svm, std::string qual, std::string cls)
{
  QCString txt(cls.c_str());
  if (txt.isEmpty())
  {
    txt = qual.c_str();
  }
  StringVector sv = {substitute(txt," ","-").str()};
  QualifierInfo *nw = new QualifierInfo;
  nw->label = qual;
  nw->classes = sv;
  svm.push_back(nw);
}

