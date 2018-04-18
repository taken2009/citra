#!/bin/bash -ex

# Compile OpenCV from source
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/lib ..
make -j7
make install

mkdir build && cd build
cmake .. -DENABLE_QT_TRANSLATION=ON -DGENERATE_QT_TRANSLATION=ON -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENCV=ON
make translation
