#!/bin/bash

path=`pwd`
cd ${path}/build
#cmake ..
#make clean;make
make
cp bin/facePro ../bin/facePro
cd ${path}

