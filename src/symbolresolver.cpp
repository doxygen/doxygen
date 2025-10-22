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
#include <cassert>

#include "symbolresolver.h"
#include "util.h"
#include "doxygen.h"
#include "namespacedef.h"
#include "config.h"
#include "defargs.h"
#include "trace.h"

#if !ENABLE_SYMBOLRESOLVER_TRACING
#undef  AUTO_TRACE
#undef  AUTO_TRACE_ADD
#undef  AUTO_TRACE_EXIT
#define AUTO_TRACE(...)      (void)0
#define AUTO_TRACE_ADD(...)  (void)0
#define AUTO_TRACE_EXIT(...) (void)0
#endif

static std::mutex g_cacheMutex;
static std::recursive_mutex g_cacheTypedefMutex;

static std::mutex g_substMapMutex;
static std::unordered_map<std::string, std::pair<QCString,const MemberDef *> > g_substMap;

//--------------------------------------------------------------------------------------

static bool isCodeSymbol(Definition::DefType defType)
{
  return defType==Definition::TypeClass   || defType==Definition::TypeNamespace ||
         defType==Definition::TypeModule  || defType==Definition::TypeMember    ||
         defType==Definition::TypePackage || defType==Definition::TypeConcept;
}

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
      m_elements.emplace_back(scope,fileScope,item);
    }
    void push(const Definition *scope,const FileDef *fileScope,const Definition *item,const QCString &expScope)
    {
      m_elements.emplace_back(scope,fileScope,item,expScope);
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

using VisitedKeys          = StringVector;
using VisitedNamespaceKeys = StringVector;
using VisitedNamespaces    = std::unordered_map<std::string,const Definition *>;

//--------------------------------------------------------------------------------------

struct SymbolResolver::Private
{
  public:
    Private(const FileDef *f) : m_fileScope(f) {}
    void reset()
    {
      m_resolvedTypedefs.clear();
      resolvedType.clear();
      typeDef = nullptr;
      templateSpec.clear();
    }
    void setFileScope(const FileDef *fileScope)
    {
      m_fileScope = fileScope;
    }
    const FileDef *fileScope() const { return m_fileScope; }

    QCString          resolvedType;
    const MemberDef  *typeDef = nullptr;
    QCString          templateSpec;

    const ClassDef *getResolvedTypeRec(
                           VisitedKeys &visitedKeys, // in
                           const Definition *scope,     // in
                           const QCString &n,           // in
                           const MemberDef **pTypeDef,  // out
                           QCString *pTemplSpec,        // out
                           QCString *pResolvedType);    // out

    const Definition *getResolvedSymbolRec(
                           VisitedKeys &visitedKeys, // in
                           const Definition *scope,     // in
                           const QCString &n,           // in
                           const QCString &args,        // in
                           bool checkCV,                // in
                           bool insideCode,             // in
                           bool onlyLinkable,           // in
                           const MemberDef **pTypeDef,  // out
                           QCString *pTemplSpec,        // out
                           QCString *pResolvedType);    // out

    int isAccessibleFrom(  VisitedKeys &visitedKeys, // in
                           AccessStack &accessStack,
                           const Definition *scope,
                           const Definition *item);

    int isAccessibleFromWithExpScope(
                           VisitedKeys &visitedKeys,                     // in
                           VisitedNamespaces &visitedNamespaces,
                           AccessStack       &accessStack,
                           const Definition *scope,
                           const Definition *item,
                           const QCString &explicitScopePart);

  private:
    void getResolvedType(  VisitedKeys &visitedKeys,
                           const Definition *scope,                             // in
                           const Definition *d,                                 // in
                           const QCString &explicitScopePart,                   // in
                           const ArgumentList *actTemplParams,                  // in
                           int &minDistance,                                    // input
                           const ClassDef *&bestMatch,                          // out
                           const MemberDef *&bestTypedef,                       // out
                           QCString &bestTemplSpec,                             // out
                           QCString &bestResolvedType                           // out
                        );

    void getResolvedSymbol(VisitedKeys &visitedKeys,                            // in
                           const Definition *scope,                             // in
                           const Definition *d,                                 // in
                           const QCString &args,                                // in
                           bool  checkCV,                                       // in
                           bool insideCode,                                     // in
                           const QCString &explicitScopePart,                   // in
                           const QCString &strippedTemplateParams,              // in
                           bool forceCallable,                                  // in
                           int &minDistance,                                    // inout
                           const Definition *&bestMatch,                        // out
                           const MemberDef *&bestTypedef,                       // out
                           QCString &bestTemplSpec,                             // out
                           QCString &bestResolvedType                           // out
                          );

    const ClassDef *newResolveTypedef(
                           VisitedKeys &visitedKeys,                            // in
                           const Definition *scope,                             // in
                           const MemberDef *md,                                 // in
                           const MemberDef **pMemType,                          // out
                           QCString *pTemplSpec,                                // out
                           QCString *pResolvedType,                             // out
                           const ArgumentList *actTemplParams = nullptr
                          );

    const Definition *followPath(VisitedKeys &visitedKeys,
                                 const Definition *start,const QCString &path);

    const Definition *endOfPathIsUsedClass(const LinkedRefMap<const Definition> &dl,const QCString &localName);

    bool accessibleViaUsingNamespace(VisitedKeys &visitedKeys,
                                     VisitedNamespaceKeys &visitedNamespaces,
                                     const LinkedRefMap<NamespaceDef> &nl,
                                     const Definition *item,
                                     const QCString &explicitScopePart="",
                                     int level=0);
    bool accessibleViaUsingDefinition(VisitedKeys &visitedKeys,
                                 const LinkedRefMap<const Definition> &dl,
                                 const Definition *item,
                                 const QCString &explicitScopePart=""
                                );
    QCString substTypedef(VisitedKeys &visitedKeys,
                          const Definition *scope,const QCString &name,
                          const MemberDef **pTypeDef=nullptr);

    const FileDef    *m_fileScope;
    std::unordered_map<std::string,const MemberDef*> m_resolvedTypedefs;
};



const ClassDef *SymbolResolver::Private::getResolvedTypeRec(
           VisitedKeys &visitedKeys,
           const Definition *scope,
           const QCString &n,
           const MemberDef **pTypeDef,
           QCString *pTemplSpec,
           QCString *pResolvedType)
{
  AUTO_TRACE("scope={} name={}",scope->name(),n);
  if (n.isEmpty()) return nullptr;
  QCString explicitScopePart;
  QCString strippedTemplateParams;
  QCString scopeName=scope!=Doxygen::globalScope ? scope->name() : QCString();
  QCString name=stripTemplateSpecifiersFromScope(n,TRUE,&strippedTemplateParams,scopeName);
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
    replaceNamespaceAliases(explicitScopePart);
    name=name.mid(qualifierIndex+2);
  }

  if (name.isEmpty())
  {
    AUTO_TRACE_EXIT("empty name");
    return nullptr; // empty name
  }

  auto &range = Doxygen::symbolMap->find(name);
  if (range.empty())
  {
    AUTO_TRACE_EXIT("no symbol with this name");
    return nullptr;
  }

  bool hasUsingStatements =
    (m_fileScope && (!m_fileScope->getUsedNamespaces().empty() ||
                     !m_fileScope->getUsedDefinitions().empty())
    );
  // Since it is often the case that the same name is searched in the same
  // scope over an over again (especially for the linked source code generation)
  // we use a cache to collect previous results. This is possible since the
  // result of a lookup is deterministic. As the key we use the concatenated
  // scope, the name to search for and the explicit scope prefix. The speedup
  // achieved by this simple cache can be enormous.
  size_t scopeNameLen = scope->name().length()+1;
  size_t nameLen = name.length()+1;
  size_t explicitPartLen = explicitScopePart.length();
  size_t fileScopeLen = hasUsingStatements ? 1+m_fileScope->absFilePath().length() : 0;

  // below is a more efficient coding of
  // QCString key=scope->name()+"+"+name+"+"+explicitScopePart+args+typesOnly?'T':'F';
  QCString key(scopeNameLen+nameLen+explicitPartLen+fileScopeLen, QCString::ExplicitSize);
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

  const ClassDef *bestMatch=nullptr;
  {
    if (std::find(visitedKeys.begin(),visitedKeys.end(),key.str())!=std::end(visitedKeys))
    {
      // we are already in the middle of find the definition for this key.
      // avoid recursion
      AUTO_TRACE_EXIT("recursion detected");
      return nullptr;
    }
    // remember the key
    visitedKeys.push_back(key.str());

    LookupInfo *pval = nullptr;
    {
      std::lock_guard lock(g_cacheMutex);
      pval = Doxygen::typeLookupCache->find(key.str());
    }
    AUTO_TRACE_ADD("key={} found={}",key,pval!=nullptr);
    if (pval)
    {
      if (pTemplSpec)    *pTemplSpec=pval->templSpec;
      if (pTypeDef)      *pTypeDef=pval->typeDef;
      if (pResolvedType) *pResolvedType=pval->resolvedType;
      AUTO_TRACE_EXIT("found cached name={} templSpec={} typeDef={} resolvedTypedef={}",
          pval->definition?pval->definition->name():QCString(),
          pval->templSpec,
          pval->typeDef?pval->typeDef->name():QCString(),
          pval->resolvedType);

      return toClassDef(pval->definition);
    }

    const MemberDef *bestTypedef=nullptr;
    QCString bestTemplSpec;
    QCString bestResolvedType;
    int minDistance=10000; // init at "infinite"

    for (Definition *d : range)
    {
      if (isCodeSymbol(d->definitionType()))
      {
        getResolvedType(visitedKeys,scope,d,explicitScopePart,actTemplParams.get(),
            minDistance,bestMatch,bestTypedef,bestTemplSpec,bestResolvedType);
      }
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

    {
      std::lock_guard lock(g_cacheMutex);
      Doxygen::typeLookupCache->insert(key.str(),
                            LookupInfo(bestMatch,bestTypedef,bestTemplSpec,bestResolvedType));
    }
    visitedKeys.erase(std::remove(visitedKeys.begin(), visitedKeys.end(), key.str()), visitedKeys.end());

    AUTO_TRACE_EXIT("found name={} templSpec={} typeDef={} resolvedTypedef={}",
        bestMatch?bestMatch->name():QCString(),
        bestTemplSpec,
        bestTypedef?bestTypedef->name():QCString(),
        bestResolvedType);
  }
  return bestMatch;
}

const Definition *SymbolResolver::Private::getResolvedSymbolRec(
           VisitedKeys &visitedKeys,
           const Definition *scope,
           const QCString &n,
           const QCString &args,
           bool checkCV,
           bool insideCode,
           bool onlyLinkable,
           const MemberDef **pTypeDef,
           QCString *pTemplSpec,
           QCString *pResolvedType)
{
  AUTO_TRACE("scope={} name={} args={} checkCV={} insideCode={}",
      scope->name(),n,args,checkCV,insideCode);
  if (n.isEmpty()) return nullptr;
  QCString explicitScopePart;
  QCString strippedTemplateParams;
  QCString scopeName=scope!=Doxygen::globalScope ? scope->name() : QCString();
  QCString name=stripTemplateSpecifiersFromScope(n,TRUE,&strippedTemplateParams,scopeName);
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
    replaceNamespaceAliases(explicitScopePart);
    name=name.mid(qualifierIndex+2);
  }
  AUTO_TRACE_ADD("qualifierIndex={} name={} explicitScopePart={} strippedTemplateParams={}",
                 qualifierIndex,name,explicitScopePart,strippedTemplateParams);

  if (name.isEmpty())
  {
    AUTO_TRACE_EXIT("empty name qualifierIndex={}",qualifierIndex);
    return nullptr; // empty name
  }

  int i=0;
  const auto &range1 = Doxygen::symbolMap->find(name);
  const auto &range  = (range1.empty() && (i=name.find('<'))!=-1) ? Doxygen::symbolMap->find(name.left(i)) : range1;
  if (range.empty())
  {
    AUTO_TRACE_ADD("no symbols with name '{}' (including unspecialized)",name);
    return nullptr;
  }
  AUTO_TRACE_ADD("{} -> {} candidates",name,range.size());

  bool hasUsingStatements =
    (m_fileScope && (!m_fileScope->getUsedNamespaces().empty() ||
                     !m_fileScope->getUsedDefinitions().empty())
    );
  // Since it is often the case that the same name is searched in the same
  // scope over an over again (especially for the linked source code generation)
  // we use a cache to collect previous results. This is possible since the
  // result of a lookup is deterministic. As the key we use the concatenated
  // scope, the name to search for and the explicit scope prefix. The speedup
  // achieved by this simple cache can be enormous.
  size_t scopeNameLen = scope!=Doxygen::globalScope ? scope->name().length()+1 : 0;
  size_t nameLen = name.length()+1;
  size_t explicitPartLen = explicitScopePart.length();
  size_t strippedTemplateParamsLen = strippedTemplateParams.length();
  size_t fileScopeLen = hasUsingStatements ? 1+m_fileScope->absFilePath().length() : 0;
  size_t argsLen = args.length()+1;

  // below is a more efficient coding of
  // QCString key=scope->name()+"+"+name+"+"+explicitScopePart+args+typesOnly?'T':'F';
  std::string key;
  key.reserve(scopeNameLen+nameLen+explicitPartLen+strippedTemplateParamsLen+fileScopeLen+argsLen);
  if (scope!=Doxygen::globalScope)
  {
    key+=scope->name().str();
    key+='+';
  }
  key+=name.str();
  key+='+';
  key+=explicitScopePart.str();
  key+=strippedTemplateParams.str();

  // if a file scope is given and it contains using statements we should
  // also use the file part in the key (as a class name can be in
  // two different namespaces and a using statement in a file can select
  // one of them).
  if (hasUsingStatements)
  {
    // below is a more efficient coding of
    // key+="+"+m_fileScope->name();
    key+='+';
    key+=m_fileScope->absFilePath().str();
  }
  if (argsLen>0)
  {
    key+='+';
    key+=args.str();
  }

  const Definition *bestMatch=nullptr;
  {
    if (std::find(visitedKeys.begin(),visitedKeys.end(),key)!=std::end(visitedKeys))
    {
      // we are already in the middle of find the definition for this key.
      // avoid recursion
      return nullptr;
    }
    // remember the key
    visitedKeys.push_back(key);
    LookupInfo *pval = nullptr;
    {
      std::lock_guard lock(g_cacheMutex);
      pval = Doxygen::symbolLookupCache->find(key);
    }
    AUTO_TRACE_ADD("key={} found={}",key,pval!=nullptr);
    if (pval)
    {
      if (pTemplSpec)    *pTemplSpec=pval->templSpec;
      if (pTypeDef)      *pTypeDef=pval->typeDef;
      if (pResolvedType) *pResolvedType=pval->resolvedType;
      AUTO_TRACE_EXIT("found cached name={} templSpec={} typeDef={} resolvedTypedef={}",
          pval->definition?pval->definition->name():QCString(),
          pval->templSpec,
          pval->typeDef?pval->typeDef->name():QCString(),
          pval->resolvedType);
      return pval->definition;
    }

    const MemberDef *bestTypedef=nullptr;
    QCString bestTemplSpec;
    QCString bestResolvedType;
    int minDistance=10000; // init at "infinite"

    for (Definition *d : range)
    {
      if (isCodeSymbol(d->definitionType()) &&
          (!onlyLinkable ||
           d->isLinkable() ||
           d->isLinkableInProject() ||
           (d->definitionType()==Definition::TypeFile &&
            (toFileDef(d))->generateSourceFile()
           ) // undocumented file that has source code we can link to
          )
         )
      {
        if (d->definitionType()==Definition::TypeMember)
        {
          const MemberDef *emd = dynamic_cast<const MemberDef *>(d);
          if (emd && emd->isEnumValue() && emd->getEnumScope() && emd->getEnumScope()->isStrong() && explicitScopePart.isEmpty())
          {
            // skip lookup for strong enum values without explicit scope, see issue #11799
            continue;
          }
        }
        getResolvedSymbol(visitedKeys,scope,d,args,checkCV,insideCode,explicitScopePart,strippedTemplateParams,false,
            minDistance,bestMatch,bestTypedef,bestTemplSpec,bestResolvedType);
      }
      if  (minDistance==0) break; // we can stop reaching if we already reached distance 0
    }

    // in case we are looking for e.g. func() and the real function is func(int x) we also
    // accept func(), see example 036 in the test set.
    if (bestMatch==nullptr && args=="()")
    {
      for (Definition *d : range)
      {
        if (isCodeSymbol(d->definitionType()))
        {
          getResolvedSymbol(visitedKeys,scope,d,QCString(),false,insideCode,explicitScopePart,strippedTemplateParams,true,
            minDistance,bestMatch,bestTypedef,bestTemplSpec,bestResolvedType);
        }
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

    {
      LookupInfo lookupInfo(bestMatch,bestTypedef,bestTemplSpec,bestResolvedType);
      std::lock_guard lock(g_cacheMutex);
      // we need to insert the item in the cache again, as it could be removed in the meantime
      Doxygen::symbolLookupCache->insert(key,std::move(lookupInfo));
    }
    visitedKeys.erase(std::remove(visitedKeys.begin(),visitedKeys.end(),key),visitedKeys.end());

    AUTO_TRACE_EXIT("found name={} templSpec={} typeDef={} resolvedTypedef={}",
        bestMatch?bestMatch->name():QCString(),
        bestTemplSpec,
        bestTypedef?bestTypedef->name():QCString(),
        bestResolvedType);
  }
  return bestMatch;
}

void SymbolResolver::Private::getResolvedType(
                         VisitedKeys &visitedKeys,                            // in
                         const Definition *scope,                             // in
                         const Definition *d,                                 // in
                         const QCString &explicitScopePart,                   // in
                         const ArgumentList *actTemplParams,                  // in
                         int &minDistance,                                    // inout
                         const ClassDef *&bestMatch,                          // out
                         const MemberDef *&bestTypedef,                       // out
                         QCString &bestTemplSpec,                             // out
                         QCString &bestResolvedType                           // out
                      )
{
  AUTO_TRACE("scope={} sym={} explicitScope={}",scope->name(),d->qualifiedName(),explicitScopePart);
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
    int distance = isAccessibleFromWithExpScope(visitedKeys,visitedNamespaces,
                                                accessStack,scope,d,explicitScopePart);
    AUTO_TRACE_ADD("distance={}",distance);
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
            AUTO_TRACE_ADD("found symbol={} at distance={} minDistance={}",cd->name(),distance,minDistance);
            minDistance=distance;
            bestMatch = cd;
            bestTypedef = nullptr;
            bestTemplSpec.clear();
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
            AUTO_TRACE_ADD("found symbol={} at distance={} minDistance={}",cd->name(),distance,minDistance);
            minDistance=distance;
            bestMatch = cd;
            bestTypedef = nullptr;
            bestTemplSpec.clear();
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
        AUTO_TRACE_ADD("member={} isTypeDef={}",md->name(),md->isTypedef());
        if (md->isTypedef()) // d is a typedef
        {
          QCString args=md->argsString();
          if (args.isEmpty()) // do not expand "typedef t a[4];"
          {
            // we found a symbol at this distance, but if it didn't
            // resolve to a class, we still have to make sure that
            // something at a greater distance does not match, since
            // that symbol is hidden by this one.
            if (distance<minDistance)
            {
              QCString spec;
              QCString type;
              minDistance=distance;
              const MemberDef *enumType = nullptr;
              const ClassDef *cd = newResolveTypedef(visitedKeys,scope,md,&enumType,&spec,&type,actTemplParams);
              if (cd)  // type resolves to a class
              {
                AUTO_TRACE_ADD("found symbol={} at distance={} minDistance={}",cd->name(),distance,minDistance);
                bestMatch = cd;
                bestTypedef = md;
                bestTemplSpec = spec;
                bestResolvedType = type;
              }
              else if (enumType) // type resolves to a member type
              {
                AUTO_TRACE_ADD("found enum");
                bestMatch = nullptr;
                bestTypedef = enumType;
                bestTemplSpec = "";
                bestResolvedType = enumType->qualifiedName();
              }
              else if (md->isReference()) // external reference
              {
                AUTO_TRACE_ADD("found external reference");
                bestMatch = nullptr;
                bestTypedef = md;
                bestTemplSpec = spec;
                bestResolvedType = type;
              }
              else
              {
                AUTO_TRACE_ADD("no match");
                bestMatch = nullptr;
                bestTypedef = md;
                bestTemplSpec.clear();
                bestResolvedType.clear();
              }
            }
            else
            {
              //printf("      not the best match %d min=%d\n",distance,minDistance);
            }
          }
          else
          {
            AUTO_TRACE_ADD("skipping complex typedef");
          }
        }
        else if (md->isEnumerate())
        {
          if (distance<minDistance)
          {
            AUTO_TRACE_ADD("found enum={} at distance={} minDistance={}",md->name(),distance,minDistance);
            minDistance=distance;
            bestMatch = nullptr;
            bestTypedef = md;
            bestTemplSpec = "";
            bestResolvedType = md->qualifiedName();
          }
        }
      }
    } // if definition accessible
    else
    {
      AUTO_TRACE_ADD("not accessible");
    }
  } // if definition is a class or member
  AUTO_TRACE_EXIT("bestMatch sym={} type={}",
      bestMatch?bestMatch->name():QCString("<none>"),bestResolvedType);
}


void SymbolResolver::Private::getResolvedSymbol(
                         VisitedKeys &visitedKeys,                            // in
                         const Definition *scope,                             // in
                         const Definition *d,                                 // in
                         const QCString &args,                                // in
                         bool  checkCV,                                       // in
                         bool  insideCode,                                    // in
                         const QCString &explicitScopePart,                   // in
                         const QCString &strippedTemplateParams,              // in
                         bool forceCallable,                                  // in
                         int &minDistance,                                    // inout
                         const Definition *&bestMatch,                        // out
                         const MemberDef *&bestTypedef,                       // out
                         QCString &bestTemplSpec,                             // out
                         QCString &bestResolvedType                           // out
                      )
{
  AUTO_TRACE("scope={} sym={}",scope->name(),d->qualifiedName());
  // only look at classes and members that are enums or typedefs
  VisitedNamespaces visitedNamespaces;
  AccessStack accessStack;
  // test accessibility of definition within scope.
  int distance = isAccessibleFromWithExpScope(visitedKeys,visitedNamespaces,accessStack,scope,d,explicitScopePart+strippedTemplateParams);
  if (distance==-1 && !strippedTemplateParams.isEmpty())
  {
    distance = isAccessibleFromWithExpScope(visitedKeys,visitedNamespaces,accessStack,scope,d,explicitScopePart);
  }
  AUTO_TRACE_ADD("distance={}",distance);
  if (distance!=-1) // definition is accessible
  {
    // see if we are dealing with a class or a typedef
    if (args.isEmpty() && !forceCallable && d->definitionType()==Definition::TypeClass) // d is a class
    {
      const ClassDef *cd = toClassDef(d);
      if (!cd->isTemplateArgument()) // skip classes that
        // are only there to
        // represent a template
        // argument
      {
        if (distance<minDistance) // found a definition that is "closer"
        {
          AUTO_TRACE_ADD("found symbol={} at distance={} minDistance={}",d->name(),distance,minDistance);
          minDistance=distance;
          bestMatch = d;
          bestTypedef = nullptr;
          bestTemplSpec.clear();
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
          AUTO_TRACE_ADD("found symbol={} at distance={} minDistance={}",d->name(),distance,minDistance);
          minDistance=distance;
          bestMatch = d;
          bestTypedef = nullptr;
          bestTemplSpec.clear();
          bestResolvedType = cd->qualifiedName();
        }
      }
      else
      {
        AUTO_TRACE_ADD("class with template arguments");
      }
    }
    else if (d->definitionType()==Definition::TypeMember)
    {
      const MemberDef *md = toMemberDef(d);

      bool match = true;
      AUTO_TRACE_ADD("member={} args={} isCallable()={}",md->name(),argListToString(md->argumentList()),md->isCallable());
      if (md->isCallable() && !args.isEmpty())
      {
        QCString actArgs;
        if (md->isArtificial() && md->formalTemplateArguments()) // for members of an instantiated template we need to replace
                                                                 // the formal arguments by the actual ones before matching
                                                                 // See issue #10640
        {
          actArgs = substituteTemplateArgumentsInString(args,md->formalTemplateArguments().value(),&md->argumentList());
        }
        else
        {
          actArgs = args;
        }
        std::unique_ptr<ArgumentList> argList = stringToArgumentList(md->getLanguage(),actArgs);
        const ArgumentList &mdAl = md->argumentList();
        match = matchArguments2(md->getOuterScope(),md->getFileDef(),md->typeString(),&mdAl,
                                scope,              md->getFileDef(),md->typeString(),argList.get(),
                                checkCV,md->getLanguage());
        AUTO_TRACE_ADD("match={}",match);
      }

      if (match && distance<minDistance)
      {
        AUTO_TRACE_ADD("found symbol={} at distance={} minDistance={}",md->name(),distance,minDistance);
        minDistance=distance;
        bestMatch = md;
        bestTypedef = md;
        bestTemplSpec = "";
        bestResolvedType = md->qualifiedName();
      }
    }
    else if ((d->definitionType()==Definition::TypeNamespace ||
              d->definitionType()==Definition::TypeFile ||
              d->definitionType()==Definition::TypeModule))
    {
      if (distance<minDistance) // found a definition that is "closer"
      {
        AUTO_TRACE_ADD("found symbol={} at distance={} minDistance={}",d->name(),distance,minDistance);
        minDistance=distance;
        bestMatch = d;
        bestTypedef = nullptr;
        bestTemplSpec.clear();
        bestResolvedType.clear();
      }
    }
  } // if definition accessible
  else
  {
    AUTO_TRACE_ADD("not accessible");
  }
  AUTO_TRACE_EXIT("bestMatch sym={} distance={}",
      bestMatch?bestMatch->name():QCString("<none>"),bestResolvedType);
}


const ClassDef *SymbolResolver::Private::newResolveTypedef(
                  VisitedKeys &visitedKeys,                            // in
                  const Definition * /* scope */,                      // in
                  const MemberDef *md,                                 // in
                  const MemberDef **pMemType,                          // out
                  QCString *pTemplSpec,                                // out
                  QCString *pResolvedType,                             // out
                  const ArgumentList *actTemplParams)                  // in
{
  AUTO_TRACE("md={}",md->qualifiedName());
  std::lock_guard<std::recursive_mutex> lock(g_cacheTypedefMutex);
  bool isCached = md->isTypedefValCached(); // value already cached
  if (isCached)
  {
    AUTO_TRACE_EXIT("cached typedef={} resolvedTypedef={} templSpec={}",
        md->getCachedTypedefVal() ? md->getCachedTypedefVal()->name() : QCString(),
        md->getCachedResolvedTypedef(),
        md->getCachedTypedefTemplSpec());

    if (pTemplSpec)    *pTemplSpec    = md->getCachedTypedefTemplSpec();
    if (pResolvedType) *pResolvedType = md->getCachedResolvedTypedef();
    return md->getCachedTypedefVal();
  }

  QCString qname = md->qualifiedName();
  if (m_resolvedTypedefs.find(qname.str())!=m_resolvedTypedefs.end())
  {
    AUTO_TRACE_EXIT("already being processed");
    return nullptr; // typedef already done
  }

  auto typedef_it = m_resolvedTypedefs.emplace(qname.str(),md).first; // put on the trace list

  const ClassDef *typeClass = md->getClassDef();
  QCString type = md->typeString(); // get the "value" of the typedef
  if (typeClass && typeClass->isTemplate() &&
      actTemplParams && !actTemplParams->empty())
  {
    type = substituteTemplateArgumentsInString(type,
            typeClass->templateArguments(),actTemplParams);
  }
  QCString typedefValue = type;
  int tl=static_cast<int>(type.length());
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
  tl=static_cast<int>(type.length()); // length may have been changed
  while (sp<tl && type.at(sp)==' ') sp++;
  const MemberDef *memTypeDef = nullptr;
  const ClassDef *result = getResolvedTypeRec(visitedKeys,md->getOuterScope(),type,
                                                &memTypeDef,nullptr,pResolvedType);
  // if type is a typedef then return what it resolves to.
  if (memTypeDef && memTypeDef->isTypedef())
  {
    AUTO_TRACE_ADD("resolving typedef");
    result=newResolveTypedef(visitedKeys,m_fileScope,memTypeDef,pMemType,pTemplSpec,nullptr);
    goto done;
  }
  else if (memTypeDef && memTypeDef->isEnumerate() && pMemType)
  {
    *pMemType = memTypeDef;
  }

  if (result==nullptr)
  {
    // try unspecialized version if type is template
    int si=type.findRev("::");
    int i=type.find('<');
    if (si==-1 && i!=-1) // typedef of a template => try the unspecialized version
    {
      if (pTemplSpec) *pTemplSpec = type.mid(i);
      result = getResolvedTypeRec(visitedKeys,md->getOuterScope(),type.left(i),nullptr,nullptr,pResolvedType);
    }
    else if (si!=-1) // A::B
    {
      i=type.find('<',si);
      if (i==-1) // Something like A<T>::B => lookup A::B
      {
        i=static_cast<int>(type.length());
      }
      else // Something like A<T>::B<S> => lookup A::B, spec=<S>
      {
        if (pTemplSpec) *pTemplSpec = type.mid(i);
      }
      result = getResolvedTypeRec(visitedKeys,md->getOuterScope(),
           stripTemplateSpecifiersFromScope(type.left(i),FALSE),nullptr,nullptr,pResolvedType);
    }
  }

done:
  if (pResolvedType)
  {
    if (result && result->definitionType()==Definition::TypeClass)
    {
      *pResolvedType = result->qualifiedName();
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
    AUTO_TRACE_ADD("caching typedef relation {}->{}",md->name(),result->name());
    MemberDefMutable *mdm = toMemberDefMutable(const_cast<MemberDef*>(md));
    if (mdm)
    {
      mdm->cacheTypedefVal(result,
        pTemplSpec ? *pTemplSpec : QCString(),
        pResolvedType ? *pResolvedType : QCString()
       );
    }
  }

  m_resolvedTypedefs.erase(typedef_it); // remove from the trace list

  AUTO_TRACE_EXIT("result={} pTemplSpec={} pResolvedType={}",
      result        ? result->name() : QCString(),
      pTemplSpec    ? *pTemplSpec    : "<nullptr>",
      pResolvedType ? *pResolvedType : "<nullptr>"
      );
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
                                     VisitedKeys &visitedKeys,
                                     VisitedNamespaces &visitedNamespaces,
                                     AccessStack       &accessStack,
                                     const Definition *scope,
                                     const Definition *item,
                                     const QCString &explicitScopePart)
{
  int result=0; // assume we found it
  AUTO_TRACE("scope={} item={} explictScopePart={}",
      scope?scope->name():QCString(), item?item->name():QCString(), explicitScopePart);
  if (explicitScopePart.isEmpty())
  {
    // handle degenerate case where there is no explicit scope.
    result = isAccessibleFrom(visitedKeys,accessStack,scope,item);
    AUTO_TRACE_EXIT("result={}",result);
    return result;
  }

  if (accessStack.find(scope,m_fileScope,item,explicitScopePart))
  {
    AUTO_TRACE_EXIT("already found");
    return -1;
  }
  accessStack.push(scope,m_fileScope,item,explicitScopePart);

  const Definition *newScope = followPath(visitedKeys,scope,explicitScopePart);
  if (newScope)  // explicitScope is inside scope => newScope is the result
  {
    Definition *itemScope = item->getOuterScope();

    AUTO_TRACE_ADD("scope traversal successful newScope={}",newScope->name());

    bool nestedClassInsideBaseClass =
         itemScope &&
         itemScope->definitionType()==Definition::TypeClass &&
         newScope->definitionType()==Definition::TypeClass &&
         (toClassDef(newScope))->isBaseClass(toClassDef(itemScope),TRUE);

    bool enumValueWithinEnum =
         item->definitionType()==Definition::TypeMember &&
         toMemberDef(item)->isEnumValue() &&
         toMemberDef(item)->getEnumScope()==newScope;

    if (itemScope==newScope)  // exact match of scopes => distance==0
    {
      AUTO_TRACE_ADD("found scope match");
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

      AUTO_TRACE_ADD("{} is a bass class of {}",scope->name(),newScope->name());
    }
    else if (enumValueWithinEnum)
    {
      AUTO_TRACE_ADD("found enum value inside enum");
      result=1;
    }
    else
    {
      int i=-1;
      if (newScope->definitionType()==Definition::TypeNamespace)
      {
        visitedNamespaces.emplace(newScope->name().str(),newScope);
        // this part deals with the case where item is a class
        // A::B::C but is explicit referenced as A::C, where B is imported
        // in A via a using directive.
        //printf("newScope is a namespace: %s!\n",qPrint(newScope->name()));
        const NamespaceDef *nscope = toNamespaceDef(newScope);
        for (const auto &ud : nscope->getUsedDefinitions())
        {
          if (ud==item)
          {
            AUTO_TRACE_ADD("found in used definition {}",ud->name());
            goto done;
          }
        }
        for (const auto &nd : nscope->getUsedNamespaces())
        {
          if (visitedNamespaces.find(nd->name().str())==visitedNamespaces.end())
          {
            i = isAccessibleFromWithExpScope(visitedKeys,visitedNamespaces,accessStack,scope,item,nd->name());
            if (i!=-1)
            {
              AUTO_TRACE_ADD("found in used namespace {}",nd->name());
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
        i = isAccessibleFromWithExpScope(visitedKeys,visitedNamespaces,accessStack,scope->getOuterScope(),item,explicitScopePart);
      }
      result = (i==-1) ? -1 : i+2;
    }
  }
  else // failed to resolve explicitScope
  {
    AUTO_TRACE_ADD("failed to resolve explicitScope");
    if (scope->definitionType()==Definition::TypeNamespace)
    {
      const NamespaceDef *nscope = toNamespaceDef(scope);
      VisitedNamespaceKeys locVisitedNamespaceKeys;
      if (accessibleViaUsingNamespace(visitedKeys,locVisitedNamespaceKeys,nscope->getUsedNamespaces(),item,explicitScopePart))
      {
        AUTO_TRACE_ADD("found in used class");
        goto done;
      }
    }
    if (scope==Doxygen::globalScope)
    {
      if (m_fileScope)
      {
        VisitedNamespaceKeys locVisitedNamespaceKeys;
        if (accessibleViaUsingNamespace(visitedKeys,locVisitedNamespaceKeys,m_fileScope->getUsedNamespaces(),item,explicitScopePart))
        {
          AUTO_TRACE_ADD("found in used namespace");
          goto done;
        }
      }
      AUTO_TRACE_ADD("not found in this scope");
      result=-1;
    }
    else // continue by looking into the parent scope
    {
      int i=isAccessibleFromWithExpScope(visitedKeys,visitedNamespaces,accessStack,scope->getOuterScope(),item,explicitScopePart);
      result= (i==-1) ? -1 : i+2;
    }
  }

done:
  AUTO_TRACE_EXIT("result={}",result);
  accessStack.pop();
  return result;
}

const Definition *SymbolResolver::Private::followPath(VisitedKeys &visitedKeys,
                                                      const Definition *start,const QCString &path)
{
  AUTO_TRACE("start={},path={}",start?start->name():QCString(), path);
  int is=0,ps=0,l=0;

  const Definition *current=start;
  // for each part of the explicit scope
  while ((is=getScopeFragment(path,ps,&l))!=-1)
  {
    // try to resolve the part if it is a typedef
    const MemberDef *memTypeDef=nullptr;
    QCString qualScopePart = substTypedef(visitedKeys,current,path.mid(is,l),&memTypeDef);
    AUTO_TRACE_ADD("qualScopePart={} memTypeDef={}",qualScopePart,memTypeDef?memTypeDef->name():"");
    const Definition *next = nullptr;
    if (memTypeDef)
    {
      const ClassDef *type = newResolveTypedef(visitedKeys,m_fileScope,memTypeDef,nullptr,nullptr,nullptr);
      if (type)
      {
        AUTO_TRACE_EXIT("type={}",type->name());
        return type;
      }
    }
    else if (m_fileScope)
    {
      next = endOfPathIsUsedClass(m_fileScope->getUsedDefinitions(),qualScopePart);
    }
    if (next==nullptr)
    {
      next = current->findInnerCompound(qualScopePart);
    }
    AUTO_TRACE_ADD("Looking for {} inside {} result={}",
        qualScopePart, current->name(), next?next->name():QCString());
    if (next==nullptr)
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
    else if (current!=Doxygen::globalScope && current->definitionType()==Definition::TypeNamespace)
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
    if (next==nullptr) // failed to follow the path
    {
      if (current->definitionType()==Definition::TypeNamespace)
      {
        next = endOfPathIsUsedClass(
            (toNamespaceDef(current))->getUsedDefinitions(),qualScopePart);
      }
      else if (current->definitionType()==Definition::TypeFile)
      {
        next = endOfPathIsUsedClass(
            (toFileDef(current))->getUsedDefinitions(),qualScopePart);
      }
      current = next;
      if (current==nullptr) break;
    }
    else // continue to follow scope
    {
      current = next;
      AUTO_TRACE_ADD("current={}",current->name());
    }
    ps=is+l;
  }

  AUTO_TRACE_EXIT("result={}",current?current->name():QCString());
  return current; // path could be followed
}

const Definition *SymbolResolver::Private::endOfPathIsUsedClass(const LinkedRefMap<const Definition> &dl,const QCString &localName)
{
  for (const auto &d : dl)
  {
    if (d->localName()==localName)
    {
      return d;
    }
  }
  return nullptr;
}

bool SymbolResolver::Private::accessibleViaUsingNamespace(
                                 VisitedKeys &visitedKeys,
                                 VisitedNamespaceKeys &visitedNamespaces,
                                 const LinkedRefMap<NamespaceDef> &nl,
                                 const Definition *item,
                                 const QCString &explicitScopePart,
                                 int level)
{
  AUTO_TRACE("item={} explicitScopePart={} level={}",item?item->name():QCString(), explicitScopePart, level);
  for (const auto &und : nl) // check used namespaces for the class
  {
    AUTO_TRACE_ADD("trying via used namespace '{}'",und->name());
    const Definition *sc = explicitScopePart.isEmpty() ? und : followPath(visitedKeys,und,explicitScopePart);
    if (sc && item->getOuterScope()==sc)
    {
      AUTO_TRACE_EXIT("true");
      return true;
    }
    if (item->getLanguage()==SrcLangExt::Cpp)
    {
      QCString key=und->qualifiedName();
      if (!und->getUsedNamespaces().empty() && std::find(visitedNamespaces.begin(),visitedNamespaces.end(),key.str())==std::end(visitedNamespaces))
      {
        visitedNamespaces.push_back(key.str());
        if (accessibleViaUsingNamespace(visitedKeys,visitedNamespaces,und->getUsedNamespaces(),item,explicitScopePart,level+1))
        {
          AUTO_TRACE_EXIT("true");
          return true;
        }

      }
    }
  }
  AUTO_TRACE_EXIT("false");
  return false;
}


bool SymbolResolver::Private::accessibleViaUsingDefinition(VisitedKeys &visitedKeys,
                                                      const LinkedRefMap<const Definition> &dl,
                                                      const Definition *item,
                                                      const QCString &explicitScopePart)
{
  AUTO_TRACE("item={} explicitScopePart={}",item?item->name():QCString(), explicitScopePart);
  for (const auto &ud : dl)
  {
    AUTO_TRACE_ADD("trying via used definition '{}'",ud->name());
    const Definition *sc = explicitScopePart.isEmpty() ? ud : followPath(visitedKeys,ud,explicitScopePart);
    if (sc && sc==item)
    {
      AUTO_TRACE_EXIT("true");
      return true;
    }
  }
  AUTO_TRACE_EXIT("false");
  return false;
}

int SymbolResolver::Private::isAccessibleFrom(VisitedKeys &visitedKeys,
                                              AccessStack &accessStack,
                                              const Definition *scope,
                                              const Definition *item)
{
  AUTO_TRACE("scope={} item={} item.definitionType={}",
      scope?scope->name():QCString(), item?item->name():QCString(),
      item?(int)item->definitionType():-1);

  if (accessStack.find(scope,m_fileScope,item))
  {
    AUTO_TRACE_EXIT("already processed!");
    return -1;
  }
  accessStack.push(scope,m_fileScope,item);

  int result=0; // assume we found it
  int i=0;

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
    AUTO_TRACE_ADD("adjusting scope to {}",itemScope?itemScope->name():QCString());
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
    AUTO_TRACE_ADD("memberAccessibleFromScope={} nestedClassInsideBaseClass={} enumValueOfStrongEnum={}",
        memberAccessibleFromScope, nestedClassInsideBaseClass, enumValueOfStrongEnum);
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
      AUTO_TRACE_ADD("found in anonymous namespace");
      result++;
      goto done;
    }
    if (m_fileScope)
    {
      if (accessibleViaUsingDefinition(visitedKeys,m_fileScope->getUsedDefinitions(),item))
      {
        AUTO_TRACE_ADD("found via used class");
        goto done;
      }
      VisitedNamespaceKeys visitedNamespaceKeys;
      if (accessibleViaUsingNamespace(visitedKeys,visitedNamespaceKeys,m_fileScope->getUsedNamespaces(),item))
      {
        AUTO_TRACE_ADD("found via used namespace");
        goto done;
      }
    }
    AUTO_TRACE_ADD("reached global scope");
    result=-1; // not found in path to globalScope
  }
  else // keep searching
  {
    // check if scope is a namespace, which is using other classes and namespaces
    if (scope->definitionType()==Definition::TypeNamespace)
    {
      const NamespaceDef *nscope = toNamespaceDef(scope);
      if (accessibleViaUsingDefinition(visitedKeys,nscope->getUsedDefinitions(),item))
      {
        AUTO_TRACE_ADD("found via used class");
        goto done;
      }
      VisitedNamespaceKeys visitedNamespaceKeys;
      if (accessibleViaUsingNamespace(visitedKeys,visitedNamespaceKeys,nscope->getUsedNamespaces(),item,QCString()))
      {
        AUTO_TRACE_ADD("found via used namespace");
        goto done;
      }
    }
    else if (scope->definitionType()==Definition::TypeFile)
    {
      const FileDef *nfile = toFileDef(scope);
      if (accessibleViaUsingDefinition(visitedKeys,nfile->getUsedDefinitions(),item))
      {
        AUTO_TRACE_ADD("found via used class");
        goto done;
      }
      VisitedNamespaceKeys visitedNamespaceKeys;
      if (accessibleViaUsingNamespace(visitedKeys,visitedNamespaceKeys,nfile->getUsedNamespaces(),item,QCString()))
      {
        AUTO_TRACE_ADD("found via used namespace");
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
    i=isAccessibleFrom(visitedKeys,accessStack,parentScope,item);
    result= (i==-1) ? -1 : i+2;
  }
done:
  AUTO_TRACE_EXIT("result={}",result);
  accessStack.pop();
  return result;
}

QCString SymbolResolver::Private::substTypedef(
                          VisitedKeys &visitedKeys,
                          const Definition *scope,const QCString &name,
                          const MemberDef **pTypeDef)
{
  AUTO_TRACE("scope={} name={}",scope?scope->name():QCString(), name);
  QCString result=name;
  if (name.isEmpty()) return result;

  auto &range = Doxygen::symbolMap->find(name);
  if (range.empty())
    return result; // no matches

  MemberDef *bestMatch=nullptr;
  int minDistance=10000; // init at "infinite"

  std::string key;
  const int maxAddrSize = 20;
  char ptr_str[maxAddrSize];
  int num = qsnprintf(ptr_str,maxAddrSize,"%p:",(void *)scope);
  assert(num>0);
  key.reserve(num+name.length()+1);
  key+=ptr_str;
  key+=name.str();
  {
    std::lock_guard lock(g_substMapMutex);
    auto it = g_substMap.find(key);
    if (it!=g_substMap.end())
    {
      if (pTypeDef) *pTypeDef = it->second.second;
      return it->second.first;
    }
  }

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
        int distance = isAccessibleFromWithExpScope(visitedKeys,visitedNamespaces,accessStack,scope,d,"");
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

  // cache the result of the computation to give a faster answers next time, especially relevant
  // if `range` has many arguments (i.e. there are many symbols with the same name in different contexts)
  {
    std::lock_guard lock(g_substMapMutex);
    g_substMap.emplace(key,std::make_pair(result,bestMatch));
  }

  AUTO_TRACE_EXIT("result={}",result);
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
  AUTO_TRACE("scope={} name={} mayBeUnlinkable={} mayBeHidden={}",
      scope?scope->name():QCString(), name, mayBeUnlinkable, mayBeHidden);
  p->reset();

  auto lang = scope ? scope->getLanguage() :
          p->fileScope() ? p->fileScope()->getLanguage() :
          SrcLangExt::Cpp;  // fallback to C++

  if (scope==nullptr ||
      (scope->definitionType()!=Definition::TypeClass &&
       scope->definitionType()!=Definition::TypeNamespace
      ) ||
      (name.stripWhiteSpace().startsWith("::")) ||
      ((lang==SrcLangExt::Java || lang==SrcLangExt::CSharp) && QCString(name).find("::")!=-1)
     )
  {
    scope=Doxygen::globalScope;
  }
  const ClassDef *result=nullptr;
  if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
  {
    result = getClass(name);
  }
  else
  {
    VisitedKeys visitedKeys;
    QCString lookupName = lang==SrcLangExt::CSharp ? mangleCSharpGenericName(name) : name;
    AUTO_TRACE_ADD("lookup={}",lookupName);
    result = p->getResolvedTypeRec(visitedKeys,scope,lookupName,&p->typeDef,&p->templateSpec,&p->resolvedType);
    if (result==nullptr) // for nested classes imported via tag files, the scope may not
                   // present, so we check the class name directly as well.
                   // See also bug701314
    {
      result = getClass(lookupName);
    }
  }
  if (!mayBeUnlinkable && result && !result->isLinkable())
  {
    if (!mayBeHidden || !result->isHidden())
    {
      AUTO_TRACE_ADD("hiding symbol {}",result->name());
      result=nullptr; // don't link to artificial/hidden classes unless explicitly allowed
    }
  }
  AUTO_TRACE_EXIT("result={}",result?result->name():QCString());
  return result;
}

const Definition *SymbolResolver::resolveSymbol(const Definition *scope,
                                                const QCString &name,
                                                const QCString &args,
                                                bool checkCV,
                                                bool insideCode,
                                                bool onlyLinkable)
{
  AUTO_TRACE("scope={} name={} args={} checkCV={} insideCode={}",
             scope?scope->name():QCString(), name, args, checkCV, insideCode);
  p->reset();
  if (scope==nullptr) scope=Doxygen::globalScope;
  VisitedKeys visitedKeys;
  const Definition *result = p->getResolvedSymbolRec(visitedKeys,scope,name,args,checkCV,insideCode,onlyLinkable,&p->typeDef,&p->templateSpec,&p->resolvedType);
  AUTO_TRACE_EXIT("result={}{}", qPrint(result?result->qualifiedName():QCString()),
                                 qPrint(result && result->definitionType()==Definition::TypeMember ? toMemberDef(result)->argsString() : QCString()));
  return result;
}

int SymbolResolver::isAccessibleFrom(const Definition *scope,const Definition *item)
{
  AUTO_TRACE("scope={} item={}",
      scope?scope->name():QCString(), item?item->name():QCString());
  p->reset();
  VisitedKeys visitedKeys;
  AccessStack accessStack;
  int result = p->isAccessibleFrom(visitedKeys,accessStack,scope,item);
  AUTO_TRACE_EXIT("result={}",result);
  return result;
}

int SymbolResolver::isAccessibleFromWithExpScope(const Definition *scope,const Definition *item,
                                                 const QCString &explicitScopePart)
{
  AUTO_TRACE("scope={} item={} explicitScopePart={}",
      scope?scope->name():QCString(), item?item->name():QCString(), explicitScopePart);
  p->reset();
  VisitedKeys visitedKeys;
  VisitedNamespaces visitedNamespaces;
  AccessStack accessStack;
  int result = p->isAccessibleFromWithExpScope(visitedKeys,visitedNamespaces,accessStack,scope,item,explicitScopePart);
  AUTO_TRACE_EXIT("result={}",result);
  return result;
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

