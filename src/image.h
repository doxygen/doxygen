/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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

#ifndef _IMAGE_H
#define _IMAGE_H
#include <qglobal.h>

class Image
{
  public:
    Image(int w,int h);
   ~Image();
   
    void setPixel(int x,int y,uchar val);
    uchar getPixel(int x,int y) const;
    void writeChar(int x,int y,char c,uchar fg);
    void writeString(int x,int y,const char *s,uchar fg);
    void drawHorzLine(int y,int xs,int xe,uchar colIndex,uint mask);
    void drawHorzArrow(int y,int xs,int xe,uchar colIndex,uint mask);
    void drawVertLine(int x,int ys,int ye,uchar colIndex,uint mask);
    void drawVertArrow(int x,int ys,int ye,uchar colIndex,uint mask);
    void drawRect(int x,int y,int width,int height,uchar colIndex,uint mask);
    void fillRect(int x,int y,int width,int height,uchar colIndex,uint mask);
    bool save(const char *fileName,int mode=0);
    friend uint stringLength(const char *s);
    uint getWidth() const { return width; }
    uint getHeight() const { return height; }
    uchar *getData() const { return data; }
    static uint stringLength(const char *s);
 
  private:
    int width;
    int height;
    uchar *data;
};

#endif
