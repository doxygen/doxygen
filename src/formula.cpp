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

#include "formula.h"
#include "message.h"
#include "config.h"
#include "util.h"
#include "portable.h"
#include "image.h"
#include "fileinfo.h"
#include "dir.h"
#include "regex.h"
#include "linkedmap.h"

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <fstream>

// TODO: remove these dependencies
#include "doxygen.h"   // for Doxygen::indexList
#include "indexlist.h" // for Doxygen::indexList

static int determineInkscapeVersion(Dir &thisDir);

// Remove the temporary files
#define RM_TMP_FILES (true)
//#define RM_TMP_FILES (false)

struct FormulaManager::Private
{
  LinkedMap<Formula>      formulas;
  std::map<int,Formula *> formulaIdMap;
  bool                    repositoriesValid = true;
  StringVector            tempFiles;
};

FormulaManager::FormulaManager() : p(std::make_unique<Private>())
{
}

FormulaManager &FormulaManager::instance()
{
  static FormulaManager fm;
  return fm;
}

void FormulaManager::initFromRepository(const QCString &dir)
{
  std::ifstream f(dir.str()+"/formula.repository",std::ifstream::in);
  if (f.is_open())
  {
    uint formulaCount=0;
    msg("Reading formula repository...\n");
    std::string readLine;
    std::string line;
    std::string prefix("\\_form#");
    int lineNr;
    int nextLineNr=1;
    bool hasNextLine = !getline(f,readLine).fail();
    while (hasNextLine)
    {
      line = readLine;
      lineNr = nextLineNr;

      // look ahead a bit because a formula can be spread over several lines
      while ((hasNextLine = !getline(f,readLine).fail()))
      {
        nextLineNr+=1;
        if (!readLine.compare(0, prefix.size(), prefix)) break;
        line += "\n" + readLine;
      }

      // new format: \_form#<digits>=<digits>x<digits>:formula
      static const reg::Ex re_new(R"(\\_form#(\d+)=(\d+)x(\d+):)");
      // old format: \_form#<digits>:formula
      static const reg::Ex re_old(R"(\\_form#(\d+):)");

      reg::Match match;
      int id     = -1;
      int width  = -1;
      int height = -1;
      std::string text;
      if (reg::search(line,match,re_new)) // try new format first
      {
        id     = std::stoi(match[1].str());
        width  = std::stoi(match[2].str());
        height = std::stoi(match[3].str());
        text   = line.substr(match.position()+match.length());
        //printf("new format found id=%d width=%d height=%d text=%s\n",id,width,height,text.c_str());
      }
      else if (reg::search(line,match,re_old)) // check for old format
      {
        //id     = std::stoi(match[1].str());
        //text   = line.substr(match.position()+match.length());
        //printf("old format found id=%d text=%s\n",id,text.c_str());
        msg("old formula.repository format detected; forcing upgrade.\n");
        p->repositoriesValid = false;
        break;
      }
      else // unexpected content
      {
        warn_uncond("%s/formula.repository contains invalid content at line %d: found: '%s'\n",qPrint(dir),lineNr,line.c_str());
        p->repositoriesValid = false;
        break;
      }

      auto it = p->formulaIdMap.find(id);
      Formula *formula=0;
      if (it!=p->formulaIdMap.end()) // formula already found in a repository for another output format
      {
        formula = it->second;
        if (formula->text().str()!=text) // inconsistency between repositories detected
        {
          msg("differences detected between formula.repository files; forcing upgrade.\n");
          p->repositoriesValid = false;
          break;
        }
        formulaCount++;
      }
      else // create new formula from cache
      {
        //printf("formula not found adding it under id=%d\n",id);
        formula = p->formulas.add(text.c_str(),id,width,height);
        p->formulaIdMap.insert(std::make_pair(id,formula));
      }

      if (formula) // if an entry in the repository exists also check if there is a generated image
      {
        QCString formImgName;
        formImgName.sprintf("form_%d",formula->id());
        FileInfo fiPng((dir+"/"+formImgName+".png").str());
        FileInfo fiSvg((dir+"/"+formImgName+".svg").str());
        // mark formula as cached, so we do not need to regenerate the images
        bool isCached = fiPng.exists() || fiSvg.exists();
        formula->setCached(isCached);
        //printf("formula %d: cached=%d\n",formula->id(),isCached);

        FileInfo fiPngDark((dir+"/"+formImgName+"_dark.png").str());
        FileInfo fiSvgDark((dir+"/"+formImgName+"_dark.svg").str());
        bool isCachedDark = fiPngDark.exists() || fiSvgDark.exists();
        formula->setCachedDark(isCachedDark);
        //printf("formula %d: cachedDark=%d\n",formula->id(),isCachedDark);
      }
    }

    // For the first repository all formulas should be new (e.g. formulaCount==0).
    // For the other repositories the same number of formulas should be found
    // (and number of formulas should be the same for all repositories, content is already check above)
    if (formulaCount>0 && formulaCount!=p->formulas.size()) // inconsistency between repositories
    {
      msg("differences detected between formula.repository files; forcing upgrade.\n");
      p->repositoriesValid = false;
    }
  }
  else // no repository found for an output format
  {
    p->repositoriesValid = false;
  }
}

void FormulaManager::checkRepositories()
{
  //printf("checkRepositories valid=%d\n",p->repositoriesValid);
  if (!p->repositoriesValid)
  {
    clear(); // clear cached formulas, so the corresponding images and repository files
             // are regenerated
    p->repositoriesValid = true;
  }
}

void FormulaManager::createLatexFile(const QCString &fileName,Format format,Mode mode,IntVector &formulasToGenerate)
{
  QCString macroFile = Config_getString(FORMULA_MACROFILE);
  QCString stripMacroFile;
  if (!macroFile.isEmpty())
  {
    FileInfo fi(macroFile.str());
    macroFile=fi.absFilePath();
    stripMacroFile = fi.fileName();
  }

  // generate a latex file containing one formula per page.
  QCString texName=fileName+".tex";
  std::ofstream f(texName.str(),std::ofstream::out | std::ofstream::binary);
  if (f.is_open())
  {
    TextStream t(&f);
    t << "\\documentclass{article}\n";
    t << "\\usepackage{ifthen}\n";
    t << "\\usepackage{epsfig}\n"; // for those who want to include images
    t << "\\usepackage[utf8]{inputenc}\n"; // looks like some older distributions with newunicode package 1.1 need this option.
    t << "\\usepackage{xcolor}\n";

    if (mode==Mode::Dark) // invert page and text colors
    {
      t << "\\color{white}\n";
      t << "\\pagecolor{black}\n";
    }

    writeExtraLatexPackages(t);
    writeLatexSpecialFormulaChars(t);
    if (!macroFile.isEmpty())
    {
      copyFile(macroFile,stripMacroFile);
      t << "\\input{" << stripMacroFile << "}\n";
    }
    t << "\\pagestyle{empty}\n";
    t << "\\begin{document}\n";
    for (const auto &formula : p->formulas)
    {
      int id = formula->id();
      // only formulas for which no image is cached are generated
      //printf("check formula %d: cached=%d cachedDark=%d\n",formula->id(),formula->isCached(),formula->isCachedDark());
      if ((mode==Mode::Light && !formula->isCached()) ||
          (mode==Mode::Dark && !formula->isCachedDark())
         )
      {
        // we force a pagebreak after each formula
        t << formula->text() << "\n\\pagebreak\n\n";
        formulasToGenerate.push_back(id);
      }
      QCString resultName;
      resultName.sprintf("form_%d%s.%s",id, mode==Mode::Light?"":"_dark", format==Format::Vector?"svg":"png");
      Doxygen::indexList->addImageFile(resultName);
    }
    t << "\\end{document}\n";
    t.flush();
    f.close();
  }
}

static bool createDVIFile(const QCString &fileName)
{
  QCString latexCmd = "latex";
  char args[4096];
  Portable::sysTimerStart();
  int rerunCount=1;
  while (rerunCount<8)
  {
    //printf("Running latex...\n");
    sprintf(args,"-interaction=batchmode %s >%s",qPrint(fileName),Portable::devNull());
    if ((Portable::system(latexCmd,args)!=0) || (Portable::system(latexCmd,args)!=0))
    {
      err("Problems running latex. Check your installation or look "
          "for typos in %s.tex and check %s.log!\n",qPrint(fileName),qPrint(fileName));
      Portable::sysTimerStop();
      return false;
    }
    // check the log file if we need to run latex again to resolve references
    QCString logFile = fileToString(fileName+".log");
    if (logFile.isEmpty() ||
        (logFile.find("Rerun to get cross-references right")==-1 && logFile.find("Rerun LaTeX")==-1))
    {
      break;
    }
    rerunCount++;
  }
  Portable::sysTimerStop();
  return true;
}

static bool createPostscriptFile(const QCString &fileName,const QCString &formBase,int pageIndex)
{
  char args[4096];
  // run dvips to convert the page with number pageIndex to an
  // postscript file.
  sprintf(args,"-q -D 600 -n 1 -p %d -o %s_tmp.ps %s.dvi",pageIndex,qPrint(formBase),qPrint(fileName));
  Portable::sysTimerStart();
  if (Portable::system("dvips",args)!=0)
  {
    err("Problems running dvips. Check your installation!\n");
    Portable::sysTimerStop();
    return false;
  }
  Portable::sysTimerStop();
  return true;
}

static bool createEPSbboxFile(const QCString &formBase)
{
  char args[4096];
  // extract the bounding box for the postscript file
  sprintf(args,"-q -dBATCH -dNOPAUSE -P- -dNOSAFER -sDEVICE=bbox %s_tmp.ps 2>%s_tmp.epsi",
      qPrint(formBase),qPrint(formBase));
  Portable::sysTimerStart();
  if (Portable::system(Portable::ghostScriptCommand(),args)!=0)
  {
    err("Problems running %s. Check your installation!\n",Portable::ghostScriptCommand());
    Portable::sysTimerStop();
    return false;
  }
  Portable::sysTimerStop();
  return true;
}

static bool extractBoundingBox(const QCString &formBase,
            int *x1,int *y1,int *x2,int *y2,
            double *x1hi,double *y1hi,double *x2hi,double *y2hi)
{
  FileInfo fi((formBase+"_tmp.epsi").str());
  if (fi.exists())
  {
    QCString eps = fileToString(formBase+"_tmp.epsi");
    int i = eps.find("%%BoundingBox:");
    if (i!=-1)
    {
      sscanf(eps.data()+i,"%%%%BoundingBox:%d %d %d %d",x1,y1,x2,y2);
    }
    else
    {
      err("Couldn't extract bounding box from %s_tmp.epsi",qPrint(formBase));
      return false;
    }
    i = eps.find("%%HiResBoundingBox:");
    if (i!=-1)
    {
      sscanf(eps.data()+i,"%%%%HiResBoundingBox:%lf %lf %lf %lf",x1hi,y1hi,x2hi,y2hi);
    }
    else
    {
      err("Couldn't extract high resolution bounding box from %s_tmp.epsi",qPrint(formBase));
      return false;
    }
  }
  //printf("Bounding box [%d %d %d %d]\n",x1,y1,x2,y2);
  return true;
}

double FormulaManager::updateFormulaSize(int pageNum,int x1,int y1,int x2,int y2)
{
  double scaleFactor = 1.25;
  int zoomFactor = Config_getInt(FORMULA_FONTSIZE);
  if (zoomFactor<8 || zoomFactor>50) zoomFactor=10;
  scaleFactor *= zoomFactor/10.0;

  auto it = p->formulaIdMap.find(pageNum);
  if (it!=p->formulaIdMap.end())
  {
    Formula *formula = it->second;
    formula->setWidth(static_cast<int>((x2-x1)*scaleFactor+0.5));
    formula->setHeight(static_cast<int>((y2-y1)*scaleFactor+0.5));
  }
  return scaleFactor;
}

static bool createCroppedPDF(const QCString &formBase,int x1,int y1,int x2,int y2)
{
  char args[4096];
  // crop the image to its bounding box
  sprintf(args,"-q -dBATCH -dNOPAUSE -P- -dNOSAFER -sDEVICE=pdfwrite"
              " -o %s_tmp.pdf -c \"[/CropBox [%d %d %d %d] /PAGES pdfmark\" -f %s_tmp.ps",
      qPrint(formBase),x1,y1,x2,y2,qPrint(formBase));
  Portable::sysTimerStart();
  if (Portable::system(Portable::ghostScriptCommand(),args)!=0)
  {
    err("Problems running %s. Check your installation!\n",Portable::ghostScriptCommand());
    Portable::sysTimerStop();
    return false;
  }
  Portable::sysTimerStop();
  return true;
}

static bool createCroppedEPS(const QCString &formBase)
{
  char args[4096];
  // crop the image to its bounding box
  sprintf(args,"-q -dBATCH -dNOPAUSE -P- -dNOSAFER -sDEVICE=eps2write"
              " -o %s_tmp.eps -f %s_tmp.ps",qPrint(formBase),qPrint(formBase));
  Portable::sysTimerStart();
  if (Portable::system(Portable::ghostScriptCommand(),args)!=0)
  {
    err("Problems running %s. Check your installation!\n",Portable::ghostScriptCommand());
    Portable::sysTimerStop();
    return false;
  }
  return true;
}

static bool createSVGFromPDF(const QCString &formBase,const QCString &outFile)
{
  char args[4096];
  sprintf(args,"%s_tmp.pdf %s",qPrint(formBase),qPrint(outFile));
  Portable::sysTimerStart();
  if (Portable::system("pdf2svg",args)!=0)
  {
    err("Problems running pdf2svg. Check your installation!\n");
    Portable::sysTimerStop();
    return false;
  }
  Portable::sysTimerStop();
  return true;
}

static bool createSVGFromPDFviaInkscape(Dir &thisDir,const QCString &formBase,const QCString &outFile)
{
  char args[4096];
  int inkscapeVersion = determineInkscapeVersion(thisDir);
  if (inkscapeVersion == -1)
  {
    err("Problems determining the version of inkscape. Check your installation!\n");
    return false;
  }
  else if (inkscapeVersion == 0)
  {
    sprintf(args,"-l %s -z %s_tmp.pdf 2>%s",qPrint(outFile),qPrint(formBase),Portable::devNull());
  }
  else // inkscapeVersion >= 1
  {
    sprintf(args,"--export-type=svg --export-filename=%s %s_tmp.pdf 2>%s",qPrint(outFile),qPrint(formBase),Portable::devNull());
  }
  Portable::sysTimerStart();
  if (Portable::system("inkscape",args)!=0)
  {
    err("Problems running inkscape. Check your installation!\n");
    Portable::sysTimerStop();
    return false;
  }
  Portable::sysTimerStop();
  return true;
}


static bool updateEPSBoundingBox(const QCString &formBase,
                                 int x1,int y1,int x2,int y2,
                                 double x1hi,double y1hi,double x2hi,double y2hi)
{
  // read back %s_tmp.eps and replace
  // bounding box values with x1,y1,x2,y2 and remove the HiResBoundingBox
  std::ifstream epsIn(formBase.str()+"_tmp.eps",std::ifstream::in);
  std::ofstream epsOut(formBase.str()+"_tmp_corr.eps",std::ofstream::out | std::ofstream::binary);
  if (epsIn.is_open() && epsOut.is_open())
  {
    std::string line;
    while (getline(epsIn,line))
    {
      if (line.rfind("%%BoundingBox",0)==0)
      {
        epsOut << "%%BoundingBox: " << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";
      }
      else if (line.rfind("%%HiResBoundingBox",0)==0)
      {
        epsOut << "%%HiResBoundingBox: " << x1hi << " " << y1hi << " " << x2hi << " " << y2hi << "\n";
      }
      else
      {
        epsOut << line << "\n";
      }
    }
    epsIn.close();
    epsOut.close();
  }
  else
  {
    err("Problems correcting the eps files from %s_tmp.eps to %s_tmp_corr.eps\n",
        qPrint(formBase),qPrint(formBase));
    return false;
  }
  return true;
}

static bool createPNG(const QCString &formBase,const QCString &outFile,double scaleFactor)
{
  char args[4096];
  Portable::sysTimerStop();
  sprintf(args,"-q -dNOSAFER -dBATCH -dNOPAUSE -dEPSCrop -sDEVICE=pnggray -dGraphicsAlphaBits=4 -dTextAlphaBits=4 "
               "-r%d -sOutputFile=%s %s_tmp_corr.eps",static_cast<int>(scaleFactor*72),qPrint(outFile),qPrint(formBase));
  Portable::sysTimerStart();
  if (Portable::system(Portable::ghostScriptCommand(),args)!=0)
  {
    err("Problems running %s. Check your installation!\n",Portable::ghostScriptCommand());
    Portable::sysTimerStop();
    return false;
  }
  Portable::sysTimerStop();
  return true;
}

void FormulaManager::createFormulasTexFile(Dir &thisDir,Format format,HighDPI hd,Mode mode)
{
  IntVector formulasToGenerate;
  QCString formulaFileName = mode==Mode::Light ? "_formulas" : "_formulas_dark";
  createLatexFile(formulaFileName,format,mode,formulasToGenerate);

  if (!formulasToGenerate.empty()) // there are new formulas
  {
    if (!createDVIFile(formulaFileName)) return;

    //printf("Running dvips...\n");
    int pageIndex=1;
    for (int pageNum : formulasToGenerate)
    {
      QCString outputFile;
      outputFile.sprintf("form_%d%s.%s",pageNum, mode==Mode::Light?"":"_dark", format==Format::Vector?"svg":"png");
      msg("Generating image %s for formula\n",qPrint(outputFile));

      QCString formBase;
      formBase.sprintf("_form%d%s",pageNum,mode==Mode::Light?"":"_dark");

      if (!createPostscriptFile(formulaFileName,formBase,pageIndex)) break;

      int x1=0,y1=0,x2=0,y2=0;
      double x1hi=0.0,y1hi=0.0,x2hi=0.0,y2hi=0.0;
      if (mode==Mode::Light)
      {
        if (!createEPSbboxFile(formBase)) break;
        // extract the bounding box info from the generated .epsi file
        if (!extractBoundingBox(formBase,&x1,&y1,&x2,&y2,&x1hi,&y1hi,&x2hi,&y2hi)) break;
      }
      else // for dark images the bounding box is wrong (includes the black) so
           // use the bounding box of the light image instead.
      {
        QCString formBaseLight;
        formBaseLight.sprintf("_form%d",pageNum);
        if (!extractBoundingBox(formBaseLight,&x1,&y1,&x2,&y2,&x1hi,&y1hi,&x2hi,&y2hi)) break;
      }

      // convert the corrected EPS to a bitmap
      double scaleFactor = updateFormulaSize(pageNum,x1,y1,x2,y2);

      if (format==Format::Vector)
      {
        if (!createCroppedPDF(formBase,x1,y1,x2,y2)) break;

        // if we have pdf2svg available use it to create a SVG image
        if (Portable::checkForExecutable("pdf2svg"))
        {
          createSVGFromPDF(formBase,outputFile);
        }
        else if (Portable::checkForExecutable("inkscape")) // alternative is to use inkscape
        {
          createSVGFromPDFviaInkscape(thisDir,formBase,outputFile);
        }
        else
        {
          err("Neither 'pdf2svg' nor 'inkscape' present for conversion of formula to 'svg'\n");
          return;
        }

        p->tempFiles.push_back(formBase.str()+"_tmp.pdf");
      }
      else // format==Format::Bitmap
      {
        if (!createCroppedEPS(formBase)) break;

        if (!updateEPSBoundingBox(formBase,x1,y1,x2,y2,x1hi,y1hi,x2hi,y2hi)) break;

        if (hd==HighDPI::On) // for high DPI display it looks much better if the
                             // image resolution is higher than the display resolution
        {
          scaleFactor*=2;
        }

        if (!createPNG(formBase,outputFile,scaleFactor)) break;

        p->tempFiles.push_back(formBase.str()+"_tmp.eps");
        p->tempFiles.push_back(formBase.str()+"_tmp_corr.eps");
      }

      // remove intermediate image files
      p->tempFiles.push_back(formBase.str()+"_tmp.ps");
      if (mode==Mode::Light)
      {
        p->tempFiles.push_back(formBase.str()+"_tmp.epsi");
      }
      pageIndex++;
    }
    // remove intermediate files produced by latex
    p->tempFiles.push_back(formulaFileName.str()+".dvi");
    p->tempFiles.push_back(formulaFileName.str()+".log");
    p->tempFiles.push_back(formulaFileName.str()+".aux");
  }
  // remove the latex file itself
  p->tempFiles.push_back(formulaFileName.str()+".tex");

  // write/update the formula repository so we know what text the
  // generated images represent (we use this next time to avoid regeneration
  // of the images, and to avoid forcing the user to delete all images in order
  // to let a browser refresh the images).
  std::ofstream f;
  f.open("formula.repository",std::ofstream::out | std::ofstream::binary);
  if (f.is_open())
  {
    TextStream t(&f);
    for (const auto &formula : p->formulas)
    {
      t << "\\_form#" << formula->id();
      if (formula->width()!=-1 && formula->height()!=-1)
      {
        t << "=" << formula->width() << "x" << formula->height();
      }
      t << ":" << formula->text() << "\n";
    }
  }
}

void FormulaManager::generateImages(const QCString &path,Format format,HighDPI hd)
{
  Dir d(path.str());
  // store the original directory
  if (!d.exists())
  {
    term("Output directory '%s' does not exist!\n",qPrint(path));
  }
  std::string oldDir = Dir::currentDirPath();

  // go to the html output directory (i.e. path)
  Dir::setCurrent(d.absPath());
  Dir thisDir;

  createFormulasTexFile(thisDir,format,hd,Mode::Light);
  if (Config_getEnum(HTML_COLORSTYLE)!=HTML_COLORSTYLE_t::LIGHT) // all modes other than light need a dark version
  {
    // note that the dark version reuses the bounding box of the light version so it needs to be
    // created after the light version.
    createFormulasTexFile(thisDir,format,hd,Mode::Dark);
  }

  // clean up temporary files
  if (RM_TMP_FILES)
  {
    for (const auto &file : p->tempFiles)
    {
      thisDir.remove(file);
    }
  }

  // reset the directory to the original location.
  Dir::setCurrent(oldDir);
}

void FormulaManager::clear()
{
  p->formulas.clear();
  p->formulaIdMap.clear();
}

int FormulaManager::addFormula(const std::string &formulaText,int width,int height)
{
  Formula *formula = p->formulas.find(formulaText);
  if (formula) // same formula already stored
  {
    return formula->id();
  }
  // add new formula
  int id = static_cast<int>(p->formulas.size());
  formula = p->formulas.add(formulaText.c_str(),id,width,height);
  p->formulaIdMap.insert(std::make_pair(id,formula));
  return id;
}

const Formula *FormulaManager::findFormula(int formulaId) const
{
  auto   it  = p->formulaIdMap.find(formulaId);
  return it != p->formulaIdMap.end() ? it->second : nullptr;
}

#if 0
Formula *FormulaManager::findFormula(int formulaId)
{
  auto   it  = p->formulaIdMap.find(formulaId);
  return it != p->formulaIdMap.end() ? it->second : nullptr;
}
#endif


bool FormulaManager::hasFormulas() const
{
  return !p->formulas.empty();
}

// helper function to detect and return the major version of inkscape.
// return -1 if the version cannot be determined.
static int determineInkscapeVersion(Dir &thisDir)
{
  // The command line interface (CLI) of Inkscape 1.0 has changed in comparison to
  // previous versions. In order to invokine Inkscape, the used version is detected
  // and based on the version the right syntax of the CLI is chosen.
  static int inkscapeVersion = -2;
  if (inkscapeVersion == -2) // initial one time version check
  {
    QCString inkscapeVersionFile = "inkscape_version" ;
    inkscapeVersion = -1;
    QCString args = "-z --version >"+inkscapeVersionFile+" 2>"+Portable::devNull();
    Portable::sysTimerStart();
    if (Portable::system("inkscape",args)!=0)
    {
      // looks like the old syntax gave problems, lets try the new syntax
      args = " --version >"+inkscapeVersionFile+" 2>"+Portable::devNull();
      if (Portable::system("inkscape",args)!=0)
      {
        Portable::sysTimerStop();
        return -1;
      }
    }
    // read version file and determine major version
    std::ifstream inkscapeVersionIn(inkscapeVersionFile.str(),std::ifstream::in);
    if (inkscapeVersionIn.is_open())
    {
      std::string line;
      while (getline(inkscapeVersionIn,line))
      {
        size_t dotPos = line.find('.');
        if (line.rfind("Inkscape ",0)==0 && dotPos>0)
        {
          // get major version
          inkscapeVersion = std::stoi(line.substr(9,dotPos-9));
          break;
        }
      }
      inkscapeVersionIn.close();
    }
    else // failed to open version file
    {
      Portable::sysTimerStop();
      return -1;
    }
    if (RM_TMP_FILES)
    {
      thisDir.remove(inkscapeVersionFile.str());
    }
    Portable::sysTimerStop();
  }
  return inkscapeVersion;
}
