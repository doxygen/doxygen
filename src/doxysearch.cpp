/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

// includes

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#ifdef PROFILING
#include <sys/time.h>
#include <unistd.h>
#endif

// defines

#ifndef bool
#define bool int
#endif

#define MAXSTRLEN 1024
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define NORMAL  1    // OR search results
#define INCLUDE 2    // AND search results
#define EXCLUDE 3    // AND NOT search results

#define TERMMASK (1<<31)
#define OFFSETMASK (~TERMMASK)

// structs

//----------------------------------------------------------------------------

struct FileInfo
{
  FileInfo() { f=0; url=0; }
 ~FileInfo() { if (f) fclose(f); 
               delete[] url;
             }
  FILE *f;
  int index;
  int refOffset;
  char *url;
  FileInfo *next;
};

//----------------------------------------------------------------------------

struct FileList
{
  FileList() { first=0; last=0; index=0; }
 ~FileList() { FileInfo *fi=first;
               while (fi) { FileInfo *ofi=fi; fi=fi->next; delete ofi; }
             }
  FileInfo *add()
             {
               FileInfo *nf=new FileInfo;
               nf->next=0;
               nf->index=index++;
               if (last) { last->next=nf; last=nf; } else { first=nf; last=nf; }
               return nf;
             }
  FileInfo *first;
  FileInfo *last;
  int index;
};

//----------------------------------------------------------------------------

struct WordInfo
{
  WordInfo() { word=0; }
 ~WordInfo() { delete[] word; }  
  char *word;
  int  freq;
  WordInfo *next;
};

//----------------------------------------------------------------------------

struct WordList
{
  WordList() { first=0; last=0; }
 ~WordList() { WordInfo *wi=first;
               while (wi) { WordInfo *owi=wi; wi=wi->next; delete owi; }
             }
  void add(const char *word,int freq)
             {
               WordInfo *nw=new WordInfo;
               nw->word = new char[strlen(word)+1];
               strcpy(nw->word,word);
               nw->freq=freq;
               nw->next=0;
               if (last) { last->next=nw; last=nw; } else { first=nw; last=nw; }
             }
  WordInfo *first;
  WordInfo *last;
};

//----------------------------------------------------------------------------

struct SearchDoc
{
  FileInfo  *fileInfo;
  int        index;
  int        freq;
  double     rank;
  SearchDoc *next;
};

//----------------------------------------------------------------------------

struct SearchResults
{
  SearchResults() { totalFreq=0; docList=0; last=0; }
 ~SearchResults() { SearchDoc *d=docList; 
                    while (d) { SearchDoc *od=d; d=d->next; delete od; } 
                  }
  void add(FileInfo *fi,int index,int freq)
         {
           SearchDoc *nd=new SearchDoc;
           nd->fileInfo=fi;
           nd->index=index;
           nd->freq=freq;
           nd->next=0;
           if (last) { last->next=nd; last=nd; } else { docList=nd; last=nd; }
         }
  SearchDoc *docList; 
  SearchDoc *last;
  int totalFreq;
};

//----------------------------------------------------------------------------

// global vars

static WordList wordList;
static FileList fileList;
static char cgiBin[MAXSTRLEN];
static char queryInput[MAXSTRLEN];
static char encQueryInput[MAXSTRLEN];
static char firstDocUrl[MAXSTRLEN];
static bool nameOnly;
static bool wordOnly;
static bool helpOnly;
static int  page;
static char *headerBuf=0;
static char *footerBuf=0;

//----------------------------------------------------------------------------

// functions

void printHeader()
{
  if (headerBuf)
  {
    printf("Content-Type: text/html\r\n\r\n%s",headerBuf);
  }
  else
  {
    printf("Content-Type: text/html\r\n\r\n");
    printf("<html><head><title>Search the documentation</title></head>\n"
           "<body bgcolor=\"#ffffff\" text=\"#000000\" link=\"#0000ee\" \n"
           "vlink=\"#551a8b\" alink=\"#ff0000\">\n");
  }
}

//----------------------------------------------------------------------------

void printFooter()
{
  if (footerBuf)
  {
    printf("%s",footerBuf);
  }
  else
  {
    printf("</body></html>\n");
  }
}

//----------------------------------------------------------------------------

void message(const char *fmt,...)
{
  printHeader();
  printf("<h2>");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args); 
  printf("</h2>");
  printFooter();
}

//----------------------------------------------------------------------------

void printSearchPage(bool open=FALSE)
{
  printf("<form name=doxyform method=GET action=\"%s\">\n"
         "<center>\n"
         "<input type=hidden name=page value=0>\n"
         "<table border=0 bgcolor=\"#ffffcc\" cellspacing=0 cellpadding=4>\n"
         "  <tr>\n"
         "    <td valign=top><font size=-1 face=\"arial, helvetica\"><b>"
         "      Search for\n"
         "      <select name=which>\n"
         "        <option value=string %s>strings\n"
         "        <option value=word %s>words\n"
         "      </select>\n"
         "      in function and class\n"
         "      <select name=what>\n"
         "        <option value=doc %s>documentation\n"
         "        <option value=name %s>names</select></b></font>\n"
         "    </td>\n"
         "    <td valign=top align=right>\n"
         "      <a href=\"%s?help=on\"><font size=-1>Help</font></a>\n"
         "    </td>\n"
         "  </tr>\n"
         "  <tr>\n"
         "    <td><input name=query size=44 wrap=virtual maxlength=1000 value=\"%s\">\n"
         "    </td>\n"
         "    <td><input type=image src=\"%s/search.gif\" height=26 width=120 border=0 name=\"search\">\n"
         "    </td>\n"
         "  </tr>\n",
         cgiBin,
         (wordOnly?"":"selected"),(wordOnly?"selected":""),
         (nameOnly?"":"selected"),(nameOnly?"selected":""),
         cgiBin,queryInput,firstDocUrl);
  if (!open)
  { 
    printf("</table>\n</center></form>\n");
  }
}

//----------------------------------------------------------------------------

int readInt(FILE *f)
{
  unsigned char c_24 = fgetc(f);
  unsigned char c_16 = fgetc(f);
  unsigned char c_08 = fgetc(f);
  unsigned char c_00 = fgetc(f);
  return ((unsigned int)c_24<<24)+
         ((unsigned int)c_16<<16)+
         ((unsigned int)c_08<<8)+
         c_00;
  //return (fgetc(f)<<24)+(fgetc(f)<<16)+(fgetc(f)<<8)+fgetc(f);
}

//----------------------------------------------------------------------------

int readEncodedNumber(FILE *f)
{
  int n=0,b,s=0;
  do { b=fgetc(f); n|=(b&0x7f)<<s; s+=7; } while (b&0x80);
  return n;
}

void readString(FILE *f,char *s,int n)
{
  int i=0,b;
  if (n<=0) return;
  while (i<n-1 && (b=fgetc(f))!=0) s[i++]=b;
  s[i]='\0';
}

//----------------------------------------------------------------------------

bool searchRecursive(SearchResults *sr,FileInfo *fi,const char *word)
{
  char entry[MAXSTRLEN];
  readString(fi->f,entry,MAXSTRLEN);
  while (entry[0]!='\0')
  {
    //printf("Found entry `%s'\n",entry);
    int i=0, w=word[0], e=entry[0];
    while (w!=0 && e!=0 && w==e) { i++; w=word[i]; e=entry[i]; }
    if (w==0 && e!=0) // word is a substring of entry
    {
      if (wordOnly) return FALSE; // no full word match
      //printf("Word found as substring of `%s%s'\n",&word[-index],&entry[i]);
      int offset=readInt(fi->f);  // <= follow for extensions
      if (!(offset&TERMMASK)) // extra info available
      {
        int tfreq=readEncodedNumber(fi->f);
        sr->totalFreq+=tfreq;
        //printf("Total frequency %d\n",tfreq);
        int s=-1,k;
        while ((k=readEncodedNumber(fi->f))!=0)
        {
          bool inName  = (k&2)==2;
          s+=(k>>3);
          int freq=readEncodedNumber(fi->f);
          //printf("Doc index %d frequency %d\n",s,freq);
          if (!nameOnly || inName) 
            //addResult(fi,sr,s,freq);
            sr->add(fi,s,freq);
        }
      }
      return TRUE;
    }
    else if (e==0) // entry is a substring of word
    {
      if (w==0) // word is equal to entry => exact match found
      {
        int offset=readInt(fi->f);  // <= follow for extensions
        if (!(offset&TERMMASK)) // extra info available
        {
          //printf("Match found\n");
          int tfreq=readEncodedNumber(fi->f);
          sr->totalFreq+=tfreq;
          //printf("Total frequency %d\n",tfreq);
          int s=-1,k;
          while ((k=readEncodedNumber(fi->f))!=0)
          {
            bool fullWord   = (k&1)==1;
            bool inName     = (k&2)==2;
            bool wordInName = (k&4)==4;
            s+=(k>>3);
            int freq=readEncodedNumber(fi->f);
            if (nameOnly && wordOnly)
            {
              if (wordInName) sr->add(fi,s,freq);
            }
            else if (!nameOnly && wordOnly)
            {
              if (fullWord) sr->add(fi,s,freq);
            }
            else if (nameOnly && !wordOnly)
            {
              if (inName) sr->add(fi,s,freq);
            }
            else // !nameOnly && !wordOnly
            {
              sr->add(fi,s,freq);
            }
          }
        }
        return TRUE;
      }
      else // follow branch to next entry
      {
        int offset=readInt(fi->f);
        if ((offset&OFFSETMASK)==0) // no next entry (leaf node)
        {
          return FALSE;
        }
        else // follow branch
        {
          fseek(fi->f,offset&OFFSETMASK,SEEK_SET);
          //printf("Recursive call; jump to %lx\n",ftell(f));
          return searchRecursive(sr,fi,&word[i]);
        }
      }
    }
    else // skip rest of the entry
    {
      int offset=readInt(fi->f);
      if (!(offset&TERMMASK)) while (readEncodedNumber(fi->f)!=0);
    }
    readString(fi->f,entry,MAXSTRLEN);
  } 
  //printf("Sorry no match found\n");
  return FALSE;
}

//----------------------------------------------------------------------------


void searchIndex(const char *word,SearchResults *results)
{
  FileInfo *fi=fileList.first;
  while (fi)
  {
    fseek(fi->f,8,SEEK_SET);
    searchRecursive(results,fi,word);
    fi=fi->next;
  }

  SearchDoc *ds = results->docList;
  while (ds)
  {
    ds->rank = ds->freq/(double)results->totalFreq;
    ds = ds->next;
  }
  
  wordList.add(word,results->totalFreq);
}

//----------------------------------------------------------------------------

extern "C" int sortResults(const void *a1,const void *a2)
{
  SearchDoc **d1=(SearchDoc **)a1;
  SearchDoc **d2=(SearchDoc **)a2;
  if ((*d1)->rank > (*d2)->rank) 
    return -1;
  else if ((*d1)->rank == (*d2)->rank) 
    return 0;
  else 
    return 1;
}

//----------------------------------------------------------------------------

void generateResults(SearchResults *sr)
{
  // compute the maximum rank
  SearchDoc *ds = sr->docList;
  double maxRank = 0.00000001;
  int resultCount=0;
  if (ds)
  {
    while (ds)
    {
      if (ds->rank>maxRank) maxRank = ds->rank;
      ds = ds->next;
    }

    // scale ranks, so the highest is 1 and count the number of links
    double oneOverMaxRank = 1.0/maxRank;
    ds = sr->docList;
    while (ds)
    {
      ds->rank *= oneOverMaxRank;
      resultCount++;
      ds = ds->next;
    }
  }
  SearchDoc **docPtrArray=0;
  if (resultCount>0)
  {
    docPtrArray=(SearchDoc **)calloc(resultCount,sizeof(SearchDoc *));
    int offset=0;
    ds = sr->docList;
    while (ds)
    {
      docPtrArray[offset++]=ds;
      ds = ds->next;
    }
    qsort(docPtrArray,resultCount,sizeof(SearchDoc *),sortResults);
  }

  // start generating output
  printHeader();
  printSearchPage(TRUE);

  printf("  <tr bgcolor=#ffffff>\n"
         "    <td colspan=2>"
         "<br><h3>Search results</h3>\n"
         "    </td>\n"
         "  </tr>\n");
  if (resultCount==0) // nothing found
  {
    printf("  <tr bgcolor=#ffffff>\n"
           "    <td colspan=2>Sorry, no documents matching your query.\n"
           "    </td>\n"
           "  </tr>\n");
  }
  else // something found
  {
    printf("  <tr bgcolor=#ffffff>\n"
           "    <td colspan=2>"
           "Found <b>%d</b> document%s matching your query. ",
            resultCount,resultCount==1?"":"s");

    if (resultCount>1) 
      printf("Showing best matches first.\n");

    printf("    <br><br>\n");
    printf("    </td>\n"
           "  </tr>\n");

    const int lpp=20;
    int numPages = (resultCount+lpp-1)/lpp;

    // clip page
    if (page<0) page=0;
    if (page>=numPages) page=numPages-1;
    int skipEntries = page * lpp;
    int pageEntries = lpp;
    
    printf("  <tr><td colspan=2 bgcolor=\"#4040c0\">"
           "<center><b><font color=\"#ffffff\">Page %d / %d</font></b>"
           "</center></td></tr>\n",page+1,numPages);
    printf("  <tr bgcolor=#ffffff>\n"
           "    <td colspan=2>\n"
           "      <dl compact>\n");
    int i;
    for (i=0 ; i < resultCount && pageEntries > 0; i++)
    {
      if (skipEntries == 0)
      {
        SearchDoc *d=docPtrArray[i];
        FILE *f=d->fileInfo->f;
        fseek(f,d->fileInfo->refOffset+d->index*4,SEEK_SET);
        int offset=readInt(f);
        fseek(f,offset,SEEK_SET);
        char linkName[MAXSTRLEN];
        char htmlName[MAXSTRLEN];
        readString(f,linkName,MAXSTRLEN);
        readString(f,htmlName,MAXSTRLEN);
        int rank=(int)(d->rank*100+0.5);
        if (rank==0) rank++;
        printf("        <dt><b><font color=#%2x00%2x>%d</font></b>"
               "<dd><a href=\"%s/%s\">%s</a>\n",
               rank*2+55, 255-rank*2, rank, 
               d->fileInfo->url, htmlName, linkName);
        pageEntries--;
      }
      else 
      {
        skipEntries--;
      }
    }
    printf("      </dl>\n"
           "    </td>\n"
           "  </tr>\n"
           "  <tr><td colspan=2 bgcolor=\"#4040c0\"><center>&nbsp;");
    
    if (page>0)
    {
      printf("<a href=\"%s?page=%d&which=%s&what=%s&query=%s\">"
             "<font color=\"ffffff\">prev</font></a>&nbsp;",
              cgiBin, page-1, 
              wordOnly?"word":"doc", 
              nameOnly?"name":"string",
              encQueryInput);
    }
    int startPage = page-5 < 0 ? 0 : page-5;
    int endPage   = page+6 > numPages ? numPages : page+5;
    if (endPage-startPage>1)
    {
      for (i=startPage;i<endPage;i++)
      {
        if (i!=page)
        {
          printf("<a href=\"%s?page=%d&which=%s&what=%s&query=%s\">"
              "<font color=\"ffffff\">%d</font></a>&nbsp;",
              cgiBin, i, 
              wordOnly?"word":"doc", 
              nameOnly?"name":"string", 
              encQueryInput, i+1);
        }
        else
        {
          printf("<b><font color=\"ffffff\">%d</font></b></a>&nbsp;",i+1);
        }
      }
    }
    if (page<numPages-1)
    {
      printf("<a href=\"%s?page=%d&which=%s&what=%s&query=%s\">"
             "<font color=\"ffffff\">next</font></a>",
              cgiBin, page+1, 
              wordOnly?"word":"doc", 
              nameOnly?"name":"string",
              encQueryInput);
    }
    printf("  &nbsp;</center></td></tr>\n"
           "  <tr bgcolor=#ffffff>\n"
           "    <td colspan=2><p>\n"
           "       Occurrence count: ");
    WordInfo *wi=wordList.first;
    while (wi)
    {
      printf("<b><font color=#ff0000>%s</font></b> ",wi->word);
      if (wi->freq>0) printf("(%d)",wi->freq); else printf("(ignored)");
      wi=wi->next;
      if (wi) 
      {
        if (wi->next) printf(", "); else printf(" and ");
      }
    }
    printf("\n"
           "    </td>\n"
           "  </tr>\n");
  }
  printf("</table>\n</center>\n</form>\n");
  printFooter();
  free(docPtrArray);
}

//----------------------------------------------------------------------------

void generateHelpPage()
{
  printHeader();
  printf("<h3>Doxysearch help page</h3>\n"
         "<b>Basic search instructions</b>\n"
         "<ul>\n"
         "Just type in one or more words or fragments of words and press the "
         "search button. Doxysearch will return a list of functions and class names, "
         "whose documentation or name matches one or more of the words. "
         "Documents containing more matches will appear earlier in the list. "
         "\n"
         "</ul>\n"
         "<b>Search modes</b>\n"
         "<ul>\n"
         "Doxysearch has four search modes. From least restrictive to most "
         "restrictive they are:\n"
         "  <ul><p>\n"
         "    <li><em>Search for strings in the documentation:</em>\n"
         "      The words you type will be searched in the documentation as substrings." 
         "      Typing the word &quot;<b>the</b>&quot; for example, will find "
         "      documents containing the words &quot;<b>the</b>&quot;, "
         "      &quot;<b>the</b>re&quot;, and &quot;fea<b>the</b>r&quot;.<p>\n"
         "    <li><em>Search for words in the documentation:</em>\n"
         "      The words you type will be searched in the documentation as words."
         "      Typing the word &quot;<b>the</b>&quot; for example, will look "
         "      for documents containing the word &quot;<b>the</b>&quot; and not for"
         "      documents containing the word &quot;<b>the</b>re&quot;.<p>\n"
         "    <li><em>Search for strings in the function and class names:</em>\n"
         "      The words you type will be searched in the function and class names "
         "      as substrings. Typing the word &quot;<b>set</b>&quot; for example, will "
         "      result in a list of links to the documentation of all functions and classes "
         "      that contain the word &quot;<b>set</b>&quot; in the name, such as the function "
         "      <code><b>set</b>Cursor</code>.<p>\n"
         "    <li><em>Search for words in the function and class names:</em>\n"
         "      The words you type will be matched against the function and class names."
         "      The result will be a list of links to the documentation of all function "
         "      and class names that are equal to one of the words.\n"
         "  </ul>\n"
         "</ul>\n"
         "<b>Requiring/Excluding words</b>\n"
         "<ul>\n"
         "  Often you will know a word that will be guaranteed to appear in a document "
         "  for which you are searching. If this is the case, require that the word appears "
         "  in all of the results by attaching a &quot;<b>+</b>&quot; to the beginning of "
         "  the word. You can quickly reject results by adding a word that appears only "
         "  in unwanted documents with a &quot;<b>-</b>&quot; before it."
         "</ul>\n"
        );
         
  printFooter();
}

//----------------------------------------------------------------------------

void mergeSearchResults(SearchResults *totalResults,
                        SearchResults *sr,int mode)
{
  SearchDoc *otd = 0,
            *td  = totalResults->docList, 
            *d   = sr->docList;
  totalResults->totalFreq += sr->totalFreq;
  if (!d) return; // nothing to add
  while (td && d)
  {
    int otdih = otd ? otd->fileInfo->index : -1,
        otdil = otd ? otd->index : -1, 
        tdih  = td->fileInfo->index,
        tdil  = td->index, 
        dih   = d->fileInfo->index,
        dil   = d->index;
    if (tdih==dih && tdil==dil) // combine results
    {
      if (mode != EXCLUDE)
      {
        td->rank += d->rank;
        td->rank *= 2;          // put extra emphasis on multiple word matches
        td->freq += d->freq;
        d = d->next;
        otd = td; td = td->next;
      }
      else // mode == EXCLUDE => remove entry from results
      {
        SearchDoc *tmp=td;
        td=td->next;
        if (otd) otd->next=td; else totalResults->docList = td;
        delete tmp;
      }
    }
    else if ((otdih<dih || (otdih==dih && otdil<dil)) &&
             (tdih>dih  || (tdih==dih && tdil>dil))) // insert
    {
      if (mode == NORMAL)
      {
        SearchDoc *nd = new SearchDoc(*d);
        if (otd) otd->next = nd; else totalResults->docList = nd;
        nd->next = td;
        td = nd;
      }
      d = d->next;
    }
    else if (tdih<dih || (tdih==dih && tdil<dil)) // remove or skip
    {
      if (mode == INCLUDE)
      {
        SearchDoc *tmp=td;
        td=td->next;
        if (otd) otd->next=td; else totalResults->docList = td;
        delete tmp;
      }
      else // mode == EXCLUDE or mode == NORMAL
      {
        otd = td; td = td->next;
      }
    }
  }
  if (td==0 && d && mode==NORMAL) // append rest of sr to totalResults
  {
    while (d)
    {
      SearchDoc *nd = new SearchDoc(*d);
      if (otd) otd->next = nd; else totalResults->docList = nd;
      nd->next = 0;
      d = d->next;
      otd = nd;
    } 
  }
  if (td && d==0 && mode==INCLUDE) // delete the rest of the results
  {
    while (td)
    {
      SearchDoc *tmp=td;
      td=td->next;
      if (otd) otd->next = td; else totalResults->docList = td;
      delete tmp;
    }
  }
}

//----------------------------------------------------------------------------

int asciiToHex(char c)
{
  char l=tolower(c);
  if (l>='0' && l<='9') 
    return l-'0';
  else if (l>='a' && l<='f')
    return l+10-'a';
  else // invalid hex char
    return 0;
}

//----------------------------------------------------------------------------

void fileToBuf(const char *name, char **buf)
{
  FILE *f;
  struct stat file_stats;
  if ((f=fopen(name,"rb"))==NULL) return;
  if (stat(name,&file_stats)==-1)
  {
    message("Error: could not fstat file %s\n",name);
    exit(1);
  }
  unsigned int len=file_stats.st_size;
  if ((*buf=(char *)malloc(len+1))==NULL) 
  {
    message("Error: out of memory\n");
    exit(1);
  }
  if (fread(*buf,1,len,f)!=len)
  {
    message("Error: could not read file %s\n",name);
    exit(1);
  }
  (*buf)[len]='\0';
  fclose(f);
}

//----------------------------------------------------------------------------

void getConfig(const char *s)
{
  int l;
  char configFile[MAXSTRLEN];
  strcpy(configFile,s);
  strcat(configFile,"/search.cfg");

  FILE *f;
  if ((f=fopen(configFile,"r"))==NULL)
  {
    message("Error: could not open config file %s\n",configFile);
    exit(1);
  }
    
  // get the URL to the documentation
  fgets(firstDocUrl,MAXSTRLEN,f);
  l=strlen(firstDocUrl)-1;
  if (firstDocUrl[l]=='\n') firstDocUrl[l]='\0';
  l=strlen(firstDocUrl);
  if (firstDocUrl[l]=='/') firstDocUrl[l]='\0';
  
  // get the URL to the cgi script
  fgets(cgiBin,MAXSTRLEN,f);
  l=strlen(cgiBin)-1;
  if (cgiBin[l]=='\n') cgiBin[l]='\0';
  
  fclose(f);
  
  char headerFile[MAXSTRLEN];
  strcpy(headerFile,s);
#if defined(_WIN32)
  strcat(headerFile,"\\header.html");
#else
  strcat(headerFile,"/header.html");
#endif
  fileToBuf(headerFile,&headerBuf);
  
  char footerFile[MAXSTRLEN];
  strcpy(footerFile,s);
#if defined(_WIN32)
  strcat(footerFile,"\\footer.html");
#else
  strcat(footerFile,"/footer.html");
#endif
  fileToBuf(footerFile,&footerBuf);
  
}

//----------------------------------------------------------------------------
// copy and convert string to lower case

void strlowercpy(char *d,const char *s)
{
  while (*s!='\0') *d++=tolower(*s++); *d='\0';
}

//----------------------------------------------------------------------------

int main(int argc,char **argv)
{
#ifdef PROFILING
  struct timeval tv_start,tv_end;
  gettimeofday(&tv_start,0);
#endif
  char *argString=getenv("QUERY_STRING");
  if (argc==1)
  {
    message("Error: invalid number of arguments. "
            "Usage: %s doc_path [doc_path ...]",argv[0]);
    exit(1);
  }
  // read the configuration file for this instance of the search engine
  getConfig(argv[1]);

  if (!argString) 
  {
    printHeader();
    printSearchPage();
    printFooter();
    exit(1);
  }
  
  SearchResults tsr;

  // parse cgi arguments
  char *arg=strtok(argString,"&");
  char *query = 0;
  char *what  = 0;
  char *which = 0;
  char *help  = 0;
  page = 0;
  while (arg)
  {
    int namelen=strcspn(arg,"=");
    if      (!strncmp(arg,"query", namelen)) query = &arg[namelen+1];
    else if (!strncmp(arg,"what",  namelen)) what  = &arg[namelen+1];
    else if (!strncmp(arg,"which", namelen)) which = &arg[namelen+1];
    else if (!strncmp(arg,"help",  namelen)) help  = &arg[namelen+1];
    else if (!strncmp(arg,"page",  namelen)) page  = atoi(&arg[namelen+1]);
    arg=strtok(0,"&");
  }

  wordOnly=which ? strcmp(which,"word")==0 : FALSE;
  nameOnly=what  ? strcmp(what, "name")==0 : FALSE;
  helpOnly=help  ? strcmp(help, "on"  )==0 : FALSE;
  
  // store encoded query string
  if (query) strcpy(encQueryInput,query); else encQueryInput[0]='\0';

  // convert query string to original input
  char *s=query,*d=queryInput;
  if (s)
  {
    while (*s!='\0')
    {
      char c=*s++;
      if (c=='+') 
        *d++=' ';
      else if (c=='%')
      {
        *d=asciiToHex(*s++)*16;
        *d+=asciiToHex(*s++);
        d++;
      }
      else
        *d++=c;
    }
  }
  *d='\0';

  if (helpOnly) 
  {
    generateHelpPage();
    exit(1);
  }

  // read search index files
  int argIndex=0;
  for (argIndex=1;argIndex<argc;argIndex++)
  {
    char configFile[MAXSTRLEN];
    strcpy(configFile,argv[argIndex]);
#if defined(_WIN32)
    strcat(configFile,"\\search.cfg");
#else
    strcat(configFile,"/search.cfg");
#endif

    char indexFile[MAXSTRLEN];
    strcpy(indexFile,argv[argIndex]);
#if defined(_WIN32)
    strcat(indexFile,"\\search.idx");
#else
    strcat(indexFile,"/search.idx");
#endif

    FileInfo *fi=fileList.add();
    FILE *g;
    
    if ((fi->f=fopen(indexFile,"rb"))==NULL)
    {
      message("Error: could not open index file %s\n",indexFile);
      exit(1);
    } 
    if ((g=fopen(configFile,"r"))==NULL)
    {
      message("Error: could not open config file %s\n",configFile);
      exit(1);
    }
    
    // get URL to the documentation
    char tmp[MAXSTRLEN];
    fgets(tmp,MAXSTRLEN,g);
    if (tmp[strlen(tmp)-1]=='\n') tmp[strlen(tmp)-1]='\0';
    if (tmp[strlen(tmp)-1]=='/')  tmp[strlen(tmp)-1]='\0';
    fi->url = new char[strlen(tmp)+1];
    strcpy(fi->url,tmp);
    fclose(g);
    
    // read & check the format of the search index file
    fseek(fi->f,0,SEEK_SET);
    char header[5];
    if (fread(header,1,4,fi->f)!=4) 
    {
      message("Error: Couldn't read header of the index file %s\n",indexFile);
      exit(1);
    }
    header[4]='\0';
    if (strcmp(header,"DOXI"))
    {
      message("Error: Index file %s has an unknown format\n",indexFile);
      exit(1);
    }
    // read and store the offset to the link index
    fi->refOffset=readInt(fi->f);
  }

  char *word;
  char wordString[MAXSTRLEN];

  // search for included words
  strlowercpy(wordString,queryInput);
  word=strtok(wordString," "); 
  bool first=TRUE;
  while (word)
  {
    if (word[0]=='+') // + character => include
    {
      SearchResults sr;
      searchIndex(&word[1],&sr);
      if (first)
        mergeSearchResults(&tsr,&sr,NORMAL);
      else
        mergeSearchResults(&tsr,&sr,INCLUDE);
      first=FALSE;
    }
    word=strtok(0," ");
  }

  // search for normal words
  strlowercpy(wordString,queryInput);
  word=strtok(wordString," "); 
  while (word)
  {
    if (word[0]!='-' && word[0]!='+') // normal word
    {
      SearchResults sr;
      searchIndex(word,&sr);
      mergeSearchResults(&tsr,&sr,NORMAL);
    }
    word=strtok(0," ");
  }

  // search for excluded words
  strlowercpy(wordString,queryInput);
  word=strtok(wordString," "); 
  while (word)
  {
    if (word[0]=='-') // - character => exclude
    {
      SearchResults sr;
      searchIndex(&word[1],&sr);
      mergeSearchResults(&tsr,&sr,EXCLUDE);
    }
    word=strtok(0," ");
  }

  // write results to HTML page
  generateResults(&tsr);

  free(headerBuf);
  free(footerBuf);

#ifdef PROFILING
  gettimeofday(&tv_end,0);
  printf("processing time %3.3f msec\n",
      ((tv_end.tv_sec-tv_start.tv_sec)*1000000+
        tv_end.tv_usec-tv_start.tv_usec)/1000.0
        );
#endif
  return 0;
}
