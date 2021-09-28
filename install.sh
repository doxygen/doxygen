#!/bin/bash
cd build && cmake -G "Unix Makefiles" .. && make || {
    echo
    echo "Doxygen build failed."
    echo
}
