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

#ifndef CONFIG_H
#define CONFIG_H

class FTextStream;

// note: this header file is generated from config.xml
#include "configvalues.h"

//! @{
//! some convenience macros for accessing the config options
//! mainly done like this for backward compatibility
#if DYNAMIC_LOOKUP // for debug purposes
#define Config_getString(val) (ConfigValues::instance().*((ConfigValues::InfoString*)ConfigValues::instance().get(#val))->item)
#define Config_getBool(val)   (ConfigValues::instance().*((ConfigValues::InfoBool*)ConfigValues::instance().get(#val))->item)
#define Config_getInt(val)    (ConfigValues::instance().*((ConfigValues::InfoInt*)ConfigValues::instance().get(#val))->item)
#define Config_getEnum(val)   (ConfigValues::instance().*((ConfigValues::InfoString*)ConfigValues::instance().get(#val))->item)
#define Config_getList(val)   (ConfigValues::instance().*((ConfigValues::InfoList*)ConfigValues::instance().get(#val))->item)
#else // direct access
#define Config_getString(val) (ConfigValues::instance().val)
#define Config_getBool(val)   (ConfigValues::instance().val)
#define Config_getInt(val)    (ConfigValues::instance().val)
#define Config_getEnum(val)   (ConfigValues::instance().val)
#define Config_getList(val)   (ConfigValues::instance().val)
#endif
//! @}

/** \brief Public function to deal with the configuration file. */
namespace Config
{
  /*! Initialize configuration variables to their default value */
  void init();

  /*! Writes a template configuration to stream \a t. If \a shortList
   *  is \c TRUE the description of each configuration option will
   *  be omitted.
   */
  void writeTemplate(FTextStream &t,bool shortList,bool updateOnly=FALSE);

  /*! Parses a configuration file with name \a fn.
   *  \returns TRUE if successful, FALSE if the file could not be
   *  opened or read.
   */
  bool parse(const char *fileName,bool update=FALSE);

  /*! Post processed the parsed data. Replaces raw string values by the actual values.
   *  and replaces environment variables.
   *  \param clearHeaderAndFooter set to TRUE when writing header and footer templates.
   */
  void postProcess(bool clearHeaderAndFooter);

  /*! Check the validity of the parsed options and correct or warn the user where needed. */
  void checkAndCorrect();

  /*! Clean up any data */
  void deinit();
}

#endif
