#!/bin/bash
mkdir build
cd build
cmake ..
cmake --build . --config Release --target install
echo Project has been built successfully!