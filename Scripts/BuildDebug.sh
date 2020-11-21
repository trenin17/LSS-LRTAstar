#!/bin/bash
cd ../Build/Debug
cmake ../../ -DCMAKE_BUILD_TYPE="Debug"
make
make install
cd ../../Scripts

