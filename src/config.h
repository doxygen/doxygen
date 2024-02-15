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
#define Config_getEnumAsString(name)   (ConfigValues::instance().name##_str())
#define Config_getStringAsEnum(name,value)   (name##_str2enum(value))
#define Config_getList(name)   (ConfigValues::instance().name())
#define Config_updateString(name,value) (ConfigValues::instance().update_##name(value));
#define Config_updateBool(name,value)   (ConfigValues::instance().update_##name(value));
#define Config_updateInt(name,value)    (ConfigValues::instance().update_##name(value));
#define Config_updateEnum(name,value)   (ConfigValues::instance().update_##name(value));
#define Config_updateList(name,...)   (ConfigValues::instance().update_##name(__VA_ARGS__));

#define Config_isAvailableEnum(name,value)   (ConfigValues::instance().isAvailable_##name(value))
//#endif
//! @}

class TextStream;

/** \brief Public function to deal with the configuration file. */
namespace Config
{
  enum class CompareMode { Full, Compressed, CompressedNoEnv };

  /*! Initialize configuration variables to their default value */
  void init();

  /*! Writes a template configuration to stream \a t. If \a shortList
   *  is \c TRUE the description of each configuration option will
   *  be omitted.
   */
  void writeTemplate(TextStream &t,bool shortList,bool updateOnly=FALSE);

  /*! Writes a the differences between the current configuration and the
   *  template configuration to stream \a t.
   */
  void compareDoxyfile(TextStream &t, CompareMode compareMode);

  /*! Writes a the used settings of the current configuration as XML format
   *  to stream \a t.
   */
  void writeXMLDoxyfile(TextStream &t);

  /*! Writes all possible setting ids to an XSD file for validation 
   *  through the stream \a t.
   */
  void writeXSDDoxyfile(TextStream &t);

  /*! Parses a configuration file with name \a fn.
   *  \returns TRUE if successful, FALSE if the file could not be
   *  opened or read.
   */
  bool parse(const QCString &fileName,bool update=FALSE, CompareMode compareMode = CompareMode::Full);

  /*! Post processed the parsed data. Replaces raw string values by the actual values.
   *  and replaces environment variables.
   *  \param clearHeaderAndFooter set to TRUE when writing header and footer templates.
   *  \param compareMode signals if we in Doxyfile compare (`-x` or `-x_noenv`) mode are or not.
   *   Influences setting of the default value and replacement of environment variables and CMake type replacement variables.
   */
  void postProcess(bool clearHeaderAndFooter, CompareMode compareMode = CompareMode::Full);

  /*! Check the validity of the parsed options and correct or warn the user where needed.
   * \param quiet setting for the QUIET option (can have been overruled by means of a command line option)
   * \param check check  HTML / LaTeX header file etc. on existence (and terminate when not present)
   */
  void checkAndCorrect(bool quiet, const bool check);

  /*! Adjust any configuration values based on the value of obsolete options. */
  void updateObsolete();

  /*! Clean up any data */
  void deinit();
}

#endif
