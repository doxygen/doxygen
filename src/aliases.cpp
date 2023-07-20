/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#include "aliases.h"
#include "containers.h"
#include "config.h"
#include "regex.h"
#include "textstream.h"
#include "util.h"

static StringMap             g_aliasMap;                     // aliases
static CharSetMap            g_aliasSeparatorMap;            // separators for aliases

static std::string expandAlias(const std::string &aliasName,const std::string &aliasValue);
//----------------------------------------------------------------------------

static void expandAliases()
{
  for (auto &[name,value] : g_aliasMap)
  {
    value = expandAlias(name,value);
  }
}

//----------------------------------------------------------------------------

static void escapeAliases()
{
  for (auto &[name,definition] : g_aliasMap)
  {
    QCString value(definition);
    QCString newValue;
    int in,p=0;
    // for each \n in the alias command value
    while ((in=value.find("\\n",p))!=-1)
    {
      newValue+=value.mid(p,in-p);
      // expand \n's except if \n is part of a built-in command.
      if (value.mid(in,5)!="\\note" &&
          value.mid(in,5)!="\\noop" &&
          value.mid(in,5)!="\\name" &&
          value.mid(in,10)!="\\namespace" &&
          value.mid(in,14)!="\\nosubgrouping"
         )
      {
        newValue+="\\ilinebr ";
      }
      else
      {
        newValue+="\\n";
      }
      p=in+2;
    }
    newValue+=value.mid(p,value.length()-p);
    p = 0;
    newValue = "";
    while ((in=value.find("^^",p))!=-1)
    {
      newValue+=value.mid(p,in-p);
      newValue+="\\ilinebr ";
      p=in+2;
    }
    newValue+=value.mid(p,value.length()-p);
    definition=newValue.str();
    //printf("Alias %s has value %s\n",name.c_str(),qPrint(newValue));
  }
}

//----------------------------------------------------------------------------

void readAliases()
{
  // add aliases to a dictionary
  const StringVector &aliasList = Config_getList(ALIASES);
  for (const auto &al : aliasList)
  {
    QCString alias(al);
    int i=alias.find('=');
    if (i>0)
    {
      QCString name=alias.left(i).stripWhiteSpace();
      QCString value=alias.right(alias.length()-i-1);
      //printf("Alias: found name='%s' value='%s'\n",qPrint(name),qPrint(value));
      if (!name.isEmpty())
      {
        auto it = g_aliasMap.find(name.str());
        if (it==g_aliasMap.end()) // insert new alias
        {
          g_aliasMap.insert(std::make_pair(name.str(),value.str()));
        }
        else // overwrite previous alias
        {
          it->second=value.str();
        }

        // check for {
        int j=name.find('{');
        if (j>0)
        {
          auto it1 = g_aliasSeparatorMap.find(name.left(j).str());
          if (it1==g_aliasSeparatorMap.end()) // insert new alias
          {
            g_aliasSeparatorMap.insert(std::make_pair(name.left(j).str(),std::set<char>()));
            it1 = g_aliasSeparatorMap.find(name.left(j).str());
          }
          // check for ,
          char sep = ',';
          int k=name.find(',');
          if (k>0) sep = name[k+1];
          it1->second.insert(sep);
        }
      }
    }
  }
  expandAliases();
  escapeAliases();
}

static QCString expandAliasRec(StringUnorderedSet &aliasesProcessed,
                               const QCString &s,bool allowRecursion=FALSE);
static int countAliasArguments(const QCString &argList, const char sep);
static QCString extractAliasArgs(const QCString &args,size_t pos);

//--------------------------------------------------------------------------------------

struct Marker
{
  Marker(size_t p, int n,size_t s) : pos(p),number(n),size(s) {}
  size_t pos; // position in the string
  int number; // argument number
  size_t size; // size of the marker
};

/** For a string \a s that starts with a command name, returns the character
 *  offset within that string representing the first character after the
 *  command. For an alias with argument, this is the offset to the
 *  character just after the argument list.
 *
 *  Examples:
 *  - s=="a b"      returns 1
 *  - s=="a{2,3} b" returns 6
 *  = s=="#"        returns 0
 */
static int findEndOfCommand(const char *s)
{
  const char *p = s;
  char c;
  int i=0;
  if (p)
  {
    while ((c=*p) && isId(c)) p++;
    if (c=='{')
    {
      QCString args = extractAliasArgs(p,0);
      i+=args.length();
    }
    i+=static_cast<int>(p-s);
  }
  return i;
}

/** Replaces the markers in an alias definition \a aliasValue
 *  with the corresponding values found in the comma separated argument
 *  list \a argList and the returns the result after recursive alias expansion.
 */
static QCString replaceAliasArguments(StringUnorderedSet &aliasesProcessed,
                                      const QCString &aliasValue,const QCString &argList,char sep)
{
  //printf("----- replaceAliasArguments(val=[%s],args=[%s])\n",qPrint(aliasValue),qPrint(argList));

  // first make a list of arguments from the comma separated argument list
  std::vector<QCString> args;
  size_t i,l=argList.length();
  size_t s=0;
  for (i=0;i<l;i++)
  {
    char c = argList.at(i);
    if (c==sep && (i==0 || argList.at(i-1)!='\\'))
    {
      args.push_back(QCString(argList.mid(s,i-s)));
      s=i+1; // start of next argument
    }
    else if (c=='@' || c=='\\')
    {
      // check if this is the start of another aliased command (see bug704172)
      i+=findEndOfCommand(argList.data()+i+1);
    }
  }
  if (l>s) args.push_back(QCString(argList.right(l-s)));
  //printf("found %d arguments\n",args.count());

  // next we look for the positions of the markers and add them to a list
  std::vector<Marker> markerList;
  l = aliasValue.length();
  char pc='\0';
  bool insideMarkerId=false;
  size_t markerStart=0;
  auto isDigit = [](char c) { return c>='0' && c<='9'; };
  for (i=0;i<=l;i++)
  {
    char c = i<l ? aliasValue.at(i) : '\0';
    if (insideMarkerId && !isDigit(c)) // found end of a markerId
    {
      insideMarkerId = false;
      size_t markerLen = i-markerStart;
      markerList.push_back(Marker(markerStart-1,
                                  aliasValue.mid(markerStart,markerLen).toInt(),
                                  markerLen+1));
    }
    if (c=='\\' && (pc=='@' || pc=='\\')) // found escaped backslash
    {
      // skip
      pc = '\0';
    }
    else
    {
      if (isDigit(c) && pc=='\\') // found start of a markerId
      {
        insideMarkerId=true;
        markerStart=i;
      }
      pc = c;
    }
  }

  // then we replace the markers with the corresponding arguments in one pass
  QCString result;
  size_t p=0;
  for (i=0;i<markerList.size();i++)
  {
    const Marker &m = markerList.at(i);
    result+=aliasValue.mid(p,m.pos-p);
    //printf("part before marker %d: '%s'\n",i,qPrint(aliasValue.mid(p,m->pos-p)));
    if (m.number>0 && m.number<=static_cast<int>(args.size())) // valid number
    {
      result+=expandAliasRec(aliasesProcessed,args.at(m.number-1),TRUE);
      //printf("marker index=%d pos=%d number=%d size=%d replacement %s\n",i,m->pos,m->number,m->size,
      //    qPrint(args.at(m->number-1)));
    }
    p=m.pos+m.size; // continue after the marker
  }
  result+=aliasValue.right(l-p); // append remainder
  //printf("string after replacement of markers: '%s'\n",qPrint(result));

  // expand the result again
  result = substitute(result,"\\{","{");
  result = substitute(result,"\\}","}");
  result = expandAliasRec(aliasesProcessed,substitute(result,"\\,",","));

  return result;
}

static QCString escapeSeparators(const QCString &s, const char sep)
{
  if (s.isEmpty()) return s;
  TextStream result;
  const char *p = s.data();
  char c,pc=0;
  while ((c=*p++))
  {
    if (c==sep && pc!='\\')
    {
      result << "\\" << sep;
    }
    else
    {
      result << c;
    }
    pc=c;
  }
  //printf("escapeSeparators: '%s'->'%s'\n",qPrint(s),qPrint(result));
  return result.str();
}

static QCString expandAliasRec(StringUnorderedSet &aliasesProcessed,const QCString &s,bool allowRecursion)
{
  QCString result;
  static const reg::Ex re(R"([\\@](\a[\w-]*))");
  std::string str = s.str();
  reg::Match match;
  size_t p = 0;
  while (search(str,match,re,p))
  {
    size_t i = match.position();
    size_t l = match.length();
    if (i>p) result+=s.mid(p,i-p);

    QCString args = extractAliasArgs(s,i+l);
    bool hasArgs = !args.isEmpty();            // found directly after command
    int argsLen = args.length();
    QCString cmd = match[1].str();
    QCString cmdNoArgs = cmd;
    int numArgs=0;
    char sep = ',';
    auto it = g_aliasMap.end();
    if (hasArgs)
    {
      QCString arg;
      auto st = g_aliasSeparatorMap.find(cmd.str());
      if (st!=g_aliasSeparatorMap.end())
      {
        for (const char &sep1 : st->second)
        {
          int numArgs1 = countAliasArguments(args,sep1);
  
          if (sep1 ==',')
            arg.sprintf("{%d}",numArgs1);
          else
            arg.sprintf("{%d,%c}",numArgs1,sep1);
  
          auto it1 = g_aliasMap.find((cmd+arg).str());
          if (it==g_aliasMap.end() && it1!=g_aliasMap.end())
          {
            it = it1;
            sep = sep1;
            numArgs = numArgs1;
            cmd += arg;
          }
          if ((sep1 == ',') && it==g_aliasMap.end() && it1==g_aliasMap.end())
          {
            arg.sprintf("{%d,%c}",numArgs1,sep1);
            auto it2 = g_aliasMap.find((cmd+arg).str());
            if (it2!=g_aliasMap.end())
            {
              it = it2;
              sep = sep1;
              numArgs = numArgs1;
              cmd += arg;
            }
          }
        }
      }
    }
    else
    {
      it = g_aliasMap.find(cmd.str());
    }

    if (hasArgs && it==g_aliasMap.end())
    { // in case there is no command with numArgs parameters, but there is a command with 1 parameter,
      // we also accept all text as the argument of that command (so you don't have to escape commas)
      auto st = g_aliasSeparatorMap.find(cmd.str());
      if (st!=g_aliasSeparatorMap.end())
      {
        for (const char &sep1 : st->second)
        {
          QCString arg;
          if (sep1 ==',')
            arg = "{1}";
          else
            arg = "{1," + QCString(sep1) + "}";
          auto it1 = g_aliasMap.find((cmdNoArgs+arg).str());
          if (it1!=g_aliasMap.end())
          {
            it = it1;
            sep = sep1;
            numArgs = 1;
            cmd = cmdNoArgs+arg;
            args = escapeSeparators(args,sep1); // escape separator so that everything is seen as one argument
          }
          else if (sep1 == ',')
          {
            arg = "{1," + QCString(sep1) + "}";
            auto it2 = g_aliasMap.find((cmdNoArgs+arg).str());
            if (it2!=g_aliasMap.end())
            {
              it = it2;
              sep = sep1;
              numArgs = 1;
              cmd = cmdNoArgs+arg;
              args = escapeSeparators(args,sep1); // escape separator so that everything is seen as one argument
            }
          }
        }
      }
    }
    //printf("Found command s='%s' cmd='%s' numArgs=%d args='%s' aliasText=%s\n",
    //    s.data(),cmd.data(),numArgs,args.data(),aliasText?aliasText->data():"<none>");
    if ((allowRecursion || aliasesProcessed.find(cmd.str())==aliasesProcessed.end()) &&
        it!=g_aliasMap.end()) // expand the alias
    {
      //printf("is an alias!\n");
      if (!allowRecursion) aliasesProcessed.insert(cmd.str());
      QCString val(it->second);
      if (hasArgs)
      {
        val = replaceAliasArguments(aliasesProcessed,val,args,sep);
        //printf("replace '%s'->'%s' args='%s'\n",
        //       aliasText->data(),val.data(),args.data());
      }
      result+=expandAliasRec(aliasesProcessed,val);
      if (!allowRecursion) aliasesProcessed.erase(cmd.str());
      p=i+l;
      if (hasArgs) p+=argsLen+2;
    }
    else // command is not an alias
    {
      //printf("not an alias!\n");
      result+=match.str();
      p=i+l;
    }
  }
  result+=s.right(s.length()-p);

  //printf("expandAliases '%s'->'%s'\n",s.data(),result.data());
  if (result == s)
  {
    std::string orgStr = s.str();
    size_t ridx = orgStr.rfind('-');
    if (ridx != std::string::npos) return expandAliasRec(aliasesProcessed,s.left(ridx),allowRecursion) + s.right(s.length() - ridx);
  }

  return result;
}


static int countAliasArguments(const QCString &argList, const char sep)
{
  int count=1;
  int l = argList.length();
  int i;
  for (i=0;i<l;i++)
  {
    char c = argList.at(i);
    if (c==sep && (i==0 || argList.at(i-1)!='\\')) count++;
    else if (c=='@' || c=='\\')
    {
      // check if this is the start of another aliased command (see bug704172)
      i+=findEndOfCommand(argList.data()+i+1);
    }
  }
  //printf("countAliasArguments=%d\n",count);
  return count;
}

static QCString extractAliasArgs(const QCString &args,size_t pos)
{
  size_t i;
  int bc=0;
  char prevChar=0;
  if (args.at(pos)=='{') // alias has argument
  {
    for (i=pos;i<args.length();i++)
    {
      if (prevChar!='\\')
      {
        if (args.at(i)=='{') bc++;
        if (args.at(i)=='}') bc--;
        prevChar=args.at(i);
      }
      else
      {
        prevChar=0;
      }

      if (bc==0)
      {
        //printf("extractAliasArgs('%s')->'%s'\n",qPrint(args),qPrint(args.mid(pos+1,i-pos-1)));
        return args.mid(pos+1,i-pos-1);
      }
    }
  }
  return "";
}

QCString resolveAliasCmd(const QCString &aliasCmd)
{
  QCString result;
  StringUnorderedSet aliasesProcessed;
  //printf("Expanding: '%s'\n",qPrint(aliasCmd));
  result = expandAliasRec(aliasesProcessed,aliasCmd);
  //printf("Expanding result: '%s'->'%s'\n",qPrint(aliasCmd),qPrint(result));
  return result;
}

static std::string expandAlias(const std::string &aliasName,const std::string &aliasValue)
{
  QCString result;
  StringUnorderedSet aliasesProcessed;
  // avoid expanding this command recursively
  aliasesProcessed.insert(aliasName);
  // expand embedded commands
  //printf("Expanding: '%s'->'%s'\n",qPrint(aliasName),qPrint(aliasValue));
  result = expandAliasRec(aliasesProcessed,aliasValue.c_str());
  //printf("Expanding result: '%s'->'%s'\n",qPrint(aliasName),qPrint(result));
  return result.str();
}
