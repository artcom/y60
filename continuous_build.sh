#!/bin/bash
set -e
set -x
set -u

echo configuring cmake build

cd ..
mkdir _build
cd _build

cmake ../src

make

echo done
