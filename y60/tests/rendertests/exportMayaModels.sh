#!/bin/sh

cd ../../products/maya-xport

# export all to x60
#./maya2x60.sh ../../tests/rendertests/*.mb

#export all b60 new
for file in ../../tests/rendertests/*.b60
do
    infile="${file%.*}"
    infile="${infile}.mb"
    if [ ! -f "${infile}" ]; then
        echo "### WARNING: File not found ${infile}"
        continue
    fi
    ./maya2x60.sh -b "$infile"
done

cd -