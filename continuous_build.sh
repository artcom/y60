#!/bin/bash

echo configuring cmake build

cd ..
mkdir _build
cd _build

cmake ../src

make

echo done
