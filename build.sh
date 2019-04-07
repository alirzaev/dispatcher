#!/usr/bin/env sh

set -ev

echo "Setting up Qt environment"
export PATH="/c/Qt/5.12.1-mingw-static/bin:"$PATH

echo "Preparing to build"
mkdir build
cd build

echo "Building..."
qmake ../dispatcher.pro
make -j 4
ls ./gui/release