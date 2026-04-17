<?xml version='1.0' encoding='utf-8'?>
<TS version="2.1" language="fr">
<context>
    <name>Expert</name>
<message>
        <source>Possible values are:</source>
        <translation>Les valeurs possibles sont :</translation>
    </message>
<message>
        <source>and</source>
        <translation>et</translation>
    </message>
<message>
        <source>The default value is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>La valeur par défaut est : &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>Minimum value: %1, maximum value: %2, default value: %3.</source>
        <translation>Valeur minimum : %1, valeur maximum : %2, valeur par défaut : %3.</translation>
    </message>
<message>
        <source>The default value is: system dependent.</source>
        <translation>La valeur par défaut dépend du système.</translation>
    </message>
<message>
        <source>The default directory is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Le répertoire par défaut est : &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The default file is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Le fichier par défaut est : &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The default file (with absolute path) is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Le fichier par défaut (avec chemin absolu) est : &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The file has to be specified with full path.</source>
        <translation>Le fichier doit être spécifié avec un chemin complet.</translation>
    </message>
<message>
        <source>The default image is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>L'image par défaut est : &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The default image (with absolute path) is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>L'image par défaut (avec chemin absolu) est : &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The image has to be specified with full path.</source>
        <translation>L'image doit être spécifiée avec un chemin complet.</translation>
    </message>
<message>
        <source>This tag requires that the tag %1 is set to &lt;code&gt;YES&lt;/code&gt;.</source>
        <translation>Cette balise nécessite que la balise %1 soit définie sur &lt;code&gt;YES&lt;/code&gt;.</translation>
    </message>
</context>
<context>
    <name>OptionDocs</name>
<message>
        <source>DOXYFILE_ENCODING</source>
            <translation>Cette balise specifie l'encodage utilise pour tous les caracteres du fichier de configuration qui suivent. La valeur par defaut est UTF-8, qui est egalement l'encodage utilise pour tout le texte avant la premiere occurrence de cette balise. Doxygen utilise \c libiconv (ou l'iconv integre dans \c libc) pour le transcodage. Voir https://www.gnu.org/software/libiconv/ pour la liste des encodages possibles.</translation>
    </message>
<message>
        <source>PROJECT_NAME</source>
            <translation>La balise \c PROJECT_NAME est un mot unique (ou une sequence de mots entoures de guillemets doubles, sauf si vous utilisez Doxywizard) qui doit identifier le projet pour lequel la documentation est generee. Ce nom est utilise dans le titre de la plupart des pages generees et dans quelques autres endroits.</translation>
    </message>
<message>
        <source>PROJECT_NUMBER</source>
            <translation>La balise \c PROJECT_NUMBER peut etre utilisee pour entrer un numero de projet ou de revision. Cela peut etre utile pour archiver la documentation generee ou si un systeme de controle de version est utilise.</translation>
    </message>
<message>
        <source>PROJECT_BRIEF</source>
            <translation>En utilisant la balise \c PROJECT_BRIEF, on peut fournir une description optionnelle d'une ligne pour un projet qui apparait en haut de chaque page et devrait donner aux lecteurs une idee rapide du but du projet. Gardez la description courte.</translation>
    </message>
<message>
        <source>PROJECT_LOGO</source>
            <translation>Avec la balise \c PROJECT_LOGO, on peut specifier un logo ou une icone qui est inclus dans la documentation. La hauteur maximale du logo ne doit pas depasser 55 pixels et la largeur maximale ne doit pas depasser 200 pixels. Doxygen copiera le logo dans le repertoire de sortie.</translation>
    </message>
<message>
        <source>PROJECT_ICON</source>
            <translation>Avec la balise \c PROJECT_ICON, on peut specifier une icone qui est incluse dans les onglets lorsque le document HTML est affiche. Doxygen copiera le logo dans le repertoire de sortie.</translation>
    </message>
<message>
        <source>OUTPUT_DIRECTORY</source>
            <translation>La balise \c OUTPUT_DIRECTORY est utilisee pour specifier le chemin (relatif ou absolu) dans lequel la documentation generee sera ecrite. Si un chemin relatif est entre, il sera relatif a l'emplacement ou Doxygen a ete lance. Si laisse vide, le repertoire courant sera utilise.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS</source>
            <translation>Si la balise \c CREATE_SUBDIRS est definie a \c YES, Doxygen creera jusqu'a 4096 sous-repertoires (sur 2 niveaux) sous le repertoire de sortie de chaque format de sortie et distribuera les fichiers generes sur ces repertoires. Activer cette option peut etre utile lors de l'alimentation de Doxygen avec une grande quantite de fichiers sources, ou mettre tous les fichiers generes dans le meme repertoire causerait autrement des problemes de performance pour le systeme de fichiers. Ajustez \c CREATE_SUBDIRS_LEVEL pour controler le nombre de sous-repertoires.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS_LEVEL</source>
            <translation>Controle le nombre de sous-repertoires qui seront crees lorsque la balise \c CREATE_SUBDIRS est definie a \c YES. Le niveau 0 represente 16 repertoires, et chaque increment de niveau double le nombre de repertoires, resultant en 4096 repertoires au niveau 8 qui est la valeur par defaut et egalement la valeur maximale. Les sous-repertoires sont organises sur 2 niveaux, le premier niveau a toujours un nombre fixe de 16 repertoires.</translation>
    </message>
<message>
        <source>ALLOW_UNICODE_NAMES</source>
            <translation>Si la balise \c ALLOW_UNICODE_NAMES est definie a \c YES, Doxygen permettra aux caracteres non-ASCII d'apparaitre dans les noms des fichiers generes. Si defini a \c NO, les caracteres non-ASCII seront echappes, par exemple _xE3_x81_x84 sera utilise pour Unicode U+3044.</translation>
    </message>
<message>
        <source>OUTPUT_LANGUAGE</source>
            <translation>La balise \c OUTPUT_LANGUAGE est utilisee pour specifier la langue dans laquelle toute la documentation generee par Doxygen est ecrite. Doxygen utilisera cette information pour generer toute la sortie constante dans la langue appropriee.</translation>
    </message>
<message>
        <source>BRIEF_MEMBER_DESC</source>
            <translation>Si la balise \c BRIEF_MEMBER_DESC est definie a \c YES, Doxygen inclura les descriptions breves des membres apres les membres qui sont listes dans la documentation du fichier et de la classe (similaire a \c Javadoc). Definir a \c NO pour desactiver cela.</translation>
    </message>
<message>
        <source>REPEAT_BRIEF</source>
            <translation>Si la balise \c REPEAT_BRIEF est definie a \c YES, Doxygen ajoutera la description breve d'un membre ou d'une fonction avant la description detaillee. &lt;br&gt;Remarque: Si \ref cfg_hide_undoc_members "HIDE_UNDOC_MEMBERS" et \ref cfg_brief_member_desc "BRIEF_MEMBER_DESC" sont tous deux definis a \c NO, les descriptions breves seront completement supprimees.</translation>
    </message>
<message>
        <source>ABBREVIATE_BRIEF</source>
            <translation>Cette balise implemente un abbreviateur de description breve quasi-intelligent qui est utilise pour former le texte dans diverses listes. Chaque chaine de cette liste, si elle est trouvee comme texte de debut de la description breve, sera retiree du texte et le resultat, apres traitement de toute la liste, est utilise comme texte annote. Sinon, la description breve est utilisee telle quelle. Si laisse vide, les valeurs suivantes sont utilisees (`$name` est automatiquement remplace par le nom de l'entite):</translation>
    </message>
<message>
        <source>ALWAYS_DETAILED_SEC</source>
            <translation>Si les balises \c ALWAYS_DETAILED_SEC et \ref cfg_repeat_brief "REPEAT_BRIEF" sont toutes deux definies a \c YES, alors Doxygen generera une section detaillee meme s'il n'y a qu'une description breve.</translation>
    </message>
<message>
        <source>INLINE_INHERITED_MEMB</source>
            <translation>Si la balise \c INLINE_INHERITED_MEMB est definie a \c YES, Doxygen affichera tous les membres herites d'une classe dans la documentation de cette classe comme si ces membres etaient des membres ordinaires de la classe. Les constructeurs, destructeurs et operateurs d'affectation des classes de base ne seront pas affiches.</translation>
    </message>
<message>
        <source>FULL_PATH_NAMES</source>
            <translation>Si la balise \c FULL_PATH_NAMES est definie a \c YES, Doxygen ajoutera le chemin complet avant les noms de fichiers dans la liste des fichiers et dans les fichiers d'en-tete. Si defini a \c NO, le chemin le plus court qui rend le nom de fichier unique sera utilise.</translation>
    </message>
<message>
        <source>STRIP_FROM_PATH</source>
            <translation>La balise \c STRIP_FROM_PATH peut etre utilisee pour retirer une partie definie par l'utilisateur du chemin. Le retrait n'est effectue que si l'une des chaines specifiees correspond a la partie gauche du chemin. La balise peut etre utilisee pour afficher des chemins relatifs dans la liste des fichiers. Si laisse vide, le repertoire a partir duquel Doxygen est lance est utilise comme chemin a retirer. &lt;br&gt;Remarquez que vous pouvez specifier des chemins absolus ici, mais aussi des chemins relatifs, qui seront relatifs au repertoire ou Doxygen est lance.</translation>
    </message>
<message>
        <source>STRIP_FROM_INC_PATH</source>
            <translation>La balise \c STRIP_FROM_INC_PATH peut etre utilisee pour retirer une partie definie par l'utilisateur du chemin mentionne dans la documentation d'une classe, qui indique au lecteur quel fichier d'en-tete inclure pour utiliser une classe. Si laisse vide, seul le nom du fichier d'en-tete contenant la definition de la classe est utilise. Sinon, on devrait specifier la liste des chemins d'inclusion qui sont normalement passes au compilateur en utilisant l'option `-I`.</translation>
    </message>
<message>
        <source>SHORT_NAMES</source>
            <translation>Si la balise \c SHORT_NAMES est definie a \c YES, Doxygen generera des noms de fichiers beaucoup plus courts (mais moins lisibles). Cela peut etre utile si votre systeme de fichiers ne supporte pas les longs noms comme sur DOS, Mac ou CD-ROM.</translation>
    </message>
<message>
        <source>JAVADOC_AUTOBRIEF</source>
            <translation>Si la balise \c JAVADOC_AUTOBRIEF est definie a \c YES alors Doxygen interpretera la premiere ligne (jusqu'au premier point, point d'interrogation ou point d'exclamation) d'un commentaire de style Javadoc comme la description breve. Si defini a \c NO, le style Javadoc se comportera comme des commentaires de style Qt normaux (requierant donc une commande \ref cmdbrief "\@brief" explicite pour une description breve).</translation>
    </message>
<message>
        <source>JAVADOC_BANNER</source>
            <translation>Si la balise \c JAVADOC_BANNER est definie a \c YES alors Doxygen interpretera une ligne telle que \verbatim /***************\endverbatim comme etant le debut d'une "banniere" de commentaire de style Javadoc. Si defini a \c NO, le style Javadoc se comportera comme des commentaires normaux et Doxygen ne l'interpretera pas.</translation>
    </message>
<message>
        <source>QT_AUTOBRIEF</source>
            <translation>Si la balise \c QT_AUTOBRIEF est definie a \c YES alors Doxygen interpretera la premiere ligne (jusqu'au premier point, point d'interrogation ou point d'exclamation) d'un commentaire de style Qt comme la description breve. Si defini a \c NO, le style Qt se comportera comme des commentaires de style Qt normaux (requierant donc une commande \ref cmdbrief "\brief" explicite pour une description breve).</translation>
    </message>
<message>
        <source>MULTILINE_CPP_IS_BRIEF</source>
            <translation>La balise \c MULTILINE_CPP_IS_BRIEF peut etre definie a \c YES pour faire que Doxygen traite un bloc de commentaire special C++ multiligne (c'est-a-dire un bloc de commentaires \c //! ou \c ///) comme une description breve. C'etait le comportement par defaut. Le nouveau defaut est de traiter un bloc de commentaire C++ multiligne comme une description detaillee. Definissez cette balise a \c YES si vous preferez l'ancien comportement. &lt;br&gt;Remarquez que definir cette balise a \c YES signifie egalement que les commentaires Rational Rose ne sont plus reconnus.</translation>
    </message>
<message>
        <source>PYTHON_DOCSTRING</source>
            <translation>Par defaut, les docstrings Python sont affichees comme du texte preformate et les commandes speciales de Doxygen ne peuvent pas etre utilisees. En definissant \c PYTHON_DOCSTRING a \c NO, les commandes speciales de Doxygen peuvent etre utilisees et le contenu des blocs de documentation docstring est affiche comme documentation Doxygen.</translation>
    </message>
<message>
        <source>INHERIT_DOCS</source>
            <translation>Si la balise \c INHERIT_DOCS est definie a \c YES alors un membre non documente herite de la documentation de tout membre documente qu'il re-implemente.</translation>
    </message>
<message>
        <source>SEPARATE_MEMBER_PAGES</source>
            <translation>Si la balise \c SEPARATE_MEMBER_PAGES est definie a \c YES alors Doxygen produira une nouvelle page pour chaque membre. Si defini a \c NO, la documentation d'un membre fera partie du fichier/classe/namespace qui le contient.</translation>
    </message>
<message>
        <source>TAB_SIZE</source>
            <translation>La balise \c TAB_SIZE peut etre utilisee pour definir le nombre d'espaces dans une tabulation. Doxygen utilise cette valeur pour remplacer les tabulations par des espaces dans les fragments de code.</translation>
    </message>
<message>
        <source>ALIASES</source>
            <translation>Cette balise peut etre utilisee pour specifier un certain nombre d'alias qui agissent comme des commandes dans la documentation. Un alias a la forme: \verbatim name=value \endverbatim Par exemple, ajouter \verbatim "sideeffect=@par Side Effects:^^" \endverbatim vous permettra de placer la commande \c \\sideeffect (ou \c \@sideeffect) dans la documentation, ce qui resultera en un paragraphe defini par l'utilisateur avec le titre "Side Effects:". Notez que vous ne pouvez pas mettre de \ref cmdn "\\n" dans la partie valeur d'un alias pour inserer des sauts de ligne (dans la sortie resultante). Vous pouvez mettre `^^` dans la partie valeur d'un alias pour inserer un saut de ligne comme si un saut de ligne physique etait dans le fichier original. Si vous avez besoin d'un `{` ou `}` ou `,` literal dans la partie valeur d'un alias, vous devez les echapper avec une barre oblique inverse (\c \\).</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_FOR_C</source>
            <translation>Definissez la balise \c OPTIMIZE_OUTPUT_FOR_C a \c YES si votre projet consiste uniquement en sources C. Doxygen generera alors une sortie plus adaptee au C. Par exemple, certains des noms utilises seront differents. La liste de tous les membres sera omise, etc.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_JAVA</source>
            <translation>Definissez la balise \c OPTIMIZE_OUTPUT_JAVA a \c YES si votre projet consiste uniquement en sources Java ou Python. Doxygen generera alors une sortie plus adaptee a ce langage. Par exemple, les namespaces seront presentes comme des packages, les portees qualifiees auront un aspect different, etc.</translation>
    </message>
<message>
        <source>OPTIMIZE_FOR_FORTRAN</source>
            <translation>Definissez la balise \c OPTIMIZE_FOR_FORTRAN a \c YES si votre projet consiste en sources Fortran. Doxygen generera alors une sortie adaptee au Fortran.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_VHDL</source>
            <translation>Definissez la balise \c OPTIMIZE_OUTPUT_VHDL a \c YES si votre projet consiste en sources VHDL. Doxygen generera alors une sortie adaptee au VHDL.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_SLICE</source>
            <translation>Definissez la balise \c OPTIMIZE_OUTPUT_SLICE a \c YES si votre projet consiste uniquement en sources Slice. Doxygen generera alors une sortie plus adaptee a ce langage. Par exemple, les namespaces seront presentes comme des modules, les types seront separes en plus de groupes, etc.</translation>
    </message>
<message>
        <source>EXTENSION_MAPPING</source>
            <translation>Doxygen selectionne l'analyseur a utiliser en fonction de l'extension des fichiers qu'il analyse. Avec cette balise, vous pouvez assigner quel analyseur utiliser pour une extension donnee. Doxygen a une correspondance integree, mais vous pouvez la remplacer ou l'etendre en utilisant cette balise. Le format est &lt;code&gt;ext=language&lt;/code&gt;, ou \c ext est une extension de fichier, et language est l'un des analyseurs supportes par Doxygen: IDL, Java, JavaScript, Csharp (C#), C, C++, Lex, D, PHP, md (Markdown), Objective-C, Python, Slice, VHDL, Fortran (Fortran format fixe: FortranFixed, Fortran format libre: FortranFree, Fortran format inconnu: Fortran. Dans ce dernier cas, l'analyseur essaie de deviner si le code est en format fixe ou libre, c'est le defaut pour les fichiers de type Fortran). Par exemple, pour faire que Doxygen traite les fichiers &lt;code&gt;.inc&lt;/code&gt; comme des fichiers Fortran (le defaut est PHP), et les fichiers &lt;code&gt;.f&lt;/code&gt; comme C (le defaut est Fortran), utilisez: `inc=Fortran f=C`. &lt;br&gt;Note: Pour les fichiers sans extension, vous pouvez utiliser `no_extension` comme espace reserve. &lt;br&gt;Notez que pour les extensions personnalisees vous devez egalement definir \ref cfg_file_patterns "FILE_PATTERNS" sinon les fichiers ne sont pas lus par Doxygen. Lorsque vous specifiez `no_extension` vous devriez ajouter `*` au \ref cfg_file_patterns "FILE_PATTERNS". &lt;br&gt;Notez voir aussi la liste des \ref default_file_extension_mapping "correspondances d'extension de fichier par defaut".</translation>
    </message>
<message>
        <source>MARKDOWN_SUPPORT</source>
            <translation>Si la balise \c MARKDOWN_SUPPORT est activee alors Doxygen pre-traite tous les commentaires selon le format Markdown, ce qui permet une documentation plus lisible. Voir https://daringfireball.net/projects/markdown/ pour plus de details. La sortie du traitement Markdown est ensuite traitee par Doxygen, vous pouvez donc melanger les commandes Doxygen, HTML et XML avec le formatage Markdown. Desactivez uniquement en cas de problemes de compatibilite ascendante.</translation>
    </message>
<message>
        <source>MARKDOWN_STRICT</source>
            <translation>Si la balise \c MARKDOWN_STRICT est activee alors Doxygen traite le texte dans les commentaires comme formatte en Markdown aussi dans les cas ou le format de balisage natif de Doxygen entre en conflit avec celui de Markdown. Cela n'est pertinent que dans les cas ou des backticks sont utilises. Le style de balisage natif de Doxygen permet a un guillemet simple de terminer un fragment de texte commence par un backtick et de le traiter comme un morceau de texte cite, alors qu'en Markdown un tel fragment est traite comme verbatim et ne se termine que lorsqu'un deuxieme backtick correspondant est trouve. De plus, le format de balisage natif de Doxygen necessite que les guillemets doubles soient echappes lorsqu'ils apparaissent dans une section backtick, alors que cela n'est pas necessaire pour Markdown.</translation>
    </message>
<message>
        <source>TOC_INCLUDE_HEADINGS</source>
            <translation>Lorsque la balise \c TOC_INCLUDE_HEADINGS est definie a une valeur non nulle, tous les titres jusqu'a ce niveau sont automatiquement inclus dans la table des matieres, meme s'ils n'ont pas d'attribut id. \note Cette fonctionnalite s'applique actuellement uniquement aux titres Markdown.</translation>
    </message>
<message>
        <source>MARKDOWN_ID_STYLE</source>
            <translation>La balise \c MARKDOWN_ID_STYLE peut etre utilisee pour specifier l'algorithme utilise pour generer les identifiants pour les titres Markdown. Note: Chaque identifiant est unique.</translation>
    </message>
<message>
        <source>AUTOLINK_SUPPORT</source>
            <translation>Lorsqu'active, Doxygen essaie de lier les mots qui correspondent a des classes documentees, ou des namespaces a leur documentation correspondante. Un tel lien peut etre empesche dans des cas individuels en mettant un signe \c % devant le mot ou globalement en definissant \c AUTOLINK_SUPPORT a \c NO. Les mots listes dans la balise \c AUTOLINK_IGNORE_WORDS sont exclus de la liaison automatique.</translation>
    </message>
<message>
        <source>AUTOLINK_IGNORE_WORDS</source>
            <translation>Cette balise specifie une liste de mots qui, lorsqu'ils correspondent au debut d'un mot dans la documentation, supprimeront la generation de liens automatiques, si elle est activee via AUTOLINK_SUPPORT. Cette liste n'affecte pas les liens crees explicitement en utilisant \# ou les commandes \ref cmdlink "\\link" ou \ref cmdref "\\ref".</translation>
    </message>
<message>
        <source>BUILTIN_STL_SUPPORT</source>
            <translation>Si vous utilisez des classes STL (c'est-a-dire `std::string`, `std::vector`, etc.) mais ne voulez pas inclure (un fichier de balises pour) les sources STL comme entree, alors vous devriez definir cette balise a \c YES afin de laisser Doxygen faire correspondre les declarations et definitions de fonctions dont les arguments contiennent des classes STL (par ex. `func(std::string`); versus `func(std::string) {}`). Cela rend egalement les diagrammes d'heritage et de collaboration qui impliquent des classes STL plus complets et precis.</translation>
    </message>
<message>
        <source>CPP_CLI_SUPPORT</source>
            <translation>Si vous utilisez le langage C++/CLI de Microsoft, vous devriez definir cette option a \c YES pour activer le support de l'analyse.</translation>
    </message>
<message>
        <source>SIP_SUPPORT</source>
            <translation>Definissez la balise \c SIP_SUPPORT a \c YES si votre projet consiste uniquement en sources &lt;a href="https://www.riverbankcomputing.com/software"&gt;sip&lt;/a&gt;. Doxygen les analysera comme du C++ normal mais supposera que toutes les classes utilisent l'heritage public au lieu de l'heritage prive lorsqu'aucun mot-cle de protection explicite n'est present.</translation>
    </message>
<message>
        <source>IDL_PROPERTY_SUPPORT</source>
            <translation>Pour l'IDL de Microsoft, il y a des attributs \c propget et \c propput pour indiquer les methodes getter et setter pour une propriete. Definir cette option a \c YES fera que Doxygen remplacera les methodes get et set par une propriete dans la documentation. Cela ne fonctionnera que si les methodes obtiennent ou definissent effectivement un type simple. Si ce n'est pas le cas, ou si vous voulez montrer les methodes quand meme, vous devriez definir cette option a \c NO.</translation>
    </message>
<message>
        <source>DISTRIBUTE_GROUP_DOC</source>
            <translation>Si le regroupement de membres est utilise dans la documentation et que la balise \c DISTRIBUTE_GROUP_DOC est definie a \c YES alors Doxygen reutilisera la documentation du premier membre du groupe (s'il y en a) pour les autres membres du groupe. Par defaut, tous les membres d'un groupe doivent etre documentes explicitement.</translation>
    </message>
<message>
        <source>GROUP_NESTED_COMPOUNDS</source>
            <translation>Si on ajoute une struct ou une classe a un groupe et que cette option est activee, alors toute classe ou struct imbriquee est egalement ajoutee au meme groupe. Par defaut cette option est desactivee et on doit ajouter les composes imbriques explicitement via \ref cmdingroup "\\ingroup".</translation>
    </message>
<message>
        <source>SUBGROUPING</source>
            <translation>Definissez la balise \c SUBGROUPING a \c YES pour permettre aux groupes de membres de classe du meme type (par exemple un groupe de fonctions publiques) d'etre mis comme sous-groupe de ce type (par exemple sous la section Fonctions Publiques). Definissez-la a \c NO pour empecher le sous-groupement. Alternativement, cela peut etre fait par classe en utilisant la commande \ref cmdnosubgrouping "\\nosubgrouping".</translation>
    </message>
<message>
        <source>INLINE_GROUPED_CLASSES</source>
            <translation>Lorsque la balise \c INLINE_GROUPED_CLASSES est definie a \c YES, les classes, structs et unions sont affiches a l'interieur du groupe dans lequel ils sont inclus (par exemple en utilisant \ref cmdingroup "\\ingroup") au lieu d'une page separee (pour HTML et pages de manuel) ou section (pour \f$\mbox{\LaTeX}\f$ et RTF). &lt;br&gt;Notez que cette fonctionnalite ne fonctionne pas en combinaison avec \ref cfg_separate_member_pages "SEPARATE_MEMBER_PAGES".</translation>
    </message>
<message>
        <source>INLINE_SIMPLE_STRUCTS</source>
            <translation>Lorsque la balise \c INLINE_SIMPLE_STRUCTS est definie a \c YES, les structs, classes et unions avec uniquement des champs de donnees publics ou des champs typedef simples seront affiches en ligne dans la documentation de la portee dans laquelle ils sont definis (c'est-a-dire documentation de fichier, namespace ou groupe), a condition que cette portee soit documentee. Si defini a \c NO, les structs, classes et unions sont affiches sur une page separee (pour HTML et pages de manuel) ou section (pour \f$\mbox{\LaTeX}\f$ et RTF).</translation>
    </message>
<message>
        <source>TYPEDEF_HIDES_STRUCT</source>
            <translation>Lorsque la balise \c TYPEDEF_HIDES_STRUCT est activee, un typedef d'une struct, union ou enum est documente comme struct, union ou enum avec le nom du typedef. Donc &lt;code&gt;typedef struct TypeS {} TypeT&lt;/code&gt; apparaitra dans la documentation comme une struct avec le nom \c TypeT. Lorsque desactive, le typedef apparaitra comme un membre d'un fichier, namespace ou classe. Et la struct sera nommee \c TypeS. Cela peut typiquement etre utile pour le code C au cas ou la convention de codage dicte que tous les types composes sont typedefes et seul le typedef est reference, jamais le nom de tag.</translation>
    </message>
<message>
        <source>LOOKUP_CACHE_SIZE</source>
            <translation>La taille du cache de recherche de symboles peut etre definie en utilisant \c LOOKUP_CACHE_SIZE. Ce cache est utilise pour resoudre les symboles donnes leur nom et portee. Puisque cela peut etre un processus couteux et souvent le meme symbole apparait plusieurs fois dans le code, Doxygen garde un cache de symboles pre-resolus. Si le cache est trop petit Doxygen deviendra plus lent. Si le cache est trop grand, la memoire est gaspillee. La taille du cache est donnee par cette formule: \f$2^{(16+\mbox{LOOKUP\_CACHE\_SIZE})}\f$. La plage valide est 0..9, le defaut est 0, correspondant a une taille de cache de \f$2^{16} = 65536\f$ symboles. A la fin d'une execution Doxygen rapportera l'utilisation du cache et suggerera la taille de cache optimale d'un point de vue vitesse.</translation>
    </message>
<message>
        <source>NUM_PROC_THREADS</source>
            <translation>Le \c NUM_PROC_THREADS specifie le nombre de threads que Doxygen est autorise a utiliser pendant le traitement. Lorsque defini a \c 0 Doxygen se basera sur le nombre de coeurs disponibles dans le systeme. Vous pouvez le definir explicitement a une valeur superieure a 0 pour avoir plus de controle sur l'equilibre entre la charge CPU et la vitesse de traitement. Pour le moment seul le traitement d'entree peut etre fait en utilisant plusieurs threads. Puisque c'est encore une fonctionnalite experimentale, le defaut est defini a 1, ce qui desactive effectivement le traitement parallele. Veuillez signaler tout probleme que vous rencontrez. La generation de graphes dot en parallele est controlee par le parametre \c DOT_NUM_THREADS.</translation>
    </message>
<message>
        <source>TIMESTAMP</source>
            <translation>Si la balise \c TIMESTAMP est definie differemment de \c NO alors chaque page generee contiendra la date ou la date et l'heure ou la page a ete generee. Definir cela a \c NO peut aider lors de la comparaison de la sortie de plusieurs executions.</translation>
    </message>
<message>
        <source>EXTRACT_ALL</source>
            <translation>Si la balise \c EXTRACT_ALL est definie a \c YES, Doxygen supposera que toutes les entites dans la documentation sont documentees, meme si aucune documentation n'etait disponible. Les membres prives de classe et les membres statiques de fichier seront caches sauf si les balises \ref cfg_extract_private "EXTRACT_PRIVATE" respectivement \ref cfg_extract_static "EXTRACT_STATIC" sont definies a \c YES. \note Cela desactivera egalement les avertissements sur les membres non documentes qui sont normalement produits lorsque \ref cfg_warnings "WARNINGS" est defini a \c YES.</translation>
    </message>
<message>
        <source>EXTRACT_PRIVATE</source>
            <translation>Si la balise \c EXTRACT_PRIVATE est definie a \c YES, tous les membres prives d'une classe seront inclus dans la documentation.</translation>
    </message>
<message>
        <source>EXTRACT_PRIV_VIRTUAL</source>
            <translation>Si la balise \c EXTRACT_PRIV_VIRTUAL est definie a \c YES, les methodes virtuelles prives documentees d'une classe seront incluses dans la documentation.</translation>
    </message>
<message>
        <source>EXTRACT_PACKAGE</source>
            <translation>Si la balise \c EXTRACT_PACKAGE est definie a \c YES, tous les membres avec une portee package ou interne seront inclus dans la documentation.</translation>
    </message>
<message>
        <source>EXTRACT_STATIC</source>
            <translation>Si la balise \c EXTRACT_STATIC est definie a \c YES, tous les membres statiques d'un fichier seront inclus dans la documentation.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_CLASSES</source>
            <translation>Si la balise \c EXTRACT_LOCAL_CLASSES est definie a \c YES, les classes (et structs) definies localement dans les fichiers sources seront incluses dans la documentation. Si defini a \c NO, seules les classes definies dans les fichiers d'en-tete sont incluses. N'a aucun effet pour les sources Java.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_METHODS</source>
            <translation>Ce drapeau n'est utile que pour le code Objective-C. Si defini a \c YES, les methodes locales, qui sont definies dans la section d'implementation mais pas dans l'interface sont incluses dans la documentation. Si defini a \c NO, seules les methodes dans l'interface sont incluses.</translation>
    </message>
<message>
        <source>EXTRACT_ANON_NSPACES</source>
            <translation>Si ce drapeau est defini a \c YES, les membres des namespaces anonymes seront extraits et apparaitront dans la documentation comme un namespace appele 'anonymous_namespace{file}', ou file sera remplace par le nom de base du fichier qui contient le namespace anonyme. Par defaut les namespaces anonymes sont caches.</translation>
    </message>
<message>
        <source>RESOLVE_UNNAMED_PARAMS</source>
            <translation>Si ce drapeau est defini a \c YES, le nom d'un parametre sans nom dans une declaration sera determine par la definition correspondante. Par defaut les parametres sans nom restent sans nom dans la sortie.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_MEMBERS</source>
            <translation>Si la balise \c HIDE_UNDOC_MEMBERS est definie a \c YES, Doxygen cachera tous les membres non documentes a l'interieur des classes ou fichiers documentes. Si defini a \c NO ces membres seront inclus dans les differents aperçus, mais aucune section de documentation n'est generee. Cette option n'a aucun effet si \ref cfg_extract_all "EXTRACT_ALL" est active.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_CLASSES</source>
            <translation>Si la balise \c HIDE_UNDOC_CLASSES est definie a \c YES, Doxygen cachera toutes les classes non documentees qui sont normalement visibles dans la hierarchie de classes. Si defini a \c NO, ces classes seront incluses dans les differents aperçus. Cette option cachera egalement les concepts C++ non documentes si active. Cette option n'a aucun effet si \ref cfg_extract_all "EXTRACT_ALL" est active.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_NAMESPACES</source>
            <translation>Si la balise \c HIDE_UNDOC_NAMESPACES est definie a \c YES, Doxygen cachera tous les namespaces non documentes qui sont normalement visibles dans la hierarchie des namespaces. Si defini a \c NO, ces namespaces seront inclus dans les differents aperçus. Cette option n'a aucun effet si \ref cfg_extract_all "EXTRACT_ALL" est active.</translation>
    </message>
<message>
        <source>HIDE_FRIEND_COMPOUNDS</source>
            <translation>Si la balise \c HIDE_FRIEND_COMPOUNDS est definie a \c YES, Doxygen cachera toutes les declarations friend. Si defini a \c NO, ces declarations seront incluses dans la documentation.</translation>
    </message>
<message>
        <source>HIDE_IN_BODY_DOCS</source>
            <translation>Si la balise \c HIDE_IN_BODY_DOCS est definie a \c YES, Doxygen cachera tous les blocs de documentation trouves a l'interieur du corps d'une fonction. Si defini a \c NO, ces blocs seront ajoutes au bloc de documentation detaille de la fonction.</translation>
    </message>
<message>
        <source>INTERNAL_DOCS</source>
            <translation>La balise \c INTERNAL_DOCS determine si la documentation qui est tapee apres une commande \ref cmdinternal "\\internal" est incluse. Si la balise est definie a \c NO alors la documentation sera exclue. Definissez-la a \c YES pour inclure la documentation interne.</translation>
    </message>
<message>
        <source>CASE_SENSE_NAMES</source>
            <translation>Avec le reglage correct de l'option \c CASE_SENSE_NAMES, Doxygen sera mieux a meme de correspondre aux capacites du systeme de fichiers sous-jacent. Si le systeme de fichiers est sensible a la casse (c'est-a-dire qu'il supporte des fichiers dans le meme repertoire dont les noms ne different que par la casse), l'option doit etre definie a \c YES pour traiter correctement ces fichiers s'ils apparaissent dans l'entree. Pour les systemes de fichiers qui ne sont pas sensibles a la casse, l'option devrait etre definie a \c NO pour traiter correctement les fichiers de sortie ecrits pour des symboles qui ne different que par la casse, comme pour deux classes, une nommee \c CLASS et l'autre nommee \c Class, et pour supporter egalement les references aux fichiers sans avoir a specifier la casse exacte correspondante. Sur Windows (y compris Cygwin) et macOS, les utilisateurs devraient generalement definir cette option a \c NO, alors que sur Linux ou d'autres variantes Unix, elle devrait generalement etre definie a \c YES.</translation>
    </message>
<message>
        <source>HIDE_SCOPE_NAMES</source>
            <translation>Si la balise \c HIDE_SCOPE_NAMES est definie a \c NO alors Doxygen affichera les membres avec leurs portees de classe et de namespace completes dans la documentation. Si defini a \c YES, la portee sera cachee.</translation>
    </message>
<message>
        <source>HIDE_COMPOUND_REFERENCE</source>
            <translation>Si la balise \c HIDE_COMPOUND_REFERENCE est definie a \c NO (defaut) alors Doxygen ajoutera du texte supplementaire au titre d'une page, tel que Class Reference. Si defini a \c YES la reference du compose sera cachee.</translation>
    </message>
<message>
        <source>SHOW_HEADERFILE</source>
            <translation>Si la balise \c SHOW_HEADERFILE est definie a \c YES alors la documentation d'une classe montrera quel fichier doit etre inclus pour utiliser la classe.</translation>
    </message>
<message>
        <source>SHOW_INCLUDE_FILES</source>
            <translation>Si la balise \c SHOW_INCLUDE_FILES est definie a \c YES alors Doxygen mettra une liste des fichiers qui sont inclus par un fichier dans la documentation de ce fichier.</translation>
    </message>
<message>
        <source>SHOW_GROUPED_MEMB_INC</source>
            <translation>Si la balise \c SHOW_GROUPED_MEMB_INC est definie a \c YES alors Doxygen ajoutera pour chaque membre groupe une instruction d'inclusion a la documentation, indiquant au lecteur quel fichier inclure pour utiliser le membre.</translation>
    </message>
<message>
        <source>FORCE_LOCAL_INCLUDES</source>
            <translation>Si la balise \c FORCE_LOCAL_INCLUDES est definie a \c YES alors Doxygen listera les fichiers d'inclusion avec des guillemets doubles dans la documentation plutot qu'avec des chevrons.</translation>
    </message>
<message>
        <source>INLINE_INFO</source>
            <translation>Si la balise \c INLINE_INFO est definie a \c YES alors une etiquette [inline] est inseree dans la documentation pour les membres inline.</translation>
    </message>
<message>
        <source>SORT_MEMBER_DOCS</source>
            <translation>Si la balise \c SORT_MEMBER_DOCS est definie a \c YES alors Doxygen triera la documentation (detaillee) des membres de fichier et de classe alphabetiquement par nom de membre. Si defini a \c NO, les membres apparaitront dans l'ordre de declaration.</translation>
    </message>
<message>
        <source>SORT_BRIEF_DOCS</source>
            <translation>Si la balise \c SORT_BRIEF_DOCS est definie a \c YES alors Doxygen triera les descriptions breves des membres de fichier, namespace et classe alphabetiquement par nom de membre. Si defini a \c NO, les membres apparaitront dans l'ordre de declaration. Notez que cela influencera egalement l'ordre des classes dans la liste des classes.</translation>
    </message>
<message>
        <source>SORT_MEMBERS_CTORS_1ST</source>
            <translation>Si la balise \c SORT_MEMBERS_CTORS_1ST est definie a \c YES alors Doxygen triera la documentation (breve et detaillee) des membres de classe de sorte que les constructeurs et destructeurs soient listes en premier. Si defini a \c NO les constructeurs apparaitront dans les ordres respectifs definis par \ref cfg_sort_brief_docs "SORT_BRIEF_DOCS" et \ref cfg_sort_member_docs "SORT_MEMBER_DOCS". \note Si \ref cfg_sort_brief_docs "SORT_BRIEF_DOCS" est defini a \c NO cette option est ignoree pour le tri de la documentation breve des membres. \note Si \ref cfg_sort_member_docs "SORT_MEMBER_DOCS" est defini a \c NO cette option est ignoree pour le tri de la documentation detaillee des membres.</translation>
    </message>
<message>
        <source>SORT_GROUP_NAMES</source>
            <translation>Si la balise \c SORT_GROUP_NAMES est definie a \c YES alors Doxygen triera la hierarchie des noms de groupes en ordre alphabetique. Si defini a \c NO les noms de groupes apparaitront dans leur ordre defini.</translation>
    </message>
<message>
        <source>SORT_BY_SCOPE_NAME</source>
            <translation>Si la balise \c SORT_BY_SCOPE_NAME est definie a \c YES, la liste des classes sera triee par noms completement qualifies, y compris les namespaces. Si defini a \c NO, la liste des classes sera triee uniquement par nom de classe, sans inclure la partie namespace. \note Cette option n'est pas tres utile si \ref cfg_hide_scope_names "HIDE_SCOPE_NAMES" est defini a \c YES. \note Cette option s'applique uniquement a la liste des classes, pas a la liste alphabetique.</translation>
    </message>
<message>
        <source>STRICT_PROTO_MATCHING</source>
            <translation>Si l'option \c STRICT_PROTO_MATCHING est activee et que Doxygen echoue a faire une resolution de type correcte de tous les parametres d'une fonction, il rejettera une correspondance entre le prototype et l'implementation d'une fonction membre meme s'il n'y a qu'un seul candidat ou s'il est evident quel candidat choisir en faisant une simple correspondance de chaine. En desactivant \c STRICT_PROTO_MATCHING, Doxygen acceptera toujours une correspondance entre prototype et implementation dans de tels cas.</translation>
    </message>
<message>
        <source>GENERATE_TODOLIST</source>
            <translation>La balise \c GENERATE_TODOLIST peut etre utilisee pour activer (\c YES) ou desactiver (\c NO) la liste des taches. Cette liste est creee en mettant des commandes \ref cmdtodo "\\todo" dans la documentation.</translation>
    </message>
<message>
        <source>GENERATE_TESTLIST</source>
            <translation>La balise \c GENERATE_TESTLIST peut etre utilisee pour activer (\c YES) ou desactiver (\c NO) la liste des tests. Cette liste est creee en mettant des commandes \ref cmdtest "\\test" dans la documentation.</translation>
    </message>
<message>
        <source>GENERATE_BUGLIST</source>
            <translation>La balise \c GENERATE_BUGLIST peut etre utilisee pour activer (\c YES) ou desactiver (\c NO) la liste des bugs. Cette liste est creee en mettant des commandes \ref cmdbug "\\bug" dans la documentation.</translation>
    </message>
<message>
        <source>GENERATE_DEPRECATEDLIST</source>
            <translation>La balise \c GENERATE_DEPRECATEDLIST peut etre utilisee pour activer (\c YES) ou desactiver (\c NO) la liste des elements deprecies. Cette liste est creee en mettant des commandes \ref cmddeprecated "\\deprecated" dans la documentation.</translation>
    </message>
<message>
        <source>GENERATE_REQUIREMENTS</source>
            <translation>La balise \c GENERATE_REQUIREMENTS peut etre utilisee pour activer (\c YES) ou desactiver (\c NO) la page des exigences. Lorsqu'activee, cette page est automatiquement creee lorsqu'au moins un bloc de commentaire avec une commande \ref cmdrequirement "\\requirement" apparait dans l'entree.</translation>
    </message>
<message>
        <source>REQ_TRACEABILITY_INFO</source>
            <translation>La balise \c REQ_TRACEABILITY_INFO controle si les informations de tracabilite sont affichees sur la page des exigences (pertinent uniquement lors de l'utilisation de blocs de commentaire \ref cmdrequirement "\\requirement"). Le parametre \c NO desactivera completement les informations de tracabilite. Le parametre \c UNSATISFIED_ONLY affichera une liste des exigences qui manquent d'une relation satisfies (via la commande: \ref cmdsatisfies "\\satisfies"). De meme le parametre \c UNVERIFIED_ONLY affichera une liste des exigences qui manquent d'une relation verifies (via la commande: \ref cmdverifies "\\verifies"). Definir la balise a \c YES (le defaut) affichera les deux listes si applicable.</translation>
    </message>
<message>
        <source>ENABLED_SECTIONS</source>
            <translation>La balise \c ENABLED_SECTIONS peut etre utilisee pour activer des sections de documentation conditionnelles, marquees par \ref cmdif "\\if" \&lt;section_label\&gt; ... \ref cmdendif "\\endif" et \ref cmdcond "\\cond" \&lt;section_label\&gt; ... \ref cmdendcond "\\endcond".</translation>
    </message>
<message>
        <source>MAX_INITIALIZER_LINES</source>
            <translation>La balise \c MAX_INITIALIZER_LINES determine le nombre maximal de lignes que la valeur initiale d'une variable ou macro / define peut avoir pour apparaitre dans la documentation. Si l'initialiseur consiste en plus de lignes que specifie ici, il sera cache. Utilisez une valeur de 0 pour cacher completement les initialiseurs. L'apparence de la valeur des variables individuelles et macros / defines peut etre controlee en utilisant la commande \ref cmdshowinitializer "\\showinitializer" ou \ref cmdhideinitializer "\\hideinitializer" dans la documentation independamment de ce parametre.</translation>
    </message>
<message>
        <source>SHOW_USED_FILES</source>
            <translation>Definissez la balise \c SHOW_USED_FILES a \c NO pour desactiver la liste des fichiers generes au bas de la documentation des classes et structs. Si defini a \c YES, la liste mentionnera les fichiers qui ont ete utilises pour generer la documentation.</translation>
    </message>
<message>
        <source>SHOW_FILES</source>
            <translation>Definissez la balise \c SHOW_FILES a \c NO pour desactiver la generation de la page Fichiers. Cela supprimera l'entree Fichiers de l'Index Rapide et de la Vue Arborescente des Dossiers (si specifie).</translation>
    </message>
<message>
        <source>SHOW_NAMESPACES</source>
            <translation>Definissez la balise \c SHOW_NAMESPACES a \c NO pour desactiver la generation de la page Namespaces. Cela supprimera l'entree Namespaces de l'Index Rapide et de la Vue Arborescente des Dossiers (si specifie).</translation>
    </message>
<message>
        <source>FILE_VERSION_FILTER</source>
            <translation>La balise \c FILE_VERSION_FILTER peut etre utilisee pour specifier un programme ou script que Doxygen devrait invoquer pour obtenir la version actuelle de chaque fichier (generalement a partir du systeme de controle de version). Doxygen invoquera le programme en executant (via &lt;code&gt;popen()&lt;/code&gt;) la commande &lt;code&gt;command input-file&lt;/code&gt;, ou \c command est la valeur de la balise \c FILE_VERSION_FILTER, et \c input-file est le nom d'un fichier d'entree fourni par Doxygen. Ce que le programme ecrit sur la sortie standard est utilise comme version de fichier.</translation>
    </message>
<message>
        <source>LAYOUT_FILE</source>
            <translation>La balise \c LAYOUT_FILE peut etre utilisee pour specifier un fichier de mise en page qui sera analyse par Doxygen. Le fichier de mise en page controle la structure globale des fichiers de sortie generes d'une maniere independante du format de sortie. Pour creer le fichier de mise en page qui represente les valeurs par defaut de Doxygen, executez Doxygen avec l'option `-l`. Vous pouvez optionnellement specifier un nom de fichier apres l'option, si omis \c DoxygenLayout.xml sera utilise comme nom du fichier de mise en page. Voir aussi la section \ref layout pour plus d'informations. &lt;br&gt;Notez que si vous executez Doxygen a partir d'un repertoire contenant un fichier appele \c DoxygenLayout.xml, Doxygen l'analysera automatiquement meme si la balise \c LAYOUT_FILE est laissee vide.</translation>
    </message>
<message>
        <source>CITE_BIB_FILES</source>
            <translation>La balise \c CITE_BIB_FILES peut etre utilisee pour specifier un ou plusieurs fichiers \c bib contenant les definitions de reference. Cela doit etre une liste de fichiers &lt;code&gt;.bib&lt;/code&gt;. L'extension &lt;code&gt;.bib&lt;/code&gt; est automatiquement ajoutee si omise. Cela necessite que l'outil \c bibtex soit installe. Voir aussi https://en.wikipedia.org/wiki/BibTeX pour plus d'infos. Pour \f$\mbox{\LaTeX}\f$ le style de la bibliographie peut etre controle en utilisant \ref cfg_latex_bib_style "LATEX_BIB_STYLE". Pour utiliser cette fonctionnalite vous avez besoin de \c bibtex et \c perl disponibles dans le chemin de recherche. Voir aussi \ref cmdcite "\\cite" pour plus d'infos sur la creation de references.</translation>
    </message>
<message>
        <source>EXTERNAL_TOOL_PATH</source>
            <translation>La balise \c EXTERNAL_TOOL_PATH peut etre utilisee pour etendre le chemin de recherche (variable d'environnement PATH) afin que des outils externes tels que \c latex et \c gs puissent etre trouves. \note Les repertoires specifies avec EXTERNAL_TOOL_PATH sont ajoutes devant le chemin deja specifie par la variable PATH, et sont ajoutes dans l'ordre specifie. \note Cette option est particulierement utile pour macOS version 14 (Sonoma) et superieures, lors de l'execution de Doxygen depuis Doxywizard, car dans ce cas toutes les modifications definies par l'utilisateur du PATH sont ignorees. Un exemple typique sur macOS est de definir \verbatim EXTERNAL_TOOL_PATH = /Library/TeX/texbin /usr/local/bin \endverbatim avec le chemin standard, le chemin de recherche complet utilise par doxygen lors du lancement d'outils externes deviendra alors \verbatim PATH=/Library/TeX/texbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin \endverbatim</translation>
    </message>
<message>
        <source>QUIET</source>
            <translation>La balise \c QUIET peut etre utilisee pour activer/desactiver les messages qui sont generes vers la sortie standard par Doxygen. Si \c QUIET est defini a \c YES cela implique que les messages sont desactives.</translation>
    </message>
<message>
        <source>WARNINGS</source>
            <translation>La balise \c WARNINGS peut etre utilisee pour activer/desactiver les messages d'avertissement qui sont generes vers la sortie d'erreur standard (\c stderr) par Doxygen. Si \c WARNINGS est defini a \c YES cela implique que les avertissements sont actives. &lt;br&gt; &lt;b&gt;Conseil:&lt;/b&gt; Activez les avertissements pendant l'ecriture de la documentation.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOCUMENTED</source>
            <translation>Si la balise \c WARN_IF_UNDOCUMENTED est definie a \c YES alors Doxygen generera des avertissements pour les membres non documentes. Si \ref cfg_extract_all "EXTRACT_ALL" est defini a \c YES alors ce drapeau sera automatiquement desactive.</translation>
    </message>
<message>
        <source>WARN_IF_DOC_ERROR</source>
            <translation>Si la balise \c WARN_IF_DOC_ERROR est definie a \c YES, Doxygen generera des avertissements pour les erreurs potentielles dans la documentation, comme documenter certains parametres dans une fonction documentee deux fois, ou documenter des parametres qui n'existent pas ou utiliser incorrectement les commandes de balisage.</translation>
    </message>
<message>
        <source>WARN_IF_INCOMPLETE_DOC</source>
            <translation>Si \c WARN_IF_INCOMPLETE_DOC est defini a \c YES, Doxygen avertira des documentations de parametres de fonction incompletes. Si defini a \c NO, Doxygen acceptera que certains parametres n'aient pas de documentation sans avertissement.</translation>
    </message>
<message>
        <source>WARN_NO_PARAMDOC</source>
            <translation>Cette option \c WARN_NO_PARAMDOC peut etre activee pour obtenir des avertissements pour les fonctions qui sont documentees, mais qui n'ont pas de documentation pour leurs parametres ou valeur de retour. Si defini a \c NO, Doxygen avertira uniquement des erreurs de documentation de parametres, mais pas de l'absence de documentation. Si \ref cfg_extract_all "EXTRACT_ALL" est defini a \c YES alors ce drapeau sera automatiquement desactive. Voir aussi \ref cfg_warn_if_incomplete_doc "WARN_IF_INCOMPLETE_DOC"</translation>
    </message>
<message>
        <source>WARN_IF_UNDOC_ENUM_VAL</source>
            <translation>Si l'option \c WARN_IF_UNDOC_ENUM_VAL est definie a \c YES, Doxygen avertira des valeurs d'enumeration non documentees. Si defini a \c NO, Doxygen acceptera les valeurs d'enumeration non documentees. Si \ref cfg_extract_all "EXTRACT_ALL" est defini a \c YES alors ce drapeau sera automatiquement desactive.</translation>
    </message>
<message>
        <source>WARN_LAYOUT_FILE</source>
            <translation>Si l'option \c WARN_LAYOUT_FILE est definie a \c YES, Doxygen avertira des problemes trouves lors de l'analyse du fichier de mise en page defini par l'utilisateur, tels que des elements manquants ou incorrects. Voir aussi \ref cfg_layout_file "LAYOUT_FILE" pour plus de details. Si defini a \c NO, les problemes avec le fichier de mise en page seront supprimes.</translation>
    </message>
<message>
        <source>WARN_AS_ERROR</source>
            <translation>Si la balise \c WARN_AS_ERROR est definie a \c YES alors Doxygen s'arretera immediatement lorsqu'un avertissement est rencontre. Si la balise \c WARN_AS_ERROR est definie a \c FAIL_ON_WARNINGS alors Doxygen continuera a s'executer comme si la balise \c WARN_AS_ERROR etait definie a \c NO, mais a la fin du processus Doxygen retournera un statut non nul. Si la balise \c WARN_AS_ERROR est definie a \c FAIL_ON_WARNINGS_PRINT alors Doxygen se comporte comme \c FAIL_ON_WARNINGS mais au cas ou aucun \ref cfg_warn_logfile "WARN_LOGFILE" n'est defini Doxygen n'ecrira pas les messages d'avertissement entre les autres messages mais les ecrira a la fin d'une execution, au cas ou un \ref cfg_warn_logfile "WARN_LOGFILE" est defini les messages d'avertissement seront en plus d'etre dans le fichier defini egalement affiches a la fin d'une execution, sauf si le \ref cfg_warn_logfile "WARN_LOGFILE" est defini comme `-` c'est-a-dire la sortie standard (`stdout`) dans ce cas le comportement restera comme avec le parametre \c FAIL_ON_WARNINGS.</translation>
    </message>
<message>
        <source>WARN_FORMAT</source>
            <translation>La balise \c WARN_FORMAT determine le format des messages d'avertissement que Doxygen peut produire. La chaine doit contenir les balises &lt;code&gt;\$file&lt;/code&gt;, &lt;code&gt;\$line&lt;/code&gt;, et &lt;code&gt;\$text&lt;/code&gt;, qui seront remplacees par le fichier et le numero de ligne d'ou l'avertissement a originate et le texte de l'avertissement. Optionnellement le format peut contenir &lt;code&gt;$version&lt;/code&gt;, qui sera remplace par la version du fichier (si elle a pu etre obtenue via \ref cfg_file_version_filter "FILE_VERSION_FILTER") \sa \ref cfg_warn_line_format "WARN_LINE_FORMAT"</translation>
    </message>
<message>
        <source>WARN_LINE_FORMAT</source>
            <translation>Dans la partie `$text` de la commande \ref cfg_warn_format "WARN_FORMAT" il est possible qu'une reference a un endroit plus specifique soit donnee. Pour faciliter le saut a cet endroit (en dehors de Doxygen) l'utilisateur peut definir une chaine "couper" / "coller" personnalisee. Exemple: \verbatim WARN_LINE_FORMAT = "'vi $file +$line'" \endverbatim \sa \ref cfg_warn_format "WARN_FORMAT"</translation>
    </message>
<message>
        <source>WARN_LOGFILE</source>
            <translation>La balise \c WARN_LOGFILE peut etre utilisee pour specifier un fichier dans lequel les messages d'avertissement et d'erreur doivent etre ecrits. Si laissee vide, la sortie est ecrite vers la sortie d'erreur standard (`stderr`). Au cas ou le fichier specifie ne peut pas etre ouvert pour ecriture, les messages d'avertissement et d'erreur sont ecrits vers la sortie d'erreur standard. Lorsqu'un fichier `-` est specifie, les messages d'avertissement et d'erreur sont ecrits vers la sortie standard (`stdout`).</translation>
    </message>
<message>
        <source>INPUT</source>
            <translation>La balise \c INPUT est utilisee pour specifier les fichiers et/ou repertoires qui contiennent les fichiers sources documentes. Vous pouvez entrer des noms de fichiers comme \c myfile.cpp ou des repertoires comme \c /usr/src/myproject. Separez les fichiers ou repertoires par des espaces. Voir aussi \ref cfg_file_patterns "FILE_PATTERNS" et \ref cfg_extension_mapping "EXTENSION_MAPPING" \note Si cette balise est vide, le repertoire courant est recherche.</translation>
    </message>
<message>
        <source>INPUT_ENCODING</source>
            <translation>Cette balise peut etre utilisee pour specifier l'encodage de caracteres des fichiers sources que Doxygen analyse. Interne Doxygen utilise l'encodage UTF-8. Doxygen utilise `libiconv` (ou le `iconv` integre dans `libc`) pour le transcodage. Voir &lt;a href="https://www.gnu.org/software/libiconv/"&gt;la documentation libiconv&lt;/a&gt; pour la liste des encodages possibles. \sa \ref cfg_input_file_encoding "INPUT_FILE_ENCODING"</translation>
    </message>
<message>
        <source>INPUT_FILE_ENCODING</source>
            <translation>Cette balise peut etre utilisee pour specifier l'encodage de caracteres des fichiers sources que Doxygen analyse. La balise \c INPUT_FILE_ENCODING peut etre utilisee pour specifier l'encodage de caracteres sur une base par modele de fichier. Doxygen comparera le nom du fichier avec chaque modele et appliquera l'encodage au lieu du \ref cfg_input_encoding "INPUT_ENCODING" par defaut s'il y a une correspondance. Les encodages de caracteres sont une liste de la forme: modele=encodage (comme `*.php=ISO-8859-1`). \sa \ref cfg_input_encoding "INPUT_ENCODING" pour plus d'informations sur les encodages supportes.</translation>
    </message>
<message>
        <source>FILE_PATTERNS</source>
            <translation>Si la valeur de la balise \ref cfg_input "INPUT" contient des repertoires, vous pouvez utiliser la balise \c FILE_PATTERNS pour specifier un ou plusieurs modeles de caracteres generiques (comme `*.cpp` et `*.h`) pour filtrer les fichiers sources dans les repertoires. &lt;br&gt;Notez que pour les extensions personnalisees ou non directement supportees vous devez egalement definir \ref cfg_extension_mapping "EXTENSION_MAPPING" pour l'extension sinon les fichiers ne sont pas lus par Doxygen. &lt;br&gt;Notez que la liste des modeles de fichiers verifies par defaut peut différer de la liste des \ref default_file_extension_mapping "correspondances d'extension de fichier par defaut". &lt;br&gt;Si laissee vide, les modeles suivants sont testes:</translation>
    </message>
<message>
        <source>RECURSIVE</source>
            <translation>La balise \c RECURSIVE peut etre utilisee pour specifier si les sous-repertoires doivent etre recherches pour les fichiers d'entree egalement.</translation>
    </message>
<message>
        <source>EXCLUDE</source>
            <translation>La balise \c EXCLUDE peut etre utilisee pour specifier les fichiers et/ou repertoires qui doivent etre exclus des fichiers sources \ref cfg_input "INPUT". De cette facon, vous pouvez facilement exclure un sous-repertoire d'une arborescence de repertoires dont la racine est specifiee avec la balise \ref cfg_input "INPUT". &lt;br&gt;Remarquez que les chemins relatifs sont relatifs au repertoire a partir duquel Doxygen est execute.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMLINKS</source>
            <translation>La balise \c EXCLUDE_SYMLINKS peut etre utilisee pour selectionner si les fichiers ou repertoires qui sont des liens symboliques (une fonctionnalite du systeme de fichiers Unix) sont exclus de l'entree.</translation>
    </message>
<message>
        <source>EXCLUDE_PATTERNS</source>
            <translation>Si la valeur de la balise \ref cfg_input "INPUT" contient des repertoires, vous pouvez utiliser la balise \c EXCLUDE_PATTERNS pour specifier un ou plusieurs modeles de caracteres generiques pour exclure certains fichiers de ces repertoires. &lt;br&gt;Notez que les caracteres generiques sont compares au fichier avec le chemin absolu, donc pour exclure tous les repertoires de test par exemple utilisez le modele `*``/test/``*`</translation>
    </message>
<message>
        <source>EXCLUDE_SYMBOLS</source>
            <translation>La balise \c EXCLUDE_SYMBOLS peut etre utilisee pour specifier un ou plusieurs noms de symboles (namespaces, classes, fonctions, etc.) qui doivent etre exclus de la sortie. Le nom de symbole peut etre un nom completement qualifie, un mot, ou si le caractere generique `*` est utilise, une sous-chaine. Exemples: `ANamespace`, `AClass`, `ANamespace::AClass`, `ANamespace::*Test`</translation>
    </message>
<message>
        <source>EXAMPLE_PATH</source>
            <translation>La balise \c EXAMPLE_PATH peut etre utilisee pour specifier un ou plusieurs fichiers ou repertoires qui contiennent des fragments de code exemple qui sont inclus (voir la commande \ref cmdinclude "\\include").</translation>
    </message>
<message>
        <source>EXAMPLE_PATTERNS</source>
            <translation>Si la valeur de la balise \ref cfg_example_path "EXAMPLE_PATH" contient des repertoires, vous pouvez utiliser la balise \c EXAMPLE_PATTERNS pour specifier un ou plusieurs modeles de caracteres generiques (comme `*.cpp` et `*.h`) pour filtrer les fichiers sources dans les repertoires. Si laissee vide, tous les fichiers sont inclus.</translation>
    </message>
<message>
        <source>EXAMPLE_RECURSIVE</source>
            <translation>Si la balise \c EXAMPLE_RECURSIVE est definie a \c YES alors les sous-repertoires seront recherches pour les fichiers d'entree a utiliser avec les commandes \ref cmdinclude "\\include" ou \ref cmddontinclude "\\dontinclude" independamment de la valeur de la balise \ref cfg_recursive "RECURSIVE".</translation>
    </message>
<message>
        <source>IMAGE_PATH</source>
            <translation>La balise \c IMAGE_PATH peut etre utilisee pour specifier un ou plusieurs fichiers ou repertoires qui contiennent des images qui doivent etre incluses dans la documentation (voir la commande \ref cmdimage "\\image").</translation>
    </message>
<message>
        <source>INPUT_FILTER</source>
            <translation>La balise \c INPUT_FILTER peut etre utilisee pour specifier un programme que Doxygen devrait invoquer pour filtrer chaque fichier d'entree. Doxygen invoquera le programme de filtre en executant (via &lt;code&gt;popen()&lt;/code&gt;) la commande: &lt;br&gt; &lt;code&gt;\&lt;filtre\&gt; \&lt;fichier-entree\&gt;&lt;/code&gt; &lt;br&gt; ou &lt;code&gt;\&lt;filtre\&gt;&lt;/code&gt; est la valeur de la balise \c INPUT_FILTER, et &lt;code&gt;\&lt;fichier-entree\&gt;&lt;/code&gt; est le nom d'un fichier d'entree. Doxygen utilisera ensuite la sortie que le programme de filtre ecrit sur la sortie standard. Si \ref cfg_filter_patterns "FILTER_PATTERNS" est specifie, cette balise sera ignoree. &lt;br&gt;Notez que le filtre ne doit pas ajouter ou supprimer de lignes; il est applique avant que le code ne soit scanne, mais pas lorsque le code de sortie est genere. Si des lignes sont ajoutees ou supprimees, les ancres ne seront pas placees correctement. &lt;br&gt;Notez que Doxygen utilisera les donnees traitees et ecrites sur la sortie standard pour un traitement ulterieur, donc rien d'autre, comme des instructions de debogage ou des commandes utilisees (donc dans le cas d'un fichier batch Windows utilisez toujours `@echo OFF`), ne doit etre ecrit sur la sortie standard. &lt;br&gt;Notez que pour les extensions personnalisees ou non directement supportees vous devez egalement definir \ref cfg_extension_mapping "EXTENSION_MAPPING" pour l'extension sinon les fichiers ne sont pas correctement traites par Doxygen.&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_PATTERNS</source>
            <translation>La balise \c FILTER_PATTERNS peut etre utilisee pour specifier des filtres sur une base par modele de fichier. Doxygen comparera le nom du fichier avec chaque modele et appliquera le filtre s'il y a une correspondance. Les filtres sont une liste de la forme: modele=filtre (comme `*.cpp=my_cpp_filter`). Voir \ref cfg_input_filter "INPUT_FILTER" pour plus d'informations sur l'utilisation des filtres. Si la balise \c FILTER_PATTERNS est vide ou si aucun des modeles ne correspond au nom du fichier, \ref cfg_input_filter "INPUT_FILTER" est applique. &lt;br&gt;Notez que pour les extensions personnalisees ou non directement supportees vous devez egalement definir \ref cfg_extension_mapping "EXTENSION_MAPPING" pour l'extension sinon les fichiers ne sont pas correctement traites par Doxygen.&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_SOURCE_FILES</source>
            <translation>Si la balise \c FILTER_SOURCE_FILES est definie a \c YES, le filtre d'entree (si defini en utilisant \ref cfg_input_filter "INPUT_FILTER") sera egalement utilise pour filtrer les fichiers d'entree qui sont utilises pour produire les fichiers sources a parcourir (c'est-a-dire lorsque \ref cfg_source_browser "SOURCE_BROWSER" est defini a \c YES).</translation>
    </message>
<message>
        <source>FILTER_SOURCE_PATTERNS</source>
            <translation>La balise \c FILTER_SOURCE_PATTERNS peut etre utilisee pour specifier des filtres sources par modele de fichier. Un modele remplacera le parametre pour \ref cfg_filter_patterns "FILTER_PATTERN" (s'il y en a) et il est egalement possible de desactiver le filtrage source pour un modele specifique en utilisant `*.ext=` (donc sans nommer de filtre).</translation>
    </message>
<message>
        <source>USE_MDFILE_AS_MAINPAGE</source>
            <translation>Si la balise \c USE_MDFILE_AS_MAINPAGE fait reference au nom d'un fichier markdown qui fait partie de l'entree, son contenu sera place sur la page principale (`index.html`). Cela peut etre utile si vous avez un projet par exemple sur GitHub et voulez reutiliser la page d'introduction egalement pour la sortie Doxygen.</translation>
    </message>
<message>
        <source>IMPLICIT_DIR_DOCS</source>
            <translation>Si la balise \c IMPLICIT_DIR_DOCS est definie a \c YES, tout fichier `README.md` trouve dans les sous-repertoires de la racine du projet est utilise comme documentation pour ce sous-repertoire, sauf si le `README.md` commence par une commande \ref cmddir "\\dir", \ref cmdpage "\\page" ou \ref cmdmainpage "\\mainpage". Si defini a \c NO, le fichier `README.md` doit commencer par une commande \ref cmddir "\\dir" explicite pour etre utilise comme documentation de repertoire.</translation>
    </message>
<message>
        <source>FORTRAN_COMMENT_AFTER</source>
            <translation>La norme Fortran specifie que pour le code Fortran format fixe, tous les caracteres a partir de la position 72 sont consideres comme des commentaires. Une extension courante est d'autoriser des lignes plus longues avant que le commentaire automatique ne commence. Le parametre \c FORTRAN_COMMENT_AFTER permettra egalement que des lignes plus longues puissent etre traitees avant que le commentaire automatique ne commence.</translation>
    </message>
<message>
        <source>SOURCE_BROWSER</source>
            <translation>Si la balise \c SOURCE_BROWSER est definie a \c YES, une liste de fichiers sources sera generee. Les entites documentees seront referencees croisees avec ces sources. &lt;br&gt;Remarque: Pour se debarrasser de tout le code source dans la sortie generee, assurez-vous que egalement \ref cfg_verbatim_headers "VERBATIM_HEADERS" est defini a \c NO.</translation>
    </message>
<message>
        <source>INLINE_SOURCES</source>
            <translation>Definir la balise \c INLINE_SOURCES a \c YES inclura le corps des fonctions, macros multilignes, enumerations ou variables a initialisation de liste directement dans la documentation.</translation>
    </message>
<message>
        <source>STRIP_CODE_COMMENTS</source>
            <translation>Definir la balise \c STRIP_CODE_COMMENTS a \c YES indiquera a Doxygen de cacher tous les blocs de commentaires speciaux des fragments de code source generes. Les commentaires normaux C, C++ et Fortran resteront toujours visibles.</translation>
    </message>
<message>
        <source>REFERENCED_BY_RELATION</source>
            <translation>Si la balise \c REFERENCED_BY_RELATION est definie a \c YES, alors pour chaque entite documentee, toutes les fonctions documentees la referencant seront listees.</translation>
    </message>
<message>
        <source>REFERENCES_RELATION</source>
            <translation>Si la balise \c REFERENCES_RELATION est definie a \c YES, alors pour chaque fonction documentee, toutes les entites documentees appelees/utilisees par cette fonction seront listees.</translation>
    </message>
<message>
        <source>REFERENCES_LINK_SOURCE</source>
            <translation>Si la balise \c REFERENCES_LINK_SOURCE est definie a \c YES et que la balise \ref cfg_source_browser "SOURCE_BROWSER" est definie a \c YES, alors les hyperliens des fonctions dans les listes \ref cfg_references_relation "REFERENCES_RELATION" et \ref cfg_referenced_by_relation "REFERENCED_BY_RELATION" pointeront vers le code source. Sinon, ils pointeront vers la documentation.</translation>
    </message>
<message>
        <source>SOURCE_TOOLTIPS</source>
            <translation>Si \c SOURCE_TOOLTIPS est active (le defaut), alors survoler un hyperlien dans le code source affichera une infobulle avec des informations supplementaires telles que le prototype, la description breve et les liens vers la definition et la documentation. Comme cela rendra le fichier HTML plus grand et le chargement des grands fichiers un peu plus lent, vous pouvez choisir de desactiver cette fonctionnalite.</translation>
    </message>
<message>
        <source>USE_HTAGS</source>
            <translation>Si la balise \c USE_HTAGS est definie a \c YES, les references au code source pointeront vers le HTML genere par l'outil \c htags(1) au lieu du navigateur de source integre de Doxygen. L'outil \c htags fait partie du systeme de balisage de source global de GNU (voir https://www.gnu.org/software/global/global.html). Vous aurez besoin de la version 4.8.6 ou superieure. &lt;br&gt; Pour l'utiliser, faites ce qui suit: -# Installez la derniere version de \c global -# Activez \ref cfg_source_browser "SOURCE_BROWSER" et \c USE_HTAGS dans le fichier de configuration -# Assurez-vous que \ref cfg_input "INPUT" pointe vers la racine de l'arborescence des sources -# Executez \c doxygen normalement &lt;br&gt; Doxygen invoquera \c htags (et qui invoquera a son tour \c gtags), donc ces outils doivent etre disponibles depuis la ligne de commande (c'est-a-dire dans le chemin de recherche). &lt;br&gt; Resultat: au lieu du navigateur de source genere par Doxygen, les liens vers le code source pointeront maintenant vers la sortie de \c htags.</translation>
    </message>
<message>
        <source>VERBATIM_HEADERS</source>
            <translation>Si la balise \c VERBATIM_HEADERS est definie a \c YES, Doxygen generera une copie verbatim du fichier d'en-tete pour chaque classe pour laquelle une inclusion est specifiee. Definir a \c NO pour desactiver cela. \sa Section \ref cmdclass "\class".</translation>
    </message>
<message>
        <source>CLANG_ASSISTED_PARSING</source>
            <translation>Si la balise \c CLANG_ASSISTED_PARSING est definie a \c YES, Doxygen utilisera l'&lt;a href="http://clang.llvm.org/"&gt;analyseur clang&lt;/a&gt; pour une analyse plus precise au prix de performances reduites. Cela peut etre particulierement utile avec le code C++ riche en templates pour lequel l'analyseur integre de Doxygen manque des informations de type necessaires. @note La disponibilite de cette option depend de si Doxygen a ete genere avec l'option `-Duse_libclang=ON` pour CMake.</translation>
    </message>
<message>
        <source>CLANG_ADD_INC_PATHS</source>
            <translation>Si la balise \c CLANG_ASSISTED_PARSING est definie a \c YES et que la balise \c CLANG_ADD_INC_PATHS est definie a \c YES, Doxygen ajoutera le repertoire de chaque entree au chemin d'inclusion.</translation>
    </message>
<message>
        <source>CLANG_OPTIONS</source>
            <translation>Si l'analyse assistee par clang est activee, vous pouvez fournir au compilateur des options de ligne de commande que vous utiliseriez normalement lors de l'appel du compilateur. Remarquez que les chemins d'inclusion seront deja definis par Doxygen pour les fichiers et repertoires specifies avec \ref cfg_input "INPUT" et \ref cfg_include_path "INCLUDE_PATH".</translation>
    </message>
<message>
        <source>CLANG_DATABASE_PATH</source>
            <translation>Si l'analyse assistee par clang est activee, vous pouvez fournir a l'analyseur clang le chemin vers le repertoire contenant un fichier appele `compile_commands.json`. Ce fichier est la &lt;a href="http://clang.llvm.org/docs/HowToSetupToolingForLLVM.html"&gt;base de donnees de compilation&lt;/a&gt; contenant les options utilisees lors de la construction des fichiers sources. Cela equivaut a specifier l'option `-p` a un outil clang, tel que `clang-check`. Ces options seront ensuite passees a l'analyseur. Toutes les options specifiees avec \ref cfg_clang_options "CLANG_OPTIONS" seront egalement ajoutees. @note La disponibilite de cette option depend de si Doxygen a ete genere avec l'option `-Duse_libclang=ON` pour CMake.</translation>
    </message>
<message>
        <source>ALPHABETICAL_INDEX</source>
            <translation>Si la balise \c ALPHABETICAL_INDEX est definie a \c YES, un index alphabetique de tous les composes sera genere. Activez cela si le projet contient beaucoup de classes, structures, unions ou interfaces.</translation>
    </message>
<message>
        <source>IGNORE_PREFIX</source>
            <translation>La balise \c IGNORE_PREFIX peut etre utilisee pour specifier un prefixe (ou une liste de prefixes) qui doit etre ignore lors de la generation des en-tetes d'index. La balise \c IGNORE_PREFIX fonctionne pour les noms de classes, fonctions et membres. L'entite sera placee dans la liste alphabetique sous la premiere lettre du nom de l'entite qui reste apres suppression du prefixe.</translation>
    </message>
<message>
        <source>GENERATE_HTML</source>
            <translation>Si la balise \c GENERATE_HTML est definie a \c YES, Doxygen generera une sortie HTML</translation>
    </message>
<message>
        <source>HTML_OUTPUT</source>
            <translation>La balise \c HTML_OUTPUT est utilisee pour specifier ou les docs HTML seront placees. Si un chemin relatif est entre, la valeur de \ref cfg_output_directory "OUTPUT_DIRECTORY" sera mise devant.</translation>
    </message>
<message>
        <source>HTML_FILE_EXTENSION</source>
            <translation>La balise \c HTML_FILE_EXTENSION peut etre utilisee pour specifier l'extension de fichier pour chaque page HTML generee (par exemple: &lt;code&gt;.htm, .php, .asp&lt;/code&gt;).</translation>
    </message>
<message>
        <source>HTML_HEADER</source>
            <translation>La balise \c HTML_HEADER peut etre utilisee pour specifier un fichier d'en-tete HTML defini par l'utilisateur pour chaque page HTML generee. Si la balise est laissee vide, Doxygen generera un en-tete standard. &lt;br&gt;Pour obtenir un HTML valide, le fichier d'en-tete doit inclure tous les scripts et feuilles de style dont Doxygen a besoin, ce qui depend des options de configuration utilisees (par exemple le parametre \ref cfg_generate_treeview "GENERATE_TREEVIEW"). Il est fortement recommande de commencer avec un en-tete par defaut en utilisant \verbatim doxygen -w html new_header.html new_footer.html new_stylesheet.css YourConfigFile \endverbatim puis de modifier le fichier \c new_header.html. Voir aussi la section \ref doxygen_usage pour plus d'informations sur la generation de l'en-tete par defaut que Doxygen utilise normalement. @note L'en-tete est sujet a changement, donc vous devez generalement regenerer l'en-tete par defaut lors de la mise a jour vers une version plus recente de Doxygen.</translation>
    </message>
<message>
        <source>HTML_FOOTER</source>
            <translation>La balise \c HTML_FOOTER peut etre utilisee pour specifier un pied de page HTML defini par l'utilisateur pour chaque page HTML generee. Si la balise est laissee vide, Doxygen generera un pied de page standard. Voir \ref cfg_html_header "HTML_HEADER" pour plus d'informations sur la generation d'un pied de page par defaut et les commandes speciales qui peuvent etre utilisees dans le pied de page. Voir aussi la section \ref doxygen_usage pour plus d'informations sur la generation du pied de page par defaut que Doxygen utilise normalement.</translation>
    </message>
<message>
        <source>HTML_STYLESHEET</source>
            <translation>La balise \c HTML_STYLESHEET peut etre utilisee pour specifier une feuille de style en cascade definie par l'utilisateur qui est utilisee par chaque page HTML. Elle peut etre utilisee pour affiner l'apparence de la sortie HTML. Si laissee vide, Doxygen generera une feuille de style par defaut. Voir aussi la section \ref doxygen_usage pour plus d'informations sur la generation de la feuille de style que Doxygen utilise normalement. \note Il est recommande d'utiliser \ref cfg_html_extra_stylesheet "HTML_EXTRA_STYLESHEET" au lieu de cette balise, car elle est plus robuste et cette balise (&lt;code&gt;HTML_STYLESHEET&lt;/code&gt;) deviendra obsolete a l'avenir.</translation>
    </message>
<message>
        <source>HTML_EXTRA_STYLESHEET</source>
            <translation>La balise \c HTML_EXTRA_STYLESHEET peut etre utilisee pour specifier des feuilles de style en cascade supplementaires definies par l'utilisateur qui sont incluses apres les feuilles de style standard creees par Doxygen. En utilisant cette option, on peut redefinir certains aspects de style. Ceci est preferable a l'utilisation de \ref cfg_html_stylesheet "HTML_STYLESHEET" car elle ne remplace pas la feuille de style standard et est donc plus robuste face aux mises a jour futures. Doxygen copiera les fichiers de feuille de style dans le repertoire de sortie. \note L'ordre des fichiers de feuille de style supplementaires est important (par exemple, la derniere feuille de style de la liste redefinit les parametres des precedentes dans la liste). \note Comme le style des barres de defilement ne peut actuellement pas etre remplace dans Webkit/Chromium, le style sera omis du doxygen.css par defaut si une ou plusieurs feuilles de style supplementaires ont ete specifiees. Donc si la personnalisation des barres de defilement est souhaitee, elle doit etre ajoutee explicitement.</translation>
    </message>
<message>
        <source>HTML_EXTRA_FILES</source>
            <translation>La balise \c HTML_EXTRA_FILES peut etre utilisee pour specifier un ou plusieurs fichiers images supplementaires ou autres fichiers sources qui doivent etre copies dans le repertoire de sortie HTML. Notez que ces fichiers seront copies dans le repertoire de sortie HTML de base. Utilisez le marqueur &lt;code&gt;$relpath^&lt;/code&gt; dans les fichiers \ref cfg_html_header "HTML_HEADER" et/ou \ref cfg_html_footer "HTML_FOOTER" pour charger ces fichiers. Dans le fichier \ref cfg_html_stylesheet "HTML_STYLESHEET", utilisez uniquement le nom du fichier. Notez egalement que les fichiers seront copies tels quels; il n'y a pas de commandes ou de marqueurs disponibles.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE</source>
            <translation>La balise \c HTML_COLORSTYLE peut etre utilisee pour specifier si la sortie HTML generee doit etre rendue avec un theme sombre ou clair.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_HUE</source>
            <translation>La balise \c HTML_COLORSTYLE_HUE controle la couleur de la sortie HTML. Doxygen ajustera les couleurs dans la feuille de style et les images d'arriere-plan en fonction de cette couleur. La teinte est specifiee comme un angle sur une roue chromatique, voir https://en.wikipedia.org/wiki/Hue pour plus d'informations. Par exemple, la valeur 0 represente le rouge, 60 le jaune, 120 le vert, 180 le cyan, 240 le bleu, 300 le violet et 360 le rouge a nouveau.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_SAT</source>
            <translation>La balise \c HTML_COLORSTYLE_SAT controle la purete (ou saturation) des couleurs dans la sortie HTML. Pour une valeur de 0, la sortie utilisera uniquement des niveaux de gris. Une valeur de 255 produira les couleurs les plus vives.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_GAMMA</source>
            <translation>La balise \c HTML_COLORSTYLE_GAMMA controle la correction gamma appliquee a la composante de luminance des couleurs dans la sortie HTML. Les valeurs inferieures a 100 rendent progressivement la sortie plus claire, tandis que les valeurs superieures a 100 rendent la sortie plus foncee. La valeur divisee par 100 est le gamma reel applique, donc 80 represente un gamma de 0.8, la valeur 220 represente un gamma de 2.2, et 100 ne change pas le gamma.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_MENUS</source>
            <translation>Si la balise \c HTML_DYNAMIC_MENUS est definie a \c YES, la documentation HTML generee contiendra un index principal avec des menus de navigation verticaux qui sont crees dynamiquement via JavaScript. Si desactive, l'index de navigation consistera en plusieurs niveaux d'onglets qui sont integres statiquement dans chaque page HTML. Desactivez cette option pour prendre en charge les navigateurs qui n'ont pas JavaScript, comme le navigateur d'aide Qt.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_SECTIONS</source>
            <translation>Si la balise \c HTML_DYNAMIC_SECTIONS est definie a \c YES, la documentation HTML generee contiendra des sections qui peuvent etre cachees et affichees apres le chargement de la page.</translation>
    </message>
<message>
        <source>HTML_CODE_FOLDING</source>
            <translation>Si la balise \c HTML_CODE_FOLDING est definie a \c YES, les classes et fonctions peuvent etre repliees et developpees dynamiquement dans le code source HTML genere.</translation>
    </message>
<message>
        <source>HTML_COPY_CLIPBOARD</source>
            <translation>Si la balise \c HTML_COPY_CLIPBOARD est definie a \c YES, Doxygen affichera une icone dans le coin superieur droit des fragments de code et de texte qui permet a l'utilisateur de copier son contenu dans le presse-papiers. Notez que cela ne fonctionne que si le navigateur le prend en charge et que la page Web est servie via un &lt;a href="https://www.w3.org/TR/secure-contexts/"&gt;contexte securise&lt;/a&gt;, c'est-a-dire en utilisant le protocole https: ou file:.</translation>
    </message>
<message>
        <source>HTML_PROJECT_COOKIE</source>
            <translation>Doxygen stocke quelques parametres de maniere persistante dans le navigateur (via par exemple les cookies). Par defaut, ces parametres s'appliquent a toutes les pages HTML generees par Doxygen pour tous les projets. La balise \c HTML_PROJECT_COOKIE peut etre utilisee pour stocker les parametres sous une cle specifique au projet, de sorte que les preferences de l'utilisateur seront stockees separement.</translation>
    </message>
<message>
        <source>HTML_INDEX_NUM_ENTRIES</source>
            <translation>Avec \c HTML_INDEX_NUM_ENTRIES, on peut controler le nombre prefere d'entrees affichees initialement dans les differents indices structures en arborescence; l'utilisateur peut developper et reduire les entrees dynamiquement plus tard. Doxygen developpera l'arbre a un tel niveau qu'au maximum le nombre specifie d'entrees sont visibles (sauf si un arbre completement reduit depasse deja ce montant). Donc definir le nombre d'entrees a 1 produira un arbre completement reduit par defaut. 0 est une valeur speciale representant un nombre infini d'entrees et resultera en un arbre completement developpe par defaut.</translation>
    </message>
<message>
        <source>GENERATE_DOCSET</source>
            <translation>Si la balise \c GENERATE_DOCSET est definie a \c YES, des fichiers d'index supplementaires seront generes qui peuvent etre utilises comme entree pour l'&lt;a href="https://developer.apple.com/xcode/"&gt;environnement de developpement integre Xcode 3 d'Apple&lt;/a&gt;, introduit avec OSX 10.5 (Leopard). Pour creer un ensemble de documentation, Doxygen generera un Makefile dans le repertoire de sortie HTML. L'execution de \c make produira le docset dans ce repertoire et l'execution de &lt;code&gt;make install&lt;/code&gt; installera le docset dans &lt;code&gt;~/Library/Developer/Shared/Documentation/DocSets&lt;/code&gt; afin que Xcode le trouve au demarrage. Voir https://developer.apple.com/library/archive/featuredarticles/DoxygenXcode/_index.html pour plus d'informations.</translation>
    </message>
<message>
        <source>DOCSET_FEEDNAME</source>
            <translation>Cette balise determine le nom du flux docset. Un flux de documentation fournit un parapluie sous lequel plusieurs ensembles de documentation d'un seul fournisseur (comme une entreprise ou une suite de produits) peuvent etre regroupes.</translation>
    </message>
<message>
        <source>DOCSET_FEEDURL</source>
            <translation>Cette balise determine l'URL du flux docset. Un flux de documentation fournit un parapluie sous lequel plusieurs ensembles de documentation d'un seul fournisseur (comme une entreprise ou une suite de produits) peuvent etre regroupes.</translation>
    </message>
<message>
        <source>DOCSET_BUNDLE_ID</source>
            <translation>Cette balise specifie une chaine qui doit identifier de maniere unique l'ensemble de documentation bundle. Il doit s'agir d'une chaine de style nom de domaine inverse, par exemple &lt;code&gt;com.mycompany.MyDocSet&lt;/code&gt;. Doxygen ajoutera &lt;code&gt;.docset&lt;/code&gt; au nom.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_ID</source>
            <translation>La balise \c DOCSET_PUBLISHER_ID specifie une chaine qui doit identifier de maniere unique l'editeur de la documentation. Il doit s'agir d'une chaine de style nom de domaine inverse, par exemple &lt;code&gt;com.mycompany.MyDocSet.documentation&lt;/code&gt;.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_NAME</source>
            <translation>La balise \c DOCSET_PUBLISHER_NAME identifie l'editeur de la documentation.</translation>
    </message>
<message>
        <source>GENERATE_HTMLHELP</source>
            <translation>Si la balise \c GENERATE_HTMLHELP est definie a \c YES, Doxygen genere trois fichiers d'index HTML supplementaires: \c index.hhp, \c index.hhc, et \c index.hhk. Le \c index.hhp est un fichier de projet qui peut etre lu par l'atelier d'aide HTML de Microsoft sur Windows. Au debut de 2021, Microsoft a mis hors ligne la page originale, avec notamment les liens de telechargement (l'atelier d'aide HTML etait deja en mode maintenance depuis de nombreuses annees). Vous pouvez telecharger l'atelier d'aide HTML depuis les archives Web a &lt;a href="http://web.archive.org/web/20160201063255/https://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe"&gt;executable d'installation&lt;/a&gt;. &lt;br&gt;L'atelier d'aide HTML contient un compilateur qui peut convertir toute la sortie HTML generee par Doxygen en un seul fichier HTML compile (`.chm`). Les fichiers HTML compiles sont maintenant utilises comme format d'aide Windows 98 et remplaceront l'ancien format d'aide Windows (`.hlp`) sur toutes les plateformes Windows a l'avenir. Les fichiers HTML compresses contiennent egalement un index, une table des matieres, et vous pouvez rechercher des mots dans la documentation. L'atelier HTML contient egalement une visionneuse pour les fichiers HTML compresses.</translation>
    </message>
<message>
        <source>CHM_FILE</source>
            <translation>La balise \c CHM_FILE peut etre utilisee pour specifier le nom du fichier `.chm` resultant. Vous pouvez ajouter un chemin devant le fichier si le resultat ne doit pas etre ecrit dans le repertoire de sortie html.</translation>
    </message>
<message>
        <source>HHC_LOCATION</source>
            <translation>La balise \c HHC_LOCATION peut etre utilisee pour specifier l'emplacement (chemin absolu incluant le nom de fichier) du compilateur d'aide HTML (\c hhc.exe). Si non vide, Doxygen essaiera d'executer le compilateur d'aide HTML sur le \c index.hhp genere.</translation>
    </message>
<message>
        <source>GENERATE_CHI</source>
            <translation>Le drapeau \c GENERATE_CHI controle si un fichier d'index `.chi` separe est genere (\c YES) ou s'il doit etre inclus dans le fichier `.chm` principal (\c NO).</translation>
    </message>
<message>
        <source>CHM_INDEX_ENCODING</source>
            <translation>Le \c CHM_INDEX_ENCODING est utilise pour encoder le contenu du fichier d'index HtmlHelp (\c hhk), de la table des matieres (\c hhc) et du fichier de projet.</translation>
    </message>
<message>
        <source>BINARY_TOC</source>
            <translation>Le drapeau \c BINARY_TOC controle si une table des matieres binaire est generee (\c YES) ou une table des matieres normale (\c NO) dans le fichier `.chm`. De plus, il active les boutons `Precedent` et `Suivant`.</translation>
    </message>
<message>
        <source>TOC_EXPAND</source>
            <translation>Le drapeau \c TOC_EXPAND peut etre defini a \c YES pour ajouter des elements supplementaires pour les membres de groupe a la table des matieres de la documentation d'aide HTML et a la vue en arborescence.</translation>
    </message>
<message>
        <source>SITEMAP_URL</source>
            <translation>La balise \c SITEMAP_URL est utilisee pour specifier l'URL complete de l'endroit ou la documentation generee sera placee sur le serveur par l'utilisateur lors du deploiement de la documentation. Le plan de site genere s'appelle `sitemap.xml` et est place dans le repertoire specifie par \ref cfg_html_output "HTML_OUTPUT". Si aucun \c SITEMAP_URL n'est specifie, aucun plan de site n'est genere. Pour plus d'informations sur le protocole de plan de site, voir https://www.sitemaps.org</translation>
    </message>
<message>
        <source>GENERATE_QHP</source>
            <translation>Si la balise \c GENERATE_QHP est definie a \c YES et que \ref cfg_qhp_namespace "QHP_NAMESPACE" et \ref cfg_qhp_virtual_folder "QHP_VIRTUAL_FOLDER" sont tous deux definis, un fichier d'index supplementaire sera genere qui peut etre utilise comme entree pour le qhelpgenerator de Qt pour generer une aide compresse Qt (`.qch`) de la documentation HTML generee.</translation>
    </message>
<message>
        <source>QCH_FILE</source>
            <translation>Si la balise \ref cfg_qhg_location "QHG_LOCATION" est specifiee, la balise \c QCH_FILE peut etre utilisee pour specifier le nom du fichier `.qch` resultant. Le chemin specifie est relatif au dossier de sortie HTML.</translation>
    </message>
<message>
        <source>QHP_NAMESPACE</source>
            <translation>La balise \c QHP_NAMESPACE specifie l'espace de noms a utiliser lors de la generation de la sortie du projet d'aide Qt. Pour plus d'informations, voir &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#namespace"&gt;Projet d'aide Qt / Namespace&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_VIRTUAL_FOLDER</source>
            <translation>La balise \c QHP_VIRTUAL_FOLDER specifie l'espace de noms a utiliser lors de la generation de la sortie du projet d'aide Qt. Pour plus d'informations, voir &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#virtual-folders"&gt;Projet d'aide Qt / Dossiers virtuels&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_NAME</source>
            <translation>Si la balise \c QHP_CUST_FILTER_NAME est definie, elle specifie le nom d'un filtre personnalise a ajouter. Pour plus d'informations, voir &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters"&gt;Projet d'aide Qt / Filtres personnalises&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_ATTRS</source>
            <translation>La balise \c QHP_CUST_FILTER_ATTRS specifie la liste des attributs du filtre personnalise a ajouter. Pour plus d'informations, voir &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters"&gt;Projet d'aide Qt / Filtres personnalises&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_SECT_FILTER_ATTRS</source>
            <translation>La balise \c QHP_SECT_FILTER_ATTRS specifie la liste des attributs avec lesquels la section de filtre de ce projet correspond. &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#filter-attributes"&gt;Projet d'aide Qt / Attributs de filtre&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHG_LOCATION</source>
            <translation>La balise \c QHG_LOCATION peut etre utilisee pour specifier l'emplacement (chemin absolu incluant le nom de fichier) du qhelpgenerator de Qt. Si non vide, Doxygen essaiera d'executer qhelpgenerator sur le fichier `.qhp` genere.</translation>
    </message>
<message>
        <source>GENERATE_ECLIPSEHELP</source>
            <translation>Si la balise \c GENERATE_ECLIPSEHELP est definie a \c YES, des fichiers d'index supplementaires seront generes, et avec les fichiers HTML, ils forment un plugin d'aide `Eclipse`. Pour installer ce plugin et le rendre disponible sous le menu du contenu d'aide dans `Eclipse`, le contenu du repertoire contenant les fichiers HTML et XML doit etre copie dans le repertoire des plugins d'eclipse. Le nom du repertoire dans le repertoire des plugins doit etre le meme que la valeur \ref cfg_eclipse_doc_id "ECLIPSE_DOC_ID". Apres la copie, `Eclipse` doit etre redemarre avant que l'aide n'apparaisse.</translation>
    </message>
<message>
        <source>ECLIPSE_DOC_ID</source>
            <translation>Un identifiant unique pour le plugin d'aide `Eclipse`. Lors de l'installation du plugin, le nom du repertoire contenant les fichiers HTML et XML doit egalement avoir ce nom. Chaque ensemble de documentation doit avoir son propre identifiant.</translation>
    </message>
<message>
        <source>DISABLE_INDEX</source>
            <translation>Si vous voulez un controle complet sur la mise en page des pages HTML generees, il peut etre necessaire de desactiver l'index et de le remplacer par le votre. La balise \c DISABLE_INDEX peut etre utilisee pour activer/desactiver l'index condense (onglets) en haut de chaque page HTML. Une valeur de \c NO active l'index et la valeur \c YES le desactive. Comme les onglets de l'index contiennent les memes informations que l'arbre de navigation, vous pouvez definir cette option a \c YES si vous definissez egalement \ref cfg_generate_treeview "GENERATE_TREEVIEW" a \c YES.</translation>
    </message>
<message>
        <source>GENERATE_TREEVIEW</source>
            <translation>La balise \c GENERATE_TREEVIEW est utilisee pour specifier si une structure d'index en arborescence doit etre generee pour afficher des informations hierarchiques. Si la valeur de la balise est definie a \c YES, un panneau lateral sera genere contenant une structure d'index en arborescence (tout comme celle qui est generee pour l'aide HTML). Pour que cela fonctionne, un navigateur qui prend en charge JavaScript, DHTML, CSS et les cadres est requis (c'est-a-dire tout navigateur moderne). Les utilisateurs de Windows sont probablement mieux lotis en utilisant la fonctionnalite d'aide HTML. Via des feuilles de style personnalisees (voir \ref cfg_html_extra_stylesheet "HTML_EXTRA_STYLESHEET"), on peut affiner davantage l'apparence de l'index (voir \ref doxygen_finetune). A titre d'exemple, la feuille de style par defaut generee par Doxygen a un exemple qui montre comment placer une image a la racine de l'arbre au lieu du \ref cfg_project_name "PROJECT_NAME". Comme l'arbre a fondamentalement plus d'informations detaillees que l'index des onglets, vous pouvez envisager de definir \ref cfg_disable_index "DISABLE_INDEX" a \c YES lors de l'activation de cette option.</translation>
    </message>
<message>
        <source>PAGE_OUTLINE_PANEL</source>
            <translation>Lorsque \ref cfg_generate_treeview "GENERATE_TREEVIEW" est defini a \c YES, l'option \c PAGE_OUTLINE_PANEL determine si un panneau de navigation supplementaire est affiche sur le cote droit de l'ecran, affichant un plan du contenu de la page principale, similaire a par exemple https://developer.android.com/reference. Si \ref cfg_generate_treeview "GENERATE_TREEVIEW" est defini a \c NO, cette option n'a aucun effet.</translation>
    </message>
<message>
        <source>FULL_SIDEBAR</source>
            <translation>Lorsque \ref cfg_generate_treeview "GENERATE_TREEVIEW" est defini a \c YES, l'option \c FULL_SIDEBAR determine si la barre laterale est limitee uniquement a la zone de l'arborescence (valeur \c NO) ou si elle doit s'etendre sur toute la hauteur de la fenetre (valeur \c YES). Definir ceci a \c YES donne une mise en page similaire a par exemple https://docs.readthedocs.io avec plus d'espace pour le contenu, mais moins d'espace pour le logo, le titre et la description du projet. Si \ref cfg_generate_treeview "GENERATE_TREEVIEW" est defini a \c NO, cette option n'a aucun effet.</translation>
    </message>
<message>
        <source>ENUM_VALUES_PER_LINE</source>
            <translation>La balise \c ENUM_VALUES_PER_LINE peut etre utilisee pour definir le nombre de valeurs d'enumeration que Doxygen regroupera sur une ligne dans la documentation HTML generee. &lt;br&gt;Notez qu'une valeur de 0 supprimera completement les valeurs d'enumeration de la section de vue d'ensemble.</translation>
    </message>
<message>
        <source>SHOW_ENUM_VALUES</source>
            <translation>Lorsque la balise \c SHOW_ENUM_VALUES est definie, Doxygen affichera les valeurs d'enumeration specifiees a cote des mnemoniques d'enumeration.</translation>
    </message>
<message>
        <source>TREEVIEW_WIDTH</source>
            <translation>Si l'arborescence est activee (voir \ref cfg_generate_treeview "GENERATE_TREEVIEW"), cette balise peut etre utilisee pour definir la largeur initiale (en pixels) du cadre dans lequel l'arbre est affiche.</translation>
    </message>
<message>
        <source>EXT_LINKS_IN_WINDOW</source>
            <translation>Si l'option \c EXT_LINKS_IN_WINDOW est definie a \c YES, Doxygen ouvrira les liens vers des symboles externes importes via des fichiers de balises dans une fenetre separee.</translation>
    </message>
<message>
        <source>OBFUSCATE_EMAILS</source>
            <translation>Si la balise \c OBFUSCATE_EMAILS est definie a \c YES, Doxygen offusquera les adresses email.</translation>
    </message>
<message>
        <source>HTML_FORMULA_FORMAT</source>
            <translation>Si l'option \c HTML_FORMULA_FORMAT est definie a \c svg, Doxygen utilisera l'outil pdf2svg (voir https://github.com/dawbarton/pdf2svg) ou inkscape (voir https://inkscape.org) pour generer des formules en tant qu'images SVG au lieu de PNG pour la sortie HTML. Ces images auront generalement une meilleure apparence a des resolutions mises a l'echelle.</translation>
    </message>
<message>
        <source>FORMULA_FONTSIZE</source>
            <translation>Utilisez cette balise pour changer la taille de police des formules \f$\mbox{\LaTeX}\f$ incluses en tant qu'images dans la documentation HTML. Lorsque vous changez la taille de police apres une execution Doxygen reussie, vous devez supprimer manuellement toutes les images `form_*.png` du repertoire de sortie HTML pour forcer leur regeneration.</translation>
    </message>
<message>
        <source>FORMULA_MACROFILE</source>
            <translation>Le \c FORMULA_MACROFILE peut contenir des commandes \f$\mbox{\LaTeX}\f$ `\newcommand` et `\renewcommand` pour creer de nouvelles commandes \f$\mbox{\LaTeX}\f$ a utiliser dans les formules comme blocs de construction. Voir la section \ref formulas pour plus de details.</translation>
    </message>
<message>
        <source>USE_MATHJAX</source>
            <translation>Activez l'option \c USE_MATHJAX pour rendre les formules \f$\mbox{\LaTeX}\f$ en utilisant MathJax (voir https://www.mathjax.org) qui utilise JavaScript cote client pour le rendu au lieu d'utiliser des bitmaps pre-rendus. Utilisez ceci si vous n'avez pas \f$\mbox{\LaTeX}\f$ installe ou si vous voulez que les formules soient plus jolies dans la sortie HTML. Lorsqu'active, vous devrez peut-etre egalement installer MathJax separement et configurer le chemin d'acces en utilisant l'option \ref cfg_mathjax_relpath "MATHJAX_RELPATH".</translation>
    </message>
<message>
        <source>MATHJAX_VERSION</source>
            <translation>Avec \c MATHJAX_VERSION, il est possible de specifier la version de MathJax a utiliser. Notez que les differentes versions de MathJax ont des exigences differentes en ce qui concerne les differents parametres, il est donc possible que d'autres parametres MathJax doivent egalement etre modifies lors du passage entre les differentes versions de MathJax.</translation>
    </message>
<message>
        <source>MATHJAX_FORMAT</source>
            <translation>Lorsque MathJax est active, vous pouvez definir le format de sortie par defaut a utiliser pour la sortie MathJax. Pour plus de details sur le format de sortie, voir &lt;a href="https://docs.mathjax.org/en/v2.7/output.html"&gt;MathJax version 2&lt;/a&gt;, &lt;a href="https://docs.mathjax.org/en/v3.2/output/index.html"&gt;MathJax version 3&lt;/a&gt; et &lt;a href="https://docs.mathjax.org/en/v4.0/output/index.htm"&gt;MathJax version 4&lt;/a&gt;.</translation>
    </message>
<message>
        <source>MATHJAX_RELPATH</source>
            <translation>Lorsque MathJax est active, vous devez specifier l'emplacement relatif au repertoire de sortie HTML en utilisant l'option \c MATHJAX_RELPATH. Pour la version 2 de Mathjax, le repertoire de destination doit contenir le script `MathJax.js`. Par exemple, si le repertoire \c mathjax est situe au meme niveau que le repertoire de sortie HTML, alors \c MATHJAX_RELPATH doit etre &lt;code&gt;../mathjax&lt;/code&gt;. Pour les versions 3 et 4 de Mathjax, le repertoire de destination doit contenir le script `tex-&lt;format&gt;.js` (ou `&lt;format&gt;` est soit `chtml` soit `svg`). La valeur par defaut pointe vers le reseau de diffusion de contenu MathJax afin que vous puissiez voir rapidement le resultat sans installer MathJax. Cependant, il est fortement recommande d'installer une copie locale de MathJax depuis https://www.mathjax.org avant le deploiement. La valeur par defaut est: - dans le cas de la version 2 de MathJax: https://cdn.jsdelivr.net/npm/mathjax@2 - dans le cas de la version 3 de MathJax: https://cdn.jsdelivr.net/npm/mathjax@3 - dans le cas de la version 4 de MathJax: https://cdn.jsdelivr.net/npm/mathjax@4</translation>
    </message>
<message>
        <source>MATHJAX_EXTENSIONS</source>
            <translation>La balise \c MATHJAX_EXTENSIONS peut etre utilisee pour specifier un ou plusieurs noms d'extension MathJax qui doivent etre actives pendant le rendu MathJax. Par exemple pour la version 2 de MathJax (voir https://docs.mathjax.org/en/v2.7/tex.html): \verbatim MATHJAX_EXTENSIONS = TeX/AMSmath TeX/AMSsymbols \endverbatim Par exemple pour la version 3 de MathJax (voir https://docs.mathjax.org/en/v3.2/input/tex/extensions/): \verbatim MATHJAX_EXTENSIONS = ams \endverbatim Par exemple pour la version 4 de MathJax (voir https://docs.mathjax.org/en/v4.0/input/tex/extensions/): \verbatim MATHJAX_EXTENSIONS = units \endverbatim Notez que pour la version 4 de Mathjax, quelques extensions sont deja automatiquement chargees. Pour desactiver un paquet dans la version 4 de Mathjax, on peut utiliser le nom du paquet precede d'un signe moins (`-` comme `MATHJAX_EXTENSIONS += -textmacros`)</translation>
    </message>
<message>
        <source>MATHJAX_CODEFILE</source>
            <translation>La balise \c MATHJAX_CODEFILE peut etre utilisee pour specifier un fichier avec des morceaux de code JavaScript qui seront utilises au demarrage du code MathJax. Voir le site Mathjax pour plus de details: - &lt;a href="https://docs.mathjax.org/en/v2.7/"&gt;MathJax version 2&lt;/a&gt; - &lt;a href="https://docs.mathjax.org/en/v3.2/"&gt;MathJax version 3&lt;/a&gt; - &lt;a href="https://docs.mathjax.org/en/v4.0/"&gt;MathJax version 4&lt;/a&gt;</translation>
    </message>
<message>
        <source>SEARCHENGINE</source>
            <translation>Lorsque la balise \c SEARCHENGINE est activee, Doxygen generera une boite de recherche pour la sortie HTML. Le moteur de recherche sous-jacent utilise JavaScript et DHTML et devrait fonctionner sur tout navigateur moderne. Notez que lors de l'utilisation de l'aide HTML (\ref cfg_generate_htmlhelp "GENERATE_HTMLHELP"), de l'aide Qt (\ref cfg_generate_qhp "GENERATE_QHP") ou des docsets (\ref cfg_generate_docset "GENERATE_DOCSET"), il existe deja une fonction de recherche, celle-ci devrait donc generalement etre desactivee. Pour les grands projets, le moteur de recherche base sur JavaScript peut etre lent, alors l'activation de \ref cfg_server_based_search "SERVER_BASED_SEARCH" peut fournir une meilleure solution. Il est possible de rechercher en utilisant le clavier; pour acceder a la boite de recherche, utilisez &lt;code&gt;\&lt;touche d'acces\&gt; + S&lt;/code&gt; (ce qu'est la &lt;code&gt;\&lt;touche d'acces\&gt;&lt;/code&gt; depend de l'OS et du navigateur, mais c'est generalement &lt;code&gt;\&lt;CTRL\&gt;&lt;/code&gt;, &lt;code&gt;\&lt;ALT\&gt;&lt;/code&gt;/&lt;code&gt;\&lt;option\&gt;&lt;/code&gt;, ou les deux). Dans la boite de recherche, utilisez la &lt;code&gt;\&lt;touche curseur bas\&gt;&lt;/code&gt; pour acceder a la fenetre des resultats de recherche, les resultats peuvent etre navigues en utilisant les &lt;code&gt;\&lt;touches curseur\&gt;&lt;/code&gt;. Appuyez sur &lt;code&gt;\&lt;Entree\&gt;&lt;/code&gt; pour selectionner un element ou &lt;code&gt;\&lt;echap\&gt;&lt;/code&gt; pour annuler la recherche. Les options de filtre peuvent etre selectionnees lorsque le curseur est dans la boite de recherche en appuyant sur &lt;code&gt;\&lt;Maj\&gt;+\&lt;curseur bas\&gt;&lt;/code&gt;. Ici aussi, utilisez les &lt;code&gt;\&lt;touches curseur\&gt;&lt;/code&gt; pour selectionner un filtre et &lt;code&gt;\&lt;Entree\&gt;&lt;/code&gt; ou &lt;code&gt;\&lt;echap\&gt;&lt;/code&gt; pour activer ou annuler l'option de filtre.</translation>
    </message>
<message>
        <source>SERVER_BASED_SEARCH</source>
            <translation>Lorsque la balise \c SERVER_BASED_SEARCH est activee, le moteur de recherche sera implemente en utilisant un serveur Web au lieu d'un client Web utilisant JavaScript. Il existe deux types de recherche basee sur un serveur Web selon le parametre \ref cfg_external_search "EXTERNAL_SEARCH". Lorsqu'il est desactive, Doxygen generera un script PHP pour la recherche et un fichier d'index utilise par le script. Lorsque \ref cfg_external_search "EXTERNAL_SEARCH" est active, l'indexation et la recherche doivent etre fournies par des outils externes. Voir la section \ref extsearch pour plus de details.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH</source>
            <translation>Lorsque la balise \c EXTERNAL_SEARCH est activee, Doxygen ne generera plus le script PHP pour la recherche. Au lieu de cela, les resultats de recherche sont ecrits dans un fichier XML qui doit etre traite par un indexeur externe. Doxygen invoquera un moteur de recherche externe pointe par l'option \ref cfg_searchengine_url "SEARCHENGINE_URL" pour obtenir les resultats de recherche. &lt;br&gt;Doxygen est fourni avec un exemple d'indexeur (\c doxyindexer) et de moteur de recherche (&lt;code&gt;doxysearch.cgi&lt;/code&gt;) qui sont bases sur la bibliotheque de moteur de recherche open source &lt;a href="https://xapian.org/"&gt;Xapian&lt;/a&gt;. &lt;br&gt;Voir la section \ref extsearch pour plus de details.</translation>
    </message>
<message>
        <source>SEARCHENGINE_URL</source>
            <translation>Le \c SEARCHENGINE_URL doit pointer vers un moteur de recherche heberge par un serveur Web qui retourner les resultats de recherche lorsque \ref cfg_external_search "EXTERNAL_SEARCH" est active. &lt;br&gt;Doxygen est fourni avec un exemple d'indexeur (\c doxyindexer) et de moteur de recherche (&lt;code&gt;doxysearch.cgi&lt;/code&gt;) qui sont bases sur la bibliotheque de moteur de recherche open source &lt;a href="https://xapian.org/"&gt;Xapian&lt;/a&gt;. Voir la section \ref extsearch pour plus de details.</translation>
    </message>
<message>
        <source>SEARCHDATA_FILE</source>
            <translation>Lorsque \ref cfg_server_based_search "SERVER_BASED_SEARCH" et \ref cfg_external_search "EXTERNAL_SEARCH" sont tous deux actives, les donnees de recherche non indexees sont ecrites dans un fichier pour l'indexation par un outil externe. Avec la balise \c SEARCHDATA_FILE, le nom de ce fichier peut etre specifie.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH_ID</source>
            <translation>Lorsque \ref cfg_server_based_search "SERVER_BASED_SEARCH" et \ref cfg_external_search "EXTERNAL_SEARCH" sont tous deux actives, la balise \c EXTERNAL_SEARCH_ID peut etre utilisee comme identifiant pour le projet. Ceci est utile en combinaison avec \ref cfg_extra_search_mappings "EXTRA_SEARCH_MAPPINGS" pour rechercher dans plusieurs projets et rediriger les resultats vers le bon projet.</translation>
    </message>
<message>
        <source>EXTRA_SEARCH_MAPPINGS</source>
            <translation>La balise \c EXTRA_SEARCH_MAPPINGS peut etre utilisee pour activer la recherche dans des projets Doxygen autres que celui defini par ce fichier de configuration, mais qui sont tous ajoutes au meme index de recherche externe. Chaque projet doit avoir un identifiant unique defini via \ref cfg_external_search_id "EXTERNAL_SEARCH_ID". Le mappage de recherche mappe ensuite l'identifiant vers un emplacement relatif ou la documentation peut etre trouvee. Le format est: \verbatim EXTRA_SEARCH_MAPPINGS = tagname1=loc1 tagname2=loc2 ... \endverbatim</translation>
    </message>
<message>
        <source>GENERATE_LATEX</source>
            <translation>Si la balise \c GENERATE_LATEX est definie a \c YES, Doxygen generera une sortie \f$\mbox{\LaTeX}\f$.</translation>
    </message>
<message>
        <source>LATEX_OUTPUT</source>
            <translation>La balise \c LATEX_OUTPUT est utilisee pour specifier ou les docs \f$\mbox{\LaTeX}\f$ seront placees. Si un chemin relatif est entre, la valeur de \ref cfg_output_directory "OUTPUT_DIRECTORY" sera mise devant.</translation>
    </message>
<message>
        <source>LATEX_CMD_NAME</source>
            <translation>La balise \c LATEX_CMD_NAME peut etre utilisee pour specifier le nom de la commande \f$\mbox{\LaTeX}\f$ a invoquer. &lt;br&gt;Notez que lorsque \ref cfg_use_pdflatex "USE_PDFLATEX" n'est pas active, le defaut est \c latex, lorsque \ref cfg_use_pdflatex "USE_PDFLATEX" est active, le defaut est \c pdflatex et dans ce dernier cas si \c latex est choisi, il est ecrase par \c pdflatex. Pour des langages de sortie specifiques, le defaut peut avoir ete defini differemment, cela depend de l'implementation du langage de sortie.</translation>
    </message>
<message>
        <source>MAKEINDEX_CMD_NAME</source>
            <translation>La balise \c MAKEINDEX_CMD_NAME peut etre utilisee pour specifier le nom de la commande pour generer l'index pour \f$\mbox{\LaTeX}\f$. @note Cette balise est utilisee dans le `Makefile` / `make.bat`. \sa \ref cfg_latex_makeindex_cmd "LATEX_MAKEINDEX_CMD" pour la partie dans le fichier de sortie genere (`.tex`).</translation>
    </message>
<message>
        <source>LATEX_MAKEINDEX_CMD</source>
            <translation>La balise \c LATEX_MAKEINDEX_CMD peut etre utilisee pour specifier le nom de la commande pour generer l'index pour \f$\mbox{\LaTeX}\f$. S'il n'y a pas de barre oblique inverse (`\`) comme premier caractere, elle sera automatiquement ajoutee dans le code \f$\mbox{\LaTeX}\f$. @note Cette balise est utilisee dans le fichier de sortie genere (`.tex`). \sa \ref cfg_makeindex_cmd_name "MAKEINDEX_CMD_NAME" pour la partie dans le `Makefile` / `make.bat`.</translation>
    </message>
<message>
        <source>COMPACT_LATEX</source>
            <translation>Si la balise \c COMPACT_LATEX est definie a \c YES, Doxygen genere des documents \f$\mbox{\LaTeX}\f$ plus compacts. Cela peut etre utile pour les petits projets et peut aider a economiser quelques arbres en general.</translation>
    </message>
<message>
        <source>PAPER_TYPE</source>
            <translation>La balise \c PAPER_TYPE peut etre utilisee pour definir le type de papier utilise par l'imprimante.</translation>
    </message>
<message>
        <source>EXTRA_PACKAGES</source>
            <translation>La balise \c EXTRA_PACKAGES peut etre utilisee pour specifier un ou plusieurs noms de paquets \f$\mbox{\LaTeX}\f$ qui doivent etre inclus dans la sortie \f$\mbox{\LaTeX}\f$. Le paquet peut etre specifie simplement par son nom ou avec la syntaxe correcte a utiliser avec la commande \f$\mbox{\LaTeX}\f$ `\usepackage`. Pour obtenir la police `times` par exemple, vous pouvez specifier: \verbatim EXTRA_PACKAGES=times ou EXTRA_PACKAGES={times} \endverbatim Pour utiliser l'option `intlimits` avec le paquet `amsmath`, vous pouvez specifier: \verbatim EXTRA_PACKAGES=[intlimits]{amsmath} \endverbatim Si laisse vide, aucun paquet supplementaire ne sera inclus.</translation>
    </message>
<message>
        <source>LATEX_HEADER</source>
            <translation>La balise \c LATEX_HEADER peut etre utilisee pour specifier un en-tete \f$\mbox{\LaTeX}\f$ defini par l'utilisateur pour le document \f$\mbox{\LaTeX}\f$ genere. L'en-tete doit contenir tout jusqu'au premier chapitre. S'il est laisse vide, Doxygen generera un en-tete standard. Il est fortement recommande de commencer avec un en-tete par defaut en utilisant \verbatim doxygen -w latex new_header.tex new_footer.tex new_stylesheet.sty \endverbatim puis de modifier le fichier \c new_header.tex. Voir aussi la section \ref doxygen_usage pour plus d'informations sur la generation de l'en-tete par defaut que Doxygen utilise normalement. &lt;br&gt;Note: N'utilisez un en-tete defini par l'utilisateur que si vous savez ce que vous faites! @note L'en-tete est sujet a changement, donc vous devez generalement regenerer l'en-tete par defaut lors de la mise a jour vers une version plus recente de Doxygen. Les commandes suivantes ont une signification speciale dans l'en-tete (et le pied de page):</translation>
    </message>
<message>
        <source>LATEX_FOOTER</source>
            <translation>La balise \c LATEX_FOOTER peut etre utilisee pour specifier un pied de page \f$\mbox{\LaTeX}\f$ defini par l'utilisateur pour le document \f$\mbox{\LaTeX}\f$ genere. Le pied de page doit contenir tout apres le dernier chapitre. S'il est laisse vide, Doxygen generera un pied de page standard. Voir \ref cfg_latex_header "LATEX_HEADER" pour plus d'informations sur la generation d'un pied de page par defaut et les commandes speciales qui peuvent etre utilisees dans le pied de page. Voir aussi la section \ref doxygen_usage pour plus d'informations sur la generation du pied de page par defaut que Doxygen utilise normalement. Note: N'utilisez un pied de page defini par l'utilisateur que si vous savez ce que vous faites!</translation>
    </message>
<message>
        <source>LATEX_EXTRA_STYLESHEET</source>
            <translation>La balise \c LATEX_EXTRA_STYLESHEET peut etre utilisee pour specifier des feuilles de style \f$\mbox{\LaTeX}\f$ supplementaires definies par l'utilisateur qui sont incluses apres les feuilles de style standard creees par Doxygen. En utilisant cette option, on peut redefinir certains aspects de style. Doxygen copiera les fichiers de feuille de style dans le repertoire de sortie. \note L'ordre des fichiers de feuille de style supplementaires est important (par exemple, la derniere feuille de style de la liste redefinit les parametres des precedentes dans la liste).</translation>
    </message>
<message>
        <source>LATEX_EXTRA_FILES</source>
            <translation>La balise \c LATEX_EXTRA_FILES peut etre utilisee pour specifier une ou plusieurs images supplementaires ou autres fichiers sources qui doivent etre copies dans le repertoire de sortie \ref cfg_latex_output "LATEX_OUTPUT". Notez que les fichiers seront copies tels quels; il n'y a pas de commandes ou de marqueurs disponibles.</translation>
    </message>
<message>
        <source>PDF_HYPERLINKS</source>
            <translation>Si la balise \c PDF_HYPERLINKS est definie a \c YES, le \f$\mbox{\LaTeX}\f$ genere est prepare pour la conversion en PDF (en utilisant \c ps2pdf ou \c pdflatex). Le fichier PDF contiendra des liens (tout comme la sortie HTML) au lieu de references de pages. Cela rend la sortie adaptee a la navigation en ligne en utilisant un lecteur PDF.</translation>
    </message>
<message>
        <source>USE_PDFLATEX</source>
            <translation>Si la balise \c USE_PDFLATEX est definie a \c YES, Doxygen utilisera le moteur comme specifie avec \ref cfg_latex_cmd_name "LATEX_CMD_NAME" pour generer le fichier PDF directement a partir des fichiers \f$\mbox{\LaTeX}\f$. Definissez cette option a \c YES pour obtenir une documentation PDF de meilleure qualite. &lt;br&gt;Voir aussi la section \ref cfg_latex_cmd_name "LATEX_CMD_NAME" pour selectionner le moteur.</translation>
    </message>
<message>
        <source>LATEX_BATCHMODE</source>
            <translation>La balise \c LATEX_BATCHMODE signale le comportement de \f$\mbox{\LaTeX}\f$ en cas d'erreur.</translation>
    </message>
<message>
        <source>LATEX_HIDE_INDICES</source>
            <translation>Si la balise \c LATEX_HIDE_INDICES est definie a \c YES, Doxygen n'inclura pas les chapitres d'index (tels que l'index des fichiers, l'index des composes, etc.) dans la sortie.</translation>
    </message>
<message>
        <source>LATEX_BIB_STYLE</source>
            <translation>La balise \c LATEX_BIB_STYLE peut etre utilisee pour specifier le style a utiliser pour la bibliographie, par exemple \c plainnat ou \c ieeetr. Voir https://en.wikipedia.org/wiki/BibTeX et \ref cmdcite "\cite" pour plus d'informations.</translation>
    </message>
<message>
        <source>LATEX_EMOJI_DIRECTORY</source>
            <translation>La balise \c LATEX_EMOJI_DIRECTORY est utilisee pour specifier le chemin (relatif ou absolu) a partir duquel les images emoji seront lues. Si un chemin relatif est entre, il sera relatif au repertoire \ref cfg_latex_output "LATEX_OUTPUT". Si laisse vide, le repertoire \ref cfg_latex_output "LATEX_OUTPUT" sera utilise.</translation>
    </message>
<message>
        <source>GENERATE_RTF</source>
            <translation>Si la balise \c GENERATE_RTF est definie a \c YES, Doxygen generera une sortie RTF. La sortie RTF est optimisee pour Word 97 et peut ne pas sembler tres jolie avec d'autres lecteurs/editeurs RTF.</translation>
    </message>
<message>
        <source>RTF_OUTPUT</source>
            <translation>La balise \c RTF_OUTPUT est utilisee pour specifier ou les docs RTF seront placees. Si un chemin relatif est entre, la valeur de \ref cfg_output_directory "OUTPUT_DIRECTORY" sera mise devant.</translation>
    </message>
<message>
        <source>COMPACT_RTF</source>
            <translation>Si la balise \c COMPACT_RTF est definie a \c YES, Doxygen genere des documents RTF plus compacts. Cela peut etre utile pour les petits projets et peut aider a economiser quelques arbres en general.</translation>
    </message>
<message>
        <source>RTF_HYPERLINKS</source>
            <translation>Si la balise \c RTF_HYPERLINKS est definie a \c YES, le RTF genere contiendra des champs de lien hypertexte. Le fichier RTF contiendra des liens (tout comme la sortie HTML) au lieu de references de pages. Cela rend la sortie adaptee a la navigation en ligne en utilisant Word ou d'autres lecteurs compatibles Word qui prennent en charge ces champs. &lt;br&gt;Note: WordPad (write) et d'autres ne prennent pas en charge les liens.</translation>
    </message>
<message>
        <source>RTF_STYLESHEET_FILE</source>
            <translation>Charge les definitions de feuille de style a partir d'un fichier. La syntaxe est similaire au fichier de configuration de Doxygen, c'est-a-dire une serie d'affectations. Vous devez uniquement fournir des remplacements, les definitions manquantes sont definies a leur valeur par defaut. &lt;br&gt;Voir aussi la section \ref doxygen_usage pour plus d'informations sur la generation de la feuille de style par defaut que Doxygen utilise normalement.</translation>
    </message>
<message>
        <source>RTF_EXTENSIONS_FILE</source>
            <translation>Definit des variables optionnelles utilisees dans la generation d'un document RTF. La syntaxe est similaire au fichier de configuration de Doxygen. Un fichier d'extensions modele peut etre genere en utilisant &lt;code&gt;doxygen -e rtf extensionFile&lt;/code&gt;.</translation>
    </message>
<message>
        <source>RTF_EXTRA_FILES</source>
            <translation>La balise \c RTF_EXTRA_FILES peut etre utilisee pour specifier une ou plusieurs images supplementaires ou autres fichiers sources qui doivent etre copies dans le repertoire de sortie \ref cfg_rtf_output "RTF_OUTPUT". Notez que les fichiers seront copies tels quels; il n'y a pas de commandes ou de marqueurs disponibles.</translation>
    </message>
<message>
        <source>GENERATE_MAN</source>
            <translation>Si la balise \c GENERATE_MAN est definie a \c YES, Doxygen generera des pages de manuel pour les classes et les fichiers.</translation>
    </message>
<message>
        <source>MAN_OUTPUT</source>
            <translation>La balise \c MAN_OUTPUT est utilisee pour specifier ou les pages de manuel seront placees. Si un chemin relatif est entre, la valeur de \ref cfg_output_directory "OUTPUT_DIRECTORY" sera mise devant. Un repertoire \c man3 sera cree dans le repertoire specifie par \c MAN_OUTPUT.</translation>
    </message>
<message>
        <source>MAN_EXTENSION</source>
            <translation>La balise \c MAN_EXTENSION determine l'extension qui est ajoutee aux pages de manuel generees. Si la section du manuel ne commence pas par un chiffre, le chiffre 3 est ajoute au debut. Le point (.) au debut de la balise \c MAN_EXTENSION est optionnel.</translation>
    </message>
<message>
        <source>MAN_SUBDIR</source>
            <translation>La balise \c MAN_SUBDIR determine le nom du repertoire cree dans \c MAN_OUTPUT dans lequel les pages de manuel sont placees. Par defaut, c'est man suivi de \c MAN_EXTENSION avec le `.` initial supprime.</translation>
    </message>
<message>
        <source>MAN_LINKS</source>
            <translation>Si la balise \c MAN_LINKS est definie a \c YES et que Doxygen genere une sortie man, alors il generera un fichier man supplementaire pour chaque entite documentee dans la ou les vraies pages de manuel. Ces fichiers supplementaires ne font que sourcer la vraie page de manuel, mais sans eux la commande \c man serait incapable de trouver la bonne page.</translation>
    </message>
<message>
        <source>GENERATE_XML</source>
            <translation>Si la balise \c GENERATE_XML est definie a \c YES, Doxygen generera un fichier XML qui capture la structure du code, y compris toute la documentation.</translation>
    </message>
<message>
        <source>XML_OUTPUT</source>
            <translation>La balise \c XML_OUTPUT est utilisee pour specifier ou les pages XML seront placees. Si un chemin relatif est entre, la valeur de \ref cfg_output_directory "OUTPUT_DIRECTORY" sera mise devant.</translation>
    </message>
<message>
        <source>XML_PROGRAMLISTING</source>
            <translation>Si la balise \c XML_PROGRAMLISTING est definie a \c YES, Doxygen copiera les listes de programmes (y compris la coloration syntaxique et les informations de reference croisee) dans la sortie XML. Notez que l'activation de cela augmentera de maniere significative la taille de la sortie XML.</translation>
    </message>
<message>
        <source>XML_NS_MEMB_FILE_SCOPE</source>
            <translation>Si la balise \c XML_NS_MEMB_FILE_SCOPE est definie a \c YES, Doxygen inclura les membres de namespace dans la portee du fichier egalement, correspondant a la sortie HTML.</translation>
    </message>
<message>
        <source>GENERATE_DOCBOOK</source>
            <translation>Si la balise \c GENERATE_DOCBOOK est definie a \c YES, Doxygen generera des fichiers Docbook qui peuvent etre utilises pour generer du PDF.</translation>
    </message>
<message>
        <source>DOCBOOK_OUTPUT</source>
            <translation>La balise \c DOCBOOK_OUTPUT est utilisee pour specifier ou les pages Docbook seront placees. Si un chemin relatif est entre, la valeur de \ref cfg_output_directory "OUTPUT_DIRECTORY" sera mise devant.</translation>
    </message>
<message>
        <source>GENERATE_AUTOGEN_DEF</source>
            <translation>Si la balise \c GENERATE_AUTOGEN_DEF est definie a \c YES, Doxygen generera un fichier de definitions AutoGen (voir https://autogen.sourceforge.net/) qui capture la structure du code, y compris toute la documentation. Notez que cette fonctionnalite est encore experimentale et incomplete pour le moment.</translation>
    </message>
<message>
        <source>GENERATE_SQLITE3</source>
            <translation>Si la balise \c GENERATE_SQLITE3 est definie a \c YES, Doxygen generera une base de donnees \c Sqlite3 avec les symboles trouves par Doxygen stockes dans des tables.</translation>
    </message>
<message>
        <source>SQLITE3_OUTPUT</source>
            <translation>La balise \c SQLITE3_OUTPUT est utilisee pour specifier ou la base de donnees \c Sqlite3 sera placee. Si un chemin relatif est entre, la valeur de \ref cfg_output_directory "OUTPUT_DIRECTORY" sera mise devant.</translation>
    </message>
<message>
        <source>SQLITE3_RECREATE_DB</source>
            <translation>Si la balise \c SQLITE3_RECREATE_DB est definie a \c YES, le fichier de base de donnees doxygen_sqlite3.db existant sera recree a chaque execution de Doxygen. Si defini a \c NO, Doxygen avertira si un fichier de base de donnees est deja trouve et ne le modifiera pas.</translation>
    </message>
<message>
        <source>GENERATE_PERLMOD</source>
            <translation>Si la balise \c GENERATE_PERLMOD est definie a \c YES, Doxygen generera un fichier de module Perl qui capture la structure du code, y compris toute la documentation. &lt;br&gt;Notez que cette fonctionnalite est encore experimentale et incomplete pour le moment.</translation>
    </message>
<message>
        <source>PERLMOD_LATEX</source>
            <translation>Si la balise \c PERLMOD_LATEX est definie a \c YES, Doxygen generera les regles \c Makefile necessaires, les scripts \c Perl et le code \f$\mbox{\LaTeX}\f$ pour pouvoir generer une sortie PDF et DVI a partir de la sortie du module Perl.</translation>
    </message>
<message>
        <source>PERLMOD_PRETTY</source>
            <translation>Si la balise \c PERLMOD_PRETTY est definie a \c YES, la sortie du module Perl sera bien formatee pour qu'elle puisse etre analyse par un lecteur humain. Cela est utile si vous voulez comprendre ce qui se passe. D'un autre cote, si cette balise est definie a \c NO, la taille de la sortie du module Perl sera beaucoup plus petite et Perl l'analyser de la meme maniere.</translation>
    </message>
<message>
        <source>PERLMOD_MAKEVAR_PREFIX</source>
            <translation>Les noms des variables make dans le fichier `doxyrules.make` genere sont prefixes par la chaine contenue dans \c PERLMOD_MAKEVAR_PREFIX. Cela est utile pour que differents fichiers `doxyrules.make` inclus par le meme `Makefile` n'ecrasent pas les variables les uns des autres.</translation>
    </message>
<message>
        <source>ENABLE_PREPROCESSING</source>
            <translation>Si la balise \c ENABLE_PREPROCESSING est definie a \c YES, Doxygen evaluera toutes les directives de preprocesseur C trouvees dans les sources et les fichiers d'inclusion.</translation>
    </message>
<message>
        <source>MACRO_EXPANSION</source>
            <translation>Si la balise \c MACRO_EXPANSION est definie a \c YES, Doxygen etendra tous les noms de macros dans le code source. Si defini a \c NO, seule la compilation conditionnelle sera effectuee. L'expansion de macro peut etre faite de maniere controlee en definissant \ref cfg_expand_only_predef "EXPAND_ONLY_PREDEF" a \c YES.</translation>
    </message>
<message>
        <source>EXPAND_ONLY_PREDEF</source>
            <translation>Si les balises \c EXPAND_ONLY_PREDEF et \ref cfg_macro_expansion "MACRO_EXPANSION" sont toutes deux definies a \c YES, alors l'expansion de macro est limitee aux macros specifiees avec les balises \ref cfg_predefined "PREDEFINED" et \ref cfg_expand_as_defined "EXPAND_AS_DEFINED".</translation>
    </message>
<message>
        <source>SEARCH_INCLUDES</source>
            <translation>Si la balise \c SEARCH_INCLUDES est definie a \c YES, les fichiers d'inclusion dans le \ref cfg_include_path "INCLUDE_PATH" seront recherches si un \c \#include est trouve.</translation>
    </message>
<message>
        <source>INCLUDE_PATH</source>
            <translation>La balise \c INCLUDE_PATH peut etre utilisee pour specifier un ou plusieurs repertoires qui contiennent des fichiers d'inclusion qui ne sont pas des fichiers d'entree mais qui doivent etre traites par le preprocesseur. Notez que le \c INCLUDE_PATH n'est pas recursif, donc le parametre de \ref cfg_recursive "RECURSIVE" n'a aucun effet ici.</translation>
    </message>
<message>
        <source>INCLUDE_FILE_PATTERNS</source>
            <translation>Vous pouvez utiliser la balise \c INCLUDE_FILE_PATTERNS pour specifier un ou plusieurs modeles de caracteres generiques (comme `*.h` et `*.hpp`) pour filtrer les fichiers d'en-tete dans les repertoires. Si laisse vide, les modeles specifies avec \ref cfg_file_patterns "FILE_PATTERNS" seront utilises.</translation>
    </message>
<message>
        <source>PREDEFINED</source>
            <translation>La balise \c PREDEFINED peut etre utilisee pour specifier un ou plusieurs noms de macros qui sont definis avant que le preprocesseur ne soit lance (similaire a l'option `-D` de par exemple \c gcc). L'argument de la balise est une liste de macros de la forme: &lt;code&gt;name&lt;/code&gt; ou &lt;code&gt;name=definition&lt;/code&gt; (sans espaces). Si la definition et le `=`" sont omis, `=1` est suppose. Pour empecher une definition de macro d'etre non definie via \c \#undef ou recursivement developpee, utilisez l'operateur &lt;code&gt;:=&lt;/code&gt; au lieu de l'operateur \c =.</translation>
    </message>
<message>
        <source>EXPAND_AS_DEFINED</source>
            <translation>Si les balises \ref cfg_macro_expansion "MACRO_EXPANSION" et \ref cfg_expand_only_predef "EXPAND_ONLY_PREDEF" sont definies a \c YES, cette balise peut etre utilisee pour specifier une liste de noms de macros qui doivent etre developpees. La definition de macro trouvee dans les sources sera utilisee. Utilisez la balise \ref cfg_predefined "PREDEFINED" si vous voulez utiliser une definition de macro differente qui prime sur la definition trouvee dans le code source.</translation>
    </message>
<message>
        <source>SKIP_FUNCTION_MACROS</source>
            <translation>Si la balise \c SKIP_FUNCTION_MACROS est definie a \c YES, le preprocesseur de Doxygen supprimera toutes les references aux macros de type fonction qui sont seules sur une ligne, ont un nom entierement en majuscules et ne se terminent pas par un point-virgule. De telles macros de fonction sont generalement utilisees pour le code standard et perturberont l'analyseur si elles ne sont pas supprimees.</translation>
    </message>
<message>
        <source>TAGFILES</source>
            <translation>La balise \c TAGFILES peut etre utilisee pour specifier un ou plusieurs fichiers de balises. Pour chaque fichier de balises, l'emplacement de la documentation externe doit etre ajoute. Le format d'un fichier de balises sans cet emplacement est le suivant: \verbatim TAGFILES = file1 file2 ... \endverbatim L'ajout d'un emplacement pour les fichiers de balises se fait comme suit: \verbatim TAGFILES = file1=loc1 "file2 = loc2" ... \endverbatim ou `loc1` et `loc2` peuvent etre des chemins relatifs ou absolus ou des URL. Voir la section \ref external pour plus d'informations sur l'utilisation des fichiers de balises. \note Chaque fichier de balises doit avoir un nom unique (ou le nom n'inclut PAS le chemin). Si un fichier de balises n'est pas situe dans le repertoire dans lequel Doxygen est execute, vous devez egalement specifier le chemin d'acces au fichier de balises ici.</translation>
    </message>
<message>
        <source>GENERATE_TAGFILE</source>
            <translation>Lorsqu'un nom de fichier est specifie apres \c GENERATE_TAGFILE, Doxygen creera un fichier de balises base sur les fichiers d'entree qu'il lit. Voir la section \ref external pour plus d'informations sur l'utilisation des fichiers de balises.</translation>
    </message>
<message>
        <source>ALLEXTERNALS</source>
            <translation>Si la balise \c ALLEXTERNALS est definie a \c YES, toutes les classes et namespaces externes seront listes dans l'index des classes et des namespaces. Si defini a \c NO, seules les classes externes heritees seront listees.</translation>
    </message>
<message>
        <source>EXTERNAL_GROUPS</source>
            <translation>Si la balise \c EXTERNAL_GROUPS est definie a \c YES, tous les groupes externes seront listes dans l'index des sujets. Si defini a \c NO, seuls les groupes du projet actuel seront listes.</translation>
    </message>
<message>
        <source>EXTERNAL_PAGES</source>
            <translation>Si la balise \c EXTERNAL_PAGES est definie a \c YES, toutes les pages externes seront listees dans l'index des pages connexes. Si defini a \c NO, seules les pages du projet actuel seront listees.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_RELATIONS</source>
            <translation>Si defini a \c YES, les graphes d'heritage et de collaboration cacheront les relations d'heritage et d'utilisation si la cible est non documentee ou n'est pas une classe.</translation>
    </message>
<message>
        <source>HAVE_DOT</source>
            <translation>Si vous definissez la balise \c HAVE_DOT a \c YES, Doxygen supposera que l'outil \c dot est disponible depuis le \c path. Cet outil fait partie de &lt;a href="https://www.graphviz.org/"&gt;Graphviz&lt;/a&gt;, une boite a outils de visualisation de graphes d'AT&amp;T et Lucent Bell Labs. Les autres options de cette section n'ont aucun effet si cette option est definie a \c NO.</translation>
    </message>
<message>
        <source>DOT_NUM_THREADS</source>
            <translation>Le \c DOT_NUM_THREADS specifie le nombre d'invocations \c dot que Doxygen est autorise a executer en parallele. Lorsqu'il est defini a \c 0, Doxygen le basera sur le nombre de processeurs disponibles dans le systeme. Vous pouvez le definir explicitement a une valeur superieure a 0 pour avoir un controle sur l'equilibre entre la charge CPU et la vitesse de traitement.</translation>
    </message>
<message>
        <source>DOT_COMMON_ATTR</source>
            <translation>\c DOT_COMMON_ATTR est des attributs communs pour les noeuds, les aretes et les etiquettes de sous-graphes. Lorsque vous voulez une police d'apparence differente dans les fichiers dot que Doxygen genere, vous pouvez specifier les attributs fontname, fontcolor et fontsize. Pour plus de details, voir &lt;a href=https://graphviz.org/doc/info/attrs.html&gt;specification des attributs de Noeud, Arete et Graphe&lt;/a&gt;. Vous devez vous assurer que dot est capable de trouver la police, ce qui peut etre fait en la placant dans un emplacement standard ou en definissant la variable d'environnement \c DOTFONTPATH ou en definissant \ref cfg_dot_fontpath "DOT_FONTPATH" sur le repertoire contenant la police. La taille de police graphviz par defaut est 14.</translation>
    </message>
<message>
        <source>DOT_EDGE_ATTR</source>
            <translation>\c DOT_EDGE_ATTR est concatene avec \ref cfg_dot_common_attr "DOT_COMMON_ATTR". Pour un style elegant, vous pouvez ajouter 'arrowhead=open, arrowtail=open, arrowsize=0.5'. &lt;a href=https://graphviz.org/doc/info/arrows.html&gt;Documentation complete sur les formes de fleches.&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_NODE_ATTR</source>
            <translation>\c DOT_NODE_ATTR est concatene avec \ref cfg_dot_common_attr "DOT_COMMON_ATTR". Pour une vue sans boites autour des noeuds, definissez 'shape=plain' ou 'shape=plaintext'. &lt;a href=https://www.graphviz.org/doc/info/shapes.html&gt;Specification des formes.&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_FONTPATH</source>
            <translation>Vous pouvez definir le chemin ou \c dot peut trouver la police specifiee avec fontname dans \ref cfg_dot_common_attr "DOT_COMMON_ATTR" et d'autres attributs dot.</translation>
    </message>
<message>
        <source>CLASS_GRAPH</source>
            <translation>Si la balise \c CLASS_GRAPH est definie a \c YES ou \c GRAPH ou \c BUILTIN, Doxygen generera un graphe pour chaque classe documentee montrant les relations d'heritage directes et indirectes. Si la balise \c CLASS_GRAPH est definie a \c YES ou \c GRAPH et que \ref cfg_have_dot "HAVE_DOT" est active egalement, alors `dot` sera utilise pour dessiner le graphe. Si la balise \c CLASS_GRAPH est definie a \c YES et que \ref cfg_have_dot "HAVE_DOT" est desactive ou si la balise \c CLASS_GRAPH est definie a \c BUILTIN, alors le generateur integre sera utilise. Si la balise \c CLASS_GRAPH est definie a \c TEXT, les relations d'heritage directes et indirectes seront affichees sous forme de textes / liens. L'activation explicite d'un graphe d'heritage ou le choix d'une representation differente pour un graphe d'heritage d'une classe specifique peut etre accompli au moyen de la commande \ref cmdinheritancegraph "\inheritancegraph". La desactivation d'un graphe d'heritage peut etre accomplie au moyen de la commande \ref cmdhideinheritancegraph "\hideinheritancegraph".</translation>
    </message>
<message>
        <source>COLLABORATION_GRAPH</source>
            <translation>Si la balise \c COLLABORATION_GRAPH est definie a \c YES, Doxygen generera un graphe pour chaque classe documentee montrant les dependances d'implementation directes et indirectes (heritage, containment et variables de reference de classe) de la classe avec d'autres classes documentees. L'activation explicite d'un graphe de collaboration, lorsque \c COLLABORATION_GRAPH est defini a \c NO, peut etre accomplie au moyen de la commande \ref cmdcollaborationgraph "\collaborationgraph". La desactivation d'un graphe de collaboration peut etre accomplie au moyen de la commande \ref cmdhidecollaborationgraph "\hidecollaborationgraph".</translation>
    </message>
<message>
        <source>UML_LOOK</source>
            <translation>Si la balise \c UML_LOOK est definie a \c YES, Doxygen generera des diagrammes d'heritage et de collaboration dans un style similaire au langage de modelisation unifie de l'OMG.</translation>
    </message>
<message>
        <source>UML_LIMIT_NUM_FIELDS</source>
            <translation>Si la balise \ref cfg_uml_look "UML_LOOK" est activee, les champs et methodes sont affiches a l'interieur du noeud de classe. S'il y a beaucoup de champs ou methodes et beaucoup de noeuds, le graphe peut devenir trop grand pour etre utile. Le seuil \c UML_LIMIT_NUM_FIELDS limite le nombre d'elements pour chaque type pour rendre la taille plus gerable. Definissez ceci a 0 pour aucune limite. Notez que le seuil peut etre depasse de 50% avant que la limite ne soit appliquee. Donc lorsque vous definissez le seuil a 10, jusqu'a 15 champs peuvent apparaitre, mais si le nombre depasse 15, le nombre total de champs affiches est limite a 10.</translation>
    </message>
<message>
        <source>UML_MAX_EDGE_LABELS</source>
            <translation>Si la balise \ref cfg_uml_look "UML_LOOK" est activee, les etiquettes de champ sont affichees le long de l'arete entre deux noeuds de classe. S'il y a beaucoup de champs et beaucoup de noeuds, le graphe peut devenir trop encombre. Le seuil \c UML_MAX_EDGE_LABELS limite le nombre d'elements pour rendre la taille plus gerable. Definissez ceci a 0 pour aucune limite.</translation>
    </message>
<message>
        <source>DOT_UML_DETAILS</source>
            <translation>Si la balise \c DOT_UML_DETAILS est definie a \c NO, Doxygen affichera les attributs et methodes sans types et arguments dans les graphes UML. Si la balise \c DOT_UML_DETAILS est definie a \c YES, Doxygen ajoutera le type et les arguments pour les attributs et methodes dans les graphes UML. Si la balise \c DOT_UML_DETAILS est definie a \c NONE, Doxygen ne generera pas de champs avec des informations de membre de classe dans les graphes UML. Les diagrammes de classe ressembleront aux diagrammes de classe par defaut mais en utilisant la notation UML pour les relations.</translation>
    </message>
<message>
        <source>DOT_WRAP_THRESHOLD</source>
            <translation>La balise \c DOT_WRAP_THRESHOLD peut etre utilisee pour definir le nombre maximal de caracteres a afficher sur une seule ligne. Si la longueur reelle de la ligne depasse significativement ce seuil, elle sera divisee sur plusieurs lignes. Quelques heuristiques sont appliquees pour eviter les coupures de ligne inesthetiques.</translation>
    </message>
<message>
        <source>TEMPLATE_RELATIONS</source>
            <translation>Si la balise \c TEMPLATE_RELATIONS est definie a \c YES, les graphes d'heritage et de collaboration montreront les relations entre les templates et leurs instances.</translation>
    </message>
<message>
        <source>INCLUDE_GRAPH</source>
            <translation>Si les balises \c INCLUDE_GRAPH, \ref cfg_enable_preprocessing "ENABLE_PREPROCESSING" et \ref cfg_search_includes "SEARCH_INCLUDES" sont definies a \c YES, Doxygen generera un graphe pour chaque fichier documente montrant les dependances d'inclusion directes et indirectes du fichier avec d'autres fichiers documentes. L'activation explicite d'un graphe d'inclusion, lorsque \c INCLUDE_GRAPH est defini a \c NO, peut etre accomplie au moyen de la commande \ref cmdincludegraph "\includegraph". La desactivation d'un graphe d'inclusion peut etre accomplie au moyen de la commande \ref cmdhideincludegraph "\hideincludegraph".</translation>
    </message>
<message>
        <source>INCLUDED_BY_GRAPH</source>
            <translation>Si les balises \c INCLUDED_BY_GRAPH, \ref cfg_enable_preprocessing "ENABLE_PREPROCESSING" et \ref cfg_search_includes "SEARCH_INCLUDES" sont definies a \c YES, Doxygen generera un graphe pour chaque fichier documente montrant les dependances d'inclusion directes et indirectes du fichier avec d'autres fichiers documentes. L'activation explicite d'un graphe d'inclusion par, lorsque \c INCLUDED_BY_GRAPH est defini a \c NO, peut etre accomplie au moyen de la commande \ref cmdincludedbygraph "\includedbygraph". La desactivation d'un graphe d'inclusion par peut etre accomplie au moyen de la commande \ref cmdhideincludedbygraph "\hideincludedbygraph".</translation>
    </message>
<message>
        <source>CALL_GRAPH</source>
            <translation>Si la balise \c CALL_GRAPH est definie a \c YES, Doxygen generera un graphe de dependance d'appel pour chaque fonction globale ou methode de classe. &lt;br&gt;Notez que l'activation de cette option augmentera de maniere significative le temps d'une execution. Donc dans la plupart des cas, il sera preferable d'activer les graphes d'appel uniquement pour les fonctions selectionnees en utilisant la commande \ref cmdcallgraph "\callgraph". La desactivation d'un graphe d'appel peut etre accomplie au moyen de la commande \ref cmdhidecallgraph "\hidecallgraph".</translation>
    </message>
<message>
        <source>CALLER_GRAPH</source>
            <translation>Si la balise \c CALLER_GRAPH est definie a \c YES, Doxygen generera un graphe de dependance d'appelant pour chaque fonction globale ou methode de classe. &lt;br&gt;Notez que l'activation de cette option augmentera de maniere significative le temps d'une execution. Donc dans la plupart des cas, il sera preferable d'activer les graphes d'appelant uniquement pour les fonctions selectionnees en utilisant la commande \ref cmdcallergraph "\callergraph". La desactivation d'un graphe d'appelant peut etre accomplie au moyen de la commande \ref cmdhidecallergraph "\hidecallergraph".</translation>
    </message>
<message>
        <source>GRAPHICAL_HIERARCHY</source>
            <translation>Si la balise \c GRAPHICAL_HIERARCHY est definie a \c YES, Doxygen generera une hierarchie graphique de toutes les classes au lieu d'une textuelle.</translation>
    </message>
<message>
        <source>DIRECTORY_GRAPH</source>
            <translation>Si la balise \c DIRECTORY_GRAPH est definie a \c YES, Doxygen montrera les dependances d'un repertoire sur d'autres repertoires de maniere graphique. Les relations de dependance sont determinees par les relations \c \#include entre les fichiers dans les repertoires. L'activation explicite d'un graphe de repertoire, lorsque \c DIRECTORY_GRAPH est defini a \c NO, peut etre accomplie au moyen de la commande \ref cmddirectorygraph "\directorygraph". La desactivation d'un graphe de repertoire peut etre accomplie au moyen de la commande \ref cmdhidedirectorygraph "\hidedirectorygraph".</translation>
    </message>
<message>
        <source>DIR_GRAPH_MAX_DEPTH</source>
            <translation>La balise \c DIR_GRAPH_MAX_DEPTH peut etre utilisee pour limiter le nombre maximal de niveaux de sous-repertoires generes dans les graphes de dependance de repertoire par \c dot.</translation>
    </message>
<message>
        <source>DOT_IMAGE_FORMAT</source>
            <translation>La balise \c DOT_IMAGE_FORMAT peut etre utilisee pour definir le format d'image des images generees par \c dot. Pour une explication des formats d'image, voir la section des formats de sortie dans la documentation de l'outil \c dot (&lt;a href="https://www.graphviz.org/"&gt;Graphviz&lt;/a&gt;). &lt;br&gt;Notez que les formats `svg:cairo` et `svg:cairo:cairo` ne peuvent pas etre utilises en combinaison avec \ref cfg_interactive_svg "INTERACTIVE_SVG" (le \ref cfg_interactive_svg "INTERACTIVE_SVG" sera defini a `NO`).</translation>
    </message>
<message>
        <source>INTERACTIVE_SVG</source>
            <translation>Si \ref cfg_dot_image_format "DOT_IMAGE_FORMAT" est defini a `svg` ou `svg:svg` ou `svg:svg:core`, cette option peut etre definie a \c YES pour activer la generation d'images SVG interactives qui permettent le zoom et le panoramique. &lt;br&gt;Notez que cela necessite un navigateur moderne autre qu'Internet Explorer. Testes et fonctionnels: Firefox, Chrome, Safari et Opera. &lt;br&gt;Notez que cette option sera automatiquement desactivee lorsque \ref cfg_dot_image_format "DOT_IMAGE_FORMAT" est defini a `svg:cairo` ou `svg:cairo:cairo`.</translation>
    </message>
<message>
        <source>DOT_PATH</source>
            <translation>La balise \c DOT_PATH peut etre utilisee pour specifier le chemin ou l'outil \c dot peut etre trouve. Si laisse vide, il est suppose que l'outil \c dot peut etre trouve dans le \c path.</translation>
    </message>
<message>
        <source>DOTFILE_DIRS</source>
            <translation>La balise \c DOTFILE_DIRS peut etre utilisee pour specifier un ou plusieurs repertoires qui contiennent des fichiers dot qui sont inclus dans la documentation (voir la commande \ref cmddotfile "\dotfile").</translation>
    </message>
<message>
        <source>DIA_PATH</source>
            <translation>Vous pouvez inclure des diagrammes faits avec dia dans la documentation Doxygen. Doxygen executera ensuite dia pour produire le diagramme et l'inserer dans la documentation. La balise DIA_PATH vous permet de specifier le repertoire ou reside le binaire dia. Si laisse vide, dia est suppose etre trouve dans le chemin de recherche par defaut.</translation>
    </message>
<message>
        <source>DIAFILE_DIRS</source>
            <translation>La balise \c DIAFILE_DIRS peut etre utilisee pour specifier un ou plusieurs repertoires qui contiennent des fichiers dia qui sont inclus dans la documentation (voir la commande \ref cmddiafile "\diafile").</translation>
    </message>
<message>
        <source>PLANTUML_JAR_PATH</source>
            <translation>Lors de l'utilisation de PlantUML, la balise \c PLANTUML_JAR_PATH devrait etre utilisee pour specifier le chemin ou java peut trouver le fichier \c plantuml.jar ou le nom de fichier du fichier \c jar a utiliser. Si laisse vide, il est suppose que PlantUML n'est pas utilise ou appele lors d'une etape de pretraitement. Doxygen generera un avertissement lorsqu'il rencontrera une commande \ref cmdstartuml "\startuml" dans ce cas et ne generera pas de sortie pour le diagramme.</translation>
    </message>
<message>
        <source>PLANTUML_CFG_FILE</source>
            <translation>Lors de l'utilisation de PlantUML, la balise \c PLANTUML_CFG_FILE peut etre utilisee pour specifier un fichier de configuration pour PlantUML.</translation>
    </message>
<message>
        <source>PLANTUML_INCLUDE_PATH</source>
            <translation>Lors de l'utilisation de PlantUML, les chemins specifies sont recherches pour les fichiers specifies par l'instruction \c !include dans un bloc PlantUML.</translation>
    </message>
<message>
        <source>PLANTUMLFILE_DIRS</source>
            <translation>La balise \c PLANTUMLFILE_DIRS peut etre utilisee pour specifier un ou plusieurs repertoires qui contiennent des fichiers PlantUml qui sont inclus dans la documentation (voir la commande \ref cmdplantumlfile "\plantumlfile").</translation>
    </message>
<message>
        <source>MERMAID_PATH</source>
            <translation>Lors de l'utilisation de diagrammes Mermaid avec le rendu CLI, la balise \c MERMAID_PATH devrait etre utilisee pour specifier le repertoire ou l'executable \c mmdc (Mermaid CLI) peut etre trouve. Si laisse vide, le rendu base sur CLI est desactive. Pour la sortie HTML, le rendu cote client via JavaScript est utilise par defaut et ne necessite pas \c mmdc. Pour la sortie LaTeX/PDF, \c mmdc est requis pour pre-generer les images. Doxygen generera un avertissement lorsque le rendu CLI est necessaire mais que \c mmdc n'est pas disponible.</translation>
    </message>
<message>
        <source>MERMAID_CONFIG_FILE</source>
            <translation>Lors de l'utilisation de diagrammes Mermaid, la balise \c MERMAID_CONFIG_FILE peut etre utilisee pour specifier un fichier de configuration JSON pour l'outil Mermaid CLI (\c mmdc). Ce fichier peut contenir des parametres de theme et d'autres options de configuration Mermaid.</translation>
    </message>
<message>
        <source>MERMAID_RENDER_MODE</source>
            <translation>La balise \c MERMAID_RENDER_MODE selectionne la maniere dont les diagrammes Mermaid sont rendus.</translation>
    </message>
<message>
        <source>MERMAID_JS_URL</source>
            <translation>La balise \c MERMAID_JS_URL specifie l'URL a partir de laquelle charger \c mermaid.js lors de l'utilisation du rendu cote client (\c MERMAID_RENDER_MODE est \c CLIENT_SIDE ou \c AUTO). Le defaut pointe vers la derniere version Mermaid v11 sur le CDN jsDelivr. \par Exemples: - Derniere v11 (defaut): \c 'https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs' - Version epinglee: \c 'https://cdn.jsdelivr.net/npm/mermaid@11.3.0/dist/mermaid.esm.min.mjs' - Copie locale: \c './mermaid.esm.min.mjs' (l'utilisateur doit placer le fichier dans le repertoire de sortie HTML) \par L'URL CDN par defaut necessite un acces Internet lors de la consultation de la documentation generee. Pour une utilisation hors ligne, telechargez \c mermaid.esm.min.mjs et definissez ceci sur un chemin relatif, ou utilisez \c MERMAID_RENDER_MODE=CLI pour pre-generer les images a la place.</translation>
    </message>
<message>
        <source>MERMAIDFILE_DIRS</source>
            <translation>La balise \c MERMAIDFILE_DIRS peut etre utilisee pour specifier un ou plusieurs repertoires qui contiennent des fichiers Mermaid qui sont inclus dans la documentation (voir la commande \ref cmdmermaidfile "\mermaidfile").</translation>
    </message>
<message>
        <source>DOT_GRAPH_MAX_NODES</source>
            <translation>La balise \c DOT_GRAPH_MAX_NODES peut etre utilisee pour definir le nombre maximal de noeuds qui seront affiches dans le graphe. Si le nombre de noeuds dans un graphe devient superieur a cette valeur, Doxygen tronquera le graphe, ce qui est visualise en representant un noeud comme une boite rouge. Notez que si le nombre d'enfants directs du noeud racine dans un graphe est deja superieur a \c DOT_GRAPH_MAX_NODES, le graphe ne sera pas affiche du tout. Notez egalement que la taille d'un graphe peut etre encore restreinte par \ref cfg_max_dot_graph_depth "MAX_DOT_GRAPH_DEPTH".</translation>
    </message>
<message>
        <source>MAX_DOT_GRAPH_DEPTH</source>
            <translation>La balise \c MAX_DOT_GRAPH_DEPTH peut etre utilisee pour definir la profondeur maximale des graphes generes par \c dot. Une valeur de profondeur de 3 signifie que seuls les noeuds atteignables depuis la racine en suivant un chemin via au maximum 3 aretes seront affiches. Les noeuds qui se trouvent plus loin du noeud racine seront omis. Notez que definir cette option a 1 ou 2 peut reduire considerablement le temps de calcul necessaire pour les grandes bases de code. Notez egalement que la taille d'un graphe peut etre encore restreinte par \ref cfg_dot_graph_max_nodes "DOT_GRAPH_MAX_NODES". Utiliser une profondeur de 0 signifie aucune restriction de profondeur.</translation>
    </message>
<message>
        <source>DOT_MULTI_TARGETS</source>
            <translation>Definissez la balise \c DOT_MULTI_TARGETS a \c YES pour permettre a dot de generer plusieurs fichiers de sortie en une seule execution (c'est-a-dire plusieurs options -o et -T sur la ligne de commande). Cela fait fonctionner \c dot plus rapidement, mais comme seules les versions plus recentes de \c dot (&gt;1.8.10) prennent en charge cela, cette fonctionnalite est desactivee par defaut.</translation>
    </message>
<message>
        <source>GENERATE_LEGEND</source>
            <translation>Si la balise \c GENERATE_LEGEND est definie a \c YES, Doxygen generera une page de legende expliquant la signification des diverses boites et fleches dans les graphes generes par dot. \note Cette balise necessite que \ref cfg_uml_look "UML_LOOK" ne soit pas defini, c'est-a-dire que la representation graphique interne de Doxygen pour les diagrammes d'heritage et de collaboration soit utilisee.</translation>
    </message>
<message>
        <source>DOT_CLEANUP</source>
            <translation>Si la balise \c DOT_CLEANUP est definie a \c YES, Doxygen supprimera les fichiers intermediaires qui sont utilises pour generer les divers graphes. &lt;br&gt;Note: Ce parametre est utilise non seulement pour les fichiers dot mais aussi pour les fichiers temporaires msc.</translation>
    </message>
<message>
        <source>MSCGEN_TOOL</source>
            <translation>Vous pouvez definir des diagrammes de sequence de messages dans les commentaires Doxygen en utilisant la commande \ref cmdmsc "\msc". Si la balise \c MSCGEN_TOOL est laissee vide (le defaut), Doxygen utilisera une version integree de l'outil mscgen pour produire les diagrammes. Alternativement, la balise \c MSCGEN_TOOL peut egalement specifier le nom d'un outil externe. Par exemple, en specifiant `prog` comme valeur, Doxygen appellera l'outil comme `prog -T &lt;outfile_format&gt; -o &lt;outputfile&gt; &lt;inputfile&gt;`. L'outil externe devrait prendre en charge les formats de fichier de sortie "png", "eps", "svg" et "ismap".</translation>
    </message>
<message>
        <source>MSCFILE_DIRS</source>
            <translation>La balise \c MSCFILE_DIRS peut etre utilisee pour specifier un ou plusieurs repertoires qui contiennent des fichiers msc qui sont inclus dans la documentation (voir la commande \ref cmdmscfile "\mscfile").</translation>
    </message>
<message>
        <source>GROUP_GRAPHS</source>
            <translation>Si la balise \c GROUP_GRAPHS est definie a \c YES, Doxygen generera un graphe pour les groupes, montrant les dependances directes des groupes. L'activation explicite d'un graphe de dependance de groupe, lorsque \c GROUP_GRAPHS est defini a \c NO, peut etre accomplie au moyen de la commande \ref cmdgroupgraph "\groupgraph". La desactivation d'un graphe de repertoire peut etre accomplie au moyen de la commande \ref cmdhidegroupgraph "\hidegroupgraph". Voir aussi le chapitre \ref grouping "Grouping" dans le manuel.</translation>
    </message>
</context>
<context>
    <name>OptionValue</name>
<message>
        <source>(United States)</source>
        <translation>(Etats-Unis)</translation>
    </message>
<message>
        <source>(Persian)</source>
        <translation>(Persan)</translation>
    </message>
<message>
        <source>(Japanese with English messages)</source>
        <translation>(Japonais avec messages en anglais)</translation>
    </message>
<message>
        <source>(Korean with English messages)</source>
        <translation>(Coreen avec messages en anglais)</translation>
    </message>
<message>
        <source>(Farsi)</source>
        <translation>(Farsi)</translation>
    </message>
<message>
        <source>use a fixed 'autotoc_md' string followed by a sequence number starting at 0</source>
        <translation>utiliser une chaine 'autotoc_md' fixe suivie d'un numero de sequence commencant a 0</translation>
    </message>
<message>
        <source>use the lower case version of title with any whitespace replaced by '-' and punctuation characters removed</source>
        <translation>utiliser la version en minuscules du titre avec tous les espaces remplaces par '-' et les caracteres de ponctuation supprimes</translation>
    </message>
<message>
        <source>(to be provided as Doxygen C comment)</source>
        <translation>(a fournir comme commentaire C Doxygen)</translation>
    </message>
<message>
        <source>always generates light mode output</source>
        <translation>genere toujours une sortie en mode clair</translation>
    </message>
<message>
        <source>always generates dark mode output</source>
        <translation>genere toujours une sortie en mode sombre</translation>
    </message>
<message>
        <source>automatically sets the mode according to the user preference, uses light mode if no preference is set (the default)</source>
        <translation>definit automatiquement le mode selon la preference de l'utilisateur, utilise le mode clair si aucune preference n'est definie (par defaut)</translation>
    </message>
<message>
        <source>automatically sets the mode according to the user preference, uses dark mode if no preference is set</source>
        <translation>definit automatiquement le mode selon la preference de l'utilisateur, utilise le mode sombre si aucune preference n'est definie</translation>
    </message>
<message>
        <source>allows a user to switch between light and dark mode via a button</source>
        <translation>permet a l'utilisateur de basculer entre le mode clair et sombre via un bouton</translation>
    </message>
<message>
        <source>(the default)</source>
        <translation>(par defaut)</translation>
    </message>
<message>
        <source>(looks nicer but requires the pdf2svg or inkscape tool)</source>
        <translation>(plus joli mais necessite l'outil pdf2svg ou inkscape)</translation>
    </message>
<message>
        <source>(which is slower, but has the best compatibility. This is the name for Mathjax version 2, for MathJax version 3 this will be translated into \c chtml)</source>
        <translation>(plus lent, mais avec la meilleure compatibilite. C'est le nom pour MathJax version 2, pour MathJax version 3 cela sera traduit en \c chtml)</translation>
    </message>
<message>
        <source>(i.e. MathML. Only supported for MathJax 2. For MathJax version 3 \c chtml will be used instead.)</source>
        <translation>(c'est-a-dire MathML. Pris en charge uniquement pour MathJax 2. Pour MathJax version 3, \c chtml sera utilise a la place.)</translation>
    </message>
<message>
        <source>(This is the name for Mathjax version 3, for MathJax version 2 this will be translated into \c HTML-CSS)</source>
        <translation>(C'est le nom pour MathJax version 3, pour MathJax version 2 cela sera traduit en \c HTML-CSS)</translation>
    </message>
<message>
        <source>(210 x 297 mm)</source>
        <translation>(210 x 297 mm)</translation>
    </message>
<message>
        <source>(8.5 x 11 inches)</source>
        <translation>(8,5 x 11 pouces)</translation>
    </message>
<message>
        <source>(8.5 x 14 inches)</source>
        <translation>(8,5 x 14 pouces)</translation>
    </message>
<message>
        <source>(7.25 x 10.5 inches)</source>
        <translation>(7,25 x 10,5 pouces)</translation>
    </message>
<message>
        <source>same as ERROR_STOP</source>
        <translation>identique a ERROR_STOP</translation>
    </message>
<message>
        <source>same as BATCH</source>
        <translation>identique a BATCH</translation>
    </message>
<message>
        <source>In batch mode nothing is printed on the terminal, errors are scrolled as if \&lt;return\&gt; is hit at every error; missing files that TeX tries to input or request from keyboard input (\\read on a not open input stream) cause the job to abort</source>
        <translation>En mode batch, rien n'est imprime sur le terminal, les erreurs defilent comme si \&lt;return\&gt; etait appuye a chaque erreur; les fichiers manquants que TeX essaie de lire ou de demander a partir de l'entree clavier (\\read sur un flux d'entree non ouvert) provoquent l'arret du travail</translation>
    </message>
<message>
        <source>In nonstop mode the diagnostic message will appear on the terminal, but there is no possibility of user interaction just like in batch mode</source>
        <translation>En mode nonstop, le message de diagnostic apparaitra sur le terminal, mais il n'y a aucune possibilite d'interaction utilisateur comme en mode batch</translation>
    </message>
<message>
        <source>In scroll mode, TeX will stop only for missing files to input or if keyboard input is necessary</source>
        <translation>En mode scroll, TeX s'arretera uniquement pour les fichiers manquants a lire ou si une entree clavier est necessaire</translation>
    </message>
<message>
        <source>In errorstop mode, TeX will stop at each error, asking for user intervention</source>
        <translation>En mode errorstop, TeX s'arretera a chaque erreur, demandant l'intervention de l'utilisateur</translation>
    </message>
<message>
        <source>(use client-side rendering for HTML and \c mmdc for LaTeX/PDF and other formats. If \c MERMAID_PATH is not set, non-HTML diagrams will produce a warning)</source>
        <translation>(utiliser le rendu cote client pour HTML et \c mmdc pour LaTeX/PDF et autres formats. Si \c MERMAID_PATH n'est pas defini, les diagrammes non-HTML produiront un avertissement)</translation>
    </message>
<message>
        <source>(use the \c mmdc tool to pre-generate images (requires \c Node.js and \c mermaid-js/mermaid-cli). Works for all output formats)</source>
        <translation>(utiliser l'outil \c mmdc pour pre-generer les images (necessite \c Node.js et \c mermaid-js/mermaid-cli). Fonctionne pour tous les formats de sortie)</translation>
    </message>
<message>
        <source>(embed \c mermaid.js in HTML output for client-side rendering. Does not require \c mmdc but only works for HTML output)</source>
        <translation>(integrer \c mermaid.js dans la sortie HTML pour le rendu cote client. Ne necessite pas \c mmdc mais fonctionne uniquement pour la sortie HTML)</translation>
    </message>
</context>
</TS>