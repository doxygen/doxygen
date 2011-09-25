/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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

#ifndef FORMULA_H
#define FORMULA_H

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>

class Formula
{
  public:
    Formula(const char *text);
   ~Formula();
    int getId();
    QCString getFormulaText() const { return form; }
    
  private:
    int number;
    QCString form;
};

class FormulaList : public QList<Formula>
{
  public:
    void generateBitmaps(const char *path);
};

class FormulaListIterator : public QListIterator<Formula>
{
  public:
    FormulaListIterator(const FormulaList &l) : 
      QListIterator<Formula>(l) {}
};

class FormulaDict : public QDict<Formula>
{
  public:
    FormulaDict(uint size) : 
      QDict<Formula>(size) {}
   ~FormulaDict() {}
};

#endif
