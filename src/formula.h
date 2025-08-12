/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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
#include "containers.h"

class Dir;

/** Class representing a LaTeX formula as found in the documentation */
class Formula
{
  public:
    Formula(const QCString &text,int id,int width=-1,int height=-1)
      : m_text(text), m_id(id), m_width(width), m_height(height) {}

    int width() const     { return m_width;  }
    int height() const    { return m_height; }
    int id() const        { return m_id;     }
    QCString text() const { return m_text;   }
    bool isCached() const { return m_cached && !m_forceRegen; }
    bool isCachedDark() const { return m_cached && !m_forceRegen; }
    void setWidth(int width)    { m_width  = width;  }
    void setHeight(int height)  { m_height = height; }

  private:
    friend class FormulaManager;
    void setCached(bool cached) { m_cached = cached; m_forceRegen = m_forceRegen || !cached; }
    void setCachedDark(bool cached) { m_cachedDark = cached; m_forceRegen = m_forceRegen || !cached; }

    QCString m_text;
    int m_id;
    int m_width;
    int m_height;
    bool m_cached = false;
    bool m_cachedDark = false; // dark version cached?
    bool m_forceRegen = false; // true if there is an inconsistency in setCache calls
};

/*! Manager class to handle formulas */
class FormulaManager
{
  public:
    static FormulaManager &instance();

    //! @name repository functions
    //! @{
    void initFromRepository(const QCString &dir);
    void checkRepositories();
    //! @}

    //! @name formula functions
    //! @{
    void           clear();
    int            addFormula(const QCString &formulaText,int width=-1,int height=-1);
    const Formula *findFormula(int formulaId) const;
    bool           hasFormulas() const;
    //! @}

    //! @name generator functions
    //! @{
    enum class Format { Bitmap, Vector };
    enum class HighDPI { On, Off };
    enum class Mode { Dark, Light };
    void generateImages(const QCString &outputDir,Format format,HighDPI hd = HighDPI::Off);
    //! @}

  private:
    void createFormulasTexFile(Dir &d,Format format,HighDPI hd,Mode mode);
    void createLatexFile(const QCString &fileName,Format format,Mode mode,IntVector &formulasToGenerate);
    FormulaManager();
    struct Private;
    std::unique_ptr<Private> p;
};

#endif
