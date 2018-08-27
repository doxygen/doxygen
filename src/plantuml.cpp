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

#include "plantuml.h"
#include "portable.h"
#include "config.h"
#include "doxygen.h"
#include "index.h"
#include "message.h"
#include "util.h"

#include <qdir.h>
#include <string>
#include <map>

#include <dirent.h>

static const int maxCmdLine = 40960;

QCString writePlantUMLSource(const QCString &outDir,const QCString &fileName,const QCString &content)
{
  QCString baseName(4096);
  static int umlindex=1;

  if (fileName.isEmpty()) // generate name
  {
    baseName = outDir+"/inline_umlgraph_"+QCString().setNum(umlindex++);
  }
  else // user specified name
  {
    baseName = fileName;
    int i=baseName.findRev('.');
    if (i!=-1) baseName = baseName.left(i);
    baseName.prepend(outDir+"/");
  }
  QFile file(baseName+".pu");
  if (!file.open(IO_WriteOnly))
  {
    err("Could not open file %s for writing\n",baseName.data());
  }
  QCString text = "@startuml\n";
  text+=content;
  text+="\n@enduml\n";
  file.writeBlock( text, text.length() );
  file.close();
  return baseName;
}


struct comparer
{
    public:
    bool operator()(const std::string x, const std::string y) const
    {
         return x.compare(y)<0;
    }
};

extern void restoreCacheFromFile(std::string *path);
static std::map<std::string, std::string, comparer> *puMap = NULL;	//* plantuml Map < plantuml_contents , plantuml.[format] binary name> 
static std::map<std::string, int, comparer> *puDir = NULL;		//* directory Map

void generatePlantUMLOutput(const char *baseName,const char *outDir,PlantUMLOutputFormat format)
{
  static QCString plantumlJarPath = Config_getString(PLANTUML_JAR_PATH);
  static QCString plantumlConfigFile = Config_getString(PLANTUML_CFG_FILE);
  static QCString dotPath = Config_getString(DOT_PATH);

  if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
	  printf("config PLANTUML_CACHE : %d\n",Config_getBool(PLANTUML_CACHE));
    printf("config PLANTUML_CACHE_DEBUG_PRINT : %d\n",Config_getBool(PLANTUML_CACHE_DEBUG_PRINT));
    printf("config DOT_CLEANUP : %d\n",Config_getBool(DOT_CLEANUP));
  }

  QCString pumlExe = "java";
  QCString pumlArgs = "";

  QStrList &pumlIncludePathList = Config_getList(PLANTUML_INCLUDE_PATH);
  char *s=pumlIncludePathList.first();
  if (s)
  {
    pumlArgs += "-Dplantuml.include.path=\"";
    pumlArgs += s;
    s = pumlIncludePathList.next(); 
  }
  while (s)
  {
    pumlArgs += portable_pathListSeparator();
    pumlArgs += s;
    s = pumlIncludePathList.next(); 
  }
  if (pumlIncludePathList.first()) pumlArgs += "\" ";
  pumlArgs += "-Djava.awt.headless=true -jar \""+plantumlJarPath+"plantuml.jar\" ";
  if (!plantumlConfigFile.isEmpty())
  {
    pumlArgs += "-config \"";
    pumlArgs += plantumlConfigFile;
    pumlArgs += "\" ";
  }
  if (Config_getBool(HAVE_DOT) && !dotPath.isEmpty())
  {
    pumlArgs += "-graphvizdot \"";
    pumlArgs += dotPath;
    pumlArgs += "dot";
    pumlArgs += portable_commandExtension();
    pumlArgs += "\" ";
  }
  pumlArgs+="-o \"";
  pumlArgs+=outDir;
  pumlArgs+="\" ";
  QCString imgName = baseName;
  QCString imgExt = "";
  // The basename contains path, we need to strip the path from the filename in order
  // to create the image file name which should be included in the index.qhp (Qt help index file).
  int i;
  if ((i=imgName.findRev('/'))!=-1) // strip path
  {
    imgName=imgName.right(imgName.length()-i-1);
  }
  switch (format)
  {
    case PUML_BITMAP:
      pumlArgs+="-tpng";
      imgName+=".png";
      imgExt=".png";
      break;
    case PUML_EPS:
      pumlArgs+="-teps";
      imgName+=".eps";
      imgExt=".eps";
      break;
    case PUML_SVG:
      pumlArgs+="-tsvg";
      imgName+=".svg";
      imgExt=".svg";
      break;
  }
  pumlArgs+=" \"";
  pumlArgs+=baseName;
  pumlArgs+=".pu\" ";
  pumlArgs+="-charset UTF-8 ";
  int exitCode;
  msg("Running PlantUML on generated file %s.pu\n",baseName);
  if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
	  printf("*** running PlantUML on generated file %s.pu  ext=%s  img=%s\n",baseName,imgExt.data() , imgName.data());
  }

  std::string puData;
  std::size_t pos;
  bool puCacheMatchFlag = false;
  portable_sysTimerStart();
  if (Config_getBool(PLANTUML_CACHE)){
    if(puMap == NULL){
      puMap = new (std::map<std::string,std::string,comparer>);
      puDir = new (std::map<std::string,int,comparer>);
    }

    std::string dirname(baseName);
	  if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){ printf("*** basename : [%s]\n",baseName); }
    std::size_t found = dirname.find_last_of("/");
	  if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){ printf("*** path : [%s]\n",dirname.substr(0,found).c_str());  }		 // " file: " << str.substr(found+1)
    std::string path = dirname.substr(0,found);
    std::map<std::string, int, comparer>::iterator dir=puDir->find(path);
    if(dir == puDir->end()){
      (*puDir)[path] = 1;
      restoreCacheFromFile(&path);			// ex. *.[format].cache.pu			*.[format].cache
    } 

    QFile pu( QCString(baseName)+".pu");
    if(!pu.open(0x0011)){ 
      err("\n\n\nError : Can Not open %s%s file.\n\n\n",baseName,".pu");  
      return; 
    }
    puData = pu.readAll().data();
    pu.close();

	  if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
		  //printf("%s%s contents (readAll) = [[%s]]\n", baseName,".pu",puData.c_str());
	  }
    pos = puData.find("@enduml\n");
    if(pos != std::string::npos){
		  if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){ printf("*** @enduml pos:%lu\n",pos); }
      puData.erase (puData.begin()+pos+sizeof("@enduml"), puData.end());
      puData += imgExt;
	    if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
		    printf("*** key = {{%s}}\n", puData.c_str());
      }
      std::map<std::string, std::string, comparer>::iterator it=puMap->find(puData);
      if( (it!=puMap->end()) && (QFile(QCString(it->second.c_str())).exists()) ){
	      if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
		      printf("*** value = %s\n", it->second.c_str());
        }
        msg("Running copy  %s -> %s%s\n\n" , it->second.c_str(),baseName,imgExt.data() );
        //QFile(QCString(baseName)+imgExt).remove();
        if(copyFile( QCString(it->second.c_str()) , QCString(baseName)+imgExt ) == true){
          if (Config_getBool(DOT_CLEANUP)) {
            QFile(QCString(baseName)+".pu").remove();
          }
          puCacheMatchFlag = true;
        } else {
          err("\n\n\nError : copy fail  %s%s -> %s%s\n\n\n" , it->second.c_str(),imgExt.data() ,baseName,imgExt.data() );
        }
      }
    }
  }
  if (puCacheMatchFlag == false){
    msg("Running JAVA PlantUML on generated file %s\n", imgName.data());
	  if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
      printf("*** running: %s %s outDir:%s %s\n",pumlExe.data(),pumlArgs.data(),outDir,baseName);
    }
    if ((exitCode=portable_system(pumlExe,pumlArgs,TRUE))!=0)
    {
      err("Problems running PlantUML. Verify that the command 'java -jar \"%splantuml.jar\" -h' works from the command line. Exit code: %d\n",
          plantumlJarPath.data(),exitCode);
    }
    else {
      if (Config_getBool(PLANTUML_CACHE)){ 
        if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
          printf("*** create file : %s.cache%s.pu  and %s.cache%s\n",baseName,imgExt.data(),baseName,imgExt.data());
        }
        puMap->insert(std::pair<std::string,std::string>(puData,std::string(baseName)+imgExt.data()));
        copyFile( QCString(baseName)+".pu" , QCString(baseName)+imgExt+".cache.pu");           // ex. inline.._5.cache.png.pu
        copyFile( QCString(baseName)+imgExt , QCString(baseName)+imgExt+".cache");      // ex. inline.._5.cache.png
      }
      if (Config_getBool(DOT_CLEANUP))
      {
        QFile(QCString(baseName)+".pu").remove();
      } 
    }
  } 
  portable_sysTimerStop();

  if ( (format==PUML_EPS) && (Config_getBool(USE_PDFLATEX)) )
  {
    QCString epstopdfArgs(maxCmdLine);
    epstopdfArgs.sprintf("\"%s.eps\" --outfile=\"%s.pdf\"",baseName,baseName);
    portable_sysTimerStart();
    if ((exitCode=portable_system("epstopdf",epstopdfArgs))!=0)
    {
      err("Problems running epstopdf. Check your TeX installation! Exit code: %d\n",exitCode);
    }
    portable_sysTimerStop();
  }
  Doxygen::indexList->addImageFile(imgName);
}

void restoreCacheFromFile(std::string *path)   // ex. path+*.[format].cache.pu			path+*.[format].cache
{
  DIR* dirp;
  struct dirent* direntp;

  if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
    printf("====== start of restoreCacheFromFile()\n");
    printf( "*** restoreCacheFromFile path: %s\n", path->c_str());
  }

  dirp = opendir( path->c_str() );
  if( dirp == NULL ) {
    printf( "can't open %s" , path->c_str());
  } else {
    for(;;) {
      direntp = readdir( dirp );
      if( direntp == NULL ) break;
      std::string str(direntp->d_name);
      std::size_t found = str.rfind(".cache.pu");

      if(found != std::string::npos){
        if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
          printf( "*** matched filename : %s\n", direntp->d_name );
          printf( "*** extension : %s\n", str.substr(found-4,4).c_str());
        }
        std::string extension = str.substr(found-4,4);
        std::size_t myfound = str.find_last_of(".");
        str.erase(str.begin() + myfound,str.end());   // erase .pu
        //printf( "%lu %s\n", myfound , str.c_str() );
        std::string baseName = *path + "/" + str;   // path /  *.[format].cache
        //printf( "%s\n", baseName.c_str() );

        QFile pu( QCString(baseName.c_str())+".pu");
        if (!pu.open(0x0011)){ 
          err("\n\n\nError : Can Not open %s%s file.\n\n\n",baseName.c_str(), ".pu");  
          return; 
        }

        std::string puData = pu.readAll().data();
        pu.close();
        std::size_t pos;
        //printf("restoreCacheFromFile[[%s]]\n", puData.c_str());
        pos = puData.find("@enduml\n");
        if(pos != std::string::npos){
          //printf("pos:%lu\n",pos);
          puData.erase (puData.begin()+pos+sizeof("@enduml"), puData.end());
          puData += extension;
          //printf("restoreCacheFromFile{{%s}}\n", puData.c_str());
          std::map<std::string, std::string, comparer>::iterator it=puMap->find(puData);
          if( it == puMap->end() ){
            puMap->insert(std::pair<std::string,std::string>(puData,baseName));
          }
        }
        puData.clear();
      }
    }

    closedir( dirp );
  }

  if(Config_getBool(PLANTUML_CACHE_DEBUG_PRINT)){
    for(std::map<std::string, std::string, comparer>::iterator it = puMap->begin(); it != puMap->end(); it++) {
      printf("***key:[%s]\n",it->first.c_str());
      printf("***value:[%s]\n",it->second.c_str());
    }
    printf("====== end of restoreCacheFromFile()\n");
  }

}
