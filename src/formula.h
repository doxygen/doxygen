/******************************************************************************
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

#ifndef FORMULA_H
#define FORMULA_H

#include <qlist.h>
#include <qdict.h>

/** Class representing a formula in the output. */
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

/** A list of Formula objects. */
class FormulaList : public QList<Formula>
{
  public:
    void generateBitmaps(const char *path);
};

/** Iterator for Formula objects in a FormulaList. */
class FormulaListIterator : public QListIterator<Formula>
{
  public:
    FormulaListIterator(const FormulaList &l) : 
      QListIterator<Formula>(l) {}
};

/** Unsorted dictionary of Formula objects. */
class FormulaDict : public QDict<Formula>
{
  public:
    FormulaDict(uint size) : 
      QDict<Formula>(size) {}
   ~FormulaDict() {}
};

#endif
