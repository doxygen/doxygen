#!/bin/bash
mkdir build
cd build
# create configvalues.h
python3 ../src/configgen.py -maph ../src/config.xml > configvalues.h
# create dummy settings.h
echo "#define IS_SUPPORTED(x) 0" > settings.h
# build the language.cpp file standalone
g++ -c -Wall -Werror -std=c++17 -DDISABLE_JAVACC ../src/language.cpp -I../src -I../libversion -I. -I../deps/fmt/include
