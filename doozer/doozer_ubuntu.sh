#!/bin/bash

ROOT_PATH=/project/repo/checkout
wget https://github.com/libevent/libevent/releases/download/release-2.1.8-stable/libevent-2.1.8-stable.tar.gz
tar -zxf libevent-2.1.8-stable.tar.gz
cd libevent-2.1.8-stable 
sudo ./configure 
sudo make 
sudo make install

# install gtest
cd /usr/src/gtest 
sudo cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++-4.8 .
sudo make
sudo cp libgtest*.a /usr/local/lib

cd /project/repo/checkout
mkdir build
cd build
cmake --version
cmake -DCMAKE_BUILD_TYPE=Debug -DM32=$1 -DUSEKAFKA=0 -DADINFVERSION=1 -DADINFSOVERSION=1 -DDEV=1 -DCMAKE_CXX_COMPILER=/usr/bin/g++-4.8 ..
make
