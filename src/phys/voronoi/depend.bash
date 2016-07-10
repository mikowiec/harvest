#!/bin/bash

echo "CXX = g++"
echo
echo "CXXFLAGS = -g -I/usr/include/stlport/ -I../../"
echo
files="stlhelper point polygon visualize cell face mesh polyhedron utility winged edge feature plane vertex distance geometry"

for file in $files
do
    sources="$sources $file.cpp";
    objects="$objects $file.o";
done;

echo "objects = $objects"
echo
echo 'test : $(objects) test.cpp'
echo '	$(CXX) $(CXXFLAGS) $(objects) test.cpp -o test -L/usr/lib/ -lstlport_gcc -lGL -lGLU -lglut'

for file in $sources
do
    echo
    g++ -MM -I../../ -I/usr/include/stlport/ $file;
done;

echo
echo ".PHONY : clean"
echo "clean :"
echo '	-rm test $(objects)'

