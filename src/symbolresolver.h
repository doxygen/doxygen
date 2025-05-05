/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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

#ifndef SYMBOLRESOLVER_H
#define SYMBOLRESOLVER_H

#include <memory>

#include "qcstring.h"
#include "classdef.h"
#include "construct.h"

class Definition;
class FileDef;
class MemberDef;

//! Helper class to find a class definition or check if
//! A symbol is accessible in a given scope.
class SymbolResolver
{
  public:
    explicit SymbolResolver(const FileDef *fileScope = nullptr);
   ~SymbolResolver();
    NON_COPYABLE(SymbolResolver)

    // actions

    /** Find the class definition matching name within
     *  the scope set.
     *  @param scope The scope to search from.
     *  @param name  The name of the symbol.
     *  @param maybeUnlinkable include unlinkable symbols in the search.
     *  @param mayBeHidden include hidden symbols in the search.
     *  @note As a result of this call the getters getTypedef(),
     *  getTemplateSpec(), and getResolvedType() are set as well.
     */
    const ClassDef *resolveClass(const Definition *scope,
                                 const QCString &name,
                                 bool maybeUnlinkable=false,
                                 bool mayBeHidden=false);

    /** Wrapper around resolveClass that returns a mutable interface to
     *  the class object or a nullptr if the symbol is immutable.
     */
    ClassDefMutable *resolveClassMutable(const Definition *scope,
                                         const QCString &name,
                                         bool mayBeUnlinkable=false,
                                         bool mayBeHidden=false)
    {
      return toClassDefMutable(const_cast<ClassDef*>(resolveClass(scope,name,mayBeUnlinkable,mayBeHidden)));
    }

    /** Find the symbool definition matching name within the scope set.
     *  @param scope   The scope to search from.
     *  @param name    The name of the symbol.
     *  @param args    Argument list associated with the symbol (for functions)
     *  @param checkCV Check const/volatile qualifiers (for methods)
     *  @param insideCode Is the symbol found in a code fragment
     *  @param onlyLinkable Only search linkable definitions
     */
    const Definition *resolveSymbol(const Definition *scope,
                                    const QCString &name,
                                    const QCString &args=QCString(),
                                    bool checkCV=false,
                                    bool insideCode=false,
                                    bool onlyLinkable=false
                                   );

    /** Checks if symbol \a item is accessible from within \a scope.
     *  @returns -1 if \a item is not accessible or a number indicating how
     *  many scope levels up the nearest match was found.
     */
    int isAccessibleFrom(const Definition *scope,
                         const Definition *item);

    /** Check if symbol \a item is accessible from within \a scope,
     *  where it has to match the \a explicitScopePart.
     *  @returns -1 if \a item is not accessible or a number indicating how
     *  many scope levels up the nearest match was found.
     */
    int isAccessibleFromWithExpScope(const Definition *scope,
                                     const Definition *item,
                                     const QCString &explicitScopePart
                                    );

    /** Sets or updates the file scope using when resolving symbols. */
    void setFileScope(const FileDef *fd);

    // getters

    /** In case a call to resolveClass() resolves to a type member (e.g. an enum)
     *  this method will return it.
     */
    const MemberDef *getTypedef() const;

    /** In case a call to resolveClass() points to a template specialization, the
     *  template part is return via this method.
     */
    QCString   getTemplateSpec() const;

    /** In case a call to resolveClass() points to a typedef or using declaration.
     *  The type name it resolved to is returned via this method.
     */
    QCString   getResolvedType() const;

  private:
    struct Private;
    std::unique_ptr<Private> p;
};

#endif
