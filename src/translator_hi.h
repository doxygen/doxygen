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
class TranslatorHindi : public TranslatorAdapter_1_9_4
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    QCString idLanguage() override
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
    QCString latexLanguageSupportCommand() override
    { return ""; }

    QCString latexCommandName() override
    {
      return p_latexCommandName("xelatex");
    }

    QCString trISOLang() override
    { return "hi-IN"; }

    QCString getLanguageString() override
    { return "0x439 Hindi"; }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "संबंधित फलन"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(ध्यान दें कि ये सदस्य फलन नहीं हैं।)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "विस्तृत विवरण"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "विवरण"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "सदस्य प्ररुप-परिभाषा दस्तावेज़ीकरण"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "सदस्य परिगणना दस्तावेज़ीकरण"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "सदस्य फलन दस्तावेज़ीकरण"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "भाग दस्तावेज़ीकरण";
      else
        return "सदस्य आंकड़े दस्तावेज़ीकरण";
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "और..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "सभी सदस्यों की सूची"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "सदस्य सूची"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "यह है सदस्यों की पूरी सूची "; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", सभी विरासत में मिले सदस्यों सहित।"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    {
      QCString result = "स्रोत संकेत-लिपि से ";
      if (!s.isEmpty())
        result += s + " के लिए ";
      result += "डॉक्सिजन द्वारा स्वचालित रूप से उत्पन्न किया गया।";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "परिगणक नाम"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "परिगणक मूल्य"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "में परिभाषित"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "अनुखंडगण"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "वर्ग पदानुक्रम"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े संरचनाएं";
      else
        return "वर्ग सूची";
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "फ़ाइल सूची"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े भागगण";
      else
        return "वर्ग सदस्यगण";
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "वैश्र्विकगण";
      else
        return "फ़ाइल सदस्यगण";
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "संबंधित पृष्ठगण"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "उदाहरणगण"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "खोजें"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
        return "यहाँ वर्गगण की एक पदानुक्रमनुसार सूची दी गई है:";
      else
        return "यह वरासत सूची मोटे तौर पर क्रमबद्ध है, लेकिन पूरी तरह से नहीं, वर्णानुक्रम में:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result = "यहाँ संक्षिप्त विवरण के साथ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "फ़ाइलों की सूची दी गई है:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "यहाँ संक्षिप्त विवरण के साथ आंकड़े संरचनाएँ हैं:";
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
        return "यहाँ संक्षिप्त विवरण के साथ वर्गगण दी गई हैं:";
      else
        return "यहाँ संक्षिप्त विवरण के साथ सभी वर्गगण, संरचनाएं, मिलनगण और अंतराफलकगण की सूची दी गई हैं:";
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
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
    QCString trFileMembersDescription(bool extractAll) override
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
    QCString trExamplesDescription() override
    { return "यहाँ सभी उदाहरणों की एक सूची दी गई है:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "यहाँ सभी संबंधित दस्तावेज़ीकरण पृष्ठों की सूची दी गई है:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "यहाँ सभी अनुखंडों की एक सूची है:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "दस्तावेज़ीकरण"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "अनुखंड अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "पदानुक्रमनुसार अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े संरचना अनुक्रमणिका";
      else
        return "वर्ग अनुक्रमणिका";
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "फ़ाइल अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "अनुखंड दस्तावेज़ीकरण"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "फ़ाइल दस्तावेज़ीकरण"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "संदर्भ पुस्तिका"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "मैक्रोगण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "प्ररुप-परिभाषाएं"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "परिगणकगण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "फलनगण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "परिवर्तनशीलगण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "परिगणक"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "मैक्रो परिभाषा दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "प्ररुप-परिभाषा दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "परिगणना प्ररूप दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "फलन दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "परिवर्तनशील दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े संरचनाएं";
      else
        return "वर्गगण";
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result;
      if (!projName.isEmpty())
        result += projName + " के लिए ";
      result += date + " पर उतपन्न किया गया: ";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    { return clName + " के लिए वरासत आरेख:"; }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "चेतावनी"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "संस्करण"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "दिनांक"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "वापसी"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "यह भी देखें"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "मापदंडगण"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "अपवादगण"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "द्वारा उत्पन्न"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "नाम-स्थान सूची"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
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
    QCString trFriends() override
    { return "मित्रगण"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "मित्रगण और संबंधित फलन दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(
      const QCString &clName,
      ClassDef::CompoundType compType,
      bool isTemplate) override
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
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result = fileName;
      result += " फ़ाइल संदर्भ";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result = namespaceName;
      result += " नाम-स्थान संदर्भ";
      return result;
    }

    QCString trPublicMembers() override
    { return "सार्वजनिक सदस्य फलनगण"; }

    QCString trPublicSlots() override
    { return "सार्वजनिक खांचें"; }

    QCString trSignals() override
    { return "संकेतगण"; }

    QCString trStaticPublicMembers() override
    { return "अचल सार्वजनिक सदस्य फलनगण"; }

    QCString trProtectedMembers() override
    { return "संरक्षित सदस्य फलनगण"; }

    QCString trProtectedSlots() override
    { return "संरक्षित खांचे"; }

    QCString trStaticProtectedMembers() override
    { return "अचल संरक्षित सदस्य फलनगण"; }

    QCString trPrivateMembers() override
    { return "निजी सदस्य फलनगण"; }

    QCString trPrivateSlots() override
    { return "निजी खांचें"; }

    QCString trStaticPrivateMembers() override
    { return "अचल निजी सदस्य फलनगण"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries) override
    {
      QCString result;
      // the inherits list contain `numEntries' classes
      for (int i = 0; i < numEntries; ++i)
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
    QCString trInheritsList(int numEntries) override
    { return trWriteList(numEntries) + " से विरासित।"; }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    { return trWriteList(numEntries) + " द्वारा विरासित।"; }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    { return trWriteList(numEntries) + " से पुन: लागू किया गया।"; }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    { return trWriteList(numEntries) + " में पुन: लागू किया गया।"; }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "नाम-स्थान सदस्यगण"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "नाम-स्थान अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "नाम-स्थान दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "नाम-स्थानगण"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    {
      // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
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
    QCString trReturnValues() override
    { return "वापसी मान"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "मुख्य पृष्ठ"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "पृ."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    { return "फ़ाइल @1 की लाइन @0 पर परिभाषित।"; }

    QCString trDefinedInSourceFile() override
    { return "फ़ाइल @0 में परिभाषित।"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    { return "पदावनत"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    { return clName + " के लिए सहयोगीकरण आरेख:"; }

    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    { return fName + " के लिए निर्भरता लेखाचित्र शामिल करें:"; }

    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    { return "निर्माता और नाशक दस्तावेज़ीकरण"; }

    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    { return "इस फाइल कि स्त्रोत संकेत-लिपि को देखें।"; }

    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    { return "इस फ़ाइल के दस्तावेज़ीकरण पर जाएं।"; }

    /*! Text for the \\pre command */
    QCString trPrecondition() override
    { return "पूर्वशर्त"; }

    /*! Text for the \\post command */
    QCString trPostcondition() override
    { return "शर्तपश्चात्"; }

    /*! Text for the \\invariant command */
    QCString trInvariant() override
    { return "अपरिवर्तनीय"; }

    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    { return "प्रारंभिक मूल्य:"; }

    /*! Text used the source code in the file index */
    QCString trCode() override
    { return "संकेत-लिपि"; }

    QCString trGraphicalHierarchy() override
    { return "चित्रात्मक वर्ग पदानुक्रम"; }

    QCString trGotoGraphicalHierarchy() override
    { return "चित्रात्मक वर्ग पदानुक्रम पर जाएँ"; }

    QCString trGotoTextualHierarchy() override
    { return "पाठ्य वर्ग पदानुक्रम पर जाएँ"; }

    QCString trPageIndex() override
    { return "पृष्ठ अनुक्रमणिका"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    { return "ध्यान दें"; }

    QCString trPublicTypes() override
    { return "सार्वजनिक प्ररुपगण"; }

    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े भागगण";
      else
        return "सार्वजनिक गुणगण";
    }

    QCString trStaticPublicAttribs() override
    { return "अचल सार्वजनिक गुणगण"; }

    QCString trProtectedTypes() override
    { return "संरक्षित प्ररुपगण"; }

    QCString trProtectedAttribs() override
    { return "संरक्षित गुणगण"; }

    QCString trStaticProtectedAttribs() override
    { return "अचल संरक्षित गुणगण"; }

    QCString trPrivateTypes() override
    { return "निजी प्ररुपगण"; }

    QCString trPrivateAttribs() override
    { return "निजी गुणगण"; }

    QCString trStaticPrivateAttribs() override
    { return "अचल निजी गुणगण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    { return "करने के लिए"; }

    /*! Used as the header of the todo list */
    QCString trTodoList() override
    { return "करने के लिए सूची"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    { return "द्वारा संदर्भित"; }

    QCString trRemarks() override
    { return "टिप्पणियाँ"; }

    QCString trAttention() override
    { return "ध्यान"; }

    QCString trInclByDepGraph() override
    { return "यह लेखाचित्र दिखाता है कि कौन सी फ़ाइलें प्रत्यक्ष या परोक्ष रूप से इस फ़ाइल को शामिल करती हैं:"; }

    QCString trSince() override
    { return "जबसे"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    { return "लेखाचित्र किंवदंती"; }

    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
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
    QCString trLegend() override
    { return "किंवदंती"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    { return "परीक्षा"; }
    /*! Used as the header of the test list */
    QCString trTestList() override
    { return "परीक्षा सूची"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    { return "संपत्तियाँ"; }

    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    { return "संपत्ति दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        return "आंकड़े संरचनाएं";
      else
        return "वर्गगण";
    }

    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    { return "संकुल " + name; }

    /*! The description of the package index page */
    QCString trPackageListDescription() override
    { return "यहाँ संक्षिप्त विवरण के साथ संकुल दिए गए हैं (यदि उपलब्ध हो):"; }

    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    { return "संकुलगण"; }

    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    { return "मूल्य:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    { return "त्रुटि"; }

    /*! Used as the header of the bug list */
    QCString trBugList() override
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
    QCString trRTFansicp() override
    { return "65001"; }

    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    { return "1252"; }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    { return "अनुक्रमणिका"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool /* first_capital */, bool singular) override
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
    QCString trFile(bool /* first_capital */, bool singular) override
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
    QCString trNamespace(bool /* first_capital */, bool singular) override
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
    QCString trGroup(bool /* first_capital */, bool singular) override
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
    QCString trPage(bool /* first_capital */, bool singular) override
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
    QCString trMember(bool /* first_capital */, bool singular) override
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
    QCString trGlobal(bool /* first_capital */, bool singular) override
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
    QCString trAuthor(bool /* first_capital */, bool singular) override
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
    QCString trReferences() override
    { return "संदर्भ"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    { return trWriteList(numEntries) + " लागू करता है।"; }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    { return trWriteList(numEntries) + " में लागू करता है।"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    { return "विषय-सूची"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    { return "पदावनत सूची"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    { return "घटनाएं"; }

    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    { return "घटना दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    { return "संकुल प्ररूपगण"; }

    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    { return "संकुल फलनगण"; }
    QCString trPackageMembers() override
    { return "संकुल सदस्यगण"; }

    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    { return "अचल संकुल फलनगण"; }

    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    { return "संकुल गुणगण"; }

    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    { return "अचल संकुल गुणगण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    { return "सभी"; }

    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    { return "इस फलन के लिए बुलावा लेखाचित्र यहां दिया गया है:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    { return "खोज के परिणामगण"; }

    /*! This string is put just before listing the search results. The
     *  text can be different depending on the number of documents found.
     *  Inside the text you can put the special marker $num to insert
     *  the number representing the actual number of search results.
     *  The @a numDocuments parameter can be either 0, 1 or 2, where the
     *  value 2 represents 2 or more matches. HTML markup is allowed inside
     *  the returned string.
     */
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    { return "मिलानगण:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    { return filename + " स्त्रोत फ़ाइल"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "निर्देशिकाएं पदानुक्रम"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "निर्देशिका दस्तावेज़ीकरण"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "निर्देशिकाएं"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    {
      QCString result = dirName;
      result += " निर्देशिका संदर्भ";
      return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    QCString trDir(bool /* first_capital */, bool singular) override
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
    QCString trOverloadText() override
    {
      return "यह अतिभारित सदस्य फलन सुविधा के लिए प्रदान किया गया है। यह उपरोक्त"
              " फलन से केवल इस बात में भिन्न है कि यह किस तर्क को स्वीकार करता है।";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "इस फलन के लिए बुलावा लेखाचित्र यहाँ दिया गया है:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "परिगणक दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "सदस्य फलन/उपफलन दस्तावेज़ीकरण"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "आंकड़े प्ररुपगण सूची"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "आंकड़े भागगण"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "यहाँ संक्षिप्त विवरण के साथ आँकड़े प्ररूपगण हैं:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "आंकड़े प्ररुप अनुक्रमणिका"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "आंकड़े प्ररुप दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "फलनगण/उपफलनगण"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "फलन/उपफलन दस्तावेज़ीकरण"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
    QCString trDataTypes() override
    { return "आंकड़े प्ररुपगण"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "अनुखंडगण सूची"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result = "यहाँ संक्षिप्त विवरण के साथ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "अनुखंडगण की सूची दी गई है:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(
      const QCString &clName,
      ClassDef::CompoundType compType,
      bool isTemplate) override
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
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result = namespaceName;
      result += " अनुखंड संदर्भ";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "अनुखंडगण सदस्यगण"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "अनुखंडगण अनुक्रमणिका"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool /* first_capital */, bool singular) override
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
    QCString trGeneratedFromFilesFortran(
      ClassDef::CompoundType compType,
      bool single) override
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
    QCString trType(bool /* first_capital */, bool singular) override
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
    QCString trSubprogram(bool /* first_capital */, bool singular) override
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "उपप्रोग्राम";
      if (!singular)
        result += "गण";
      return result;
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    { return "प्ररुप बाध्यताएं"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    { return name + " रिश्ता"; }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    { return "लादा जा रहा..."; }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    { return "वैश्र्विक नाम-स्थान"; }

    /*! Message shown while searching */
    QCString trSearching() override
    { return "खोजां जा रहा..."; }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    { return "कोई समानता नहीं"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    QCString trFileIn(const QCString &name) override
    { return name + " में फ़ाइल"; }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(
      int year, int month, int day, int dayOfWeek,
      int hour, int minutes, int seconds, DateTimeType includeTime) override
    {
      static const char *days[] = { "सोमवार", "मंगलवार", "बुधवार", "गुरुवार",
                                    "शुक्रवार", "शनिवार", "रविवार" };
      static const char *months[] = { "जनवरी", "फरवरी", "मार्च", "अप्रैल", "मई", "जून",
                                    "जुलाई", "अगस्त", "सितम्बर", "अक्टूबर", "नवम्बर", "दिसम्बर" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %s %d %d", days[dayOfWeek - 1], months[month - 1], day, year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        QCString stime;
        stime.sprintf("%.2d:%.2d:%.2d", hour, minutes, seconds);
        sdate += stime;
      }
      return sdate;
    }
    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "सोम.", "मंगल.", "बुध.", "गुरु.", "शुक्र.", "शनि.", "रवि." };
      static const char *days_full[]    = { "सोमवार", "मंगलवार", "बुधवार", "गुरुवार", "शुक्रवार", "शनिवार", "रविवार" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "जनवरी", "फरवरी", "मार्च", "अप्रैल", "मई", "जून", "जुलाई", "अगस्त", "सितम्बर", "अक्तूबर", "नवम्बर", "दिसम्बर" };
      static const char *months_full[]  = { "जनवरी", "फरवरी", "मार्च", "अप्रैल", "मई", "जून", "जुलाई", "अगस्त", "सितम्बर", "अक्तूबर", "नवम्बर", "दिसम्बर" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "am", "pm" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "ग्रन्थसूची"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "कॉपीराइट"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return name + " के लिए निर्देशिका निर्भरता लेखाचित्र:"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "विस्तार स्तर"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "टेम्पलेट मापदंड"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "और " + number + " अधिक..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    {
      QCString result = "इस परिगणक के लिए दस्तावेज़ीकरण निम्न फ़ाइल";
      if (!single)
        result += "ों";
      result += " से उत्पन्न किया गया था:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return name + " परिगणक संदर्भ"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return what + " से विरासत में मिले " + members; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "अतिरिक्त विरासत में मिले सदस्य"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "चालू" : "बंद";
      return "पैनल सिंक्रनाइज़ीकरण " + opt + " करने के लिए क्लिक करें";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    { return "श्रेणी @0 द्वारा प्रदान किया गया।"; }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    { return "वर्ग @0 को विस्तार करता है।"; }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    { return "वर्ग विधियाँ"; }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    { return "उदाहरण विधियाँ"; }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    { return "विधि दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "निर्यातीत अंतराफलकगण"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "शामिलीत सेवाएं"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "स्थिर समूहगण"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result = namespaceName;
      result += " स्थिर समूह संदर्भ";
      return result;
    }

    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result = sName;
      result += " सेवा संदर्भ";
      return result;
    }

    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result = sName;
      result += " एकल संदर्भ";
      return result;
    }

    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result = "इस सेवा के लिए दस्तावेज़ीकरण निम्न फ़ाइल";
      if (!single)
        result += "ों";
      result += " से उत्पन्न किया गया था:";
      return result;
    }

    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
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
    QCString trDesignUnitHierarchy() override
    { return "डिज़ाइन इकाई पदानुक्रम"; }

    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "डिज़ाइन इकाई सूची"; }

    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "डिज़ाइन इकाई सदस्यगण"; }

    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    { return "यहाँ उन सभी डिज़ाइन इकाई सदस्यों की सूची उनसे संबंधित इकाईयों की लिंक के साथ दी गई हैं:"; }

    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "डिज़ाइन इकाई अनुक्रमणिका"; }

    /** VHDL design units */
    QCString trDesignUnits() override
    { return "डिज़ाइन इकाईयाँ"; }

    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "फलनगण/कार्यविधियाँ/प्रक्रियाएं"; }

    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch (type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "संग्रह";
          else        return "संग्रहगण";
        case VhdlSpecifier::PACKAGE:
          if (single) return "संकुल";
          else        return "संकुलगण";
        case VhdlSpecifier::SIGNAL:
          if (single) return "संकेत";
          else        return "संकेतगण";
        case VhdlSpecifier::COMPONENT:
          if (single) return "अंग";
          else        return "अंगगण";
        case VhdlSpecifier::CONSTANT:
          if (single) return "स्थिर";
          else        return "स्थिरगण";
        case VhdlSpecifier::ENTITY:
          if (single) return "इकाई";
          else        return "इकाईयाँ";
        case VhdlSpecifier::TYPE:
          if (single) return "प्ररूप";
          else        return "प्ररूपगण";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "उपप्ररूप";
          else        return "उपप्ररूपगण";
        case VhdlSpecifier::FUNCTION:
          if (single) return "फलन";
          else        return "फलनगण";
        case VhdlSpecifier::RECORD:
          if (single) return "अभिलेख";
          else        return "अभिलेखगण";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "कार्यविधि";
          else        return "कार्यविधियाँ";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "वास्तुकला";
          else        return "वास्तुकलाएं";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "तर्क";
          else        return "तर्कगण";
        case VhdlSpecifier::PROCESS:
          if (single) return "प्रक्रिया";
          else        return "प्रक्रियाएं";
        case VhdlSpecifier::PORT:
          if (single) return "द्वार";
          else        return "द्वारगण";
        case VhdlSpecifier::USE:
          if (single) return "प्रयोग खंड";
          else        return "प्रयोग खंडगण";
        case VhdlSpecifier::GENERIC:
          if (single) return "सामान्य";
          else        return "सामान्यगण";
        case VhdlSpecifier::PACKAGE_BODY:
          return "संकुल शरीर";
        case VhdlSpecifier::UNITS:
          return "इकाईयाँ";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "साझाकृत परिवर्तनशील";
          else        return "साझाकृत परिवर्तनशीलगण";
        case VhdlSpecifier::VFILE:
          if (single) return "फ़ाइल";
          else        return "फ़ाइलगण";
        case VhdlSpecifier::GROUP:
          if (single) return "समूह";
          else        return "समूहगण";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "उदाहरणीकरण";
          else        return "उदाहरणीकरणगण";
        case VhdlSpecifier::ALIAS:
          if (single) return "उपनाम";
          else        return "उपनामगण";
        case VhdlSpecifier::CONFIG:
          if (single) return "विन्यास";
          else        return "विन्यासगण";
        case VhdlSpecifier::MISCELLANEOUS:
          return "विविध";
        case VhdlSpecifier::UCF_CONST:
          return "बाध्यताएं";
        default:
          return "वर्ग";
      }
    }

    QCString trCustomReference(const QCString &name) override
    { return name + " संदर्भ"; }

    QCString trConstants() override
    { return "स्थिरगण"; }

    QCString trConstantDocumentation() override
    { return "स्थिर दस्तावेज़ीकरण"; }

    QCString trSequences() override
    { return "अनुक्रमगण"; }

    QCString trSequenceDocumentation() override
    { return "अनुक्रम दस्तावेज़ीकरण"; }

    QCString trDictionaries() override
    { return "शब्दकोशगण"; }

    QCString trDictionaryDocumentation() override
    { return "शब्दकोश दस्तावेज़ीकरण"; }

    QCString trSliceInterfaces() override
    { return "अंतराफलकगण"; }

    QCString trInterfaceIndex() override
    { return "अंतराफलक अनुक्रमणिका"; }

    QCString trInterfaceList() override
    { return "अंतराफलक सूची"; }

    QCString trInterfaceListDescription() override
    { return "यहाँ संक्षिप्त विवरण के साथ अंतराफलकगण हैं:"; }

    QCString trInterfaceHierarchy() override
    { return "अंतराफलक पदानुक्रम"; }

    QCString trInterfaceHierarchyDescription() override
    { return "यह अंतराफलक विरासत सूची मोटे तौर पर क्रमबद्ध है, लेकिन पूरी तरह से नहीं, वर्णानुक्रम में:"; }

    QCString trInterfaceDocumentation() override
    { return "अंतराफलक दस्तावेज़ीकरण"; }

    QCString trStructs() override
    { return "संरचनाएं"; }

    QCString trStructIndex() override
    { return "संरचना अनुक्रमणिका"; }

    QCString trStructList() override
    { return "संरचना सूची"; }

    QCString trStructListDescription() override
    { return "यहाँ संक्षिप्त विवरण के साथ संरचनाएं हैं:"; }

    QCString trStructDocumentation() override
    { return "संरचना दस्तावेज़ीकरण"; }

    QCString trExceptionIndex() override
    { return "अपवाद अनुक्रमणिका"; }

    QCString trExceptionList() override
    { return "अपवाद सूची"; }

    QCString trExceptionListDescription() override
    { return "यहाँ संक्षिप्त विवरण के साथ अपवादगण हैं:"; }

    QCString trExceptionHierarchy() override
    { return "अपवाद पदानुक्रम"; }

    QCString trExceptionHierarchyDescription() override
    { return "यह अपवाद विरासत सूची मोटे तौर पर क्रमबद्ध है, लेकिन पूरी तरह से नहीं, वर्णानुक्रम में:"; }

    QCString trExceptionDocumentation() override
    { return "अपवाद दस्तावेज़ीकरण"; }

    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
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

    QCString trOperations() override
    { return "कार्यवाहीयाँ"; }

    QCString trOperationDocumentation() override
    { return "कार्यवाही दस्तावेज़ीकरण"; }

    QCString trDataMembers() override
    { return "आंकड़े सदस्यगण"; }

    QCString trDataMemberDocumentation() override
    { return "आंकड़े सदस्य दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    { return "डिज़ाइन इकाई दस्तावेज़ीकरण"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool /* first_capital */, bool singular) override
    {
      // There is no first-letter capitalization notion in Hindi.
      QCString result = "संकल्पना";
      if (!singular) result += "एं";
      return result;
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result = conceptName;
      result += " संकल्पना संदर्भ";
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "संकल्पना सूची"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "संकल्पना अनुक्रमणिका"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "संकल्पना दस्तावेज़ीकरण"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result = "यहाँ संक्षिप्त विवरण के साथ सभी ";
      if (!extractAll)
        result += "दस्तावेज़ीकृत ";
      result += "संकल्पनाएं की सूची दी गई है:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    { return "संकल्पना परिभाषा"; }
};

#endif
