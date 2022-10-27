#!/bin/bash
if ! [ -d build/ ]; then
    mkdir build
fi
cd build && cmake -G "Unix Makefiles" .. && make || {
    echo
    echo "Doxygen build failed."
    echo
}
