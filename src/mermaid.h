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
#include "dstring.h"

struct MermaidDiagramInfo
{
  MermaidDiagramInfo(const DString &baseName_, const DString &content_,
                     const DString &outDir_, const DString &srcFile_, int srcLine_)
     : baseName(baseName_), content(content_), outDir(outDir_),
       srcFile(srcFile_), srcLine(srcLine_) {}
  DString baseName;
  DString content;
  DString outDir;
  DString srcFile;
  int srcLine;
};

/** Singleton that manages Mermaid diagram rendering via the mmdc CLI tool. */
class MermaidManager
{
  public:
    /** Mermaid output image formats */
    enum class OutputFormat { HTML, LaTeX, RTF, Docbook };
    enum class ImageFormat { PNG, SVG, PDF };

    static MermaidManager &instance();

    void setHasInlineDiagrams();
    bool hasInlineDiagrams() const { return m_hasInlineDiagrams; }
    bool needToRun() const { return !m_diagrams.empty(); }

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
    DString writeMermaidSource(const DString &outDirArg, const DString &fileName,
                                const DString &content, ImageFormat format,
                                const DString &srcFile, int srcLine);

    /** Register a generated Mermaid image with the index.
     *  @param[in] baseName the name of the generated file (as returned by writeMermaidSource())
     *  @param[in] outDir   the directory containing the resulting image.
     *  @param[in] format   the image format that was generated.
     *  @param[in] toIndex  add the file to the index lists for htmlhelp / qhc etc.
     */
    void generateMermaidOutput(const DString &baseName, const DString &outDir, ImageFormat format, bool toIndex);

    struct MermaidDiagram
    {
      MermaidDiagram(ImageFormat fmt, MermaidDiagramInfo &&inf) : imageFormat(fmt), info(std::move(inf)) {}
      ImageFormat imageFormat;
      MermaidDiagramInfo info;
    };
    using DiagramList = std::vector<MermaidDiagram>;

    static DString imageExtension(ImageFormat imageFormat);
    static ImageFormat convertToImageFormat(OutputFormat outputFormat);

  private:
    MermaidManager();

    DiagramList m_diagrams;
    bool m_hasInlineDiagrams = false;
};

#endif
