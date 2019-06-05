#!/bin/bash
#download the boost library
wget http://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz -O boost_1_70_0.tar.gz
tar xvf boost_1_70_0.tar.gz -C .
cd ./boost_1_70_0/
sudo ./bootstrap.sh --prefix=/usr/local/
sudo ./b2 toolset=gcc cxxflags="-std=c++11 -fPIC" --build-type=minimal --with-chrono --with-date_time --with-system --with-filesystem --with-thread link=static -j8 install

#cloning the grpc and third parties
cd ..
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
cd grpc
git submodule update --init

#install grpc and third parties which are needed by grpc
cd ..
sudo ./run_distrib_cmake.sh

