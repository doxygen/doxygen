/******************************************************************************
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
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qdir.h>

#include "formula.h"
#include "image.h"
#include "util.h"
#include "message.h"
#include "config.h"
#include "portable.h"
#include "index.h"
#include "doxygen.h"
#include "ftextstream.h"

Formula::Formula(const char *text, bool fig)
{
  static int form_count=0;
  static int fig_count=0;
  if (fig)
  {
    number = fig_count++;
  }
  else
  {
    number = form_count++;
  }
  form=text;
}

Formula::~Formula()
{
}

int Formula::getId()
{
  return number;
}

void FormulaList::generateBitmaps(const char *path, bool fig, bool rtf)
{
  QCString    texBaseName;
  const char *pngPreName;
  const char *formPreName;
  const char *repository;
  if (fig)
  {
    texBaseName="_figures";
    pngPreName="figform_";
    formPreName="_figform";
    repository = "figure.repository";
  }
  else
  {
    texBaseName="_formulas";
    pngPreName="form_";
    formPreName="_form";
    repository = "formula.repository";
  }
  int x1,y1,x2,y2;
  QDir d(path);
  // store the original directory
  if (!d.exists()) { err("Output dir %s does not exist!\n",path); exit(1); }
  QCString oldDir = QDir::currentDirPath().utf8();
  // go to the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;
  // generate a latex file containing one formula per page.
  QList<int> pagesToGenerate;
  pagesToGenerate.setAutoDelete(TRUE);
  FormulaListIterator fli(*this);
  Formula *formula;
  QFile f(texBaseName+".tex");
  bool formulaError=FALSE;
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    if (Config_getBool(LATEX_BATCHMODE)) t << "\\batchmode" << endl;
    t << "\\documentclass{article}" << endl;
    t << "\\usepackage{ifthen}" << endl;
    t << "\\usepackage{epsfig}" << endl; // for those who want to include images
    t << "\\usepackage[utf8]{inputenc}" << endl; // looks like some older distributions with newunicode package 1.1 need this option.
    writeExtraLatexPackages(t);
    writeLatexSpecialFormulaChars(t);
    t << "\\pagestyle{empty}" << endl; 
    t << "\\begin{document}" << endl;
    int page=0;
    for (fli.toFirst();(formula=fli.current());++fli)
    {
      QCString resultName;
      resultName.sprintf("%s%d.png",pngPreName,formula->getId());
      // only formulas for which no image exists are generated
      QFileInfo fi(resultName);
      if (!fi.exists())
      {
        // we force a pagebreak after each formula
        t << formula->getFormulaText() << endl << "\\pagebreak\n\n";
        pagesToGenerate.append(new int(page));
      }
      Doxygen::indexList->addImageFile(resultName);
      page++;
    }
    t << "\\end{document}" << endl;
    f.close();
  }
  if (pagesToGenerate.count()>0) // there are new formulas
  {
    //printf("Running latex...\n");
    //system("latex _formulas.tex </dev/null >/dev/null");
    QCString latexCmd = "latex";
    portable_sysTimerStart();
    if (portable_system(latexCmd,(texBaseName+".tex").data())!=0)
    {
      err("Problems running latex. Check your installation or look "
          "for typos in %s and check %s!\n",(texBaseName+".tex").data(),(texBaseName+".log").data());
      formulaError=TRUE;
      //return;
    }
    portable_sysTimerStop();
    //printf("Running dvips...\n");
    QListIterator<int> pli(pagesToGenerate);
    int *pagePtr;
    int pageIndex=1;
    for (;(pagePtr=pli.current());++pli,++pageIndex)
    {
      int pageNum=*pagePtr;
      msg("Generating image %s%d.png for formula\n",pngPreName,pageNum);
      char dviArgs[4096];
      char psArgs[4096];
      QCString formBase;
      formBase.sprintf("%s%d",formPreName,pageNum);
      // run dvips to convert the page with number pageIndex to an
      // postscript file.
      sprintf(dviArgs,"-q -D 600 -n 1 -p %d -o %s_tmp.ps %s",
          pageIndex,formBase.data(),(texBaseName+".dvi").data());
      portable_sysTimerStart();
      if (portable_system("dvips",dviArgs)!=0)
      {
        err("Problems running dvips. Check your installation!\n");
        portable_sysTimerStop();
        QDir::setCurrent(oldDir);
        return;
      }
      portable_sysTimerStop();
      // run ps2epsi to convert to an encapsulated postscript file with
      // boundingbox (dvips with -E has some problems here).
      sprintf(psArgs,"%s_tmp.ps %s.eps",formBase.data(),formBase.data()); 
      portable_sysTimerStart();
      if (portable_system("ps2epsi",psArgs)!=0)
      {
        err("Problems running ps2epsi. Check your installation!\n");
        portable_sysTimerStop();
        QDir::setCurrent(oldDir);
        return;
      }
      portable_sysTimerStop();
      // now we read the generated postscript file to extract the bounding box
      QFileInfo fi(formBase+".eps");
      if (fi.exists())
      {
        QCString eps = fileToString(formBase+".eps");
        int i=eps.find("%%BoundingBox:");
        if (i!=-1)
        {
          sscanf(eps.data()+i,"%%%%BoundingBox:%d %d %d %d",&x1,&y1,&x2,&y2);
        }
        else
        {
          err("Couldn't extract bounding box!\n");
        }
      } 

      char gsArgs[4096];
      // scale the image so that it is four times larger than needed.
      // and the sizes are a multiple of four.
      double scaleFactor = 16.0/3.0; 
      int zoomFactor = Config_getInt(FORMULA_FONTSIZE);
      if (zoomFactor<8 || zoomFactor>50) zoomFactor=10;
      scaleFactor *= zoomFactor/10.0;
      int gx = (((int)((x2-x1)*scaleFactor))+3)&~1;
      int gy = (((int)((y2-y1)*scaleFactor))+3)&~1;
      if (!fig)
      {
        // next we generate a postscript file which contains the eps
        // and displays it in the right colors and the right bounding box
        f.setName(formBase+".ps");
        if (f.open(IO_WriteOnly))
        {
          FTextStream t(&f);
          t << "1 1 1 setrgbcolor" << endl;  // anti-alias to white background
          t << "newpath" << endl;
          t << "-1 -1 moveto" << endl;
          t << (x2-x1+2) << " -1 lineto" << endl;
          t << (x2-x1+2) << " " << (y2-y1+2) << " lineto" << endl;
          t << "-1 " << (y2-y1+2) << " lineto" <<endl;
          t << "closepath" << endl;
          t << "fill" << endl;
          t << -x1 << " " << -y1 << " translate" << endl;
          t << "0 0 0 setrgbcolor" << endl;
          t << "(" << formBase << ".eps) run" << endl;
          f.close();
        }
        // Then we run ghostscript to convert the postscript to a pixmap
        // The pixmap is a truecolor image, where only black and white are
        // used.  
  
        sprintf(gsArgs,"-q -g%dx%d -r%dx%d -sDEVICE=ppmraw "
                      "-sOutputFile=%s.pnm -dNOPAUSE -dBATCH -- %s.ps",
                      gx,gy,(int)(scaleFactor*72),(int)(scaleFactor*72),
                      formBase.data(),formBase.data()
               );
      }
      else
      {
	if (!rtf)
	{
	  gx /= 4;
	  gy /= 4;
	}
        sprintf(gsArgs,"-q -g%dx%d -r%dx%d -sDEVICE=pngalpha "
                      "-sOutputFile=%s%d.png -dNOPAUSE -dEPSFitPage -dBATCH -- %s.eps",
                      gx,gy,(int)(scaleFactor*72),(int)(scaleFactor*72),
		      pngPreName,pageNum,formBase.data()
               );
      }
      portable_sysTimerStart();
      if (portable_system(portable_ghostScriptCommand(),gsArgs)!=0)
      {
        err("Problem running ghostscript %s %s. Check your installation!\n",portable_ghostScriptCommand(),gsArgs);
        portable_sysTimerStop();
        QDir::setCurrent(oldDir);
        return;
      }
      portable_sysTimerStop();
      if(!fig)
      {
        f.setName(formBase+".pnm");
        uint imageX=0,imageY=0;
        // we read the generated image again, to obtain the pixel data.
        if (f.open(IO_ReadOnly))
        {
          QTextStream t(&f);
          QCString s;
          if (!t.eof())
            s=t.readLine().utf8();
          if (s.length()<2 || s.left(2)!="P6")
            err("ghostscript produced an illegal image format!");
          else
          {
            // assume the size is after the first line that does not start with
            // # excluding the first line of the file.
            while (!t.eof() && (s=t.readLine().utf8()) && !s.isEmpty() && s.at(0)=='#') { }
            sscanf(s,"%d %d",&imageX,&imageY);
          }
          if (imageX>0 && imageY>0)
          {
            //printf("Converting image...\n");
            char *data = new char[imageX*imageY*3]; // rgb 8:8:8 format
            uint i,x,y,ix,iy;
            f.readBlock(data,imageX*imageY*3);
            Image srcImage(imageX,imageY),
                  filteredImage(imageX,imageY),
                  dstImage(imageX/4,imageY/4);
            uchar *ps=srcImage.getData();
            // convert image to black (1) and white (0) index.
            for (i=0;i<imageX*imageY;i++) *ps++= (data[i*3]==0 ? 1 : 0);
            // apply a simple box filter to the image 
            static int filterMask[]={1,2,1,2,8,2,1,2,1};
            for (y=0;y<srcImage.getHeight();y++)
            {
              for (x=0;x<srcImage.getWidth();x++)
              {
                int s=0;
                for (iy=0;iy<2;iy++)
                {
                  for (ix=0;ix<2;ix++)
                  {
                    s+=srcImage.getPixel(x+ix-1,y+iy-1)*filterMask[iy*3+ix];
                  }
                }
                filteredImage.setPixel(x,y,s);
              }
            }
            // down-sample the image to 1/16th of the area using 16 gray scale
            // colors.
            // TODO: optimize this code.
            for (y=0;y<dstImage.getHeight();y++)
            {
              for (x=0;x<dstImage.getWidth();x++)
              {
                int xp=x<<2;
                int yp=y<<2;
                int c=filteredImage.getPixel(xp+0,yp+0)+
                      filteredImage.getPixel(xp+1,yp+0)+
                      filteredImage.getPixel(xp+2,yp+0)+
                      filteredImage.getPixel(xp+3,yp+0)+
                      filteredImage.getPixel(xp+0,yp+1)+
                      filteredImage.getPixel(xp+1,yp+1)+
                      filteredImage.getPixel(xp+2,yp+1)+
                      filteredImage.getPixel(xp+3,yp+1)+
                      filteredImage.getPixel(xp+0,yp+2)+
                      filteredImage.getPixel(xp+1,yp+2)+
                      filteredImage.getPixel(xp+2,yp+2)+
                      filteredImage.getPixel(xp+3,yp+2)+
                      filteredImage.getPixel(xp+0,yp+3)+
                      filteredImage.getPixel(xp+1,yp+3)+
                      filteredImage.getPixel(xp+2,yp+3)+
                      filteredImage.getPixel(xp+3,yp+3);
                // here we scale and clip the color value so the
                // resulting image has a reasonable contrast
                dstImage.setPixel(x,y,QMIN(15,(c*15)/(16*10)));
              }
            }
            // save the result as a bitmap
            QCString resultName;
            resultName.sprintf("%s%d.png",pngPreName,pageNum);
            // the option parameter 1 is used here as a temporary hack
            // to select the right color palette! 
            dstImage.save(resultName,1);
            delete[] data;
          }
          f.close();
        } 
        // remove intermediate image files
        thisDir.remove(formBase+".pnm");
        thisDir.remove(formBase+".ps");
      }
      // remove intermediate image files
      thisDir.remove(formBase+"_tmp.ps");
      thisDir.remove(formBase+".eps");
    }
    // remove intermediate files produced by latex
    thisDir.remove((texBaseName+".dvi").data());
    if (!formulaError) thisDir.remove((texBaseName+".log").data()); // keep file in case of errors
    thisDir.remove((texBaseName+".aux").data());
  }
  // remove the latex file itself
  if (!formulaError) thisDir.remove((texBaseName+".tex").data());
  // write/update the formula repository so we know what text the 
  // generated images represent (we use this next time to avoid regeneration
  // of the images, and to avoid forcing the user to delete all images in order
  // to let a browser refresh the images).
  f.setName(repository);
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    for (fli.toFirst();(formula=fli.current());++fli)
    {
      if (fig)
      {
        t << "\\figform#" << formula->getId() << ":" << formula->getFormulaText() << endl;
      }
      else
      {
        t << "\\form#" << formula->getId() << ":" << formula->getFormulaText() << endl;
      }
    }
    f.close();
  }
  // reset the directory to the original location.
  QDir::setCurrent(oldDir);
}


#ifdef FORMULA_TEST
int main()
{
  FormulaList fl;
  fl.append(new Formula("$x^2$"));
  fl.append(new Formula("$y^2$"));
  fl.append(new Formula("$\\sqrt{x_0^2+x_1^2+x_2^2}$"));
  fl.generateBitmaps("dest");
  return 0;
}
#endif
