#!/bin/bash
sudo wget https://github.com/libevent/libevent/releases/download/release-2.1.8-stable/libevent-2.1.8-stable.tar.gz
sudo tar -zxf libevent-2.1.8-stable.tar.gz
cd libevent-2.1.8-stable 
sudo ./configure 
sudo make 
sudo make install
