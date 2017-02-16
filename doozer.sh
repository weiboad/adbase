#!/bin/bash
sudo wget https://github.com/libevent/libevent/releases/download/release-2.1.8-stable/libevent-2.1.8-stable.tar.gz
sudo tar -zxf libevent-2.1.8-stable.tar.gz
cd libevent-2.1.8-stable 
sudo ./configure 
sudo make 
sudo make install

# install gtest
cd /usr/src/gtest
sudo cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++-4.8 .
sudo make
sudo cp libgtest*.a /usr/local/lib
