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

type makeconfig                 > Makefile
type Makefile.win_%MAKE%.in     >>Makefile
type makeconfig                 > qtools\Makefile
type qtools\Makefile.in         >>qtools\Makefile
type makeconfig                 > src\Makefile
type src\Makefile.in            >>src\Makefile
type makeconfig                 > examples\Makefile
type examples\Makefile.win.in   >>examples\Makefile
type makeconfig                 > doc\Makefile
type doc\Makefile.win_%MAKE%.in >>doc\Makefile

REM build in release or debug mode 
type qtools\qtools.pro.in | sed "s/\$extraopts/%MODE%/g" >qtools\qtools.pro
type src\libdoxygen.pro.in | sed "s/\$extraopts/%MODE%/g" >src\libdoxygen.pro
type src\doxygen.pro.in | sed "s/\$extraopts/%MODE%/g" >src\doxygen.pro
type src\doxytag.pro.in | sed "s/\$extraopts/%MODE%/g" >src\doxytag.pro
type src\doxysearch.pro.in | sed "s/\$extraopts/%MODE%/g" >src\doxysearch.pro

REM run make
%MAKE%.exe
goto END

:USAGE
echo "Call with '%0 [bcc|mingw|msvc] [debug]'!"
echo "  bcc:   compile with Borland C++"
echo "  mingw: compile with GCC for windows (see www.mingw.org)"
echo "  msvc:  compile with Microsoft Visual C++"

:END

