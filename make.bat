@echo off
REM make script for Microsoft Visual C++
REM ...and MINGW and Borland 5.5 also!

if "%1"=="bcc"   SET CC=borland
if "%1"=="mingw" SET CC=mingw
if "%1"=="msvc"  SET CC=msvc
if "%1"=="" goto USAGE

if "%CC%"=="msvc"     SET MAKE=nmake
if not "%CC%"=="msvc" SET MAKE=make

SET MODE=release
if "%2"=="debug" SET MODE=debug

REM use perl to create the config file
perl wintools\make.pl %CC%

type makeconfig                   > Makefile
type Makefile.win_%MAKE%.in       >>Makefile
type makeconfig                   > qtools\Makefile
type qtools\Makefile.in           >>qtools\Makefile
type makeconfig                   > libpng\Makefile
type libpng\Makefile.in           >>libpng\Makefile
type makeconfig                   > src\Makefile
type src\Makefile.in              >>src\Makefile
type makeconfig                   > examples\Makefile
type examples\Makefile.win.in     >>examples\Makefile
type makeconfig                   > doc\Makefile
type doc\Makefile.win_%MAKE%.in   >>doc\Makefile
type makeconfig                   > addon\doxywizard\Makefile
type addon\doxywizard\Makefile.win_%MAKE%.in >>addon\doxywizard\Makefile

REM build in release or debug mode 
REM sed is used to replace $extraopts by either debug or release while copying
sed "s/\$extraopts/%MODE%/g" qtools\qtools.pro.in >qtools\qtools.pro
sed "s/\$extraopts/%MODE%/g" libpng\libpng.pro.in >libpng\libpng.pro
sed "s/\$extraopts/%MODE%/g" src\libdoxygen.pro.in >src\libdoxygen.pro
sed "s/\$extraopts/%MODE%/g" src\libdoxycfg.pro.in >src\libdoxycfg.pro
sed "s/\$extraopts/%MODE%/g" src\doxygen.pro.in >src\doxygen.pro
sed "s/\$extraopts/%MODE%/g" src\doxytag.pro.in >src\doxytag.pro
sed "s/\$extraopts/%MODE%/g" src\doxysearch.pro.in >src\doxysearch.pro
sed "s/\$extraopts/%MODE%/g" addon\doxywizard\doxywizard.pro.in >addon\doxywizard\doxywizard.pro

REM run make
%MAKE%.exe
goto END

:USAGE
echo "Call with '%0 [bcc|mingw|msvc] [debug]'!"
echo "  bcc:   compile with Borland C++"
echo "  mingw: compile with GCC for windows (see www.mingw.org)"
echo "  msvc:  compile with Microsoft Visual C++"

:END

