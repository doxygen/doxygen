/******************************************************************************
 * <notice>This is the template for generating language.doc.  
 * Edit manually this file, not the language.doc!</notice>
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
/*! \page langhowto Internationalization

<h3>Support for multiple languages</h3>

Doxygen has support for multiple languages. This means 
that the text fragments that doxygen generates can changed into languages 
other than English (the default) at configuration time.
<p>
Currently (version $version), $numlang languages 
are supported (sorted alphabetically): 
$languages.

The table of information related to the supported languages follows.
It is sorted by language alphabetically.  The <B>Status</B> column
was generated from sources and shows approximately the last version
when the translator was updated.
<p>

$information_table
<p>

Most people on the list have indicated that they were also busy
doing other things, so if you want to help to speed things up please 
let them (or me) know.

If you want to add support for a language that is not yet listed 
please see the next section.

<h3>Adding a new language to doxygen</h3>

This short HOWTO explains how to add support for a new language to Doxygen:

Just follow these steps:
<ol>
<li>Tell me for which language you want to add support. If no one else
    is already working on support for that language, you will be 
    assigned as the maintainer for the language. 
<li>Create a copy of translator_en.h and name it 
    translator_<your_2_letter_counter_code>.h
    I'll use xx in the rest of this document.
<li>Edit language.cpp:
    Add a 
\verbatim
#include<translator_xx.h>
\endverbatim
    in <code>setTranslator()</code> add
\verbatim
    else if (L_EQUAL("your_language_name"))
    {
      theTranslator = new TranslatorYourLanguage;
    }
\endverbatim
    after the <code>if { ... }</code>
<li>Edit libdoxygen.pro.in and add \c translator_xx.h to 
    the \c HEADERS line in the file doxygen.pro.
<li>Edit <code>translator_xx.h</code>:
   <ul>
   <li>Rename <code>TRANSLATOR_EN_H</code> to <code>TRANSLATOR_XX_H</code> twice.
   <li>Rename TranslatorEnglish to TranslatorYourLanguage 
   <li>In the member <code>idLanguage()</code> change "english" into the 
     name of the your language (use lower case characters only). Depending
     on the language you may also wish to change the member functions 
     latexLanguageSupportCommand() and idLanguageCharset().
   <li>Edit all the strings that are returned by the member functions that 
     start with tr. 
     Try to match punctuation and capitals!
     To enter special characters (with accents) you can:
     <ul>
     <li>  Enter them directly if your keyboard supports that and you are 
           using a Latin-1 font.
           Doxygen will translate the characters to proper Latex and
           leave the Html and man output for what it is (which is fine, if
           idLanguageCharset() is set correctly).
     <li>  Use html codes like \&auml; for an a with an umlaut (i.e. &auml;).
           See the HTML specification for the codes.
     </ul>
   </ul>
<li>Run configure and make again from the root of the distribution, 
    in order to regenerated the Makefiles.
<li>Now you can use <code>OUTPUT_LANGUAGE = your_language_name</code> 
    in the config file to generate output in your language.
<li>Send <code>translator_xx.h</code> to me so I can add it to doxygen.
</ol>

<h3>Maintaining a language</h3>

As new versions of doxygen appear, new sentences will be
added to the Translator interface class. Of course these need to be translated
as well (otherwise doxygen wouldn't even compile!). 

Waiting until all language maintainers have translated these new sentences
and sent the results would not be very practical for me.

Instead, a new class TranslatorAdapter_x_y_z will be added to 
translator_adapter.h (here x,y, and z correspond to the current
version of doxygen). And all translators that previous derived from 
Translator will now derive from this adapter class.

The Adapter class contains the new sentences with
default translations using the English translator (which is always up to date). 
Instead of deriving your TranslatorXX class directly from Translator it will 
derive from the intermediate class TranslatorAdapter_x_y_z. 

Thus, if a translator class inherits from a adapter class 
maintenance is needed. By looking at the adapter class itself (and its base
classes) you can easily see which methods need to be updated.

To update a language simply make your translator class derive from
the abstract class Translator and provide translations for the methods that
were previously provided by the adapter class (and its base classes).

*/

