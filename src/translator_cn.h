/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2005 by Dimitri van Heesch.
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

#ifndef TRANSLATOR_CN_H
#define TRANSLATOR_CN_H

/*!
   If you want insert a space whenever Chinese meets English charactors, set
   CN_SPC to " ", else null.
*/
#define CN_SPC

class TranslatorChinese : public Translator
{
  public:
		/*! Used for identification of the language. The identification
		 * should not be translated. It should be replaced by the name
		 * of the language in English using lower-case characters only
		 * (e.g. "czech", "japanese", "russian", etc.). It sould be equal to
		 * the identification used in the language.cpp.
		 */
    virtual QCString idLanguage()
    { return "chinese"; }

    /*! Used to get the LaTeX command(s) for the language support. 
     * This method should return string with commands that switch
     * LaTeX to the desired language.  For example 
     * <pre>"\\usepackage[german]{babel}\n"
     * </pre>
     * or
     * <pre>"\\usepackage{polski}\n"
     * "\\usepackage[latin2]{inputenc}\n"
     * "\\usepackage[T1]{fontenc}\n"
     * </pre>
     * 
     * The English LaTeX does not use such commands.  Because of this
     * the empty string is returned in this implementation.
     */
    virtual QCString latexLanguageSupportCommand()
    {
      return "";
    }


		/*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    { return "gb2312"; }

		/*! used in the compound documentation before a list of related functions. 
		 */
    virtual QCString trRelatedFunctions()
    { return "Ïà¹Øº¯Êı"; }

		/*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "£¨×¢Òâ£ºÕâĞ©²»ÊÇ³ÉÔ±º¯Êı¡££©"; }

		/*! header that is put before the detailed description of files, 
		 * classes and namespaces. 
		 */
    virtual QCString trDetailedDescription()
    { return "ÏêÏ¸ÃèÊö"; }

		/*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "³ÉÔ±ÀàĞÍ¶¨ÒåÎÄµµ"; }

		/*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "³ÉÔ±Ã¶¾ÙÀàĞÍÎÄµµ"; }

    virtual QCString trEnumerationValueDocumentation()
    { return "³ÉÔ±Ã¶¾ÙÖµÎÄµµ"; }

		/*! header that is put before the list of member function. */
    virtual QCString trMemberFunctionDocumentation()
    { return "³ÉÔ±º¯ÊıÎÄµµ"; }

		/*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) 
			{
				return "×Ö¶ÎÎÄµµ"; 
			}
			else {
				return "³ÉÔ±Êı¾İÎÄµµ"; 
			}
		}

		/*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "¸ü¶à..."; }

		/*! put in the class documention */
    virtual QCString trListOfAllMembers()
    { return "ËùÓĞ³ÉÔ±µÄÁĞ±í¡£"; }

		/*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "³ÉÔ±ÁĞ±í"; }

		/*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "³ÉÔ±µÄÍêÕûÁĞ±í£¬ÕâĞ©³ÉÔ±ÊôÓÚ"CN_SPC; }

		/*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return "£¬°üÀ¨ËùÓĞ¼Ì³Ğ¶øÀ´µÄ³ÉÔ±"; }

		/*! this is put at the author sections at the bottom of man pages.
		 *  parameter s is name of the project name.
		 */
    virtual QCString trGeneratedAutomatically(const char *s)
    {
			QCString result;
      if (s) result=(QCString)"Îª"CN_SPC+s+"£¬";
      result+="ÓÉ"CN_SPC"Doyxgen"CN_SPC"Í¨¹ı·ÖÎöÔ´´úÂë×Ô¶¯Éú³É¡£"; 
      return result;
    }

		/*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "Ã¶¾ÙÃû³Æ"; }

		/*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "Ã¶¾ÙÖµ"; }

		/*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "¶¨ÒåÓÚ"CN_SPC; }

		// quick reference sections


		/*! This is put above each page as a link to the list of all groups of 
		 *  compounds or files (see the \\group command).
		 */
    virtual QCString trModules()
    { return "Ä£¿é"; }

		/*! This is put above each page as a link to the class hierarchy */
 		virtual QCString trClassHierarchy()
    { return "Àà¼Ì³Ğ¹ØÏµ"; }

		/*! This is put above each page as a link to the list of annotated class */
 		virtual QCString trCompoundList()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
				return "Êı¾İ½á¹¹"; 
			}
			else {
				return "×éºÏÀàĞÍÁĞ±í"; 
			}
		}

		/*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "ÎÄ¼şÁĞ±í"; }


		/*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return "Í·ÎÄ¼ş"; }

		/*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) { 
				return "Êı¾İ×Ö¶Î"; 
			}
			else {
				return "×éºÏÀàĞÍ³ÉÔ±"; 
			}
		}

		/*! This is put above each page as a link to all member of files. */
    virtual QCString trFileMembers()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
				return "È«¾Ö¶¨Òå"; 
			} else {
				return "ÎÄ¼ş³ÉÔ±"; 
			}
		}

		/*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Ïà¹ØÒ³Ãæ"; }

		/*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Ê¾Àı"; }

    virtual QCString trSearch()
    { return "ËÑË÷"; }

    virtual QCString trClassHierarchyDescription()
    { return "´ËÁĞ±í»ù±¾°´×ÖµäË³ĞòÅÅĞò£º"; }

    virtual QCString trFileListDescription(bool extractAll)
    {
   		QCString result="ÕâÀïÁĞ³öËùÓĞ";
      if (!extractAll) result+="ÎÄµµ»¯µÄ";
      result+="ÎÄ¼ş£¬¸½´ø¼òÒªËµÃ÷£º";
      return result;
    }

    virtual QCString trCompoundListDescription()
    { 

			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{
				return "ÕâÀïÁĞ³öËùÓĞÊı¾İ½á¹¹£¬¸½´ø¼òÒªËµÃ÷£º"; 
			}
			else
			{
				return "ÕâÀïÁĞ³öËùÓĞÀà¡¢½á¹¹¡¢ÁªºÏÒÔ¼°½Ó¿Ú¶¨Òå£¬¸½´ø¼òÒªËµÃ÷£º"; 
			}
    }

    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
   		QCString result="ÕâÀïÁĞ³öËùÓĞ";
      if (!extractAll) {
				result+="ÎÄµµ»¯µÄ";
			}
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
				result+="½á¹¹ºÍÁªºÏµÄ³ÉÔ±£¬¸½´ø";
			}
			else {
				result+="Àà³ÉÔ±£¬¸½´ø";
			}

      if (extractAll) {
				if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
					result+="ËùÔÚ½á¹¹£¯ÁªºÏµÄÎÄµµµÄÁ´½Ó£º";
				}
				else {
					result+="ËùÔÚÀàµÄÎÄµµµÄÁ´½Ó£º";
				}
			}
      else {
				if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
					result+="ËùÊô½á¹¹£¯ÁªºÏµÄÁ´½Ó£º";
				}
				else {
					result+="ËùÊôÀàµÄÁ´½Ó£º";
				}
			}
      return result;
    }

    virtual QCString trFileMembersDescription(bool extractAll)
    {
   		QCString result="ÕâÀïÁĞ³öËùÓĞ";
      if (!extractAll) 
				result +="ÎÄµµ»¯µÄ";

			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C")) {
				result +="º¯Êı£¬±äÁ¿£¬ºê£¬Ã¶¾ÙºÍÀàĞÍ¶¨Òå£¬¸½´ø";
			} 
			else {
				result +="ÎÄ¼ş³ÉÔ±£¬¸½´ø";
			}

      if (extractAll) 
				result+="ËùÔÚÎÄ¼şµÄÎÄµµµÄÁ´½Ó£º";
      else 
				result+="ËùÔÚÎÄ¼şµÄÁ´½Ó£º";
      return result;
    }

    virtual QCString trHeaderFilesDescription()
    { return "ÕâÀïÁĞ³ö×é³ÉAPIµÄÍ·ÎÄ¼ş£º"; }

    virtual QCString trExamplesDescription()
    { return "ÕâÀïÁĞ³öËùÓĞÊ¾Àı£º"; }

    virtual QCString trRelatedPagesDescription()
    { return "ÕâÀïÁĞ³öËùÓĞÏà¹ØµÄÒ³Ãæ£º"; }

    virtual QCString trModulesDescription()
    { return "ÕâÀïÁĞ³öËùÓĞÄ£¿é"; }

    virtual QCString trNoDescriptionAvailable()
    { return "ÎŞ¿ÉÓÃÎÄµµ"; }

    virtual QCString trDocumentation()
    { return "ÎÄµµ"; }

    virtual QCString trModuleIndex()
    { return "Ä£¿éË÷Òı"; }

    virtual QCString trHierarchicalIndex()
    { return "¼Ì³Ğ¹ØÏµË÷Òı"; }

    virtual QCString trCompoundIndex()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{
				return "Êı¾İ½á¹¹Ë÷Òı"; 
			} 
			else {
				return "×éºÏÀàĞÍË÷Òı"; 
			}
		}

    virtual QCString trFileIndex() 
    { return "ÎÄ¼şË÷Òı"; }

    virtual QCString trModuleDocumentation()
    { return "Ä£¿éÎÄµµ"; }

    virtual QCString trClassDocumentation()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{
				return "Êı¾İ½á¹¹ÎÄµµ";
			}
			else {
				return "ÀàÎÄµµ"; 
			}
		}

    virtual QCString trFileDocumentation()
    { return "ÎÄ¼şÎÄµµ"; }

    virtual QCString trExampleDocumentation()
    { return "Ê¾ÀıÎÄµµ"; }

    virtual QCString trPageDocumentation()
    { return "Ò³ÃæÎÄµµ"; }

    virtual QCString trReferenceManual()
    { return "²Î¿¼ÊÖ²á"; }

    virtual QCString trDefines()
    { return "ºê¶¨Òå"; }

    virtual QCString trFuncProtos()
    { return "º¯ÊıÔ­ĞÍ"; }

    virtual QCString trTypedefs()
    { return "ÀàĞÍ¶¨Òå"; }

    virtual QCString trEnumerations()
    { return "Ã¶¾Ù"; }

    virtual QCString trFunctions()
    { return "º¯Êı"; }

    virtual QCString trVariables()
    { return "±äÁ¿"; }

    virtual QCString trEnumerationValues()
    { return "Ã¶¾ÙÖµ"; }


 		virtual QCString trDefineDocumentation()
    { return "ºê¶¨ÒåÎÄµµ"; }

 		virtual QCString trFunctionPrototypeDocumentation()
    { return "º¯ÊıÔ­ĞÍÎÄµµ"; }

 		virtual QCString trTypedefDocumentation()
    { return "ÀàĞÍ¶¨ÒåÎÄµµ"; }

 		virtual QCString trEnumerationTypeDocumentation()
    { return "Ã¶¾ÙÀàĞÍÎÄµµ"; }

 		virtual QCString trFunctionDocumentation()
    { return "º¯ÊıÎÄµµ"; }

 		virtual QCString trVariableDocumentation()
    { return "±äÁ¿ÎÄµµ"; }

 		virtual QCString trCompounds()
    { 
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{ 
				return "Êı¾İ½á¹¹";
			}
			else {
				return "×éºÏÀàĞÍ"; 
			}
		}

 		virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
   		QCString result=(QCString)"Generated at "+date;
      if (projName) result+=(QCString)" for "+projName;
      result+=(QCString)" by";
      return result;
    }

 		virtual QCString trWrittenBy()
    {
      return "written by";
    }

 		virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"¼Ì³ĞÍ¼£¬Àà"CN_SPC+clName;
    }

 		virtual QCString trForInternalUseOnly()
    { return "½öÏŞÄÚ²¿Ê¹ÓÃ¡£"; }

 		virtual QCString trReimplementedForInternalReasons()
    { return "ÓÉÓÚÄÚ²¿Ô­Òò±»ÖØÔØ£»µ«²»Ó°ÏìAPI"; 
    }

 		virtual QCString trWarning()
    { return "¾¯¸æ"; }

 		virtual QCString trBugsAndLimitations()
    { return "BUG"CN_SPC"Óë¾ÖÏŞ"; }

 		virtual QCString trVersion()
    { return "°æ±¾"; }

 		virtual QCString trDate()
    { return "ÈÕÆÚ"; }

 		virtual QCString trReturns()
    { return "·µ»Ø"; }

 		virtual QCString trSeeAlso()
    { return "²Î¼û"; }

 		virtual QCString trParameters()
    { return "²ÎÊı"; }

 		virtual QCString trExceptions()
    { return "Òì³£"; }

 		virtual QCString trGeneratedBy()
    { return "ÖÆ×÷Õß"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
 		virtual QCString trNamespaceList()
    { return "Ãû×Ö¿Õ¼äÁĞ±í"; }

 		virtual QCString trNamespaceListDescription(bool extractAll)
    {
   		QCString result="ÕâÀïÁĞ³öËùÓĞ";
      if (!extractAll) result+="ÎÄµµ»¯µÄ";
      result+="Ãû×Ö¿Õ¼ä¶¨Òå£¬¸½´ø¼òÒªËµÃ÷£º";
      return result;
    }

 		virtual QCString trFriends()
    { return "ÓÑÔª"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
 		virtual QCString trRelatedFunctionDocumentation()
    { return "ÓÑÔª¼°Ïà¹Øº¯ÊıÎÄµµ"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

 		virtual QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
      // used as the title of the HTML page of a class/struct/union
    {
   		QCString result=(QCString)clName;
      if (isTemplate) result+=CN_SPC"Ä£°å";
      switch(compType)
      {
        case ClassDef::Class:  result+="Àà"; break;
        case ClassDef::Struct: result+="½á¹¹"; break;
        case ClassDef::Union:  result+="ÁªºÏ"; break;
        case ClassDef::Interface:  result+="½Ó¿Ú"; break;
        case ClassDef::Protocol:   result+="Ğ­Òé"; break; // translate me!
        case ClassDef::Category:   result+="·ÖÀà"; break; // translate me!
        case ClassDef::Exception:  result+="Òì³£"; break;
      }
      result+="²Î¿¼";
      return result;
    }

 		virtual QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
   		QCString result=fileName;
      result+=CN_SPC"ÎÄ¼ş²Î¿¼"; 
      return result;
    }

 		virtual QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
   		QCString result=namespaceName;
      result+=CN_SPC"Ãû×Ö¿Õ¼ä²Î¿¼";
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
 		virtual QCString trPublicMembers()
    { return "¹«ÓĞ³ÉÔ±"; }

 		virtual QCString trPublicSlots()
    { return "¹«ÓĞ²Û"; }

 		virtual QCString trSignals()
    { return "ĞÅºÅ"; }

 		virtual QCString trStaticPublicMembers()
    { return "¾²Ì¬¹«ÓĞ³ÉÔ±"; }

 		virtual QCString trProtectedMembers()
    { return "±£»¤³ÉÔ±"; }

 		virtual QCString trProtectedSlots()
    { return "±£»¤²Û"; }

 		virtual QCString trStaticProtectedMembers()
    { return "¾²Ì¬±£»¤³ÉÔ±"; }
		
 		virtual QCString trPrivateMembers()
    { return "Ë½ÓĞ³ÉÔ±"; }
		
 		virtual QCString trPrivateSlots()
    { return "Ë½ÓĞ²Û"; }
		
 		virtual QCString trStaticPrivateMembers()
    { return "¾²Ì¬Ë½ÓĞ³ÉÔ±"; }
		
    // end of member sections 
    
 		virtual QCString trWriteList(int numEntries)
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
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
            result+="¡¢";
          else                // the fore last entry
            result+=CN_SPC"¼°"CN_SPC;
        }
      }
      return result; 
    }
    
 		virtual QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "¼Ì³Ğ×Ô"CN_SPC+trWriteList(numEntries)+"¡£";
    }

 		virtual QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "±»"CN_SPC+trWriteList(numEntries)+CN_SPC"¼Ì³Ğ.";
    }
		
 		virtual QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "ÖØÔØ"CN_SPC+trWriteList(numEntries)+"¡£";
    }
		
 		virtual QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "±»"CN_SPC+trWriteList(numEntries)+CN_SPC"ÖØÔØ¡£";
    }

 		virtual QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Ãû×Ö¿Õ¼ä³ÉÔ±"; }
		
 		virtual QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
   		QCString result="ÕâÀïÁĞ³öÁËËùÓĞ";
      if (!extractAll) result+="ÎÄµµ»¯µÄ";
      result+="Ãû×Ö¿Õ¼ä³ÉÔ±£¬¸½´ø";
      if (extractAll) 
        result+="ËùÔÚÀàµÄÎÄµµµÄÁ´½Ó£º";
      else 
        result+="ËùÔÚÀàµÄÁ´½Ó£º";
      return result;
    }

 		virtual QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "Ãû×Ö¿Õ¼äË÷Òı"; }

 		virtual QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Ãû×Ö¿Õ¼äÎÄµµ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
 		virtual QCString trNamespaces()
    {
      return "Namespaces";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
 		virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
   		QCString result=(QCString)"¸Ã";
      switch(compType)
      {
        case ClassDef::Class:      result+="Àà"; break;
        case ClassDef::Struct:     result+="½á¹¹"; break;
        case ClassDef::Union:      result+="ÁªºÏ"; break;
        case ClassDef::Interface:  result+="½Ó¿Ú"; break;
        case ClassDef::Protocol:   result+="Ğ­Òé"; break; // translate me!
        case ClassDef::Category:   result+="·ÖÀà"; break; // translate me!
        case ClassDef::Exception:  result+="Òì³£"; break;
      }
      result+="µÄÎÄµµÓÉÒÔÏÂÎÄ¼şÉú³É£º";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
 		virtual QCString trAlphabeticalList()
    { return "°´×ÖµäË³ĞòÅÅĞòµÄÁĞ±í"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
 		virtual QCString trReturnValues()
    { return "·µ»ØÖµ"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
 		virtual QCString trMainPage()
    { return "Ê×Ò³"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
 		virtual QCString trPageAbbreviation()
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

 		virtual QCString trSources()
    {
      return "Ô´´úÂë";
    }

 		virtual QCString trDefinedAtLineInSourceFile()
    {
      return "ÔÚÎÄ¼ş"CN_SPC"@1"CN_SPC"µÚ"CN_SPC"@0"CN_SPC"ĞĞ¶¨Òå¡£";
    }

 		virtual QCString trDefinedInSourceFile()
    {
      return "ÔÚÎÄ¼ş"CN_SPC"@0"CN_SPC"ÖĞ¶¨Òå¡£";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

 		virtual QCString trDeprecated()
    {
      return "Deprecated";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
 		virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)clName+CN_SPC"ºÏ×÷Í¼£º";
    }

    /*! this text is put before an include dependency graph */
 		virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)fName+CN_SPC"°üº¬/ÒÀÀµ¹ØÏµÍ¼£º";
    }

    /*! header that is put before the list of constructor/destructors. */
 		virtual QCString trConstructorDocumentation()
    {
      return "¹¹Ôì¼°Îö¹¹º¯ÊıÎÄµµ"; 
    }

    /*! Used in the file documentation to point to the corresponding sources. */
 		virtual QCString trGotoSourceCode()
    {
      return "ä¯ÀÀ¸ÃÎÄ¼şµÄÔ´´úÂë¡£";
    }

    /*! Used in the file sources to point to the corresponding documentation. */
 		virtual QCString trGotoDocumentation()
    {
      return "ä¯ÀÀ¸ÃÎÄ¼şµÄÎÄµµ¡£";
    }

    /*! Text for the \\pre command */
 		virtual QCString trPrecondition()
    {
      return "Ç°ÖÃÌõ¼ş";
    }

    /*! Text for the \\post command */
 		virtual QCString trPostcondition()
    {
      return "ºóÖÃÌõ¼ş";
    }

    /*! Text for the \\invariant command */
 		virtual QCString trInvariant()
    {
      return "²»±äĞÔ";
    }

    /*! Text shown before a multi-line variable/enum initialization */
 		virtual QCString trInitialValue()
    {
      return "³õÊ¼»¯ĞòÁĞ£º";
    }

    /*! Text used the source code in the file index */
 		virtual QCString trCode()
    {
      return "´úÂë";
    }

 		virtual QCString trGraphicalHierarchy()
    {
      return "Àà¼Ì³Ğ¹ØÏµÍ¼";
    }

 		virtual QCString trGotoGraphicalHierarchy()
    {
      return "ä¯ÀÀÀà¼Ì³Ğ¹ØÏµÍ¼";
    }

 		virtual QCString trGotoTextualHierarchy()
    {
      return "ä¯ÀÀÀà¼Ì³Ğ¹ØÏµ±í";
    }

 		virtual QCString trPageIndex()
    {
      return "Ò³ÃæË÷Òı";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
 		virtual QCString trNote()
    {
      return "×¢½â";
    }

 		virtual QCString trPublicTypes()
    {
      return "¹«ÓĞÀàĞÍ";
    }

 		virtual QCString trPublicAttribs()
    {
			 if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			 { 
				 return "Êı¾İ³ÉÔ±";
			 }
			 else {
				 return "¹«ÓĞÊôĞÔ";
			 }
    }

 		virtual QCString trStaticPublicAttribs()
    {
      return "¾²Ì¬¹«ÓĞÊôĞÔ";
    }

 		virtual QCString trProtectedTypes()
    {
      return "±£»¤ÀàĞÍ";
    }

 		virtual QCString trProtectedAttribs()
    {
      return "±£»¤ÊôĞÔ";
    }

 		virtual QCString trStaticProtectedAttribs()
    {
      return "¾²Ì¬±£»¤ÊôĞÔ";
    }

 		virtual QCString trPrivateTypes()
    {
      return "Ë½ÓĞÀàĞÍ";
    }

 		virtual QCString trPrivateAttribs()
    {
      return "Ë½ÓĞÊôĞÔ";
    }

 		virtual QCString trStaticPrivateAttribs()
    {
      return "¾²Ì¬Ë½ÓĞÊôĞÔ";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
 		virtual QCString trTodo()
    {
      return "TODO";
    }

    /*! Used as the header of the todo list */
 		virtual QCString trTodoList()
    {
      return "TODO"CN_SPC"ÁĞ±í";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

 		virtual QCString trReferencedBy()
    {
      return "²Î¿¼×Ô";
    }

 		virtual QCString trRemarks()
    {
      return "ÆÀÂÛ";
    }

 		virtual QCString trAttention()
    {
      return "×¢Òâ";
    }

 		virtual QCString trInclByDepGraph()
    {
      return "´ËÍ¼Õ¹Ê¾Ö±½Ó»ò¼ä½Ó°üº¬¸ÃÎÄ¼şµÄÎÄ¼ş£º";
    }

 		virtual QCString trSince()
    {
      return "×Ô´Ó";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
 		virtual QCString trLegendTitle()
    {
      return "Í¼Àı";
    }

    /*! page explaining how the dot graph's should be interpreted */
 		virtual QCString trLegendDocs()
    {
      return 
        "±¾Ò³ÏòÄúÕ¹Ê¾ÈçºÎÀí½âÓÉ"CN_SPC"Doxygen"CN_SPC"Éú³ÉµÄÍ¼ĞÎ¡£<p>\n"
        "Çë¿¼ÂÇÈçÏÂÊ¾Àı£º\n"
        "\\code\n"
        "/*! ÓÉÓÚ½ØÈ¡¶øÊ¹¸ÃÀà²»¿É¼û */\n"
        "class Invisible { };\n\n"
        "/*! ±»½ØÈ¡µÄÀà£¬¼Ì³Ğ¹ØÏµ±»Òş²ØÆğÀ´ÁË */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Ã»ÓĞ±»"CN_SPC"doxygen"CN_SPC"µÄ×¢ÊÍÎÄµµ»¯µÄÀà */\n"
        "class Undocumented { };\n\n"
        "/*! ±»¹«ÓĞ¼Ì³ĞµÄÀà */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! ±»±£»¤¼Ì³ĞµÄÀà */\n"
        "class ProtectedBase { };\n\n"
        "/*! ±»Ë½ÓĞ¼Ì³ĞµÄÀà */\n"
        "class PrivateBase { };\n\n"
        "/*! ±»Ê¹ÓÃµÄÀà */\n"
        "class Used { };\n\n"
        "/*! ¼Ì³ĞÁËÈô¸ÉÆäËüÀàµÄÀà */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Èç¹ûÔÚÅäÖÃÎÄ¼şÖĞÖ¸¶¨ÁË"CN_SPC"MAX_DOT_GRAPH_HEIGHT"CN_SPC"µÄÖµÎª200£¬"
        "ÔòDoxygen½«Éú³ÉÈçÏÂµÄÍ¼ĞÎ£º"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "ÒÔÉÏÍ¼ĞÎÖĞµÄ¾ØĞÎÓĞÈçÏÂµÄº¬Òå£º\n"
        "<ul>\n"
        "<li>±»ºÚÉ«Ìî³äµÄ¾ØĞÎ´ú±íµ±Ç°µÄÀà»ò½á¹¹¡£\n"
        "<li>ºÚÉ«±ß¿òµÄ¾ØĞÎ´ú±í<i>ÎÄµµ»¯</i>µÄÀà»ò½á¹¹¡£\n"
        "<li>»ÒÉ«±ß¿òµÄ¾ØĞÎ´ú±íÃ»ÓĞ<i>ÎÄµµ»¯</i>µÄÀà»ò½á¹¹¡£\n"
        "<li>ºìÉ«±ß¿òµÄ¾ØĞÎ´ú±í¼Ì³Ğ/°üº¬¹ØÏµÃ»ÓĞ±»ÍêÕûÏÔÊ¾³öµÄÀà»ò½á¹¹¡£Èç¹ûÒ»·ùÍ¼ÏñµÄ³ß"
        "´ç´óÓÚÖ¸¶¨³ß´ç£¬Ëü½«±»½ØÈ¡¡£"
        "</ul>\n"
        "¸÷¸ö¼ıÍ·ÓĞÈçÏÂµÄº¬Òå£º\n"
        "<ul>\n"
        "<li>ÉîÀ¶É«µÄ¼ıÍ·ÓÃÓÚÏÔÊ¾Á½¸öÀàÖ®¼äµÄ¹«ÓĞ¼Ì³Ğ¹ØÏµ¡£\n"
        "<li>ÉîÂÌÉ«µÄ¼ıÍ·ÓÃÓÚÏÔÊ¾±£»¤¼Ì³Ğ¹ØÏµ¡£\n"
        "<li>ÉîºìÉ«µÄ¼ıÍ·ÓÃÓÚÏÔÊ¾Ë½ÓĞ¼Ì³Ğ¹ØÏµ¡£\n"
        "<li>×ÏÉ«µã×´ÏßÌõµÄ¼ıÍ·ÓÃÓÚÏÔÊ¾Á½¸öÀàÖ®¼ä°üº¬»òÕßÊ¹ÓÃµÄ¹ØÏµ¡£Í¨¹ı¼ıÍ·ÅÔ±ßµÄ±äÁ¿¿ÉÒÔ"
        "·ÃÎÊµ½¼ıÍ·ËùÖ¸µÄÀà»ò½á¹¹¡£\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
 		virtual QCString trLegend()
    {
      return "Í¼Àı";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "²âÊÔ";
    }

    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "²âÊÔÁĞ±í";
    }

//////////////////////////////////////////////////////////////////////////
//// new since 1.2.1
////////////////////////////////////////////////////////////////////////////
		/*! Used as a section header for KDE-2 IDL methods */
		virtual QCString trDCOPMethods()
		{
			return "DCOP"CN_SPC"·½·¨";
		}

//////////////////////////////////////////////////////////////////////////
//// new since 1.2.2
////////////////////////////////////////////////////////////////////////////

		/*! Used as a section header for IDL properties */
		virtual QCString trProperties()
		{
			return "ÊôĞÔ";
		}

		/*! Used as a section header for IDL property documentation */
		virtual QCString trPropertyDocumentation()
		{
			return "ÊôĞÔÎÄµµ";
		}

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return "½Ó¿Ú";
    }

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
			if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			{
				return "Êı¾İ½á¹¹";
			}
			else
			{
				return "Àà";
			}
    }

    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"°ü "+name;
    }
		
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "°üÁĞ±í";
    }

    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
			 return "ÕâÀïÁĞ³öËùÓĞµÄ°ü£¬¸½´ø¼òÒªËµÃ÷(Èç¹ûÓĞµÄ»°)£º"; 
    }

    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "°ü";
    }

    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return "°üµÄÎÄµµ";
    }

    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return 	"Öµ:";
    }

////////////////////////////////////////////////////////////////////////////
//// new since 1.2.6
////////////////////////////////////////////////////////////////////////////
		virtual QCString trBug ()
		{
			return "È±Ïİ";
		}

		virtual QCString trBugList ()
		{
			return "È±ÏİÁĞ±í";
		}

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
    {
      return "936";
    }

    /*! Used as ansicpg for RTF fcharset 
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "134";
    }


    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Ë÷Òı";
    }

   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Class" : "class"));
      if (!singular)  result+="es";
      return result; 
			*/
			return "Àà";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "File" : "file"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "ÎÄ¼ş";

    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Namespace" : "namespace"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "Ãû×Ö¿Õ¼ä";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Group" : "group"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "×é";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Page" : "page"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "Ò³";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Member" : "member"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "³ÉÔ±";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trField(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Field" : "field"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "×Ö¶Î";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool /*first_capital*/, bool /*singular*/)
    { 
			/*
   		QCString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "È«¾Ö¶¨Òå";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool /*first_capital*/, bool /*singular*/)
    {                                                                         
			/*
   		QCString result((first_capital ? "Author" : "author"));
      if (!singular)  result+="s";
      return result; 
			*/
			return "×÷Õß";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "²Î¿¼";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      /* return "Implements "+trWriteList(numEntries)+"."; */
      return "ÊµÏÖÁË"CN_SPC+trWriteList(numEntries)+"¡£";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      /* return "Implemented in "+trWriteList(numEntries)+"."; */
      return "ÔÚ"CN_SPC+trWriteList(numEntries)+CN_SPC"ÄÚ±»ÊµÏÖ¡£";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      /* return "Table of Contents"; */
      return "Ä¿Â¼";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
/*    return "Deprecated List";  */
      return "¹ıÊ±ÁĞ±í";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      /*      return "Events"; */
      return "ÊÂ¼ş";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      /* return "Event Documentation"; */
      return "ÊÂ¼şÎÄµµ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      /* return "Package Types"; */
      return "Ä£¿éÀàĞÍ";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      /* return "Package Functions"; */
      return "Ä£¿éº¯Êı";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      /* return "Static Package Functions"; */
      return "¾²Ì¬Ä£¿éº¯Êı";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      /* return "Package Attributes"; */
      return "Ä£¿éÊôĞÔ";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      /* return "Static Package Attributes"; */
      return "¾²Ì¬Ä£¿éÊôĞÔ";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      /* return "All"; */
      return "È«²¿";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      /* return "Here is the call graph for this function:"; */
      return "º¯Êıµ÷ÓÃÍ¼:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! When the search engine is enabled this text is put in the header 
     *  of each page before the field where one can enter the text to search 
     *  for. 
     */
    virtual QCString trSearchForIndex()
    {
      /* return "Search for"; */
      return "ËÑË÷";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      /* return "Search Results"; */
      return "ËÑË÷½á¹û";
    }
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
      if (numDocuments==0)
      {
        /* return "Sorry, no documents matching your query."; */
        return "¶Ô²»Æğ£¬ÕÒ²»µ½ÓëÄãµÄ²éÑ¯Ïà·ûµÄÎÄµµ¡£";
      }
      else if (numDocuments==1)
      {
        /* return "Found <b>1</b> document matching your query."; */
        return "ÕÒµ½<b>1</b>ÆªÓëÄãµÄ²éÑ¯Ïà·ûµÄÎÄµµ¡£";
      }
      else 
      {
        /* return "Found <b>$num</b> documents matching your query. "
               "Showing best matches first."; */
        return "ÕÒµ½<b>$num</b>ÆªÓëÄãµÄ²éÑ¯Ïà·ûµÄÎÄµµ¡£"
               "ÏÈÏÔÊ¾×îÎÇºÏµÄÎÄµµ¡£";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      /* return "Matches:"; */
      return "·ûºÏµÄ½á¹û:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      /* return filename + " Source File"; */
      return filename + CN_SPC"Ô´ÎÄ¼ş";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    /*     { return "Directory Hierarchy"; } */
    {
      return "$(BL\\Â¼½á$(DC((B";
    }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    /*     { return "Directory Documentation"; } */
    {
      return "$(BL\\Â¼J8[c(B";
    }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    /*     { return "Directories"; } */
    {
      return "$(BL\\Â¼(B";
    }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
/*     { return "This directory hierarchy is sorted roughly, " */
/*              "but not completely, alphabetically:"; */
/*     } */
    {
      return "$(BL\\Â¼½á$(DC(½ö¾­¹ı$(BBgCWE*GSNs!$VuL$40A40D;zJlË³=x(B";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    /*     { QCString result=dirName; result+=" Directory Reference"; return result; } */
    {
      QCString result=dirName;
      result+=CN_SPC"$(BL\\Â¼;29M(B"; 
      return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool, bool)
    { 
      /*       QCString result((first_capital ? "Director" : "director")); */
      /*       if (singular) result+="y"; else result+="ies"; */
      /*       return result;  */
      return "$(BL\\Â¼(B";
    }
    
};

#endif
