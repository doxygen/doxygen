REM make script for Microsoft Visual C++

REM use perl to create the config file
perl wintools\make.pl

type makeconfig Makefile.win.in >Makefile
type makeconfig src\Makefile.in >src\Makefile
type makeconfig examples\Makefile.win.in >examples\Makefile
type makeconfig doc\Makefile.win.in >doc\Makefile
copy src\doxygen.pro.in src\doxygen.pro
copy src\doxytag.pro.in src\doxytag.pro
copy src\doxysearch.pro.in src\doxysearch.pro

nmake
