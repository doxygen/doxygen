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

#ifndef DATETIME_H
#define DATETIME_H

#include <ctime>
#include "qcstring.h"

/** @brief Date and time related functions. */

constexpr int SF_Date    = 1<<0; //!< a date is presenting in the format string
constexpr int SF_Time    = 1<<1; //!< a time is presenting in the format string
constexpr int SF_Seconds = 1<<2; //!< the seconds are presenting in the format string
constexpr int SF_NumBits = 3;    //!< number of bits in SF vector

/** Helper function that returns the name related one of the SF bits. Used for generating warnings.
 *  @param[in] bitNumber bit value in range [0..SF_NumBits) for which to return the string value.
 */
constexpr const char *SF_bit2str(int bitNumber)
{
  constexpr const char *partNames[] = { "date", "time", "seconds" };
  return bitNumber>=0 && bitNumber<SF_NumBits ? partNames[bitNumber] : "";
}

enum class DateTimeType { DateTime,
                          Date,
                          Time
                        };

/** Returns the filled in std::tm for a given string representing a date and/or time.
 *
 *  @param[in] spec The string representation of the date and/or time
 *  Possible formats:
 *  - the empty string:      the current date and time is returned
 *  - `YYYY-MM-DD HH:MM:SS`: the date and time are fully specified
 *  - `YYYY-MM-DD HH:MM`:    the date and time without seconds
 *  - `YYYY-MM-DD`:          the date without time
 *  - `HH:MM:SS`:            the time with seconds but without date
 *  - `HH:MM`:               the time without seconds and without date
 *
 *  @param[out] dt  The corresponding datetime value.
 *  @param[out] format The parts that have been found in spec; a bitwise or
 *              of `SF_Date`, `SF_Time` and `SF_Seconds`.
 *  @returns An empty string if the spec has a supported format,
 *  or an error message if the format is invalid.
 */
QCString dateTimeFromString(const QCString &spec,std::tm &dt,int &format);


/** Return a string representation for a given std::tm value that is formatted according to the
 *  pattern given by a format.
 *  @param[in] format the string used for format the date and time, e.g. `%Y-%m-%d`
 *  @param[in] dt     the date and time value to fill in
 *  @param[out] formatUsed A bitwise OR of `SF_Date`, `SF_Time` and `SF_Seconds` representing the
 *                         the types of markers found in the format string.
 */
QCString formatDateTime(const QCString &format,const std::tm &dt,int &formatUsed);

/** Returns the filled in std::tm for the current date and time */
std::tm getCurrentDateTime();

/** Returns the current year as a string */
QCString yearToString();

/** Returns the current date, when \c includeTime is set also the time is provided.
 *  @param[in] includeTime include the time in the output
 */
QCString dateToString(DateTimeType includeTime);

#endif
