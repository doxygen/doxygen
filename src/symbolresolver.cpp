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

#include <unordered_map>
#include <string>
#include <vector>

#include "symbolresolver.h"
#include "util.h"
#include "doxygen.h"
#include "namespacedef.h"
#include "config.h"
#include "defargs.h"

static std::mutex g_cacheMutex;
static std::recursive_mutex g_cacheTypedefMutex;

//--------------------------------------------------------------------------------------

/** Helper class representing the stack of items considered while resolving
 *  the scope.
 */
class AccessStack
{
    /** Element in the stack. */
    struct AccessElem
    {
      AccessElem(const Definition *d,const FileDef *f,const Definition *i) : scope(d), fileScope(f), item(i) {}
      AccessElem(const Definition *d,const FileDef *f,const Definition *i,const QCString &e) : scope(d), fileScope(f), item(i), expScope(e) {}
      const Definition *scope;
      const FileDef *fileScope;
      const Definition *item;
      QCString expScope;
    };
  public:
    void push(const Definition *scope,const FileDef *fileScope,const Definition *item)
    {
      m_elements.push_back(AccessElem(scope,fileScope,item));
    }
    void push(const Definition *scope,const FileDef *fileScope,const Definition *item,const QCString &expScope)
    {
      m_elements.push_back(AccessElem(scope,fileScope,item,expScope));
    }
    void pop()
    {
      if (!m_elements.empty()) m_elements.pop_back();
    }
    bool find(const Definition *scope,const FileDef *fileScope, const Definition *item)
    {
      auto it = std::find_if(m_elements.begin(),m_elements.end(),
                             [&](const AccessElem &e) { return e.scope==scope && e.fileScope==fileScope && e.item==item; });
      return it!=m_elements.end();
    }
    bool find(const Definition *scope,const FileDef *fileScope, const Definition *item,const QCString &expScope)
    {
      auto it = std::find_if(m_elements.begin(),m_elements.end(),
                             [&](const AccessElem &e) { return e.scope==scope && e.fileScope==fileScope && e.item==item && e.expScope==expScope; });
      return it!=m_elements.end();
    }
    void clear()
    {
      m_elements.clear();
    }

  private:
    std::vector<AccessElem> m_elements;
};

//--------------------------------------------------------------------------------------

using VisitedNamespaces = std::unordered_map<std::string,const Definition *>;

//--------------------------------------------------------------------------------------

struct SymbolResolver::Private
{
  public:
    Private(const FileDef *f) : m_fileScope(f) {}
    void reset()
    {
      m_resolvedTypedefs.clear();
      resolvedType.resize(0);
      typeDef = 0;
      templateSpec.resize(0);
    }
    void setFileScope(const FileDef *fileScope)
    {
      m_fileScope = fileScope;
    }

    QCString          resolvedType;
    const MemberDef  *typeDef = 0;
    QCString          templateSpec;

    const ClassDef *getResolvedTypeRec(
                           const Definition *scope,    // in
                           const QCString &n,          // in
                           const MemberDef **pTypeDef, // out
                           QCString *pTemplSpec,       // out
                           QCString *pResolvedType);   // out
                                                       //
    const Definition *getResolvedSymbolRec(
                           const Definition *scope,    // in
                           const QCString &n,          // in
                           const QCString &args,       // in
                           bool checkCV,               // in
                           const MemberDef **pTypeDef, // out
                           QCString *pTemplSpec,       // out
                           QCString *pResolvedType);   // out

    int isAccessibleFrom(  AccessStack &accessStack,
                           const Definition *scope,
                           const Definition *item);

    int isAccessibleFromWithExpScope(
                           VisitedNamespaces &visitedNamespaces,
                           AccessStack       &accessStack,
                           const Definition *scope,
                           const Definition *item,
                           const QCString &explicitScopePart);

  private:
    void getResolvedType(  const Definition *scope,                             // in
                           const Definition *d,                                 // in
                           const QCString &explicitScopePart,                   // in
                           const std::unique_ptr<ArgumentList> &actTemplParams, // in
                           int &minDistance,                                    // input
                           const ClassDef *&bestMatch,                          // out
                           const MemberDef *&bestTypedef,                       // out
                           QCString &bestTemplSpec,                             // out
                           QCString &bestResolvedType                           // out
                        );

    void getResolvedSymbol(const Definition *scope,                             // in
                           const Definition *d,                                 // in
                           const QCString &args,                                // in
                           bool  checkCV,                                       // in
                           const QCString &explicitScopePart,                   // in
                           const std::unique_ptr<ArgumentList> &actTemplParams, // in
                           int &minDistance,                                    // inout
                           const Definition *&bestMatch,                        // out
                           const MemberDef *&bestTypedef,                       // out
                           QCString &bestTemplSpec,                             // out
                           QCString &bestResolvedType                           // out
                          );

    const ClassDef *newResolveTypedef(
                           const Definition *scope,                             // in
                           const MemberDef *md,                                 // in
                           const MemberDef **pMemType,                          // out
                           QCString *pTemplSpec,                                // out
                           QCString *pResolvedType,                             // out
                           const std::unique_ptr<ArgumentList> &actTemplParams = std::unique_ptr<ArgumentList>()
                          );

    const Definition *followPath(const Definition *start,const QCString &path);

    const Definition *endOfPathIsUsedClass(const LinkedRefMap<const ClassDef> &cl,const QCString &localName);

    bool accessibleViaUsingNamespace(StringUnorderedSet &visited,
                                     const LinkedRefMap<const NamespaceDef> &nl,
                                     const Definition *item,
                                     const QCString &explicitScopePart="",
                                     int level=0);
    bool accessibleViaUsingClass(const LinkedRefMap<const ClassDef> &cl,
                                 const Definition *item,
                                 const QCString &explicitScopePart=""
                                );
    QCString substTypedef(const Definition *scope,const QCString &name,
                          const MemberDef **pTypeDef=0);

    const FileDef    *m_fileScope;
    std::unordered_map<std::string,const MemberDef*> m_resolvedTypedefs;
};



const ClassDef *SymbolResolver::Private::getResolvedTypeRec(
           const Definition *scope,
           const QCString &n,
           const MemberDef **pTypeDef,
           QCString *pTemplSpec,
           QCString *pResolvedType)
{
  if (n.isEmpty()) return 0;
  //static int level=0;
  //printf("\n%d [getResolvedTypeRec(%s,%s)\n",level++,scope?qPrint(scope->name()):"<global>",qPrint(n));
  QCString explicitScopePart;
  QCString strippedTemplateParams;
  QCString name=stripTemplateSpecifiersFromScope(n,TRUE,&strippedTemplateParams);
  std::unique_ptr<ArgumentList> actTemplParams;
  if (!strippedTemplateParams.isEmpty()) // template part that was stripped
  {
    actTemplParams = stringToArgumentList(scope->getLanguage(),strippedTemplateParams);
  }

  int qualifierIndex = computeQualifiedIndex(name);
  //printf("name=%s qualifierIndex=%d\n",qPrint(name),qualifierIndex);
  if (qualifierIndex!=-1) // qualified name
  {
    // split off the explicit scope part
    explicitScopePart=name.left(qualifierIndex);
    // todo: improve namespace alias substitution
    replaceNamespaceAliases(explicitScopePart,explicitScopePart.length());
    name=name.mid(qualifierIndex+2);
  }

  if (name.isEmpty())
  {
    //printf("%d ] empty name\n",--level);
    return 0; // empty name
  }

  //printf("Looking for type %s\n",qPrint(name));
  auto &range = Doxygen::symbolMap->find(name);
  if (range.empty())
  {
    return 0;
  }
  //printf("found type!\n");

  bool hasUsingStatements =
    (m_fileScope && (!m_fileScope->getUsedNamespaces().empty() ||
                     !m_fileScope->getUsedClasses().empty())
    );
  //printf("hasUsingStatements=%d\n",hasUsingStatements);
  // Since it is often the case that the same name is searched in the same
  // scope over an over again (especially for the linked source code generation)
  // we use a cache to collect previous results. This is possible since the
  // result of a lookup is deterministic. As the key we use the concatenated
  // scope, the name to search for and the explicit scope prefix. The speedup
  // achieved by this simple cache can be enormous.
  int scopeNameLen = scope->name().length()+1;
  int nameLen = name.length()+1;
  int explicitPartLen = explicitScopePart.length();
  int fileScopeLen = hasUsingStatements ? 1+m_fileScope->absFilePath().length() : 0;

  // below is a more efficient coding of
  // QCString key=scope->name()+"+"+name+"+"+explicitScopePart+args+typesOnly?'T':'F';
  QCString key(scopeNameLen+nameLen+explicitPartLen+fileScopeLen+1);
  char *pk=key.rawData();
  qstrcpy(pk,scope->name().data()); *(pk+scopeNameLen-1)='+';
  pk+=scopeNameLen;
  qstrcpy(pk,name.data()); *(pk+nameLen-1)='+';
  pk+=nameLen;
  qstrcpy(pk,explicitScopePart.data());
  pk+=explicitPartLen;

  // if a file scope is given and it contains using statements we should
  // also use the file part in the key (as a class name can be in
  // two different namespaces and a using statement in a file can select
  // one of them).
  if (hasUsingStatements)
  {
    // below is a more efficient coding of
    // key+="+"+m_fileScope->name();
    *pk++='+';
    qstrcpy(pk,m_fileScope->absFilePath().data());
    pk+=fileScopeLen-1;
  }
  *pk='\0';

  {
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    LookupInfo *pval = Doxygen::typeLookupCache->find(key.str());
    //printf("Searching for %s result=%p\n",qPrint(key),(void*)pval);
    if (pval)
    {
      //printf("LookupInfo %p %p '%s' %p\n",
      //    pval->classDef, pval->typeDef, qPrint(pval->templSpec),
      //    qPrint(pval->resolvedType));
      if (pTemplSpec)    *pTemplSpec=pval->templSpec;
      if (pTypeDef)      *pTypeDef=pval->typeDef;
      if (pResolvedType) *pResolvedType=pval->resolvedType;
      //printf("%d ] cachedMatch=%s\n",--level,
      //    pval->definition?qPrint(pval->definition->name()):"<none>");
      //if (pTemplSpec)
      //  printf("templSpec=%s\n",pTemplSpec->data());
      return toClassDef(pval->definition);
    }
    else // not found yet; we already add a 0 to avoid the possibility of
      // endless recursion.
    {
      Doxygen::typeLookupCache->insert(key.str(),LookupInfo());
    }
  }

  const ClassDef *bestMatch=0;
  const MemberDef *bestTypedef=0;
  QCString bestTemplSpec;
  QCString bestResolvedType;
  int minDistance=10000; // init at "infinite"

  for (Definition *d : range)
  {
    getResolvedType(scope,d,explicitScopePart,actTemplParams,
                    minDistance,bestMatch,bestTypedef,bestTemplSpec,bestResolvedType);
    if  (minDistance==0) break; // we can stop reaching if we already reached distance 0
  }

  if (pTypeDef)
  {
    *pTypeDef = bestTypedef;
  }
  if (pTemplSpec)
  {
    *pTemplSpec = bestTemplSpec;
  }
  if (pResolvedType)
  {
    *pResolvedType = bestResolvedType;
  }

  //printf("getResolvedSymbolRec: bestMatch=%p pval->resolvedType=%s\n",
  //    bestMatch,qPrint(bestResolvedType));

  {
    // we need to insert the item in the cache again, as it could be removed in the meantime
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    Doxygen::typeLookupCache->insert(key.str(),
                          LookupInfo(bestMatch,bestTypedef,bestTemplSpec,bestResolvedType));
  }
  //printf("%d ] bestMatch=%s distance=%d\n",--level,
  //    bestMatch?qPrint(bestMatch->name()):"<none>",minDistance);
  //if (pTemplSpec)
  //  printf("templSpec=%s\n",pTemplSpec->data());
  return bestMatch;
}

const Definition *SymbolResolver::Private::getResolvedSymbolRec(
           const Definition *scope,
           const QCString &n,
           const QCString &args,
           bool checkCV,
           const MemberDef **pTypeDef,
           QCString *pTemplSpec,
           QCString *pResolvedType)
{
  if (n.isEmpty()) return 0;
  //static int level=0;
  //printf("\n%d [getResolvedSymbolRec(%s,%s)\n",level++,scope?qPrint(scope->name()):"<global>",qPrint(n));
  QCString explicitScopePart;
  QCString strippedTemplateParams;
  QCString name=stripTemplateSpecifiersFromScope(n,TRUE,&strippedTemplateParams);
  std::unique_ptr<ArgumentList> actTemplParams;
  if (!strippedTemplateParams.isEmpty()) // template part that was stripped
  {
    actTemplParams = stringToArgumentList(scope->getLanguage(),strippedTemplateParams);
  }

  int qualifierIndex = computeQualifiedIndex(name);
  //printf("name=%s qualifierIndex=%d\n",qPrint(name),qualifierIndex);
  if (qualifierIndex!=-1) // qualified name
  {
    // split off the explicit scope part
    explicitScopePart=name.left(qualifierIndex);
    // todo: improve namespace alias substitution
    replaceNamespaceAliases(explicitScopePart,explicitScopePart.length());
    name=name.mid(qualifierIndex+2);
  }

  if (name.isEmpty())
  {
    //printf("%d ] empty name\n",--level);
    return 0; // empty name
  }

  //printf("Looking for symbol %s\n",qPrint(name));
  auto &range = Doxygen::symbolMap->find(name);
  if (range.empty())
  {
    //printf("%d ] not symbols\n",--level);
    return 0;
  }
  //printf("found symbol %zu times!\n",range.size());

  bool hasUsingStatements =
    (m_fileScope && (!m_fileScope->getUsedNamespaces().empty() ||
                     !m_fileScope->getUsedClasses().empty())
    );
  //printf("hasUsingStatements=%d\n",hasUsingStatements);
  // Since it is often the case that the same name is searched in the same
  // scope over an over again (especially for the linked source code generation)
  // we use a cache to collect previous results. This is possible since the
  // result of a lookup is deterministic. As the key we use the concatenated
  // scope, the name to search for and the explicit scope prefix. The speedup
  // achieved by this simple cache can be enormous.
  int scopeNameLen = scope->name().length()+1;
  int nameLen = name.length()+1;
  int explicitPartLen = explicitScopePart.length();
  int fileScopeLen = hasUsingStatements ? 1+m_fileScope->absFilePath().length() : 0;
  int argsLen = args.length()+1;

  // below is a more efficient coding of
  // QCString key=scope->name()+"+"+name+"+"+explicitScopePart+args+typesOnly?'T':'F';
  QCString key(scopeNameLen+nameLen+explicitPartLen+fileScopeLen+argsLen+1);
  char *pk=key.rawData();
  qstrcpy(pk,scope->name().data()); *(pk+scopeNameLen-1)='+';
  pk+=scopeNameLen;
  qstrcpy(pk,name.data()); *(pk+nameLen-1)='+';
  pk+=nameLen;
  qstrcpy(pk,explicitScopePart.data());
  pk+=explicitPartLen;

  // if a file scope is given and it contains using statements we should
  // also use the file part in the key (as a class name can be in
  // two different namespaces and a using statement in a file can select
  // one of them).
  if (hasUsingStatements)
  {
    // below is a more efficient coding of
    // key+="+"+m_fileScope->name();
    *pk++='+';
    qstrcpy(pk,m_fileScope->absFilePath().data());
    pk+=fileScopeLen-1;
  }
  if (argsLen>0)
  {
    qstrcpy(pk,args.data());
    pk+=argsLen-1;
  }
  *pk='\0';

  {
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    LookupInfo *pval = Doxygen::symbolLookupCache->find(key.str());
    //printf("Searching for %s result=%p\n",qPrint(key),(void*)pval);
    if (pval)
    {
      //printf("LookupInfo %p %p '%s' %p\n",
      //    pval->classDef, pval->typeDef, qPrint(pval->templSpec),
      //    qPrint(pval->resolvedType));
      if (pTemplSpec)    *pTemplSpec=pval->templSpec;
      if (pTypeDef)      *pTypeDef=pval->typeDef;
      if (pResolvedType) *pResolvedType=pval->resolvedType;
      //printf("%d ] cachedMatch=%s\n",--level,
      //    pval->definition?qPrint(pval->definition->name()):"<none>");
      //if (pTemplSpec)
      //  printf("templSpec=%s\n",pTemplSpec->data());
      return pval->definition;
    }
    else // not found yet; we already add a 0 to avoid the possibility of
      // endless recursion.
    {
      Doxygen::symbolLookupCache->insert(key.str(),LookupInfo());
    }
  }

  const Definition *bestMatch=0;
  const MemberDef *bestTypedef=0;
  QCString bestTemplSpec;
  QCString bestResolvedType;
  int minDistance=10000; // init at "infinite"

  for (Definition *d : range)
  {
    getResolvedSymbol(scope,d,args,checkCV,explicitScopePart,actTemplParams,
                      minDistance,bestMatch,bestTypedef,bestTemplSpec,bestResolvedType);
    if  (minDistance==0) break; // we can stop reaching if we already reached distance 0
  }

  // in case we are looking for e.g. func() and the real function is func(int x) we also
  // accept func(), see example 036 in the test set.
  if (bestMatch==0 && args=="()")
  {
    for (Definition *d : range)
    {
      getResolvedSymbol(scope,d,QCString(),false,explicitScopePart,actTemplParams,
                      minDistance,bestMatch,bestTypedef,bestTemplSpec,bestResolvedType);
      if  (minDistance==0) break; // we can stop reaching if we already reached distance 0
    }
  }

  if (pTypeDef)
  {
    *pTypeDef = bestTypedef;
  }
  if (pTemplSpec)
  {
    *pTemplSpec = bestTemplSpec;
  }
  if (pResolvedType)
  {
    *pResolvedType = bestResolvedType;
  }

  //printf("getResolvedSymbolRec: bestMatch=%p pval->resolvedType=%s\n",
  //    bestMatch,qPrint(bestResolvedType));

  {
    // we need to insert the item in the cache again, as it could be removed in the meantime
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    Doxygen::symbolLookupCache->insert(key.str(),
                          LookupInfo(bestMatch,bestTypedef,bestTemplSpec,bestResolvedType));
  }
  //printf("%d ] bestMatch=%s distance=%d\n",--level,
  //    bestMatch?qPrint(bestMatch->name()):"<none>",minDistance);
  //if (pTemplSpec)
  //  printf("templSpec=%s\n",pTemplSpec->data());
  return bestMatch;
}

void SymbolResolver::Private::getResolvedType(
                         const Definition *scope,                             // in
                         const Definition *d,                                 // in
                         const QCString &explicitScopePart,                   // in
                         const std::unique_ptr<ArgumentList> &actTemplParams, // in
                         int &minDistance,                                    // inout
                         const ClassDef *&bestMatch,                          // out
                         const MemberDef *&bestTypedef,                       // out
                         QCString &bestTemplSpec,                             // out
                         QCString &bestResolvedType                           // out
                      )
{
  //fprintf(stderr,"getResolvedType(%s,%s)\n",qPrint(scope->name()),qPrint(d->qualifiedName()));
  // only look at classes and members that are enums or typedefs
  if (d->definitionType()==Definition::TypeClass ||
      (d->definitionType()==Definition::TypeMember &&
       ((toMemberDef(d))->isTypedef() ||
        (toMemberDef(d))->isEnumerate())
      )
     )
  {
    VisitedNamespaces visitedNamespaces;
    AccessStack accessStack;
    // test accessibility of definition within scope.
    int distance = isAccessibleFromWithExpScope(visitedNamespaces,accessStack,scope,d,explicitScopePart);
    //fprintf(stderr,"  %s; distance %s (%p) is %d\n",qPrint(scope->name()),qPrint(d->name()),d,distance);
    if (distance!=-1) // definition is accessible
    {
      // see if we are dealing with a class or a typedef
      if (d->definitionType()==Definition::TypeClass) // d is a class
      {
        const ClassDef *cd = toClassDef(d);
        //printf("cd=%s\n",qPrint(cd->name()));
        if (!cd->isTemplateArgument()) // skip classes that
          // are only there to
          // represent a template
          // argument
        {
          //printf("is not a templ arg\n");
          if (distance<minDistance) // found a definition that is "closer"
          {
            minDistance=distance;
            bestMatch = cd;
            bestTypedef = 0;
            bestTemplSpec.resize(0);
            bestResolvedType = cd->qualifiedName();
          }
          else if (distance==minDistance &&
              m_fileScope && bestMatch &&
              !m_fileScope->getUsedNamespaces().empty() &&
              d->getOuterScope()->definitionType()==Definition::TypeNamespace &&
              bestMatch->getOuterScope()==Doxygen::globalScope
              )
          {
            // in case the distance is equal it could be that a class X
            // is defined in a namespace and in the global scope. When searched
            // in the global scope the distance is 0 in both cases. We have
            // to choose one of the definitions: we choose the one in the
            // namespace if the fileScope imports namespaces and the definition
            // found was in a namespace while the best match so far isn't.
            // Just a non-perfect heuristic but it could help in some situations
            // (kdecore code is an example).
            minDistance=distance;
            bestMatch = cd;
            bestTypedef = 0;
            bestTemplSpec.resize(0);
            bestResolvedType = cd->qualifiedName();
          }
        }
        else
        {
          //printf("  is a template argument!\n");
        }
      }
      else if (d->definitionType()==Definition::TypeMember)
      {
        const MemberDef *md = toMemberDef(d);
        //fprintf(stderr,"  member isTypedef()=%d\n",md->isTypedef());
        if (md->isTypedef()) // d is a typedef
        {
          QCString args=md->argsString();
          if (args.isEmpty()) // do not expand "typedef t a[4];"
          {
            //printf("    found typedef!\n");

            // we found a symbol at this distance, but if it didn't
            // resolve to a class, we still have to make sure that
            // something at a greater distance does not match, since
            // that symbol is hidden by this one.
            if (distance<minDistance)
            {
              QCString spec;
              QCString type;
              minDistance=distance;
              const MemberDef *enumType = 0;
              const ClassDef *cd = newResolveTypedef(scope,md,&enumType,&spec,&type,actTemplParams);
              if (cd)  // type resolves to a class
              {
                //printf("      bestTypeDef=%p spec=%s type=%s\n",md,qPrint(spec),qPrint(type));
                bestMatch = cd;
                bestTypedef = md;
                bestTemplSpec = spec;
                bestResolvedType = type;
              }
              else if (enumType) // type resolves to a member type
              {
                //printf("      is enum\n");
                bestMatch = 0;
                bestTypedef = enumType;
                bestTemplSpec = "";
                bestResolvedType = enumType->qualifiedName();
              }
              else if (md->isReference()) // external reference
              {
                bestMatch = 0;
                bestTypedef = md;
                bestTemplSpec = spec;
                bestResolvedType = type;
              }
              else
              {
                bestMatch = 0;
                bestTypedef = md;
                bestTemplSpec.resize(0);
                bestResolvedType.resize(0);
                //printf("      no match\n");
              }
            }
            else
            {
              //printf("      not the best match %d min=%d\n",distance,minDistance);
            }
          }
          else
          {
            //printf("     not a simple typedef\n")
          }
        }
        else if (md->isEnumerate())
        {
          if (distance<minDistance)
          {
            minDistance=distance;
            bestMatch = 0;
            bestTypedef = md;
            bestTemplSpec = "";
            bestResolvedType = md->qualifiedName();
          }
        }
      }
    } // if definition accessible
    else
    {
      //printf("  Not accessible!\n");
    }
  } // if definition is a class or member
  //printf("  bestMatch=%p bestResolvedType=%s\n",bestMatch,qPrint(bestResolvedType));
}


void SymbolResolver::Private::getResolvedSymbol(
                         const Definition *scope,                             // in
                         const Definition *d,                                 // in
                         const QCString &args,                                // in
                         bool  checkCV,                                       // in
                         const QCString &explicitScopePart,                   // in
                         const std::unique_ptr<ArgumentList> &actTemplParams, // in
                         int &minDistance,                                    // inout
                         const Definition *&bestMatch,                        // out
                         const MemberDef *&bestTypedef,                       // out
                         QCString &bestTemplSpec,                             // out
                         QCString &bestResolvedType                           // out
                      )
{
  //fprintf(stderr,"getResolvedSymbol(%s,%s)\n",qPrint(scope->name()),qPrint(d->qualifiedName()));
  // only look at classes and members that are enums or typedefs
  VisitedNamespaces visitedNamespaces;
  AccessStack accessStack;
  // test accessibility of definition within scope.
  int distance = isAccessibleFromWithExpScope(visitedNamespaces,accessStack,scope,d,explicitScopePart);
  //fprintf(stderr,"  %s; distance %s (%p) is %d\n",qPrint(scope->name()),qPrint(d->name()),d,distance);
  //printf("%s: distance=%d scope=%s explScope=%s\n",qPrint(d->name()),distance,qPrint(scope?scope->name():QCString()),qPrint(explicitScopePart));
  if (distance!=-1) // definition is accessible
  {
    // see if we are dealing with a class or a typedef
    if (d->definitionType()==Definition::TypeClass) // d is a class
    {
      const ClassDef *cd = toClassDef(d);
      //printf("cd=%s\n",qPrint(cd->name()));
      if (!cd->isTemplateArgument()) // skip classes that
        // are only there to
        // represent a template
        // argument
      {
        //printf("is not a templ arg\n");
        if (distance<minDistance) // found a definition that is "closer"
        {
          minDistance=distance;
          bestMatch = d;
          bestTypedef = 0;
          bestTemplSpec.resize(0);
          bestResolvedType = cd->qualifiedName();
        }
        else if (distance==minDistance &&
            m_fileScope && bestMatch &&
            !m_fileScope->getUsedNamespaces().empty() &&
            d->getOuterScope()->definitionType()==Definition::TypeNamespace &&
            bestMatch->getOuterScope()==Doxygen::globalScope
            )
        {
          // in case the distance is equal it could be that a class X
          // is defined in a namespace and in the global scope. When searched
          // in the global scope the distance is 0 in both cases. We have
          // to choose one of the definitions: we choose the one in the
          // namespace if the fileScope imports namespaces and the definition
          // found was in a namespace while the best match so far isn't.
          // Just a non-perfect heuristic but it could help in some situations
          // (kdecore code is an example).
          minDistance=distance;
          bestMatch = d;
          bestTypedef = 0;
          bestTemplSpec.resize(0);
          bestResolvedType = cd->qualifiedName();
        }
      }
      else
      {
        //printf("  is a template argument!\n");
      }
    }
    else if (d->definitionType()==Definition::TypeMember)
    {
      const MemberDef *md = toMemberDef(d);

      bool match = true;
      //printf("@@ checking %s\n",qPrint(md->name()));
      if (md->isFunction() && !args.isEmpty())
      {
        std::unique_ptr<ArgumentList> argList = stringToArgumentList(md->getLanguage(),args);
        const ArgumentList &mdAl = md->argumentList();
        match = matchArguments2(md->getOuterScope(),md->getFileDef(),&mdAl,
              scope, md->getFileDef(),argList.get(),
              checkCV,md->getLanguage());
        //printf("@@ %s (%p): matching %s against %s -> %d\n",qPrint(md->name()),(void*)md,qPrint(args),qPrint(argListToString(mdAl)),match);
      }

      //fprintf(stderr,"  member isTypedef()=%d\n",md->isTypedef());
      if (match && distance<minDistance)
      {
        minDistance=distance;
        bestMatch = md;
        bestTypedef = md;
        bestTemplSpec = "";
        bestResolvedType = md->qualifiedName();
      }
    }
    else if ((d->definitionType()==Definition::TypeNamespace ||
              d->definitionType()==Definition::TypeFile))
    {
      if (distance<minDistance) // found a definition that is "closer"
      {
        minDistance=distance;
        bestMatch = d;
        bestTypedef = 0;
        bestTemplSpec.resize(0);
        bestResolvedType.resize(0);
      }
    }
  } // if definition accessible
  else
  {
    //printf("  Not accessible!\n");
  }
  //printf("bestMatch=%s bestResolvedType=%s\n",qPrint(bestMatch?bestMatch->name():"<none>"),qPrint(bestResolvedType));
}


const ClassDef *SymbolResolver::Private::newResolveTypedef(
                  const Definition *scope,                             // in
                  const MemberDef *md,                                 // in
                  const MemberDef **pMemType,                          // out
                  QCString *pTemplSpec,                                // out
                  QCString *pResolvedType,                             // out
                  const std::unique_ptr<ArgumentList> &actTemplParams) // in
{
  std::lock_guard<std::recursive_mutex> lock(g_cacheTypedefMutex);
  //printf("newResolveTypedef(md=%p,cachedVal=%p)\n",md,md->getCachedTypedefVal());
  bool isCached = md->isTypedefValCached(); // value already cached
  if (isCached)
  {
    //printf("Already cached %s->%s [%s]\n",
    //    qPrint(md->name()),
    //    md->getCachedTypedefVal()?qPrint(md->getCachedTypedefVal()->name()):"<none>",
    //    md->getCachedResolvedTypedef()?qPrint(md->getCachedResolvedTypedef()):"<none>");

    if (pTemplSpec)    *pTemplSpec    = md->getCachedTypedefTemplSpec();
    if (pResolvedType) *pResolvedType = md->getCachedResolvedTypedef();
    return md->getCachedTypedefVal();
  }
  //printf("new typedef\n");
  QCString qname = md->qualifiedName();
  if (m_resolvedTypedefs.find(qname.str())!=m_resolvedTypedefs.end())
  {
    return 0; // typedef already done
  }

  auto typedef_it = m_resolvedTypedefs.insert({qname.str(),md}).first; // put on the trace list

  const ClassDef *typeClass = md->getClassDef();
  QCString type = md->typeString(); // get the "value" of the typedef
  if (typeClass && typeClass->isTemplate() &&
      actTemplParams && !actTemplParams->empty())
  {
    type = substituteTemplateArgumentsInString(type,
            typeClass->templateArguments(),actTemplParams);
  }
  QCString typedefValue = type;
  int tl=type.length();
  int ip=tl-1; // remove * and & at the end
  while (ip>=0 && (type.at(ip)=='*' || type.at(ip)=='&' || type.at(ip)==' '))
  {
    ip--;
  }
  type=type.left(ip+1);
  type.stripPrefix("const ");  // strip leading "const"
  type.stripPrefix("volatile ");  // strip leading "volatile"
  type.stripPrefix("struct "); // strip leading "struct"
  type.stripPrefix("union ");  // strip leading "union"
  int sp=0;
  tl=type.length(); // length may have been changed
  while (sp<tl && type.at(sp)==' ') sp++;
  const MemberDef *memTypeDef = 0;
  const ClassDef *result = getResolvedTypeRec(md->getOuterScope(),type,
                                                &memTypeDef,0,pResolvedType);
  // if type is a typedef then return what it resolves to.
  if (memTypeDef && memTypeDef->isTypedef())
  {
    result=newResolveTypedef(m_fileScope,memTypeDef,pMemType,pTemplSpec,0);
    goto done;
  }
  else if (memTypeDef && memTypeDef->isEnumerate() && pMemType)
  {
    *pMemType = memTypeDef;
  }

  //printf("type=%s result=%p\n",qPrint(type),result);
  if (result==0)
  {
    // try unspecialized version if type is template
    int si=type.findRev("::");
    int i=type.find('<');
    if (si==-1 && i!=-1) // typedef of a template => try the unspecialized version
    {
      if (pTemplSpec) *pTemplSpec = type.mid(i);
      result = getResolvedTypeRec(md->getOuterScope(),type.left(i),0,0,pResolvedType);
      //printf("result=%p pRresolvedType=%s sp=%d ip=%d tl=%d\n",
      //    result,pResolvedType?pResolvedType->data():"<none>",sp,ip,tl);
    }
    else if (si!=-1) // A::B
    {
      i=type.find('<',si);
      if (i==-1) // Something like A<T>::B => lookup A::B
      {
        i=type.length();
      }
      else // Something like A<T>::B<S> => lookup A::B, spec=<S>
      {
        if (pTemplSpec) *pTemplSpec = type.mid(i);
      }
      result = getResolvedTypeRec(md->getOuterScope(),
           stripTemplateSpecifiersFromScope(type.left(i),FALSE),0,0,pResolvedType);
    }

    //if (result) ip=si+sp+1;
  }

done:
  if (pResolvedType)
  {
    if (result && result->definitionType()==Definition::TypeClass)
    {
      *pResolvedType = result->qualifiedName();
      //printf("*pResolvedType=%s\n",pResolvedType->data());
      if (sp>0)    pResolvedType->prepend(typedefValue.left(sp));
      if (ip<tl-1) pResolvedType->append(typedefValue.right(tl-ip-1));
    }
    else
    {
      *pResolvedType = typedefValue;
    }
  }

  // remember computed value for next time
  if (result && result->getDefFileName()!="<code>")
    // this check is needed to prevent that temporary classes that are
    // introduced while parsing code fragments are being cached here.
  {
    //printf("setting cached typedef %p in result %p\n",md,result);
    //printf("==> %s (%s,%d)\n",qPrint(result->name()),qPrint(result->getDefFileName()),result->getDefLine());
    //printf("*pResolvedType=%s\n",pResolvedType?pResolvedType->data():"<none>");
    MemberDefMutable *mdm = toMemberDefMutable(md);
    if (mdm)
    {
      mdm->cacheTypedefVal(result,
        pTemplSpec ? *pTemplSpec : QCString(),
        pResolvedType ? *pResolvedType : QCString()
       );
    }
  }

  m_resolvedTypedefs.erase(typedef_it); // remove from the trace list

  return result;
}

#if 0
static bool isParentScope(const Definition *parent,const Definition *item)
{
  if (parent==item || item==0 || item==Doxygen::globalScope) return false;
  if (parent==0 || parent==Doxygen::globalScope)             return true;
  return isParentScope(parent->getOuterScope(),item);
}
#endif

int SymbolResolver::Private::isAccessibleFromWithExpScope(
                                     VisitedNamespaces &visitedNamespaces,
                                     AccessStack       &accessStack,
                                     const Definition *scope,
                                     const Definition *item,
                                     const QCString &explicitScopePart)
{
  if (explicitScopePart.isEmpty())
  {
    // handle degenerate case where there is no explicit scope.
    return isAccessibleFrom(accessStack,scope,item);
  }

  if (accessStack.find(scope,m_fileScope,item,explicitScopePart))
  {
    return -1;
  }
  accessStack.push(scope,m_fileScope,item,explicitScopePart);


  //printf(" <isAccessibleFromWithExpScope(%s,%s,%s)\n",scope?qPrint(scope->name()):"<global>",
  //                                      item?qPrint(item->qualifiedName()):"<none>",
  //                                      qPrint(explicitScopePart));
  int result=0; // assume we found it
  const Definition *newScope = followPath(scope,explicitScopePart);
  if (newScope)  // explicitScope is inside scope => newScope is the result
  {
    Definition *itemScope = item->getOuterScope();

    //printf("    scope traversal successful %s<->%s!\n",qPrint(itemScope->name()),qPrint(newScope->name()));

    bool nestedClassInsideBaseClass =
         itemScope &&
         itemScope->definitionType()==Definition::TypeClass &&
         newScope->definitionType()==Definition::TypeClass &&
         (toClassDef(newScope))->isBaseClass(toClassDef(itemScope),TRUE);

    bool enumValueWithinEnum =
         item->definitionType()==Definition::TypeMember &&
         toMemberDef(item)->isEnumValue() &&
         toMemberDef(item)->getEnumScope()==newScope;

    //if (newScope && newScope->definitionType()==Definition::TypeClass)
    //{
    //  ClassDef *cd = (ClassDef *)newScope;
    //  printf("---> Class %s: bases=%p\n",qPrint(cd->name()),cd->baseClasses());
    //}
    if (itemScope==newScope)  // exact match of scopes => distance==0
    {
      //printf(" > found it\n");
    }
    else if (nestedClassInsideBaseClass)
    {
      // inheritance is also ok. Example: looking for B::I, where
      // class A { public: class I {} };
      // class B : public A {}
      // but looking for B::I, where
      // class A { public: class I {} };
      // class B { public: class I {} };
      // will find A::I, so we still prefer a direct match and give this one a distance of 1
      result=1;

      //printf("scope(%s) is base class of newScope(%s)\n",
      //    qPrint(scope->name()),qPrint(newScope->name()));
    }
    else if (enumValueWithinEnum)
    {
      result=1;
    }
    else
    {
      int i=-1;
      if (newScope->definitionType()==Definition::TypeNamespace)
      {
        visitedNamespaces.insert({newScope->name().str(),newScope});
        // this part deals with the case where item is a class
        // A::B::C but is explicit referenced as A::C, where B is imported
        // in A via a using directive.
        //printf("newScope is a namespace: %s!\n",qPrint(newScope->name()));
        const NamespaceDef *nscope = toNamespaceDef(newScope);
        for (const auto &cd : nscope->getUsedClasses())
        {
          //printf("Trying for class %s\n",qPrint(cd->name()));
          if (cd==item)
          {
            goto done;
          }
        }
        for (const auto &nd : nscope->getUsedNamespaces())
        {
          if (visitedNamespaces.find(nd->name().str())==visitedNamespaces.end())
          {
            //printf("Trying for namespace %s\n",qPrint(nd->name()));
            i = isAccessibleFromWithExpScope(visitedNamespaces,accessStack,scope,item,nd->name());
            if (i!=-1)
            {
              //printf("> found via explicit scope of used namespace\n");
              goto done;
            }
          }
        }
      }
#if 0  // this caused problems resolving A::f() in the docs when there was a A::f(int) but also a
       // global function f() that exactly matched the argument list.
      else if (isParentScope(scope,newScope) && newScope->definitionType()==Definition::TypeClass)
      {
        // if we a look for a type B and have explicit scope A, then it is also fine if B
        // is found at the global scope.
        result = 1;
        goto done;
      }
#endif
      // repeat for the parent scope
      if (scope!=Doxygen::globalScope)
      {
        i = isAccessibleFromWithExpScope(visitedNamespaces,accessStack,scope->getOuterScope(),item,explicitScopePart);
      }
      //printf("  | result=%d\n",i);
      result = (i==-1) ? -1 : i+2;
    }
  }
  else // failed to resolve explicitScope
  {
    //printf("    failed to resolve explicitScope=%s: scope=%s\n",qPrint(explicitScopePart), qPrint(scope->name()));
    if (scope->definitionType()==Definition::TypeNamespace)
    {
      const NamespaceDef *nscope = toNamespaceDef(scope);
      StringUnorderedSet visited;
      if (accessibleViaUsingNamespace(visited,nscope->getUsedNamespaces(),item,explicitScopePart))
      {
        //printf(" > found in used namespace\n");
        goto done;
      }
    }
    if (scope==Doxygen::globalScope)
    {
      if (m_fileScope)
      {
        StringUnorderedSet visited;
        if (accessibleViaUsingNamespace(visited,m_fileScope->getUsedNamespaces(),item,explicitScopePart))
        {
          //printf(" > found in used namespace\n");
          goto done;
        }
      }
      //printf(" > not found\n");
      result=-1;
    }
    else // continue by looking into the parent scope
    {
      int i=isAccessibleFromWithExpScope(visitedNamespaces,accessStack,scope->getOuterScope(),item,explicitScopePart);
      //printf(" > result=%d\n",i);
      result= (i==-1) ? -1 : i+2;
    }
  }

done:
  //printf(" > result=%d\n",result);
  accessStack.pop();
  return result;
}

const Definition *SymbolResolver::Private::followPath(const Definition *start,const QCString &path)
{
  int is,ps;
  int l;
  const Definition *current=start;
  ps=0;
  //printf("followPath: start='%s' path='%s'\n",start?qPrint(start->name()):"<none>",qPrint(path));
  // for each part of the explicit scope
  while ((is=getScopeFragment(path,ps,&l))!=-1)
  {
    // try to resolve the part if it is a typedef
    const MemberDef *memTypeDef=0;
    QCString qualScopePart = substTypedef(current,path.mid(is,l),&memTypeDef);
    //printf("       qualScopePart=%s\n",qPrint(qualScopePart));
    if (memTypeDef)
    {
      const ClassDef *type = newResolveTypedef(m_fileScope,memTypeDef,0,0,0);
      if (type)
      {
        //printf("Found type %s\n",qPrint(type->name()));
        return type;
      }
    }
    const Definition *next = current->findInnerCompound(qualScopePart);
    //printf("++ Looking for %s inside %s result %s\n",
    //     qPrint(qualScopePart),
    //     qPrint(current->name()),
    //     next?qPrint(next->name()):"<null>");
    if (next==0)
    {
      next = current->findInnerCompound(qualScopePart+"-p");
    }
    if (current->definitionType()==Definition::TypeClass)
    {
      const MemberDef *classMember = toClassDef(current)->getMemberByName(qualScopePart);
      if (classMember && classMember->isEnumerate())
      {
        next = classMember;
      }
    }
    else if (current->definitionType()==Definition::TypeNamespace)
    {
      const MemberDef *namespaceMember = toNamespaceDef(current)->getMemberByName(qualScopePart);
      if (namespaceMember && namespaceMember->isEnumerate())
      {
        next = namespaceMember;
      }
    }
    else if (current==Doxygen::globalScope || current->definitionType()==Definition::TypeFile)
    {
       auto &range = Doxygen::symbolMap->find(qualScopePart);
       for (Definition *def : range)
       {
         const Definition *outerScope = def->getOuterScope();
         if (
             (outerScope==Doxygen::globalScope || // global scope or
              (outerScope && // anonymous namespace in the global scope
               outerScope->name().startsWith("anonymous_namespace{") &&
               outerScope->getOuterScope()==Doxygen::globalScope
              )
             ) &&
             (def->definitionType()==Definition::TypeClass ||
              def->definitionType()==Definition::TypeMember ||
              def->definitionType()==Definition::TypeNamespace
             )
            )
         {
           next=def;
           break;
         }
       }
    }
    if (next==0) // failed to follow the path
    {
      //printf("==> next==0!\n");
      if (current->definitionType()==Definition::TypeNamespace)
      {
        next = endOfPathIsUsedClass(
            (toNamespaceDef(current))->getUsedClasses(),qualScopePart);
      }
      else if (current->definitionType()==Definition::TypeFile)
      {
        next = endOfPathIsUsedClass(
            (toFileDef(current))->getUsedClasses(),qualScopePart);
      }
      current = next;
      if (current==0) break;
    }
    else // continue to follow scope
    {
      current = next;
      //printf("==> current = %p\n",(void*)current);
    }
    ps=is+l;
  }
  //printf("followPath(start=%s,path=%s) result=%s\n",
  //    qPrint(start->name()),qPrint(path),current?qPrint(current->name()):"<null>");
  return current; // path could be followed
}

const Definition *SymbolResolver::Private::endOfPathIsUsedClass(const LinkedRefMap<const ClassDef> &cl,const QCString &localName)
{
  for (const auto &cd : cl)
  {
    if (cd->localName()==localName)
    {
      return cd;
    }
  }
  return 0;
}

bool SymbolResolver::Private::accessibleViaUsingNamespace(StringUnorderedSet &visited,
                                 const LinkedRefMap<const NamespaceDef> &nl,
                                 const Definition *item,
                                 const QCString &explicitScopePart,
                                 int level)
{
  //size_t count=0;
  for (const auto &und : nl) // check used namespaces for the class
  {
    //printf("%d [Trying via used namespace %s: count=%zu/%zu\n",level,qPrint(und->name()),
    //    count++,nl.size());
    const Definition *sc = explicitScopePart.isEmpty() ? und : followPath(und,explicitScopePart);
    if (sc && item->getOuterScope()==sc)
    {
      //printf("%d ] found it\n",level);
      return true;
    }
    if (item->getLanguage()==SrcLangExt_Cpp)
    {
      QCString key=und->qualifiedName();
      if (!und->getUsedNamespaces().empty() && visited.insert(key.str()).second)
      {
        if (accessibleViaUsingNamespace(visited,und->getUsedNamespaces(),item,explicitScopePart,level+1))
        {
          //printf("%d ] found it via recursion\n",level);
          return true;
        }

      }
    }
    //printf("%d ] Try via used namespace done\n",level);
  }
  return false;
}


bool SymbolResolver::Private::accessibleViaUsingClass(const LinkedRefMap<const ClassDef> &cl,
                                                      const Definition *item,
                                                      const QCString &explicitScopePart)
{
  for (const auto &ucd : cl)
  {
    //printf("Trying via used class %s\n",qPrint(ucd->name()));
    const Definition *sc = explicitScopePart.isEmpty() ? ucd : followPath(ucd,explicitScopePart);
    if (sc && sc==item) return true;
    //printf("Try via used class done\n");
  }
  return false;
}

int SymbolResolver::Private::isAccessibleFrom(AccessStack &accessStack,
                                              const Definition *scope,
                                              const Definition *item)
{
  //printf("<isAccessibleFrom(scope=%s,item=%s itemScope=%s)\n",
  //    qPrint(scope->name()),qPrint(item->name()),qPrint(item->getOuterScope()->name()));

  if (accessStack.find(scope,m_fileScope,item))
  {
    return -1;
  }
  accessStack.push(scope,m_fileScope,item);

  int result=0; // assume we found it
  int i;

  const Definition *itemScope=item->getOuterScope();
  bool itemIsMember = item->definitionType()==Definition::TypeMember;
  bool itemIsClass  = item->definitionType()==Definition::TypeClass;

  // if item is a global member and scope points to a specific file
  // we adjust the scope so the file gets preference over members with the same name in
  // other files.
  if ((itemIsMember || itemIsClass) &&
      (itemScope==Doxygen::globalScope || // global
       (itemScope && itemScope->name().startsWith("anonymous_namespace{")) // member of an anonymous namespace
      ) &&
      scope->definitionType()==Definition::TypeFile)
  {
    if (itemIsMember)
    {
      itemScope = toMemberDef(item)->getFileDef();
    }
    else if (itemIsClass)
    {
      itemScope = toClassDef(item)->getFileDef();
    }
    //printf("adjust scope to %s\n",qPrint(itemScope?itemScope->name():QCString()));
  }

  bool memberAccessibleFromScope =
      (itemIsMember &&                                                     // a member
       itemScope && itemScope->definitionType()==Definition::TypeClass  && // of a class
       scope->definitionType()==Definition::TypeClass &&                   // accessible
       (toClassDef(scope))->isAccessibleMember(toMemberDef(item)) // from scope
      );
  bool nestedClassInsideBaseClass =
      (itemIsClass &&                                                      // a nested class
       itemScope && itemScope->definitionType()==Definition::TypeClass &&  // inside a base
       scope->definitionType()==Definition::TypeClass &&                   // class of scope
       (toClassDef(scope))->isBaseClass(toClassDef(itemScope),TRUE)
      );
  bool enumValueOfStrongEnum =
      (itemIsMember &&
       toMemberDef(item)->isStrongEnumValue() &&
       scope->definitionType()==Definition::TypeMember &&
       toMemberDef(scope)->isEnumerate() &&
       scope==toMemberDef(item)->getEnumScope()
      );

  if (itemScope==scope || memberAccessibleFromScope || nestedClassInsideBaseClass || enumValueOfStrongEnum)
  {
    //printf("> found it memberAccessibleFromScope=%d nestedClassInsideBaseClass=%d enumValueOfStrongEnum=%d\n",memberAccessibleFromScope,nestedClassInsideBaseClass,enumValueOfStrongEnum);
    int distanceToBase=0;
    if (nestedClassInsideBaseClass)
    {
      result++; // penalty for base class to prevent
                                              // this is preferred over nested class in this class
                                              // see bug 686956
    }
    else if (memberAccessibleFromScope &&
             itemScope &&
             itemScope->definitionType()==Definition::TypeClass &&
             scope->definitionType()==Definition::TypeClass &&
             (distanceToBase=toClassDef(scope)->isBaseClass(toClassDef(itemScope),TRUE))>0
            )
    {
      result+=distanceToBase; // penalty if member is accessible via a base class
    }
  }
  else if (scope==Doxygen::globalScope)
  {
    if (itemScope &&
        itemScope->definitionType()==Definition::TypeNamespace &&
        toNamespaceDef(itemScope)->isAnonymous() &&
        itemScope->getOuterScope()==Doxygen::globalScope)
    { // item is in an anonymous namespace in the global scope and we are
      // looking in the global scope
      //printf("> found in anonymous namespace\n");
      result++;
      goto done;
    }
    if (m_fileScope)
    {
      if (accessibleViaUsingClass(m_fileScope->getUsedClasses(),item))
      {
        //printf("> found via used class\n");
        goto done;
      }
      StringUnorderedSet visited;
      if (accessibleViaUsingNamespace(visited,m_fileScope->getUsedNamespaces(),item))
      {
        //printf("> found via used namespace\n");
        goto done;
      }
    }
    //printf("> reached global scope\n");
    result=-1; // not found in path to globalScope
  }
  else // keep searching
  {
    // check if scope is a namespace, which is using other classes and namespaces
    if (scope->definitionType()==Definition::TypeNamespace)
    {
      const NamespaceDef *nscope = toNamespaceDef(scope);
      //printf("  %s is namespace with %d used classes\n",qPrint(nscope->name()),nscope->getUsedClasses());
      if (accessibleViaUsingClass(nscope->getUsedClasses(),item))
      {
        //printf("> found via used class\n");
        goto done;
      }
      StringUnorderedSet visited;
      if (accessibleViaUsingNamespace(visited,nscope->getUsedNamespaces(),item,0))
      {
        //printf("> found via used namespace\n");
        goto done;
      }
    }
    // repeat for the parent scope
    const Definition *parentScope = scope->getOuterScope();
    if (parentScope==Doxygen::globalScope)
    {
      if (scope->definitionType()==Definition::TypeClass)
      {
        const FileDef *fd = toClassDef(scope)->getFileDef();
        if (fd)
        {
          parentScope = fd;
        }
      }
    }
    i=isAccessibleFrom(accessStack,parentScope,item);
    //printf("> result=%d\n",i);
    result= (i==-1) ? -1 : i+2;
  }
done:
  accessStack.pop();
  return result;
}

QCString SymbolResolver::Private::substTypedef(
                          const Definition *scope,const QCString &name,
                          const MemberDef **pTypeDef)
{
  QCString result=name;
  if (name.isEmpty()) return result;

  auto &range = Doxygen::symbolMap->find(name);
  if (range.empty())
    return result; // no matches

  MemberDef *bestMatch=0;
  int minDistance=10000; // init at "infinite"

  for (Definition *d : range)
  {
    // only look at members
    if (d->definitionType()==Definition::TypeMember)
    {
      // that are also typedefs
      MemberDef *md = toMemberDef(d);
      if (md->isTypedef()) // d is a typedef
      {
        VisitedNamespaces visitedNamespaces;
        AccessStack accessStack;
        // test accessibility of typedef within scope.
        int distance = isAccessibleFromWithExpScope(visitedNamespaces,accessStack,scope,d,"");
        if (distance!=-1 && distance<minDistance)
          // definition is accessible and a better match
        {
          minDistance=distance;
          bestMatch = md;
        }
      }
    }
  }

  if (bestMatch)
  {
    result = bestMatch->typeString();
    if (pTypeDef) *pTypeDef=bestMatch;
  }

  //printf("substTypedef(%s,%s)=%s\n",scope?qPrint(scope->name()):"<global>",
  //                                  qPrint(name),qPrint(result));
  return result;
}

//----------------------------------------------------------------------------------------------


SymbolResolver::SymbolResolver(const FileDef *fileScope)
  : p(std::make_unique<Private>(fileScope))
{
}

SymbolResolver::~SymbolResolver()
{
}


const ClassDef *SymbolResolver::resolveClass(const Definition *scope,
                                             const QCString &name,
                                             bool mayBeUnlinkable,
                                             bool mayBeHidden)
{
  p->reset();

  if (scope==0 ||
      (scope->definitionType()!=Definition::TypeClass &&
       scope->definitionType()!=Definition::TypeNamespace
      ) ||
      (name.stripWhiteSpace().startsWith("::")) ||
      (scope->getLanguage()==SrcLangExt_Java && QCString(name).find("::")!=-1)
     )
  {
    scope=Doxygen::globalScope;
  }
  //fprintf(stderr,"------------ resolveClass(scope=%s,name=%s,mayUnlinkable=%d)\n",
  //    scope?qPrint(scope->name()):"<global>",
  //    qPrint(name),
  //    mayBeUnlinkable
  //   );
  const ClassDef *result=0;
  if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
  {
    result = getClass(name);
  }
  else
  {
    result = p->getResolvedTypeRec(scope,name,&p->typeDef,&p->templateSpec,&p->resolvedType);
    if (result==0) // for nested classes imported via tag files, the scope may not
                   // present, so we check the class name directly as well.
                   // See also bug701314
    {
      result = getClass(name);
    }
  }
  if (!mayBeUnlinkable && result && !result->isLinkable())
  {
    if (!mayBeHidden || !result->isHidden())
    {
      //fprintf(stderr,"result was %s\n",result?qPrint(result->name()):"<none>");
      result=0; // don't link to artificial/hidden classes unless explicitly allowed
    }
  }
  //fprintf(stderr,"ResolvedClass(%s,%s)=%s\n",scope?qPrint(scope->name()):"<global>",
  //                                  qPrint(name),result?qPrint(result->name()):"<none>");
  return result;
}

const Definition *SymbolResolver::resolveSymbol(const Definition *scope,
                                                const QCString &name,
                                                const QCString &args,
                                                bool checkCV)
{
  p->reset();
  if (scope==0) scope=Doxygen::globalScope;
  const Definition *result = p->getResolvedSymbolRec(scope,name,args,checkCV,&p->typeDef,&p->templateSpec,&p->resolvedType);
  //printf("resolveSymbol(%s,%s,%s,%d)=%s\n",qPrint(scope?scope->name():QCString()),qPrint(name),qPrint(args),checkCV,qPrint(result?result->qualifiedName():QCString()));
  return result;
}

int SymbolResolver::isAccessibleFrom(const Definition *scope,const Definition *item)
{
  p->reset();
  AccessStack accessStack;
  return p->isAccessibleFrom(accessStack,scope,item);
}

int SymbolResolver::isAccessibleFromWithExpScope(const Definition *scope,const Definition *item,
                                                 const QCString &explicitScopePart)
{
  p->reset();
  VisitedNamespaces visitedNamespaces;
  AccessStack accessStack;
  return p->isAccessibleFromWithExpScope(visitedNamespaces,accessStack,scope,item,explicitScopePart);
}

void SymbolResolver::setFileScope(const FileDef *fileScope)
{
  p->setFileScope(fileScope);
}

const MemberDef *SymbolResolver::getTypedef() const
{
  return p->typeDef;
}

QCString SymbolResolver::getTemplateSpec() const
{
  return p->templateSpec;
}

QCString SymbolResolver::getResolvedType() const
{
  return p->resolvedType;
}

