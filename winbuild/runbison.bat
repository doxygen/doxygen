bison -l -d -p cppExpYY %1\..\src\constexp.y -o %1\..\src\ce_parse.c
del %1\..\src\ce_parse.c
bison -l -p cppExpYY %1\..\src\constexp.y -o %1\..\src\ce_parse.cpp
