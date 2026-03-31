/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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
#include <vector>

#include "containers.h"
#include "qcstring.h"

struct MermaidContent
{
  MermaidContent(const QCString &baseName_, const QCString &content_,
                 const QCString &outDir_, const QCString &srcFile_, int srcLine_)
     : baseName(baseName_), content(content_), outDir(outDir_),
       srcFile(srcFile_), srcLine(srcLine_) {}
  QCString baseName;
  QCString content;
  QCString outDir;
  QCString srcFile;
  int srcLine;
};

/** Singleton that manages Mermaid diagram rendering via the mmdc CLI tool. */
class MermaidManager
{
  public:
    /** Mermaid output image formats */
    enum OutputFormat { MERM_BITMAP, MERM_SVG };

    static MermaidManager &instance();

    /** Run mmdc tool for all collected diagrams */
    void run();

    /** Write a Mermaid source file and register it for CLI rendering.
     *  @param[in] outDirArg   the output directory to write the file to.
     *  @param[in] fileName    the name of the file. If empty a name will be chosen automatically.
     *  @param[in] content     the Mermaid diagram source.
     *  @param[in] format      the image format to generate.
     *  @param[in] srcFile     the source file resulting in the write command.
     *  @param[in] srcLine     the line number resulting in the write command.
     *  @returns The base name of the generated file (without extension).
     */
    QCString writeMermaidSource(const QCString &outDirArg, const QCString &fileName,
                                const QCString &content, OutputFormat format,
                                const QCString &srcFile, int srcLine);

    /** Register a generated Mermaid image with the index.
     *  @param[in] baseName the name of the generated file (as returned by writeMermaidSource())
     *  @param[in] outDir   the directory containing the resulting image.
     *  @param[in] format   the image format that was generated.
     */
    void generateMermaidOutput(const QCString &baseName, const QCString &outDir, OutputFormat format);

    using ContentList = std::vector<MermaidContent>;

  private:
    MermaidManager();

    static QCString imageExtension(OutputFormat format);

    ContentList m_pngContent;
    ContentList m_svgContent;
};

#endif
