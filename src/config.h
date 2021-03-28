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

#include <ostream>

// note: this header file is generated from config.xml
#include "configvalues.h"

//! @{
//! some convenience macros for accessing the config options
//! mainly done like this for backward compatibility
//#if DYNAMIC_LOOKUP // for debug purposes
//#define Config_getString(val) (ConfigValues::instance().*((ConfigValues::InfoString*)ConfigValues::instance().get(#val))->item)
//#define Config_getBool(val)   (ConfigValues::instance().*((ConfigValues::InfoBool*)ConfigValues::instance().get(#val))->item)
//#define Config_getInt(val)    (ConfigValues::instance().*((ConfigValues::InfoInt*)ConfigValues::instance().get(#val))->item)
//#define Config_getEnum(val)   (ConfigValues::instance().*((ConfigValues::InfoString*)ConfigValues::instance().get(#val))->item)
//#define Config_getList(val)   (ConfigValues::instance().*((ConfigValues::InfoList*)ConfigValues::instance().get(#val))->item)
//#else // direct access
#define Config_getString(name) (ConfigValues::instance().name())
#define Config_getBool(name)   (ConfigValues::instance().name())
#define Config_getInt(name)    (ConfigValues::instance().name())
#define Config_getEnum(name)   (ConfigValues::instance().name())
#define Config_getList(name)   (ConfigValues::instance().name())
#define Config_updateString(name,value) (ConfigValues::instance().update_##name(value));
#define Config_updateBool(name,value)   (ConfigValues::instance().update_##name(value));
#define Config_updateInt(name,value)    (ConfigValues::instance().update_##name(value));
#define Config_updateEnum(name,value)   (ConfigValues::instance().update_##name(value));
#define Config_updateList(name,...)   (ConfigValues::instance().update_##name(__VA_ARGS__));
//#endif
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
  void writeTemplate(std::ostream &t,bool shortList,bool updateOnly=FALSE);

  /*! Writes a the differences between the current configuration and the
   *  template configuration to stream \a t.
   */
  void compareDoxyfile(std::ostream &t);

  /*! Writes a the used settings of the current configuartion as XML format
   *  to stream \a t.
   */
  void writeXMLDoxyfile(std::ostream &t);

  /*! Parses a configuration file with name \a fn.
   *  \returns TRUE if successful, FALSE if the file could not be
   *  opened or read.
   */
  bool parse(const char *fileName,bool update=FALSE);

  /*! Post processed the parsed data. Replaces raw string values by the actual values.
   *  and replaces environment variables.
   *  \param clearHeaderAndFooter set to TRUE when writing header and footer templates.
   *  \param compare signals if we in Doxyfile compare (`-x`) mode are or not. Influences
   *  setting of the default value.
   */
  void postProcess(bool clearHeaderAndFooter, bool compare = FALSE);

  /*! Check the validity of the parsed options and correct or warn the user where needed. */
  void checkAndCorrect();

  /*! Clean up any data */
  void deinit();
}

#endif
