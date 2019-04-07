#!/usr/bin/env sh

set -v

echo "Extracting Qt for MinGW (static build)"
7z x ./.deps/Qt-5.12.1-mingw-static.zip -y -otmp
mkdir /c/Qt
mv ./tmp/5.12.1-mingw-static/ /c/Qt/5.12.1-mingw-static

if [ ! -d "/c/Qt/Tools/mingw730_64/x86_64-w64-mingw32/lib/" ]; then
  mkdir -p "/c/Qt/Tools/mingw730_64/x86_64-w64-mingw32/lib/"
fi
cp ./.deps/libd2d1.a /c/Qt/Tools/mingw730_64/x86_64-w64-mingw32/lib/libd2d1.a
cp ./.deps/libdwrite.a /c/Qt/Tools/mingw730_64/x86_64-w64-mingw32/lib/libdwrite.a

echo "Installing GNU make"
choco install make
