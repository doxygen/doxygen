\documentclass[twoside]{<% if config.COMPACT_LATEX %>article<% else %>book<% endif %>}

% Packages required by doxygen
\usepackage{fixltx2e}
\usepackage{calc}
\usepackage{doxygen}
\usepackage[export]{adjustbox} % also loads graphicx
<% for package in config.LATEX_EXTRA_STYLESHEET %>
\usepackage{<{package|stripExtension:'.sty'}>}
<% endfor %>
\usepackage{graphicx}
\usepackage[utf8]{inputenc}
\usepackage{makeidx}
\usepackage{multicol}
\usepackage{multirow}
\PassOptionsToPackage{warn}{textcomp}
\usepackage{textcomp}
\usepackage[nointegrals]{wasysym}
\usepackage[table]{xcolor}

<# TODO: languageSupportCommand #>

% Font selection
\usepackage[T1]{fontenc}
\usepackage[scaled=.90]{helvet}
\usepackage{courier}
\usepackage{amssymb}
\usepackage{sectsty}
\renewcommand{\familydefault}{\sfdefault}
\allsectionsfont{
  \fontseries{bc}\selectfont
  \color{darkgray}
}
\renewcommand{\DoxyLabelFont}{
  \fontseries{bc}\selectfont
  \color{darkgray}
}
\newcommand{\+}{\discretionary{\mbox{\scriptsize$\hookleftarrow$}}{}{}}

% Page & text layout
\usepackage{geometry}
\geometry{
  <{ config.PAPER_TYPE }>paper,
  top=2.5cm,
  bottom=2.5cm,
  left=2.5cm,
  right=2.5cm
}
\tolerance=750
\hfuzz=15pt
\hbadness=750
\setlength{\emergencystretch}{15pt}
\setlength{\parindent}{0cm}
\setlength{\parskip}{3ex plus 2ex minus 2ex}
\makeatletter
\renewcommand{\paragraph}{
  \@startsection{paragraph}{4}{0ex}{-1.0ex}{1.0ex}{
    \normalfont\normalsize\bfseries\SS@parafont
  }
}
\renewcommand{\subparagraph}{
  \@startsection{subparagraph}{5}{0ex}{-1.0ex}{1.0ex}{
    \normalfont\normalsize\bfseries\SS@subparafont
  }
}
\makeatother

% Headers & footers
\usepackage{fancyhdr}
\pagestyle{fancyplain}
\fancyhead[LE]{\fancyplain{}{\bfseries\thepage}}
\fancyhead[CE]{\fancyplain{}{}}
\fancyhead[RE]{\fancyplain{}{\bfseries\leftmark}}
\fancyhead[LO]{\fancyplain{}{\bfseries\rightmark}}
\fancyhead[CO]{\fancyplain{}{}}
\fancyhead[RO]{\fancyplain{}{\bfseries\thepage}}
\fancyfoot[LE]{\fancyplain{}{}}
\fancyfoot[CE]{\fancyplain{}{}}
\fancyfoot[RE]{\fancyplain{}{\bfseries\scriptsize <{ tr.generatedAt:doxygen.date,config.PROJECT_NAME }>}}
\fancyfoot[LO]{\fancyplain{}{\bfseries\scriptsize <{ tr.generatedAt:doxygen.date,config.PROJECT_NAME }>}}
\fancyfoot[CO]{\fancyplain{}{}}
\fancyfoot[RO]{\fancyplain{}{}}
\renewcommand{\footrulewidth}{0.4pt}
<% if not config.COMPACT_LATEX %>
\renewcommand{\chaptermark}[1]{
  \markboth{ #1}{}%
}
<% endif %>
\renewcommand{\sectionmark}[1]{
  \markright{\thesection\ #1}
}

% Indices & bibliography
\usepackage{natbib}
\usepackage[titles]{tocloft}
\setcounter{tocdepth}{3}
\setcounter{secnumdepth}{5}
\makeindex

<% if config.EXTRA_PACKAGES %>
% Packages requested by user
<% for package in config.EXTRA_PACKAGES %>
\usepackage{<{ package }>}
<% endfor %>
<% endif %>

<% if config.PDF_HYPERLINKS %>
% Hyperlinks (required, but should be loaded last)
\usepackage{ifpdf}
\ifpdf
  \usepackage[pdftex,pagebackref=true]{hyperref}
\else
  \usepackage[ps2pdf,pagebackref=true]{hyperref}
\fi
\hypersetup{
  colorlinks=true,
  linkcolor=blue,
  citecolor=blue,
  unicode
}
<% endif %>

% Custom commands
\newcommand{\clearemptydoublepage}{
  \newpage{\pagestyle{empty}\cleardoublepage}
}

%===== C O N T E N T S =====

\begin{document}
<# TODO: select language for greek #>

% Titlepage & ToC
<% if config.USE_PDFLATEX and config.PDF_HYPERLINKS %>
\hypersetup{pageanchor=false,
             bookmarks=true,
             bookmarksnumbered=true,
             pdfencoding=unicode
            }
<% endif %>
\pagenumbering{roman}
\begin{titlepage}
<% tabbing %>
\vspace*{7cm}
\begin{center}%
{\Large
<% if config.PROJECT_NAME %>
  <{ config.PROJECT_NAME }>
<% else %>
  <{ tr.referenceManual }>
<% endif %>
<% if config.PROJECT_NUMBER %>
\\[1ex]\large <{ config.PROJECT_NUMBER }>
<% endif %>
}\\
\vspace*{1cm}{\large <{ tr.generatedBy }> Doxygen <{ doxygen.version }>}\\
\vspace*{0.5cm}{\small <{ doxygen.date }>}\\
\end{center}
<% endtabbing %>
\end{titlepage}
<% if not config.COMPACT_LATEX %>\clearemptydoublepage<% endif %>

\tableofcontents
<% if not config.COMPACT_LATEX %>\clearemptydoublepage<% endif %>
\pagenumbering{arabic}
<% if config.USE_PDFLATEX and config.PDF_HYPERLINKS %>
\hypersetup{pageanchor=true}
<% endif %>

%--- Begin generated contents ---
<# TODO: loop over pages #>
<% if moduleTree.tree %>
<% if config.COMPACT_LATEX %>\section<% else %>\chapter<% endif %>{<{ tr.moduleIndex }>}
\input{modules}
<% endif %>
<% if namespaceTree.tree %>
<% if config.COMPACT_LATEX %>\section<% else %>\chapter<% endif %>{<{ tr.namespaceIndex }>}
\input{namespaces}
<% endif %>
<% if classTree.tree %>
<% if config.COMPACT_LATEX %>\section<% else %>\chapter<% endif %>{<{ tr.classIndex }>}
\input{annotated}
<% endif %>
<% if fileTree.tree %>
<% if config.COMPACT_LATEX %>\section<% else %>\chapter<% endif %>{<{ tr.fileIndex }>}
\input{files}
<% endif %>
<% if moduleList %>
<% if config.COMPACT_LATEX %>\section<% else %>\chapter<% endif %>{<{ tr.moduleDocumentation }>}
<% for compound in moduleList %>
\input{<{ compound.fileName|raw }>}
<% endfor %>
<% endif %>
<% if namespaceList %>
<% if config.COMPACT_LATEX %>\section<% else %>\chapter<% endif %>{<{ tr.namespaceDocumentation }>}
<% for compound in namespaceList %>
\input{<{ compound.fileName|raw }>}
<% endfor %>
<% endif %>
<% if classList %>
<% if config.COMPACT_LATEX %>\section<% else %>\chapter<% endif %>{<{ tr.classDocumentation }>}
<% for compound in classList %>
\input{<{ compound.fileName|raw }>}
<% endfor %>
<% endif %>
<% if fileList %>
<% if config.COMPACT_LATEX %>\section<% else %>\chapter<% endif %>{<{ tr.fileDocumentation }>}
<% for compound in fileList %>
\input{<{ compound.fileName|raw }>}
<% endfor %>
<% endif %>
%--- End generated contents ---

<# TODO: write bibliography #>
% Index
<% if not config.COMPACT_LATEX %>
\backmatter
<% endif %>
\newpage
\phantomsection
\clearemptydoublepage
\addcontentsline{toc}{<% if config.COMPACT_LATEX %>section<% else %>chapter<% endif %>}{<{ tr.index }>}
\printindex

\end{document}


