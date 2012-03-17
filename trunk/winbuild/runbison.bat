bison -l -d -p cppExpYY %1\..\src\constexp.y -o %1\..\src\ce_parse.c
del %1\..\src\ce_parse.c
bison -l -p cppExpYY %1\..\src\constexp.y -o %1\..\src\ce_parse.cpp

bison -l -d -p vhdlScanYY %1\..\src\vhdlparser.y -o %1\..\src\vhdlparser.c
del %1\..\src\vhdlparser.c
bison -l -p vhdlScanYY %1\..\src\vhdlparser.y -o %1\..\src\vhdlparser.cpp
