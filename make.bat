REM make script for Microsoft Visual C++

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
