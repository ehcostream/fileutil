#!/bin/bash
mkdir build
cd build
cmake ..
make -j8 install
echo Project has been built successfully!
