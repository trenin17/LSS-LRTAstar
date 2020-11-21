#!/bin/bash
cd ../Build/Release
cmake ../../ -DCMAKE_BUILD_TYPE="Release"
make
make install
cd ../../Scripts
