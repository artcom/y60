#!/bin/bash
set -e # abort on error
set -x # print all commands
set -u # abort on undefined variables

echo "=== Environment"
env

BUILD_DIR="_build"
CMAKE_GENERATOR="Unix Makefiles"
MAKE_ARGS=""

UNAME_OS=`uname -s`

case "$UNAME_OS" in
    Cygwin* | cygwin* | CYGWIN* | MINGW*)
        echo "Building on windows using a unix shell is not supported."
        exit 1
        ;;
    Darwin* | Linux*)
        ;;
	*)
		echo What?
		;;
esac

echo "=== Running on operating system ${UNAME_OS}"

echo "=== Build directory is $BUILD_DIR"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

echo "=== Configuring cmake build for $CMAKE_GENERATOR"
cmake -G "$CMAKE_GENERATOR" ..

echo "=== Running make $MAKE_ARGS"
make $MAKE_ARGS

echo "Build done"

exit 0
