#!/bin/sh

clang++ -g other.cpp ar.cpp dijkstra.cpp fuzzy.cpp -o ar -std=c++17
./ar
