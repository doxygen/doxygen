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

#ifndef MERMAID_H
#define MERMAID_H

#include <map>
#include <string>

#include "containers.h"
#include "qcstring.h"

#define MERMAID_CACHE_FILENAME          "inline_mermaidgraph_cache_all.mmd"
//#define DIVIDE_COUNT            4
#define MIN_MERMAID_COUNT      8

class QCString;
struct MermaidContent
{
  MermaidContent(const QCString &content_, const QCString &outDir_, const QCString &srcFile_, int srcLine_)
     : content(content_), outDir(outDir_), srcFile(srcFile_), srcLine(srcLine_) {}
  QCString content;
  QCString outDir;
  QCString srcFile;
  int srcLine;
};

/** Singleton that manages mermaid relation actions */
class MermaidManager
{
  public:
    /** Mermaid output image formats */
    enum OutputFormat { MERMAID_BITMAP, MERMAID_PDF, MERMAID_SVG };

    static MermaidManager &instance();

    /** Run mermaid-cli tool for all images */
    void run();

    /** Write a Mermaid compatible file.
     *  @param[in] outDir   the output directory to write the file to.
     *  @param[in] fileName the name of the file. If empty a name will be chosen automatically.
     *  @param[in] content  the contents of the Mermaid file.
     *  @param[in] format   the image format to generate.
     *  @param[in] engine   the mermaid engine to use.
     *  @param[in] srcFile  the source file resulting in the write command.
     *  @param[in] srcLine  the line number resulting in the write command.
     *  @returns The name of the generated file.
     */
    QCString writeMermaidSource(const QCString &outDir,const QCString &fileName,
                                const QCString &content, OutputFormat format,
                                const QCString &engine,const QCString &srcFile,
                                int srcLine);

    /** Convert a Mermaid file to an image.
     *  @param[in] baseName the name of the generated file (as returned by writeMermaidSource())
     *  @param[in] outDir   the directory to write the resulting image into.
     *  @param[in] format   the image format to generate.
     *  @param[in] engine   the mermaid engine to be used so the start command will be `@start<engine>`
     */
    void generateMermaidOutput(const QCString &baseName,const QCString &outDir,OutputFormat format);

    using FilesMap   = std::map< std::string, StringVector    >;
    using ContentMap = std::map< std::string, MermaidContent >;
  private:
    MermaidManager();
    void insert(const std::string &key,
                const std::string &value,
                const QCString &outDir,
                OutputFormat format,
                const QCString &puContent,
                const QCString &srcFile,
                int srcLine);

    FilesMap   m_pngMermaidFiles;
    FilesMap   m_svgMermaidFiles;
    FilesMap   m_pdfMermaidFiles;
    ContentMap m_pngMermaidContent;               // use circular queue for using multi-processor (multi threading)
    ContentMap m_svgMermaidContent;
    ContentMap m_pdfMermaidContent;
    QCString   m_cachedMermaidAllContent;         // read from MERMAID_CACHE_FILENAME file
    QCString   m_currentMermaidAllContent;        // processing mermaid then write it into MERMAID_CACHE_FILENAME to reuse the next time as cache information
};

#endif
