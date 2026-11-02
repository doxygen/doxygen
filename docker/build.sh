#!/usr/bin/sh

cd /doxygen/jquery
make clean
make
make install

rm -rf /doxygen_build
mkdir /doxygen_build
cd /doxygen_build
cmake -G "Unix Makefiles" /doxygen
make clean
make
make install
