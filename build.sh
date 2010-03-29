#!/bin/bash

[ -d build ] && rm -R build

mkdir build && cd build

cmake ..
make
