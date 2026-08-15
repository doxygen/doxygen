#!/bin/bash
if ! [ -d build/ ]; then
    mkdir build
fi
cd build && cmake -G "Unix Makefiles" .. && make -j || {
    echo
    echo "Doxygen build failed."
    echo
}
