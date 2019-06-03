/*****************************************************************************
 *
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

#include <stdlib.h>

#include <qdir.h>
#include <qfile.h>
#include <qqueue.h>
#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qregexp.h>

#include "dot.h"
#include "doxygen.h"
#include "message.h"
#include "util.h"
#include "config.h"
#include "language.h"
#include "defargs.h"
#include "docparser.h"
#include "debug.h"
#include "pagedef.h"
#include "portable.h"
#include "dirdef.h"
#include "vhdldocgen.h"
#include "ftextstream.h"
#include "md5.h"
#include "memberlist.h"
#include "groupdef.h"
#include "classlist.h"
#include "filename.h"
#include "namespacedef.h"
#include "memberdef.h"
#include "membergroup.h"
#include "dotrunner.h"
#include "dotfilepatcher.h"

#define MAP_CMD "cmapx"

#define INTERACTIVE_SVG       Config_getBool(INTERACTIVE_SVG)
#define DOT_TRANSPARENT       Config_getBool(DOT_TRANSPARENT)
#define USE_PDFLATEX          Config_getBool(USE_PDFLATEX)
#define UML_LOOK              Config_getBool(UML_LOOK)
#define EXTRACT_PRIVATE       Config_getBool(EXTRACT_PRIVATE)
#define OPTIMIZE_OUTPUT_SLICE Config_getBool(OPTIMIZE_OUTPUT_SLICE)
#define TEMPLATE_RELATIONS    Config_getBool(TEMPLATE_RELATIONS)
#define HIDE_SCOPE_NAMES      Config_getBool(HIDE_SCOPE_NAMES)
#define DOT_FONTPATH          Config_getString(DOT_FONTPATH);
#define DOT_PATH              Config_getString(DOT_PATH)
static int DOT_NUM_THREADS;   // will be initialized in initDot
#define UML_LIMIT_NUM_FIELDS  Config_getInt(UML_LIMIT_NUM_FIELDS)
#define DOT_IMAGE_FORMAT      Config_getEnum(DOT_IMAGE_FORMAT)
#define MAX_DOT_GRAPH_DEPTH   Config_getInt(MAX_DOT_GRAPH_DEPTH)
#define DOT_GRAPH_MAX_NODES   Config_getInt(DOT_GRAPH_MAX_NODES)

//--------------------------------------------------------------------

void initDot()
{
  DotGraph::DOT_FONTNAME = Config_getString(DOT_FONTNAME);
  if (DotGraph::DOT_FONTNAME.isEmpty()) 
  {
    //DOT_FONTNAME="FreeSans.ttf";
    DotGraph::DOT_FONTNAME="Helvetica";
  }

  DotGraph::DOT_FONTSIZE = Config_getInt(DOT_FONTSIZE);
  if (DotGraph::DOT_FONTSIZE<4) DotGraph::DOT_FONTSIZE=4;

  DOT_NUM_THREADS = Config_getInt(DOT_NUM_THREADS);
  if (DOT_NUM_THREADS > 32) DOT_NUM_THREADS = 32;
  if (DOT_NUM_THREADS <= 0) DOT_NUM_THREADS = QMAX(2,QThread::idealThreadCount()+1);

  // these are copied to be sure to be thread save
  DotRunner::DOT_CLEANUP = Config_getBool(DOT_CLEANUP);
  DotRunner::DOT_MULTI_TARGETS = Config_getBool(DOT_MULTI_TARGETS);
  DotRunner::DOT_EXE.set(DOT_PATH + "dot");

  DotGraph::IMG_EXT = getDotImageExtension();
}


static QCString g_dotFontPath;

static void setDotFontPath(const char *path)
{
  ASSERT(g_dotFontPath.isEmpty());
  g_dotFontPath = portable_getenv("DOTFONTPATH");
  QCString newFontPath = DOT_FONTPATH;
  QCString spath = path;
  if (!newFontPath.isEmpty() && !spath.isEmpty())
  {
    newFontPath.prepend(spath+portable_pathListSeparator());
  }
  else if (newFontPath.isEmpty() && !spath.isEmpty())
  {
    newFontPath=path;
  }
  else
  {
    portable_unsetenv("DOTFONTPATH");
    return;
  }
  portable_setenv("DOTFONTPATH",newFontPath);
}

static void unsetDotFontPath()
{
  if (g_dotFontPath.isEmpty())
  {
    portable_unsetenv("DOTFONTPATH");
  }
  else
  {
    portable_setenv("DOTFONTPATH",g_dotFontPath);
  }
  g_dotFontPath="";
}

// extract size from a dot generated SVG file
static bool readSVGSize(const QCString &fileName,int *width,int *height)
{
  bool found=FALSE;
  QFile f(fileName);
  if (!f.open(IO_ReadOnly))
  {
    return FALSE;
  }
  const int maxLineLen=4096;
  char buf[maxLineLen];
  while (!f.atEnd() && !found)
  {
    int numBytes = f.readLine(buf,maxLineLen-1); // read line
    if (numBytes>0)
    {
      buf[numBytes]='\0';
      if (qstrncmp(buf,"<!--zoomable ",13)==0)
      {
        *width=-1;
        *height=-1;
        sscanf(buf,"<!--zoomable %d",height);
        //printf("Found zoomable for %s!\n",fileName.data());
        found=TRUE;
      }
      else if (sscanf(buf,"<svg width=\"%dpt\" height=\"%dpt\"",width,height)==2)
      {
        //printf("Found fixed size %dx%d for %s!\n",*width,*height,fileName.data());
        found=TRUE;
      }
    }
    else // read error!
    {
      //printf("Read error %d!\n",numBytes);
      return FALSE;
    }
  }
  return TRUE;
}

static void writeSVGNotSupported(FTextStream &out)
{
  out << "<p><b>This browser is not able to show SVG: try Firefox, Chrome, Safari, or Opera instead.</b></p>";
}

// check if a reference to a SVG figure can be written and does so if possible.
// return FALSE if not possible (for instance because the SVG file is not yet generated).
bool writeSVGFigureLink(FTextStream &out,const QCString &relPath,
                        const QCString &baseName,const QCString &absImgName)
{
  int width=600,height=600;
  if (!readSVGSize(absImgName,&width,&height))
  {
    return FALSE;
  }
  if (width==-1)
  {
    if (height<=60) 
      height=300;
    else 
      height+=300; // add some extra space for zooming
    if (height>600) height=600; // clip to maximum height of 600 pixels
    out << "<div class=\"zoom\">";
    //out << "<object type=\"image/svg+xml\" data=\"" 
    //out << "<embed type=\"image/svg+xml\" src=\"" 
    out << "<iframe scrolling=\"no\" frameborder=\"0\" src=\"" 
        << relPath << baseName << ".svg\" width=\"100%\" height=\"" << height << "\">";
  }
  else
  {
    //out << "<object type=\"image/svg+xml\" data=\"" 
    //out << "<embed type=\"image/svg+xml\" src=\"" 
    out << "<iframe scrolling=\"no\" frameborder=\"0\" src=\"" 
        << relPath << baseName << ".svg\" width=\"" 
        << ((width*96+48)/72) << "\" height=\"" 
        << ((height*96+48)/72) << "\">";
  }
  writeSVGNotSupported(out);
  //out << "</object>";
  //out << "</embed>";
  out << "</iframe>";
  if (width==-1)
  {
    out << "</div>";
  }

  return TRUE;
}

//--------------------------------------------------------------------

DotManager *DotManager::m_theInstance = 0;

DotManager *DotManager::instance()
{
  if (!m_theInstance)
  {
    m_theInstance = new DotManager;
  }
  return m_theInstance;
}

DotManager::DotManager() : m_dotMaps(1009)
{
  m_dotRuns.setAutoDelete(TRUE);
  m_dotMaps.setAutoDelete(TRUE);
  m_queue = new DotRunnerQueue;
  int i;
  if (DOT_NUM_THREADS!=1)
  {
    for (i=0;i<DOT_NUM_THREADS;i++)
    {
      DotWorkerThread *thread = new DotWorkerThread(m_queue);
      thread->start();
      if (thread->isRunning())
      {
        m_workers.append(thread);
      }
      else // no more threads available!
      {
        delete thread;
      }
    }
    ASSERT(m_workers.count()>0);
  }
}

DotManager::~DotManager()
{
  delete m_queue;
}

void DotManager::addRun(DotRunner *run)
{
  m_dotRuns.append(run);
}

int DotManager::addMap(const QCString &file,const QCString &mapFile,
                const QCString &relPath,bool urlOnly,const QCString &context,
                const QCString &label)
{
  DotFilePatcher *map = m_dotMaps.find(file);
  if (map==0)
  {
    map = new DotFilePatcher(file);
    m_dotMaps.append(file,map);
  }
  return map->addMap(mapFile,relPath,urlOnly,context,label);
}

int DotManager::addFigure(const QCString &file,const QCString &baseName,
                          const QCString &figureName,bool heightCheck)
{
  DotFilePatcher *map = m_dotMaps.find(file);
  if (map==0)
  {
    map = new DotFilePatcher(file);
    m_dotMaps.append(file,map);
  }
  return map->addFigure(baseName,figureName,heightCheck);
}

int DotManager::addSVGConversion(const QCString &file,const QCString &relPath,
                       bool urlOnly,const QCString &context,bool zoomable,
                       int graphId)
{
  DotFilePatcher *map = m_dotMaps.find(file);
  if (map==0)
  {
    map = new DotFilePatcher(file);
    m_dotMaps.append(file,map);
  }
  return map->addSVGConversion(relPath,urlOnly,context,zoomable,graphId);
}

int DotManager::addSVGObject(const QCString &file,const QCString &baseName,
                             const QCString &absImgName,const QCString &relPath)
{
  DotFilePatcher *map = m_dotMaps.find(file);
  if (map==0)
  {
    map = new DotFilePatcher(file);
    m_dotMaps.append(file,map);
  }
  return map->addSVGObject(baseName,absImgName,relPath);
}

bool DotManager::run()
{
  uint numDotRuns = m_dotRuns.count();
  uint numDotMaps = m_dotMaps.count();
  if (numDotRuns+numDotMaps>1)
  {
    if (m_workers.count()==0)
    {
      msg("Generating dot graphs in single threaded mode...\n");
    }
    else
    {
      msg("Generating dot graphs using %d parallel threads...\n",QMIN(numDotRuns+numDotMaps,m_workers.count()));
    }
  }
  int i=1;
  QListIterator<DotRunner> li(m_dotRuns);

  bool setPath=FALSE;
  if (Config_getBool(GENERATE_HTML))
  {
    setDotFontPath(Config_getString(HTML_OUTPUT));
    setPath=TRUE;
  }
  else if (Config_getBool(GENERATE_LATEX))
  {
    setDotFontPath(Config_getString(LATEX_OUTPUT));
    setPath=TRUE;
  }
  else if (Config_getBool(GENERATE_RTF))
  {
    setDotFontPath(Config_getString(RTF_OUTPUT));
    setPath=TRUE;
  }
  else if (Config_getBool(GENERATE_DOCBOOK))
  {
    setDotFontPath(Config_getString(DOCBOOK_OUTPUT));
    setPath=TRUE;
  }
  portable_sysTimerStart();
  // fill work queue with dot operations
  DotRunner *dr;
  int prev=1;
  if (m_workers.count()==0) // no threads to work with
  {
    for (li.toFirst();(dr=li.current());++li)
    {
      msg("Running dot for graph %d/%d\n",prev,numDotRuns);
      dr->run();
      prev++;
    }
  }
  else // use multiple threads to run instances of dot in parallel
  {
    for (li.toFirst();(dr=li.current());++li)
    {
      m_queue->enqueue(dr);
    }
    // wait for the queue to become empty
    while ((i=m_queue->count())>0)
    {
      i = numDotRuns - i;
      while (i>=prev)
      {
        msg("Running dot for graph %d/%d\n",prev,numDotRuns);
        prev++;
      }
      portable_sleep(100);
    }
    while ((int)numDotRuns>=prev)
    {
      msg("Running dot for graph %d/%d\n",prev,numDotRuns);
      prev++;
    }
    // signal the workers we are done
    for (i=0;i<(int)m_workers.count();i++)
    {
      m_queue->enqueue(0); // add terminator for each worker
    }
    // wait for the workers to finish
    for (i=0;i<(int)m_workers.count();i++)
    {
      m_workers.at(i)->wait();
    }
    // clean up dot files from main thread
    for (i=0;i<(int)m_workers.count();i++)
    {
      m_workers.at(i)->cleanup();
    }
  }
  portable_sysTimerStop();
  if (setPath)
  {
    unsetDotFontPath();
  }

  // patch the output file and insert the maps and figures
  i=1;
  SDict<DotFilePatcher>::Iterator di(m_dotMaps);
  DotFilePatcher *map;
  // since patching the svg files may involve patching the header of the SVG
  // (for zoomable SVGs), and patching the .html files requires reading that
  // header after the SVG is patched, we first process the .svg files and 
  // then the other files. 
  for (di.toFirst();(map=di.current());++di)
  {
    if (map->file().right(4)==".svg")
    {
      msg("Patching output file %d/%d\n",i,numDotMaps);
      if (!map->run()) return FALSE;
      i++;
    }
  }
  for (di.toFirst();(map=di.current());++di)
  {
    if (map->file().right(4)!=".svg")
    {
      msg("Patching output file %d/%d\n",i,numDotMaps);
      if (!map->run()) return FALSE;
      i++;
    }
  }
  return TRUE;
}

bool DotManager::containsRun(const QCString& absDotName, const QCString& md5Hash)
{
  QListIterator<DotRunner> li(m_dotRuns);
  DotRunner *dr;
  for (li.toFirst();(dr=li.current());++li)
  {
    if (absDotName != QCString(dr->getFileName().data())) continue;
    // we have a match
    if (md5Hash != QCString(dr->getMd5Hash().data()))
    {
      err("md5 hash does not match for two different runs of %s !\n", absDotName.data());
    }
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------

class GraphLegendDotGraph : public DotGraph {

  virtual QCString getBaseName() const 
  {
    return "graph_legend";
  } 

  virtual void computeTheGraph()
  {
    FTextStream md5stream(&m_theGraph);
    writeGraphHeader(md5stream,theTranslator->trLegendTitle());
    md5stream << "  Node9 [shape=\"box\",label=\"Inherited\",fontsize=\"" << DOT_FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << DOT_FONTNAME << "\",fillcolor=\"grey75\",style=\"filled\" fontcolor=\"black\"];\n";
    md5stream << "  Node10 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << DOT_FONTSIZE << "\",style=\"solid\",fontname=\"" << DOT_FONTNAME << "\"];\n";
    md5stream << "  Node10 [shape=\"box\",label=\"PublicBase\",fontsize=\"" << DOT_FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << DOT_FONTNAME << "\",color=\"black\",URL=\"$classPublicBase" << Doxygen::htmlFileExtension << "\"];\n";
    md5stream << "  Node11 -> Node10 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << DOT_FONTSIZE << "\",style=\"solid\",fontname=\"" << DOT_FONTNAME << "\"];\n";
    md5stream << "  Node11 [shape=\"box\",label=\"Truncated\",fontsize=\"" << DOT_FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << DOT_FONTNAME << "\",color=\"red\",URL=\"$classTruncated" << Doxygen::htmlFileExtension << "\"];\n";
    md5stream << "  Node13 -> Node9 [dir=\"back\",color=\"darkgreen\",fontsize=\"" << DOT_FONTSIZE << "\",style=\"solid\",fontname=\"" << DOT_FONTNAME << "\"];\n";
    md5stream << "  Node13 [shape=\"box\",label=\"ProtectedBase\",fontsize=\"" << DOT_FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << DOT_FONTNAME << "\",color=\"black\",URL=\"$classProtectedBase" << Doxygen::htmlFileExtension << "\"];\n";
    md5stream << "  Node14 -> Node9 [dir=\"back\",color=\"firebrick4\",fontsize=\"" << DOT_FONTSIZE << "\",style=\"solid\",fontname=\"" << DOT_FONTNAME << "\"];\n";
    md5stream << "  Node14 [shape=\"box\",label=\"PrivateBase\",fontsize=\"" << DOT_FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << DOT_FONTNAME << "\",color=\"black\",URL=\"$classPrivateBase" << Doxygen::htmlFileExtension << "\"];\n";
    md5stream << "  Node15 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << DOT_FONTSIZE << "\",style=\"solid\",fontname=\"" << DOT_FONTNAME << "\"];\n";
    md5stream << "  Node15 [shape=\"box\",label=\"Undocumented\",fontsize=\"" << DOT_FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << DOT_FONTNAME << "\",color=\"grey75\"];\n";
    md5stream << "  Node16 -> Node9 [dir=\"back\",color=\"midnightblue\",fontsize=\"" << DOT_FONTSIZE << "\",style=\"solid\",fontname=\"" << DOT_FONTNAME << "\"];\n";
    md5stream << "  Node16 [shape=\"box\",label=\"Templ< int >\",fontsize=\"" << DOT_FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << DOT_FONTNAME << "\",color=\"black\",URL=\"$classTempl" << Doxygen::htmlFileExtension << "\"];\n";
    md5stream << "  Node17 -> Node16 [dir=\"back\",color=\"orange\",fontsize=\"" << DOT_FONTSIZE << "\",style=\"dashed\",label=\"< int >\",fontname=\"" << DOT_FONTNAME << "\"];\n";
    md5stream << "  Node17 [shape=\"box\",label=\"Templ< T >\",fontsize=\"" << DOT_FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << DOT_FONTNAME << "\",color=\"black\",URL=\"$classTempl" << Doxygen::htmlFileExtension << "\"];\n";
    md5stream << "  Node18 -> Node9 [dir=\"back\",color=\"darkorchid3\",fontsize=\"" << DOT_FONTSIZE << "\",style=\"dashed\",label=\"m_usedClass\",fontname=\"" << DOT_FONTNAME << "\"];\n";
    md5stream << "  Node18 [shape=\"box\",label=\"Used\",fontsize=\"" << DOT_FONTSIZE << "\",height=0.2,width=0.4,fontname=\"" << DOT_FONTNAME << "\",color=\"black\",URL=\"$classUsed" << Doxygen::htmlFileExtension << "\"];\n";
    writeGraphFooter(md5stream);
  }

  QCString getMapLabel() const
  {
    return "";
  }

  friend void generateGraphLegend(const char* path);
};

void generateGraphLegend(const char *path)
{
  QDir d(path);
  GraphLegendDotGraph dg;
  FTextStream ts;
  dg.writeGraph(ts, GOF_BITMAP, EOF_Html, path, "", "", FALSE, 0);

  if (getDotImageExtension()=="svg")
  {
    DotManager::instance()->addSVGObject(
        dg.absBaseName()+Config_getString(HTML_FILE_EXTENSION),
        "graph_legend",
        dg.absImgName(),QCString());
  }

}

void writeDotGraphFromFile(const char *inFile,const char *outDir,
                           const char *outFile,GraphOutputFormat format)
{
  QDir d(outDir);
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",outDir); exit(1);
  }

  QCString imgExt = getDotImageExtension();
  QCString imgName = (QCString)outFile+"."+imgExt;
  QCString absImgName = d.absPath().utf8()+"/"+imgName;
  QCString absOutFile = d.absPath().utf8()+"/"+outFile;

  DotRunner dotRun(inFile,d.absPath().data(),QCString(),FALSE,absImgName);
  if (format==GOF_BITMAP)
    dotRun.addJob(DOT_IMAGE_FORMAT,absImgName);
  else // format==GOF_EPS
  {
    if (USE_PDFLATEX)
    {
      dotRun.addJob("pdf",absOutFile+".pdf",absOutFile);
    }
    else
    {
      dotRun.addJob("ps",absOutFile+".eps");
    }
  }

  dotRun.preventCleanUp();
  if (!dotRun.run())
  {
     return;
  }

  if (format==GOF_BITMAP) checkDotResult(getDotImageExtension(),absImgName);

  Doxygen::indexList->addImageFile(imgName);

}

 
/*! Writes user defined image map to the output.
 *  \param t text stream to write to
 *  \param inFile just the basename part of the filename
 *  \param outDir output directory
 *  \param relPath relative path the to root of the output dir
 *  \param baseName the base name of the output files
 *  \param context the scope in which this graph is found (for resolving links)
 *  \param graphId a unique id for this graph, use for dynamic sections
 */
void writeDotImageMapFromFile(FTextStream &t,
                            const QCString &inFile, const QCString &outDir,
                            const QCString &relPath, const QCString &baseName,
                            const QCString &context,int graphId)
{

  QDir d(outDir);
  if (!d.exists())
  {
    err("Output dir %s does not exist!\n",outDir.data()); exit(1);
  }

  QCString mapName = baseName+".map";
  QCString imgExt = getDotImageExtension();
  QCString imgName = baseName+"."+imgExt;
  QCString absOutFile = d.absPath().utf8()+"/"+mapName;

  DotRunner dotRun(inFile,d.absPath().data(),QCString(),FALSE);
  dotRun.addJob(MAP_CMD,absOutFile);
  dotRun.preventCleanUp();
  if (!dotRun.run())
  {
    return;
  }

  if (imgExt=="svg") // vector graphics
  {
    //writeSVGFigureLink(t,relPath,inFile,inFile+".svg");
    //DotFilePatcher patcher(inFile+".svg");
    QCString svgName=outDir+"/"+baseName+".svg";
    writeSVGFigureLink(t,relPath,baseName,svgName);
    DotFilePatcher patcher(svgName);
    patcher.addSVGConversion("",TRUE,context,TRUE,graphId);
    patcher.run();
  }
  else // bitmap graphics
  {
    QGString result;
    FTextStream tt(&result);

    t << "<img src=\"" << relPath << imgName << "\" alt=\""
      << imgName << "\" border=\"0\" usemap=\"#" << mapName << "\"/>" << endl;
    convertMapFile(tt, absOutFile, relPath ,TRUE, context);
    if (!result.isEmpty())
    {
      t << "<map name=\"" << mapName << "\" id=\"" << mapName << "\">";
      t << result;
      t << "</map>" << endl;
    }
  }
  d.remove(absOutFile);
}
