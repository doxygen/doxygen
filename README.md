ParaMonte Doxygen
=================

This repository contains a variation of the original Doxygen software that is specially tailored for the needs of the ParaMonte library documentation website.

Installation instructions on Windows
====================================

1. Download and install the [Microsoft Visual Studio Community version](https://visualstudio.microsoft.com/vs/community/) on your Windows system.
1. Download and install a [recent GNU gcc compiler](https://github.com/LKedward/quickstart-fortran/releases) for Windows.
1. Download and install a [recent version of Flex](https://gnuwin32.sourceforge.net/packages/flex.htm) on your Windows system.
1. Download and install a recent version of GNU Bison.
1. A recent version of the last two software above can be collectively found and installed from https://github.com/lexxmark/winflexbison/releases
1. Download and install a recent version of [CMake software](https://cmake.org/download/) on yout Windows system.
1. Esnure the path to `cmake.exe`, `bison.exe` and `flex.exe` and GNU compilers (`gcc.exe`, `g++.exe`) exist in the environmental `PATH` variable of your Windows CMD shell.
1. Download the [ParaMonte Doxygen project](https://github.com/cdslaborg/doxygen) from github. 
   If you have `git` software installed on your system, you can readily download via `git clone https://github.com/cdslaborg/doxygen` on a git-aware Windows command line.
1. Navigate to the root directory of ParaMonte Doxygen project on the Windows command lind and type:
   ```batch  
   mkdir build
   cd build
   cmake -G "NMake Makefiles" ..
   nmake
   ```  
1. The above commands will create a new folder `bin` within the current working directory (`build`) that contains the doxygen software eexecutable binary.
1. You can now build the ParaMonte library documentation with this specific customized version of Doxygen software.

Installation instructions on Linux/macOS
========================================

The installation instructions on Linux/macOS are identical to those offered by [Doxygen developers](https://www.doxygen.nl/manual/install.html).

Doxygen
===============
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=9HHLRBCC8B2B8)

Doxygen is the de facto standard tool for generating documentation from
annotated C++ sources, but it also supports other popular programming
languages such as C, Objective-C, C#, PHP, Java, Python, IDL
(Corba, Microsoft, and UNO/OpenOffice flavors), Fortran, VHDL,
and to some extent D.

Doxygen can help you in three ways:

1. It can generate an on-line documentation browser (in HTML) and/or an
   off-line reference manual (in LaTeX) from a set of documented source files.
   There is also support for generating output in RTF (MS-Word), PostScript,
   hyperlinked PDF, compressed HTML, DocBook and Unix man pages.
   The documentation is extracted directly from the sources, which makes
   it much easier to keep the documentation consistent with the source code.
2. You can configure doxygen to extract the code structure from undocumented
   source files. This is very useful to quickly find your way in large
   source distributions. Doxygen can also visualize the relations between
   the various elements by means of include dependency graphs, inheritance
   diagrams, and collaboration diagrams, which are all generated automatically.
3. You can also use doxygen for creating normal documentation (as I did for
   the doxygen user manual and doxygen web-site).

Download
---------
The latest binaries and source of Doxygen can be downloaded from:
* https://www.doxygen.nl/

Developers
---------
* Linux & Windows and MacOS Build Status: <a href="https://github.com/doxygen/doxygen/actions"><img alt="Github Actions Build Status" src="https://github.com/doxygen/doxygen/workflows/CMake%20Build%20for%20Doxygen/badge.svg"></a>

* Coverity Scan Build Status: <a href="https://scan.coverity.com/projects/2860"> <img alt="Coverity Scan Build Status" src="https://scan.coverity.com/projects/2860/badge.svg"/> </a>

* Doxygen's Doxygen Documentation: <a href="https://codedocs.xyz/doxygen/doxygen/"><img src="https://codedocs.xyz/doxygen/doxygen.svg"/></a>

* Install: Please read the installation section of the manual (https://www.doxygen.nl/manual/install.html)

* Project stats: https://www.openhub.net/p/doxygen

Issues, bugs, requests, ideas
----------------------------------
Use the [issue](https://github.com/doxygen/doxygen/issues) tracker to report bugs.

Comms
----------------------------------
### Mailing Lists ###

There are three mailing lists:

* doxygen-announce@lists.sourceforge.net     - Announcement of new releases only
* doxygen-users@lists.sourceforge.net        - for doxygen users
* doxygen-develop@lists.sourceforge.net      - for doxygen developers
* To subscribe follow the link to
    * https://sourceforge.net/projects/doxygen/

Source Code
----------------------------------
In May 2013, Doxygen moved from
subversion to git hosted at GitHub
* https://github.com/doxygen/doxygen

Enjoy,

Dimitri van Heesch (doxygen at gmail.com)
