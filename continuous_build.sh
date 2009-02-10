#!/bin/bash
set -e # abort on error
set -x # print all commands
set -u # abort on undefined variables

BUILD_DIR=_build
UNAME_OS=`uname -s`

case "$UNAME_OS" in
    Cygwin* | cygwin* | CYGWIN* | MINGW*)
        echo "Building on windows using a unix shell is not supported"
        exit 1
		#CMAKE_GENERATOR="Visual Studio 9 2008"
		#MAKE_COMMAND='vcbuild PRO60.sln Release|Win32'
        ;;
    Darwin* | Linux*)
		CMAKE_GENERATOR="Unix Makefiles"
		MAKE_COMMAND=make
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

echo "=== Running $MAKE_COMMAND $MAKE_ARGS"
$MAKE_COMMAND

echo "Build done"

exit 0
