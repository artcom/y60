#!/bin/bash
set -e

BUILD_DIR="_build"

if [[ "$VERBOSE" && "$VERBOSE" != "0" ]]; then 
    set -x
    env
fi

if [[ ! "$NUMCORES" || "$NUMCORES" = "0" ]]; then 
    NUMCORES=1
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake ..
make -j $NUMCORES

echo "=== Build done"
exit 0
