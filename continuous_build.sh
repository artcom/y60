#!/bin/bash
set -e

BUILD_DIR="_build"

if [[ -n "$VERBOSE" && "$VERBOSE" != "0" ]]; then 
    set -x
    env
fi

if [[ -n "$BUILD_TYPE" ]]; then 
    CMAKE_ARGS="$CMAKE_ARGS -D CMAKE_BUILD_TYPE=$BUILD_TYPE"
fi

if [[ -z "$NUMCORES" || "$NUMCORES" = "0" ]]; then 
    NUMCORES=1
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake $CMAKE_ARGS ..
make -j $NUMCORES

if [[ -z "$SKIP_TESTS" || "$SKIP_TESTS" == "0" ]]; then 
    make test
fi

exit 0
