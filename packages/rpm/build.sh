#!/bin/bash

set -e

[ $(whoami) == "root" ] || {
echo "Don't I need to be root to make RPM packages ?"
}

RPMBUILDPLACE=/usr/src/RPM/

cp doxygen.spec $RPMBUILDPLACE/SPECS

ls *.patch && {
cp *.patch $RPMBUILDPLACE/SOURCES
}

cp ../../../doxygen*.gz $RPMBUILDPLACE/SOURCES

urpmi --auto rpm-build libqt3-devel flex tetex-latex dvips

rpm -ba SPECS/doxygen.spec
