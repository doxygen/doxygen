/******************************************************************************
 *
 *
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

/* English to Hindi keyword dictionary
 * alias = उपनाम
 * alias = उपनामगण
 * architecture = वास्तुकला
 * architectures = वास्तुकलाएं
 * argument = तर्क
 * arguments = तर्कगण
 * attribute = गुण
 * attrubutes = गुणगण
 * category = श्रेणी
 * categories = श्रेणीयाँ
 * code = संकेत-लिपि
 * codes = संकेत-लिपियाॅं
 * component = अंग
 * components = अंगगण
 * concept = संकल्पना
 * concepts = संकल्पनाएं
 * configuration = विन्यास
 * configurations = विन्यासगण
 * const/constant = स्थिर
 * consts/constants = स्थिरगण
 * constructor = निर्माता
 * class = वर्ग
 * classes = वर्गगण
 * datum = आंकड़ा
 * data = आंकड़े
 * data structure = आंकड़ा संरचना (datum structure)
 * data structures = आंकड़े संरचनाएं
 * destructor = नाशक
 * directory = निर्देशिका
 * directories = निर्देशिकाएं
 * doxygen = डाॅक्सिजन
 * entity = इकाइ
 * entities = इकाइयाॅं
 * enum/enumerator = परिगणक
 * enumeration = परिगणना
 * event = घटना
 * events = घटनाएं
 * example = उदाहरण
 * examples = उदाहरणगण
 * exception = अपवाद
 * exceptions = अपवादगण
 * field = भाग
 * fields = भागगण
 * file = फ़ाइल
 * friend = मित्र
 * friends = मित्रगण
 * function/routine = फलन
 * functions/routines = फलनगण
 * global = वैश्र्विक
 * globals = वैश्र्विकगण
 * group = समूह
 * groups = समूहगण
 * instance = उदाहरण
 * instances = उदाहरणगण
 * instantiation = उदाहरणीकरण
 * instantiations = उदाहरणीकरणगण
 * interface = अंतराफलक
 * interfaces = अंतराफलकगण
 * inherit = 
 * inheritance = वरासत
 * inherited = वरासित
 * library = संग्रह
 * libraries = संग्रहगण
 * member = सदस्य
 * members = सदस्यगण
 * method = विधि
 * methods = विधियाँ
 * module = अनुखंड
 * modules = अनुखंडगण
 * namespace = नाम-स्थान
 * operation = कार्यवाही
 * operations = कार्यवाहीयाँ
 * overload = अधिभार
 * overloaded = अधिभारित
 * package = संकुल
 * packages = संकुलगण
 * page = पृष्ठ
 * pages = पृष्ठगण
 * parameter = मापदंड
 * parameters = मापदंडगण
 * port = द्वार
 * ports = द्वारगण
 * private = निजी
 * procedure = कार्यविधि
 * procedures = कार्यविधियाँ
 * process = प्रक्रिया
 * processes = प्रक्रियाएं
 * property = संपत्ति
 * properties = संपत्तियाँ
 * protected = संरक्षित
 * protocol = प्रोटोकॉल
 * public = सार्वजनिक
 * record = अभिलेख
 * records = अभिलेखगण
 * shared = साझाकृत
 * search = खोजें
 * service = सेवा
 * services = सेवाएं
 * singleton = एकल
 * singletons = एकलगण
 * static = अचल
 * struct/structure = संरचना
 * structs/structures = संरचनाएं
 * subfunction/subroutine = उपफलन
 * subfunctions/subroutines = उपफलनगण
 * subtype = उपप्ररुप
 * subtypes = उपप्ररुपगण
 * super = उत्तम
 * synchronize = सिंक्रनाइज़
 * synchronization = सिंक्रनाइज़ीकरण
 * template = टेम्पलेट
 * templates = टेम्पलेटगण
 * typedef = प्ररुप-परिभाषा
 * typedefs = प्ररुप-परिभाषागण
 * union = मिलन
 * unions = मिलनगण
 * variable = परिवर्तनशील
 * variables = परिवर्तनशीलगण
 */

#ifndef TRANSLATOR_HI_H
#define TRANSLATOR_HI_H

/*!
 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionally to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorHindi : public Translator
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage()
    { return "hindi"; }

    /*! Used to get the LaTeX command(s) for the language support.
     *  This method should return string with commands that switch
     *  LaTeX to the desired language.  For example
     *  <pre>"\\usepackage[german]{babel}\n"
     *  </pre>
     *  or
     *  <pre>"\\usepackage{polski}\n"
     *  "\\usepackage[latin2]{inputenc}\n"
     *  "\\usepackage[T1]{fontenc}\n"
     *  </pre>
     *
     * The English LaTeX does not use such commands.  Because of this
     * the empty string is returned in this implementation.
     */
    virtual QCString latexLanguageSupportCommand()
    { return ""; }

    virtual QCString trISOLang()
    { return "hi-IN"; }

    virtual QCString getLanguageString()
    { return "0x439 Hindi"; }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "संबंधित फलन"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(ध्यान दें कि ये सदस्य फलन नहीं हैं।)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "विस्तृत विवरण"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "सदस्य प्ररुप-परिभाषा दस्तावेज़ीकरण"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "सदस्य परिगणना दस्तावेज़ीकरण"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "सदस्य फलन दस्तावेज़ीकरण"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "भाग दस्तावेज़ीकरण";
      else
        return "सदस्य आंकड़े दस्तावेज़ीकरण";
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "और..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "सभी सदस्यों की सूची"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "सदस्य सूची"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "यह है सदस्यों की पूरी सूची "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", सभी विरासत में मिले सदस्यों सहित।"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s)
    {
      QCString result = "स्रोत संकेत-लिपि से ";
      if (!s.isEmpty())
        result += s + " के लिए ";
      result += "डॉक्सिजन द्वारा स्वचालित रूप से उत्पन्न किया गया।";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "परिगणक नाम"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "परिगणक मूल्य"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "में परिभाषित"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "अनुखंडगण"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "वर्ग पदानुक्रम"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े संरचनाएं";
      else
        return "वर्ग सूची";
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "फ़ाइल सूची"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े भागगण";
      else
        return "वर्ग सदस्यगण";
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "वैश्र्विकगण";
      else
        return "फ़ाइल सदस्यगण";
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "संबंधित पृष्ठगण"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "उदाहरणगण"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "खोजें"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
        return "यहाँ वर्गगण की एक पदानुक्रमनुसार सूची दी गई है:";
      else
        return "यह वरासत सूची मोटे तौर पर क्रमबद्ध है, लेकिन पूरी तरह से नहीं, वर्णानुक्रम में:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result = "यहाँ संक्षिप्त विवरण के साथ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "फ़ाइलों की सूची दी गई है:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "यहाँ संक्षिप्त विवरण के साथ आंकड़े संरचनाएँ हैं:";
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
        return "यहाँ संक्षिप्त विवरण के साथ वर्गगण दी गई हैं:";
      else
        return "यहाँ संक्षिप्त विवरण के साथ सभी वर्गगण, संरचनाएं, मिलनगण और अंतराफलकगण की सूची दी गई हैं:";
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result = "यहाँ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        result += "संरचना और मिलन भागगण";
      else
        result += "वर्ग सदस्यगण";
      result += " कि लिंको के साथ ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          result += "प्रत्येक भाग के संरचना/मिलन दस्तावेज़ीकरण ";
        else
          result += "प्रत्येक भाग के वर्ग दस्तावेज़ीकरण ";
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          result += "संबंधित संरचनाएं/मिलनगण ";
        else
          result += "संबंधित वर्गगण ";
      }
      result += "कि सूची दि गई हैं:";
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result = "यहाँ सभी ";
      if (!extractAll) result += "दस्तावेज़ीकृत ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        result += "फलनगण, परिवर्तनशीलगण, मैक्रोगण, परिगणकगण और प्ररुप-परिभाषाएं";
      else
        result += "फ़ाइल सदस्यगण";
      result += " कि लिंको के साथ ";
      if (extractAll)
        result += "संबंधित फाइलों ";
      else
        result += "दस्तावेज़ीकरण ";
      result += "कि सूची दि गई हैं:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "यहाँ सभी उदाहरणों की एक सूची दी गई है:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "यहाँ सभी संबंधित दस्तावेज़ीकरण पृष्ठों की सूची दी गई है:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "यहाँ सभी अनुखंडों की एक सूची है:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "दस्तावेज़ीकरण"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "अनुखंड अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "पदानुक्रमनुसार अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े संरचना अनुक्रमणिका";
      else
        return "वर्ग अनुक्रमणिका";
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex()
    { return "फ़ाइल अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "अनुखंड दस्तावेज़ीकरण"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े संरचना दस्तावेज़ीकरण";
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
        return trDesignUnitDocumentation();
      else
        return "वर्ग दस्तावेज़ीकरण";
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "फ़ाइल दस्तावेज़ीकरण"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "उदाहरण दस्तावेज़ीकरण"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "संदर्भ पुस्तिका"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "मैक्रोगण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "प्ररुप-परिभाषाएं"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "परिगणकगण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "फलनगण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "परिवर्तनशीलगण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "परिगणक"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "मैक्रो परिभाषा दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "प्ररुप-परिभाषा दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "परिगणना प्ररूप दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "फलन दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "परिवर्तनशील दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े संरचनाएं";
      else
        return "वर्गगण";
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName)
    {
      QCString result;
      if (!projName.isEmpty())
        result += projName + " के लिए ";
      result += date + " पर उतपन्न किया गया: ";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName)
    { return clName + " के लिए वरासत आरेख:"; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "चेतावनी"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "संस्करण"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "दिनांक"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "वापसी"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "यह भी देखें"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "मापदंडगण"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "अपवादगण"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "द्वारा उत्पन्न"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "नाम-स्थान सूची"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result = "यहाँ संक्षिप्त विवरण के साथ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "नाम-स्थानों की सूची दी गई है:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "मित्रगण"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "मित्रगण और संबंधित फलन दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(
      const QCString &clName,
      ClassDef::CompoundType compType,
      bool isTemplate)
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result += " वर्ग"; break;
        case ClassDef::Struct:     result += " संरचना"; break;
        case ClassDef::Union:      result += " मिलन"; break;
        case ClassDef::Interface:  result += " अंतराफलक"; break;
        case ClassDef::Protocol:   result += " प्रोटोकॉल"; break;
        case ClassDef::Category:   result += " श्रेणी"; break;
        case ClassDef::Exception:  result += " अपवाद"; break;
        default: break;
      }
      if (isTemplate)
        result += " टेम्पलेट";
      result += " संदर्भ";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const QCString &fileName)
    {
      QCString result = fileName;
      result += " फ़ाइल संदर्भ";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName)
    {
      QCString result = namespaceName;
      result += " नाम-स्थान संदर्भ";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "सार्वजनिक सदस्य फलनगण"; }

    virtual QCString trPublicSlots()
    { return "सार्वजनिक खांचें"; }

    virtual QCString trSignals()
    { return "संकेतगण"; }

    virtual QCString trStaticPublicMembers()
    { return "अचल सार्वजनिक सदस्य फलनगण"; }

    virtual QCString trProtectedMembers()
    { return "संरक्षित सदस्य फलनगण"; }

    virtual QCString trProtectedSlots()
    { return "संरक्षित खांचे"; }

    virtual QCString trStaticProtectedMembers()
    { return "अचल संरक्षित सदस्य फलनगण"; }

    virtual QCString trPrivateMembers()
    { return "निजी सदस्य फलनगण"; }

    virtual QCString trPrivateSlots()
    { return "निजी खांचें"; }

    virtual QCString trStaticPrivateMembers()
    { return "अचल निजी सदस्य फलनगण"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries)
    {
      QCString result;
      int i;
      // the inherits list contain `numEntries' classes
      for (i = 0; i < numEntries; ++i)
      {
        // use generateMarker to generate placeholders for the class links!
        result += generateMarker(i); // generate marker for entry i in the list
                                    // (order is left to right)
        if (i != numEntries - 1)  // not the last entry, so we need a separator
        {
          if (i < numEntries - 2) // not the fore last entry
            result += ", ";
          else                    // the fore last entry
            result += ", और ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    { return trWriteList(numEntries) + " से विरासित।"; }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    { return trWriteList(numEntries) + " द्वारा विरासित।"; }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    { return trWriteList(numEntries) + " से पुन: लागू किया गया।"; }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    { return trWriteList(numEntries) + " में पुन: लागू किया गया।"; }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "नाम-स्थान सदस्यगण"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result = "यहाँ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "नाम-स्थान सदस्यगण कि लिंको के साथ ";
      if (extractAll)
        result += "प्रत्येक सदस्य के नाम-स्थान दस्तावेज़ीकरण ";
      else
        result += "उनसे संबंधित नाम-स्थानों ";
      result += "कि सूची दि गई हैं:";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "नाम-स्थान अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "नाम-स्थान दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "नाम-स्थानगण"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    {
      // single is true implies a single file
      static bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result;
      switch (compType)
      {
        case ClassDef::Class:     result += vhdlOpt ? "डिज़ाइन इकाई" : "वर्ग"; break;
        case ClassDef::Struct:    result += "संस्करण"; break;
        case ClassDef::Union:     result += "मिलन"; break;
        case ClassDef::Interface: result += "अंतराफलक"; break;
        case ClassDef::Protocol:  result += "प्रोटोकॉल"; break;
        case ClassDef::Category:  result += "श्रेणी"; break;
        case ClassDef::Exception: result += "अपवाद"; break;
        default: break;
      }
      result += " के लिए दस्तावेज़ीकरण ";
      if (single)
        result += " फ़ाइल से उत्पन्न किया गया था:";
      else
        result += " निम्न फ़ाइलों से उत्पन्न किया गया था:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "वापसी मान"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "मुख्य पृष्ठ"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "पृ."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    { return "फ़ाइल @1 की लाइन @0 पर परिभाषित।"; }

    virtual QCString trDefinedInSourceFile()
    { return "फ़ाइल @0 में परिभाषित।"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    { return "पदावनत"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName)
    { return clName + " के लिए सहयोगीकरण आरेख:"; }

    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName)
    { return fName + " के लिए निर्भरता लेखाचित्र शामिल करें:"; }

    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    { return "निर्माता और नाशक दस्तावेज़ीकरण"; }

    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    { return "इस फाइल कि स्त्रोत संकेत-लिपि को देखें।"; }

    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    { return "इस फ़ाइल के दस्तावेज़ीकरण पर जाएं।"; }

    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    { return "पूर्वशर्त"; }

    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    { return "शर्तपश्चात्"; }

    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    { return "अपरिवर्तनीय"; }

    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    { return "प्रारंभिक मूल्य:"; }

    /*! Text used the source code in the file index */
    virtual QCString trCode()
    { return "संकेत-लिपि"; }

    virtual QCString trGraphicalHierarchy()
    { return "चित्रात्मक वर्ग पदानुक्रम"; }

    virtual QCString trGotoGraphicalHierarchy()
    { return "चित्रात्मक वर्ग पदानुक्रम पर जाएँ"; }

    virtual QCString trGotoTextualHierarchy()
    { return "पाठ्य वर्ग पदानुक्रम पर जाएँ"; }

    virtual QCString trPageIndex()
    { return "पृष्ठ अनुक्रमणिका"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    { return "ध्यान दें"; }

    virtual QCString trPublicTypes()
    { return "सार्वजनिक प्ररुपगण"; }

    virtual QCString trPublicAttribs()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े भागगण";
      else
        return "सार्वजनिक गुणगण";
    }

    virtual QCString trStaticPublicAttribs()
    { return "अचल सार्वजनिक गुणगण"; }

    virtual QCString trProtectedTypes()
    { return "संरक्षित प्ररुपगण"; }

    virtual QCString trProtectedAttribs()
    { return "संरक्षित गुणगण"; }

    virtual QCString trStaticProtectedAttribs()
    { return "अचल संरक्षित गुणगण"; }

    virtual QCString trPrivateTypes()
    { return "निजी प्ररुपगण"; }

    virtual QCString trPrivateAttribs()
    { return "निजी गुणगण"; }

    virtual QCString trStaticPrivateAttribs()
    { return "अचल निजी गुणगण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    { return "करने के लिए"; }

    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    { return "करने के लिए सूची"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    { return "द्वारा संदर्भित"; }

    virtual QCString trRemarks()
    { return "टिप्पणियाँ"; }

    virtual QCString trAttention()
    { return "ध्यान"; }

    virtual QCString trInclByDepGraph()
    { return "यह लेखाचित्र दिखाता है कि कौन सी फ़ाइलें प्रत्यक्ष या परोक्ष रूप से इस फ़ाइल को शामिल करती हैं:"; }

    virtual QCString trSince()
    { return "जबसे"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    { return "लेखाचित्र किंवदंती"; }

    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return
        "यह पृष्ठ बताता है कि डॉक्सिजन द्वारा उत्पन्न लेखाचित्र की व्याख्या कैसे करें।\n"
        "<p>\n"
        "निम्नलिखित उदाहरण पर विचार करें:\n"
        "\\code\n"
        "/*! काट-छाँट के कारण अदृश्य वर्ग */\n"
        "class Invisible { };\n\n"
        "/*! वरासत संबंध छिपा हुआ है, वर्ग काट-छाँट दिया गया */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* वर्ग को डॉक्सिजन टिप्पणियों के साथ दस्तावेज़ीकृत नहीं किया गया */\n"
        "class Undocumented { };\n\n"
        "/*! वह वर्ग जो सार्वजनिक वरासत का उपयोग करके विरासित की गई */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! एक टेम्पलेट वर्ग */\n"
        "template<class T> class Templ { };\n\n"
        "/*! वह वर्ग जो संरक्षित वरासत का उपयोग करके विरासित की गई */\n"
        "class ProtectedBase { };\n\n"
        "/*! वह वर्ग जो निजी वरासत का उपयोग करके विरासित की गई */\n"
        "class PrivateBase { };\n\n"
        "/*! वह वर्ग जिसका उपयोग विरासित वर्ग द्वारा किया जाता हैं */\n"
        "class Used { };\n\n"
        "/*! उत्तम वर्ग जो कई अन्य वर्गों से वरासित हैं */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "इसका परिणाम निम्नलिखित लेखाचित्र में होगा:"
        "<p><center><img alt=\"\" src=\"graph_legend." + getDotImageExtension() + "\"></center></p>\n"
        "<p>\n"
        "उपरोक्त लेखाचित्र के डिब्बों के निम्नलिखित अर्थ हैं:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%A भरा ग्रे डिब्बा उस संरचना या वर्ग को दर्शाता है जिसके लिए लेखाचित्र बनाया गया हैं।</li>\n"
        "<li>%A डिब्बा, काली बॉर्डर वाला, एक दस्तावेज़ीकृत संरचना या वर्ग को निरूपित करता हैं।</li>\n"
        "<li>%A डिब्बा, ग्रे बॉर्डर वाला, एक निरदस्तावेज़ीकृत संरचना या वर्ग को निरूपित है।</li>\n"
        "<li>%A डिब्बा, लाल बॉर्डर वाला, एक दस्तावेज़ीकृत संरचना या वर्ग को निरूपित है जिसके लिए "
        "सभी वरासत संबंध नहीं दिखाए गए हैं। %A लेखाचित्र को काट-छाँट दिया जाता है यदि वह निर्दिष्ट "
        "सीमाओं के भीतर नहीं समा पाता हैं।</li>\n"
        "</ul>\n"
        "<p>\n"
        "तीर के निम्नलिखित अर्थ हैं:\n"
        "</p>\n"
        "<ul>\n"
        "<li>%A गहरे नीले तीर का उपयोग दो वर्गों के बीच सार्वजनिक वरासत संबंध की कल्पना करने "
        "के लिए किया जाता हैं।</li>\n"
        "<li>%A गहरे हरे तीर का उपयोग संरक्षित वरासत के लिए किया जाता हैं।</li>\n"
        "<li>%A गहरे लाल तीर का उपयोग निजी वरासत के लिए किया जाता हैं।</li>\n"
        "<li>%A बैंगनी धराशायी तीर का उपयोग किया जाता है यदि कोई वर्ग समाहित है या किसी अन्य "
        "वर्ग द्वारा उपयोग किया जाता है। तीर को परिवर्तनशील(गण) के साथ लेबल किया गया है जिसके "
        "माध्यम से वर्ग या संरचना पहुंचने योग्य हैं।</li>\n"
        "<li>%A पीला धराशायी तीर एक टेम्पलेट उदाहरण और उस टेम्पलेट वर्ग के बीच संबंध को दर्शाता "
        "है जिससे इसे उदाहरणीत किया गया था। तीर को उदाहरण के टेम्पलेट मापदंड के साथ लेबल किया गया है।</li>\n"
        "</ul>\n";
    }

    /*! text for the link to the legend page */
    virtual QCString trLegend()
    { return "किंवदंती"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    { return "परीक्षा"; }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    { return "परीक्षा सूची"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    { return "संपत्तियाँ"; }

    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    { return "संपत्ति दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े संरचनाएं";
      else
        return "वर्गगण";
    }

    /*! Used as the title of a Java package */
    virtual QCString trPackage(const QCString &name)
    { return "संकुल " + name; }

    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    { return "यहाँ संक्षिप्त विवरण के साथ संकुल दिए गए हैं (यदि उपलब्ध हो):"; }

    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    { return "संकुलगण"; }

    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    { return "मूल्य:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    { return "त्रुटि"; }

    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    { return "त्रुटि सूची"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file
     *
     * The following table shows the correlation of Charset name, Charset Value and
     * <pre>
     * Codepage number:
     * Charset Name       Charset Value(hex)  Codepage number
     * ------------------------------------------------------
     * DEFAULT_CHARSET           1 (x01)
     * SYMBOL_CHARSET            2 (x02)
     * OEM_CHARSET             255 (xFF)
     * ANSI_CHARSET              0 (x00)            1252
     * RUSSIAN_CHARSET         204 (xCC)            1251
     * EE_CHARSET              238 (xEE)            1250
     * GREEK_CHARSET           161 (xA1)            1253
     * TURKISH_CHARSET         162 (xA2)            1254
     * BALTIC_CHARSET          186 (xBA)            1257
     * HEBREW_CHARSET          177 (xB1)            1255
     * ARABIC _CHARSET         178 (xB2)            1256
     * SHIFTJIS_CHARSET        128 (x80)             932
     * HANGEUL_CHARSET         129 (x81)             949
     * GB2313_CHARSET          134 (x86)             936
     * CHINESEBIG5_CHARSET     136 (x88)             950
     * </pre>
     *
     */
    virtual QCString trRTFansicp()
    { return "65001"; }

    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    { return "1252"; }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    { return "अनुक्रमणिका"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "वर्ग";
      if (!singular)
        result += "गण";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "फ़ाइल";
      if (!singular)
        result += "ें";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "नाम-स्थान";
      if (!singular)
        result += "गण";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "अनुखंड";
      if (!singular)
        result += "गण";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "पृष्ठ";
      if (!singular)
        result += "गण";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "सदस्य";
      if (!singular)
        result += "गण";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "वैश्र्विक";
      if (!singular)
        result += "गण";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "लेखक";
      if (!singular)
        result += "गण";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    { return "संदर्भ"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    { return trWriteList(numEntries) + " लागू करता है।"; }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    { return trWriteList(numEntries) + " में लागू करता है।"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    { return "विषय-सूची"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    { return "पदावनत सूची"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    { return "घटनाएं"; }

    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    { return "घटना दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { return "संकुल प्ररूपगण"; }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    { return "संकुल फलनगण"; }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { return "अचल संकुल फलनगण"; }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    { return "संकुल गुणगण"; }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { return "अचल संकुल गुणगण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    { return "सभी"; }

    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    { return "इस फलन के लिए बुलावा लेखाचित्र यहां दिया गया है:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    { return "खोज के परिणामगण"; }

    /*! This string is put just before listing the search results. The
     *  text can be different depending on the number of documents found.
     *  Inside the text you can put the special marker $num to insert
     *  the number representing the actual number of search results.
     *  The @a numDocuments parameter can be either 0, 1 or 2, where the
     *  value 2 represents 2 or more matches. HTML markup is allowed inside
     *  the returned string.
     */
    virtual QCString trSearchResults(int numDocuments)
    {
      if (numDocuments == 0)
        return "क्षमा करें, आपकी जिज्ञासा से मेल खाने वाला कोई दस्तावेज़ नहीं है।";
      else if (numDocuments == 1)
        return "आपकी जिज्ञासा से मेल खाने वाला <b>1</b> दस्तावेज़ मिला।";
      else
        return "आपकी जिज्ञासा से मेल खाने वाले <b>$num</b> दस्तावेज़ मिले। सर्वश्रेष्ठ मिलान पहले दिखा रहे हैं।";
    }

    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    { return "मिलानगण:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    { return filename + " स्त्रोत फ़ाइल"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "निर्देशिकाएं पदानुक्रम"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "निर्देशिका दस्तावेज़ीकरण"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "निर्देशिकाएं"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName)
    {
      QCString result = dirName;
      result += " निर्देशिका संदर्भ";
      return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "निर्देशिका";
      if (!singular)
        result += "एं";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
    {
      return "यह अतिभारित सदस्य फलन सुविधा के लिए प्रदान किया गया है। यह उपरोक्त"
              " फलन से केवल इस बात में भिन्न है कि यह किस तर्क को स्वीकार करता है।";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "इस फलन के लिए बुलावा लेखाचित्र यहाँ दिया गया है:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "परिगणक दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "सदस्य फलन/उपफलन दस्तावेज़ीकरण"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "आंकड़े प्ररुपगण सूची"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "आंकड़े भागगण"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "यहाँ संक्षिप्त विवरण के साथ आँकड़े प्ररूपगण हैं:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result = "यहाँ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "आंकड़े प्ररूपगण कि लिंको के साथ ";
      if (!extractAll)
        result += "प्रत्येक सदस्य के आंकड़े संरचना दस्तावेज़ीकरण ";
      else
        result += "उनसे संबंधित आंकड़े प्ररूपगण ";
      result += "कि सूची दि गई हैं:";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "आंकड़े प्ररुप अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "आंकड़े प्ररुप दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "फलनगण/उपफलनगण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "फलन/उपफलन दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
    virtual QCString trDataTypes()
    { return "आंकड़े प्ररुपगण"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "अनुखंडगण सूची"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result = "यहाँ संक्षिप्त विवरण के साथ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "अनुखंडगण की सूची दी गई है:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(
      const QCString &clName,
      ClassDef::CompoundType compType,
      bool isTemplate)
    {
      QCString result = clName;
      switch (compType)
      {
        case ClassDef::Class:      result+=" अनुखंड"; break;
        case ClassDef::Struct:     result+=" प्ररुप"; break;
        case ClassDef::Union:      result+=" मिलन"; break;
        case ClassDef::Interface:  result+=" अंतराफलक"; break;
        case ClassDef::Protocol:   result+=" प्रोटोकॉल"; break;
        case ClassDef::Category:   result+=" श्रेणी"; break;
        case ClassDef::Exception:  result+=" अपवाद"; break;
        default: break;
      }
      if (isTemplate)
        result += " टेम्पलेट";
      result += " संदर्भ";
      return result;
    }

    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const QCString &namespaceName)
    {
      QCString result = namespaceName;
      result += " अनुखंड संदर्भ";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "अनुखंडगण सदस्यगण"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    {
      QCString result = "यहाँ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "अनुखंड सदस्यगण कि लिंको के साथ ";
      if (extractAll)
        result += "प्रत्येक सदस्य के दस्तावेज़ीकरण ";
      else
        result += "उनसे संबंधित अनुखंडगण ";
      result += "कि सूची दि गई हैं:";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "अनुखंडगण अनुक्रमणिका"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "अनुखंड";
      if (!singular)
        result += "गण";
      return result;
    }

    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(
      ClassDef::CompoundType compType,
      bool single)
    {
      // single is true implies a single file
      QCString result;
      switch(compType)
      {
        case ClassDef::Class:     result += "अनुखंड"; break;
        case ClassDef::Struct:    result += "प्ररुप"; break;
        case ClassDef::Union:     result += "मिलन"; break;
        case ClassDef::Interface: result += "अंतराफलक"; break;
        case ClassDef::Protocol:  result += "प्रोटोकॉल"; break;
        case ClassDef::Category:  result += "श्रेणी"; break;
        case ClassDef::Exception: result += "अपवाद"; break;
        default: break;
      }
      result += " के लिए दस्तावेज़ीकरण ";
      if (single)
        result += " फ़ाइल से उत्पन्न किया गया था:";
      else
        result += " निम्न फ़ाइलों से उत्पन्न किया गया था:";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "प्ररुप";
      if (!singular)
        result += "गण";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "उपप्रोग्राम";
      if (!singular)
        result += "गण";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    { return "प्ररुप बाध्यताएं"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const QCString &name)
    { return name + " रिश्ता"; }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    { return "लादा जा रहा..."; }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    { return "वैश्र्विक नाम-स्थान"; }

    /*! Message shown while searching */
    virtual QCString trSearching()
    { return "खोजां जा रहा..."; }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    { return "कोई समानता नहीं"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const QCString &name)
    { return name + " में फ़ाइल"; }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name)
    { return name + " में फ़ाइल शामिल है"; }

    /** Compiles a date string.
     *  @param year Year in 4 digits
     *  @param month Month of the year: 1=January
     *  @param day Day of the Month: 1..31
     *  @param dayOfWeek Day of the week: 1=Monday..7=Sunday
     *  @param hour Hour of the day: 0..23
     *  @param minutes Minutes in the hour: 0..59
     *  @param seconds Seconds within the minute: 0..59
     *  @param includeTime Include time in the result string?
     */
    virtual QCString trDateTime(
      int year, int month, int day, int dayOfWeek,
      int hour, int minutes, int seconds, bool includeTime)
    {
      static const char *days[] = { "सोमवार", "मंगलवार", "बुधवार", "गुरुवार",
                                    "शुक्रवार", "शनिवार", "रविवार" };
      static const char *months[] = { "जनवरी", "फरवरी", "मार्च", "अप्रैल", "मई", "जून",
                                    "जुलाई", "अगस्त", "सितम्बर", "अक्टूबर", "नवम्बर", "दिसम्बर" };
      QCString sdate;
      sdate.sprintf("%s %s %d %d", days[dayOfWeek - 1], months[month - 1], day, year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d", hour, minutes, seconds);
        sdate += stime;
      }
      return sdate;
    }
    virtual QCString trDayOfWeek(int dayOfWeek, bool, bool full)
    {
      static const char *days_short[]   = { "सोम.", "मंगल.", "बुध.", "गुरु.", "शुक्र.", "शनि.", "रवि." };
      static const char *days_full[]    = { "सोमवार", "मंगलवार", "बुधवार", "गुरुवार", "शुक्रवार", "शनिवार", "रविवार" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    virtual QCString trMonth(int month, bool, bool full)
    {
      static const char *months_short[] = { "जनवरी", "फरवरी", "मार्च", "अप्रैल", "मई", "जून", "जुलाई", "अगस्त", "सितम्बर", "अक्तूबर", "नवम्बर", "दिसम्बर" };
      static const char *months_full[]  = { "जनवरी", "फरवरी", "मार्च", "अप्रैल", "मई", "जून", "जुलाई", "अगस्त", "सितम्बर", "अक्तूबर", "नवम्बर", "दिसम्बर" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "ग्रन्थसूची"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "कॉपीराइट"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const QCString &name)
    { return name + " के लिए निर्देशिका निर्भरता लेखाचित्र:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "विस्तार स्तर"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "टेम्पलेट मापदंड"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "और " + number + " अधिक..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    {
      QCString result = "इस परिगणक के लिए दस्तावेज़ीकरण निम्न फ़ाइल";
      if (!single)
        result += "ों";
      result += " से उत्पन्न किया गया था:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const QCString &name)
    { return name + " परिगणक संदर्भ"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const QCString &members,const QCString &what)
    { return what + " से विरासत में मिले " + members; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "अतिरिक्त विरासत में मिले सदस्य"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable)
    {
      QCString opt = enable ? "चालू" : "बंद";
      return "पैनल सिंक्रनाइज़ीकरण " + opt + " करने के लिए क्लिक करें";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory()
    { return "श्रेणी @0 द्वारा प्रदान किया गया।"; }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass()
    { return "वर्ग @0 को विस्तार करता है।"; }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    { return "वर्ग विधियाँ"; }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    { return "उदाहरण विधियाँ"; }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation()
    { return "विधि दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces()
    { return "निर्यातीत अंतराफलकगण"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices()
    { return "शामिलीत सेवाएं"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups()
    { return "स्थिर समूहगण"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const QCString &namespaceName)
    {
      QCString result = namespaceName;
      result += " स्थिर समूह संदर्भ";
      return result;
    }

    /** UNO IDL service page title */
    virtual QCString trServiceReference(const QCString &sName)
    {
      QCString result = sName;
      result += " सेवा संदर्भ";
      return result;
    }

    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const QCString &sName)
    {
      QCString result = sName;
      result += " एकल संदर्भ";
      return result;
    }

    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single)
    {
      // single is true implies a single file
      QCString result = "इस सेवा के लिए दस्तावेज़ीकरण निम्न फ़ाइल";
      if (!single)
        result += "ों";
      result += " से उत्पन्न किया गया था:";
      return result;
    }

    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single)
    {
      // single is true implies a single file
      QCString result = "इस एकल के लिए दस्तावेज़ीकरण निम्न फ़ाइल";
      if (!single)
        result += "ों";
      result += " से उत्पन्न किया गया था:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    virtual QCString trDesignUnitHierarchy()
    { return "डिज़ाइन इकाई पदानुक्रम"; }

    /** VHDL design unit list */
    virtual QCString trDesignUnitList()
    { return "डिज़ाइन इकाई सूची"; }

    /** VHDL design unit members */
    virtual QCString trDesignUnitMembers()
    { return "डिज़ाइन इकाई सदस्यगण"; }

    /** VHDL design unit list description */
    virtual QCString trDesignUnitListDescription()
    { return "यहाँ उन सभी डिज़ाइन इकाई सदस्यों की सूची उनसे संबंधित इकाईयों की लिंक के साथ दी गई हैं:"; }

    /** VHDL design unit index */
    virtual QCString trDesignUnitIndex()
    { return "डिज़ाइन इकाई अनुक्रमणिका"; }

    /** VHDL design units */
    virtual QCString trDesignUnits()
    { return "डिज़ाइन इकाईयाँ"; }

    /** VHDL functions/procedures/processes */
    virtual QCString trFunctionAndProc()
    { return "फलनगण/कार्यविधियाँ/प्रक्रियाएं"; }

    /** VHDL type */
    virtual QCString trVhdlType(uint64 type,bool single)
    {
      switch (type)
      {
        case VhdlDocGen::LIBRARY:
          if (single) return "संग्रह";
          else        return "संग्रहगण";
        case VhdlDocGen::PACKAGE:
          if (single) return "संकुल";
          else        return "संकुलगण";
        case VhdlDocGen::SIGNAL:
          if (single) return "संकेत";
          else        return "संकेतगण";
        case VhdlDocGen::COMPONENT:
          if (single) return "अंग";
          else        return "अंगगण";
        case VhdlDocGen::CONSTANT:
          if (single) return "स्थिर";
          else        return "स्थिरगण";
        case VhdlDocGen::ENTITY:
          if (single) return "इकाई";
          else        return "इकाईयाँ";
        case VhdlDocGen::TYPE:
          if (single) return "प्ररूप";
          else        return "प्ररूपगण";
        case VhdlDocGen::SUBTYPE:
          if (single) return "उपप्ररूप";
          else        return "उपप्ररूपगण";
        case VhdlDocGen::FUNCTION:
          if (single) return "फलन";
          else        return "फलनगण";
        case VhdlDocGen::RECORD:
          if (single) return "अभिलेख";
          else        return "अभिलेखगण";
        case VhdlDocGen::PROCEDURE:
          if (single) return "कार्यविधि";
          else        return "कार्यविधियाँ";
        case VhdlDocGen::ARCHITECTURE:
          if (single) return "वास्तुकला";
          else        return "वास्तुकलाएं";
        case VhdlDocGen::ATTRIBUTE:
          if (single) return "तर्क";
          else        return "तर्कगण";
        case VhdlDocGen::PROCESS:
          if (single) return "प्रक्रिया";
          else        return "प्रक्रियाएं";
        case VhdlDocGen::PORT:
          if (single) return "द्वार";
          else        return "द्वारगण";
        case VhdlDocGen::USE:
          if (single) return "प्रयोग खंड";
          else        return "प्रयोग खंडगण";
        case VhdlDocGen::GENERIC:
          if (single) return "सामान्य";
          else        return "सामान्यगण";
        case VhdlDocGen::PACKAGE_BODY:
          return "संकुल शरीर";
        case VhdlDocGen::UNITS:
          return "इकाईयाँ";
        case VhdlDocGen::SHAREDVARIABLE:
          if (single) return "साझाकृत परिवर्तनशील";
          else        return "साझाकृत परिवर्तनशीलगण";
        case VhdlDocGen::VFILE:
          if (single) return "फ़ाइल";
          else        return "फ़ाइलगण";
        case VhdlDocGen::GROUP:
          if (single) return "समूह";
          else        return "समूहगण";
        case VhdlDocGen::INSTANTIATION:
          if (single) return "उदाहरणीकरण";
          else        return "उदाहरणीकरणगण";
        case VhdlDocGen::ALIAS:
          if (single) return "उपनाम";
          else        return "उपनामगण";
        case VhdlDocGen::CONFIG:
          if (single) return "विन्यास";
          else        return "विन्यासगण";
        case VhdlDocGen::MISCELLANEOUS:
          return "विविध";
        case VhdlDocGen::UCF_CONST:
          return "बाध्यताएं";
        default:
          return "वर्ग";
      }
    }

    virtual QCString trCustomReference(const QCString &name)
    { return name + " संदर्भ"; }

    virtual QCString trConstants()
    { return "स्थिरगण"; }

    virtual QCString trConstantDocumentation()
    { return "स्थिर दस्तावेज़ीकरण"; }

    virtual QCString trSequences()
    { return "अनुक्रमगण"; }

    virtual QCString trSequenceDocumentation()
    { return "अनुक्रम दस्तावेज़ीकरण"; }

    virtual QCString trDictionaries()
    { return "शब्दकोशगण"; }

    virtual QCString trDictionaryDocumentation()
    { return "शब्दकोश दस्तावेज़ीकरण"; }

    virtual QCString trSliceInterfaces()
    { return "अंतराफलकगण"; }

    virtual QCString trInterfaceIndex()
    { return "अंतराफलक अनुक्रमणिका"; }

    virtual QCString trInterfaceList()
    { return "अंतराफलक सूची"; }

    virtual QCString trInterfaceListDescription()
    { return "यहाँ संक्षिप्त विवरण के साथ अंतराफलकगण हैं:"; }

    virtual QCString trInterfaceHierarchy()
    { return "अंतराफलक पदानुक्रम"; }

    virtual QCString trInterfaceHierarchyDescription()
    { return "यह अंतराफलक विरासत सूची मोटे तौर पर क्रमबद्ध है, लेकिन पूरी तरह से नहीं, वर्णानुक्रम में:"; }

    virtual QCString trInterfaceDocumentation()
    { return "अंतराफलक दस्तावेज़ीकरण"; }

    virtual QCString trStructs()
    { return "संरचनाएं"; }

    virtual QCString trStructIndex()
    { return "संरचना अनुक्रमणिका"; }

    virtual QCString trStructList()
    { return "संरचना सूची"; }

    virtual QCString trStructListDescription()
    { return "यहाँ संक्षिप्त विवरण के साथ संरचनाएं हैं:"; }

    virtual QCString trStructDocumentation()
    { return "संरचना दस्तावेज़ीकरण"; }

    virtual QCString trExceptionIndex()
    { return "अपवाद अनुक्रमणिका"; }

    virtual QCString trExceptionList()
    { return "अपवाद सूची"; }

    virtual QCString trExceptionListDescription()
    { return "यहाँ संक्षिप्त विवरण के साथ अपवादगण हैं:"; }

    virtual QCString trExceptionHierarchy()
    { return "अपवाद पदानुक्रम"; }

    virtual QCString trExceptionHierarchyDescription()
    { return "यह अपवाद विरासत सूची मोटे तौर पर क्रमबद्ध है, लेकिन पूरी तरह से नहीं, वर्णानुक्रम में:"; }

    virtual QCString trExceptionDocumentation()
    { return "अपवाद दस्तावेज़ीकरण"; }

    virtual QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal)
    {
      QCString result = clName;
      if (isLocal) result += " स्थानीय";
      switch (compType)
      {
        case ClassDef::Class:     result+=" वर्ग"; break;
        case ClassDef::Struct:    result+=" संरचना"; break;
        case ClassDef::Union:     result+=" मिलन"; break;
        case ClassDef::Interface: result+=" अंतराफलक"; break;
        case ClassDef::Protocol:  result+=" प्रोटोकॉल"; break;
        case ClassDef::Category:  result+=" श्रेणी"; break;
        case ClassDef::Exception: result+=" अपवाद"; break;
        default: break;
      }
      result += " संदर्भ";
      return result;
    }

    virtual QCString trOperations()
    { return "कार्यवाहीयाँ"; }

    virtual QCString trOperationDocumentation()
    { return "कार्यवाही दस्तावेज़ीकरण"; }

    virtual QCString trDataMembers()
    { return "आंकड़े सदस्यगण"; }

    virtual QCString trDataMemberDocumentation()
    { return "आंकड़े सदस्य दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    virtual QCString trDesignUnitDocumentation()
    { return "डिज़ाइन इकाई दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    virtual QCString trConcept(bool first_capital, bool singular)
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "संकल्पना";
      if (!singular) result += "एं";
      return result;
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    virtual QCString trConceptReference(const QCString &conceptName)
    {
      QCString result = conceptName;
      result += " संकल्पना संदर्भ";
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    virtual QCString trConceptList()
    { return "संकल्पना सूची"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    virtual QCString trConceptIndex()
    { return "संकल्पना अनुक्रमणिका"; }

    /*! used as the title of chapter containing all information about concepts. */
    virtual QCString trConceptDocumentation()
    { return "संकल्पना दस्तावेज़ीकरण"; }

    /*! used as an introduction to the concept list */
    virtual QCString trConceptListDescription(bool extractAll)
    {
      QCString result = "यहाँ संक्षिप्त विवरण के साथ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "संकल्पनाएं की सूची दी गई है:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    virtual QCString trConceptDefinition()
    { return "संकल्पना परिभाषा"; }
};

#endif
