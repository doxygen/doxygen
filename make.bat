REM make script for Microsoft Visual C++

REM goto OK

if not '%QTDIR%'=='' goto OK
echo The QTDIR environment variable is not set! See the INSTALL file for more info. 
rem You can also remove the comment from the upper "REM goto OK" line 
rem and set the QTDIR correctly on the line below.
goto END
:OK

REM Change the QTDIR setting as appropriate (at the following line)
if '%QTDIR%'=='' set QTDIR=c:\usr\qt-1.44

REM Generate the batch for compilation from the src subdirectory (make.bat)
echo set QTDIR=%QTDIR%>src\make.bat
echo nmake>>src\make.bat

REM use perl to create the config file
perl wintools\make.pl

type makeconfig Makefile.win.in >Makefile
type makeconfig src\Makefile.in >src\Makefile
type makeconfig examples\Makefile.win.in >examples\Makefile
type makeconfig doc\Makefile.win.in >doc\Makefile
type src\doxygen.pro.in | sed "s/\$extraopts/release/g" >src\doxygen.pro
type src\doxytag.pro.in | sed "s/\$extraopts/release/g" >src\doxytag.pro
type src\doxysearch.pro.in | sed "s/\$extraopts/release/g" >src\doxysearch.pro

nmake

:END
