/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
 * ----------------------------------------------------------------------------
 *
 * Slovak translation by Stanislav Kudlac (skudlac@pobox.sk)
 *
 * ----------------------------------------------------------------------------
 */

#ifndef TRANSLATOR_SK_H
#define TRANSLATOR_SK_H

class TranslatorSlovak : public TranslatorAdapter_1_2_18
{
  private:
	/*! The Decode() inline assumes the source written in the
		Windows encoding (maintainer only dependent).
	 */
	inline QCString Decode(const QCString & sInput)
	{
#ifdef _WIN32
	  return sInput;
#else
	  return Win1250ToISO88592(sInput);
#endif
	}

  public:
	// --- Language control methods -------------------

	virtual QCString idLanguage()
	{ return "slovak"; }

	virtual QCString latexLanguageSupportCommand()
	{ return "\\usepackage{slovak}\n"; }

	/*! return the language charset. This will be used for the HTML output */
	virtual QCString idLanguageCharset()
	{
#ifdef _WIN32
		return "windows-1250";
#else
		return "iso-8859-2";
#endif
	}

	// --- Language translation methods -------------------

	/*! used in the compound documentation before a list of related functions. */
	virtual QCString trRelatedFunctions()
	{ return Decode("Súvisiace funkcie"); }

	/*! subscript for the related functions. */
	virtual QCString trRelatedSubscript()
	{ return Decode("(Uvedené funkcie niesú èlenskımi funkciami.)"); }

	/*! header that is put before the detailed description of files, classes and namespaces. */
	virtual QCString trDetailedDescription()
	{ return Decode("Detailnı popis"); }

	/*! header that is put before the list of typedefs. */
	virtual QCString trMemberTypedefDocumentation()
	{ return Decode("Dokumentácia k èlenskım typom"); }

	/*! header that is put before the list of enumerations. */
	virtual QCString trMemberEnumerationDocumentation()
	{ return Decode("Dokumentácia k èlenskım enumeráciám"); }

	/*! header that is put before the list of member functions. */
	virtual QCString trMemberFunctionDocumentation()
	{ return Decode("Dokumentácia k metódam"); }

	/*! header that is put before the list of member attributes. */
	virtual QCString trMemberDataDocumentation()
	{
	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		return Decode("Dokumentácia k polokám");
	  }
	  else
	  {
		return Decode("Dokumentácia k dátovım èlenom");
	  }
	}

	/*! this is the text of a link put after brief descriptions. */
	virtual QCString trMore()
	{ return Decode("..."); }

	/*! put in the class documentation */
	virtual QCString trListOfAllMembers()
	{ return Decode("Zoznam všetkıch èlenov."); }

	/*! used as the title of the "list of all members" page of a class */
	virtual QCString trMemberList()
	{ return Decode("Zoznam èlenov triedy"); }

	/*! this is the first part of a sentence that is followed by a class name */
	virtual QCString trThisIsTheListOfAllMembers()
	{ return Decode("Tu nájdete úplnı zoznam èlenov triedy "); }

	/*! this is the remainder of the sentence after the class name */
	virtual QCString trIncludingInheritedMembers()
	{ return Decode(", vrátane všetkıch zdedenıch èlenov."); }

	/*! this is put at the author sections at the bottom of man pages.
	 *	parameter s is name of the project name.
	 */
	virtual QCString trGeneratedAutomatically(const char *s)
	{ QCString result="Generované automaticky programom Doxygen "
					  "zo zdrojovıch textov";
	  if (s) result+=(QCString)" projektu "+s;
	  result+=".";
	  return Decode(result);
	}

	/*! put after an enum name in the list of all members */
	virtual QCString trEnumName()
	{ return Decode("meno enumerácie"); }

	/*! put after an enum value in the list of all members */
	virtual QCString trEnumValue()
	{ return Decode("hodnota enumerácie"); }

	/*! put after an undocumented member in the list of all members */
	virtual QCString trDefinedIn()
	{ return Decode("definovanı v"); }

	// quick reference sections

	/*! This is put above each page as a link to the list of all groups of
	 *	compounds or files (see the \\group command).
	 */
	virtual QCString trModules()
	{ return Decode("Moduly"); }

	/*! This is put above each page as a link to the class hierarchy */
	virtual QCString trClassHierarchy()
	{ return Decode("Hierarchia tried"); }

	/*! This is put above each page as a link to the list of annotated classes */
	virtual QCString trCompoundList()
	{
	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		return Decode("Dátové štruktúry");
	  }
	  else
	  {
		return Decode("Zoznam tried");
	  }
	}

	/*! This is put above each page as a link to the list of documented files */
	virtual QCString trFileList()
	{ return Decode("Zoznam súborov"); }

	/*! This is put above each page as a link to the list of all verbatim headers */
	virtual QCString trHeaderFiles()
	{ return Decode("Zoznam hlavièkovıch súborov"); }

	/*! This is put above each page as a link to all members of compounds. */
	virtual QCString trCompoundMembers()
	{
	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		return Decode("Dátové poloky");
	  }
	  else
	  {
		return Decode("Zoznam èlenov tried");
	  }
	}

	/*! This is put above each page as a link to all members of files. */
	virtual QCString trFileMembers()
	{
	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		return Decode("Globálne symboly");
	  }
	  else
	  {
		return Decode("Symboly v súboroch");
	  }
	}

	/*! This is put above each page as a link to all related pages. */
	virtual QCString trRelatedPages()
	{ return Decode("Ostatné stránky"); }

	/*! This is put above each page as a link to all examples. */
	virtual QCString trExamples()
	{ return Decode("Príklady"); }

	/*! This is put above each page as a link to the search engine. */
	virtual QCString trSearch()
	{ return Decode("H¾ada"); }

	/*! This is an introduction to the class hierarchy. */
	virtual QCString trClassHierarchyDescription()
	{ return Decode("Tu nájdete zoznam, vyjadrujúci vzah dediènosti tried. "
			 "Je zoradenı pribline (ale nie úplne) pod¾a abecedy:");
	}

	/*! This is an introduction to the list with all files. */
	virtual QCString trFileListDescription(bool extractAll)
	{
	  QCString result="Tu nájdete zoznam všetkıch ";
	  if (!extractAll) result+="dokumentovanıch ";
	  result+="súborov so struènımi popismi:";
	  return Decode(result);
	}

	/*! This is an introduction to the annotated compound list. */
	virtual QCString trCompoundListDescription()
	{
	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		return Decode("Nasledujúci zoznam obsahuje identifikáciu dátovıch "
					  "štruktúr a ich struèné popisy:");
	  }
	  else
	  {
		return Decode("Nasledujúci zoznam obsahuje predovšetkım identifikáciu "
					  "tried, ale nachádzajú sa tu i ïalšie netriviálne prvky, "
					  "ako sú štruktúry (struct), uniony (union) a rozhrania "
					  "(interface). V zozname sú uvedené ich struèné "
					  "popisy:");
	  }
	}

	/*! This is an introduction to the page with all class members. */
	virtual QCString trCompoundMembersDescription(bool extractAll)
	{
	  QCString result= "Tu nájdete zoznam všetkıch ";
	  if (!extractAll)
	  {
		result += "dokumentovanıch ";
	  }

	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		result += "poloiek štruktúr (struct) a unionov (union) ";
	  }
	  else
	  {
		result += "èlenov tried ";
	  }

	  result += "s odkazmi na ";

	  if (!extractAll)
	  {
		if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
		{
		  result += "dokumentáciu štruktúr/unionov, ku ktorım prislúchajú:";
		}
		else
		{
		  result += "dokumentáciu tried, ku ktorım prislúchajú:";
		}
	  }
	  else
	  {
		if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
		{
		  result+="štruktúry/uniony, ku ktorım prislúchajú:";
		}
		else
		{
		  result+="triedy, ku ktorım prislúchajú:";
		}
	  }

	  return Decode(result);
	}

	/*! This is an introduction to the page with all file members. */
	virtual QCString trFileMembersDescription(bool extractAll)
	{
	  QCString result="Tu nájdete zoznam všetkıch ";
	  if (!extractAll) result+="dokumentovanıch ";

	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		result+="funkcií, premennıch, makier, enumerácií a definícií typov (typedef) "
				"s odkazmi na ";
	  }
	  else
	  {
		result+="symbolov, ktoré sú definované na úrovni svojich súborov. "
				"Pre kadı symbol je uvedenı odkaz na ";
	  }

	  if (extractAll)
		result+="súbory, ku ktorım prislúchajú:";
	  else
		result+="dokumentáciu:";

	  return Decode(result);
	}

	/*! This is an introduction to the page with the list of all header files. */
	virtual QCString trHeaderFilesDescription()
	{ return Decode("Tu nájdete hlavièkové súbory, ktoré tvoria "
			 "aplikaèné programové rozhranie (API):"); }

	/*! This is an introduction to the page with the list of all examples */
	virtual QCString trExamplesDescription()
	{ return Decode("Tu nájdete zoznam všetkıch príkladov:"); }

	/*! This is an introduction to the page with the list of related pages */
	virtual QCString trRelatedPagesDescription()
	{ return Decode("Nasledujúci zoznam odkazuje na ïalšie stránky projektu, "
					"ktoré majú charakter usporiadanıch zoznamov informácií, "
					"pozbieranıch z rôznych miest v zdrojovıch súboroch:"); }

	/*! This is an introduction to the page with the list of class/file groups */
	virtual QCString trModulesDescription()
	{ return Decode("Tu nájdete zoznam všetkıch modulov:"); }

	/*! This sentences is used in the annotated class/file lists if no brief
	 * description is given.
	 */
	virtual QCString trNoDescriptionAvailable()
	{ return Decode("Popis nieje k dispozícii"); }

	// index titles (the project name is prepended for these)


	/*! This is used in HTML as the title of index.html. */
	virtual QCString trDocumentation()
	{ return Decode("Dokumentácia"); }

	/*! This is used in LaTeX as the title of the chapter with the
	 * index of all groups.
	 */
	virtual QCString trModuleIndex()
	{ return Decode("Register modulov"); }

	/*! This is used in LaTeX as the title of the chapter with the
	 * class hierarchy.
	 */
	virtual QCString trHierarchicalIndex()
	{ return Decode("Register hierarchie tried"); }

	/*! This is used in LaTeX as the title of the chapter with the
	 * annotated compound index.
	 */
	virtual QCString trCompoundIndex()
	{
	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		return Decode("Register dátovıch štruktúr");
	  }
	  else
	  {
		return Decode("Register tried");
	  }
	}

	/*! This is used in LaTeX as the title of the chapter with the
	 * list of all files.
	 */
	virtual QCString trFileIndex()
	{ return Decode("Register súborov"); }

	/*! This is used in LaTeX as the title of the chapter containing
	 *	the documentation of all groups.
	 */
	virtual QCString trModuleDocumentation()
	{ return Decode("Dokumentácia modulov"); }

	/*! This is used in LaTeX as the title of the chapter containing
	 *	the documentation of all classes, structs and unions.
	 */
	virtual QCString trClassDocumentation()
	{ return Decode("Dokumentácia tried"); }

	/*! This is used in LaTeX as the title of the chapter containing
	 *	the documentation of all files.
	 */
	virtual QCString trFileDocumentation()
	{ return Decode("Dokumentácia súborov"); }

	/*! This is used in LaTeX as the title of the chapter containing
	 *	the documentation of all examples.
	 */
	virtual QCString trExampleDocumentation()
	{ return Decode("Dokumentácia príkladov"); }

	/*! This is used in LaTeX as the title of the chapter containing
	 *	the documentation of all related pages.
	 */
	virtual QCString trPageDocumentation()
	{ return Decode("Dokumentácia súvisiacich stránok"); }

	/*! This is used in LaTeX as the title of the document */
	virtual QCString trReferenceManual()
	{ return Decode("Referenèná príruèka"); }

	/*! This is used in the documentation of a file as a header before the
	 *	list of defines
	 */
	virtual QCString trDefines()
	{ return Decode("Definícia makier"); }

	/*! This is used in the documentation of a file as a header before the
	 *	list of function prototypes
	 */
	virtual QCString trFuncProtos()
	{ return Decode("Prototypy"); }

	/*! This is used in the documentation of a file as a header before the
	 *	list of typedefs
	 */
	virtual QCString trTypedefs()
	{ return Decode("Definícia typov"); }

	/*! This is used in the documentation of a file as a header before the
	 *	list of enumerations
	 */
	virtual QCString trEnumerations()
	{ return Decode("Enumerácie"); }

	/*! This is used in the documentation of a file as a header before the
	 *	list of (global) functions
	 */
	virtual QCString trFunctions()
	{ return Decode("Funkcie"); }

	/*! This is used in the documentation of a file as a header before the
	 *	list of (global) variables
	 */
	virtual QCString trVariables()
	{ return Decode("Premenné"); }

	/*! This is used in the documentation of a file as a header before the
	 *	list of (global) variables
	 */
	virtual QCString trEnumerationValues()
	{ return Decode("Hodnoty enumerácií"); }

	/*! This is used in the documentation of a file before the list of
	 *	documentation blocks for defines
	 */
	virtual QCString trDefineDocumentation()
	{ return Decode("Dokumentácia k definíciám makier"); }

	/*! This is used in the documentation of a file/namespace before the list
	 *	of documentation blocks for function prototypes
	 */
	virtual QCString trFunctionPrototypeDocumentation()
	{ return Decode("Dokumentácia prototypov"); }

	/*! This is used in the documentation of a file/namespace before the list
	 *	of documentation blocks for typedefs
	 */
	virtual QCString trTypedefDocumentation()
	{ return Decode("Dokumentácia definícií typov"); }

	/*! This is used in the documentation of a file/namespace before the list
	 *	of documentation blocks for enumeration types
	 */
	virtual QCString trEnumerationTypeDocumentation()
	{ return Decode("Dokumentácia enumeraènıch typov"); }

	/*! This is used in the documentation of a file/namespace before the list
	 *	of documentation blocks for enumeration values
	 */
	virtual QCString trEnumerationValueDocumentation()
	{ return Decode("Dokumentácia enumeraènıch hodnôt"); }

	/*! This is used in the documentation of a file/namespace before the list
	 *	of documentation blocks for functions
	 */
	virtual QCString trFunctionDocumentation()
	{ return Decode("Dokumentácia funkcií"); }

	/*! This is used in the documentation of a file/namespace before the list
	 *	of documentation blocks for variables
	 */
	virtual QCString trVariableDocumentation()
	{ return Decode("Dokumentácia premennıch"); }

	/*! This is used in the documentation of a file/namespace/group before
	 *	the list of links to documented compounds
	 */
	virtual QCString trCompounds()
	{
	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		return Decode("Dátové štruktúry");
	  }
	  else
	  {
		return Decode("Triedy");
	  }
	}

	/*! This is used in the standard footer of each page and indicates when
	 *	the page was generated
	 */
	virtual QCString trGeneratedAt(const char *date,const char *projName)
	{
	  QCString result=(QCString)"Generované "+date;
	  if (projName) result+=(QCString)" pre projekt "+projName;
	  result+=(QCString)" programom";
	  return Decode(result);
	}

	/*! This is part of the sentence used in the standard footer of each page.
	 */
	virtual QCString trWrittenBy()
	{
	  return Decode(" -- autor ");
	}

	/*! this text is put before a class diagram */
	virtual QCString trClassDiagram(const char *clName)
	{
	  return Decode((QCString)"Diagram dediènosti pre triedu "+clName);
	}

	/*! this text is generated when the \\internal command is used. */
	virtual QCString trForInternalUseOnly()
	{ return Decode("Iba pre interné pouitie."); }

	/*! this text is generated when the \\reimp command is used. */
	virtual QCString trReimplementedForInternalReasons()
	{ return Decode("Reimplementované z internıch dôvodov; "
					"aplikaèné rozhranie zachované."); }

	/*! this text is generated when the \\warning command is used. */
	virtual QCString trWarning()
	{ return Decode("Pozor"); }

	/*! this text is generated when the \\bug command is used. */
	virtual QCString trBugsAndLimitations()
	{ return Decode("Chyby a obmedzenia"); }

	/*! this text is generated when the \\version command is used. */
	virtual QCString trVersion()
	{ return Decode("Verzia"); }

	/*! this text is generated when the \\date command is used. */
	virtual QCString trDate()
	{ return Decode("Dátum"); }

	/*! this text is generated when the \\return command is used. */
	virtual QCString trReturns()
	{ return Decode("Návratová hodnota"); }

	/*! this text is generated when the \\sa command is used. */
	virtual QCString trSeeAlso()
	{ return Decode("Viz tie"); }

	/*! this text is generated when the \\param command is used. */
	virtual QCString trParameters()
	{ return Decode("Parametre"); }

	/*! this text is generated when the \\exception command is used. */
	virtual QCString trExceptions()
	{ return Decode("Vınimky"); }

	/*! this text is used in the title page of a LaTeX document. */
	virtual QCString trGeneratedBy()
	{ return Decode("Generované programom"); }

	// new since 0.49-990307

	/*! used as the title of page containing all the index of all namespaces. */
	virtual QCString trNamespaceList()
	{ return Decode("Zoznam priestorov mien"); }

	/*! used as an introduction to the namespace list */
	virtual QCString trNamespaceListDescription(bool extractAll)
	{
	  QCString result="Tu nájdete zoznam všetkıch ";
	  if (!extractAll) result+="dokumentovanıch ";
	  result+="priestorov mien so struènım popisom:";
	  return Decode(result);
	}

	/*! used in the class documentation as a header before the list of all
	 *	friends of a class
	 */
	virtual QCString trFriends()
	{ return Decode("Priatelia (friends)"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

	/*! used in the class documentation as a header before the list of all
	 * related classes
	 */
	virtual QCString trRelatedFunctionDocumentation()
	{ return Decode("Dokumentácia k priate¾om (friends)"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

	/*! used as the title of the HTML page of a class/struct/union */
	virtual QCString trCompoundReference(const char *clName,
									ClassDef::CompoundType compType,
									bool isTemplate)
	{
	  QCString result("Dokumentácia ");
	  if (isTemplate) result+="šablóny ";
	  switch(compType)
	  {
		case ClassDef::Class:	   result+="triedy "; break;
		case ClassDef::Struct:	   result+="štruktúry "; break;
		case ClassDef::Union:	   result+="unionu "; break;
		case ClassDef::Interface:  result+="rozhrania "; break;
		case ClassDef::Protocol:   result+="protocol "; break; // translate me!
		case ClassDef::Category:   result+="category "; break; // translate me!
		case ClassDef::Exception:  result+="vınimky "; break;
	  }
	  result+=clName;
	  return Decode(result);
	}

	/*! used as the title of the HTML page of a file */
	virtual QCString trFileReference(const char *fileName)
	{
	  QCString result("Dokumentácia súboru ");
	  result+=fileName;
	  return Decode(result);
	}

	/*! used as the title of the HTML page of a namespace */
	virtual QCString trNamespaceReference(const char *namespaceName)
	{
	  QCString result("Dokumentácia priestoru mien ");
	  result+=namespaceName;
	  return Decode(result);
	}

	/* these are for the member sections of a class, struct or union */
	virtual QCString trPublicMembers()
	{ return Decode("Verejné metódy"); }
	virtual QCString trPublicSlots()
	{ return Decode("Verejné sloty"); }
	virtual QCString trSignals()
	{ return Decode("Signály"); }
	virtual QCString trStaticPublicMembers()
	{ return Decode("Statické verejné metódy"); }
	virtual QCString trProtectedMembers()
	{ return Decode("Chránené metódy"); }
	virtual QCString trProtectedSlots()
	{ return Decode("Chránené sloty"); }
	virtual QCString trStaticProtectedMembers()
	{ return Decode("Statické chránené metódy"); }
	virtual QCString trPrivateMembers()
	{ return Decode("Privátne metódy"); }
	virtual QCString trPrivateSlots()
	{ return Decode("Privátne sloty"); }
	virtual QCString trStaticPrivateMembers()
	{ return Decode("Statické privátne metódy"); }

	/*! this function is used to produce a comma-separated list of items.
	 *	use generateMarker(i) to indicate where item i should be put.
	 */
	virtual QCString trWriteList(int numEntries)
	{
	  QCString result;
	  int i;
	  // the inherits list contain `numEntries' classes
	  for (i=0;i<numEntries;i++)
	  {
		// use generateMarker to generate placeholders for the class links!
		result+=generateMarker(i); // generate marker for entry i in the list
								   // (order is left to right)

		if (i!=numEntries-1)  // not the last entry, so we need a separator
		{
		  if (i<numEntries-2) // not the fore last entry
			result+=", ";
		  else				  // the fore last entry
			result+=" a ";
		}
	  }
	  return Decode(result);
	}

	/*! used in class documentation to produce a list of base classes,
	 *	if class diagrams are disabled.
	 */
	virtual QCString trInheritsList(int numEntries)
	{
	  QCString result("Dedí od ");
	  result += (numEntries == 1) ? "bázovej triedy " : "bázovıch tried ";
	  result += trWriteList(numEntries)+".";
	  return Decode(result);
	}

	/*! used in class documentation to produce a list of super classes,
	 *	if class diagrams are disabled.
	 */
	virtual QCString trInheritedByList(int numEntries)
	{
	  QCString result("Zdedená ");
	  result += (numEntries == 1) ? "triedou " : "triedami ";
	  result += trWriteList(numEntries)+".";
	  return Decode(result);
	}

	/*! used in member documentation blocks to produce a list of
	 *	members that are hidden by this one.
	 */
	virtual QCString trReimplementedFromList(int numEntries)
	{
	  QCString result("Reimplementuje ");
	  result += (numEntries == 1) ? "metódu triedy " : "metódy tried ";
	  result += trWriteList(numEntries)+".";
	  return Decode(result);
	}

	/*! used in member documentation blocks to produce a list of
	 *	all member that overwrite the implementation of this member.
	 */
	virtual QCString trReimplementedInList(int numEntries)
	{
	  QCString result("Reimplementované ");
	  result += (numEntries == 1) ? "triedou " : "triedami ";
	  result += trWriteList(numEntries)+".";
	  return Decode(result);
	}

	/*! This is put above each page as a link to all members of namespaces. */
	virtual QCString trNamespaceMembers()
	{ return Decode("Symboly v priestoroch mien"); }

	/*! This is an introduction to the page with all namespace members */
	virtual QCString trNamespaceMemberDescription(bool extractAll)
	{
	  QCString result="Tu nájdete zoznam všetkıch ";
	  if (!extractAll) result+="dokumentovanıch ";
	  result+="symbolov, ktoré sú definované vo svojich priestoroch mien. "
			  "U kadého je uvedenı odkaz na ";
	  if (extractAll)
		result+="dokumentáciu príslušného priestoru mien:";
	  else
		result+="príslušnı priestor mien:";
	  return Decode(result);
	}
	/*! This is used in LaTeX as the title of the chapter with the
	 *	index of all namespaces.
	 */
	virtual QCString trNamespaceIndex()
	{ return Decode("Register priestorov mien"); }

	/*! This is used in LaTeX as the title of the chapter containing
	 *	the documentation of all namespaces.
	 */
	virtual QCString trNamespaceDocumentation()
	{ return Decode("Dokumentácia priestorov mien"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

	/*! This is used in the documentation before the list of all
	 *	namespaces in a file.
	 */
	virtual QCString trNamespaces()
	{ return Decode("Priestory mien"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

	/*! This is put at the bottom of a class documentation page and is
	 *	followed by a list of files that were used to generate the page.
	 */
	virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
		bool single)
	{ // here s is one of " Class", " Struct" or " Union"
	  // single is true implies a single file
	  QCString result=(QCString)"Dokumentácia pre ";
	  switch(compType)
	  {
		case ClassDef::Class:	   result+="túto triedu"; break;
		case ClassDef::Struct:	   result+="túto štruktúru (struct)"; break;
		case ClassDef::Union:	   result+="tento union"; break;
		case ClassDef::Interface:  result+="toto rozhranie"; break;
		case ClassDef::Protocol:   result+="protocol"; break; // translate me!
		case ClassDef::Category:   result+="category"; break; // translate me!
		case ClassDef::Exception:  result+="túto vınimku"; break;
	  }
	  result+=" bola generovaná z ";
	  if (single) result+="nasledujúceho súboru:";
	  else		  result+="nasledujúcich súborov:";
	  return Decode(result);
	}

	/*! This is in the (quick) index as a link to the alphabetical compound
	 * list.
	 */
	virtual QCString trAlphabeticalList()
	{ return Decode("Register tried"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

	/*! This is used as the heading text for the retval command. */
	virtual QCString trReturnValues()
	{ return Decode("Návratové hodnoty"); }

	/*! This is in the (quick) index as a link to the main page (index.html)
	 */
	virtual QCString trMainPage()
	{ return Decode("Hlavná stránka"); }

	/*! This is used in references to page that are put in the LaTeX
	 *	documentation. It should be an abbreviation of the word page.
	 */
	virtual QCString trPageAbbreviation()
	{ return Decode("s."); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

	virtual QCString trSources()
	{
	  return Decode("Zdroje");
	}
	virtual QCString trDefinedAtLineInSourceFile()
	{
	  return Decode("Definícia je uvedená na riadku @0 v súbore @1.");
	}
	virtual QCString trDefinedInSourceFile()
	{
	  return Decode("Definícia v súbore @0.");
	}

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

	virtual QCString trDeprecated()
	{
	  return Decode("Zastaralé");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

	/*! this text is put before a collaboration diagram */
	virtual QCString trCollaborationDiagram(const char *clName)
	{
	  return Decode((QCString)"Diagram tried pre "+clName+":");
	}
	/*! this text is put before an include dependency graph */
	virtual QCString trInclDepGraph(const char *fName)
	{
	  return Decode((QCString)"Graf závislostí na vkladanıch súboroch "
					"pre "+fName+":");
	}
	/*! header that is put before the list of constructor/destructors. */
	virtual QCString trConstructorDocumentation()
	{
	  return Decode("Dokumentácia konštruktoru a deštruktoru");
	}
	/*! Used in the file documentation to point to the corresponding sources. */
	virtual QCString trGotoSourceCode()
	{
	  return Decode("Zobrazi zdrojovı text tohoto súboru.");
	}
	/*! Used in the file sources to point to the corresponding documentation. */
	virtual QCString trGotoDocumentation()
	{
	  return Decode("Zobrazi dokumentáciu tohoto súboru.");
	}
	/*! Text for the \\pre command */
	virtual QCString trPrecondition()
	{
	  return Decode("Prepodmienka");
	}
	/*! Text for the \\post command */
	virtual QCString trPostcondition()
	{
	  return Decode("Postpodmienka");
	}
	/*! Text for the \\invariant command */
	virtual QCString trInvariant()
	{
	  return Decode("Invariant");
	}
	/*! Text shown before a multi-line variable/enum initialization */
	virtual QCString trInitialValue()
	{
	  return Decode("Inicializátor:");
	}
	/*! Text used the source code in the file index */
	virtual QCString trCode()
	{
	  return Decode("zdrojovı text");
	}
	virtual QCString trGraphicalHierarchy()
	{
	  return Decode("Grafické zobrazenie hierarchie tried");
	}
	virtual QCString trGotoGraphicalHierarchy()
	{
	  return Decode("Zobrazi grafickú podobu hierarchie tried");
	}
	virtual QCString trGotoTextualHierarchy()
	{
	  return Decode("Zobrazi textovú podobu hierarchie tried");
	}
	virtual QCString trPageIndex()
	{
	  return Decode("Register stránok");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

	virtual QCString trNote()
	{
	  return Decode("Poznámka");
	}
	virtual QCString trPublicTypes()
	{
	  return Decode("Verejné typy");
	}
	virtual QCString trPublicAttribs()
	{
	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
	  {
		return Decode("Dátové poloky");
	  }
	  else
	  {
		return Decode("Verejné atribúty");
	  }
	}
	virtual QCString trStaticPublicAttribs()
	{
	  return Decode("Statické verejné atribúty");
	}
	virtual QCString trProtectedTypes()
	{
	  return Decode("Chránené typy");
	}
	virtual QCString trProtectedAttribs()
	{
	  return Decode("Chránené atribúty");
	}
	virtual QCString trStaticProtectedAttribs()
	{
	  return Decode("Statické chránené atribúty");
	}
	virtual QCString trPrivateTypes()
	{
	  return Decode("Privátne typy");
	}
	virtual QCString trPrivateAttribs()
	{
	  return Decode("Privátne atribúty");
	}
	virtual QCString trStaticPrivateAttribs()
	{
	  return Decode("Statické privátne atribúty");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

	/*! Used as a marker that is put before a todo item */
	virtual QCString trTodo()
	{
	  return Decode("Plánované úpravy");
	}
	/*! Used as the header of the todo list */
	virtual QCString trTodoList()
	{
	  return Decode("Zoznam plánovanıch úprav");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

	virtual QCString trReferencedBy()
	{
	  return Decode("Pouíva sa v");
	}
	virtual QCString trRemarks()
	{
	  return Decode("Poznámky"); // ??? not checked in a context
	}
	virtual QCString trAttention()
	{
	  return Decode("Upozornenie"); // ??? not checked in a context
	}
	virtual QCString trInclByDepGraph()
	{
	  return Decode("Nasledujúci graf ukazuje, ktoré súbory priamo alebo "
			 "nepriamo vkladajú tento súbor:");
	}
	virtual QCString trSince()
	{
	  return Decode("Od"); // ??? not checked in a context
	}

////////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

	/*! title of the graph legend page */
	virtual QCString trLegendTitle()
	{
	  return Decode("Vysvetlivky ku grafu");
	}
	/*! page explaining how the dot graph's should be interpreted */
	virtual QCString trLegendDocs()
	{
	  QCString result(
		"Tu nájdete vysvetlenie, ako majú by interpretované grafy, "
		"ktoré boli generované programom doxygen.<p>\n"
		"Uvaujte nasledujúci príklad:\n"
		"\\code\n"
		"/*! Neviditelná trieda, ktorá sa v grafe nezobrazuje, pretoe "
		"došlo k orezaniu grafu. */\n"
		"class Invisible { };\n\n"
		"/*! Trieda, u ktorej došlo k orezaniu grafu. Vzah dediènosti "
		"je skrytı. */\n"
		"class Truncated : public Invisible { };\n\n"
		"/* Trieda, ktorá nieje dokumentovaná komentármi programu doxygen. */\n"
		"class Undocumented { };\n\n"
		"/*! Odvodená trieda s verejnım (public) dedením bázovej triedy. */\n"
		"class PublicBase : public Truncated { };\n\n"
		"/*! Odvodená trieda s chránenım (protected) dedením bázovej triedy. */\n"
		"class ProtectedBase { };\n\n"
		"/*! Odvodená trieda s privátnym dedením bázovej triedy. */\n"
		"class PrivateBase { };\n\n"
		"/*! Trieda, ktorá je vyuívaná triedou Inherited. */\n"
		"class Used { };\n\n"
		"/*! Odvodená trieda, ktorá rôznym spôsobom dedí od viacerıch bázovıch "
		"tried. */\n"
		"class Inherited : public PublicBase,\n"
		"				   protected ProtectedBase,\n"
		"				   private PrivateBase,\n"
		"				   public Undocumented\n"
		"{\n"
		"  private:\n"
		"	 Used *m_usedClass;\n"
		"};\n"
		"\\endcode\n"
		"Pokia¾ je poloka \\c MAX_DOT_GRAPH_HEIGHT konfiguraèného súboru "
		"nastavená na hodnotu 200, bude vygenerovanı nasledujúci graf:"
		"<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
		"<p>\n"
		"Bloky (tj. uzly) v uvedenom grafe majú nasledujúci vıznam:\n"
		"<ul>\n"
		"<li>Èierne vyplnenı obdånik reprezentuje štruktúru alebo triedu, "
			"pre ktorú bol graf generovanı.\n"
		"<li>Obdånik s èiernym obrysom oznaèuje dokumentovanú "
			"štruktúru alebo triedu.\n"
		"<li>Obdånik so šedım obrysom oznaèuje nedokumentovanú "
			"štruktúru alebo triedu.\n"
		"<li>Obdånik s èervenım obrysom oznaèuje dokumentovanú "
			"štruktúru alebo triedu, pre ktorú\n"
			"niesú zobrazené všetky vzahy dediènosti alebo obsiahnutia. "
			"Graf je orezanı v prípade, kedy ho\n"
			"nieje moné umiestni do vymedzenıch hraníc.\n"
		"</ul>\n"
		"Šípky (tj. hrany grafu) majú nasledujúcí vıznam:\n"
		"<ul>\n"
		"<li>Tmavo modrá šípka sa pouíva pre oznaèenie vzahu verejnej "
			"dediènosti medzi dvoma triedami.\n"
		"<li>Tmavo zelená šípka oznaèuje vzah chránenej dediènosti "
			"(protected).\n"
		"<li>Tmavo èervená šípka oznaèuje vzah privátnej dediènosti.\n"
		"<li>Purpurová šípka kreslená èiarkovane sa pouíva v prípade, "
			"ak je trieda obsiahnutá v inej triede,\n"
			"alebo ak je pouívaná inou triedou. Je oznaèená identifikátorom "
			"jednej alebo viacerıch premennıch (objektov), cez ktoré\n"
			"je trieda alebo štruktúra zprístupnena.\n"
		"</ul>\n");

		return Decode(result);
	}
	/*! text for the link to the legend page */
	virtual QCString trLegend()
	{
	  return Decode("vysvetlivky");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

	/*! Used as a marker that is put before a test item */
	virtual QCString trTest()
	{
	  return Decode("Test");
	}

	/*! Used as the header of the test list */
	virtual QCString trTestList()
	{
	  return Decode("Zoznam testov");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

	/*! Used as a section header for KDE-2 IDL methods */
	virtual QCString trDCOPMethods()
	{
	  return Decode("Metódy DCOP");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

	/*! Used as a section header for IDL properties */
	virtual QCString trProperties()
	{
	  return Decode("Vlastnosti");
	}
	/*! Used as a section header for IDL property documentation */
	virtual QCString trPropertyDocumentation()
	{
	  return Decode("Dokumentácia k vlastnosti");
	}
//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

	/*! Used for Java interfaces in the summary section of Java packages */
	virtual QCString trInterfaces()
	{
	  return Decode("Rozhranie");
	}
	/*! Used for Java classes in the summary section of Java packages */
	virtual QCString trClasses()
	{
	  return Decode("Triedy");
	}
	/*! Used as the title of a Java package */
	virtual QCString trPackage(const char *name)
	{
	  return Decode((QCString)"Balík "+name);
	}
	/*! Title of the package index page */
	virtual QCString trPackageList()
	{
	  return Decode("Zoznam balíkov");
	}
	/*! The description of the package index page */
	virtual QCString trPackageListDescription()
	{
	  return Decode("Tu nájdete zoznam balíkov so struènım popisom "
					"(pokia¾ bol uvedenı):");
	}
	/*! The link name in the Quick links header for each page */
	virtual QCString trPackages()
	{
	  return Decode("Balíky");
	}
	/*! Used as a chapter title for Latex & RTF output */
	virtual QCString trPackageDocumentation()
	{
	  return Decode("Dokumentácia balíku");
	}
	/*! Text shown before a multi-line define */
	virtual QCString trDefineValue()
	{
	  return Decode("Hodnota:");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

	/*! Used as a marker that is put before a \\bug item */
	virtual QCString trBug()
	{
	  return Decode("Chyba");
	}
	/*! Used as the header of the bug list */
	virtual QCString trBugList()
	{
	  return Decode("Zoznam chıb");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6-20010422
//////////////////////////////////////////////////////////////////////////

	/*! Used as ansicpg for RTF file */
	virtual QCString trRTFansicp()
	{
	  return "1250";
	}

	/*! Used as ansicpg for RTF fcharset */
	virtual QCString trRTFCharSet()
	{
	  return "3";
	}

	/*! Used as header RTF general index */
	virtual QCString trRTFGeneralIndex()
	{
	  return "Index";
	}

	/*! This is used for translation of the word that will possibly
	 *	be followed by a single name or by a list of names
	 *	of the category.
	 */
	virtual QCString trClass(bool first_capital, bool singular)
	{
	  QCString result((first_capital ? "Tried" : "tried"));
	  result+=(singular ? "a" : "y");
	  return Decode(result);
	}

	/*! This is used for translation of the word that will possibly
	 *	be followed by a single name or by a list of names
	 *	of the category.
	 */
	virtual QCString trFile(bool first_capital, bool singular)
	{
	  QCString result((first_capital ? "Súbor" : "súbor"));
	  if (!singular)  result+="y";
	  return Decode(result);
	}

	/*! This is used for translation of the word that will possibly
	 *	be followed by a single name or by a list of names
	 *	of the category.
	 */
	virtual QCString trNamespace(bool first_capital, bool singular)
	{
	  QCString result((first_capital ? "Priestor" : "priestor"));
	  if (!singular)  result+="y";
	  result+=" mien";
	  return Decode(result);
	}

	/*! This is used for translation of the word that will possibly
	 *	be followed by a single name or by a list of names
	 *	of the category.
	 */
	virtual QCString trGroup(bool first_capital, bool singular)
	{
	  QCString result((first_capital ? "Skupin" : "skupin"));
	  result+=(singular ? "a" : "y");
	  return Decode(result);
	}

	/*! This is used for translation of the word that will possibly
	 *	be followed by a single name or by a list of names
	 *	of the category.
	 */
	virtual QCString trPage(bool first_capital, bool singular)
	{
	  QCString result((first_capital ? "Stránk" : "stránk"));
	  result+=(singular ? "a" : "y");
	  return Decode(result);
	}

	/*! This is used for translation of the word that will possibly
	 *	be followed by a single name or by a list of names
	 *	of the category.
	 */
	virtual QCString trMember(bool first_capital, bool singular)
	{
	  QCString result((first_capital ? "Èlen" : "èlen"));
	  if (!singular)  result+="y";
	  return Decode(result);
	}

	/*! This is used for translation of the word that will possibly
	 *	be followed by a single name or by a list of names
	 *	of the category.
	 */
	virtual QCString trField(bool first_capital, bool singular)
	{
	  QCString result((first_capital ? "Polok" : "polok"));
	  result+=(singular ? "a" : "y");
	  return Decode(result);
	}

	/*! This is used for translation of the word that will possibly
	 *	be followed by a single name or by a list of names
	 *	of the category.
	 */
	virtual QCString trGlobal(bool first_capital, bool singular)
	{
	  QCString result((first_capital ? "Globáln" : "globáln"));
	  result+=(singular ? "y" : "e");
	  return result;
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

	/*! This text is generated when the \\author command is used and
	 *	for the author section in man pages. */
	virtual QCString trAuthor(bool first_capital, bool singular)
	{
	  QCString result((first_capital ? "Auto" : "auto"));
	  result += (singular) ? "r" : "ri";
	  return result;
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

	/*! This text is put before the list of members referenced by a member
	 */
	virtual QCString trReferences()
	{
	  return Decode("Odkazuje sa na");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

	/*! used in member documentation blocks to produce a list of
	 *	members that are implemented by this one.
	 */
	virtual QCString trImplementedFromList(int numEntries)
	{
	  return "Implementuje " + trWriteList(numEntries) + ".";
	}

	/*! used in member documentation blocks to produce a list of
	 *	all members that implement this member.
	 */
	virtual QCString trImplementedInList(int numEntries)
	{
	  return Decode("Implementované v " + trWriteList(numEntries) + ".");
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

	/*! used in RTF documentation as a heading for the Table
	 *	of Contents.
	 */
	virtual QCString trRTFTableOfContents()
	{
	  return "Obsah";
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

	/*! Used as the header of the list of item that have been
	 *	flagged deprecated
	 */
	virtual QCString trDeprecatedList()
	{
	  return "Zastarané metódy";
	}
};

#endif // TRANSLATOR_SK_H
