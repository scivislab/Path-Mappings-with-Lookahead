#!/bin/bash

#wget http://www.csc.kth.se/~weinkauf/datasets/Cylinder2D.7z
7z x Cylinder2D.7z -oCylinder2D
mv Cylinder2D Cylinder2D.cdb
g++ amira_to_vtk.cpp
cp a.out Cylinder2D.cdb/
cd Cylinder2D.cdb
./a.out
