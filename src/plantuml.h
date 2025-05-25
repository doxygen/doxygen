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

#include <map>
#include <string>

#include "containers.h"
#include "qcstring.h"

#define DIVIDE_COUNT            4
#define MIN_PLANTUML_COUNT      8

class QCString;
struct PlantumlContent
{
  PlantumlContent(const QCString &content_, const QCString &outDir_, const QCString &srcFile_, int srcLine_)
     : content(content_), outDir(outDir_), srcFile(srcFile_), srcLine(srcLine_) {}
  QCString content;
  QCString outDir;
  QCString srcFile;
  int srcLine;
};

/** Singleton that manages plantuml relation actions */
class PlantumlManager
{
  public:
    /** Plant UML output image formats */
    enum OutputFormat { PUML_BITMAP, PUML_EPS, PUML_SVG };

    static PlantumlManager &instance();

    /** Run plant UML tool for all images */
    void run();

    /** Write a PlantUML compatible file.
     *  @param[in] outDirArg   the output directory to write the file to.
     *  @param[in] fileName    the name of the file. If empty a name will be chosen automatically.
     *  @param[in] content     the contents of the PlantUML file.
     *  @param[in] format      the image format to generate.
     *  @param[in] engine      the plantuml engine to use.
     *  @param[in] srcFile     the source file resulting in the write command.
     *  @param[in] srcLine     the line number resulting in the write command.
     *  @param[in] inlineCode  the code is coming from the `\statuml ... \enduml` (`true`) command or
     *   from the `\planumlfile` command (`false`)
     *  @returns The names of the generated files.
     */
    StringVector writePlantUMLSource(const QCString &outDirArg,const QCString &fileName,
                                     const QCString &content, OutputFormat format,
                                     const QCString &engine,const QCString &srcFile,
                                     int srcLine,bool inlineCode);

    /** Convert a PlantUML file to an image.
     *  @param[in] baseName the name of the generated file (as returned by writePlantUMLSource())
     *  @param[in] outDir   the directory to write the resulting image into.
     *  @param[in] format   the image format to generate.
     */
    void generatePlantUMLOutput(const QCString &baseName,const QCString &outDir,OutputFormat format);

    using FilesMap   = std::map< std::string, StringVector    >;
    using ContentMap = std::map< std::string, PlantumlContent >;
  private:
    PlantumlManager();
    void insert(const std::string &key,
                const std::string &value,
                const QCString &outDir,
                OutputFormat format,
                const QCString &puContent,
                const QCString &srcFile,
                int srcLine);
    void generatePlantUmlFileNames(const QCString &fileName,OutputFormat format,const QCString &outDir,
                                                    QCString &baseName,QCString &puName,QCString &imgName);

    FilesMap   m_pngPlantumlFiles;
    FilesMap   m_svgPlantumlFiles;
    FilesMap   m_epsPlantumlFiles;
    ContentMap m_pngPlantumlContent;               // use circular queue for using multi-processor (multi threading)
    ContentMap m_svgPlantumlContent;
    ContentMap m_epsPlantumlContent;
};

#endif

