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

#ifndef PLANTUML_H
#define PLANTUML_H

#include <qdict.h>
#include <qlist.h>

#define CACHE_FILENAME          "inline_umlgraph_cache_all.pu"
#define DIVIDE_COUNT            4
#define MIN_PLANTUML_COUNT      8

class QCString;
struct PlantumlContent
{
  QCString outDir;
  QCString content;
  PlantumlContent(const QCString Content, const QCString OutDir)
  {
    outDir = OutDir;
    content = Content;
  };
  ~PlantumlContent(){};
};

/** Singleton that manages plantuml relation actions */
class PlantumlManager
{
  public:
    /** Plant UML output image formats */
    enum OutputFormat { PUML_BITMAP, PUML_EPS, PUML_SVG };

    static PlantumlManager *instance();

    /** Run plant UML tool for all images */
    void run();

    /** Write a PlantUML compatible file.
     *  @param[in] outDir   the output directory to write the file to.
     *  @param[in] fileName the name of the file. If empty a name will be chosen automatically.
     *  @param[in] content  the contents of the PlantUML file.
     *  @param[in] format   the image format to generate.
     *  @returns The name of the generated file.
     */
    QCString writePlantUMLSource(const QCString &outDir,const QCString &fileName,const QCString &content, OutputFormat format);

    /** Convert a PlantUML file to an image.
     *  @param[in] baseName the name of the generated file (as returned by writePlantUMLSource())
     *  @param[in] outDir   the directory to write the resulting image into.
     *  @param[in] format   the image format to generate.
     */
    void generatePlantUMLOutput(const char *baseName,const char *outDir,OutputFormat format);

  private:
    PlantumlManager();
    ~PlantumlManager();
    void insert(const QCString &key, 
                const QCString &value,
                const QCString &outDir,
                OutputFormat format,
                const QCString &puContent);
    static PlantumlManager     *m_theInstance;
    QDict< QList<QCString> >    m_pngPlantumlFiles;
    QDict< QList<QCString> >    m_svgPlantumlFiles;
    QDict< QList<QCString> >    m_epsPlantumlFiles;
    QDict< PlantumlContent >    m_pngPlantumlContent;     // use circular queue for using multi-processor (multi threading)
    QDict< PlantumlContent >    m_svgPlantumlContent;
    QDict< PlantumlContent >    m_epsPlantumlContent;
    QCString                    m_cachedPlantumlAllContent;         // read from CACHE_FILENAME file
    QCString                    m_currentPlantumlAllContent;        // processing plantuml then write it into CACHE_FILENAME to reuse the next time as cache information
};

#endif

