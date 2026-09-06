#!/usr/bin/sh

mkdir /myproject
cp -r /source_to_document/* /myproject
cd /myproject/source/doc/config
doxygen config.doxy

cp -r /myproject/doc /source_to_document
