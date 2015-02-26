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

class QCString;

/** Plant UML output image formats */
enum PlantUMLOutputFormat { PUML_BITMAP, PUML_EPS, PUML_SVG };

/** Write a PlantUML compatible file.
 *  @param[in] outDir   the output directory to write the file to.
 *  @param[in] fileName the name of the file. If empty a name will be chosen automatically.
 *  @param[in] content  the contents of the PlantUML file.
 *  @returns The name of the generated file.
 */
QCString writePlantUMLSource(const QCString &outDir,const QCString &fileName,const QCString &content);

/** Convert a PlantUML file to an image.
 *  @param[in] baseName the name of the generated file (as returned by writePlantUMLSource())
 *  @param[in] outDir   the directory to write the resulting image into.
 *  @param[in] format   the image format to generate.
 */
void generatePlantUMLOutput(const char *baseName,const char *outDir,PlantUMLOutputFormat format);

#endif

