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

#include <memory>
#include <string>

#include "qcstring.h"

/*! Manager class to handle formulas */
class FormulaManager
{
  public:
    struct DisplaySize
    {
      DisplaySize(int w,int h) : width(w), height(h) {}
      int width;
      int height;
    };
    enum class Format { Bitmap, Vector };
    enum class HighDPI { On, Off };
    static FormulaManager &instance();
    void readFormulas(const char *dir,bool doCompare=false);
    void clear();
    int addFormula(const std::string &formulaText);
    void generateImages(const char *outputDir,Format format,HighDPI hd = HighDPI::Off) const;
    std::string findFormula(int formulaId) const;
    bool hasFormulas() const;
    DisplaySize displaySize(int formulaId) const;
  private:
    FormulaManager();
    struct Private;
    std::unique_ptr<Private> p;
};

#endif
