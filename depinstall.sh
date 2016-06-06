#!/bin/bash

(
    echo Installing / Upgrading packages
    sudo apt-get update
    sudo apt-get install libglew-dev cmake libglu1-mesa-dev freeglut3-dev mesa-common-dev git xorg-dev libgl1-mesa-dev libsdl2-dev libsdl2-mixer-dev software-properties-common -y
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test && \
    sudo apt-get update && \
    sudo apt-get install g++-5 -y

    START_DIR=`pwd`
    TMP_DIR=`mktemp -d`

    echo Installing lastest GLFW
    git clone --recursive https://github.com/glfw/glfw ${TMP_DIR} && \
    cd ${TMP_DIR} && \
    cmake . && \
    make && \
    sudo make install
    cd ${START_DIR}
    rm -fr ${TMP_DIR}
)
