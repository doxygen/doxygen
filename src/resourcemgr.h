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
#ifndef RESOURCEMGR_H
#define RESOURCEMGR_H

#include <memory>
#include <initializer_list>

#include "qcstring.h"
#include "construct.h"

/** @brief Compiled resource */
struct Resource
{
  enum Type { Verbatim, SVG };
  const char *category;
  const char *name;
  const unsigned char *data;
  int size;
  Type type;
};

/** @brief Singleton for managing resources compiled into an executable */
class ResourceMgr
{
  public:
    /** Returns the one and only instance of this class */
    static ResourceMgr &instance();

    /** Registers an array of resources */
    void registerResources(std::initializer_list<Resource> resources);

    /** Writes all resource belonging to a given category to a given target directory */
    bool writeCategory(const QCString &categoryName,const QCString &targetDir) const;

    /** Copies a registered resource to a given target directory */
    bool copyResource(const QCString &name,const QCString &targetDir) const;

    /** Copies a registered resource to a given target directory under a given target name */
    bool copyResourceAs(const QCString &name,const QCString &targetDir,const QCString &targetName, bool append=false) const;

    /** Gets the resource data as a C string */
    QCString getAsString(const QCString &name) const;

  private:
    /** Returns a pointer to the resource object with the given name. */
    const Resource *get(const QCString &name) const;

    ResourceMgr();
   ~ResourceMgr();
    NON_COPYABLE(ResourceMgr)

    class Private;
    std::unique_ptr<Private> p;
};

#endif
