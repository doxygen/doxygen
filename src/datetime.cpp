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

#include <cstdlib>
#include <chrono>
#include <memory>
#include <array>
#include <functional>

#include "regex.h"
#include "datetime.h"
#include "config.h"
#include "portable.h"
#include "language.h"
#include "message.h"

std::tm getCurrentDateTime()
{
  QCString sourceDateEpoch = Portable::getenv("SOURCE_DATE_EPOCH");
  if (!sourceDateEpoch.isEmpty()) // see https://reproducible-builds.org/specs/source-date-epoch/
  {
    bool ok = false;
    uint64_t epoch = sourceDateEpoch.toUInt64(&ok);
    if (!ok)
    {
      static bool warnedOnce=FALSE;
      if (!warnedOnce)
      {
        warn_uncond("Environment variable SOURCE_DATE_EPOCH does not contain a valid number; value is '{}'\n",
            sourceDateEpoch);
        warnedOnce=TRUE;
      }
    }
    else // use given epoch value as current 'built' time
    {
      auto epoch_start    = std::chrono::time_point<std::chrono::system_clock>{};
      auto epoch_seconds  = std::chrono::seconds(epoch);
      auto build_time     = epoch_start + epoch_seconds;
      std::time_t time    = std::chrono::system_clock::to_time_t(build_time);
      return *gmtime(&time);
    }
  }

  // return current local time
  auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);
  return *localtime(&time);
}

QCString dateToString(DateTimeType includeTime)
{
  auto current = getCurrentDateTime();
  return theTranslator->trDateTime(current.tm_year + 1900,
                                   current.tm_mon + 1,
                                   current.tm_mday,
                                   (current.tm_wday+6)%7+1, // map: Sun=0..Sat=6 to Mon=1..Sun=7
                                   current.tm_hour,
                                   current.tm_min,
                                   current.tm_sec,
                                   includeTime);
}

QCString yearToString()
{
  auto current = getCurrentDateTime();
  return QCString().setNum(current.tm_year+1900);
}

struct SpecFormat
{
  const reg::Ex re;
  int count;
  int offset;
  int format;
};

using TMFieldAssigner = std::function< void(std::tm &,int value) >;

struct DateTimeField
{
  TMFieldAssigner assigner;
  int minVal;
  int maxVal;
  const char *name;
};

static std::array g_specFormats
{
  // regular expression,                            num values, offset, format bits
  SpecFormat{ std::string_view(R"((\d+)-(\d+)-(\d+)\s*(\d+):(\d+):(\d+))"),  6,  0,  SF_Date|SF_Time|SF_Seconds }, // format 13-04-2015 12:34:56
  SpecFormat{ std::string_view(R"((\d+)-(\d+)-(\d+)\s*(\d+):(\d+))"),        5,  0,  SF_Date|SF_Time            }, // format 13-04-2015 12:34
  SpecFormat{ std::string_view(R"((\d+)-(\d+)-(\d+))"),                      3,  0,  SF_Date                    }, // format 13-04-2015
  SpecFormat{ std::string_view(R"((\d+):(\d+):(\d+))"),                      3,  3,  SF_Time|SF_Seconds         }, // format 12:34:56
  SpecFormat{ std::string_view(R"((\d+):(\d+))"),                            2,  3,  SF_Time                    }  // format 12:34
};

static std::array g_assignValues
{
  // assigner,                                            minVal,     maxVal,    name
  DateTimeField{ [](std::tm &tm,int value) { tm.tm_year = value-1900; }, 1900,       9999,    "year"   },
  DateTimeField{ [](std::tm &tm,int value) { tm.tm_mon  = value-1;    },    1,         12,    "month"  },
  DateTimeField{ [](std::tm &tm,int value) { tm.tm_mday = value;      },    1,         31,    "day"    },
  DateTimeField{ [](std::tm &tm,int value) { tm.tm_hour = value;      },    0,         23,    "hour"   },
  DateTimeField{ [](std::tm &tm,int value) { tm.tm_min  = value;      },    0,         59,    "minute" },
  DateTimeField{ [](std::tm &tm,int value) { tm.tm_sec  = value;      },    0,         59,    "second" }
};

static void determine_weekday( std::tm& tm )
{
  auto cpy = tm;
  // there are some problems when the hr:min:sec are on 00:00:00 in determining the weekday
  cpy.tm_hour = 12;
  const auto as_time_t = std::mktime( &cpy ) ;
  if (as_time_t != -1)
  {
    cpy = *std::localtime( &as_time_t ) ;
    tm.tm_wday = cpy.tm_wday;
  }
}

QCString dateTimeFromString(const QCString &spec,std::tm &dt,int &format)
{
  // for an empty spec field return the current date and time
  dt = getCurrentDateTime();
  if (spec.isEmpty())
  {
    format = SF_Date | SF_Time | SF_Seconds;
    return QCString();
  }

  // find a matching pattern
  const std::string &s = spec.str();
  for (const auto &fmt : g_specFormats)
  {
    reg::Match m;
    if (reg::match(s,m,fmt.re)) // match found
    {
      for (int i=0; i<fmt.count; i++)
      {
        int value = std::atoi(m[i+1].str().c_str());
        const DateTimeField &dtf = g_assignValues[i+fmt.offset];
        if (value<dtf.minVal || value>dtf.maxVal) // check if the value is in the expected range
        {
          return QCString().sprintf("value for %s is %d which is outside of the value range [%d..%d]",
              dtf.name, value, dtf.minVal, dtf.maxVal);
        }
        dtf.assigner(dt,value);
      }
      format = fmt.format;
      if (format&SF_Date) // if we have a date also determine the weekday
      {
        determine_weekday(dt);
      }
      return QCString();
    }
  }

  // no matching pattern found
  return "invalid or non representable date/time argument";
}

QCString formatDateTime(const QCString &format,const std::tm &dt,int &formatUsed)
{
  formatUsed = 0;
  auto getYear      = [](const std::tm &dat) { return dat.tm_year+1900;    };
  auto getMonth     = [](const std::tm &dat) { return dat.tm_mon+1;        };
  auto getDay       = [](const std::tm &dat) { return dat.tm_mday;         };
  auto getDayOfWeek = [](const std::tm &dat) { return (dat.tm_wday+6)%7+1; };
  QCString result;
  result.reserve(256);
  auto addInt = [&result](const char *fmt,int value) {
    char tmp[50];
    qsnprintf(tmp,50,fmt,value);
    result+=tmp;
  };
  char c = 0;
  const char *p            = format.data();
  const char *fmt_zero     = "%02d";
  const char *fmt_nonzero  = "%d";
  const char *fmt_selected = nullptr;
  if (p==nullptr) return QCString();
  while ((c=*p++))
  {
    char nc = *p;
    switch (c)
    {
      case '%':
        fmt_selected = nc=='-' ? fmt_nonzero : fmt_zero; // %-H produces 1 and %H produces 01
        if (nc=='-') nc=*++p; // skip over -
        switch (nc)
        {
          case '%': result+='%';                                                                                       break;
          case 'y': addInt(fmt_selected,getYear(dt)%100);                              formatUsed|=SF_Date;    break;
          case 'Y': addInt("%d",getYear(dt));                                          formatUsed|=SF_Date;    break;
          case 'm': addInt(fmt_selected,getMonth(dt));                                 formatUsed|=SF_Date;    break;
          case 'b': result+=theTranslator->trMonth(getMonth(dt),false,false);          formatUsed|=SF_Date;    break;
          case 'B': result+=theTranslator->trMonth(getMonth(dt),false,true);           formatUsed|=SF_Date;    break;
          case 'd': addInt(fmt_selected,getDay(dt));                                   formatUsed|=SF_Date;    break;
          case 'u': addInt("%d",getDayOfWeek(dt));   /* Monday = 1 ... Sunday = 7 */   formatUsed|=SF_Date;    break;
          case 'w': addInt("%d",getDayOfWeek(dt)%7); /* Sunday = 0 ... Saturday = 6 */ formatUsed|=SF_Date;    break;
          case 'a': result+=theTranslator->trDayOfWeek(getDayOfWeek(dt),false,false);  formatUsed|=SF_Date;    break;
          case 'A': result+=theTranslator->trDayOfWeek(getDayOfWeek(dt),false,true);   formatUsed|=SF_Date;    break;
          case 'H': addInt(fmt_selected,dt.tm_hour);                                   formatUsed|=SF_Time;    break;
          case 'I': addInt(fmt_selected,dt.tm_hour%12);                                formatUsed|=SF_Time;    break;
          case 'p': result+=theTranslator->trDayPeriod(dt.tm_hour>=12);                formatUsed|=SF_Time;    break;
          case 'M': addInt(fmt_selected,dt.tm_min);                                    formatUsed|=SF_Time;    break;
          case 'S': addInt(fmt_selected,dt.tm_sec);                                    formatUsed|=SF_Seconds; break;
          default:
            result+=c;
            if (*(p-1)=='-') result+='-';
            result+=nc;
            break;
        }
        p++;
        break;
      default:
        result+=c;
        break;
    }
  }
  return result;
}

