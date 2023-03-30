#!/bin/sh

clang++ -g ar.cpp dijkstra.cpp fuzzy.cpp -o ar -std=c++17
./ar
