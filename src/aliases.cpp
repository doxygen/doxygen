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

#include <unordered_map>
#include <cassert>

#include "message.h"
#include "aliases.h"
#include "containers.h"
#include "config.h"
#include "regex.h"
#include "textstream.h"
#include "util.h"
#include "debug.h"

//-----------------------------------------------------------

struct AliasInfo
{
  AliasInfo(const std::string &val,const std::string &sep=std::string())
    : value(val), separator(sep) {}
  std::string value;
  std::string separator;
};

using AliasOverloads = std::unordered_map<int,AliasInfo>;                // key = parameter count
using AliasInfoMap   = std::unordered_map<std::string,AliasOverloads>;   // key = alias name (with parameter part)

//-----------------------------------------------------------

static QCString expandAliasRec(StringUnorderedSet &aliasesProcessed,
                               const QCString &s,bool allowRecursion=FALSE);
static int countAliasArguments(const std::string &args, const std::string &sep);
static QCString extractAliasArgs(const QCString &args,size_t pos);
static std::string expandAlias(const std::string &aliasName,const std::string &aliasValue);

//-----------------------------------------------------------

static AliasInfoMap          g_aliasInfoMap;

//-----------------------------------------------------------

static void addValidAliasToMap(const std::string &aliasStr)
{
  bool        valid = true;
  std::string aliasName;
  std::string aliasValue;
  int         numParams = 0;
  std::string separator;

  static const char *separators="!#$%&,.?|;:'+=~`/";
  auto isValidSeparator = [](char c) -> bool
  {
    const char *p=separators;
    while (*p++) { if (c==*p) return true; }
    return false;
  };

  static const reg::Ex re1(R"(^\a[\w-]*\s*=)");
  static const reg::Ex re2(R"(^\a[\w-]*{[^}]*}\s*=)");
  if (reg::search(aliasStr,re1) || reg::search(aliasStr,re2)) // valid name= or name{...}= part
  {
    QCString alias=QCString(aliasStr);
    int i=alias.find('=');
    assert(i!=-1); // based on re1 and re2 there is always a =
    QCString name = alias.left(static_cast<size_t>(i)).stripWhiteSpace();
    aliasValue    = alias.right(alias.length()-i-1).str();
    //printf("Alias: found name='%s' value='%s'\n",qPrint(name),qPrint(value));
    int j=name.find('{');
    if (j>0) // alias with parameters
    {
      int l = static_cast<int>(name.length());
      if (name.at(l-1)=='}')
      {
        separator=",";
        int k=j+1;
        while (k<l-1 && isdigit(name[k])) k++;
        numParams = atoi(name.mid(j+1,k-j-1).data());
        if (numParams>0)
        {
          if (k<l-1) // we have a separator
          {
            int m=k;
            while (isValidSeparator(name[m])) m++;
            if (m<l-1)
            {
              err("Invalid alias '%s': invalid separator character '%c' (code %d), allowed characters: %s. Check your config file.\n",qPrint(alias),name[m],name[m],separators);
              valid=false;
            }
            else
            {
              separator=name.mid(k,l-k-1).str();
            }
          }
          if (valid) // valid alias with parameters
          {
            aliasName = name.left(j).str();
            Debug::print(Debug::Alias,0,"Alias definition: name='%s' #param='%d' separator='%s' value='%s'\n",
                qPrint(aliasName),numParams,qPrint(separator),qPrint(aliasValue));
          }
          else
          {
            err("Invalid alias '%s': invalid number of parameters. Check your config file.\n",qPrint(alias));
            valid=false;
          }
        }
        else
        {
          err("Invalid alias '%s': missing closing bracket. Check your config file.\n",qPrint(alias));
          valid=false;
        }
      }
    }
    else // valid alias without parameters
    {
      aliasName = name.str();
      numParams = 0;
      Debug::print(Debug::Alias,0,"Alias definition: name='%s' value='%s'\n",qPrint(aliasName),qPrint(aliasValue));
    }
  }
  else
  {
    err("Invalid alias '%s': invalid 'name=' or 'name{...}=' part. Check you config file.\n",qPrint(aliasStr));
    valid=false;
  }

  if (valid) // alias definition passed all checks, so store it.
  {
    auto it = g_aliasInfoMap.find(aliasName);
    if (it==g_aliasInfoMap.end()) // insert new alias
    {
      AliasOverloads overloads { { numParams, AliasInfo(aliasValue, separator) } };
      g_aliasInfoMap.insert(std::make_pair(aliasName,overloads));
    }
    else // replace exiting alias with new definition
    {
      auto it2 = it->second.find(numParams);
      if (it2==it->second.end()) // new alias overload for the given number of parameters
      {
        it->second.insert(std::make_pair(numParams, AliasInfo(aliasValue,separator)));
      }
      else // replace alias with new definition
      {
        it2->second = AliasInfo(aliasValue,separator);
      }
    }
  }
}


//----------------------------------------------------------------------------

static std::string escapeAlias(const std::string &aliasValue)
{
  QCString value(aliasValue);
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
  return newValue.str();
  //printf("Alias %s has value %s\n",name.c_str(),qPrint(newValue));
}

//----------------------------------------------------------------------------

void readAliases()
{
  // add aliases to a dictionary
  const StringVector &aliasList = Config_getList(ALIASES);
  for (const auto &al : aliasList)
  {
    addValidAliasToMap(al);
  }
  for (auto &[name,overloads] : g_aliasInfoMap)
  {
    for (auto &[numParams,aliasInfo] : overloads)
    {
      aliasInfo.value = expandAlias(name+":"+std::to_string(numParams),aliasInfo.value);
    }
  }
  for (auto &[name,overloads] : g_aliasInfoMap)
  {
    for (auto &[numParams,aliasInfo] : overloads)
    {
      aliasInfo.value = escapeAlias(aliasInfo.value);
    }
  }
}

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
                                      const std::string &aliasValue,const std::string &argList,const std::string &sep)
{
  //printf("----- replaceAliasArguments(val=[%s],args=[%s],sep=[%s])\n",qPrint(aliasValue),qPrint(argList),qPrint(sep));

  // first make a list of arguments from the comma separated argument list
  StringVector args;
  size_t i,l=argList.length();
  size_t p=0;
  for (i=0;i<l;i++)
  {
    char c = argList.at(i);
    if (c==sep[0] && // start with separator character
        (i==0 || argList.at(i-1)!='\\') && // is not escaped
        qstrncmp(argList.c_str()+i, sep.c_str(), sep.length())==0) // whole separator matches
    {
      args.push_back(argList.substr(p,i-p));
      p=i+sep.length(); // start of next argument
      i=p-1; // compensate with -1 for loop iterator
    }
    else if (c=='@' || c=='\\') // command
    {
      // check if this is the start of another aliased command (see bug704172)
      i+=findEndOfCommand(argList.c_str()+i+1);
    }
  }
  if (l>p) args.push_back(argList.substr(p));
  //printf("found %zu arguments\n",args.size());

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
                                  std::stoi(aliasValue.substr(markerStart,markerLen)),
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
  p=0;
  for (i=0;i<markerList.size();i++)
  {
    const Marker &m = markerList.at(i);
    result+=aliasValue.substr(p,m.pos-p);
    //printf("part before marker %d: '%s'\n",i,qPrint(aliasValue.mid(p,m->pos-p)));
    if (m.number>0 && m.number<=static_cast<int>(args.size())) // valid number
    {
      result+=expandAliasRec(aliasesProcessed,QCString(args.at(m.number-1)),true);
      //printf("marker index=%zu pos=%zu number=%d size=%zu replacement %s\n",i,m.pos,m.number,m.size,
      //    qPrint(args.at(m.number-1)));
    }
    p=m.pos+m.size; // continue after the marker
  }
  result+=aliasValue.substr(p); // append remainder
  //printf("string after replacement of markers: '%s'\n",qPrint(result));

  // expand the result again
  result = substitute(result,"\\{","{");
  result = substitute(result,"\\}","}");
  result = expandAliasRec(aliasesProcessed,substitute(result,QCString("\\"+sep),QCString(sep)));

  return result;
}

static std::string escapeSeparators(const std::string &s, const std::string &sep)
{
  if (s.empty() || sep.empty()) return s;
  std::string result;
  result.reserve(s.length()+10);
  size_t i,p=0,l=sep.length();
  while ((i=s.find(sep,p))!=std::string::npos)
  {
    result+=s.substr(p,i-p);
    if (i>0 && s[i-1]!='\\') // escape the separator
    {
      result+='\\';
    }
    result+=s.substr(i,l);
    p=i+l;
  }
  result+=s.substr(p);
  //printf("escapeSeparators(%s,sep='%s')=%s\n",qPrint(s),qPrint(sep),qPrint(result));
  return result;
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

    std::string args = extractAliasArgs(s,i+l).str();
    bool hasArgs = !args.empty();            // found directly after command
    size_t argsLen = args.length();
    QCString cmd = match[1].str();
    int selectedNumArgs = -1;
    //printf("looking for alias '%s' with params '%s'\n",qPrint(cmd),qPrint(args));
    auto it = g_aliasInfoMap.find(cmd.str());
    if (it == g_aliasInfoMap.end())
    {
      // if command has a - then also try part in without it
      int minusPos = cmd.find('-');
      if (minusPos!=-1)
      {
        it = g_aliasInfoMap.find(cmd.left(minusPos).str());
        if (it!=g_aliasInfoMap.end()) // found part before - as alias
        {
          cmd     = cmd.left(minusPos);
          args    = "";
          hasArgs = false;
          argsLen = 0;
          l       = cmd.length()+1; // +1 for the minus sign
        }
      }
    }
    if (it != g_aliasInfoMap.end()) // cmd is an alias
    {
      if (hasArgs)
      {
        // Find the an alias that matches the number of arguments.
        // If there are multiple candidates, take the one that matches the most parameters
        for (const auto &[numParams,aliasInfo] : it->second)
        {
          int numArgs = countAliasArguments(args,aliasInfo.separator);
          if (numParams==numArgs && numArgs>selectedNumArgs)
          {
            selectedNumArgs = numArgs;
          }
        }
        if (selectedNumArgs==-1) // no match found, check if there is an alias with one argument
        {
          auto it2 = it->second.find(1);
          if (it2 != it->second.end())
          {
            args = escapeSeparators(args,it2->second.separator); // escape separator so that everything is seen as one argument
            selectedNumArgs = 1;
          }
        }
      }
      else
      {
        selectedNumArgs=0;
      }
    }
    else
    {
      //printf("Alias %s not found\n",qPrint(cmd));
    }
    //printf("Found command s='%s' cmd='%s' numArgs=%d args='%s'\n", qPrint(s),qPrint(cmd),selectedNumArgs,qPrint(args));
    std::string qualifiedName = cmd.str()+":"+std::to_string(selectedNumArgs);
    if ((allowRecursion || aliasesProcessed.find(qualifiedName)==aliasesProcessed.end()) &&
        it!=g_aliasInfoMap.end() && selectedNumArgs!=-1 &&
        it->second.find(selectedNumArgs)!=it->second.end()) // expand the alias
    {
      const auto &aliasInfo = it->second.find(selectedNumArgs)->second;
      //printf("is an alias with separator='%s' hasArgs=%d!\n",qPrint(aliasInfo.separator),hasArgs);
      if (!allowRecursion) aliasesProcessed.insert(qualifiedName);
      std::string val = aliasInfo.value;
      if (hasArgs)
      {
        val = replaceAliasArguments(aliasesProcessed,val,args,aliasInfo.separator).str();
        //printf("replace sep='%s' val='%s' args='%s'\n",
        //       qPrint(aliasInfo.separator),qPrint(val),qPrint(args));
      }
      result+=expandAliasRec(aliasesProcessed,QCString(val));
      if (!allowRecursion) aliasesProcessed.erase(qualifiedName);
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
  //printf("expandAliases \"%s\"->\"%s\"\n",qPrint(s),qPrint(result));
  return result;
}


static int countAliasArguments(const std::string &args, const std::string &sep)
{
  int count=1;
  size_t l = args.length();
  size_t i;
  for (i=0;i<l;i++)
  {
    char c = args.at(i);
    if (c==sep[0] && // start with separator character
        (i==0 || args.at(i-1)!='\\') && // is not escaped
        qstrncmp(args.c_str()+i,sep.c_str(),sep.length())==0) // whole separator matches
    {
      count++;
    }
    else if (c=='@' || c=='\\')
    {
      // check if this is the start of another aliased command (see bug704172)
      i+=findEndOfCommand(args.c_str()+i+1);
    }
  }
  //printf("countAliasArguments(%s,sep=%s)=%zu\n",qPrint(args),qPrint(sep),count);
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
  Debug::print(Debug::Alias,0,"Resolving alias: cmd='%s' result='%s'\n",qPrint(aliasCmd),qPrint(result));
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
  result = expandAliasRec(aliasesProcessed,QCString(aliasValue));
  //printf("Expanding result: '%s'->'%s'\n",qPrint(aliasName),qPrint(result));
  Debug::print(Debug::Alias,0,"Expanding alias: input='%s' result='%s'\n",qPrint(aliasValue),qPrint(result));
  return result.str();
}

bool isAliasCmd(const QCString &aliasCmd)
{
  auto it    = g_aliasInfoMap.find(aliasCmd.str());
  return it != g_aliasInfoMap.end();
}
