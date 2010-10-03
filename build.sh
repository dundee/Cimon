#!/bin/bash

[ -d build ] && rm -R build

mkdir build && cd build

cmake ..
make || return 1
install -m755 cimond ../cimond
